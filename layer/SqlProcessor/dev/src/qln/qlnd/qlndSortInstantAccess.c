/*******************************************************************************
 * qlndSortInstantAccess.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndSortInstantAccess.c 10973 2014-01-16 07:17:19Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndSortInstantAccess.c
 * @brief SQL Processor Layer Data Optimization Node - SORT INSTANT ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief SORT INSTANT ACCESS Execution node에 대한 Data 정보를 구축한다.
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
stlStatus qlndDataOptimizeSortInstantAccess( smlTransId              aTransID,
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
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE,
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
                    sExeSortInstant->mTableReadColBlock = sNewBlock;
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
    sFetchInfo->mColList = sExeSortInstant->mReadColBlock;


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

    sFetchInfo->mSkipCnt         = 0;
    sFetchInfo->mFetchCnt        = QLL_FETCH_COUNT_MAX;
    sFetchInfo->mIsEndOfScan     = STL_FALSE;


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

    sExecNode->mNodeType  = QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE;
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


/**
 * @brief SORT INSTANT ACCESS Optimization Node로 부터 Index Scan에 필요한 range & filter 정보를 구성한다.
 * @param[in]      aDBCStmt           DBC Statement
 * @param[in,out]  aSQLStmt           SQL Statement
 * @param[in]      aDataArea          Data Area (Data Optimization 결과물)
 * @param[in]      aOptSortInstant    Index Access Optimization Node
 * @param[in,out]  aExeSortInstant    Index Access Execution Node
 * @param[in]      aEnv               Environment
 */
stlStatus qlndGetRangePredInfoForSortInstant( qllDBCStmt       * aDBCStmt,
                                              qllStatement     * aSQLStmt,
                                              qllDataArea      * aDataArea,
                                              qlnoInstant      * aOptSortInstant,
                                              qlnxInstant      * aExeSortInstant,
                                              qllEnv           * aEnv )
{
    qlnoInstant         * sOptSortInstant      = NULL;
    qlnxInstant         * sExeSortInstant      = NULL;

    qloIndexScanInfo    * sIndexScanInfo       = NULL;

    smlFetchInfo        * sFetchInfo           = NULL;
    smlFetchRecordInfo  * sFetchRecordInfo     = NULL;

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptSortInstant != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExeSortInstant != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Node 획득
     */

    sOptSortInstant = aOptSortInstant;
    sExeSortInstant = aExeSortInstant;
    sIndexScanInfo  = sOptSortInstant->mIndexScanInfo;


    /**********************************************************
     * Rewrite Filter 
     **********************************************************/

    /**
     * @todo Predicate 생성 여부 판단
     */


    /**
     * @todo Range & Filter 재구성
     * @remark Min/Max Range 및 Filter 의 Constant Value 를 고려하여,
     *    <BR> 불필요한 조건들을 제거한다.
     *    <BR> 남은 조건들만을 가지고 Physical / Logical Filter 구성
     *    <BR> 
     *    <BR> Bind Parameter에 대한 value가 설정되어 있기 때문에
     *    <BR> 정확한 Range 및 Filter를 구성할 수 있다.
     *    <BR> Scan 여부를 판가름 할 수도 있는 튜닝 요소임.
     */

    
    

    /**
     * @todo MIN Range를 분석하여 불필요한 조건들 제거
     */

    
    /**
     * @todo MAX Range를 분석하여 불필요한 조건들 제거
     */


    /**
     * @todo Physical Key Filter를 분석하여 불필요한 조건들 제거
     */

    
    /**
     * @todo Logical Key Filter를 분석하여 불필요한 조건들 제거
     */


    /**
     * @todo Physical Table Filter를 분석하여 불필요한 조건들 제거
     */


    /**
     * @todo Logical Table Filter를 분석하여 불필요한 조건들 제거
     */
    

    /**
     * 항상 같은 값을 반환하는 경우 Ragne Predicate 생성하지 않음 (Optimization 정보)
     */

    sFetchInfo = & sExeSortInstant->mFetchInfo;


    /**********************************************************
     * Key Compare List 설정
     **********************************************************/

    /**
     * Key Compare List 구성
     */

    STL_TRY( qlndGetKeyCompareListForSortInstant( aSQLStmt,
                                                  aOptSortInstant,
                                                  & QLL_DATA_PLAN( aDBCStmt ),
                                                  & sFetchInfo->mKeyCompList,
                                                  aEnv )
             == STL_SUCCESS );

    
    /**********************************************************
     * Range 정보 설정
     **********************************************************/

    if( sIndexScanInfo->mIsExist == STL_FALSE )
    {
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlRange ),
                                    (void **) & sFetchInfo->mRange,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        sFetchInfo->mRange->mMinRange = NULL;
        sFetchInfo->mRange->mMaxRange = NULL;
        sFetchInfo->mRange->mNext     = NULL;
        
        sFetchInfo->mPhysicalFilter   = NULL;
        sFetchInfo->mLogicalFilter    = NULL;

        sFetchInfo->mFilterEvalInfo   = NULL;

        STL_THROW( RAMP_SET_FETCH_RECORD_INFO );
    }
    else
    {
        /* Do Nothing */
    }


    /**********************************************************
     * Index Scan을 위한 Fetch Info 구성
     **********************************************************/
    
    /**
     * Range 관련 Compare List 구성
     */

    STL_TRY( qlndSetFetchInfoFromRangeExprForSortInstant( aDataArea,
                                                          sOptSortInstant,
                                                          sExeSortInstant,
                                                          & QLL_DATA_PLAN( aDBCStmt ),
                                                          aEnv )
             == STL_SUCCESS );


    /**********************************************************
     * Blocked Join Fetch Information 설정 
     **********************************************************/

    sFetchInfo->mBlockJoinFetchInfo = NULL;

    /**********************************************************
     * Index Scan을 위한 Fetch Record Info 구성
     **********************************************************/

    STL_RAMP( RAMP_SET_FETCH_RECORD_INFO );


    /**
     * Fetch Record 필요 여부 검사
     */

    if( sExeSortInstant->mTableReadColBlock == NULL )
    {
        sFetchRecordInfo = NULL;
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Fetch Record 정보를 위한 공간 할당
     */
        
    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( smlFetchRecordInfo ),
                                (void **) & sFetchRecordInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Fetch Record 정보 설정 : Table & Read Column 관련
     */
    
    sFetchRecordInfo->mRelationHandle  = NULL;
    sFetchRecordInfo->mColList         = sExeSortInstant->mTableReadColBlock;
    sFetchRecordInfo->mRowIdColList    = NULL;

    
    /**
     * Physical Table Filter 구성
     */

    if( sIndexScanInfo->mPhysicalTableFilterExprList != NULL )
    {
        if( sIndexScanInfo->mPhysicalTableFilterExprList->mCount > 0 )
        {
            STL_TRY( qlndMakePhysicalFilterFromExprListForSortInstant(
                         sIndexScanInfo->mPhysicalTableFilterExprList,
                         sFetchRecordInfo->mColList,
                         aDataArea->mExprDataContext,
                         STL_FALSE,
                         aOptSortInstant,
                         & QLL_DATA_PLAN( aDBCStmt ),
                         & sFetchRecordInfo->mPhysicalFilter,
                         aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sFetchRecordInfo->mPhysicalFilter = NULL;
        }
    }
    else
    {
        sFetchRecordInfo->mPhysicalFilter = NULL;
    }

    
    /**
     * Iterator 의 Read Mode 는 Dictionary Iterator, Foreign Key Iterator 를 제외하고
     * 모두 SNAPSHOT 의 Transaction Read Mode 와 Statement Read Mode 를 갖는다.
     */

    sFetchRecordInfo->mTransReadMode = SML_TRM_SNAPSHOT;
    sFetchRecordInfo->mStmtReadMode  = SML_SRM_SNAPSHOT;


    /**
     * Scn Block List 설정
     */
    
    sFetchRecordInfo->mScnBlock = sFetchInfo->mRowBlock->mScnBlock;
    

    /**
     * Logical Table Filter 구성
     */

    if( sOptSortInstant->mTableLogicalStack == NULL )
    {
        sFetchRecordInfo->mLogicalFilter = NULL;
        sFetchRecordInfo->mLogicalFilterEvalInfo = NULL;
    }
    else
    {
        sFetchRecordInfo->mLogicalFilter = sOptSortInstant->mTableLogicalStack;
        
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprEvalInfo ),
                                    (void **) & sFetchRecordInfo->mLogicalFilterEvalInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sFetchRecordInfo->mLogicalFilterEvalInfo->mExprStack =
            sOptSortInstant->mTableLogicalStack;
        sFetchRecordInfo->mLogicalFilterEvalInfo->mContext =
            aDataArea->mExprDataContext;
        sFetchRecordInfo->mLogicalFilterEvalInfo->mResultBlock =
            aDataArea->mPredResultBlock;
        sFetchRecordInfo->mLogicalFilterEvalInfo->mBlockIdx = 0;
        sFetchRecordInfo->mLogicalFilterEvalInfo->mBlockCount = 0;
    }


    /**
     * View Scn & Tcn 초기화 : storage layer 에서 설정하는 부분
     */
    
    sFetchRecordInfo->mViewScn = 0;
    sFetchRecordInfo->mViewTcn = 0;
    
            
    STL_RAMP( RAMP_FINISH );

    sFetchInfo->mFetchRecordInfo = sFetchRecordInfo;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Range Expression List로부터 Fetch를 위한 Range Predicate 정보 구성하기
 * @param[in]      aDataArea          Data Area (Data Optimization 결과물)
 * @param[in]      aOptSortInstant    Sort Instant Access Optimization Node
 * @param[in,out]  aExeSortInstant    Sort Instant Access Execution Node
 * @param[in]      aRegionMem         영역 기반 메모리 할당자 (knlRegionMem)
 * @param[in,out]  aEnv               커널 Environment
 */
stlStatus qlndSetFetchInfoFromRangeExprForSortInstant( qllDataArea      * aDataArea,
                                                       qlnoInstant      * aOptSortInstant,
                                                       qlnxInstant      * aExeSortInstant,
                                                       knlRegionMem     * aRegionMem,
                                                       qllEnv           * aEnv )
{
    qloIndexScanInfo   * sIndexScanInfo     = NULL;
    knlExprContext     * sExprContexts      = NULL;
    qlvInstantColDesc  * sSortKeyDesc       = NULL;
    qlvRefExprItem     * sListItem          = NULL;
    qlvInitExpression  * sExpr              = NULL;
    
    smlFetchInfo       * sFetchInfo         = NULL;

    knlCompareList     * sMinRangeCompList  = NULL;
    knlCompareList     * sMaxRangeCompList  = NULL;
    knlCompareList     * sCurComp           = NULL;
    dtlDataValue       * sRangeValue        = NULL;
    dtlDataType          sColDataType       = DTL_TYPE_NA;
 
    stlInt32             sLoop              = 0;
    stlInt32             sRangeExprCnt      = 0;
    stlUInt32            sKeyColOffset      = 0;
    stlUInt32            sRangeValOffset    = 0;
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptSortInstant != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExeSortInstant != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Range Expression 존재 여부 검사
     */

    sIndexScanInfo = aOptSortInstant->mIndexScanInfo;
    sFetchInfo     = & aExeSortInstant->mFetchInfo;

    STL_TRY_THROW( aOptSortInstant->mIndexScanInfo->mReadKeyCnt > 0,
                   RAMP_MAKE_KEY_PHYSICAL_FILTER );

    sExprContexts  = aDataArea->mExprDataContext->mContexts;
    sSortKeyDesc   = aOptSortInstant->mColumnDesc;


    /**
     * Fetch Info 정보 설정 : Key Column 관련
     */
    
    sFetchInfo->mColList = aExeSortInstant->mIndexReadColBlock;


    /*************************************************
     * MIN Range Compare List 구성
     *************************************************/

    /**
     * Compare List 공간 할당
     */

    sRangeExprCnt = 0;
    for( sLoop = 0 ;
         ( ( sLoop < sIndexScanInfo->mReadKeyCnt ) &&
           ( sIndexScanInfo->mMinRangeExpr[ sLoop ]->mCount > 0 ) );
         sLoop++ )
    {
        sRangeExprCnt += sIndexScanInfo->mMinRangeExpr[ sLoop ]->mCount;
    }

    if( sRangeExprCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlCompareList ) * sRangeExprCnt,
                                    (void**) & sMinRangeCompList,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        stlMemset( sMinRangeCompList,
                   0x00,
                   STL_SIZEOF( knlCompareList ) * sRangeExprCnt );


        /**
         * Compare List 설정
         */

        sCurComp = sMinRangeCompList;
        for( sLoop = 0 ; sLoop < sIndexScanInfo->mReadKeyCnt ; sLoop++ )
        {
            sListItem = sIndexScanInfo->mMinRangeExpr[ sLoop ]->mHead;
        
            while( sListItem != NULL )
            {
                /**
                 * Expression으로부터 Range 구성 정보 얻기
                 */
            
                sExpr = sListItem->mExprPtr;
                STL_TRY( qloGetRangeFromExpr( sExpr,
                                              sIndexScanInfo->mNullValueExpr->mOffset,
                                              sIndexScanInfo->mFalseValueExpr->mOffset,
                                              & sKeyColOffset,
                                              & sRangeValOffset,
                                              & sCurComp->mIsIncludeEqual,
                                              & sCurComp->mCompNullStop,
                                              & sCurComp->mIsLikeFunc,
                                              aEnv )
                         == STL_SUCCESS );

            
                /**
                 * Comapre 정보 구성
                 */
            
                STL_DASSERT( sExpr->mExpr.mFunction->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN );
                
                sColDataType = sExprContexts[ sKeyColOffset ].mInfo.mValueInfo->mDataValue[0].mType;

                if( sExpr->mExpr.mFunction->mFuncId == KNL_BUILTIN_FUNC_LIKE )
                {
                    STL_DASSERT( sExpr->mExpr.mFunction->mArgs[1]->mIterationTime !=
                                 DTL_ITERATION_TIME_FOR_EACH_EXPR );

                    STL_DASSERT( sExprContexts[ sRangeValOffset ].mCast == NULL );

                    switch( sExpr->mExpr.mFunction->mArgs[1]->mType )
                    {
                        case QLV_EXPR_TYPE_FUNCTION :
                        case QLV_EXPR_TYPE_CAST :
                        case QLV_EXPR_TYPE_CASE_EXPR :
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mFuncData->mResultValue;
                            break;
                        case QLV_EXPR_TYPE_LIST_FUNCTION :
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mListFunc->mResultValue;
                            break;
                        default :
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mValueInfo->mDataValue[0];
                            break;
                    }
                }
                else
                {
                    if( sExprContexts[ sRangeValOffset ].mCast == NULL )
                    {
                        if( sExpr->mExpr.mFunction->mArgCount == 1 )
                        {
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mValueInfo->mDataValue[0];
                        }
                        else
                        {
                            switch( sExpr->mExpr.mFunction->mArgs[1]->mType )
                            {
                                case QLV_EXPR_TYPE_FUNCTION :
                                case QLV_EXPR_TYPE_CAST :
                                case QLV_EXPR_TYPE_CASE_EXPR :
                                    sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mFuncData->mResultValue;
                                    break;
                                case QLV_EXPR_TYPE_LIST_FUNCTION :
                                    sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mListFunc->mResultValue;
                                    break;
                                default :
                                    sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mValueInfo->mDataValue[0];
                                    break;
                            }
                        }
                    }
                    else
                    {
                        sRangeValue = & sExprContexts[ sRangeValOffset ].mCast->mCastResultBuf;
                    }
                }
                
                sCurComp->mColOrder    = sLoop;
                sCurComp->mRangeVal    = sRangeValue->mValue;
                sCurComp->mRangeLen    = sRangeValue->mLength;
                sCurComp->mConstVal    = sRangeValue;

                if( sCurComp->mIsLikeFunc == STL_TRUE )
                {
                    sCurComp->mCompFunc = dtlLikePhysicalCompareChar;
                }
                else
                {
                    sCurComp->mCompFunc =
                        dtlPhysicalCompareFuncList[ sColDataType ][ sRangeValue->mType ];
                }

                sCurComp->mIsAsc         = ( DTL_GET_KEY_FLAG_SORT_ORDER( sSortKeyDesc[ sLoop ].mKeyColFlags) ==
                                             DTL_KEYCOLUMN_INDEX_ORDER_ASC );
                sCurComp->mIsNullFirst   = ( DTL_GET_KEY_FLAG_NULL_ORDER( sSortKeyDesc[ sLoop ].mKeyColFlags) ==
                                             DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST );

                if( sIndexScanInfo->mNullValueExpr->mOffset == sRangeValOffset )
                {
                    sCurComp->mIsDiffTypeCmp = STL_FALSE;
                }
                else
                {
                    sCurComp->mIsDiffTypeCmp = ( sColDataType != sRangeValue->mType );
                }

                sCurComp->mNext = sCurComp + 1;
                sCurComp++;

                sListItem = sListItem->mNext;
            }
        }

        sMinRangeCompList[ sRangeExprCnt - 1 ].mNext = NULL;
    }
    else
    {
        sMinRangeCompList = NULL;
    }

    
    /*************************************************
     * MAX Range Compare List 구성
     *************************************************/

    /**
     * Compare List 공간 할당
     */

    sRangeExprCnt = 0;
    for( sLoop = 0 ;
         ( ( sLoop < sIndexScanInfo->mReadKeyCnt ) &&
           ( sIndexScanInfo->mMaxRangeExpr[ sLoop ]->mCount > 0 ) );
         sLoop++ )
    {
        sRangeExprCnt += sIndexScanInfo->mMaxRangeExpr[ sLoop ]->mCount;
    }
    
    if( sRangeExprCnt > 0 )
    {
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlCompareList ) * sRangeExprCnt,
                                    (void**) & sMaxRangeCompList,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        stlMemset( sMaxRangeCompList,
                   0x00,
                   STL_SIZEOF( knlCompareList ) * sRangeExprCnt );


        /**
         * Compare List 설정
         */

        sCurComp = sMaxRangeCompList;
        for( sLoop = 0 ; sLoop < sIndexScanInfo->mReadKeyCnt ; sLoop++ )
        {
            sListItem = sIndexScanInfo->mMaxRangeExpr[ sLoop ]->mHead;
        
            while( sListItem != NULL )
            {
                /**
                 * Expression으로부터 Range 구성 정보 얻기
                 */
            
                sExpr = sListItem->mExprPtr;
                STL_TRY( qloGetRangeFromExpr( sExpr,
                                              sIndexScanInfo->mNullValueExpr->mOffset,
                                              sIndexScanInfo->mFalseValueExpr->mOffset,
                                              & sKeyColOffset,
                                              & sRangeValOffset,
                                              & sCurComp->mIsIncludeEqual,
                                              & sCurComp->mCompNullStop,
                                              & sCurComp->mIsLikeFunc,
                                              aEnv )
                         == STL_SUCCESS );

            
                /**
                 * Comapre 정보 구성
                 */

                STL_DASSERT( sExpr->mExpr.mFunction->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN );
            
                sColDataType = sExprContexts[ sKeyColOffset ].mInfo.mValueInfo->mDataValue[0].mType;

                if( sExpr->mExpr.mFunction->mFuncId == KNL_BUILTIN_FUNC_LIKE )
                {
                    STL_DASSERT( sExpr->mExpr.mFunction->mArgs[1]->mIterationTime !=
                                 DTL_ITERATION_TIME_FOR_EACH_EXPR );

                    STL_DASSERT( sExprContexts[ sRangeValOffset ].mCast == NULL );

                    switch( sExpr->mExpr.mFunction->mArgs[1]->mType )
                    {
                        case QLV_EXPR_TYPE_FUNCTION :
                        case QLV_EXPR_TYPE_CAST :
                        case QLV_EXPR_TYPE_CASE_EXPR :
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mFuncData->mResultValue;
                            break;
                        case QLV_EXPR_TYPE_LIST_FUNCTION :
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mListFunc->mResultValue;
                            break;
                        default :
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mValueInfo->mDataValue[0];
                            break;
                    }
                }
                else
                {
                    if( sExprContexts[ sRangeValOffset ].mCast == NULL )
                    {
                        if( sExpr->mExpr.mFunction->mArgCount == 1 )
                        {
                            sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mValueInfo->mDataValue[0];
                        }
                        else
                        {
                            switch( sExpr->mExpr.mFunction->mArgs[1]->mType )
                            {
                                case QLV_EXPR_TYPE_FUNCTION :
                                case QLV_EXPR_TYPE_CAST :
                                case QLV_EXPR_TYPE_CASE_EXPR :
                                    sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mFuncData->mResultValue;
                                    break;
                                case QLV_EXPR_TYPE_LIST_FUNCTION :
                                    sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mListFunc->mResultValue;
                                    break;
                                default :
                                    sRangeValue = & sExprContexts[ sRangeValOffset ].mInfo.mValueInfo->mDataValue[0];
                                    break;
                            }
                        }
                    }
                    else
                    {
                        sRangeValue = & sExprContexts[ sRangeValOffset ].mCast->mCastResultBuf;
                    }
                }
                
                sCurComp->mColOrder    = sLoop;
                sCurComp->mRangeVal    = sRangeValue->mValue;
                sCurComp->mRangeLen    = sRangeValue->mLength;
                sCurComp->mConstVal    = sRangeValue;

                if( sCurComp->mIsLikeFunc == STL_TRUE )
                {
                    sCurComp->mCompFunc = dtlLikePhysicalCompareChar;
                }
                else
                {
                    sCurComp->mCompFunc =
                        dtlPhysicalCompareFuncList[ sColDataType ][ sRangeValue->mType ];
                }

                sCurComp->mIsAsc       = ( DTL_GET_KEY_FLAG_SORT_ORDER( sSortKeyDesc[ sLoop ].mKeyColFlags) ==
                                           DTL_KEYCOLUMN_INDEX_ORDER_ASC );
                sCurComp->mIsNullFirst = ( DTL_GET_KEY_FLAG_NULL_ORDER( sSortKeyDesc[ sLoop ].mKeyColFlags) ==
                                           DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST );

                if( sIndexScanInfo->mNullValueExpr->mOffset == sRangeValOffset )
                {
                    sCurComp->mIsDiffTypeCmp = STL_FALSE;
                }
                else
                {
                    sCurComp->mIsDiffTypeCmp = ( sColDataType != sRangeValue->mType );
                }
                
                sCurComp->mNext = sCurComp + 1;
                sCurComp++;

                sListItem = sListItem->mNext;
            }
        }

        sMaxRangeCompList[ sRangeExprCnt - 1 ].mNext = NULL;
    }
    else
    {
        /* Coverage : Code Opt에서 Not Null 체크를 하지 않고,
         * Nulls Last로 고정시키기 때문에 Max Range는 항상 존재한다.
         * 둘 중 하나라도 지원되면 이 코드를 탈 것이다. */
        sMaxRangeCompList = NULL;
    }
    

    /*************************************************
     * Key Physical Filter 구성
     *************************************************/

    STL_RAMP( RAMP_MAKE_KEY_PHYSICAL_FILTER );

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlRange ),
                                (void **) & sFetchInfo->mRange,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    sFetchInfo->mRange->mMinRange = sMinRangeCompList;
    sFetchInfo->mRange->mMaxRange = sMaxRangeCompList;
    sFetchInfo->mRange->mNext     = NULL;


    /**
     * Physical Key Filter 구성 : Fetch Info의 Physical Filter
     */

    STL_DASSERT( sIndexScanInfo->mPhysicalKeyFilterExprList != NULL );

    if( sIndexScanInfo->mPhysicalKeyFilterExprList->mCount > 0 )
    {
        STL_TRY( qlndMakePhysicalFilterFromExprListForSortInstant(
                     sIndexScanInfo->mPhysicalKeyFilterExprList,
                     sFetchInfo->mColList,
                     aDataArea->mExprDataContext,
                     STL_TRUE,
                     aOptSortInstant,
                     aRegionMem,
                     & sFetchInfo->mPhysicalFilter,
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sFetchInfo->mPhysicalFilter = NULL;
    }

    
    /*************************************************
     * Fetch Info 정보 설정
     *************************************************/

    /**
     * Range Predicate 구성
     */

    if( aOptSortInstant->mIndexKeyLogicalStack == NULL )
    {
        sFetchInfo->mLogicalFilter = NULL;
        sFetchInfo->mFilterEvalInfo = NULL;
    }
    else
    {
        sFetchInfo->mLogicalFilter = aOptSortInstant->mIndexKeyLogicalStack;
        
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlExprEvalInfo ),
                                    (void **) & sFetchInfo->mFilterEvalInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        sFetchInfo->mFilterEvalInfo->mExprStack   = aOptSortInstant->mIndexKeyLogicalStack;
        sFetchInfo->mFilterEvalInfo->mContext     = aDataArea->mExprDataContext;
        sFetchInfo->mFilterEvalInfo->mResultBlock = aDataArea->mPredResultBlock;
        sFetchInfo->mFilterEvalInfo->mBlockIdx    = 0;
        sFetchInfo->mFilterEvalInfo->mBlockCount  = 0;
    }

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Key Compare를 위한 Key Compare List 구성
 * @param[in,out] aSQLStmt         SQL Statement
 * @param[in]     aOptSortInstant  Sort Instant Table
 * @param[in]     aRegionMem       Region Memory
 * @param[out]    aKeyCompList     Key Compare List
 * @param[in]     aEnv             Environment
 */
stlStatus qlndGetKeyCompareListForSortInstant( qllStatement        * aSQLStmt,
                                               qlnoInstant         * aOptSortInstant,
                                               knlRegionMem        * aRegionMem,
                                               knlKeyCompareList  ** aKeyCompList,
                                               qllEnv              * aEnv )
{
    knlKeyCompareList  * sKeyCompList;
    qlvInstantColDesc  * sSortKeyDesc;
    stlInt32             sIndexKeyCnt;
    stlInt32             sLoop;
    dtlDataType          sType;
    knlValueBlockList  * sValueList     = NULL;
    knlValueBlockList  * sPrevValueList = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptSortInstant != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyCompList != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Index 정보 획득
     */
    
    sIndexKeyCnt = aOptSortInstant->mInstantNode->mKeyColCnt;
    sSortKeyDesc = aOptSortInstant->mColumnDesc;


    /**
     * Key Compare List 공간 할당
     */

    STL_TRY( knlCreateKeyCompareList( sIndexKeyCnt,
                                      & sKeyCompList,
                                      aRegionMem,
                                      KNL_ENV(aEnv) )
             == STL_SUCCESS );    


    /**
     * Table Column Order 정보 구성 - SORT
     */

    for( sLoop = 0; sLoop < sIndexKeyCnt ; sLoop++ )
    {
        /* table column order */
        sKeyCompList->mTableColOrder[ sLoop ]  = sLoop;
        sKeyCompList->mIndexColOffset[ sLoop ] = sLoop;

        /* function pointer */
        sType = sSortKeyDesc[ sLoop ].mType;
        sKeyCompList->mCompFunc[ sLoop ] =
            dtlPhysicalFuncArg2FuncList[ sType ][ sType ][ DTL_PHYSICAL_FUNC_IS_EQUAL ];

        /* value block list */
        STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                    & sValueList,
                                    1,
                                    STL_TRUE,
                                    STL_LAYER_SQL_PROCESSOR,
                                    sLoop,
                                    sLoop,
                                    sType,
                                    gDefaultDataTypeDef[ sType ].mArgNum1,
                                    gDefaultDataTypeDef[ sType ].mArgNum2,
                                    gDefaultDataTypeDef[ sType ].mStringLengthUnit,
                                    gDefaultDataTypeDef[ sType ].mIntervalIndicator,
                                    aRegionMem,
                                    aEnv )
                 == STL_SUCCESS );

        if( sKeyCompList->mValueList == NULL )
        {
            sKeyCompList->mValueList = sValueList;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevValueList, sValueList );
        }
        sPrevValueList = sValueList;
    }    

    
    /**
     * OUTPUT 설정
     */

    *aKeyCompList = sKeyCompList;
        
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Execution List를 위한 Physical Filter 정보 구축 
 * @param[in]     aExprList          Expression List
 * @param[in]     aReadValueBlocks  Read Value Block List
 * @param[in]     aExprDataContext   Context Info
 * @param[in]     aIsKeyFilter       Key Filter 인지 여부
 * @param[in]     aOptSortInstant    Sort Instant Optimization Node
 * @param[in]     aRegionMem         Region Memory
 * @param[out]    aPhysicalFilter    Physical Filter
 * @param[in]     aEnv               Environment
 */
stlStatus qlndMakePhysicalFilterFromExprListForSortInstant( qlvRefExprList       * aExprList,
                                                            knlValueBlockList    * aReadValueBlocks,
                                                            knlExprContextInfo   * aExprDataContext,
                                                            stlBool                aIsKeyFilter,
                                                            qlnoInstant          * aOptSortInstant,
                                                            knlRegionMem         * aRegionMem,
                                                            knlPhysicalFilter   ** aPhysicalFilter,
                                                            qllEnv               * aEnv )
{
    qlvInitExpression       * sFilterExpr      = NULL;
    knlPhysicalFilter       * sPhysicalFilter  = NULL;
    knlPhysicalFilter       * sCurFilter       = NULL;
    knlValueBlockList       * sValueBlock      = NULL;
    qlvInitFunction         * sFuncCode        = NULL;
    qlvInitListFunc         * sListFunc        = NULL;
    qlvInitListCol          * sListColLeft     = NULL;
    qlvInitRowExpr          * sRowExpr1        = NULL;
    qlvInitListCol          * sListColRight    = NULL;
//    qlvInitRowExpr          * sRowExpr2        = NULL;
    knlColumnInReadRow      * sColumnInReadRow = NULL;
    dtlDataValue            * sDataValue       = NULL;
    stlInt32                  sLoop1           = 0;
    stlInt32                  sLoop2           = 0;
    stlInt32                  sLeftIdx         = 0;
    stlInt32                  sRightIdx        = 0;

    knlPhysicalFilterPtr    * sSortList        = NULL;
    knlPhysicalFilter       * sSortTemp        = NULL;

    stlInt32                  sExprListCnt     = 0;
    qlvRefExprItem          * sListItem        = NULL;
    
    /* qlvInitInstantNode      * sIniSortInstant  = NULL; */
    qlvInstantColDesc       * sColumnDesc      = NULL;
    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprDataContext != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptSortInstant != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPhysicalFilter != NULL, QLL_ERROR_STACK(aEnv) );

    
#define GET_VALUE_BLOCK_WITH_COLUMN_ORDER( block, order )               \
    {                                                                   \
        (block) = aReadValueBlocks;                                     \
        while( (block) != NULL )                                        \
        {                                                               \
            if( KNL_GET_BLOCK_COLUMN_ORDER( (block) ) == (order) )      \
            {                                                           \
                break;                                                  \
            }                                                           \
            (block) = (block)->mNext;                                   \
        }                                                               \
                                                                        \
        STL_DASSERT( (block) != NULL );                                 \
    }


    /**
     * Filter 공간 할당
     */

    sExprListCnt = aExprList->mCount;

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlPhysicalFilter ) * sExprListCnt,
                                (void **) & sPhysicalFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    

    /**
     * Physical Filter List 생성
     */
    
    /* sIniSortInstant = aOptSortInstant->mInstantNode; */
    sColumnDesc     = aOptSortInstant->mColumnDesc;

    sCurFilter = sPhysicalFilter;
    sListItem  = aExprList->mHead;
    
    while( sListItem != NULL )
    {
        /**
         * Filter 구성
         */
        sFilterExpr = sListItem->mExprPtr;
        
        /* Column Order 기준으로 Filter entry 정렬 */
        if( sFilterExpr->mType == QLV_EXPR_TYPE_FUNCTION )
        {
            /* function 으로 구성된 filter */
            sFuncCode = sFilterExpr->mExpr.mFunction;
            
            if( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN )
            {
                if( ( sFuncCode->mArgCount == 2 ) &&
                    ( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                {
                    /**
                     * 두 개의 column으로 구성된 filter
                     */

                    sLeftIdx  = *sFuncCode->mArgs[0]->mExpr.mInnerColumn->mIdx;
                    sRightIdx = *sFuncCode->mArgs[1]->mExpr.mInnerColumn->mIdx;
                    
                    if( sLeftIdx >= sRightIdx )
                    {
                        /**
                         * column vs column ( idx1 >= idx2 )
                         */
                        
                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER( sValueBlock,
                                                           sLeftIdx );

                        sCurFilter->mColIdx1 = sLeftIdx;
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER( sValueBlock,
                                                           sRightIdx );

                        sCurFilter->mColIdx2  = sRightIdx;
                        sCurFilter->mColVal2  = & sValueBlock->mColumnInRow;
                        sCurFilter->mConstVal = NULL;

                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               sColumnDesc[ sLeftIdx ].mType,
                                               sColumnDesc[ sRightIdx ].mType,
                                               & sCurFilter->mFunc,
                                               STL_FALSE );
                    }
                    else
                    {
                        /**
                         * column vs column ( idx1 < idx2 ) : Operand의 위치 바꾸기 => reverse function 적용
                         */
                        
                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER( sValueBlock,
                                                           sRightIdx );
                        
                        sCurFilter->mColIdx1 = sRightIdx;
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER( sValueBlock,
                                                           sLeftIdx );

                        sCurFilter->mColIdx2  = sLeftIdx;
                        sCurFilter->mColVal2  = & sValueBlock->mColumnInRow;
                        sCurFilter->mConstVal = NULL;
                        
                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               sColumnDesc[ sLeftIdx ].mType,
                                               sColumnDesc[ sRightIdx ].mType,
                                               & sCurFilter->mFunc,
                                               STL_TRUE );
                    }
                }
                else
                {
                    sLeftIdx = *sFuncCode->mArgs[0]->mExpr.mInnerColumn->mIdx;
                    
                    if( sFuncCode->mArgCount == 1 )
                    {
                        /**
                         * column 하나만으로 구성된 filter
                         */ 

                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER( sValueBlock,
                                                           sLeftIdx );
            
                        sCurFilter->mColIdx1 = sLeftIdx;
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* emtpy value 설정 */
                        sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT;
                        sCurFilter->mColVal2  = knlEmtpyColumnValue;
                        sCurFilter->mConstVal = NULL;
                        
                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               sColumnDesc[ sLeftIdx ].mType,
                                               0,
                                               & sCurFilter->mFunc,
                                               STL_FALSE );
                    }
                    else
                    {
                        /**
                         * column vs constant value
                         */
            
                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER( sValueBlock,
                                                           sLeftIdx );

                        sCurFilter->mColIdx1 = sLeftIdx;
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* constant value 설정 */
                        if( aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].mCast != NULL )
                        {
                            sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].mCast->mCastResultBuf;
                        }
                        else
                        {
                            if( (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                                (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_VALUE) ||
                                (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                                (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_OUTER_COLUMN) )
                            {
                                sDataValue = aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].
                                    mInfo.mValueInfo->mDataValue;
                            }
                            else
                            {
                                if( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
                                {
                                    sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].
                                        mInfo.mListFunc->mResultValue;
                                }
                                else
                                {
                                    sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].
                                        mInfo.mFuncData->mResultValue;
                                }
                            }
                        }

                        STL_TRY( knlAllocRegionMem( aRegionMem,
                                                    STL_SIZEOF( knlColumnInReadRow ),
                                                    (void **) & sColumnInReadRow,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        sColumnInReadRow->mValue  = (void *) sDataValue->mValue;
                        sColumnInReadRow->mLength = sDataValue->mLength;
                                
                        sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT;
                        sCurFilter->mColVal2  = sColumnInReadRow;
                        sCurFilter->mConstVal = sDataValue;
                        
                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               sColumnDesc[ sLeftIdx ].mType,
                                               sDataValue->mType,
                                               & sCurFilter->mFunc,
                                               STL_FALSE );
                    }
                }
            }
            else
            {
                /**
                 * column vs constant value : reverse function
                 */
                
                STL_DASSERT( sFuncCode->mArgCount == 2 );         /* constant value만으로 구성된 physical filter는 없다. */
                STL_DASSERT( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_INNER_COLUMN );

                sRightIdx = *sFuncCode->mArgs[1]->mExpr.mInnerColumn->mIdx;

                /* column value 설정 */
                GET_VALUE_BLOCK_WITH_COLUMN_ORDER( sValueBlock,
                                                   sRightIdx );

                sCurFilter->mColIdx1 = sRightIdx;
                sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                /* constant value 설정 */
                if( aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].mCast != NULL )
                {
                    sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].mCast->mCastResultBuf;
                }
                else
                {
                    if( (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                        (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_VALUE) ||
                        (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                        (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_OUTER_COLUMN) )
                    {
                        sDataValue = aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].
                            mInfo.mValueInfo->mDataValue;
                    }
                    else
                    {
                        if( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
                        {
                            sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].
                                mInfo.mListFunc->mResultValue;
                        }
                        else
                        {
                            sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].
                                mInfo.mFuncData->mResultValue;
                        }
                    }
                }

                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            STL_SIZEOF( knlColumnInReadRow ),
                                            (void **) & sColumnInReadRow,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sColumnInReadRow->mValue  = (void *) sDataValue->mValue;
                sColumnInReadRow->mLength = sDataValue->mLength;
                                
                sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT;
                sCurFilter->mColVal2  = sColumnInReadRow;
                sCurFilter->mConstVal = sDataValue;

                /* function pointer 설정 */
                qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                       sDataValue->mType,
                                       sColumnDesc[ sRightIdx ].mType,
                                       & sCurFilter->mFunc,
                                       STL_TRUE );
           }
        }
        else if( sFilterExpr->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
        {
            /* list function 으로 구성된 filter */
            sListFunc = sFilterExpr->mExpr.mListFunc;

            /* List Function으로 구성된 filter */
            sListFunc = sFilterExpr->mExpr.mListFunc;
            STL_DASSERT( sListFunc->mArgs[1]->mType == QLV_EXPR_TYPE_LIST_COLUMN );

            sListColLeft = sListFunc->mArgs[1]->mExpr.mListCol;
            STL_DASSERT( sListColLeft->mArgs[0]->mType == QLV_EXPR_TYPE_ROW_EXPR );

            sRowExpr1 = sListColLeft->mArgs[0]->mExpr.mRowExpr;
            
            STL_DASSERT( sRowExpr1->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN );

            sLeftIdx  = *sRowExpr1->mArgs[0]->mExpr.mInnerColumn->mIdx;

            /* column value 설정 */
            GET_VALUE_BLOCK_WITH_COLUMN_ORDER( sValueBlock,
                                               sLeftIdx );

            sCurFilter->mColIdx1 = sLeftIdx;
            sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;
                            
            STL_DASSERT( sListFunc->mArgs[0]->mType == QLV_EXPR_TYPE_LIST_COLUMN );

            sListColRight = sListFunc->mArgs[0]->mExpr.mListCol;
            STL_DASSERT( sListColRight->mArgs[0]->mType == QLV_EXPR_TYPE_ROW_EXPR );

//            sRowExpr2 = sListColRight->mArgs[0]->mExpr.mRowExpr;

            /* Constant value 설정 */

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( knlColumnInReadRow ),
                                        (void **) & sColumnInReadRow,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( dtlDataValue ) * ( sListColRight->mArgCount + 1 ),
                                        (void **) & sDataValue,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            
            /**
             * @todo IN Physical Function Pointer
             * aIsInPhysical = STL_TRUE인 경우, 기존의 Physical Function 과 달리
             * Left와 Right의 void형 포인트로 dtlDataValue 가 list 로 오게된다.
             * Function pointer를 통해 dtlDataValue를 physical function안에서 하나씩 처리하도록 함.
             */
            sDataValue[sListColRight->mArgCount].mType = sColumnDesc[ sLeftIdx ].mType;

            for( sLoop2 = 0 ; sLoop2 < sListColRight->mArgCount ; sLoop2 ++ )
            {
                if( aExprDataContext->mContexts
                    [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mCast != NULL )
                {
                    sDataValue[sLoop2].mValue = & aExprDataContext->mContexts
                        [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mCast->mCastResultBuf;
                }
                else
                {
                    if( (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                        (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_VALUE) ||
                        (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                        (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_OUTER_COLUMN) )
                    {
                        sDataValue[sLoop2].mValue = aExprDataContext->mContexts
                            [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mInfo.mValueInfo->mDataValue;
                    }
                    else
                    {
                        if( sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType ==
                            QLV_EXPR_TYPE_LIST_FUNCTION )
                        {
                            sDataValue[sLoop2].mValue = & aExprDataContext->mContexts
                                [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mInfo.mListFunc->mResultValue;
                        }
                        else
                        {
                            sDataValue[sLoop2].mValue = & aExprDataContext->mContexts
                                [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mInfo.mFuncData->mResultValue;
                        }
                    }
                }
            }
            sColumnInReadRow->mValue  = sDataValue;
            sColumnInReadRow->mLength = sListColRight->mArgCount;
                    
            sCurFilter->mColIdx2  = KNL_PHYSICAL_LIST_VALUE_CONSTANT;
            sCurFilter->mColVal2  = sColumnInReadRow;
            sCurFilter->mConstVal = sDataValue;
            
            sCurFilter->mFunc = dtlInPhysicalFuncPointer[KNL_IN_PHYSICAL_FUNC_ID_PTR( sListFunc->mFuncId )];
            
        }
        else
        {
            /* Coverage : 하위 노드로 push 될 수 있는 filter이다. hint를 확장하면 올 수 있다. */
            STL_DASSERT( 0 );
            
            /* /\* column 하나만으로 구성된 filter *\/  */
            /* STL_DASSERT( sFilterExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN ); */

            /* /\* function pointer 설정 *\/ */
            /* sCurFilter->mFunc = dtlPhysicalFuncIsTrue; */
            
            /* sLeftIdx = *sFilterExpr->mExpr.mInnerColumn->mIdx; */
            
            /* /\* column value 설정 *\/ */
            /* GET_VALUE_BLOCK_WITH_COLUMN_ORDER( sValueBlock, */
            /*                                    sLeftIdx ); */
            
            /* sCurFilter->mColIdx1 = sLeftIdx; */
            /* sCurFilter->mColVal1 = & sValueBlock->mColumnInRow; */

            /* /\* emtpy value 설정 *\/ */
            /* sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT; */
            /* sCurFilter->mColVal2  = knlEmtpyColumnValue; */
            /* sCurFilter->mConstVal = NULL; */
        }

        STL_DASSERT( sCurFilter->mFunc != NULL );
        
        sCurFilter++;
        sListItem = sListItem->mNext;
    }

    
    /**
     * Column1 Order 기준으로 Filter 정렬
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlPhysicalFilterPtr ) * sExprListCnt,
                                (void **) & sSortList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* insertion sort */
    for( sLoop1 = 0 ; sLoop1 < sExprListCnt ; sLoop1++ )
    {
        sSortList[ sLoop1 ] = & sPhysicalFilter[ sLoop1 ];
    }

    for( sLoop1 = 1 ; sLoop1 < sExprListCnt ; sLoop1++ )
    {
        if( sSortList[ sLoop1 - 1 ]->mColIdx1 <= sSortList[ sLoop1 ]->mColIdx1 )
        {
            continue;
        }

        sSortTemp = sSortList[ sLoop1 ];
        sSortList[ sLoop1 ] = sSortList[ sLoop1 - 1 ];
            
        for( sLoop2 = sLoop1 - 1 ; sLoop2 > 0 ; sLoop2-- )
        {
            if( sSortList[ sLoop2 - 1 ]->mColIdx1 <= sSortTemp->mColIdx1 )
            {
                break;
            }
            sSortList[ sLoop2 ] = sSortList[ sLoop2 - 1 ];
        }
        sSortList[ sLoop2 ] = sSortTemp;
    }

    
    /**
     * Physical Filter List의 Link 구성
     */
    
    for( sLoop1 = 0 ; sLoop1 < sExprListCnt - 1 ; sLoop1++ )
    {
        sSortList[ sLoop1 ]->mNext = sSortList[ sLoop1 + 1 ];
    }
    sSortList[ sLoop1 ]->mNext = NULL;


    /**
     * OUTPUT 설정
     */
    
    *aPhysicalFilter = sSortList[0];

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} qlnd */
