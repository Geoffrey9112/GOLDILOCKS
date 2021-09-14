/*******************************************************************************
 * qlndSortJoinInstantAccess.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndSortJoinInstantAccess.c 10973 2014-01-16 07:17:19Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndSortJoinInstantAccess.c
 * @brief SQL Processor Layer Data Optimization Node - SORT JOIN INSTANT ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief SORT JOIN INSTANT ACCESS Execution node에 대한 Data 정보를 구축한다.
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
stlStatus qlndDataOptimizeSortJoinInstantAccess( smlTransId              aTransID,
                                                 qllDBCStmt            * aDBCStmt,
                                                 qllStatement          * aSQLStmt,
                                                 qllOptimizationNode   * aOptNode,
                                                 qllDataArea           * aDataArea,
                                                 qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode       = NULL;
    qlnoInstant       * sOptSortInstant = NULL;
    qlnxInstant       * sExeSortInstant = NULL;
    smlFetchInfo      * sFetchInfo      = NULL;
    qlnxCommonInfo    * sCommonInfo     = NULL;

    knlValueBlockList   sTempColBlock;
    knlValueBlockList * sLastColBlock   = NULL;
    knlValueBlockList * sNewBlock       = NULL;
    knlValueBlockList * sCurBlock       = NULL;
    knlValueBlockList * sLastTableBlock = NULL;
    qlnxRowBlockItem  * sRowBlockItem   = NULL;
    smlRowBlock       * sRowBlock       = NULL;
    dtlDataValue      * sScnValue       = NULL;
    dtlDataValue      * sRidValue       = NULL;
    stlInt32            sColOrder       = 0;
    stlInt32            sLoop           = 0;
    stlInt32            sRowIdx         = 0;
    stlInt32            sKeyColCnt      = 0;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * SORT INSTANT ACCESS Optimization Node 획득
     */

    sOptSortInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptSortInstant->mChildNode,
                        aDataArea,
                        aEnv );


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

    
    /**
     * SORT INSTANT ACCESS Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxInstant ),
                                (void **) & sExeSortInstant,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeSortInstant, 0x00, STL_SIZEOF( qlnxInstant ) );


    /**
     * SORT INSTANT TABLE Iterator 에 대한 Fetch Info 설정
     */

    sFetchInfo = & sExeSortInstant->mFetchInfo;


    /**
     * Read Row Block 구성
     */

    sTempColBlock.mNext = NULL;
    sLastColBlock = & sTempColBlock;
    sCommonInfo = (qlnxCommonInfo*) aDataArea->mExecNodeList[ sOptSortInstant->mChildNode->mIdx ].mExecNode;
        
    if( sCommonInfo->mRowBlockList == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        if( sCommonInfo->mRowBlockList->mCount > 0 )
        {
            sRowBlockItem = sCommonInfo->mRowBlockList->mHead;
            sColOrder     = ( sOptSortInstant->mInstantNode->mKeyColCnt +
                              sOptSortInstant->mInstantNode->mRecColCnt );
            
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
     * Iterator를 위한 Sort Key Value Block List 구성
     */

    STL_DASSERT( sOptSortInstant->mInstantNode->mKeyColList != NULL );
    
    STL_DASSERT( sOptSortInstant->mInstantNode->mKeyColList->mCount > 0 );

    STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                         sOptSortInstant->mInstantNode->mKeyColList,
                                         STL_TRUE,
                                         & sExeSortInstant->mKeyColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );


    /**
     * Iterator를 위한 Record Value Block List 구성
     */

    STL_DASSERT( sOptSortInstant->mInstantNode->mRecColList != NULL );
    
    if( sOptSortInstant->mInstantNode->mRecColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptSortInstant->mInstantNode->mRecColList,
                                             STL_TRUE,
                                             & sExeSortInstant->mRecColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );

        /* Record Column List의 마지막 Value Block 얻기 */
        sLastColBlock = sExeSortInstant->mRecColBlock;
        while( sLastColBlock->mNext != NULL )
        {
            sLastColBlock = sLastColBlock->mNext;
        }

        /* Read Row Block 연결 */
        sLastColBlock->mNext = sTempColBlock.mNext;
    }
    else
    {
        sExeSortInstant->mRecColBlock = sTempColBlock.mNext;
    }


    /**
     * Iterator를 위한 Read Value Block List 구성
     */

    STL_DASSERT( sOptSortInstant->mInstantNode->mReadColList != NULL );
    
    if( sOptSortInstant->mInstantNode->mReadColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptSortInstant->mInstantNode->mReadColList,
                                             STL_TRUE,
                                             & sExeSortInstant->mReadColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );


        /**
         * Read Column Block을 INDEX와 TABLE로 구분
         */

        sKeyColCnt = sOptSortInstant->mInstantNode->mKeyColList->mCount;
        sCurBlock  = sExeSortInstant->mReadColBlock;
        sExeSortInstant->mIndexReadColBlock = NULL;
        sExeSortInstant->mTableReadColBlock = NULL;
        sLastColBlock = NULL;
        sLastTableBlock = NULL;

        while( sCurBlock != NULL )
        {
            /* Share Block 할당 */
            STL_TRY( knlInitShareBlock( & sNewBlock,
                                        sCurBlock,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        KNL_ENV( aEnv ) )
                     == STL_SUCCESS );

            if( KNL_GET_BLOCK_COLUMN_ORDER( sCurBlock ) < sKeyColCnt )
            {
                /**
                 * Index Read Column Block 설정
                 */

                if( sLastColBlock == NULL )
                {
                    sExeSortInstant->mIndexReadColBlock = sNewBlock;
                }
                else
                {
                    KNL_LINK_BLOCK_LIST( sLastColBlock, sNewBlock );
                }
                sLastColBlock = sNewBlock;
            }
            else
            {
                /**
                 * Table Read Column Block 설정
                 */

                if( sLastTableBlock == NULL )
                {
                    sExeSortInstant->mTableReadColBlock = sCurBlock;
                }
                else
                {
                    KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock );
                }
                sLastTableBlock = sNewBlock;
            }

            sCurBlock = sCurBlock->mNext;
        }
        
        /* Read Column List의 마지막 Value Block 얻기 */
        sLastColBlock = sExeSortInstant->mReadColBlock;
        while( sLastColBlock->mNext != NULL )
        {
            sLastColBlock = sLastColBlock->mNext;
        }

        /* Read Row Block 연결 */
        sLastColBlock->mNext = sTempColBlock.mNext;
        if( sLastTableBlock == NULL )
        {
            sExeSortInstant->mTableReadColBlock = sTempColBlock.mNext;
        }
        else
        {
            sLastTableBlock->mNext = sTempColBlock.mNext;
        }
    }
    else
    {
        sExeSortInstant->mReadColBlock = sTempColBlock.mNext;
    }
    sFetchInfo->mColList = sExeSortInstant->mIndexReadColBlock;


    /**
     * Insert Value Block List 설정
     */

    sLastTableBlock = NULL;

    sCurBlock = sExeSortInstant->mKeyColBlock;
    while( sCurBlock != NULL )
    {
        STL_TRY( knlInitShareBlock( & sNewBlock,
                                    sCurBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        KNL_SET_BLOCK_COLUMN_ORDER( sNewBlock,
                                    KNL_GET_BLOCK_COLUMN_ORDER( sCurBlock ) );

        if( sLastTableBlock == NULL )
        {
            sExeSortInstant->mInsertColBlock = sNewBlock;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock );
        }
        sLastTableBlock = sNewBlock;

        sCurBlock = sCurBlock->mNext;
    }

    STL_DASSERT( sLastTableBlock != NULL );

    sCurBlock = sExeSortInstant->mRecColBlock;
    while( sCurBlock != NULL )
    {
        STL_TRY( knlInitShareBlock( & sNewBlock,
                                    sCurBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        KNL_SET_BLOCK_COLUMN_ORDER( sNewBlock,
                                    KNL_GET_BLOCK_COLUMN_ORDER( sCurBlock ) );

        KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock );
        sLastTableBlock = sNewBlock;

        sCurBlock = sCurBlock->mNext;
    }


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
     * Index 관련 정보 & Fetch Record 설정
     */

    STL_TRY( qlndGetRangePredInfoForSortInstant( aDBCStmt,
                                                 aSQLStmt,
                                                 aDataArea,
                                                 sOptSortInstant,
                                                 sExeSortInstant,
                                                 aEnv )
             == STL_SUCCESS );


    /**
     * Fetch 정보 초기화
     */

    sFetchInfo->mSkipCnt        = 0;
    sFetchInfo->mFetchCnt       = QLL_FETCH_COUNT_MAX;
    sFetchInfo->mIsEndOfScan    = STL_FALSE;
    sFetchInfo->mGroupKeyFetch  = STL_TRUE;


    /**
     * Materialize 정보 초기화
     */

    sExeSortInstant->mNeedMaterialize = STL_TRUE;


    /**
     * Rebuild 정보 설정
     */

    sExeSortInstant->mNeedRebuild = sOptSortInstant->mNeedRebuild;


    /**
     * Prepare Range 정보 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptSortInstant->mPrepareRangeStack,
                                           & sExeSortInstant->mPreRangeEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Prepare Key Filter 정보 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptSortInstant->mPrepareKeyFilterStack,
                                           & sExeSortInstant->mPreKeyFilterEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Prepare Filter 정보 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptSortInstant->mPrepareFilterStack,
                                           & sExeSortInstant->mPreFilterEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Common Info 설정
     */

    sExeSortInstant->mCommonInfo.mResultColBlock   = sExeSortInstant->mReadColBlock;
    sExeSortInstant->mCommonInfo.mRowBlockList     = sCommonInfo->mRowBlockList;


    /**
     * Outer Column Value Block List 설정
     */

    STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                         sOptSortInstant->mOuterColumnList,
                                         & sExeSortInstant->mCommonInfo.mOuterColBlock,
                                         & sExeSortInstant->mCommonInfo.mOuterOrgColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );


    /***************************************************************************
     * Execution Node 설정
     **************************************************************************/

    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeSortInstant;

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
