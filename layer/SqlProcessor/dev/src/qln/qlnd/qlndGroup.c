/*******************************************************************************
 * qlndGroup.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndGroup.c 10602 2013-12-05 06:56:59Z jhkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndGroup.c
 * @brief SQL Processor Layer Data Optimization Node - GROUP
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief GROUP Execution node에 대한 Data 정보를 구축한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in,out]  aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Data Optimization 단계에서 수행한다.
 *    <BR> Execution Node를 생성하고, Execution을 위한 정보를 구축한다.
 *    <BR> 공간 할당시 Data Plan 메모리 관리자를 이용한다.
 *    <BR> 상위 Query Node를 가지지 않는다.
 */
stlStatus qlndDataOptimizeGroup( smlTransId              aTransID,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 qllEnv                * aEnv )
{
    qllExecutionNode   * sExecNode       = NULL;
    qlnoGroup          * sOptGroup       = NULL;
    qlnxGroup          * sExeGroup       = NULL;

    knlValueBlockList  * sNewBlock       = NULL;
    knlValueBlockList  * sCurBlock       = NULL;
    knlValueBlockList  * sLastTableBlock = NULL;
    knlValueBlockList  * sInsertColBlock = NULL;

    dtlDataValue       * sDataValue      = NULL;
    stlInt64             sBufferSize     = 0;
    stlInt64             sLoop           = 0;


    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * GROUP Optimization Node 획득
     */

    sOptGroup = (qlnoGroup *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptGroup->mChildNode,
                        aDataArea,
                        aEnv );


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

    
    /**
     * GROUP INSTANT ACCESS Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxGroup ),
                                (void **) & sExeGroup,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeGroup, 0x00, STL_SIZEOF( qlnxGroup ) );


    /**
     * Iterator를 위한 Group Key Value Block List 구성
     */

    STL_DASSERT( sOptGroup->mInstantNode->mKeyColList != NULL );
    
    STL_DASSERT(  sOptGroup->mInstantNode->mKeyColList->mCount > 0 );

    STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                         sOptGroup->mInstantNode->mKeyColList,
                                         STL_TRUE,
                                         & sExeGroup->mKeyColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );


    /**
     * Iterator를 위한 Record Value Block List 구성
     */

    STL_DASSERT( sOptGroup->mInstantNode->mRecColList != NULL );
    
    if( sOptGroup->mInstantNode->mRecColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptGroup->mInstantNode->mRecColList,
                                             STL_TRUE,
                                             & sExeGroup->mRecColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeGroup->mRecColBlock = NULL;
    }


    /**
     * Iterator를 위한 Read Value Block List 구성
     */

    STL_DASSERT( sOptGroup->mInstantNode->mReadColList != NULL );
    
    if( sOptGroup->mInstantNode->mReadColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptGroup->mInstantNode->mReadColList,
                                             STL_TRUE,
                                             & sExeGroup->mReadColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeGroup->mReadColBlock = NULL;
    }

    
    /**
     * Total Value Block List 설정
     */

    sLastTableBlock = NULL;

    sCurBlock = sExeGroup->mKeyColBlock;
    while( sCurBlock != NULL )
    {
        STL_TRY( knlInitShareBlock( & sNewBlock,
                                    sCurBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        if( sLastTableBlock == NULL )
        {
            sExeGroup->mTotalColBlock = sNewBlock;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock );
        }
        sLastTableBlock = sNewBlock;

        sCurBlock = sCurBlock->mNext;
    }

    STL_DASSERT( sLastTableBlock != NULL );

    sCurBlock = sExeGroup->mRecColBlock;
    while( sCurBlock != NULL )
    {
        STL_TRY( knlInitShareBlock( & sNewBlock,
                                    sCurBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock );
        sLastTableBlock = sNewBlock;

        sCurBlock = sCurBlock->mNext;
    }


    /**
     * switch total value block list
     */

    sLastTableBlock = NULL;

    sCurBlock = sExeGroup->mTotalColBlock;
    while( sCurBlock != NULL )
    {
        if( KNL_GET_BLOCK_IS_SEP_BUFF( sNewBlock ) == STL_TRUE )
        {
            STL_TRY( qlndCopyBlock( & aSQLStmt->mLongTypeValueList,
                                    & sNewBlock,
                                    STL_LAYER_SQL_PROCESSOR,
                                    aDataArea->mBlockAllocCnt,
                                    sCurBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( knlInitShareBlock( & sNewBlock,
                                        sCurBlock,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
        
        if( sLastTableBlock == NULL )
        {
            sExeGroup->mSwitchBlock = sNewBlock;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock );
        }
        sLastTableBlock = sNewBlock;

        sCurBlock = sCurBlock->mNext;
    }


    /**
     * switch record column value block list
     */

    sCurBlock = sExeGroup->mSwitchBlock;
    for( sLoop = 0 ; sLoop < sOptGroup->mInstantNode->mKeyColList->mCount ; sLoop++ )
    {
        sCurBlock = sCurBlock->mNext;
    }
    sExeGroup->mSwitchRecBlock = sCurBlock;


    /**
     * Logical Filter 구성
     */

    if( sOptGroup->mPredicate == NULL )
    {
        sExeGroup->mFilterExprEvalInfo = NULL;
    }
    else
    {
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprEvalInfo ),
                                    (void **) & sExeGroup->mFilterExprEvalInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        sExeGroup->mFilterExprEvalInfo->mExprStack   = sOptGroup->mLogicalStack;
        sExeGroup->mFilterExprEvalInfo->mContext     = aDataArea->mExprDataContext;
        sExeGroup->mFilterExprEvalInfo->mResultBlock = aDataArea->mPredResultBlock;
        sExeGroup->mFilterExprEvalInfo->mBlockIdx    = 0;
        sExeGroup->mFilterExprEvalInfo->mBlockCount  = 0;
    }


    /**
     * Common Info 설정
     */

    sExeGroup->mCommonInfo.mResultColBlock = sExeGroup->mReadColBlock;
    sExeGroup->mCommonInfo.mRowBlockList   = NULL;


    /**
     * Outer Column Value Block List 설정
     */

    STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                         sOptGroup->mOuterColumnList,
                                         & sExeGroup->mCommonInfo.mOuterColBlock,
                                         & sExeGroup->mCommonInfo.mOuterOrgColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );


    /***************************************************************************
     * Aggregation 정보 구성
     **************************************************************************/

    sExeGroup->mHasAggr = ( sOptGroup->mAggrFuncCnt > 0 );
    

    /**
     *  Aggregation Functions Execution 정보 설정
     */
    
    if( sOptGroup->mAggrFuncCnt > 0 )
    {
        /* fetch & aggr */
        STL_TRY( qlnfGetAggrFuncExecInfo( aSQLStmt,
                                          sOptGroup->mAggrFuncCnt,
                                          sOptGroup->mAggrOptInfo,
                                          aDataArea,
                                          & sExeGroup->mAggrExecInfo,
                                          & QLL_DATA_PLAN( aDBCStmt ),
                                          aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     *  Aggregation Distinct Functions Execution 정보 설정
     */

    if( sOptGroup->mAggrDistFuncCnt > 0 )
    {
        /* fetch & aggr */
        STL_TRY( qlnfGetAggrDistFuncExecInfo( aSQLStmt,
                                              sOptGroup->mAggrDistFuncCnt,
                                              sOptGroup->mAggrDistOptInfo,
                                              aDataArea,
                                              & sExeGroup->mAggrDistExecInfo,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                 == STL_SUCCESS );

        /* insert column list 앞부분에 key column 추가 */
        for( sLoop = 0 ; sLoop < sOptGroup->mAggrDistFuncCnt ; sLoop++ )
        {
            sLastTableBlock = NULL;
            
            sCurBlock = sExeGroup->mKeyColBlock;
            while( sCurBlock != NULL )
            {
                STL_TRY( knlInitShareBlock( & sNewBlock,
                                            sCurBlock,
                                            & QLL_DATA_PLAN( aDBCStmt ),
                                            KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
        
                if( sLastTableBlock == NULL )
                {
                    sInsertColBlock = sNewBlock;
                }
                else
                {
                    KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock );
                }
                sLastTableBlock = sNewBlock;

                sCurBlock = sCurBlock->mNext;
            }

            KNL_LINK_BLOCK_LIST( sLastTableBlock, sExeGroup->mAggrDistExecInfo[ sLoop ].mInsertColList );
            
            sExeGroup->mAggrDistExecInfo[ sLoop ].mInsertColList = sInsertColBlock;
        }
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Group Value 정보
     */

    /* Group Column Count */
    sExeGroup->mGroupColCount = sOptGroup->mInstantNode->mKeyColList->mCount;

    /* Group 대표값들 : array */
    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( dtlDataValue ) * sExeGroup->mGroupColCount,
                                (void **) & sExeGroup->mGroupColValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sExeGroup->mGroupColValue, 0x00, STL_SIZEOF( dtlDataValue ) * sExeGroup->mGroupColCount );

    sCurBlock  = sExeGroup->mKeyColBlock;
    sDataValue = & sExeGroup->mGroupColValue[ 0 ];
    while( sCurBlock != NULL )
    {
        STL_TRY( qlndAllocDataValueArray( & aSQLStmt->mLongTypeValueList,
                                          sDataValue,
                                          KNL_GET_BLOCK_DB_TYPE( sCurBlock ),
                                          KNL_GET_BLOCK_ARG_NUM1( sCurBlock ),
                                          KNL_GET_BLOCK_STRING_LENGTH_UNIT( sCurBlock ),
                                          1,
                                          & QLL_DATA_PLAN( aDBCStmt ),
                                          & sBufferSize,
                                          aEnv )
                 == STL_SUCCESS );
        
        sCurBlock = sCurBlock->mNext;
        sDataValue++;
    }

        
    /**
     * Row Block 공간 할당
     */

    STL_TRY( smlInitRowBlock( & sExeGroup->mRowBlock,
                              aDataArea->mBlockAllocCnt,
                              & QLL_DATA_PLAN( aDBCStmt ),
                              SML_ENV(aEnv) )
             == STL_SUCCESS );


    /***************************************************************************
     * Execution Node 설정
     **************************************************************************/

    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_GROUP_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeGroup;

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
        sExecNode->mExecutionCost = NULL;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

    
/** @} qlnd */
