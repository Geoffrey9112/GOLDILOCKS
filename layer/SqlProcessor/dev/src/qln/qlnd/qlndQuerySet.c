/*******************************************************************************
 * qlndQuerySet.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndQuerySet.c 10333 2013-11-15 05:47:10Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndQuerySet.c
 * @brief SQL Processor Layer Data Optimization Node - QUERY SET
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief Query Set 을 Data Optimize 한다.
 * @param[in]  aTransID   Transcation ID
 * @param[in]  aDBCStmt   DBC Statement
 * @param[in]  aSQLStmt   SQL Statement
 * @param[in]  aOptNode   Code Optimization Node
 * @param[in]  aDataArea  Data Area
 * @param[in]  aEnv       Environment
 * @remarks
 */
stlStatus qlndDataOptimizeQuerySet( smlTransId              aTransID,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode = NULL;
    qlnoQuerySet      * sOptQuerySet = NULL;
    qlnxQuerySet      * sExeQuerySet = NULL;

    knlValueBlockList * sPrevBlock = NULL;
    knlValueBlockList * sNewBlock = NULL;
    knlValueBlockList * sCurBlock = NULL;
    
    stlInt32   i = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_QUERY_SET_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    /*********************************************************
     * 기본 정보 획득
     *********************************************************/
    
    /**
     * Optimization Node 획득
     */

    sOptQuerySet = (qlnoQuerySet *) aOptNode->mOptNode;

    /**
     * Common Execution Node 획득
     */
    
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, aOptNode->mIdx );
    
    /*********************************************************
     * Child Data Optimize
     *********************************************************/
    
    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptQuerySet->mChildNode,
                        aDataArea,
                        aEnv );

        
    /*********************************************************
     * QUERY SET node 정보 설정
     *********************************************************/

    /**
     * QUERY SET execution node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxQuerySet ),
                                (void **) & sExeQuerySet,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeQuerySet, 0x00, STL_SIZEOF( qlnxQuerySet ) );

    /**
     * Set Column Block 구성
     */
    
    STL_TRY( qlndBuildRefBlockList( aSQLStmt,
                                    aDataArea,
                                    NULL,  /* statement expression list */
                                    sOptQuerySet->mColumnList,
                                    & sExeQuerySet->mSetColumnBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Set Column 의 Column Order 부여
     */
    
    i = 0;
    sCurBlock = sExeQuerySet->mSetColumnBlock;
    
    while ( sCurBlock != NULL )
    {
        sCurBlock->mColumnOrder = i;

        i++;
        sCurBlock = sCurBlock->mNext;
    }
    
    /**
     * Set OP 는 target block 을 대체하면서 작업을 수행한다.
     * - 따라서, 원복할 Origin Block 을 구성해 둔다.
     */
    sExeQuerySet->mOriginColumnBlock = NULL;
    sCurBlock = sExeQuerySet->mSetColumnBlock;

    while ( sCurBlock != NULL )
    {
        STL_TRY( knlInitShareBlock( & sNewBlock,
                                    sCurBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* column order 부여 */
        sNewBlock->mColumnOrder = sCurBlock->mColumnOrder;
        
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlValueBlock ),
                                    (void **) & sNewBlock->mValueBlock,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemcpy( sNewBlock->mValueBlock, sCurBlock->mValueBlock, STL_SIZEOF( knlValueBlock ) );
        
        if ( sExeQuerySet->mOriginColumnBlock == NULL )
        {
            sExeQuerySet->mOriginColumnBlock = sNewBlock;
        }
        else
        {
            sPrevBlock->mNext = sNewBlock;
        }
        
        sPrevBlock = sNewBlock;
        sCurBlock = sCurBlock->mNext;
    }
    
    /**
     * Result Block 구성 
     */

    STL_TRY( qlndBuildRefBlockList( aSQLStmt,
                                    aDataArea,
                                    NULL,  /* statement expression list */
                                    sOptQuerySet->mTargetList,
                                    & sExeQuerySet->mTargetColBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Result Block 의 Column Order 부여
     */
    
    i = 0;
    sCurBlock = sExeQuerySet->mTargetColBlock;
    
    while ( sCurBlock != NULL )
    {
        sCurBlock->mColumnOrder = i;

        i++;
        sCurBlock = sCurBlock->mNext;
    }
    
    
    /*********************************************************
     * execution node 의 공통 정보 설정
     *********************************************************/

    /**
     * Common Information 설정
     * - Query Set 은 updatable query 가 아니므로 ROWID 등을 구성할 필요가 없다.
     */

    sExeQuerySet->mCommonInfo.mResultColBlock   = sExeQuerySet->mTargetColBlock;
    sExeQuerySet->mCommonInfo.mRowBlockList     = NULL;
    sExeQuerySet->mCommonInfo.mOuterColBlock    = NULL;
    sExeQuerySet->mCommonInfo.mOuterOrgColBlock = NULL;

    
    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType = QLL_PLAN_NODE_QUERY_SET_TYPE;
    sExecNode->mOptNode  = aOptNode;
    sExecNode->mExecNode = sExeQuerySet;

    if( aDataArea->mIsEvaluateCost == STL_TRUE )
    {
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qllExecutionCost ),
                                    (void **) & sExecNode->mExecutionCost,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sExecNode->mExecutionCost, 0x00, STL_SIZEOF( qllExecutionCost ) );
    }
    else
    {
        sExecNode->mExecutionCost  = NULL;
    }
    
    return STL_SUCCESS;
   
    STL_FINISH;

    return STL_FAILURE;
}





/**
 * @brief Set Operation 을 Data Optimize 한다.
 * @param[in]  aTransID   Transcation ID
 * @param[in]  aDBCStmt   DBC Statement
 * @param[in]  aSQLStmt   SQL Statement
 * @param[in]  aOptNode   Code Optimization Node
 * @param[in]  aDataArea  Data Area
 * @param[in]  aEnv       Environment
 * @remarks
 */
stlStatus qlndDataOptimizeSetOP( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode = NULL;
    qlnoSetOP         * sOptSetOP = NULL;
    qlnxSetOP         * sExeSetOP = NULL;

    knlValueBlockList * sCurBlock = NULL;
    
    stlInt32    i = 0;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aOptNode->mType == QLL_PLAN_NODE_UNION_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_UNION_DISTINCT_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_EXCEPT_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_ALL_TYPE) ||
                        (aOptNode->mType == QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );

    /*********************************************************
     * 기본 정보 획득
     *********************************************************/
    
    /**
     * Optimization Node 획득
     */

    sOptSetOP = (qlnoSetOP *) aOptNode->mOptNode;

    /**
     * Common Execution Node 획득
     */
    
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, aOptNode->mIdx );
    
    /*********************************************************
     * Child Data Optimize
     *********************************************************/

    for ( i = 0; i < sOptSetOP->mChildCount; i++ )
    {
        /**
         * 하위 node에 대한 Data Optimize 수행
         */
        
        QLND_DATA_OPTIMIZE( aTransID,
                            aDBCStmt,
                            aSQLStmt,
                            sOptSetOP->mChildNodeArray[i],
                            aDataArea,
                            aEnv );
    }

    /*********************************************************
     * Set Operation node 정보 설정
     *********************************************************/

    /**
     * Set OP execution node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxSetOP ),
                                (void **) & sExeSetOP,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeSetOP, 0x00, STL_SIZEOF( qlnxSetOP ) );

    
    /**
     * Set OP 의 Column Block 구성
     */
    
    STL_TRY( qlndBuildRefBlockList( aSQLStmt,
                                    aDataArea,
                                    NULL,  /* statement expression list */
                                    sOptSetOP->mRootQuerySet->mColumnList,
                                    & sExeSetOP->mColumnBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Column Block 의 Order 부여 
     */
    
    i = 0;
    sCurBlock = sExeSetOP->mColumnBlock;
    
    while ( sCurBlock != NULL )
    {
        sCurBlock->mColumnOrder = i;

        i++;
        sCurBlock = sCurBlock->mNext;
    }

    /**
     * Hash Instant Table에 Insert 시 사용될 Row Block 생성
     */
    
    sExeSetOP->mHashRowBlock.mBlockSize = aDataArea->mBlockAllocCnt;
    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( smlRid ) * aDataArea->mBlockAllocCnt,
                                (void **) & sExeSetOP->mHashRowBlock.mRidBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( smlScn ) * aDataArea->mBlockAllocCnt,
                                (void **) & sExeSetOP->mHashRowBlock.mScnBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Fetch Information 초기화
     */
    
    SML_SET_FETCH_INFO_FOR_TABLE( & sExeSetOP->mFetchInfo,    /* Fetch Info */
                                  NULL,                       /* physical filter */
                                  NULL,                       /* logical filter */
                                  sExeSetOP->mColumnBlock,    /* read column list */
                                  NULL,                       /* rowid column list */
                                  & sExeSetOP->mHashRowBlock, /* row block */
                                  0,                          /* skip count */
                                  0,                          /* fetch count  */
                                  STL_FALSE,                  /* group key fetch */
                                  NULL );                     /* filter evaluate info */
    

    /*********************************************************
     * execution node 의 공통 정보 설정
     *********************************************************/

    /**
     * Common Information 설정
     * - Set Operation 은 updatable query 가 아니므로 ROWID 등을 구성할 필요가 없다.
     */

    sExeSetOP->mCommonInfo.mResultColBlock   = sExeSetOP->mColumnBlock;
    sExeSetOP->mCommonInfo.mRowBlockList     = NULL;
    sExeSetOP->mCommonInfo.mOuterColBlock    = NULL;
    sExeSetOP->mCommonInfo.mOuterOrgColBlock = NULL;

    
    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType = aOptNode->mType;
    sExecNode->mOptNode  = aOptNode;
    sExecNode->mExecNode = sExeSetOP;

    if( aDataArea->mIsEvaluateCost == STL_TRUE )
    {
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qllExecutionCost ),
                                    (void **) & sExecNode->mExecutionCost,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sExecNode->mExecutionCost, 0x00, STL_SIZEOF( qllExecutionCost ) );
    }
    else
    {
        sExecNode->mExecutionCost  = NULL;
    }
    
    return STL_SUCCESS;
   
    STL_FINISH;

    return STL_FAILURE;
}



/** @} qlnd */
