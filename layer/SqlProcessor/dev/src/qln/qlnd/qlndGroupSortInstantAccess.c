/*******************************************************************************
 * qlndGroupSortInstantAccess.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndGroupSortInstantAccess.c 10973 2014-01-16 07:17:19Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndGroupSortInstantAccess.c
 * @brief SQL Processor Layer Data Optimization Node - GROUP INSTANT ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


#if 0
/**
 * @brief GROUP INSTANT ACCESS Execution node에 대한 Data 정보를 구축한다.
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
stlStatus qlndDataOptimizeGroupSortInstantAccess( smlTransId              aTransID,
                                                  qllDBCStmt            * aDBCStmt,
                                                  qllStatement          * aSQLStmt,
                                                  qllOptimizationNode   * aOptNode,
                                                  qllDataArea           * aDataArea,
                                                  qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode        = NULL;
    qlnoInstant       * sOptGroupInstant = NULL;
    qlnxInstant       * sExeGroupInstant = NULL;
    qlnoQuerySpec     * sOptQuerySpec    = NULL;
    smlFetchInfo      * sFetchInfo       = NULL;
    smlFetchInfo      * sAggrFetchInfo   = NULL;

    knlValueBlockList   sTempColBlock;
    knlValueBlockList * sLastColBlock    = NULL;
    knlValueBlockList * sNewBlock        = NULL;
    knlValueBlockList * sCurBlock        = NULL;
    knlValueBlockList * sLastTableBlock  = NULL;
    qlnxRowBlockItem  * sRowBlockItem    = NULL;
    smlRowBlock       * sRowBlock        = NULL;
    dtlDataValue      * sScnValue        = NULL;
    dtlDataValue      * sRidValue        = NULL;
    stlInt32            sColOrder        = 0;
    stlInt32            sLoop            = 0;
    stlInt32            sRowIdx          = 0;
    stlInt32            sKeyColCnt       = 0;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * GROUP INSTANT ACCESS Optimization Node 획득
     */

    sOptGroupInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptGroupInstant->mChildNode,
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
                                STL_SIZEOF( qlnxInstant ),
                                (void **) & sExeGroupInstant,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeGroupInstant, 0x00, STL_SIZEOF( qlnxInstant ) );


    /**
     * GROUP INSTANT TABLE Iterator 에 대한 Fetch Info 설정
     */

    sFetchInfo = & sExeGroupInstant->mFetchInfo;


    /**
     * Common Info 설정
     */

    sExeGroupInstant->mCommonInfo.mResultColBlock =
        ((qlnxCommonInfo*)(aDataArea->mExecNodeList[ sOptGroupInstant->mChildNode->mIdx ].mExecNode))->mResultColBlock;
    sExeGroupInstant->mCommonInfo.mRowBlockList =
        ((qlnxCommonInfo*)(aDataArea->mExecNodeList[ sOptGroupInstant->mChildNode->mIdx ].mExecNode))->mRowBlockList;


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


    /**
     * Iterator를 위한 Group Key Value Block List 구성
     */

    STL_DASSERT( sOptGroupInstant->mInstantNode->mKeyColList != NULL );
    
    if( sOptGroupInstant->mInstantNode->mKeyColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptGroupInstant->mInstantNode->mKeyColList,
                                             STL_TRUE,
                                             & sExeGroupInstant->mKeyColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeGroupInstant->mKeyColBlock = NULL;
    }


    /**
     * Iterator를 위한 Record Value Block List 구성
     */

    STL_DASSERT( sOptGroupInstant->mInstantNode->mRecColList != NULL );
    
    if( sOptGroupInstant->mInstantNode->mRecColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptGroupInstant->mInstantNode->mRecColList,
                                             STL_TRUE,
                                             & sExeGroupInstant->mRecColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );

        /* Record Column List의 마지막 Value Block 얻기 */
        sLastColBlock = sExeGroupInstant->mRecColBlock;
        while( sLastColBlock->mNext != NULL )
        {
            sLastColBlock = sLastColBlock->mNext;
        }

        /* Read Row Block 연결 */
        sLastColBlock->mNext = sTempColBlock.mNext;
    }
    else
    {
        sExeGroupInstant->mRecColBlock = sTempColBlock.mNext;
    }


    /**
     * Iterator를 위한 Read Value Block List 구성
     */

    STL_DASSERT( sOptGroupInstant->mInstantNode->mReadColList != NULL );
    
    if( sOptGroupInstant->mInstantNode->mReadColList->mCount > 0 )
    {
        sKeyColCnt = sOptGroupInstant->mInstantNode->mKeyColList->mCount;

        if( sOptGroupInstant->mAggrFuncCnt > 0 )
        {
            sLastTableBlock = NULL;

            sCurBlock = sExeGroupInstant->mKeyColBlock;
            while( sCurBlock != NULL )
            {
                STL_TRY( knlInitShareBlock( & sNewBlock,
                                            sCurBlock,
                                            & QLL_DATA_PLAN( aDBCStmt ),
                                            KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
        
                if( sLastTableBlock == NULL )
                {
                    sExeGroupInstant->mReadColBlock = sNewBlock;
                }
                else
                {
                    KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock );
                }
                sLastTableBlock = sNewBlock;

                sCurBlock = sCurBlock->mNext;
            }

            STL_DASSERT( sLastTableBlock != NULL );

            sCurBlock = sExeGroupInstant->mRecColBlock;
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
        }
        else
        {
            STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                                 sOptGroupInstant->mInstantNode->mReadColList,
                                                 STL_TRUE,
                                                 & sExeGroupInstant->mReadColBlock,
                                                 & QLL_DATA_PLAN( aDBCStmt ),
                                                 aEnv )
                     == STL_SUCCESS );
        }

        
        /**
         * Read Column Block을 INDEX와 TABLE로 구분
         */

        sCurBlock = sExeGroupInstant->mReadColBlock;
        sExeGroupInstant->mIndexReadColBlock = NULL;
        sExeGroupInstant->mTableReadColBlock = NULL;
        
        while( sCurBlock != NULL )
        {
            if( KNL_GET_BLOCK_COLUMN_ORDER( sCurBlock ) < sKeyColCnt )
            {
                /**
                 * Index Read Column Block 설정
                 */

                if( sExeGroupInstant->mIndexReadColBlock == NULL )
                {
                    sExeGroupInstant->mIndexReadColBlock = sCurBlock;
                }
                else
                {
                    /* Do Nothing */
                }
                sCurBlock = sCurBlock->mNext;
            }
            else
            {
                /**
                 * Table Read Column Block 설정
                 */

                if( sExeGroupInstant->mTableReadColBlock == NULL )
                {
                    sExeGroupInstant->mTableReadColBlock = sCurBlock;
                    break;
                }
                else
                {
                    /* Do Nothing */
                }
                sCurBlock = sCurBlock->mNext;
            }
        }
        
        /* Read Column List의 마지막 Value Block 얻기 */
        sLastColBlock = sExeGroupInstant->mReadColBlock;
        while( sLastColBlock->mNext != NULL )
        {
            sLastColBlock = sLastColBlock->mNext;
        }

        /* Read Row Block 연결 */
        sLastColBlock->mNext = sTempColBlock.mNext;
    }
    else
    {
        sExeGroupInstant->mReadColBlock = sTempColBlock.mNext;
    }
    sFetchInfo->mColList = sExeGroupInstant->mReadColBlock;

    
    /**
     * Insert Value Block List 설정
     */

    sLastTableBlock = NULL;

    sCurBlock = sExeGroupInstant->mKeyColBlock;
    while( sCurBlock != NULL )
    {
        STL_TRY( knlInitShareBlock( & sNewBlock,
                                    sCurBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        if( sLastTableBlock == NULL )
        {
            sExeGroupInstant->mInsertColBlock = sNewBlock;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock );
        }
        sLastTableBlock = sNewBlock;

        sCurBlock = sCurBlock->mNext;
    }

    STL_DASSERT( sLastTableBlock != NULL );

    sCurBlock = sExeGroupInstant->mRecColBlock;
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
     * Read Row Block 구성
     */
            
    sLastColBlock = & sTempColBlock;
        
    if( sExeGroupInstant->mCommonInfo.mRowBlockList == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        if( sExeGroupInstant->mCommonInfo.mRowBlockList->mCount > 0 )
        {
            sRowBlockItem = sExeGroupInstant->mCommonInfo.mRowBlockList->mHead;
            sColOrder     = ( sOptGroupInstant->mInstantNode->mKeyColCnt +
                              sOptGroupInstant->mInstantNode->mRecColCnt );
            
            for( sLoop = sExeGroupInstant->mCommonInfo.mRowBlockList->mCount ; sLoop > 0 ; sLoop-- )
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
     * Index 관련 정보 & Fetch Record 설정
     */

    /* Sort Instant 와 동일한 구조 */
    STL_TRY( qlndGetRangePredInfoForSortInstant( aDBCStmt,
                                                 aSQLStmt,
                                                 aDataArea,
                                                 sOptGroupInstant,
                                                 sExeGroupInstant,
                                                 aEnv )
             == STL_SUCCESS );

    
    /***************************************************************************
     * Aggregation 정보 구성
     **************************************************************************/

    sExeGroupInstant->mHasAggr = ( sOptGroupInstant->mAggrFuncCnt > 0 );
    sExeGroupInstant->mHasFalseFilter = STL_FALSE;

    if( ( sOptGroupInstant->mAggrFuncCnt == 0 ) &&
        ( sOptGroupInstant->mScrollable == ELL_CURSOR_SCROLLABILITY_NO_SCROLL ) && 
        ( sOptGroupInstant->mTotalFilterExpr == NULL ) )
    {
        /**
         * @todo Top-Down 방식 확장
         */
        
        /* sExeGroupInstant->mNeedMaterialize = STL_FALSE; */
        sExeGroupInstant->mNeedMaterialize = STL_TRUE;
    }
    else
    {
        sExeGroupInstant->mNeedMaterialize = STL_TRUE;
    }

    STL_TRY_THROW( sExeGroupInstant->mHasAggr == STL_TRUE, RAMP_FINISH_AGGREGATION );

    
    /* /\** */
    /*  * Aggregation 수행 정보 구성 */
    /*  *\/ */

    /* STL_PARAM_VALIDATE( aOptNode->mOptCurQueryNode != NULL, */
    /*                     QLL_ERROR_STACK(aEnv) ); */
            
    /* sOptQuerySpec = (qlnoQuerySpec *) aOptNode->mOptCurQueryNode->mOptNode; */

    /* STL_TRY( qlnfGetAggrFuncExecInfo( sOptGroupInstant->mAggrFuncCnt, */
    /*                                   sOptGroupInstant->mAggrOptInfo, */
    /*                                   aDataArea, */
    /*                                   sOptQuerySpec->mQueryExprList->mStmtExprList, */
    /*                                   & sExeGroupInstant->mAggrDistExecInfo, */
    /*                                   & QLL_DATA_PLAN( aDBCStmt ), */
    /*                                   aEnv ) */
    /*          == STL_SUCCESS ); */

    
    /* /\** */
    /*  * Instant Table for Aggregation 공간 할당 */
    /*  *\/ */

    /* STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ), */
    /*                             STL_SIZEOF( qlnInstantTable ), */
    /*                             (void **) & sExeGroupInstant->mAggrDistInstantTable, */
    /*                             KNL_ENV(aEnv) ) */
    /*          == STL_SUCCESS ); */
    

    /* /\** */
    /*  * Instant Table Key Column List 설정 */
    /*  *\/ */

    /* /\* Group Instant Table의 Key Column과 동일 *\/ */
    /* sExeGroupInstant->mAggrDistKeyColBlock = sExeGroupInstant->mKeyColBlock; */


    /* /\** */
    /*  * Instant Table Record Column List 설정 */
    /*  *\/ */

    /* /\* Aggregation 수행 결과 리스트로 설정 *\/ */
    /* sLastTableBlock = NULL; */

    /* for( sLoop = 0 ; sLoop < sOptGroupInstant->mAggrFuncCnt ; sLoop++ ) */
    /* { */
    /*     STL_TRY( knlInitShareBlock( & sNewBlock, */
    /*                                 sExeGroupInstant->mAggrDistExecInfo[ sLoop ].mResultValue, */
    /*                                 & QLL_DATA_PLAN( aDBCStmt ), */
    /*                                 KNL_ENV( aEnv ) ) */
    /*              == STL_SUCCESS ); */
        
    /*     if( sLastTableBlock == NULL ) */
    /*     { */
    /*         sExeGroupInstant->mAggrDistRecColBlock = sNewBlock; */
    /*     } */
    /*     else */
    /*     { */
    /*         KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock ); */
    /*     } */
    /*     sLastTableBlock = sNewBlock; */
    /* } */
    

    /* /\** */
    /*  * Instant Table Read Column List 설정 */
    /*  *\/ */

    /* /\* Key Column List + Record Column List  *\/ */

    /* sLastTableBlock = NULL; */
    /* sColOrder       = 0; */
    
    /* sCurBlock = sExeGroupInstant->mAggrDistKeyColBlock; */
    /* while( sCurBlock != NULL ) */
    /* { */
    /*     STL_TRY( knlInitShareBlock( & sNewBlock, */
    /*                                 sCurBlock, */
    /*                                 & QLL_DATA_PLAN( aDBCStmt ), */
    /*                                 KNL_ENV( aEnv ) ) */
    /*              == STL_SUCCESS ); */
        
    /*     if( sLastTableBlock == NULL ) */
    /*     { */
    /*         sExeGroupInstant->mAggrDistReadColBlock = sNewBlock; */
    /*     } */
    /*     else */
    /*     { */
    /*         KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock ); */
    /*     } */
    /*     sLastTableBlock = sNewBlock; */

    /*     sColOrder++; */
    /*     sCurBlock = sCurBlock->mNext; */
    /* } */

    /* STL_DASSERT( sLastTableBlock != NULL ); */

    /* sCurBlock = sExeGroupInstant->mAggrDistRecColBlock; */
    /* while( sCurBlock != NULL ) */
    /* { */
    /*     KNL_SET_BLOCK_COLUMN_ORDER( sCurBlock, sColOrder ); */
    /*     STL_TRY( knlInitShareBlock( & sNewBlock, */
    /*                                 sCurBlock, */
    /*                                 & QLL_DATA_PLAN( aDBCStmt ), */
    /*                                 KNL_ENV( aEnv ) ) */
    /*              == STL_SUCCESS ); */

    /*     KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock ); */
    /*     sLastTableBlock = sNewBlock; */

    /*     sColOrder++; */
    /*     sCurBlock = sCurBlock->mNext; */
    /* } */


    /* /\** */
    /*  * Instant Table Insert Column List 설정 */
    /*  *\/ */

    /* /\* Read Column List 와 동일 *\/ */
    /* sExeGroupInstant->mAggrDistInsertColBlock = sExeGroupInstant->mAggrDistReadColBlock; */


    /* /\** */
    /*  * Instant Table Index Read Column List 설정 */
    /*  *\/ */

    /* /\* Key Column List 와 동일 *\/ */
    /* sExeGroupInstant->mAggrDistIndexReadColBlock = sExeGroupInstant->mAggrDistKeyColBlock; */


    /* /\** */
    /*  * Instant Table Table Read Column List 설정 */
    /*  *\/ */

    /* /\* Record Column List 와 동일 *\/ */
    /* sExeGroupInstant->mAggrDistTableReadColBlock = sExeGroupInstant->mAggrDistRecColBlock; */


    /* /\** */
    /*  * Fetch Info for Aggregation 공간 할당 */
    /*  *\/ */

    /* STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ), */
    /*                             STL_SIZEOF( smlFetchInfo ), */
    /*                             (void **) & sAggrFetchInfo, */
    /*                             KNL_ENV(aEnv) ) */
    /*          == STL_SUCCESS ); */
    /* stlMemcpy( sAggrFetchInfo, sFetchInfo, STL_SIZEOF( smlFetchInfo ) ); */

    /* sExeGroupInstant->mAggrDistFetchInfo = sAggrFetchInfo; */


    /* /\** */
    /*  * Aggregation Grouping Fetch Info 설정 */
    /*  *\/ */

    /* sAggrFetchInfo->mMinRange       = NULL; */
    /* sAggrFetchInfo->mMaxRange       = NULL; */
    /* sAggrFetchInfo->mKeyCompList    = NULL; */
    /* sAggrFetchInfo->mPhysicalFilter = NULL; */
    
    /* sAggrFetchInfo->mColList     = sExeGroupInstant->mAggrDistReadColBlock;   */
    /* sAggrFetchInfo->mSkipCnt     = 0; */
    /* sAggrFetchInfo->mFetchCnt    = QLL_FETCH_COUNT_MAX; */
    /* sAggrFetchInfo->mIsEndOfScan = STL_FALSE; */


    /* /\** */
    /*  * Grouping Fetch Info 재설정 */
    /*  *\/ */

    /* sFetchInfo->mLogicalFilter   = NULL; */
    /* sFetchInfo->mFilterEvalInfo  = NULL; */
    /* sFetchInfo->mFetchRecordInfo = NULL; */

    
    /* /\** */
    /*  * Aggregation For Filter 정보 구성 */
    /*  *\/ */

    /* /\* if( sOptGroupInstant->mAggrDistFuncCntForFilter > 0 ) *\/ */
    /* /\* { *\/ */
    /* /\*     STL_PARAM_VALIDATE( aOptNode->mOptCurQueryNode != NULL, *\/ */
    /* /\*                         QLL_ERROR_STACK(aEnv) ); *\/ */
            
    /* /\*     sOptQuerySpec = (qlnoQuerySpec *) aOptNode->mOptCurQueryNode->mOptNode; *\/ */

    /* /\*     STL_TRY( qlnfGetAggrFuncExecInfo( sOptGroupInstant->mAggrFuncCntForFilter, *\/ */
    /* /\*                                       sOptGroupInstant->mAggrOptInfoForFilter, *\/ */
    /* /\*                                       aDataArea, *\/ */
    /* /\*                                       sOptQuerySpec->mQueryExprList->mStmtExprList, *\/ */
    /* /\*                                       & sExeGroupInstant->mAggrDistExecInfoForFilter, *\/ */
    /* /\*                                       & QLL_DATA_PLAN( aDBCStmt ), *\/ */
    /* /\*                                       aEnv ) *\/ */
    /* /\*              == STL_SUCCESS ); *\/ */
    /* /\* } *\/ */
    /* /\* else *\/ */
    /* /\* { *\/ */
    /* /\*     sExeGroupInstant->mAggrDistExecInfoForFilter = NULL; *\/ */
    /* /\* } *\/ */


    STL_RAMP( RAMP_FINISH_AGGREGATION );
    

    /***************************************************************************
     * Execution Node 설정
     **************************************************************************/

    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeGroupInstant;

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
