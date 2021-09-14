/*******************************************************************************
 * qlndFlatInstantAccess.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndFlatInstantAccess.c 10973 2014-01-16 07:17:19Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndFlatInstantAccess.c
 * @brief SQL Processor Layer Data Optimization Node - FLAT INSTANT ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


#if 0
/**
 * @brief FLAT INSTANT ACCESS Execution node에 대한 Data 정보를 구축한다.
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
stlStatus qlndDataOptimizeFlatInstantAccess( smlTransId              aTransID,
                                             qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptimizationNode   * aOptNode,
                                             qllDataArea           * aDataArea,
                                             qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode       = NULL;
    qlnoInstant       * sOptFlatInstant = NULL;
    qlnxInstant       * sExeFlatInstant = NULL;
    smlFetchInfo      * sFetchInfo      = NULL;
    qlnxCommonInfo    * sCommonInfo     = NULL;

    knlValueBlockList   sTempColBlock;
    knlValueBlockList * sLastColBlock   = NULL;
    knlValueBlockList * sNewBlock       = NULL;
    qlnxRowBlockItem  * sRowBlockItem   = NULL;
    smlRowBlock       * sRowBlock       = NULL;
    dtlDataValue      * sScnValue       = NULL;
    dtlDataValue      * sRidValue       = NULL;
    stlInt32            sColOrder       = 0;
    stlInt32            sLoop           = 0;
    stlInt32            sRowIdx         = 0;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * FLAT INSTANT ACCESS Optimization Node 획득
     */

    sOptFlatInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptFlatInstant->mChildNode,
                        aDataArea,
                        aEnv );


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

    
    /**
     * FLAT INSTANT ACCESS Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxInstant ),
                                (void **) & sExeFlatInstant,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeFlatInstant, 0x00, STL_SIZEOF( qlnxInstant ) );


    /**
     * FLAT INSTANT TABLE Iterator 에 대한 Fetch Info 설정
     */

    sFetchInfo = & sExeFlatInstant->mFetchInfo;


    /**
     * Read Row Block 구성
     */

    sTempColBlock.mNext = NULL;
    sLastColBlock = & sTempColBlock;
    sCommonInfo = (qlnxCommonInfo*) aDataArea->mExecNodeList[ sOptFlatInstant->mChildNode->mIdx ].mExecNode;

    if( sCommonInfo->mRowBlockList == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        if( sCommonInfo->mRowBlockList->mCount > 0 )
        {
            sRowBlockItem = sCommonInfo->mRowBlockList->mHead;
            sColOrder     = sOptFlatInstant->mInstantNode->mReadColCnt;
            
            for( sLoop = sCommonInfo->mRowBlockList->mCount ; sLoop > 0 ; sLoop-- )
            {
                /* SCN value block 생성 */
                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & sNewBlock,
                                            aDataArea->mBlockAllocCnt,
                                            STL_TRUE, /* IsSepBuff */
                                            STL_LAYER_SQL_PROCESSOR,
                                            0, /* TableID */
                                            sColOrder,
                                            DTL_TYPE_BINARY,
                                            STL_SIZEOF( smlScn ),
                                            DTL_SCALE_NA,
                                            DTL_STRING_LENGTH_UNIT_NA,
                                            DTL_INTERVAL_INDICATOR_NA,
                                            & QLL_DATA_PLAN( aDBCStmt ),
                                            aEnv )
                         == STL_SUCCESS );

                sScnValue = sNewBlock->mValueBlock->mDataValue;

                /* link */
                KNL_LINK_BLOCK_LIST( sLastColBlock, sNewBlock );
                sLastColBlock = sNewBlock;
                sColOrder++;

                /* RID value block 생성 */
                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & sNewBlock,
                                            aDataArea->mBlockAllocCnt,
                                            STL_TRUE, /* IsSepBuff */
                                            STL_LAYER_SQL_PROCESSOR,
                                            0, /* TableID */
                                            sColOrder,
                                            DTL_TYPE_BINARY,
                                            STL_SIZEOF( smlRid ),
                                            DTL_SCALE_NA,
                                            DTL_STRING_LENGTH_UNIT_NA,
                                            DTL_INTERVAL_INDICATOR_NA,
                                            & QLL_DATA_PLAN( aDBCStmt ),
                                            aEnv )
                         == STL_SUCCESS );

                sRidValue = sNewBlock->mValueBlock->mDataValue;

                /* link */
                KNL_LINK_BLOCK_LIST( sLastColBlock, sNewBlock );
                sLastColBlock = sNewBlock;
                sColOrder++;

                /* Row Block(smlRowBlock)과 Value Block's Data Value 연결 */
                sRowBlock = sRowBlockItem->mRowBlock;
                for( sRowIdx = 0 ; sRowIdx < aDataArea->mBlockAllocCnt ; sRowIdx++ )
                {
                    sScnValue->mValue = & sRowBlock->mScnBlock[ sRowIdx ];
                    sScnValue->mLength = STL_SIZEOF( smlScn );
                    sScnValue++;
            
                    sRidValue->mValue = & sRowBlock->mRidBlock[ sRowIdx ];
                    sRidValue->mLength = STL_SIZEOF( smlRid );
                    sRidValue++;
                }

                sRowBlockItem = sRowBlockItem->mNext;
            }
        }
        else
        {
            /* Do Nothing */
        }
    }


    /**
     * Iterator를 위한 Key Value Block List 구성
     */

    sExeFlatInstant->mKeyColBlock = NULL;


    /**
     * Iterator를 위한 Record Value Block List 구성
     */

    STL_DASSERT( sOptFlatInstant->mInstantNode->mRecColList != NULL );
    
    if( sOptFlatInstant->mInstantNode->mRecColList->mCount > 0 )
    { 
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptFlatInstant->mInstantNode->mRecColList,
                                             STL_TRUE,
                                             & sExeFlatInstant->mRecColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );

        /* Record Column List의 마지막 Value Block 얻기 */
        sLastColBlock = sExeFlatInstant->mRecColBlock;
        while( sLastColBlock->mNext != NULL )
        {
            sLastColBlock = sLastColBlock->mNext;
        }

        /* Read Row Block 연결 */
        sLastColBlock->mNext = sTempColBlock.mNext;
    }
    else
    {
        sExeFlatInstant->mRecColBlock = sTempColBlock.mNext;
    }
    

    /**
     * Iterator를 위한 Read Value Block List 구성
     */

    if( sOptFlatInstant->mInstantNode->mReadColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptFlatInstant->mInstantNode->mReadColList,
                                             STL_TRUE,
                                             & sExeFlatInstant->mReadColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );

        /* Read Column List의 마지막 Value Block 얻기 */
        sLastColBlock = sExeFlatInstant->mReadColBlock;
        while( sLastColBlock->mNext != NULL )
        {
            sLastColBlock = sLastColBlock->mNext;
        }

        /* Read Row Block 연결 */
        sLastColBlock->mNext = sTempColBlock.mNext;
    }
    else
    {
        sExeFlatInstant->mReadColBlock = NULL;
    }
    sFetchInfo->mColList = sExeFlatInstant->mReadColBlock;
    

    /**
     * Index Read Column Block 설정
     */

    sExeFlatInstant->mIndexReadColBlock = NULL;
    
    
    /**
     * Table Read Column Block 설정
     */

    sExeFlatInstant->mTableReadColBlock = sExeFlatInstant->mReadColBlock;

    
    /**
     * Insert Value Block List 설정
     */

    sExeFlatInstant->mInsertColBlock = sExeFlatInstant->mRecColBlock;


    /**
     * Physical Filter 구성
     */
    
    if( sOptFlatInstant->mTablePhysicalFilterExpr == NULL )
    {
        sFetchInfo->mPhysicalFilter = NULL;
    }
    else
    {
        STL_TRY( qlndMakePhysicalFilter( (qlvInitNode *) sOptFlatInstant->mInstantNode,
                                         sOptFlatInstant->mTablePhysicalFilterExpr,
                                         sFetchInfo->mColList,
                                         aDataArea->mExprDataContext,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         & sFetchInfo->mPhysicalFilter,
                                         aEnv )
                 == STL_SUCCESS );
    }
    

    /**
     * Logical Filter 구성
     */

    if( sOptFlatInstant->mPredicate == NULL )
    {
        sFetchInfo->mLogicalFilter  = NULL;
        sFetchInfo->mFilterEvalInfo = NULL;
    }
    else
    {
        sFetchInfo->mLogicalFilter   = sOptFlatInstant->mTableLogicalStack;

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprEvalInfo ),
                                    (void **) & sFetchInfo->mFilterEvalInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        sFetchInfo->mFilterEvalInfo->mExprStack   = sFetchInfo->mLogicalFilter;
        sFetchInfo->mFilterEvalInfo->mContext     = aDataArea->mExprDataContext;
        sFetchInfo->mFilterEvalInfo->mResultBlock = aDataArea->mPredResultBlock;
        sFetchInfo->mFilterEvalInfo->mBlockIdx    = 0;
        sFetchInfo->mFilterEvalInfo->mBlockCount  = 0;
    }


    /**
     * Index 관련 정보 초기화
     */

    sFetchInfo->mRange           = NULL;
    sFetchInfo->mKeyCompList     = NULL;
    sFetchInfo->mFetchRecordInfo = NULL;
    sFetchInfo->mAggrFetchInfo   = NULL;
    sFetchInfo->mBlockJoinFetchInfo   = NULL;

    
    /**
     * Row Block 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( smlRowBlock ),
                                (void **) & sFetchInfo->mRowBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( smlInitRowBlock( sFetchInfo->mRowBlock,
                              aDataArea->mBlockAllocCnt,
                              & QLL_DATA_PLAN( aDBCStmt ),
                              SML_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Fetch 정보 초기화
     */

    sFetchInfo->mSkipCnt         = 0;
    sFetchInfo->mFetchCnt        = QLL_FETCH_COUNT_MAX;
    sFetchInfo->mIsEndOfScan     = STL_FALSE;


    /**
     * Materialize 정보 초기화
     */

    sExeFlatInstant->mNeedMaterialize = STL_TRUE;
    

    /**
     * Common Info 설정
     */

    sExeFlatInstant->mCommonInfo.mResultColBlock   = sExeFlatInstant->mReadColBlock;
    sExeFlatInstant->mCommonInfo.mRowBlockList     = sCommonInfo->mRowBlockList;


    /**
     * Outer Column Value Block List 설정
     */

    STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                         sOptFlatInstant->mOuterColumnList,
                                         & sExeFlatInstant->mCommonInfo.mOuterColBlock,
                                         & sExeFlatInstant->mCommonInfo.mOuterOrgColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );


    /***************************************************************************
     * Execution Node 설정
     **************************************************************************/

    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeFlatInstant;

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
#endif

/** @} qlnd */
