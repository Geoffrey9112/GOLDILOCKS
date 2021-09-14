/*******************************************************************************
 * qlncCBOptSubQuery.c
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
 * @file qlncCBOptSubQuery.c
 * @brief SQL Processor Layer cost based SubQuery optimization
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Cost Based Optimizer SubQuery Function List
 */
const qlncCostBasedOptimizerFuncPtr qlncCBOptSubQueryFuncList[ QLNC_NODE_TYPE_MAX ] =
{
    qlncCBOptSubQueryQuerySet,          /**< QLNC_NODE_TYPE_QUERY_SET */
    qlncCBOptSubQuerySetOP,             /**< QLNC_NODE_TYPE_SET_OP */
    qlncCBOptSubQueryQuerySpec,         /**< QLNC_NODE_TYPE_QUERY_SPEC */
    qlncCBOptSubQueryBaseTable,         /**< QLNC_NODE_TYPE_BASE_TABLE */
    qlncCBOptSubQuerySubTable,          /**< QLNC_NODE_TYPE_SUB_TABLE */
    qlncCBOptSubQueryJoinTable,         /**< QLNC_NODE_TYPE_JOIN_TABLE */
    NULL,                               /**< QLNC_NODE_TYPE_FLAT_INSTANT */
    qlncCBOptSubQuerySortInstant,       /**< QLNC_NODE_TYPE_SORT_INSTANT */
    qlncCBOptSubQueryHashInstant,       /**< QLNC_NODE_TYPE_HASH_INSTANT */
    
    NULL,                               /**< QLNC_NODE_TYPE_CREATE_NODE_MAX */
    
    NULL,                               /**< QLNC_NODE_TYPE_INNER_JOIN_TABLE */
    NULL,                               /**< QLNC_NODE_TYPE_OUTER_JOIN_TABLE */
    NULL,                               /**< QLNC_NODE_TYPE_SEMI_JOIN_TABLE */
    NULL                                /**< QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE */
};


/**
 * @brief Query Set Node에 존재하는 SubQuery의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptSubQueryQuerySet( qlncCBOptParamInfo * aCBOptParamInfo,
                                     qllEnv             * aEnv )
{
    qlncQuerySet    * sCandQuerySet     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_QUERY_SET,
                        QLL_ERROR_STACK(aEnv) );
 

    /**
     * Query Set Candidate Node 설정
     */

    sCandQuerySet = (qlncQuerySet*)(aCBOptParamInfo->mLeftNode);


    /**
     * Child Node SubQuery Optimizer
     */

    aCBOptParamInfo->mLeftNode = sCandQuerySet->mChildNode;
    STL_TRY( qlncCBOptSubQueryFuncList[ sCandQuerySet->mChildNode->mNodeType ]( aCBOptParamInfo,
                                                                                aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Set OP에 존재하는 SubQuery의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptSubQuerySetOP( qlncCBOptParamInfo    * aCBOptParamInfo,
                                  qllEnv                * aEnv )
{
    stlInt32      i;
    qlncSetOP   * sCandSetOP    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_SET_OP,
                        QLL_ERROR_STACK(aEnv) );
 

    /**
     * Set OP Candidate Node 설정
     */

    sCandSetOP = (qlncSetOP*)(aCBOptParamInfo->mLeftNode);


    /**
     * Child Node SubQuery Optimizer
     */

    for( i = 0; i < sCandSetOP->mChildCount; i++ )
    {
        aCBOptParamInfo->mLeftNode = sCandSetOP->mChildNodeArr[i];
        STL_TRY( qlncCBOptSubQueryFuncList[ sCandSetOP->mChildNodeArr[i]->mNodeType ]( aCBOptParamInfo,
                                                                                       aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Spec Node에 존재하는 SubQuery의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptSubQueryQuerySpec( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qllEnv                * aEnv )
{
    qlncQuerySpec       * sCandQuerySpec    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC,
                        QLL_ERROR_STACK(aEnv) );
 

    /**
     * Query Spec Candidate Node 설정
     */

    sCandQuerySpec = (qlncQuerySpec*)(aCBOptParamInfo->mLeftNode);


    /**
     * Child Node SubQuery Optimizer
     */

    aCBOptParamInfo->mLeftNode = sCandQuerySpec->mChildNode;
    STL_TRY( qlncCBOptSubQueryFuncList[ sCandQuerySpec->mChildNode->mNodeType ]( aCBOptParamInfo,
                                                                                 aEnv )
             == STL_SUCCESS );


    /**
     * Non-Filter SubQuery Expression List Optimizer 수행
     */

    if( sCandQuerySpec->mNonFilterSubQueryExprList != NULL )
    {
        STL_TRY( qlncCBOptSubQueryExprList( &(aCBOptParamInfo->mParamInfo),
                                            sCandQuerySpec->mNonFilterSubQueryExprList,
                                            aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Base Table Node에 존재하는 SubQuery의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptSubQueryBaseTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qllEnv                * aEnv )
{
    qlncBaseTableNode   * sCandBaseTable    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );
 

    /**
     * Base Table Candidate Node 설정
     */

    sCandBaseTable = (qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode);


    /**
     * And Filter내의 SubQuery Optimizer
     */

    if( sCandBaseTable->mFilter != NULL )
    {
        STL_TRY( qlncCBOptSubQueryAndFilter( &(aCBOptParamInfo->mParamInfo),
                                             sCandBaseTable->mFilter,
                                             aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Sub Table Node에 존재하는 SubQuery의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptSubQuerySubTable( qlncCBOptParamInfo     * aCBOptParamInfo,
                                     qllEnv                 * aEnv )
{
    qlncSubTableNode    * sCandSubTable     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE,
                        QLL_ERROR_STACK(aEnv) );
 

    /**
     * Sub Table Candidate Node 설정
     */

    sCandSubTable = (qlncSubTableNode*)(aCBOptParamInfo->mLeftNode);


    /**
     * Child Node SubQuery Optimizer
     */

    aCBOptParamInfo->mLeftNode = sCandSubTable->mQueryNode;
    STL_TRY( qlncCBOptSubQueryFuncList[ sCandSubTable->mQueryNode->mNodeType ]( aCBOptParamInfo,
                                                                                aEnv )
             == STL_SUCCESS );


    /**
     * And Filter내의 SubQuery Optimizer
     */

    if( sCandSubTable->mFilter != NULL )
    {
        STL_TRY( qlncCBOptSubQueryAndFilter( &(aCBOptParamInfo->mParamInfo),
                                             sCandSubTable->mFilter,
                                             aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Table Node에 존재하는 SubQuery의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptSubQueryJoinTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qllEnv                * aEnv )
{
    stlInt32              i;
    qlncJoinTableNode   * sCandJoinTable    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );
 

    /**
     * Join Table Candidate Node 설정
     */

    sCandJoinTable = (qlncJoinTableNode*)(aCBOptParamInfo->mLeftNode);


    /**
     * Child Node SubQuery Optimizer
     */

    for( i = 0; i < sCandJoinTable->mNodeArray.mCurCnt; i++ )
    {
        aCBOptParamInfo->mLeftNode = sCandJoinTable->mNodeArray.mNodeArr[i];
        STL_TRY( qlncCBOptSubQueryFuncList[ aCBOptParamInfo->mLeftNode->mNodeType ]( aCBOptParamInfo,
                                                                                     aEnv )
                 == STL_SUCCESS );
    }


    /**
     * And Filter내의 SubQuery Optimizer
     */

    if( sCandJoinTable->mJoinCond != NULL )
    {
        STL_TRY( qlncCBOptSubQueryAndFilter( &(aCBOptParamInfo->mParamInfo),
                                             sCandJoinTable->mJoinCond,
                                             aEnv )
                 == STL_SUCCESS );
    }

    if( sCandJoinTable->mFilter != NULL )
    {
        STL_TRY( qlncCBOptSubQueryAndFilter( &(aCBOptParamInfo->mParamInfo),
                                             sCandJoinTable->mFilter,
                                             aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


#if 0
/**
 * @brief Flat Instant Node에 존재하는 SubQuery의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptSubQueryFlatInstant( qlncCBOptParamInfo  * aCBOptParamInfo,
                                        qllEnv              * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_FLAT_INSTANT,
                        QLL_ERROR_STACK(aEnv) );


    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlncCBOptSubQueryFlatInstant()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}
#endif


/**
 * @brief Sort Instant Node에 존재하는 SubQuery의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptSubQuerySortInstant( qlncCBOptParamInfo  * aCBOptParamInfo,
                                        qllEnv              * aEnv )
{
    qlncSortInstantNode * sCandSortInstant  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT,
                        QLL_ERROR_STACK(aEnv) );
 

    /**
     * Sort Instant Candidate Node 설정
     */

    sCandSortInstant = (qlncSortInstantNode*)(aCBOptParamInfo->mLeftNode);


    /**
     * Child Node SubQuery Optimizer
     */

    aCBOptParamInfo->mLeftNode = sCandSortInstant->mChildNode;
    STL_TRY( qlncCBOptSubQueryFuncList[ sCandSortInstant->mChildNode->mNodeType ]( aCBOptParamInfo,
                                                                                   aEnv )
             == STL_SUCCESS );


    /**
     * Non-Filter SubQuery Expression List Optimizer 수행
     */

    if( sCandSortInstant->mNonFilterSubQueryExprList != NULL )
    {
        STL_TRY( qlncCBOptSubQueryExprList( &(aCBOptParamInfo->mParamInfo),
                                            sCandSortInstant->mNonFilterSubQueryExprList,
                                            aEnv )
                 == STL_SUCCESS );
    }


    /**
     * And Filter내의 SubQuery Optimizer
     */

    if( sCandSortInstant->mKeyFilter != NULL )
    {
        /* @todo 현재 Sort Instant에 Filter를 기술할 수 있는 경우가 없으나
         * 복잡한 질의의 경우 Optimizer에 의해 발생 가능할 것으로 판단됨 */

        STL_TRY( qlncCBOptSubQueryAndFilter( &(aCBOptParamInfo->mParamInfo),
                                             sCandSortInstant->mKeyFilter,
                                             aEnv )
                 == STL_SUCCESS );
    }

    if( sCandSortInstant->mRecFilter != NULL )
    {
        /* @todo 현재 Sort Instant에 Filter를 기술할 수 있는 경우가 없으나
         * 복잡한 질의의 경우 Optimizer에 의해 발생 가능할 것으로 판단됨 */

        STL_TRY( qlncCBOptSubQueryAndFilter( &(aCBOptParamInfo->mParamInfo),
                                             sCandSortInstant->mRecFilter,
                                             aEnv )
                 == STL_SUCCESS );
    }

    if( sCandSortInstant->mFilter != NULL )
    {
        STL_TRY( qlncCBOptSubQueryAndFilter( &(aCBOptParamInfo->mParamInfo),
                                             sCandSortInstant->mFilter,
                                             aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Instant Node에 존재하는 SubQuery의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptSubQueryHashInstant( qlncCBOptParamInfo  * aCBOptParamInfo,
                                        qllEnv              * aEnv )
{
    qlncHashInstantNode * sCandHashInstant  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT,
                        QLL_ERROR_STACK(aEnv) );
 

    /**
     * Hash Instant Candidate Node 설정
     */

    sCandHashInstant = (qlncHashInstantNode*)(aCBOptParamInfo->mLeftNode);


    /**
     * Child Node SubQuery Optimizer
     */

    aCBOptParamInfo->mLeftNode = sCandHashInstant->mChildNode;
    STL_TRY( qlncCBOptSubQueryFuncList[ sCandHashInstant->mChildNode->mNodeType ]( aCBOptParamInfo,
                                                                                   aEnv )
             == STL_SUCCESS );


    /**
     * Non-Filter SubQuery Expression List Optimizer 수행
     */

    if( sCandHashInstant->mNonFilterSubQueryExprList != NULL )
    {
        STL_TRY( qlncCBOptSubQueryExprList( &(aCBOptParamInfo->mParamInfo),
                                            sCandHashInstant->mNonFilterSubQueryExprList,
                                            aEnv )
                 == STL_SUCCESS );
    }


    /**
     * And Filter내의 SubQuery Optimizer
     */

    if( sCandHashInstant->mKeyFilter != NULL )
    {
        /* @todo 현재 Hash Instant의 Key에 Filter를 기술하는 경우
         * 하위 노드로 Push되어 여기에 들어올 수 없을 것 같으나
         * 복잡한 질의의 경우 Optimizer에 의해 발생 가능할 것으로 판단됨 */

        STL_TRY( qlncCBOptSubQueryAndFilter( &(aCBOptParamInfo->mParamInfo),
                                             sCandHashInstant->mKeyFilter,
                                             aEnv )
                 == STL_SUCCESS );
    }

    if( sCandHashInstant->mRecFilter != NULL )
    {
        STL_TRY( qlncCBOptSubQueryAndFilter( &(aCBOptParamInfo->mParamInfo),
                                             sCandHashInstant->mRecFilter,
                                             aEnv )
                 == STL_SUCCESS );
    }

    if( sCandHashInstant->mFilter != NULL )
    {
        STL_TRY( qlncCBOptSubQueryAndFilter( &(aCBOptParamInfo->mParamInfo),
                                             sCandHashInstant->mFilter,
                                             aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;

}




/////////////////////////////////////////////////////////////////////////


/**
 * @brief SubQuery Expression List에 대한 Optimizer를 수행한다.
 * @param[in]   aParamInfo          Parameter Info
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncCBOptSubQueryExprList( qlncParamInfo      * aParamInfo,
                                     qlncRefExprList    * aSubQueryExprList,
                                     qllEnv             * aEnv )
{
    qlncRefExprItem     * sRefExprItem      = NULL;
    qlncCBOptParamInfo    sCBOptParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );


    sRefExprItem = aSubQueryExprList->mHead;
    while( sRefExprItem != NULL )
    {
        STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_SUB_QUERY );

        /* SubQuery의 Node에 대한 SubQuery Optimizer 수행 */
        stlMemset( &sCBOptParamInfo, 0x00, STL_SIZEOF( qlncCBOptParamInfo ) );
        sCBOptParamInfo.mParamInfo = *aParamInfo;
        sCBOptParamInfo.mLeftNode = ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode;

        STL_TRY( qlncCBOptSubQueryFuncList[sCBOptParamInfo.mLeftNode->mNodeType]( &sCBOptParamInfo,
                                                                                   aEnv )
                 == STL_SUCCESS );

        /* SubQuery의 Node에 대한 Cost Based Optimizer 수행 */
        stlMemset( &sCBOptParamInfo, 0x00, STL_SIZEOF( qlncCBOptParamInfo ) );
        sCBOptParamInfo.mParamInfo = *aParamInfo;
        sCBOptParamInfo.mLeftNode = ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode;

        STL_TRY( qlncCostBasedOptimizerFuncList[ sCBOptParamInfo.mLeftNode->mNodeType ]( &sCBOptParamInfo,
                                                                                         aEnv )
                 == STL_SUCCESS );

        /* SubQuery의 Node에 대한 Post Optimizer 수행 */
        STL_TRY( qlncPostOptimizer( aParamInfo,
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
 * @brief Anbd Filter SubQuery Expression에 대한 Optimizer를 수행한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aAndFilter  And Filter
 * @param[in]   aEnv        Environment
 */
stlStatus qlncCBOptSubQueryAndFilter( qlncParamInfo     * aParamInfo,
                                      qlncAndFilter     * aAndFilter,
                                      qllEnv            * aEnv )
{
    stlInt32              i;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter != NULL, QLL_ERROR_STACK(aEnv) );


    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        if( aAndFilter->mOrFilters[i]->mSubQueryExprList != NULL )
        {
            STL_TRY( qlncCBOptSubQueryExprList( aParamInfo,
                                                aAndFilter->mOrFilters[i]->mSubQueryExprList,
                                                aEnv )
                     == STL_SUCCESS );
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */
