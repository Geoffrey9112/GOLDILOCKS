/*******************************************************************************
 * qlnvOrderBy.c
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
 * @file qlnvOrderBy.c
 * @brief SQL Processor Layer Validation - Order By Functions
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
 * @brief Order By List를 생성한다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aValidationObjList   Validation Object List
 * @param[in]  aRefTableList        Reference Table List
 * @param[in]  aTargetCount         Target Count
 * @param[in]  aTargets             Target Array
 * @param[in]  aAllowedAggrDepth    Allowed Aggregation Depth
 * @param[in]  aOrderBySource       Order By Clause Source
 * @param[out] aOrderBy             Order By Clause
 * @param[in]  aEnv                 Environment
 * @remarks 
 */
stlStatus qlvValidateOrderBy( qlvStmtInfo       * aStmtInfo,
                              ellObjectList     * aValidationObjList,
                              qlvRefTableList   * aRefTableList,
                              stlInt32            aTargetCount,
                              qlvInitTarget     * aTargets,
                              stlInt32            aAllowedAggrDepth,
                              qlpList           * aOrderBySource,
                              qlvOrderBy       ** aOrderBy,
                              qllEnv            * aEnv )
{
    qlvOrderBy          * sOrderBy      = NULL;
    qlpOrderBy          * sParseOrderBy = NULL;
    qlpListElement      * sListElem     = NULL;

    qlvInitExpression   * sSortKeyExpr  = NULL;

    stlChar             * sNumString    = NULL;
    stlInt64              sNumber       = 0;

    qlvRefTableList     * sRefTableList = NULL;

    qlvInitNode         * sTableNode    = NULL;
    qlvRelationName     * sRelationName = NULL;

    stlUInt8              sColFlag      = 0;
    stlInt32              sLoop         = 0;

    qllNode * sNode = NULL;
    

    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefTableList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargets != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrderBySource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrderBy != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * 준비작업
     **********************************************************/

    sRefTableList = aRefTableList;

    /**
     * Instant Table의 Relation 이름 결정
     */

    STL_DASSERT( sRefTableList->mCount >= 1 );

    sTableNode = sRefTableList->mHead->mTableNode;

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
            {
                sRelationName = ((qlvInitInstantNode *) sTableNode)->mRelationName;
                break;
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }


    /**********************************************************
     * Order By Clause Validation
     **********************************************************/

    /**
     * Instant Table 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aStmtInfo->mDBCStmt),
                                STL_SIZEOF( qlvOrderBy ),
                                (void **) & sOrderBy,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sOrderBy, 0x00, STL_SIZEOF( qlvOrderBy ) );

    STL_TRY( qlnoInitInstant( & sOrderBy->mInstant,
                              QLV_NODE_TYPE_SORT_INSTANT,
                              sRelationName,
                              QLL_INIT_PLAN(aStmtInfo->mDBCStmt),
                              aEnv )
             == STL_SUCCESS );


    /**
     * Order By Expression Validation
     */

    if( sTableNode->mType == QLV_NODE_TYPE_QUERY_SET )
    {
        /**
         * Query Set을 Child Node로 하는 경우 record column list 구성 후 key column을 구성한다.
         */
    
        STL_DASSERT( sOrderBy->mInstant.mKeyColCnt == 0 );

        
        /**
         * Target Expression을 Sort Instant의 Record Column으로 설정
         */

        for( sLoop = 0 ; sLoop < aTargetCount ; sLoop++ )
        {
            sSortKeyExpr = aTargets[ sLoop ].mExpr;

            STL_TRY( qlnoAddRecColToInstantRestrict( aStmtInfo->mSQLString,
                                                     & sOrderBy->mInstant,
                                                     sSortKeyExpr,
                                                     & aTargets[ sLoop ].mExpr,
                                                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                     aEnv )
                     == STL_SUCCESS );
        }


        /**
         * Sort Key 분석
         */

        QLP_LIST_FOR_EACH( aOrderBySource, sListElem )
        {
            /**
             * Query Set 에 대한 ORDER BY 에는 column name 과 indicator 만 올 수 있다.
             */

            sParseOrderBy = (qlpOrderBy *) QLP_LIST_GET_POINTER_VALUE( sListElem );
            
            STL_DASSERT( sParseOrderBy->mSortKey->mType == QLL_VALUE_EXPR_TYPE );
            
            sNode = (qllNode *) ((qlpValueExpr *) sParseOrderBy->mSortKey)->mExpr;

            if ( sNode->mType == QLL_BNF_CONSTANT_TYPE )
            {
                sNode = (qllNode *) ((qlpConstantValue *) sNode)->mValue;
            }
            else
            {
                /* column 이어야 함 */
            }

            STL_TRY_THROW( (sNode->mType == QLL_BNF_NUMBER_CONSTANT_TYPE) ||
                           (sNode->mType == QLL_COLUMN_REF_TYPE),
                           RAMP_ERR_QUERY_SET_ORDER_BY_EXPRESSION );
            
            STL_TRY( qlvValidateValueExpr( aStmtInfo,
                                           QLV_EXPR_PHRASE_ORDERBY,
                                           aValidationObjList,
                                           sRefTableList,
                                           STL_FALSE,
                                           STL_FALSE,  /* Row Expr */
                                           0, /* aAllowedAggrDepth */
                                           sParseOrderBy->mSortKey,
                                           & sSortKeyExpr,
                                           NULL,
                                           KNL_BUILTIN_FUNC_INVALID,
                                           DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                           DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                           QLL_INIT_PLAN(aStmtInfo->mDBCStmt),
                                           aEnv )
                     == STL_SUCCESS );

            
            /**
             * Sort Key가 Index인 경우 해당 Index의 Target이 존재하는지 판별한다.
             */

            sNumber = 0;
            if( (sSortKeyExpr->mType == QLV_EXPR_TYPE_VALUE) &&
                ( (sSortKeyExpr->mExpr.mValue->mValueType == QLV_VALUE_TYPE_NUMERIC) ||
                  (sSortKeyExpr->mExpr.mValue->mValueType == QLV_VALUE_TYPE_BINARY_DOUBLE) ||
                  (sSortKeyExpr->mExpr.mValue->mValueType == QLV_VALUE_TYPE_BINARY_REAL) ) )
            {
                /**
                 * Number가 Target 범위에 속하는지 판별
                 */

                sNumString = sSortKeyExpr->mExpr.mValue->mData.mString;

                if( dtlGetIntegerFromStringWithoutRound( sNumString,
                                                         stlStrlen( sNumString ),
                                                         &sNumber,
                                                         QLL_ERROR_STACK(aEnv) )
                    == STL_SUCCESS )
                {
                    STL_TRY_THROW( ( (sNumber != 0) &&
                                     (sNumber <= sRefTableList->mHead->mTargetCount) ),
                                   RAMP_ERR_ORDER_BY_NUMBER_MISMATCH );

                    sSortKeyExpr = aTargets[ sNumber - 1 ].mExpr;
                }
                else
                {
                    /**
                     * Error Stack을 비운다.
                     */

                    while( stlPopError( QLL_ERROR_STACK(aEnv) ) != NULL )
                    {
                        /* Do Nothing */
                    }

                    STL_THROW( RAMP_ERR_ORDER_BY_NUMBER_MISMATCH );
                }
            }
            else
            {
                /**
                 * Rewrite Expression
                 */

                STL_TRY( qlvRewriteExpr( sSortKeyExpr,
                                         DTL_ITERATION_TIME_FOR_EACH_AGGREGATION,
                                         aStmtInfo->mQueryExprList,
                                         & sSortKeyExpr,
                                         QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                         aEnv )
                         == STL_SUCCESS );
            }

            
            /**
             * Set Sort Key Column Flag
             */
        
            /* Sort Key Flag 설정 */
            DTL_SET_INDEX_COLUMN_FLAG(
                sColFlag,
                ( sParseOrderBy->mSortOrder == NULL                   /* SORT ORDER */
                  ? DTL_KEYCOLUMN_INDEX_ORDER_ASC
                  : ( sParseOrderBy->mSortOrder->mValue.mInteger == STL_TRUE
                      ? DTL_KEYCOLUMN_INDEX_ORDER_ASC
                      : DTL_KEYCOLUMN_INDEX_ORDER_DESC ) ),           
                DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE,                    /* NULLABLE */
                ( sParseOrderBy->mNullOrder == NULL                   /* NULLS ORDER */ 
                  ? DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST
                  : ( sParseOrderBy->mNullOrder->mValue.mInteger == STL_TRUE
                      ? DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST
                      :DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST ) ) );


            /**
             * Set Operator 의 경우 상수도 Instant 에 저장해야 함.
             * Add Key Column To Sort Instant
             */
            
            STL_TRY( qlnoAddKeyColToInstantRestrict( aStmtInfo->mSQLString,
                                                     & sOrderBy->mInstant,
                                                     sSortKeyExpr,
                                                     sColFlag,
                                                     & sSortKeyExpr,
                                                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                     aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /**
         * key column list 구성 후 record column을 구성한다.
         */
        
        QLP_LIST_FOR_EACH( aOrderBySource, sListElem )
        {
            /**
             * Sort Key 분석
             */

            sParseOrderBy = (qlpOrderBy *) QLP_LIST_GET_POINTER_VALUE( sListElem );

            STL_TRY( qlvValidateValueExpr( aStmtInfo,
                                           QLV_EXPR_PHRASE_ORDERBY,
                                           aValidationObjList,
                                           sRefTableList,
                                           STL_FALSE,
                                           STL_FALSE,  /* Row Expr */
                                           aAllowedAggrDepth,
                                           sParseOrderBy->mSortKey,
                                           & sSortKeyExpr,
                                           NULL,
                                           KNL_BUILTIN_FUNC_INVALID,
                                           DTL_ITERATION_TIME_FOR_EACH_AGGREGATION,
                                           DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                           QLL_INIT_PLAN(aStmtInfo->mDBCStmt),
                                           aEnv )
                     == STL_SUCCESS );
            

            /**
             * Sort Key가 Index인 경우 해당 Index의 Target이 존재하는지 판별한다.
             */

            sNumber = 0;
            if( (sSortKeyExpr->mType == QLV_EXPR_TYPE_VALUE) &&
                ( (sSortKeyExpr->mExpr.mValue->mValueType == QLV_VALUE_TYPE_NUMERIC) ||
                  (sSortKeyExpr->mExpr.mValue->mValueType == QLV_VALUE_TYPE_BINARY_DOUBLE) ||
                  (sSortKeyExpr->mExpr.mValue->mValueType == QLV_VALUE_TYPE_BINARY_REAL) ) )
            {
                /**
                 * Number가 Target 범위에 속하는지 판별
                 */

                sNumString = sSortKeyExpr->mExpr.mValue->mData.mString;

                if( dtlGetIntegerFromStringWithoutRound( sNumString,
                                                         stlStrlen( sNumString ),
                                                         &sNumber,
                                                         QLL_ERROR_STACK(aEnv) )
                    == STL_SUCCESS )
                {
                    STL_TRY_THROW( ( (sNumber != 0) &&
                                     (sNumber <= sRefTableList->mHead->mTargetCount) ),
                                   RAMP_ERR_ORDER_BY_NUMBER_MISMATCH );

                    sSortKeyExpr = aTargets[ sNumber - 1 ].mExpr;
                }
                else
                {
                    /**
                     * Error Stack을 비운다.
                     */

                    while( stlPopError( QLL_ERROR_STACK(aEnv) ) != NULL )
                    {
                        /* Do Nothing */
                    }

                    STL_THROW( RAMP_ERR_ORDER_BY_NUMBER_MISMATCH );
                }
            }
            else
            {
                /**
                 * Rewrite Expression
                 */

                STL_TRY( qlvRewriteExpr( sSortKeyExpr,
                                         DTL_ITERATION_TIME_FOR_EACH_AGGREGATION,
                                         aStmtInfo->mQueryExprList,
                                         & sSortKeyExpr,
                                         QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                         aEnv )
                         == STL_SUCCESS );
            }
        

            /**
             * Set Sort Key Column Flag
             */
        
            /* Sort Key Flag 설정 */
            DTL_SET_INDEX_COLUMN_FLAG(
                sColFlag,
                ( sParseOrderBy->mSortOrder == NULL                   /* SORT ORDER */
                  ? DTL_KEYCOLUMN_INDEX_ORDER_ASC
                  : ( sParseOrderBy->mSortOrder->mValue.mInteger == STL_TRUE
                      ? DTL_KEYCOLUMN_INDEX_ORDER_ASC
                      : DTL_KEYCOLUMN_INDEX_ORDER_DESC ) ),           
                DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE,                    /* NULLABLE */
                ( sParseOrderBy->mNullOrder == NULL                   /* NULLS ORDER */ 
                  ? DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST
                  : ( sParseOrderBy->mNullOrder->mValue.mInteger == STL_TRUE
                      ? DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST
                      :DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST ) ) );     
        
            /**
             * Expression이 Constant Expression이면 INSTANT TABLE의 column으로 구성하지 않음
             */
            
            switch( sSortKeyExpr->mIterationTime )
            {
                case DTL_ITERATION_TIME_FOR_EACH_AGGREGATION :
                case DTL_ITERATION_TIME_FOR_EACH_EXPR :
                case DTL_ITERATION_TIME_FOR_EACH_QUERY :
                    {
                        /**
                         * Add Key Column To Sort Instant
                         */
                        
                        STL_TRY( qlnoAddKeyColToInstant( aStmtInfo->mSQLString,
                                                         & sOrderBy->mInstant,
                                                         sSortKeyExpr,
                                                         sColFlag,
                                                         & sSortKeyExpr,
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

        
        /**
         * Sort Key가 Constant Value 로만 이루어져 있는지 체크
         */
    
        if( sOrderBy->mInstant.mKeyColCnt == 0 )
        {
            sOrderBy = NULL;
        }
        else
        {
            /**
             * Target Expression을 Sort Instant의 Record Column으로 설정
             */

            for( sLoop = 0 ; sLoop < aTargetCount ; sLoop++ )
            {
                sSortKeyExpr = aTargets[ sLoop ].mExpr;

                STL_TRY( qlnoAddRecColToInstant( & sOrderBy->mInstant,
                                                 sSortKeyExpr,
                                                 & aTargets[ sLoop ].mExpr,
                                                 QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                 aEnv )
                         == STL_SUCCESS );
            }
        }
    }


    /**
     * OUTPUT 설정
     */

    *aOrderBy = sOrderBy;
    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_QUERY_SET_ORDER_BY_EXPRESSION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ORDER_BY_ITEM_MUST_BE_NUMBER_OF_SELECT_LIST_EXPRESSION,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sParseOrderBy->mSortKey->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_ORDER_BY_NUMBER_MISMATCH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ORDER_BY_NUMBER_MISMATCH,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sParseOrderBy->mSortKey->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnv */

