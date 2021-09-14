/*******************************************************************************
 * qlnoCommon.c
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
 * @file qlnoCommon.c
 * @brief SQL Processor Layer Optimization Node Common Functions
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */


/**
 * @brief Column Name 이 같은지 판단
 * @param[in]      aName1     Column Name 1
 * @param[in]      aName2     Column Name 2
 * @return Column Name 일치 여부
 */
stlBool qlnoIsSameColumnName( stlChar  * aName1,
                              stlChar  * aName2 )
{
    if( ( aName1 != NULL ) && ( aName2 != NULL ) )
    {
        return ( stlStrcmp( aName1, aName2 ) == 0 );
    }
    else
    {
        return STL_FALSE;
    }
}


/**
 * @brief Optimization Code Node List 생성
 * @param[out]     aOptNodeList   생성된 Optimization Code Node List 
 * @param[in]      aRegionMem     Region Memory
 * @param[in]      aEnv           Environment
 */
stlStatus qlnoCreateOptNodeList( qllOptNodeList  ** aOptNodeList,
                                 knlRegionMem     * aRegionMem,
                                 qllEnv           * aEnv )
{
    qllOptNodeList  * sOptNodeList = NULL;
    
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Optimization Node List 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qllOptNodeList ),
                                (void **) & sOptNodeList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Optimization Node List 초기화
     */
    
    sOptNodeList->mCount = 0;
    sOptNodeList->mHead  = NULL;
    sOptNodeList->mTail  = NULL;
        

    /**
     * OUTPUT 설정
     */

    *aOptNodeList = sOptNodeList;


    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
    
}


/**
 * @brief Optimization Code Node List에 Node 추가
 * @param[in,out]  aOptNodeList   Optimization Code Node List 
 * @param[in]      aOptNode       리스트에 추가할 Optimization Code Node
 * @param[in]      aRegionMem     Region Memory
 * @param[in]      aEnv           Environment
 */
stlStatus qlnoAddToNodeList( qllOptNodeList       * aOptNodeList,
                             qllOptimizationNode  * aOptNode,
                             knlRegionMem         * aRegionMem,
                             qllEnv               * aEnv )
{
    qllOptNodeItem  * sNodeItem = NULL;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * qllOptNodeItem 생성 및 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qllOptNodeItem ),
                                (void **) & sNodeItem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sNodeItem->mNode = aOptNode;
    sNodeItem->mPrev = NULL;
    sNodeItem->mNext = NULL;

    
    /**
     * qllOptNodeList 설정
     */

    if( aOptNodeList->mTail == NULL )
    {
        STL_DASSERT( aOptNodeList->mHead == NULL );
        
        aOptNodeList->mCount = 1;
        aOptNodeList->mHead  = sNodeItem;
        aOptNodeList->mTail  = sNodeItem;
    }
    else
    {
        aOptNodeList->mCount++;
        sNodeItem->mPrev           = aOptNodeList->mTail;
        aOptNodeList->mTail->mNext = sNodeItem;
        aOptNodeList->mTail        = sNodeItem;
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Common Optimization Code Node List 생성
 * @param[in]      aOptNodeList      Optimization Code Node List 
 * @param[in]      aNodeType         Optimization Node Type
 * @param[in]      aNode             세부 Optimization Node
 * @param[in]      aCurQueryNode     현재 Node가 속한 Query Optimization Node
 * @param[in]      aOptCost          Optimization Cost 정보
 * @param[in]      aBindParamInfo    Bind Parameter 정보
 * @param[out]     aNewOptNode       생성된 Common Optimization Code Node
 * @param[in]      aRegionMem        Region Memory
 * @param[in]      aEnv              Environment
 */
stlStatus qlnoCreateOptNode( qllOptNodeList        * aOptNodeList,
                             qllPlanNodeType         aNodeType,
                             void                  * aNode,
                             qllOptimizationNode   * aCurQueryNode,
                             qllEstimateCost       * aOptCost,
                             qllBindParamInfo      * aBindParamInfo,
                             qllOptimizationNode  ** aNewOptNode,
                             knlRegionMem          * aRegionMem,
                             qllEnv                * aEnv )
{
    qllOptimizationNode  * sOptNode = NULL;

    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList->mCount < STL_INT16_MAX, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aNodeType >= 0 ) && ( aNodeType < QLL_PLAN_NODE_TYPE_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qllOptimizationNode ),
                                (void **) & sOptNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Optimization Node 설정
     */
    
    sOptNode->mType             = aNodeType;
    sOptNode->mIdx              = aOptNodeList->mCount;
    sOptNode->mOptNode          = aNode;
    /* sOptNode->mOptCurQueryNode = aCurQueryNode; */
    /* sOptNode->mEstimateCost    = aOptCost; */
    sOptNode->mBindParamInfo    = aBindParamInfo;   
    sOptNode->mIsAtomic         = STL_FALSE;
    

    /**
     * Optimization Node List 정보 변경 
     */

    STL_TRY( qlnoAddToNodeList( aOptNodeList,
                                sOptNode,
                                aRegionMem,
                                aEnv )
             == STL_SUCCESS );


    /**
     * OUTPUT 설정
     */

    *aNewOptNode = sOptNode;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief  Instant Table을 초기화 한다.
 * @param[in,out] aInstTable      Instant Table
 * @param[in]     aInstTableType  Instant Table Type
 * @param[in]     aBaseRelName    Base Relation Name
 * @param[in]     aRegionMem      Region Memory
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoInitInstant( qlvInitInstantNode  * aInstTable,
                           qlvNodeType           aInstTableType,
                           qlvRelationName     * aBaseRelName,
                           knlRegionMem        * aRegionMem,
                           qllEnv              * aEnv )
{
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aInstTable != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( ( aInstTableType == QLV_NODE_TYPE_FLAT_INSTANT ) ||
                          ( aInstTableType == QLV_NODE_TYPE_SORT_INSTANT ) ||
                          ( aInstTableType == QLV_NODE_TYPE_HASH_INSTANT ) ||
                          ( aInstTableType == QLV_NODE_TYPE_GROUP_HASH_INSTANT ) ||
                          ( aInstTableType == QLV_NODE_TYPE_GROUP_SORT_INSTANT ) ||
                          ( aInstTableType == QLV_NODE_TYPE_SORT_JOIN_INSTANT ) ||
                          ( aInstTableType == QLV_NODE_TYPE_HASH_JOIN_INSTANT ) ),
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Instant Table 정보 초기화
     */

    /* common */
    aInstTable->mType         = aInstTableType;
    aInstTable->mRelationName = aBaseRelName;
    aInstTable->mTableNode    = NULL;

    STL_TRY( qlvCreateRefExprList( & aInstTable->mKeyColList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & aInstTable->mRecColList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & aInstTable->mReadColList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );
    
    STL_TRY( qlvCreateRefExprList( & aInstTable->mOuterColumnList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    aInstTable->mKeyColCnt = 0;
    aInstTable->mRecColCnt = 0;
    aInstTable->mReadColCnt = 0;

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( stlUInt8 ) * SML_MAX_INDEX_KEY_COL_COUNT,
                                (void **) & aInstTable->mKeyFlags,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( aInstTable->mKeyFlags,
               0x00,
               STL_SIZEOF( stlUInt8 ) * SML_MAX_INDEX_KEY_COL_COUNT );
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Instant Table에 Key Column 을 추가한다.
 * @param[in]     aSQLString      SQL String
 * @param[in,out] aInstTable      Instant Table
 * @param[in]     aOrgExpr        Original Expression
 * @param[in]     aKeyFlag        Key Column Flag
 * @param[out]    aColExpr        Column Expression (Inner Column)
 * @param[in]     aRegionMem      Region Memory
 * @param[in]     aEnv            Environment
 *
 * @remark Constant Expression을 Instant Table에 추가하려는 경우,
 *    <BR> 해당 Expression은 Instant Table의 Column List에 삽입되지 않고
 *    <BR> 결과 Expression으로 입력 받은 Expression이 그대로 전달된다. 
 */
stlStatus qlnoAddKeyColToInstant( stlChar              * aSQLString,
                                  qlvInitInstantNode   * aInstTable,
                                  qlvInitExpression    * aOrgExpr,
                                  stlUInt8               aKeyFlag,
                                  qlvInitExpression   ** aColExpr,
                                  knlRegionMem         * aRegionMem,
                                  qllEnv               * aEnv )
{
    qlvInitExpression  * sColExpr  = aOrgExpr;
    qlvRefExprItem     * sExprItem = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstTable != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( ( aInstTable->mType == QLV_NODE_TYPE_SORT_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_HASH_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_GROUP_HASH_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_GROUP_SORT_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_SORT_JOIN_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_HASH_JOIN_INSTANT ) ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrgExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstTable->mReadColList != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Expression이 INSTANT TABLE의 Key Column List에 들어 있는 column인지 확인
     */

    sExprItem = aInstTable->mKeyColList->mHead;

    while( sExprItem != NULL )
    {
        if( qlvIsSameExpression( sColExpr, sExprItem->mExprPtr ) == STL_TRUE )
        {
            STL_TRY( qlvCopyExpr( sExprItem->mExprPtr,
                                  & sColExpr,
                                  aRegionMem,
                                  aEnv )
                     == STL_SUCCESS );
        
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            sExprItem = sExprItem->mNext;
        }
    }


    /**
     * 반드시 Key Column List의 구성은 Record Column List 구성보다 선행되어야 한다.
     */

    STL_DASSERT( aInstTable->mRecColList->mCount == 0 );


    /**
     * INNER COLUMN EXPRESSION 구성
     */
    
    STL_TRY( qlvAddInnerColumnExpr( & sColExpr,
                                    (qlvInitNode *) aInstTable,
                                    0,
                                    aRegionMem,
                                    aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvAddExprToRefExprList( aInstTable->mKeyColList,
                                      sColExpr,
                                      STL_FALSE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );

    aInstTable->mKeyColCnt++;
        
    
    /**
     * Key Column Flag 설정
     */

    /* candidate plan 구성시 만들어지는 instant이기 때문에 해당 함수를 호출하지 않는다. */
    STL_DASSERT( ( aInstTable->mType != QLV_NODE_TYPE_SORT_JOIN_INSTANT ) &&
                 ( aInstTable->mType != QLV_NODE_TYPE_HASH_JOIN_INSTANT ) );

    /* 미구현 */
    STL_DASSERT( aInstTable->mType != QLV_NODE_TYPE_GROUP_SORT_INSTANT );

    if( ( aInstTable->mType == QLV_NODE_TYPE_SORT_INSTANT ) ||
        ( aInstTable->mType == QLV_NODE_TYPE_GROUP_HASH_INSTANT ) )
    {
        if( ( aInstTable->mKeyColCnt % SML_MAX_INDEX_KEY_COL_COUNT ) == 0 )
        {
            stlUInt8 * sKeyFlags;
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( stlUInt8 ) * ( aInstTable->mKeyColCnt + SML_MAX_INDEX_KEY_COL_COUNT ),
                                        (void **) & sKeyFlags,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemset( sKeyFlags,
                       0x00,
                       STL_SIZEOF( stlUInt8 ) * ( aInstTable->mKeyColCnt + SML_MAX_INDEX_KEY_COL_COUNT ) );

            stlMemcpy( sKeyFlags,
                       aInstTable->mKeyFlags,
                       STL_SIZEOF( stlUInt8 ) * aInstTable->mKeyColCnt );

            aInstTable->mKeyFlags = sKeyFlags;
        }
        
        aInstTable->mKeyFlags[ aInstTable->mKeyColCnt - 1 ] = aKeyFlag;
    }
    else
    {
        /* QLV_NODE_TYPE_HASH_INSTANT */
        /* Do Nothing */
    }

    STL_RAMP( RAMP_FINISH );
    

    /**
     * OUTPUT 설정
     */

    *aColExpr = sColExpr;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Instant Table에 Key Column 을 추가한다. (position까지 동일한지 체크)
 * @param[in]     aSQLString      SQL String
 * @param[in,out] aInstTable      Instant Table
 * @param[in]     aOrgExpr        Original Expression
 * @param[in]     aKeyFlag        Key Column Flag
 * @param[out]    aColExpr        Column Expression (Inner Column)
 * @param[in]     aRegionMem      Region Memory
 * @param[in]     aEnv            Environment
 *
 * @remark Constant Expression을 Instant Table에 추가하려는 경우,
 *    <BR> 해당 Expression은 Instant Table의 Column List에 삽입되지 않고
 *    <BR> 결과 Expression으로 입력 받은 Expression이 그대로 전달된다. 
 */
stlStatus qlnoAddKeyColToInstantRestrict( stlChar              * aSQLString,
                                          qlvInitInstantNode   * aInstTable,
                                          qlvInitExpression    * aOrgExpr,
                                          stlUInt8               aKeyFlag,
                                          qlvInitExpression   ** aColExpr,
                                          knlRegionMem         * aRegionMem,
                                          qllEnv               * aEnv )
{
    qlvInitExpression  * sColExpr      = aOrgExpr;
    qlvInitExpression  * sRecColExpr   = NULL;
    qlvRefExprItem     * sExprItem     = NULL;
    qlvRefExprItem     * sPrevExprItem = NULL;
    stlBool              sIsExistInKey = STL_FALSE;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstTable != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( ( aInstTable->mType == QLV_NODE_TYPE_SORT_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_HASH_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_GROUP_HASH_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_GROUP_SORT_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_SORT_JOIN_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_HASH_JOIN_INSTANT ) ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrgExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstTable->mReadColList != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Expression이 INSTANT TABLE의 Record Column List에 들어 있는 column인지 확인
     */

    STL_DASSERT( sColExpr->mPosition > 0 );

    if( sColExpr->mPosition == sColExpr->mExpr.mInnerColumn->mOrgExpr->mPosition )
    {
        sExprItem = aInstTable->mRecColList->mHead;

        while( sExprItem != NULL )
        {
            if( qlvIsExistInExpression( sColExpr, sExprItem->mExprPtr ) == STL_TRUE )
            {
                STL_TRY_THROW( sRecColExpr == NULL, RAMP_ERR_COLUMN_AMBIGUOUS );
            
                sRecColExpr = sExprItem->mExprPtr;
            }

            sExprItem = sExprItem->mNext;
        }

        sExprItem = aInstTable->mKeyColList->mHead;

        while( sExprItem != NULL )
        {
            if( qlvIsExistInExpression( sColExpr, sExprItem->mExprPtr ) == STL_TRUE )
            {
                STL_TRY_THROW( sRecColExpr == NULL, RAMP_ERR_COLUMN_AMBIGUOUS );
            
                sRecColExpr = sExprItem->mExprPtr;
                sIsExistInKey = STL_TRUE;
            }

            sExprItem = sExprItem->mNext;
        }
    }
    else
    {
        sExprItem = aInstTable->mRecColList->mHead;

        while( sExprItem != NULL )
        {
            if( qlnoIsSameColumnName( sColExpr->mColumnName,
                                      sExprItem->mExprPtr->mColumnName )
                == STL_TRUE )
            {
                STL_TRY_THROW( sRecColExpr == NULL, RAMP_ERR_COLUMN_AMBIGUOUS );
            
                sRecColExpr = sExprItem->mExprPtr;
            }

            sExprItem = sExprItem->mNext;
        }

        sExprItem = aInstTable->mKeyColList->mHead;

        while( sExprItem != NULL )
        {
            if( qlnoIsSameColumnName( sColExpr->mColumnName,
                                      sExprItem->mExprPtr->mColumnName )
                == STL_TRUE )
            {
                STL_TRY_THROW( sRecColExpr == NULL, RAMP_ERR_COLUMN_AMBIGUOUS );
            
                sRecColExpr = sExprItem->mExprPtr;
                sIsExistInKey = STL_TRUE;
            }

            sExprItem = sExprItem->mNext;
        }
    }

    STL_TRY_THROW( sRecColExpr != NULL, RAMP_ERR_COLUMN_AMBIGUOUS );

    
    /**
     * Key Column List 에 동일한 Key Column이 존재한다면 추가하지 않는다.
     */

    if( sIsExistInKey == STL_TRUE )
    {
        STL_TRY( qlvCopyExpr( sRecColExpr,
                              & sColExpr,
                              aRegionMem,
                              aEnv )
                 == STL_SUCCESS );
                
        STL_THROW( RAMP_FINISH );
    }


    /**
     * Record Column List 에서 해당 Column Expression 제거
     */

    sExprItem = aInstTable->mRecColList->mHead;

    while( sExprItem != NULL )
    {
        if( sRecColExpr == sExprItem->mExprPtr )
        {
            if( sPrevExprItem == NULL )
            {
                aInstTable->mRecColList->mHead = sExprItem->mNext;
            }
            else
            {
                sPrevExprItem->mNext = sExprItem->mNext;
            }

            if( sExprItem == aInstTable->mRecColList->mTail )
            {
                aInstTable->mRecColList->mTail = sPrevExprItem;
            }

            aInstTable->mRecColList->mCount--;
            aInstTable->mRecColCnt--;

            break;
        }

        sPrevExprItem = sExprItem;
        sExprItem = sExprItem->mNext;
    }
    

    /**
     * INNER COLUMN EXPRESSION 구성
     */

    STL_DASSERT( sRecColExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
    sColExpr = sRecColExpr->mExpr.mInnerColumn->mOrgExpr;

    STL_TRY( qlvAddInnerColumnExpr( & sColExpr,
                                    (qlvInitNode *) aInstTable,
                                    0,
                                    aRegionMem,
                                    aEnv )
             == STL_SUCCESS );
    

    STL_TRY( qlvAddExprToRefExprList( aInstTable->mKeyColList,
                                      sColExpr,
                                      STL_FALSE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );

    aInstTable->mKeyColCnt++;

    sRecColExpr->mExpr.mInnerColumn->mOrgExpr = sColExpr;
        
    
    /**
     * Key Column Flag 설정
     */

    if( aInstTable->mType == QLV_NODE_TYPE_SORT_INSTANT )
    {
        if( ( aInstTable->mKeyColCnt % SML_MAX_INDEX_KEY_COL_COUNT ) == 0 )
        {
            stlUInt8 * sKeyFlags;
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( stlUInt8 ) * ( aInstTable->mKeyColCnt + SML_MAX_INDEX_KEY_COL_COUNT ),
                                        (void **) & sKeyFlags,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemset( sKeyFlags,
                       0x00,
                       STL_SIZEOF( stlUInt8 ) * ( aInstTable->mKeyColCnt + SML_MAX_INDEX_KEY_COL_COUNT ) );

            stlMemcpy( sKeyFlags,
                       aInstTable->mKeyFlags,
                       STL_SIZEOF( stlUInt8 ) * aInstTable->mKeyColCnt );

            aInstTable->mKeyFlags = sKeyFlags;
        }

        aInstTable->mKeyFlags[ aInstTable->mKeyColCnt - 1 ] = aKeyFlag;
    }
    else
    {
        /* Do Nothing */
    }

    STL_RAMP( RAMP_FINISH );
    

    /**
     * OUTPUT 설정
     */

    *aColExpr = sColExpr;
    

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_AMBIGUOUS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_AMBIGUOUS,
                      qlgMakeErrPosString( aSQLString,
                                           sColExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      NULL );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Instant Table에 Record Column 을 추가한다.
 * @param[in,out] aInstTable      Instant Table
 * @param[in]     aOrgExpr        Original Expression
 * @param[out]    aColExpr        Column Expression (Inner Column)
 * @param[in]     aRegionMem      Region Memory
 * @param[in]     aEnv            Environment
 *
 * @remark Constant Expression을 Instant Table에 추가하려는 경우,
 *    <BR> 해당 Expression은 Instant Table의 Column List에 삽입되지 않고
 *    <BR> 결과 Expression으로 입력 받은 Expression이 그대로 전달된다. 
 */
stlStatus qlnoAddRecColToInstant( qlvInitInstantNode   * aInstTable,
                                  qlvInitExpression    * aOrgExpr,
                                  qlvInitExpression   ** aColExpr,
                                  knlRegionMem         * aRegionMem,
                                  qllEnv               * aEnv )
{
    qlvInitExpression  * sColExpr  = aOrgExpr;
    qlvRefExprItem     * sExprItem = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aInstTable != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( ( aInstTable->mType == QLV_NODE_TYPE_SORT_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_HASH_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_GROUP_HASH_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_GROUP_SORT_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_SORT_JOIN_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_HASH_JOIN_INSTANT ) ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrgExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstTable->mReadColList != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Expression이 Constant Expression이면 INSTANT TABLE의 column으로 구성하지 않음
     */
        
    STL_TRY_THROW( ( aOrgExpr->mIterationTime != DTL_ITERATION_TIME_FOR_EACH_STATEMENT ) &&
                   ( aOrgExpr->mIterationTime != DTL_ITERATION_TIME_NONE ),
                   RAMP_FINISH );


    /**
     * Expression이 INSTANT TABLE의 Key Column List에 들어 있는 column인지 확인
     */

    sExprItem = aInstTable->mKeyColList->mHead;

    while( sExprItem != NULL )
    {
        if( qlvIsSameExpression( sColExpr, sExprItem->mExprPtr ) == STL_TRUE )
        {
            STL_TRY( qlvCopyExpr( sExprItem->mExprPtr,
                                  & sColExpr,
                                  aRegionMem,
                                  aEnv )
                     == STL_SUCCESS );
        
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            sExprItem = sExprItem->mNext;
        }
    }


    /**
     * Expression이 INSTANT TABLE의 Record Column List에 들어 있는 column인지 확인
     */

    sExprItem = aInstTable->mRecColList->mHead;

    while( sExprItem != NULL )
    {
        if( qlvIsSameExpression( sColExpr, sExprItem->mExprPtr ) == STL_TRUE )
        {
            STL_TRY( qlvCopyExpr( sExprItem->mExprPtr,
                                  & sColExpr,
                                  aRegionMem,
                                  aEnv )
                     == STL_SUCCESS );
        
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            sExprItem = sExprItem->mNext;
        }
    }


    /**
     * INNER COLUMN EXPRESSION 구성
     */

    STL_TRY( qlvAddInnerColumnExpr( & sColExpr,
                                    (qlvInitNode *) aInstTable,
                                    0,
                                    aRegionMem,
                                    aEnv )
             == STL_SUCCESS );
    

    STL_TRY( qlvAddExprToRefExprList( aInstTable->mRecColList,
                                      sColExpr,
                                      STL_FALSE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );

    aInstTable->mRecColCnt++;
    

    STL_RAMP( RAMP_FINISH );
    

    /**
     * OUTPUT 설정
     */

    *aColExpr = sColExpr;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Instant Table에 Record Column 을 추가한다. (position까지 동일한지 체크)
 * @param[in]     aSQLString      SQL String
 * @param[in,out] aInstTable      Instant Table
 * @param[in]     aOrgExpr        Original Expression
 * @param[out]    aColExpr        Column Expression (Inner Column)
 * @param[in]     aRegionMem      Region Memory
 * @param[in]     aEnv            Environment
 *
 * @remark 입력 받은 Expression을 모두 Record Column으로 추가한다.
 *    <BR> 단, Key Column List 설정 이전에 Record Column을 구성한다.
 */
stlStatus qlnoAddRecColToInstantRestrict( stlChar              * aSQLString,
                                          qlvInitInstantNode   * aInstTable,
                                          qlvInitExpression    * aOrgExpr,
                                          qlvInitExpression   ** aColExpr,
                                          knlRegionMem         * aRegionMem,
                                          qllEnv               * aEnv )
{
    qlvInitExpression  * sColExpr  = aOrgExpr;
    qlvRefExprItem     * sExprItem = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstTable != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( ( aInstTable->mType == QLV_NODE_TYPE_SORT_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_HASH_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_GROUP_HASH_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_GROUP_SORT_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_SORT_JOIN_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_HASH_JOIN_INSTANT ) ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrgExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstTable->mReadColList != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Expression이 Constant Expression이면 INSTANT TABLE의 column으로 구성하지 않음
     */
        
    STL_TRY_THROW( ( aOrgExpr->mIterationTime != DTL_ITERATION_TIME_FOR_EACH_STATEMENT ) &&
                   ( aOrgExpr->mIterationTime != DTL_ITERATION_TIME_NONE ),
                   RAMP_FINISH );


    /**
     * Key Column List는 비어있어야 한다.
     */

    STL_DASSERT( aInstTable->mKeyColList->mCount == 0 );


    /**
     * Record Column List에 동일한 Expression 이 들어 있는지 확인
     */

    sExprItem = aInstTable->mRecColList->mHead;

    while( sExprItem != NULL )
    {
        if( sColExpr == sExprItem->mExprPtr )
        {
            STL_THROW( RAMP_ERR_COLUMN_AMBIGUOUS );
        }

        sExprItem = sExprItem->mNext;
    }


    /**
     * INNER COLUMN EXPRESSION 구성
     */

    STL_TRY( qlvAddInnerColumnExpr( & sColExpr,
                                    (qlvInitNode *) aInstTable,
                                    0,
                                    aRegionMem,
                                    aEnv )
             == STL_SUCCESS );
    

    STL_TRY( qlvAddExprToRefExprList( aInstTable->mRecColList,
                                      sColExpr,
                                      STL_FALSE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );

    aInstTable->mRecColCnt++;

    STL_RAMP( RAMP_FINISH );
    

    /**
     * OUTPUT 설정
     */

    *aColExpr = sColExpr;
    

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_AMBIGUOUS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_AMBIGUOUS,
                      qlgMakeErrPosString( aSQLString,
                                           sColExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      NULL );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Instant Table에 Read Key Column 을 추가한다.
 * @param[in,out] aInstTable      Instant Table
 * @param[in]     aOrgExpr        Original Expression
 * @param[out]    aColExpr        Column Expression (Inner Column)
 * @param[in]     aRegionMem      Region Memory
 * @param[in]     aEnv            Environment
 *
 * @remark Constant Expression을 Instant Table에 추가하려는 경우,
 *    <BR> 해당 Expression은 Instant Table의 Column List에 삽입되지 않고
 *    <BR> 결과 Expression으로 입력 받은 Expression이 그대로 전달된다. 
 */
stlStatus qlnoAddReadColToInstant( qlvInitInstantNode   * aInstTable,
                                   qlvInitExpression    * aOrgExpr,
                                   qlvInitExpression   ** aColExpr,
                                   knlRegionMem         * aRegionMem,
                                   qllEnv               * aEnv )
{
    qlvInitExpression  * sColExpr      = aOrgExpr;
    qlvInitExpression  * sTempExpr     = NULL;
    qlvRefExprItem     * sExprItem     = NULL;
    qlvRefExprItem     * sSameExprItem = NULL;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aInstTable != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrgExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstTable->mReadColList != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Expression이 Constant Expression이면 INSTANT TABLE의 column으로 구성하지 않음
     */

    STL_TRY_THROW( ( aOrgExpr->mIterationTime != DTL_ITERATION_TIME_FOR_EACH_STATEMENT ) &&
                   ( aOrgExpr->mIterationTime != DTL_ITERATION_TIME_NONE ),
                   RAMP_FINISH );
    
    
    /**
     * Expression이 INSTANT TABLE의 Key Column List에 들어 있는 column인지 확인
     */

    sExprItem = aInstTable->mKeyColList->mHead;

    while( sExprItem != NULL )
    {
        if( qlvIsSameExpression( sColExpr, sExprItem->mExprPtr ) == STL_TRUE )
        {
            /**
             * Expression이 INSTANT TABLE의 Read Column List에 들어 있는 column인지 확인
             */

            sSameExprItem = sExprItem;
            sExprItem     = aInstTable->mReadColList->mHead;

            while( sExprItem != NULL )
            {
                if( qlvIsSameExpression( sColExpr, sExprItem->mExprPtr ) == STL_TRUE )
                {
                    sColExpr = sExprItem->mExprPtr;
                    STL_TRY( qlvAddInnerColumnExpr( & sColExpr,
                                                    (qlvInitNode *) aInstTable,
                                                    0,
                                                    aRegionMem,
                                                    aEnv )
                             == STL_SUCCESS );
            
                    sColExpr->mExpr.mInnerColumn->mIdx =
                        sExprItem->mExprPtr->mExpr.mInnerColumn->mIdx;
                    
                    STL_THROW( RAMP_FINISH );
                }
                else
                {
                    sExprItem = sExprItem->mNext;
                }
            }
            
        
            /**
             * INSTANT TABLE의 READ COLUMN LIST에 추가
             */
            
            STL_TRY( qlvCopyExpr( sSameExprItem->mExprPtr,
                                  & sColExpr,
                                  aRegionMem,
                                  aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlvAddExprToRefExprList( aInstTable->mReadColList,
                                              sColExpr,
                                              STL_FALSE,
                                              aRegionMem,
                                              aEnv )
                     == STL_SUCCESS );

            aInstTable->mReadColCnt++;

            sTempExpr = sColExpr;
            STL_TRY( qlvAddInnerColumnExpr( & sColExpr,
                                            (qlvInitNode *) aInstTable,
                                            0,
                                            aRegionMem,
                                            aEnv )
                     == STL_SUCCESS );

            sColExpr->mExpr.mInnerColumn->mIdx =
                sTempExpr->mExpr.mInnerColumn->mIdx;
            
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            sExprItem = sExprItem->mNext;
        }
    }


    /**
     * Expression이 INSTANT TABLE의 Record Column List에 들어 있는 column인지 확인
     */

    sExprItem = aInstTable->mRecColList->mHead;

    while( sExprItem != NULL )
    {
        if( qlvIsSameExpression( sColExpr, sExprItem->mExprPtr ) == STL_TRUE )
        {
            /**
             * Expression이 INSTANT TABLE의 Read Column List에 들어 있는 column인지 확인
             */

            sSameExprItem = sExprItem;
            sExprItem     = aInstTable->mReadColList->mHead;

            while( sExprItem != NULL )
            {
                if( qlvIsSameExpression( sColExpr, sExprItem->mExprPtr ) == STL_TRUE )
                {
                    sColExpr = sExprItem->mExprPtr;
                    STL_TRY( qlvAddInnerColumnExpr( & sColExpr,
                                                    (qlvInitNode *) aInstTable,
                                                    0,
                                                    aRegionMem,
                                                    aEnv )
                             == STL_SUCCESS );

                    sColExpr->mExpr.mInnerColumn->mIdx =
                        sExprItem->mExprPtr->mExpr.mInnerColumn->mIdx;

                    STL_THROW( RAMP_FINISH );
                }
                else
                {
                    sExprItem = sExprItem->mNext;
                }
            }
            
        
            /**
             * INSTANT TABLE의 READ COLUMN LIST에 추가
             */

            STL_TRY( qlvCopyExpr( sSameExprItem->mExprPtr,
                                  & sColExpr,
                                  aRegionMem,
                                  aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlvAddExprToRefExprList( aInstTable->mReadColList,
                                              sColExpr,
                                              STL_FALSE,
                                              aRegionMem,
                                              aEnv )
                     == STL_SUCCESS );

            aInstTable->mReadColCnt++;

            sTempExpr = sColExpr;
            STL_TRY( qlvAddInnerColumnExpr( & sColExpr,
                                            (qlvInitNode *) aInstTable,
                                            0,
                                            aRegionMem,
                                            aEnv )
                     == STL_SUCCESS );

            sColExpr->mExpr.mInnerColumn->mIdx =
                sTempExpr->mExpr.mInnerColumn->mIdx;
            
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            sExprItem = sExprItem->mNext;
        }
    }


    /**
     * INNER COLUMN EXPRESSION 구성
     */
    
    STL_TRY( qlvAddInnerColumnExpr( & sColExpr,
                                    (qlvInitNode *) aInstTable,
                                    0,
                                    aRegionMem,
                                    aEnv )
             == STL_SUCCESS );
    

    /**
     * INSTANT TABLE의 RECORD COLUMN LIST에 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aInstTable->mRecColList,
                                      sColExpr,
                                      STL_FALSE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );

    aInstTable->mRecColCnt++;

    
    /**
     * INSTANT TABLE의 READ COLUMN LIST에 추가
     */

    STL_TRY( qlvAddExprToRefExprList( aInstTable->mReadColList,
                                      sColExpr,
                                      STL_FALSE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );

    aInstTable->mReadColCnt++;
    
    STL_RAMP( RAMP_FINISH );
    

    /**
     * OUTPUT 설정
     */

    *aColExpr = sColExpr;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

    
/**
 * @brief Instant Table에 Read Key Column 을 추가한다. (position까지 동일한지 체크)
 * @param[in,out] aInstTable      Instant Table
 * @param[in]     aOrgExpr        Original Expression
 * @param[out]    aColExpr        Column Expression (Inner Column)
 * @param[in]     aRegionMem      Region Memory
 * @param[in]     aEnv            Environment
 *
 * @remark Constant Expression을 Instant Table에 추가하려는 경우,
 *    <BR> 해당 Expression은 Instant Table의 Column List에 삽입되지 않고
 *    <BR> 결과 Expression으로 입력 받은 Expression이 그대로 전달된다. 
 */
stlStatus qlnoAddReadColToInstantRestrict( qlvInitInstantNode   * aInstTable,
                                           qlvInitExpression    * aOrgExpr,
                                           qlvInitExpression   ** aColExpr,
                                           knlRegionMem         * aRegionMem,
                                           qllEnv               * aEnv )
{
    qlvInitExpression  * sColExpr      = aOrgExpr;
    qlvInitExpression  * sTempExpr     = NULL;
    qlvRefExprItem     * sExprItem     = NULL;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aInstTable != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrgExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstTable->mReadColList != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Expression이 Constant Expression이면 INSTANT TABLE의 column으로 구성하지 않음
     */

    STL_TRY_THROW( ( aOrgExpr->mIterationTime != DTL_ITERATION_TIME_FOR_EACH_STATEMENT ) &&
                   ( aOrgExpr->mIterationTime != DTL_ITERATION_TIME_NONE ),
                   RAMP_FINISH );
    
    
    /**
     * Expression이 INSTANT TABLE의 Key Column List에 들어 있는 column인지 확인
     */

    sExprItem = aInstTable->mKeyColList->mHead;

    while( sExprItem != NULL )
    {
        if( qlvIsExistInExpression( sColExpr, sExprItem->mExprPtr ) == STL_TRUE )
        {
            STL_THROW( RAMP_ADD_READ_COLUMN );
        }

        sExprItem = sExprItem->mNext;
    }

    
    /**
     * Expression이 INSTANT TABLE의 Record Column List에 들어 있는 column인지 확인
     */

    sExprItem = aInstTable->mRecColList->mHead;

    while( sExprItem != NULL )
    {
        if( qlvIsExistInExpression( sColExpr, sExprItem->mExprPtr ) == STL_TRUE )
        {
            STL_THROW( RAMP_ADD_READ_COLUMN );
        }

        sExprItem = sExprItem->mNext;
    }
        

    STL_RAMP( RAMP_ADD_READ_COLUMN );

    STL_DASSERT( sExprItem != NULL );

    sColExpr = sExprItem->mExprPtr;

    
    /**
     * INSTANT TABLE의 READ COLUMN LIST에 추가
     */
            
    STL_TRY( qlvCopyExpr( sColExpr,
                          & sColExpr,
                          aRegionMem,
                          aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvAddExprToRefExprList( aInstTable->mReadColList,
                                      sColExpr,
                                      STL_FALSE,
                                      aRegionMem,
                                      aEnv )
             == STL_SUCCESS );

    aInstTable->mReadColCnt++;

    sTempExpr = sColExpr;
    STL_TRY( qlvAddInnerColumnExpr( & sColExpr,
                                    (qlvInitNode *) aInstTable,
                                    0,
                                    aRegionMem,
                                    aEnv )
             == STL_SUCCESS );

    sColExpr->mExpr.mInnerColumn->mIdx =
        sTempExpr->mExpr.mInnerColumn->mIdx;
    
    STL_RAMP( RAMP_FINISH );
    

    /**
     * OUTPUT 설정
     */

    *aColExpr = sColExpr;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Instant Table에서 Column List 중에 해당 Expression이 있는지 확인한다.
 * @param[in,out] aInstTable      Instant Table
 * @param[in]     aExpr           Expression
 * @param[in]     aIsCheckKeyCol  Key Column List에서 Expression을 찾을지 여부
 * @param[out]    aIsExist        Column List에 Expression이 존재하는지 여부
 * @param[out]    aSameExpr       Column List에 존재하는 동일한 Expression
 * @param[in]     aRegionMem      Region Memory
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoIsExistExprFromInstant( qlvInitInstantNode   * aInstTable,
                                      qlvInitExpression    * aExpr,
                                      stlBool                aIsCheckKeyCol,
                                      stlBool              * aIsExist,
                                      qlvInitExpression   ** aSameExpr,
                                      knlRegionMem         * aRegionMem,
                                      qllEnv               * aEnv )
{
    qlvRefExprItem  * sExprItem = NULL;
    

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aInstTable != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( ( aInstTable->mType == QLV_NODE_TYPE_FLAT_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_SORT_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_HASH_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_GROUP_HASH_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_GROUP_SORT_INSTANT ) ||
                          ( aInstTable->mType == QLV_NODE_TYPE_HASH_JOIN_INSTANT ) |
                          ( aInstTable->mType == QLV_NODE_TYPE_SORT_JOIN_INSTANT ) ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIsExist != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * INSTANT TABLE의 대상 Column List 선정
     */

    *aIsExist = STL_FALSE;
    
    if( aIsCheckKeyCol == STL_TRUE )
    {
        sExprItem = aInstTable->mKeyColList->mHead;
    }
    else
    {
        /* coverage : 현재 record column list 를 대상으로 탐색하는 호출은 없음 */
        sExprItem = aInstTable->mRecColList->mHead;
    }

    
    /**
     * Expression이 INSTANT TABLE의 Column List에 들어 있는 column인지 확인
     */

    while( sExprItem != NULL )
    {
        if( qlvIsSameExpression( aExpr, sExprItem->mExprPtr ) == STL_TRUE )
        {
            if( aSameExpr != NULL )
            {
                *aSameExpr = sExprItem->mExprPtr;
                STL_TRY( qlvAddInnerColumnExpr( aSameExpr,
                                                (qlvInitNode *) aInstTable,
                                                0,
                                                aRegionMem,
                                                aEnv )
                         == STL_SUCCESS );

                (*aSameExpr)->mExpr.mInnerColumn->mIdx =
                    sExprItem->mExprPtr->mExpr.mInnerColumn->mIdx;
            }
            else
            {
                /* Do Nothing */
            }
            
            *aIsExist = STL_TRUE;
            break;
        }
        else
        {
            sExprItem = sExprItem->mNext;
        }
    }
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief  Instant Table의 Column에 대한 Idx를 설정한다.
 * @param[in,out] aInstTable   Instant Table
 * @param[in]     aEnv            Environment
 *
 * @remark Instant Table의 Column 구성을 완료한 후에 반드시 호출하여야 한다.
 */
stlStatus qlnoSetIdxInstant( qlvInitInstantNode  * aInstTable,
                             qllEnv              * aEnv )
{
    qlvRefExprItem  * sRefColumnItem = NULL;
    stlInt32          sLoop          = 0;
    stlInt32          sIdx           = 0;
    
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aInstTable != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Idx 설정 :  Key Column
     */

    sRefColumnItem = aInstTable->mKeyColList->mHead;

    for( sLoop = 0 ; sLoop < aInstTable->mKeyColCnt ; sLoop++, sIdx++ )
    {
        STL_DASSERT( sRefColumnItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
        
        *sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx = sIdx;

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /**
     * Idx 설정 : Record Column
     */

    sRefColumnItem = aInstTable->mRecColList->mHead;

    for( sLoop = 0 ; sLoop < aInstTable->mRecColCnt ; sLoop++, sIdx++ )
    {
        STL_DASSERT( sRefColumnItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
        
        *sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx = sIdx;

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /**
     * Idx 설정 : Read Column
     * @remark Read Column은 Key Column 또는 Record Column을 참조하기 때문에 idx를 부여하지 않는다.
     */
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Node를 기반으로 Ref Table List 정보를 구축한다.
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in]     aOptNode        Optimization Node
 * @param[in,out] aBaseTableList  Reference Base Table List
 * @param[in]     aRegionMem      Region Memory
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoGetRefBaseTableNodeList( qllOptNodeList       * aOptNodeList,
                                       qllOptimizationNode  * aOptNode,
                                       qllOptNodeList       * aBaseTableList,
                                       knlRegionMem         * aRegionMem,
                                       qllEnv               * aEnv )
{
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBaseTableList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Query Node의 대상 Table Node를 분석
     * @todo Aggregation Distinct와 같이 Table Node가 지정되지 않은 Node 제거
     */

    while( STL_TRUE )
    {
        if( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE )
        {
            aOptNode = ((qlnoSubQueryList *) aOptNode->mOptNode)->mChildNode;
        }
        else if( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE )
        {
            aOptNode = ((qlnoSubQueryFilter *) aOptNode->mOptNode)->mChildNode;
            STL_DASSERT( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE );
            aOptNode = ((qlnoSubQueryList *) aOptNode->mOptNode)->mChildNode;
        }
        else
        {
            break;
        }
    }

    switch( aOptNode->mType )
    {
        case QLL_PLAN_NODE_TABLE_ACCESS_TYPE :
        case QLL_PLAN_NODE_INDEX_ACCESS_TYPE :
        case QLL_PLAN_NODE_ROWID_ACCESS_TYPE :
        case QLL_PLAN_NODE_CONCAT_TYPE :
            {
                /* add table to list */
                STL_TRY( qlnoAddToNodeList( aBaseTableList,
                                            aOptNode,
                                            aRegionMem,
                                            aEnv )
                         == STL_SUCCESS );
                    
                break;
            }
        case QLL_PLAN_NODE_FAST_DUAL_ACCESS_TYPE :
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case QLL_PLAN_NODE_QUERY_SPEC_TYPE :
            {
                /* Sub Query가 아닌 Query Spec이 분석이 대상이 경우는 없다. */
                STL_DASSERT( 0 );
                break;
            }
        case QLL_PLAN_NODE_QUERY_SET_TYPE:
        case QLL_PLAN_NODE_UNION_ALL_TYPE:
        case QLL_PLAN_NODE_UNION_DISTINCT_TYPE:
        case QLL_PLAN_NODE_EXCEPT_ALL_TYPE:
        case QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE:
        case QLL_PLAN_NODE_INTERSECT_ALL_TYPE:
        case QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE:
            {
                /* add table to list */
                STL_TRY( qlnoAddToNodeList( aBaseTableList,
                                            aOptNode,
                                            aRegionMem,
                                            aEnv )
                         == STL_SUCCESS );
                    
                break;
            }
        case QLL_PLAN_NODE_SUB_QUERY_TYPE:
            {
                /* add table to list */
                STL_TRY( qlnoAddToNodeList( aBaseTableList,
                                            aOptNode,
                                            aRegionMem,
                                            aEnv )
                         == STL_SUCCESS );
                    
                break;
            }
        case QLL_PLAN_NODE_NESTED_LOOPS_TYPE :
        case QLL_PLAN_NODE_MERGE_JOIN_TYPE :
        case QLL_PLAN_NODE_HASH_JOIN_TYPE :
            {
                STL_TRY( qlnoGetRefBaseTableNodeList(
                             aOptNodeList,
                             ((qlnoNestedLoopsJoin *) aOptNode->mOptNode)->mLeftChildNode,
                             aBaseTableList,
                             aRegionMem,
                             aEnv )
                         == STL_SUCCESS );

                return qlnoGetRefBaseTableNodeList(
                    aOptNodeList,
                    ((qlnoNestedLoopsJoin *) aOptNode->mOptNode)->mRightChildNode,
                    aBaseTableList,
                    aRegionMem,
                    aEnv );
            }
        case QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE :
        case QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE :
        case QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE :
        case QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE :
        case QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE :
        case QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE :
        case QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE :
            {
                /* add table to list */
                STL_TRY( qlnoAddToNodeList( aBaseTableList,
                                            aOptNode,
                                            aRegionMem,
                                            aEnv )
                         == STL_SUCCESS );
                    
                break;
            }
        case QLL_PLAN_NODE_HASH_AGGREGATION_TYPE :
            {
                return qlnoGetRefBaseTableNodeList(
                    aOptNodeList,
                    ((qlnoAggregation *) aOptNode->mOptNode)->mChildNode,
                    aBaseTableList,
                    aRegionMem,
                    aEnv );
            }
        case QLL_PLAN_NODE_GROUP_TYPE :
            {
                return qlnoGetRefBaseTableNodeList(
                    aOptNodeList,
                    ((qlnoGroup *) aOptNode->mOptNode)->mChildNode,
                    aBaseTableList,
                    aRegionMem,
                    aEnv );
            }
        default :
            {
                STL_DASSERT( 0 );
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
                      "qlnoGetRefBaseTableNodeList()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Code Plan을 위한 Bind Param 정보를 구축한다.
 * @param[in]  aSQLStmt         SQL Statement 객체
 * @param[out] aBindParamInfo   Bind Parameter Info
 * @param[in]  aRegionMem       Region Memory
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlnoCreateBindParamInfo( qllStatement       * aSQLStmt,
                                   qllBindParamInfo  ** aBindParamInfo,
                                   knlRegionMem       * aRegionMem,
                                   qllEnv             * aEnv )
{
    qllBindParamInfo   * sBindParamInfo = NULL;
    knlValueBlockList  * sValueBlock    = NULL;
    stlInt32             sLoop          = 0;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBindParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Bind Param Info 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qllBindParamInfo ),
                                (void **) & sBindParamInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Bind Param Info 설정
     */

    if( aSQLStmt->mBindContext != NULL )
    {
        if( aSQLStmt->mBindCount > 0 )
        {
            /* Data Type Info */
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( dtlDataTypeInfo ) * aSQLStmt->mBindCount,
                                        (void **) & sBindParamInfo->mBindDataTypeInfo,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( sLoop = 0 ; sLoop < aSQLStmt->mBindCount ; sLoop++ )
            {
                if( aSQLStmt->mBindContext->mMap[ sLoop ]->mBindType == KNL_BIND_TYPE_IN )
                {
                    STL_TRY( knlGetParamValueBlock( aSQLStmt->mBindContext,
                                                    sLoop + 1,
                                                    & sValueBlock,
                                                    NULL,
                                                    KNL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( knlGetParamValueBlock( aSQLStmt->mBindContext,
                                                    sLoop + 1,
                                                    NULL,
                                                    & sValueBlock,
                                                    KNL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                }

                sBindParamInfo->mBindDataTypeInfo[ sLoop ].mDataType =
                    KNL_GET_BLOCK_DB_TYPE( sValueBlock );
                sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum1 =
                    KNL_GET_BLOCK_ARG_NUM1( sValueBlock );
                sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum2 = 0;
                sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum3 = 0;

                switch( KNL_GET_BLOCK_DB_TYPE( sValueBlock ) )
                {
                    case DTL_TYPE_FLOAT :
                    case DTL_TYPE_NUMBER :
                    case DTL_TYPE_DECIMAL :
                        {
                            sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum2 =
                                (stlInt64) KNL_GET_BLOCK_ARG_NUM2( sValueBlock );
                            break;
                        }
                    case DTL_TYPE_CHAR :
                    case DTL_TYPE_VARCHAR :
                        {
                            sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum3 =
                                (stlInt64) KNL_GET_BLOCK_STRING_LENGTH_UNIT( sValueBlock );

                            STL_DASSERT( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum3 >= 0 );
                            STL_DASSERT( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum3 <
                                         DTL_STRING_LENGTH_UNIT_MAX );
                            break;
                        }
 
                    case DTL_TYPE_INTERVAL_YEAR_TO_MONTH :
                    case DTL_TYPE_INTERVAL_DAY_TO_SECOND :
                        {
                            sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum2 =
                                (stlInt64) KNL_GET_BLOCK_ARG_NUM2( sValueBlock );
                            sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum3 =
                                (stlInt64) KNL_GET_BLOCK_INTERVAL_INDICATOR( sValueBlock );

                            STL_DASSERT( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum3 >= 0 );
                            STL_DASSERT( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum3 <
                                         DTL_INTERVAL_INDICATOR_MAX );
                            break;
                        }
                    default :
                    {
                        sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum2 = 0;
                        break;
                    }
                }
            }
        }
        else
        {
            sBindParamInfo->mBindDataTypeInfo = NULL;
        }
    }
    else
    {
        sBindParamInfo->mBindDataTypeInfo = NULL;
    }
    
    
    /**
     * OUTPUT 설정
     */

    *aBindParamInfo = sBindParamInfo;


    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Code Plan에서 사용된 Bind Param 정보와 Bind Context가 부합하는지를 확인한다.
 * @param[in]  aBindCount       Bind Parameter 개수 
 * @param[in]  aBindContext     Bind Context
 * @param[in]  aCodePlan        Code Plan
 * @param[out] aIsValid         Bind Parameter 유효성
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlnoValidateParamInfo( stlInt32               aBindCount,
                                 knlBindContext       * aBindContext,
                                 qllOptimizationNode  * aCodePlan,
                                 stlBool              * aIsValid,
                                 qllEnv               * aEnv )
{
    qllBindParamInfo   * sBindParamInfo = NULL;
    knlValueBlockList  * sValueBlock    = NULL;
    stlInt32             sLoop          = 0;
    stlBool              sIsValid       = STL_TRUE;
    

    /**
     * Bind Parameter 정보가 유효한지 확인
     */
    
    if( aBindCount > 0 )
    {
        STL_PARAM_VALIDATE( aBindContext != NULL, QLL_ERROR_STACK(aEnv) );
        STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );

        sBindParamInfo = aCodePlan->mBindParamInfo;

        for( sLoop = 0 ; sLoop < aBindCount ; sLoop++ )
        {
            if( aBindContext->mMap[ sLoop ]->mBindType == KNL_BIND_TYPE_IN )
            {
                STL_TRY( knlGetParamValueBlock( aBindContext,
                                                sLoop + 1,
                                                & sValueBlock,
                                                NULL,
                                                KNL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
            }
            else
            {
                STL_TRY( knlGetParamValueBlock( aBindContext,
                                                sLoop + 1,
                                                NULL,
                                                & sValueBlock,
                                                KNL_ERROR_STACK(aEnv) )
                         == STL_SUCCESS );
            }

            if( ( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mDataType ==
                  KNL_GET_BLOCK_DB_TYPE( sValueBlock ) ) &&
                ( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum1 ==
                  KNL_GET_BLOCK_ARG_NUM1( sValueBlock ) ) )
            {
                switch( KNL_GET_BLOCK_DB_TYPE( sValueBlock ) )
                {
                    case DTL_TYPE_FLOAT :
                    case DTL_TYPE_NUMBER :
                    case DTL_TYPE_DECIMAL :
                        {
                            STL_DASSERT( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum3 == 0 );
                            
                            sIsValid = ( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum2 ==
                                         (stlInt64) KNL_GET_BLOCK_ARG_NUM2( sValueBlock ) );
                            break;
                        }
                    case DTL_TYPE_CHAR :
                    case DTL_TYPE_VARCHAR :
                        {
                            STL_DASSERT( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum3 >= 0 );
                            STL_DASSERT( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum3 <
                                         DTL_STRING_LENGTH_UNIT_MAX );

                            STL_DASSERT( (stlInt64) KNL_GET_BLOCK_INTERVAL_INDICATOR( sValueBlock ) >= 0 );
                            STL_DASSERT( (stlInt64) KNL_GET_BLOCK_INTERVAL_INDICATOR( sValueBlock ) <
                                         DTL_STRING_LENGTH_UNIT_MAX );
                            
                            sIsValid = ( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum3 ==
                                         (stlInt64) KNL_GET_BLOCK_STRING_LENGTH_UNIT( sValueBlock ) );
                            break;
                        }
 
                    case DTL_TYPE_INTERVAL_YEAR_TO_MONTH :
                    case DTL_TYPE_INTERVAL_DAY_TO_SECOND :
                        {
                            sIsValid = ( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum2 ==
                                         (stlInt64) KNL_GET_BLOCK_ARG_NUM2( sValueBlock ) );

                            if( sIsValid == STL_TRUE )
                            {
                                STL_DASSERT( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum3 >= 0 );
                                STL_DASSERT( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum3 <
                                             DTL_INTERVAL_INDICATOR_MAX );

                                STL_DASSERT( (stlInt64) KNL_GET_BLOCK_INTERVAL_INDICATOR( sValueBlock ) >= 0 );
                                STL_DASSERT( (stlInt64) KNL_GET_BLOCK_INTERVAL_INDICATOR( sValueBlock ) <
                                             DTL_INTERVAL_INDICATOR_MAX );
                                
                                sIsValid = ( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum3 ==
                                             (stlInt64) KNL_GET_BLOCK_INTERVAL_INDICATOR( sValueBlock ) );
                            }
                            break;
                        }
                    default :
                        {
                            STL_DASSERT( sBindParamInfo->mBindDataTypeInfo[ sLoop ].mArgNum3 == 0 );
                            
                            break;
                        }
                }

                if( sIsValid == STL_FALSE )
                {
                    break;
                }
                else
                {
                    /* Do Nothing */
                }
            }
            else
            {
                sIsValid = STL_FALSE;
                break;
            }
        }
    }
    else
    {
        /* Do Nothing */
    }

        
    /**
     * OUTPUT 설정
     */

    *aIsValid = sIsValid;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Code Plan에서 사용된 Bind Type과 User Bind Type이 부합하는지를 확인한다.
 * @param[in]  aSQLStmt     SQL Statement
 * @param[in]  aInitPlan    Init Plan
 * @param[out] aIsValid     Is Valid Init Plan
 * @param[in]  aEnv         Environment
 * @remarks
 */
stlStatus qlnoValidateParamType( qllStatement  * aSQLStmt,
                                 qlvInitPlan   * aInitPlan,
                                 stlBool       * aIsValid,
                                 qllEnv        * aEnv )
{
    stlBool     sIsValid = STL_TRUE;
    stlInt32    sLoop;
    knlBindType sUserBindType;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIsValid != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Bind Parameter 정보가 유효한지 확인
     */

    for( sLoop = 0; sLoop < aInitPlan->mBindCount; sLoop++ )
    {
        STL_TRY( knlGetBindParamType( aSQLStmt->mBindContext,
                                      sLoop + 1,
                                      & sUserBindType,
                                      KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
        
        switch( aInitPlan->mBindIOType[sLoop] )
        {
            case KNL_BIND_TYPE_IN:
                sIsValid = ( (sUserBindType == KNL_BIND_TYPE_IN) ||
                             (sUserBindType == KNL_BIND_TYPE_INOUT) );
                break;
            case KNL_BIND_TYPE_OUT:
                sIsValid = ( (sUserBindType == KNL_BIND_TYPE_OUT) ||
                             (sUserBindType == KNL_BIND_TYPE_INOUT) );
                break;
            default:
                STL_DASSERT( 0 );
                break;
        }

        if( sIsValid == STL_FALSE )
        {
            break;
        }
    }

    *aIsValid = sIsValid;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Join Node의 Read Column들을 정렬한다.
 * @param[in]       aRegionMem      Region Memory
 * @param[in,out]   aReadColumnList Read Column List
 * @param[in]       aEnv            Environment
 */
stlStatus qlnoAdjustJoinReadColumnOrder( knlRegionMem   * aRegionMem,
                                         qlvRefExprList * aReadColumnList,
                                         qllEnv         * aEnv )
{
    stlBool               sIsLess;
    stlInt32              i;
    stlInt32              j;
    stlInt32              sReadColumnCount;
    qlvInitExpression   * sInitExpr             = NULL;
    qlvRefExprItem      * sRefExprItem          = NULL;
    qlvRefExprItem     ** sReadColumnPtrArr     = NULL;

    typedef struct _qlnoColumnInfo
    {
        stlInt8       mTableType;   /**< 0: Base Table, 1: Sub Table */
        stlChar       mPadding[7];
        stlInt64      mTableID;
        stlChar     * mTableName;
        stlInt64      mColumnID;
    } _qlnoColumnInfo;

    _qlnoColumnInfo     * sColumnInfoArr        = NULL;


#define _QLNO_BASE_TABLE_TYPE   0
#define _QLNO_SUB_TABLE_TYPE    1
#define _QLNO_SUB_TABLE_ID      0
#define _QLNO_ROWID_COLUMN_ID   -1

#define _QLNO_IS_LESS_THAN_ORDER( _aLeftColumnInfo, _aRightColumnInfo, _aIsLess )       \
    {                                                                                   \
        stlInt32      sDiff;                                                            \
                                                                                        \
        if( (_aLeftColumnInfo)->mTableType == (_aRightColumnInfo)->mTableType )         \
        {                                                                               \
            sDiff = stlStrcmp( (_aLeftColumnInfo)->mTableName,                          \
                               (_aRightColumnInfo)->mTableName );                       \
            if( sDiff == 0 )                                                            \
            {                                                                           \
                STL_DASSERT( (_aLeftColumnInfo)->mColumnID !=                           \
                             (_aRightColumnInfo)->mColumnID );                          \
                (_aIsLess) =                                                            \
                    ( (_aLeftColumnInfo)->mColumnID < (_aRightColumnInfo)->mColumnID    \
                      ? STL_TRUE: STL_FALSE );                                          \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                (_aIsLess) = (sDiff < 0 ? STL_TRUE : STL_FALSE);                        \
            }                                                                           \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            (_aIsLess) =                                                                \
                ( (_aLeftColumnInfo)->mTableType < (_aRightColumnInfo)->mTableType      \
                  ? STL_TRUE : STL_FALSE );                                             \
        }                                                                               \
    }

#define _QLNO_SWAP_READ_COLUMN_PTR( _aReadColumnPtr1, _aReadColumnPtr2 )\
    {                                                                   \
        qlvRefExprItem  * _sTmpReadColumnPtr    = NULL;                 \
        _sTmpReadColumnPtr = (_aReadColumnPtr1);                        \
        (_aReadColumnPtr1) = (_aReadColumnPtr2);                        \
        (_aReadColumnPtr2) = _sTmpReadColumnPtr;                        \
    }

#define _QLNO_SWAP_COLUMN_INFO( _aColumnInfo1, _aColumnInfo2 )  \
    {                                                           \
        _qlnoColumnInfo   _sTmpColumnInfo;                      \
        _sTmpColumnInfo = (_aColumnInfo1);                      \
        (_aColumnInfo1) = (_aColumnInfo2);                      \
        (_aColumnInfo2) = _sTmpColumnInfo;                      \
    }

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReadColumnList != NULL, QLL_ERROR_STACK(aEnv) );


    /* Read Column Count가 1개이면 Skip */
    STL_TRY_THROW( aReadColumnList->mCount > 0, RAMP_FINISH );

    /* Read Column Count 설정 */
    sReadColumnCount = aReadColumnList->mCount;

    /* Read Column Pointer Array 할당 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvRefExprItem* ) * sReadColumnCount,
                                (void**) &sReadColumnPtrArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Column Info Array 할당 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( _qlnoColumnInfo ) * sReadColumnCount,
                                (void**) &sColumnInfoArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Read Column을 Read Column Pointer Array에 설정 및 Column Info Array 설정 */
    i = 0;
    sRefExprItem = aReadColumnList->mHead;
    while( sRefExprItem != NULL )
    {
        sReadColumnPtrArr[i] = sRefExprItem;

        sInitExpr = sRefExprItem->mExprPtr;
        while( STL_TRUE )
        {
            STL_DASSERT( sInitExpr != NULL );
            if( sInitExpr->mType == QLV_EXPR_TYPE_COLUMN )
            {
                qlvInitBaseTableNode    * sBaseTableNode    = NULL;

                sBaseTableNode = (qlvInitBaseTableNode*)(sInitExpr->mExpr.mColumn->mRelationNode);
                sColumnInfoArr[i].mTableType = _QLNO_BASE_TABLE_TYPE;
                sColumnInfoArr[i].mTableID = ellGetTableID( &(sBaseTableNode->mTableHandle) );
                sColumnInfoArr[i].mTableName = sBaseTableNode->mRelationName->mTable;
                sColumnInfoArr[i].mColumnID = ellGetColumnID( sInitExpr->mExpr.mColumn->mColumnHandle );

                break;
            }
            else if( sInitExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN )
            {
                qlvInitBaseTableNode    * sBaseTableNode    = NULL;

                sBaseTableNode = (qlvInitBaseTableNode*)(sInitExpr->mExpr.mRowIdColumn->mRelationNode);
                sColumnInfoArr[i].mTableType = _QLNO_BASE_TABLE_TYPE;
                sColumnInfoArr[i].mTableID = ellGetTableID( &(sBaseTableNode->mTableHandle) );
                sColumnInfoArr[i].mTableName = sBaseTableNode->mRelationName->mTable;
                sColumnInfoArr[i].mColumnID = _QLNO_ROWID_COLUMN_ID;

                break;
            }
            else
            {
                STL_DASSERT( sInitExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
                if( sInitExpr->mExpr.mInnerColumn->mRelationNode->mType == QLV_NODE_TYPE_SUB_TABLE )
                {
                    qlvInitSubTableNode * sSubTableNode = NULL;

                    sSubTableNode = (qlvInitSubTableNode*)(sInitExpr->mExpr.mInnerColumn->mRelationNode);
                    sColumnInfoArr[i].mTableType = _QLNO_SUB_TABLE_TYPE;
                    sColumnInfoArr[i].mTableID = _QLNO_SUB_TABLE_ID;
                    sColumnInfoArr[i].mTableName = sSubTableNode->mRelationName->mTable;
                    sColumnInfoArr[i].mColumnID = *(sInitExpr->mExpr.mInnerColumn->mIdx);

                    break;
                }
                else
                {
                    sInitExpr = sInitExpr->mExpr.mInnerColumn->mOrgExpr;
                }
            }
        }

        i++;
        sRefExprItem = sRefExprItem->mNext;
    }

    /* Read Column List 초기화 */
    aReadColumnList->mCount = 0;
    aReadColumnList->mHead = NULL;
    aReadColumnList->mTail = NULL;

    /* Read Column Pointer Array를 순차적으로 돌며, 하나씩 Read Column List에 정렬순서로 넣기 */
    for( i = 1; i < sReadColumnCount; i++ )
    {
        for( j = i; j > 0; j-- )
        {
            _QLNO_IS_LESS_THAN_ORDER( &sColumnInfoArr[j], &sColumnInfoArr[j-1], sIsLess );
            if( sIsLess == STL_TRUE )
            {
                _QLNO_SWAP_READ_COLUMN_PTR( sReadColumnPtrArr[j], sReadColumnPtrArr[j-1] );
                _QLNO_SWAP_COLUMN_INFO( sColumnInfoArr[j], sColumnInfoArr[j-1] );
            }
        }
    }

    /* Read Column을 List에 넣기 */
    for( i = 0; i < sReadColumnCount; i++ )
    {
        /* Expression이 Inner Column일 경우 Idx 변경 */
        if( sReadColumnPtrArr[i]->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
        {
            *(sReadColumnPtrArr[i]->mExprPtr->mExpr.mInnerColumn->mIdx) = i;
        }

        /* Next Pointer 초기화 */
        sReadColumnPtrArr[i]->mNext = NULL;

        /* Read Column List에 추가 */
        if( aReadColumnList->mCount == 0 )
        {
            aReadColumnList->mHead = sReadColumnPtrArr[i];
            aReadColumnList->mTail = sReadColumnPtrArr[i];
        }
        else
        {
            aReadColumnList->mTail->mNext = sReadColumnPtrArr[i];
            aReadColumnList->mTail = sReadColumnPtrArr[i];
        }
        aReadColumnList->mCount++;
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 두 Data Type에 대하여 Compare Type을 찾는다.
 * @param[in]   aSQLString      SQL String
 * @param[in]   aDataType1      Left DataType
 * @param[in]   aDataType2      Right DataType
 * @param[in]   aIsConst1       Left의 Constant 여부
 * @param[in]   aIsConst2       Right의 Constant 여부
 * @param[in]   aExprPos        Expression의 Position
 * @param[out]  aCompareType    Compare Type
 * @param[in]   aEnv            Environment
 */
stlStatus qlnoGetCompareType( stlChar               * aSQLString,
                              dtlDataType             aDataType1,
                              dtlDataType             aDataType2,
                              stlBool                 aIsConst1,
                              stlBool                 aIsConst2,
                              stlInt32                aExprPos,
                              const dtlCompareType ** aCompareType,
                              qllEnv                * aEnv )
{
    stlChar * sLastError    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataType1 != DTL_TYPE_NA, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataType2 != DTL_TYPE_NA, QLL_ERROR_STACK(aEnv) );


    STL_TRY( dtlGetCompareConversionInfo( aIsConst1,
                                          aIsConst2,
                                          aDataType1,
                                          aDataType2,
                                          aCompareType,
                                          QLL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    if( aSQLString != NULL )
    {
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );

        if( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage(
                QLL_ERROR_STACK(aEnv),
                qlgMakeErrPosString( aSQLString,
                                     aExprPos,
                                     aEnv ) );
        }
    }

    return STL_FAILURE;
}


/**
 * @brief 원본 string과 동일한 string을 할당받아 설정한다.
 * @param[in]   aSrcString      Source String
 * @param[out]  aResultString   Result String
 * @param[in]   aRegionMem      Region Memory
 * @param[in]   aEnv            Environment
 */
stlStatus qlnoAllocAndCopyString( stlChar        * aSrcString,
                                  stlChar       ** aResultString,
                                  knlRegionMem   * aRegionMem,
                                  qllEnv         * aEnv )
{
    stlChar  * sResultString = NULL;
    stlInt32   sStrLen       = 0;

    
    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aSrcString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * 공간 할당 및 설정
     */
    
    sStrLen = stlStrlen( aSrcString );

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                sStrLen + 1,
                                (void **) & sResultString,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlStrcpy( sResultString, aSrcString );

    
    /**
     * OUTPUT 설정
     */
    
    *aResultString = sResultString;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} qlno */
