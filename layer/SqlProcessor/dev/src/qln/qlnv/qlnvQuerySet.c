/*******************************************************************************
 * qlnvQuerySet.c
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
 * @file qlnvQuerySet.c
 * @brief SQL Processor Layer Validation - Query Node Functions
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnv
 * @{
 */



/**
 * @brief SET EXPRESSION 구문을 Validation 한다.
 * @param[in]       aStmtInfo           Stmt Information
 * @param[in]       aCalledFromSubQuery Subquery로부터 이 함수가 호출되었는지 여부
 * @param[in,out]   aValidationObjList  Validation Object List
 * @param[in]       aRefTableList       Reference Table List
 * @param[in]       aParseQuerySetNode  Set Expression Parse Tree
 * @param[out]      aQueryNode          Query Node
 * @param[in]       aEnv                Environment
 */
stlStatus qlvValidateQuerySetNode( qlvStmtInfo          * aStmtInfo,
                                   stlBool                aCalledFromSubQuery,
                                   ellObjectList        * aValidationObjList,
                                   qlvRefTableList      * aRefTableList,
                                   qlpQuerySetNode      * aParseQuerySetNode,
                                   qlvInitNode         ** aQueryNode,
                                   qllEnv               * aEnv )
{
    qlvInitQuerySetNode * sQuerySetNode     = NULL;
    
    qllNode              * sLeftQueryNode = NULL;
    qlpQuerySpecNode     * sLeftMostParseSpec = NULL;
    qlvInitNode          * sLeftMostInitNode = NULL;
    qlvInitQuerySpecNode * sLeftMostInitSpec = NULL;

    stlInt32           sSetColumnCount = 0;
    qlvInitTarget    * sSetColumns = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseQuerySetNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseQuerySetNode->mType == QLL_QUERY_SET_NODE_TYPE),
                        QLL_ERROR_STACK(aEnv) );

    /****************************************************************
     * Left-most Query Spec Validation
     ****************************************************************/
     
    /**
     * Left-most Query Spec 를 찾음
     */

    sLeftQueryNode = aParseQuerySetNode->mLeftQueryNode;

    while( sLeftQueryNode->mType == QLL_SUB_TABLE_NODE_TYPE )
    {
        sLeftQueryNode = ((qlpSubTableNode *) sLeftQueryNode)->mQueryNode;
    }

    while ( sLeftQueryNode->mType == QLL_QUERY_SET_NODE_TYPE )
    {
        sLeftQueryNode = ((qlpQuerySetNode *) sLeftQueryNode)->mLeftQueryNode;

        while( sLeftQueryNode->mType == QLL_SUB_TABLE_NODE_TYPE )
        {
            sLeftQueryNode = ((qlpSubTableNode *) sLeftQueryNode)->mQueryNode;
        }
    }

    sLeftMostParseSpec = (qlpQuerySpecNode*) sLeftQueryNode;
    
    /**
     * Left-most Query Spec Validation
     */
    
    STL_TRY( qlvValidateQuerySpecNode( aStmtInfo,
                                       aCalledFromSubQuery,
                                       aValidationObjList,
                                       aRefTableList,
                                       sLeftMostParseSpec,
                                       & sLeftMostInitNode,
                                       aEnv )
             == STL_SUCCESS );

    sLeftMostInitSpec = (qlvInitQuerySpecNode *) sLeftMostInitNode;

    /****************************************************************
     * Query Set 을 Recursive Validation
     ****************************************************************/

    /**
     * Query Set Node 생성 및 초기화
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF( qlvInitQuerySetNode ),
                                (void **) & sQuerySetNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sQuerySetNode, 0x00, STL_SIZEOF( qlvInitQuerySetNode ) );
    sQuerySetNode->mType = QLV_NODE_TYPE_QUERY_SET;


    /**
     * Query Block Name 기본값 및 Query Block Identifier 설정
     */

    stlSnprintf( sQuerySetNode->mQBName,
                 QLV_QUERY_BLOCK_NAME_MAX_SIZE - 1,
                 "%s%d",
                 aStmtInfo->mQBPrefix,
                 *(aStmtInfo->mQBIndex) );

    sQuerySetNode->mQBID = *(aStmtInfo->mQBIndex);

    (*(aStmtInfo->mQBIndex))++;


    /**
     * Query Set Relation 의 Column 정보 구축
     */

    STL_TRY( qlvMakeSetRelationColumns( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                        sQuerySetNode,
                                        sLeftMostInitSpec,
                                        & sSetColumnCount,
                                        & sSetColumns,
                                        aEnv )
             == STL_SUCCESS );

    /**
     * Query Set Validation
     */
    
    STL_TRY( qlvValidateQuerySetInfo( aStmtInfo,
                                      aCalledFromSubQuery,
                                      aValidationObjList,
                                      aRefTableList,
                                      STL_TRUE,  /* aIsRootSet */
                                      STL_TRUE,  /* aIsLeftSet */
                                      sLeftMostInitNode,
                                      sSetColumnCount,
                                      sSetColumns,
                                      aParseQuerySetNode,
                                      sQuerySetNode,
                                      aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aQueryNode = (qlvInitNode *) sQuerySetNode;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Set 정보를 Validation 한다.
 * @param[in]       aStmtInfo           Stmt Information
 * @param[in]       aCalledFromSubQuery Subquery로부터 이 함수가 호출되었는지 여부
 * @param[in,out]   aValidationObjList  Validation Object List
 * @param[in]       aRefTableList       Reference Table List
 * @param[in]       aIsRootSet          Root Set 여부
 * @param[in]       aIsLeftSet          Left Set 여부
 * @param[in]       aLeftMostQuerySpec  Left-Most Query Spec Node
 * @param[in]       aSetColumnCount     Set Relation 의 Column Count
 * @param[in]       aSetColumns         Set Relation 의 Column List
 * @param[in]       aParseQuerySetNode  Set Expression Parse Tree
 * @param[in,out]   aQuerySetNode       Query Set Node
 * @param[in]       aEnv                Environment
 */
stlStatus qlvValidateQuerySetInfo( qlvStmtInfo          * aStmtInfo,
                                   stlBool                aCalledFromSubQuery,
                                   ellObjectList        * aValidationObjList,
                                   qlvRefTableList      * aRefTableList,
                                   stlBool                aIsRootSet,
                                   stlBool                aIsLeftSet,
                                   qlvInitNode          * aLeftMostQuerySpec,
                                   stlInt32               aSetColumnCount,
                                   qlvInitTarget        * aSetColumns,
                                   qlpQuerySetNode      * aParseQuerySetNode,
                                   qlvInitQuerySetNode  * aQuerySetNode,
                                   qllEnv               * aEnv )
{
    qlvInitQuerySetNode  * sLeftSetNode = NULL;
    qlvInitQuerySetNode  * sRightSetNode = NULL;

    qlpQuerySpecNode     * sChildParseSpec = NULL;
    qlvInitQuerySpecNode * sChildInitSpec  = NULL;

    qlvRefTableList      * sRefTableList = NULL;

    qllNode              * sLeftQueryNode = NULL;
    qllNode              * sRightQueryNode = NULL;

    qlvInitTarget        * sLeftTargets = NULL;
    qlvInitTarget        * sRightTargets = NULL;

    stlInt32  i = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseQuerySetNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLeftMostQuerySpec != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSetColumnCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSetColumns != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseQuerySetNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseQuerySetNode->mType == QLL_QUERY_SET_NODE_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQuerySetNode != NULL, QLL_ERROR_STACK(aEnv) );


    /***************************************************
     * 기본 정보 설정
     ***************************************************/
    
    /**
     * Query Set 의 Root 및 Pure 여부 
     */
    
    aQuerySetNode->mIsRootSet = aIsRootSet;

    if ( (aParseQuerySetNode->mOrderBy == NULL) &&
         (aParseQuerySetNode->mResultSkip == NULL) &&
         (aParseQuerySetNode->mResultLimit == NULL) )
    {
        aQuerySetNode->mIsPureSet = STL_TRUE;
    }
    else
    {
        aQuerySetNode->mIsPureSet = STL_FALSE;
    }

    /**
     * Set Type
     */

    switch( aParseQuerySetNode->mSetType )
    {
        case QLP_SET_TYPE_UNION:
            aQuerySetNode->mSetType = QLV_SET_TYPE_UNION;
            break;
        case QLP_SET_TYPE_EXCEPT:
            aQuerySetNode->mSetType = QLV_SET_TYPE_EXCEPT;
            break;
        case QLP_SET_TYPE_INTERSECT:
            aQuerySetNode->mSetType = QLV_SET_TYPE_INTERSECT;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }


    /**
     * Set Option
     */

    switch( aParseQuerySetNode->mSetQuantifier )
    {
        case QLP_SET_QUANTIFIER_ALL:
            aQuerySetNode->mSetOption = QLV_SET_OPTION_ALL;
            break;
        case QLP_SET_QUANTIFIER_DISTINCT:
            aQuerySetNode->mSetOption = QLV_SET_OPTION_DISTINCT;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    
    /***************************************************
     * With 절 Validation
     ***************************************************/

    /**
     * @todo With Clause
     */


    /***************************************************
     * Child Validation
     ***************************************************/

    /**
     * Left Query Node Validation
     */

    sLeftQueryNode = aParseQuerySetNode->mLeftQueryNode;

    while( sLeftQueryNode->mType == QLL_SUB_TABLE_NODE_TYPE )
    {
        sLeftQueryNode = ((qlpSubTableNode *) sLeftQueryNode)->mQueryNode;
    }
        
    if ( sLeftQueryNode->mType == QLL_QUERY_SET_NODE_TYPE )
    {
        /**
         * Left Query Set Node 생성 및 초기화
         */

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF( qlvInitQuerySetNode ),
                                    (void **) & sLeftSetNode,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( sLeftSetNode, 0x00, STL_SIZEOF( qlvInitQuerySetNode ) );
        sLeftSetNode->mType = QLV_NODE_TYPE_QUERY_SET;
        
        STL_TRY( qlvValidateQuerySetInfo( aStmtInfo,
                                          aCalledFromSubQuery,
                                          aValidationObjList,
                                          aRefTableList,
                                          STL_FALSE,  /* aIsRootSet */
                                          aIsLeftSet,  /* aIsLeftSet */
                                          aLeftMostQuerySpec,
                                          aSetColumnCount,
                                          aSetColumns,
                                          (qlpQuerySetNode *) sLeftQueryNode,
                                          sLeftSetNode,
                                          aEnv )
                 == STL_SUCCESS );

        aQuerySetNode->mLeftQueryNode = (qlvInitNode *) sLeftSetNode;
    }
    else
    {
        if ( aIsLeftSet == STL_TRUE )
        {
            aQuerySetNode->mLeftQueryNode = aLeftMostQuerySpec;
        }
        else
        {
            STL_TRY( qlvValidateQuerySpecNode( aStmtInfo,
                                               aCalledFromSubQuery,
                                               aValidationObjList,
                                               aRefTableList,
                                               (qlpQuerySpecNode*) sLeftQueryNode,
                                               & aQuerySetNode->mLeftQueryNode,
                                               aEnv )
                     == STL_SUCCESS );
        }
        
        sLeftSetNode = NULL;

        sChildParseSpec = (qlpQuerySpecNode*) sLeftQueryNode;
        sChildInitSpec  = (qlvInitQuerySpecNode *) aQuerySetNode->mLeftQueryNode;
        STL_TRY_THROW( aSetColumnCount == sChildInitSpec->mTargetCount,  RAMP_ERR_INCORRECT_RESULT_COLUMN_COUNT );

        
        /**
         * Sequence 사용 체크
         */
        
        STL_TRY_THROW( sChildInitSpec->mQueryExprList->mPseudoColExprList->mCount == 0,
                       RAMP_ERR_PSEUDO_COLUMN_NOT_ALLOWED );
    }

    
    /**
     * Right Query Node Validation
     */
    
    sRightQueryNode = aParseQuerySetNode->mRightQueryNode;

    while( sRightQueryNode->mType == QLL_SUB_TABLE_NODE_TYPE )
    {
        sRightQueryNode = ((qlpSubTableNode *) sRightQueryNode)->mQueryNode;
    }

    if ( sRightQueryNode->mType == QLL_QUERY_SET_NODE_TYPE )
    {
        /**
         * Right Query Set Node 생성 및 초기화
         */

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF( qlvInitQuerySetNode ),
                                    (void **) & sRightSetNode,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( sRightSetNode, 0x00, STL_SIZEOF( qlvInitQuerySetNode ) );
        sRightSetNode->mType = QLV_NODE_TYPE_QUERY_SET;
        
        STL_TRY( qlvValidateQuerySetInfo( aStmtInfo,
                                          aCalledFromSubQuery,
                                          aValidationObjList,
                                          aRefTableList,
                                          STL_FALSE,  /* aIsRootSet */
                                          STL_FALSE,  /* aIsLeftSet */
                                          aLeftMostQuerySpec,
                                          aSetColumnCount,
                                          aSetColumns,
                                          (qlpQuerySetNode *) sRightQueryNode,
                                          sRightSetNode,
                                          aEnv )
                 == STL_SUCCESS );

        aQuerySetNode->mRightQueryNode = (qlvInitNode *) sRightSetNode;
    }
    else
    {
        STL_TRY( qlvValidateQuerySpecNode( aStmtInfo,
                                           aCalledFromSubQuery,
                                           aValidationObjList,
                                           aRefTableList,
                                           (qlpQuerySpecNode*) sRightQueryNode,
                                           & aQuerySetNode->mRightQueryNode,
                                           aEnv )
                 == STL_SUCCESS );

        sRightSetNode = NULL;
        
        sChildParseSpec = (qlpQuerySpecNode*) sRightQueryNode;
        sChildInitSpec  = (qlvInitQuerySpecNode *) aQuerySetNode->mRightQueryNode;
        STL_TRY_THROW( aSetColumnCount == sChildInitSpec->mTargetCount,  RAMP_ERR_INCORRECT_RESULT_COLUMN_COUNT );

        /**
         * Sequence 사용 체크
         */

        STL_TRY_THROW( sChildInitSpec->mQueryExprList->mPseudoColExprList->mCount == 0,
                       RAMP_ERR_PSEUDO_COLUMN_NOT_ALLOWED );
    }

    /***************************************************
     * Multiple Set 연산자 정보 설정 
     ***************************************************/

    aQuerySetNode->mSameSetChildCnt = 0;
    
    if ( (sLeftSetNode == NULL) && (sRightSetNode == NULL) )
    {
        /* child 가 모두 query spec 인 경우 */
        aQuerySetNode->mSameSetChildCnt = 2;
    }
    else
    {
        /**
         * Left Child
         */
        
        if ( (sLeftSetNode != NULL) && (sLeftSetNode->mIsPureSet == STL_TRUE) )
        {
            /* left child 가 set 연산자인 경우 */
            
            if ( aQuerySetNode->mSetType == sLeftSetNode->mSetType )
            {
                /* 동일한 set 연산자인 경우 */

                if ( aQuerySetNode->mSetType == QLV_SET_TYPE_EXCEPT )
                {
                    /* EXCEPT 인 경우 */
                    
                    if ( aQuerySetNode->mSetOption == sLeftSetNode->mSetOption )
                    {
                        /* 모두 ALL 이거나 DISTINCT 여야 함. */
                        aQuerySetNode->mSameSetChildCnt = sLeftSetNode->mSameSetChildCnt;
                    }
                    else
                    {
                        aQuerySetNode->mSameSetChildCnt = 1;
                    }
                }
                else
                {
                    if ( aQuerySetNode->mSetOption == QLV_SET_OPTION_DISTINCT )
                    {
                        aQuerySetNode->mSameSetChildCnt = sLeftSetNode->mSameSetChildCnt;
                    }
                    else
                    {
                        if ( sLeftSetNode->mSetOption == QLV_SET_OPTION_ALL )
                        {
                            aQuerySetNode->mSameSetChildCnt = sLeftSetNode->mSameSetChildCnt;
                        }
                        else
                        {
                            aQuerySetNode->mSameSetChildCnt = 1;
                        }
                    }
                }
            }
            else
            {
                aQuerySetNode->mSameSetChildCnt = 1;
            }
        }
        else
        {
            aQuerySetNode->mSameSetChildCnt = 1;
        }

        /**
         * Right Child
         */
        
        if ( (sRightSetNode != NULL) && (sRightSetNode->mIsPureSet == STL_TRUE) )
        {
            /* right child 가 set 연산자인 경우 */
            
            if ( aQuerySetNode->mSetType == QLV_SET_TYPE_EXCEPT )
            {
                /* EXCEPT 는 left->right 순서를 변경할 수 없음 */
                aQuerySetNode->mSameSetChildCnt++;
            }
            else
            {
                if ( aQuerySetNode->mSetType == sRightSetNode->mSetType )
                {
                    /* 동일한 set 연산자인 경우 */
                    
                    if ( aQuerySetNode->mSetOption == QLV_SET_OPTION_DISTINCT )
                    {
                        aQuerySetNode->mSameSetChildCnt += sRightSetNode->mSameSetChildCnt;
                    }
                    else
                    {
                        if ( sRightSetNode->mSetOption == QLV_SET_OPTION_ALL )
                        {
                            aQuerySetNode->mSameSetChildCnt += sRightSetNode->mSameSetChildCnt;
                        }
                        else
                        {
                            aQuerySetNode->mSameSetChildCnt++;
                        }
                    }
                }
                else
                {
                    aQuerySetNode->mSameSetChildCnt++;
                }
            }
        }
        else
        {
            aQuerySetNode->mSameSetChildCnt++;
        }
    }
    
    /***************************************************
     * Set Target 설정 
     ***************************************************/

    /**
     * @todo corresponding specification
     */

    
    /**
     * Data Type Info 설정
     */

    switch( aQuerySetNode->mLeftQueryNode->mType )
    {
        case QLV_NODE_TYPE_QUERY_SET :
            sLeftTargets = ((qlvInitQuerySetNode *)aQuerySetNode->mLeftQueryNode)->mSetTargets;
            break;
        case QLV_NODE_TYPE_QUERY_SPEC :
            sLeftTargets = ((qlvInitQuerySpecNode *)aQuerySetNode->mLeftQueryNode)->mTargets;
            break;
        default :
            STL_DASSERT( 0 );
            break;
    }

    switch( aQuerySetNode->mRightQueryNode->mType )
    {
        case QLV_NODE_TYPE_QUERY_SET :
            sRightTargets = ((qlvInitQuerySetNode *)aQuerySetNode->mRightQueryNode)->mSetTargets;
            break;
        case QLV_NODE_TYPE_QUERY_SPEC :
            sRightTargets = ((qlvInitQuerySpecNode *)aQuerySetNode->mRightQueryNode)->mTargets;
            break;
        default :
            STL_DASSERT( 0 );
            break;
    }

    
    /**
     * Set Column Result Type Combination
     */

    for( i = 0; i < aSetColumnCount; i++ )
    {
        sLeftTargets[i].mDataTypeInfo.mIsBaseColumn = STL_FALSE;
        sLeftTargets[i].mDataTypeInfo.mUpdatable    = STL_FALSE;
        
        sRightTargets[i].mDataTypeInfo.mIsBaseColumn = STL_FALSE;
        sRightTargets[i].mDataTypeInfo.mUpdatable    = STL_FALSE;
        
        if( sLeftTargets[i].mDataTypeInfo.mIsIgnore == STL_TRUE )
        {
            if( sRightTargets[i].mDataTypeInfo.mIsIgnore == STL_TRUE )
            {
                aSetColumns[i].mDataTypeInfo = sLeftTargets[i].mDataTypeInfo;
            }
            else
            {
                aSetColumns[i].mDataTypeInfo = sRightTargets[i].mDataTypeInfo;
            }
        }
        else
        {
            if( sRightTargets[i].mDataTypeInfo.mIsIgnore == STL_TRUE )
            {
                aSetColumns[i].mDataTypeInfo = sLeftTargets[i].mDataTypeInfo;
            }
            else
            {
                STL_TRY( qlvGetResultTypeCombiantion( aStmtInfo->mSQLString,
                                                      NULL,
                                                      aSetColumns[i].mExpr->mPosition,
                                                      STL_FALSE,
                                                      & sLeftTargets[i].mDataTypeInfo,
                                                      & sRightTargets[i].mDataTypeInfo,
                                                      & aSetColumns[i].mDataTypeInfo,
                                                      aEnv )
                         == STL_SUCCESS );
            }
        }
    }

    
    /**
     * Set Target 설정
     */

    aQuerySetNode->mSetTargetCount = aSetColumnCount;
    aQuerySetNode->mSetColumns     = aSetColumns;

    STL_TRY( qlvMakeSetTargets( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                aIsRootSet,
                                aQuerySetNode,
                                aQuerySetNode->mSetTargetCount,
                                aQuerySetNode->mSetColumns,
                                & aQuerySetNode->mSetTargets,
                                aEnv )
             == STL_SUCCESS );

    
    /***************************************************
     * Order By 절 Validation
     ***************************************************/

    /**
     * Order By Clause
     * ----------------------------------------------
     * Query Set에서 Order By에 올 수 있는 필드들은
     * 위에서 설정한 Set Target에 오는 필드들만 가능하다.
     */

    /**
     * qlvOrderByList 생성
     */


    if( aParseQuerySetNode->mOrderBy == NULL )
    {
        aQuerySetNode->mOrderBy = NULL;
    }
    else
    {
        /**
         * Query Set 의 ORDER BY 는 Query Spec 의 ORDER BY 와 다른 특징을 갖는다.
         */
        
        /**
         * Query Set Node에서 온 경우 Reference Table List가 NULL이다.
         * 이 경우 Value Expr Validation을 위하여
         * 임시 Reference Table List를 만든다.
         */

        STL_TRY( qlvCreateRefTableList( &QLL_VALIDATE_MEM(aEnv),
                                        NULL,
                                        &sRefTableList,
                                        aEnv )
                 == STL_SUCCESS );

        sRefTableList->mHead->mTableNode = (qlvInitNode*)aQuerySetNode;

        /**
         * Target 정보를 Reference Table List에 추가
         */

        STL_TRY( qlvSetTargetToRefTableItem( sRefTableList,
                                             aQuerySetNode->mSetTargetCount,
                                             aQuerySetNode->mSetTargets,
                                             aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlvValidateOrderBy( aStmtInfo,
                                     aValidationObjList,
                                     sRefTableList,
                                     aQuerySetNode->mSetTargetCount,
                                     aQuerySetNode->mSetTargets,
                                     0, /* Allowed Aggregation Depth */
                                     aParseQuerySetNode->mOrderBy,
                                     & aQuerySetNode->mOrderBy,
                                     aEnv )
                 == STL_SUCCESS );

        /**
         * Read Column 정보를 Sort Instant Table 에 추가
         */

        for ( i = 0; i < aQuerySetNode->mSetTargetCount; i++ )
        {
            STL_TRY( qlnoAddReadColToInstantRestrict( & aQuerySetNode->mOrderBy->mInstant,
                                                      aQuerySetNode->mSetTargets[i].mExpr,
                                                      & aQuerySetNode->mSetTargets[i].mExpr,
                                                      QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                      aEnv )
                     == STL_SUCCESS );
        }
    }


    /**********************************************************
     * 모든 Instant Node에 대하여 Inner Column의 Idx 설정
     **********************************************************/

    /* Order By Instant Node */
    if( aQuerySetNode->mOrderBy != NULL )
    {
        STL_TRY( qlnoSetIdxInstant( &(aQuerySetNode->mOrderBy->mInstant),
                                    aEnv )
                 == STL_SUCCESS );
    }


    /**********************************************************
     * OFFSET .. LIMIT .. validation
     **********************************************************/

    /**
     * OFFSET : Result Skip
     */

    if( aParseQuerySetNode->mResultSkip == NULL )
    {
        aQuerySetNode->mResultSkip = NULL;
    }
    else
    {
        STL_TRY( qlvValidateValueExpr( aStmtInfo,
                                       QLV_EXPR_PHRASE_OFFSETLIMIT,
                                       aValidationObjList,
                                       aRefTableList,
                                       STL_FALSE,
                                       STL_FALSE,  /* Row Expr */
                                       0 /* Allowed Aggregation Depth */,
                                       aParseQuerySetNode->mResultSkip,
                                       & aQuerySetNode->mResultSkip,
                                       NULL,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       DTL_ITERATION_TIME_FOR_EACH_QUERY,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW ( qlvIsValidSkipLimit( aQuerySetNode->mResultSkip, aEnv ) == STL_TRUE,
                        RAMP_ERR_INVALID_RESULT_SKIP );
        
        if( aQuerySetNode->mResultSkip->mType == QLV_EXPR_TYPE_VALUE )
        {
            STL_TRY( dtlGetIntegerFromString(
                         aQuerySetNode->mResultSkip->mExpr.mValue->mData.mString,
                         stlStrlen( aQuerySetNode->mResultSkip->mExpr.mValue->mData.mString ),
                         & aQuerySetNode->mSkipCnt,
                         QLL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            STL_TRY_THROW( aQuerySetNode->mSkipCnt >= 0, RAMP_ERR_INVALID_RESULT_SKIP );
        }
        else
        {
            STL_DASSERT( aQuerySetNode->mResultSkip->mType == QLV_EXPR_TYPE_BIND_PARAM );
            aQuerySetNode->mSkipCnt = 0;
        }
    }

    /**
     * LIMIT : Result Limit
     */

    if( aParseQuerySetNode->mResultLimit == NULL )
    {
        aQuerySetNode->mResultLimit = NULL;
    }
    else
    {
        STL_TRY( qlvValidateValueExpr( aStmtInfo,
                                       QLV_EXPR_PHRASE_OFFSETLIMIT,
                                       aValidationObjList,
                                       aRefTableList,
                                       STL_FALSE,
                                       STL_FALSE,  /* Row Expr */
                                       0 /* Allowed Aggregation Depth */,
                                       aParseQuerySetNode->mResultLimit,
                                       & aQuerySetNode->mResultLimit,
                                       NULL,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       DTL_ITERATION_TIME_FOR_EACH_QUERY,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW ( qlvIsValidSkipLimit( aQuerySetNode->mResultLimit, aEnv ) == STL_TRUE,
                        RAMP_ERR_INVALID_RESULT_LIMIT );

        if( aQuerySetNode->mResultLimit->mType == QLV_EXPR_TYPE_VALUE )
        {
            STL_TRY( dtlGetIntegerFromString(
                         aQuerySetNode->mResultLimit->mExpr.mValue->mData.mString,
                         stlStrlen( aQuerySetNode->mResultLimit->mExpr.mValue->mData.mString ),
                         & aQuerySetNode->mFetchCnt,
                         QLL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );

            STL_TRY_THROW( aQuerySetNode->mFetchCnt > 0, RAMP_ERR_INVALID_RESULT_LIMIT );
        }
        else
        {
            STL_DASSERT( aQuerySetNode->mResultLimit->mType == QLV_EXPR_TYPE_BIND_PARAM );
            aQuerySetNode->mFetchCnt = QLL_FETCH_COUNT_MAX;
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INCORRECT_RESULT_COLUMN_COUNT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SUBQUERY_BLOCK_HAS_INCORRECT_NUMBER_RESULT_COLUMNS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sChildParseSpec->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_RESULT_SKIP )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_RESULT_OFFSET_MUST_BE_ZERO_OR_POSITIVE,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aParseQuerySetNode->mResultSkip->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_RESULT_LIMIT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_RESULT_LIMIT_MUST_BE_POSITIVE,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aParseQuerySetNode->mResultLimit->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_PSEUDO_COLUMN_NOT_ALLOWED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SEQUENCE_NUMBER_NOT_ALLOWED,
                      qlgMakeErrPosString(
                          aStmtInfo->mSQLString,
                          sChildInitSpec->mQueryExprList->mPseudoColExprList->mHead->mExprPtr->mPosition,
                          aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief Set Relation 의 Column List를 생성한다.
 * @param[in]  aRegionMem        Region Memory
 * @param[in]  aQuerySetNode     Query Set Node
 * @param[in]  aLeftMostSpecNode Left-most Query Spec Node
 * @param[out] aTargetCount      Target Count
 * @param[out] aSetColumns       Set Relation 의 Column List
 * @param[in]  aEnv              Environment
 */
stlStatus qlvMakeSetRelationColumns( knlRegionMem          * aRegionMem,
                                     qlvInitQuerySetNode   * aQuerySetNode,
                                     qlvInitQuerySpecNode  * aLeftMostSpecNode,
                                     stlInt32              * aTargetCount,
                                     qlvInitTarget        ** aSetColumns,
                                     qllEnv                * aEnv )
{
    stlInt32                  i = 0;
    qlvInitTarget           * sSourceTargets = NULL;
    qlvInitTarget           * sColumns = NULL;
    qlvInitExpression       * sExpr = NULL;
    qlvInitInnerColumn      * sInnerColumn = NULL;
    stlInt32                  sTargetCount = 0;


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQuerySetNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLeftMostSpecNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetCount != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSetColumns != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sSourceTargets  = aLeftMostSpecNode->mTargets;
    sTargetCount    = aLeftMostSpecNode->mTargetCount;


    /**
     * Set Relation 의 Column List 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitTarget ) * sTargetCount,
                                (void **) & sColumns,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < sTargetCount; i++ )
    {
        /**
         * Name 연결
         */

        sColumns[i].mDisplayName = sSourceTargets[i].mDisplayName;
        sColumns[i].mDisplayPos  = sSourceTargets[i].mDisplayPos;
        sColumns[i].mAliasName   = sSourceTargets[i].mAliasName;

        /**
         * Inner Column 생성
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvInitInnerColumn ),
                                    (void **) & sInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( stlInt32 ),
                                    (void **) & sInnerColumn->mIdx,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sInnerColumn->mRelationNode = (qlvInitNode *) aQuerySetNode;
        sInnerColumn->mIdx[0] = i;
        sInnerColumn->mOrgExpr = sSourceTargets[i].mExpr;
        
        /**
         * Expression 생성
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvInitExpression ),
                                    (void **) & sExpr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sExpr->mType               = QLV_EXPR_TYPE_INNER_COLUMN;
        sExpr->mPhraseType         = QLV_EXPR_PHRASE_TARGET;
        sExpr->mRelatedFuncId      = KNL_BUILTIN_FUNC_INVALID;

        sExpr->mIterationTime      = DTL_ITERATION_TIME_FOR_EACH_EXPR;
        if ( sSourceTargets[i].mAliasName != NULL )
        {
            sExpr->mColumnName = sSourceTargets[i].mAliasName;
        }
        else
        {
            sExpr->mColumnName = sSourceTargets[i].mExpr->mColumnName;
        }
        
        QLV_VALIDATE_INCLUDE_EXPR_CNT( sSourceTargets[i].mExpr->mIncludeExprCnt + 1, aEnv );
        
        sExpr->mPosition           = sSourceTargets[i].mExpr->mPosition;
        sExpr->mIncludeExprCnt     = sSourceTargets[i].mExpr->mIncludeExprCnt + 1;
        sExpr->mOffset             = 0;
        sExpr->mIsSetOffset        = STL_FALSE;
        sExpr->mExpr.mInnerColumn  = sInnerColumn;

        /**
         * Set Column에 Expr 연결
         */

        sColumns[i].mExpr           = sExpr;
    }


    /**********************************************************
     * 마무리
     **********************************************************/

    /**
     * OUTPUT 설정
     */

    *aTargetCount   = sTargetCount;
    *aSetColumns    = sColumns;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Set Target List를 생성한다.
 * @param[in]  aRegionMem       Region Memory
 * @param[in]  aIsRootSet       Root Set 여부
 * @param[in]  aQuerySetNode    Query Set Node
 * @param[in]  aTargetCount     Target Count
 * @param[in]  aSetColumns      Set Relation 의 Column List
 * @param[out] aSetTargets      Set 의 Target List
 * @param[in]  aEnv             Environment
 */
stlStatus qlvMakeSetTargets( knlRegionMem        * aRegionMem,
                             stlBool               aIsRootSet,
                             qlvInitQuerySetNode * aQuerySetNode,
                             stlInt32              aTargetCount,
                             qlvInitTarget       * aSetColumns,
                             qlvInitTarget      ** aSetTargets,
                             qllEnv              * aEnv )
{
    stlInt32                  i = 0;
    qlvInitTarget           * sTargets = NULL;
    qlvInitExpression       * sExpr = NULL;
    qlvInitInnerColumn      * sInnerColumn = NULL;
    

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQuerySetNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSetColumns != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSetTargets != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Set Target List 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitTarget ) * aTargetCount,
                                (void **) & sTargets,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < aTargetCount; i++ )
    {
        /**
         * Name 연결
         */

        sTargets[i].mDisplayName = aSetColumns[i].mDisplayName;
        sTargets[i].mDisplayPos  = aSetColumns[i].mDisplayPos;
        sTargets[i].mAliasName   = aSetColumns[i].mAliasName;

        sTargets[i].mDataTypeInfo = aSetColumns[i].mDataTypeInfo;

        if( aIsRootSet == STL_FALSE )
        {
            /**
             * Inner Column 생성
             */

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( qlvInitInnerColumn ),
                                        (void **) & sInnerColumn,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( stlInt32 ),
                                        (void **) & sInnerColumn->mIdx,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sInnerColumn->mRelationNode = aSetColumns[i].mExpr->mExpr.mInnerColumn->mRelationNode;
            sInnerColumn->mIdx[0] = i;
            sInnerColumn->mOrgExpr = aSetColumns[i].mExpr;
        
            /**
             * Expression 생성
             */

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( qlvInitExpression ),
                                        (void **) & sExpr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sExpr->mType               = QLV_EXPR_TYPE_INNER_COLUMN;
            sExpr->mPhraseType         = QLV_EXPR_PHRASE_TARGET;
            sExpr->mRelatedFuncId      = KNL_BUILTIN_FUNC_INVALID;

            sExpr->mIterationTime      = DTL_ITERATION_TIME_FOR_EACH_EXPR;
            sExpr->mColumnName         = aSetColumns[i].mExpr->mColumnName;
        
            QLV_VALIDATE_INCLUDE_EXPR_CNT( aSetColumns[i].mExpr->mIncludeExprCnt + 1, aEnv );
            
            sExpr->mPosition           = aSetColumns[i].mExpr->mPosition;
            sExpr->mIncludeExprCnt     = aSetColumns[i].mExpr->mIncludeExprCnt + 1;
            sExpr->mOffset             = 0;  
            sExpr->mIsSetOffset        = STL_FALSE;
            sExpr->mExpr.mInnerColumn  = sInnerColumn;


            /**
             * Set Target 에 Expr 연결
             */

            sTargets[i].mExpr           = sExpr;
        }
        else
        {
            /**
             * Inner Column 생성
             */
        
            sTargets[i].mExpr = aSetColumns[i].mExpr;

            STL_TRY( qlvAddInnerColumnExpr( & sTargets[i].mExpr,
                                            (qlvInitNode *) aQuerySetNode,
                                            i,
                                            aRegionMem,
                                            aEnv )
                     == STL_SUCCESS );
        }
    }


    /**********************************************************
     * 마무리
     **********************************************************/

    /**
     * OUTPUT 설정
     */

    *aSetTargets    = sTargets;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlnv */
