/*******************************************************************************
 * qlncHeuristicQueryTransformation.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
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
 * @file qlncHeuristicQueryTransformation.c
 * @brief SQL Processor Layer SELECT statement Heuristic Query Transformation
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Heuristic Query Transformation을 수행한다.
 * @param[in]       aParamInfo  Parameter Info
 * @param[in,out]   aNode       Candidate Node
 * @param[in]       aEnv        Environment
 */
stlStatus qlncHeuristicQueryTransformation( qlncParamInfo   * aParamInfo,
                                            qlncNodeCommon  * aNode,
                                            qllEnv          * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Heuristic Query Transformation (HQT)
     ****************************************************************/

    /**
     * 1. Join Factorization (JF)
     *    : Union / Union All 사용시 공통으로 사용하는 테이블을 분리시키는 것
     * 2. Table Expansion (TE)
     * 3. Simple View Merging (SVM)
     * 4. Complex View Merging (CVM)
     * 5. Transitive Predicate Generation (TPG)
     *    : where절에 'a.i1 = b.i1 and a.i1 = 30'일 경우
     *      'b.i1 = 30'을 추가하는 작업
     * 6. Common Subexpression Elimination (CSE)
     * 7. Predicate Move-Around (PM)
     *    : Multiview 사이에서 Predicate Pushdown and Pullup
     * 8. Join Elimination (JE)
     *    : Outer-join to inner join conversion
     */

    /* Unuseful Node/Filter Elimination */
    /**
     * ex) i1 in ( select i1 from B order by i1 )   => order by 필요 없음
     */

    /* Constant Boolean Filter 처리 */

    /* Exists/Not Exists 연산 Target 최적화 처리 */
    STL_TRY( qlncRewriteTargetOnExists( aParamInfo,
                                        aNode,
                                        aEnv )
             == STL_SUCCESS );

    /* Transitive Predicate Generation */
    STL_TRY( qlncTransitivePredicateGeneration( aParamInfo,
                                                aNode,
                                                aEnv )
             == STL_SUCCESS );

    /* Filter Push Down */
    STL_TRY( qlncFilterPushDown( aParamInfo,
                                 aNode,
                                 aEnv )
             == STL_SUCCESS );

    /* Quantify Operator Elemination */
    STL_TRY( qlncQuantifyOperatorElemination( aParamInfo,
                                              aNode,
                                              aEnv )
             == STL_SUCCESS );

    /* Single Table Min/Max Aggregation Conversion */
    STL_TRY( qlncSingleTableAggregationConversion( aParamInfo,
                                                   aNode,
                                                   aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Exists/Not Exists연산에 대하여 Target을 Rewrite 한다.
 * @param[in]       aParamInfo  Parameter Info
 * @param[in,out]   aNode       Candidate Node
 * @param[in]       aEnv        Environment
 */
stlStatus qlncRewriteTargetOnExists( qlncParamInfo  * aParamInfo,
                                     qlncNodeCommon * aNode,
                                     qllEnv         * aEnv )
{
    stlInt32          i;
    qlncNodeCommon  * sNode     = NULL;
    qlncNodeArray   * sNodeArr  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    sNode = aNode;

    STL_RAMP( RAMP_RETRY );

    switch( sNode->mNodeType )
    {
        case QLNC_NODE_TYPE_QUERY_SET:
            sNode = ((qlncQuerySet*)sNode)->mChildNode;
            STL_THROW( RAMP_RETRY );
            break;

        case QLNC_NODE_TYPE_SET_OP:
            for( i = 0; i < ((qlncSetOP*)sNode)->mChildCount; i++ )
            {
                STL_TRY( qlncRewriteTargetOnExists( aParamInfo,
                                                    ((qlncSetOP*)sNode)->mChildNodeArr[i],
                                                    aEnv )
                         == STL_SUCCESS );
            }
            break;

        case QLNC_NODE_TYPE_QUERY_SPEC:
            sNode = ((qlncQuerySpec*)sNode)->mChildNode;
            STL_THROW( RAMP_RETRY );
            break;

        case QLNC_NODE_TYPE_BASE_TABLE:
            if( ((qlncBaseTableNode*)sNode)->mFilter != NULL )
            {
                STL_TRY( qlncRTEAndFilter( aParamInfo,
                                           ((qlncBaseTableNode*)sNode)->mFilter,
                                           aEnv )
                         == STL_SUCCESS );
            }
            break;

        case QLNC_NODE_TYPE_SUB_TABLE:
            if( ((qlncSubTableNode*)sNode)->mFilter != NULL )
            {
                STL_TRY( qlncRTEAndFilter( aParamInfo,
                                           ((qlncSubTableNode*)sNode)->mFilter,
                                           aEnv )
                         == STL_SUCCESS );
            }
            sNode = ((qlncSubTableNode*)sNode)->mQueryNode;
            STL_THROW( RAMP_RETRY );
            break;

        case QLNC_NODE_TYPE_JOIN_TABLE:
            if( ((qlncJoinTableNode*)sNode)->mJoinCond != NULL )
            {
                STL_TRY( qlncRTEAndFilter( aParamInfo,
                                           ((qlncJoinTableNode*)sNode)->mJoinCond,
                                           aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncJoinTableNode*)sNode)->mFilter != NULL )
            {
                STL_TRY( qlncRTEAndFilter( aParamInfo,
                                           ((qlncJoinTableNode*)sNode)->mFilter,
                                           aEnv )
                         == STL_SUCCESS );
            }

            sNodeArr = &(((qlncJoinTableNode*)sNode)->mNodeArray);
            for( i = 0; i < sNodeArr->mCurCnt; i++ )
            {
                STL_TRY( qlncRewriteTargetOnExists( aParamInfo,
                                                    sNodeArr->mNodeArr[i],
                                                    aEnv )
                         == STL_SUCCESS );
            }
            break;

        case QLNC_NODE_TYPE_FLAT_INSTANT:
            STL_DASSERT( 0 );
            break;

        case QLNC_NODE_TYPE_SORT_INSTANT:
            if( ((qlncSortInstantNode*)sNode)->mFilter != NULL )
            {
                STL_TRY( qlncRTEAndFilter( aParamInfo,
                                           ((qlncSortInstantNode*)sNode)->mFilter,
                                           aEnv )
                         == STL_SUCCESS );
            }
            sNode = ((qlncSortInstantNode*)sNode)->mChildNode;
            STL_THROW( RAMP_RETRY );
            break;

        case QLNC_NODE_TYPE_HASH_INSTANT:
            if( ((qlncHashInstantNode*)sNode)->mFilter != NULL )
            {
                STL_TRY( qlncRTEAndFilter( aParamInfo,
                                           ((qlncHashInstantNode*)sNode)->mFilter,
                                           aEnv )
                         == STL_SUCCESS );
            }
            sNode = ((qlncHashInstantNode*)sNode)->mChildNode;
            STL_THROW( RAMP_RETRY );
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
 * @brief And Filter에 존재하는 SubQuery에 대하여 Rewrite Target on Exists를 수행한다.
 * @param[in]       aParamInfo  Parameter Info
 * @param[in,out]   aAndFilter  And Filter
 * @param[in]       aEnv        Environment
 */
stlStatus qlncRTEAndFilter( qlncParamInfo   * aParamInfo,
                            qlncAndFilter   * aAndFilter,
                            qllEnv          * aEnv )
{
    stlInt32              i;
    stlInt32              j;
    stlInt32              k;
    stlInt32              l;
    qlncRefExprItem     * sRefExprItem  = NULL;
    qlncExprCommon      * sExpr         = NULL;
    qlncExprSubQuery    * sSubQuery     = NULL;
    qlncQuerySpec       * sQuerySpec    = NULL;

    stlBool               sIsExists;
    stlBool               sHasGroupBy;

    qlncExprValue       * sValue        = NULL;
    qlncExprConstExpr   * sConstExpr    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter != NULL, QLL_ERROR_STACK(aEnv) );


    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        if( aAndFilter->mOrFilters[i]->mSubQueryExprList == NULL )
        {
            continue;
        }

        /* SubQuery의 Node에 대하여 Rewrite Target On Exists를 수행 */
        sRefExprItem = aAndFilter->mOrFilters[i]->mSubQueryExprList->mHead;
        while( sRefExprItem != NULL )
        {
            STL_TRY( qlncRewriteTargetOnExists( aParamInfo,
                                                ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode,
                                                aEnv )
                     == STL_SUCCESS );

            sRefExprItem = sRefExprItem->mNext;
        }

        for( j = 0; j < aAndFilter->mOrFilters[i]->mFilterCount; j++ )
        {
            if( aAndFilter->mOrFilters[i]->mFilters[j]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER )
            {
                sExpr = ((qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mExpr;
            }
            else
            {
                STL_DASSERT( aAndFilter->mOrFilters[i]->mFilters[j]->mType
                             == QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER );
                sExpr = ((qlncConstBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mExpr;
            }

            /* Expression이 List Function이어야 함 */
            if( sExpr->mType != QLNC_EXPR_TYPE_LIST_FUNCTION )
            {
                continue;
            }

            /* Exists 또는 Not Exist이어야 함 */
            if( ((qlncExprListFunc*)sExpr)->mFuncId == KNL_BUILTIN_FUNC_EXISTS )
            {
                sIsExists = STL_TRUE;
            }
            else if( ((qlncExprListFunc*)sExpr)->mFuncId == KNL_BUILTIN_FUNC_NOT_EXISTS )
            {
                sIsExists = STL_FALSE;
            }
            else
            {
                continue;
            }

            STL_DASSERT( ((qlncExprListFunc*)sExpr)->mArgCount == 1 );
            STL_DASSERT( ((qlncExprListCol*)(((qlncExprListFunc*)sExpr)->mArgs[0]))->mArgCount == 1 );
            STL_DASSERT( ((qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)sExpr)->mArgs[0]))->mArgs[0]))->mArgCount == 1 );
            STL_DASSERT( ((qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)sExpr)->mArgs[0]))->mArgs[0]))->mArgs[0]->mType
                         == QLNC_EXPR_TYPE_SUB_QUERY );

            sSubQuery = (qlncExprSubQuery*)(((qlncExprRowExpr*)(((qlncExprListCol*)(((qlncExprListFunc*)sExpr)->mArgs[0]))->mArgs[0]))->mArgs[0]);

            /**
             * Exists의 Target 변경 규칙
             *
             * 1. Query Node가 Query Set이면 Target을 변경하지 않는다.
             * 2. Group By가 존재하는 경우
             *   2-1. Target에 Nested Aggregation이 존재하지 않으면 Target을 상수로 변경
             *   2-2. Target에 Nested Aggregation이 존재하면 Target을 변경하지 않음
             * 3. Group By가 존재하지 않는 경우
             *   3-1. Target에 Aggregation이 존재하지 않으면 Target을 상수로 변경
             *   3-2. Target에 Aggregation이 존재하면 Target을 변경하지 않음
             */

            /* 1. Query Node가 Query Set이면 Target을 변경하지 않는다. */
            if( sSubQuery->mNode->mNodeType == QLNC_NODE_TYPE_QUERY_SET )
            {
                continue;
            }

            /* Query Spec 설정 */
            sQuerySpec = (qlncQuerySpec*)(sSubQuery->mNode);

            /* Group By가 존재하는지 체크 */
            if( (sQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT) &&
                (((qlncHashInstantNode*)(sQuerySpec->mChildNode))->mInstantType == QLNC_INSTANT_TYPE_GROUP) )
            {
                sHasGroupBy = STL_TRUE;
            }
            else if( sQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
            {
                if( (((qlncSortInstantNode*)(sQuerySpec->mChildNode))->mChildNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT) &&
                    (((qlncHashInstantNode*)(((qlncSortInstantNode*)(sQuerySpec->mChildNode))->mChildNode))->mInstantType == QLNC_INSTANT_TYPE_GROUP) )
                {
                    sHasGroupBy = STL_TRUE;
                }
                else
                {
                    sHasGroupBy = STL_FALSE;
                }
            }
            else
            {
                sHasGroupBy = STL_FALSE;
            }

            /* Target의 변경이 가능한지 판단한다. */
            if( sHasGroupBy == STL_TRUE )
            {
                /* Target에 Nested Aggregation이 존재하는지 판단 */
                for( k = 0; k < sQuerySpec->mTargetCount; k++ )
                {
                    sExpr = qlncRTEGetAggrExpr( sQuerySpec->mTargetArr[k].mExpr );
                    if( sExpr != NULL )
                    {
                        for( l = 0; l < ((qlncExprAggregation*)sExpr)->mArgCount; l++ )
                        {
                            if( qlncRTEGetAggrExpr( ((qlncExprAggregation*)sExpr)->mArgs[l] ) != NULL )
                            {
                                break;
                            }
                        }

                        if( l < ((qlncExprAggregation*)sExpr)->mArgCount )
                        {
                            break;
                        }
                    }
                }

                /* Target에 Nested Aggregation이 존재하면 Skip */
                if( k < sQuerySpec->mTargetCount )
                {
                    continue;
                }
            }
            else
            {
                /* Target에 Aggregation이 존재하는지 판단 */
                for( k = 0; k < sQuerySpec->mTargetCount; k++ )
                {
                    if( qlncRTEGetAggrExpr( sQuerySpec->mTargetArr[k].mExpr )
                        != NULL )
                    {
                        break;
                    }
                }

                /* Target에 Aggregation이 존재하면 Skip */
                if( k < sQuerySpec->mTargetCount )
                {
                    continue;
                }
            }


            /**
             * Target이 이미 상수이거나 상수로 바뀌어 있는 경우 Skip
             */

            if( sQuerySpec->mTargetCount == 1 )
            {
                if( (sQuerySpec->mTargetArr[0].mExpr->mType
                     == QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT) &&
                    (((qlncExprConstExpr*)(sQuerySpec->mTargetArr[0].mExpr))->mOrgExpr->mType
                      == QLNC_EXPR_TYPE_VALUE) )
                {
                    sValue =
                        (qlncExprValue*)(((qlncExprConstExpr*)(sQuerySpec->mTargetArr[0].mExpr))->mOrgExpr);
                    if( (sValue->mValueType == QLNC_VALUE_TYPE_BOOLEAN) &&
                        (sValue->mInternalBindIdx == QLV_INTERNAL_BIND_VARIABLE_IDX_NA) &&
                        (sValue->mData.mInteger == sIsExists) )
                    {
                        continue;
                    }
                }
            }


            /**
             * Target을 상수로 바꾼다.
             */

            /* Value Expression 할당 */
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprValue ),
                                        (void**) &sValue,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sValue->mValueType = QLNC_VALUE_TYPE_BOOLEAN;
            sValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
            sValue->mData.mInteger = sIsExists;

            QLNC_SET_EXPR_COMMON( &sValue->mCommon,
                                  &QLL_CANDIDATE_MEM( aEnv ),
                                  aEnv,
                                  QLNC_EXPR_TYPE_VALUE,
                                  QLNC_EXPR_PHRASE_TARGET,
                                  DTL_ITERATION_TIME_NONE,
                                  sSubQuery->mCommon.mPosition,
                                  ( sIsExists == STL_TRUE
                                    ? "TRUE" : "FALSE" ),
                                  DTL_TYPE_BOOLEAN );

            /* Constant Result Expression 할당 */
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprConstExpr ),
                                        (void**) &sConstExpr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_SET_EXPR_COMMON( &sConstExpr->mCommon,
                                  &QLL_CANDIDATE_MEM( aEnv ),
                                  aEnv,
                                  QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT,
                                  sValue->mCommon.mExprPhraseFlag,
                                  sValue->mCommon.mIterationTime,
                                  sValue->mCommon.mPosition,
                                  ( sIsExists == STL_TRUE
                                    ? "TRUE" : "FALSE" ),
                                  DTL_TYPE_BOOLEAN );

            sConstExpr->mOrgExpr = (qlncExprCommon*)sValue;

            /* Constant Result Expression을 Target Column으로 설정 */
            sQuerySpec->mTargetCount = 1;
            sQuerySpec->mTargetArr[0].mExpr = (qlncExprCommon*)sConstExpr;
            
            sQuerySpec->mTargetArr[0].mDataTypeInfo.mIsBaseColumn = STL_FALSE;
            sQuerySpec->mTargetArr[0].mDataTypeInfo.mNullable     = STL_FALSE;
            sQuerySpec->mTargetArr[0].mDataTypeInfo.mUpdatable    = STL_FALSE;
            sQuerySpec->mTargetArr[0].mDataTypeInfo.mIsIgnore     = STL_TRUE;
            
            sQuerySpec->mTargetArr[0].mDataTypeInfo.mDataType
                = DTL_TYPE_BOOLEAN;
            sQuerySpec->mTargetArr[0].mDataTypeInfo.mArgNum1
                = gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1;
            sQuerySpec->mTargetArr[0].mDataTypeInfo.mArgNum2
                = gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2;
            sQuerySpec->mTargetArr[0].mDataTypeInfo.mStringLengthUnit
                = gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit;
            sQuerySpec->mTargetArr[0].mDataTypeInfo.mIntervalIndicator
                = gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator;

            /* SubQuery의 Target을 변경 */
            sSubQuery->mTargetCount = 1;
            sSubQuery->mTargetArr = sQuerySpec->mTargetArr;
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expression에서 Aggregation이 나오면 해당 Expression을 반환한다.
 * @param[in]   aExpr   Expression
 */
qlncExprCommon * qlncRTEGetAggrExpr( qlncExprCommon * aExpr )
{
    qlncExprCommon  * sResult   = NULL;


    STL_DASSERT( aExpr != NULL );


    switch( aExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
        case QLNC_EXPR_TYPE_BIND_PARAM:
        case QLNC_EXPR_TYPE_COLUMN:
            {
                return NULL;
            }
        case QLNC_EXPR_TYPE_FUNCTION:
            {
                qlncExprFunction    * sFunction = NULL;
                stlInt32              i;

                sFunction = (qlncExprFunction*)aExpr;
                for( i = 0; i < sFunction->mArgCount; i++ )
                {
                    sResult = qlncRTEGetAggrExpr( sFunction->mArgs[i] );
                    if( sResult != NULL )
                    {
                        return sResult;
                    }
                }

                return NULL;
            }
        case QLNC_EXPR_TYPE_CAST:
            {
                qlncExprTypeCast    * sTypeCast = NULL;
                stlInt32              i;

                sTypeCast = (qlncExprTypeCast*)aExpr;
                for( i = 0; i < sTypeCast->mArgCount; i++ )
                {
                    sResult = qlncRTEGetAggrExpr( sTypeCast->mArgs[i] );
                    if( sResult != NULL )
                    {
                        return sResult;
                    }
                }

                return NULL;
            }
        case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
        case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
        case QLNC_EXPR_TYPE_SUB_QUERY:
        case QLNC_EXPR_TYPE_REFERENCE:
            {
                return NULL;
            }
        case QLNC_EXPR_TYPE_INNER_COLUMN:
            {
                qlncExprInnerColumn * sInnerColumn  = NULL;

                sInnerColumn = (qlncExprInnerColumn*)aExpr;

                if( sInnerColumn->mOrgExpr == NULL )
                {
                    return NULL;
                }
                else
                {
                    return qlncRTEGetAggrExpr( sInnerColumn->mOrgExpr );
                }
            }
        case QLNC_EXPR_TYPE_OUTER_COLUMN:
            {
                STL_DASSERT( 0 );
                return NULL;
            }
        case QLNC_EXPR_TYPE_ROWID_COLUMN:
            {
                return NULL;
            }
        case QLNC_EXPR_TYPE_AGGREGATION:
            {
                return aExpr;
            }
        case QLNC_EXPR_TYPE_CASE_EXPR:
            {
                qlncExprCaseExpr    * sCaseExpr = NULL;
                stlInt32              i;

                sCaseExpr = (qlncExprCaseExpr*)aExpr;
                for( i = 0; i < sCaseExpr->mCount; i++ )
                {
                    sResult = qlncRTEGetAggrExpr( sCaseExpr->mWhenArr[i] );
                    if( sResult != NULL )
                    {
                        return sResult;
                    }

                    sResult = qlncRTEGetAggrExpr( sCaseExpr->mThenArr[i] );
                    if( sResult != NULL )
                    {
                        return sResult;
                    }
                }

                sResult = qlncRTEGetAggrExpr( sCaseExpr->mDefResult );
                if( sResult != NULL )
                {
                    return sResult;
                }

                return NULL;
            }
        case QLNC_EXPR_TYPE_LIST_FUNCTION:
            {
                qlncExprListFunc    * sListFunc = NULL;
                stlInt32              i;

                sListFunc = (qlncExprListFunc*)aExpr;
                for( i = 0; i < sListFunc->mArgCount; i++ )
                {
                    sResult = qlncRTEGetAggrExpr( sListFunc->mArgs[i] );
                    if( sResult != NULL )
                    {
                        return sResult;
                    }
                }

                return NULL;
            }
        case QLNC_EXPR_TYPE_LIST_COLUMN:
            {
                qlncExprListCol     * sListCol  = NULL;
                stlInt32              i;

                sListCol = (qlncExprListCol*)aExpr;
                for( i = 0; i < sListCol->mArgCount; i++ )
                {
                    sResult = qlncRTEGetAggrExpr( sListCol->mArgs[i] );
                    if( sResult != NULL )
                    {
                        return sResult;
                    }
                }

                return NULL;
            }
        case QLNC_EXPR_TYPE_ROW_EXPR:
            {
                qlncExprRowExpr     * sRowExpr  = NULL;
                stlInt32              i;

                sRowExpr = (qlncExprRowExpr*)aExpr;
                for( i = 0; i < sRowExpr->mArgCount; i++ )
                {
                    sResult = qlncRTEGetAggrExpr( sRowExpr->mArgs[i] );
                    if( sResult != NULL )
                    {
                        return sResult;
                    }
                }

                return NULL;
            }
        case QLNC_EXPR_TYPE_AND_FILTER:
        case QLNC_EXPR_TYPE_OR_FILTER:
        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
        default:
            {
                STL_DASSERT( 0 );
                return NULL;
            }
    }


    return NULL;
}


/**
 * @brief Transitive Predicate Generation을 수행한다.
 * @param[in]       aParamInfo  Parameter Info
 * @param[in,out]   aNode       Candidate Node
 * @param[in]       aEnv        Environment
 */
stlStatus qlncTransitivePredicateGeneration( qlncParamInfo  * aParamInfo,
                                             qlncNodeCommon * aNode,
                                             qllEnv         * aEnv )
{
    stlInt32          i;
    qlncNodeCommon  * sNode     = NULL;
    qlncNodeArray   * sNodeArr  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    sNode = aNode;

    STL_RAMP( RAMP_RETRY );

    switch( sNode->mNodeType )
    {
        case QLNC_NODE_TYPE_QUERY_SET:
            sNode = ((qlncQuerySet*)sNode)->mChildNode;
            STL_THROW( RAMP_RETRY );
            break;

        case QLNC_NODE_TYPE_SET_OP:
            for( i = 0; i < ((qlncSetOP*)sNode)->mChildCount; i++ )
            {
                STL_TRY( qlncTransitivePredicateGeneration( aParamInfo,
                                                            ((qlncSetOP*)sNode)->mChildNodeArr[i],
                                                            aEnv )
                         == STL_SUCCESS );
            }
            break;

        case QLNC_NODE_TYPE_QUERY_SPEC:
            STL_TRY_THROW( ((qlncQuerySpec*)sNode)->mQueryTransformHint->mAllowedQueryTransform == STL_TRUE,
                           RAMP_FINISH );
            sNode = ((qlncQuerySpec*)sNode)->mChildNode;
            STL_THROW( RAMP_RETRY );
            break;

        case QLNC_NODE_TYPE_BASE_TABLE:
            /* Do Nothing */
            break;

        case QLNC_NODE_TYPE_SUB_TABLE:
            sNode = ((qlncSubTableNode*)sNode)->mQueryNode;
            STL_THROW( RAMP_RETRY );
            break;

        case QLNC_NODE_TYPE_JOIN_TABLE:
            if( ((qlncJoinTableNode*)sNode)->mJoinType == QLNC_JOIN_TYPE_INNER )
            {
                STL_TRY( qlncTPGExpression( aParamInfo,
                                            ((qlncJoinTableNode*)sNode)->mJoinCond,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlncTPGExpression( aParamInfo,
                                            ((qlncJoinTableNode*)sNode)->mFilter,
                                            aEnv )
                         == STL_SUCCESS );
            }

            sNodeArr = &(((qlncJoinTableNode*)sNode)->mNodeArray);
            for( i = 0; i < sNodeArr->mCurCnt; i++ )
            {
                STL_TRY( qlncTransitivePredicateGeneration( aParamInfo,
                                                            sNodeArr->mNodeArr[i],
                                                            aEnv )
                         == STL_SUCCESS );
            }
            break;

        case QLNC_NODE_TYPE_FLAT_INSTANT:
            STL_DASSERT( 0 );
            break;

        case QLNC_NODE_TYPE_SORT_INSTANT:
            sNode = ((qlncSortInstantNode*)sNode)->mChildNode;
            STL_THROW( RAMP_RETRY );
            break;

        case QLNC_NODE_TYPE_HASH_INSTANT:
            sNode = ((qlncHashInstantNode*)sNode)->mChildNode;
            STL_THROW( RAMP_RETRY );
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expression에 대한 Transitive Predicate Generation을 수행한다.
 * @param[in]       aParamInfo  Parameter Info
 * @param[in,out]   aAndFilter  And Filter
 * @param[in]       aEnv        Environment
 */
stlStatus qlncTPGExpression( qlncParamInfo  * aParamInfo,
                             qlncAndFilter  * aAndFilter,
                             qllEnv         * aEnv )
{
    stlInt32              i;
    stlInt32              j;
    stlBool               sIsSameLeftExpr;

    qlncAndFilter         sAddAndFilter;
    qlncBooleanFilter   * sBooleanFilter        = NULL;
    qlncBooleanFilter   * sTmpBooleanFilter     = NULL;
    qlncBooleanFilter   * sCopiedBooleanFilter  = NULL;

    qlncExprCommon      * sLeftExpr             = NULL;
    qlncExprCommon      * sRightExpr            = NULL;
    qlncExprCommon      * sTmpExpr              = NULL;

    qlncConvertExprParamInfo      sConvertExprParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    if( aAndFilter == NULL )
    {
        STL_THROW( RAMP_FINISH );
    }

    /* Add And Filter 초기화 */
    QLNC_INIT_AND_FILTER( &sAddAndFilter );

    /* Equi-Join Condition을 찾아 TPG가 가능한 조건을 찾음 */
    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        /* Or가 없고 Boolean Filter이어야 한다. */
        STL_TRY_THROW( (aAndFilter->mOrFilters[i]->mFilterCount == 1) &&
                       (aAndFilter->mOrFilters[i]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER),
                       RAMP_SKIP_TWO_TABLE_FILTER );

        /* Boolean Filter 가져오기 */
        sBooleanFilter = (qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[0]);

        /* Is Equal Function이어야 한다. */
        STL_TRY_THROW( sBooleanFilter->mFuncID == KNL_BUILTIN_FUNC_IS_EQUAL,
                       RAMP_SKIP_TWO_TABLE_FILTER );

        /* Function의 Left에 단일 컬럼이 존재해야 한다. */
        STL_TRY_THROW( (sBooleanFilter->mLeftRefNodeList != NULL) &&
                       (sBooleanFilter->mLeftRefNodeList->mCount == 1) &&
                       (sBooleanFilter->mLeftRefNodeList->mHead->mRefColumnList.mCount == 1),
                       RAMP_SKIP_TWO_TABLE_FILTER );

        /* Function의 Right에 단일 컬럼이 존재해야 한다. */
        STL_TRY_THROW( (sBooleanFilter->mRightRefNodeList != NULL) &&
                       (sBooleanFilter->mRightRefNodeList->mCount == 1) &&
                       (sBooleanFilter->mRightRefNodeList->mHead->mRefColumnList.mCount == 1),
                       RAMP_SKIP_TWO_TABLE_FILTER );

        /* 올바른 Function인지 체크 */
        STL_DASSERT( (sBooleanFilter->mExpr->mType == QLNC_EXPR_TYPE_FUNCTION) &&
                     (((qlncExprFunction*)(sBooleanFilter->mExpr))->mFuncId == KNL_BUILTIN_FUNC_IS_EQUAL) &&
                     (((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgCount == 2) );

        /* Left Expression과 Right Expression을 가져옴 */
        sLeftExpr = ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
        sRightExpr = ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];

        /* Left Expression과 Right Expression의 Data Type이 동일해야 한다. */
        STL_TRY_THROW( sLeftExpr->mDataType == sRightExpr->mDataType,
                       RAMP_SKIP_TWO_TABLE_FILTER );

        /* Left에 존재하는 컬럼은 Base Table의 Column이거나 Sub Table의 Inner Column이어야 한다. */
        STL_TRY_THROW( ( sLeftExpr->mType == QLNC_EXPR_TYPE_COLUMN ) ||
                       ( (sLeftExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN) &&
                         (((qlncExprInnerColumn*)sLeftExpr)->mNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) ),
                       RAMP_SKIP_TWO_TABLE_FILTER );

        /* Right에 존재하는 컬럼은 Base Table의 Column이거나 Sub Table의 Inner Column이어야 한다. */
        STL_TRY_THROW( ( sRightExpr->mType == QLNC_EXPR_TYPE_COLUMN ) ||
                       ( (sRightExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN) &&
                         (((qlncExprInnerColumn*)sRightExpr)->mNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) ),
                       RAMP_SKIP_TWO_TABLE_FILTER );

        /* One Table Filter를 찾는다. */
        for( j = 0; j < aAndFilter->mOrCount; j++ )
        {
            /* Or가 없고 Boolean Filter이어야 한다. */
            STL_TRY_THROW( (aAndFilter->mOrFilters[j]->mFilterCount == 1) &&
                           (aAndFilter->mOrFilters[j]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER),
                           RAMP_SKIP_ONE_TABLE_FILTER );

            /* Boolean Filter 가져오기 */
            sTmpBooleanFilter = (qlncBooleanFilter*)(aAndFilter->mOrFilters[j]->mFilters[0]);

            /* Function에 단일 컬럼이 존재해야 한다. */
            STL_TRY_THROW( (sTmpBooleanFilter->mRefNodeList.mCount == 1) &&
                           (sTmpBooleanFilter->mRefNodeList.mHead->mRefColumnList.mCount == 1) &&
                           (sTmpBooleanFilter->mRefNodeList.mHead->mRefColumnList.mHead->mRefCount == 1),
                           RAMP_SKIP_ONE_TABLE_FILTER );

            /* One Table Filter 복사 */
            switch( sTmpBooleanFilter->mFuncID )
            {
                /* First Operator만 비교하면 되는 경우 */
                case KNL_BUILTIN_FUNC_IS:
                case KNL_BUILTIN_FUNC_IS_NOT:
                case KNL_BUILTIN_FUNC_IS_NULL:
                case KNL_BUILTIN_FUNC_IS_NOT_NULL:
                case KNL_BUILTIN_FUNC_IS_UNKNOWN:
                case KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN:
                case KNL_BUILTIN_FUNC_BETWEEN:
                case KNL_BUILTIN_FUNC_NOT_BETWEEN:
                case KNL_BUILTIN_FUNC_BETWEEN_SYMMETRIC:
                case KNL_BUILTIN_FUNC_NOT_BETWEEN_SYMMETRIC:
                    sTmpExpr = ((qlncExprFunction*)(sTmpBooleanFilter->mExpr))->mArgs[0];
                    if( qlncIsSameExpr( sTmpExpr, sLeftExpr ) == STL_TRUE )
                    {
                        sIsSameLeftExpr = STL_TRUE;
                    }
                    else if( qlncIsSameExpr( sTmpExpr, sRightExpr ) == STL_TRUE )
                    {
                        sIsSameLeftExpr = STL_FALSE;
                    }
                    else
                    {
                        STL_THROW( RAMP_SKIP_ONE_TABLE_FILTER );
                    }

                    /* Boolean Filter 복사 */
                    STL_TRY( qlncDuplicateExpr( aParamInfo,
                                                (qlncExprCommon*)sTmpBooleanFilter,
                                                (qlncExprCommon**)&sCopiedBooleanFilter,
                                                aEnv )
                             == STL_SUCCESS );

                    /* Expression 변경 복사 */
                    STL_TRY( qlncDuplicateExpr( aParamInfo,
                                                ( sIsSameLeftExpr == STL_TRUE
                                                  ? sRightExpr : sLeftExpr ),
                                                &sTmpExpr,
                                                aEnv )
                             == STL_SUCCESS );

                    ((qlncExprFunction*)(sCopiedBooleanFilter->mExpr))->mArgs[0] = sTmpExpr;

                    /* Node List 설정 */
                    QLNC_INIT_LIST( &(sCopiedBooleanFilter->mRefNodeList) );

                    sConvertExprParamInfo.mParamInfo = *aParamInfo;
                    sConvertExprParamInfo.mTableMapArr = NULL;
                    sConvertExprParamInfo.mOuterTableMapArr = NULL;
                    sConvertExprParamInfo.mInitExpr = NULL;
                    sConvertExprParamInfo.mCandExpr = NULL;
                    sConvertExprParamInfo.mExprPhraseFlag = 0;
                    sConvertExprParamInfo.mSubQueryExprList = NULL;
                    sConvertExprParamInfo.mExprSubQuery = NULL;
                    sConvertExprParamInfo.mRefNodeList = &(sCopiedBooleanFilter->mRefNodeList);

                    STL_TRY( qlncFindAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                                sCopiedBooleanFilter->mExpr,
                                                                aEnv )
                             == STL_SUCCESS );

                    /* Add And Filter에 추가 */
                    STL_TRY( qlncAppendFilterToAndFilter( aParamInfo,
                                                          &sAddAndFilter,
                                                          (qlncExprCommon*)sCopiedBooleanFilter,
                                                          aEnv )
                             == STL_SUCCESS );

                    break;

                    /* Two Operator에 대해 비교해야 되는 경우 */
                case KNL_BUILTIN_FUNC_IS_EQUAL:
                case KNL_BUILTIN_FUNC_IS_NOT_EQUAL:
                case KNL_BUILTIN_FUNC_IS_LESS_THAN:
                case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL:
                case KNL_BUILTIN_FUNC_IS_GREATER_THAN:
                case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL:
                    sTmpExpr = ((qlncExprFunction*)(sTmpBooleanFilter->mExpr))->mArgs[0];
                    if( ( sTmpExpr->mType == QLNC_EXPR_TYPE_COLUMN ) ||
                       ( (sTmpExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN) &&
                         (((qlncExprInnerColumn*)sTmpExpr)->mNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) ) )
                    {
                        if( qlncIsSameExpr( sTmpExpr, sLeftExpr ) == STL_TRUE )
                        {
                            sIsSameLeftExpr = STL_TRUE;
                        }
                        else if( qlncIsSameExpr( sTmpExpr, sRightExpr ) == STL_TRUE )
                        {
                            sIsSameLeftExpr = STL_FALSE;
                        }
                        else
                        {
                            STL_THROW( RAMP_SKIP_ONE_TABLE_FILTER );
                        }

                        /* Boolean Filter 복사 */
                        STL_TRY( qlncDuplicateExpr( aParamInfo,
                                                    (qlncExprCommon*)sTmpBooleanFilter,
                                                    (qlncExprCommon**)&sCopiedBooleanFilter,
                                                    aEnv )
                                 == STL_SUCCESS );

                        /* Expression 변경 복사 */
                        STL_TRY( qlncDuplicateExpr( aParamInfo,
                                                    ( sIsSameLeftExpr == STL_TRUE
                                                      ? sRightExpr : sLeftExpr ),
                                                    &sTmpExpr,
                                                    aEnv )
                                 == STL_SUCCESS );

                        ((qlncExprFunction*)(sCopiedBooleanFilter->mExpr))->mArgs[0] = sTmpExpr;
                    }
                    else
                    {
                        sTmpExpr = ((qlncExprFunction*)(sTmpBooleanFilter->mExpr))->mArgs[1];
                        if( ( sTmpExpr->mType == QLNC_EXPR_TYPE_COLUMN ) ||
                            ( (sTmpExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN) &&
                              (((qlncExprInnerColumn*)sTmpExpr)->mNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) ) )
                        {
                            if( qlncIsSameExpr( sTmpExpr, sLeftExpr ) == STL_TRUE )
                            {
                                sIsSameLeftExpr = STL_TRUE;
                            }
                            else if( qlncIsSameExpr( sTmpExpr, sRightExpr ) == STL_TRUE )
                            {
                                sIsSameLeftExpr = STL_FALSE;
                            }
                            else
                            {
                                STL_THROW( RAMP_SKIP_ONE_TABLE_FILTER );
                            }

                            /* Boolean Filter 복사 */
                            STL_TRY( qlncDuplicateExpr( aParamInfo,
                                                        (qlncExprCommon*)sTmpBooleanFilter,
                                                        (qlncExprCommon**)&sCopiedBooleanFilter,
                                                        aEnv )
                                     == STL_SUCCESS );

                            /* Expression 변경 복사 */
                            STL_TRY( qlncDuplicateExpr( aParamInfo,
                                                        ( sIsSameLeftExpr == STL_TRUE
                                                          ? sRightExpr : sLeftExpr ),
                                                        &sTmpExpr,
                                                        aEnv )
                                     == STL_SUCCESS );

                            ((qlncExprFunction*)(sCopiedBooleanFilter->mExpr))->mArgs[1] = sTmpExpr;
                        }
                        else
                        {
                            STL_THROW( RAMP_SKIP_ONE_TABLE_FILTER );
                        }
                    }

                    /* Node List 설정 */
                    QLNC_INIT_LIST( &(sCopiedBooleanFilter->mRefNodeList) );

                    sConvertExprParamInfo.mParamInfo = *aParamInfo;
                    sConvertExprParamInfo.mTableMapArr = NULL;
                    sConvertExprParamInfo.mOuterTableMapArr = NULL;
                    sConvertExprParamInfo.mInitExpr = NULL;
                    sConvertExprParamInfo.mCandExpr = NULL;
                    sConvertExprParamInfo.mExprPhraseFlag = 0;
                    sConvertExprParamInfo.mSubQueryExprList = NULL;
                    sConvertExprParamInfo.mExprSubQuery = NULL;
                    sConvertExprParamInfo.mRefNodeList = &(sCopiedBooleanFilter->mRefNodeList);

                    STL_TRY( qlncFindAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                                sCopiedBooleanFilter->mExpr,
                                                                aEnv )
                             == STL_SUCCESS );

                    /* Add And Filter에 추가 */
                    STL_TRY( qlncAppendFilterToAndFilter( aParamInfo,
                                                          &sAddAndFilter,
                                                          (qlncExprCommon*)sCopiedBooleanFilter,
                                                          aEnv )
                             == STL_SUCCESS );

                    break;

                default:
                    break;
            }

            STL_RAMP( RAMP_SKIP_ONE_TABLE_FILTER );
        }

        STL_RAMP( RAMP_SKIP_TWO_TABLE_FILTER );
    }

    if( sAddAndFilter.mOrCount > 0 )
    {
        STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                    aAndFilter,
                                                    sAddAndFilter.mOrCount,
                                                    sAddAndFilter.mOrFilters,
                                                    aEnv )
                 == STL_SUCCESS );
    }

    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Filter Push Down을 수행한다.
 * @param[in]       aParamInfo  Parameter Info
 * @param[in,out]   aNode       Candidate Node
 * @param[in]       aEnv        Environment
 */
stlStatus qlncFilterPushDown( qlncParamInfo     * aParamInfo,
                              qlncNodeCommon    * aNode,
                              qllEnv            * aEnv )
{
    stlInt32      i;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aNode->mNodeType )
    {
        case QLNC_NODE_TYPE_QUERY_SET:
            STL_TRY( qlncFilterPushDown( aParamInfo,
                                         ((qlncQuerySet*)aNode)->mChildNode,
                                         aEnv )
                     == STL_SUCCESS );

            break;

        case QLNC_NODE_TYPE_SET_OP:
            for( i = 0; i < ((qlncSetOP*)aNode)->mChildCount; i++ )
            {
                STL_TRY( qlncFilterPushDown( aParamInfo,
                                             ((qlncSetOP*)aNode)->mChildNodeArr[i],
                                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_QUERY_SPEC:
            STL_TRY_THROW( ((qlncQuerySpec*)aNode)->mQueryTransformHint->mAllowedQueryTransform == STL_TRUE,
                           RAMP_FINISH );

            STL_TRY( qlncFilterPushDown( aParamInfo,
                                         ((qlncQuerySpec*)aNode)->mChildNode,
                                         aEnv )
                     == STL_SUCCESS );

            if( ((qlncQuerySpec*)aNode)->mNonFilterSubQueryExprList != NULL )
            {
                STL_TRY( qlncFPDSubQueryExprList( aParamInfo,
                                                  ((qlncQuerySpec*)aNode)->mNonFilterSubQueryExprList,
                                                  aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_BASE_TABLE:
            if( ((qlncBaseTableNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncFPDSubQueryOnAndFilter( aParamInfo,
                                                     ((qlncBaseTableNode*)aNode)->mFilter,
                                                     aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_SUB_TABLE:
            if( ((qlncSubTableNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncFPDSubTableNode( aParamInfo,
                                              (qlncSubTableNode*)aNode,
                                              aEnv )
                         == STL_SUCCESS );
            }

            STL_TRY( qlncFilterPushDown( aParamInfo,
                                         ((qlncSubTableNode*)aNode)->mQueryNode,
                                         aEnv )
                     == STL_SUCCESS );

            if( ((qlncSubTableNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncFPDSubQueryOnAndFilter( aParamInfo,
                                                     ((qlncSubTableNode*)aNode)->mFilter,
                                                     aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_JOIN_TABLE:
            switch( ((qlncJoinTableNode*)aNode)->mJoinType )
            {
                case QLNC_JOIN_TYPE_INNER:
                case QLNC_JOIN_TYPE_SEMI:
                case QLNC_JOIN_TYPE_ANTI_SEMI:
                case QLNC_JOIN_TYPE_ANTI_SEMI_NA:
                    STL_TRY( qlncFPDNonOuterJoinTableNode( aParamInfo,
                                                           (qlncJoinTableNode*)aNode,
                                                           aEnv )
                             == STL_SUCCESS );
                    break;
                case QLNC_JOIN_TYPE_OUTER:
                    STL_TRY( qlncFPDOuterJoinTableNode( aParamInfo,
                                                        (qlncJoinTableNode*)aNode,
                                                        aEnv )
                             == STL_SUCCESS );
                    break;
                default:
                    STL_DASSERT( 0 );
                    break;
            }

            if( ((qlncJoinTableNode*)aNode)->mJoinCond!= NULL )
            {
                STL_TRY( qlncFPDSubQueryOnAndFilter( aParamInfo,
                                                     ((qlncJoinTableNode*)aNode)->mJoinCond,
                                                     aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncJoinTableNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncFPDSubQueryOnAndFilter( aParamInfo,
                                                     ((qlncJoinTableNode*)aNode)->mFilter,
                                                     aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_FLAT_INSTANT:
            STL_DASSERT( 0 );
            break;

        case QLNC_NODE_TYPE_SORT_INSTANT:
            if( (((qlncSortInstantNode*)aNode)->mKeyFilter != NULL) ||
                (((qlncSortInstantNode*)aNode)->mRecFilter != NULL) ||
                (((qlncSortInstantNode*)aNode)->mFilter != NULL) )
            {
                STL_TRY( qlncFPDSortInstantNode( aParamInfo,
                                                 (qlncSortInstantNode*)aNode,
                                                 aEnv )
                         == STL_SUCCESS );
            }

            STL_TRY( qlncFilterPushDown( aParamInfo,
                                         ((qlncSortInstantNode*)aNode)->mChildNode,
                                         aEnv )
                     == STL_SUCCESS );

            if( ((qlncSortInstantNode*)aNode)->mFilter != NULL )
            {
                /* @todo 일반적으로는 들어올 수 없으나 복잡한 질의에서는 들어올 가능성도 있음 */
                STL_TRY( qlncFPDSubQueryOnAndFilter( aParamInfo,
                                                     ((qlncSortInstantNode*)aNode)->mFilter,
                                                     aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncSortInstantNode*)aNode)->mNonFilterSubQueryExprList != NULL )
            {
                STL_TRY( qlncFPDSubQueryExprList(
                             aParamInfo,
                             ((qlncSortInstantNode*)aNode)->mNonFilterSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_HASH_INSTANT:
            if( (((qlncHashInstantNode*)aNode)->mKeyFilter != NULL) ||
                (((qlncHashInstantNode*)aNode)->mRecFilter != NULL) ||
                (((qlncHashInstantNode*)aNode)->mFilter != NULL) )
            {
                STL_TRY( qlncFPDHashInstantNode( aParamInfo,
                                                 (qlncHashInstantNode*)aNode,
                                                 aEnv )
                         == STL_SUCCESS );
            }

            STL_TRY( qlncFilterPushDown( aParamInfo,
                                         ((qlncHashInstantNode*)aNode)->mChildNode,
                                         aEnv )
                     == STL_SUCCESS );

            if( ((qlncHashInstantNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncFPDSubQueryOnAndFilter( aParamInfo,
                                                     ((qlncHashInstantNode*)aNode)->mFilter,
                                                     aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncHashInstantNode*)aNode)->mNonFilterSubQueryExprList != NULL )
            {
                STL_TRY( qlncFPDSubQueryExprList(
                             aParamInfo,
                             ((qlncHashInstantNode*)aNode)->mNonFilterSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        default:
            STL_DASSERT( 0 );
            break;
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief And Filter에 존재하는 SubQuery에 대하여 Filter Push Down을 수행한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aAndFilter  AndFilter
 * @param[in]   aEnv        Environment
 */
stlStatus qlncFPDSubQueryOnAndFilter( qlncParamInfo * aParamInfo,
                                      qlncAndFilter * aAndFilter,
                                      qllEnv        * aEnv )
{
    stlInt32          i;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter != NULL, QLL_ERROR_STACK(aEnv) );


    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        if( aAndFilter->mOrFilters[i]->mSubQueryExprList != NULL )
        {
            STL_TRY( qlncFPDSubQueryExprList( aParamInfo,
                                              aAndFilter->mOrFilters[i]->mSubQueryExprList,
                                              aEnv )
                     == STL_SUCCESS );
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Expression List에 존재하는 SubQuery에 대하여 Filter Push Down을 수행한다.
 * @param[in]   aParamInfo          Parameter Info
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncFPDSubQueryExprList( qlncParamInfo    * aParamInfo,
                                   qlncRefExprList  * aSubQueryExprList,
                                   qllEnv           * aEnv )
{
    qlncRefExprItem * sRefExprItem  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );


    sRefExprItem = aSubQueryExprList->mHead;
    while( sRefExprItem != NULL )
    {
        STL_TRY( qlncFilterPushDown( aParamInfo,
                                     ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode,
                                     aEnv )
                 == STL_SUCCESS );

        sRefExprItem = sRefExprItem->mNext;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Table Node에 대한 Filter Push Down을 수행한다.
 * @param[in]       aParamInfo      Parameter Info
 * @param[in,out]   aSubTableNode   Candidate Sub Table Node
 * @param[in]       aEnv            Environment
 */
stlStatus qlncFPDSubTableNode( qlncParamInfo        * aParamInfo,
                               qlncSubTableNode     * aSubTableNode,
                               qllEnv               * aEnv )
{
    stlInt32          i;
    stlInt32          sTargetCount;
    qlncAndFilter     sPushFilter;
    qlncTarget      * sTargetArr    = NULL;
    stlInt32        * sTargetIdxArr = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubTableNode->mFilter != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Query Node에 대하여 Filter Push Down이 가능한지 체크한다.
     */

    /* Target정보를 얻고 Target Index Array를 생성한다. */
    if( aSubTableNode->mQueryNode->mNodeType == QLNC_NODE_TYPE_QUERY_SET )
    {
        sTargetCount = ((qlncQuerySet*)(aSubTableNode->mQueryNode))->mSetTargetCount;
        sTargetArr = ((qlncQuerySet*)(aSubTableNode->mQueryNode))->mSetTargetArr;
    }
    else
    {
        sTargetCount = ((qlncQuerySpec*)(aSubTableNode->mQueryNode))->mTargetCount;
        sTargetArr = ((qlncQuerySpec*)(aSubTableNode->mQueryNode))->mTargetArr;
    }

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( stlInt32 ) * sTargetCount,
                                (void**)&sTargetIdxArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sTargetIdxArr, 0x00, STL_SIZEOF( stlInt32 ) * sTargetCount );

    /* Push 가능한 Filter만 선별하여 사용되는 Column들의 Target Index를 체크한다. */
    QLNC_INIT_AND_FILTER( &sPushFilter );
    for( i = 0; i < aSubTableNode->mFilter->mOrCount; i++ )
    {
        /* Constant가 아닌 SubQuery를 가진 경우 Push 불가 */
        if( aSubTableNode->mFilter->mOrFilters[i]->mSubQueryExprList != NULL )
        {
            /* SubQuery는 무조건 push 하지 않는다. (Cost Optimizer에서 판단하도록 한다.) */
            continue;
        }

        (void)qlncFPDSetUsedTargetIdxOnFilter( aSubTableNode->mNodeCommon.mNodeID,
                                               sTargetCount,
                                               sTargetIdxArr,
                                               (qlncExprCommon*)(aSubTableNode->mFilter->mOrFilters[i]) );
    }

    /* Query Node에 대하여 Filter Push Down이 가능한지 체크한다. */
    STL_TRY_THROW( qlncFPDCheckQueryNode( sTargetArr,
                                          sTargetIdxArr,
                                          aSubTableNode->mQueryNode )
                   == STL_TRUE,
                   RAMP_FINISH );


    /* Push 가능한 Filter만 선별 */
    QLNC_INIT_AND_FILTER( &sPushFilter );
    for( i = 0; i < aSubTableNode->mFilter->mOrCount; i++ )
    {
        /* Constant가 아닌 SubQuery를 가진 경우 Push 불가 */
        if( aSubTableNode->mFilter->mOrFilters[i]->mSubQueryExprList != NULL )
        {
            /* SubQuery는 무조건 push 하지 않는다. (Cost Optimizer에서 판단하도록 한다.) */
            continue;
        }

        STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                    &sPushFilter,
                                                    1,
                                                    &(aSubTableNode->mFilter->mOrFilters[i]),
                                                    aEnv )
                 == STL_SUCCESS );

        aSubTableNode->mFilter->mOrFilters[i] = NULL;
    }

    /* Push Filter가 없으면 종료 */
    STL_TRY_THROW( sPushFilter.mOrCount > 0, RAMP_FINISH );

    /* Sub Table Node의 Filter Compact */
    STL_TRY( qlncCompactAndFilter( aSubTableNode->mFilter,
                                   aEnv )
             == STL_SUCCESS );

    if( aSubTableNode->mFilter->mOrCount == 0 )
    {
        aSubTableNode->mFilter = NULL;
    }

    STL_TRY( qlncFPDPushFilterOnQueryNode( aParamInfo,
                                           aSubTableNode,
                                           aSubTableNode->mQueryNode,
                                           &sPushFilter,
                                           aEnv )
             == STL_SUCCESS );


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Non Outer Join Table Node에 대한 Filter Push Down을 수행한다.
 * @param[in]       aParamInfo      Parameter Info
 * @param[in,out]   aJoinNode       Candidate Join Node
 * @param[in]       aEnv            Environment
 */
stlStatus qlncFPDNonOuterJoinTableNode( qlncParamInfo       * aParamInfo,
                                        qlncJoinTableNode   * aJoinNode,
                                        qllEnv              * aEnv )
{
    stlInt32          i, j;
    stlBool           sIsUsable;
    qlncNodeArray   * sNodeArr          = NULL;
    qlncAndFilter   * sAndFilter        = NULL;
    qlncAndFilter   * sPushAndFilter    = NULL;
    qlncAndFilter  ** sTmpAndFilter     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinNode->mNodeCommon.mNodeType == QLNC_NODE_TYPE_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aJoinNode->mJoinType == QLNC_JOIN_TYPE_INNER) ||
                        (aJoinNode->mJoinType == QLNC_JOIN_TYPE_SEMI) ||
                        (aJoinNode->mJoinType == QLNC_JOIN_TYPE_ANTI_SEMI) ||
                        (aJoinNode->mJoinType == QLNC_JOIN_TYPE_ANTI_SEMI_NA),
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Join Condition
     */

    if( aJoinNode->mJoinCond != NULL )
    {
        sNodeArr = &(aJoinNode->mNodeArray);
        for( i = 0; i < sNodeArr->mCurCnt; i++ )
        {
            /* 현재 노드에 존재하는 모든 Filter를 찾는다. */
            sAndFilter = aJoinNode->mJoinCond;
            for( j = 0; j < sAndFilter->mOrCount; j++ )
            {
                if( sAndFilter->mOrFilters[j] == NULL )
                {
                    continue;
                }

                /* SubQuery가 존재하는 경우 */
                if( sAndFilter->mOrFilters[j]->mSubQueryExprList != NULL )
                {
                    /* SubQuery는 무조건 push 하지 않는다. (Cost Optimizer에서 판단하도록 한다.) */
                    continue;
                }

                sIsUsable = qlncIsUsableOrFilterOnTableNode( sNodeArr->mNodeArr[i],
                                                             sAndFilter->mOrFilters[j] );
                if( sIsUsable == STL_TRUE )
                {
                    if( sPushAndFilter == NULL )
                    {
                        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                    STL_SIZEOF( qlncAndFilter ),
                                                    (void**)&sPushAndFilter,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        QLNC_SET_EXPR_COMMON( &sPushAndFilter->mCommon,
                                              &QLL_CANDIDATE_MEM( aEnv ),
                                              aEnv,
                                              QLNC_EXPR_TYPE_AND_FILTER,
                                              sAndFilter->mOrFilters[j]->mCommon.mExprPhraseFlag,        
                                              sAndFilter->mOrFilters[j]->mCommon.mIterationTime,         
                                              sAndFilter->mOrFilters[j]->mCommon.mPosition,              
                                              sAndFilter->mOrFilters[j]->mCommon.mColumnName,
                                              DTL_TYPE_BOOLEAN );                                       

                        sPushAndFilter->mOrCount = 1;

                        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                    STL_SIZEOF( qlncOrFilter* ),
                                                    (void**)&(sPushAndFilter->mOrFilters),
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        sPushAndFilter->mOrFilters[0] = sAndFilter->mOrFilters[j];
                        sAndFilter->mOrFilters[j] = NULL;
                    }
                    else
                    {
                        STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                                    sPushAndFilter,
                                                                    1,
                                                                    &(sAndFilter->mOrFilters[j]),
                                                                    aEnv )
                                 == STL_SUCCESS );
                        sAndFilter->mOrFilters[j] = NULL;
                    }
                }
            }

            /* 해당 노드에 Filter를 Push한다. */
            switch( sNodeArr->mNodeArr[i]->mNodeType )
            {
                case QLNC_NODE_TYPE_BASE_TABLE:
                    sTmpAndFilter = &(((qlncBaseTableNode*)(sNodeArr->mNodeArr[i]))->mFilter);
                    break;

                case QLNC_NODE_TYPE_SUB_TABLE:
                    sTmpAndFilter = &(((qlncSubTableNode*)(sNodeArr->mNodeArr[i]))->mFilter);
                    break;

                case QLNC_NODE_TYPE_JOIN_TABLE:
                    sTmpAndFilter = &(((qlncJoinTableNode*)(sNodeArr->mNodeArr[i]))->mFilter);
                    break;

                default:
                    STL_DASSERT( 0 );
                    break;
            }

            if( *sTmpAndFilter == NULL )
            {
                *sTmpAndFilter = sPushAndFilter;
            }
            else
            {
                if( sPushAndFilter != NULL )
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                                *sTmpAndFilter,
                                                                sPushAndFilter->mOrCount,
                                                                sPushAndFilter->mOrFilters,
                                                                aEnv )
                             == STL_SUCCESS );
                }
            }

            sPushAndFilter = NULL;
        }

        /* Compact And Filter */
        STL_TRY( qlncCompactAndFilter( aJoinNode->mJoinCond,
                                       aEnv )
                 == STL_SUCCESS );

        if( aJoinNode->mJoinCond->mOrCount == 0 )
        {
            aJoinNode->mJoinCond = NULL;
        }
    }



    /**
     * Join Filter
     */

    if( aJoinNode->mFilter != NULL )
    {
        sNodeArr = &(aJoinNode->mNodeArray);
        for( i = 0; i < sNodeArr->mCurCnt; i++ )
        {
            /* 현재 노드에 존재하는 모든 Filter를 찾는다. */
            sAndFilter = aJoinNode->mFilter;
            for( j = 0; j < sAndFilter->mOrCount; j++ )
            {
                if( sAndFilter->mOrFilters[j] == NULL )
                {
                    continue;
                }

                /* SubQuery가 존재하는 경우 */
                if( sAndFilter->mOrFilters[j]->mSubQueryExprList != NULL )
                {
                    /* SubQuery는 무조건 push 하지 않는다. (Cost Optimizer에서 판단하도록 한다.) */
                    continue;
                }

                sIsUsable = qlncIsUsableOrFilterOnTableNode( sNodeArr->mNodeArr[i],
                                                             sAndFilter->mOrFilters[j] );
                if( sIsUsable == STL_TRUE )
                {
                    if( sPushAndFilter == NULL )
                    {
                        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                    STL_SIZEOF( qlncAndFilter ),
                                                    (void**)&sPushAndFilter,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        QLNC_SET_EXPR_COMMON( &sPushAndFilter->mCommon,
                                              &QLL_CANDIDATE_MEM( aEnv ),
                                              aEnv,
                                              QLNC_EXPR_TYPE_AND_FILTER,
                                              sAndFilter->mOrFilters[j]->mCommon.mExprPhraseFlag,        
                                              sAndFilter->mOrFilters[j]->mCommon.mIterationTime,         
                                              sAndFilter->mOrFilters[j]->mCommon.mPosition,              
                                              sAndFilter->mOrFilters[j]->mCommon.mColumnName,
                                              DTL_TYPE_BOOLEAN );                                       

                        sPushAndFilter->mOrCount = 1;

                        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                    STL_SIZEOF( qlncOrFilter* ),
                                                    (void**)&(sPushAndFilter->mOrFilters),
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        sPushAndFilter->mOrFilters[0] = sAndFilter->mOrFilters[j];
                        sAndFilter->mOrFilters[j] = NULL;
                    }
                    else
                    {
                        STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                                    sPushAndFilter,
                                                                    1,
                                                                    &(sAndFilter->mOrFilters[j]),
                                                                    aEnv )
                                 == STL_SUCCESS );
                        sAndFilter->mOrFilters[j] = NULL;
                    }
                }
            }

            /* 해당 노드에 Filter를 Push한다. */
            switch( sNodeArr->mNodeArr[i]->mNodeType )
            {
                case QLNC_NODE_TYPE_BASE_TABLE:
                    sTmpAndFilter = &(((qlncBaseTableNode*)(sNodeArr->mNodeArr[i]))->mFilter);
                    break;

                case QLNC_NODE_TYPE_SUB_TABLE:
                    sTmpAndFilter = &(((qlncSubTableNode*)(sNodeArr->mNodeArr[i]))->mFilter);
                    break;

                case QLNC_NODE_TYPE_JOIN_TABLE:
                    sTmpAndFilter = &(((qlncJoinTableNode*)(sNodeArr->mNodeArr[i]))->mFilter);
                    break;

                default:
                    STL_DASSERT( 0 );
                    break;
            }

            if( *sTmpAndFilter == NULL )
            {
                *sTmpAndFilter = sPushAndFilter;
            }
            else
            {
                if( sPushAndFilter != NULL )
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                                *sTmpAndFilter,
                                                                sPushAndFilter->mOrCount,
                                                                sPushAndFilter->mOrFilters,
                                                                aEnv )
                             == STL_SUCCESS );
                }
            }

            sPushAndFilter = NULL;
        }

        /* Compact And Filter */
        STL_TRY( qlncCompactAndFilter( aJoinNode->mFilter,
                                       aEnv )
                 == STL_SUCCESS );

        if( aJoinNode->mFilter->mOrCount == 0 )
        {
            aJoinNode->mFilter = NULL;
        }
    }


    /**
     * 하위 노드가 Sub Table Node이거나 Inner Join Node, Outer Join Node인 경우 Filter Predicate Down 수행
     */

    sNodeArr = &(aJoinNode->mNodeArray);
    for( i = 0; i < sNodeArr->mCurCnt; i++ )
    {
        if( sNodeArr->mNodeArr[i]->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
        {
            STL_TRY( qlncFilterPushDown( aParamInfo,
                                         sNodeArr->mNodeArr[i],
                                         aEnv )
                     == STL_SUCCESS );
        }
        else if( sNodeArr->mNodeArr[i]->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE )
        {
            switch( ((qlncJoinTableNode*)(sNodeArr->mNodeArr[i]))->mJoinType )
            {
                case QLNC_JOIN_TYPE_INNER:
                case QLNC_JOIN_TYPE_SEMI:
                case QLNC_JOIN_TYPE_ANTI_SEMI:
                case QLNC_JOIN_TYPE_ANTI_SEMI_NA:
                    STL_TRY( qlncFPDNonOuterJoinTableNode( aParamInfo,
                                                           (qlncJoinTableNode*)(sNodeArr->mNodeArr[i]),
                                                           aEnv )
                             == STL_SUCCESS );
                    break;
                case QLNC_JOIN_TYPE_OUTER:
                    STL_TRY( qlncFPDOuterJoinTableNode( aParamInfo,
                                                        (qlncJoinTableNode*)(sNodeArr->mNodeArr[i]),
                                                        aEnv )
                             == STL_SUCCESS );
                    break;
                default:
                    STL_DASSERT( 0 );
                    break;
            }
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Outer Join Table Node에 대한 Filter Push Down을 수행한다.
 * @param[in]       aParamInfo      Parameter Info
 * @param[in,out]   aJoinNode       Candidate Join Node
 * @param[in]       aEnv            Environment
 */
stlStatus qlncFPDOuterJoinTableNode( qlncParamInfo          * aParamInfo,
                                     qlncJoinTableNode      * aJoinNode,
                                     qllEnv                 * aEnv )
{
    stlInt32          i;
    stlBool           sIsUsable;
    qlncNodeCommon  * sNode             = NULL;
    qlncAndFilter   * sAndFilter        = NULL;
    qlncAndFilter   * sPushAndFilter    = NULL;
    qlncAndFilter  ** sTmpAndFilter     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinNode->mNodeCommon.mNodeType == QLNC_NODE_TYPE_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinNode->mJoinType == QLNC_JOIN_TYPE_OUTER, QLL_ERROR_STACK(aEnv) );


    /* Full Outer Join은 Filter를 Push할 수 없다. */
    STL_TRY_THROW( aJoinNode->mJoinDirect != QLNC_JOIN_DIRECT_FULL,
                   RAMP_FINISH );


    /**
     * Join Condition
     */

    if( aJoinNode->mJoinCond != NULL )
    {
        /* Outer Join Type별 Node 찾기 */
        if( aJoinNode->mJoinDirect == QLNC_JOIN_DIRECT_LEFT )
        {
            /* Left Outer Join은 Right Table Node에만 Filter Push Down 가능 */
            sNode = aJoinNode->mNodeArray.mNodeArr[1];
        }
        else
        {
            STL_DASSERT( aJoinNode->mJoinDirect == QLNC_JOIN_DIRECT_RIGHT );
            /* Right Outer Join은 Left Table Node에만 Filter Push Down 가능 */
            sNode = aJoinNode->mNodeArray.mNodeArr[0];
        }

        /* 현재 노드에 존재하는 모든 Filter를 찾는다. */
        sAndFilter = aJoinNode->mJoinCond;
        for( i = 0; i < sAndFilter->mOrCount; i++ )
        {
            if( sAndFilter->mOrFilters[i] == NULL )
            {
                continue;
            }

            sIsUsable = qlncIsUsableOrFilterOnTableNode( sNode,
                                                         sAndFilter->mOrFilters[i] );
            if( sIsUsable == STL_TRUE )
            {
                if( sPushAndFilter == NULL )
                {
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncAndFilter ),
                                                (void**)&sPushAndFilter,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    QLNC_SET_EXPR_COMMON( &sPushAndFilter->mCommon,
                                          &QLL_CANDIDATE_MEM( aEnv ),
                                          aEnv,
                                          QLNC_EXPR_TYPE_AND_FILTER,
                                          sAndFilter->mOrFilters[i]->mCommon.mExprPhraseFlag,        
                                          sAndFilter->mOrFilters[i]->mCommon.mIterationTime,         
                                          sAndFilter->mOrFilters[i]->mCommon.mPosition,              
                                          sAndFilter->mOrFilters[i]->mCommon.mColumnName,
                                          DTL_TYPE_BOOLEAN );                                       

                    sPushAndFilter->mOrCount = 1;

                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncOrFilter* ),
                                                (void**)&(sPushAndFilter->mOrFilters),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    sPushAndFilter->mOrFilters[0] = sAndFilter->mOrFilters[i];
                    sAndFilter->mOrFilters[i] = NULL;
                }
                else
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                                sPushAndFilter,
                                                                1,
                                                                &(sAndFilter->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                    sAndFilter->mOrFilters[i] = NULL;
                }
            }
        }

        /* 해당 노드에 Filter를 Push한다. */
        switch( sNode->mNodeType )
        {
            case QLNC_NODE_TYPE_BASE_TABLE:
                sTmpAndFilter = &(((qlncBaseTableNode*)sNode)->mFilter);
                break;

            case QLNC_NODE_TYPE_SUB_TABLE:
                sTmpAndFilter = &(((qlncSubTableNode*)sNode)->mFilter);
                break;

            case QLNC_NODE_TYPE_JOIN_TABLE:
                sTmpAndFilter = &(((qlncJoinTableNode*)sNode)->mFilter);
                break;

            default:
                STL_DASSERT( 0 );
                break;
        }

        if( *sTmpAndFilter == NULL )
        {
            *sTmpAndFilter = sPushAndFilter;
        }
        else
        {
            if( sPushAndFilter != NULL )
            {
                /* @todo 일반적으로는 들어올 수 없으나 복잡한 질의에서는 들어올 가능성도 있음 */
                STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                            *sTmpAndFilter,
                                                            sPushAndFilter->mOrCount,
                                                            sPushAndFilter->mOrFilters,
                                                            aEnv )
                         == STL_SUCCESS );
            }
        }

        sPushAndFilter = NULL;

        /* Compact And Filter */
        STL_TRY( qlncCompactAndFilter( aJoinNode->mJoinCond,
                                       aEnv )
                 == STL_SUCCESS );

        if( aJoinNode->mJoinCond->mOrCount == 0 )
        {
            aJoinNode->mJoinCond = NULL;
        }
    }


    /**
     * Join Filter
     */

    if( aJoinNode->mFilter != NULL )
    {
        /* Outer Join Type별 Node 찾기 */
        if( aJoinNode->mJoinDirect == QLNC_JOIN_DIRECT_LEFT )
        {
            /* Left Outer Join은 Left Table Node에만 Filter Push Down 가능 */
            sNode = aJoinNode->mNodeArray.mNodeArr[0];
        }
        else
        {
            STL_DASSERT( aJoinNode->mJoinDirect == QLNC_JOIN_DIRECT_RIGHT );
            /* Right Outer Join은 Right Table Node에만 Filter Push Down 가능 */
            sNode = aJoinNode->mNodeArray.mNodeArr[1];
        }

        /* 현재 노드에 존재하는 모든 Filter를 찾는다. */
        sAndFilter = aJoinNode->mFilter;
        for( i = 0; i < sAndFilter->mOrCount; i++ )
        {
            if( sAndFilter->mOrFilters[i] == NULL )
            {
                continue;
            }

            sIsUsable = qlncIsUsableOrFilterOnTableNode( sNode,
                                                         sAndFilter->mOrFilters[i] );
            if( sIsUsable == STL_TRUE )
            {
                if( sPushAndFilter == NULL )
                {
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncAndFilter ),
                                                (void**)&sPushAndFilter,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    QLNC_SET_EXPR_COMMON( &sPushAndFilter->mCommon,
                                          &QLL_CANDIDATE_MEM( aEnv ),
                                          aEnv,
                                          QLNC_EXPR_TYPE_AND_FILTER,
                                          sAndFilter->mOrFilters[i]->mCommon.mExprPhraseFlag,        
                                          sAndFilter->mOrFilters[i]->mCommon.mIterationTime,         
                                          sAndFilter->mOrFilters[i]->mCommon.mPosition,              
                                          sAndFilter->mOrFilters[i]->mCommon.mColumnName,
                                          DTL_TYPE_BOOLEAN );                                       

                    sPushAndFilter->mOrCount = 1;

                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncOrFilter* ),
                                                (void**)&(sPushAndFilter->mOrFilters),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    sPushAndFilter->mOrFilters[0] = sAndFilter->mOrFilters[i];
                    sAndFilter->mOrFilters[i] = NULL;
                }
                else
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                                sPushAndFilter,
                                                                1,
                                                                &(sAndFilter->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                    sAndFilter->mOrFilters[i] = NULL;
                }
            }
        }

        /* 해당 노드에 Filter를 Push한다. */
        switch( sNode->mNodeType )
        {
            case QLNC_NODE_TYPE_BASE_TABLE:
                sTmpAndFilter = &(((qlncBaseTableNode*)sNode)->mFilter);
                break;

            case QLNC_NODE_TYPE_SUB_TABLE:
                sTmpAndFilter = &(((qlncSubTableNode*)sNode)->mFilter);
                break;

            case QLNC_NODE_TYPE_JOIN_TABLE:
                sTmpAndFilter = &(((qlncJoinTableNode*)sNode)->mFilter);
                break;

            default:
                STL_DASSERT( 0 );
                break;
        }

        if( *sTmpAndFilter == NULL )
        {
            *sTmpAndFilter = sPushAndFilter;
        }
        else
        {
            if( sPushAndFilter != NULL )
            {
                /* @todo 일반적으로는 들어올 수 없으나 복잡한 질의에서는 들어올 가능성도 있음 */
                STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                            *sTmpAndFilter,
                                                            sPushAndFilter->mOrCount,
                                                            sPushAndFilter->mOrFilters,
                                                            aEnv )
                         == STL_SUCCESS );
            }
        }

        sPushAndFilter = NULL;

        /* Compact And Filter */
        STL_TRY( qlncCompactAndFilter( aJoinNode->mFilter,
                                       aEnv )
                 == STL_SUCCESS );

        if( aJoinNode->mFilter->mOrCount == 0 )
        {
            aJoinNode->mFilter = NULL;
        }
    }

    STL_RAMP( RAMP_FINISH );


    /**
     * 하위 노드가 Sub Table Node이거나 Inner Join Node, Outer Join Node인 경우 Filter Predicate Down 수행
     */

    sNode = aJoinNode->mNodeArray.mNodeArr[0];
    if( sNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
    {
        STL_TRY( qlncFilterPushDown( aParamInfo,
                                     sNode,
                                     aEnv )
                 == STL_SUCCESS );
    }
    else if( sNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE )
    {
        switch( ((qlncJoinTableNode*)sNode)->mJoinType )
        {
            case QLNC_JOIN_TYPE_INNER:
            case QLNC_JOIN_TYPE_SEMI:
            case QLNC_JOIN_TYPE_ANTI_SEMI:
            case QLNC_JOIN_TYPE_ANTI_SEMI_NA:
                STL_TRY( qlncFPDNonOuterJoinTableNode( aParamInfo,
                                                       (qlncJoinTableNode*)sNode,
                                                       aEnv )
                         == STL_SUCCESS );
                break;
            case QLNC_JOIN_TYPE_OUTER:
                STL_TRY( qlncFPDOuterJoinTableNode( aParamInfo,
                                                    (qlncJoinTableNode*)sNode,
                                                    aEnv )
                         == STL_SUCCESS );
                break;
            default:
                STL_DASSERT( 0 );
                break;
        }
    }

    sNode = aJoinNode->mNodeArray.mNodeArr[1];
    if( sNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
    {
        STL_TRY( qlncFilterPushDown( aParamInfo,
                                     sNode,
                                     aEnv )
                 == STL_SUCCESS );
    }
    else if( sNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE )
    {
        switch( ((qlncJoinTableNode*)sNode)->mJoinType )
        {
            case QLNC_JOIN_TYPE_INNER:
            case QLNC_JOIN_TYPE_SEMI:
            case QLNC_JOIN_TYPE_ANTI_SEMI:
            case QLNC_JOIN_TYPE_ANTI_SEMI_NA:
                STL_TRY( qlncFPDNonOuterJoinTableNode( aParamInfo,
                                                       (qlncJoinTableNode*)sNode,
                                                       aEnv )
                         == STL_SUCCESS );
                break;
            case QLNC_JOIN_TYPE_OUTER:
                STL_TRY( qlncFPDOuterJoinTableNode( aParamInfo,
                                                    (qlncJoinTableNode*)sNode,
                                                    aEnv )
                         == STL_SUCCESS );
                break;
            default:
                STL_DASSERT( 0 );
                break;
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sort Instant Node에 대한 Filter Push Down을 수행한다.
 * @param[in]       aParamInfo          Parameter Info
 * @param[in,out]   aSortInstantNode    Candidate Sort Instant Node
 * @param[in]       aEnv                Environment
 */
stlStatus qlncFPDSortInstantNode( qlncParamInfo         * aParamInfo,
                                  qlncSortInstantNode   * aSortInstantNode,
                                  qllEnv                * aEnv )
{
    stlInt32              i;
    qlncRefExprItem     * sRefExprItem      = NULL;
    qlncAndFilter         sPushFilter;
    qlncAndFilter      ** sChildAndFilter   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSortInstantNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aSortInstantNode->mKeyFilter != NULL) ||
                        (aSortInstantNode->mFilter != NULL),
                        QLL_ERROR_STACK(aEnv) );


    /**
     * @todo Child Node가 Set Operator인 경우 Filter Push Down은 향후 진행
     */

    STL_TRY_THROW( (aSortInstantNode->mChildNode->mNodeType != QLNC_NODE_TYPE_QUERY_SET) &&
                   (aSortInstantNode->mChildNode->mNodeType != QLNC_NODE_TYPE_SET_OP),
                   RAMP_FINISH );

    /**
     * Key Column, Record Column에 Aggregation이 존재하는 경우
     * 하위 노드에 Base Table이 존재하면 불가
     */

    if( aSortInstantNode->mKeyColList != NULL )
    {
        sRefExprItem = aSortInstantNode->mKeyColList->mHead;
        while( sRefExprItem != NULL )
        {
            if( qlncHasAggregationOnExpr( sRefExprItem->mExpr,
                                          aSortInstantNode->mNodeCommon.mNodeID,
                                          STL_FALSE ) == STL_TRUE )
            {
                /* 하위 노드가 Base Table인 경우 불가 */
                STL_TRY_THROW( aSortInstantNode->mChildNode->mNodeType != QLNC_NODE_TYPE_BASE_TABLE,
                               RAMP_FINISH );
                break;
            }

            sRefExprItem = sRefExprItem->mNext;
        }
    }

    if( aSortInstantNode->mRecColList != NULL )
    {
        sRefExprItem = aSortInstantNode->mRecColList->mHead;
        while( sRefExprItem != NULL )
        {
            if( qlncHasAggregationOnExpr( sRefExprItem->mExpr,
                                          aSortInstantNode->mNodeCommon.mNodeID,
                                          STL_FALSE ) == STL_TRUE )
            {
                /* 하위 노드가 Base Table인 경우 불가 */
                STL_TRY_THROW( aSortInstantNode->mChildNode->mNodeType != QLNC_NODE_TYPE_BASE_TABLE,
                               RAMP_FINISH );
                break;
            }

            sRefExprItem = sRefExprItem->mNext;
        }
    }


    /* Filter에서 SubTable의 Target Column에 해당하는 부분을
     * Query Spec의 Target Expr로 대체 */
    QLNC_INIT_AND_FILTER( &sPushFilter );

    /* Key Filter에 대한 처리 */
    if( aSortInstantNode->mKeyFilter != NULL )
    {
        /* @todo 일반적으로는 들어올 수 없으나 복잡한 질의에서는 들어올 가능성도 있음 */

        for( i = 0; i < aSortInstantNode->mKeyFilter->mOrCount; i++ )
        {
            /* Constant가 아닌 SubQuery를 가진 경우 Push 불가 */
            if( aSortInstantNode->mKeyFilter->mOrFilters[i]->mSubQueryExprList != NULL )
            {
                sRefExprItem = aSortInstantNode->mKeyFilter->mOrFilters[i]->mSubQueryExprList->mHead;
                while( sRefExprItem != NULL )
                {
                    if( (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL) &&
                        (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList->mCount > 0) )
                    {
                        break;
                    }

                    sRefExprItem = sRefExprItem->mNext;
                }

                if( sRefExprItem != NULL )
                {
                    continue;
                }
            }

            /* Aggregation이 존재하는 경우 */
            if( qlncHasAggregationOnExpr( (qlncExprCommon*)(aSortInstantNode->mKeyFilter->mOrFilters[i]),
                                          aSortInstantNode->mNodeCommon.mNodeID,
                                          STL_FALSE ) == STL_TRUE )
            {
                if( (aSortInstantNode->mInstantType == QLNC_INSTANT_TYPE_GROUP) ||
                    (aSortInstantNode->mInstantType == QLNC_INSTANT_TYPE_DISTINCT) )
                {
                    /* Group, Distinct인 경우 Push 불가 */
                    continue;
                }
                else
                {
                    /* 하위 노드가 Base Table인 경우 불가 */
                    if( aSortInstantNode->mChildNode->mNodeType
                        == QLNC_NODE_TYPE_BASE_TABLE )
                    {
                        continue;
                    }
                }
            }

            STL_TRY( qlncFPDChangeInnerColumnToOrgExpr(
                         aParamInfo,
                         (qlncNodeCommon*)aSortInstantNode,
                         (qlncExprCommon*)(aSortInstantNode->mKeyFilter->mOrFilters[i]),
                         (qlncExprCommon**)&(aSortInstantNode->mKeyFilter->mOrFilters[i]),
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        &sPushFilter,
                                                        1,
                                                        &(aSortInstantNode->mKeyFilter->mOrFilters[i]),
                                                        aEnv )
                     == STL_SUCCESS );

            aSortInstantNode->mKeyFilter->mOrFilters[i] = NULL;
        }

        /* Sort Instant Node의 Filter Compact */
        STL_TRY( qlncCompactAndFilter( aSortInstantNode->mKeyFilter,
                                       aEnv )
                 == STL_SUCCESS );

        if( aSortInstantNode->mKeyFilter->mOrCount == 0 )
        {
            aSortInstantNode->mKeyFilter = NULL;
        }
    }

    /* Filter에 대한 처리 */
    if( aSortInstantNode->mFilter != NULL )
    {
        for( i = 0; i < aSortInstantNode->mFilter->mOrCount; i++ )
        {
            /* Constant가 아닌 SubQuery를 가진 경우 Push 불가 */
            if( aSortInstantNode->mFilter->mOrFilters[i]->mSubQueryExprList != NULL )
            {
                /* @todo Subquery가 있는 경우 Push가 불가하기 때문에
                 * 여기에 올 수 있는 경우가 없을 것으로 판단됨 */

                sRefExprItem = aSortInstantNode->mFilter->mOrFilters[i]->mSubQueryExprList->mHead;
                while( sRefExprItem != NULL )
                {
                    if( (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL) &&
                        (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList->mCount > 0) )
                    {
                        break;
                    }

                    sRefExprItem = sRefExprItem->mNext;
                }

                if( sRefExprItem != NULL )
                {
                    continue;
                }
            }

            /* Aggregation이 존재하는 경우 */
            if( qlncHasAggregationOnExpr( (qlncExprCommon*)(aSortInstantNode->mFilter->mOrFilters[i]),
                                          aSortInstantNode->mNodeCommon.mNodeID,
                                          STL_FALSE ) == STL_TRUE )
            {
                if( (aSortInstantNode->mInstantType == QLNC_INSTANT_TYPE_GROUP) ||
                    (aSortInstantNode->mInstantType == QLNC_INSTANT_TYPE_DISTINCT) )
                {
                    /* Group, Distinct인 경우 Push 불가 */
                    /* @todo 현재 Sort Instant로 Group나 Distinct가 불가능 */
                    continue;
                }
                else
                {
                    /* 하위 노드가 Base Table인 경우 불가 */
                    if( aSortInstantNode->mChildNode->mNodeType
                        == QLNC_NODE_TYPE_BASE_TABLE )
                    {
                        continue;
                    }
                }
            }

            STL_TRY( qlncFPDChangeInnerColumnToOrgExpr(
                         aParamInfo,
                         (qlncNodeCommon*)aSortInstantNode,
                         (qlncExprCommon*)(aSortInstantNode->mFilter->mOrFilters[i]),
                         (qlncExprCommon**)&(aSortInstantNode->mFilter->mOrFilters[i]),
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        &sPushFilter,
                                                        1,
                                                        &(aSortInstantNode->mFilter->mOrFilters[i]),
                                                        aEnv )
                     == STL_SUCCESS );

            aSortInstantNode->mFilter->mOrFilters[i] = NULL;
        }

        /* Sort Instant Node의 Filter Compact */
        STL_TRY( qlncCompactAndFilter( aSortInstantNode->mFilter,
                                       aEnv )
                 == STL_SUCCESS );

        if( aSortInstantNode->mFilter->mOrCount == 0 )
        {
            aSortInstantNode->mFilter = NULL;
        }
    }

    /* Push Filter가 없으면 종료 */
    STL_TRY_THROW( sPushFilter.mOrCount > 0, RAMP_FINISH );

    /* Child Node의 Filter 찾기 */
    switch( aSortInstantNode->mChildNode->mNodeType )
    {
        case QLNC_NODE_TYPE_BASE_TABLE:
            sChildAndFilter =
                &(((qlncBaseTableNode*)(aSortInstantNode->mChildNode))->mFilter);
            break;
        case QLNC_NODE_TYPE_SUB_TABLE:
            sChildAndFilter =
                &(((qlncSubTableNode*)(aSortInstantNode->mChildNode))->mFilter);
            break;
        case QLNC_NODE_TYPE_JOIN_TABLE:
            sChildAndFilter =
                &(((qlncJoinTableNode*)(aSortInstantNode->mChildNode))->mFilter);
            break;
        case QLNC_NODE_TYPE_SORT_INSTANT:
            /* @todo 현재 Sort Instant의 Child Node로 Sort Instant가 올 수 없다. */
            sChildAndFilter =
                &(((qlncSortInstantNode*)(aSortInstantNode->mChildNode))->mFilter);
            break;
        case QLNC_NODE_TYPE_HASH_INSTANT:
            sChildAndFilter =
                &(((qlncHashInstantNode*)(aSortInstantNode->mChildNode))->mFilter);
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Child Node의 And Filter에 Push Filter 적용 */
    if( *sChildAndFilter == NULL )
    {
        STL_TRY( qlncCopyAndFilter( aParamInfo,
                                    &sPushFilter,
                                    sChildAndFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                    *sChildAndFilter,
                                                    sPushFilter.mOrCount,
                                                    sPushFilter.mOrFilters,
                                                    aEnv )
                 == STL_SUCCESS );
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Instant Node에 대한 Filter Push Down을 수행한다.
 * @param[in]       aParamInfo          Parameter Info
 * @param[in,out]   aHashInstantNode    Candidate Hash Instant Node
 * @param[in]       aEnv                Environment
 */
stlStatus qlncFPDHashInstantNode( qlncParamInfo         * aParamInfo,
                                  qlncHashInstantNode   * aHashInstantNode,
                                  qllEnv                * aEnv )
{
    stlInt32              i;
    qlncRefExprItem     * sRefExprItem      = NULL;
    qlncAndFilter         sPushFilter;
    qlncAndFilter      ** sChildAndFilter   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashInstantNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aHashInstantNode->mKeyFilter != NULL) ||
                        (aHashInstantNode->mRecFilter != NULL) ||
                        (aHashInstantNode->mFilter != NULL),
                        QLL_ERROR_STACK(aEnv) );


    /**
     * @todo Child Node가 Set Operator인 경우 Filter Push Down은 향후 진행
     */

    STL_TRY_THROW( (aHashInstantNode->mChildNode->mNodeType != QLNC_NODE_TYPE_QUERY_SET) &&
                   (aHashInstantNode->mChildNode->mNodeType != QLNC_NODE_TYPE_SET_OP),
                   RAMP_FINISH );

    /* Filter에서 SubTable의 Target Column에 해당하는 부분을
     * Query Spec의 Target Expr로 대체 */
    QLNC_INIT_AND_FILTER( &sPushFilter );

    /* Key Filter에 대한 처리 */
    if( aHashInstantNode->mKeyFilter != NULL )
    {
        /* @todo 일반적으로는 들어올 수 없으나 복잡한 질의에서는 들어올 가능성도 있음 */

        for( i = 0; i < aHashInstantNode->mKeyFilter->mOrCount; i++ )
        {
            /* Constant가 아닌 SubQuery를 가진 경우 Push 불가 */
            if( aHashInstantNode->mKeyFilter->mOrFilters[i]->mSubQueryExprList != NULL )
            {
                sRefExprItem = aHashInstantNode->mKeyFilter->mOrFilters[i]->mSubQueryExprList->mHead;
                while( sRefExprItem != NULL )
                {
                    if( (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL) &&
                        (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList->mCount > 0) )
                    {
                        break;
                    }

                    sRefExprItem = sRefExprItem->mNext;
                }

                if( sRefExprItem != NULL )
                {
                    continue;
                }
            }

            /* Node가 Group Type이고 Aggregation이 존재하면 Push 불가 */
            if( ( (aHashInstantNode->mInstantType == QLNC_INSTANT_TYPE_GROUP) ||
                  (aHashInstantNode->mInstantType == QLNC_INSTANT_TYPE_DISTINCT) ) &&
                ( qlncHasAggregationOnExpr( (qlncExprCommon*)(aHashInstantNode->mKeyFilter->mOrFilters[i]),
                                            aHashInstantNode->mNodeCommon.mNodeID,
                                            STL_FALSE ) == STL_TRUE ) )
            {
                continue;
            }

            STL_TRY( qlncFPDChangeInnerColumnToOrgExpr(
                         aParamInfo,
                         (qlncNodeCommon*)aHashInstantNode,
                         (qlncExprCommon*)(aHashInstantNode->mKeyFilter->mOrFilters[i]),
                         (qlncExprCommon**)&(aHashInstantNode->mKeyFilter->mOrFilters[i]),
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        &sPushFilter,
                                                        1,
                                                        &(aHashInstantNode->mKeyFilter->mOrFilters[i]),
                                                        aEnv )
                     == STL_SUCCESS );

            aHashInstantNode->mKeyFilter->mOrFilters[i] = NULL;
        }

        /* Hash Instant Node의 Filter Compact */
        STL_TRY( qlncCompactAndFilter( aHashInstantNode->mKeyFilter,
                                       aEnv )
                 == STL_SUCCESS );

        if( aHashInstantNode->mKeyFilter->mOrCount == 0 )
        {
            aHashInstantNode->mKeyFilter = NULL;
        }
    }

    /* Rec Filter에 대한 처리 */
    if( aHashInstantNode->mRecFilter != NULL )
    {
        for( i = 0; i < aHashInstantNode->mRecFilter->mOrCount; i++ )
        {
            /* Constant가 아닌 SubQuery를 가진 경우 Push 불가 */
            if( aHashInstantNode->mRecFilter->mOrFilters[i]->mSubQueryExprList != NULL )
            {
                sRefExprItem = aHashInstantNode->mRecFilter->mOrFilters[i]->mSubQueryExprList->mHead;
                while( sRefExprItem != NULL )
                {
                    if( (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL) &&
                        (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList->mCount > 0) )
                    {
                        break;
                    }

                    sRefExprItem = sRefExprItem->mNext;
                }

                if( sRefExprItem != NULL )
                {
                    continue;
                }
            }

            /* Node가 Group Type이고 Aggregation이 존재하면 Push 불가 */
            if( ( (aHashInstantNode->mInstantType == QLNC_INSTANT_TYPE_GROUP) ||
                  (aHashInstantNode->mInstantType == QLNC_INSTANT_TYPE_DISTINCT) ) &&
                ( qlncHasAggregationOnExpr( (qlncExprCommon*)(aHashInstantNode->mRecFilter->mOrFilters[i]),
                                            aHashInstantNode->mNodeCommon.mNodeID,
                                            STL_FALSE ) == STL_TRUE ) )
            {
                continue;
            }

            STL_TRY( qlncFPDChangeInnerColumnToOrgExpr(
                         aParamInfo,
                         (qlncNodeCommon*)aHashInstantNode,
                         (qlncExprCommon*)(aHashInstantNode->mRecFilter->mOrFilters[i]),
                         (qlncExprCommon**)&(aHashInstantNode->mRecFilter->mOrFilters[i]),
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        &sPushFilter,
                                                        1,
                                                        &(aHashInstantNode->mRecFilter->mOrFilters[i]),
                                                        aEnv )
                     == STL_SUCCESS );

            aHashInstantNode->mRecFilter->mOrFilters[i] = NULL;
        }

        /* Hash Instant Node의 Filter Compact */
        STL_TRY( qlncCompactAndFilter( aHashInstantNode->mRecFilter,
                                       aEnv )
                 == STL_SUCCESS );

        if( aHashInstantNode->mRecFilter->mOrCount == 0 )
        {
            aHashInstantNode->mRecFilter = NULL;
        }
    }

    /* Filter에 대한 처리 */
    if( aHashInstantNode->mFilter != NULL )
    {
        for( i = 0; i < aHashInstantNode->mFilter->mOrCount; i++ )
        {
            /* Constant가 아닌 SubQuery를 가진 경우 Push 불가 */
            if( aHashInstantNode->mFilter->mOrFilters[i]->mSubQueryExprList != NULL )
            {
                /* @todo Subquery가 있는 경우 Push가 불가하기 때문에
                 * 여기에 올 수 있는 경우가 없을 것으로 판단됨 */

                sRefExprItem = aHashInstantNode->mFilter->mOrFilters[i]->mSubQueryExprList->mHead;
                while( sRefExprItem != NULL )
                {
                    if( (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL) &&
                        (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList->mCount > 0) )
                    {
                        break;
                    }

                    sRefExprItem = sRefExprItem->mNext;
                }

                if( sRefExprItem != NULL )
                {
                    continue;
                }
            }

            /* Node가 Group Type이고 Aggregation이 존재하면 Push 불가 */
            if( ( (aHashInstantNode->mInstantType == QLNC_INSTANT_TYPE_GROUP) ||
                  (aHashInstantNode->mInstantType == QLNC_INSTANT_TYPE_DISTINCT) ) &&
                ( qlncHasAggregationOnExpr( (qlncExprCommon*)(aHashInstantNode->mFilter->mOrFilters[i]),
                                            aHashInstantNode->mNodeCommon.mNodeID,
                                            STL_FALSE ) == STL_TRUE ) )
            {
                continue;
            }

            STL_TRY( qlncFPDChangeInnerColumnToOrgExpr(
                         aParamInfo,
                         (qlncNodeCommon*)aHashInstantNode,
                         (qlncExprCommon*)(aHashInstantNode->mFilter->mOrFilters[i]),
                         (qlncExprCommon**)&(aHashInstantNode->mFilter->mOrFilters[i]),
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        &sPushFilter,
                                                        1,
                                                        &(aHashInstantNode->mFilter->mOrFilters[i]),
                                                        aEnv )
                     == STL_SUCCESS );

            aHashInstantNode->mFilter->mOrFilters[i] = NULL;
        }

        /* Hash Instant Node의 Filter Compact */
        STL_TRY( qlncCompactAndFilter( aHashInstantNode->mFilter,
                                       aEnv )
                 == STL_SUCCESS );

        if( aHashInstantNode->mFilter->mOrCount == 0 )
        {
            aHashInstantNode->mFilter = NULL;
        }
    }

    /* Push Filter가 없으면 종료 */
    STL_TRY_THROW( sPushFilter.mOrCount > 0, RAMP_FINISH );

    /* Child Node의 Filter 찾기 */
    switch( aHashInstantNode->mChildNode->mNodeType )
    {
        case QLNC_NODE_TYPE_BASE_TABLE:
            sChildAndFilter =
                &(((qlncBaseTableNode*)(aHashInstantNode->mChildNode))->mFilter);
            break;
        case QLNC_NODE_TYPE_SUB_TABLE:
            sChildAndFilter =
                &(((qlncSubTableNode*)(aHashInstantNode->mChildNode))->mFilter);
            break;
        case QLNC_NODE_TYPE_JOIN_TABLE:
            sChildAndFilter =
                &(((qlncJoinTableNode*)(aHashInstantNode->mChildNode))->mFilter);
            break;
        case QLNC_NODE_TYPE_SORT_INSTANT:
            /* @todo 현재 Hash Instant의 Child Node로 현재 Sort Instant가 올 수 없다. */
            sChildAndFilter =
                &(((qlncHashInstantNode*)(aHashInstantNode->mChildNode))->mFilter);
            break;
        case QLNC_NODE_TYPE_HASH_INSTANT:
            sChildAndFilter =
                &(((qlncHashInstantNode*)(aHashInstantNode->mChildNode))->mFilter);
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Child Node의 And Filter에 Push Filter 적용 */
    if( *sChildAndFilter == NULL )
    {
        STL_TRY( qlncCopyAndFilter( aParamInfo,
                                    &sPushFilter,
                                    sChildAndFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                    *sChildAndFilter,
                                                    sPushFilter.mOrCount,
                                                    sPushFilter.mOrFilters,
                                                    aEnv )
                 == STL_SUCCESS );
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubTable에 대한 Inner Column을 하위노드의 Target Expr로 변경한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aNodeID         SubTable Node ID
 * @param[in]   aTargetCount    SubTable Child Node의 Target Count
 * @param[in]   aTargetArr      SubTable Child Node의 Target Array
 * @param[in]   aExpr           Expression
 * @param[out]  aOutExpr        Output Expression
 * @param[in]   aEnv            Environment
 */
stlStatus qlncFPDChangeInnerColumnToTargetExpr( qlncParamInfo   * aParamInfo,
                                                stlInt32          aNodeID,
                                                stlInt32          aTargetCount,
                                                qlncTarget      * aTargetArr,
                                                qlncExprCommon  * aExpr,
                                                qlncExprCommon ** aOutExpr,
                                                qllEnv          * aEnv )
{
    qlncExprCommon  * sOutExpr  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetArr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
        case QLNC_EXPR_TYPE_BIND_PARAM:
        case QLNC_EXPR_TYPE_COLUMN:
            {
                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_FUNCTION:
            {
                qlncExprFunction    * sFunction = NULL;
                stlInt32              i;

                sFunction = (qlncExprFunction*)aExpr;
                for( i = 0; i < sFunction->mArgCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToTargetExpr( aParamInfo,
                                                                   aNodeID,
                                                                   aTargetCount,
                                                                   aTargetArr,
                                                                   sFunction->mArgs[i],
                                                                   &(sFunction->mArgs[i]),
                                                                   aEnv )
                             == STL_SUCCESS );
                }

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_CAST:
            {
                qlncExprTypeCast    * sTypeCast = NULL;
                stlInt32              i;

                sTypeCast = (qlncExprTypeCast*)aExpr;
                for( i = 0; i < sTypeCast->mArgCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToTargetExpr( aParamInfo,
                                                                   aNodeID,
                                                                   aTargetCount,
                                                                   aTargetArr,
                                                                   sTypeCast->mArgs[i],
                                                                   &(sTypeCast->mArgs[i]),
                                                                   aEnv )
                             == STL_SUCCESS );
                }

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
        case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
        case QLNC_EXPR_TYPE_SUB_QUERY:
        case QLNC_EXPR_TYPE_REFERENCE:
            {
                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_INNER_COLUMN:
            {
                qlncExprInnerColumn * sInnerColumn  = NULL;
                qlncExprCommon      * sTmpExpr      = NULL;
                stlInt32              sTmpNodeID;

                sInnerColumn = (qlncExprInnerColumn*)aExpr;
                if( sInnerColumn->mNode->mNodeID == aNodeID )
                {
                    STL_DASSERT( sInnerColumn->mIdx < aTargetCount );

                    sTmpExpr = aTargetArr[sInnerColumn->mIdx].mExpr;
                    if( sTmpExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN )
                    {
                        sTmpNodeID = ((qlncExprInnerColumn*)sTmpExpr)->mNode->mNodeID;
                        while( (((qlncExprInnerColumn*)sTmpExpr)->mOrgExpr != NULL) &&
                               (((qlncExprInnerColumn*)sTmpExpr)->mOrgExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN) )
                        {
                            if( (((qlncExprInnerColumn*)(((qlncExprInnerColumn*)sTmpExpr)->mOrgExpr))->mNode->mNodeID == sTmpNodeID) ||
                                (((qlncExprInnerColumn*)(((qlncExprInnerColumn*)sTmpExpr)->mOrgExpr))->mNode->mNodeID == aNodeID) )
                            {
                                sTmpExpr = ((qlncExprInnerColumn*)sTmpExpr)->mOrgExpr;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }

                    STL_TRY( qlncDuplicateExpr( aParamInfo,
                                                sTmpExpr,
                                                &sOutExpr,
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    if( sInnerColumn->mOrgExpr != NULL )
                    {
                        /* @todo 현재 여기로 들어올 수 있는 질의가 없는 것으로 판단됨 */
                        STL_TRY( qlncFPDChangeInnerColumnToTargetExpr( aParamInfo,
                                                                       aNodeID,
                                                                       aTargetCount,
                                                                       aTargetArr,
                                                                       sInnerColumn->mOrgExpr,
                                                                       &(sInnerColumn->mOrgExpr),
                                                                       aEnv )
                                 == STL_SUCCESS );
                    }

                    sOutExpr = aExpr;
                }

                break;
            }
        case QLNC_EXPR_TYPE_OUTER_COLUMN:
            {
                STL_DASSERT( 0 );
                sOutExpr = aExpr;
                break;
            }
        case QLNC_EXPR_TYPE_ROWID_COLUMN:
            {
                sOutExpr = aExpr;
                break;
            }
        case QLNC_EXPR_TYPE_AGGREGATION:
            {
                qlncExprAggregation * sAggregation  = NULL;
                stlInt32              i;

                /* @todo Aggregation이 존재하는 경우 Push가 불가능한 것으로 판단됨 */
                sAggregation = (qlncExprAggregation*)aExpr;
                for( i = 0; i < sAggregation->mArgCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToTargetExpr( aParamInfo,
                                                                   aNodeID,
                                                                   aTargetCount,
                                                                   aTargetArr,
                                                                   sAggregation->mArgs[i],
                                                                   &(sAggregation->mArgs[i]),
                                                                   aEnv )
                             == STL_SUCCESS );
                }

                if( sAggregation->mFilter != NULL )
                {
                    /* @todo Aggregation에 Filter는 향후 지원한다. */
                    STL_TRY( qlncFPDChangeInnerColumnToTargetExpr( aParamInfo,
                                                                   aNodeID,
                                                                   aTargetCount,
                                                                   aTargetArr,
                                                                   sAggregation->mFilter,
                                                                   &(sAggregation->mFilter),
                                                                   aEnv )
                             == STL_SUCCESS );
                }

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_CASE_EXPR:
            {
                qlncExprCaseExpr    * sCaseExpr = NULL;
                stlInt32              i;

                sCaseExpr = (qlncExprCaseExpr*)aExpr;
                for( i = 0; i < sCaseExpr->mCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToTargetExpr( aParamInfo,
                                                                   aNodeID,
                                                                   aTargetCount,
                                                                   aTargetArr,
                                                                   sCaseExpr->mWhenArr[i],
                                                                   &(sCaseExpr->mWhenArr[i]),
                                                                   aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlncFPDChangeInnerColumnToTargetExpr( aParamInfo,
                                                                   aNodeID,
                                                                   aTargetCount,
                                                                   aTargetArr,
                                                                   sCaseExpr->mThenArr[i],
                                                                   &(sCaseExpr->mThenArr[i]),
                                                                   aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncFPDChangeInnerColumnToTargetExpr( aParamInfo,
                                                               aNodeID,
                                                               aTargetCount,
                                                               aTargetArr,
                                                               sCaseExpr->mDefResult,
                                                               &(sCaseExpr->mDefResult),
                                                               aEnv )
                         == STL_SUCCESS );

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_LIST_FUNCTION:
            {
                qlncExprListFunc    * sListFunc = NULL;
                stlInt32              i;

                sListFunc = (qlncExprListFunc*)aExpr;
                for( i = 0; i < sListFunc->mArgCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToTargetExpr( aParamInfo,
                                                                   aNodeID,
                                                                   aTargetCount,
                                                                   aTargetArr,
                                                                   sListFunc->mArgs[i],
                                                                   &(sListFunc->mArgs[i]),
                                                                   aEnv )
                             == STL_SUCCESS );
                }

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_LIST_COLUMN:
            {
                qlncExprListCol     * sListCol  = NULL;
                stlInt32              i;

                sListCol = (qlncExprListCol*)aExpr;
                for( i = 0; i < sListCol->mArgCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToTargetExpr( aParamInfo,
                                                                   aNodeID,
                                                                   aTargetCount,
                                                                   aTargetArr,
                                                                   sListCol->mArgs[i],
                                                                   &(sListCol->mArgs[i]),
                                                                   aEnv )
                             == STL_SUCCESS );
                }

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_ROW_EXPR:
            {
                qlncExprRowExpr     * sRowExpr  = NULL;
                stlInt32              i;

                sRowExpr = (qlncExprRowExpr*)aExpr;
                for( i = 0; i < sRowExpr->mArgCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToTargetExpr( aParamInfo,
                                                                   aNodeID,
                                                                   aTargetCount,
                                                                   aTargetArr,
                                                                   sRowExpr->mArgs[i],
                                                                   &(sRowExpr->mArgs[i]),
                                                                   aEnv )
                             == STL_SUCCESS );
                }

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_AND_FILTER:
            {
                qlncAndFilter   * sAndFilter    = NULL;
                stlInt32          i;

                sAndFilter = (qlncAndFilter*)aExpr;
                for( i = 0; i < sAndFilter->mOrCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToTargetExpr( aParamInfo,
                                                                   aNodeID,
                                                                   aTargetCount,
                                                                   aTargetArr,
                                                                   (qlncExprCommon*)(sAndFilter->mOrFilters[i]),
                                                                   (qlncExprCommon**)&(sAndFilter->mOrFilters[i]),
                                                                   aEnv )
                             == STL_SUCCESS );
                }

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_OR_FILTER:
            {
                qlncOrFilter    * sOrFilter = NULL;
                qlncRefExprList   sTmpSubQueryExprList;
                qlncRefExprItem * sRefExprItem      = NULL;
                qlncRefExprItem * sNewRefExprItem   = NULL;
                stlInt32          i;

                sOrFilter = (qlncOrFilter*)aExpr;
                QLNC_INIT_LIST( &sTmpSubQueryExprList );
                for( i = 0; i < sOrFilter->mFilterCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToTargetExpr( aParamInfo,
                                                                   aNodeID,
                                                                   aTargetCount,
                                                                   aTargetArr,
                                                                   sOrFilter->mFilters[i],
                                                                   &(sOrFilter->mFilters[i]),
                                                                   aEnv )
                             == STL_SUCCESS );

                    sRefExprItem = NULL;
                    if( sOrFilter->mFilters[i]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER )
                    {
                        if( ((qlncBooleanFilter*)(sOrFilter->mFilters[i]))->mSubQueryExprList != NULL )
                        {
                            sRefExprItem = ((qlncBooleanFilter*)(sOrFilter->mFilters[i]))->mSubQueryExprList->mHead;
                        }
                    }
                    else
                    {
                        STL_DASSERT( sOrFilter->mFilters[i]->mType == QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER );
                        if( ((qlncConstBooleanFilter*)(sOrFilter->mFilters[i]))->mSubQueryExprList != NULL )
                        {
                            sRefExprItem = ((qlncConstBooleanFilter*)(sOrFilter->mFilters[i]))->mSubQueryExprList->mHead;
                        }

                    }

                    while( sRefExprItem != NULL )
                    {
                        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                    STL_SIZEOF( qlncRefExprItem ),
                                                    (void**) &sNewRefExprItem,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        sNewRefExprItem->mExpr = sRefExprItem->mExpr;
                        sNewRefExprItem->mNext = NULL;

                        QLNC_APPEND_ITEM_TO_LIST( &sTmpSubQueryExprList, sNewRefExprItem );

                        sRefExprItem = sRefExprItem->mNext;
                    }
                }

                if( sTmpSubQueryExprList.mCount > 0 )
                {
                        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                    STL_SIZEOF( qlncRefExprList ),
                                                    (void**) &(sOrFilter->mSubQueryExprList),
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                    QLNC_COPY_LIST_INFO( &sTmpSubQueryExprList,
                                         sOrFilter->mSubQueryExprList );
                }

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
            {
                qlncBooleanFilter       * sBooleanFilter        = NULL;
                qlncConstBooleanFilter  * sConstBooleanFilter   = NULL;

                qlncConvertExprParamInfo      sConvertExprParamInfo;

                qlncRefNodeItem             * sRefNodeItem      = NULL;
                qlncRefColumnItem           * sRefColumnItem    = NULL;
                qlncRefExprList           sTmpSubQueryExprList;

                sBooleanFilter = (qlncBooleanFilter*)aExpr;
                STL_TRY( qlncFPDChangeInnerColumnToTargetExpr( aParamInfo,
                                                               aNodeID,
                                                               aTargetCount,
                                                               aTargetArr,
                                                               sBooleanFilter->mExpr,
                                                               &(sBooleanFilter->mExpr),
                                                               aEnv )
                         == STL_SUCCESS );

                /* Node List 초기화 */
                QLNC_INIT_LIST( &(sBooleanFilter->mRefNodeList) );
                sBooleanFilter->mLeftRefNodeList = NULL;
                sBooleanFilter->mRightRefNodeList = NULL;

                /* Convert Expr Param Info 초기화 */
                sConvertExprParamInfo.mParamInfo = *aParamInfo;
                sConvertExprParamInfo.mTableMapArr = NULL;
                sConvertExprParamInfo.mOuterTableMapArr = NULL;
                sConvertExprParamInfo.mInitExpr = NULL;
                sConvertExprParamInfo.mCandExpr = NULL;
                sConvertExprParamInfo.mExprPhraseFlag = 0;
                sConvertExprParamInfo.mSubQueryExprList = NULL;
                sConvertExprParamInfo.mExprSubQuery = NULL;
                sConvertExprParamInfo.mRefNodeList = NULL;

                if( (sBooleanFilter->mExpr->mType == QLNC_EXPR_TYPE_FUNCTION) &&
                    (sBooleanFilter->mLeftRefNodeList != NULL) &&
                    (sBooleanFilter->mRightRefNodeList != NULL) )
                {
                    /* @todo Left와 Right의 RefNodeList가 존재하는 경우가 없는 것으로 판단됨 */

                    STL_DASSERT( ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgCount == 2 );

                    /* Left Expression에 있는 Reference Node List 설정 */
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncRefNodeList ),
                                                (void**) &(sBooleanFilter->mLeftRefNodeList),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    QLNC_INIT_LIST( sBooleanFilter->mLeftRefNodeList );

                    sConvertExprParamInfo.mRefNodeList = sBooleanFilter->mLeftRefNodeList;
                    STL_TRY( qlncFindAddRefColumnToRefNodeList(
                                 &sConvertExprParamInfo,
                                 ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0],
                                 aEnv )
                             == STL_SUCCESS );

                    /* Right Expression에 있는 Reference Node List 설정 */
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncRefNodeList ),
                                                (void**) &(sBooleanFilter->mRightRefNodeList),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    QLNC_INIT_LIST( sBooleanFilter->mRightRefNodeList );

                    sConvertExprParamInfo.mRefNodeList = sBooleanFilter->mRightRefNodeList;
                    STL_TRY( qlncFindAddRefColumnToRefNodeList(
                                 &sConvertExprParamInfo,
                                 ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1],
                                 aEnv )
                             == STL_SUCCESS );

                    /* Append Left Reference Node List */
                    sConvertExprParamInfo.mRefNodeList = &(sBooleanFilter->mRefNodeList);
                    sRefNodeItem = sBooleanFilter->mLeftRefNodeList->mHead;
                    while( sRefNodeItem != NULL )
                    {
                        sRefColumnItem = sRefNodeItem->mRefColumnList.mHead;
                        while( sRefColumnItem != NULL )
                        {
                            STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                                    sRefColumnItem->mExpr,
                                                                    aEnv )
                                     == STL_SUCCESS );

                            sRefColumnItem = sRefColumnItem->mNext;
                        }

                        sRefNodeItem = sRefNodeItem->mNext;
                    }

                    /* Append Right Reference Node List */
                    sConvertExprParamInfo.mRefNodeList = &(sBooleanFilter->mRefNodeList);
                    sRefNodeItem = sBooleanFilter->mRightRefNodeList->mHead;
                    while( sRefNodeItem != NULL )
                    {
                        sRefColumnItem = sRefNodeItem->mRefColumnList.mHead;
                        while( sRefColumnItem != NULL )
                        {
                            STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                                    sRefColumnItem->mExpr,
                                                                    aEnv )
                                     == STL_SUCCESS );

                            sRefColumnItem = sRefColumnItem->mNext;
                        }

                        sRefNodeItem = sRefNodeItem->mNext;
                    }
                }
                else
                {
                    STL_DASSERT( (sBooleanFilter->mLeftRefNodeList == NULL) &&
                                 (sBooleanFilter->mRightRefNodeList == NULL) );

                    sConvertExprParamInfo.mRefNodeList = &(sBooleanFilter->mRefNodeList);
                    STL_TRY( qlncFindAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                                sBooleanFilter->mExpr,
                                                                aEnv )
                             == STL_SUCCESS );
                }

                QLNC_INIT_LIST( &sTmpSubQueryExprList );
                STL_TRY( qlncFindAddSubQueryExprToSubQueryExprList( aParamInfo,
                                                                    sBooleanFilter->mExpr,
                                                                    &sTmpSubQueryExprList,
                                                                    aEnv )
                         == STL_SUCCESS );

                if( sTmpSubQueryExprList.mCount > 0 )
                {
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncRefExprList ),
                                                (void**)&(sBooleanFilter->mSubQueryExprList),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    QLNC_COPY_LIST_INFO( &sTmpSubQueryExprList,
                                         sBooleanFilter->mSubQueryExprList );
                }

                /* 참조 컬럼이 없는 경우 Constant Boolean Filter로 변경한다. */
                if( sBooleanFilter->mRefNodeList.mCount == 0 )
                {
                   STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncConstBooleanFilter ),
                                                (void**)&sConstBooleanFilter,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    QLNC_SET_EXPR_COMMON( &sConstBooleanFilter->mCommon,
                                          &QLL_CANDIDATE_MEM( aEnv ),
                                          aEnv,
                                          QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER,
                                          sBooleanFilter->mCommon.mExprPhraseFlag,
                                          sBooleanFilter->mCommon.mIterationTime,
                                          sBooleanFilter->mCommon.mPosition,
                                          sBooleanFilter->mCommon.mColumnName,
                                          DTL_TYPE_BOOLEAN );

                    sConstBooleanFilter->mExpr = sBooleanFilter->mExpr;
                    sConstBooleanFilter->mSubQueryExprList = sBooleanFilter->mSubQueryExprList;

                    sOutExpr = (qlncExprCommon*)sConstBooleanFilter;
                }
                else
                {
                    sOutExpr = aExpr;
                }

                break;
            }
        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
            {
                sOutExpr = aExpr;
                break;
            }
        default:
            {
                sOutExpr = aExpr;
                STL_DASSERT( 0 );
                break;
            }
    }


    /* Output 설정 */
    *aOutExpr = sOutExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Instant Node에 대한 Inner Column을 Org Expr로 변경한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aNode           Candidate Node
 * @param[in]   aExpr           Expression
 * @param[out]  aOutExpr        Output Expression
 * @param[in]   aEnv            Environment
 */
stlStatus qlncFPDChangeInnerColumnToOrgExpr( qlncParamInfo      * aParamInfo,
                                             qlncNodeCommon     * aNode,
                                             qlncExprCommon     * aExpr,
                                             qlncExprCommon    ** aOutExpr,
                                             qllEnv             * aEnv )
{
    qlncExprCommon  * sOutExpr  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
        case QLNC_EXPR_TYPE_BIND_PARAM:
        case QLNC_EXPR_TYPE_COLUMN:
            {
                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_FUNCTION:
            {
                qlncExprFunction    * sFunction = NULL;
                stlInt32              i;

                sFunction = (qlncExprFunction*)aExpr;
                for( i = 0; i < sFunction->mArgCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToOrgExpr( aParamInfo,
                                                                aNode,
                                                                sFunction->mArgs[i],
                                                                &(sFunction->mArgs[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_CAST:
            {
                qlncExprTypeCast    * sTypeCast = NULL;
                stlInt32              i;

                sTypeCast = (qlncExprTypeCast*)aExpr;
                for( i = 0; i < sTypeCast->mArgCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToOrgExpr( aParamInfo,
                                                                aNode,
                                                                sTypeCast->mArgs[i],
                                                                &(sTypeCast->mArgs[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
        case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
        case QLNC_EXPR_TYPE_SUB_QUERY:
        case QLNC_EXPR_TYPE_REFERENCE:
            {
                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_INNER_COLUMN:
            {
                qlncExprInnerColumn * sInnerColumn  = NULL;
                qlncExprCommon      * sTmpExpr      = NULL;
                stlInt32              sTmpNodeID;

                sInnerColumn = (qlncExprInnerColumn*)aExpr;
                if( sInnerColumn->mNode->mNodeID == aNode->mNodeID )
                {
                    STL_DASSERT( sInnerColumn->mOrgExpr != NULL );

                    /* 현재 노드와 동일한 Inner Column을 제거 */
                    while( sInnerColumn->mOrgExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN )
                    {
                        if( ((qlncExprInnerColumn*)(sInnerColumn->mOrgExpr))->mNode->mNodeID == aNode->mNodeID )
                        {
                            sInnerColumn = (qlncExprInnerColumn*)(sInnerColumn->mOrgExpr);
                        }
                        else
                        {
                            break;
                        }
                    }

                    /* 하위 노드에 대하여 Inner Column인 경우 동일한 Node의 중첩 Inner Column 제거 */
                    sTmpExpr = sInnerColumn->mOrgExpr;
                    if( sTmpExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN )
                    {
                        sTmpNodeID = ((qlncExprInnerColumn*)sTmpExpr)->mNode->mNodeID;
                        while( (((qlncExprInnerColumn*)sTmpExpr)->mOrgExpr != NULL) &&
                               (((qlncExprInnerColumn*)sTmpExpr)->mOrgExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN) )
                        {
                            if( ((qlncExprInnerColumn*)(((qlncExprInnerColumn*)sTmpExpr)->mOrgExpr))->mNode->mNodeID == sTmpNodeID )
                            {
                                sTmpExpr = ((qlncExprInnerColumn*)sTmpExpr)->mOrgExpr;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }

                    STL_TRY( qlncDuplicateExpr( aParamInfo,
                                                sTmpExpr,
                                                &sOutExpr,
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_DASSERT( qlncIsExistNode( aNode, sInnerColumn->mNode ) == STL_FALSE );
                    sOutExpr = aExpr;
                }

                break;
            }
        case QLNC_EXPR_TYPE_OUTER_COLUMN:
            {
                STL_DASSERT( 0 );
                sOutExpr = aExpr;
                break;
            }
        case QLNC_EXPR_TYPE_ROWID_COLUMN:
            {
                sOutExpr = aExpr;
                break;
            }
        case QLNC_EXPR_TYPE_AGGREGATION:
            {
                qlncExprAggregation * sAggregation  = NULL;
                stlInt32              i;

                /* @todo Aggregation에 대해서 들어오는 경우가 없는 것으로 판단됨 */

                sAggregation = (qlncExprAggregation*)aExpr;
                for( i = 0; i < sAggregation->mArgCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToOrgExpr( aParamInfo,
                                                                aNode,
                                                                sAggregation->mArgs[i],
                                                                &(sAggregation->mArgs[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncFPDChangeInnerColumnToOrgExpr( aParamInfo,
                                                            aNode,
                                                            sAggregation->mFilter,
                                                            &(sAggregation->mFilter),
                                                            aEnv )
                         == STL_SUCCESS );

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_CASE_EXPR:
            {
                qlncExprCaseExpr    * sCaseExpr = NULL;
                stlInt32              i;

                sCaseExpr = (qlncExprCaseExpr*)aExpr;
                for( i = 0; i < sCaseExpr->mCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToOrgExpr( aParamInfo,
                                                                aNode,
                                                                sCaseExpr->mWhenArr[i],
                                                                &(sCaseExpr->mWhenArr[i]),
                                                                aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlncFPDChangeInnerColumnToOrgExpr( aParamInfo,
                                                                aNode,
                                                                sCaseExpr->mThenArr[i],
                                                                &(sCaseExpr->mThenArr[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncFPDChangeInnerColumnToOrgExpr( aParamInfo,
                                                            aNode,
                                                            sCaseExpr->mDefResult,
                                                            &(sCaseExpr->mDefResult),
                                                            aEnv )
                         == STL_SUCCESS );

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_LIST_FUNCTION:
            {
                qlncExprListFunc    * sListFunc = NULL;
                stlInt32              i;

                sListFunc = (qlncExprListFunc*)aExpr;
                for( i = 0; i < sListFunc->mArgCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToOrgExpr( aParamInfo,
                                                                aNode,
                                                                sListFunc->mArgs[i],
                                                                &(sListFunc->mArgs[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_LIST_COLUMN:
            {
                qlncExprListCol     * sListCol  = NULL;
                stlInt32              i;

                sListCol = (qlncExprListCol*)aExpr;
                for( i = 0; i < sListCol->mArgCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToOrgExpr( aParamInfo,
                                                                aNode,
                                                                sListCol->mArgs[i],
                                                                &(sListCol->mArgs[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_ROW_EXPR:
            {
                qlncExprRowExpr     * sRowExpr  = NULL;
                stlInt32              i;

                sRowExpr = (qlncExprRowExpr*)aExpr;
                for( i = 0; i < sRowExpr->mArgCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToOrgExpr( aParamInfo,
                                                                aNode,
                                                                sRowExpr->mArgs[i],
                                                                &(sRowExpr->mArgs[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_AND_FILTER:
            {
                qlncAndFilter   * sAndFilter    = NULL;
                stlInt32          i;

                sAndFilter = (qlncAndFilter*)aExpr;
                for( i = 0; i < sAndFilter->mOrCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToOrgExpr( aParamInfo,
                                                                aNode,
                                                                (qlncExprCommon*)(sAndFilter->mOrFilters[i]),
                                                                (qlncExprCommon**)&(sAndFilter->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_OR_FILTER:
            {
                qlncOrFilter    * sOrFilter = NULL;
                stlInt32          i;

                sOrFilter = (qlncOrFilter*)aExpr;
                for( i = 0; i < sOrFilter->mFilterCount; i++ )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToOrgExpr( aParamInfo,
                                                                aNode,
                                                                sOrFilter->mFilters[i],
                                                                &(sOrFilter->mFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }

                sOutExpr = aExpr;

                break;
            }
        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
            {
                qlncBooleanFilter       * sBooleanFilter        = NULL;
                qlncConstBooleanFilter  * sConstBooleanFilter   = NULL;

                qlncConvertExprParamInfo      sConvertExprParamInfo;

                sBooleanFilter = (qlncBooleanFilter*)aExpr;
                STL_TRY( qlncFPDChangeInnerColumnToOrgExpr( aParamInfo,
                                                            aNode,
                                                            sBooleanFilter->mExpr,
                                                            &(sBooleanFilter->mExpr),
                                                            aEnv )
                         == STL_SUCCESS );

                STL_DASSERT( (sBooleanFilter->mLeftRefNodeList == NULL) &&
                             (sBooleanFilter->mRightRefNodeList == NULL) );

                /* Node List 초기화 */
                QLNC_INIT_LIST( &(sBooleanFilter->mRefNodeList) );

                /* 현재 Expression에 대하여 Reference Column List를 찾아 등록 */
                sConvertExprParamInfo.mParamInfo = *aParamInfo;
                sConvertExprParamInfo.mTableMapArr = NULL;
                sConvertExprParamInfo.mOuterTableMapArr = NULL;
                sConvertExprParamInfo.mInitExpr = NULL;
                sConvertExprParamInfo.mCandExpr = NULL;
                sConvertExprParamInfo.mExprPhraseFlag = 0;
                sConvertExprParamInfo.mSubQueryExprList = NULL;
                sConvertExprParamInfo.mExprSubQuery = NULL;
                sConvertExprParamInfo.mRefNodeList = &(sBooleanFilter->mRefNodeList);

                STL_TRY( qlncFindAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                            sBooleanFilter->mExpr,
                                                            aEnv )
                         == STL_SUCCESS );

                /* 참조 컬럼이 없는 경우 Constant Boolean Filter로 변경한다. */
                if( sBooleanFilter->mRefNodeList.mCount == 0 )
                {
                    /* @todo Reference Node List가 없는 Boolean Filter가
                     * 들어오는 경우가 없을 것으로 판단됨 */

                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncConstBooleanFilter ),
                                                (void**)&sConstBooleanFilter,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    QLNC_SET_EXPR_COMMON( &sConstBooleanFilter->mCommon,
                                          &QLL_CANDIDATE_MEM( aEnv ),
                                          aEnv,
                                          QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER,
                                          sBooleanFilter->mCommon.mExprPhraseFlag,
                                          sBooleanFilter->mCommon.mIterationTime,
                                          sBooleanFilter->mCommon.mPosition,
                                          sBooleanFilter->mCommon.mColumnName,
                                          DTL_TYPE_BOOLEAN );

                    sConstBooleanFilter->mExpr = sBooleanFilter->mExpr;
                    sConstBooleanFilter->mSubQueryExprList = sBooleanFilter->mSubQueryExprList;

                    sOutExpr = (qlncExprCommon*)sConstBooleanFilter;
                }
                else
                {
                    sOutExpr = aExpr;
                }

                break;
            }
        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
            {
                sOutExpr = aExpr;
                break;
            }
        default:
            {
                sOutExpr = aExpr;
                STL_DASSERT( 0 );
                break;
            }
    }


    /* Output 설정 */
    *aOutExpr = sOutExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Filter로부터 사용되는 Column에 대한 Target Index를 설정한다.
 * @param[in]       aNodeID         Candidate Node ID
 * @param[in]       aTargetCount    Target Count
 * @param[in,out]   aTargetIdxArr   Target Index Array
 * @param[in]       aExpr           Expression
 */
void qlncFPDSetUsedTargetIdxOnFilter( stlInt32            aNodeID,
                                      stlInt32            aTargetCount,
                                      stlInt32          * aTargetIdxArr,
                                      qlncExprCommon    * aExpr )
{
    STL_DASSERT( aNodeID >= 0 );
    STL_DASSERT( aExpr != NULL );
    STL_DASSERT( aTargetCount > 0 );
    STL_DASSERT( aTargetIdxArr != NULL );


    switch( aExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
        case QLNC_EXPR_TYPE_BIND_PARAM:
        case QLNC_EXPR_TYPE_COLUMN:
            {
                break;
            }
        case QLNC_EXPR_TYPE_FUNCTION:
            {
                qlncExprFunction    * sFunction = NULL;
                stlInt32              i;

                sFunction = (qlncExprFunction*)aExpr;
                for( i = 0; i < sFunction->mArgCount; i++ )
                {
                    (void)qlncFPDSetUsedTargetIdxOnFilter( aNodeID,
                                                           aTargetCount,
                                                           aTargetIdxArr,
                                                           sFunction->mArgs[i] );
                }

                break;
            }
        case QLNC_EXPR_TYPE_CAST:
            {
                qlncExprTypeCast    * sTypeCast = NULL;
                stlInt32              i;

                sTypeCast = (qlncExprTypeCast*)aExpr;
                for( i = 0; i < sTypeCast->mArgCount; i++ )
                {
                    (void)qlncFPDSetUsedTargetIdxOnFilter( aNodeID,
                                                           aTargetCount,
                                                           aTargetIdxArr,
                                                           sTypeCast->mArgs[i] );
                }

                break;
            }
        case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
        case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
        case QLNC_EXPR_TYPE_SUB_QUERY:
        case QLNC_EXPR_TYPE_REFERENCE:
            {
                break;
            }
        case QLNC_EXPR_TYPE_INNER_COLUMN:
            {
                qlncExprInnerColumn * sInnerColumn  = NULL;

                sInnerColumn = (qlncExprInnerColumn*)aExpr;
                if( sInnerColumn->mNode->mNodeID == aNodeID )
                {
                    STL_DASSERT( sInnerColumn->mIdx < aTargetCount );
                    aTargetIdxArr[sInnerColumn->mIdx]++;
                }
                else
                {
                    if( sInnerColumn->mOrgExpr != NULL )
                    {
                        (void)qlncFPDSetUsedTargetIdxOnFilter( aNodeID,
                                                               aTargetCount,
                                                               aTargetIdxArr,
                                                               sInnerColumn->mOrgExpr );
                    }
                }

                break;
            }
        case QLNC_EXPR_TYPE_OUTER_COLUMN:
            {
                STL_DASSERT( 0 );
                break;
            }
        case QLNC_EXPR_TYPE_ROWID_COLUMN:
            {
                break;
            }
        case QLNC_EXPR_TYPE_AGGREGATION:
            {
                qlncExprAggregation * sAggregation  = NULL;
                stlInt32              i;

                sAggregation = (qlncExprAggregation*)aExpr;
                for( i = 0; i < sAggregation->mArgCount; i++ )
                {
                    (void)qlncFPDSetUsedTargetIdxOnFilter( aNodeID,
                                                           aTargetCount,
                                                           aTargetIdxArr,
                                                           sAggregation->mArgs[i] );
                }

                if( sAggregation->mFilter != NULL )
                {
                    /* @todo Aggregation에 Filter는 향후 지원한다. */
                    (void)qlncFPDSetUsedTargetIdxOnFilter( aNodeID,
                                                           aTargetCount,
                                                           aTargetIdxArr,
                                                           sAggregation->mFilter );
                }

                break;
            }
        case QLNC_EXPR_TYPE_CASE_EXPR:
            {
                qlncExprCaseExpr    * sCaseExpr = NULL;
                stlInt32              i;

                sCaseExpr = (qlncExprCaseExpr*)aExpr;
                for( i = 0; i < sCaseExpr->mCount; i++ )
                {
                    (void)qlncFPDSetUsedTargetIdxOnFilter( aNodeID,
                                                           aTargetCount,
                                                           aTargetIdxArr,
                                                           sCaseExpr->mWhenArr[i] );

                    (void)qlncFPDSetUsedTargetIdxOnFilter( aNodeID,
                                                           aTargetCount,
                                                           aTargetIdxArr,
                                                           sCaseExpr->mThenArr[i] );
                }

                (void)qlncFPDSetUsedTargetIdxOnFilter( aNodeID,
                                                       aTargetCount,
                                                       aTargetIdxArr,
                                                       sCaseExpr->mDefResult );

                break;
            }
        case QLNC_EXPR_TYPE_LIST_FUNCTION:
            {
                qlncExprListFunc    * sListFunc = NULL;
                stlInt32              i;

                sListFunc = (qlncExprListFunc*)aExpr;
                for( i = 0; i < sListFunc->mArgCount; i++ )
                {
                    (void)qlncFPDSetUsedTargetIdxOnFilter( aNodeID,
                                                           aTargetCount,
                                                           aTargetIdxArr,
                                                           sListFunc->mArgs[i] );
                }

                break;
            }
        case QLNC_EXPR_TYPE_LIST_COLUMN:
            {
                qlncExprListCol     * sListCol  = NULL;
                stlInt32              i;

                sListCol = (qlncExprListCol*)aExpr;
                for( i = 0; i < sListCol->mArgCount; i++ )
                {
                    (void)qlncFPDSetUsedTargetIdxOnFilter( aNodeID,
                                                           aTargetCount,
                                                           aTargetIdxArr,
                                                           sListCol->mArgs[i] );
                }

                break;
            }
        case QLNC_EXPR_TYPE_ROW_EXPR:
            {
                qlncExprRowExpr     * sRowExpr  = NULL;
                stlInt32              i;

                sRowExpr = (qlncExprRowExpr*)aExpr;
                for( i = 0; i < sRowExpr->mArgCount; i++ )
                {
                    (void)qlncFPDSetUsedTargetIdxOnFilter( aNodeID,
                                                           aTargetCount,
                                                           aTargetIdxArr,
                                                           sRowExpr->mArgs[i] );
                }

                break;
            }
        case QLNC_EXPR_TYPE_AND_FILTER:
            {
                qlncAndFilter   * sAndFilter    = NULL;
                stlInt32          i;

                sAndFilter = (qlncAndFilter*)aExpr;
                for( i = 0; i < sAndFilter->mOrCount; i++ )
                {
                    (void)qlncFPDSetUsedTargetIdxOnFilter( aNodeID,
                                                           aTargetCount,
                                                           aTargetIdxArr,
                                                           (qlncExprCommon*)(sAndFilter->mOrFilters[i]) );
                }

                break;
            }
        case QLNC_EXPR_TYPE_OR_FILTER:
            {
                qlncOrFilter    * sOrFilter = NULL;
                stlInt32          i;

                sOrFilter = (qlncOrFilter*)aExpr;
                for( i = 0; i < sOrFilter->mFilterCount; i++ )
                {
                    (void)qlncFPDSetUsedTargetIdxOnFilter( aNodeID,
                                                           aTargetCount,
                                                           aTargetIdxArr,
                                                           sOrFilter->mFilters[i] );
                }

                break;
            }
        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
            {
                (void)qlncFPDSetUsedTargetIdxOnFilter( aNodeID,
                                                       aTargetCount,
                                                       aTargetIdxArr,
                                                       ((qlncBooleanFilter*)aExpr)->mExpr );

                break;
            }
        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
            {
                break;
            }
        default:
            {
                STL_DASSERT( 0 );
                break;
            }
    }
}


/**
 * @brief Query Set Node에 대하여 Filter Push Down이 가능한지 체크한다.
 * @param[in]   aTargetArr      Target Array
 * @param[in]   aTargetIdxArr   Target Index Array
 * @param[in]   aNode           Candidate Node
 */
stlBool qlncFPDCheckQueryNode( qlncTarget       * aTargetArr,
                               stlInt32         * aTargetIdxArr,
                               qlncNodeCommon   * aNode )
{
    stlBool       sIsPossible;
    stlInt32      i;
    qlncSetOP   * sSetOP        = NULL;


    sIsPossible = STL_FALSE;
    if( aNode->mNodeType == QLNC_NODE_TYPE_QUERY_SET )
    {
        /* Query Set Node에 Offset/Limit 구문이 있으면 안된다. */
        STL_TRY_THROW( (((qlncQuerySet*)aNode)->mOffset == NULL) &&
                       (((qlncQuerySet*)aNode)->mLimit == NULL),
                       RAMP_FINISH );

        /* Query Set Node에 Order By가 존재하는 경우
         * Query Set, Sort Instant, Set OP 순으로 노드가 구성되어 있다. */
        if( ((qlncQuerySet*)aNode)->mChildNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
        {
            STL_DASSERT( ((qlncSortInstantNode*)(((qlncQuerySet*)aNode)->mChildNode))->mChildNode->mNodeType == QLNC_NODE_TYPE_SET_OP );
            sSetOP = (qlncSetOP*)(((qlncSortInstantNode*)(((qlncQuerySet*)aNode)->mChildNode))->mChildNode);
        }
        else
        {
            STL_DASSERT( ((qlncQuerySet*)aNode)->mChildNode->mNodeType == QLNC_NODE_TYPE_SET_OP );
            sSetOP = (qlncSetOP*)(((qlncQuerySet*)aNode)->mChildNode);
        }

        for( i = 0; i < sSetOP->mChildCount; i++ )
        {
            STL_TRY_THROW( qlncFPDCheckQueryNode( aTargetArr,
                                                  aTargetIdxArr,
                                                  sSetOP->mChildNodeArr[i] )
                           == STL_TRUE,
                           RAMP_FINISH );
        }
    }
    else
    {
        STL_DASSERT( aNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC );

        /* Query Spec Node에 Offset/Limit 구문이 있으면 안된다. */
        STL_TRY_THROW( (((qlncQuerySpec*)aNode)->mOffset == NULL) &&
                       (((qlncQuerySpec*)aNode)->mLimit == NULL),
                       RAMP_FINISH );

        /* Target절에 Aggregation이 존재하고
         * Query Spec Node 하위에 Base Table Node가 존재하면 안된다. */
        for( i = 0; i < ((qlncQuerySpec*)aNode)->mTargetCount; i++ )
        {
            if( qlncHasAggregationOnExpr( ((qlncQuerySpec*)aNode)->mTargetArr[i].mExpr,
                                          aNode->mNodeID,
                                          STL_FALSE )
                == STL_TRUE )
            {
                STL_TRY_THROW( ((qlncQuerySpec*)aNode)->mChildNode->mNodeType
                               != QLNC_NODE_TYPE_BASE_TABLE,
                               RAMP_FINISH );
                break;
            }

            /* Filter에 속하는 Column들에 대해서
             * SubTable의 Target과 Query Spec의 Target의 Type이 일치하여야 한다. */
            if( aTargetIdxArr[i] > 0 )
            {
                STL_TRY_THROW( ((qlncQuerySpec*)aNode)->mTargetArr[i].mExpr->mDataType
                               == aTargetArr[i].mExpr->mDataType,
                               RAMP_FINISH );
            }
        }
    }

    sIsPossible = STL_TRUE;


    STL_RAMP( RAMP_FINISH );


    return sIsPossible;
}


/**
 * @brief Query Set Node에 존재하는 Query Spec Node들 모두에 Filter를 Push한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aSubTableNode   Sub-Table Candidate Node
 * @param[in]   aNode           Candidate Node
 * @param[in]   aPushFilter     Push And Filter
 * @param[in]   aEnv            Environment
 */
stlStatus qlncFPDPushFilterOnQueryNode( qlncParamInfo       * aParamInfo,
                                        qlncSubTableNode    * aSubTableNode,
                                        qlncNodeCommon      * aNode,
                                        qlncAndFilter       * aPushFilter,
                                        qllEnv              * aEnv )
{
    stlInt32          i;
    qlncSetOP       * sSetOP            = NULL;
    qlncAndFilter   * sTmpPushFilter    = NULL;
    qlncAndFilter  ** sChildAndFilter   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPushFilter != NULL, QLL_ERROR_STACK(aEnv) );


    if( aNode->mNodeType == QLNC_NODE_TYPE_QUERY_SET )
    {
        /* Query Set Node에 Order By가 존재하는 경우
         * Query Set, Sort Instant, Set OP 순으로 노드가 구성되어 있다. */
        if( ((qlncQuerySet*)aNode)->mChildNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
        {
            STL_DASSERT( ((qlncSortInstantNode*)(((qlncQuerySet*)aNode)->mChildNode))->mChildNode->mNodeType == QLNC_NODE_TYPE_SET_OP );
            sSetOP = (qlncSetOP*)(((qlncSortInstantNode*)(((qlncQuerySet*)aNode)->mChildNode))->mChildNode);
        }
        else
        {
            STL_DASSERT( ((qlncQuerySet*)aNode)->mChildNode->mNodeType == QLNC_NODE_TYPE_SET_OP );
            sSetOP = (qlncSetOP*)(((qlncQuerySet*)aNode)->mChildNode);
        }

        for( i = 0; i < sSetOP->mChildCount; i++ )
        {
            STL_TRY( qlncFPDPushFilterOnQueryNode( aParamInfo,
                                                   aSubTableNode,
                                                   sSetOP->mChildNodeArr[i],
                                                   aPushFilter,
                                                   aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        STL_DASSERT( aNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC );

        /* Filter에서 SubTable의 Target Column에 해당하는 부분을
         * Query Spec의 Target Expr로 대체 */
        STL_TRY( qlncDuplicateExpr( aParamInfo,
                                    (qlncExprCommon*)aPushFilter,
                                    (qlncExprCommon**)&sTmpPushFilter,
                                    aEnv )
                 == STL_SUCCESS );

        for( i = 0; i < sTmpPushFilter->mOrCount; i++ )
        {
            STL_TRY( qlncFPDChangeInnerColumnToTargetExpr(
                         aParamInfo,
                         aSubTableNode->mNodeCommon.mNodeID,
                         ((qlncQuerySpec*)aNode)->mTargetCount,
                         ((qlncQuerySpec*)aNode)->mTargetArr,
                         (qlncExprCommon*)(sTmpPushFilter->mOrFilters[i]),
                         (qlncExprCommon**)&(sTmpPushFilter->mOrFilters[i]),
                         aEnv )
                     == STL_SUCCESS );
        }

        /* Child Node의 Filter 찾기 */
        switch( ((qlncQuerySpec*)aNode)->mChildNode->mNodeType )
        {
            case QLNC_NODE_TYPE_BASE_TABLE:
                sChildAndFilter =
                    &(((qlncBaseTableNode*)(((qlncQuerySpec*)aNode)->mChildNode))->mFilter);
                break;
            case QLNC_NODE_TYPE_SUB_TABLE:
                sChildAndFilter =
                    &(((qlncSubTableNode*)(((qlncQuerySpec*)aNode)->mChildNode))->mFilter);
                break;
            case QLNC_NODE_TYPE_JOIN_TABLE:
                sChildAndFilter =
                    &(((qlncJoinTableNode*)(((qlncQuerySpec*)aNode)->mChildNode))->mFilter);
                break;
            case QLNC_NODE_TYPE_SORT_INSTANT:
                sChildAndFilter =
                    &(((qlncSortInstantNode*)(((qlncQuerySpec*)aNode)->mChildNode))->mFilter);
                break;
            case QLNC_NODE_TYPE_HASH_INSTANT:
                sChildAndFilter =
                    &(((qlncHashInstantNode*)(((qlncQuerySpec*)aNode)->mChildNode))->mFilter);
                break;
            default:
                STL_DASSERT( 0 );
                break;
        }

        /* Child Node의 And Filter에 Push Filter 적용 */
        if( *sChildAndFilter == NULL )
        {
            STL_TRY( qlncCopyAndFilter( aParamInfo,
                                        sTmpPushFilter,
                                        sChildAndFilter,
                                        aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        *sChildAndFilter,
                                                        sTmpPushFilter->mOrCount,
                                                        sTmpPushFilter->mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Single Table Min/Max Aggregation을 Index Scan을 이용하도록 Conversion 한다.
 * @param[in]       aParamInfo  Parameter Info
 * @param[in,out]   aNode       Candidate Node
 * @param[in]       aEnv        Environment
 */
stlStatus qlncSingleTableAggregationConversion( qlncParamInfo   * aParamInfo,
                                                qlncNodeCommon  * aNode,
                                                qllEnv          * aEnv )
{
    stlInt32      i;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aNode->mNodeType )
    {
        case QLNC_NODE_TYPE_QUERY_SET:
            STL_TRY( qlncSingleTableAggregationConversion( aParamInfo,
                                                           ((qlncQuerySet*)aNode)->mChildNode,
                                                           aEnv )
                     == STL_SUCCESS );
            break;

        case QLNC_NODE_TYPE_SET_OP:
            for( i = 0; i < ((qlncSetOP*)aNode)->mChildCount; i++ )
            {
                STL_TRY( qlncSingleTableAggregationConversion( aParamInfo,
                                                               ((qlncSetOP*)aNode)->mChildNodeArr[i],
                                                               aEnv )
                         == STL_SUCCESS );
            }
            break;

        case QLNC_NODE_TYPE_QUERY_SPEC:
            STL_TRY_THROW( ((qlncQuerySpec*)aNode)->mQueryTransformHint->mAllowedQueryTransform == STL_TRUE,
                           RAMP_FINISH );

            if( (((qlncQuerySpec*)aNode)->mChildNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE) &&
                (((qlncBaseTableNode*)(((qlncQuerySpec*)aNode)->mChildNode)))->mFilter == NULL )
            {
                /* Query Spec의 Child Node가 Base Table Node이고,
                 * Filter가 없는 경우에 한해서 Conversion 수행 */
                STL_TRY( qlncSTACQuerySpec( aParamInfo,
                                            (qlncQuerySpec*)aNode,
                                            aEnv )
                         == STL_SUCCESS );
            }

            STL_TRY( qlncSingleTableAggregationConversion( aParamInfo,
                                                           ((qlncQuerySpec*)aNode)->mChildNode,
                                                           aEnv )
                     == STL_SUCCESS );

            if( ((qlncQuerySpec*)aNode)->mNonFilterSubQueryExprList != NULL )
            {
                STL_TRY( qlncSTACSubQueryExprList( aParamInfo,
                                                   ((qlncQuerySpec*)aNode)->mNonFilterSubQueryExprList,
                                                   aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_BASE_TABLE:
            if( ((qlncBaseTableNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncSTACSubQueryOnAndFilter( aParamInfo,
                                                      ((qlncBaseTableNode*)aNode)->mFilter,
                                                      aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_SUB_TABLE:
            STL_TRY( qlncSingleTableAggregationConversion( aParamInfo,
                                                           ((qlncSubTableNode*)aNode)->mQueryNode,
                                                           aEnv )
                     == STL_SUCCESS );

            if( ((qlncSubTableNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncSTACSubQueryOnAndFilter( aParamInfo,
                                                      ((qlncSubTableNode*)aNode)->mFilter,
                                                      aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_JOIN_TABLE:
            for( i = 0; i < ((qlncJoinTableNode*)aNode)->mNodeArray.mCurCnt; i++ )
            {
                STL_TRY( qlncSingleTableAggregationConversion(
                             aParamInfo,
                             ((qlncJoinTableNode*)aNode)->mNodeArray.mNodeArr[i],
                             aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncJoinTableNode*)aNode)->mJoinCond!= NULL )
            {
                STL_TRY( qlncSTACSubQueryOnAndFilter( aParamInfo,
                                                      ((qlncJoinTableNode*)aNode)->mJoinCond,
                                                      aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncJoinTableNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncSTACSubQueryOnAndFilter( aParamInfo,
                                                      ((qlncJoinTableNode*)aNode)->mFilter,
                                                      aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_FLAT_INSTANT:
            STL_DASSERT( 0 );
            break;

        case QLNC_NODE_TYPE_SORT_INSTANT:
            STL_TRY( qlncSingleTableAggregationConversion( aParamInfo,
                                                           ((qlncSortInstantNode*)aNode)->mChildNode,
                                                           aEnv )
                     == STL_SUCCESS );

            if( ((qlncSortInstantNode*)aNode)->mFilter != NULL )
            {
                /* @todo 현재 Sort Instant에 Filter가 존재하는 경우가 없는 것으로 판단됨 */

                STL_TRY( qlncSTACSubQueryOnAndFilter( aParamInfo,
                                                      ((qlncSortInstantNode*)aNode)->mFilter,
                                                      aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncSortInstantNode*)aNode)->mNonFilterSubQueryExprList != NULL )
            {
                STL_TRY( qlncSTACSubQueryExprList(
                             aParamInfo,
                             ((qlncSortInstantNode*)aNode)->mNonFilterSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_HASH_INSTANT:
            STL_TRY( qlncSingleTableAggregationConversion( aParamInfo,
                                                           ((qlncHashInstantNode*)aNode)->mChildNode,
                                                           aEnv )
                     == STL_SUCCESS );

            if( ((qlncHashInstantNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncSTACSubQueryOnAndFilter( aParamInfo,
                                                      ((qlncHashInstantNode*)aNode)->mFilter,
                                                      aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncHashInstantNode*)aNode)->mNonFilterSubQueryExprList != NULL )
            {
                STL_TRY( qlncSTACSubQueryExprList(
                             aParamInfo,
                             ((qlncHashInstantNode*)aNode)->mNonFilterSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        default:
            STL_DASSERT( 0 );
            break;
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief And Filter에 존재하는 SubQuery에 대하여 Single Table Min/Max Aggregation을 Index Scan을 이용하도록 Conversion 한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aAndFilter  AndFilter
 * @param[in]   aEnv        Environment
 */
stlStatus qlncSTACSubQueryOnAndFilter( qlncParamInfo    * aParamInfo,
                                       qlncAndFilter    * aAndFilter,
                                       qllEnv           * aEnv )
{
    stlInt32          i;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter != NULL, QLL_ERROR_STACK(aEnv) );


    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        if( aAndFilter->mOrFilters[i]->mSubQueryExprList != NULL )
        {
            STL_TRY( qlncSTACSubQueryExprList( aParamInfo,
                                               aAndFilter->mOrFilters[i]->mSubQueryExprList,
                                               aEnv )
                     == STL_SUCCESS );
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Expression List에 존재하는 SubQuery에 대하여 Single Table Min/Max Aggregation을 Index Scan을 이용하도록 Conversion 한다.
 * @param[in]   aParamInfo          Parameter Info
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncSTACSubQueryExprList( qlncParamInfo   * aParamInfo,
                                    qlncRefExprList * aSubQueryExprList,
                                    qllEnv          * aEnv )
{
    qlncRefExprItem * sRefExprItem  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );


    sRefExprItem = aSubQueryExprList->mHead;
    while( sRefExprItem != NULL )
    {
        STL_TRY( qlncSingleTableAggregationConversion(
                     aParamInfo,
                     ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode,
                     aEnv )
                 == STL_SUCCESS );

        sRefExprItem = sRefExprItem->mNext;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Spec에서 Min/Max Aggregation의 Index Scan으로 변경이 가능한 경우 변경한다.
 * @param[in]       aParamInfo  Parameter Info
 * @param[in,out]   aQuerySpec  Query Spec Candidate Node
 * @param[in]       aEnv        Environment
 */
stlStatus qlncSTACQuerySpec( qlncParamInfo  * aParamInfo,
                             qlncQuerySpec  * aQuerySpec,
                             qllEnv         * aEnv )
{
    stlInt32              i;
    stlInt32              sIndexCount;
    stlInt32              sIndexStatIdx;
    qlncExprAggregation * sAggregation      = NULL;
    qlncIndexStat       * sIndexStatArr     = NULL;
    qlncAccPathHint     * sAccPathHint      = NULL;
    qlncExprValue       * sValue            = NULL;
    qlncExprFunction    * sFunction         = NULL;
    qlncBooleanFilter   * sBooleanFilter    = NULL;
    stlChar             * sTmpValue         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQuerySpec != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );

    /* Target이 1개이고 Aggregation인지 체크 */
    STL_TRY_THROW( (aQuerySpec->mTargetCount == 1) &&
                   (aQuerySpec->mTargetArr[0].mExpr->mType == QLNC_EXPR_TYPE_AGGREGATION),
                   RAMP_FINISH );

    /* Min 또는 Max인지 체크 */
    sAggregation = (qlncExprAggregation*)(aQuerySpec->mTargetArr[0].mExpr);
    STL_TRY_THROW( (sAggregation->mAggrId == KNL_BUILTIN_AGGREGATION_MIN) ||
                   (sAggregation->mAggrId == KNL_BUILTIN_AGGREGATION_MAX),
                   RAMP_FINISH );

    /* Aggregation의 인자가 단일 Column인지 체크 */
    STL_TRY_THROW( (sAggregation->mArgCount == 1) &&
                   (sAggregation->mArgs[0]->mType == QLNC_EXPR_TYPE_COLUMN),
                   RAMP_FINISH );

    /* Aggregation의 Column이 Index의 첫번째 Column으로 구성된 Index가 존재하는지 체크 */
    sIndexCount = ((qlncBaseTableNode*)(aQuerySpec->mChildNode))->mIndexCount;
    sIndexStatArr = ((qlncBaseTableNode*)(aQuerySpec->mChildNode))->mIndexStatArr;

    for( i = 0; i < sIndexCount; i++ )
    {
        if( ((qlncExprColumn*)(sAggregation->mArgs[0]))->mColumnID ==
            ellGetColumnID( &(sIndexStatArr[i].mIndexKeyDesc[0].mKeyColumnHandle) ) )
        {
            sIndexStatIdx = i;
            break;
        }
    }

    STL_TRY_THROW( i < sIndexCount, RAMP_FINISH );

    /* Query Spec의 Offset이 없거나 0이어야 한다. */
    if( aQuerySpec->mOffset != NULL )
    {
        /** @todo offset이 0인지 여부 체크하는 것을 추가해야 함 */
        STL_THROW( RAMP_FINISH );
    }

    /* Access Path Hint를 사용자가 지정했었는지 여부 체크 */
    sAccPathHint = ((qlncBaseTableNode*)(aQuerySpec->mChildNode))->mAccPathHint;
    if( (sAccPathHint->mPossibleTableScan == STL_TRUE) &&
        (sAccPathHint->mPossibleRowidScan == STL_TRUE) )
    {
        if( sIndexCount > 0 )
        {
            if( (sAccPathHint->mPossibleIndexScan == STL_TRUE) &&
                (sAccPathHint->mPossibleIndexCombine == STL_TRUE) )
            {
                /* Do Nothing */
            }
            else
            {
                STL_THROW( RAMP_FINISH );
            }
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        STL_THROW( RAMP_FINISH );
    }


    /**
     * 여기까지 온 경우는 Index Scan으로 변경 가능한 경우임
     */

    /* Base Table의 Access Path Hint를 IndexScan으로 변경 */
    STL_DASSERT( sAccPathHint->mPossibleIndexScanCount > 0 );

    sAccPathHint->mPossibleTableScan = STL_FALSE;
    sAccPathHint->mPossibleIndexScan = STL_TRUE;
    sAccPathHint->mPossibleRowidScan = STL_FALSE;
    sAccPathHint->mPossibleIndexCombine = STL_FALSE;

    sAccPathHint->mPossibleIndexScanCount = 1;
    sAccPathHint->mPossibleIndexScanInfoArr[0].mOffset = sIndexStatIdx;

    if( sAggregation->mAggrId == KNL_BUILTIN_AGGREGATION_MIN )
    {
        if( sIndexStatArr[sIndexStatIdx].mIndexKeyDesc[0].mKeyOrdering == ELL_INDEX_COLUMN_ASCENDING )
        {
            sAccPathHint->mPossibleIndexScanInfoArr[0].mIsAsc = STL_TRUE;
        }
        else
        {
            STL_DASSERT( sIndexStatArr[sIndexStatIdx].mIndexKeyDesc[0].mKeyOrdering ==
                         ELL_INDEX_COLUMN_DESCENDING );
            sAccPathHint->mPossibleIndexScanInfoArr[0].mIsAsc = STL_FALSE;
        }
    }
    else
    {
        STL_DASSERT( sAggregation->mAggrId == KNL_BUILTIN_AGGREGATION_MAX );
        if( sIndexStatArr[sIndexStatIdx].mIndexKeyDesc[0].mKeyOrdering == ELL_INDEX_COLUMN_ASCENDING )
        {
            sAccPathHint->mPossibleIndexScanInfoArr[0].mIsAsc = STL_FALSE;
        }
        else
        {
            STL_DASSERT( sIndexStatArr[sIndexStatIdx].mIndexKeyDesc[0].mKeyOrdering ==
                         ELL_INDEX_COLUMN_DESCENDING );
            sAccPathHint->mPossibleIndexScanInfoArr[0].mIsAsc = STL_TRUE;
        }
    }

    /* Base Table의 Filter에 Aggregation Column의 Is Not Null 조건을 추가 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprFunction ),
                                (void**) &sFunction,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTmpValue = NULL;
    QLNC_SET_EXPR_COMMON( &sFunction->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_FUNCTION,
                          QLNC_EXPR_PHRASE_WHERE,
                          DTL_ITERATION_TIME_FOR_EACH_EXPR,
                          0,
                          sTmpValue,
                          DTL_TYPE_BOOLEAN );

    sFunction->mFuncId = KNL_BUILTIN_FUNC_IS_NOT_NULL;
    sFunction->mArgCount = 1;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ),
                                (void**) &(sFunction->mArgs),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlncDuplicateExpr( aParamInfo,
                                sAggregation->mArgs[0],
                                &(sFunction->mArgs[0]),
                                aEnv )
             == STL_SUCCESS );

    /* Boolean Filter 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncBooleanFilter ),
                                (void**) &sBooleanFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTmpValue = NULL;
    QLNC_SET_EXPR_COMMON( &sBooleanFilter->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_BOOLEAN_FILTER,
                          QLNC_EXPR_PHRASE_WHERE,
                          DTL_ITERATION_TIME_FOR_EACH_EXPR,
                          0,
                          sTmpValue,
                          DTL_TYPE_BOOLEAN );

    sBooleanFilter->mFuncID = sFunction->mFuncId;
    sBooleanFilter->mExpr = (qlncExprCommon*)sFunction;
    QLNC_INIT_LIST( &(sBooleanFilter->mRefNodeList) );
    sBooleanFilter->mLeftRefNodeList = NULL;
    sBooleanFilter->mRightRefNodeList = NULL;
    sBooleanFilter->mPossibleJoinCond = STL_FALSE;
    sBooleanFilter->mIsPhysicalFilter = STL_FALSE;
    sBooleanFilter->mSubQueryExprList = NULL;

    if( ((qlncBaseTableNode*)(aQuerySpec->mChildNode))->mFilter == NULL )
    {
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncAndFilter ),
                                    (void**)&(((qlncBaseTableNode*)(aQuerySpec->mChildNode))->mFilter),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_INIT_AND_FILTER( ((qlncBaseTableNode*)(aQuerySpec->mChildNode))->mFilter );
    }

    STL_TRY( qlncAppendFilterToAndFilter( aParamInfo,
                                          ((qlncBaseTableNode*)(aQuerySpec->mChildNode))->mFilter,
                                          (qlncExprCommon*)sBooleanFilter,
                                          aEnv )
             == STL_SUCCESS );


    /* Query Spec Node의 Target의 Aggregation을 Column으로 변경 */
    aQuerySpec->mTargetArr[0].mExpr = sAggregation->mArgs[0];

    /* Query Spec Node에 Limit 정보를 추가 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprValue ),
                                (void**) &sValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sValue->mValueType = QLV_VALUE_TYPE_NUMERIC;
    sValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    QLNC_ALLOC_AND_COPY_STRING( sValue->mData.mString,
                                "1",
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );

    sTmpValue = NULL;
    QLNC_SET_EXPR_COMMON( &sValue->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_VALUE,
                          QLNC_EXPR_PHRASE_OFFSET_LIMIT,
                          DTL_ITERATION_TIME_NONE,
                          0,
                          sTmpValue,
                          DTL_TYPE_NUMBER );

    aQuerySpec->mLimit = (qlncExprCommon*)sValue;


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Quantify Operator를 제거한다.
 * @param[in]       aParamInfo  Parameter Info
 * @param[in,out]   aNode       Candidate Node
 * @param[in]       aEnv        Environment
 */
stlStatus qlncQuantifyOperatorElemination( qlncParamInfo    * aParamInfo,
                                           qlncNodeCommon   * aNode,
                                           qllEnv           * aEnv )
{
    stlInt32      i;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aNode->mNodeType )
    {
        case QLNC_NODE_TYPE_QUERY_SET:
            STL_TRY( qlncQuantifyOperatorElemination( aParamInfo,
                                                      ((qlncQuerySet*)aNode)->mChildNode,
                                                      aEnv )
                     == STL_SUCCESS );
            break;

        case QLNC_NODE_TYPE_SET_OP:
            for( i = 0; i < ((qlncSetOP*)aNode)->mChildCount; i++ )
            {
                STL_TRY( qlncQuantifyOperatorElemination( aParamInfo,
                                                          ((qlncSetOP*)aNode)->mChildNodeArr[i],
                                                          aEnv )
                         == STL_SUCCESS );
            }
            break;

        case QLNC_NODE_TYPE_QUERY_SPEC:
            STL_TRY_THROW( ((qlncQuerySpec*)aNode)->mQueryTransformHint->mAllowedQueryTransform == STL_TRUE,
                           RAMP_FINISH );

            STL_TRY( qlncQuantifyOperatorElemination( aParamInfo,
                                                      ((qlncQuerySpec*)aNode)->mChildNode,
                                                      aEnv )
                     == STL_SUCCESS );

            if( ((qlncQuerySpec*)aNode)->mNonFilterSubQueryExprList != NULL )
            {
                STL_TRY( qlncQOESubQueryExprList( aParamInfo,
                                                  ((qlncQuerySpec*)aNode)->mNonFilterSubQueryExprList,
                                                  aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_BASE_TABLE:
            if( ((qlncBaseTableNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncQOEOnAndFilter( aParamInfo,
                                             ((qlncBaseTableNode*)aNode)->mFilter,
                                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_SUB_TABLE:
            STL_TRY( qlncQuantifyOperatorElemination( aParamInfo,
                                                      ((qlncSubTableNode*)aNode)->mQueryNode,
                                                      aEnv )
                     == STL_SUCCESS );

            if( ((qlncSubTableNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncQOEOnAndFilter( aParamInfo,
                                             ((qlncSubTableNode*)aNode)->mFilter,
                                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_JOIN_TABLE:
            for( i = 0; i < ((qlncJoinTableNode*)aNode)->mNodeArray.mCurCnt; i++ )
            {
                STL_TRY( qlncQuantifyOperatorElemination(
                             aParamInfo,
                             ((qlncJoinTableNode*)aNode)->mNodeArray.mNodeArr[i],
                             aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncJoinTableNode*)aNode)->mJoinCond!= NULL )
            {
                STL_TRY( qlncQOEOnAndFilter( aParamInfo,
                                             ((qlncJoinTableNode*)aNode)->mJoinCond,
                                             aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncJoinTableNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncQOEOnAndFilter( aParamInfo,
                                             ((qlncJoinTableNode*)aNode)->mFilter,
                                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_FLAT_INSTANT:
            STL_DASSERT( 0 );
            break;

        case QLNC_NODE_TYPE_SORT_INSTANT:
            STL_TRY( qlncQuantifyOperatorElemination( aParamInfo,
                                                      ((qlncSortInstantNode*)aNode)->mChildNode,
                                                      aEnv )
                     == STL_SUCCESS );

            if( ((qlncSortInstantNode*)aNode)->mFilter != NULL )
            {
                /* @todo 현재 Sort Instant에 Filter가 존재하는 경우가 없는 것으로 판단됨 */

                STL_TRY( qlncQOEOnAndFilter( aParamInfo,
                                             ((qlncSortInstantNode*)aNode)->mFilter,
                                             aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncSortInstantNode*)aNode)->mNonFilterSubQueryExprList != NULL )
            {
                STL_TRY( qlncQOESubQueryExprList(
                             aParamInfo,
                             ((qlncSortInstantNode*)aNode)->mNonFilterSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_HASH_INSTANT:
            STL_TRY( qlncQuantifyOperatorElemination( aParamInfo,
                                                      ((qlncHashInstantNode*)aNode)->mChildNode,
                                                      aEnv )
                     == STL_SUCCESS );

            if( ((qlncHashInstantNode*)aNode)->mFilter != NULL )
            {
                STL_TRY( qlncQOEOnAndFilter( aParamInfo,
                                             ((qlncHashInstantNode*)aNode)->mFilter,
                                             aEnv )
                         == STL_SUCCESS );
            }

            if( ((qlncHashInstantNode*)aNode)->mNonFilterSubQueryExprList != NULL )
            {
                STL_TRY( qlncQOESubQueryExprList(
                             aParamInfo,
                             ((qlncHashInstantNode*)aNode)->mNonFilterSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        default:
            STL_DASSERT( 0 );
            break;
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief And Filter에 존재하는 Quantify Operator에 대하여 제거 가능한지 판단하여 가능하면 제거한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aAndFilter  AndFilter
 * @param[in]   aEnv        Environment
 */
stlStatus qlncQOEOnAndFilter( qlncParamInfo * aParamInfo,
                              qlncAndFilter * aAndFilter,
                              qllEnv        * aEnv )
{
    stlInt32              i;
    stlInt32              j;
    qlncExprListFunc    * sListFunc         = NULL;
    qlncExprSubQuery    * sSubQuery         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter != NULL, QLL_ERROR_STACK(aEnv) );


    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        /* SubQuery가 존재해야 한다. */
        if( aAndFilter->mOrFilters[i]->mSubQueryExprList == NULL )
        {
            continue;
        }

        for( j = 0; j < aAndFilter->mOrFilters[i]->mFilterCount; j++ )
        {
            if( aAndFilter->mOrFilters[i]->mFilters[j]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER )
            {
                /* SubQuery가 존재해야 한다. */
                if( ((qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mSubQueryExprList == NULL )
                {
                    continue;
                }

                /* List function이어야 한다. */
                if( ((qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mExpr->mType
                    != QLNC_EXPR_TYPE_LIST_FUNCTION )
                {
                    continue;
                }

                sListFunc = (qlncExprListFunc*)(((qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mExpr);
            }
            else
            {
                STL_DASSERT( aAndFilter->mOrFilters[i]->mFilters[j]->mType == QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER );

                /* SubQuery가 존재해야 한다. */
                if( ((qlncConstBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mSubQueryExprList == NULL )
                {
                    continue;
                }

                /* List function이어야 한다. */
                if( ((qlncConstBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mExpr->mType
                    != QLNC_EXPR_TYPE_LIST_FUNCTION )
                {
                    continue;
                }

                sListFunc = (qlncExprListFunc*)(((qlncConstBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mExpr);
            }

            /**
             * @todo ALL 연산자는 no rows와 null 데이터에 대한 체크가 불가능하기 때문에
             * 현재 지원하지 않고, 나중에 지원하도록 한다.
             */

            /* List Function의 ID는 Quantify Operator이어야 한다. */
            if( (sListFunc->mFuncId < KNL_BUILTIN_FUNC_LESS_THAN_ANY) ||
                (sListFunc->mFuncId > KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY) )
            {
                continue;
            }
            STL_DASSERT( sListFunc->mArgCount == 2 );

            /* Left List Column의 Row Expression은 1개의 Expression만 가져야 한다. */
            if( ((qlncExprListCol*)(sListFunc->mArgs[1]))->mArgCount != 1 )
            {
                continue;
            }

            if( ((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[1]))->mArgs[0]))->mArgCount != 1 )
            {
                continue;
            }

            /* Right List Column의 Row Expression은 1개의 SubQuery Expression으로 구성되어 있어야 한다. */
            if( ((qlncExprListCol*)(sListFunc->mArgs[0]))->mArgCount != 1 )
            {
                /* @todo 현재 Left와 Right의 Row Expression의 개수는 동일하기 때문에
                 * 여기로 들어오는 경우가 없다. */
                continue;
            }

            if( ((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[0]))->mArgs[0]))->mArgCount != 1 )
            {
                continue;
            }

            if( ((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[0]))->mArgs[0]))->mArgs[0]->mType != QLNC_EXPR_TYPE_SUB_QUERY )
            {
                /* @todo 현재 Right에 Row Expression과 Expression이 각각 1개만 존재하는 경우
                 * Validation에서 List Function을 Function으로 변경시킨다. */
                continue;
            }

            /* SubQuery 설정 */
            sSubQuery = (qlncExprSubQuery*)(((qlncExprRowExpr*)(((qlncExprListCol*)(sListFunc->mArgs[0]))->mArgs[0]))->mArgs[0]);

            /* SubQuery는 Outer Column을 참조하지 않아야 한다. */
            if( (sSubQuery->mRefExprList != NULL) &&
                (sSubQuery->mRefExprList->mCount > 0) )
            {
                continue;
            }

            /* SubQuery는 Query Set이 아니어야 하고,
             * 하나의 Target으로 구성되어 있어야 하며,
             * Offset, Limit이 없어야 한다. */
            if( sSubQuery->mNode->mNodeType != QLNC_NODE_TYPE_QUERY_SPEC )
            {
                continue;
            }

            if( ((qlncQuerySpec*)(sSubQuery->mNode))->mTargetCount != 1 )
            {
                continue;
            }

            if( (((qlncQuerySpec*)(sSubQuery->mNode))->mOffset != NULL) ||
                (((qlncQuerySpec*)(sSubQuery->mNode))->mLimit != NULL) )
            {
                continue;
            }

            /* SubQuery는 Query Spec의 Target의 Type은 min/max aggregation이 가능해야 한다. */
            if( (((qlncQuerySpec*)(sSubQuery->mNode))->mTargetArr[0].mExpr->mDataType == DTL_TYPE_LONGVARCHAR) ||
                (((qlncQuerySpec*)(sSubQuery->mNode))->mTargetArr[0].mExpr->mDataType == DTL_TYPE_CLOB) ||
                (((qlncQuerySpec*)(sSubQuery->mNode))->mTargetArr[0].mExpr->mDataType == DTL_TYPE_LONGVARBINARY) ||
                (((qlncQuerySpec*)(sSubQuery->mNode))->mTargetArr[0].mExpr->mDataType == DTL_TYPE_BLOB) )
            {
                continue;
            }

            /* SubQuery는 Query Spec에 Distinct가 존재해서는 안되며,
             * Order By가 존재해서도 안되며,
             * group by없이 aggregation이 있으면 안된다. */
            if( ((qlncQuerySpec*)(sSubQuery->mNode))->mChildNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT )
            {
                /* order by가 없이 group by 혹은 distinct가 기술된 경우 */
                if( ((qlncHashInstantNode*)(((qlncQuerySpec*)(sSubQuery->mNode))->mChildNode))->mInstantType == QLNC_INSTANT_TYPE_DISTINCT )
                {
                    continue;
                }
            }
            else if( ((qlncQuerySpec*)(sSubQuery->mNode))->mChildNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
            {
                continue;
            }
            else
            {
                /* Target에 aggregation이 있는지 체크 */
                if( qlncHasAggregationOnExpr(
                        ((qlncQuerySpec*)(sSubQuery->mNode))->mTargetArr[0].mExpr,
                        ((qlncQuerySpec*)(sSubQuery->mNode))->mNodeCommon.mNodeID,
                        STL_FALSE )
                    == STL_TRUE )
                {
                    continue;
                }
            }

            if( aAndFilter->mOrFilters[i]->mFilters[j]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER )
            {
                STL_TRY( qlncQOEListFunctionToFunction(
                             aParamInfo,
                             sListFunc,
                             &(((qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mExpr),
                             aEnv )
                         == STL_SUCCESS );
                ((qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mFuncID
                    = ((qlncExprFunction*)(((qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mExpr))->mFuncId;
            }
            else
            {
                STL_TRY( qlncQOEListFunctionToFunction(
                             aParamInfo,
                             sListFunc,
                             &(((qlncConstBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mExpr),
                             aEnv )
                         == STL_SUCCESS );
            }
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;

}


/**
 * @brief List Function을 Function으로 변환한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aListFunc       List Function
 * @param[out]  aFunction       Candidate Expression
 * @param[in]   aEnv            Environment
 */
stlStatus qlncQOEListFunctionToFunction( qlncParamInfo      * aParamInfo,
                                         qlncExprListFunc   * aListFunc,
                                         qlncExprCommon    ** aFunction,
                                         qllEnv             * aEnv )
{
    qlncExprFunction    * sFunction         = NULL;
    qlncExprAggregation * sAggregation      = NULL;
    qlncQuerySpec       * sQuerySpec        = NULL;
    knlBuiltInFunc        sFuncId;
    knlBuiltInAggrFunc    sAggrId;
    qlncExprCommon      * sTmpExpr      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aListFunc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFunction != NULL, QLL_ERROR_STACK(aEnv) );

#define QLNC_ALLOC_FUNCTION( _aFunction,                                            \
                             _aFuncID,                                              \
                             _aArgCount,                                            \
                             _aSrcExpr,                                             \
                             _aDataType )                                           \
    {                                                                               \
        /* Candidate Function Expression 할당 */                                    \
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),                     \
                                    STL_SIZEOF( qlncExprFunction ),                 \
                                    (void**) &(_aFunction),                         \
                                    KNL_ENV(aEnv) )                                 \
                 == STL_SUCCESS );                                                  \
                                                                                    \
        /* Function Expression 정보 셋팅 */                                         \
        (_aFunction)->mFuncId = (_aFuncID);                                         \
                                                                                    \
        /* Function Argument Count 정보 셋팅 */                                     \
        (_aFunction)->mArgCount = (_aArgCount);                                     \
                                                                                    \
        /* Function Argument Expression 할당 */                                     \
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),                     \
                                    STL_SIZEOF( qlncExprCommon* ) * (_aArgCount),   \
                                    (void**) &((_aFunction)->mArgs),                \
                                    KNL_ENV(aEnv) )                                 \
                 == STL_SUCCESS );                                                  \
                                                                                    \
        /* Function의 Expression Common 정보 셋팅 */                                \
        QLNC_SET_EXPR_COMMON( &(_aFunction)->mCommon,                               \
                              &QLL_CANDIDATE_MEM( aEnv ),                           \
                              aEnv,                                                 \
                              QLNC_EXPR_TYPE_FUNCTION,                              \
                              (_aSrcExpr)->mExprPhraseFlag,                         \
                              (_aSrcExpr)->mIterationTime,                          \
                              (_aSrcExpr)->mPosition,                               \
                              (_aSrcExpr)->mColumnName,                             \
                              (_aDataType) );                                       \
    }

#define QLNC_ALLOC_AGGREGATION( _aAggregation,                          \
                                _aAggrId,                               \
                                _aIsDistinct,                           \
                                _aHasNestedAggr,                        \
                                _aFilter,                               \
                                _aSrcExpr )                             \
    {                                                                   \
        /* Candidate Aggregation Expression 할당 */                     \
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),         \
                                    STL_SIZEOF( qlncExprAggregation ),  \
                                    (void**) &(_aAggregation),          \
                                    KNL_ENV(aEnv) )                     \
                 == STL_SUCCESS );                                      \
                                                                        \
        /* Aggregation ID 정보 셋팅 */                                  \
        (_aAggregation)->mAggrId = (_aAggrId);                          \
                                                                        \
        /* Aggregation Count 정보 셋팅 */                               \
        (_aAggregation)->mArgCount = 1;                                 \
                                                                        \
        /* Aggregation Argument Expression 할당 */                      \
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),         \
                                    STL_SIZEOF( qlncExprCommon* ),      \
                                    (void**) &((_aAggregation)->mArgs), \
                                    KNL_ENV(aEnv) )                     \
                 == STL_SUCCESS );                                      \
                                                                        \
        /* Aggregation 기타 정보 셋팅 */                                \
        (_aAggregation)->mIsDistinct = (_aIsDistinct);                  \
        (_aAggregation)->mHasNestedAggr = (_aHasNestedAggr);            \
        (_aAggregation)->mFilter = (_aFilter);                          \
                                                                        \
        /* Aggregation의 Expression Common 정보 셋팅 */                 \
        QLNC_SET_EXPR_COMMON( &(_aAggregation)->mCommon,                \
                              &QLL_CANDIDATE_MEM( aEnv ),               \
                              aEnv,                                     \
                              QLNC_EXPR_TYPE_AGGREGATION,               \
                              (_aSrcExpr)->mExprPhraseFlag,             \
                              (_aSrcExpr)->mIterationTime,              \
                              (_aSrcExpr)->mPosition,                   \
                              (_aSrcExpr)->mColumnName,                 \
                              (_aSrcExpr)->mDataType );                 \
    }


    /* Function ID 및 Aggregation ID 설정 */
    if( aListFunc->mFuncId == KNL_BUILTIN_FUNC_LESS_THAN_ANY )
    {
        sFuncId = KNL_BUILTIN_FUNC_IS_LESS_THAN;
        sAggrId = KNL_BUILTIN_AGGREGATION_MAX;
    }
    else if( aListFunc->mFuncId == KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY )
    {
        sFuncId = KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL;
        sAggrId = KNL_BUILTIN_AGGREGATION_MAX;
    }
    else if( aListFunc->mFuncId == KNL_BUILTIN_FUNC_GREATER_THAN_ANY )
    {
        sFuncId = KNL_BUILTIN_FUNC_IS_GREATER_THAN;
        sAggrId = KNL_BUILTIN_AGGREGATION_MIN;
    }
    else
    {
        STL_DASSERT( aListFunc->mFuncId == KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY );
        sFuncId = KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL;
        sAggrId = KNL_BUILTIN_AGGREGATION_MIN;
    }

    /* Function 할당 */
    QLNC_ALLOC_FUNCTION( sFunction,
                         sFuncId,
                         2,
                         (qlncExprCommon*)aListFunc,
                         DTL_TYPE_BOOLEAN );

    /* Left Expression 정보 셋팅 */
    sFunction->mArgs[0] = ((qlncExprRowExpr*)(((qlncExprListCol*)(aListFunc->mArgs[1]))->mArgs[0]))->mArgs[0];

    /* Right Expression 정보 셋팅 */
    sFunction->mArgs[1] = ((qlncExprRowExpr*)(((qlncExprListCol*)(aListFunc->mArgs[0]))->mArgs[0]))->mArgs[0];
    STL_DASSERT( sFunction->mArgs[1]->mType == QLNC_EXPR_TYPE_SUB_QUERY );

    /* SubQuery의 Relation 여부를 False로 셋팅 */
    ((qlncExprSubQuery*)(sFunction->mArgs[1]))->mIsRelSubQuery = STL_FALSE;

    /* SubQuery의 Related Function ID 변경 */
    ((qlncExprSubQuery*)(sFunction->mArgs[1]))->mRelatedFuncId = sFunction->mFuncId;

    /* SubQuery의 Target을 Aggregation으로 변경한다. */
    sQuerySpec = (qlncQuerySpec*)(((qlncExprSubQuery*)(sFunction->mArgs[1]))->mNode);

    QLNC_ALLOC_AGGREGATION( sAggregation,
                            sAggrId,
                            STL_FALSE,
                            STL_FALSE,
                            NULL,
                            sQuerySpec->mTargetArr[0].mExpr );

    /* 하위 노드가 group by이거나 group by order by인 경우 */
    if( sQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT )
    {
        /* Aggregation Argument 정보 셋팅 */
        sAggregation->mArgs[0] = sQuerySpec->mTargetArr[0].mExpr;

        sAggregation->mHasNestedAggr
            = qlncHasAggregationOnExpr(
                  sQuerySpec->mTargetArr[0].mExpr,
                  sQuerySpec->mChildNode->mNodeID,
                  STL_FALSE );

        /* group by 노드의 key column에 대하여 aggregation이 이루어진 경우로
         * read column에 해당 aggregation을 추가한다. */
        if( sAggregation->mHasNestedAggr == STL_FALSE )
        {
            STL_TRY( qlncQOEAddAggregationOnHashInstant(
                         aParamInfo,
                         ((qlncHashInstantNode*)(sQuerySpec->mChildNode)),
                         (qlncExprCommon*)sAggregation,
                         &sTmpExpr,
                         aEnv )
                     == STL_SUCCESS );

            /**
             * single row 반환을 위해 해당 aggregation을 다시 동일 aggregation으로 한번 더 감싼다.
             */

            QLNC_ALLOC_AGGREGATION( sAggregation,
                                    sAggrId,
                                    STL_FALSE,
                                    STL_TRUE,
                                    NULL,
                                    sTmpExpr );

            /* Aggregation Argument 정보 셋팅 */
            sAggregation->mArgs[0] = sTmpExpr;

            sTmpExpr = (qlncExprCommon*)sAggregation;
        }
        else
        {
            sTmpExpr = (qlncExprCommon*)sAggregation;
        }

        /* Aggregation을 Target에 셋팅 */
        sQuerySpec->mTargetArr[0].mExpr = sTmpExpr;
    }
    else
    {
        /* Aggregation Argument 정보 셋팅 */
        sAggregation->mArgs[0] = sQuerySpec->mTargetArr[0].mExpr;

        sAggregation->mHasNestedAggr
            = qlncHasAggregationOnExpr(
                  sQuerySpec->mTargetArr[0].mExpr,
                  sQuerySpec->mNodeCommon.mNodeID,
                  STL_FALSE );

        /* Aggregation을 Target에 셋팅 */
        sQuerySpec->mTargetArr[0].mExpr = (qlncExprCommon*)sAggregation;
    }

    /* Output 설정 */
    *aFunction = (qlncExprCommon*)sFunction;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;

}


/**
 * @brief SubQuery Expression List에 존재하는 SubQuery에 대하여 Quantify Operator Elemination 한다.
 * @param[in]   aParamInfo          Parameter Info
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncQOESubQueryExprList( qlncParamInfo    * aParamInfo,
                                   qlncRefExprList  * aSubQueryExprList,
                                   qllEnv           * aEnv )
{
    qlncRefExprItem * sRefExprItem  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );


    sRefExprItem = aSubQueryExprList->mHead;
    while( sRefExprItem != NULL )
    {
        STL_TRY( qlncQuantifyOperatorElemination(
                     aParamInfo,
                     ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode,
                     aEnv )
                 == STL_SUCCESS );

        sRefExprItem = sRefExprItem->mNext;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Instant에 Aggregation을 추가한다.
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aHashInstant    Hash Instant
 * @param[in]   aAggregation    Aggregation Expression
 * @param[out]  aOutExpr        Output Expression
 * @param[in]   aEnv            Environment
 */
stlStatus qlncQOEAddAggregationOnHashInstant( qlncParamInfo         * aParamInfo,
                                              qlncHashInstantNode   * aHashInstant,
                                              qlncExprCommon        * aAggregation,
                                              qlncExprCommon       ** aOutExpr,
                                              qllEnv                * aEnv )
{
    stlInt32              sIdx;
    qlncRefExprItem     * sRefExprItem      = NULL;
    qlncRefExprItem     * sTmpRefExprItem   = NULL;
    qlncExprCommon      * sTmpExpr          = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashInstant != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggregation != NULL, QLL_ERROR_STACK(aEnv) );


    sRefExprItem = NULL;
    if( aHashInstant->mReadColList != NULL )
    {
        sRefExprItem = aHashInstant->mReadColList->mHead;
        while( sRefExprItem != NULL )
        {
            STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
            if( qlncIsSameExpr( aAggregation,
                                ((qlncExprInnerColumn*)(sRefExprItem->mExpr))->mOrgExpr )
                == STL_TRUE )
            {
                /* @todo 상위에서 새로 만든 Aggregation으로 기존에 존재하지 않는 Aggregation이다.
                 * 기존에 존재한 Aggregation도 Nested Aggregation으로 만드므로 겹치는 일이 없다. */
                break;
            }

            sRefExprItem = sRefExprItem->mNext;
        }
    }

    if( sRefExprItem != NULL )
    {
        /* @todo 위에서 DASSERT로 해놓았으므로 여기도 들어올 일이 없다. */
        sTmpExpr = sRefExprItem->mExpr;
    }
    else
    {
        STL_DASSERT( aHashInstant->mKeyColList != NULL );
        if( aHashInstant->mRecColList == NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefExprList ),
                                        (void**) &(aHashInstant->mRecColList),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            QLNC_INIT_LIST( aHashInstant->mRecColList );

            sIdx = aHashInstant->mKeyColList->mCount;
        }
        else
        {
            sIdx = aHashInstant->mKeyColList->mCount + aHashInstant->mRecColList->mCount;
        }

        if( aHashInstant->mReadColList == NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefExprList ),
                                        (void**) &(aHashInstant->mReadColList),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            QLNC_INIT_LIST( aHashInstant->mReadColList );
        }

        /* Inner Column으로 감싸 등록 */
        STL_TRY( qlncWrapExprInInnerColumn(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     aAggregation,
                     (qlncNodeCommon*)aHashInstant,
                     sIdx,
                     &sTmpExpr,
                     aEnv )
                 == STL_SUCCESS );

        /* Reference Expression Item 할당 */
        STL_TRY( knlAllocRegionMem(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     STL_SIZEOF( qlncRefExprItem ),
                     (void**) &sTmpRefExprItem,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sTmpRefExprItem->mExpr = sTmpExpr;
        sTmpRefExprItem->mNext = NULL;

        /* Rec Column List에 추가 */
        QLNC_APPEND_ITEM_TO_LIST( aHashInstant->mRecColList, sTmpRefExprItem );

        /* Inner Column으로 감싸 등록 */
        STL_TRY( qlncWrapExprInInnerColumn(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     aAggregation,
                     (qlncNodeCommon*)aHashInstant,
                     sIdx,
                     &sTmpExpr,
                     aEnv )
                 == STL_SUCCESS );

        /* Reference Expression Item 할당 */
        STL_TRY( knlAllocRegionMem(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     STL_SIZEOF( qlncRefExprItem ),
                     (void**) &sTmpRefExprItem,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sTmpRefExprItem->mExpr = sTmpExpr;
        sTmpRefExprItem->mNext = NULL;

        /* Read Column List에 추가 */
        QLNC_APPEND_ITEM_TO_LIST( aHashInstant->mReadColList, sTmpRefExprItem );
    }

    /* Output 설정 */
    *aOutExpr = sTmpExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */

