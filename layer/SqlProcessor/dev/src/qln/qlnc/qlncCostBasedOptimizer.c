/*******************************************************************************
 * qlncCostBasedOptimizer.c
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
 * @file qlncCostBasedOptimizer.c
 * @brief SQL Processor Layer SELECT statement cost based optimization
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Cost Based Optimizer Function List
 */
const qlncCostBasedOptimizerFuncPtr qlncCostBasedOptimizerFuncList[ QLNC_NODE_TYPE_MAX ] =
{
    qlncCBOptQuerySet,          /**< QLNC_NODE_TYPE_QUERY_SET */
    qlncCBOptSetOP,             /**< QLNC_NODE_TYPE_SET_OP */
    qlncCBOptQuerySpec,         /**< QLNC_NODE_TYPE_QUERY_SPEC */
    qlncCBOptBaseTable,         /**< QLNC_NODE_TYPE_BASE_TABLE */
    qlncCBOptSubTable,          /**< QLNC_NODE_TYPE_SUB_TABLE */
    qlncCBOptJoinTable,         /**< QLNC_NODE_TYPE_JOIN_TABLE */
    NULL,                       /**< QLNC_NODE_TYPE_FLAT_INSTANT */
    qlncCBOptSortInstant,       /**< QLNC_NODE_TYPE_SORT_INSTANT */
    qlncCBOptHashInstant,       /**< QLNC_NODE_TYPE_HASH_INSTANT */
    NULL,
    qlncCBOptInnerJoinTable,    /**< QLNC_NODE_TYPE_INNER_JOIN_TABLE */
    qlncCBOptOuterJoinTable,    /**< QLNC_NODE_TYPE_OUTER_JOIN_TABLE */
    qlncCBOptSemiJoinTable,     /**< QLNC_NODE_TYPE_SEMI_JOIN_TABLE */
    qlncCBOptAntiSemiJoinTable  /**< QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE */
};


/**
 * @brief Query Set Node의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptQuerySet( qlncCBOptParamInfo * aCBOptParamInfo,
                             qllEnv             * aEnv )
{
    qlncQuerySet    * sCandQuerySet     = NULL;
    qlncCostCommon  * sQuerySetCost     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_QUERY_SET,
                        QLL_ERROR_STACK(aEnv) );
 

    /* 분석 여부 체크 */
    if( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE )
    {
        STL_THROW( RAMP_FINISH );
    }


    /**
     * Query Set Candidate Node 설정
     */

    sCandQuerySet = (qlncQuerySet*)(aCBOptParamInfo->mLeftNode);


    /**
     * Child Node Optimizer
     */

    aCBOptParamInfo->mLeftNode = sCandQuerySet->mChildNode;
    STL_TRY( qlncCostBasedOptimizerFuncList[ sCandQuerySet->mChildNode->mNodeType ]( aCBOptParamInfo,
                                                                                     aEnv )
             == STL_SUCCESS );

    /* Query Set Cost 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncCostCommon ),
                                (void**) &sQuerySetCost,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Query Set Cost 계산 */
    sQuerySetCost->mFirstFilterCost =
        sCandQuerySet->mChildNode->mBestCost->mFirstFilterCost;
    sQuerySetCost->mNextFilterCost =
        sCandQuerySet->mChildNode->mBestCost->mNextFilterCost;
    sQuerySetCost->mBuildCost =
        sCandQuerySet->mChildNode->mBestCost->mBuildCost;
    sQuerySetCost->mReadIOCost =
        ( sCandQuerySet->mChildNode->mBestCost->mReadIOCost +
          sCandQuerySet->mChildNode->mBestCost->mOutputIOCost );
    sQuerySetCost->mOutputIOCost = (stlFloat64)0.0;

    QLNC_SET_TOTAL_COST( sQuerySetCost );
    sQuerySetCost->mResultRowCount =
        sCandQuerySet->mChildNode->mBestCost->mResultRowCount;

    /* Query Set Node에 Query Set Cost 설정 */
    sCandQuerySet->mQuerySetCost = sQuerySetCost;

    /* Query Set Node에 Best Cost 등록 */
    sCandQuerySet->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_QUERY_SET;
    sCandQuerySet->mNodeCommon.mBestCost = sQuerySetCost;

    /* Query Set Node에 분석여부 설정 */
    sCandQuerySet->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Set OP Node의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptSetOP( qlncCBOptParamInfo    * aCBOptParamInfo,
                          qllEnv                * aEnv )
{
    stlInt32          i;
    qlncSetOP       * sCandSetOP    = NULL;
    qlncCostCommon  * sSetOPCost    = NULL;

    stlFloat64        sFilterCost;
    stlFloat64        sBuildCost;
    stlFloat64        sReadIOCost;
    stlFloat64        sResultRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_SET_OP,
                        QLL_ERROR_STACK(aEnv) );
 

    /* 분석 여부 체크 */
    if( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE )
    {
        /* Set OP는 항상 Query Set에서 호출되는 구조로 되어 있어
         * 여기로 들어오는 경우는 없을 것으로 판단됨 */

        STL_THROW( RAMP_FINISH );
    }


    /**
     * Set OP Candidate Node 설정
     */

    sCandSetOP = (qlncSetOP*)(aCBOptParamInfo->mLeftNode);


    /**
     * Child Node Optimizer
     */

    sFilterCost = (stlFloat64)0.0;
    sBuildCost = (stlFloat64)0.0;
    sReadIOCost = (stlFloat64)0.0;
    sResultRowCount = (stlFloat64)0.0;

    for( i = 0; i < sCandSetOP->mChildCount; i++ )
    {
        aCBOptParamInfo->mLeftNode = sCandSetOP->mChildNodeArr[i];
        STL_TRY( qlncCostBasedOptimizerFuncList[ sCandSetOP->mChildNodeArr[i]->mNodeType ](
                     aCBOptParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        sFilterCost +=
            ( sCandSetOP->mChildNodeArr[i]->mBestCost->mFirstFilterCost +
              sCandSetOP->mChildNodeArr[i]->mBestCost->mNextFilterCost +
              sCandSetOP->mChildNodeArr[i]->mBestCost->mReadIOCost +
              sCandSetOP->mChildNodeArr[i]->mBestCost->mOutputIOCost );
        sBuildCost +=
            sCandSetOP->mChildNodeArr[i]->mBestCost->mBuildCost;
        sResultRowCount += sCandSetOP->mChildNodeArr[i]->mBestCost->mResultRowCount;
    }

    /* Query Set Cost 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncCostCommon ),
                                (void**) &sSetOPCost,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Query Set Cost 계산 */
    sSetOPCost->mFirstFilterCost = (stlFloat64)0.0;
    sSetOPCost->mNextFilterCost = sFilterCost;
    sSetOPCost->mBuildCost = sBuildCost;
    sSetOPCost->mReadIOCost = sReadIOCost;
    sSetOPCost->mOutputIOCost = (stlFloat64)0.0;

    QLNC_SET_TOTAL_COST( sSetOPCost );
    sSetOPCost->mResultRowCount = sResultRowCount;

    /* Set OP Node에 Set OP Cost 설정 */
    sCandSetOP->mSetOPCost = sSetOPCost;

    /* Set OP Node에 Best Cost 등록 */
    sCandSetOP->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_SET_OP;
    sCandSetOP->mNodeCommon.mBestCost = sSetOPCost;

    /* Set OP Node에 분석여부 설정 */
    sCandSetOP->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Spec Node의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptQuerySpec( qlncCBOptParamInfo    * aCBOptParamInfo,
                              qllEnv                * aEnv )
{
    qlncQuerySpec       * sCandQuerySpec    = NULL;
    qlncQuerySpecCost   * sQuerySpecCost    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC,
                        QLL_ERROR_STACK(aEnv) );


    /* 분석 여부 체크 */
    if( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE )
    {
        STL_THROW( RAMP_FINISH );
    }


    /**
     * Query Spec Candidate Node 설정
     */

    sCandQuerySpec = (qlncQuerySpec*)(aCBOptParamInfo->mLeftNode);


    /**
     * Child Node Optimizer
     */

    aCBOptParamInfo->mLeftNode = sCandQuerySpec->mChildNode;
    STL_TRY( qlncCostBasedOptimizerFuncList[ sCandQuerySpec->mChildNode->mNodeType ]( aCBOptParamInfo,
                                                                                      aEnv )
             == STL_SUCCESS );


    /**
     * Child Node가 SubTable인 경우 SubQuery Filter가 존재하면 하위의 적정 노드로 Push Down
     */

    if( sCandQuerySpec->mChildNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
    {

        STL_TRY( qlncPushDownSubQueryFilter(
                     &(aCBOptParamInfo->mParamInfo),
                     sCandQuerySpec->mChildNode,
                     (qlncNodeCommon*)sCandQuerySpec,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* Query Spec Cost 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncQuerySpecCost ),
                                (void**) &sQuerySpecCost,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sQuerySpecCost->mChildNode = sCandQuerySpec->mChildNode;

    /* Query Spec Cost 계산 */
    sQuerySpecCost->mCostCommon.mFirstFilterCost =
        sCandQuerySpec->mChildNode->mBestCost->mFirstFilterCost;
    sQuerySpecCost->mCostCommon.mNextFilterCost =
        sCandQuerySpec->mChildNode->mBestCost->mNextFilterCost;
    sQuerySpecCost->mCostCommon.mBuildCost =
        sCandQuerySpec->mChildNode->mBestCost->mBuildCost;
    sQuerySpecCost->mCostCommon.mReadIOCost =
        ( sCandQuerySpec->mChildNode->mBestCost->mReadIOCost +
          sCandQuerySpec->mChildNode->mBestCost->mOutputIOCost );
    sQuerySpecCost->mCostCommon.mOutputIOCost = (stlFloat64)0.0;

    QLNC_SET_TOTAL_COST( &(sQuerySpecCost->mCostCommon) );
    sQuerySpecCost->mCostCommon.mResultRowCount =
        sCandQuerySpec->mChildNode->mBestCost->mResultRowCount;

    /* Query Spec Node에 Query Spec Cost 설정 */
    sCandQuerySpec->mQuerySpecCost = sQuerySpecCost;

    /* Query Spec Node에 Best Cost 등록 */
    sCandQuerySpec->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_QUERY_SPEC;
    sCandQuerySpec->mNodeCommon.mBestCost = (qlncCostCommon*)sQuerySpecCost;

    /* Query Spec Node에 분석여부 설정 */
    sCandQuerySpec->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Base Table의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptBaseTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                              qllEnv                * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    if( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_FALSE )
    {
        STL_TRY( qlncCBOptBaseTableInternal( aCBOptParamInfo,
                                             aEnv )
                 == STL_SUCCESS );

        /* Trace Log 정보 출력 */
        if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
        {
            STL_TRY( qlncTraceCBOptBaseTable( &QLL_CANDIDATE_MEM( aEnv ),
                                              (qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode),
                                              aEnv )
                     == STL_SUCCESS );
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Copied Base Table의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptCopiedBaseTable( qlncCBOptParamInfo  * aCBOptParamInfo,
                                    qllEnv              * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    if( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_FALSE )
    {
        STL_TRY( qlncCBOptBaseTableInternal( aCBOptParamInfo,
                                             aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Base Table의 실제 Cost 계산을 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptBaseTableInternal( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qllEnv                * aEnv )
{
    stlInt32              i;
    qlncBaseTableNode   * sCandBaseTable    = NULL;
    qlncRefExprItem     * sRefExprItem      = NULL;
    stlBool               sPossibleTableScan;
    stlBool               sPossibleIndexScan;
    stlBool               sPossibleIndexCombine;
    stlBool               sPossibleInKeyRange;
    stlBool               sPossibleRowidScan;

    qlncRefExprList     * sInKeyRangeAndFilterListArr;
    qlncListColMapList ** sInKeyRangeListColMapListArr;
    qlncIndexScanCost     sTmpIndexScanCost;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /* Candidate Base Table Node 설정 */
    sCandBaseTable = (qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode);

    sCandBaseTable->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_NA;
    sCandBaseTable->mNodeCommon.mBestCost = NULL;

    /* Scan Hint 설정 */
    sPossibleTableScan = sCandBaseTable->mAccPathHint->mPossibleTableScan;
    sPossibleIndexScan = sCandBaseTable->mAccPathHint->mPossibleIndexScan;
    sPossibleIndexCombine = sCandBaseTable->mAccPathHint->mPossibleIndexCombine;
    sPossibleInKeyRange = sCandBaseTable->mAccPathHint->mPossibleInKeyRange;
    sPossibleRowidScan = sCandBaseTable->mAccPathHint->mPossibleRowidScan;

    /* Index Combine 가능 여부 체크 (Rowid Scan 가능 여부 체크보다 반드시 먼저 하도록 한다.) */
    if( sPossibleIndexCombine == STL_TRUE )
    {
        stlInt32      sDistribCnfCount;

        STL_DASSERT( sCandBaseTable->mAccPathHint->mPossibleIndexScanCount > 0 );

        /* DNF로 처리 가능한지 체크 */
        sDistribCnfCount = qlncGetDistribCnfCount( sCandBaseTable->mFilter );
        if( (sDistribCnfCount < 2) ||
            (sDistribCnfCount > QLNC_MAX_DISTRIBUTED_CNF_COUNT ) ||
            (ellGetTableType( sCandBaseTable->mTableInfo->mTableHandle ) != ELL_TABLE_TYPE_BASE_TABLE) )
        {
            /* Index Combine이 불가능한 경우 Access Path Hint 재조정 */
            sPossibleIndexCombine = STL_FALSE;
            if( (sPossibleTableScan == STL_FALSE) &&
                (sPossibleIndexScan == STL_FALSE) &&
                (sPossibleRowidScan == STL_FALSE) &&
                (sPossibleInKeyRange == STL_FALSE) )
            {
                /* Index Combine Hint만 주어진 경우 */
                sPossibleTableScan = STL_TRUE;
                sPossibleIndexScan = STL_TRUE;
                sPossibleRowidScan = STL_TRUE;
                sPossibleInKeyRange = STL_TRUE;
            }
        }
        else
        {
            /* SubQuery가 포함된 Filter이면 Index Combine 불가 */
            STL_DASSERT( sCandBaseTable->mFilter != NULL );
            if( qlncIsExistSubQueryFilter( sCandBaseTable->mFilter ) == STL_TRUE )
            {
                sPossibleIndexCombine = STL_FALSE;
                if( (sPossibleTableScan == STL_FALSE) &&
                    (sPossibleIndexScan == STL_FALSE) &&
                    (sPossibleRowidScan == STL_FALSE) &&
                    (sPossibleInKeyRange == STL_FALSE) )
                {
                    /* Index Combine Hint만 주어진 경우 */
                    sPossibleTableScan = STL_TRUE;
                    sPossibleIndexScan = STL_TRUE;
                    sPossibleRowidScan = STL_TRUE;
                    sPossibleInKeyRange = STL_TRUE;
                }
            }
        }
    }

    /* Rowid Scan 가능 여부 체크 */
    if( sPossibleRowidScan == STL_TRUE )
    {
        stlBool   sHasRowidFilter;

        /* Rowid Scan 가능여부 체크 */
        sHasRowidFilter = STL_FALSE;
        if( sCandBaseTable->mFilter != NULL )
        {
            for( i = 0; i < sCandBaseTable->mFilter->mOrCount; i++ )
            {
                if( qlncIsRowidORFilter( sCandBaseTable,
                                         sCandBaseTable->mFilter->mOrFilters[i] )
                    == STL_TRUE )
                {
                    sHasRowidFilter = STL_TRUE;
                    break;
                }
            }
        }

        /* Rowid Scan이 불가능한 경우 Access Path Hint 재조정 */
        if( sHasRowidFilter == STL_FALSE )
        {
            sPossibleRowidScan = STL_FALSE;
            if( (sPossibleTableScan == STL_FALSE) &&
                (sPossibleIndexScan == STL_FALSE) &&
                (sPossibleIndexCombine == STL_FALSE) &&
                (sPossibleInKeyRange == STL_FALSE) )
            {
                /* Rowid Scan Hint만 주어진 경우 -> Table Scan으로 변경 */
                sPossibleTableScan = STL_TRUE;
            }
        }
    }

    /* In Key Range 가능 여부 체크 */
    if( sPossibleInKeyRange == STL_TRUE )
    {
        /* In Key Range And Filter List Array 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncRefExprList ) * sCandBaseTable->mAccPathHint->mPossibleIndexScanCount,
                                    (void**)&sInKeyRangeAndFilterListArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* In Key Range List Column Map List Array 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncListColMapList* ) * sCandBaseTable->mAccPathHint->mPossibleIndexScanCount,
                                    (void**)&sInKeyRangeListColMapListArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( i = 0; i < sCandBaseTable->mAccPathHint->mPossibleIndexScanCount; i++ )
        {
            /* Filter로 부터 In-Key Range 대상이 될 수 있는 List Function 들을 얻어온다 */
            STL_TRY( qlncGetInKeyRangeAndFilter( aCBOptParamInfo,
                                                 &(sCandBaseTable->mAccPathHint->mPossibleIndexScanInfoArr[i]),
                                                 sCandBaseTable->mFilter,
                                                 &sInKeyRangeAndFilterListArr[i],
                                                 &sInKeyRangeListColMapListArr[i],
                                                 aEnv )
                     == STL_SUCCESS );
        }

        for( i = 0; i < sCandBaseTable->mAccPathHint->mPossibleIndexScanCount; i++ )
        {
            if( sInKeyRangeAndFilterListArr[i].mCount > 0 )
            {
                break;
            }
        }

        /* In Key Range가 불가능한 경우 Access Path Hint 재조정 */
        if( i == sCandBaseTable->mAccPathHint->mPossibleIndexScanCount )
        {
            sPossibleInKeyRange = STL_FALSE;
            if( (sPossibleTableScan == STL_FALSE) &&
                (sPossibleIndexScan == STL_FALSE) &&
                (sPossibleIndexCombine == STL_FALSE) &&
                (sPossibleRowidScan == STL_FALSE) )
            {
                /* In Key Range Hint만 주어진 경우 -> Table Scan으로 변경 */
                sPossibleTableScan = STL_TRUE;
            }
        }
    }


    STL_RAMP( RAMP_RETRY );


    /* Table Scan Cost 계산 */
    if( sPossibleTableScan == STL_TRUE )
    {
        /* Table Scan Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncTableScanCost ),
                                    (void**)&(sCandBaseTable->mTableScanCost),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        aCBOptParamInfo->mFilter = sCandBaseTable->mFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(sCandBaseTable->mTableScanCost);

        STL_TRY( qlncCBOptTableScanCost( aCBOptParamInfo,
                                         aEnv )
                 == STL_SUCCESS );

        sCandBaseTable->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_TABLE_SCAN;
        sCandBaseTable->mNodeCommon.mBestCost = (qlncCostCommon*)(sCandBaseTable->mTableScanCost);
    }
    else
    {
        sCandBaseTable->mTableScanCost = NULL;
    }

    /* Index Scan Cost 계산 */
    if( sPossibleIndexScan == STL_TRUE )
    {
        sCandBaseTable->mIndexScanCostCount = sCandBaseTable->mAccPathHint->mPossibleIndexScanCount;

        /* Index Scan Info 생성 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncIndexScanCost ) * sCandBaseTable->mIndexScanCostCount,
                                    (void**)&(sCandBaseTable->mIndexScanCostArr),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Index 에 대한 Cost 를 평가한다. */
        for( i = 0; i < sCandBaseTable->mIndexScanCostCount; i++ )
        {
            aCBOptParamInfo->mFilter = sCandBaseTable->mFilter;
            aCBOptParamInfo->mCost = (qlncCostCommon*)&(sCandBaseTable->mIndexScanCostArr[i]);

            STL_TRY( qlncCBOptIndexScanCost( aCBOptParamInfo,
                                             &(sCandBaseTable->mAccPathHint->mPossibleIndexScanInfoArr[i]),
                                             sCandBaseTable->mAccPathHint->mPossibleIndexScanInfoArr[i].mIsAsc,
                                             NULL,
                                             aEnv )
                     == STL_SUCCESS );

            sCandBaseTable->mIndexScanCostArr[i].mListColMap = NULL;

            /**
             * Index의 Range Filter가 존재하는 경우 무조건 우선적으로 선택되도록 한다.
             * 만약 Index Range Scan이 다수 존재하는 경우 각각의 Cost 비교에 의해 선택한다.
             * In Key Range가 더 좋은 경우 In Key Range를 선택하고,
             * Index Combine이나 Rowid Scan의 경우에도 Cost가 더 좋다면 해당 방법의 Scan을 선택한다.
             */
            if( (sCandBaseTable->mIndexScanCostArr[i].mIndexRangeAndFilter != NULL) &&
                (sCandBaseTable->mIndexScanCostArr[i].mIndexRangeAndFilter->mOrCount > 0) )
            {
                if( (sCandBaseTable->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_SCAN) &&
                    (((qlncIndexScanCost*)(sCandBaseTable->mNodeCommon.mBestCost))->mIndexRangeAndFilter != NULL) &&
                    (((qlncIndexScanCost*)(sCandBaseTable->mNodeCommon.mBestCost))->mIndexRangeAndFilter->mOrCount > 0) )
                {
                    if( QLNC_LESS_THAN_DOUBLE( sCandBaseTable->mIndexScanCostArr[i].mCostCommon.mTotalCost,
                                               sCandBaseTable->mNodeCommon.mBestCost->mTotalCost ) )
                    {
                        sCandBaseTable->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_INDEX_SCAN;
                        sCandBaseTable->mNodeCommon.mBestCost =
                            (qlncCostCommon*)&(sCandBaseTable->mIndexScanCostArr[i]);
                    }
                }
                else
                {
                    sCandBaseTable->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_INDEX_SCAN;
                    sCandBaseTable->mNodeCommon.mBestCost =
                        (qlncCostCommon*)&(sCandBaseTable->mIndexScanCostArr[i]);
                }
            }
            else
            {
                if( (sCandBaseTable->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_SCAN) &&
                    (((qlncIndexScanCost*)(sCandBaseTable->mNodeCommon.mBestCost))->mIndexRangeAndFilter != NULL) &&
                    (((qlncIndexScanCost*)(sCandBaseTable->mNodeCommon.mBestCost))->mIndexRangeAndFilter->mOrCount > 0) )
                {
                    /* Do Nothing */
                }
                else
                {
                    if( (sCandBaseTable->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
                        (QLNC_LESS_THAN_DOUBLE( sCandBaseTable->mIndexScanCostArr[i].mCostCommon.mTotalCost,
                                                sCandBaseTable->mNodeCommon.mBestCost->mTotalCost )) )
                    {
                        sCandBaseTable->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_INDEX_SCAN;
                        sCandBaseTable->mNodeCommon.mBestCost =
                            (qlncCostCommon*)&(sCandBaseTable->mIndexScanCostArr[i]);
                    }
                }
            }
        }
    }
    else
    {
        sCandBaseTable->mIndexScanCostCount = 0;
        sCandBaseTable->mIndexScanCostArr = NULL;
    }

    /* In Key Range Cost 계산 */
    if( sPossibleInKeyRange == STL_TRUE )
    {
        sCandBaseTable->mBestInKeyRangeCostCount = sCandBaseTable->mAccPathHint->mPossibleIndexScanCount;

        /* Best In Key Range Info 생성 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncIndexScanCost ) * sCandBaseTable->mBestInKeyRangeCostCount,
                                    (void**)&(sCandBaseTable->mBestInKeyRangeCostArr),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sCandBaseTable->mBestInKeyRangeCostArr,
                   0x00,
                   STL_SIZEOF( qlncIndexScanCost ) * sCandBaseTable->mBestInKeyRangeCostCount );

        for( i = 0; i < sCandBaseTable->mBestInKeyRangeCostCount; i++ )
        {
            if( sInKeyRangeAndFilterListArr[i].mCount > 0 )
            {
                /* In-Key Range 고려하여 각각의 Index 에 대한 Cost 를 평가한다 */
                sRefExprItem = sInKeyRangeAndFilterListArr[i].mHead;
                while( sRefExprItem != NULL )
                {
                    aCBOptParamInfo->mFilter = (qlncAndFilter*)sRefExprItem->mExpr;
                    aCBOptParamInfo->mCost = (qlncCostCommon*)&sTmpIndexScanCost;

                    STL_TRY( qlncCBOptIndexScanCost( aCBOptParamInfo,
                                                     &(sCandBaseTable->mAccPathHint->mPossibleIndexScanInfoArr[i]),
                                                     sCandBaseTable->mAccPathHint->mPossibleIndexScanInfoArr[i].mIsAsc,
                                                     sRefExprItem->mExpr,
                                                     aEnv )
                             == STL_SUCCESS );

                    sTmpIndexScanCost.mListColMap = & sInKeyRangeListColMapListArr[i]->mListColMap;

                    /* 반드시 Range를 타는 경우에 대해서만 적용할 수 있다. */
                    if( qlncIsPossibleInKeyRange( &sTmpIndexScanCost ) == STL_TRUE )
                    {
                        /* In-Key Range의 Right 쪽의 Value 개수만큼 Cost를 곱한다. */
                        sTmpIndexScanCost.mCostCommon.mTotalCost *=
                            sInKeyRangeListColMapListArr[i]->mListColMap.mMapCount;
                        sTmpIndexScanCost.mCostCommon.mFirstFilterCost *=
                            sInKeyRangeListColMapListArr[i]->mListColMap.mMapCount;
                        sTmpIndexScanCost.mCostCommon.mNextFilterCost *=
                            sInKeyRangeListColMapListArr[i]->mListColMap.mMapCount;

                        if( (sCandBaseTable->mBestInKeyRangeCostArr[i].mIndexHandle == NULL) ||
                            (QLNC_LESS_THAN_DOUBLE( sTmpIndexScanCost.mCostCommon.mTotalCost,
                                                    sCandBaseTable->mBestInKeyRangeCostArr[i].mCostCommon.mTotalCost )) )
                        {
                            stlMemcpy( &sCandBaseTable->mBestInKeyRangeCostArr[i],
                                       &sTmpIndexScanCost,
                                       STL_SIZEOF( qlncIndexScanCost ) );
                        }
                    }

                    sRefExprItem = sRefExprItem->mNext;
                    sInKeyRangeListColMapListArr[i] = sInKeyRangeListColMapListArr[i]->mNext;
                }

                /* Best In Key Range Cost를 Base Table의 Best Cost와 비교한다. */
                if( sCandBaseTable->mBestInKeyRangeCostArr[i].mIndexHandle != NULL )
                {
                    if( (sCandBaseTable->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
                        (QLNC_LESS_THAN_DOUBLE( sCandBaseTable->mBestInKeyRangeCostArr[i].mCostCommon.mTotalCost,
                                                sCandBaseTable->mNodeCommon.mBestCost->mTotalCost )) )
                    {
                        sCandBaseTable->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_INDEX_SCAN;
                        sCandBaseTable->mNodeCommon.mBestCost =
                            (qlncCostCommon*)&(sCandBaseTable->mBestInKeyRangeCostArr[i]);
                    }
                }
            }
        }

        if( sCandBaseTable->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA )
        {
            if( (sPossibleTableScan == STL_FALSE) &&
                (sPossibleIndexScan == STL_FALSE) &&
                (sPossibleIndexCombine == STL_FALSE) &&
                (sPossibleRowidScan == STL_FALSE) )
            {
                /* In Key Range Hint만 주어진 경우 -> Table Scan으로 변경 */
                sPossibleTableScan = STL_TRUE;
                sPossibleInKeyRange = STL_FALSE;

                /* 다시 수행한다. */
                STL_THROW( RAMP_RETRY );
            }
        }
    }
    else
    {
        sCandBaseTable->mBestInKeyRangeCostCount = 0;
        sCandBaseTable->mBestInKeyRangeCostArr = NULL;
    }


    /* Rowid Scan Cost 계산 */
    if( sPossibleRowidScan == STL_TRUE )
    {
        /* Rowid Scan Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncRowidScanCost ),
                                    (void**)&(sCandBaseTable->mRowidScanCost),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        aCBOptParamInfo->mFilter = sCandBaseTable->mFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(sCandBaseTable->mRowidScanCost);

        STL_TRY( qlncCBOptRowidScanCost( aCBOptParamInfo,
                                         aEnv )
                 == STL_SUCCESS );

        if( (sCandBaseTable->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sCandBaseTable->mRowidScanCost->mCostCommon.mTotalCost,
                                    sCandBaseTable->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sCandBaseTable->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_ROWID_SCAN;
            sCandBaseTable->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sCandBaseTable->mRowidScanCost);
        }
    }
    else
    {
        sCandBaseTable->mRowidScanCost = NULL;
    }

    /* Index Combine Cost 계산 */
    if( sPossibleIndexCombine == STL_TRUE )
    {
        /* Index Combine Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncIndexCombineCost ),
                                    (void**)&(sCandBaseTable->mIndexCombineCost),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        aCBOptParamInfo->mFilter = sCandBaseTable->mFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(sCandBaseTable->mIndexCombineCost);

        STL_TRY( qlncCBOptIndexCombineCost( aCBOptParamInfo,
                                            aEnv )
                 == STL_SUCCESS );

        if( (sCandBaseTable->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sCandBaseTable->mIndexCombineCost->mCostCommon.mTotalCost,
                                    sCandBaseTable->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sCandBaseTable->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_INDEX_COMBINE;
            sCandBaseTable->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sCandBaseTable->mIndexCombineCost);
        }
    }
    else
    {
        sCandBaseTable->mIndexCombineCost = NULL;
    }

    /* Cost 분석여부 설정 */
    sCandBaseTable->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Table의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptSubTable( qlncCBOptParamInfo     * aCBOptParamInfo,
                             qllEnv                 * aEnv )
{
    stlInt32              i;
    stlBool               sHasGroupBy;
    knlRegionMark         sRegionMemMark;
    qlncSubTableNode    * sCandSubTable     = NULL;
    qlncSubTableNode    * sCopiedSubTable   = NULL;
    qlncNodeCommon      * sChildNode        = NULL;
    qlncAndFilter       * sPushFilter       = NULL;
    qlncExprCommon      * sTmpExpr          = NULL;
    qlncNodeCommon      * sTmpNode          = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /* 분석 여부 체크 */
    if( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE )
    {
        STL_THROW( RAMP_FINISH );
    }

    sCandSubTable = (qlncSubTableNode*)(aCBOptParamInfo->mLeftNode);

    /* Query Node에 대한 Cost 계산 수행 */
    aCBOptParamInfo->mLeftNode = sCandSubTable->mQueryNode;
    STL_TRY( qlncCostBasedOptimizerFuncList[ sCandSubTable->mQueryNode->mNodeType ]( aCBOptParamInfo,
                                                                                     aEnv )
             == STL_SUCCESS );

    /* Filter Cost 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncFilterCost ),
                                (void**) &(sCandSubTable->mFilterCost),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Filter Cost 계산 */
    aCBOptParamInfo->mLeftNode = (qlncNodeCommon*)sCandSubTable;
    aCBOptParamInfo->mFilter = sCandSubTable->mFilter;
    aCBOptParamInfo->mCost = (qlncCostCommon*)(sCandSubTable->mFilterCost);

    STL_TRY( qlncCBOptFilterCost( aCBOptParamInfo,
                                  aEnv )
             == STL_SUCCESS );

    /* Cost 분석여부 설정 */
    sCandSubTable->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_FILTER;
    sCandSubTable->mNodeCommon.mBestCost = (qlncCostCommon*)(sCandSubTable->mFilterCost);
    sCandSubTable->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;


    /* Filter Push에 의한 Cost 계산 */
    if( (sCandSubTable->mFilter != NULL) &&
        (sCandSubTable->mQueryNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC) &&
        (((qlncQuerySpec*)(sCandSubTable->mQueryNode))->mOffset == NULL) &&
        (((qlncQuerySpec*)(sCandSubTable->mQueryNode))->mLimit == NULL) )
    {
        /* Region Memory Mark */
        KNL_INIT_REGION_MARK( &sRegionMemMark );
        STL_TRY( knlMarkRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                   &sRegionMemMark,
                                   KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Sub Table Node 복사 */
        STL_TRY( qlncCopyAllNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                (qlncNodeCommon*)sCandSubTable,
                                                (qlncNodeCommon**)&sCopiedSubTable,
                                                aEnv )
                 == STL_SUCCESS );

        /* Push Filter 할당 및 초기화 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncAndFilter ),
                                    (void**) &sPushFilter,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_AND_FILTER( sPushFilter );

        /* Group By Node가 존재하는지 체크 */
        if( ((qlncQuerySpec*)(sCopiedSubTable->mQueryNode))->mChildNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
        {
            sTmpNode = ((qlncSortInstantNode*)(((qlncQuerySpec*)(sCopiedSubTable->mQueryNode))->mChildNode))->mChildNode;
        }
        else
        {
            sTmpNode = ((qlncQuerySpec*)(sCopiedSubTable->mQueryNode))->mChildNode;
        }

        if( sTmpNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT )
        {
            if( ((qlncHashInstantNode*)sTmpNode)->mInstantType == QLNC_INSTANT_TYPE_DISTINCT )
            {
                sTmpNode = ((qlncHashInstantNode*)sTmpNode)->mChildNode;
                if( sTmpNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT )
                {
                    STL_DASSERT( ((qlncHashInstantNode*)sTmpNode)->mInstantType == QLNC_INSTANT_TYPE_GROUP );
                    sHasGroupBy = STL_TRUE;
                }
                else
                {
                    sHasGroupBy = STL_FALSE;
                }
            }
            else
            {
                STL_DASSERT( ((qlncHashInstantNode*)sTmpNode)->mInstantType == QLNC_INSTANT_TYPE_GROUP );
                sHasGroupBy = STL_TRUE;
            }
        }
        else
        {
            sHasGroupBy = STL_FALSE;
        }

        for( i = 0; i < sCopiedSubTable->mFilter->mOrCount; i++ )
        {
            /* SubQuery를 갖는 Filter는 더이상 Push하지 않는다. */
            if( sCopiedSubTable->mFilter->mOrFilters[i]->mSubQueryExprList != NULL )
            {
                continue;
            }

            /* Aggregation을 갖는 Filter는 Push하지 못한다. */
            if( qlncHasAggregationOnExpr(
                    (qlncExprCommon*)(sCopiedSubTable->mFilter->mOrFilters[i]),
                    sCopiedSubTable->mNodeCommon.mNodeID,
                    (sHasGroupBy == STL_TRUE ? STL_TRUE : STL_FALSE) )
                == STL_TRUE )
            {
                continue;
            }

            /* Filter가 변경될 수 있으므로 Duplicate한다. */
            STL_TRY( qlncDuplicateExpr( &(aCBOptParamInfo->mParamInfo),
                                        (qlncExprCommon*)(sCopiedSubTable->mFilter->mOrFilters[i]),
                                        &sTmpExpr,
                                        aEnv )
                     == STL_SUCCESS );

            /* Filter의 Inner Column을 Target의 Expression으로 변경한다. */
            STL_TRY( qlncFPDChangeInnerColumnToTargetExpr(
                         &(aCBOptParamInfo->mParamInfo),
                         sCopiedSubTable->mNodeCommon.mNodeID,
                         ((qlncQuerySpec*)(sCopiedSubTable->mQueryNode))->mTargetCount,
                         ((qlncQuerySpec*)(sCopiedSubTable->mQueryNode))->mTargetArr,
                         sTmpExpr,
                         &sTmpExpr,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                        sPushFilter,
                                                        1,
                                                        (qlncOrFilter**)&sTmpExpr,
                                                        aEnv )
                     == STL_SUCCESS );

            sCopiedSubTable->mFilter->mOrFilters[i] = NULL;
        }

        /* Push Filter가 없으면 종료 */
        if( sPushFilter->mOrCount == 0 )
        {
            /* Region Memory UnMark */
            STL_TRY( knlFreeUnmarkedRegionMem(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         &sRegionMemMark,
                         STL_FALSE, /* aFreeChunk */
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_THROW( RAMP_FINISH );
        }

        /* Push Filter에 대하여 Reference Node List에서 Outer Table Node를 제거한다. */
        STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                     &(aCBOptParamInfo->mParamInfo),
                     (qlncExprCommon*)sPushFilter,
                     sCopiedSubTable->mQueryNode,
                     (qlncExprCommon**)&sPushFilter,
                     aEnv )
                 == STL_SUCCESS );

        /* Copied Sub Table Node의 Filter Compact */
        STL_TRY( qlncCompactAndFilter( sCopiedSubTable->mFilter,
                                       aEnv )
                 == STL_SUCCESS );

        if( sCopiedSubTable->mFilter->mOrCount == 0 )
        {
            sCopiedSubTable->mFilter = NULL;
        }

        /* Push Filter를 Table Node로 Push */
        sChildNode = ((qlncQuerySpec*)(sCopiedSubTable->mQueryNode))->mChildNode;
        if( sChildNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
        {
            if( ((qlncBaseTableNode*)sChildNode)->mFilter == NULL )
            {
                ((qlncBaseTableNode*)sChildNode)->mFilter = sPushFilter;
            }
            else
            {
                STL_TRY( qlncAppendOrFilterToAndFilterTail(
                             &(aCBOptParamInfo->mParamInfo),
                             ((qlncBaseTableNode*)sChildNode)->mFilter,
                             sPushFilter->mOrCount,
                             sPushFilter->mOrFilters,
                             aEnv )
                         == STL_SUCCESS );
            }
        }
        else if( sChildNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
        {
            if( ((qlncSubTableNode*)sChildNode)->mFilter == NULL )
            {
                ((qlncSubTableNode*)sChildNode)->mFilter = sPushFilter;
            }
            else
            {
                STL_TRY( qlncAppendOrFilterToAndFilterTail(
                             &(aCBOptParamInfo->mParamInfo),
                             ((qlncSubTableNode*)sChildNode)->mFilter,
                             sPushFilter->mOrCount,
                             sPushFilter->mOrFilters,
                             aEnv )
                         == STL_SUCCESS );
            }
        }
        else if( sChildNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE )
        {
            /* 하위 노드가 Inner Join Node이면 Join Condition에 합친다. */
            if( ((qlncJoinTableNode*)sChildNode)->mJoinType == QLNC_JOIN_TYPE_INNER )
            {
                if( ((qlncJoinTableNode*)sChildNode)->mJoinCond == NULL )
                {
                    ((qlncJoinTableNode*)sChildNode)->mJoinCond = sPushFilter;
                }
                else
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail(
                                 &(aCBOptParamInfo->mParamInfo),
                                 ((qlncJoinTableNode*)sChildNode)->mJoinCond,
                                 sPushFilter->mOrCount,
                                 sPushFilter->mOrFilters,
                                 aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                if( ((qlncJoinTableNode*)sChildNode)->mFilter == NULL )
                {
                    ((qlncJoinTableNode*)sChildNode)->mFilter = sPushFilter;
                }
                else
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail(
                                 &(aCBOptParamInfo->mParamInfo),
                                 ((qlncJoinTableNode*)sChildNode)->mFilter,
                                 sPushFilter->mOrCount,
                                 sPushFilter->mOrFilters,
                                 aEnv )
                             == STL_SUCCESS );
                }
            }
        }
        else if( sChildNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
        {
            if( ((qlncSortInstantNode*)sChildNode)->mFilter == NULL )
            {
                ((qlncSortInstantNode*)sChildNode)->mFilter = sPushFilter;
            }
            else
            {
                STL_TRY( qlncAppendOrFilterToAndFilterTail(
                             &(aCBOptParamInfo->mParamInfo),
                             ((qlncSortInstantNode*)sChildNode)->mFilter,
                             sPushFilter->mOrCount,
                             sPushFilter->mOrFilters,
                             aEnv )
                         == STL_SUCCESS );
            }
        }
        else
        {
            STL_DASSERT( sChildNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT );

            if( ((qlncHashInstantNode*)sChildNode)->mFilter == NULL )
            {
                ((qlncHashInstantNode*)sChildNode)->mFilter = sPushFilter;
            }
            else
            {
                STL_TRY( qlncAppendOrFilterToAndFilterTail(
                             &(aCBOptParamInfo->mParamInfo),
                             ((qlncHashInstantNode*)sChildNode)->mFilter,
                             sPushFilter->mOrCount,
                             sPushFilter->mOrFilters,
                             aEnv )
                         == STL_SUCCESS );
            }
        }

        /* Heuristic Query Transformation 수행 */
        STL_TRY( qlncHeuristicQueryTransformation( &(aCBOptParamInfo->mParamInfo),
                                                   (qlncNodeCommon*)sCopiedSubTable,
                                                   aEnv )
                 == STL_SUCCESS );

        /* Query Node에 대한 Cost 계산 수행 */
        aCBOptParamInfo->mLeftNode = sCopiedSubTable->mQueryNode;
        STL_TRY( qlncCostBasedOptimizerFuncList[ sCopiedSubTable->mQueryNode->mNodeType ]( aCBOptParamInfo,
                                                                                           aEnv )
                 == STL_SUCCESS );

        /* Filter Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncFilterCost ),
                                    (void**) &(sCopiedSubTable->mFilterCost),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Filter Cost 계산 */
        aCBOptParamInfo->mLeftNode = (qlncNodeCommon*)sCopiedSubTable;
        aCBOptParamInfo->mFilter = sCopiedSubTable->mFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(sCopiedSubTable->mFilterCost);

        STL_TRY( qlncCBOptFilterCost( aCBOptParamInfo,
                                      aEnv )
                 == STL_SUCCESS );

        /* Cost 분석여부 설정 */
        sCopiedSubTable->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_FILTER;
        sCopiedSubTable->mNodeCommon.mBestCost = (qlncCostCommon*)(sCopiedSubTable->mFilterCost);
        sCopiedSubTable->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;

        /* Copied Sub Table Node의 Cost가 더 좋으면 Sub Table Node를 교체한다.  */
        if( QLNC_LESS_THAN_DOUBLE( sCopiedSubTable->mNodeCommon.mBestCost->mTotalCost,
                                   sCandSubTable->mNodeCommon.mBestCost->mTotalCost ) )
        {
            stlMemcpy( sCandSubTable, sCopiedSubTable, STL_SIZEOF( qlncSubTableNode ) );
        }
        else
        {
            /* Region Memory UnMark */
            STL_TRY( knlFreeUnmarkedRegionMem(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         &sRegionMemMark,
                         STL_FALSE, /* aFreeChunk */
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Table의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptJoinTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                              qllEnv                * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Join Type에 따른 Optimizer
     */

    switch( ((qlncJoinTableNode*)(aCBOptParamInfo->mLeftNode))->mJoinType )
    {
        case QLNC_JOIN_TYPE_INNER:
            /* Inner Join Table Cost Based Optimizer */
            STL_TRY( qlncCBOptBestInnerJoinTable( aCBOptParamInfo,
                                                  aEnv )
                     == STL_SUCCESS );
            break;

        case QLNC_JOIN_TYPE_OUTER:
            /* Outer Join Table Cost Based Optimizer */
            STL_TRY( qlncCBOptBestOuterJoinTable( aCBOptParamInfo,
                                                  aEnv )
                     == STL_SUCCESS );
            break;

        case QLNC_JOIN_TYPE_SEMI:
            /* Semi Join Table Cost Based Optimizer */
            STL_TRY( qlncCBOptBestSemiJoinTable( aCBOptParamInfo,
                                                 1,
                                                 aEnv )
                     == STL_SUCCESS );
            break;

        case QLNC_JOIN_TYPE_ANTI_SEMI:
        case QLNC_JOIN_TYPE_ANTI_SEMI_NA:
            /* Anti Semi Join Table Cost Based Optimizer */
            STL_TRY( qlncCBOptBestAntiSemiJoinTable( aCBOptParamInfo,
                                                     1,
                                                     aEnv )
                     == STL_SUCCESS );
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


#if 0
/**
 * @brief Flat Instant의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptFlatInstant( qlncCBOptParamInfo  * aCBOptParamInfo,
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
                      "qlncCBOptFlatInstant()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}
#endif


/**
 * @brief Sort Instant의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptSortInstant( qlncCBOptParamInfo  * aCBOptParamInfo,
                                qllEnv              * aEnv )
{
    qlncSortInstantNode     * sSortInstantNode  = NULL;
    qlncSortInstantCost     * sSortInstantCost  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT,
                        QLL_ERROR_STACK(aEnv) );


    /* 이미 분석했는지 체크 */
    if( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE )
    {
        STL_THROW( RAMP_FINISH );
    }

    /* Sort Instant Node 설정 */
    sSortInstantNode = (qlncSortInstantNode*)(aCBOptParamInfo->mLeftNode);

    /* Child Node Cost 계산 */
    aCBOptParamInfo->mLeftNode = sSortInstantNode->mChildNode;
    STL_TRY( qlncCostBasedOptimizerFuncList[ sSortInstantNode->mChildNode->mNodeType ]( aCBOptParamInfo,
                                                                                        aEnv )
             == STL_SUCCESS );

    /* Sort Instant Cost 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncSortInstantCost ),
                                (void**) &sSortInstantCost,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Sort Instant Cost 계산 */
    aCBOptParamInfo->mLeftNode = (qlncNodeCommon*)sSortInstantNode;
    aCBOptParamInfo->mCost = (qlncCostCommon*)sSortInstantCost;
    STL_TRY( qlncCBOptSortInstantCost( aCBOptParamInfo,
                                       aEnv )
             == STL_SUCCESS );

    /* Sort Instant Node에 Best Cost 등록 */
    sSortInstantNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_SORT_INSTANT;
    sSortInstantNode->mNodeCommon.mBestCost = (qlncCostCommon*)(sSortInstantNode->mSortInstantCost);

    /* Sort Instant Node에 분석여부 설정 */
    sSortInstantNode->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Instant의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptHashInstant( qlncCBOptParamInfo  * aCBOptParamInfo,
                                qllEnv              * aEnv )
{
    qlncHashInstantNode     * sHashInstantNode  = NULL;
    qlncHashInstantCost     * sHashInstantCost  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT,
                        QLL_ERROR_STACK(aEnv) );


    /* 이미 분석했는지 체크 */
    if( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE )
    {
        STL_THROW( RAMP_FINISH );
    }

    /* Hash Instant Node 설정 */
    sHashInstantNode = (qlncHashInstantNode*)(aCBOptParamInfo->mLeftNode);

    /* Child Node Cost 계산 */
    aCBOptParamInfo->mLeftNode = sHashInstantNode->mChildNode;
    STL_TRY( qlncCostBasedOptimizerFuncList[ sHashInstantNode->mChildNode->mNodeType ]( aCBOptParamInfo,
                                                                                        aEnv )
             == STL_SUCCESS );

    /* Hash Instant Cost 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncHashInstantCost ),
                                (void**) &sHashInstantCost,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Hash Instant Cost 계산 */
    aCBOptParamInfo->mLeftNode = (qlncNodeCommon*)sHashInstantNode;
    aCBOptParamInfo->mCost = (qlncCostCommon*)sHashInstantCost;
    STL_TRY( qlncCBOptHashInstantCost( aCBOptParamInfo,
                                       aEnv )
             == STL_SUCCESS );

    sHashInstantNode->mGroupCost = NULL;

    /* Hash Instant Node에 Best Cost 등록 */
    sHashInstantNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_HASH_INSTANT;
    sHashInstantNode->mNodeCommon.mBestCost = (qlncCostCommon*)(sHashInstantNode->mHashInstantCost);

    /* Hash Instant Node에 분석여부 설정 */
    sHashInstantNode->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Inner Join Table의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptInnerJoinTable( qlncCBOptParamInfo   * aCBOptParamInfo,
                                   qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /* 분석 여부 체크 */
    STL_DASSERT( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Outer Join Table의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptOuterJoinTable( qlncCBOptParamInfo   * aCBOptParamInfo,
                                   qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /* 분석 여부 체크 */
    STL_DASSERT( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Semi Join Table의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptSemiJoinTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                                  qllEnv                * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /* @todo 상위 노드인 Join Table에서 체크되어 여기는 들어오지 않을 것으로 판단됨 */

    /* 분석 여부 체크 */
    STL_DASSERT( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Anti Semi Join Table의 Cost Based Optimizer를 수행한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptAntiSemiJoinTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qllEnv                * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /* 분석 여부 체크 */
    STL_DASSERT( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Inner Join Table의 Cost Based Optimizer를 수행하여 Best Plan을 찾는다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptBestInnerJoinTable( qlncCBOptParamInfo   * aCBOptParamInfo,
                                       qllEnv               * aEnv )
{
    qlncJoinTableNode       * sCandJoinTable        = NULL;
    qlncNodeArray           * sNodeArray            = NULL;

    stlInt32                  i, j;
    stlInt32                  sFixedNodeCount;
    stlInt32                  sNonFixedNodeCount;
    stlInt32                  sSeqNum;

    qlncNodeCommon         ** sFixedNodeArr         = NULL;
    qlncNodeCommon         ** sNonFixedNodeArr      = NULL;
    qlncInnerJoinNodeInfo   * sInnerJoinNodeInfoArr = NULL;

    qlncAndFilter           * sJoinAndFilter        = NULL;

    qlncNodeCommon          * sBestInnerJoinNode    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ((qlncJoinTableNode*)(aCBOptParamInfo->mLeftNode))->mJoinType == QLNC_JOIN_TYPE_INNER,
                        QLL_ERROR_STACK(aEnv) );


    /* 분석 여부 체크 */
    if( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE )
    {
        STL_THROW( RAMP_FINISH );
    }

    /* Join Table Node 설정 */
    sCandJoinTable = (qlncJoinTableNode*)(aCBOptParamInfo->mLeftNode);

    /* Node Array 찾기 */
    sNodeArray = &(sCandJoinTable->mNodeArray);

    /* Fixed Node Array 및 Non-Fixed Node Array */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncNodeCommon* ) * sNodeArray->mCurCnt,
                                (void**)&sFixedNodeArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncNodeCommon* ) * sNodeArray->mCurCnt,
                                (void**)&sNonFixedNodeArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Fixed Node와 Non-Fixed Node를 분리 */
    sFixedNodeCount = 0;
    sNonFixedNodeCount = 0;
    for( i = 0; i < sNodeArray->mCurCnt; i++ )
    {
        /* Node 분석 수행 */
        aCBOptParamInfo->mLeftNode = sNodeArray->mNodeArr[i];
        STL_TRY( qlncCostBasedOptimizerFuncList[ sNodeArray->mNodeArr[i]->mNodeType ]( aCBOptParamInfo,
                                                                                       aEnv )
                 == STL_SUCCESS );

        switch( sNodeArray->mNodeArr[i]->mNodeType )
        {
            case QLNC_NODE_TYPE_BASE_TABLE:
                if( (sNonFixedNodeCount == 0) &&
                    (((qlncBaseTableNode*)(sNodeArray->mNodeArr[i]))->mJoinHint->mFixedOrder == STL_TRUE) )
                {
                    sFixedNodeArr[sFixedNodeCount] = sNodeArray->mNodeArr[i];
                    sFixedNodeCount++;
                }
                else
                {
                    sNonFixedNodeArr[sNonFixedNodeCount] = sNodeArray->mNodeArr[i];
                    sNonFixedNodeCount++;
                }
                break;
            case QLNC_NODE_TYPE_SUB_TABLE:
                if( (sNonFixedNodeCount == 0) &&
                    (((qlncSubTableNode*)(sNodeArray->mNodeArr[i]))->mJoinHint->mFixedOrder == STL_TRUE) )
                {
                    sFixedNodeArr[sFixedNodeCount] = sNodeArray->mNodeArr[i];
                    sFixedNodeCount++;
                }
                else
                {
                    sNonFixedNodeArr[sNonFixedNodeCount] = sNodeArray->mNodeArr[i];
                    sNonFixedNodeCount++;
                }
                break;
            case QLNC_NODE_TYPE_JOIN_TABLE:
                /* Inner Join이나 Outer Join이면 Hint로 순서를 고정할 수 없다. */
                sNonFixedNodeArr[sNonFixedNodeCount] = sNodeArray->mNodeArr[i];
                sNonFixedNodeCount++;
                break;
            default:
                STL_DASSERT( 0 );
                break;
        }
    }

    /* Non Fixed Position 노드들을 Result Row Count 순으로 정렬 */
    for( i = 1; i < sNonFixedNodeCount; i++ )
    {
        for( j = i; j > 0; j-- )
        {
            if( sNonFixedNodeArr[j]->mBestCost->mResultRowCount <
                sNonFixedNodeArr[j-1]->mBestCost->mResultRowCount )
            {
                QLNC_SWAP_NODE( sNonFixedNodeArr[j-1], sNonFixedNodeArr[j] );
            }
            else
            {
                break;
            }
        }
    }

    /* Inner Join Node Info Array */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncInnerJoinNodeInfo ) * sNodeArray->mCurCnt,
                                (void**)&sInnerJoinNodeInfoArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < sFixedNodeCount; i++ )
    {
        sInnerJoinNodeInfoArr[i].mNode = sFixedNodeArr[i];
        sInnerJoinNodeInfoArr[i].mAnalyzedNode = NULL;
        sInnerJoinNodeInfoArr[i].mIsFixed = STL_TRUE;
        sInnerJoinNodeInfoArr[i].mIsAnalyzed = STL_FALSE;
    }

    for( j = 0; j < sNonFixedNodeCount; j++, i++ )
    {
        sInnerJoinNodeInfoArr[i].mNode = sNonFixedNodeArr[j];
        sInnerJoinNodeInfoArr[i].mAnalyzedNode = NULL;
        sInnerJoinNodeInfoArr[i].mIsFixed = STL_FALSE;
        sInnerJoinNodeInfoArr[i].mIsAnalyzed = STL_FALSE;
    }

    /* Join Condition 및 Join Filter 복사 */
    if( QLNC_IS_EXIST_AND_FILTER( sCandJoinTable->mJoinCond ) )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sCandJoinTable->mJoinCond,
                                    &sJoinAndFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sJoinAndFilter = NULL;
    }

    if( QLNC_IS_EXIST_AND_FILTER( sCandJoinTable->mFilter ) )
    {
        if( sJoinAndFilter != NULL )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail(
                         &(aCBOptParamInfo->mParamInfo),
                         sJoinAndFilter,
                         sCandJoinTable->mFilter->mOrCount,
                         sCandJoinTable->mFilter->mOrFilters,
                         aEnv )
                     == STL_SUCCESS );

        }
        else
        {
            STL_TRY( qlncCopyAndFilter(
                         &(aCBOptParamInfo->mParamInfo),
                         sCandJoinTable->mFilter,
                         &sJoinAndFilter,
                         aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Join Cost 계산 수행
     */

    sSeqNum = 1;

    /* Query Timeout 체크 */
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Trace Log 정보 출력 */
    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        /* Join Order */
        STL_TRY( qlncTraceCBOptJoinOrder(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     sSeqNum,
                     sNodeArray->mCurCnt,
                     sInnerJoinNodeInfoArr,
                     aEnv )
                 == STL_SUCCESS );
    }
    sSeqNum++;

    /* Fixed Node 처리 */
    for( i = 0; i < sFixedNodeCount; i++ )
    {
        if( i == 0 )
        {
            sInnerJoinNodeInfoArr[0].mAnalyzedNode = sInnerJoinNodeInfoArr[0].mNode;
            sInnerJoinNodeInfoArr[0].mIsAnalyzed = STL_TRUE;
            continue;
        }

        aCBOptParamInfo->mLeftNode = sInnerJoinNodeInfoArr[i-1].mAnalyzedNode;
        aCBOptParamInfo->mRightNode = sInnerJoinNodeInfoArr[i].mNode;
        aCBOptParamInfo->mJoinCond = sJoinAndFilter;

        STL_TRY( qlncCBOptInnerJoinCost( aCBOptParamInfo,
                                         sInnerJoinNodeInfoArr,
                                         sNodeArray->mCurCnt,
                                         i,
                                         aEnv )
                 == STL_SUCCESS );

        /* Cost를 계산한 Inner Join Node를 Left Node로 설정 */
        sInnerJoinNodeInfoArr[i].mAnalyzedNode = aCBOptParamInfo->mRightNode;
        sInnerJoinNodeInfoArr[i].mIsAnalyzed = STL_TRUE;
    }

    /* Inner Join Node 처리 */
    sBestInnerJoinNode = NULL;
    if( sNonFixedNodeCount < QLNC_THRESHOLD_FOR_SWITCHING_INNER_JOIN_OPTIMIZER )
    {
        STL_TRY( qlncCBOptFindBestInnerJoinTable( aCBOptParamInfo,
                                                  sNodeArray->mCurCnt,
                                                  sInnerJoinNodeInfoArr,
                                                  sFixedNodeCount,
                                                  &sSeqNum,
                                                  sJoinAndFilter,
                                                  &sBestInnerJoinNode,
                                                  aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncCBOptFindBestInnerJoinTable_Sparse( aCBOptParamInfo,
                                                         sNodeArray->mCurCnt,
                                                         sInnerJoinNodeInfoArr,
                                                         sFixedNodeCount,
                                                         &sSeqNum,
                                                         sJoinAndFilter,
                                                         &sBestInnerJoinNode,
                                                         aEnv )
                 == STL_SUCCESS );

        /* Inner Join Optimizer의 Stop Threshold를 넘는 경우 중단 */
        if( sSeqNum > QLNC_THRESHOLD_FOR_STOPPING_INNER_JOIN_OPTIMIZER )
        {
            /* Trace Log 정보 출력 */
            if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
            {
                /* @todo Coverage를 위한 테스트 케이스를 만들기 어려움 */
                STL_TRY( qllTraceBody( &QLL_CANDIDATE_MEM( aEnv ),
                                       aEnv,
                                       "\n**********************\n"
                                       "Stop Join Optimizer..\n"
                                       "Number of join permutations tried: %d\n"
                                       "**********************\n",
                                       sSeqNum - 1 )
                         == STL_SUCCESS );
            }

            STL_THROW( RAMP_STOP_OPTIMIZER );
        }
    }

    STL_RAMP( RAMP_STOP_OPTIMIZER );


    /**
     * SubQuery Filter가 존재하는 경우 하위의 적정 노드로 Push Down
     */

    STL_TRY( qlncPushDownSubQueryFilter(
                 &(aCBOptParamInfo->mParamInfo),
                 sBestInnerJoinNode,
                 NULL,
                 aEnv )
             == STL_SUCCESS );

    /* Named Column List 설정 */
    ((qlncInnerJoinTableNode*)sBestInnerJoinNode)->mNamedColumnList =
        sCandJoinTable->mNamedColumnList;

    /* Cost 분석여부 설정 */
    STL_DASSERT( (sBestInnerJoinNode != NULL) &&
                 (sBestInnerJoinNode->mBestOptType != QLNC_BEST_OPT_TYPE_NA) &&
                 (sBestInnerJoinNode->mBestCost != NULL) );
    sCandJoinTable->mBestInnerJoinTable = (qlncInnerJoinTableNode*)sBestInnerJoinNode;
    sCandJoinTable->mBestOuterJoinTable = NULL;
    sCandJoinTable->mBestSemiJoinTable = NULL;
    sCandJoinTable->mBestAntiSemiJoinTable = NULL;
    sCandJoinTable->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_INNER_JOIN;
    sCandJoinTable->mNodeCommon.mBestCost = sBestInnerJoinNode->mBestCost;
    sCandJoinTable->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Best Inner Join Tree을 찾는다.
 * @param[in]   aCBOptParamInfo         Cost Based Optimizer Parameter Info
 * @param[in]   aNodeCount              Node Count
 * @param[in]   aInnerJoinNodeInfoArr   Inner Join Candidate Node Info Array
 * @param[in]   aCurNodeIdx             Current Node Idx
 * @param[in]   aSeqNum                 Sequence Number
 * @param[in]   aJoinAndFilter          Join And Filter
 * @param[out]  aBestInnerJoinNode      Best Inner Join Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCBOptFindBestInnerJoinTable( qlncCBOptParamInfo       * aCBOptParamInfo,
                                           stlInt32                   aNodeCount,
                                           qlncInnerJoinNodeInfo    * aInnerJoinNodeInfoArr,
                                           stlInt32                   aCurNodeIdx,
                                           stlInt32                 * aSeqNum,
                                           qlncAndFilter            * aJoinAndFilter,
                                           qlncNodeCommon          ** aBestInnerJoinNode,
                                           qllEnv                   * aEnv )
{
    stlInt32                  i;
    stlInt32                  sUsedNodeIdx;

    qlncInnerJoinNodeInfo     sTmpInnerJoinNodeInfo;
    qlncAndFilter           * sTmpJoinAndFilter         = NULL;

    knlRegionMark             sRegionMemMark;
    qlncNodeCommon          * sPrevBestInnerJoinNode    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* 더이상 노드가 없는 경우로 Best Plan이다. */
    if( aCurNodeIdx == aNodeCount )
    {
        STL_DASSERT( aInnerJoinNodeInfoArr[aCurNodeIdx-1].mAnalyzedNode != NULL );
        STL_DASSERT( ((*aBestInnerJoinNode) == NULL) ||
                     (QLNC_LESS_THAN_DOUBLE(
                          aInnerJoinNodeInfoArr[aCurNodeIdx-1].mAnalyzedNode->mBestCost->mTotalCost,
                          (*aBestInnerJoinNode)->mBestCost->mTotalCost )) );
        (*aBestInnerJoinNode) = aInnerJoinNodeInfoArr[aCurNodeIdx-1].mAnalyzedNode;

        STL_THROW( RAMP_FINISH );
    }

    STL_DASSERT( aInnerJoinNodeInfoArr[aCurNodeIdx].mIsAnalyzed == STL_FALSE );

    /* Region Memory Mark */
    KNL_INIT_REGION_MARK( &sRegionMemMark );
    STL_TRY( knlMarkRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                               &sRegionMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sPrevBestInnerJoinNode = *aBestInnerJoinNode;

    sUsedNodeIdx = aCurNodeIdx;

    /* RETRY */
    STL_RAMP( RAMP_RETRY );

    /* Plan의 Cost를 계산해야 하는 경우 */
    STL_DASSERT( aInnerJoinNodeInfoArr[aCurNodeIdx].mIsAnalyzed == STL_FALSE );
    if( aCurNodeIdx == 0 )
    {
        /* Left Node 분석 수행 */
        aCBOptParamInfo->mLeftNode = aInnerJoinNodeInfoArr[aCurNodeIdx].mNode;
        STL_TRY( qlncCostBasedOptimizerFuncList[ aCBOptParamInfo->mLeftNode->mNodeType ](
                     aCBOptParamInfo,
                     aEnv )
                 == STL_SUCCESS );
        aInnerJoinNodeInfoArr[aCurNodeIdx].mAnalyzedNode = aInnerJoinNodeInfoArr[aCurNodeIdx].mNode;
        aInnerJoinNodeInfoArr[aCurNodeIdx].mIsAnalyzed = STL_TRUE;

        STL_TRY( qlncCBOptFindBestInnerJoinTable( aCBOptParamInfo,
                                                  aNodeCount,
                                                  aInnerJoinNodeInfoArr,
                                                  aCurNodeIdx + 1,
                                                  aSeqNum,
                                                  aJoinAndFilter,
                                                  aBestInnerJoinNode,
                                                  aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Right Node 분석 수행 */
        aCBOptParamInfo->mLeftNode = aInnerJoinNodeInfoArr[aCurNodeIdx].mNode;
        STL_TRY( qlncCostBasedOptimizerFuncList[ aCBOptParamInfo->mLeftNode->mNodeType ](
                     aCBOptParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        /* Join And Filter 복사 */
        if( aJoinAndFilter != NULL )
        {
            STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                        aJoinAndFilter,
                                        &sTmpJoinAndFilter,
                                        aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sTmpJoinAndFilter = NULL;
        }

        /* Inner Join Table Node의 Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = aInnerJoinNodeInfoArr[aCurNodeIdx-1].mAnalyzedNode;
        aCBOptParamInfo->mRightNode = aInnerJoinNodeInfoArr[aCurNodeIdx].mNode;
        aCBOptParamInfo->mJoinCond = sTmpJoinAndFilter;

        STL_TRY( qlncCBOptInnerJoinCost( aCBOptParamInfo,
                                         aInnerJoinNodeInfoArr,
                                         aNodeCount,
                                         aCurNodeIdx,
                                         aEnv )
                 == STL_SUCCESS );

        /* Cost를 계산한 Inner Join Node를 Left Node로 설정 */
        aInnerJoinNodeInfoArr[aCurNodeIdx].mAnalyzedNode = aCBOptParamInfo->mRightNode;
        aInnerJoinNodeInfoArr[aCurNodeIdx].mIsAnalyzed = STL_TRUE;

        if( (*aBestInnerJoinNode) != NULL )
        {
            if( QLNC_LESS_THAN_DOUBLE(
                    aInnerJoinNodeInfoArr[aCurNodeIdx].mAnalyzedNode->mBestCost->mTotalCost,
                    (*aBestInnerJoinNode)->mBestCost->mTotalCost ) )
            {
                /* 계속 진행 */
                STL_TRY( qlncCBOptFindBestInnerJoinTable( aCBOptParamInfo,
                                                          aNodeCount,
                                                          aInnerJoinNodeInfoArr,
                                                          aCurNodeIdx + 1,
                                                          aSeqNum,
                                                          sTmpJoinAndFilter,
                                                          aBestInnerJoinNode,
                                                          aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /* 이미 현재 Plan보다 Best한 Plan이 존재하므로 더이상 진행할 필요 없음 */
                /* Trace Log 정보 출력 */
                if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
                {
                    /* Join Abort */
                    STL_TRY( qllTraceBody( &QLL_CANDIDATE_MEM( aEnv ),
                                           aEnv,
                                           "Join Order Aborted: Best Plan Cost(%.4lf) is Better than Current Cost(%.4lf)\n",
                                           (*aBestInnerJoinNode)->mBestCost->mTotalCost,
                                           aInnerJoinNodeInfoArr[aCurNodeIdx].mAnalyzedNode->mBestCost->mTotalCost )
                             == STL_SUCCESS );
                }
            }
        }
        else
        {
            /* 계속 진행 */
            STL_TRY( qlncCBOptFindBestInnerJoinTable( aCBOptParamInfo,
                                                      aNodeCount,
                                                      aInnerJoinNodeInfoArr,
                                                      aCurNodeIdx + 1,
                                                      aSeqNum,
                                                      sTmpJoinAndFilter,
                                                      aBestInnerJoinNode,
                                                      aEnv )
                     == STL_SUCCESS );
        }
    }

    /* 위치 변경한 노드에 대하여 원래 위치로 복구 */
    if( sUsedNodeIdx > aCurNodeIdx )
    {
        QLNC_COPY_INNER_JOIN_NODE_INFO( &sTmpInnerJoinNodeInfo,
                                        &aInnerJoinNodeInfoArr[aCurNodeIdx] );

        for( i = aCurNodeIdx + 1; i <= sUsedNodeIdx; i++ )
        {
            QLNC_COPY_INNER_JOIN_NODE_INFO( &aInnerJoinNodeInfoArr[i-1],
                                            &aInnerJoinNodeInfoArr[i] );
        }

        QLNC_COPY_INNER_JOIN_NODE_INFO( &aInnerJoinNodeInfoArr[sUsedNodeIdx],
                                        &sTmpInnerJoinNodeInfo );
    }

    /* 메모리 정리 */
    if( sPrevBestInnerJoinNode == *aBestInnerJoinNode )
    {
        /* Region Memory UnMark */
        STL_TRY( knlFreeUnmarkedRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                           &sRegionMemMark,
                                           STL_FALSE, /* aFreeChunk */
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        /* Region Memory Mark 재설정 */
        KNL_INIT_REGION_MARK( &sRegionMemMark );
        STL_TRY( knlMarkRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                   &sRegionMemMark,
                                   KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sPrevBestInnerJoinNode = *aBestInnerJoinNode;
    }

    /* 새로운 노드 찾기 */
    sUsedNodeIdx++;

    if( sUsedNodeIdx < aNodeCount )
    {
        /* 새로운 노드를 찾은 경우 */

        /* 새로운 노드를 현재 위치로 이동 및 노드 정보 Reset */
        QLNC_COPY_INNER_JOIN_NODE_INFO( &sTmpInnerJoinNodeInfo,
                                        &aInnerJoinNodeInfoArr[sUsedNodeIdx] );

        for( i = sUsedNodeIdx; i > aCurNodeIdx; i-- )
        {
            /* 노드 이동 */
            QLNC_COPY_INNER_JOIN_NODE_INFO( &aInnerJoinNodeInfoArr[i],
                                            &aInnerJoinNodeInfoArr[i-1] );

            /* 노드 정보 Reset */
            aInnerJoinNodeInfoArr[i].mAnalyzedNode = NULL;
            aInnerJoinNodeInfoArr[i].mIsAnalyzed = STL_FALSE;
        }

        QLNC_COPY_INNER_JOIN_NODE_INFO( &aInnerJoinNodeInfoArr[aCurNodeIdx],
                                        &sTmpInnerJoinNodeInfo );

        /* 노드 정보 Reset */
        aInnerJoinNodeInfoArr[aCurNodeIdx].mAnalyzedNode = NULL;
        aInnerJoinNodeInfoArr[aCurNodeIdx].mIsAnalyzed = STL_FALSE;

        /* Used Node 이후에 존재하는 노드들에 대한 노드 정보 Reset */
        for( i = sUsedNodeIdx; i < aNodeCount; i++ )
        {
            aInnerJoinNodeInfoArr[i].mAnalyzedNode = NULL;
            aInnerJoinNodeInfoArr[i].mIsAnalyzed = STL_FALSE;
        }

        /* Inner Join Optimizer의 Stop Threshold을 넘지 않아야 한다. */
        STL_DASSERT( *aSeqNum <= QLNC_THRESHOLD_FOR_STOPPING_INNER_JOIN_OPTIMIZER );

        /* Query Timeout 체크 */
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Trace Log 정보 출력 */
        if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
        {
            /* Join Order */
            STL_TRY( qlncTraceCBOptJoinOrder(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         *aSeqNum,
                         aNodeCount,
                         aInnerJoinNodeInfoArr,
                         aEnv )
                     == STL_SUCCESS );
        }
        (*aSeqNum)++;

        STL_THROW( RAMP_RETRY );
    }
    else
    {
        /* 더이상 노드가 없는 경우 */
        STL_THROW( RAMP_FINISH );
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 순서 변경 없이 Best Inner Join Tree을 찾는다.
 * @param[in]   aCBOptParamInfo         Cost Based Optimizer Parameter Info
 * @param[in]   aNodeCount              Node Count
 * @param[in]   aInnerJoinNodeInfoArr   Inner Join Candidate Node Info Array
 * @param[in]   aCurNodeIdx             Current Node Index
 * @param[in]   aJoinAndFilter          Join And Filter
 * @param[out]  aBestInnerJoinNode      Best Inner Join Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCBOptFindBestInnerJoinTable_Internal( qlncCBOptParamInfo      * aCBOptParamInfo,
                                                    stlInt32                  aNodeCount,
                                                    qlncInnerJoinNodeInfo   * aInnerJoinNodeInfoArr,
                                                    stlInt32                  aCurNodeIdx,
                                                    qlncAndFilter           * aJoinAndFilter,
                                                    qlncNodeCommon         ** aBestInnerJoinNode,
                                                    qllEnv                  * aEnv )
{
    qlncAndFilter           * sTmpJoinAndFilter         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* 더이상 노드가 없는 경우로 Best Plan이다. */
    if( aCurNodeIdx == aNodeCount )
    {
        STL_DASSERT( aInnerJoinNodeInfoArr[aCurNodeIdx-1].mAnalyzedNode != NULL );
        STL_DASSERT( ((*aBestInnerJoinNode) == NULL) ||
                     (QLNC_LESS_THAN_DOUBLE(
                          aInnerJoinNodeInfoArr[aCurNodeIdx-1].mAnalyzedNode->mBestCost->mTotalCost,
                          (*aBestInnerJoinNode)->mBestCost->mTotalCost )) );
        (*aBestInnerJoinNode) = aInnerJoinNodeInfoArr[aCurNodeIdx-1].mAnalyzedNode;

        STL_THROW( RAMP_FINISH );
    }

    STL_DASSERT( aInnerJoinNodeInfoArr[aCurNodeIdx].mIsAnalyzed == STL_FALSE );

    /* Plan의 Cost를 계산해야 하는 경우 */
    STL_DASSERT( aInnerJoinNodeInfoArr[aCurNodeIdx].mIsAnalyzed == STL_FALSE );
    if( aCurNodeIdx == 0 )
    {
        /* Left Node 분석 수행 */
        aCBOptParamInfo->mLeftNode = aInnerJoinNodeInfoArr[aCurNodeIdx].mNode;
        STL_TRY( qlncCostBasedOptimizerFuncList[ aCBOptParamInfo->mLeftNode->mNodeType ](
                     aCBOptParamInfo,
                     aEnv )
                 == STL_SUCCESS );
        aInnerJoinNodeInfoArr[aCurNodeIdx].mAnalyzedNode = aInnerJoinNodeInfoArr[aCurNodeIdx].mNode;
        aInnerJoinNodeInfoArr[aCurNodeIdx].mIsAnalyzed = STL_TRUE;
        KNL_INIT_REGION_MARK( &(aInnerJoinNodeInfoArr[aCurNodeIdx].mRegionMemMarkForSparse) );
        aInnerJoinNodeInfoArr[aCurNodeIdx].mRemainAndFilterForSparse = aJoinAndFilter;

        STL_TRY( qlncCBOptFindBestInnerJoinTable_Internal( aCBOptParamInfo,
                                                           aNodeCount,
                                                           aInnerJoinNodeInfoArr,
                                                           aCurNodeIdx + 1,
                                                           aJoinAndFilter,
                                                           aBestInnerJoinNode,
                                                           aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Right Node 분석 수행 */
        aCBOptParamInfo->mLeftNode = aInnerJoinNodeInfoArr[aCurNodeIdx].mNode;
        STL_TRY( qlncCostBasedOptimizerFuncList[ aCBOptParamInfo->mLeftNode->mNodeType ](
                     aCBOptParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        /* Join And Filter 복사 */
        if( aJoinAndFilter != NULL )
        {
            STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                        aJoinAndFilter,
                                        &sTmpJoinAndFilter,
                                        aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sTmpJoinAndFilter = NULL;
        }

        /* 현재 Node에 대한 Region Memory Mark 설정 */
        KNL_INIT_REGION_MARK( &(aInnerJoinNodeInfoArr[aCurNodeIdx].mRegionMemMarkForSparse) );
        STL_TRY( knlMarkRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                   &(aInnerJoinNodeInfoArr[aCurNodeIdx].mRegionMemMarkForSparse),
                                   KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Inner Join Table Node의 Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = aInnerJoinNodeInfoArr[aCurNodeIdx-1].mAnalyzedNode;
        aCBOptParamInfo->mRightNode = aInnerJoinNodeInfoArr[aCurNodeIdx].mNode;
        aCBOptParamInfo->mJoinCond = sTmpJoinAndFilter;

        STL_TRY( qlncCBOptInnerJoinCost( aCBOptParamInfo,
                                         aInnerJoinNodeInfoArr,
                                         aNodeCount,
                                         aCurNodeIdx,
                                         aEnv )
                 == STL_SUCCESS );

        /* 현재 노드까지 적용하고 남은 Join Filter를 설정 */
        aInnerJoinNodeInfoArr[aCurNodeIdx].mRemainAndFilterForSparse = sTmpJoinAndFilter;

        /* Cost를 계산한 Inner Join Node를 Left Node로 설정 */
        aInnerJoinNodeInfoArr[aCurNodeIdx].mAnalyzedNode = aCBOptParamInfo->mRightNode;
        aInnerJoinNodeInfoArr[aCurNodeIdx].mIsAnalyzed = STL_TRUE;

        if( (*aBestInnerJoinNode) != NULL )
        {
            if( QLNC_LESS_THAN_DOUBLE(
                    aInnerJoinNodeInfoArr[aCurNodeIdx].mAnalyzedNode->mBestCost->mTotalCost,
                    (*aBestInnerJoinNode)->mBestCost->mTotalCost ) )
            {
                /* 계속 진행 */
                STL_TRY( qlncCBOptFindBestInnerJoinTable_Internal( aCBOptParamInfo,
                                                                   aNodeCount,
                                                                   aInnerJoinNodeInfoArr,
                                                                   aCurNodeIdx + 1,
                                                                   sTmpJoinAndFilter,
                                                                   aBestInnerJoinNode,
                                                                   aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /* 이미 현재 Plan보다 Best한 Plan이 존재하므로 더이상 진행할 필요 없음 */
                /* Trace Log 정보 출력 */
                if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
                {
                    /* Join Abort */
                    STL_TRY( qllTraceBody( &QLL_CANDIDATE_MEM( aEnv ),
                                           aEnv,
                                           "Join Order Aborted: Best Plan Cost(%.4lf) is Better than Current Cost(%.4lf)\n",
                                           (*aBestInnerJoinNode)->mBestCost->mTotalCost,
                                           aInnerJoinNodeInfoArr[aCurNodeIdx].mAnalyzedNode->mBestCost->mTotalCost )
                             == STL_SUCCESS );
                }
            }
        }
        else
        {
            /* 계속 진행 */
            STL_TRY( qlncCBOptFindBestInnerJoinTable_Internal( aCBOptParamInfo,
                                                               aNodeCount,
                                                               aInnerJoinNodeInfoArr,
                                                               aCurNodeIdx + 1,
                                                               sTmpJoinAndFilter,
                                                               aBestInnerJoinNode,
                                                               aEnv )
                     == STL_SUCCESS );
        }
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Threshold를 넘는 테이블을 갖는 Inner Join에 대하여 Best Inner Join Tree을 찾는다.
 * @param[in]   aCBOptParamInfo         Cost Based Optimizer Parameter Info
 * @param[in]   aNodeCount              Node Count
 * @param[in]   aInnerJoinNodeInfoArr   Inner Join Candidate Node Info Array
 * @param[in]   aCurNodeIdx             Current Node Index
 * @param[in]   aSeqNum                 Sequence Number
 * @param[in]   aJoinAndFilter          Join And Filter
 * @param[out]  aBestInnerJoinNode      Best Inner Join Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCBOptFindBestInnerJoinTable_Sparse( qlncCBOptParamInfo        * aCBOptParamInfo,
                                                  stlInt32                    aNodeCount,
                                                  qlncInnerJoinNodeInfo     * aInnerJoinNodeInfoArr,
                                                  stlInt32                    aCurNodeIdx,
                                                  stlInt32                  * aSeqNum,
                                                  qlncAndFilter             * aJoinAndFilter,
                                                  qlncNodeCommon           ** aBestInnerJoinNode,
                                                  qllEnv                    * aEnv )
{
    stlBool                   sIsOpt;
    stlInt32                  i;
    stlInt32                  j;

    qlncInnerJoinNodeInfo     sTmpInnerJoinNodeInfo;
    qlncNodeCommon          * sPrevBestInnerJoinNode    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNodeCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInnerJoinNodeInfoArr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCurNodeIdx >= 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqNum != NULL, QLL_ERROR_STACK(aEnv) );


    if( aCurNodeIdx == 0 )
    {
        /* 다음 노드에 대한 Cost 계산 수행 */
        STL_TRY( qlncCBOptFindBestInnerJoinTable_Sparse( aCBOptParamInfo,
                                                         aNodeCount,
                                                         aInnerJoinNodeInfoArr,
                                                         aCurNodeIdx + 1,
                                                         aSeqNum,
                                                         aJoinAndFilter,
                                                         aBestInnerJoinNode,
                                                         aEnv )
                 == STL_SUCCESS );

        STL_THROW( RAMP_FINISH );
    }

    /* 모든 노드의 순서가 위치되었으면 Cost Optimizer 수행 */
    if( aCurNodeIdx == aNodeCount - 1 )
    {
        /* Inner Join Optimizer의 Stop Threshold를 넘는 경우 중단 */
        if( (*aSeqNum) > QLNC_THRESHOLD_FOR_STOPPING_INNER_JOIN_OPTIMIZER )
        {
            /* @todo coverage를 위한 테스트 케이스를 만들기 어려움 */
            STL_THROW( RAMP_FINISH );
        }

        /* Query Timeout 체크 */
        STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Trace Log 정보 출력 */
        if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
        {
            /* Join Order */
            STL_TRY( qlncTraceCBOptJoinOrder(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         *aSeqNum,
                         aNodeCount,
                         aInnerJoinNodeInfoArr,
                         aEnv )
                     == STL_SUCCESS );
        }
        (*aSeqNum)++;

        /* Cost 계산 시작 위치 찾기 */
        for( i = 0; i < aNodeCount; i++ )
        {
            if( aInnerJoinNodeInfoArr[i].mIsAnalyzed == STL_FALSE )
            {
                break;
            }
        }

        STL_TRY( qlncCBOptFindBestInnerJoinTable_Internal( aCBOptParamInfo,
                                                           aNodeCount,
                                                           aInnerJoinNodeInfoArr,
                                                           i,
                                                           (i == 0
                                                            ? aJoinAndFilter
                                                            : aInnerJoinNodeInfoArr[i-1].mRemainAndFilterForSparse),
                                                           aBestInnerJoinNode,
                                                           aEnv )
                 == STL_SUCCESS );

        STL_THROW( RAMP_FINISH );
    }

    /* 현재 Node Idx부터 Join Condition이 존재하는 노드를 찾아 Cost 계산을 수행한다. */
    sIsOpt = STL_FALSE;
    for( i = aCurNodeIdx; i < aNodeCount; i++ )
    {
        /* Inner Join Optimizer의 Stop Threshold를 넘는 경우 중단 */
        if( (*aSeqNum) > QLNC_THRESHOLD_FOR_STOPPING_INNER_JOIN_OPTIMIZER )
        {
            STL_THROW( RAMP_FINISH );
        }

        if( i > aCurNodeIdx )
        {
            /* i번째 Node를 Cur Node Idx로 이동 */
            QLNC_COPY_INNER_JOIN_NODE_INFO( &sTmpInnerJoinNodeInfo,
                                            &aInnerJoinNodeInfoArr[i] );

            for( j = i; j > aCurNodeIdx; j-- )
            {
                QLNC_COPY_INNER_JOIN_NODE_INFO( &aInnerJoinNodeInfoArr[j],
                                                &aInnerJoinNodeInfoArr[j-1] );
            }

            QLNC_COPY_INNER_JOIN_NODE_INFO( &aInnerJoinNodeInfoArr[aCurNodeIdx],
                                            &sTmpInnerJoinNodeInfo );

            /* 노드 정보 Reset */
            for( j = aCurNodeIdx; j < aNodeCount; j++ )
            {
                aInnerJoinNodeInfoArr[j].mAnalyzedNode = NULL;
                aInnerJoinNodeInfoArr[j].mIsAnalyzed = STL_FALSE;
                KNL_INIT_REGION_MARK( &(aInnerJoinNodeInfoArr[j].mRegionMemMarkForSparse) );
                aInnerJoinNodeInfoArr[aCurNodeIdx].mRemainAndFilterForSparse = NULL;
            }
        }

        if( qlncIsExistJoinCondOnInnerJoinNode( aInnerJoinNodeInfoArr,
                                                aCurNodeIdx,
                                                aJoinAndFilter )
            == STL_TRUE )
        {
            sPrevBestInnerJoinNode = (*aBestInnerJoinNode);

            /* 다음 노드에 대한 Cost 계산 수행 */
            STL_TRY( qlncCBOptFindBestInnerJoinTable_Sparse( aCBOptParamInfo,
                                                             aNodeCount,
                                                             aInnerJoinNodeInfoArr,
                                                             aCurNodeIdx + 1,
                                                             aSeqNum,
                                                             aJoinAndFilter,
                                                             aBestInnerJoinNode,
                                                             aEnv )
                     == STL_SUCCESS );

            /* 메모리 정리 */
            if( (sPrevBestInnerJoinNode == (*aBestInnerJoinNode)) &&
                (KNL_IS_VALID_REGION_MARK( &(aInnerJoinNodeInfoArr[aCurNodeIdx].mRegionMemMarkForSparse) )) )
            {
                /* Region Memory UnMark */
                STL_TRY( knlFreeUnmarkedRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                   &(aInnerJoinNodeInfoArr[aCurNodeIdx].mRegionMemMarkForSparse),
                                                   STL_FALSE, /* aFreeChunk */
                                                   KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }

            sIsOpt = STL_TRUE;
        }

        if( i > aCurNodeIdx )
        {
            /* Cur Node Idx의 Node를 i번째로 이동 */
            QLNC_COPY_INNER_JOIN_NODE_INFO( &sTmpInnerJoinNodeInfo,
                                            &aInnerJoinNodeInfoArr[aCurNodeIdx] );

            for( j = aCurNodeIdx; j < i; j++ )
            {
                QLNC_COPY_INNER_JOIN_NODE_INFO( &aInnerJoinNodeInfoArr[j],
                                                &aInnerJoinNodeInfoArr[j+1] );
            }

            QLNC_COPY_INNER_JOIN_NODE_INFO( &aInnerJoinNodeInfoArr[i],
                                            &sTmpInnerJoinNodeInfo );
        }
    }

    if( sIsOpt == STL_FALSE )
    {
        /* 노드 정보 Reset */
        for( j = aCurNodeIdx; j < aNodeCount; j++ )
        {
            aInnerJoinNodeInfoArr[j].mAnalyzedNode = NULL;
            aInnerJoinNodeInfoArr[j].mIsAnalyzed = STL_FALSE;
        }

        /* 다음 노드에 대한 Cost 계산 수행 */
        STL_TRY( qlncCBOptFindBestInnerJoinTable_Sparse( aCBOptParamInfo,
                                                         aNodeCount,
                                                         aInnerJoinNodeInfoArr,
                                                         aCurNodeIdx + 1,
                                                         aSeqNum,
                                                         aJoinAndFilter,
                                                         aBestInnerJoinNode,
                                                         aEnv )
                 == STL_SUCCESS );
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Outer Join Table의 Cost Based Optimizer를 수행하여 Best Plan을 찾는다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptBestOuterJoinTable( qlncCBOptParamInfo   * aCBOptParamInfo,
                                       qllEnv               * aEnv )
{
    qlncJoinTableNode       * sCandJoinTable        = NULL;

    qlncAndFilter           * sJoinCondition        = NULL;
    qlncAndFilter           * sJoinFilter           = NULL;

    qlncNodeCommon          * sLeftNode             = NULL;
    qlncNodeCommon          * sRightNode            = NULL;

    qlncNodeCommon          * sBestOuterJoinNode    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ((qlncJoinTableNode*)(aCBOptParamInfo->mLeftNode))->mJoinType == QLNC_JOIN_TYPE_OUTER,
                        QLL_ERROR_STACK(aEnv) );


    /* 분석 여부 체크 */
    if( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE )
    {
        STL_THROW( RAMP_FINISH );
    }

    /* Join Table Node 설정 */
    sCandJoinTable = (qlncJoinTableNode*)(aCBOptParamInfo->mLeftNode);

    /* Left Node와 Right Node 찾기 */
    STL_DASSERT( sCandJoinTable->mNodeArray.mCurCnt == 2 );
    sLeftNode = sCandJoinTable->mNodeArray.mNodeArr[0];
    sRightNode = sCandJoinTable->mNodeArray.mNodeArr[1];

    /* Node 분석 수행 */
    aCBOptParamInfo->mLeftNode = sLeftNode;
    STL_TRY( qlncCostBasedOptimizerFuncList[ sLeftNode->mNodeType ]( aCBOptParamInfo,
                                                                     aEnv )
             == STL_SUCCESS );

    aCBOptParamInfo->mLeftNode = sRightNode;
    STL_TRY( qlncCostBasedOptimizerFuncList[ sRightNode->mNodeType ]( aCBOptParamInfo,
                                                                      aEnv )
             == STL_SUCCESS );

    /* Join Condition 및 Join Filter 복사 */
    if( QLNC_IS_EXIST_AND_FILTER( sCandJoinTable->mJoinCond ) )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sCandJoinTable->mJoinCond,
                                    &sJoinCondition,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sJoinCondition = NULL;
    }

    if( QLNC_IS_EXIST_AND_FILTER( sCandJoinTable->mFilter ) )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sCandJoinTable->mFilter,
                                    &sJoinFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sJoinFilter = NULL;
    }


    /**
     * Join Cost 계산 수행
     */

    /* Inner Join Table Node의 Join Cost 계산 */
    aCBOptParamInfo->mLeftNode = sLeftNode;
    aCBOptParamInfo->mRightNode = sRightNode;
    aCBOptParamInfo->mJoinCond = sJoinCondition;
    aCBOptParamInfo->mFilter = sJoinFilter;

    STL_TRY( qlncCBOptOuterJoinCost( aCBOptParamInfo,
                                     sCandJoinTable->mJoinDirect,
                                     aEnv )
             == STL_SUCCESS );

    sBestOuterJoinNode = aCBOptParamInfo->mRightNode;

    /* Cost 분석여부 설정 */
    sCandJoinTable->mBestInnerJoinTable = NULL;
    sCandJoinTable->mBestOuterJoinTable = (qlncOuterJoinTableNode*)sBestOuterJoinNode;
    sCandJoinTable->mBestSemiJoinTable = NULL;
    sCandJoinTable->mBestAntiSemiJoinTable = NULL;
    sCandJoinTable->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_OUTER_JOIN;
    sCandJoinTable->mNodeCommon.mBestCost = sBestOuterJoinNode->mBestCost;
    sCandJoinTable->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Semi Join Table의 Cost Based Optimizer를 수행하여 Best Plan을 찾는다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aWeightValue    Weight Value
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptBestSemiJoinTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      stlFloat64              aWeightValue,
                                      qllEnv                * aEnv )
{
    stlInt32                  i;
    qlncJoinTableNode       * sCandJoinTable        = NULL;

    qlncAndFilter           * sJoinCondition        = NULL;
    qlncAndFilter           * sJoinFilter           = NULL;

    qlncNodeCommon          * sLeftNode             = NULL;
    qlncNodeCommon          * sRightNode            = NULL;

    qlncNodeCommon          * sBestSemiJoinNode     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ((qlncJoinTableNode*)(aCBOptParamInfo->mLeftNode))->mJoinType == QLNC_JOIN_TYPE_SEMI,
                        QLL_ERROR_STACK(aEnv) );


    /* 분석 여부 체크 */
    if( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE )
    {
        STL_THROW( RAMP_FINISH );
    }

    /* Join Table Node 설정 */
    sCandJoinTable = (qlncJoinTableNode*)(aCBOptParamInfo->mLeftNode);

    /* Left Node와 Right Node 찾기 */
    STL_DASSERT( sCandJoinTable->mNodeArray.mCurCnt == 2 );
    sLeftNode = sCandJoinTable->mNodeArray.mNodeArr[0];
    sRightNode = sCandJoinTable->mNodeArray.mNodeArr[1];

    /* Node 분석 수행 */
    aCBOptParamInfo->mLeftNode = sLeftNode;
    STL_TRY( qlncCostBasedOptimizerFuncList[ sLeftNode->mNodeType ]( aCBOptParamInfo,
                                                                     aEnv )
             == STL_SUCCESS );

    aCBOptParamInfo->mLeftNode = sRightNode;
    STL_TRY( qlncCostBasedOptimizerFuncList[ sRightNode->mNodeType ]( aCBOptParamInfo,
                                                                      aEnv )
             == STL_SUCCESS );

    /* Join Condition 및 Join Filter 복사 */
    if( QLNC_IS_EXIST_AND_FILTER( sCandJoinTable->mJoinCond ) )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sCandJoinTable->mJoinCond,
                                    &sJoinCondition,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sJoinCondition = NULL;
    }

    if( QLNC_IS_EXIST_AND_FILTER( sCandJoinTable->mFilter ) )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sCandJoinTable->mFilter,
                                    &sJoinFilter,
                                    aEnv )
                 == STL_SUCCESS );

        /* Join Filter에서 SubQuery Filer를 제외한 filter를 Join Condition으로 옮긴다. */
        for( i = 0; i < sJoinFilter->mOrCount; i++ )
        {
            if( sJoinFilter->mOrFilters[i]->mSubQueryExprList != NULL )
            {
                continue;
            }

            if( sJoinCondition == NULL )
            {
                /* @todo 현재 여기에 들어올 수 없는 것으로 판단됨 */

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncAndFilter ),
                                            (void**) &sJoinCondition,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                QLNC_INIT_AND_FILTER( sJoinCondition );
            }

            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                        sJoinCondition,
                                                        1,
                                                        &(sJoinFilter->mOrFilters[i]),
                                                        aEnv )
                     == STL_SUCCESS );

            sJoinFilter->mOrFilters[i] = NULL;
        }

        /* Join Filter 재조정 */
        STL_TRY( qlncCompactAndFilter( sJoinFilter,
                                       aEnv )
                 == STL_SUCCESS );

        if( sJoinFilter->mOrCount == 0 )
        {
            sJoinFilter = NULL;
        }
    }
    else
    {
        sJoinFilter = NULL;
    }


    /**
     * Join Cost 계산 수행
     */

    /* Semi-Join Table Node의 Join Cost 계산 */
    aCBOptParamInfo->mLeftNode = sLeftNode;
    aCBOptParamInfo->mRightNode = sRightNode;
    aCBOptParamInfo->mJoinCond = sJoinCondition;
    aCBOptParamInfo->mFilter = sJoinFilter;

    STL_TRY( qlncCBOptSemiJoinCost( aCBOptParamInfo,
                                    sCandJoinTable->mJoinDirect,
                                    aWeightValue,
                                    aEnv )
             == STL_SUCCESS );

    sBestSemiJoinNode = aCBOptParamInfo->mRightNode;

    /* Cost 분석여부 설정 */
    sCandJoinTable->mBestInnerJoinTable = NULL;
    sCandJoinTable->mBestOuterJoinTable = NULL;
    sCandJoinTable->mBestSemiJoinTable = (qlncSemiJoinTableNode*)sBestSemiJoinNode;
    sCandJoinTable->mBestAntiSemiJoinTable = NULL;
    sCandJoinTable->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_SEMI_JOIN;
    sCandJoinTable->mNodeCommon.mBestCost = sBestSemiJoinNode->mBestCost;
    sCandJoinTable->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Anti Semi Join Table의 Cost Based Optimizer를 수행하여 Best Plan을 찾는다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aWeightValue    Weight Value
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptBestAntiSemiJoinTable( qlncCBOptParamInfo    * aCBOptParamInfo,
                                          stlFloat64              aWeightValue,
                                          qllEnv                * aEnv )
{
//    stlInt32                  i;
    qlncJoinTableNode       * sCandJoinTable        = NULL;

    qlncAndFilter           * sJoinCondition        = NULL;
    qlncAndFilter           * sJoinFilter           = NULL;

    qlncNodeCommon          * sLeftNode             = NULL;
    qlncNodeCommon          * sRightNode            = NULL;

    qlncNodeCommon          * sBestAntiSemiJoinNode = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (((qlncJoinTableNode*)(aCBOptParamInfo->mLeftNode))->mJoinType == QLNC_JOIN_TYPE_ANTI_SEMI) ||
                        (((qlncJoinTableNode*)(aCBOptParamInfo->mLeftNode))->mJoinType == QLNC_JOIN_TYPE_ANTI_SEMI_NA),
                        QLL_ERROR_STACK(aEnv) );


    /* 분석 여부 체크 */
    if( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE )
    {
        STL_THROW( RAMP_FINISH );
    }

    /* Join Table Node 설정 */
    sCandJoinTable = (qlncJoinTableNode*)(aCBOptParamInfo->mLeftNode);

    /* Left Node와 Right Node 찾기 */
    STL_DASSERT( sCandJoinTable->mNodeArray.mCurCnt == 2 );
    sLeftNode = sCandJoinTable->mNodeArray.mNodeArr[0];
    sRightNode = sCandJoinTable->mNodeArray.mNodeArr[1];

    /* Node 분석 수행 */
    aCBOptParamInfo->mLeftNode = sLeftNode;
    STL_TRY( qlncCostBasedOptimizerFuncList[ sLeftNode->mNodeType ]( aCBOptParamInfo,
                                                                     aEnv )
             == STL_SUCCESS );

    aCBOptParamInfo->mLeftNode = sRightNode;
    STL_TRY( qlncCostBasedOptimizerFuncList[ sRightNode->mNodeType ]( aCBOptParamInfo,
                                                                      aEnv )
             == STL_SUCCESS );

    /* Join Condition 및 Join Filter 복사 */
    if( QLNC_IS_EXIST_AND_FILTER( sCandJoinTable->mJoinCond ) )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sCandJoinTable->mJoinCond,
                                    &sJoinCondition,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sJoinCondition = NULL;
    }

    if( QLNC_IS_EXIST_AND_FILTER( sCandJoinTable->mFilter ) )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sCandJoinTable->mFilter,
                                    &sJoinFilter,
                                    aEnv )
                 == STL_SUCCESS );

    }
    else
    {
        sJoinFilter = NULL;
    }


    /**
     * Join Cost 계산 수행
     */

    /* Anti-Semi-Join Table Node의 Join Cost 계산 */
    aCBOptParamInfo->mLeftNode = sLeftNode;
    aCBOptParamInfo->mRightNode = sRightNode;
    aCBOptParamInfo->mJoinCond = sJoinCondition;
    aCBOptParamInfo->mFilter = sJoinFilter;

    STL_TRY( qlncCBOptAntiSemiJoinCost( aCBOptParamInfo,
                                        sCandJoinTable->mJoinDirect,
                                        aWeightValue,
                                        aEnv )
             == STL_SUCCESS );

    sBestAntiSemiJoinNode = aCBOptParamInfo->mRightNode;

    /* Anti-Semi Join Table에 Null Aware 여부 설정 */
    if( sCandJoinTable->mJoinType == QLNC_JOIN_TYPE_ANTI_SEMI )
    {
        ((qlncAntiSemiJoinTableNode*)sBestAntiSemiJoinNode)->mIsNullAware = STL_FALSE;
    }
    else
    {
        STL_DASSERT( sCandJoinTable->mJoinType == QLNC_JOIN_TYPE_ANTI_SEMI_NA );
        ((qlncAntiSemiJoinTableNode*)sBestAntiSemiJoinNode)->mIsNullAware = STL_TRUE;
    }

    /* Cost 분석여부 설정 */
    sCandJoinTable->mBestInnerJoinTable = NULL;
    sCandJoinTable->mBestOuterJoinTable = NULL;
    sCandJoinTable->mBestSemiJoinTable = NULL;
    sCandJoinTable->mBestAntiSemiJoinTable = (qlncAntiSemiJoinTableNode*)sBestAntiSemiJoinNode;
    sCandJoinTable->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN;
    sCandJoinTable->mNodeCommon.mBestCost = sBestAntiSemiJoinNode->mBestCost;
    sCandJoinTable->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Table Scan Cost를 계산한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptTableScanCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                  qllEnv                * aEnv )
{
    qlncTableScanCost   * sTableScanCost    = NULL;

    stlFloat64            sRowCount;
    stlFloat64            sFilterCost;
    stlFloat64            sPhysicalRowCount;
    stlFloat64            sTmpFilterCost;
    
    qlncBaseTableNode   * sBaseTableNode;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /* Table Scan Cost 설정 */
    sTableScanCost = (qlncTableScanCost*)(aCBOptParamInfo->mCost);

    /* And Filter 복사 */
    if( aCBOptParamInfo->mFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    aCBOptParamInfo->mFilter,
                                    &(sTableScanCost->mTableAndFilter),
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sTableScanCost->mTableAndFilter = NULL;
    }

    /* Table And Filter 재정렬 */
    if( sTableScanCost->mTableAndFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sTableScanCost->mTableAndFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Table Scan Cost 계산*/
    sBaseTableNode = (qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode);
    sRowCount = sBaseTableNode->mTableStat->mRowCount;

    /* Access Table Page Cost 계산 */
    sFilterCost = ( sRowCount * QLNC_READ_COST_PER_ROW );

    /* Filter 비용 계산 */
    sPhysicalRowCount = sRowCount;
    if( sTableScanCost->mTableAndFilter != NULL )
    {
        sFilterCost += QLNC_FILTER_PREPARE_COST;

        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    aCBOptParamInfo->mLeftNode,
                                                    sTableScanCost->mTableAndFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost += sTmpFilterCost;
    }

    sTableScanCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sTableScanCost->mCostCommon.mNextFilterCost = sFilterCost + QLNC_OUTPUT_COST_PER_ROW;
    sTableScanCost->mCostCommon.mBuildCost = (stlFloat64)0.0;
    sTableScanCost->mCostCommon.mReadIOCost = (stlFloat64)0.0;
    /**
     * @todo 정확한 Output Column List를 알 수 있는 경우 Output IO Cost를 계산하도록 한다.
     */
    /* sTableScanCost->mCostCommon.mOutputIOCost = */
    /*     (sPhysicalRowCount * sResultRowLen * QLNC_DATA_COPY_COST_PER_BYTE) * */
    /*     QLNC_GET_ADJUST_VALUE_BY_PAGE_COUNT( sBaseTableNode->mTableStat->mPageCount ); */
    sTableScanCost->mCostCommon.mOutputIOCost = (stlFloat64)0.0;

    QLNC_SET_TOTAL_COST( &(sTableScanCost->mCostCommon) );
    sTableScanCost->mCostCommon.mResultRowCount = sRowCount;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index Scan Cost를 계산한다.
 * @param[in]   aCBOptParamInfo     Cost Based Optimizer Parameter Info
 * @param[in]   aIndexScanInfo      Index Scan Info
 * @param[in]   aIsScanAsc          Scan을 ASC로 할것인지 여부
 * @param[in]   aInKeyRangeExpr     IN Key Range Expression
 * @param[in]   aEnv                Environment
 */
stlStatus qlncCBOptIndexScanCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                  qlncIndexScanInfo     * aIndexScanInfo,
                                  stlBool                 aIsScanAsc,
                                  qlncExprCommon        * aInKeyRangeExpr,
                                  qllEnv                * aEnv )
{
    qlncIndexScanCost   * sIndexScanCost        = NULL;

    qlncIndexStat       * sIndexStat            = NULL;

    qlncAndFilter         sIndexRangeAndFilter;
    qlncAndFilter         sIndexKeyAndFilter;
    qlncAndFilter         sTableAndFilter;

    stlInt32              i, j;
    stlInt32              sEqualIndexKeyCount;
    stlBool             * sTmpHasLikeFilterArr  = NULL;
    qlncAndFilter       * sTmpRangeAndFilterArr = NULL;
    qlncAndFilter       * sAndFilter            = NULL;

    qlncRefNodeItem     * sRefNodeItem          = NULL;
    qlncRefColumnItem   * sRefColumnItem        = NULL;

    stlFloat64            sRowCount;
    stlFloat64            sIndexRowLen;
    stlFloat64            sTableRowLen;
    stlFloat64            sIndexColumnPosCalCount;
    stlFloat64            sTableColumnPosCalCount;

    stlFloat64            sIndexRangeFilterCost;
    stlFloat64            sIndexKeyFilterCost;
    stlFloat64            sTableFilterCost;
    stlFloat64            sTmpFilterCost;
    /* stlFloat64            sOutputIOCost; */
    stlFloat64            sPhysicalRowCount;

    const dtlCompareType  * sCompareType        = NULL;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /* Index Scan Cost 설정 */
    sIndexScanCost = (qlncIndexScanCost*)(aCBOptParamInfo->mCost);

    /* Index Statistic 정보 얻기 */
    sIndexStat = &(((qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode))->mIndexStatArr[ aIndexScanInfo->mOffset ]);

    /* Index Scan Info 설정 */
    sIndexScanCost->mIndexHandle = sIndexStat->mIndexHandle;
    sIndexScanCost->mIndexPhyHandle = sIndexStat->mIndexPhyHandle;
    sIndexScanCost->mIsAsc = aIsScanAsc;
    sIndexScanCost->mListColMap = NULL;

    /* Index Range Filter와 Index Key Filter, Table Filter 분리 */
    QLNC_INIT_AND_FILTER( &sIndexRangeAndFilter );
    QLNC_INIT_AND_FILTER( &sIndexKeyAndFilter );
    QLNC_INIT_AND_FILTER( &sTableAndFilter );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncAndFilter ) * sIndexStat->mIndexKeyCount,
                                (void**)&sTmpHasLikeFilterArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncAndFilter ) * sIndexStat->mIndexKeyCount,
                                (void**)&sTmpRangeAndFilterArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < sIndexStat->mIndexKeyCount; i++ )
    {
        sTmpHasLikeFilterArr[i] = STL_FALSE;
        QLNC_INIT_AND_FILTER( &sTmpRangeAndFilterArr[i] );
    }

    /* Filter 분석 */
    sAndFilter = aCBOptParamInfo->mFilter;
    if( sAndFilter == NULL )
    {
        sEqualIndexKeyCount = 0;
        STL_THROW( RAMP_SKIP_FILTER_ANALYZER );
    }

    for( i = 0; i < sAndFilter->mOrCount; i++ )
    {
        /* SubQuery가 존재하는 경우 처리 */
        if( (sAndFilter->mOrFilters[i]->mSubQueryExprList != NULL) &&
            (sAndFilter->mOrFilters[i]->mSubQueryExprList->mCount > 0) )
        {
            /* Index에만 존재하는 Column으로 구성되어 있는지 판단 */
            if( qlncIsOnlyIndexColumnFilter( aCBOptParamInfo->mLeftNode,
                                             sAndFilter->mOrFilters[i],
                                             sIndexStat->mIndexKeyDesc,
                                             sIndexStat->mIndexKeyCount ) )
            {
                qlncRefExprItem * sRefExprItem  = NULL;
                /* Index Column으로만 처리 가능한 Filter인 경우 */

                /* Statement단위 처리의 SubQuery인지 판별 */
                sRefExprItem = sAndFilter->mOrFilters[i]->mSubQueryExprList->mHead;
                while( sRefExprItem != NULL )
                {
                    if( (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL) ||
                        (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mIsRelSubQuery == STL_TRUE) )
                    {
                        break;
                    }

                    sRefExprItem = sRefExprItem->mNext;
                }

                if( sRefExprItem != NULL )
                {
                    /* Statement단위 처리 불가능 SubQuery 존재 */
                    STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                &sTableAndFilter,
                                                                1,
                                                                &(sAndFilter->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );

                    continue;
                }
                else
                {
                    /* Statement단위 처리 가능 SubQuery */
                    if( sAndFilter->mOrFilters[i]->mFilterCount > 1 )
                    {
                        STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                    &sIndexKeyAndFilter,
                                                                    1,
                                                                    &(sAndFilter->mOrFilters[i]),
                                                                    aEnv )
                                 == STL_SUCCESS );

                        continue;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
            }
            else
            {
                /* Index Column으로만 처리 불가능한 Filter인 경우 */
                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                            &sTableAndFilter,
                                                            1,
                                                            &(sAndFilter->mOrFilters[i]),
                                                            aEnv )
                         == STL_SUCCESS );

                continue;
            }
        }

        /* Or가 존재하면 Index Range Filter 불가 */
        if( sAndFilter->mOrFilters[i]->mFilterCount > 1 )
        {
            /* Index에만 존재하는 Column으로 구성되어 있는지 판단 */
            if( qlncIsOnlyIndexColumnFilter( aCBOptParamInfo->mLeftNode,
                                             sAndFilter->mOrFilters[i],
                                             sIndexStat->mIndexKeyDesc,
                                             sIndexStat->mIndexKeyCount ) )
            {
                /* Index Column으로만 처리 가능한 Filter인 경우 */
                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                            &sIndexKeyAndFilter,
                                                            1,
                                                            &(sAndFilter->mOrFilters[i]),
                                                            aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /* Index Column으로만 처리 불가능한 Filter인 경우 */
                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                            &sTableAndFilter,
                                                            1,
                                                            &(sAndFilter->mOrFilters[i]),
                                                            aEnv )
                         == STL_SUCCESS );
            }
        }
        else
        {
            if( sAndFilter->mOrFilters[i]->mFilters[0]->mType ==
                QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER )
            {
                /* 현재 Node에 관련된 Column이 하나도 없는 Filter인 경우 */
                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                            &sIndexKeyAndFilter,
                                                            1,
                                                            &(sAndFilter->mOrFilters[i]),
                                                            aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                qlncBooleanFilter   * sBooleanFilter    = NULL;

                STL_DASSERT( sAndFilter->mOrFilters[i]->mFilters[0]->mType ==
                             QLNC_EXPR_TYPE_BOOLEAN_FILTER );

                sBooleanFilter = (qlncBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[0]);

                /* Reference Node List에서 현재 노드에 대한 Reference Node Item을 찾음 */
                sRefNodeItem = sBooleanFilter->mRefNodeList.mHead;
                while( sRefNodeItem != NULL )
                {
                    if( sRefNodeItem->mNode->mNodeID == aCBOptParamInfo->mLeftNode->mNodeID )
                    {
                        break;
                    }

                    sRefNodeItem = sRefNodeItem->mNext;
                }

                if( sRefNodeItem == NULL )
                {
                    /* 현재 Node에 해당하는 Column이 하나도 없는 경우 */
                    STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                &sTableAndFilter,
                                                                1,
                                                                &(sAndFilter->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    if( sRefNodeItem->mRefColumnList.mCount > 1 )
                    {
                        /* Column이 2개 이상 나오는 경우 Index Range Filter 불가 */
                        if( qlncIsOnlyIndexColumnFilter( aCBOptParamInfo->mLeftNode,
                                                         sAndFilter->mOrFilters[i],
                                                         sIndexStat->mIndexKeyDesc,
                                                         sIndexStat->mIndexKeyCount ) )
                        {
                            /* Index Column으로만 처리 가능한 Filter인 경우 */
                            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                        &sIndexKeyAndFilter,
                                                                        1,
                                                                        &(sAndFilter->mOrFilters[i]),
                                                                        aEnv )
                                     == STL_SUCCESS );
                        }
                        else
                        {
                            /* Index Column으로만 처리 불가능한 Filter인 경우 */
                            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                        &sTableAndFilter,
                                                                        1,
                                                                        &(sAndFilter->mOrFilters[i]),
                                                                        aEnv )
                                     == STL_SUCCESS );
                        }
                    }
                    else
                    {
                        /* ROWID Column이 오는 경우 Table Filter로만 처리 가능 */
                        if( sRefNodeItem->mRefColumnList.mHead->mExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN )
                        {
                            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                        &sTableAndFilter,
                                                                        1,
                                                                        &(sAndFilter->mOrFilters[i]),
                                                                        aEnv )
                                     == STL_SUCCESS );
                            continue;
                        }

                        /* Column이 하나 나오는 경우 */
                        for( j = 0; j < sIndexStat->mIndexKeyCount; j++ )
                        {
                            if( ((qlncExprColumn*)(sRefNodeItem->mRefColumnList.mHead->mExpr))->mColumnID ==
                                ellGetColumnID( &(sIndexStat->mIndexKeyDesc[j].mKeyColumnHandle) ) )
                            {
                                break;
                            }
                        }

                        if( j < sIndexStat->mIndexKeyCount )
                        {
                            /* Index Range 처리 가능한 Filter인 경우 */
                            if( qlncHasKeyColForRange( sBooleanFilter->mExpr,
                                                       aCBOptParamInfo->mLeftNode )
                                == STL_FALSE )
                            {
                                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                            &sIndexKeyAndFilter,
                                                                            1,
                                                                            &(sAndFilter->mOrFilters[i]),
                                                                            aEnv )
                                         == STL_SUCCESS );
                            }
                            else if( QLNC_IS_INDEX_EQUAL_FILTER( sBooleanFilter->mFuncID ) )
                            {
                                switch( sBooleanFilter->mFuncID )
                                {
                                    case KNL_BUILTIN_FUNC_IS_NULL :
                                        STL_TRY( qlncAppendOrFilterToAndFilterHead( &(aCBOptParamInfo->mParamInfo),
                                                                                    &sTmpRangeAndFilterArr[j],
                                                                                    1,
                                                                                    &(sAndFilter->mOrFilters[i]),
                                                                                    aEnv )
                                                 == STL_SUCCESS );
                                        break;
                                        
                                    case KNL_BUILTIN_FUNC_IS_UNKNOWN :
                                        if( ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[0]->mDataType ==
                                            DTL_TYPE_BOOLEAN )
                                        {
                                            STL_TRY( qlncAppendOrFilterToAndFilterHead( &(aCBOptParamInfo->mParamInfo),
                                                                                        &sTmpRangeAndFilterArr[j],
                                                                                        1,
                                                                                        &(sAndFilter->mOrFilters[i]),
                                                                                        aEnv )
                                                     == STL_SUCCESS );
                                        }
                                        else
                                        {
                                            /* Column에 대한 cast 연산이 필요한 경우 */
                                            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                        &sIndexKeyAndFilter,
                                                                                        1,
                                                                                        &(sAndFilter->mOrFilters[i]),
                                                                                        aEnv )
                                                     == STL_SUCCESS );
                                        }
                                        break;
                                            
                                    case KNL_BUILTIN_FUNC_IS :
                                        if( qlncIsExistColumnInExpr( ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[0],
                                                                     aCBOptParamInfo->mLeftNode )
                                            == STL_TRUE )
                                        {
                                            if( ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[0]->mDataType ==
                                                DTL_TYPE_BOOLEAN )
                                            {
                                                STL_TRY( qlncAppendOrFilterToAndFilterHead( &(aCBOptParamInfo->mParamInfo),
                                                                                            &sTmpRangeAndFilterArr[j],
                                                                                            1,
                                                                                            &(sAndFilter->mOrFilters[i]),
                                                                                            aEnv )
                                                         == STL_SUCCESS );
                                            }
                                            else
                                            {
                                                /* Column에 대한 cast 연산이 필요한 경우 */
                                                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                            &sIndexKeyAndFilter,
                                                                                            1,
                                                                                            &(sAndFilter->mOrFilters[i]),
                                                                                            aEnv )
                                                         == STL_SUCCESS );
                                            }
                                        }
                                        else
                                        {
                                            /* @todo 현재 여기를 올 수 없는 것으로 판단됨 */

                                            STL_TRY( qlncAppendOrFilterToAndFilterHead( &(aCBOptParamInfo->mParamInfo),
                                                                                        &sTmpRangeAndFilterArr[j],
                                                                                        1,
                                                                                        &(sAndFilter->mOrFilters[i]),
                                                                                        aEnv )
                                                     == STL_SUCCESS );
                                        }
                                        break;
                                    case KNL_BUILTIN_FUNC_IS_EQUAL :
                                        if( qlncIsExistColumnInExpr( ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[0],
                                                                     aCBOptParamInfo->mLeftNode )
                                            == STL_TRUE )
                                        {
                                            STL_TRY( qlnoGetCompareType( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                                         ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[0]->mDataType,
                                                                         ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[1]->mDataType,
                                                                         STL_FALSE,
                                                                         STL_TRUE,
                                                                         sBooleanFilter->mExpr->mPosition,
                                                                         &sCompareType,
                                                                         aEnv )
                                                     == STL_SUCCESS );
                                                
                                            if( sCompareType->mLeftType ==
                                                ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[0]->mDataType )
                                            {
                                                STL_TRY( qlncAppendOrFilterToAndFilterHead( &(aCBOptParamInfo->mParamInfo),
                                                                                            &sTmpRangeAndFilterArr[j],
                                                                                            1,
                                                                                            &(sAndFilter->mOrFilters[i]),
                                                                                            aEnv )
                                                         == STL_SUCCESS );
                                            }
                                            else
                                            {
                                                /* Column에 대한 cast 연산이 필요한 경우 */
                                                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                            &sIndexKeyAndFilter,
                                                                                            1,
                                                                                            &(sAndFilter->mOrFilters[i]),
                                                                                            aEnv )
                                                         == STL_SUCCESS );
                                            }
                                        }
                                        else
                                        {
                                            STL_TRY( qlnoGetCompareType( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                                         ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[0]->mDataType,
                                                                         ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[1]->mDataType,
                                                                         STL_TRUE,
                                                                         STL_FALSE,
                                                                         sBooleanFilter->mExpr->mPosition,
                                                                         &sCompareType,
                                                                         aEnv )
                                                     == STL_SUCCESS );
                                                
                                            if( sCompareType->mRightType ==
                                                ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[1]->mDataType )
                                            {
                                                STL_TRY( qlncAppendOrFilterToAndFilterHead( &(aCBOptParamInfo->mParamInfo),
                                                                                            &sTmpRangeAndFilterArr[j],
                                                                                            1,
                                                                                            &(sAndFilter->mOrFilters[i]),
                                                                                            aEnv )
                                                         == STL_SUCCESS );
                                            }
                                            else
                                            {
                                                /* Column에 대한 cast 연산이 필요한 경우 */
                                                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                            &sIndexKeyAndFilter,
                                                                                            1,
                                                                                            &(sAndFilter->mOrFilters[i]),
                                                                                            aEnv )
                                                         == STL_SUCCESS );
                                            }
                                        }
                                        break;
                                    default :
                                        STL_DASSERT( 0 );
                                        break;
                                }
                            }
                            else if( sBooleanFilter->mFuncID == KNL_BUILTIN_FUNC_LIKE )
                            {
                                if( QLNC_IS_INDEXABLE_LIKE(
                                        sBooleanFilter,
                                        sRefNodeItem->mNode->mNodeID,
                                        ellGetColumnID( &(sIndexStat->mIndexKeyDesc[j].mKeyColumnHandle) ) ) )
                                {
                                    STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                &sTmpRangeAndFilterArr[j],
                                                                                1,
                                                                                &(sAndFilter->mOrFilters[i]),
                                                                                aEnv )
                                             == STL_SUCCESS );
                                    sTmpHasLikeFilterArr[j] = STL_TRUE;
                                }
                                else
                                {
                                    /* @todo 현재 위에서 처리한 조건에 의해 여기를 올 수 없을 것으로 판단됨 */

                                    STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                &sIndexKeyAndFilter,
                                                                                1,
                                                                                &(sAndFilter->mOrFilters[i]),
                                                                                aEnv )
                                             == STL_SUCCESS );
                                }
                            }
                            else
                            {
                                if( ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgCount == 1 )
                                {
                                    switch( sBooleanFilter->mFuncID )
                                    {
                                        case KNL_BUILTIN_FUNC_IS_NOT_NULL :
                                            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                        &sTmpRangeAndFilterArr[j],
                                                                                        1,
                                                                                        &(sAndFilter->mOrFilters[i]),
                                                                                        aEnv )
                                                     == STL_SUCCESS );
                                            break;
                                            
                                        case KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN :
                                        case KNL_BUILTIN_FUNC_NOT :
                                            if( ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[0]->mDataType ==
                                                DTL_TYPE_BOOLEAN )
                                            {
                                                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                            &sTmpRangeAndFilterArr[j],
                                                                                            1,
                                                                                            &(sAndFilter->mOrFilters[i]),
                                                                                            aEnv )
                                                         == STL_SUCCESS );
                                            }
                                            else
                                            {
                                                /* Column에 대한 cast 연산이 필요한 경우 */
                                                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                            &sIndexKeyAndFilter,
                                                                                            1,
                                                                                            &(sAndFilter->mOrFilters[i]),
                                                                                            aEnv )
                                                         == STL_SUCCESS );
                                            }
                                            break;
                                            
                                        default :
                                            STL_DASSERT( 0 );
                                            break;
                                    }
                                }
                                else if( ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgCount == 2 )
                                {
                                    if( sBooleanFilter->mFuncID == KNL_BUILTIN_FUNC_IS_NOT )
                                    {
                                        if( qlncIsExistColumnInExpr( ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[0],
                                                                     aCBOptParamInfo->mLeftNode )
                                            == STL_TRUE )
                                        {
                                            if( ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[0]->mDataType ==
                                                DTL_TYPE_BOOLEAN )
                                            {
                                                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                            &sTmpRangeAndFilterArr[j],
                                                                                            1,
                                                                                            &(sAndFilter->mOrFilters[i]),
                                                                                            aEnv )
                                                         == STL_SUCCESS );
                                            }
                                            else
                                            {
                                                /* Column에 대한 cast 연산이 필요한 경우 */
                                                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                            &sIndexKeyAndFilter,
                                                                                            1,
                                                                                            &(sAndFilter->mOrFilters[i]),
                                                                                            aEnv )
                                                         == STL_SUCCESS );
                                            }
                                        }
                                        else
                                        {
                                            /* @todo is not 연산 뒤에는 column이 올 수 없으므로 여기에 들어올 수 없다. */

                                            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                        &sTmpRangeAndFilterArr[j],
                                                                                        1,
                                                                                        &(sAndFilter->mOrFilters[i]),
                                                                                        aEnv )
                                                     == STL_SUCCESS );
                                        }
                                    }
                                    else if( ( sBooleanFilter->mFuncID ==  KNL_BUILTIN_FUNC_IS_LESS_THAN ) ||
                                             ( sBooleanFilter->mFuncID ==  KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL ) ||
                                             ( sBooleanFilter->mFuncID ==  KNL_BUILTIN_FUNC_IS_GREATER_THAN ) ||
                                             ( sBooleanFilter->mFuncID ==  KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL ) )
                                    {
                                        if( qlncIsExistColumnInExpr( ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[0],
                                                                     aCBOptParamInfo->mLeftNode )
                                            == STL_TRUE )
                                        {
                                            STL_TRY( qlnoGetCompareType( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                                         ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[0]->mDataType,
                                                                         ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[1]->mDataType,
                                                                         STL_FALSE,
                                                                         STL_TRUE,
                                                                         sBooleanFilter->mExpr->mPosition,
                                                                         &sCompareType,
                                                                         aEnv )
                                                     == STL_SUCCESS );
                                                
                                            if( sCompareType->mLeftType ==
                                                ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[0]->mDataType )
                                            {
                                                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                            &sTmpRangeAndFilterArr[j],
                                                                                            1,
                                                                                            &(sAndFilter->mOrFilters[i]),
                                                                                            aEnv )
                                                         == STL_SUCCESS );
                                            }
                                            else
                                            {
                                                /* Column에 대한 cast 연산이 필요한 경우 */
                                                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                            &sIndexKeyAndFilter,
                                                                                            1,
                                                                                            &(sAndFilter->mOrFilters[i]),
                                                                                            aEnv )
                                                         == STL_SUCCESS );
                                            }
                                        }
                                        else
                                        {
                                            STL_TRY( qlnoGetCompareType( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                                         ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[0]->mDataType,
                                                                         ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[1]->mDataType,
                                                                         STL_TRUE,
                                                                         STL_FALSE,
                                                                         sBooleanFilter->mExpr->mPosition,
                                                                         &sCompareType,
                                                                         aEnv )
                                                     == STL_SUCCESS );
                                                
                                            if( sCompareType->mRightType ==
                                                ((qlncExprFunction* )sBooleanFilter->mExpr)->mArgs[1]->mDataType )
                                            {
                                                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                            &sTmpRangeAndFilterArr[j],
                                                                                            1,
                                                                                            &(sAndFilter->mOrFilters[i]),
                                                                                            aEnv )
                                                         == STL_SUCCESS );
                                            }
                                            else
                                            {
                                                /* Column에 대한 cast 연산이 필요한 경우 */
                                                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                            &sIndexKeyAndFilter,
                                                                                            1,
                                                                                            &(sAndFilter->mOrFilters[i]),
                                                                                            aEnv )
                                                         == STL_SUCCESS );
                                            }
                                        }
                                    }
                                    else
                                    {
                                        STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                    &sIndexKeyAndFilter,
                                                                                    1,
                                                                                    &(sAndFilter->mOrFilters[i]),
                                                                                    aEnv )
                                                 == STL_SUCCESS );
                                    }
                                }
                                else
                                {
                                    /* @todo Function의 Argument가 2개 이상인 경우는 없음 */

                                    /* Physical Compare로 처리가 불가능한 경우 */
                                    STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                                &sIndexKeyAndFilter,
                                                                                1,
                                                                                &(sAndFilter->mOrFilters[i]),
                                                                                aEnv )
                                             == STL_SUCCESS );
                                }
                            }
                        }
                        else
                        {
                            /* Index Column으로만 처리 불가능한 Filter인 경우 */
                            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                        &sTableAndFilter,
                                                                        1,
                                                                        &(sAndFilter->mOrFilters[i]),
                                                                        aEnv )
                                     == STL_SUCCESS );
                        }
                    }
                }
            }
        }
    }

    /* Temp Index Range Filter에 대하여 Range 가능 Filter와 불가능 Filter를 분리 */
    sEqualIndexKeyCount = 0;
    for( i = 0; i < sIndexStat->mIndexKeyCount; i++ )
    {
        if( sTmpRangeAndFilterArr[i].mOrCount == 0 )
        {
            /* 현재 Key Column에 조건이 없는 경우 */
            /* 이후의 모든 Filter들을 Index Key Filter로 등록 */
            for( i++; i< sIndexStat->mIndexKeyCount; i++ )
            {
                if( sTmpRangeAndFilterArr[i].mOrCount > 0 )
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                &sIndexKeyAndFilter,
                                                                sTmpRangeAndFilterArr[i].mOrCount,
                                                                sTmpRangeAndFilterArr[i].mOrFilters,
                                                                aEnv )
                             == STL_SUCCESS );
                }
            }
        }
        else
        {
            if( (i + 1) == sIndexStat->mIndexKeyCount )
            {
                /* 마지막 Index Column인 경우 */
                if( ( QLNC_IS_INDEX_EQUAL_FILTER(
                          ((qlncBooleanFilter*)(sTmpRangeAndFilterArr[i].mOrFilters[0]->mFilters[0]))->mFuncID ) ) ||
                    ( ((qlncBooleanFilter*)(sTmpRangeAndFilterArr[i].mOrFilters[0]->mFilters[0]))->mFuncID ==
                      KNL_BUILTIN_FUNC_LIKE ) )
                {
                    sEqualIndexKeyCount++;
                }

                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                            &sIndexRangeAndFilter,
                                                            sTmpRangeAndFilterArr[i].mOrCount,
                                                            sTmpRangeAndFilterArr[i].mOrFilters,
                                                            aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sTmpRangeAndFilterArr[i].mOrFilters[0]->mFilters[0]->mType ==
                             QLNC_EXPR_TYPE_BOOLEAN_FILTER );

                if( (sTmpRangeAndFilterArr[i+1].mOrCount == 0) ||
                    ( !QLNC_IS_INDEX_EQUAL_FILTER(
                           ((qlncBooleanFilter*)(sTmpRangeAndFilterArr[i].mOrFilters[0]->mFilters[0]))->mFuncID ) ) ||
                      ( sTmpHasLikeFilterArr[i] == STL_TRUE ) )
                {
                    /* 뒤에 오는 Index Column에 해당하는 Filter가 없는 경우 이거나 */
                    /* Equal Filter가 존재하지 않는 경우 */
                    STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                &sIndexRangeAndFilter,
                                                                sTmpRangeAndFilterArr[i].mOrCount,
                                                                sTmpRangeAndFilterArr[i].mOrFilters,
                                                                aEnv )
                             == STL_SUCCESS );

                    for( i++; i< sIndexStat->mIndexKeyCount; i++ )
                    {
                        if( sTmpRangeAndFilterArr[i].mOrCount > 0 )
                        {
                            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                        &sIndexKeyAndFilter,
                                                                        sTmpRangeAndFilterArr[i].mOrCount,
                                                                        sTmpRangeAndFilterArr[i].mOrFilters,
                                                                        aEnv )
                                     == STL_SUCCESS );
                        }
                    }
                }
                else
                {
                    sEqualIndexKeyCount++;

                    /* Equal Filter가 존재하고 다음 Column에 해당하는 Filter가 존재하는 경우 */
                    if( ( sTmpHasLikeFilterArr[i] == STL_TRUE ) &&
                        ( !QLNC_IS_INDEX_EQUAL_FILTER(
                               ((qlncBooleanFilter*)(sTmpRangeAndFilterArr[i].mOrFilters[j]->mFilters[0]))->mFuncID ) ) )
                    {
                        STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                    &sIndexRangeAndFilter,
                                                                    sTmpRangeAndFilterArr[i].mOrCount,
                                                                    sTmpRangeAndFilterArr[i].mOrFilters,
                                                                    aEnv )
                                 == STL_SUCCESS );

                        for( i++; i< sIndexStat->mIndexKeyCount; i++ )
                        {
                            if( sTmpRangeAndFilterArr[i].mOrCount > 0 )
                            {
                                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                            &sIndexKeyAndFilter,
                                                                            sTmpRangeAndFilterArr[i].mOrCount,
                                                                            sTmpRangeAndFilterArr[i].mOrFilters,
                                                                            aEnv )
                                         == STL_SUCCESS );
                            }
                        }
                    }
                    else
                    {
                        for( j = 0; j < sTmpRangeAndFilterArr[i].mOrCount; j++ )
                        {
                            if( ( QLNC_IS_INDEX_EQUAL_FILTER(
                                      ((qlncBooleanFilter*)(sTmpRangeAndFilterArr[i].mOrFilters[j]->mFilters[0]))->mFuncID ) ) ||
                                ( ((qlncBooleanFilter*)(sTmpRangeAndFilterArr[i].mOrFilters[j]->mFilters[0]))->mFuncID ==
                                  KNL_BUILTIN_FUNC_LIKE ) )
                            {
                                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                            &sIndexRangeAndFilter,
                                                                            1,
                                                                            &(sTmpRangeAndFilterArr[i].mOrFilters[j]),
                                                                            aEnv )
                                         == STL_SUCCESS );
                            }
                            else
                            {
                                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                            &sIndexKeyAndFilter,
                                                                            1,
                                                                            &(sTmpRangeAndFilterArr[i].mOrFilters[j]),
                                                                            aEnv )
                                         == STL_SUCCESS );
                            }
                        }
                    }
                }
            }
        }
    }

    STL_RAMP( RAMP_SKIP_FILTER_ANALYZER );
    
    /* Table Scan 필요 여부 검사 */
    if( sTableAndFilter.mOrCount > 0 )
    {
        sIndexScanCost->mNeedTableScan = STL_TRUE;
    }
    else
    {
        sIndexScanCost->mNeedTableScan = STL_FALSE;
        sRefColumnItem = ((qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode))->mReadColumnList.mHead;
        while( sRefColumnItem != NULL )
        {
            if( sRefColumnItem->mExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN )
            {
                sRefColumnItem = sRefColumnItem->mNext;
                continue;
            }

            for( i = 0; i < sIndexStat->mIndexKeyCount; i++ )
            {
                STL_DASSERT( sRefColumnItem->mExpr->mType == QLNC_EXPR_TYPE_COLUMN );
                if( ((qlncExprColumn*)(sRefColumnItem->mExpr))->mColumnID ==
                    ellGetColumnID( &(sIndexStat->mIndexKeyDesc[i].mKeyColumnHandle) ) )
                {
                    break;
                }
            }

            if( i == sIndexStat->mIndexKeyCount )
            {
                sIndexScanCost->mNeedTableScan = STL_TRUE;
                break;
            }

            sRefColumnItem = sRefColumnItem->mNext;
        }
    }

    /* Index Scan Cost 정보 설정 */
    if( sIndexRangeAndFilter.mOrCount > 0 )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    &sIndexRangeAndFilter,
                                    &(sIndexScanCost->mIndexRangeAndFilter),
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sIndexScanCost->mIndexRangeAndFilter = NULL;
    }

    if( sIndexKeyAndFilter.mOrCount > 0 )
    {
        /* /\** */
        /*  * @todo CHECK 가 필요함 */
        /*  *\/ */
        /* if ( (sIndexRangeAndFilter.mOrCount == 0) && */
        /*      (sTableAndFilter.mOrCount > 0) ) */
        /* { */
        /*     /\** */
        /*      * Key Range 없이 */
        /*      * Key Filter 와 Table Filter 가 함께 존재하는 경우임. */
        /*      * */
        /*      * (Index Full Scan + Table RID Scan) 비용이 (Table Full Scan) 보다 훨씬 좋게 평가될 수 있음. */
        /*      * */
        /*      * Index Full Scan 선택이 정확한 경우,   약간의 성능 향상이 있으나, */
        /*      * Index Full Scan 선택이 부정확한 경우, 매우 큰 성능 저하가 발생함. */
        /*      *\/ */

        /*     STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo), */
        /*                                                 &sTableAndFilter, */
        /*                                                 sIndexKeyAndFilter.mOrCount, */
        /*                                                 sIndexKeyAndFilter.mOrFilters, */
        /*                                                 aEnv ) */
        /*              == STL_SUCCESS ); */
            
        /*     sIndexKeyAndFilter.mOrCount = 0; */
        /* } */
        /* else */
        /* { */
            STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                        &sIndexKeyAndFilter,
                                        &(sIndexScanCost->mIndexKeyAndFilter),
                                        aEnv )
                     == STL_SUCCESS );
        /* } */
    }
    else
    {
        sIndexScanCost->mIndexKeyAndFilter = NULL;
    }

    if( sTableAndFilter.mOrCount > 0 )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    &sTableAndFilter,
                                    &(sIndexScanCost->mTableAndFilter),
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sIndexScanCost->mTableAndFilter = NULL;
    }

    /* Index Range And Filter 재정렬 */
    if( sIndexScanCost->mIndexRangeAndFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sIndexScanCost->mIndexRangeAndFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Index Key And Filter 재정렬 */
    if( sIndexScanCost->mIndexKeyAndFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sIndexScanCost->mIndexKeyAndFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Table And Filter 재정렬 */
    if( sIndexScanCost->mTableAndFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sIndexScanCost->mTableAndFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Index Read Bytes 및 Table Read Bytes 계산 */
    sIndexColumnPosCalCount = (stlFloat64)0.0;
    sTableColumnPosCalCount = (stlFloat64)0.0;
    sIndexRowLen = (stlFloat64)0.0;
    sTableRowLen = (stlFloat64)0.0;
    /* sOutputIOCost = (stlFloat64)0.0; */
    sRefColumnItem = ((qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode))->mReadColumnList.mHead;
    while( sRefColumnItem != NULL )
    {
        if( sRefColumnItem->mIdx != QLNC_ROWID_COLUMN_IDX )
        {
            for( i = 0; i < sIndexStat->mIndexKeyCount; i++ )
            {
                STL_DASSERT( sRefColumnItem->mExpr->mType == QLNC_EXPR_TYPE_COLUMN );
                if( ((qlncExprColumn*)(sRefColumnItem->mExpr))->mColumnID ==
                    ellGetColumnID( &(sIndexStat->mIndexKeyDesc[i].mKeyColumnHandle) ) )
                {
                    break;
                }
            }

            if( i < sIndexStat->mIndexKeyCount )
            {
                /* Index Row Len 계산 */
                sIndexRowLen +=
                    ((qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode))->mTableStat->mColumnStat[sRefColumnItem->mIdx].mAvgColumnLen;
                if( sIndexColumnPosCalCount < sRefColumnItem->mIdx )
                {
                    sIndexColumnPosCalCount = sRefColumnItem->mIdx;
                }
            }
            else
            {
                /* Table Row Len 계산 */
                sTableRowLen +=
                    ((qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode))->mTableStat->mColumnStat[sRefColumnItem->mIdx].mAvgColumnLen;
                if( sTableColumnPosCalCount < sRefColumnItem->mIdx )
                {
                    sTableColumnPosCalCount = sRefColumnItem->mIdx;
                }
            }
        }

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /**
     * Index Scan Cost 계산
     */

    /* Index Range Filter Cost 계산 */
    sRowCount = ((qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode))->mTableStat->mRowCount;

    if( sIndexRangeAndFilter.mOrCount > 0 )
    {
        if( sIndexStat->mIsUnique == STL_TRUE )
        {
            sIndexRangeFilterCost = QLNC_UNIQ_IDX_RANGE_COST_PER_SEARCH;
        }
        else
        {
            sIndexRangeFilterCost = QLNC_NON_UNIQ_IDX_RANGE_COST_PER_SEARCH;
        }

        /* Index의 Unique 여부에 따른 처리 */
        if( (sIndexStat->mIsUnique == STL_TRUE) &&
            (sIndexStat->mIndexKeyCount == sEqualIndexKeyCount) )
        {
            /* Index의 Last Column에 대한 Filter가 Equal로 이루어진 경우 */
            sRowCount = 1;
        }
        else
        {
            sRowCount = 
                qlncGetExpectedAndFilterResultCount( &sIndexRangeAndFilter,
                                                     STL_TRUE,
                                                     sRowCount );
        }
    }
    else
    {
        sIndexRangeFilterCost = (stlFloat64)0.0;
    }

    /* Index Key Filter Cost 계산 */
    if( sIndexKeyAndFilter.mOrCount > 0 )
    {
        sIndexKeyFilterCost = QLNC_FILTER_PREPARE_COST;

        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    aCBOptParamInfo->mLeftNode,
                                                    &sIndexKeyAndFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sIndexKeyFilterCost += sTmpFilterCost;
    }
    else
    {
        sIndexKeyFilterCost = (stlFloat64)0.0;
    }

    /* Table Filter Cost 및 Table Scan에 따른 Page 접근 비용 계산 */
    sPhysicalRowCount = sRowCount;
    if( sIndexScanCost->mNeedTableScan == STL_TRUE )
    {
        sTableFilterCost = (sRowCount * QLNC_INDEX_TO_TABLE_COST_PER_ROW );

        /* Table Filter Cost 계산 */
        if( sTableAndFilter.mOrCount > 0 )
        {
            sTableFilterCost += (sRowCount * QLNC_FILTER_PREPARE_COST );

            STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                        aCBOptParamInfo->mLeftNode,
                                                        &sTableAndFilter,
                                                        sRowCount,
                                                        &sTmpFilterCost,
                                                        &sPhysicalRowCount,
                                                        &sRowCount,
                                                        aEnv )
                     == STL_SUCCESS );

            sTableFilterCost += sTmpFilterCost;
        }
    }
    else
    {
        sTableFilterCost = (stlFloat64)0.0;
    }

    /* Table에 속한 컬럼들 Data Copy 비용 */
    /* sOutputIOCost += (sPhysicalRowCount * sTableRowLen * QLNC_DATA_COPY_COST_PER_BYTE); */

    /* IO Cost 조정 */
    /* sOutputIOCost *= QLNC_GET_ADJUST_VALUE_BY_PAGE_COUNT( sIndexStat->mLeafPageCount ); */

    /* Index Cost 설정 */
    sIndexScanCost->mCostCommon.mFirstFilterCost = sIndexRangeFilterCost;
    sIndexScanCost->mCostCommon.mNextFilterCost =
        ( sIndexKeyFilterCost + sTableFilterCost + QLNC_OUTPUT_COST_PER_ROW );
    sIndexScanCost->mCostCommon.mBuildCost = (stlFloat64)0.0;
    sIndexScanCost->mCostCommon.mReadIOCost = (stlFloat64)0.0;
    /**
     * @todo 정확한 Output Column List를 알 수 있는 경우 Output IO Cost를 계산하도록 한다.
     */
    /* sIndexScanCost->mCostCommon.mOutputIOCost = sOutputIOCost; */
    /* sIndexScanCost->mCostCommon.mTotalCost = */
    /*     sIndexRangeFilterCost + */
    /*     sIndexKeyFilterCost + */
    /*     sTableFilterCost + */
    /*     sOutputIOCost; */
    sIndexScanCost->mCostCommon.mOutputIOCost = (stlFloat64)0.0;

    QLNC_SET_TOTAL_COST( &(sIndexScanCost->mCostCommon) );
    sIndexScanCost->mCostCommon.mResultRowCount = sRowCount;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index Combine Cost를 계산한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptIndexCombineCost( qlncCBOptParamInfo * aCBOptParamInfo,
                                     qllEnv             * aEnv )
{
    qlncIndexCombineCost    * sIndexCombineCost = NULL;
    qlncRefExprItem         * sRefExprItem      = NULL;

    stlInt32                  i, j;
    stlBool                   sIsFirstCost;
    qlncIndexScanCost         sTmpIndexScanCost;

    stlFloat64                sFilterCost;
    stlFloat64                sBuildCost;
    stlFloat64                sReadIOCost;
    /* stlFloat64                sOutputIOCost; */
    stlFloat64                sResultRowCount;
    qlncRefExprList           sAndFilterList;
    qlncListColMapList      * sListColMapList;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /* Index Combine Cost 설정 */
    sIndexCombineCost = (qlncIndexCombineCost*)(aCBOptParamInfo->mCost);

    /* CNF -> DNF 변환 */
    STL_TRY( qlncConvertCnf2Dnf( &(aCBOptParamInfo->mParamInfo),
                                 aCBOptParamInfo->mFilter,
                                 &(sIndexCombineCost->mDnfAndFilterCount),
                                 &(sIndexCombineCost->mDnfAndFilterArr),
                                 aEnv )
             == STL_SUCCESS );

    /* Index Scan Cost Array 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncIndexScanCost ) * sIndexCombineCost->mDnfAndFilterCount,
                                (void**)&(sIndexCombineCost->mIndexScanCostArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* DNF에 대한 Cost 계산 */
    sFilterCost = (stlFloat64)0.0;
    sReadIOCost = (stlFloat64)0.0;
    /* sOutputIOCost = (stlFloat64)0.0; */
    sResultRowCount = (stlFloat64)0.0;
    for( i = 0; i < sIndexCombineCost->mDnfAndFilterCount; i++ )
    {
        sIsFirstCost = STL_TRUE;
        
        /* DNF And Filter에 대하여 각 Index별 Cost 계산 */
        for( j = 0; j < ((qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode))->mAccPathHint->mPossibleIndexScanCount; j++ )
        {
            aCBOptParamInfo->mFilter = &(sIndexCombineCost->mDnfAndFilterArr[i]);
            aCBOptParamInfo->mCost = (qlncCostCommon*)&sTmpIndexScanCost;

            STL_TRY( qlncCBOptIndexScanCost( aCBOptParamInfo,
                                             &(((qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode))->mAccPathHint->mPossibleIndexScanInfoArr[j]),
                                             ((qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode))->mAccPathHint->mPossibleIndexScanInfoArr[j].mIsAsc,
                                             NULL,
                                             aEnv )
                     == STL_SUCCESS );

            sTmpIndexScanCost.mListColMap = NULL;

            if( sIsFirstCost == STL_TRUE )
            {
                stlMemcpy( &(sIndexCombineCost->mIndexScanCostArr[i]),
                           &sTmpIndexScanCost,
                           STL_SIZEOF( qlncIndexScanCost ) );

                sIsFirstCost = STL_FALSE;
            }
            else
            {
                if( (sTmpIndexScanCost.mIndexRangeAndFilter != NULL) &&
                    (sTmpIndexScanCost.mIndexRangeAndFilter->mOrCount > 0) )
                {
                    if( (sIndexCombineCost->mIndexScanCostArr[i].mIndexRangeAndFilter != NULL) &&
                        (sIndexCombineCost->mIndexScanCostArr[i].mIndexRangeAndFilter->mOrCount > 0) )
                    {
                        if( QLNC_LESS_THAN_DOUBLE(
                                sTmpIndexScanCost.mCostCommon.mTotalCost,
                                sIndexCombineCost->mIndexScanCostArr[i].mCostCommon.mTotalCost ) )
                        {
                            stlMemcpy( &(sIndexCombineCost->mIndexScanCostArr[i]),
                                       &sTmpIndexScanCost,
                                       STL_SIZEOF( qlncIndexScanCost ) );
                        }
                    }
                    else
                    {
                        stlMemcpy( &(sIndexCombineCost->mIndexScanCostArr[i]),
                                   &sTmpIndexScanCost,
                                   STL_SIZEOF( qlncIndexScanCost ) );
                    }
                }
                else
                {
                    if( (sIndexCombineCost->mIndexScanCostArr[i].mIndexRangeAndFilter != NULL) &&
                        (sIndexCombineCost->mIndexScanCostArr[i].mIndexRangeAndFilter->mOrCount > 0) )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        if( QLNC_LESS_THAN_DOUBLE(
                                sTmpIndexScanCost.mCostCommon.mTotalCost,
                                sIndexCombineCost->mIndexScanCostArr[i].mCostCommon.mTotalCost ) )
                        {
                            stlMemcpy( &(sIndexCombineCost->mIndexScanCostArr[i]),
                                       &sTmpIndexScanCost,
                                       STL_SIZEOF( qlncIndexScanCost ) );
                        }
                    }
                }
            }
        }

        /* DNF And Filter에 대하여 각 Index별 In Key Range Cost 계산 */
        for( j = 0; j < ((qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode))->mAccPathHint->mPossibleIndexScanCount; j++ )
        {
            /* Filter로 부터 In-Key Range 대상이 될 수 있는 List Function 들을 얻어온다 */
            STL_TRY( qlncGetInKeyRangeAndFilter( aCBOptParamInfo,
                                                 &(((qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode))->mAccPathHint->mPossibleIndexScanInfoArr[j]),
                                                 &(sIndexCombineCost->mDnfAndFilterArr[i]),
                                                 &sAndFilterList,
                                                 &sListColMapList,
                                                 aEnv )
                     == STL_SUCCESS );
            
            /* In-Key Range 고려하여 각각의 Index 에 대한 Cost 를 평가한다 */
            sRefExprItem = sAndFilterList.mHead;
            while( sRefExprItem != NULL )
            {
                aCBOptParamInfo->mFilter = (qlncAndFilter*)sRefExprItem->mExpr;
                aCBOptParamInfo->mCost = (qlncCostCommon*)&sTmpIndexScanCost;

                STL_TRY( qlncCBOptIndexScanCost( aCBOptParamInfo,
                                                 &(((qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode))->mAccPathHint->mPossibleIndexScanInfoArr[j]),
                                                 ((qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode))->mAccPathHint->mPossibleIndexScanInfoArr[j].mIsAsc,
                                                 sRefExprItem->mExpr,
                                                 aEnv )
                         == STL_SUCCESS );

                sTmpIndexScanCost.mListColMap = & sListColMapList->mListColMap;

                /* 반드시 Range를 타는 경우에 대해서만 적용할 수 있다. */
                if( qlncIsPossibleInKeyRange( &sTmpIndexScanCost ) == STL_TRUE )
                {
                    /* In-Key Range의 Right 쪽의 Value 개수만큼 Cost를 곱한다. */
                    sTmpIndexScanCost.mCostCommon.mTotalCost *=
                        sListColMapList->mListColMap.mMapCount;
                    sTmpIndexScanCost.mCostCommon.mFirstFilterCost *=
                        sListColMapList->mListColMap.mMapCount;
                    sTmpIndexScanCost.mCostCommon.mNextFilterCost *=
                        sListColMapList->mListColMap.mMapCount;

                    if( sIsFirstCost == STL_TRUE )
                    {
                        stlMemcpy( &(sIndexCombineCost->mIndexScanCostArr[i]),
                                   &sTmpIndexScanCost,
                                   STL_SIZEOF( qlncIndexScanCost ) );

                        sIsFirstCost = STL_FALSE;
                    }
                    else
                    {
                        if( (sIndexCombineCost->mIndexScanCostArr[i].mIndexRangeAndFilter != NULL) &&
                            (sIndexCombineCost->mIndexScanCostArr[i].mIndexRangeAndFilter->mOrCount > 0) )
                        {
                            if( QLNC_LESS_THAN_DOUBLE(
                                    sTmpIndexScanCost.mCostCommon.mTotalCost,
                                    sIndexCombineCost->mIndexScanCostArr[i].mCostCommon.mTotalCost ) )
                            {
                                stlMemcpy( &(sIndexCombineCost->mIndexScanCostArr[i]),
                                           &sTmpIndexScanCost,
                                           STL_SIZEOF( qlncIndexScanCost ) );
                            }
                        }
                        else
                        {
                            stlMemcpy( &(sIndexCombineCost->mIndexScanCostArr[i]),
                                       &sTmpIndexScanCost,
                                       STL_SIZEOF( qlncIndexScanCost ) );
                        }
                    }
                }
            
                sRefExprItem = sRefExprItem->mNext;
                sListColMapList = sListColMapList->mNext;
            }
        }

        sFilterCost +=
            ( sIndexCombineCost->mIndexScanCostArr[i].mCostCommon.mFirstFilterCost +
              sIndexCombineCost->mIndexScanCostArr[i].mCostCommon.mNextFilterCost );
        sReadIOCost +=
            ( sIndexCombineCost->mIndexScanCostArr[i].mCostCommon.mBuildCost +
              sIndexCombineCost->mIndexScanCostArr[i].mCostCommon.mReadIOCost );
        /* sOutputIOCost += sIndexCombineCost->mIndexScanCostArr[i].mCostCommon.mOutputIOCost; */
        sResultRowCount += sIndexCombineCost->mIndexScanCostArr[i].mCostCommon.mResultRowCount;
    }

    /* Concat Node에 Insert 및 Unique 체크 비용 추가 */
    if( sResultRowCount < QLNC_HASH_SECTION1_MAX_ROW_COUNT )
    {
        sFilterCost += QLNC_HASH_KEY_FILTER_COST_PER_ROW_SEC1;
    }
    else if( sResultRowCount < QLNC_HASH_SECTION2_MAX_ROW_COUNT )
    {
        sFilterCost += QLNC_HASH_KEY_FILTER_COST_PER_ROW_SEC2;
    }
    else
    {
        sFilterCost += QLNC_HASH_KEY_FILTER_COST_PER_ROW_SEC3;
    }

    sBuildCost = ( sResultRowCount * QLNC_HASH_INSTANT_BUILD_COST_PER_ROW );

    /* Concat Node에 의해 제거되는 중복 result 개수 반영 */
    sResultRowCount *= ( (stlFloat64)1.0 - QLNC_DEFAULT_CONCAT_DUP_RATE );

    /* Index Combine Cost 설정 */
    sIndexCombineCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sIndexCombineCost->mCostCommon.mNextFilterCost = sFilterCost;
    sIndexCombineCost->mCostCommon.mBuildCost = sBuildCost;
    sIndexCombineCost->mCostCommon.mReadIOCost = sReadIOCost;

    /**
     * @todo 정확한 Output Column List를 알 수 있는 경우 Output IO Cost를 계산하도록 한다.
     */
    /* sIndexCombineCost->mCostCommon.mOutputIOCost = sOutputIOCost; */
    /* sIndexCombineCost->mCostCommon.mTotalCost = sFilterCost + sReadIOCost + sOutputIOCost; */
    sIndexCombineCost->mCostCommon.mOutputIOCost = (stlFloat64)0.0;

    QLNC_SET_TOTAL_COST( &(sIndexCombineCost->mCostCommon) );
    sIndexCombineCost->mCostCommon.mResultRowCount = sResultRowCount;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Rowid Scan Cost를 계산한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptRowidScanCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                  qllEnv                * aEnv )
{
    qlncRowidScanCost   * sRowidScanCost    = NULL;

    stlInt32              i;
    qlncAndFilter         sRowidAndFilter;
    qlncAndFilter         sTableAndFilter;
    qlncAndFilter       * sTmpAndFilter     = NULL;

    stlFloat64            sRowCount;
    stlFloat64            sFilterCost;
    stlFloat64            sTmpFilterCost;
    stlFloat64            sPhysicalRowCount;

    qlncBaseTableNode   * sBaseTableNode;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /* Candidate Base Table Node 설정 */
    sBaseTableNode = (qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode);

    /* Rowid Scan Cost 설정 */
    sRowidScanCost = (qlncRowidScanCost*)(aCBOptParamInfo->mCost);

    /* Rowid Filter와 Table Filter 분리 */
    QLNC_INIT_AND_FILTER( &sRowidAndFilter );
    QLNC_INIT_AND_FILTER( &sTableAndFilter );

    /* And Filter 복사 */
    STL_DASSERT( aCBOptParamInfo->mFilter != NULL );
    sTmpAndFilter = aCBOptParamInfo->mFilter;
    for( i = 0; i < sTmpAndFilter->mOrCount; i++ )
    {
        if( qlncIsRowidORFilter( sBaseTableNode,
                                 sTmpAndFilter->mOrFilters[i] )
            == STL_TRUE )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                        &sRowidAndFilter,
                                                        1,
                                                        &(sTmpAndFilter->mOrFilters[i]),
                                                        aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                        &sTableAndFilter,
                                                        1,
                                                        &(sTmpAndFilter->mOrFilters[i]),
                                                        aEnv )
                     == STL_SUCCESS );
        }
    }

    STL_DASSERT( sRowidAndFilter.mOrCount > 0 );

    STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                &sRowidAndFilter,
                                &(sRowidScanCost->mRowidAndFilter),
                                aEnv )
             == STL_SUCCESS );

    if( sTableAndFilter.mOrCount > 0 )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    &sTableAndFilter,
                                    &(sRowidScanCost->mTableAndFilter),
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sRowidScanCost->mTableAndFilter = NULL;
    }

    /* Table And Filter 재정렬 */
    if( sRowidScanCost->mTableAndFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sRowidScanCost->mTableAndFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Rowid Scan Cost 계산*/
    sRowCount = sBaseTableNode->mTableStat->mRowCount;

    /* Rowid Filter Cost */
    sFilterCost = QLNC_FILTER_PREPARE_COST;

    qlncCBOptRowidAndFilterCostAndRowCount( &sRowidAndFilter,
                                            sRowCount,
                                            &sTmpFilterCost,
                                            &sRowCount );
    sFilterCost += sTmpFilterCost;

    /* Access Table Page Cost 계산 */
    sFilterCost += (sRowCount * QLNC_READ_COST_PER_ROW);

    /* Table Filter Cost */
    sPhysicalRowCount = sRowCount;
    if( sTableAndFilter.mOrCount > 0 )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    aCBOptParamInfo->mLeftNode,
                                                    &sTableAndFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost += sTmpFilterCost;
    }

    sRowidScanCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sRowidScanCost->mCostCommon.mNextFilterCost = sFilterCost + QLNC_OUTPUT_COST_PER_ROW;
    sRowidScanCost->mCostCommon.mBuildCost = (stlFloat64)0.0;
    sRowidScanCost->mCostCommon.mReadIOCost = (stlFloat64)0.0;
    /**
     * @todo 정확한 Output Column List를 알 수 있는 경우 Output IO Cost를 계산하도록 한다.
     */
    /* sRowidScanCost->mCostCommon.mOutputIOCost = */
    /*     (sPhysicalRowCount * sResultRowLen * QLNC_DATA_COPY_COST_PER_BYTE) * */
    /*     QLNC_GET_ADJUST_VALUE_BY_PAGE_COUNT( sBaseTableNode->mTableStat->mPageCount ); */
    sRowidScanCost->mCostCommon.mOutputIOCost = (stlFloat64)0.0;

    QLNC_SET_TOTAL_COST( &(sRowidScanCost->mCostCommon) );
    sRowidScanCost->mCostCommon.mResultRowCount = sRowCount;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Filter Cost를 계산한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptFilterCost( qlncCBOptParamInfo   * aCBOptParamInfo,
                               qllEnv               * aEnv )
{
    stlInt32              i;
    qlncSubTableNode    * sSubTableNode     = NULL;
    qlncFilterCost      * sSubFilterCost    = NULL;

    stlFloat64            sRowCount;
    stlFloat64            sFilterCost;
    stlFloat64            sTmpFilterCost;
    stlFloat64            sPhysicalRowCount;

    qlncNodeArray         sNodeArray;

    stlBool               sNeedMaterializedView;

//    stlInt32              sHashKeyCount;
//    qlncExprCommon     ** sLeftHashKeyArr       = NULL;
//    qlncExprCommon     ** sRightHashKeyArr      = NULL;

    stlInt32              sSortKeyCount;
    qlncExprCommon     ** sLeftSortKeyArr       = NULL;
    qlncExprCommon     ** sRightSortKeyArr      = NULL;

    stlUInt8            * sSortKeyFlags         = NULL;
    smlSortTableAttr      sSortTableAttr = { STL_FALSE,     /* not top-down */
                                             STL_FALSE,     /* not volatile */
                                             STL_FALSE,     /* not leaf-only */
                                             STL_FALSE,     /* not unique */
                                             STL_FALSE };   /* not null excluded */

    qlncNodeCommon      * sCopiedSubTableNode   = NULL;
    qlncNodeCommon      * sCopiedNode           = NULL;

//    smlIndexAttr          sHashTableAttr;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /* Sub Table Node 설정 */
    sSubTableNode = (qlncSubTableNode*)(aCBOptParamInfo->mLeftNode);

    /* Filter Cost 설정 */
    sSubFilterCost = (qlncFilterCost*)(aCBOptParamInfo->mCost);

    /**
     * Materialized View가 필요한지 체크
     */

    /* Node Array 초기화 */
    STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                      &sNodeArray,
                                      aEnv )
             == STL_SUCCESS );

    /* Node Array 생성 */
    STL_TRY( qlncMakeNodeArrayOnCurrNode( &sNodeArray,
                                          (qlncNodeCommon*)sSubTableNode,
                                          aEnv )
             == STL_SUCCESS );

    /* Matialized View 필요 여부 체크 */
    if( aCBOptParamInfo->mFilter != NULL )
    {
        /* Filter에 외부 노드 참조 컬럼이 존재해야 한다. */
        if( qlncIsExistOuterColumnOnExpr( (qlncExprCommon*)(aCBOptParamInfo->mFilter),
                                          &sNodeArray )
            == STL_TRUE )
        {
            /* Query Node에는 외부 노드 참조 컬럼이 존재하지 않아야 한다. */
            if( qlncIsExistOuterColumnOnCandNode( sSubTableNode->mQueryNode,
                                                  &sNodeArray )
                == STL_FALSE )
            {
                sNeedMaterializedView = STL_TRUE;
            }
            else
            {
                sNeedMaterializedView = STL_FALSE;
            }
        }
        else
        {
            sNeedMaterializedView = STL_FALSE;
        }
    }
    else
    {
        sNeedMaterializedView = STL_FALSE;
    }

    if( sNeedMaterializedView == STL_TRUE )
    {
        /* Materialized View가 필요한 경우 */
        qlncAndFilter     sOuterAndFilter;
        qlncAndFilter     sNonOuterAndFilter;
        qlncAndFilter   * sEquiOuterAndFilter       = NULL;
        qlncAndFilter   * sNonEquiOuterAndFilter    = NULL;
        qlncAndFilter   * sPushKeyFilter            = NULL;

        /* Outer Column이 존재하는 Filter 분리 */
        QLNC_INIT_AND_FILTER( &sOuterAndFilter );
        QLNC_INIT_AND_FILTER( &sNonOuterAndFilter );

        for( i = 0; i < aCBOptParamInfo->mFilter->mOrCount; i++ )
        {
            if( qlncIsExistOuterColumnOnExpr( (qlncExprCommon*)(aCBOptParamInfo->mFilter->mOrFilters[i]),
                                              &sNodeArray )
                == STL_TRUE )
            {
                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                            &sOuterAndFilter,
                                                            1,
                                                            &(aCBOptParamInfo->mFilter->mOrFilters[i]),
                                                            aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                            &sNonOuterAndFilter,
                                                            1,
                                                            &(aCBOptParamInfo->mFilter->mOrFilters[i]),
                                                            aEnv )
                         == STL_SUCCESS );
            }
        }

        /* Outer And Filter에 대해 Hash Instant가 가능한지 판단 */
        /** @todo Hash Instant 구현필요
         * 현재 Hash Instant가 구현되어 있지 않아 무조건 Sort Instant로 동작하도록 한다.
         * 구현되면 아래 if 0 부분이 동작하도록 하면 된다.
         */
#if 0
        for( i = 0; i < sOuterAndFilter.mOrCount; i++ )
        {
            if( (sOuterAndFilter.mOrFilters[i]->mFilterCount == 1) &&
                (sOuterAndFilter.mOrFilters[i]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER) )
            {
                if( qlncIsPossibleHashJoinCondition(
                        (qlncBooleanFilter*)(sOuterAndFilter.mOrFilters[i]->mFilters[0]),
                        (qlncNodeCommon*)sSubTableNode,
                        NULL )
                    == STL_TRUE )
                {
                    break;
                }
            }
        }
#else
        i = sOuterAndFilter.mOrCount;
#endif

        if( i < sOuterAndFilter.mOrCount )
        {
#if 0
            /* Hash Instant 가능 */
            STL_TRY( qlncSeperateJoinConditionForHashJoin( &(aCBOptParamInfo->mParamInfo),
                                                           (qlncNodeCommon*)sSubTableNode,
                                                           &sOuterAndFilter,
                                                           &sEquiOuterAndFilter,
                                                           &sNonEquiOuterAndFilter,
                                                           aEnv )
                     == STL_SUCCESS );

            /* Hash Key 수집 */
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * sEquiOuterAndFilter->mOrCount,
                                        (void**) &sLeftHashKeyArr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * sEquiOuterAndFilter->mOrCount,
                                        (void**) &sRightHashKeyArr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sHashKeyCount = sEquiOuterAndFilter->mOrCount;
            for( i = 0; i < sEquiOuterAndFilter->mOrCount; i++ )
            {
                qlncBooleanFilter   * sBooleanFilter    = NULL;

                STL_DASSERT( sEquiOuterAndFilter->mOrFilters[i]->mFilterCount == 1 );
                sBooleanFilter = (qlncBooleanFilter*)(sEquiOuterAndFilter->mOrFilters[i]->mFilters[0]);
                if( qlncIsExistNode( (qlncNodeCommon*)sSubTableNode,
                                     sBooleanFilter->mLeftRefNodeList->mHead->mNode ) )
                {
                    sLeftHashKeyArr[i] =
                        ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
                    sRightHashKeyArr[i] =
                        ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
                }
                else
                {
                    sLeftHashKeyArr[i] =
                        ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
                    sRightHashKeyArr[i] =
                        ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
                }
            }

            /* Hash Table Attribute 설정 */
            stlMemset( &sHashTableAttr, 0x00, STL_SIZEOF( smlIndexAttr ) );

            sHashTableAttr.mValidFlags |= SML_INDEX_LOGGING_YES;
            sHashTableAttr.mLoggingFlag = STL_FALSE;
            sHashTableAttr.mValidFlags |= SML_INDEX_UNIQUENESS_YES;
            sHashTableAttr.mUniquenessFlag = STL_FALSE;
            sHashTableAttr.mValidFlags |= SML_INDEX_PRIMARY_YES;
            sHashTableAttr.mPrimaryFlag = STL_FALSE;
            sHashTableAttr.mValidFlags |= SML_INDEX_SKIP_NULL_YES;
            sHashTableAttr.mSkipNullFlag = STL_TRUE;

            /* Sub Table Node 복사 */
            STL_TRY( qlncCopySubNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                    (qlncNodeCommon*)sSubTableNode,
                                                    &sCopiedSubTableNode,
                                                    aEnv )
                     == STL_SUCCESS );

            /* Non Outer And Filter 복사 */
            if( sNonOuterAndFilter.mOrCount > 0 )
            {
                STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                            &sNonOuterAndFilter,
                                            &(((qlncSubTableNode*)sCopiedSubTableNode)->mFilter),
                                            aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                ((qlncSubTableNode*)sCopiedSubTableNode)->mFilter = NULL;
            }

            /* Sub Table Node Cost 계산 */
            aCBOptParamInfo->mLeftNode = sCopiedSubTableNode;
            STL_TRY( qlncCBOptSubTable( aCBOptParamInfo,
                                        aEnv )
                     == STL_SUCCESS );

            /* Hash Instant 생성 처리 */
            STL_TRY( qlncCreateHashInstantForHashJoin( &(aCBOptParamInfo->mParamInfo),
                                                       QLNC_INSTANT_TYPE_NORMAL,
                                                       &sHashTableAttr,
                                                       ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                                       sHashKeyCount,
                                                       sRightHashKeyArr,
                                                       sCopiedSubTableNode,
                                                       &sCopiedNode,
                                                       aEnv )
                     == STL_SUCCESS );

            /* Hash Key에 대한 Expression을 생성하여 Push Filter에 추가 */
            for( i = 0; i < sHashKeyCount; i++ )
            {
                qlncBooleanFilter   * sBooleanFilter    = NULL;
                qlncExprFunction    * sFunction         = NULL;

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncExprFunction ),
                                            (void**) &sFunction,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sBooleanFilter = (qlncBooleanFilter*)(sEquiOuterAndFilter->mOrFilters[i]->mFilters[0]);
                stlMemcpy( sFunction,
                           sBooleanFilter->mExpr,
                           STL_SIZEOF( qlncExprFunction ) );

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncExprCommon* ) * sFunction->mArgCount,
                                            (void**) &(sFunction->mArgs),
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( qlncIsExistNode( (qlncNodeCommon*)sSubTableNode,
                                     sBooleanFilter->mLeftRefNodeList->mHead->mNode ) )
                {
                    sFunction->mArgs[1] = sLeftHashKeyArr[i];
                    sFunction->mArgs[0] = sRightHashKeyArr[i];
                }
                else
                {
                    sFunction->mArgs[1] = sRightHashKeyArr[i];
                    sFunction->mArgs[0] = sLeftHashKeyArr[i];
                }

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncBooleanFilter ),
                                            (void**) &sBooleanFilter,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sBooleanFilter,
                           sEquiOuterAndFilter->mOrFilters[i]->mFilters[0],
                           STL_SIZEOF( qlncBooleanFilter ) );

                sBooleanFilter->mExpr = (qlncExprCommon*)sFunction;

                if( sPushKeyFilter == NULL )
                {
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncAndFilter ),
                                                (void**) &sPushKeyFilter,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    QLNC_SET_EXPR_COMMON( &sPushKeyFilter->mCommon,
                                          &QLL_CANDIDATE_MEM( aEnv ),
                                          aEnv,
                                          QLNC_EXPR_TYPE_AND_FILTER,
                                          sBooleanFilter->mExpr->mExprPhraseFlag,
                                          sBooleanFilter->mExpr->mIterationTime,
                                          sBooleanFilter->mExpr->mPosition,
                                          sBooleanFilter->mExpr->mColumnName,
                                          DTL_TYPE_BOOLEAN );

                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncOrFilter* ),
                                                (void**) &(sPushKeyFilter->mOrFilters),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    sPushKeyFilter->mOrCount = 1;

                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncOrFilter ),
                                                (void**) &(sPushKeyFilter->mOrFilters[0]),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    QLNC_SET_EXPR_COMMON( &sPushKeyFilter->mOrFilters[0]->mCommon,
                                          &QLL_CANDIDATE_MEM( aEnv ),
                                          aEnv,
                                          QLNC_EXPR_TYPE_OR_FILTER,
                                          sBooleanFilter->mExpr->mExprPhraseFlag,
                                          sBooleanFilter->mExpr->mIterationTime,
                                          sBooleanFilter->mExpr->mPosition,
                                          sBooleanFilter->mExpr->mColumnName,
                                          DTL_TYPE_BOOLEAN );

                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncExprCommon* ),
                                                (void**) &(sPushKeyFilter->mOrFilters[0]->mFilters),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    sPushKeyFilter->mOrFilters[0]->mFilterCount = 1;
                    sPushKeyFilter->mOrFilters[0]->mFilters[0] = (qlncExprCommon*)sBooleanFilter;
                    sPushKeyFilter->mOrFilters[0]->mSubQueryExprList = NULL;
                }
                else
                {
                    STL_TRY( qlncAppendFilterToAndFilter( &(aCBOptParamInfo->mParamInfo),
                                                          sPushKeyFilter,
                                                          (qlncExprCommon*)sBooleanFilter,
                                                          aEnv )
                             == STL_SUCCESS );
                }
            }

            /* Push Filter을 Hash Instant에 추가 */
            ((qlncHashInstantNode*)sCopiedNode)->mKeyFilter = sPushKeyFilter;
            ((qlncHashInstantNode*)sCopiedNode)->mRecFilter = sNonEquiOuterAndFilter;
            sNonEquiOuterAndFilter = NULL;

            /* Hash Instant Cost 계산 */
            aCBOptParamInfo->mLeftNode = sCopiedNode;
            STL_TRY( qlncCBOptHashInstant( aCBOptParamInfo,
                                           aEnv )
                     == STL_SUCCESS );

            /* Sub Filter Cost의 Filter 설정 */
            sSubFilterCost->mFilter = NULL;

            /* Materialized Node 설정 */
            sSubFilterCost->mMaterializedNode = sCopiedNode;

            /* Filter Cost 설정 */
            sSubFilterCost->mCostCommon.mFirstFilterCost = sCopiedNode->mBestCost->mFirstFilterCost;
            sSubFilterCost->mCostCommon.mNextFilterCost = sCopiedNode->mBestCost->mNextFilterCost;
            sSubFilterCost->mCostCommon.mBuildCost = sCopiedNode->mBestCost->mBuildCost;
            sSubFilterCost->mCostCommon.mReadIOCost = sCopiedNode->mBestCost->mReadIOCost;
            sSubFilterCost->mCostCommon.mOutputIOCost = sCopiedNode->mBestCost->mOutputIOCost;

            QLNC_SET_TOTAL_COST( &(sSubFilterCost->mCostCommon) );
            sSubFilterCost->mCostCommon.mResultRowCount = sCopiedNode->mBestCost->mResultRowCount;

            STL_THROW( RAMP_FINISH );
#endif
        }
        else
        {
            /* Sort Instant 가능 */
            STL_TRY( qlncSeperateJoinConditionForMergeJoin( &(aCBOptParamInfo->mParamInfo),
                                                            &sOuterAndFilter,
                                                            &sEquiOuterAndFilter,
                                                            &sNonEquiOuterAndFilter,
                                                            aEnv )
                     == STL_SUCCESS );

            /* Non Equi Outer And Filter의 하나를 Equi Outer And Filter에 추가 */
            if( sEquiOuterAndFilter == NULL )
            {
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncAndFilter ),
                                            (void**) &sEquiOuterAndFilter,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                QLNC_INIT_AND_FILTER( sEquiOuterAndFilter );
            }

            if( sNonEquiOuterAndFilter != NULL )
            {
                for( i = 0; i < sNonEquiOuterAndFilter->mOrCount; i++ )
                {
                    /* Or가 없는 Boolean Filter 경우에 대해서 추가 */
                    if( (sNonEquiOuterAndFilter->mOrFilters[i]->mFilterCount == 1) &&
                        (sNonEquiOuterAndFilter->mOrFilters[i]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER) )
                    {
                        qlncBooleanFilter   * sBooleanFilter    = NULL;

                        sBooleanFilter = (qlncBooleanFilter*)(sNonEquiOuterAndFilter->mOrFilters[i]->mFilters[0]);
                        if( (sBooleanFilter->mPossibleJoinCond == STL_TRUE) &&
                            (sBooleanFilter->mFuncID != KNL_BUILTIN_FUNC_AND) &&
                            (sBooleanFilter->mFuncID != KNL_BUILTIN_FUNC_OR) &&
                            (sBooleanFilter->mLeftRefNodeList != NULL) &&
                            (sBooleanFilter->mLeftRefNodeList->mCount == 0) &&
                            (sBooleanFilter->mRightRefNodeList != NULL) &&
                            (sBooleanFilter->mRightRefNodeList->mCount == 0) )
                        {
                            /* @todo 현재 여기에 들어올 수 없음 */

                            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                        sEquiOuterAndFilter,
                                                                        1,
                                                                        &(sNonEquiOuterAndFilter->mOrFilters[0]),
                                                                        aEnv )
                                     == STL_SUCCESS );
                            break;
                        }
                    }
                }

                /* Non Equi Outer And Filter에서 Filter 하나가 빠진 경우 Compact 수행 */
                if( i < sNonEquiOuterAndFilter->mOrCount )
                {
                    /* @todo 현재 여기에 들어올 수 없음 */

                    if( sNonEquiOuterAndFilter->mOrCount == 1 )
                    {
                        sNonEquiOuterAndFilter = NULL;
                    }
                    else
                    {
                        STL_TRY( qlncCompactAndFilter( sNonEquiOuterAndFilter,
                                                       aEnv )
                                 == STL_SUCCESS );
                    }
                }
            }

            /* Sort Key가 존재하지 않으면 Materialized View를 생성하지 않는다. */
            /**
             * @todo 향후 Flat Instant으로 처리할 수 있도록 구현 필요
             */

            STL_TRY_THROW( sEquiOuterAndFilter->mOrCount > 0, RAMP_NON_MATERIALIZED );

            /* Sort Key 수집 */
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * sEquiOuterAndFilter->mOrCount,
                                        (void**) &sLeftSortKeyArr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * sEquiOuterAndFilter->mOrCount,
                                        (void**) &sRightSortKeyArr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sSortKeyCount = sEquiOuterAndFilter->mOrCount;
            if( sEquiOuterAndFilter != NULL )
            {
                for( i = 0; i < sEquiOuterAndFilter->mOrCount; i++ )
                {
                    qlncBooleanFilter   * sBooleanFilter    = NULL;

                    STL_DASSERT( sEquiOuterAndFilter->mOrFilters[i]->mFilterCount == 1 );
                    sBooleanFilter = (qlncBooleanFilter*)(sEquiOuterAndFilter->mOrFilters[i]->mFilters[0]);
                    if( qlncIsExistNode( (qlncNodeCommon*)sSubTableNode,
                                         sBooleanFilter->mLeftRefNodeList->mHead->mNode ) )
                    {
                        sLeftSortKeyArr[i] =
                            ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
                        sRightSortKeyArr[i] =
                            ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
                    }
                    else
                    {
                        sLeftSortKeyArr[i] =
                            ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
                        sRightSortKeyArr[i] =
                            ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
                    }
                }
            }

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( stlUInt8 ) * sSortKeyCount,
                                        (void**) &sSortKeyFlags,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemset( sSortKeyFlags, 0x00, STL_SIZEOF( stlInt8 ) * sSortKeyCount );

            for( i = 0; i < sSortKeyCount; i++ )
            {
                DTL_SET_INDEX_COLUMN_FLAG( sSortKeyFlags[i],
                                           DTL_KEYCOLUMN_INDEX_ORDER_ASC,
                                           DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE,
                                           DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST );
            }

            /* Sub Table Node 복사 */
            STL_TRY( qlncCopySubNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                    (qlncNodeCommon*)sSubTableNode,
                                                    &sCopiedSubTableNode,
                                                    aEnv )
                     == STL_SUCCESS );

            /* Non Outer And Filter 복사 */
            if( sNonOuterAndFilter.mOrCount > 0 )
            {
                STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                            &sNonOuterAndFilter,
                                            &(((qlncSubTableNode*)sCopiedSubTableNode)->mFilter),
                                            aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                ((qlncSubTableNode*)sCopiedSubTableNode)->mFilter = NULL;
            }

            /* Sub Table Node Cost 계산 */
            aCBOptParamInfo->mLeftNode = sCopiedSubTableNode;
            STL_TRY( qlncCBOptSubTable( aCBOptParamInfo,
                                        aEnv )
                     == STL_SUCCESS );

            /* Sort Instant 생성 처리 */
            STL_TRY( qlncCreateSortInstantForMergeJoin( &(aCBOptParamInfo->mParamInfo),
                                                        QLNC_INSTANT_TYPE_NORMAL,
                                                        &sSortTableAttr,
                                                        ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                                        sSortKeyCount,
                                                        sRightSortKeyArr,
                                                        sSortKeyFlags,
                                                        sCopiedSubTableNode,
                                                        &sCopiedNode,
                                                        aEnv )
                     == STL_SUCCESS );

            /* Sort Key에 대한 Expression을 생성하여 Push Filter에 추가 */
            for( i = 0; i < sSortKeyCount; i++ )
            {
                qlncBooleanFilter   * sBooleanFilter    = NULL;
                qlncExprFunction    * sFunction         = NULL;

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncExprFunction ),
                                            (void**) &sFunction,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sBooleanFilter = (qlncBooleanFilter*)(sEquiOuterAndFilter->mOrFilters[i]->mFilters[0]);
                stlMemcpy( sFunction,
                           sBooleanFilter->mExpr,
                           STL_SIZEOF( qlncExprFunction ) );

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncExprCommon* ) * sFunction->mArgCount,
                                            (void**) &(sFunction->mArgs),
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( qlncIsExistNode( (qlncNodeCommon*)sSubTableNode,
                                     sBooleanFilter->mLeftRefNodeList->mHead->mNode ) )
                {
                    sFunction->mArgs[1] = sLeftSortKeyArr[i];
                    sFunction->mArgs[0] = sRightSortKeyArr[i];
                }
                else
                {
                    sFunction->mArgs[1] = sRightSortKeyArr[i];
                    sFunction->mArgs[0] = sLeftSortKeyArr[i];
                }

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncBooleanFilter ),
                                            (void**) &sBooleanFilter,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sBooleanFilter,
                           sEquiOuterAndFilter->mOrFilters[i]->mFilters[0],
                           STL_SIZEOF( qlncBooleanFilter ) );

                sBooleanFilter->mExpr = (qlncExprCommon*)sFunction;

                if( sPushKeyFilter == NULL )
                {
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncAndFilter ),
                                                (void**) &sPushKeyFilter,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    QLNC_SET_EXPR_COMMON( &sPushKeyFilter->mCommon,
                                          &QLL_CANDIDATE_MEM( aEnv ),
                                          aEnv,
                                          QLNC_EXPR_TYPE_AND_FILTER,
                                          sBooleanFilter->mExpr->mExprPhraseFlag,
                                          sBooleanFilter->mExpr->mIterationTime,
                                          sBooleanFilter->mExpr->mPosition,
                                          sBooleanFilter->mExpr->mColumnName,
                                          DTL_TYPE_BOOLEAN );

                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncOrFilter* ),
                                                (void**) &(sPushKeyFilter->mOrFilters),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    sPushKeyFilter->mOrCount = 1;

                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncOrFilter ),
                                                (void**) &(sPushKeyFilter->mOrFilters[0]),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    QLNC_SET_EXPR_COMMON( &sPushKeyFilter->mOrFilters[0]->mCommon,
                                          &QLL_CANDIDATE_MEM( aEnv ),
                                          aEnv,
                                          QLNC_EXPR_TYPE_OR_FILTER,
                                          sBooleanFilter->mExpr->mExprPhraseFlag,
                                          sBooleanFilter->mExpr->mIterationTime,
                                          sBooleanFilter->mExpr->mPosition,
                                          sBooleanFilter->mExpr->mColumnName,
                                          DTL_TYPE_BOOLEAN );

                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncExprCommon* ),
                                                (void**) &(sPushKeyFilter->mOrFilters[0]->mFilters),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    sPushKeyFilter->mOrFilters[0]->mFilterCount = 1;
                    sPushKeyFilter->mOrFilters[0]->mFilters[0] = (qlncExprCommon*)sBooleanFilter;
                    sPushKeyFilter->mOrFilters[0]->mSubQueryExprList = NULL;
                }
                else
                {
                    STL_TRY( qlncAppendFilterToAndFilter( &(aCBOptParamInfo->mParamInfo),
                                                          sPushKeyFilter,
                                                          (qlncExprCommon*)sBooleanFilter,
                                                          aEnv )
                             == STL_SUCCESS );
                }
            }

            /* Push Filter을 Sort Instant에 추가 */
            ((qlncSortInstantNode*)sCopiedNode)->mKeyFilter = sPushKeyFilter;
            ((qlncSortInstantNode*)sCopiedNode)->mRecFilter = sNonEquiOuterAndFilter;
            sNonEquiOuterAndFilter = NULL;

            /* Sort Instant Cost 계산 */
            aCBOptParamInfo->mLeftNode = sCopiedNode;
            STL_TRY( qlncCBOptSortInstant( aCBOptParamInfo,
                                           aEnv )
                     == STL_SUCCESS );

            /* Sub Filter Cost의 Filter 설정 */
            sSubFilterCost->mFilter = NULL;

            /* Materialized Node 설정 */
            sSubFilterCost->mMaterializedNode = sCopiedNode;

            /* Filter Cost 설정 */
            sSubFilterCost->mCostCommon.mFirstFilterCost = sCopiedNode->mBestCost->mFirstFilterCost;
            sSubFilterCost->mCostCommon.mNextFilterCost = sCopiedNode->mBestCost->mNextFilterCost;
            sSubFilterCost->mCostCommon.mBuildCost = sCopiedNode->mBestCost->mBuildCost;
            sSubFilterCost->mCostCommon.mReadIOCost = sCopiedNode->mBestCost->mReadIOCost;
            sSubFilterCost->mCostCommon.mOutputIOCost = sCopiedNode->mBestCost->mOutputIOCost;

            QLNC_SET_TOTAL_COST( &(sSubFilterCost->mCostCommon) );
            sSubFilterCost->mCostCommon.mResultRowCount = sCopiedNode->mBestCost->mResultRowCount;

            STL_THROW( RAMP_FINISH );
        }
    }


    STL_RAMP( RAMP_NON_MATERIALIZED );


    /* And Filter 복사 */
    if( aCBOptParamInfo->mFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    aCBOptParamInfo->mFilter,
                                    &(sSubFilterCost->mFilter),
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sSubFilterCost->mFilter = NULL;
    }

    /* And Filter 재정렬 */
    if( sSubFilterCost->mFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sSubFilterCost->mFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Filter Cost 계산*/
    sRowCount = sSubTableNode->mQueryNode->mBestCost->mResultRowCount;
    if( sSubFilterCost->mFilter == NULL )
    {
        sFilterCost = (stlFloat64)0.0;
    }
    else
    {
        sFilterCost = QLNC_FILTER_PREPARE_COST;

        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sSubFilterCost->mFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost += sTmpFilterCost;
    }

    /* Materialized Node 설정 */
    sSubFilterCost->mMaterializedNode = NULL;

    /* Filter Cost 설정 */
    sSubFilterCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sSubFilterCost->mCostCommon.mNextFilterCost =
        ( sFilterCost + sSubTableNode->mQueryNode->mBestCost->mTotalCost );
    sSubFilterCost->mCostCommon.mBuildCost = (stlFloat64)0.0;
    sSubFilterCost->mCostCommon.mReadIOCost = (stlFloat64)0.0;
    /**
     * @todo 정확한 Output Column List를 알 수 있는 경우 Output IO Cost를 계산하도록 한다.
     */
    /* sSubFilterCost->mCostCommon.mOutputIOCost = */
    /*     (sRowCount * sResultRowLen * QLNC_DATA_COPY_COST_PER_BYTE); */
    sSubFilterCost->mCostCommon.mOutputIOCost = (stlFloat64)0.0;

    QLNC_SET_TOTAL_COST( &(sSubFilterCost->mCostCommon) );
    sSubFilterCost->mCostCommon.mResultRowCount = sRowCount;


    STL_RAMP( RAMP_FINISH );


    /* Filter Cost 정보를 Sub Table Node에 연결 */
    sSubTableNode->mFilterCost = sSubFilterCost;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Inner Join Cost를 계산한다.
 * @param[in]   aCBOptParamInfo         Cost Based Optimizer Parameter Info
 * @param[in]   aInnerJoinNodeInfoArr   Inner Join Node Info Array
 * @param[in]   aNodeCount              Inner Join Node Info Array의 Node Count
 * @param[in]   aNodeIdx                Node Index Number
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCBOptInnerJoinCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                  qlncInnerJoinNodeInfo * aInnerJoinNodeInfoArr,
                                  stlInt32                aNodeCount,
                                  stlInt32                aNodeIdx,
                                  qllEnv                * aEnv )
{
    qlncInnerJoinTableNode  * sInnerJoinTableNode   = NULL;
    qlncNodeCommon          * sLeftNode             = NULL;
    qlncNodeCommon          * sRightNode            = NULL;
    qlncAndFilter           * sJoinCondition        = NULL;
    qlncAndFilter           * sJoinFilter           = NULL;
    qlncAndFilter           * sJoinCombineAndFilter = NULL;

    stlBool                   sPossibleNestedJoin       = STL_TRUE;
    stlBool                   sPossibleIndexNestedJoin  = STL_TRUE;
    stlBool                   sPossibleMergeJoin        = STL_TRUE;
    stlBool                   sPossibleHashJoin         = STL_TRUE;
    stlBool                   sPossibleJoinCombine      = STL_TRUE;

    stlBool                   sFixedPos;

    stlInt32                  sDistribCnfCount;

    stlFloat64                sExpectedJoinResultRowCount;

    qlncNestedJoinCost      * sNestedJoinCost       = NULL;
    qlncNestedJoinCost      * sSwitchNestedJoinCost = NULL;
    qlncMergeJoinCost       * sMergeJoinCost        = NULL;
    qlncMergeJoinCost       * sSwitchMergeJoinCost  = NULL;
    qlncHashJoinCost        * sHashJoinCost         = NULL;
    qlncHashJoinCost        * sSwitchHashJoinCost   = NULL;
    qlncJoinCombineCost     * sJoinCombineCost      = NULL;
    stlInt32                  i;
    stlBool                   sIsPossibleHashJoin;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* Inner Join Table Node 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncInnerJoinTableNode ),
                                (void**)&sInnerJoinTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_NODE_COMMON( &(sInnerJoinTableNode->mNodeCommon),
                           *(aCBOptParamInfo->mParamInfo.mGlobalNodeID),
                           QLNC_NODE_TYPE_INNER_JOIN_TABLE );
    (*(aCBOptParamInfo->mParamInfo.mGlobalNodeID))++;    /**< Global ID 증가 */
    sInnerJoinTableNode->mNamedColumnList = NULL;
    sInnerJoinTableNode->mLeftNode = aCBOptParamInfo->mLeftNode;
    sInnerJoinTableNode->mRightNode = aCBOptParamInfo->mRightNode;
    sLeftNode = sInnerJoinTableNode->mLeftNode;
    sRightNode = sInnerJoinTableNode->mRightNode;

    /* 처리 가능한 Join Condition 및 Join Filter 찾기 */
    if( aCBOptParamInfo->mJoinCond != NULL )
    {
        STL_TRY( qlncFindUsableFilterOnInnerJoinNode( &(aCBOptParamInfo->mParamInfo),
                                                      sInnerJoinTableNode,
                                                      aCBOptParamInfo->mJoinCond,
                                                      ( aNodeIdx + 1 == aNodeCount
                                                        ? STL_TRUE : STL_FALSE ),
                                                      aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sInnerJoinTableNode->mJoinCondition = NULL;
        sInnerJoinTableNode->mJoinFilter = NULL;
    }

    /* Join Combine을 위한 And Filter 복사 */
    if( sInnerJoinTableNode->mJoinCondition != NULL )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sInnerJoinTableNode->mJoinCondition,
                                    &sJoinCombineAndFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sJoinCombineAndFilter = NULL;
    }

    if( sInnerJoinTableNode->mJoinFilter != NULL )
    {
        if( sJoinCombineAndFilter != NULL )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                        sJoinCombineAndFilter,
                                                        sInnerJoinTableNode->mJoinFilter->mOrCount,
                                                        sInnerJoinTableNode->mJoinFilter->mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                        sInnerJoinTableNode->mJoinFilter,
                                        &sJoinCombineAndFilter,
                                        aEnv )
                     == STL_SUCCESS );
        }
    }

    if( sJoinCombineAndFilter != NULL )
    {
        STL_TRY( qlncCompactAndFilter( sJoinCombineAndFilter,
                                       aEnv )
                 == STL_SUCCESS );
    }

    /* Left Node와 Right Node의 위치 설정 */
    QLNC_SET_JOIN_CHILD_NODE_POS( sLeftNode, sRightNode, sFixedPos );

    /* 가능한 Join Method 찾기 */
    STL_TRY( qlncFindPossibleJoinOperation( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                            sInnerJoinTableNode->mJoinCondition,
                                            QLNC_JOIN_TYPE_INNER,
                                            sLeftNode,
                                            sRightNode,
                                            STL_FALSE,
                                            &sPossibleNestedJoin,
                                            &sPossibleIndexNestedJoin,
                                            NULL,
                                            &sPossibleMergeJoin,
                                            &sPossibleHashJoin,
                                            NULL,
                                            aEnv )
             == STL_SUCCESS );

    sDistribCnfCount = qlncGetDistribCnfCount( sJoinCombineAndFilter );
    if( (sDistribCnfCount > 1) && (sDistribCnfCount <= QLNC_MAX_DISTRIBUTED_CNF_COUNT) )
    {
        if( (qlncIsUpdatableNode( sLeftNode ) == STL_TRUE) &&
            (qlncIsUpdatableNode( sRightNode ) == STL_TRUE) )
        {
            sPossibleJoinCombine = STL_TRUE;
        }
        else
        {
            sPossibleJoinCombine = STL_FALSE;
        }
    }
    else
    {
        sPossibleJoinCombine = STL_FALSE;
    }


    /**
     * Join Cost 계산
     */

    /* Expected Join Result Row Count 계산 */
    if( (sInnerJoinTableNode->mJoinCondition != NULL) ||
        (sInnerJoinTableNode->mJoinFilter != NULL) )
    {
        aCBOptParamInfo->mJoinCond = sInnerJoinTableNode->mJoinCondition;
        aCBOptParamInfo->mFilter = sInnerJoinTableNode->mJoinFilter;
        STL_TRY( qlncGetExpectedInnerJoinResultRowCount( aCBOptParamInfo,
                                                         aInnerJoinNodeInfoArr,
                                                         aNodeIdx + 1,
                                                         &sExpectedJoinResultRowCount,
                                                         aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( sLeftNode->mIsAnalyzedCBO == STL_TRUE );
        STL_DASSERT( sRightNode->mIsAnalyzedCBO == STL_TRUE );
        sExpectedJoinResultRowCount =
            sLeftNode->mBestCost->mResultRowCount *
            sRightNode->mBestCost->mResultRowCount;
    }

    if( (sPossibleNestedJoin == STL_TRUE) ||
        (sPossibleIndexNestedJoin == STL_TRUE) )
    {
        /* Join Condition과 Join Filter를 결합 (SubQuery의 경우 Join Filter에 남겨야 한다.) */
        sJoinCondition = NULL;
        sJoinFilter = NULL;
        if( sInnerJoinTableNode->mJoinCondition != NULL )
        {
            STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                        sInnerJoinTableNode->mJoinCondition,
                                        &sJoinCondition,
                                        aEnv )
                     == STL_SUCCESS );
        }

        if( sInnerJoinTableNode->mJoinFilter != NULL )
        {
            for( i = 0; i < sInnerJoinTableNode->mJoinFilter->mOrCount; i++ )
            {
                if( sInnerJoinTableNode->mJoinFilter->mOrFilters[i]->mSubQueryExprList == NULL )
                {
                    /* Join Condition에 추가 */
                    if( sJoinCondition == NULL )
                    {
                        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                    STL_SIZEOF( qlncAndFilter ),
                                                    (void**) &sJoinCondition,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        QLNC_INIT_AND_FILTER( sJoinCondition );
                    }

                    STL_TRY( qlncAppendOrFilterToAndFilterTail(
                                 &(aCBOptParamInfo->mParamInfo),
                                 sJoinCondition,
                                 1,
                                 &(sInnerJoinTableNode->mJoinFilter->mOrFilters[i]),
                                 aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /* Join Filter에 추가 */
                    if( sJoinFilter == NULL )
                    {
                        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                    STL_SIZEOF( qlncAndFilter ),
                                                    (void**) &sJoinFilter,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        QLNC_INIT_AND_FILTER( sJoinFilter );
                    }

                    STL_TRY( qlncAppendOrFilterToAndFilterTail(
                                 &(aCBOptParamInfo->mParamInfo),
                                 sJoinFilter,
                                 1,
                                 &(sInnerJoinTableNode->mJoinFilter->mOrFilters[i]),
                                 aEnv )
                             == STL_SUCCESS );
                }
            }
        }

        /* Nested Join Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncNestedJoinCost ),
                                    (void**)&sNestedJoinCost,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Nested Inner Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = sLeftNode;
        aCBOptParamInfo->mRightNode = sRightNode;
        aCBOptParamInfo->mJoinCond = sJoinCondition;
        aCBOptParamInfo->mFilter = sJoinFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)sNestedJoinCost;
        STL_TRY( qlncCBOptNestedInnerJoinCost( aCBOptParamInfo,
                                               sExpectedJoinResultRowCount,
                                               sPossibleNestedJoin,
                                               sPossibleIndexNestedJoin,
                                               aEnv )
                 == STL_SUCCESS );
        sInnerJoinTableNode->mNestedJoinCost = sNestedJoinCost;

        if( sFixedPos == STL_FALSE )
        {
            /* Nested Join Cost 할당 */
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncNestedJoinCost ),
                                        (void**)&sSwitchNestedJoinCost,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            aCBOptParamInfo->mLeftNode = sRightNode;
            aCBOptParamInfo->mRightNode = sLeftNode;
            aCBOptParamInfo->mJoinCond = sJoinCondition;
            aCBOptParamInfo->mFilter = sJoinFilter;
            aCBOptParamInfo->mCost = (qlncCostCommon*)sSwitchNestedJoinCost;
            STL_TRY( qlncCBOptNestedInnerJoinCost( aCBOptParamInfo,
                                                   sExpectedJoinResultRowCount,
                                                   sPossibleNestedJoin,
                                                   sPossibleIndexNestedJoin,
                                                   aEnv )
                     == STL_SUCCESS );

            if( QLNC_LESS_THAN_DOUBLE( sSwitchNestedJoinCost->mCostCommon.mTotalCost,
                                       sInnerJoinTableNode->mNestedJoinCost->mCostCommon.mTotalCost ) )
            {
                sInnerJoinTableNode->mNestedJoinCost = sSwitchNestedJoinCost;
            }
        }

        if( (sInnerJoinTableNode->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sInnerJoinTableNode->mNestedJoinCost->mCostCommon.mTotalCost,
                                    sInnerJoinTableNode->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sInnerJoinTableNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_NESTED_LOOPS;
            sInnerJoinTableNode->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sInnerJoinTableNode->mNestedJoinCost);
        }
    }
    else
    {
        sInnerJoinTableNode->mNestedJoinCost = NULL;
    }

    if( sPossibleMergeJoin == STL_TRUE )
    {
        /* Merge Join Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncMergeJoinCost ),
                                    (void**)&sMergeJoinCost,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Merge Inner Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = sLeftNode;
        aCBOptParamInfo->mRightNode = sRightNode;
        aCBOptParamInfo->mJoinCond = sInnerJoinTableNode->mJoinCondition;
        aCBOptParamInfo->mFilter = sInnerJoinTableNode->mJoinFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)sMergeJoinCost;
        STL_TRY( qlncCBOptMergeInnerJoinCost( aCBOptParamInfo,
                                              sExpectedJoinResultRowCount,
                                              aEnv )
                 == STL_SUCCESS );
        sInnerJoinTableNode->mMergeJoinCost = sMergeJoinCost;

        if( sFixedPos == STL_FALSE )
        {
            /* Merge Join Cost 할당 */
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncMergeJoinCost ),
                                        (void**)&sSwitchMergeJoinCost,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            aCBOptParamInfo->mLeftNode = sRightNode;
            aCBOptParamInfo->mRightNode = sLeftNode;
            aCBOptParamInfo->mJoinCond = sInnerJoinTableNode->mJoinCondition;
            aCBOptParamInfo->mFilter = sInnerJoinTableNode->mJoinFilter;
            aCBOptParamInfo->mCost = (qlncCostCommon*)sSwitchMergeJoinCost;
            STL_TRY( qlncCBOptMergeInnerJoinCost( aCBOptParamInfo,
                                                  sExpectedJoinResultRowCount,
                                                  aEnv )
                     == STL_SUCCESS );

            if( QLNC_LESS_THAN_DOUBLE( sSwitchMergeJoinCost->mCostCommon.mTotalCost,
                                       sInnerJoinTableNode->mMergeJoinCost->mCostCommon.mTotalCost ) )
            {
                sInnerJoinTableNode->mMergeJoinCost = sSwitchMergeJoinCost;
            }
        }

        if( (sInnerJoinTableNode->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sInnerJoinTableNode->mMergeJoinCost->mCostCommon.mTotalCost,
                                    sInnerJoinTableNode->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sInnerJoinTableNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_MERGE;
            sInnerJoinTableNode->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sInnerJoinTableNode->mMergeJoinCost);
        }
    }
    else
    {
        sInnerJoinTableNode->mMergeJoinCost = NULL;
    }

    if( sPossibleHashJoin == STL_TRUE )
    {
        /* Hash Join Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncHashJoinCost ),
                                    (void**)&sHashJoinCost,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Hash Inner Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = sLeftNode;
        aCBOptParamInfo->mRightNode = sRightNode;
        aCBOptParamInfo->mJoinCond = sInnerJoinTableNode->mJoinCondition;
        aCBOptParamInfo->mFilter = sInnerJoinTableNode->mJoinFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)sHashJoinCost;
        STL_TRY( qlncCBOptHashInnerJoinCost( aCBOptParamInfo,
                                             sExpectedJoinResultRowCount,
                                             aEnv )
                 == STL_SUCCESS );
        sInnerJoinTableNode->mHashJoinCost = sHashJoinCost;

        if( sFixedPos == STL_FALSE )
        {
            /* Join Condition에 Equi-Join 조건이 존재하는지 검사 */
            for( i = 0; i < sInnerJoinTableNode->mJoinCondition->mOrCount; i++ )
            {
                if( (sInnerJoinTableNode->mJoinCondition->mOrFilters[i]->mFilterCount == 1) &&
                    (sInnerJoinTableNode->mJoinCondition->mOrFilters[i]->mFilters[0]->mType ==
                     QLNC_EXPR_TYPE_BOOLEAN_FILTER) )
                {
                    STL_TRY( qlncIsPossibleHashJoinCondition(
                                 aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                 (qlncBooleanFilter*)(sInnerJoinTableNode->mJoinCondition->mOrFilters[i]->mFilters[0]),
                                 sLeftNode,
                                 NULL,
                                 &sIsPossibleHashJoin,
                                 aEnv )
                             == STL_SUCCESS );

                    if( sIsPossibleHashJoin == STL_TRUE )
                    {
                        break;
                    }
                }
            }

            if( i == sInnerJoinTableNode->mJoinCondition->mOrCount )
            {
                /* Do Nothing */
            }
            else
            {
                /* Hash Join Cost 할당 */
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncHashJoinCost ),
                                            (void**)&sSwitchHashJoinCost,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                aCBOptParamInfo->mLeftNode = sRightNode;
                aCBOptParamInfo->mRightNode = sLeftNode;
                aCBOptParamInfo->mJoinCond = sInnerJoinTableNode->mJoinCondition;
                aCBOptParamInfo->mFilter = sInnerJoinTableNode->mJoinFilter;
                aCBOptParamInfo->mCost = (qlncCostCommon*)sSwitchHashJoinCost;

                STL_TRY( qlncCBOptHashInnerJoinCost( aCBOptParamInfo,
                                                     sExpectedJoinResultRowCount,
                                                     aEnv )
                         == STL_SUCCESS );

                if( QLNC_LESS_THAN_DOUBLE( sSwitchHashJoinCost->mCostCommon.mTotalCost,
                                           sInnerJoinTableNode->mHashJoinCost->mCostCommon.mTotalCost ) )
                {
                    sInnerJoinTableNode->mHashJoinCost = sSwitchHashJoinCost;
                }
            }
        }

        if( (sInnerJoinTableNode->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sInnerJoinTableNode->mHashJoinCost->mCostCommon.mTotalCost,
                                    sInnerJoinTableNode->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sInnerJoinTableNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_HASH;
            sInnerJoinTableNode->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sInnerJoinTableNode->mHashJoinCost);
        }
    }
    else
    {
        sInnerJoinTableNode->mHashJoinCost = NULL;
    }

    /* Trace Log 정보 출력 */
    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        STL_TRY( qlncTraceCBOptJoinTableName(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     aNodeIdx,
                     aInnerJoinNodeInfoArr[aNodeIdx].mNode,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceCBOptNestedJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     sNestedJoinCost,
                     sSwitchNestedJoinCost,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceCBOptMergeJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     sMergeJoinCost,
                     sSwitchMergeJoinCost,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceCBOptHashJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     sHashJoinCost,
                     sSwitchHashJoinCost,
                     aEnv )
                 == STL_SUCCESS );

        if( sPossibleJoinCombine == STL_TRUE )
        {
            (void)qllTraceString( "Join Combine\n"
                                  "---------------------------------\n", aEnv );
        }
    }

    /* Join Combine */
    if( sPossibleJoinCombine == STL_TRUE )
    {
        /* Join Combine Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncJoinCombineCost ),
                                    (void**)&sJoinCombineCost,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Inner Join Combine Cost 계산 */
        aCBOptParamInfo->mLeftNode = sLeftNode;
        aCBOptParamInfo->mRightNode = sRightNode;
        aCBOptParamInfo->mJoinCond = sJoinCombineAndFilter;
        aCBOptParamInfo->mFilter = NULL;
        aCBOptParamInfo->mCost = (qlncCostCommon*)sJoinCombineCost;
        STL_TRY( qlncCBOptInnerJoinCombineCost( aCBOptParamInfo,
                                                aInnerJoinNodeInfoArr,
                                                aNodeCount,
                                                aNodeIdx,
                                                sExpectedJoinResultRowCount,
                                                aEnv )
                 == STL_SUCCESS );
        sInnerJoinTableNode->mJoinCombineCost = sJoinCombineCost;

        if( (sInnerJoinTableNode->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sInnerJoinTableNode->mJoinCombineCost->mCostCommon.mTotalCost,
                                    sInnerJoinTableNode->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sInnerJoinTableNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_JOIN_COMBINE;
            sInnerJoinTableNode->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sInnerJoinTableNode->mJoinCombineCost);
        }
    }
    else
    {
        sInnerJoinTableNode->mJoinCombineCost = NULL;
    }

    /* Trace Log 정보 출력 */
    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        if( sPossibleJoinCombine == STL_TRUE )
        {
            (void)qllTraceString( "---------------------------------\n", aEnv );
        }

        STL_TRY( qlncTraceCBOptBestJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     (qlncNodeCommon*)sInnerJoinTableNode,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* Analyzed 여부 설정 */
    sInnerJoinTableNode->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;

    /* Output 설정 */
    aCBOptParamInfo->mRightNode = (qlncNodeCommon*)sInnerJoinTableNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Outer Join Cost를 계산한다.
 * @param[in]   aCBOptParamInfo     Cost Based Optimizer Parameter Info
 * @param[in]   aJoinDirect         Join Direct
 * @param[in]   aEnv                Environment
 */
stlStatus qlncCBOptOuterJoinCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                  qlncJoinDirect          aJoinDirect,
                                  qllEnv                * aEnv )
{
    qlncOuterJoinTableNode  * sOuterJoinTableNode   = NULL;

    stlBool                   sPossibleNestedJoin       = STL_TRUE;
    stlBool                   sPossibleIndexNestedJoin  = STL_TRUE;
    stlBool                   sPossibleMergeJoin        = STL_TRUE;
    stlBool                   sPossibleHashJoin         = STL_TRUE;

    stlFloat64                sExpectedJoinResultRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* Outer Join Table Node 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOuterJoinTableNode ),
                                (void**)&sOuterJoinTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_NODE_COMMON( &(sOuterJoinTableNode->mNodeCommon),
                           *(aCBOptParamInfo->mParamInfo.mGlobalNodeID),
                           QLNC_NODE_TYPE_OUTER_JOIN_TABLE );
    (*(aCBOptParamInfo->mParamInfo.mGlobalNodeID))++;    /**< Global ID 증가 */

    if( aJoinDirect == QLNC_JOIN_DIRECT_RIGHT )
    {
        sOuterJoinTableNode->mLeftNode = aCBOptParamInfo->mRightNode;
        sOuterJoinTableNode->mRightNode = aCBOptParamInfo->mLeftNode;
        sOuterJoinTableNode->mJoinDirect = QLNC_JOIN_DIRECT_LEFT;
    }
    else
    {
        sOuterJoinTableNode->mLeftNode = aCBOptParamInfo->mLeftNode;
        sOuterJoinTableNode->mRightNode = aCBOptParamInfo->mRightNode;
        sOuterJoinTableNode->mJoinDirect = aJoinDirect;
    }

    sOuterJoinTableNode->mJoinCondition = aCBOptParamInfo->mJoinCond;
    sOuterJoinTableNode->mJoinFilter = aCBOptParamInfo->mFilter;

    /* Query Timeout 체크 */
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Trace Log 정보 출력 */
    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        qlncInnerJoinNodeInfo     sInnerJoinNodeInfoArr[2];

        sInnerJoinNodeInfoArr[0].mNode = sOuterJoinTableNode->mLeftNode;
        sInnerJoinNodeInfoArr[0].mAnalyzedNode = NULL;
        sInnerJoinNodeInfoArr[0].mIsFixed = STL_FALSE;
        sInnerJoinNodeInfoArr[0].mIsAnalyzed = STL_FALSE;

        sInnerJoinNodeInfoArr[1].mNode = sOuterJoinTableNode->mRightNode;
        sInnerJoinNodeInfoArr[1].mAnalyzedNode = NULL;
        sInnerJoinNodeInfoArr[1].mIsFixed = STL_FALSE;
        sInnerJoinNodeInfoArr[1].mIsAnalyzed = STL_FALSE;

        /* Join Order */
        STL_TRY( qlncTraceCBOptJoinOrder(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     1,
                     2,
                     sInnerJoinNodeInfoArr,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* 가능한 Join Method 찾기 */
    STL_TRY( qlncFindPossibleJoinOperation( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                            sOuterJoinTableNode->mJoinCondition,
                                            QLNC_JOIN_TYPE_OUTER,
                                            sOuterJoinTableNode->mLeftNode,
                                            sOuterJoinTableNode->mRightNode,
                                            STL_FALSE,
                                            &sPossibleNestedJoin,
                                            &sPossibleIndexNestedJoin,
                                            NULL,
                                            &sPossibleMergeJoin,
                                            &sPossibleHashJoin,
                                            NULL,
                                            aEnv )
             == STL_SUCCESS );

    /**
     * Join Cost 계산
     */

    /* Expected Join Result Row Count 계산 */
    STL_TRY( qlncGetExpectedOuterJoinResultRowCount( aCBOptParamInfo,
                                                     sOuterJoinTableNode->mJoinDirect,
                                                     &sExpectedJoinResultRowCount,
                                                     aEnv )
             == STL_SUCCESS );

    if( sPossibleNestedJoin == STL_TRUE )
    {
        /* Nested Join Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncNestedJoinCost ),
                                    (void**)&(sOuterJoinTableNode->mNestedJoinCost),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Nested Outer Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = sOuterJoinTableNode->mLeftNode;
        aCBOptParamInfo->mRightNode = sOuterJoinTableNode->mRightNode;
        aCBOptParamInfo->mJoinCond = sOuterJoinTableNode->mJoinCondition;
        aCBOptParamInfo->mFilter = sOuterJoinTableNode->mJoinFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(sOuterJoinTableNode->mNestedJoinCost);
        STL_TRY( qlncCBOptNestedOuterJoinCost( aCBOptParamInfo,
                                               sOuterJoinTableNode->mJoinDirect,
                                               sExpectedJoinResultRowCount,
                                               aEnv )
                 == STL_SUCCESS );

        if( (sOuterJoinTableNode->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sOuterJoinTableNode->mNestedJoinCost->mCostCommon.mTotalCost,
                                    sOuterJoinTableNode->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sOuterJoinTableNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_NESTED_LOOPS;
            sOuterJoinTableNode->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sOuterJoinTableNode->mNestedJoinCost);
        }
    }
    else
    {
        sOuterJoinTableNode->mNestedJoinCost = NULL;
    }

    if( sPossibleMergeJoin == STL_TRUE )
    {
        /* Merge Join Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncMergeJoinCost ),
                                    (void**)&(sOuterJoinTableNode->mMergeJoinCost),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Merge Outer Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = sOuterJoinTableNode->mLeftNode;
        aCBOptParamInfo->mRightNode = sOuterJoinTableNode->mRightNode;
        aCBOptParamInfo->mJoinCond = sOuterJoinTableNode->mJoinCondition;
        aCBOptParamInfo->mFilter = sOuterJoinTableNode->mJoinFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(sOuterJoinTableNode->mMergeJoinCost);
        STL_TRY( qlncCBOptMergeOuterJoinCost( aCBOptParamInfo,
                                              sOuterJoinTableNode->mJoinDirect,
                                              sExpectedJoinResultRowCount,
                                              aEnv )
                 == STL_SUCCESS );

        if( (sOuterJoinTableNode->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sOuterJoinTableNode->mMergeJoinCost->mCostCommon.mTotalCost,
                                    sOuterJoinTableNode->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sOuterJoinTableNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_MERGE;
            sOuterJoinTableNode->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sOuterJoinTableNode->mMergeJoinCost);
        }
    }
    else
    {
        sOuterJoinTableNode->mMergeJoinCost = NULL;
    }

    if( sPossibleHashJoin == STL_TRUE )
    {
        /* Hash Join Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncHashJoinCost ),
                                    (void**)&(sOuterJoinTableNode->mHashJoinCost),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Hash Outer Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = sOuterJoinTableNode->mLeftNode;
        aCBOptParamInfo->mRightNode = sOuterJoinTableNode->mRightNode;
        aCBOptParamInfo->mJoinCond = sOuterJoinTableNode->mJoinCondition;
        aCBOptParamInfo->mFilter = sOuterJoinTableNode->mJoinFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(sOuterJoinTableNode->mHashJoinCost);
        STL_TRY( qlncCBOptHashOuterJoinCost( aCBOptParamInfo,
                                             sOuterJoinTableNode->mJoinDirect,
                                             sExpectedJoinResultRowCount,
                                             aEnv )
                 == STL_SUCCESS );

        if( (sOuterJoinTableNode->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sOuterJoinTableNode->mHashJoinCost->mCostCommon.mTotalCost,
                                    sOuterJoinTableNode->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sOuterJoinTableNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_HASH;
            sOuterJoinTableNode->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sOuterJoinTableNode->mHashJoinCost);
        }
    }
    else
    {
        sOuterJoinTableNode->mHashJoinCost = NULL;
    }

    /* Trace Log 정보 출력 */
    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        STL_TRY( qlncTraceCBOptJoinTableName(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     1,
                     sOuterJoinTableNode->mRightNode,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceCBOptNestedJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     sOuterJoinTableNode->mNestedJoinCost,
                     NULL,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceCBOptMergeJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     sOuterJoinTableNode->mMergeJoinCost,
                     NULL,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceCBOptHashJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     sOuterJoinTableNode->mHashJoinCost,
                     NULL,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceCBOptBestJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     (qlncNodeCommon*)sOuterJoinTableNode,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* Analyzed 여부 설정 */
    sOuterJoinTableNode->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;

    /* Output 설정 */
    aCBOptParamInfo->mRightNode = (qlncNodeCommon*)sOuterJoinTableNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Semi Join Cost를 계산한다.
 * @param[in]   aCBOptParamInfo     Cost Based Optimizer Parameter Info
 * @param[in]   aJoinDirect         Join Direct
 * @param[in]   aWeightValue        Weight Value
 * @param[in]   aEnv                Environment
 */
stlStatus qlncCBOptSemiJoinCost( qlncCBOptParamInfo     * aCBOptParamInfo,
                                 qlncJoinDirect           aJoinDirect,
                                 stlFloat64               aWeightValue,
                                 qllEnv                 * aEnv )
{
    qlncSemiJoinTableNode   * sSemiJoinTableNode    = NULL;

    stlBool                   sPossibleNestedJoin       = STL_TRUE;
    stlBool                   sPossibleIndexNestedJoin  = STL_TRUE;
    stlBool                   sPossibleMergeJoin        = STL_TRUE;
    stlBool                   sPossibleHashJoin         = STL_TRUE;

    stlBool                   sPossibleInvertedNestedSemiJoin;
    stlBool                   sPossibleInvertedHashSemiJoin;

    stlFloat64                sExpectedJoinResultRowCount;

    qlncNestedJoinCost      * sNestedJoinCost           = NULL;
    qlncNestedJoinCost      * sInvertedNestedJoinCost   = NULL;
    qlncHashJoinCost        * sHashJoinCost             = NULL;
    qlncHashJoinCost        * sInvertedHashJoinCost     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* Semi Join Table Node 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncSemiJoinTableNode ),
                                (void**)&sSemiJoinTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_NODE_COMMON( &(sSemiJoinTableNode->mNodeCommon),
                           *(aCBOptParamInfo->mParamInfo.mGlobalNodeID),
                           QLNC_NODE_TYPE_SEMI_JOIN_TABLE );
    (*(aCBOptParamInfo->mParamInfo.mGlobalNodeID))++;    /**< Global ID 증가 */

    if( aJoinDirect == QLNC_JOIN_DIRECT_RIGHT )
    {
        sSemiJoinTableNode->mLeftNode = aCBOptParamInfo->mRightNode;
        sSemiJoinTableNode->mRightNode = aCBOptParamInfo->mLeftNode;
        sSemiJoinTableNode->mJoinDirect = QLNC_JOIN_DIRECT_LEFT;
    }
    else
    {
        sSemiJoinTableNode->mLeftNode = aCBOptParamInfo->mLeftNode;
        sSemiJoinTableNode->mRightNode = aCBOptParamInfo->mRightNode;
        sSemiJoinTableNode->mJoinDirect = aJoinDirect;
    }

    sSemiJoinTableNode->mJoinCondition = aCBOptParamInfo->mJoinCond;
    sSemiJoinTableNode->mJoinFilter = aCBOptParamInfo->mFilter;

    /* Query Timeout 체크 */
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Trace Log 정보 출력 */
    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        qlncInnerJoinNodeInfo     sInnerJoinNodeInfoArr[2];

        sInnerJoinNodeInfoArr[0].mNode = sSemiJoinTableNode->mLeftNode;
        sInnerJoinNodeInfoArr[0].mAnalyzedNode = NULL;
        sInnerJoinNodeInfoArr[0].mIsFixed = STL_FALSE;
        sInnerJoinNodeInfoArr[0].mIsAnalyzed = STL_FALSE;

        sInnerJoinNodeInfoArr[1].mNode = sSemiJoinTableNode->mRightNode;
        sInnerJoinNodeInfoArr[1].mAnalyzedNode = NULL;
        sInnerJoinNodeInfoArr[1].mIsFixed = STL_FALSE;
        sInnerJoinNodeInfoArr[1].mIsAnalyzed = STL_FALSE;

        /* Join Order */
        STL_TRY( qlncTraceCBOptJoinOrder(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     1,
                     2,
                     sInnerJoinNodeInfoArr,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* 가능한 Join Method 찾기 */
    STL_TRY( qlncFindPossibleJoinOperation( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                            sSemiJoinTableNode->mJoinCondition,
                                            QLNC_JOIN_TYPE_SEMI,
                                            sSemiJoinTableNode->mLeftNode,
                                            sSemiJoinTableNode->mRightNode,
                                            STL_TRUE,
                                            &sPossibleNestedJoin,
                                            &sPossibleIndexNestedJoin,
                                            &sPossibleInvertedNestedSemiJoin,
                                            &sPossibleMergeJoin,
                                            &sPossibleHashJoin,
                                            &sPossibleInvertedHashSemiJoin,
                                            aEnv )
             == STL_SUCCESS );

    if( sPossibleInvertedNestedSemiJoin == STL_TRUE )
    {
        STL_TRY( qlncIsPossibleInvertedNestedSemiJoin( &(aCBOptParamInfo->mParamInfo),
                                                       sSemiJoinTableNode,
                                                       &sPossibleInvertedNestedSemiJoin,
                                                       aEnv )
                 == STL_SUCCESS );

        if( sPossibleInvertedNestedSemiJoin == STL_FALSE )
        {
            if( (sPossibleNestedJoin == STL_FALSE) &&
                (sPossibleMergeJoin == STL_FALSE) &&
                (sPossibleHashJoin == STL_FALSE) )
            {
                sPossibleNestedJoin = STL_TRUE;
            }
        }
    }


    /**
     * Join Cost 계산
     */

    /* Expected Join Result Row Count 계산 */
    STL_TRY( qlncGetExpectedSemiJoinResultRowCount( aCBOptParamInfo,
                                                    sSemiJoinTableNode->mJoinDirect,
                                                    &sExpectedJoinResultRowCount,
                                                    aEnv )
             == STL_SUCCESS );

    if( sPossibleNestedJoin == STL_TRUE )
    {
        /* Nested Join Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncNestedJoinCost ),
                                    (void**)&(sNestedJoinCost),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Nested Semi Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = sSemiJoinTableNode->mLeftNode;
        aCBOptParamInfo->mRightNode = sSemiJoinTableNode->mRightNode;
        aCBOptParamInfo->mJoinCond = sSemiJoinTableNode->mJoinCondition;
        aCBOptParamInfo->mFilter = sSemiJoinTableNode->mJoinFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)sNestedJoinCost;

        STL_TRY( qlncCBOptNestedSemiJoinCost( aCBOptParamInfo,
                                              sSemiJoinTableNode->mJoinDirect,
                                              STL_FALSE,
                                              sExpectedJoinResultRowCount,
                                              aEnv )
                 == STL_SUCCESS );

        /* Weight Value에 따른 Semi Join Cost 재조정 */
        QLNC_ADJUST_SEMI_JOIN_COST_BY_WEIGHT_VALUE( (qlncCostCommon*)sNestedJoinCost,
                                                    aWeightValue );

        sSemiJoinTableNode->mNestedJoinCost = sNestedJoinCost;

        if( (sSemiJoinTableNode->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sSemiJoinTableNode->mNestedJoinCost->mCostCommon.mTotalCost,
                                    sSemiJoinTableNode->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sSemiJoinTableNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_NESTED_LOOPS;
            sSemiJoinTableNode->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sSemiJoinTableNode->mNestedJoinCost);
        }
    }
    else
    {
        sSemiJoinTableNode->mNestedJoinCost = NULL;
    }

    if( sPossibleInvertedNestedSemiJoin == STL_TRUE )
    {
        /* Inverted Nested Join Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncNestedJoinCost ),
                                    (void**)&(sInvertedNestedJoinCost),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Inverted Nested Semi Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = sSemiJoinTableNode->mLeftNode;
        aCBOptParamInfo->mRightNode = sSemiJoinTableNode->mRightNode;
        aCBOptParamInfo->mJoinCond = sSemiJoinTableNode->mJoinCondition;
        aCBOptParamInfo->mFilter = sSemiJoinTableNode->mJoinFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(sInvertedNestedJoinCost);

        STL_TRY( qlncCBOptInvertedNestedSemiJoinCost( aCBOptParamInfo,
                                                      sSemiJoinTableNode->mJoinDirect,
                                                      sExpectedJoinResultRowCount,
                                                      aEnv )
                 == STL_SUCCESS );

        /* Weight Value에 따른 Semi Join Cost 재조정 */
        QLNC_ADJUST_SEMI_JOIN_COST_BY_WEIGHT_VALUE( (qlncCostCommon*)sInvertedNestedJoinCost,
                                                    aWeightValue );

        if( (sSemiJoinTableNode->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sInvertedNestedJoinCost->mCostCommon.mTotalCost,
                                    sSemiJoinTableNode->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sSemiJoinTableNode->mNestedJoinCost = sInvertedNestedJoinCost;
            sSemiJoinTableNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_NESTED_LOOPS;
            sSemiJoinTableNode->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sSemiJoinTableNode->mNestedJoinCost);
        }
    }

    if( sPossibleMergeJoin == STL_TRUE )
    {
        /* Merge Join Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncMergeJoinCost ),
                                    (void**)&(sSemiJoinTableNode->mMergeJoinCost),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Merge Semi Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = sSemiJoinTableNode->mLeftNode;
        aCBOptParamInfo->mRightNode = sSemiJoinTableNode->mRightNode;
        aCBOptParamInfo->mJoinCond = sSemiJoinTableNode->mJoinCondition;
        aCBOptParamInfo->mFilter = sSemiJoinTableNode->mJoinFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(sSemiJoinTableNode->mMergeJoinCost);

        STL_TRY( qlncCBOptMergeSemiJoinCost( aCBOptParamInfo,
                                             sSemiJoinTableNode->mJoinDirect,
                                             sExpectedJoinResultRowCount,
                                             aEnv )
                 == STL_SUCCESS );

        /* Weight Value에 따른 Semi Join Cost 재조정 */
        QLNC_ADJUST_SEMI_JOIN_COST_BY_WEIGHT_VALUE( (qlncCostCommon*)(sSemiJoinTableNode->mMergeJoinCost),
                                                    aWeightValue );

        if( (sSemiJoinTableNode->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sSemiJoinTableNode->mMergeJoinCost->mCostCommon.mTotalCost,
                                    sSemiJoinTableNode->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sSemiJoinTableNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_MERGE;
            sSemiJoinTableNode->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sSemiJoinTableNode->mMergeJoinCost);
        }
    }
    else
    {
        sSemiJoinTableNode->mMergeJoinCost = NULL;
    }

    if( sPossibleHashJoin == STL_TRUE )
    {
        /* Hash Join Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncHashJoinCost ),
                                    (void**)&(sHashJoinCost),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Hash Semi Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = sSemiJoinTableNode->mLeftNode;
        aCBOptParamInfo->mRightNode = sSemiJoinTableNode->mRightNode;
        aCBOptParamInfo->mJoinCond = sSemiJoinTableNode->mJoinCondition;
        aCBOptParamInfo->mFilter = sSemiJoinTableNode->mJoinFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)sHashJoinCost;

        STL_TRY( qlncCBOptHashSemiJoinCost( aCBOptParamInfo,
                                            sSemiJoinTableNode->mJoinDirect,
                                            sExpectedJoinResultRowCount,
                                            aEnv )
                 == STL_SUCCESS );

        /* Weight Value에 따른 Semi Join Cost 재조정 */
        QLNC_ADJUST_SEMI_JOIN_COST_BY_WEIGHT_VALUE( (qlncCostCommon*)sHashJoinCost,
                                                    aWeightValue );

        sSemiJoinTableNode->mHashJoinCost = sHashJoinCost;

        if( (sSemiJoinTableNode->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sSemiJoinTableNode->mHashJoinCost->mCostCommon.mTotalCost,
                                    sSemiJoinTableNode->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sSemiJoinTableNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_HASH;
            sSemiJoinTableNode->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sSemiJoinTableNode->mHashJoinCost);
        }
    }
    else
    {
        sSemiJoinTableNode->mHashJoinCost = NULL;
    }

    if( sPossibleInvertedHashSemiJoin == STL_TRUE )
    {
        /* Inverted Hash Join Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncHashJoinCost ),
                                    (void**)&(sInvertedHashJoinCost),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Inverted Hash Semi Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = sSemiJoinTableNode->mLeftNode;
        aCBOptParamInfo->mRightNode = sSemiJoinTableNode->mRightNode;
        aCBOptParamInfo->mJoinCond = sSemiJoinTableNode->mJoinCondition;
        aCBOptParamInfo->mFilter = sSemiJoinTableNode->mJoinFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(sInvertedHashJoinCost);

        STL_TRY( qlncCBOptInvertedHashSemiJoinCost( aCBOptParamInfo,
                                                    sSemiJoinTableNode->mJoinDirect,
                                                    sExpectedJoinResultRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        /* Weight Value에 따른 Semi Join Cost 재조정 */
        QLNC_ADJUST_SEMI_JOIN_COST_BY_WEIGHT_VALUE( (qlncCostCommon*)sInvertedHashJoinCost,
                                                    aWeightValue );

        if( (sSemiJoinTableNode->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sInvertedHashJoinCost->mCostCommon.mTotalCost,
                                    sSemiJoinTableNode->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sSemiJoinTableNode->mHashJoinCost = sInvertedHashJoinCost;
            sSemiJoinTableNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_HASH;
            sSemiJoinTableNode->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sSemiJoinTableNode->mHashJoinCost);
        }
    }

    /* Trace Log 정보 출력 */
    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        STL_TRY( qlncTraceCBOptJoinTableName(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     1,
                     sSemiJoinTableNode->mRightNode,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceCBOptNestedJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     sNestedJoinCost,
                     sInvertedNestedJoinCost,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceCBOptMergeJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     sSemiJoinTableNode->mMergeJoinCost,
                     NULL,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceCBOptHashJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     sHashJoinCost,
                     sInvertedHashJoinCost,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceCBOptBestJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     (qlncNodeCommon*)sSemiJoinTableNode,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* Analyzed 여부 설정 */
    sSemiJoinTableNode->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;

    /* Output 설정 */
    aCBOptParamInfo->mRightNode = (qlncNodeCommon*)sSemiJoinTableNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Anti Semi Join Cost를 계산한다.
 * @param[in]   aCBOptParamInfo     Cost Based Optimizer Parameter Info
 * @param[in]   aJoinDirect         Join Direct
 * @param[in]   aWeightValue        Weight Value
 * @param[in]   aEnv                Environment
 */
stlStatus qlncCBOptAntiSemiJoinCost( qlncCBOptParamInfo * aCBOptParamInfo,
                                     qlncJoinDirect       aJoinDirect,
                                     stlFloat64           aWeightValue,
                                     qllEnv             * aEnv )
{
    qlncAntiSemiJoinTableNode   * sAntiSemiJoinTableNode    = NULL;

    stlBool                       sPossibleNestedJoin       = STL_TRUE;
    stlBool                       sPossibleIndexNestedJoin  = STL_TRUE;
    stlBool                       sPossibleMergeJoin        = STL_TRUE;
    stlBool                       sPossibleHashJoin         = STL_TRUE;

    stlFloat64                    sExpectedJoinResultRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* Anti Semi Join Table Node 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncAntiSemiJoinTableNode ),
                                (void**)&sAntiSemiJoinTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_NODE_COMMON( &(sAntiSemiJoinTableNode->mNodeCommon),
                           *(aCBOptParamInfo->mParamInfo.mGlobalNodeID),
                           QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE );
    (*(aCBOptParamInfo->mParamInfo.mGlobalNodeID))++;    /**< Global ID 증가 */

    if( aJoinDirect == QLNC_JOIN_DIRECT_RIGHT )
    {
        sAntiSemiJoinTableNode->mLeftNode = aCBOptParamInfo->mRightNode;
        sAntiSemiJoinTableNode->mRightNode = aCBOptParamInfo->mLeftNode;
        sAntiSemiJoinTableNode->mJoinDirect = QLNC_JOIN_DIRECT_LEFT;
    }
    else
    {
        sAntiSemiJoinTableNode->mLeftNode = aCBOptParamInfo->mLeftNode;
        sAntiSemiJoinTableNode->mRightNode = aCBOptParamInfo->mRightNode;
        sAntiSemiJoinTableNode->mJoinDirect = aJoinDirect;
    }

    sAntiSemiJoinTableNode->mJoinCondition = aCBOptParamInfo->mJoinCond;
    sAntiSemiJoinTableNode->mJoinFilter = aCBOptParamInfo->mFilter;

    /* Query Timeout 체크 */
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Trace Log 정보 출력 */
    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        qlncInnerJoinNodeInfo     sInnerJoinNodeInfoArr[2];

        sInnerJoinNodeInfoArr[0].mNode = sAntiSemiJoinTableNode->mLeftNode;
        sInnerJoinNodeInfoArr[0].mAnalyzedNode = NULL;
        sInnerJoinNodeInfoArr[0].mIsFixed = STL_FALSE;
        sInnerJoinNodeInfoArr[0].mIsAnalyzed = STL_FALSE;

        sInnerJoinNodeInfoArr[1].mNode = sAntiSemiJoinTableNode->mRightNode;
        sInnerJoinNodeInfoArr[1].mAnalyzedNode = NULL;
        sInnerJoinNodeInfoArr[1].mIsFixed = STL_FALSE;
        sInnerJoinNodeInfoArr[1].mIsAnalyzed = STL_FALSE;

        /* Join Order */
        STL_TRY( qlncTraceCBOptJoinOrder(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     1,
                     2,
                     sInnerJoinNodeInfoArr,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* 가능한 Join Method 찾기 */
    STL_TRY( qlncFindPossibleJoinOperation( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                            sAntiSemiJoinTableNode->mJoinCondition,
                                            QLNC_JOIN_TYPE_ANTI_SEMI,
                                            sAntiSemiJoinTableNode->mLeftNode,
                                            sAntiSemiJoinTableNode->mRightNode,
                                            STL_TRUE,
                                            &sPossibleNestedJoin,
                                            &sPossibleIndexNestedJoin,
                                            NULL,
                                            &sPossibleMergeJoin,
                                            &sPossibleHashJoin,
                                            NULL,
                                            aEnv )
             == STL_SUCCESS );

    /**
     * Join Cost 계산
     */

    /* Expected Join Result Row Count 계산 */
    STL_TRY( qlncGetExpectedAntiSemiJoinResultRowCount( aCBOptParamInfo,
                                                        sAntiSemiJoinTableNode->mJoinDirect,
                                                        &sExpectedJoinResultRowCount,
                                                        aEnv )
             == STL_SUCCESS );

    if( sPossibleNestedJoin == STL_TRUE )
    {
        /* Nested Join Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncNestedJoinCost ),
                                    (void**)&(sAntiSemiJoinTableNode->mNestedJoinCost),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Nested Anti Semi Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = sAntiSemiJoinTableNode->mLeftNode;
        aCBOptParamInfo->mRightNode = sAntiSemiJoinTableNode->mRightNode;
        aCBOptParamInfo->mJoinCond = sAntiSemiJoinTableNode->mJoinCondition;
        aCBOptParamInfo->mFilter = sAntiSemiJoinTableNode->mJoinFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(sAntiSemiJoinTableNode->mNestedJoinCost);

        /* Anti Semi Join의 Cost 계산은 Semi Join Cost와 동일하다. */
        STL_TRY( qlncCBOptNestedSemiJoinCost( aCBOptParamInfo,
                                              sAntiSemiJoinTableNode->mJoinDirect,
                                              STL_TRUE,
                                              sExpectedJoinResultRowCount,
                                              aEnv )
                 == STL_SUCCESS );

        /* Weight Value에 따른 Semi Join Cost 재조정 */
        QLNC_ADJUST_SEMI_JOIN_COST_BY_WEIGHT_VALUE( (qlncCostCommon*)(sAntiSemiJoinTableNode->mNestedJoinCost),
                                                    aWeightValue );

        if( (sAntiSemiJoinTableNode->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sAntiSemiJoinTableNode->mNestedJoinCost->mCostCommon.mTotalCost,
                                    sAntiSemiJoinTableNode->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sAntiSemiJoinTableNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_NESTED_LOOPS;
            sAntiSemiJoinTableNode->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sAntiSemiJoinTableNode->mNestedJoinCost);
        }
    }
    else
    {
        sAntiSemiJoinTableNode->mNestedJoinCost = NULL;
    }

    if( sPossibleMergeJoin == STL_TRUE )
    {
        /* Merge Join Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncMergeJoinCost ),
                                    (void**)&(sAntiSemiJoinTableNode->mMergeJoinCost),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Merge Anti Semi Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = sAntiSemiJoinTableNode->mLeftNode;
        aCBOptParamInfo->mRightNode = sAntiSemiJoinTableNode->mRightNode;
        aCBOptParamInfo->mJoinCond = sAntiSemiJoinTableNode->mJoinCondition;
        aCBOptParamInfo->mFilter = sAntiSemiJoinTableNode->mJoinFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(sAntiSemiJoinTableNode->mMergeJoinCost);

        /* Anti Semi Join의 Cost 계산은 Semi Join Cost와 동일하다. */
        STL_TRY( qlncCBOptMergeSemiJoinCost( aCBOptParamInfo,
                                             sAntiSemiJoinTableNode->mJoinDirect,
                                             sExpectedJoinResultRowCount,
                                             aEnv )
                 == STL_SUCCESS );

        /* Weight Value에 따른 Semi Join Cost 재조정 */
        QLNC_ADJUST_SEMI_JOIN_COST_BY_WEIGHT_VALUE( (qlncCostCommon*)(sAntiSemiJoinTableNode->mMergeJoinCost),
                                                    aWeightValue );

        if( (sAntiSemiJoinTableNode->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sAntiSemiJoinTableNode->mMergeJoinCost->mCostCommon.mTotalCost,
                                    sAntiSemiJoinTableNode->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sAntiSemiJoinTableNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_MERGE;
            sAntiSemiJoinTableNode->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sAntiSemiJoinTableNode->mMergeJoinCost);
        }
    }
    else
    {
        sAntiSemiJoinTableNode->mMergeJoinCost = NULL;
    }

    if( sPossibleHashJoin == STL_TRUE )
    {
        /* Hash Join Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncHashJoinCost ),
                                    (void**)&(sAntiSemiJoinTableNode->mHashJoinCost),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Hash Anti Semi Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = sAntiSemiJoinTableNode->mLeftNode;
        aCBOptParamInfo->mRightNode = sAntiSemiJoinTableNode->mRightNode;
        aCBOptParamInfo->mJoinCond = sAntiSemiJoinTableNode->mJoinCondition;
        aCBOptParamInfo->mFilter = sAntiSemiJoinTableNode->mJoinFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(sAntiSemiJoinTableNode->mHashJoinCost);

        /* Anti Semi Join의 Cost 계산은 Semi Join Cost와 동일하다. */
        STL_TRY( qlncCBOptHashSemiJoinCost( aCBOptParamInfo,
                                            sAntiSemiJoinTableNode->mJoinDirect,
                                            sExpectedJoinResultRowCount,
                                            aEnv )
                 == STL_SUCCESS );

        /* Weight Value에 따른 Semi Join Cost 재조정 */
        QLNC_ADJUST_SEMI_JOIN_COST_BY_WEIGHT_VALUE( (qlncCostCommon*)(sAntiSemiJoinTableNode->mHashJoinCost),
                                                    aWeightValue );

        if( (sAntiSemiJoinTableNode->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_NA) ||
            (QLNC_LESS_THAN_DOUBLE( sAntiSemiJoinTableNode->mHashJoinCost->mCostCommon.mTotalCost,
                                    sAntiSemiJoinTableNode->mNodeCommon.mBestCost->mTotalCost )) )
        {
            sAntiSemiJoinTableNode->mNodeCommon.mBestOptType = QLNC_BEST_OPT_TYPE_HASH;
            sAntiSemiJoinTableNode->mNodeCommon.mBestCost =
                (qlncCostCommon*)(sAntiSemiJoinTableNode->mHashJoinCost);
        }
    }
    else
    {
        sAntiSemiJoinTableNode->mHashJoinCost = NULL;
    }

    /* Trace Log 정보 출력 */
    if( QLL_IS_ENABLE_TRACE_LOGGER(aEnv) )
    {
        STL_TRY( qlncTraceCBOptJoinTableName(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     1,
                     sAntiSemiJoinTableNode->mRightNode,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceCBOptNestedJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     sAntiSemiJoinTableNode->mNestedJoinCost,
                     NULL,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceCBOptMergeJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     sAntiSemiJoinTableNode->mMergeJoinCost,
                     NULL,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceCBOptHashJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     sAntiSemiJoinTableNode->mHashJoinCost,
                     NULL,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceCBOptBestJoinCost(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     (qlncNodeCommon*)sAntiSemiJoinTableNode,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* Analyzed 여부 설정 */
    sAntiSemiJoinTableNode->mNodeCommon.mIsAnalyzedCBO = STL_TRUE;

    /* Output 설정 */
    aCBOptParamInfo->mRightNode = (qlncNodeCommon*)sAntiSemiJoinTableNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Nested Inner Join Cost를 계산한다.
 * @param[in]   aCBOptParamInfo             Cost Based Optimizer Parameter Info
 * @param[in]   aExpectedResultRowCount     Expected Result Row Count
 * @param[in]   aPossibleNestedJoin         Nested Join 가능 여부
 * @param[in]   aPossibleIndexNestedJoin    Index Nested Join 가능 여부
 * @param[in]   aEnv                        Environment
 */
stlStatus qlncCBOptNestedInnerJoinCost( qlncCBOptParamInfo  * aCBOptParamInfo,
                                        stlFloat64            aExpectedResultRowCount,
                                        stlBool               aPossibleNestedJoin,
                                        stlBool               aPossibleIndexNestedJoin,
                                        qllEnv              * aEnv )
{
    qlncNodeCommon      * sLeftNode         = NULL;
    qlncNodeCommon      * sRightNode        = NULL;
    qlncNodeCommon      * sSortInstantNode  = NULL;

    qlncAndFilter       * sJoinCondition    = NULL;
    qlncAndFilter       * sJoinFilter       = NULL;

    qlncNestedJoinCost  * sNestedJoinCost   = NULL;

    smlSortTableAttr      sSortTableAttr    = { STL_FALSE,     /* not top-down */
                                                STL_FALSE,     /* not volatile */
                                                STL_FALSE,     /* not leaf-only */
                                                STL_FALSE,     /* not unique */
                                                STL_FALSE };   /* not null excluded */

    stlFloat64            sFilterCost;
    stlFloat64            sBuildCost;
    stlFloat64            sReadIOCost;
    stlFloat64            sOutputIOCost;
    stlFloat64            sRowCount;
    stlFloat64            sTmpFilterCost;
    stlFloat64            sPhysicalRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mRightNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Nested Join Cost 설정 */
    sNestedJoinCost = (qlncNestedJoinCost*)(aCBOptParamInfo->mCost);
    sNestedJoinCost->mCopiedBaseNode = NULL;

    /* Left Node 설정 */
    sLeftNode = aCBOptParamInfo->mLeftNode;

    /* Join Condition 및 Join Filter 설정 */
    sJoinCondition = aCBOptParamInfo->mJoinCond;
    sJoinFilter = aCBOptParamInfo->mFilter;

    /* Join Condition에 의한 Right Node 설정 */
    if( sJoinCondition == NULL )
    {
        sRightNode = aCBOptParamInfo->mRightNode;
    }
    else
    {
        /* Join Condition 설정 */
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sJoinCondition,
                                    &sJoinCondition,
                                    aEnv )
                 == STL_SUCCESS );

        if( aPossibleIndexNestedJoin == STL_TRUE )
        {
            STL_TRY( qlncCBOptMakeSortInstantForNestedLoops( &(aCBOptParamInfo->mParamInfo),
                                                             &sSortTableAttr,
                                                             aCBOptParamInfo->mRightNode,
                                                             sJoinCondition,
                                                             STL_TRUE,
                                                             NULL,
                                                             &sSortInstantNode,
                                                             aEnv )
                     == STL_SUCCESS );

            /* Sort Instant Cost 계산 */
            if( sSortInstantNode != NULL )
            {
                aCBOptParamInfo->mLeftNode = sSortInstantNode;
                STL_TRY( qlncCBOptSortInstant( aCBOptParamInfo,
                                               aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                aPossibleNestedJoin = STL_TRUE;
            }
        }
        else
        {
            sSortInstantNode = NULL;
            STL_DASSERT( aPossibleNestedJoin == STL_TRUE );
        }

        if( aPossibleNestedJoin == STL_TRUE )
        {
            if( aCBOptParamInfo->mRightNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
            {
                /* Base Table Node 복사 */
                STL_TRY( qlncCopyBaseNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                         aCBOptParamInfo->mRightNode,
                                                         &sRightNode,
                                                         aEnv )
                         == STL_SUCCESS );

                /* Push Filter 설정 */
                QLNC_SET_PUSH_FILTER( &(aCBOptParamInfo->mParamInfo),
                                      ((qlncBaseTableNode*)sRightNode)->mFilter,
                                      sJoinCondition,
                                      aEnv );
                sJoinCondition = NULL;

                /* Base Table Node Cost 계산 */
                aCBOptParamInfo->mLeftNode = sRightNode;
                STL_TRY( qlncCBOptCopiedBaseTable( aCBOptParamInfo,
                                                   aEnv )
                         == STL_SUCCESS );

                sNestedJoinCost->mCopiedBaseNode = sRightNode;

                if( sSortInstantNode != NULL )
                {
                    if( QLNC_LESS_THAN_DOUBLE( sSortInstantNode->mBestCost->mTotalCost,
                                               sRightNode->mBestCost->mTotalCost ) )
                    {
                        sRightNode = sSortInstantNode;
                    }
                }
            }
            else if( aCBOptParamInfo->mRightNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
            {
                /* Sub Table Node 복사 */
                STL_TRY( qlncCopySubNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                        aCBOptParamInfo->mRightNode,
                                                        &sRightNode,
                                                        aEnv )
                         == STL_SUCCESS );

                /* Push Filter 설정 */
                QLNC_SET_PUSH_FILTER( &(aCBOptParamInfo->mParamInfo),
                                      ((qlncSubTableNode*)sRightNode)->mFilter,
                                      sJoinCondition,
                                      aEnv );
                sJoinCondition = NULL;

                /* Sub Table Node Cost 계산 */
                aCBOptParamInfo->mLeftNode = sRightNode;
                STL_TRY( qlncCBOptSubTable( aCBOptParamInfo,
                                            aEnv )
                         == STL_SUCCESS );
                /* Sub Table은 자체로 Materialized가 가능하면 하므로 별도로 Instant를 처리할 필요 없음 */
            }
            else
            {
                sRightNode = aCBOptParamInfo->mRightNode;

                if( sSortInstantNode != NULL )
                {
                    if( QLNC_LESS_THAN_DOUBLE( sSortInstantNode->mBestCost->mTotalCost,
                                               sRightNode->mBestCost->mTotalCost ) )
                    {
                        /* @todo 여기에 들어오는 테스트 케이스를 만들기 어려움 */
                        sRightNode = sSortInstantNode;
                    }
                }
            }
        }
        else
        {
            STL_DASSERT( sSortInstantNode != NULL );
            sRightNode = sSortInstantNode;
            sJoinCondition = NULL;
        }
    }

    /* Join Filter 설정 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sJoinFilter,
                                    &sJoinFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /* Join And Filter 재정렬 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sJoinFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Nested Join Cost 계산
     */

    QLNC_SET_BASIC_NL_COST( sLeftNode,
                            sRightNode,
                            sFilterCost,
                            sBuildCost,
                            sReadIOCost,
                            sOutputIOCost );

    sRowCount = 
        ( sLeftNode->mBestCost->mResultRowCount *
          sRightNode->mBestCost->mResultRowCount );

    /* Join Condition Cost */
    if( sJoinCondition == NULL )
    {
        sTmpFilterCost = (stlFloat64)0.0;
    }
    else
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sJoinCondition,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    sFilterCost += sTmpFilterCost;

    /* Join Filter Cost */
    if( sJoinFilter == NULL )
    {
        sTmpFilterCost = (stlFloat64)0.0;
    }
    else
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sJoinFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    sFilterCost += sTmpFilterCost;


    /* Nested Join Cost 정보 설정 */
    sNestedJoinCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sNestedJoinCost->mCostCommon.mNextFilterCost = sFilterCost + QLNC_OUTPUT_COST_PER_ROW;
    sNestedJoinCost->mCostCommon.mBuildCost = sBuildCost;
    sNestedJoinCost->mCostCommon.mReadIOCost = sReadIOCost;
    sNestedJoinCost->mCostCommon.mOutputIOCost = sOutputIOCost;

    QLNC_SET_TOTAL_COST( &(sNestedJoinCost->mCostCommon) );
    sNestedJoinCost->mCostCommon.mResultRowCount = aExpectedResultRowCount;

    sNestedJoinCost->mLeftNode = sLeftNode;
    sNestedJoinCost->mRightNode = sRightNode;

    sNestedJoinCost->mJoinCondition = sJoinCondition;
    sNestedJoinCost->mSubQueryFilter = NULL;
    sNestedJoinCost->mJoinFilter = sJoinFilter;

    sNestedJoinCost->mIsInverted = STL_FALSE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Merge Inner Join Cost를 계산한다.
 * @param[in]   aCBOptParamInfo             Cost Based Optimizer Parameter Info
 * @param[in]   aExpectedResultRowCount     Expected Result Row Count
 * @param[in]   aEnv                        Environment
 */
stlStatus qlncCBOptMergeInnerJoinCost( qlncCBOptParamInfo   * aCBOptParamInfo,
                                       stlFloat64             aExpectedResultRowCount,
                                       qllEnv               * aEnv )
{
    stlInt32              i;

    qlncMergeJoinCost   * sMergeJoinCost        = NULL;
    qlncNodeCommon      * sLeftNode             = NULL;
    qlncNodeCommon      * sRightNode            = NULL;

    qlncAndFilter       * sEquiJoinCondition    = NULL;
    qlncAndFilter       * sNonEquiJoinCondition = NULL;
    qlncAndFilter       * sJoinFilter           = NULL;

    stlInt32              sSortKeyCount;
    qlncExprCommon     ** sLeftSortKeyArr       = NULL;
    qlncExprCommon     ** sRightSortKeyArr      = NULL;
    stlBool               sIsLeftPureColumn;
    stlBool               sIsRightPureColumn;
    qlncIndexScanInfo   * sLeftUsableIndexScanInfo  = NULL;
    qlncIndexScanInfo   * sRightUsableIndexScanInfo = NULL;

    qlncNodeCommon      * sCopiedNode           = NULL;

    stlUInt8            * sSortKeyFlags         = NULL;
    smlSortTableAttr      sSortTableAttr = { STL_FALSE,     /* not top-down */
                                             STL_FALSE,     /* not volatile */
                                             STL_TRUE,      /* leaf-only */
                                             STL_FALSE,     /* not unique */
                                             STL_FALSE };   /* not null excluded */

    ellIndexColumnOrdering    sLeftIndexOrdering;
    ellIndexColumnOrdering    sRightIndexOrdering;

    stlFloat64            sFilterCost;
    stlFloat64            sBuildCost;
    stlFloat64            sReadIOCost;
    stlFloat64            sOutputIOCost;
    stlFloat64            sRowCount;
    stlFloat64            sTmpFilterCost;
    stlFloat64            sPhysicalRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mRightNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mJoinCond != NULL, QLL_ERROR_STACK(aEnv) );


    /* Merge Join Cost 설정 */
    sMergeJoinCost = (qlncMergeJoinCost*)(aCBOptParamInfo->mCost);

    /* Node 설정 */
    sLeftNode = aCBOptParamInfo->mLeftNode;
    sRightNode = aCBOptParamInfo->mRightNode;

    /* Join Condition 분리 */
    STL_TRY( qlncSeperateJoinConditionForMergeJoin( &(aCBOptParamInfo->mParamInfo),
                                                    aCBOptParamInfo->mJoinCond,
                                                    &sEquiJoinCondition,
                                                    &sNonEquiJoinCondition,
                                                    aEnv )
             == STL_SUCCESS );

    /* Equi-Join Condition And Filter 재정렬 */
    if( sEquiJoinCondition != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sEquiJoinCondition,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Non Equi-Join Condition And Filter 재정렬 */
    if( sNonEquiJoinCondition != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sNonEquiJoinCondition,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Join Filter 설정 */
    if( aCBOptParamInfo->mFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    aCBOptParamInfo->mFilter,
                                    &sJoinFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sJoinFilter = NULL;
    }

    /* Join And Filter 재정렬 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sJoinFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    STL_DASSERT( sEquiJoinCondition != NULL );


    /**
     * Sort Key 수집
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sEquiJoinCondition->mOrCount,
                                (void**) &sLeftSortKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sEquiJoinCondition->mOrCount,
                                (void**) &sRightSortKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sSortKeyCount = sEquiJoinCondition->mOrCount;
    sIsLeftPureColumn = STL_TRUE;
    sIsRightPureColumn = STL_TRUE;
    for( i = 0; i < sEquiJoinCondition->mOrCount; i++ )
    {
        qlncBooleanFilter   * sBooleanFilter    = NULL;

        STL_DASSERT( sEquiJoinCondition->mOrFilters[i]->mFilterCount == 1 );
        sBooleanFilter = (qlncBooleanFilter*)(sEquiJoinCondition->mOrFilters[i]->mFilters[0]);
        if( qlncIsExistNode( sLeftNode,
                             sBooleanFilter->mLeftRefNodeList->mHead->mNode ) )
        {
            sLeftSortKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
            sRightSortKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
        }
        else
        {
            sLeftSortKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
            sRightSortKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
        }

        /* Sort Key가 Column으로만 구성되었는지 여부 체크 */
        if( sLeftSortKeyArr[i]->mType != QLNC_EXPR_TYPE_COLUMN )
        {
            sIsLeftPureColumn = STL_FALSE;
        }

        if( sRightSortKeyArr[i]->mType != QLNC_EXPR_TYPE_COLUMN )
        {
            sIsRightPureColumn = STL_FALSE;
        }
    }

    /* Left Node에 대해 Index Scan이 가능한지 여부 체크 */
    if( sIsLeftPureColumn == STL_TRUE )
    {
        if( (sLeftNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE) &&
            (qlncHasSubQuery( sLeftNode ) == STL_FALSE) )
        {
            (void)qlncFindIndexForSortKey( (qlncBaseTableNode*)sLeftNode,
                                           sSortKeyCount,
                                           sLeftSortKeyArr,
                                           STL_FALSE, /* Full Match */
                                           STL_FALSE, /* Only Unique */
                                           &sLeftUsableIndexScanInfo,
                                           &sLeftIndexOrdering );
        }
        else
        {
            /* Sort Instant 생성 필요 */
            sLeftUsableIndexScanInfo = NULL;
        }
    }
    else
    {
        /* Sort Instant 생성 필요 */
        sLeftUsableIndexScanInfo = NULL;
    }

    /* Right Node에 대해 Index Scan이 가능한지 여부 체크 */
    if( sIsRightPureColumn == STL_TRUE )
    {
        if( (sRightNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE) &&
            (qlncHasSubQuery( sRightNode ) == STL_FALSE) )
        {
            (void)qlncFindIndexForSortKey( (qlncBaseTableNode*)sRightNode,
                                           sSortKeyCount,
                                           sRightSortKeyArr,
                                           STL_TRUE, /* Full Match */
                                           STL_TRUE, /* Only Unique */
                                           &sRightUsableIndexScanInfo,
                                           &sRightIndexOrdering );
        }
        else
        {
            /* Sort Instant 생성 필요 */
            sRightUsableIndexScanInfo = NULL;
        }
    }
    else
    {
        /* Sort Instant 생성 필요 */
        sRightUsableIndexScanInfo = NULL;
    }


    /**
     * Sort Merge Cost 계산
     */

    /* Left Node Cost */
    if( sLeftUsableIndexScanInfo != NULL )
    {
        /* Index Scan이 가능한 경우 */

        /* Left Node 복사 */
        STL_TRY( qlncCopyBaseNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                 sLeftNode,
                                                 &sCopiedNode,
                                                 aEnv )
                 == STL_SUCCESS );

        /* Index Scan Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncIndexScanCost ),
                                    (void**) &(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        ((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostCount = 1;

        /* Index Scan Cost 계산 */
        aCBOptParamInfo->mLeftNode = sCopiedNode;
        aCBOptParamInfo->mFilter = ((qlncBaseTableNode*)sCopiedNode)->mFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr);
        STL_TRY( qlncCBOptIndexScanCost( aCBOptParamInfo,
                                         sLeftUsableIndexScanInfo,
                                         (sLeftIndexOrdering == ELL_INDEX_COLUMN_ASCENDING
                                          ? STL_TRUE : STL_FALSE),
                                         NULL,
                                         aEnv )
                 == STL_SUCCESS );
        aCBOptParamInfo->mFilter = NULL;

        sCopiedNode->mBestOptType = QLNC_BEST_OPT_TYPE_INDEX_SCAN;
        sCopiedNode->mBestCost = &(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr[0].mCostCommon);
        sCopiedNode->mIsAnalyzedCBO = STL_TRUE;

        sLeftNode = sCopiedNode;
    }
    else
    {
        /* Sort Instant를 생성해야 하는 경우 */

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( stlUInt8 ) * sSortKeyCount,
                                    (void**) &sSortKeyFlags,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sSortKeyFlags, 0x00, STL_SIZEOF( stlInt8 ) * sSortKeyCount );

        for( i = 0; i < sSortKeyCount; i++ )
        {
            DTL_SET_INDEX_COLUMN_FLAG( sSortKeyFlags[i],
                                       DTL_KEYCOLUMN_INDEX_ORDER_ASC,
                                       DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE,
                                       DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST );
        }

        STL_TRY( qlncCreateSortInstantForMergeJoin( &(aCBOptParamInfo->mParamInfo),
                                                    QLNC_INSTANT_TYPE_NORMAL,
                                                    &sSortTableAttr,
                                                    ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                                    sSortKeyCount,
                                                    sLeftSortKeyArr,
                                                    sSortKeyFlags,
                                                    sLeftNode,
                                                    &sCopiedNode,
                                                    aEnv )
                 == STL_SUCCESS );

        /* Sort Instant Cost 계산 */
        aCBOptParamInfo->mLeftNode = sCopiedNode;
        STL_TRY( qlncCBOptSortInstant( aCBOptParamInfo,
                                       aEnv )
                 == STL_SUCCESS );

        sLeftNode = sCopiedNode;
    }

    /* Right Node Cost */
    if( sRightUsableIndexScanInfo != NULL )
    {
        /* Index Scan이 가능한 경우 */

        /* Right Node 복사 */
        STL_TRY( qlncCopyBaseNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                 sRightNode,
                                                 &sCopiedNode,
                                                 aEnv )
                 == STL_SUCCESS );

        /* Index Scan Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncIndexScanCost ),
                                    (void**) &(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        ((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostCount = 1;

        /* Index Scan Cost 계산 */
        aCBOptParamInfo->mLeftNode = sCopiedNode;
        aCBOptParamInfo->mFilter = ((qlncBaseTableNode*)sCopiedNode)->mFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr);
        STL_TRY( qlncCBOptIndexScanCost( aCBOptParamInfo,
                                         sRightUsableIndexScanInfo,
                                         (sRightIndexOrdering == ELL_INDEX_COLUMN_ASCENDING
                                          ? STL_TRUE : STL_FALSE),
                                         NULL,
                                         aEnv )
                 == STL_SUCCESS );
        aCBOptParamInfo->mFilter = NULL;

        sCopiedNode->mBestOptType = QLNC_BEST_OPT_TYPE_INDEX_SCAN;
        sCopiedNode->mBestCost = &(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr[0].mCostCommon);
        sCopiedNode->mIsAnalyzedCBO = STL_TRUE;

        sRightNode = sCopiedNode;
    }
    else
    {
        /* Sort Instant를 생성해야 하는 경우 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( stlUInt8 ) * sSortKeyCount,
                                    (void**) &sSortKeyFlags,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sSortKeyFlags, 0x00, STL_SIZEOF( stlInt8 ) * sSortKeyCount );

        for( i = 0; i < sSortKeyCount; i++ )
        {
            DTL_SET_INDEX_COLUMN_FLAG( sSortKeyFlags[i],
                                       DTL_KEYCOLUMN_INDEX_ORDER_ASC,
                                       DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE,
                                       DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST );
        }

        STL_TRY( qlncCreateSortInstantForMergeJoin( &(aCBOptParamInfo->mParamInfo),
                                                    QLNC_INSTANT_TYPE_JOIN,
                                                    &sSortTableAttr,
                                                    ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                                    sSortKeyCount,
                                                    sRightSortKeyArr,
                                                    sSortKeyFlags,
                                                    sRightNode,
                                                    &sCopiedNode,
                                                    aEnv )
                 == STL_SUCCESS );

        /* Sort Instant Cost 계산 */
        aCBOptParamInfo->mLeftNode = sCopiedNode;
        STL_TRY( qlncCBOptSortInstant( aCBOptParamInfo,
                                       aEnv )
                 == STL_SUCCESS );

        sRightNode = sCopiedNode;
    }


    QLNC_SET_BASIC_MERGE_COST( sLeftNode,
                               sRightNode,
                               sFilterCost,
                               sBuildCost,
                               sReadIOCost,
                               sOutputIOCost );

    /* Filter Cost 와 Row Count 계산 */
    STL_DASSERT( sEquiJoinCondition != NULL );
    if( sRightUsableIndexScanInfo != NULL )
    {
        /* Equi-Join Condition Cost */
        sFilterCost +=
            ( ( sLeftNode->mBestCost->mResultRowCount +
                sRightNode->mBestCost->mResultRowCount ) *
              sEquiJoinCondition->mOrCount *
              QLNC_SORT_KEY_COMPARE_COST_PER_KEY );
    }
    else
    {
        /* Equi-Join Condition Cost */
        sFilterCost +=
            ( sLeftNode->mBestCost->mResultRowCount *
              sRightNode->mBestCost->mResultRowCount *
              sEquiJoinCondition->mOrCount *
              QLNC_SORT_KEY_COMPARE_COST_PER_KEY );
    }

    /* 다른 Join Method와 동일한 조건으로 계산하기 위하여 재보정 */
    sRowCount = aExpectedResultRowCount;

    /* Non Equi-Join Condition Cost */
    if( sNonEquiJoinCondition != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sNonEquiJoinCondition,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost += sTmpFilterCost;
    }

    /* Join Filter Cost */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sJoinFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost += sTmpFilterCost;
    }

    /* Merge Join Cost 정보 설정 */
    sMergeJoinCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sMergeJoinCost->mCostCommon.mNextFilterCost = sFilterCost + QLNC_OUTPUT_COST_PER_ROW;
    sMergeJoinCost->mCostCommon.mBuildCost = sBuildCost;
    sMergeJoinCost->mCostCommon.mReadIOCost = sReadIOCost;
    sMergeJoinCost->mCostCommon.mOutputIOCost = sOutputIOCost;

    QLNC_SET_TOTAL_COST( &(sMergeJoinCost->mCostCommon) );
    sMergeJoinCost->mCostCommon.mResultRowCount = aExpectedResultRowCount;

    sMergeJoinCost->mLeftNode = sLeftNode;
    sMergeJoinCost->mRightNode = sRightNode;

    sMergeJoinCost->mEquiJoinCondition = sEquiJoinCondition;
    sMergeJoinCost->mNonEquiJoinCondition = sNonEquiJoinCondition;
    sMergeJoinCost->mJoinFilter = sJoinFilter;

    sMergeJoinCost->mSortKeyCount = sSortKeyCount;
    sMergeJoinCost->mLeftSortKeyArr = sLeftSortKeyArr;
    sMergeJoinCost->mRightSortKeyArr = sRightSortKeyArr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Inner Join Cost를 계산한다.
 * @param[in]   aCBOptParamInfo             Cost Based Optimizer Parameter Info
 * @param[in]   aExpectedResultRowCount     Expected Result Row Count
 * @param[in]   aEnv                        Environment
 */
stlStatus qlncCBOptHashInnerJoinCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      stlFloat64              aExpectedResultRowCount,
                                      qllEnv                * aEnv )
{
    stlInt32              i;

    qlncHashJoinCost    * sHashJoinCost         = NULL;
    qlncNodeCommon      * sLeftNode             = NULL;
    qlncNodeCommon      * sRightNode            = NULL;

    qlncAndFilter       * sHashKeyFilter        = NULL;
    qlncAndFilter       * sEquiJoinCondition    = NULL;
    qlncAndFilter       * sNonEquiJoinCondition = NULL;
    qlncAndFilter       * sJoinFilter           = NULL;

    stlInt32              sHashKeyCount;
    qlncExprCommon     ** sLeftHashKeyArr       = NULL;
    qlncExprCommon     ** sRightHashKeyArr      = NULL;

    qlncNodeCommon      * sCopiedNode           = NULL;

    smlIndexAttr          sHashTableAttr;

    stlFloat64            sFilterCost;
    stlFloat64            sBuildCost;
    stlFloat64            sReadIOCost;
    stlFloat64            sOutputIOCost;
    stlFloat64            sTmpFilterCost;
    stlFloat64            sRowCount;
    stlFloat64            sPhysicalRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mRightNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mJoinCond != NULL, QLL_ERROR_STACK(aEnv) );


    /* Hash Join Cost 설정 */
    sHashJoinCost = (qlncHashJoinCost*)(aCBOptParamInfo->mCost);

    /* Node 설정 */
    sLeftNode = aCBOptParamInfo->mLeftNode;
    sRightNode = aCBOptParamInfo->mRightNode;

    /* Join Condition 분리 */
    STL_TRY( qlncSeperateJoinConditionForHashJoin( &(aCBOptParamInfo->mParamInfo),
                                                   sRightNode,
                                                   aCBOptParamInfo->mJoinCond,
                                                   &sEquiJoinCondition,
                                                   &sNonEquiJoinCondition,
                                                   aEnv )
             == STL_SUCCESS );

    /* Equi-Join Condition And Filter 재정렬 */
    if( sEquiJoinCondition != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sEquiJoinCondition,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Non Equi-Join Condition And Filter 재정렬 */
    if( sNonEquiJoinCondition != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sNonEquiJoinCondition,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Join Filter 설정 */
    if( aCBOptParamInfo->mFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    aCBOptParamInfo->mFilter,
                                    &sJoinFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sJoinFilter = NULL;
    }

    /* Join And Filter 재정렬 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sJoinFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    STL_DASSERT( sEquiJoinCondition != NULL );


    /**
     * Hash Key 수집
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sEquiJoinCondition->mOrCount,
                                (void**) &sLeftHashKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sEquiJoinCondition->mOrCount,
                                (void**) &sRightHashKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sHashKeyCount = sEquiJoinCondition->mOrCount;
    for( i = 0; i < sEquiJoinCondition->mOrCount; i++ )
    {
        qlncBooleanFilter   * sBooleanFilter    = NULL;

        STL_DASSERT( sEquiJoinCondition->mOrFilters[i]->mFilterCount == 1 );
        sBooleanFilter = (qlncBooleanFilter*)(sEquiJoinCondition->mOrFilters[i]->mFilters[0]);
        if( qlncIsExistNode( sRightNode,
                             sBooleanFilter->mRightRefNodeList->mHead->mNode ) )
        {
            sLeftHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
            sRightHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
        }
        else
        {
            STL_DASSERT( qlncIsExistNode( sRightNode,
                                          sBooleanFilter->mLeftRefNodeList->mHead->mNode ) );

            sLeftHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
            sRightHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
        }
    }

    /* Hash Table Attribute 설정 */
    stlMemset( &sHashTableAttr, 0x00, STL_SIZEOF( smlIndexAttr ) );

    sHashTableAttr.mValidFlags |= SML_INDEX_LOGGING_YES;
    sHashTableAttr.mLoggingFlag = STL_FALSE;
    sHashTableAttr.mValidFlags |= SML_INDEX_UNIQUENESS_NO;
    sHashTableAttr.mUniquenessFlag = STL_FALSE;
    sHashTableAttr.mPrimaryFlag = STL_FALSE;
    sHashTableAttr.mValidFlags |= SML_INDEX_SKIP_NULL_YES;
    sHashTableAttr.mSkipNullFlag = STL_TRUE;

    /* Hash Instant 생성 처리 */
    STL_TRY( qlncCreateHashInstantForHashJoin( &(aCBOptParamInfo->mParamInfo),
                                               QLNC_INSTANT_TYPE_JOIN,
                                               &sHashTableAttr,
                                               ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                               sHashKeyCount,
                                               sRightHashKeyArr,
                                               sRightNode,
                                               &sCopiedNode,
                                               aEnv )
             == STL_SUCCESS );

    /* Hash Key에 대한 Expression을 생성하여 Push Filter에 추가 */
    sHashKeyFilter = NULL;
    for( i = 0; i < sHashKeyCount; i++ )
    {
        qlncBooleanFilter   * sBooleanFilter    = NULL;
        qlncExprFunction    * sFunction         = NULL;

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprFunction ),
                                    (void**) &sFunction,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sBooleanFilter = (qlncBooleanFilter*)(sEquiJoinCondition->mOrFilters[i]->mFilters[0]);
        stlMemcpy( sFunction,
                   sBooleanFilter->mExpr,
                   STL_SIZEOF( qlncExprFunction ) );

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprCommon* ) * sFunction->mArgCount,
                                    (void**) &(sFunction->mArgs),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( qlncIsExistNode( sRightNode,
                             sBooleanFilter->mRightRefNodeList->mHead->mNode ) )
        {
            sFunction->mArgs[0] = sLeftHashKeyArr[i];
            sFunction->mArgs[1] = sRightHashKeyArr[i];
        }
        else
        {
            STL_DASSERT( qlncIsExistNode( sRightNode,
                                          sBooleanFilter->mLeftRefNodeList->mHead->mNode ) );

            sFunction->mArgs[0] = sRightHashKeyArr[i];
            sFunction->mArgs[1] = sLeftHashKeyArr[i];
        }

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncBooleanFilter ),
                                    (void**) &sBooleanFilter,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sBooleanFilter,
                   sEquiJoinCondition->mOrFilters[i]->mFilters[0],
                   STL_SIZEOF( qlncBooleanFilter ) );

        sBooleanFilter->mExpr = (qlncExprCommon*)sFunction;

        if( sHashKeyFilter == NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncAndFilter ),
                                        (void**) &sHashKeyFilter,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_SET_EXPR_COMMON( &sHashKeyFilter->mCommon,
                                  &QLL_CANDIDATE_MEM( aEnv ),
                                  aEnv,
                                  QLNC_EXPR_TYPE_AND_FILTER,
                                  sBooleanFilter->mExpr->mExprPhraseFlag,
                                  sBooleanFilter->mExpr->mIterationTime,
                                  sBooleanFilter->mExpr->mPosition,
                                  sBooleanFilter->mExpr->mColumnName,
                                  DTL_TYPE_BOOLEAN );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncOrFilter* ),
                                        (void**) &(sHashKeyFilter->mOrFilters),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            sHashKeyFilter->mOrCount = 1;

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncOrFilter ),
                                        (void**) &(sHashKeyFilter->mOrFilters[0]),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_SET_EXPR_COMMON( &sHashKeyFilter->mOrFilters[0]->mCommon,
                                  &QLL_CANDIDATE_MEM( aEnv ),
                                  aEnv,
                                  QLNC_EXPR_TYPE_OR_FILTER,
                                  sBooleanFilter->mExpr->mExprPhraseFlag,
                                  sBooleanFilter->mExpr->mIterationTime,
                                  sBooleanFilter->mExpr->mPosition,
                                  sBooleanFilter->mExpr->mColumnName,
                                  DTL_TYPE_BOOLEAN );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ),
                                        (void**) &(sHashKeyFilter->mOrFilters[0]->mFilters),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            sHashKeyFilter->mOrFilters[0]->mFilterCount = 1;
            sHashKeyFilter->mOrFilters[0]->mFilters[0] = (qlncExprCommon*)sBooleanFilter;
            sHashKeyFilter->mOrFilters[0]->mSubQueryExprList = NULL;
        }
        else
        {
            STL_TRY( qlncAppendFilterToAndFilter( &(aCBOptParamInfo->mParamInfo),
                                                  sHashKeyFilter,
                                                  (qlncExprCommon*)sBooleanFilter,
                                                  aEnv )
                     == STL_SUCCESS );
        }
    }

    /* Push Filter을 Hash Instant에 추가 */
    ((qlncHashInstantNode*)sCopiedNode)->mKeyFilter = sHashKeyFilter;
    ((qlncHashInstantNode*)sCopiedNode)->mRecFilter = sNonEquiJoinCondition;
    sNonEquiJoinCondition = NULL;

    /* Hash Instant Cost 계산 */
    aCBOptParamInfo->mLeftNode = sCopiedNode;
    STL_TRY( qlncCBOptHashInstant( aCBOptParamInfo,
                                   aEnv )
             == STL_SUCCESS );

    sRightNode = sCopiedNode;


    /**
     * Cost 계산
     */

    QLNC_SET_BASIC_HASH_COST( sLeftNode,
                              sRightNode,
                              sFilterCost,
                              sBuildCost,
                              sReadIOCost,
                              sOutputIOCost );


    sRowCount = aExpectedResultRowCount;

    /* Join Filter Cost 계산 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sJoinFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost += sTmpFilterCost;
    }

    /* Hash Join Cost 정보 설정 */
    sHashJoinCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sHashJoinCost->mCostCommon.mNextFilterCost = sFilterCost + QLNC_OUTPUT_COST_PER_ROW;
    sHashJoinCost->mCostCommon.mBuildCost = sBuildCost;
    sHashJoinCost->mCostCommon.mReadIOCost = sReadIOCost;
    sHashJoinCost->mCostCommon.mOutputIOCost = sOutputIOCost;

    QLNC_SET_TOTAL_COST( &(sHashJoinCost->mCostCommon) );
    sHashJoinCost->mCostCommon.mResultRowCount = aExpectedResultRowCount;

    sHashJoinCost->mLeftNode = sLeftNode;
    sHashJoinCost->mRightNode = sRightNode;

    sHashJoinCost->mEquiJoinCondition = NULL;
    sHashJoinCost->mNonEquiJoinCondition = sNonEquiJoinCondition;
    sHashJoinCost->mJoinFilter = sJoinFilter;

    sHashJoinCost->mHashKeyCount = sHashKeyCount;
    sHashJoinCost->mLeftHashKeyArr = sLeftHashKeyArr;
    sHashJoinCost->mRightHashKeyArr = sRightHashKeyArr;

    sHashJoinCost->mIsInverted = STL_FALSE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Inner Join Combine Cost를 계산한다.
 * @param[in]   aCBOptParamInfo         Cost Based Optimizer Parameter Info
 * @param[in]   aInnerJoinNodeInfoArr   Inner Join Node Info Array
 * @param[in]   aNodeCount              Inner Join Node Info Array의 Node Count
 * @param[in]   aNodeIdx                Node Index Number
 * @param[in]   aExpectedResultRowCount Expected Result Row Count
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCBOptInnerJoinCombineCost( qlncCBOptParamInfo     * aCBOptParamInfo,
                                         qlncInnerJoinNodeInfo  * aInnerJoinNodeInfoArr,
                                         stlInt32                 aNodeCount,
                                         stlInt32                 aNodeIdx,
                                         stlFloat64               aExpectedResultRowCount,
                                         qllEnv                 * aEnv )
{
    qlncJoinCombineCost     * sJoinCombineCost  = NULL;

    qlncNodeCommon          * sLeftNode         = NULL;
    qlncNodeCommon          * sRightNode        = NULL;

    qlncNodeCommon          * sNewLeftNode      = NULL;
    qlncNodeCommon          * sNewRightNode     = NULL;

    qlncAndFilter           * sTmpAndFilter     = NULL;

    stlInt32                  i;

    stlFloat64                sFilterCost;
    stlFloat64                sBuildCost;
    stlFloat64                sReadIOCost;
    stlFloat64                sOutputIOCost;
    stlFloat64                sResultRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mRightNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mJoinCond != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mFilter == NULL, QLL_ERROR_STACK(aEnv) );


    /* Join Combine Cost 설정 */
    sJoinCombineCost = (qlncJoinCombineCost*)(aCBOptParamInfo->mCost);

    /* Left Node 및 Right Node 설정 */
    sLeftNode = aCBOptParamInfo->mLeftNode;
    sRightNode = aCBOptParamInfo->mRightNode;

    /* CNF -> DNF 변환 */
    STL_TRY( qlncConvertCnf2Dnf( &(aCBOptParamInfo->mParamInfo),
                                 aCBOptParamInfo->mJoinCond,
                                 &(sJoinCombineCost->mDnfAndFilterCount),
                                 &(sJoinCombineCost->mDnfAndFilterArr),
                                 aEnv )
             == STL_SUCCESS );

    /* Join Node Array 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncNodeCommon* ) * sJoinCombineCost->mDnfAndFilterCount,
                                (void**)&(sJoinCombineCost->mJoinNodePtrArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* DNF에 대한 Cost 계산 */
    sFilterCost = (stlFloat64)0.0;
    sBuildCost = (stlFloat64)0.0;
    sReadIOCost = (stlFloat64)0.0;
    sOutputIOCost = (stlFloat64)0.0;
    sResultRowCount = (stlFloat64)0.0;
    for( i = 0; i < sJoinCombineCost->mDnfAndFilterCount; i++ )
    {
        /* DNF And Filter 복사 */
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    &(sJoinCombineCost->mDnfAndFilterArr[i]),
                                    &sTmpAndFilter,
                                    aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncCBOptRecomputeNodeCostByPushFilter(
                     &(aCBOptParamInfo->mParamInfo),
                     sLeftNode,
                     sTmpAndFilter,
                     &sNewLeftNode,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncCBOptRecomputeNodeCostByPushFilter(
                     &(aCBOptParamInfo->mParamInfo),
                     sRightNode,
                     sTmpAndFilter,
                     &sNewRightNode,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncCompactAndFilter( sTmpAndFilter, aEnv ) == STL_SUCCESS );
        if( sTmpAndFilter->mOrCount == 0 )
        {
            sTmpAndFilter = NULL;
        }

        /* DNF And Filter에 대하여 Join Cost 계산 */
        aCBOptParamInfo->mLeftNode = sNewLeftNode;
        aCBOptParamInfo->mRightNode = sNewRightNode;
        aCBOptParamInfo->mJoinCond = sTmpAndFilter;

        STL_TRY( qlncCBOptInnerJoinCost( aCBOptParamInfo,
                                         aInnerJoinNodeInfoArr,
                                         aNodeCount,
                                         aNodeIdx,
                                         aEnv )
                 == STL_SUCCESS );

        sJoinCombineCost->mJoinNodePtrArr[i] = aCBOptParamInfo->mRightNode;

        sFilterCost +=
            ( sJoinCombineCost->mJoinNodePtrArr[i]->mBestCost->mTotalCost -
              sJoinCombineCost->mJoinNodePtrArr[i]->mBestCost->mBuildCost );
        sBuildCost +=
            sJoinCombineCost->mJoinNodePtrArr[i]->mBestCost->mBuildCost;
        sResultRowCount += sJoinCombineCost->mJoinNodePtrArr[i]->mBestCost->mResultRowCount;
    }

    /* Concat Node에 Insert 및 Unique 체크 비용 추가 */
    if( sResultRowCount < QLNC_HASH_SECTION1_MAX_ROW_COUNT )
    {
        sFilterCost += ( sResultRowCount * QLNC_HASH_KEY_FILTER_COST_PER_ROW_SEC1 );
    }
    else if( sResultRowCount < QLNC_HASH_SECTION2_MAX_ROW_COUNT )
    {
        sFilterCost += ( sResultRowCount * QLNC_HASH_KEY_FILTER_COST_PER_ROW_SEC2 );
    }
    else
    {
        sFilterCost += ( sResultRowCount * QLNC_HASH_KEY_FILTER_COST_PER_ROW_SEC3 );
    }

    /* Join Combine Cost 설정 */
    sJoinCombineCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sJoinCombineCost->mCostCommon.mNextFilterCost = sFilterCost;
    sJoinCombineCost->mCostCommon.mBuildCost =
        ( sBuildCost + ( sResultRowCount * QLNC_HASH_INSTANT_BUILD_COST_PER_ROW ) );
    sJoinCombineCost->mCostCommon.mReadIOCost = sReadIOCost;
    sJoinCombineCost->mCostCommon.mOutputIOCost = sOutputIOCost;

    QLNC_SET_TOTAL_COST( &(sJoinCombineCost->mCostCommon) );
    sJoinCombineCost->mCostCommon.mResultRowCount = aExpectedResultRowCount;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief And Filter에서 현재 노드에서 사용가능한 Filter가 존재하면 Cost를 재계산한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aNode       Candidate Node
 * @param[in]   aAndFilter  And Filter
 * @param[out]  aNewNode    New Candidate Node
 * @param[in]   aEnv        Environment
 */
stlStatus qlncCBOptRecomputeNodeCostByPushFilter( qlncParamInfo     * aParamInfo,
                                                  qlncNodeCommon    * aNode,
                                                  qlncAndFilter     * aAndFilter,
                                                  qlncNodeCommon   ** aNewNode,
                                                  qllEnv            * aEnv )
{
    stlBool                   sExist;
    stlInt32                  i;
    stlInt32                  j;

    qlncNodeCommon          * sLeftNode     = NULL;
    qlncNodeCommon          * sRightNode    = NULL;

    qlncNodeCommon          * sCopiedNode   = NULL;
    qlncRefNodeItem         * sRefNodeItem  = NULL;
    qlncAndFilter             sPushFilter;

    qlncInnerJoinNodeInfo     sInnerJoinNodeInfoArr[2];
    qlncCBOptParamInfo        sCBOptParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter != NULL, QLL_ERROR_STACK(aEnv) );


    /* Push 가능한 Filter 찾기 */
    QLNC_INIT_AND_FILTER( &sPushFilter );
    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        if( aAndFilter->mOrFilters[i] == NULL )
        {
            continue;
        }

        sExist = STL_TRUE;
        for( j = 0; j < aAndFilter->mOrFilters[i]->mFilterCount; j++ )
        {
            if( aAndFilter->mOrFilters[i]->mFilters[j]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER )
            {
                sRefNodeItem =
                    ((qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mRefNodeList.mHead;

                /* Filter의 Reference Node들이 모두 존재하는지 판단 */
                while( sRefNodeItem != NULL )
                {
                    if( qlncIsExistNode( aNode, sRefNodeItem->mNode ) == STL_FALSE )
                    {
                        sExist = STL_FALSE;
                        break;
                    }

                    sRefNodeItem = sRefNodeItem->mNext;
                }
            }
            else
            {
                STL_DASSERT( aAndFilter->mOrFilters[i]->mFilters[j]->mType ==
                             QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER );
            }

            if( sExist == STL_FALSE )
            {
                break;
            }
        }

        if( sExist == STL_TRUE )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        &sPushFilter,
                                                        1,
                                                        &(aAndFilter->mOrFilters[i]),
                                                        aEnv )
                     == STL_SUCCESS );

            aAndFilter->mOrFilters[i] = NULL;
        }
    }

    /* Push Filter가 없으면 종료 */
    if( sPushFilter.mOrCount == 0 )
    {
        *aNewNode = aNode;
        STL_THROW( RAMP_FINISH );
    }

    /* Push Filter가 존재하는 경우 Node에 대하여 Cost를 새로 계산함 */
    if( aNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
    {
        /* Base Table Node 복사 */
        STL_TRY( qlncCopyBaseNodeWithoutCBOInfo( aParamInfo,
                                                 aNode,
                                                 &sCopiedNode,
                                                 aEnv )
                 == STL_SUCCESS );

        /* Push Filter 설정 */
        QLNC_SET_PUSH_FILTER( aParamInfo,
                              ((qlncBaseTableNode*)sCopiedNode)->mFilter,
                              &sPushFilter,
                              aEnv );

        /* Base Table Node Cost 계산 */
        sCBOptParamInfo.mParamInfo = *aParamInfo;
        sCBOptParamInfo.mLeftNode = sCopiedNode;
        sCBOptParamInfo.mRightNode = NULL;
        sCBOptParamInfo.mJoinCond = NULL;
        sCBOptParamInfo.mFilter = NULL;
        sCBOptParamInfo.mCost = NULL;

        STL_TRY( qlncCBOptCopiedBaseTable( &sCBOptParamInfo,
                                           aEnv )
                 == STL_SUCCESS );

        *aNewNode = sCopiedNode;
    }
    else if( aNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
    {
        /* Sub Table Node 복사 */
        STL_TRY( qlncCopySubNodeWithoutCBOInfo( aParamInfo,
                                                aNode,
                                                &sCopiedNode,
                                                aEnv )
                 == STL_SUCCESS );

        /* Push Filter 설정 */
        QLNC_SET_PUSH_FILTER( aParamInfo,
                              ((qlncSubTableNode*)sCopiedNode)->mFilter,
                              &sPushFilter,
                              aEnv );

        /* Sub Table Node Cost 계산 */
        sCBOptParamInfo.mParamInfo = *aParamInfo;
        sCBOptParamInfo.mLeftNode = sCopiedNode;
        sCBOptParamInfo.mRightNode = NULL;
        sCBOptParamInfo.mJoinCond = NULL;
        sCBOptParamInfo.mFilter = NULL;
        sCBOptParamInfo.mCost = NULL;

        STL_TRY( qlncCBOptSubTable( &sCBOptParamInfo,
                                    aEnv )
                 == STL_SUCCESS );

        *aNewNode = sCopiedNode;
    }
    else if( aNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE )
    {
        /* Left Node에 Push Filter 수행 */
        STL_TRY( qlncCBOptRecomputeNodeCostByPushFilter(
                     aParamInfo,
                     ((qlncInnerJoinTableNode*)aNode)->mLeftNode,
                     &sPushFilter,
                     &sLeftNode,
                     aEnv )
                 == STL_SUCCESS );

        /* Right Node에 Push Filter 수행 */
        STL_TRY( qlncCBOptRecomputeNodeCostByPushFilter(
                     aParamInfo,
                     ((qlncInnerJoinTableNode*)aNode)->mRightNode,
                     &sPushFilter,
                     &sRightNode,
                     aEnv )
                 == STL_SUCCESS );

        /* Push Filter를 Compact함 */
        STL_TRY( qlncCompactAndFilter( &sPushFilter, aEnv ) == STL_SUCCESS );

        /* Push Filter에 기존 Inner Join Table Node의 Join Condition을 추가 */
        if( ((qlncInnerJoinTableNode*)aNode)->mJoinCondition != NULL )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail(
                         aParamInfo,
                         &sPushFilter,
                         ((qlncInnerJoinTableNode*)aNode)->mJoinCondition->mOrCount,
                         ((qlncInnerJoinTableNode*)aNode)->mJoinCondition->mOrFilters,
                         aEnv )
                     == STL_SUCCESS );
        }

        /* Push Filter에 기존 Inner Join Table Node의 Join Filter를 추가 */
        if( ((qlncInnerJoinTableNode*)aNode)->mJoinFilter != NULL )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail(
                         aParamInfo,
                         &sPushFilter,
                         ((qlncInnerJoinTableNode*)aNode)->mJoinFilter->mOrCount,
                         ((qlncInnerJoinTableNode*)aNode)->mJoinFilter->mOrFilters,
                         aEnv )
                     == STL_SUCCESS );
        }

        /* Inner Join Node Info 설정 */
        sInnerJoinNodeInfoArr[0].mNode = sLeftNode;
        sInnerJoinNodeInfoArr[0].mAnalyzedNode = sLeftNode;
        sInnerJoinNodeInfoArr[0].mIsFixed = STL_FALSE;
        sInnerJoinNodeInfoArr[0].mIsAnalyzed = STL_TRUE;

        sInnerJoinNodeInfoArr[1].mNode = sRightNode;
        sInnerJoinNodeInfoArr[1].mAnalyzedNode = NULL;
        sInnerJoinNodeInfoArr[1].mIsFixed = STL_FALSE;
        sInnerJoinNodeInfoArr[1].mIsAnalyzed = STL_FALSE;

        /* Join Table Node Cost 계산 */
        sCBOptParamInfo.mParamInfo = *aParamInfo;
        sCBOptParamInfo.mLeftNode = sLeftNode;
        sCBOptParamInfo.mRightNode = sRightNode;
        sCBOptParamInfo.mJoinCond = (sPushFilter.mOrCount == 0 ? NULL : &sPushFilter);
        sCBOptParamInfo.mFilter = NULL;
        sCBOptParamInfo.mCost = NULL;

        STL_TRY( qlncCBOptInnerJoinCost( &sCBOptParamInfo,
                                         sInnerJoinNodeInfoArr,
                                         2,
                                         1,
                                         aEnv )
                 == STL_SUCCESS );

        *aNewNode = sCBOptParamInfo.mRightNode;
    }
    else if( aNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE )
    {
        /* Join Table Node 복사 */
        STL_TRY( qlncCopyJoinNodeWithoutCBOInfo( aParamInfo,
                                                 aNode,
                                                 &sCopiedNode,
                                                 aEnv )
                 == STL_SUCCESS );

        /* Push Filter 설정 */
        QLNC_SET_PUSH_FILTER( aParamInfo,
                              ((qlncJoinTableNode*)sCopiedNode)->mFilter,
                              &sPushFilter,
                              aEnv );

        /* Join Table Node Cost 계산 */
        sCBOptParamInfo.mParamInfo = *aParamInfo;
        sCBOptParamInfo.mLeftNode = sCopiedNode;
        sCBOptParamInfo.mRightNode = NULL;
        sCBOptParamInfo.mJoinCond = NULL;
        sCBOptParamInfo.mFilter = NULL;
        sCBOptParamInfo.mCost = NULL;

        STL_TRY( qlncCBOptJoinTable( &sCBOptParamInfo,
                                     aEnv )
                 == STL_SUCCESS );

        *aNewNode = sCopiedNode;
    }
    else if( aNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
    {
        /* Sort Instant Node 복사 */
        STL_TRY( qlncCopySortInstantNode( aParamInfo,
                                          aNode,
                                          &sCopiedNode,
                                          aEnv )
                 == STL_SUCCESS );

        /* Push Filter 설정 */
        QLNC_SET_PUSH_FILTER( aParamInfo,
                              ((qlncSortInstantNode*)sCopiedNode)->mFilter,
                              &sPushFilter,
                              aEnv );

        /* Filter 재정렬 */
        STL_DASSERT( ((qlncSortInstantNode*)sCopiedNode)->mFilter != NULL );
        STL_TRY( qlncAdjustAndFilterByCost(
                     aParamInfo,
                     ((qlncSortInstantNode*)sCopiedNode)->mFilter,
                     aEnv )
                 == STL_SUCCESS );

        *aNewNode = sCopiedNode;
    }
    else
    {
        STL_DASSERT( aNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT );

        /* Hash Instant Node 복사 */
        STL_TRY( qlncCopyHashInstantNode( aParamInfo,
                                          aNode,
                                          &sCopiedNode,
                                          aEnv )
                 == STL_SUCCESS );

        /* Push Filter 설정 */
        QLNC_SET_PUSH_FILTER( aParamInfo,
                              ((qlncHashInstantNode*)sCopiedNode)->mFilter,
                              &sPushFilter,
                              aEnv );

        /* Filter 재정렬 */
        STL_DASSERT( ((qlncHashInstantNode*)sCopiedNode)->mFilter != NULL );
        STL_TRY( qlncAdjustAndFilterByCost(
                     aParamInfo,
                     ((qlncHashInstantNode*)sCopiedNode)->mFilter,
                     aEnv )
                 == STL_SUCCESS );

        *aNewNode = sCopiedNode;
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Nested Outer Join Cost를 계산한다.
 * @param[in]   aCBOptParamInfo             Cost Based Optimizer Parameter Info
 * @param[in]   aJoinDirect                 Join Direct
 * @param[in]   aExpectedResultRowCount     Expected Result Row Count
 * @param[in]   aEnv                        Environment
 */
stlStatus qlncCBOptNestedOuterJoinCost( qlncCBOptParamInfo  * aCBOptParamInfo,
                                        qlncJoinDirect        aJoinDirect,
                                        stlFloat64            aExpectedResultRowCount,
                                        qllEnv              * aEnv )
{
    stlInt32              i;
    qlncNodeCommon      * sLeftNode         = NULL;
    qlncNodeCommon      * sRightNode        = NULL;

    qlncAndFilter       * sJoinCondition    = NULL;
    qlncAndFilter       * sSubQueryFilter   = NULL;
    qlncAndFilter       * sJoinFilter       = NULL;
    qlncAndFilter         sTmpAndFilter;

    qlncNestedJoinCost  * sNestedJoinCost   = NULL;

    stlFloat64            sFilterCost;
    stlFloat64            sBuildCost;
    stlFloat64            sReadIOCost;
    stlFloat64            sOutputIOCost;
    stlFloat64            sRowCount;
    stlFloat64            sTmpFilterCost;
    stlFloat64            sPhysicalRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mRightNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Nested Join Cost 설정 */
    sNestedJoinCost = (qlncNestedJoinCost*)(aCBOptParamInfo->mCost);
    sNestedJoinCost->mCopiedBaseNode = NULL;

    /* Left Node 설정 */
    sLeftNode = aCBOptParamInfo->mLeftNode;

    /* Join Condition 및 Join Filter 설정 */
    sJoinCondition = aCBOptParamInfo->mJoinCond;
    sJoinFilter = aCBOptParamInfo->mFilter;

    /* Join Condition에 의한 Right Node 설정 */
    if( sJoinCondition == NULL )
    {
        sRightNode = aCBOptParamInfo->mRightNode;
    }
    else
    {
        /* Join Condition 설정 */
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sJoinCondition,
                                    &sJoinCondition,
                                    aEnv )
                 == STL_SUCCESS );

        if( aJoinDirect == QLNC_JOIN_DIRECT_LEFT )
        {
            if( aCBOptParamInfo->mRightNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
            {
                /* Base Table Node 복사 */
                STL_TRY( qlncCopyBaseNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                         aCBOptParamInfo->mRightNode,
                                                         &sRightNode,
                                                         aEnv )
                         == STL_SUCCESS );

                /* Push Filter 설정 */
                QLNC_SET_PUSH_FILTER( &(aCBOptParamInfo->mParamInfo),
                                      ((qlncBaseTableNode*)sRightNode)->mFilter,
                                      sJoinCondition,
                                      aEnv );
                sJoinCondition = NULL;

                /* Base Table Node Cost 계산 */
                aCBOptParamInfo->mLeftNode = sRightNode;
                STL_TRY( qlncCBOptCopiedBaseTable( aCBOptParamInfo,
                                                   aEnv )
                         == STL_SUCCESS );
            }
            else if( aCBOptParamInfo->mRightNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
            {
                /* Sub Table Node 복사 */
                STL_TRY( qlncCopySubNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                        aCBOptParamInfo->mRightNode,
                                                        &sRightNode,
                                                        aEnv )
                         == STL_SUCCESS );

                /* Push Filter 설정 */
                QLNC_SET_PUSH_FILTER( &(aCBOptParamInfo->mParamInfo),
                                      ((qlncSubTableNode*)sRightNode)->mFilter,
                                      sJoinCondition,
                                      aEnv );
                sJoinCondition = NULL;

                /* Sub Table Node Cost 계산 */
                aCBOptParamInfo->mLeftNode = sRightNode;
                STL_TRY( qlncCBOptSubTable( aCBOptParamInfo,
                                            aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                sRightNode = aCBOptParamInfo->mRightNode;
            }
        }
        else
        {
            sRightNode = aCBOptParamInfo->mRightNode;
        }
    }

    /* Join Condition에서 SubQuery가 존재하는 Filter를 걸러냄 */
    if( sJoinCondition != NULL )
    {
        QLNC_INIT_AND_FILTER( &sTmpAndFilter );
        for( i = 0; i < sJoinCondition->mOrCount; i++ )
        {
            if( sJoinCondition->mOrFilters[i]->mSubQueryExprList != NULL )
            {
                STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                            &sTmpAndFilter,
                                                            1,
                                                            &(sJoinCondition->mOrFilters[i]),
                                                            aEnv )
                         == STL_SUCCESS );
                sJoinCondition->mOrFilters[i] = NULL;
            }
        }

        if( sTmpAndFilter.mOrCount > 0 )
        {
            STL_TRY( qlncCompactAndFilter( sJoinCondition,
                                           aEnv )
                     == STL_SUCCESS );

            if( sJoinCondition->mOrCount == 0 )
            {
                sJoinCondition = NULL;
            }

            STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                        &sTmpAndFilter,
                                        &sSubQueryFilter,
                                        aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sSubQueryFilter = NULL;
        }
    }
    else
    {
        sSubQueryFilter = NULL;
    }

    /* Join Filter 설정 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sJoinFilter,
                                    &sJoinFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /* Join And Filter 재정렬 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sJoinFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Nested Join Cost 계산
     */

    QLNC_SET_BASIC_NL_COST( sLeftNode,
                            sRightNode,
                            sFilterCost,
                            sBuildCost,
                            sReadIOCost,
                            sOutputIOCost );

    sRowCount = aExpectedResultRowCount;

    /* Join Condition Cost */
    if( sJoinCondition == NULL )
    {
        sTmpFilterCost = (stlFloat64)0.0;
    }
    else
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sJoinCondition,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    sFilterCost += sTmpFilterCost;

    /* SubQuery Filter Cost */
    if( sSubQueryFilter == NULL )
    {
        sTmpFilterCost = (stlFloat64)0.0;
    }
    else
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sSubQueryFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    sFilterCost += sTmpFilterCost;

    /* Join Filter Cost */
    if( sJoinFilter == NULL )
    {
        sTmpFilterCost = (stlFloat64)0.0;
    }
    else
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sJoinFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    sFilterCost += sTmpFilterCost;

    /* Nested Join Cost 정보 설정 */
    sNestedJoinCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sNestedJoinCost->mCostCommon.mNextFilterCost = sFilterCost + QLNC_OUTPUT_COST_PER_ROW;
    sNestedJoinCost->mCostCommon.mBuildCost = sBuildCost;
    sNestedJoinCost->mCostCommon.mReadIOCost = sReadIOCost;
    sNestedJoinCost->mCostCommon.mOutputIOCost = sOutputIOCost;

    QLNC_SET_TOTAL_COST( &(sNestedJoinCost->mCostCommon) );
    sNestedJoinCost->mCostCommon.mResultRowCount = aExpectedResultRowCount;

    sNestedJoinCost->mLeftNode = sLeftNode;
    sNestedJoinCost->mRightNode = sRightNode;

    sNestedJoinCost->mJoinCondition = sJoinCondition;
    sNestedJoinCost->mSubQueryFilter = sSubQueryFilter;
    sNestedJoinCost->mJoinFilter = sJoinFilter;

    sNestedJoinCost->mIsInverted = STL_FALSE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Merge Outer Join Cost를 계산한다.
 * @param[in]   aCBOptParamInfo             Cost Based Optimizer Parameter Info
 * @param[in]   aJoinDirect                 Join Direct
 * @param[in]   aExpectedResultRowCount     Expected Result Row Count
 * @param[in]   aEnv                        Environment
 */
stlStatus qlncCBOptMergeOuterJoinCost( qlncCBOptParamInfo   * aCBOptParamInfo,
                                       qlncJoinDirect         aJoinDirect,
                                       stlFloat64             aExpectedResultRowCount,
                                       qllEnv               * aEnv )
{
    stlInt32              i;

    qlncMergeJoinCost   * sMergeJoinCost        = NULL;
    qlncNodeCommon      * sLeftNode             = NULL;
    qlncNodeCommon      * sRightNode            = NULL;

    qlncAndFilter       * sEquiJoinCondition    = NULL;
    qlncAndFilter       * sNonEquiJoinCondition = NULL;
    qlncAndFilter       * sJoinFilter           = NULL;

    stlInt32              sSortKeyCount;
    qlncExprCommon     ** sLeftSortKeyArr       = NULL;
    qlncExprCommon     ** sRightSortKeyArr      = NULL;
    stlBool               sIsLeftPureColumn;
    stlBool               sIsRightPureColumn;
    qlncIndexScanInfo   * sLeftUsableIndexScanInfo  = NULL;
    qlncIndexScanInfo   * sRightUsableIndexScanInfo = NULL;

    qlncNodeCommon      * sCopiedNode           = NULL;

    stlUInt8            * sSortKeyFlags         = NULL;
    smlSortTableAttr      sSortTableAttr = { STL_FALSE,     /* not top-down */
                                             STL_FALSE,     /* not volatile */
                                             STL_TRUE,      /* leaf-only */
                                             STL_FALSE,     /* not unique */
                                             STL_FALSE };   /* not null excluded */

    ellIndexColumnOrdering    sLeftIndexOrdering;
    ellIndexColumnOrdering    sRightIndexOrdering;

    stlFloat64            sFilterCost;
    stlFloat64            sBuildCost;
    stlFloat64            sReadIOCost;
    stlFloat64            sOutputIOCost;
    stlFloat64            sRowCount;
    stlFloat64            sTmpFilterCost;
    stlFloat64            sPhysicalRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mRightNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mJoinCond != NULL, QLL_ERROR_STACK(aEnv) );


    /* Merge Join Cost 설정 */
    sMergeJoinCost = (qlncMergeJoinCost*)(aCBOptParamInfo->mCost);

    /* Node 설정 */
    sLeftNode = aCBOptParamInfo->mLeftNode;
    sRightNode = aCBOptParamInfo->mRightNode;

    /* Join Condition 분리 */
    STL_TRY( qlncSeperateJoinConditionForMergeJoin( &(aCBOptParamInfo->mParamInfo),
                                                    aCBOptParamInfo->mJoinCond,
                                                    &sEquiJoinCondition,
                                                    &sNonEquiJoinCondition,
                                                    aEnv )
             == STL_SUCCESS );

    /* Equi-Join Condition And Filter 재정렬 */
    if( sEquiJoinCondition != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sEquiJoinCondition,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Non Equi-Join Condition And Filter 재정렬 */
    if( sNonEquiJoinCondition != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sNonEquiJoinCondition,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Join Filter 설정 */
    if( aCBOptParamInfo->mFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    aCBOptParamInfo->mFilter,
                                    &sJoinFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sJoinFilter = NULL;
    }

    /* Join And Filter 재정렬 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sJoinFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    STL_DASSERT( sEquiJoinCondition != NULL );


    /**
     * Sort Key 수집
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sEquiJoinCondition->mOrCount,
                                (void**) &sLeftSortKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sEquiJoinCondition->mOrCount,
                                (void**) &sRightSortKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sSortKeyCount = sEquiJoinCondition->mOrCount;
    sIsLeftPureColumn = STL_TRUE;
    sIsRightPureColumn = STL_TRUE;
    for( i = 0; i < sEquiJoinCondition->mOrCount; i++ )
    {
        qlncBooleanFilter   * sBooleanFilter    = NULL;

        STL_DASSERT( sEquiJoinCondition->mOrFilters[i]->mFilterCount == 1 );
        sBooleanFilter = (qlncBooleanFilter*)(sEquiJoinCondition->mOrFilters[i]->mFilters[0]);
        if( qlncIsExistNode( sLeftNode,
                             sBooleanFilter->mLeftRefNodeList->mHead->mNode ) )
        {
            sLeftSortKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
            sRightSortKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
        }
        else
        {
            sLeftSortKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
            sRightSortKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
        }

        /* Sort Key가 Column으로만 구성되었는지 여부 체크 */
        if( sLeftSortKeyArr[i]->mType != QLNC_EXPR_TYPE_COLUMN )
        {
            sIsLeftPureColumn = STL_FALSE;
        }

        if( sRightSortKeyArr[i]->mType != QLNC_EXPR_TYPE_COLUMN )
        {
            sIsRightPureColumn = STL_FALSE;
        }
    }

    /* Left Node에 대해 Index Scan이 가능한지 여부 체크 */
    if( sIsLeftPureColumn == STL_TRUE )
    {
        if( (sLeftNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE) &&
            (qlncHasSubQuery( sLeftNode ) == STL_FALSE) )
        {
            (void)qlncFindIndexForSortKey( (qlncBaseTableNode*)sLeftNode,
                                           sSortKeyCount,
                                           sLeftSortKeyArr,
                                           STL_FALSE, /* Full Match */
                                           STL_FALSE, /* Only Unique */
                                           &sLeftUsableIndexScanInfo,
                                           &sLeftIndexOrdering );
        }
        else
        {
            /* Sort Instant 생성 필요 */
            sLeftUsableIndexScanInfo = NULL;
        }
    }
    else
    {
        /* Sort Instant 생성 필요 */
        sLeftUsableIndexScanInfo = NULL;
    }

    /* Right Node에 대해 Index Scan이 가능한지 여부 체크 */
    if( sIsRightPureColumn == STL_TRUE )
    {
        if( (sRightNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE) &&
            (qlncHasSubQuery( sRightNode ) == STL_FALSE) )
        {
            (void)qlncFindIndexForSortKey( (qlncBaseTableNode*)sRightNode,
                                           sSortKeyCount,
                                           sRightSortKeyArr,
                                           STL_TRUE, /* Full Match */
                                           STL_TRUE, /* Only Unique */
                                           &sRightUsableIndexScanInfo,
                                           &sRightIndexOrdering );
        }
        else
        {
            /* Sort Instant 생성 필요 */
            sRightUsableIndexScanInfo = NULL;
        }
    }
    else
    {
        /* Sort Instant 생성 필요 */
        sRightUsableIndexScanInfo = NULL;
    }


    /**
     * Sort Merge Cost 계산
     */

    /* Left Node Cost */
    if( sLeftUsableIndexScanInfo != NULL )
    {
        /* Index Scan이 가능한 경우 */

        /* Left Node 복사 */
        STL_TRY( qlncCopyBaseNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                 sLeftNode,
                                                 &sCopiedNode,
                                                 aEnv )
                 == STL_SUCCESS );

        /* Index Scan Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncIndexScanCost ),
                                    (void**) &(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        ((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostCount = 1;

        /* Index Scan Cost 계산 */
        aCBOptParamInfo->mLeftNode = sCopiedNode;
        aCBOptParamInfo->mFilter = ((qlncBaseTableNode*)sCopiedNode)->mFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr);
        STL_TRY( qlncCBOptIndexScanCost( aCBOptParamInfo,
                                         sLeftUsableIndexScanInfo,
                                         (sLeftIndexOrdering == ELL_INDEX_COLUMN_ASCENDING
                                          ? STL_TRUE : STL_FALSE),
                                         NULL,
                                         aEnv )
                 == STL_SUCCESS );
        aCBOptParamInfo->mFilter = NULL;

        sCopiedNode->mBestOptType = QLNC_BEST_OPT_TYPE_INDEX_SCAN;
        sCopiedNode->mBestCost = &(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr[0].mCostCommon);
        sCopiedNode->mIsAnalyzedCBO = STL_TRUE;

        sLeftNode = sCopiedNode;
    }
    else
    {
        /* Sort Instant를 생성해야 하는 경우 */

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( stlUInt8 ) * sSortKeyCount,
                                    (void**) &sSortKeyFlags,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sSortKeyFlags, 0x00, STL_SIZEOF( stlInt8 ) * sSortKeyCount );

        for( i = 0; i < sSortKeyCount; i++ )
        {
            DTL_SET_INDEX_COLUMN_FLAG( sSortKeyFlags[i],
                                       DTL_KEYCOLUMN_INDEX_ORDER_ASC,
                                       DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE,
                                       DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST );
        }

        STL_TRY( qlncCreateSortInstantForMergeJoin( &(aCBOptParamInfo->mParamInfo),
                                                    QLNC_INSTANT_TYPE_NORMAL,
                                                    &sSortTableAttr,
                                                    ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                                    sSortKeyCount,
                                                    sLeftSortKeyArr,
                                                    sSortKeyFlags,
                                                    sLeftNode,
                                                    &sCopiedNode,
                                                    aEnv )
                 == STL_SUCCESS );

        /* Sort Instant Cost 계산 */
        aCBOptParamInfo->mLeftNode = sCopiedNode;
        STL_TRY( qlncCBOptSortInstant( aCBOptParamInfo,
                                       aEnv )
                 == STL_SUCCESS );

        sLeftNode = sCopiedNode;
    }

    /* Right Node Cost */
    if( sRightUsableIndexScanInfo != NULL )
    {
        /* Index Scan이 가능한 경우 */

        /* Right Node 복사 */
        STL_TRY( qlncCopyBaseNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                 sRightNode,
                                                 &sCopiedNode,
                                                 aEnv )
                 == STL_SUCCESS );

        /* Index Scan Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncIndexScanCost ),
                                    (void**) &(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        ((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostCount = 1;

        /* Index Scan Cost 계산 */
        aCBOptParamInfo->mLeftNode = sCopiedNode;
        aCBOptParamInfo->mFilter = ((qlncBaseTableNode*)sCopiedNode)->mFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr);
        STL_TRY( qlncCBOptIndexScanCost( aCBOptParamInfo,
                                         sRightUsableIndexScanInfo,
                                         (sRightIndexOrdering == ELL_INDEX_COLUMN_ASCENDING
                                          ? STL_TRUE : STL_FALSE),
                                         NULL,
                                         aEnv )
                 == STL_SUCCESS );
        aCBOptParamInfo->mFilter = NULL;

        sCopiedNode->mBestOptType = QLNC_BEST_OPT_TYPE_INDEX_SCAN;
        sCopiedNode->mBestCost = &(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr[0].mCostCommon);
        sCopiedNode->mIsAnalyzedCBO = STL_TRUE;

        sRightNode = sCopiedNode;
    }
    else
    {
        /* Sort Instant를 생성해야 하는 경우 */

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( stlUInt8 ) * sSortKeyCount,
                                    (void**) &sSortKeyFlags,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sSortKeyFlags, 0x00, STL_SIZEOF( stlInt8 ) * sSortKeyCount );

        for( i = 0; i < sSortKeyCount; i++ )
        {
            DTL_SET_INDEX_COLUMN_FLAG( sSortKeyFlags[i],
                                       DTL_KEYCOLUMN_INDEX_ORDER_ASC,
                                       DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE,
                                       DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST );
        }

        STL_TRY( qlncCreateSortInstantForMergeJoin( &(aCBOptParamInfo->mParamInfo),
                                                    QLNC_INSTANT_TYPE_JOIN,
                                                    &sSortTableAttr,
                                                    ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                                    sSortKeyCount,
                                                    sRightSortKeyArr,
                                                    sSortKeyFlags,
                                                    sRightNode,
                                                    &sCopiedNode,
                                                    aEnv )
                 == STL_SUCCESS );

        /* Sort Instant Cost 계산 */
        aCBOptParamInfo->mLeftNode = sCopiedNode;
        STL_TRY( qlncCBOptSortInstant( aCBOptParamInfo,
                                       aEnv )
                 == STL_SUCCESS );

        sRightNode = sCopiedNode;
    }


    QLNC_SET_BASIC_MERGE_COST( sLeftNode,
                               sRightNode,
                               sFilterCost,
                               sBuildCost,
                               sReadIOCost,
                               sOutputIOCost );

    /* Filter Cost 와 Row Count 계산 */
    STL_DASSERT( sEquiJoinCondition != NULL );
    if( sRightUsableIndexScanInfo != NULL )
    {
        /* Equi-Join Condition Cost */
        sFilterCost +=
            ( ( sLeftNode->mBestCost->mResultRowCount +
                sRightNode->mBestCost->mResultRowCount ) *
              sEquiJoinCondition->mOrCount *
              QLNC_SORT_KEY_COMPARE_COST_PER_KEY );
    }
    else
    {
        /* Equi-Join Condition Cost */
        sFilterCost +=
            ( sLeftNode->mBestCost->mResultRowCount *
              sEquiJoinCondition->mOrCount *
              QLNC_SORT_KEY_COMPARE_COST_PER_KEY );
    }

    /* 다른 Join Method와 동일한 조건으로 계산하기 위하여 재보정 */
    sRowCount = aExpectedResultRowCount;

    /* Non Equi-Join Condition Cost */
    if( sNonEquiJoinCondition != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sNonEquiJoinCondition,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost += sTmpFilterCost;
    }

    /* Join Filter Cost */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sJoinFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost += sTmpFilterCost;
    }

    /* Merge Join Cost 정보 설정 */
    sMergeJoinCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sMergeJoinCost->mCostCommon.mNextFilterCost = sFilterCost + QLNC_OUTPUT_COST_PER_ROW;
    sMergeJoinCost->mCostCommon.mBuildCost = sBuildCost;
    sMergeJoinCost->mCostCommon.mReadIOCost = sReadIOCost;
    sMergeJoinCost->mCostCommon.mOutputIOCost = sOutputIOCost;

    QLNC_SET_TOTAL_COST( &(sMergeJoinCost->mCostCommon) );
    sMergeJoinCost->mCostCommon.mResultRowCount = aExpectedResultRowCount;

    sMergeJoinCost->mLeftNode = sLeftNode;
    sMergeJoinCost->mRightNode = sRightNode;

    sMergeJoinCost->mEquiJoinCondition = sEquiJoinCondition;
    sMergeJoinCost->mNonEquiJoinCondition = sNonEquiJoinCondition;
    sMergeJoinCost->mJoinFilter = sJoinFilter;

    sMergeJoinCost->mSortKeyCount = sSortKeyCount;
    sMergeJoinCost->mLeftSortKeyArr = sLeftSortKeyArr;
    sMergeJoinCost->mRightSortKeyArr = sRightSortKeyArr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Outer Join Cost를 계산한다.
 * @param[in]   aCBOptParamInfo             Cost Based Optimizer Parameter Info
 * @param[in]   aJoinDirect                 Join Direct
 * @param[in]   aExpectedResultRowCount     Expected Result Row Count
 * @param[in]   aEnv                        Environment
 */
stlStatus qlncCBOptHashOuterJoinCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qlncJoinDirect          aJoinDirect,
                                      stlFloat64              aExpectedResultRowCount,
                                      qllEnv                * aEnv )
{
    stlInt32              i;

    qlncHashJoinCost    * sHashJoinCost         = NULL;
    qlncNodeCommon      * sLeftNode             = NULL;
    qlncNodeCommon      * sRightNode            = NULL;

    qlncAndFilter       * sEquiJoinCondition    = NULL;
    qlncAndFilter       * sNonEquiJoinCondition = NULL;
    qlncAndFilter       * sJoinFilter           = NULL;
    qlncAndFilter       * sPushFilter           = NULL;

    stlInt32              sHashKeyCount;
    qlncExprCommon     ** sLeftHashKeyArr       = NULL;
    qlncExprCommon     ** sRightHashKeyArr      = NULL;

    qlncNodeCommon      * sCopiedNode           = NULL;

    smlIndexAttr          sHashTableAttr;

    stlFloat64            sFilterCost;
    stlFloat64            sBuildCost;
    stlFloat64            sReadIOCost;
    stlFloat64            sOutputIOCost;
    stlFloat64            sTmpFilterCost;
    stlFloat64            sRowCount;
    stlFloat64            sPhysicalRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mRightNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mJoinCond != NULL, QLL_ERROR_STACK(aEnv) );


    /* Hash Join Cost 설정 */
    sHashJoinCost = (qlncHashJoinCost*)(aCBOptParamInfo->mCost);

    /* Node 설정 */
    sLeftNode = aCBOptParamInfo->mLeftNode;
    sRightNode = aCBOptParamInfo->mRightNode;

    /* Join Condition 분리 */
    STL_TRY( qlncSeperateJoinConditionForHashJoin( &(aCBOptParamInfo->mParamInfo),
                                                   sRightNode,
                                                   aCBOptParamInfo->mJoinCond,
                                                   &sEquiJoinCondition,
                                                   &sNonEquiJoinCondition,
                                                   aEnv )
             == STL_SUCCESS );

    /* Equi-Join Condition And Filter 재정렬 */
    if( sEquiJoinCondition != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sEquiJoinCondition,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* NonEqui-Join Condition And Filter 재정렬 */
    if( sNonEquiJoinCondition != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sNonEquiJoinCondition,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Join Filter 설정 */
    if( aCBOptParamInfo->mFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    aCBOptParamInfo->mFilter,
                                    &sJoinFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sJoinFilter = NULL;
    }

    /* Join And Filter 재정렬 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sJoinFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    STL_DASSERT( sEquiJoinCondition != NULL );


    /**
     * Hash Key 수집
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sEquiJoinCondition->mOrCount,
                                (void**) &sLeftHashKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sEquiJoinCondition->mOrCount,
                                (void**) &sRightHashKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sHashKeyCount = sEquiJoinCondition->mOrCount;
    for( i = 0; i < sEquiJoinCondition->mOrCount; i++ )
    {
        qlncBooleanFilter   * sBooleanFilter    = NULL;

        STL_DASSERT( sEquiJoinCondition->mOrFilters[i]->mFilterCount == 1 );
        sBooleanFilter = (qlncBooleanFilter*)(sEquiJoinCondition->mOrFilters[i]->mFilters[0]);
        if( qlncIsExistNode( sRightNode,
                             sBooleanFilter->mRightRefNodeList->mHead->mNode ) )
        {
            sLeftHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
            sRightHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
        }
        else
        {
            STL_DASSERT( qlncIsExistNode( sRightNode,
                                          sBooleanFilter->mLeftRefNodeList->mHead->mNode ) );

            sLeftHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
            sRightHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
        }
    }

    /* Hash Table Attribute 설정 */
    stlMemset( &sHashTableAttr, 0x00, STL_SIZEOF( smlIndexAttr ) );

    sHashTableAttr.mValidFlags |= SML_INDEX_LOGGING_YES;
    sHashTableAttr.mLoggingFlag = STL_FALSE;
    sHashTableAttr.mValidFlags |= SML_INDEX_UNIQUENESS_NO;
    sHashTableAttr.mUniquenessFlag = STL_FALSE;
    sHashTableAttr.mPrimaryFlag = STL_FALSE;
    sHashTableAttr.mValidFlags |= SML_INDEX_SKIP_NULL_YES;
    sHashTableAttr.mSkipNullFlag = STL_TRUE;

    /* Hash Instant 생성 처리 */
    STL_TRY( qlncCreateHashInstantForHashJoin( &(aCBOptParamInfo->mParamInfo),
                                               QLNC_INSTANT_TYPE_JOIN,
                                               &sHashTableAttr,
                                               ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                               sHashKeyCount,
                                               sRightHashKeyArr,
                                               sRightNode,
                                               &sCopiedNode,
                                               aEnv )
             == STL_SUCCESS );

    /* Hash Key에 대한 Expression을 생성하여 Push Filter에 추가 */
    for( i = 0; i < sHashKeyCount; i++ )
    {
        qlncBooleanFilter   * sBooleanFilter    = NULL;
        qlncExprFunction    * sFunction         = NULL;

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprFunction ),
                                    (void**) &sFunction,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sBooleanFilter = (qlncBooleanFilter*)(sEquiJoinCondition->mOrFilters[i]->mFilters[0]);
        stlMemcpy( sFunction,
                   sBooleanFilter->mExpr,
                   STL_SIZEOF( qlncExprFunction ) );

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprCommon* ) * sFunction->mArgCount,
                                    (void**) &(sFunction->mArgs),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( qlncIsExistNode( sRightNode,
                             sBooleanFilter->mRightRefNodeList->mHead->mNode ) )
        {
            sFunction->mArgs[0] = sLeftHashKeyArr[i];
            sFunction->mArgs[1] = sRightHashKeyArr[i];
        }
        else
        {
            STL_DASSERT( qlncIsExistNode( sRightNode,
                                          sBooleanFilter->mLeftRefNodeList->mHead->mNode ) );

            sFunction->mArgs[0] = sRightHashKeyArr[i];
            sFunction->mArgs[1] = sLeftHashKeyArr[i];
        }

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncBooleanFilter ),
                                    (void**) &sBooleanFilter,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sBooleanFilter,
                   sEquiJoinCondition->mOrFilters[i]->mFilters[0],
                   STL_SIZEOF( qlncBooleanFilter ) );

        sBooleanFilter->mExpr = (qlncExprCommon*)sFunction;

        if( sPushFilter == NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncAndFilter ),
                                        (void**) &sPushFilter,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_SET_EXPR_COMMON( &sPushFilter->mCommon,
                                  &QLL_CANDIDATE_MEM( aEnv ),
                                  aEnv,
                                  QLNC_EXPR_TYPE_AND_FILTER,
                                  sBooleanFilter->mExpr->mExprPhraseFlag,
                                  sBooleanFilter->mExpr->mIterationTime,
                                  sBooleanFilter->mExpr->mPosition,
                                  sBooleanFilter->mExpr->mColumnName,
                                  DTL_TYPE_BOOLEAN );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncOrFilter* ),
                                        (void**) &(sPushFilter->mOrFilters),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            sPushFilter->mOrCount = 1;

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncOrFilter ),
                                        (void**) &(sPushFilter->mOrFilters[0]),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_SET_EXPR_COMMON( &sPushFilter->mOrFilters[0]->mCommon,
                                  &QLL_CANDIDATE_MEM( aEnv ),
                                  aEnv,
                                  QLNC_EXPR_TYPE_OR_FILTER,
                                  sBooleanFilter->mExpr->mExprPhraseFlag,
                                  sBooleanFilter->mExpr->mIterationTime,
                                  sBooleanFilter->mExpr->mPosition,
                                  sBooleanFilter->mExpr->mColumnName,
                                  DTL_TYPE_BOOLEAN );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ),
                                        (void**) &(sPushFilter->mOrFilters[0]->mFilters),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            sPushFilter->mOrFilters[0]->mFilterCount = 1;
            sPushFilter->mOrFilters[0]->mFilters[0] = (qlncExprCommon*)sBooleanFilter;
            sPushFilter->mOrFilters[0]->mSubQueryExprList = NULL;
        }
        else
        {
            STL_TRY( qlncAppendFilterToAndFilter( &(aCBOptParamInfo->mParamInfo),
                                                  sPushFilter,
                                                  (qlncExprCommon*)sBooleanFilter,
                                                  aEnv )
                     == STL_SUCCESS );
        }
    }

    /* Push Filter을 Hash Instant에 추가 */
    ((qlncHashInstantNode*)sCopiedNode)->mKeyFilter = sPushFilter;
    sPushFilter = NULL;

    if( aJoinDirect == QLNC_JOIN_DIRECT_LEFT )
    {
        if( (sNonEquiJoinCondition != NULL) &&
            (sNonEquiJoinCondition->mOrCount > 0) )
        {
            ((qlncHashInstantNode*)sCopiedNode)->mRecFilter = sNonEquiJoinCondition;
        }

        sNonEquiJoinCondition = NULL;
    }

    /* Hash Instant Cost 계산 */
    aCBOptParamInfo->mLeftNode = sCopiedNode;
    STL_TRY( qlncCBOptHashInstant( aCBOptParamInfo,
                                   aEnv )
             == STL_SUCCESS );

    sRightNode = sCopiedNode;


    /**
     * Cost 계산
     */

    QLNC_SET_BASIC_HASH_COST( sLeftNode,
                              sRightNode,
                              sFilterCost,
                              sBuildCost,
                              sReadIOCost,
                              sOutputIOCost );


    sRowCount = aExpectedResultRowCount;

    /* Non Equi-Join Condition Cost 계산 */
    if( sNonEquiJoinCondition != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sNonEquiJoinCondition,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost += sTmpFilterCost;
    }

    /* Join Filter Cost 계산 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sJoinFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost += sTmpFilterCost;
    }

    /* Hash Join Cost 정보 설정 */
    sHashJoinCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sHashJoinCost->mCostCommon.mNextFilterCost = sFilterCost + QLNC_OUTPUT_COST_PER_ROW;
    sHashJoinCost->mCostCommon.mBuildCost = sBuildCost;
    sHashJoinCost->mCostCommon.mReadIOCost = sReadIOCost;
    sHashJoinCost->mCostCommon.mOutputIOCost = sOutputIOCost;

    QLNC_SET_TOTAL_COST( &(sHashJoinCost->mCostCommon) );
    sHashJoinCost->mCostCommon.mResultRowCount = aExpectedResultRowCount;

    sHashJoinCost->mLeftNode = sLeftNode;
    sHashJoinCost->mRightNode = sRightNode;

    sHashJoinCost->mEquiJoinCondition = NULL;
    sHashJoinCost->mNonEquiJoinCondition = sNonEquiJoinCondition;
    sHashJoinCost->mJoinFilter = sJoinFilter;

    sHashJoinCost->mHashKeyCount = sHashKeyCount;
    sHashJoinCost->mLeftHashKeyArr = sLeftHashKeyArr;
    sHashJoinCost->mRightHashKeyArr = sRightHashKeyArr;

    sHashJoinCost->mIsInverted = STL_FALSE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Nested Semi Join Cost를 계산한다.
 * @param[in]   aCBOptParamInfo             Cost Based Optimizer Parameter Info
 * @param[in]   aJoinDirect                 Join Direct
 * @param[in]   aIsAntiSemi                 Anti Semi Join 여부
 * @param[in]   aExpectedResultRowCount     Expected Result Row Count
 * @param[in]   aEnv                        Environment
 */
stlStatus qlncCBOptNestedSemiJoinCost( qlncCBOptParamInfo   * aCBOptParamInfo,
                                       qlncJoinDirect         aJoinDirect,
                                       stlBool                aIsAntiSemi,
                                       stlFloat64             aExpectedResultRowCount,
                                       qllEnv               * aEnv )
{
    qlncNodeCommon      * sLeftNode         = NULL;
    qlncNodeCommon      * sRightNode        = NULL;
    qlncNodeCommon      * sSortInstantNode  = NULL;

    qlncAndFilter       * sJoinCondition    = NULL;
    qlncAndFilter       * sJoinFilter       = NULL;

    qlncNestedJoinCost  * sNestedJoinCost   = NULL;
    qlncNodeArray         sNodeArray;

    smlSortTableAttr      sSortTableAttr    = { STL_TRUE,      /* top-down */
                                                STL_FALSE,     /* not volatile */
                                                STL_FALSE,     /* not leaf-only */
                                                STL_TRUE,      /* unique */
                                                STL_FALSE };   /* not null excluded */

    stlFloat64            sFilterCost;
    stlFloat64            sBuildCost;
    stlFloat64            sReadIOCost;
    stlFloat64            sOutputIOCost;
    stlFloat64            sRowCount;
    stlFloat64            sTmpFilterCost;
    stlFloat64            sPhysicalRowCount;
    stlBool               sIsUseIndex;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mRightNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Nested Join Cost 설정 */
    sNestedJoinCost = (qlncNestedJoinCost*)(aCBOptParamInfo->mCost);
    sNestedJoinCost->mCopiedBaseNode = NULL;

    /* Left Node 설정 */
    sLeftNode = aCBOptParamInfo->mLeftNode;

    /* Join Condition 및 Join Filter 설정 */
    sJoinCondition = aCBOptParamInfo->mJoinCond;
    sJoinFilter = aCBOptParamInfo->mFilter;

    /* Join Condition에 의한 Right Node 설정 */
    sIsUseIndex = STL_FALSE;
    if( sJoinCondition == NULL )
    {
        sRightNode = aCBOptParamInfo->mRightNode;
    }
    else
    {
        /* Join Condition 설정 */
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sJoinCondition,
                                    &sJoinCondition,
                                    aEnv )
                 == STL_SUCCESS );

        if( aJoinDirect == QLNC_JOIN_DIRECT_LEFT )
        {
            /* Node Array 초기화 */
            STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                              &sNodeArray,
                                              aEnv )
                     == STL_SUCCESS );

            /* Node Array 생성 */
            STL_TRY( qlncMakeNodeArrayOnCurrNode( &sNodeArray,
                                                  aCBOptParamInfo->mRightNode,
                                                  aEnv )
                     == STL_SUCCESS );

            if( ( aIsAntiSemi == STL_TRUE ) ||
                ( qlncIsExistOuterColumnOnCandNode( aCBOptParamInfo->mRightNode,
                                                    &sNodeArray )
                  == STL_FALSE ) )
            {
                STL_TRY( qlncCBOptMakeSortInstantForNestedLoops( &(aCBOptParamInfo->mParamInfo),
                                                                 &sSortTableAttr,
                                                                 aCBOptParamInfo->mRightNode,
                                                                 sJoinCondition,
                                                                 STL_TRUE,
                                                                 NULL,
                                                                 &sSortInstantNode,
                                                                 aEnv )
                         == STL_SUCCESS );

                /* Sort Instant Cost 계산 */
                if( sSortInstantNode != NULL )
                {
                    aCBOptParamInfo->mLeftNode = sSortInstantNode;
                    STL_TRY( qlncCBOptSortInstant( aCBOptParamInfo,
                                                   aEnv )
                             == STL_SUCCESS );
                }
            }

            if( aCBOptParamInfo->mRightNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
            {
                /* Base Table Node 복사 */
                STL_TRY( qlncCopyBaseNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                         aCBOptParamInfo->mRightNode,
                                                         &sRightNode,
                                                         aEnv )
                         == STL_SUCCESS );

                /* Push Filter 설정 */
                QLNC_SET_PUSH_FILTER( &(aCBOptParamInfo->mParamInfo),
                                      ((qlncBaseTableNode*)sRightNode)->mFilter,
                                      sJoinCondition,
                                      aEnv );
                sJoinCondition = NULL;

                /* Base Table Node Cost 계산 */
                aCBOptParamInfo->mLeftNode = sRightNode;
                STL_TRY( qlncCBOptCopiedBaseTable( aCBOptParamInfo,
                                                   aEnv )
                         == STL_SUCCESS );

                if( sSortInstantNode != NULL )
                {
                    if( QLNC_LESS_THAN_DOUBLE( sSortInstantNode->mBestCost->mTotalCost,
                                               sRightNode->mBestCost->mTotalCost ) )
                    {
                        sRightNode = sSortInstantNode;
                        sJoinCondition = NULL;
                        sIsUseIndex = STL_TRUE;
                    }
                }

                if( sRightNode->mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_SCAN )
                {
                    sIsUseIndex = STL_TRUE;
                }
            }
            else if( aCBOptParamInfo->mRightNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
            {
                /* Sub Table Node 복사 */
                STL_TRY( qlncCopySubNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                        aCBOptParamInfo->mRightNode,
                                                        &sRightNode,
                                                        aEnv )
                         == STL_SUCCESS );

                /* Push Filter 설정 */
                QLNC_SET_PUSH_FILTER( &(aCBOptParamInfo->mParamInfo),
                                      ((qlncSubTableNode*)sRightNode)->mFilter,
                                      sJoinCondition,
                                      aEnv );
                sJoinCondition = NULL;

                /* Sub Table Node Cost 계산 */
                aCBOptParamInfo->mLeftNode = sRightNode;
                STL_TRY( qlncCBOptSubTable( aCBOptParamInfo,
                                            aEnv )
                         == STL_SUCCESS );
                /* Sub Table은 자체로 Materialized가 가능하면 하므로 별도로 Instant를 처리할 필요 없음 */

                if( ((qlncFilterCost*)(sRightNode->mBestCost))->mMaterializedNode != NULL )
                {
                    sIsUseIndex = STL_TRUE;
                }

            }
            else if( aCBOptParamInfo->mRightNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE )
            {
                /* 하위노드가 Semi Join/Anti Semi Join이 아닌 경우에 한해서 Filter Push 가능 */
                if( (((qlncJoinTableNode*)(aCBOptParamInfo->mRightNode))->mJoinType != QLNC_JOIN_TYPE_SEMI) &&
                    (((qlncJoinTableNode*)(aCBOptParamInfo->mRightNode))->mJoinType != QLNC_JOIN_TYPE_ANTI_SEMI) &&
                    (((qlncJoinTableNode*)(aCBOptParamInfo->mRightNode))->mJoinType != QLNC_JOIN_TYPE_ANTI_SEMI_NA) )
                {
                    /* Join Table Node 복사 */
                    STL_TRY( qlncCopyJoinNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                             aCBOptParamInfo->mRightNode,
                                                             &sRightNode,
                                                             aEnv )
                             == STL_SUCCESS );

                    /* Join Condition 복사 */
                    STL_TRY( qlncDuplicateExpr( &(aCBOptParamInfo->mParamInfo),
                                                (qlncExprCommon*)sJoinCondition,
                                                (qlncExprCommon**)&sJoinCondition,
                                                aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlncRemoveOuterTableNodeFromRefNodeList(
                                                                     &(aCBOptParamInfo->mParamInfo),
                                                                     (qlncExprCommon*)sJoinCondition,
                                                                     sRightNode,
                                                                     (qlncExprCommon**)&sJoinCondition,
                                                                     aEnv )
                             == STL_SUCCESS );

                    /* Push Filter 설정 */
                    QLNC_SET_PUSH_FILTER( &(aCBOptParamInfo->mParamInfo),
                                          ((qlncJoinTableNode*)sRightNode)->mFilter,
                                          sJoinCondition,
                                          aEnv );
                    sJoinCondition = NULL;

                    /* Join Table Node Cost 계산 */
                    aCBOptParamInfo->mLeftNode = sRightNode;
                    STL_TRY( qlncCBOptJoinTable( aCBOptParamInfo,
                                                 aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    sRightNode = aCBOptParamInfo->mRightNode;
                }

                if( sSortInstantNode != NULL )
                {
                    if( QLNC_LESS_THAN_DOUBLE( sSortInstantNode->mBestCost->mTotalCost,
                                               sRightNode->mBestCost->mTotalCost ) )
                    {
                        sRightNode = sSortInstantNode;
                        sJoinCondition = NULL;
                        sIsUseIndex = STL_TRUE;
                    }
                }
            }
            else if( aCBOptParamInfo->mRightNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
            {
                /* Sort Instant Node 복사 */
                STL_TRY( qlncCopySortInstantNode( &(aCBOptParamInfo->mParamInfo),
                                                  aCBOptParamInfo->mRightNode,
                                                  &sRightNode,
                                                  aEnv )
                         == STL_SUCCESS );

                /* Push Filter 설정 */
                QLNC_SET_PUSH_FILTER( &(aCBOptParamInfo->mParamInfo),
                                      ((qlncSortInstantNode*)sRightNode)->mFilter,
                                      sJoinCondition,
                                      aEnv );
                sJoinCondition = NULL;

                /* Filter 재정렬 */
                if( ((qlncSortInstantNode*)sRightNode)->mFilter != NULL )
                {
                    STL_TRY( qlncAdjustAndFilterByCost(
                                 &(aCBOptParamInfo->mParamInfo),
                                 ((qlncSortInstantNode*)sRightNode)->mFilter,
                                 aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                STL_DASSERT( aCBOptParamInfo->mRightNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT );

                /* Hash Instant Node 복사 */
                STL_TRY( qlncCopyHashInstantNode( &(aCBOptParamInfo->mParamInfo),
                                                  aCBOptParamInfo->mRightNode,
                                                  &sRightNode,
                                                  aEnv )
                         == STL_SUCCESS );

                /* Push Filter 설정 */
                QLNC_SET_PUSH_FILTER( &(aCBOptParamInfo->mParamInfo),
                                      ((qlncHashInstantNode*)sRightNode)->mFilter,
                                      sJoinCondition,
                                      aEnv );
                sJoinCondition = NULL;

                /* Filter 재정렬 */
                if( ((qlncHashInstantNode*)sRightNode)->mFilter != NULL )
                {
                    STL_TRY( qlncAdjustAndFilterByCost(
                                 &(aCBOptParamInfo->mParamInfo),
                                 ((qlncHashInstantNode*)sRightNode)->mFilter,
                                 aEnv )
                             == STL_SUCCESS );
                }
            }
        }
        else
        {
            /* @todo 현재 여기로 들어오는 경우는 없는 것으로 판단됨 */

            sRightNode = aCBOptParamInfo->mRightNode;
        }
    }

    /* Join Filter 설정 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sJoinFilter,
                                    &sJoinFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /* Join And Filter 재정렬 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sJoinFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Nested Join Cost 계산
     */

    /* Filter Cost */
    sFilterCost =
        ( sLeftNode->mBestCost->mTotalCost -
          sLeftNode->mBestCost->mBuildCost );

    if( sIsUseIndex == STL_TRUE )
    {
        if( sRightNode->mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_SCAN )
        {
            sFilterCost +=
                ( sLeftNode->mBestCost->mResultRowCount *
                  ( sRightNode->mBestCost->mTotalCost -
                    sRightNode->mBestCost->mFirstFilterCost +
                    QLNC_UNIQ_IDX_RANGE_COST_PER_SEARCH -
                    sRightNode->mBestCost->mNextFilterCost -
                    sRightNode->mBestCost->mBuildCost ) );
        }
        else
        {
            STL_DASSERT( (sRightNode->mBestOptType == QLNC_BEST_OPT_TYPE_SORT_INSTANT) ||
                         (sRightNode->mBestOptType == QLNC_BEST_OPT_TYPE_FILTER) );
            sFilterCost +=
                ( sLeftNode->mBestCost->mResultRowCount *
                  ( sRightNode->mBestCost->mTotalCost -
                    sRightNode->mBestCost->mNextFilterCost -
                    sRightNode->mBestCost->mBuildCost ) );
        }

        /* 첫 필터 이후에는 최초 일치 한건만 찾으면 된다. */
        sFilterCost +=
            ( sLeftNode->mBestCost->mResultRowCount *
              ( sRightNode->mBestCost->mNextFilterCost /
                ( sRightNode->mBestCost->mResultRowCount < (stlFloat64)1.0
                  ? (stlFloat64)1.0 : sRightNode->mBestCost->mResultRowCount ) ) );
    }
    else
    {
        sFilterCost +=
            ( sLeftNode->mBestCost->mResultRowCount *
              ( sRightNode->mBestCost->mTotalCost -
                sRightNode->mBestCost->mNextFilterCost -
                sRightNode->mBestCost->mBuildCost ) );

        /* 첫 필터 이후에는 최초 일치 한건만 찾으면 된다. */
        sFilterCost +=
            ( sLeftNode->mBestCost->mResultRowCount *
              ( sRightNode->mBestCost->mNextFilterCost /
                QLNC_EQUAL_OPER_RESULT_RATE ) );
    }

    sFilterCost +=
        ( sLeftNode->mBestCost->mReadIOCost +
          sRightNode->mBestCost->mReadIOCost );

    sFilterCost +=
        ( sLeftNode->mBestCost->mOutputIOCost +
          sRightNode->mBestCost->mOutputIOCost );

    /* Build Cost */
    sBuildCost =
        ( sLeftNode->mBestCost->mBuildCost +
          sRightNode->mBestCost->mBuildCost );

    /* Read IO Cost */
    sReadIOCost = (stlFloat64)0.0;

    /* Output IO Cost */
    sOutputIOCost = (stlFloat64)0.0;


    if( aJoinDirect == QLNC_JOIN_DIRECT_LEFT )
    {
        sRowCount = sLeftNode->mBestCost->mResultRowCount;
    }
    else
    {
        /* @todo 현재 여기로 들어오는 경우는 없는 것으로 판단됨 */

        STL_DASSERT( aJoinDirect == QLNC_JOIN_DIRECT_RIGHT );
        sRowCount = sRightNode->mBestCost->mResultRowCount;
    }

    /* Join Condition Cost */
    if( sJoinCondition != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sJoinCondition,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost +=
            ( sTmpFilterCost / (sRowCount < (stlFloat64)1.0 ? (stlFloat64)1.0 : sRowCount) );
    }

    /* Join Filter Cost */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sJoinFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost +=
            ( sTmpFilterCost / (sRowCount < (stlFloat64)1.0 ? (stlFloat64)1.0 : sRowCount) );
    }

    /* Nested Join Cost 정보 설정 */
    sNestedJoinCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sNestedJoinCost->mCostCommon.mNextFilterCost = sFilterCost + QLNC_OUTPUT_COST_PER_ROW;
    sNestedJoinCost->mCostCommon.mBuildCost = sBuildCost;
    sNestedJoinCost->mCostCommon.mReadIOCost = sReadIOCost;
    sNestedJoinCost->mCostCommon.mOutputIOCost = sOutputIOCost;

    QLNC_SET_TOTAL_COST( &(sNestedJoinCost->mCostCommon) );
    sNestedJoinCost->mCostCommon.mResultRowCount = aExpectedResultRowCount;

    sNestedJoinCost->mLeftNode = sLeftNode;
    sNestedJoinCost->mRightNode = sRightNode;

    sNestedJoinCost->mJoinCondition = sJoinCondition;
    sNestedJoinCost->mSubQueryFilter = NULL;
    sNestedJoinCost->mJoinFilter = sJoinFilter;

    sNestedJoinCost->mIsInverted = STL_FALSE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Inverted Nested Semi Join Cost를 계산한다.
 * @param[in]   aCBOptParamInfo             Cost Based Optimizer Parameter Info
 * @param[in]   aJoinDirect                 Join Direct
 * @param[in]   aExpectedResultRowCount     Expected Result Row Count
 * @param[in]   aEnv                        Environment
 */
stlStatus qlncCBOptInvertedNestedSemiJoinCost( qlncCBOptParamInfo   * aCBOptParamInfo,
                                               qlncJoinDirect         aJoinDirect,
                                               stlFloat64             aExpectedResultRowCount,
                                               qllEnv               * aEnv )
{
    qlncNodeCommon      * sLeftNode         = NULL;
    qlncNodeCommon      * sRightNode        = NULL;

    qlncAndFilter       * sJoinCondition    = NULL;
    qlncAndFilter       * sJoinFilter       = NULL;

    qlncNestedJoinCost  * sNestedJoinCost   = NULL;

    stlBool               sNeedSortInstant;
    smlSortTableAttr      sSortTableAttr    = { STL_TRUE,      /* top-down */
                                                STL_FALSE,     /* not volatile */
                                                STL_FALSE,     /* not leaf-only */
                                                STL_TRUE,      /* unique */
                                                STL_TRUE };    /* null excluded */

    stlFloat64            sFilterCost;
    stlFloat64            sBuildCost;
    stlFloat64            sReadIOCost;
    stlFloat64            sOutputIOCost;
    stlFloat64            sRowCount;
    stlFloat64            sTmpFilterCost;
    stlFloat64            sPhysicalRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mRightNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Nested Join Cost 설정 */
    sNestedJoinCost = (qlncNestedJoinCost*)(aCBOptParamInfo->mCost);
    sNestedJoinCost->mCopiedBaseNode = NULL;

    /* Left Node 및 Right Node 설정 */
    sLeftNode = aCBOptParamInfo->mRightNode;
    sRightNode = aCBOptParamInfo->mLeftNode;

    /* Join Condition 및 Join Filter 설정 */
    STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                aCBOptParamInfo->mJoinCond,
                                &sJoinCondition,
                                aEnv )
             == STL_SUCCESS );
    sJoinFilter = aCBOptParamInfo->mFilter;

    /* Left Node 설정 */
    STL_TRY( qlncCBOptNeedUniqueCheckForINL( &(aCBOptParamInfo->mParamInfo),
                                             sLeftNode,
                                             sJoinCondition,
                                             &sNeedSortInstant,
                                             aEnv )
             == STL_SUCCESS );

    if( sNeedSortInstant == STL_TRUE )
    {
        STL_TRY( qlncCBOptMakeSortInstantForNestedLoops( &(aCBOptParamInfo->mParamInfo),
                                                         &sSortTableAttr,
                                                         sLeftNode,
                                                         sJoinCondition,
                                                         STL_FALSE,
                                                         &sJoinCondition,
                                                         &sLeftNode,
                                                         aEnv )
                 == STL_SUCCESS );
        STL_DASSERT( sLeftNode != NULL );

        /* Sort Instant Cost 계산 */
        aCBOptParamInfo->mLeftNode = sLeftNode;
        STL_TRY( qlncCBOptSortInstant( aCBOptParamInfo,
                                       aEnv )
                 == STL_SUCCESS );
    }

    /* Right Node 설정 */

    /* Base Table Node 복사 */
    STL_TRY( qlncCopyBaseNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                             sRightNode,
                                             &sRightNode,
                                             aEnv )
             == STL_SUCCESS );

    /* Push Filter 설정 */
    QLNC_SET_PUSH_FILTER( &(aCBOptParamInfo->mParamInfo),
                          ((qlncBaseTableNode*)sRightNode)->mFilter,
                          sJoinCondition,
                          aEnv );
    sJoinCondition = NULL;

    /* Base Table Node Cost 계산 */
    aCBOptParamInfo->mLeftNode = sRightNode;
    STL_TRY( qlncCBOptCopiedBaseTable( aCBOptParamInfo,
                                       aEnv )
             == STL_SUCCESS );

    /* Join Filter 설정 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sJoinFilter,
                                    &sJoinFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /* Join And Filter 재정렬 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sJoinFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Nested Join Cost 계산
     */

    QLNC_SET_BASIC_NL_COST( sLeftNode,
                            sRightNode,
                            sFilterCost,
                            sBuildCost,
                            sReadIOCost,
                            sOutputIOCost );


    if( aJoinDirect == QLNC_JOIN_DIRECT_LEFT )
    {
        sRowCount = sLeftNode->mBestCost->mResultRowCount;
    }
    else
    {
        /* @todo 현재 여기로 들어오는 경우는 없는 것으로 판단됨 */

        STL_DASSERT( aJoinDirect == QLNC_JOIN_DIRECT_RIGHT );
        sRowCount = sRightNode->mBestCost->mResultRowCount;
    }

    /* Join Condition Cost */
    STL_DASSERT( sJoinCondition == NULL );

    /* Join Filter Cost */
    if( sJoinFilter == NULL )
    {
        sTmpFilterCost = (stlFloat64)0.0;
    }
    else
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sJoinFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    sFilterCost += sTmpFilterCost;

    /* Nested Join Cost 정보 설정 */
    sNestedJoinCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sNestedJoinCost->mCostCommon.mNextFilterCost = sFilterCost + QLNC_OUTPUT_COST_PER_ROW;
    sNestedJoinCost->mCostCommon.mBuildCost = sBuildCost;
    sNestedJoinCost->mCostCommon.mReadIOCost = sReadIOCost;
    sNestedJoinCost->mCostCommon.mOutputIOCost = sOutputIOCost;

    QLNC_SET_TOTAL_COST( &(sNestedJoinCost->mCostCommon) );
    sNestedJoinCost->mCostCommon.mResultRowCount = aExpectedResultRowCount;

    sNestedJoinCost->mLeftNode = sLeftNode;
    sNestedJoinCost->mRightNode = sRightNode;

    sNestedJoinCost->mJoinCondition = sJoinCondition;
    sNestedJoinCost->mSubQueryFilter = NULL;
    sNestedJoinCost->mJoinFilter = sJoinFilter;

    sNestedJoinCost->mIsInverted = STL_TRUE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Merge Semi Join Cost를 계산한다.
 * @param[in]   aCBOptParamInfo             Cost Based Optimizer Parameter Info
 * @param[in]   aJoinDirect                 Join Direct
 * @param[in]   aExpectedResultRowCount     Expected Result Row Count
 * @param[in]   aEnv                        Environment
 */
stlStatus qlncCBOptMergeSemiJoinCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qlncJoinDirect          aJoinDirect,
                                      stlFloat64              aExpectedResultRowCount,
                                      qllEnv                * aEnv )
{
    stlInt32              i;

    qlncMergeJoinCost   * sMergeJoinCost        = NULL;
    qlncNodeCommon      * sLeftNode             = NULL;
    qlncNodeCommon      * sRightNode            = NULL;

    qlncAndFilter       * sEquiJoinCondition    = NULL;
    qlncAndFilter       * sNonEquiJoinCondition = NULL;
    qlncAndFilter       * sJoinFilter           = NULL;

    stlInt32              sSortKeyCount;
    qlncExprCommon     ** sLeftSortKeyArr       = NULL;
    qlncExprCommon     ** sRightSortKeyArr      = NULL;
    stlBool               sIsLeftPureColumn;
    stlBool               sIsRightPureColumn;
    qlncIndexScanInfo   * sLeftUsableIndexScanInfo  = NULL;
    qlncIndexScanInfo   * sRightUsableIndexScanInfo = NULL;

    qlncNodeCommon      * sCopiedNode           = NULL;

    stlUInt8            * sSortKeyFlags         = NULL;
    smlSortTableAttr      sSortTableAttr = { STL_FALSE,     /* not top-down */
                                             STL_FALSE,     /* not volatile */
                                             STL_TRUE,      /* leaf-only */
                                             STL_FALSE,     /* not unique */
                                             STL_FALSE };   /* not null excluded */

    ellIndexColumnOrdering    sLeftIndexOrdering;
    ellIndexColumnOrdering    sRightIndexOrdering;

    stlFloat64            sFilterCost;
    stlFloat64            sBuildCost;
    stlFloat64            sReadIOCost;
    stlFloat64            sOutputIOCost;
    stlFloat64            sRowCount;
    stlFloat64            sTmpFilterCost;
    stlFloat64            sPhysicalRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mRightNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mJoinCond != NULL, QLL_ERROR_STACK(aEnv) );


    /* Merge Join Cost 설정 */
    sMergeJoinCost = (qlncMergeJoinCost*)(aCBOptParamInfo->mCost);

    /* Node 설정 */
    sLeftNode = aCBOptParamInfo->mLeftNode;
    sRightNode = aCBOptParamInfo->mRightNode;

    /* Join Condition 분리 */
    STL_TRY( qlncSeperateJoinConditionForMergeJoin( &(aCBOptParamInfo->mParamInfo),
                                                    aCBOptParamInfo->mJoinCond,
                                                    &sEquiJoinCondition,
                                                    &sNonEquiJoinCondition,
                                                    aEnv )
             == STL_SUCCESS );

    /* Equi-Join Condition And Filter 재정렬 */
    if( sEquiJoinCondition != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sEquiJoinCondition,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Non Equi-Join Condition And Filter 재정렬 */
    if( sNonEquiJoinCondition != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sNonEquiJoinCondition,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Join Filter 설정 */
    if( aCBOptParamInfo->mFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    aCBOptParamInfo->mFilter,
                                    &sJoinFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sJoinFilter = NULL;
    }

    /* Join And Filter 재정렬 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sJoinFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    STL_DASSERT( sEquiJoinCondition != NULL );


    /**
     * Sort Key 수집
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sEquiJoinCondition->mOrCount,
                                (void**) &sLeftSortKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sEquiJoinCondition->mOrCount,
                                (void**) &sRightSortKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sSortKeyCount = sEquiJoinCondition->mOrCount;
    sIsLeftPureColumn = STL_TRUE;
    sIsRightPureColumn = STL_TRUE;
    for( i = 0; i < sEquiJoinCondition->mOrCount; i++ )
    {
        qlncBooleanFilter   * sBooleanFilter    = NULL;

        STL_DASSERT( sEquiJoinCondition->mOrFilters[i]->mFilterCount == 1 );
        sBooleanFilter = (qlncBooleanFilter*)(sEquiJoinCondition->mOrFilters[i]->mFilters[0]);
        if( qlncIsExistNode( sLeftNode,
                             sBooleanFilter->mLeftRefNodeList->mHead->mNode ) )
        {
            sLeftSortKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
            sRightSortKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
        }
        else
        {
            sLeftSortKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
            sRightSortKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
        }

        /* Sort Key가 Column으로만 구성되었는지 여부 체크 */
        if( sLeftSortKeyArr[i]->mType != QLNC_EXPR_TYPE_COLUMN )
        {
            sIsLeftPureColumn = STL_FALSE;
        }

        if( sRightSortKeyArr[i]->mType != QLNC_EXPR_TYPE_COLUMN )
        {
            sIsRightPureColumn = STL_FALSE;
        }
    }

    /* Left Node에 대해 Index Scan이 가능한지 여부 체크 */
    if( sIsLeftPureColumn == STL_TRUE )
    {
        if( (sLeftNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE) &&
            (qlncHasSubQuery( sLeftNode ) == STL_FALSE) )
        {
            (void)qlncFindIndexForSortKey( (qlncBaseTableNode*)sLeftNode,
                                           sSortKeyCount,
                                           sLeftSortKeyArr,
                                           STL_FALSE, /* Full Match */
                                           STL_FALSE, /* Only Unique */
                                           &sLeftUsableIndexScanInfo,
                                           &sLeftIndexOrdering );
        }
        else
        {
            /* Sort Instant 생성 필요 */
            sLeftUsableIndexScanInfo = NULL;
        }
    }
    else
    {
        /* Sort Instant 생성 필요 */
        sLeftUsableIndexScanInfo = NULL;
    }

    /* Right Node에 대해 Index Scan이 가능한지 여부 체크 */
    if( sIsRightPureColumn == STL_TRUE )
    {
        if( (sRightNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE) &&
            (qlncHasSubQuery( sRightNode ) == STL_FALSE) )
        {
            (void)qlncFindIndexForSortKey( (qlncBaseTableNode*)sRightNode,
                                           sSortKeyCount,
                                           sRightSortKeyArr,
                                           STL_TRUE, /* Full Match */
                                           STL_TRUE, /* Only Unique */
                                           &sRightUsableIndexScanInfo,
                                           &sRightIndexOrdering );
        }
        else
        {
            /* Sort Instant 생성 필요 */
            sRightUsableIndexScanInfo = NULL;
        }
    }
    else
    {
        /* Sort Instant 생성 필요 */
        sRightUsableIndexScanInfo = NULL;
    }


    /**
     * Sort Merge Cost 계산
     */

    /* Left Node Cost */
    if( sLeftUsableIndexScanInfo != NULL )
    {
        /* Index Scan이 가능한 경우 */

        /* Left Node 복사 */
        STL_TRY( qlncCopyBaseNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                 sLeftNode,
                                                 &sCopiedNode,
                                                 aEnv )
                 == STL_SUCCESS );

        /* Index Scan Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncIndexScanCost ),
                                    (void**) &(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        ((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostCount = 1;

        /* Index Scan Cost 계산 */
        aCBOptParamInfo->mLeftNode = sCopiedNode;
        aCBOptParamInfo->mFilter = ((qlncBaseTableNode*)sCopiedNode)->mFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr);
        STL_TRY( qlncCBOptIndexScanCost( aCBOptParamInfo,
                                         sLeftUsableIndexScanInfo,
                                         (sLeftIndexOrdering == ELL_INDEX_COLUMN_ASCENDING
                                          ? STL_TRUE : STL_FALSE),
                                         NULL,
                                         aEnv )
                 == STL_SUCCESS );
        aCBOptParamInfo->mFilter = NULL;

        sCopiedNode->mBestOptType = QLNC_BEST_OPT_TYPE_INDEX_SCAN;
        sCopiedNode->mBestCost = &(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr[0].mCostCommon);
        sCopiedNode->mIsAnalyzedCBO = STL_TRUE;

        sLeftNode = sCopiedNode;
    }
    else
    {
        /* Sort Instant를 생성해야 하는 경우 */

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( stlUInt8 ) * sSortKeyCount,
                                    (void**) &sSortKeyFlags,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sSortKeyFlags, 0x00, STL_SIZEOF( stlInt8 ) * sSortKeyCount );

        for( i = 0; i < sSortKeyCount; i++ )
        {
            DTL_SET_INDEX_COLUMN_FLAG( sSortKeyFlags[i],
                                       DTL_KEYCOLUMN_INDEX_ORDER_ASC,
                                       DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE,
                                       DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST );
        }

        STL_TRY( qlncCreateSortInstantForMergeJoin( &(aCBOptParamInfo->mParamInfo),
                                                    QLNC_INSTANT_TYPE_NORMAL,
                                                    &sSortTableAttr,
                                                    ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                                    sSortKeyCount,
                                                    sLeftSortKeyArr,
                                                    sSortKeyFlags,
                                                    sLeftNode,
                                                    &sCopiedNode,
                                                    aEnv )
                 == STL_SUCCESS );

        /* Sort Instant Cost 계산 */
        aCBOptParamInfo->mLeftNode = sCopiedNode;
        STL_TRY( qlncCBOptSortInstant( aCBOptParamInfo,
                                       aEnv )
                 == STL_SUCCESS );

        sLeftNode = sCopiedNode;
    }

    /* Right Node Cost */
    if( sRightUsableIndexScanInfo != NULL )
    {
        /* Index Scan이 가능한 경우 */

        /* Right Node 복사 */
        STL_TRY( qlncCopyBaseNodeWithoutCBOInfo( &(aCBOptParamInfo->mParamInfo),
                                                 sRightNode,
                                                 &sCopiedNode,
                                                 aEnv )
                 == STL_SUCCESS );

        /* Index Scan Cost 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncIndexScanCost ),
                                    (void**) &(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        ((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostCount = 1;

        /* Index Scan Cost 계산 */
        aCBOptParamInfo->mLeftNode = sCopiedNode;
        aCBOptParamInfo->mFilter = ((qlncBaseTableNode*)sCopiedNode)->mFilter;
        aCBOptParamInfo->mCost = (qlncCostCommon*)(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr);
        STL_TRY( qlncCBOptIndexScanCost( aCBOptParamInfo,
                                         sRightUsableIndexScanInfo,
                                         (sRightIndexOrdering == ELL_INDEX_COLUMN_ASCENDING
                                          ? STL_TRUE : STL_FALSE),
                                         NULL,
                                         aEnv )
                 == STL_SUCCESS );
        aCBOptParamInfo->mFilter = NULL;

        sCopiedNode->mBestOptType = QLNC_BEST_OPT_TYPE_INDEX_SCAN;
        sCopiedNode->mBestCost = &(((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr[0].mCostCommon);
        sCopiedNode->mIsAnalyzedCBO = STL_TRUE;

        sRightNode = sCopiedNode;
    }
    else
    {
        /* Sort Instant를 생성해야 하는 경우 */

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( stlUInt8 ) * sSortKeyCount,
                                    (void**) &sSortKeyFlags,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sSortKeyFlags, 0x00, STL_SIZEOF( stlInt8 ) * sSortKeyCount );

        for( i = 0; i < sSortKeyCount; i++ )
        {
            DTL_SET_INDEX_COLUMN_FLAG( sSortKeyFlags[i],
                                       DTL_KEYCOLUMN_INDEX_ORDER_ASC,
                                       DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE,
                                       DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST );
        }

        sSortTableAttr.mDistinct = STL_TRUE;
        sSortTableAttr.mNullExcluded = STL_TRUE;
        sSortTableAttr.mTopDown = STL_TRUE;
        STL_TRY( qlncCreateSortInstantForMergeJoin( &(aCBOptParamInfo->mParamInfo),
                                                    QLNC_INSTANT_TYPE_JOIN,
                                                    &sSortTableAttr,
                                                    ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                                    sSortKeyCount,
                                                    sRightSortKeyArr,
                                                    sSortKeyFlags,
                                                    sRightNode,
                                                    &sCopiedNode,
                                                    aEnv )
                 == STL_SUCCESS );

        /* Sort Instant Cost 계산 */
        aCBOptParamInfo->mLeftNode = sCopiedNode;
        STL_TRY( qlncCBOptSortInstant( aCBOptParamInfo,
                                       aEnv )
                 == STL_SUCCESS );

        sRightNode = sCopiedNode;
    }


    QLNC_SET_BASIC_MERGE_COST( sLeftNode,
                               sRightNode,
                               sFilterCost,
                               sBuildCost,
                               sReadIOCost,
                               sOutputIOCost );

    /* Filter Cost 와 Row Count 계산 */
    STL_DASSERT( sEquiJoinCondition != NULL );

    /* Equi-Join Condition Cost */
    sFilterCost +=
        ( ( sLeftNode->mBestCost->mResultRowCount +
            sRightNode->mBestCost->mResultRowCount ) *
          sEquiJoinCondition->mOrCount *
          QLNC_SORT_KEY_COMPARE_COST_PER_KEY );

    /* 다른 Join Method와 동일한 조건으로 계산하기 위하여 재보정 */
    sRowCount = aExpectedResultRowCount;

    /* Non Equi-Join Condition Cost */
    if( sNonEquiJoinCondition != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sNonEquiJoinCondition,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost +=
            ( sTmpFilterCost / (sRowCount < (stlFloat64)1.0 ? (stlFloat64)1.0 : sRowCount) );
    }

    /* Join Filter Cost */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sJoinFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost +=
            ( sTmpFilterCost / (sRowCount < (stlFloat64)1.0 ? (stlFloat64)1.0 : sRowCount) );
    }

    /* Merge Join Cost 정보 설정 */
    sMergeJoinCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sMergeJoinCost->mCostCommon.mNextFilterCost = sFilterCost + QLNC_OUTPUT_COST_PER_ROW;
    sMergeJoinCost->mCostCommon.mBuildCost = sBuildCost;
    sMergeJoinCost->mCostCommon.mReadIOCost = sReadIOCost;
    sMergeJoinCost->mCostCommon.mOutputIOCost = sOutputIOCost;

    QLNC_SET_TOTAL_COST( &(sMergeJoinCost->mCostCommon) );
    sMergeJoinCost->mCostCommon.mResultRowCount = aExpectedResultRowCount;

    sMergeJoinCost->mLeftNode = sLeftNode;
    sMergeJoinCost->mRightNode = sRightNode;

    sMergeJoinCost->mEquiJoinCondition = sEquiJoinCondition;
    sMergeJoinCost->mNonEquiJoinCondition = sNonEquiJoinCondition;
    sMergeJoinCost->mJoinFilter = sJoinFilter;

    sMergeJoinCost->mSortKeyCount = sSortKeyCount;
    sMergeJoinCost->mLeftSortKeyArr = sLeftSortKeyArr;
    sMergeJoinCost->mRightSortKeyArr = sRightSortKeyArr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Semi Join Cost를 계산한다.
 * @param[in]   aCBOptParamInfo             Cost Based Optimizer Parameter Info
 * @param[in]   aJoinDirect                 Join Direct
 * @param[in]   aExpectedResultRowCount     Expected Result Row Count
 * @param[in]   aEnv                        Environment
 */
stlStatus qlncCBOptHashSemiJoinCost( qlncCBOptParamInfo     * aCBOptParamInfo,
                                     qlncJoinDirect           aJoinDirect,
                                     stlFloat64               aExpectedResultRowCount,
                                     qllEnv                 * aEnv )
{
    stlInt32              i;

    qlncHashJoinCost    * sHashJoinCost         = NULL;
    qlncNodeCommon      * sLeftNode             = NULL;
    qlncNodeCommon      * sRightNode            = NULL;

    qlncAndFilter       * sEquiJoinCondition    = NULL;
    qlncAndFilter       * sNonEquiJoinCondition = NULL;
    qlncAndFilter       * sJoinFilter           = NULL;
    qlncAndFilter       * sPushFilter           = NULL;
    qlncAndFilter         sTmpAndFilter;

    stlInt32              sHashKeyCount;
    qlncExprCommon     ** sLeftHashKeyArr       = NULL;
    qlncExprCommon     ** sRightHashKeyArr      = NULL;

    qlncNodeCommon      * sCopiedNode           = NULL;

    smlIndexAttr          sHashTableAttr;

    stlFloat64            sFilterCost;
    stlFloat64            sBuildCost;
    stlFloat64            sReadIOCost;
    stlFloat64            sOutputIOCost;
    stlFloat64            sTmpFilterCost;
    stlFloat64            sRowCount;
    stlFloat64            sPhysicalRowCount;
    stlBool               sUniquenessFlag = STL_TRUE;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mRightNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mJoinCond != NULL, QLL_ERROR_STACK(aEnv) );


    /* Hash Join Cost 설정 */
    sHashJoinCost = (qlncHashJoinCost*)(aCBOptParamInfo->mCost);

    /* Node 설정 */
    sLeftNode = aCBOptParamInfo->mLeftNode;
    sRightNode = aCBOptParamInfo->mRightNode;

    /* Join Condition 분리 */
    STL_TRY( qlncSeperateJoinConditionForHashJoin( &(aCBOptParamInfo->mParamInfo),
                                                   sRightNode,
                                                   aCBOptParamInfo->mJoinCond,
                                                   &sEquiJoinCondition,
                                                   &sNonEquiJoinCondition,
                                                   aEnv )
             == STL_SUCCESS );

    /* Equi-Join Condition And Filter 재정렬 */
    if( sEquiJoinCondition != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sEquiJoinCondition,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Non Equi-Join Condition And Filter 재정렬 */
    if( sNonEquiJoinCondition != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sNonEquiJoinCondition,
                                            aEnv )
                 == STL_SUCCESS );

        sUniquenessFlag = STL_FALSE;
    }

    /* Join Filter 설정 */
    if( aCBOptParamInfo->mFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    aCBOptParamInfo->mFilter,
                                    &sJoinFilter,
                                    aEnv )
                 == STL_SUCCESS );

        sUniquenessFlag = STL_FALSE;
    }
    else
    {
        sJoinFilter = NULL;
    }

    /* Join And Filter 재정렬 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sJoinFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    STL_DASSERT( sEquiJoinCondition != NULL );


    /**
     * Hash Key 수집
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sEquiJoinCondition->mOrCount,
                                (void**) &sLeftHashKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sEquiJoinCondition->mOrCount,
                                (void**) &sRightHashKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sHashKeyCount = sEquiJoinCondition->mOrCount;
    for( i = 0; i < sEquiJoinCondition->mOrCount; i++ )
    {
        qlncBooleanFilter   * sBooleanFilter    = NULL;

        STL_DASSERT( sEquiJoinCondition->mOrFilters[i]->mFilterCount == 1 );
        sBooleanFilter = (qlncBooleanFilter*)(sEquiJoinCondition->mOrFilters[i]->mFilters[0]);
        if( qlncIsExistNode( sRightNode,
                             sBooleanFilter->mRightRefNodeList->mHead->mNode ) )
        {
            sLeftHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
            sRightHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
        }
        else
        {
            STL_DASSERT( qlncIsExistNode( sRightNode,
                                          sBooleanFilter->mLeftRefNodeList->mHead->mNode ) );

            sLeftHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
            sRightHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
        }
    }

    /* Hash Table Attribute 설정 */
    stlMemset( &sHashTableAttr, 0x00, STL_SIZEOF( smlIndexAttr ) );

    sHashTableAttr.mValidFlags |= SML_INDEX_LOGGING_YES;
    sHashTableAttr.mLoggingFlag = STL_FALSE;
    sHashTableAttr.mValidFlags |= SML_INDEX_UNIQUENESS_YES;
    sHashTableAttr.mUniquenessFlag = sUniquenessFlag;
    sHashTableAttr.mPrimaryFlag = STL_FALSE;
    sHashTableAttr.mValidFlags |= SML_INDEX_SKIP_NULL_YES;
    sHashTableAttr.mSkipNullFlag = STL_TRUE;

    /* Hash Instant 생성 처리 */
    STL_TRY( qlncCreateHashInstantForHashJoin( &(aCBOptParamInfo->mParamInfo),
                                               QLNC_INSTANT_TYPE_JOIN,
                                               &sHashTableAttr,
                                               ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                               sHashKeyCount,
                                               sRightHashKeyArr,
                                               sRightNode,
                                               &sCopiedNode,
                                               aEnv )
             == STL_SUCCESS );

    /* Hash Key에 대한 Expression을 생성하여 Push Filter에 추가 */
    for( i = 0; i < sHashKeyCount; i++ )
    {
        qlncBooleanFilter   * sBooleanFilter    = NULL;
        qlncExprFunction    * sFunction         = NULL;

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprFunction ),
                                    (void**) &sFunction,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sBooleanFilter = (qlncBooleanFilter*)(sEquiJoinCondition->mOrFilters[i]->mFilters[0]);
        stlMemcpy( sFunction,
                   sBooleanFilter->mExpr,
                   STL_SIZEOF( qlncExprFunction ) );

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprCommon* ) * sFunction->mArgCount,
                                    (void**) &(sFunction->mArgs),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( qlncIsExistNode( sRightNode,
                             sBooleanFilter->mRightRefNodeList->mHead->mNode ) )
        {
            sFunction->mArgs[0] = sLeftHashKeyArr[i];
            sFunction->mArgs[1] = sRightHashKeyArr[i];
        }
        else
        {
            STL_DASSERT( qlncIsExistNode( sRightNode,
                                          sBooleanFilter->mLeftRefNodeList->mHead->mNode ) );

            sFunction->mArgs[0] = sRightHashKeyArr[i];
            sFunction->mArgs[1] = sLeftHashKeyArr[i];
        }

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncBooleanFilter ),
                                    (void**) &sBooleanFilter,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sBooleanFilter,
                   sEquiJoinCondition->mOrFilters[i]->mFilters[0],
                   STL_SIZEOF( qlncBooleanFilter ) );

        sBooleanFilter->mExpr = (qlncExprCommon*)sFunction;

        if( sPushFilter == NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncAndFilter ),
                                        (void**) &sPushFilter,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_SET_EXPR_COMMON( &sPushFilter->mCommon,
                                  &QLL_CANDIDATE_MEM( aEnv ),
                                  aEnv,
                                  QLNC_EXPR_TYPE_AND_FILTER,
                                  sBooleanFilter->mExpr->mExprPhraseFlag,
                                  sBooleanFilter->mExpr->mIterationTime,
                                  sBooleanFilter->mExpr->mPosition,
                                  sBooleanFilter->mExpr->mColumnName,
                                  DTL_TYPE_BOOLEAN );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncOrFilter* ),
                                        (void**) &(sPushFilter->mOrFilters),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            sPushFilter->mOrCount = 1;

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncOrFilter ),
                                        (void**) &(sPushFilter->mOrFilters[0]),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_SET_EXPR_COMMON( &sPushFilter->mOrFilters[0]->mCommon,
                                  &QLL_CANDIDATE_MEM( aEnv ),
                                  aEnv,
                                  QLNC_EXPR_TYPE_OR_FILTER,
                                  sBooleanFilter->mExpr->mExprPhraseFlag,
                                  sBooleanFilter->mExpr->mIterationTime,
                                  sBooleanFilter->mExpr->mPosition,
                                  sBooleanFilter->mExpr->mColumnName,
                                  DTL_TYPE_BOOLEAN );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ),
                                        (void**) &(sPushFilter->mOrFilters[0]->mFilters),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            sPushFilter->mOrFilters[0]->mFilterCount = 1;
            sPushFilter->mOrFilters[0]->mFilters[0] = (qlncExprCommon*)sBooleanFilter;
            sPushFilter->mOrFilters[0]->mSubQueryExprList = NULL;
        }
        else
        {
            STL_TRY( qlncAppendFilterToAndFilter( &(aCBOptParamInfo->mParamInfo),
                                                  sPushFilter,
                                                  (qlncExprCommon*)sBooleanFilter,
                                                  aEnv )
                     == STL_SUCCESS );
        }
    }

    /* Push Filter을 Hash Instant에 추가 */
    ((qlncHashInstantNode*)sCopiedNode)->mKeyFilter = sPushFilter;
    sPushFilter = NULL;

    if( aJoinDirect == QLNC_JOIN_DIRECT_LEFT )
    {
        if( (sNonEquiJoinCondition != NULL) &&
            (sNonEquiJoinCondition->mOrCount > 0) )
        {
            ((qlncHashInstantNode*)sCopiedNode)->mRecFilter = sNonEquiJoinCondition;
        }

        sNonEquiJoinCondition = NULL;

        /* Join Filter에서 SubQuery가 존재하는 Filter를 제외한 모든 필터를 Record Filter에 추가함 */
        if( sJoinFilter != NULL )
        {
            QLNC_INIT_AND_FILTER( &sTmpAndFilter );
            for( i = 0; i < sJoinFilter->mOrCount; i++ )
            {
                if( sJoinFilter->mOrFilters[i]->mSubQueryExprList == NULL )
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                                &sTmpAndFilter,
                                                                1,
                                                                &(sJoinFilter->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );

                    sJoinFilter->mOrFilters[i] = NULL;
                }
            }

            STL_TRY( qlncCompactAndFilter( sJoinFilter,
                                           aEnv )
                     == STL_SUCCESS );

            if( sJoinFilter->mOrCount == 0 )
            {
                sJoinFilter = NULL;
            }

            if( sTmpAndFilter.mOrCount > 0 )
            {
                if( ((qlncHashInstantNode*)sCopiedNode)->mRecFilter == NULL )
                {
                    STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                                &sTmpAndFilter,
                                                &(((qlncHashInstantNode*)sCopiedNode)->mRecFilter),
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /* @todo 현재 여기에 들어오는 경우는 없는 것으로 판단됨 */

                    STL_TRY( qlncAppendOrFilterToAndFilterTail(
                                &(aCBOptParamInfo->mParamInfo),
                                ((qlncHashInstantNode*)sCopiedNode)->mRecFilter,
                                sTmpAndFilter.mOrCount,
                                sTmpAndFilter.mOrFilters,
                                aEnv )
                             == STL_SUCCESS );
                }
            }
        }
    }

    /* Hash Instant Cost 계산 */
    aCBOptParamInfo->mLeftNode = sCopiedNode;
    STL_TRY( qlncCBOptHashInstant( aCBOptParamInfo,
                                   aEnv )
             == STL_SUCCESS );

    sRightNode = sCopiedNode;


    /**
     * Cost 계산
     */

    /* Filter Cost */
    sFilterCost =
        ( sLeftNode->mBestCost->mTotalCost -
          sLeftNode->mBestCost->mBuildCost );

    sFilterCost +=
        ( sLeftNode->mBestCost->mResultRowCount *
          ( sRightNode->mBestCost->mTotalCost -
            sRightNode->mBestCost->mNextFilterCost -
            sRightNode->mBestCost->mBuildCost ) );

    /* 첫 필터 이후에는 최초 일치 한건만 찾으면 된다. */
    sFilterCost +=
        ( sLeftNode->mBestCost->mResultRowCount *
          ( sRightNode->mBestCost->mNextFilterCost /
            ( sRightNode->mBestCost->mResultRowCount < (stlFloat64)1.0
              ? (stlFloat64)1.0 : sRightNode->mBestCost->mResultRowCount ) ) );

    sFilterCost +=
        ( sLeftNode->mBestCost->mReadIOCost +
          sRightNode->mBestCost->mReadIOCost );

    sFilterCost +=
        ( sLeftNode->mBestCost->mOutputIOCost +
          sRightNode->mBestCost->mOutputIOCost );

    /* Build Cost */
    sBuildCost =
        ( sLeftNode->mBestCost->mBuildCost +
          sRightNode->mBestCost->mBuildCost );

    /* Read IO Cost */
    sReadIOCost = (stlFloat64)0.0;

    /* Output IO Cost */
    sOutputIOCost = (stlFloat64)0.0;

    /* 다른 Join Method와 동일한 조건으로 계산하기 위하여 재보정 */
    sRowCount = aExpectedResultRowCount;

    /* Non Equi-Join Condition Cost 계산 */
    if( sNonEquiJoinCondition != NULL )
    {
        /* @todo 현재 여기로 들어오는 경우는 없는 것으로 판단됨 */

        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sNonEquiJoinCondition,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost +=
            ( sTmpFilterCost / (sRowCount < (stlFloat64)1.0 ? (stlFloat64)1.0 : sRowCount) );
    }

    /* Join Filter Cost 계산 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sJoinFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost +=
            ( sTmpFilterCost / (sRowCount < (stlFloat64)1.0 ? (stlFloat64)1.0 : sRowCount) );
    }

    /* Hash Join Cost 정보 설정 */
    sHashJoinCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sHashJoinCost->mCostCommon.mNextFilterCost = sFilterCost + QLNC_OUTPUT_COST_PER_ROW;
    sHashJoinCost->mCostCommon.mBuildCost = sBuildCost;
    sHashJoinCost->mCostCommon.mReadIOCost = sReadIOCost;
    sHashJoinCost->mCostCommon.mOutputIOCost = sOutputIOCost;

    QLNC_SET_TOTAL_COST( &(sHashJoinCost->mCostCommon) );
    sHashJoinCost->mCostCommon.mResultRowCount = aExpectedResultRowCount;

    sHashJoinCost->mLeftNode = sLeftNode;
    sHashJoinCost->mRightNode = sRightNode;

    sHashJoinCost->mEquiJoinCondition = NULL;
    sHashJoinCost->mNonEquiJoinCondition = sNonEquiJoinCondition;
    sHashJoinCost->mJoinFilter = sJoinFilter;

    sHashJoinCost->mHashKeyCount = sHashKeyCount;
    sHashJoinCost->mLeftHashKeyArr = sLeftHashKeyArr;
    sHashJoinCost->mRightHashKeyArr = sRightHashKeyArr;

    sHashJoinCost->mIsInverted = STL_FALSE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Inverted Hash Semi Join Cost를 계산한다.
 * @param[in]   aCBOptParamInfo             Cost Based Optimizer Parameter Info
 * @param[in]   aJoinDirect                 Join Direct
 * @param[in]   aExpectedResultRowCount     Expected Result Row Count
 * @param[in]   aEnv                        Environment
 */
stlStatus qlncCBOptInvertedHashSemiJoinCost( qlncCBOptParamInfo * aCBOptParamInfo,
                                             qlncJoinDirect       aJoinDirect,
                                             stlFloat64           aExpectedResultRowCount,
                                             qllEnv             * aEnv )
{
    stlInt32              i;

    qlncHashJoinCost    * sHashJoinCost         = NULL;
    qlncNodeCommon      * sLeftNode             = NULL;
    qlncNodeCommon      * sRightNode            = NULL;

    qlncAndFilter       * sEquiJoinCondition    = NULL;
    qlncAndFilter       * sNonEquiJoinCondition = NULL;
    qlncAndFilter       * sJoinFilter           = NULL;
    qlncAndFilter       * sPushFilter           = NULL;

    stlInt32              sHashKeyCount;
    qlncExprCommon     ** sLeftHashKeyArr       = NULL;
    qlncExprCommon     ** sRightHashKeyArr      = NULL;

    qlncNodeCommon      * sCopiedNode           = NULL;

    smlIndexAttr          sHashTableAttr;

    stlFloat64            sFilterCost;
    stlFloat64            sBuildCost;
    stlFloat64            sReadIOCost;
    stlFloat64            sOutputIOCost;
    stlFloat64            sTmpFilterCost;
    stlFloat64            sRowCount;
    stlFloat64            sPhysicalRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mRightNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mJoinCond != NULL, QLL_ERROR_STACK(aEnv) );


    /* Hash Join Cost 설정 */
    sHashJoinCost = (qlncHashJoinCost*)(aCBOptParamInfo->mCost);

    /* Node 설정 */
    sLeftNode = aCBOptParamInfo->mRightNode;
    sRightNode = aCBOptParamInfo->mLeftNode;

    /* Join Condition 분리 */
    STL_TRY( qlncSeperateJoinConditionForHashJoin( &(aCBOptParamInfo->mParamInfo),
                                                   sRightNode,
                                                   aCBOptParamInfo->mJoinCond,
                                                   &sEquiJoinCondition,
                                                   &sNonEquiJoinCondition,
                                                   aEnv )
             == STL_SUCCESS );

    /* Equi-Join Condition And Filter 재정렬 */
    if( sEquiJoinCondition != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sEquiJoinCondition,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Non Equi-Join Condition And Filter 재정렬 */
    if( sNonEquiJoinCondition != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sNonEquiJoinCondition,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Join Filter 설정 */
    if( aCBOptParamInfo->mFilter != NULL )
    {
        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    aCBOptParamInfo->mFilter,
                                    &sJoinFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sJoinFilter = NULL;
    }

    /* Join And Filter 재정렬 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sJoinFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    STL_DASSERT( sEquiJoinCondition != NULL );


    /**
     * Hash Key 수집
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sEquiJoinCondition->mOrCount,
                                (void**) &sLeftHashKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sEquiJoinCondition->mOrCount,
                                (void**) &sRightHashKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sHashKeyCount = sEquiJoinCondition->mOrCount;
    for( i = 0; i < sEquiJoinCondition->mOrCount; i++ )
    {
        qlncBooleanFilter   * sBooleanFilter    = NULL;

        STL_DASSERT( sEquiJoinCondition->mOrFilters[i]->mFilterCount == 1 );
        sBooleanFilter = (qlncBooleanFilter*)(sEquiJoinCondition->mOrFilters[i]->mFilters[0]);
        if( qlncIsExistNode( sRightNode,
                             sBooleanFilter->mRightRefNodeList->mHead->mNode ) )
        {
            sLeftHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
            sRightHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
        }
        else
        {
            STL_DASSERT( qlncIsExistNode( sRightNode,
                                          sBooleanFilter->mLeftRefNodeList->mHead->mNode ) );

            sLeftHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
            sRightHashKeyArr[i] =
                ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
        }
    }

    /* Hash Table Attribute 설정 */
    stlMemset( &sHashTableAttr, 0x00, STL_SIZEOF( smlIndexAttr ) );

    sHashTableAttr.mValidFlags |= SML_INDEX_LOGGING_YES;
    sHashTableAttr.mLoggingFlag = STL_FALSE;
    sHashTableAttr.mValidFlags |= SML_INDEX_UNIQUENESS_YES;
    sHashTableAttr.mUniquenessFlag = STL_FALSE;
    sHashTableAttr.mPrimaryFlag = STL_FALSE;
    sHashTableAttr.mValidFlags |= SML_INDEX_SKIP_NULL_YES;
    sHashTableAttr.mSkipNullFlag = STL_TRUE;

    /* Hash Instant 생성 처리 */
    STL_TRY( qlncCreateHashInstantForHashJoin( &(aCBOptParamInfo->mParamInfo),
                                               QLNC_INSTANT_TYPE_JOIN,
                                               &sHashTableAttr,
                                               ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                               sHashKeyCount,
                                               sRightHashKeyArr,
                                               sRightNode,
                                               &sCopiedNode,
                                               aEnv )
             == STL_SUCCESS );

    /* Hash Key에 대한 Expression을 생성하여 Push Filter에 추가 */
    for( i = 0; i < sHashKeyCount; i++ )
    {
        qlncBooleanFilter   * sBooleanFilter    = NULL;
        qlncExprFunction    * sFunction         = NULL;

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprFunction ),
                                    (void**) &sFunction,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sBooleanFilter = (qlncBooleanFilter*)(sEquiJoinCondition->mOrFilters[i]->mFilters[0]);
        stlMemcpy( sFunction,
                   sBooleanFilter->mExpr,
                   STL_SIZEOF( qlncExprFunction ) );

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprCommon* ) * sFunction->mArgCount,
                                    (void**) &(sFunction->mArgs),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( qlncIsExistNode( sRightNode,
                             sBooleanFilter->mRightRefNodeList->mHead->mNode ) )
        {
            sFunction->mArgs[0] = sLeftHashKeyArr[i];
            sFunction->mArgs[1] = sRightHashKeyArr[i];
        }
        else
        {
            STL_DASSERT( qlncIsExistNode( sRightNode,
                                          sBooleanFilter->mLeftRefNodeList->mHead->mNode ) );

            sFunction->mArgs[0] = sRightHashKeyArr[i];
            sFunction->mArgs[1] = sLeftHashKeyArr[i];
        }

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncBooleanFilter ),
                                    (void**) &sBooleanFilter,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sBooleanFilter,
                   sEquiJoinCondition->mOrFilters[i]->mFilters[0],
                   STL_SIZEOF( qlncBooleanFilter ) );

        sBooleanFilter->mExpr = (qlncExprCommon*)sFunction;

        if( sPushFilter == NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncAndFilter ),
                                        (void**) &sPushFilter,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_SET_EXPR_COMMON( &sPushFilter->mCommon,
                                  &QLL_CANDIDATE_MEM( aEnv ),
                                  aEnv,
                                  QLNC_EXPR_TYPE_AND_FILTER,
                                  sBooleanFilter->mExpr->mExprPhraseFlag,
                                  sBooleanFilter->mExpr->mIterationTime,
                                  sBooleanFilter->mExpr->mPosition,
                                  sBooleanFilter->mExpr->mColumnName,
                                  DTL_TYPE_BOOLEAN );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncOrFilter* ),
                                        (void**) &(sPushFilter->mOrFilters),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            sPushFilter->mOrCount = 1;

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncOrFilter ),
                                        (void**) &(sPushFilter->mOrFilters[0]),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_SET_EXPR_COMMON( &sPushFilter->mOrFilters[0]->mCommon,
                                  &QLL_CANDIDATE_MEM( aEnv ),
                                  aEnv,
                                  QLNC_EXPR_TYPE_OR_FILTER,
                                  sBooleanFilter->mExpr->mExprPhraseFlag,
                                  sBooleanFilter->mExpr->mIterationTime,
                                  sBooleanFilter->mExpr->mPosition,
                                  sBooleanFilter->mExpr->mColumnName,
                                  DTL_TYPE_BOOLEAN );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ),
                                        (void**) &(sPushFilter->mOrFilters[0]->mFilters),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            sPushFilter->mOrFilters[0]->mFilterCount = 1;
            sPushFilter->mOrFilters[0]->mFilters[0] = (qlncExprCommon*)sBooleanFilter;
            sPushFilter->mOrFilters[0]->mSubQueryExprList = NULL;
        }
        else
        {
            STL_TRY( qlncAppendFilterToAndFilter( &(aCBOptParamInfo->mParamInfo),
                                                  sPushFilter,
                                                  (qlncExprCommon*)sBooleanFilter,
                                                  aEnv )
                     == STL_SUCCESS );
        }
    }

    /* Push Filter을 Hash Instant에 추가 */
    ((qlncHashInstantNode*)sCopiedNode)->mKeyFilter = sPushFilter;
    sPushFilter = NULL;

    if( aJoinDirect == QLNC_JOIN_DIRECT_LEFT )
    {
        if( (sNonEquiJoinCondition != NULL) &&
            (sNonEquiJoinCondition->mOrCount > 0) )
        {
            ((qlncHashInstantNode*)sCopiedNode)->mRecFilter = sNonEquiJoinCondition;
        }

        sNonEquiJoinCondition = NULL;
    }

    /* Hash Instant Cost 계산 */
    aCBOptParamInfo->mLeftNode = sCopiedNode;
    STL_TRY( qlncCBOptHashInstant( aCBOptParamInfo,
                                   aEnv )
             == STL_SUCCESS );

    sRightNode = sCopiedNode;


    /**
     * Cost 계산
     */

    QLNC_SET_BASIC_HASH_COST( sLeftNode,
                              sRightNode,
                              sFilterCost,
                              sBuildCost,
                              sReadIOCost,
                              sOutputIOCost );

    if( aJoinDirect == QLNC_JOIN_DIRECT_LEFT )
    {
        sRowCount = sLeftNode->mBestCost->mResultRowCount;
    }
    else
    {
        /* @todo 현재 여기로 들어오는 경우는 없는 것으로 판단됨 */

        STL_DASSERT( aJoinDirect == QLNC_JOIN_DIRECT_RIGHT );
        sRowCount = sRightNode->mBestCost->mResultRowCount;
    }

    /* Join Filter Cost 계산 */
    if( sJoinFilter != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    NULL,
                                                    sJoinFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sFilterCost += sTmpFilterCost;
    }

    /* Hash Join Cost 정보 설정 */
    sHashJoinCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sHashJoinCost->mCostCommon.mNextFilterCost = sFilterCost + QLNC_OUTPUT_COST_PER_ROW;
    sHashJoinCost->mCostCommon.mBuildCost = sBuildCost;
    sHashJoinCost->mCostCommon.mReadIOCost = sReadIOCost;
    sHashJoinCost->mCostCommon.mOutputIOCost = sOutputIOCost;

    QLNC_SET_TOTAL_COST( &(sHashJoinCost->mCostCommon) );
    sHashJoinCost->mCostCommon.mResultRowCount = aExpectedResultRowCount;

    sHashJoinCost->mLeftNode = sLeftNode;
    sHashJoinCost->mRightNode = sRightNode;

    sHashJoinCost->mEquiJoinCondition = NULL;
    sHashJoinCost->mNonEquiJoinCondition = sNonEquiJoinCondition;
    sHashJoinCost->mJoinFilter = sJoinFilter;

    sHashJoinCost->mHashKeyCount = sHashKeyCount;
    sHashJoinCost->mLeftHashKeyArr = sLeftHashKeyArr;
    sHashJoinCost->mRightHashKeyArr = sRightHashKeyArr;

    sHashJoinCost->mIsInverted = STL_TRUE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


#if 0
/**
 * @brief Flat Instant의 Cost를 계산한다.
 * @param[in]   aCBOptParamInfo     Cost Based Optimizer Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncCBOptFlatInstantCost( qlncCBOptParamInfo  * aCBOptParamInfo,
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
                      "qlncCBOptFlatInstantCost()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}
#endif

/**
 * @brief Sort Instant의 Cost를 계산한다.
 * @param[in]   aCBOptParamInfo     Cost Based Optimizer Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncCBOptSortInstantCost( qlncCBOptParamInfo  * aCBOptParamInfo,
                                    qllEnv              * aEnv )
{
    qlncSortInstantNode     * sSortInstantNode  = NULL;
    qlncSortInstantCost     * sSortInstantCost  = NULL;
    stlFloat64                sTmpFilterCost;
    stlFloat64                sRowCount;
    stlFloat64                sPhysicalRowCount;
    qlncAndFilter           * sOrgFilter        = NULL;
    stlBool                   sNeedRebuild;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT,
                        QLL_ERROR_STACK(aEnv) );


    /* Sort Instant Node 설정 */
    sSortInstantNode = (qlncSortInstantNode*)(aCBOptParamInfo->mLeftNode);

    /* Sort Instant Cost 설정 */
    sSortInstantCost = (qlncSortInstantCost*)(aCBOptParamInfo->mCost);

    /* Sort Key And Filter 재정렬 */
    if( sSortInstantNode->mKeyFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sSortInstantNode->mKeyFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Record And Filter 재정렬 */
    if( sSortInstantNode->mRecFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sSortInstantNode->mRecFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Filter 재정렬 */
    if( sSortInstantNode->mFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sSortInstantNode->mFilter,
                                            aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                    sSortInstantNode->mFilter,
                                    &sOrgFilter,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOrgFilter = NULL;
    }

    /* Filter 설정 (Hash Key Filter와 Recrod Filter를 합한 것) */
    if( sSortInstantNode->mKeyFilter != NULL )
    {
        if( sSortInstantNode->mFilter == NULL )
        {
            STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                        sSortInstantNode->mKeyFilter,
                                        &(sSortInstantNode->mFilter),
                                        aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* @todo 여기에 들어오는 경우가 없는 것으로 판단됨 */

            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                        sSortInstantNode->mFilter,
                                                        sSortInstantNode->mKeyFilter->mOrCount,
                                                        sSortInstantNode->mKeyFilter->mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );
        }
    }

    if( sSortInstantNode->mRecFilter != NULL )
    {
        if( sSortInstantNode->mFilter == NULL )
        {
            /* @todo 여기에 들어올 수 없을 것으로 판단됨 */

            STL_TRY( qlncCopyAndFilter( &(aCBOptParamInfo->mParamInfo),
                                        sSortInstantNode->mRecFilter,
                                        &(sSortInstantNode->mFilter),
                                        aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aCBOptParamInfo->mParamInfo),
                                                        sSortInstantNode->mFilter,
                                                        sSortInstantNode->mRecFilter->mOrCount,
                                                        sSortInstantNode->mRecFilter->mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );
        }
    }


    /**
     * Sort Instant Cost 계산
     */

    /* Row Count */
    sRowCount = sSortInstantNode->mChildNode->mBestCost->mResultRowCount;

    if( sSortInstantNode->mSortTableAttr.mDistinct == STL_TRUE )
    {
        sRowCount *= ((stlFloat64)1.0 - QLNC_DEFAULT_COLUMN_DUP_RATE);
    }

    if( sSortInstantNode->mSortTableAttr.mNullExcluded == STL_TRUE )
    {
        sRowCount *= ((stlFloat64)1.0 - QLNC_DEFAULT_COLUMN_NULL_RATE);
    }

    /* Build Cost */

    /* Child Node 읽는 비용 */
    sSortInstantCost->mCostCommon.mBuildCost =
        sSortInstantNode->mChildNode->mBestCost->mTotalCost;

    /* Sort Instant 생성 비용 */
    sSortInstantCost->mCostCommon.mBuildCost +=
        ( sRowCount * QLNC_SORT_INSTANT_BUILD_COST_PER_ROW );

    sSortInstantCost->mCostCommon.mBuildCost +=
        ( sSortInstantNode->mKeyColList->mCount *
          QLNC_SORT_KEY_RADIX_SORT_COST_PER_KEY *
          sRowCount );

    /* Sort Instant에 데이터 저장 비용 */
    sSortInstantCost->mCostCommon.mBuildCost +=
        sSortInstantNode->mChildNode->mBestCost->mOutputIOCost;

    /* Read IO Cost */
    sSortInstantCost->mCostCommon.mReadIOCost = (stlFloat64)0.0;

    /* Output IO Cost */
    sSortInstantCost->mCostCommon.mOutputIOCost = (stlFloat64)0.0;

    /* Sort Key Filter에 대한 Cost 계산 */
    if( sSortInstantNode->mSortTableAttr.mDistinct == STL_TRUE )
    {
        sSortInstantCost->mCostCommon.mFirstFilterCost =
            QLNC_UNIQ_IDX_RANGE_COST_PER_SEARCH;
    }
    else
    {
        sSortInstantCost->mCostCommon.mFirstFilterCost =
            QLNC_NON_UNIQ_IDX_RANGE_COST_PER_SEARCH;
    }

    /* Next Filter Cost */
    if( sSortInstantNode->mInstantType == QLNC_INSTANT_TYPE_JOIN )
    {
        sSortInstantCost->mCostCommon.mNextFilterCost =
            ( sSortInstantCost->mCostCommon.mFirstFilterCost +
              QLNC_SORT_KEY_COMPARE_COST_PER_KEY );
    }
    else
    {
        sSortInstantCost->mCostCommon.mNextFilterCost = (stlFloat64)0.0;
    }

    if( (sSortInstantNode->mRecFilter != NULL) ||
        (sOrgFilter != NULL) )
    {
        sSortInstantCost->mCostCommon.mNextFilterCost += QLNC_FILTER_PREPARE_COST;
    }

    /* key Filter에 의한 row 계산 */
    if( sSortInstantNode->mKeyFilter != NULL )
    {
        sRowCount = qlncGetExpectedAndFilterResultCount( sSortInstantNode->mKeyFilter,
                                                         STL_FALSE,
                                                         sRowCount );
    }

    /* Record Filter에 의한 Cost 계산 */
    if( sSortInstantNode->mRecFilter != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    aCBOptParamInfo->mLeftNode,
                                                    sSortInstantNode->mRecFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sSortInstantCost->mCostCommon.mNextFilterCost += sTmpFilterCost;
    }

    /* Filter에 의한 Cost 계산 */
    if( sOrgFilter != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    aCBOptParamInfo->mLeftNode,
                                                    sOrgFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sSortInstantCost->mCostCommon.mNextFilterCost += sTmpFilterCost;
    }

    sSortInstantCost->mCostCommon.mNextFilterCost += QLNC_OUTPUT_COST_PER_ROW;

    /* Rebuild 필요 여부 체크 */
    STL_TRY( qlncNeedRebuildInstant( (qlncNodeCommon*)sSortInstantNode,
                                     &sNeedRebuild,
                                     aEnv )
             == STL_SUCCESS );

    if( sNeedRebuild == STL_TRUE )
    {
        sSortInstantCost->mCostCommon.mFirstFilterCost +=
            sSortInstantCost->mCostCommon.mBuildCost;
        sSortInstantCost->mCostCommon.mBuildCost = (stlFloat64)0.0;
    }

    QLNC_SET_TOTAL_COST( &(sSortInstantCost->mCostCommon) );
    sSortInstantCost->mCostCommon.mResultRowCount = sRowCount;

    /* Sort Instant Node에 Sort Instant Cost 설정 */
    sSortInstantNode->mSortInstantCost = sSortInstantCost;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Instant의 Cost를 계산한다.
 * @param[in]   aCBOptParamInfo     Cost Based Optimizer Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncCBOptHashInstantCost( qlncCBOptParamInfo  * aCBOptParamInfo,
                                    qllEnv              * aEnv )
{
    qlncHashInstantNode     * sHashInstantNode  = NULL;
    qlncHashInstantCost     * sHashInstantCost  = NULL;
    stlFloat64                sTmpFilterCost;
    stlFloat64                sRowCount;
    stlFloat64                sPhysicalRowCount;
    stlBool                   sNeedRebuild;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT,
                        QLL_ERROR_STACK(aEnv) );


    /* Hash Instant Node 설정 */
    sHashInstantNode = (qlncHashInstantNode*)(aCBOptParamInfo->mLeftNode);

    /* Hash Instant Cost 설정 */
    sHashInstantCost = (qlncHashInstantCost*)(aCBOptParamInfo->mCost);

    /* Hash Key And Filter 재정렬 */
    if( sHashInstantNode->mKeyFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sHashInstantNode->mKeyFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Record And Filter 재정렬 */
    if( sHashInstantNode->mRecFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sHashInstantNode->mRecFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Filter 재정렬 */
    if( sHashInstantNode->mFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sHashInstantNode->mFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Hash Instant Cost 계산
     */

    /* Row Count */
    sRowCount = sHashInstantNode->mChildNode->mBestCost->mResultRowCount;

    if( sHashInstantNode->mInstantType == QLNC_INSTANT_TYPE_GROUP )
    {
        sRowCount *= QLNC_EQUAL_OPER_RESULT_RATE;
    }

    if( ((sHashInstantNode->mHashTableAttr.mValidFlags & SML_INDEX_UNIQUENESS_MASK) == SML_INDEX_UNIQUENESS_YES) &&
        (sHashInstantNode->mHashTableAttr.mUniquenessFlag == STL_TRUE) )
    {
        sRowCount *= ((stlFloat64)1.0 - QLNC_DEFAULT_COLUMN_DUP_RATE);
    }

    if( ((sHashInstantNode->mHashTableAttr.mValidFlags & SML_INDEX_SKIP_NULL_MASK) == SML_INDEX_SKIP_NULL_YES) &&
        (sHashInstantNode->mHashTableAttr.mSkipNullFlag == STL_TRUE) )
    {
        sRowCount *= ((stlFloat64)1.0 - QLNC_DEFAULT_COLUMN_DUP_RATE);
    }

    /* Build Cost */

    /* Child Node 읽는 비용 */
    sHashInstantCost->mCostCommon.mBuildCost =
        sHashInstantNode->mChildNode->mBestCost->mTotalCost;

    /* Hash Instant 생성 비용 */
    sHashInstantCost->mCostCommon.mBuildCost +=
        ( sRowCount * QLNC_HASH_INSTANT_BUILD_COST_PER_ROW );

    /* Hash Key 생성 및 저장에 대한 Cost 계산 */
    if( sRowCount <= QLNC_HASH_SECTION1_MAX_ROW_COUNT )
    {
        sHashInstantCost->mCostCommon.mBuildCost +=
            (sRowCount * QLNC_HASH_KEY_FILTER_COST_PER_ROW_SEC1);
    }
    else if( sRowCount <= QLNC_HASH_SECTION2_MAX_ROW_COUNT )
    {
        sHashInstantCost->mCostCommon.mBuildCost +=
            (sRowCount * QLNC_HASH_KEY_FILTER_COST_PER_ROW_SEC2);
    }
    else
    {
        sHashInstantCost->mCostCommon.mBuildCost +=
            (sRowCount * QLNC_HASH_KEY_FILTER_COST_PER_ROW_SEC3);
    }

    /* Hash Instant에 데이터 저장 비용 */
    sHashInstantCost->mCostCommon.mBuildCost +=
        sHashInstantNode->mChildNode->mBestCost->mOutputIOCost;

    /* Read IO Cost */
    sHashInstantCost->mCostCommon.mReadIOCost = (stlFloat64)0.0;

    /* Output IO Cost */
    sHashInstantCost->mCostCommon.mOutputIOCost = (stlFloat64)0.0;

    /* Hash Key Filter에 대한 Cost 계산 */
    if( sRowCount <= QLNC_HASH_SECTION1_MAX_ROW_COUNT )
    {
        sHashInstantCost->mCostCommon.mFirstFilterCost =
            QLNC_HASH_KEY_FILTER_COST_PER_ROW_SEC1;
    }
    else if( sRowCount <= QLNC_HASH_SECTION2_MAX_ROW_COUNT )
    {
        sHashInstantCost->mCostCommon.mFirstFilterCost =
            QLNC_HASH_KEY_FILTER_COST_PER_ROW_SEC2;
    }
    else
    {
        sHashInstantCost->mCostCommon.mFirstFilterCost =
            QLNC_HASH_KEY_FILTER_COST_PER_ROW_SEC3;
    }

    if( (sHashInstantNode->mRecFilter != NULL) ||
        (sHashInstantNode->mFilter != NULL) )
    {
        sHashInstantCost->mCostCommon.mNextFilterCost = QLNC_FILTER_PREPARE_COST;
    }
    else
    {
        sHashInstantCost->mCostCommon.mNextFilterCost = (stlFloat64)0.0;
    }

    /* Key Filter에 의한 row 계산 */
    if( sHashInstantNode->mKeyFilter != NULL )
    {
        sRowCount = qlncGetExpectedAndFilterResultCount( sHashInstantNode->mKeyFilter,
                                                         STL_FALSE,
                                                         sRowCount );
    }

    /* Record Filter에 의한 Cost 계산 */
    if( sHashInstantNode->mRecFilter != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    aCBOptParamInfo->mLeftNode,
                                                    sHashInstantNode->mRecFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sHashInstantCost->mCostCommon.mNextFilterCost += sTmpFilterCost;
    }

    /* Read Column Filter에 의한 Cost 계산 */
    if( sHashInstantNode->mFilter != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    aCBOptParamInfo->mLeftNode,
                                                    sHashInstantNode->mFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sHashInstantCost->mCostCommon.mNextFilterCost += sTmpFilterCost;
    }

    sHashInstantCost->mCostCommon.mNextFilterCost += QLNC_OUTPUT_COST_PER_ROW;

    /* Rebuild 필요 여부 체크 */
    STL_TRY( qlncNeedRebuildInstant( (qlncNodeCommon*)sHashInstantNode,
                                     &sNeedRebuild,
                                     aEnv )
             == STL_SUCCESS );

    if( sNeedRebuild == STL_TRUE )
    {
        sHashInstantCost->mCostCommon.mFirstFilterCost +=
            sHashInstantCost->mCostCommon.mBuildCost;
        sHashInstantCost->mCostCommon.mBuildCost = (stlFloat64)0.0;
    }

    QLNC_SET_TOTAL_COST( &(sHashInstantCost->mCostCommon) );
    sHashInstantCost->mCostCommon.mResultRowCount = sRowCount;

    /* Hash Instant Node에 Hash Instant Cost 설정 */
    sHashInstantNode->mHashInstantCost = sHashInstantCost;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Group의 Cost를 계산한다.
 * @param[in]   aCBOptParamInfo     Cost Based Optimizer Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncCBOptGroupCost( qlncCBOptParamInfo    * aCBOptParamInfo,
                              qllEnv                * aEnv )
{
    qlncHashInstantNode     * sHashInstantNode  = NULL;
    qlncGroupCost           * sGroupCost        = NULL;
    stlFloat64                sTmpFilterCost;
    stlFloat64                sRowCount;
    stlFloat64                sPhysicalRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT,
                        QLL_ERROR_STACK(aEnv) );


    /* Hash Instant Node 설정 */
    sHashInstantNode = (qlncHashInstantNode*)(aCBOptParamInfo->mLeftNode);

    /* Group Cost 설정 */
    sGroupCost = (qlncGroupCost*)(aCBOptParamInfo->mCost);

    /* Hash Key And Filter 재정렬 */
    if( sHashInstantNode->mKeyFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sHashInstantNode->mKeyFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Record And Filter 재정렬 */
    if( sHashInstantNode->mRecFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sHashInstantNode->mRecFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }

    /* Filter 재정렬 */
    if( sHashInstantNode->mFilter != NULL )
    {
        STL_TRY( qlncAdjustAndFilterByCost( &(aCBOptParamInfo->mParamInfo),
                                            sHashInstantNode->mFilter,
                                            aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Hash Instant Cost 계산
     */

    /* Row Count */
    sRowCount = sHashInstantNode->mChildNode->mBestCost->mResultRowCount;
    sRowCount *= QLNC_EQUAL_OPER_RESULT_RATE;

    /* Build Cost */

    /* Child Node 읽는 비용 */
    sGroupCost->mCostCommon.mBuildCost =
        sHashInstantNode->mChildNode->mBestCost->mTotalCost;

    /* Read IO Cost */
    sGroupCost->mCostCommon.mReadIOCost = (stlFloat64)0.0;

    /* Output IO Cost */
    sGroupCost->mCostCommon.mOutputIOCost = (stlFloat64)0.0;

    /* Group Key Compare에 대한 Cost 계산 */
    sGroupCost->mCostCommon.mFirstFilterCost = (stlFloat64)0.0;
    sGroupCost->mCostCommon.mNextFilterCost =
        sRowCount * QLNC_SORT_KEY_COMPARE_COST_PER_KEY;

    if( (sHashInstantNode->mRecFilter != NULL) ||
        (sHashInstantNode->mFilter != NULL) )
    {
        sGroupCost->mCostCommon.mNextFilterCost += QLNC_FILTER_PREPARE_COST;
    }
    else
    {
        sGroupCost->mCostCommon.mNextFilterCost += (stlFloat64)0.0;
    }

    /* Key Filter에 의한 row 계산 */
    if( sHashInstantNode->mKeyFilter != NULL )
    {
        sRowCount = qlncGetExpectedAndFilterResultCount( sHashInstantNode->mKeyFilter,
                                                         STL_FALSE,
                                                         sRowCount );
    }

    /* Record Filter에 의한 Cost 계산 */
    if( sHashInstantNode->mRecFilter != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    aCBOptParamInfo->mLeftNode,
                                                    sHashInstantNode->mRecFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sGroupCost->mCostCommon.mNextFilterCost += sTmpFilterCost;
    }

    /* Read Column Filter에 의한 Cost 계산 */
    if( sHashInstantNode->mFilter != NULL )
    {
        STL_TRY( qlncCBOptAndFilterCostAndRowCount( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                    aCBOptParamInfo->mLeftNode,
                                                    sHashInstantNode->mFilter,
                                                    sRowCount,
                                                    &sTmpFilterCost,
                                                    &sPhysicalRowCount,
                                                    &sRowCount,
                                                    aEnv )
                 == STL_SUCCESS );

        sGroupCost->mCostCommon.mNextFilterCost += sTmpFilterCost;
    }

    sGroupCost->mCostCommon.mNextFilterCost += QLNC_OUTPUT_COST_PER_ROW;

    QLNC_SET_TOTAL_COST( &(sGroupCost->mCostCommon) );
    sGroupCost->mCostCommon.mResultRowCount = sRowCount;

    /* Hash Instant Node에 Hash Instant Cost 설정 */
    sHashInstantNode->mGroupCost = sGroupCost;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/////////////////////////////////////////////////////////////////////////////////

/**
 * @brief And Filter의 Cost와 Result Row Count를 계산한다.
 * @param[in]   aSQLString          SQL String
 * @param[in]   aTableNode          Physical Filter 평가를 위한 Table Node
 * @param[in]   aAndFilter          And Filter
 * @param[in]   aOrgRowCount        Original Row Count
 * @param[out]  aFilterCost         Filter Cost
 * @param[out]  aPhysicalRowCount   Result Row Count for Physical Filter
 * @param[out]  aResultRowCount     Result Row Count
 * @param[in]   aEnv                Environment
 */
stlStatus qlncCBOptAndFilterCostAndRowCount( stlChar        * aSQLString,
                                             qlncNodeCommon * aTableNode,
                                             qlncAndFilter  * aAndFilter,
                                             stlFloat64       aOrgRowCount,
                                             stlFloat64     * aFilterCost,
                                             stlFloat64     * aPhysicalRowCount,
                                             stlFloat64     * aResultRowCount,
                                             qllEnv         * aEnv )
{
    stlInt32              i, j;
    stlFloat64            sFilterCost;
    stlFloat64            sOrFilterCost;
    stlFloat64            sResultRowCount;
    stlFloat64            sPhysicalResultRowCount;
    stlFloat64            sOrFilterRowCount;
    stlFloat64            sExpectedResultRowCount;
    stlFloat64            sTmpRowCount;
    qlncBooleanFilter   * sBooleanFilter    = NULL;
    qlncRefExprItem     * sRefExprItem      = NULL;
    stlFloat64            sStmtSubQueryCost;
    stlBool               sHasNonStmtSubQuery;
    stlBool               sIsPhysicalFilter;
    stlBool               sHasConstFilter;


    sFilterCost = (stlFloat64)0.0;
    sResultRowCount = aOrgRowCount;
    /* Physical Filter 비용 계산 */
    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        if( (aTableNode != NULL ) &&
            (aAndFilter->mOrFilters[i]->mFilterCount == 1) &&
            (aAndFilter->mOrFilters[i]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER) )
        {
            sBooleanFilter = (qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[0]);

            /* Non Statement SubQuery 존재여부 판단 */
            sHasNonStmtSubQuery = STL_FALSE;
            sStmtSubQueryCost = (stlFloat64)0.0;
            if( (sBooleanFilter->mSubQueryExprList != NULL) &&
                (sBooleanFilter->mSubQueryExprList->mCount > 0) )
            {
                sRefExprItem = sBooleanFilter->mSubQueryExprList->mHead;
                while( sRefExprItem != NULL )
                {
                    STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_SUB_QUERY );
                    STL_DASSERT( ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode->mIsAnalyzedCBO ==
                                 STL_TRUE );
                    STL_DASSERT( ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode->mBestCost != NULL );

                    if( (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL) ||
                        (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mIsRelSubQuery == STL_TRUE) )
                    {
                        sHasNonStmtSubQuery = STL_TRUE;
                        break;
                    }
                    else
                    {
                         sStmtSubQueryCost +=
                             ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode->mBestCost->mTotalCost;
                    }

                    sRefExprItem = sRefExprItem->mNext;
                }
            }

            /* Physical Filter 인지 판단 */
            STL_TRY( qlncIsPhysicalFilter( aSQLString,
                                           aTableNode,
                                           sBooleanFilter->mExpr,
                                           &sIsPhysicalFilter,
                                           aEnv )
                     == STL_SUCCESS );

            if( (sHasNonStmtSubQuery == STL_FALSE) && (sIsPhysicalFilter == STL_TRUE) )
            {
                /* Physical Filter Flag 설정 */
                sBooleanFilter->mIsPhysicalFilter = STL_TRUE;

                /* Or Filter Cost 계산 */
                sFilterCost += (sResultRowCount * QLNC_PHYSICAL_FILTER_COST_PER_ROW);

                /* Statement 단위의 SubQuery 처리 비용 */
                sFilterCost += sStmtSubQueryCost;

                /* Expected Row Count 계산 */
                sResultRowCount =
                    qlncGetExpectedResultCountByFilter(
                        aAndFilter->mOrFilters[i]->mFilters[0],
                        STL_FALSE,
                        sResultRowCount );
            }
            else
            {
                /* Physical Filter Flag 설정 */
                sBooleanFilter->mIsPhysicalFilter = STL_FALSE;
            }
        }
    }
    sPhysicalResultRowCount = sResultRowCount;

    /* Logical Filter 비용 계산 */
    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        sOrFilterRowCount = (stlFloat64)0.0;
        sOrFilterCost = (stlFloat64)0.0;
        sTmpRowCount = sResultRowCount;
        sHasConstFilter = STL_FALSE;

        /* Physical Filter인 경우 Skip */
        if( (aTableNode != NULL ) &&
            (aAndFilter->mOrFilters[i]->mFilterCount == 1) &&
            (aAndFilter->mOrFilters[i]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER) &&
            (((qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[0]))->mIsPhysicalFilter == STL_TRUE) )
        {
            continue;
        }

        for( j = 0; j < aAndFilter->mOrFilters[i]->mFilterCount; j++ )
        {
            /* Or Filter Cost 계산 */
            if( aAndFilter->mOrFilters[i]->mFilters[j]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER )
            {
                sBooleanFilter = (qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]);

                sOrFilterCost +=
                    qlncGetLogicalFilterCost( sBooleanFilter->mExpr, aOrgRowCount );

                /* SubQuery에 대한 Cost 계산 */
                if( sBooleanFilter->mSubQueryExprList != NULL )
                {
                    sRefExprItem = sBooleanFilter->mSubQueryExprList->mHead;
                    while( sRefExprItem != NULL )
                    {
                        if( ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList == NULL )
                        {
                            sOrFilterCost +=
                                ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode->mBestCost->mTotalCost;
                        }

                        sRefExprItem = sRefExprItem->mNext;
                    }
                }

                /* Expected Row Count 계산 */
                sExpectedResultRowCount =
                    qlncGetExpectedResultCountByFilter(
                        aAndFilter->mOrFilters[i]->mFilters[j],
                        STL_FALSE,
                        sTmpRowCount );

                sTmpRowCount -= sExpectedResultRowCount;
                sOrFilterRowCount += sExpectedResultRowCount;
            }
            else
            {
                sOrFilterCost +=
                    qlncGetLogicalFilterCost(
                        ((qlncConstBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mExpr,
                        1 );

                /* SubQuery에 대한 Cost 계산 */
                if( ((qlncConstBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mSubQueryExprList != NULL )
                {
                     sRefExprItem =
                         ((qlncConstBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mSubQueryExprList->mHead;
                     while( sRefExprItem != NULL )
                     {
                         if( ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList == NULL )
                         {
                             sOrFilterCost +=
                                 ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode->mBestCost->mTotalCost;
                         }

                         sRefExprItem = sRefExprItem->mNext;
                     }
                }

                sHasConstFilter = STL_TRUE;
            }
        }

        if( sHasConstFilter == STL_TRUE )
        {
            sResultRowCount = sTmpRowCount;
        }
        else
        {
            sResultRowCount = sOrFilterRowCount;
        }
        sFilterCost += sOrFilterCost;
    }

    if( sResultRowCount < (stlFloat64)1.0 )
    {
        sResultRowCount = (stlFloat64)1.0;
    }


    /**
     * Output 설정
     */

    *aFilterCost = sFilterCost;
    *aPhysicalRowCount = sPhysicalResultRowCount;
    *aResultRowCount = sResultRowCount;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Logical Filter의 Cost를 계산한다.
 * @param[in]   aFilter     Filter
 * @param[in]   aRowCount   Row Count
 */
stlFloat64 qlncGetLogicalFilterCost( qlncExprCommon * aFilter,
                                     stlFloat64       aRowCount )
{
    stlFloat64        sResultCost;
    qlncRefExprItem * sRefExprItem  = NULL;


    if( aFilter->mType == QLNC_EXPR_TYPE_AND_FILTER )
    {
        stlInt32          i, j;
        qlncAndFilter   * sAndFilter    = NULL;

        sResultCost = (stlFloat64)0.0;
        sAndFilter = (qlncAndFilter*)aFilter;
        for( i = 0; i < sAndFilter->mOrCount; i++ )
        {
            for( j = 0; j < sAndFilter->mOrFilters[i]->mFilterCount; j++ )
            {
                if( sAndFilter->mOrFilters[i]->mFilters[j]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER )
                {
                    sResultCost +=
                        qlncGetLogicalFilterCost(
                            ((qlncBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[j]))->mExpr,
                            aRowCount );

                    /* SubQuery에 대한 Cost 계산 */
                    if( ((qlncBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[j]))->mSubQueryExprList != NULL )
                    {
                        sRefExprItem =
                            ((qlncBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[j]))->mSubQueryExprList->mHead;
                        while( sRefExprItem != NULL )
                        {
                            if( (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL) &&
                                (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList->mCount > 0) )
                            {
                                sResultCost +=
                                    ( aRowCount *
                                      ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode->mBestCost->mTotalCost );
                            }
                            else
                            {
                                sResultCost +=
                                    ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode->mBestCost->mTotalCost;
                            }

                            sRefExprItem = sRefExprItem->mNext;
                        }
                    }
                }
                else
                {
                    STL_DASSERT( sAndFilter->mOrFilters[i]->mFilters[j]->mType ==
                                 QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER );
                    sResultCost +=
                        qlncGetLogicalFilterCost(
                            ((qlncConstBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[j]))->mExpr,
                            aRowCount );

                    /* SubQuery에 대한 Cost 계산 */
                    if( ((qlncConstBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[j]))->mSubQueryExprList != NULL )
                    {
                        sRefExprItem =
                            ((qlncConstBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[j]))->mSubQueryExprList->mHead;
                        while( sRefExprItem != NULL )
                        {
                            if( (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL) &&
                                (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList->mCount > 0) )
                            {
                                /* @todo 여기에 들어올 수 없을 것으로 판단됨 */

                                sResultCost +=
                                    ( aRowCount *
                                      ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode->mBestCost->mTotalCost );
                            }
                            else
                            {
                                sResultCost +=
                                    ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode->mBestCost->mTotalCost;
                            }

                            sRefExprItem = sRefExprItem->mNext;
                        }
                    }
                }
            }
        }
    }
    else if( aFilter->mType == QLNC_EXPR_TYPE_CASE_EXPR )
    {
        stlInt32              i;
        qlncExprCaseExpr    * sCaseExpr = NULL;

        sResultCost = (stlFloat64)0.0;
        sCaseExpr = (qlncExprCaseExpr*)aFilter;
        for( i = 0; i < sCaseExpr->mCount; i++ )
        {
            sResultCost += qlncGetLogicalFilterCost( sCaseExpr->mWhenArr[i], aRowCount );
        }
    }
    else if( aFilter->mType == QLNC_EXPR_TYPE_LIST_FUNCTION )
    {
        sResultCost =
            aRowCount *
            QLNC_LOGICAL_FILTER_COST_PER_ROW *
            ((((qlncExprListCol*)(((qlncExprListFunc*)aFilter)->mArgs[0]))->mArgCount - 1) / 2 + 1);
    }
    else
    {
        sResultCost = aRowCount * QLNC_LOGICAL_FILTER_COST_PER_ROW;
    }


    return sResultCost;
}


/**
 * @brief Filter에 대한 Expected Result Count를 계산한다.
 * @param[in]   aFilter                 Filter
 * @param[in]   aIncludeStmtSubQuery    Statement 단위의 SubQuery에 대하여 Result Count 계산에 포함할지 여부
 * @param[in]   aOrgRowCount            Original Row Count
 */
stlFloat64 qlncGetExpectedResultCountByFilter( qlncExprCommon   * aFilter,
                                               stlBool            aIncludeStmtSubQuery,
                                               stlFloat64         aOrgRowCount )
{
    stlFloat64            sResultRowCount;

    qlncExprCommon      * sTermArg          = NULL;
    qlncExprColumn      * sColumnArg        = NULL;
    qlncBaseTableNode   * sBaseTableNode    = NULL;
    qlncRefExprItem     * sRefExprItem      = NULL;
    
    stlFloat64            sMaxColumnCard    = 0.0;
    stlFloat64            sColumnCard       = 0.0;


    if( aFilter->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER )
    {
        if( (((qlncBooleanFilter*)aFilter)->mSubQueryExprList != NULL) &&
            (((qlncBooleanFilter*)aFilter)->mSubQueryExprList->mCount > 0) )
        {
            if( aIncludeStmtSubQuery == STL_FALSE )
            {
                sResultRowCount = aOrgRowCount;
            }
            else
            {
                sRefExprItem = ((qlncBooleanFilter*)aFilter)->mSubQueryExprList->mHead;
                while( sRefExprItem != NULL )
                {
                    if( (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL) ||
                        (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mIsRelSubQuery == STL_TRUE) )
                    {
                        break;
                    }

                    sRefExprItem = sRefExprItem->mNext;
                }

                if( sRefExprItem != NULL )
                {
                    /* @todo 여기에 들어오는 경우가 없을 것으로 판단됨 */

                    sResultRowCount = aOrgRowCount;
                }
                else
                {
                    sResultRowCount =
                        qlncGetExpectedResultCountByFilter(
                            ((qlncBooleanFilter*)aFilter)->mExpr,
                            aIncludeStmtSubQuery,
                            aOrgRowCount );
                }
            }
        }
        else
        {
            sResultRowCount =
                qlncGetExpectedResultCountByFilter(
                    ((qlncBooleanFilter*)aFilter)->mExpr,
                    aIncludeStmtSubQuery,
                    aOrgRowCount );
        }
    }
    else if( aFilter->mType == QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER )
    {
        if( (((qlncConstBooleanFilter*)aFilter)->mSubQueryExprList != NULL) &&
            (((qlncConstBooleanFilter*)aFilter)->mSubQueryExprList->mCount > 0) )
        {
            if( aIncludeStmtSubQuery == STL_FALSE )
            {
                sResultRowCount = aOrgRowCount;
            }
            else
            {
                /* @todo 여기에 들어올 수 없을 것으로 판단됨 */

                sRefExprItem = ((qlncConstBooleanFilter*)aFilter)->mSubQueryExprList->mHead;
                while( sRefExprItem != NULL )
                {
                    if( (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL) ||
                        (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mIsRelSubQuery == STL_TRUE) )
                    {
                        break;
                    }

                    sRefExprItem = sRefExprItem->mNext;
                }

                if( sRefExprItem != NULL )
                {
                    sResultRowCount = aOrgRowCount;
                }
                else
                {
                    sResultRowCount = aOrgRowCount / (stlFloat64)2.0;
                }
            }
        }
        else
        {
            sResultRowCount = aOrgRowCount / (stlFloat64)2.0;
        }
    }
    else if( aFilter->mType == QLNC_EXPR_TYPE_AND_FILTER )
    {
        stlInt32          i, j;
        stlFloat64        sTmpRowCount;
        qlncAndFilter   * sAndFilter;

        sResultRowCount = aOrgRowCount;
        sAndFilter = (qlncAndFilter*)aFilter;
        for( i = 0; i < sAndFilter->mOrCount; i++ )
        {
            sTmpRowCount = (stlFloat64)0.0;
            for( j = 0; j < sAndFilter->mOrFilters[i]->mFilterCount; j++ )
            {
                sTmpRowCount +=
                    qlncGetExpectedResultCountByFilter(
                        sAndFilter->mOrFilters[i]->mFilters[j],
                        aIncludeStmtSubQuery,
                        (sResultRowCount - sTmpRowCount) );
            }
            sResultRowCount = sTmpRowCount;
        }
    }
    else if( aFilter->mType == QLNC_EXPR_TYPE_FUNCTION )
    {
        switch( ((qlncExprFunction*)aFilter)->mFuncId )
        {
            case KNL_BUILTIN_FUNC_IS_EQUAL:
                {
                    sMaxColumnCard = QLNC_DEFAULT_COLUMN_CARDINALITY;

                    sTermArg = ((qlncExprFunction*)aFilter)->mArgs[0];

                    while ( sTermArg->mType == QLNC_EXPR_TYPE_INNER_COLUMN )
                    {
                        if ( ((qlncExprInnerColumn*)sTermArg)->mOrgExpr != NULL )
                        {
                            sTermArg = ((qlncExprInnerColumn*)sTermArg)->mOrgExpr;
                        }
                        else
                        {
                            break;
                        }
                    }
                    
                    if ( sTermArg->mType == QLNC_EXPR_TYPE_COLUMN )
                    {
                        sColumnArg = (qlncExprColumn *) sTermArg;

                        if ( sColumnArg->mNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
                        {
                            sBaseTableNode = (qlncBaseTableNode *) sColumnArg->mNode;
                            sColumnCard =
                                sBaseTableNode->mTableStat->mColumnStat[sColumnArg->mColumnPos].mColumnCard;

                            if ( sColumnCard > sMaxColumnCard )
                            {
                                sMaxColumnCard = sColumnCard;
                            }

                            if( ( (sTermArg->mDataType == DTL_TYPE_CHAR) ||
                                  (sTermArg->mDataType == DTL_TYPE_VARCHAR) ) &&
                                ( sBaseTableNode->mTableStat->mColumnStat[sColumnArg->mColumnPos].mAvgColumnLen < (stlFloat64)2.0 ) )
                            {
                                if ( (stlFloat64)50.0 > sMaxColumnCard )
                                {
                                    sMaxColumnCard = (stlFloat64)50.0;
                                }
                            }
                        }
                    }

                    sTermArg = ((qlncExprFunction*)aFilter)->mArgs[1];

                    while ( sTermArg->mType == QLNC_EXPR_TYPE_INNER_COLUMN )
                    {
                        if ( ((qlncExprInnerColumn*)sTermArg)->mOrgExpr != NULL )
                        {
                            sTermArg = ((qlncExprInnerColumn*)sTermArg)->mOrgExpr;
                        }
                        else
                        {
                            break;
                        }
                    }
                    
                    if ( sTermArg->mType == QLNC_EXPR_TYPE_COLUMN )
                    {
                        sColumnArg = (qlncExprColumn *) sTermArg;
                        
                        if ( sColumnArg->mNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
                        {
                            sBaseTableNode = (qlncBaseTableNode *) sColumnArg->mNode;
                            sColumnCard =
                                sBaseTableNode->mTableStat->mColumnStat[sColumnArg->mColumnPos].mColumnCard;

                            if ( sColumnCard > sMaxColumnCard )
                            {
                                sMaxColumnCard = sColumnCard;
                            }

                            if( ( (sTermArg->mDataType == DTL_TYPE_CHAR) ||
                                  (sTermArg->mDataType == DTL_TYPE_VARCHAR) ) &&
                                ( sBaseTableNode->mTableStat->mColumnStat[sColumnArg->mColumnPos].mAvgColumnLen < (stlFloat64)2.0 ) )
                            {
                                if ( (stlFloat64)50.0 > sMaxColumnCard )
                                {
                                    sMaxColumnCard = (stlFloat64)50.0;
                                }
                            }
                        }
                    }

                    sResultRowCount = aOrgRowCount * ( 1 / sMaxColumnCard );
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_NOT_EQUAL:
                {
                    sResultRowCount = aOrgRowCount * QLNC_NOT_EQUAL_OPER_RESULT_RATE;
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_LESS_THAN:
            case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL:
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN:
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL:
                {
                    sResultRowCount = aOrgRowCount * QLNC_RANGE_OPER_RESULT_RATE;
                    break;
                }
            case KNL_BUILTIN_FUNC_IS:
                {
                    sResultRowCount = aOrgRowCount * QLNC_IS_OPER_RESULT_RATE;
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_NOT:
                {
                    sResultRowCount = aOrgRowCount * QLNC_IS_NOT_OPER_RESULT_RATE;
                    break;
                }
            case KNL_BUILTIN_FUNC_AND:
                {
                    stlInt32              i;
                    qlncExprFunction    * sFunction;

                    sResultRowCount = aOrgRowCount;
                    sFunction = (qlncExprFunction*)aFilter;
                    for( i = 0; i < sFunction->mArgCount; i++ )
                    {
                        sResultRowCount =
                            qlncGetExpectedResultCountByFilter(
                                sFunction->mArgs[i],
                                aIncludeStmtSubQuery,
                                sResultRowCount );
                    }

                    break;
                }
            case KNL_BUILTIN_FUNC_OR:
                {
                    stlInt32              i;
                    qlncExprFunction    * sFunction;

                    sResultRowCount = (stlFloat64)0.0;
                    sFunction = (qlncExprFunction*)aFilter;
                    for( i = 0; i < sFunction->mArgCount; i++ )
                    {
                        sResultRowCount +=
                            qlncGetExpectedResultCountByFilter(
                                sFunction->mArgs[i],
                                aIncludeStmtSubQuery,
                                (aOrgRowCount - sResultRowCount) );
                    }

                    break;
                }
            case KNL_BUILTIN_FUNC_LIKE:
                {
                    sResultRowCount = aOrgRowCount * QLNC_LIKE_OPER_RESULT_RATE;
                    break;
                }
            case KNL_BUILTIN_FUNC_NOT:
                {
                    sResultRowCount =
                        qlncGetExpectedResultCountByFilter( ((qlncExprFunction*)aFilter)->mArgs[0],
                                                            aIncludeStmtSubQuery,
                                                            aOrgRowCount );
                    break;
                }
            default:
                {
                    sResultRowCount = aOrgRowCount * QLNC_DEFAULT_OPER_RESULT_RATE;
                    break;
                }
        }

    }
    else if( aFilter->mType == QLNC_EXPR_TYPE_CASE_EXPR )
    {
        sResultRowCount = aOrgRowCount / (stlFloat64)2.0;
    }
    else if( aFilter->mType == QLNC_EXPR_TYPE_LIST_FUNCTION )
    {
        switch( ((qlncExprListFunc*)aFilter)->mFuncId )
        {
            case KNL_BUILTIN_FUNC_IN:
            case KNL_BUILTIN_FUNC_EXISTS:
            case KNL_BUILTIN_FUNC_EQUAL_ANY:
                {
                    stlInt32      i;

                    sResultRowCount = (stlFloat64)0.0;
                    for( i = 0; i < ((qlncExprListFunc*)aFilter)->mArgCount; i++ )
                    {
                        sResultRowCount +=
                            (aOrgRowCount - sResultRowCount) * QLNC_EQUAL_OPER_RESULT_RATE;
                    }
                    break;
                }
            case KNL_BUILTIN_FUNC_NOT_IN:
            case KNL_BUILTIN_FUNC_NOT_EXISTS:
            case KNL_BUILTIN_FUNC_NOT_EQUAL_ANY:
                {
                    stlInt32      i;

                    sResultRowCount = aOrgRowCount;
                    for( i = 0; i < ((qlncExprListFunc*)aFilter)->mArgCount; i++ )
                    {
                        sResultRowCount *= QLNC_NOT_EQUAL_OPER_RESULT_RATE;
                    }
                    break;
                }
            case KNL_BUILTIN_FUNC_LESS_THAN_ANY:
            case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY:
            case KNL_BUILTIN_FUNC_GREATER_THAN_ANY:
            case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY:
                {
                    stlInt32      i;

                    sResultRowCount = (stlFloat64)0.0;
                    for( i = 0; i < ((qlncExprListFunc*)aFilter)->mArgCount; i++ )
                    {
                        sResultRowCount +=
                            (aOrgRowCount - sResultRowCount) * QLNC_RANGE_OPER_RESULT_RATE;
                    }
                    break;
                }
            case KNL_BUILTIN_FUNC_EQUAL_ALL:
                {
                    stlInt32      i;

                    sResultRowCount = aOrgRowCount;
                    for( i = 0; i < ((qlncExprListFunc*)aFilter)->mArgCount; i++ )
                    {
                        sResultRowCount *= QLNC_EQUAL_OPER_RESULT_RATE;
                    }
                    break;
                }
            case KNL_BUILTIN_FUNC_NOT_EQUAL_ALL:
                {
                    stlInt32      i;

                    sResultRowCount = aOrgRowCount;
                    for( i = 0; i < ((qlncExprListFunc*)aFilter)->mArgCount; i++ )
                    {
                        sResultRowCount *= QLNC_NOT_EQUAL_OPER_RESULT_RATE;
                    }
                    break;
                }
            case KNL_BUILTIN_FUNC_LESS_THAN_ALL:
            case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL:
            case KNL_BUILTIN_FUNC_GREATER_THAN_ALL:
            case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL:
                {
                    stlInt32      i;

                    sResultRowCount = aOrgRowCount;
                    for( i = 0; i < ((qlncExprListFunc*)aFilter)->mArgCount; i++ )
                    {
                        sResultRowCount *= QLNC_RANGE_OPER_RESULT_RATE;
                    }
                    break;
                }
            default:
                {
                    sResultRowCount = aOrgRowCount;
                    break;
                }
        }
    }
    else
    {
        sResultRowCount = aOrgRowCount / (stlFloat64)2.0;
    }


    return sResultRowCount;
}


/**
 * @brief Rowid And Filter의 Cost와 Result Row Count를 계산한다.
 * @param[in]   aAndFilter      And Filter
 * @param[in]   aOrgRowCount    Original Row Count
 * @param[out]  aFilterCost     Filter Cost
 * @param[out]  aResultRowCount Result Row Count
 */
void qlncCBOptRowidAndFilterCostAndRowCount( qlncAndFilter  * aAndFilter,
                                             stlFloat64       aOrgRowCount,
                                             stlFloat64     * aFilterCost,
                                             stlFloat64     * aResultRowCount )
{
    stlInt32      i;
    stlFloat64    sFilterCost;
    stlFloat64    sResultRowCount;


    sFilterCost = (stlFloat64)0.0;
    sResultRowCount = aOrgRowCount;
    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        STL_DASSERT( aAndFilter->mOrFilters[i]->mFilterCount == 1 );
        STL_DASSERT( aAndFilter->mOrFilters[i]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER );

        sFilterCost += QLNC_PHYSICAL_FILTER_COST_PER_ROW;

        /* Expected Row Count 계산 */
        /* @todo 현재 rowid에 대한 filter는 =밖에 없다. */
        STL_DASSERT( ((qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[0]))->mFuncID
                     == KNL_BUILTIN_FUNC_IS_EQUAL );
        sResultRowCount = 1;
    }


    /**
     * Output 설정
     */

    *aFilterCost = sFilterCost;
    *aResultRowCount = sResultRowCount;
}


/**
 * @brief Index에 존재하는 Column으로만 처리 가능한 Filter인지 판별한다.
 * @param[in]   aNode           Node
 * @param[in]   aOrFilter       Or Filter
 * @param[in]   aIndexKeyDesc   Index Key Description
 * @param[in]   aIndexKeyCount  Index Key Count
 */
stlBool qlncIsOnlyIndexColumnFilter( qlncNodeCommon     * aNode,
                                     qlncOrFilter       * aOrFilter,
                                     ellIndexKeyDesc    * aIndexKeyDesc,
                                     stlInt32             aIndexKeyCount )
{
    stlInt32              i, j;
    qlncRefNodeItem     * sRefNodeItem      = NULL;
    qlncRefColumnItem   * sRefColumnItem    = NULL;

    stlBool               sIsOnlyIndexColumnFilter;


    sIsOnlyIndexColumnFilter = STL_TRUE;
    for( i = 0; i < aOrFilter->mFilterCount; i++ )
    {
        if( aOrFilter->mFilters[i]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER )
        {
            sRefNodeItem = ((qlncBooleanFilter*)(aOrFilter->mFilters[i]))->mRefNodeList.mHead;
            while( sRefNodeItem != NULL )
            {
                if( sRefNodeItem->mNode->mNodeID == aNode->mNodeID )
                {
                    break;
                }

                sRefNodeItem = sRefNodeItem->mNext;
            }

            if( sRefNodeItem != NULL )
            {
                sRefColumnItem = sRefNodeItem->mRefColumnList.mHead;
                while( sRefColumnItem != NULL )
                {
                    if( sRefColumnItem->mExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN )
                    {
                        sIsOnlyIndexColumnFilter = STL_FALSE;
                        break;
                    }

                    STL_DASSERT( sRefColumnItem->mExpr->mType == QLNC_EXPR_TYPE_COLUMN );
                    for( j = 0; j < aIndexKeyCount; j++ )
                    {
                        if( ((qlncExprColumn*)(sRefColumnItem->mExpr))->mColumnID ==
                            ellGetColumnID( &(aIndexKeyDesc[j].mKeyColumnHandle) ) )
                        {
                            break;
                        }
                    }

                    if( j == aIndexKeyCount )
                    {
                        sIsOnlyIndexColumnFilter = STL_FALSE;
                        break;
                    }

                    sRefColumnItem = sRefColumnItem->mNext;
                }
            }
            else
            {
                /* Index에서 처리 가능 */
            }
        }

        if( sIsOnlyIndexColumnFilter == STL_FALSE )
        {
            break;
        }
    }


    return sIsOnlyIndexColumnFilter;
}


/**
 * @brief CNF And Filter에서 DNF And Filter가 몇 개 발생하는지 반환한다.
 * @param[in]   aCnfAndFilter   CNF And Filter
 */
stlInt32 qlncGetDistribCnfCount( qlncAndFilter  * aCnfAndFilter )
{
    stlInt32      i;
    stlInt32      sDistribCnfCount;

    if( aCnfAndFilter == NULL )
    {
        return 0;
    }

    sDistribCnfCount = 1;
    for( i = 0; i < aCnfAndFilter->mOrCount; i++ )
    {
        if( aCnfAndFilter->mOrFilters[i] == NULL )
        {
            /* @todo 여기에 들어오는 경우가 없을 것으로 판단됨 */

            continue;
        }

        if( aCnfAndFilter->mOrFilters[i]->mFilterCount > 1 )
        {
            sDistribCnfCount *= aCnfAndFilter->mOrFilters[i]->mFilterCount;
        }
    }

    return sDistribCnfCount;
}


/**
 * @brief CNF And Filter를 DNF And Filter로 변경한다.
 * @param[in]   aParamInfo          Parameter Info
 * @param[in]   aCnfAndFilter       Cnf And Filter
 * @param[out]  aDnfAndFilterCount  DNF And Filter Count
 * @param[out]  aDnfAndFilterArr    DNF And Filter Array
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCnf2Dnf( qlncParamInfo     * aParamInfo,
                              qlncAndFilter     * aCnfAndFilter,
                              stlInt32          * aDnfAndFilterCount,
                              qlncAndFilter    ** aDnfAndFilterArr,
                              qllEnv            * aEnv )
{
    stlInt32          i, j;
    stlInt32          sDnfAndFilterCount;
    qlncAndFilter   * sDnfAndFilterArr  = NULL;

    stlInt32          sIdx;
    stlInt32          sIterateIdx;
    stlInt32          sIterateCnt;
    stlInt32          sRepeatIdx;
    stlInt32          sRepeatCnt;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCnfAndFilter != NULL, QLL_ERROR_STACK(aEnv) );


    /* Dnf And Filter Count 계산 */
    sDnfAndFilterCount = 1;
    for( i = 0; i < aCnfAndFilter->mOrCount; i++ )
    {
        sDnfAndFilterCount *= aCnfAndFilter->mOrFilters[i]->mFilterCount;
    }

    /* Dnf And Filter Array 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncAndFilter ) * sDnfAndFilterCount,
                                (void**)&sDnfAndFilterArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Dnf And Filter Array 초기화 */
    for( i = 0; i < sDnfAndFilterCount; i++ )
    {
        QLNC_INIT_AND_FILTER( &(sDnfAndFilterArr[i]) );
    }

    /* Dnf And Filter Array 설정 */
    sIterateCnt = 1;
    for( i = 0; i < aCnfAndFilter->mOrCount; i++ )
    {
        sRepeatCnt = sDnfAndFilterCount / (aCnfAndFilter->mOrFilters[i]->mFilterCount * sIterateCnt);

        sIdx = 0;
        for( sIterateIdx = 0; sIterateIdx < sIterateCnt; sIterateIdx++ )
        {
            for( j = 0; j < aCnfAndFilter->mOrFilters[i]->mFilterCount; j++ )
            {
                for( sRepeatIdx = 0; sRepeatIdx < sRepeatCnt; sRepeatIdx++ )
                {
                    STL_TRY( qlncAppendFilterToAndFilter( aParamInfo,
                                                          &sDnfAndFilterArr[sIdx],
                                                          aCnfAndFilter->mOrFilters[i]->mFilters[j],
                                                          aEnv )
                             == STL_SUCCESS );

                    /* Refine DNF And Filter */
                    if( (sDnfAndFilterArr[sIdx].mOrCount == 1) &&
                        (sDnfAndFilterArr[sIdx].mOrFilters[0]->mFilterCount == 1) &&
                        (sDnfAndFilterArr[sIdx].mOrFilters[0]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER) )
                    {
                        qlncBooleanFilter   * sBooleanFilter    = NULL;

                        sBooleanFilter =
                            (qlncBooleanFilter*)(sDnfAndFilterArr[sIdx].mOrFilters[0]->mFilters[0]);

                        if( sBooleanFilter->mFuncID == KNL_BUILTIN_FUNC_AND )
                        {
                            qlncAndFilter   * sTmpDnfAndFilter  = NULL;

                            STL_TRY( qlncCopyAndFilter( aParamInfo,
                                                        (qlncAndFilter*)(sBooleanFilter->mExpr),
                                                        &sTmpDnfAndFilter,
                                                        aEnv )
                                     == STL_SUCCESS );

                            sDnfAndFilterArr[sIdx] = *sTmpDnfAndFilter;
                        }
                    }

                    sIdx++;
                }
            }
        }

        sIterateCnt *= aCnfAndFilter->mOrFilters[i]->mFilterCount;
    }


    /**
     * Output 설정
     */

    *aDnfAndFilterCount = sDnfAndFilterCount;
    *aDnfAndFilterArr = sDnfAndFilterArr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 현재의 Inner Join에서 사용가능한 Join Condition이 존재하는지 판단한다. 단, aNodeCount번째 노드에 존재하는 컬럼이 반드시 존재해야 한다.
 * @param[in]   aInnerJoinNodeInfoArr   Inner Join Node Info Array
 * @param[in]   aNodeCount              검사할 노드의 개수
 * @param[in]   aAndFilter              And Filter
 */
stlBool qlncIsExistJoinCondOnInnerJoinNode( qlncInnerJoinNodeInfo   * aInnerJoinNodeInfoArr,
                                            stlInt32                  aNodeCount,
                                            qlncAndFilter           * aAndFilter )
{
    stlInt32              i, j, k;

    stlBool               sExist;
    qlncRefNodeItem     * sRefNodeItem  = NULL;


    STL_DASSERT( aInnerJoinNodeInfoArr != NULL);
    STL_DASSERT( aNodeCount > 0 );


    sExist = STL_FALSE;
    if( aAndFilter == NULL )
    {
        STL_THROW( RAMP_FINISH );
    }

    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        if( aAndFilter->mOrFilters[i] == NULL )
        {
            /* 비어있는 Filter인 경우 Skip */
            /* @todo 여기에 들어오는 경우가 없을 것으로 판단됨 */

            continue;
        }

        sExist = STL_TRUE;
        for( j = 0; j < aAndFilter->mOrFilters[i]->mFilterCount; j++ )
        {
            if( aAndFilter->mOrFilters[i]->mFilters[j]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER )
            {
                /* aNodeCount에 해당하는 노드는 반드시 존재해야 한다. */
                sRefNodeItem =
                    ((qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mRefNodeList.mHead;
                while( sRefNodeItem != NULL )
                {
                    if( qlncIsExistNode( aInnerJoinNodeInfoArr[aNodeCount].mNode,
                                         sRefNodeItem->mNode ) == STL_TRUE )
                    {
                        break;
                    }

                    sRefNodeItem = sRefNodeItem->mNext;
                }

                if( sRefNodeItem == NULL )
                {
                    sExist = STL_FALSE;
                    break;
                }

                /* Filter의 Reference Node들이 모두 존재하는지 판단 */
                sRefNodeItem =
                    ((qlncBooleanFilter*)(aAndFilter->mOrFilters[i]->mFilters[j]))->mRefNodeList.mHead;
                while( sRefNodeItem != NULL )
                {
                    for( k = 0; k <= aNodeCount; k++ )
                    {
                        if( qlncIsExistNode( aInnerJoinNodeInfoArr[k].mNode,
                                             sRefNodeItem->mNode ) == STL_TRUE )
                        {
                            break;
                        }
                    }

                    if( k > aNodeCount )
                    {
                        sExist = STL_FALSE;
                        break;
                    }

                    sRefNodeItem = sRefNodeItem->mNext;
                }

                if( sExist == STL_FALSE )
                {
                    break;
                }
            }
            else
            {
                STL_DASSERT( aAndFilter->mOrFilters[i]->mFilters[j]->mType ==
                             QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER );
                sExist = STL_FALSE;
            }
        }

        if( sExist == STL_TRUE )
        {
            break;
        }
    }


    STL_RAMP( RAMP_FINISH );


    return sExist;
}


/**
 * @brief 현재 Inner Join에 사용할 수 있는 Filter를 찾아 Join Condition과 Join Filter로 분류한다.
 * @param[in]       aParamInfo          Parameter Info
 * @param[in,out]   aJoinNode           Candidate Node
 * @param[in]       aJoinAndFilter      Join And Filter
 * @param[in]       aIsLastJoinNode     마지막 Join Node인지 여부
 * @param[in]       aEnv                Environment
 */
stlStatus qlncFindUsableFilterOnInnerJoinNode( qlncParamInfo            * aParamInfo,
                                               qlncInnerJoinTableNode   * aJoinNode,
                                               qlncAndFilter            * aJoinAndFilter,
                                               stlBool                    aIsLastJoinNode,
                                               qllEnv                   * aEnv )
{
    stlInt32              i, j;

    stlBool               sExist;
    stlBool               sIsJoinCondition;
    stlBool               sIsEquiJoinCondition;
    qlncRefNodeItem     * sRefNodeItem  = NULL;

    qlncAndFilter         sJoinCondition;
    qlncAndFilter         sJoinFilter;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinNode->mNodeCommon.mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinAndFilter != NULL, QLL_ERROR_STACK(aEnv) );


    /* Join Condition 및 Join Filter 초기화 */
    QLNC_INIT_AND_FILTER( &sJoinCondition );
    QLNC_INIT_AND_FILTER( &sJoinFilter );

    for( i = 0; i < aJoinAndFilter->mOrCount; i++ )
    {
        /* 비어있는 Filter인 경우 Skip */
        if( aJoinAndFilter->mOrFilters[i] == NULL )
        {
            continue;
        }

        /* SubQuery가 존재하는 경우 마지막 Join Node가 아니면 Skip */
        if( (aJoinAndFilter->mOrFilters[i]->mSubQueryExprList != NULL) &&
            (aIsLastJoinNode == STL_FALSE) )
        {
            continue;
        }

        sExist = STL_TRUE;
        sIsJoinCondition = STL_TRUE;
        sIsEquiJoinCondition = STL_TRUE;
        for( j = 0; j < aJoinAndFilter->mOrFilters[i]->mFilterCount; j++ )
        {
            if( aJoinAndFilter->mOrFilters[i]->mFilters[j]->mType ==
                QLNC_EXPR_TYPE_BOOLEAN_FILTER )
            {
                sRefNodeItem =
                    ((qlncBooleanFilter*)(aJoinAndFilter->mOrFilters[i]->mFilters[j]))->mRefNodeList.mHead;
                if( ((qlncBooleanFilter*)(aJoinAndFilter->mOrFilters[i]->mFilters[j]))->mPossibleJoinCond == STL_TRUE )
                {
                    if( ((qlncBooleanFilter*)(aJoinAndFilter->mOrFilters[i]->mFilters[j]))->mFuncID !=
                        KNL_BUILTIN_FUNC_IS_EQUAL )
                    {
                        sIsEquiJoinCondition = STL_FALSE;
                    }
                }
                else
                {
                    sIsJoinCondition = STL_FALSE;
                }

                /* Filter의 Reference Node들이 모두 존재하는지 판단 */
                while( sRefNodeItem != NULL )
                {
                    if( qlncIsExistNode( (qlncNodeCommon*)aJoinNode,
                                         sRefNodeItem->mNode ) == STL_FALSE )
                    {
                        sExist = STL_FALSE;
                        break;
                    }

                    sRefNodeItem = sRefNodeItem->mNext;
                }

                if( sExist == STL_FALSE )
                {
                    break;
                }
            }
            else
            {
                STL_DASSERT( aJoinAndFilter->mOrFilters[i]->mFilters[j]->mType ==
                             QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER );
                sIsJoinCondition = STL_FALSE;
                sIsEquiJoinCondition = STL_FALSE;
            }
        }

        /* Or가 존재하면 Join Condition이 될 수 없음 */
        if( aJoinAndFilter->mOrFilters[i]->mFilterCount > 1 )
        {
            sIsJoinCondition = STL_FALSE;
        }

        /* SubQuery가 존재하면 Join Condition이 될 수 없음 */
        if( (aJoinAndFilter->mOrFilters[i]->mSubQueryExprList != NULL) &&
            (aJoinAndFilter->mOrFilters[i]->mSubQueryExprList->mCount > 0) )
        {
            sIsJoinCondition = STL_FALSE;
        }

        if( (sExist == STL_TRUE) || (aIsLastJoinNode == STL_TRUE) )
        {
            if( sIsJoinCondition == STL_TRUE )
            {
                if( sIsEquiJoinCondition == STL_TRUE )
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterHead( aParamInfo,
                                                                &sJoinCondition,
                                                                1,
                                                                &(aJoinAndFilter->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                                &sJoinCondition,
                                                                1,
                                                                &(aJoinAndFilter->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                            &sJoinFilter,
                                                            1,
                                                            &(aJoinAndFilter->mOrFilters[i]),
                                                            aEnv )
                         == STL_SUCCESS );
            }

            aJoinAndFilter->mOrFilters[i] = NULL;
        }
    }


    /**
     * Output 설정
     */

    if( sJoinCondition.mOrCount > 0 )
    {
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncAndFilter ),
                                    (void**) &(aJoinNode->mJoinCondition),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( aJoinNode->mJoinCondition, &sJoinCondition, STL_SIZEOF( qlncAndFilter ) );
    }
    else
    {
        aJoinNode->mJoinCondition = NULL;
    }

    if( sJoinFilter.mOrCount > 0 )
    {
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncAndFilter ),
                                    (void**) &(aJoinNode->mJoinFilter),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( aJoinNode->mJoinFilter, &sJoinFilter, STL_SIZEOF( qlncAndFilter ) );
    }
    else
    {
        aJoinNode->mJoinFilter = NULL;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Merge Join이 가능한 Condition인지 판단한다.
 * @param[in]   aSQLString          SQL String
 * @param[in]   aBooleanFilter      Boolean Filter
 * @param[in]   aRightNode          Merge Join의 Inner 쪽의 Candidate Node
 * @param[out]  aIsPossible         Merge Join이 가능한지 여부
 * @param[in]   aEnv                Environment
 */
stlStatus qlncIsPossibleMergeJoinCondition( stlChar             * aSQLString,
                                            qlncBooleanFilter   * aBooleanFilter,
                                            qlncNodeCommon      * aRightNode,
                                            stlBool             * aIsPossible,
                                            qllEnv              * aEnv )
{
    stlBool           sIsPossibleMergeJoinCond;
    qlncExprCommon  * sOuterExpr;
    qlncExprCommon  * sInnerExpr;

    const dtlCompareType  * sCompType;

    if( aBooleanFilter->mFuncID == KNL_BUILTIN_FUNC_IS_EQUAL )
    {
        if( (aBooleanFilter->mLeftRefNodeList != NULL) &&
            (aBooleanFilter->mRightRefNodeList != NULL) &&
            (aBooleanFilter->mLeftRefNodeList->mCount >= 1) &&
            (aBooleanFilter->mRightRefNodeList->mCount >= 1) &&
            ( (aBooleanFilter->mLeftRefNodeList->mCount == 1) ||
              (aBooleanFilter->mRightRefNodeList->mCount == 1) ) )
        {
            if( (aBooleanFilter->mLeftRefNodeList->mCount == 1) &&
                qlncIsExistNode( aRightNode, aBooleanFilter->mLeftRefNodeList->mHead->mNode ) )
            {
                sInnerExpr = ((qlncExprFunction*)(aBooleanFilter->mExpr))->mArgs[0];
                sOuterExpr = ((qlncExprFunction*)(aBooleanFilter->mExpr))->mArgs[1];
            }
            else if( (aBooleanFilter->mRightRefNodeList->mCount == 1) &&
                     qlncIsExistNode( aRightNode, aBooleanFilter->mRightRefNodeList->mHead->mNode ) )
            {
                sInnerExpr = ((qlncExprFunction*)(aBooleanFilter->mExpr))->mArgs[1];
                sOuterExpr = ((qlncExprFunction*)(aBooleanFilter->mExpr))->mArgs[0];
            }
            else
            {
                sIsPossibleMergeJoinCond = STL_FALSE;
                STL_THROW( RAMP_FINISH );
            }

            if( qlncIsSameExpr( sOuterExpr, sInnerExpr ) == STL_TRUE )
            {
                /* @todo 여기에 들어오지 않는 것으로 판단됨 */

                sIsPossibleMergeJoinCond = STL_FALSE;
                STL_THROW( RAMP_FINISH );
            }

            if( ( dtlCheckKeyCompareType( sOuterExpr->mDataType ) ==  STL_FALSE ) ||
                ( dtlCheckKeyCompareType( sInnerExpr->mDataType ) ==  STL_FALSE ) )
            {
                sIsPossibleMergeJoinCond = STL_FALSE;
            }
            else
            {
                STL_TRY( qlnoGetCompareType( aSQLString,
                                             sOuterExpr->mDataType,
                                             sInnerExpr->mDataType,
                                             STL_FALSE,
                                             STL_FALSE,
                                             aBooleanFilter->mExpr->mPosition,
                                             &sCompType,
                                             aEnv )
                         == STL_SUCCESS );

                if( (sCompType->mLeftType != sOuterExpr->mDataType) ||
                    (sCompType->mRightType != sInnerExpr->mDataType) )
                {
                    sIsPossibleMergeJoinCond = STL_FALSE;
                }
                else
                {
                    sIsPossibleMergeJoinCond = STL_TRUE;
                }
            }
        }
        else
        {
            sIsPossibleMergeJoinCond = STL_FALSE;
        }
    }
    else
    {
        sIsPossibleMergeJoinCond = STL_FALSE;
    }


    STL_RAMP( RAMP_FINISH );

    
    *aIsPossible =  sIsPossibleMergeJoinCond;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Join이 가능한 Condition인지 판단한다.
 * @param[in]   aSQLString          SQL String
 * @param[in]   aBooleanFilter      Boolean Filter
 * @param[in]   aHashNode           Hash Candidate Node
 * @param[in]   aEquiJoinCondition  Equi Join Condition
 * @param[out]  aIsPossible         Hash Join이 가능한지 여부
 * @param[in]   aEnv                Environment
 */
stlStatus qlncIsPossibleHashJoinCondition( stlChar              * aSQLString,
                                           qlncBooleanFilter    * aBooleanFilter,
                                           qlncNodeCommon       * aHashNode,
                                           qlncAndFilter        * aEquiJoinCondition,
                                           stlBool              * aIsPossible,
                                           qllEnv               * aEnv )
{
    stlBool           sIsPossibleHashJoinCond;
    qlncExprCommon  * sHashColExpr;
    qlncExprCommon  * sHashValueExpr;
    
    const dtlCompareType  * sCompType;
        
    if( aBooleanFilter->mFuncID == KNL_BUILTIN_FUNC_IS_EQUAL )
    {
        if( (aBooleanFilter->mLeftRefNodeList != NULL) &&
            (aBooleanFilter->mRightRefNodeList != NULL) &&
            ( ( (aBooleanFilter->mLeftRefNodeList->mCount == 1) &&
                (aBooleanFilter->mRightRefNodeList->mCount > 0) ) ||
              ( (aBooleanFilter->mLeftRefNodeList->mCount > 0) &&
                (aBooleanFilter->mRightRefNodeList->mCount == 1) ) ) )
        {
            if( (aBooleanFilter->mLeftRefNodeList->mCount == 1) &&
                qlncIsExistNode( aHashNode, aBooleanFilter->mLeftRefNodeList->mHead->mNode ) )
            {
                sHashColExpr   = ((qlncExprFunction*)(aBooleanFilter->mExpr))->mArgs[0];
                sHashValueExpr = ((qlncExprFunction*)(aBooleanFilter->mExpr))->mArgs[1];
            }
            else if( (aBooleanFilter->mRightRefNodeList->mCount == 1) &&
                     qlncIsExistNode( aHashNode, aBooleanFilter->mRightRefNodeList->mHead->mNode ) )
            {
                sHashColExpr   = ((qlncExprFunction*)(aBooleanFilter->mExpr))->mArgs[1];
                sHashValueExpr = ((qlncExprFunction*)(aBooleanFilter->mExpr))->mArgs[0];
            }
            else
            {
                sIsPossibleHashJoinCond = STL_FALSE;
                STL_THROW( RAMP_FINISH );
            }

            if( qlncIsSameExpr( sHashColExpr, sHashValueExpr ) == STL_TRUE )
            {
                /* @todo 여기에 들어오지 않는 것으로 판단됨 */

                sIsPossibleHashJoinCond = STL_FALSE;
                STL_THROW( RAMP_FINISH );
            }

            if( dtlCheckKeyCompareType( sHashColExpr->mDataType ) == STL_FALSE )
            {
                sIsPossibleHashJoinCond = STL_FALSE;
            }
            else
            {
                STL_TRY( qlnoGetCompareType( aSQLString,
                                             sHashColExpr->mDataType,
                                             sHashValueExpr->mDataType,
                                             STL_FALSE,
                                             STL_TRUE,
                                             aBooleanFilter->mExpr->mPosition,
                                             &sCompType,
                                             aEnv )
                         == STL_SUCCESS );

                if( ( sCompType->mLeftType != sHashColExpr->mDataType ) ||
                    ( sCompType->mRightType == DTL_TYPE_NA ) )
                {
                    sIsPossibleHashJoinCond = STL_FALSE;
                }
                else
                {
                    /* Cast는 가능하지만 Cast시 Data Loss가 발생할 수 있는 경우는 hash join 불가 */
                    if( dtlIsNoDataLossForCast
                        [ sHashValueExpr->mDataType ]
                        [ sHashColExpr->mDataType ]
                        == STL_TRUE )
                    {
                        sIsPossibleHashJoinCond =
                            qlncIsPossibleEquiHashJoinCondition( sHashColExpr,
                                                                 aHashNode,
                                                                 aEquiJoinCondition );
                    }
                    else
                    {
                        sIsPossibleHashJoinCond = STL_FALSE;
                    }
                }
            }
        }
        else
        {
            sIsPossibleHashJoinCond = STL_FALSE;
        }
    }
    else
    {
        sIsPossibleHashJoinCond = STL_FALSE;
    }


    STL_RAMP( RAMP_FINISH );


    *aIsPossible =  sIsPossibleHashJoinCond;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Euqi Join Condition으로 사용이 가능한지 판단한다.
 * @param[in]   aColExpr            Column Of Join Condition
 * @param[in]   aHashNode           Hash Candidate Node
 * @param[in]   aEquiJoinCondition  Equi Join Condition
 */
stlBool qlncIsPossibleEquiHashJoinCondition( qlncExprCommon  * aColExpr,
                                             qlncNodeCommon  * aHashNode,
                                             qlncAndFilter   * aEquiJoinCondition )
{
    qlncBooleanFilter  * sBooleanFilter;
    qlncExprCommon     * sJoinColExpr;
    stlInt32             sLoop;

    if( aEquiJoinCondition == NULL )
    {
        return STL_TRUE;
    }
    
    for( sLoop = 0 ; sLoop < aEquiJoinCondition->mOrCount ; sLoop++ )
    {
        STL_DASSERT( aEquiJoinCondition->mOrFilters[ sLoop ]->mFilterCount == 1 );
        
        sBooleanFilter =
            (qlncBooleanFilter*)(aEquiJoinCondition->mOrFilters[ sLoop ]->mFilters[0]);
                            
        if( qlncIsExistNode( aHashNode, sBooleanFilter->mLeftRefNodeList->mHead->mNode ) )
        {
            sJoinColExpr = ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
        }
        else
        {
            sJoinColExpr = ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
        }

        if( qlncIsSameExpr( aColExpr, sJoinColExpr ) == STL_TRUE )
        {
            return STL_FALSE;
        }
    }

    return STL_TRUE;
}


/**
 * @brief Inverted Nested Semi Join이 가능한지 판단한다.
 * @param[in]   aParamInfo          Parameter Info
 * @param[in]   aSemiJoinTableNode  Semi Join Table Node
 * @param[out]  aOutIsPossible      Inverted Nested Semi Join이 가능한지 여부
 * @param[in]   aEnv                Environment
 */
stlStatus qlncIsPossibleInvertedNestedSemiJoin( qlncParamInfo           * aParamInfo,
                                                qlncSemiJoinTableNode   * aSemiJoinTableNode,
                                                stlBool                 * aOutIsPossible,
                                                qllEnv                  * aEnv )
{
    stlInt32              i;
    qlncBooleanFilter   * sBooleanFilter    = NULL;
    qlncNodeArray         sNodeArray;


    STL_DASSERT( aSemiJoinTableNode != NULL );

    /* Join Condition이 존재해야 하고, Left Node가 Base Table이어야 한다. */
    if( (aSemiJoinTableNode->mJoinCondition == NULL) ||
        (aSemiJoinTableNode->mLeftNode->mNodeType != QLNC_NODE_TYPE_BASE_TABLE) )
    {
        *aOutIsPossible = STL_FALSE;
        STL_THROW( RAMP_FINISH );
    }

    /* Node Array 초기화 */
    STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                      &sNodeArray,
                                      aEnv )
             == STL_SUCCESS );

    /* Node Array 생성 */
    STL_TRY( qlncMakeNodeArrayOnCurrNode( &sNodeArray,
                                          aSemiJoinTableNode->mRightNode,
                                          aEnv )
             == STL_SUCCESS );

    /* Join Condition은 And List들로만 구성되어야 하고,
     * Function은 모두 Equal Operator로 구성되어야 한다.
     * 또한, Constant Filter이면 안된다. */
    *aOutIsPossible = STL_FALSE;
    for( i = 0; i < aSemiJoinTableNode->mJoinCondition->mOrCount; i++ )
    {
        if( aSemiJoinTableNode->mJoinCondition->mOrFilters[i]->mFilterCount > 1 )
        {
            *aOutIsPossible = STL_FALSE;
            STL_THROW( RAMP_FINISH );
        }

        /* SubQuery Filter가 존재하면 제외 */
        if( aSemiJoinTableNode->mJoinCondition->mOrFilters[i]->mSubQueryExprList != NULL )
        {
            /* @todo 여기에 들어오지 않는 것으로 판단됨 */

            continue;
        }

        if( aSemiJoinTableNode->mJoinCondition->mOrFilters[i]->mFilters[0]->mType
            == QLNC_EXPR_TYPE_BOOLEAN_FILTER )
        {
            sBooleanFilter =
                (qlncBooleanFilter*)(aSemiJoinTableNode->mJoinCondition->mOrFilters[i]->mFilters[0]);

            if( sBooleanFilter->mPossibleJoinCond == STL_FALSE )
            {
                *aOutIsPossible = STL_FALSE;
                STL_THROW( RAMP_FINISH );
            }

            STL_DASSERT( (sBooleanFilter->mLeftRefNodeList != NULL) &&
                         (sBooleanFilter->mRightRefNodeList != NULL) );

            if( sBooleanFilter->mFuncID != KNL_BUILTIN_FUNC_IS_EQUAL )
            {
                *aOutIsPossible = STL_FALSE;
                STL_THROW( RAMP_FINISH );
            }

            /* Left/Right Reference Node List에 대하여
             * 한쪽에 Left Node가 다른 한쪽에 Right Node가 존재해야 함. */
            if( qlncIsExistNode( aSemiJoinTableNode->mRightNode,
                                 sBooleanFilter->mLeftRefNodeList->mHead->mNode ) )
            {
                if( qlncIsExistNode( aSemiJoinTableNode->mRightNode,
                                     sBooleanFilter->mRightRefNodeList->mHead->mNode ) )
                {
                    /* @todo 여기에 들어오는 경우가 없는 것으로 판단됨 */

                    *aOutIsPossible = STL_FALSE;
                    STL_THROW( RAMP_FINISH );
                }

                if( !dtlCheckKeyCompareType( ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0]->mDataType ) )
                {
                    continue;
                }
            }
            else if( qlncIsExistNode( aSemiJoinTableNode->mRightNode,
                                              sBooleanFilter->mRightRefNodeList->mHead->mNode ) )
            {
                if( qlncIsExistNode( aSemiJoinTableNode->mRightNode,
                                     sBooleanFilter->mLeftRefNodeList->mHead->mNode ) )
                {
                    /* @todo 여기에 들어오는 경우가 없는 것으로 판단됨 */

                    *aOutIsPossible = STL_FALSE;
                    STL_THROW( RAMP_FINISH );
                }

                if( !dtlCheckKeyCompareType( ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1]->mDataType ) )
                {
                    continue;
                }
            }
            else
            {
                /* Sort Key가 존재하지 않는 Filter */
                continue;
            }

            *aOutIsPossible = STL_TRUE;
        }
        else
        {
            *aOutIsPossible = STL_FALSE;
            STL_THROW( RAMP_FINISH );
        }
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 가능한 Join Operation를 찾는다.
 * @param[in]   aSQLString                  SQL String
 * @param[in]   aJoinCondition              Join Condition
 * @param[in]   aJoinType                   Join Type
 * @param[in]   aLeftNode                   Left Table Node
 * @param[in]   aRightNode                  Right Table Node
 * @param[in]   aIsSemiJoin                 Semi-Join/Anti-Semi-Join인지 여부
 * @param[out]  aPossibleNestedJoin         Nested Join이 가능한지 여부
 * @param[out]  aPossibleIndexNestedJoin    Index Nested Join이 가능한지 여부
 * @param[out]  aPossibleInvertedNestedJoin Inverted Nested Join이 가능한지 여부
 * @param[out]  aPossibleMergeJoin          Merge Join이 가능한지 여부
 * @param[out]  aPossibleHashJoin           Hash Join이 가능한지 여부
 * @param[out]  aPossibleInvertedHashJoin   Inverted Hash Join이 가능한지 여부
 * @param[in]   aEnv                        Environment
 */
stlStatus qlncFindPossibleJoinOperation( stlChar        * aSQLString,
                                         qlncAndFilter  * aJoinCondition,
                                         qlncJoinType     aJoinType,
                                         qlncNodeCommon * aLeftNode,
                                         qlncNodeCommon * aRightNode,
                                         stlBool          aIsSemiJoin,
                                         stlBool        * aPossibleNestedJoin,
                                         stlBool        * aPossibleIndexNestedJoin,
                                         stlBool        * aPossibleInvertedNestedJoin,
                                         stlBool        * aPossibleMergeJoin,
                                         stlBool        * aPossibleHashJoin,
                                         stlBool        * aPossibleInvertedHashJoin,
                                         qllEnv         * aEnv )
{
    stlBool               sHasEquiMergeJoin;
    stlBool               sHasEquiHashJoin;
    stlBool               sHasInvertedEquiHashJoin;
    stlInt32              i;
    qlncJoinOperType      sJoinOperType;
    stlBool               sPossibleInvertedNestedJoin;
    stlBool               sPossibleInvertedHashJoin;
    stlBool               sIsExistJoinOperOnLeft;


    sPossibleInvertedNestedJoin = STL_FALSE;
    sPossibleInvertedHashJoin = STL_FALSE;

    /* Join Condition이 없으면 Nested Loops Join만 가능 */
    if( aJoinCondition == NULL )
    {
        *aPossibleNestedJoin = STL_TRUE;
        *aPossibleIndexNestedJoin = STL_FALSE;
        *aPossibleMergeJoin = STL_FALSE;
        *aPossibleHashJoin = STL_FALSE;

        STL_THROW( RAMP_FINISH );
    }

    /* Join Condition에 Equi-Join 조건이 존재하는지 검사 */
    sHasEquiMergeJoin = STL_FALSE;
    for( i = 0; i < aJoinCondition->mOrCount; i++ )
    {
        if( (aJoinCondition->mOrFilters[i]->mFilterCount == 1) &&
            (aJoinCondition->mOrFilters[i]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER) )
        {
            STL_TRY( qlncIsPossibleMergeJoinCondition(
                         aSQLString,
                         (qlncBooleanFilter*)(aJoinCondition->mOrFilters[i]->mFilters[0]),
                         aRightNode,
                         &sHasEquiMergeJoin,
                         aEnv )
                     == STL_SUCCESS );

            if( sHasEquiMergeJoin == STL_TRUE )
            {
                break;
            }
        }
    }
    
    /* Join Condition에 SubQuery Expression이 존재하는지 검사 */
    for( i = 0; i < aJoinCondition->mOrCount; i++ )
    {
        if( aJoinCondition->mOrFilters[i]->mSubQueryExprList != NULL )
        {
            break;
        }
    }

    /* Join Condition에 SubQuery Expression이 있는 경우 Nested Loops Join만 가능 */
    if( i < aJoinCondition->mOrCount )
    {
        *aPossibleNestedJoin = STL_TRUE;
        *aPossibleIndexNestedJoin = STL_FALSE;
        *aPossibleMergeJoin = STL_FALSE;
        *aPossibleHashJoin = STL_FALSE;

        STL_THROW( RAMP_FINISH );
    }


    /* Join Condition에 Hash Equi-Join 조건이 존재하는지 검사 */
    sHasEquiHashJoin = STL_FALSE;
    for( i = 0; i < aJoinCondition->mOrCount; i++ )
    {
        if( (aJoinCondition->mOrFilters[i]->mFilterCount == 1) &&
            (aJoinCondition->mOrFilters[i]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER) )
        {
            STL_TRY( qlncIsPossibleHashJoinCondition(
                         aSQLString,
                         (qlncBooleanFilter*)(aJoinCondition->mOrFilters[i]->mFilters[0]),
                         aRightNode,
                         NULL,
                         &sHasEquiHashJoin,
                         aEnv )
                     == STL_SUCCESS );

            if( sHasEquiHashJoin == STL_TRUE )
            {
                break;
            }
        }
    }

    /* Join Condition에 Inverted Hash Equi-Join 조건이 존재하는지 검사 */
    sHasInvertedEquiHashJoin = STL_FALSE;
    for( i = 0; i < aJoinCondition->mOrCount; i++ )
    {
        if( (aJoinCondition->mOrFilters[i]->mFilterCount == 1) &&
            (aJoinCondition->mOrFilters[i]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER) )
        {
            STL_TRY( qlncIsPossibleHashJoinCondition(
                         aSQLString,
                         (qlncBooleanFilter*)(aJoinCondition->mOrFilters[i]->mFilters[0]),
                         aLeftNode,
                         NULL,
                         &sHasInvertedEquiHashJoin,
                         aEnv )
                     == STL_SUCCESS );

            if( sHasInvertedEquiHashJoin == STL_TRUE )
            {
                break;
            }
        }
    }

    /* 모든 Join Operation이 가능한 경우로 Join Hint를 보고 판별 */
    if( (aLeftNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE) &&
        (aRightNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE) &&
        (aIsSemiJoin == STL_FALSE) )
    {
        /* Right Table Node가 Base Table 또는 Sub Table이 아닌 경우 모두 가능 */
        *aPossibleNestedJoin = STL_TRUE;
        *aPossibleIndexNestedJoin = STL_TRUE;
        *aPossibleMergeJoin = sHasEquiMergeJoin;
        *aPossibleHashJoin = sHasEquiHashJoin;

        STL_THROW( RAMP_FINISH );
    }

    /* Join Operation Hint는 Right 우선 적용 */
    if( aRightNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
    {
        sJoinOperType = ((qlncBaseTableNode*)aRightNode)->mJoinHint->mJoinOperation;
        sPossibleInvertedNestedJoin = ((qlncBaseTableNode*)aRightNode)->mJoinHint->mIsInverted;
        sPossibleInvertedHashJoin = ((qlncBaseTableNode*)aRightNode)->mJoinHint->mIsInverted;
    }
    else if( aRightNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
    {
        sJoinOperType = ((qlncSubTableNode*)aRightNode)->mJoinHint->mJoinOperation;
        sPossibleInvertedNestedJoin = ((qlncSubTableNode*)aRightNode)->mJoinHint->mIsInverted;
        sPossibleInvertedHashJoin = ((qlncSubTableNode*)aRightNode)->mJoinHint->mIsInverted;
    }
    else
    {
        if( aIsSemiJoin == STL_FALSE )
        {
            sJoinOperType = QLNC_JOIN_OPER_TYPE_DEFAULT;
            sPossibleInvertedNestedJoin = STL_FALSE;
            sPossibleInvertedHashJoin = STL_FALSE;
        }
        else
        {
            /* Semi-Join/Anti-Semi-Join인 경우
             * Right의 Join Node, Sort Instant Node Hash Instant Node에 한하여
             * Join Operation이 지정되어 있을 수 있다. */
            if( aRightNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE )
            {
                sJoinOperType = ((qlncJoinTableNode*)aRightNode)->mSemiJoinHint->mJoinOperation;
                sPossibleInvertedNestedJoin = ((qlncJoinTableNode*)aRightNode)->mSemiJoinHint->mIsInverted;
                sPossibleInvertedHashJoin = ((qlncJoinTableNode*)aRightNode)->mSemiJoinHint->mIsInverted;
            }
            else if( aRightNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
            {
                sJoinOperType = ((qlncSortInstantNode*)aRightNode)->mSemiJoinHint->mJoinOperation;
                sPossibleInvertedNestedJoin = ((qlncSortInstantNode*)aRightNode)->mSemiJoinHint->mIsInverted;
                sPossibleInvertedHashJoin = ((qlncSortInstantNode*)aRightNode)->mSemiJoinHint->mIsInverted;
            }
            else
            {
                STL_DASSERT( aRightNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT );
                sJoinOperType = ((qlncHashInstantNode*)aRightNode)->mSemiJoinHint->mJoinOperation;
                sPossibleInvertedNestedJoin = ((qlncHashInstantNode*)aRightNode)->mSemiJoinHint->mIsInverted;
                sPossibleInvertedHashJoin = ((qlncHashInstantNode*)aRightNode)->mSemiJoinHint->mIsInverted;
            }
        }
    }

    if( sJoinOperType == QLNC_JOIN_OPER_TYPE_DEFAULT )
    {
        sPossibleInvertedNestedJoin = STL_TRUE;
        sPossibleInvertedHashJoin = STL_TRUE;

        if( aLeftNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
        {
            sJoinOperType = ((qlncBaseTableNode*)aLeftNode)->mJoinHint->mJoinOperation;
        }
        else if( aLeftNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
        {
            sJoinOperType = ((qlncSubTableNode*)aLeftNode)->mJoinHint->mJoinOperation;
        }
        else
        {
            sJoinOperType = QLNC_JOIN_OPER_TYPE_DEFAULT;
        }

        sIsExistJoinOperOnLeft = STL_TRUE;
    }
    else
    {
        sIsExistJoinOperOnLeft = STL_FALSE;
    }

    if( sJoinOperType != QLNC_JOIN_OPER_TYPE_DEFAULT )
    {
        if( sJoinOperType != QLNC_JOIN_OPER_TYPE_USE_NESTED )
        {
            sPossibleInvertedNestedJoin = STL_FALSE;
        }

        if( sJoinOperType != QLNC_JOIN_OPER_TYPE_USE_HASH )
        {
            sPossibleInvertedHashJoin = STL_FALSE;
        }
    }

    if( sHasInvertedEquiHashJoin == STL_FALSE )
    {
        sPossibleInvertedHashJoin = STL_FALSE;
    }

    switch( sJoinOperType )
    {
        case QLNC_JOIN_OPER_TYPE_DEFAULT:
            *aPossibleNestedJoin = STL_TRUE;
            *aPossibleIndexNestedJoin = STL_TRUE;
            *aPossibleMergeJoin = sHasEquiMergeJoin;
            *aPossibleHashJoin = sHasEquiHashJoin;
            break;
        case QLNC_JOIN_OPER_TYPE_USE_NESTED:
            if( (sPossibleInvertedNestedJoin == STL_TRUE) &&
                (sIsExistJoinOperOnLeft == STL_FALSE) )
            {
                *aPossibleNestedJoin = STL_FALSE;
            }
            else
            {
                *aPossibleNestedJoin = STL_TRUE;
            }
            *aPossibleIndexNestedJoin = STL_FALSE;
            *aPossibleMergeJoin = STL_FALSE;
            *aPossibleHashJoin = STL_FALSE;
            break;
        case QLNC_JOIN_OPER_TYPE_NO_USE_NESTED:
            *aPossibleNestedJoin = STL_FALSE;
            *aPossibleIndexNestedJoin = STL_TRUE;
            *aPossibleMergeJoin = sHasEquiMergeJoin;
            *aPossibleHashJoin = sHasEquiHashJoin;
            break;
        case QLNC_JOIN_OPER_TYPE_USE_INDEX_NESTED:
            if( aJoinType == QLNC_JOIN_TYPE_INNER )
            {
                *aPossibleNestedJoin = STL_FALSE;
                *aPossibleIndexNestedJoin = STL_TRUE;
                *aPossibleMergeJoin = STL_FALSE;
                *aPossibleHashJoin = STL_FALSE;
            }
            else
            {
                *aPossibleNestedJoin = STL_TRUE;
                *aPossibleIndexNestedJoin = STL_FALSE;
                *aPossibleMergeJoin = sHasEquiMergeJoin;
                *aPossibleHashJoin = sHasEquiHashJoin;
            }
            break;
        case QLNC_JOIN_OPER_TYPE_NO_USE_INDEX_NESTED:
            *aPossibleNestedJoin = STL_TRUE;
            *aPossibleIndexNestedJoin = STL_FALSE;
            *aPossibleMergeJoin = sHasEquiMergeJoin;
            *aPossibleHashJoin = sHasEquiHashJoin;
            break;
        case QLNC_JOIN_OPER_TYPE_USE_MERGE:
            if( sHasEquiMergeJoin == STL_TRUE )
            {
                *aPossibleNestedJoin = STL_FALSE;
                *aPossibleIndexNestedJoin = STL_FALSE;
                *aPossibleMergeJoin = STL_TRUE;
                *aPossibleHashJoin = STL_FALSE;
            }
            else
            {
                *aPossibleNestedJoin = STL_TRUE;
                *aPossibleIndexNestedJoin = STL_TRUE;
                *aPossibleMergeJoin = STL_FALSE;
                *aPossibleHashJoin = sHasEquiHashJoin;
            }
            break;
        case QLNC_JOIN_OPER_TYPE_NO_USE_MERGE:
            *aPossibleNestedJoin = STL_TRUE;
            *aPossibleIndexNestedJoin = STL_TRUE;
            *aPossibleMergeJoin = STL_FALSE;
            *aPossibleHashJoin = sHasEquiHashJoin;
            break;
        case QLNC_JOIN_OPER_TYPE_USE_HASH:
            if( sHasEquiHashJoin == STL_TRUE )
            {
                *aPossibleNestedJoin = STL_FALSE;
                *aPossibleIndexNestedJoin = STL_FALSE;
                *aPossibleMergeJoin = STL_FALSE;
                if( (sPossibleInvertedHashJoin == STL_TRUE) &&
                    (sIsExistJoinOperOnLeft == STL_FALSE) )
                {
                    *aPossibleHashJoin = STL_FALSE;
                }
                else
                {
                    *aPossibleHashJoin = STL_TRUE;
                }
            }
            else
            {
                *aPossibleNestedJoin = STL_TRUE;
                *aPossibleIndexNestedJoin = STL_TRUE;
                *aPossibleMergeJoin = sHasEquiMergeJoin;
                *aPossibleHashJoin = STL_FALSE;
                sPossibleInvertedHashJoin = STL_FALSE;
            }
            break;
        case QLNC_JOIN_OPER_TYPE_NO_USE_HASH:
            *aPossibleNestedJoin = STL_TRUE;
            *aPossibleIndexNestedJoin = STL_TRUE;
            *aPossibleMergeJoin = sHasEquiMergeJoin;
            *aPossibleHashJoin = STL_FALSE;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    STL_RAMP( RAMP_FINISH );

    if( aPossibleInvertedNestedJoin != NULL )
    {
        *aPossibleInvertedNestedJoin = sPossibleInvertedNestedJoin;
    }

    if( aPossibleInvertedHashJoin != NULL )
    {
        *aPossibleInvertedHashJoin = sPossibleInvertedHashJoin;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expected And Filter Result Count를 계산한다.
 * @param[in]   aAndFilter              And Filter
 * @param[in]   aIncludeStmtSubQuery    Statement 단위의 SubQuery에 대하여 Result Count 계산에 포함할지 여부
 * @param[in]   aOrgRowCount            Original Row Count
 */
stlFloat64 qlncGetExpectedAndFilterResultCount( qlncAndFilter   * aAndFilter,
                                                stlBool           aIncludeStmtSubQuery,
                                                stlFloat64        aOrgRowCount )
{
    stlInt32      i;
    stlFloat64    sResultRowCount;


    sResultRowCount = aOrgRowCount;
    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        sResultRowCount =
            qlncGetExpectedOrFilterResultRowCount(
                aAndFilter->mOrFilters[i],
                aIncludeStmtSubQuery,
                sResultRowCount );
    }

    return sResultRowCount;
}


/**
 * @brief Expected Or Filter Result Count를 계산한다.
 * @param[in]   aOrFilter               Or Filter
 * @param[in]   aIncludeStmtSubQuery    Statement 단위의 SubQuery에 대하여 Result Count 계산에 포함할지 여부
 * @param[in]   aOrgRowCount            Original Row Count
 */
stlFloat64 qlncGetExpectedOrFilterResultRowCount( qlncOrFilter  * aOrFilter,
                                                  stlBool         aIncludeStmtSubQuery,
                                                  stlFloat64      aOrgRowCount )
{
    stlInt32      i;
    stlFloat64    sResultRowCount;


    sResultRowCount = (stlFloat64)0.0;
    for( i = 0; i < aOrFilter->mFilterCount; i++ )
    {
        sResultRowCount +=
            qlncGetExpectedResultCountByFilter(
                aOrFilter->mFilters[i],
                aIncludeStmtSubQuery,
                (aOrgRowCount - sResultRowCount) );
    }

    return sResultRowCount;
}


/**
 * @brief Expected Inner Join Result Count를 계산한다.
 * @param[in]   aCBOptParamInfo         Cost Based Optimizer Parameter Info
 * @param[in]   aInnerJoinNodeInfoArr   Inner Join Node Info Array
 * @param[in]   aNodeCount              Node Count
 * @param[out]  aResultRowCount         Result Row Count
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncGetExpectedInnerJoinResultRowCount( qlncCBOptParamInfo    * aCBOptParamInfo,
                                                  qlncInnerJoinNodeInfo * aInnerJoinNodeInfoArr,
                                                  stlInt32                aNodeCount,
                                                  stlFloat64            * aResultRowCount,
                                                  qllEnv                * aEnv )
{
    stlInt32              i, j;
    knlRegionMark         sRegionMemMark;

    qlncRefColumnList   * sColListArr;

    qlncAndFilter       * sAndFilter;
    qlncExprFunction    * sFunction;

    qlncBooleanFilter  ** sEquiJoinCondArr  = NULL;
    stlInt32              sEquiJoinCondCount;

    stlFloat64            sTmpResultRate;

    stlFloat64            sResultRowCount;
    stlFloat64            sEstCard;
    stlFloat64            sTmpEstCard;
    stlBool               sIsMatchedUnique;
    stlBool               sTmpIsMatchedUnique;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* 초기 Result Row Count 계산 */
    STL_DASSERT( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE );
    STL_DASSERT( aCBOptParamInfo->mRightNode->mIsAnalyzedCBO == STL_TRUE );
    sResultRowCount =
        ( aCBOptParamInfo->mLeftNode->mBestCost->mResultRowCount *
          aCBOptParamInfo->mRightNode->mBestCost->mResultRowCount );

    /* Region Memory Mark */
    KNL_INIT_REGION_MARK( &sRegionMemMark );
    STL_TRY( knlMarkRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                               &sRegionMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Column List Array 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncRefColumnList ) * aNodeCount,
                                (void**) &sColListArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < aNodeCount; i++ )
    {
        QLNC_INIT_LIST( &sColListArr[i] );
    }

    /* Equi-Join Condition Array 생성 */
    if( aCBOptParamInfo->mJoinCond != NULL )
    {
        STL_TRY( knlAllocRegionMem(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     STL_SIZEOF( qlncBooleanFilter* ) * aCBOptParamInfo->mJoinCond->mOrCount,
                     (void**) &sEquiJoinCondArr,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sEquiJoinCondCount = 0;
    }
    else
    {
        sEquiJoinCondArr = NULL;
        sEquiJoinCondCount = 0;
    }


    /**
     * Left Node와 Right Node가 모두 Base Table Node인 경우이고
     * Column간 Equi-Join 조건인 경우에만 보다 정확한 Result Size를 구할 수 있다.
     */

    /* Join Condition 체크 */
    if( aCBOptParamInfo->mJoinCond != NULL )
    {
        sAndFilter = aCBOptParamInfo->mJoinCond;
        for( i = 0; i < sAndFilter->mOrCount; i++ )
        {
            if( (sAndFilter->mOrFilters[i]->mFilterCount == 1) &&
                (sAndFilter->mOrFilters[i]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER) )
            {
                if( (((qlncBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[0]))->mRefNodeList.mCount == 2) &&
                    (((qlncBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[0]))->mFuncID ==
                     KNL_BUILTIN_FUNC_IS_EQUAL) )
                {
                    sFunction = (qlncExprFunction*)(((qlncBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[0]))->mExpr);
                    if( (sFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_COLUMN) &&
                        (sFunction->mArgs[1]->mType == QLNC_EXPR_TYPE_COLUMN) )
                    {
                        for( j = 0; j < aNodeCount; j++ )
                        {
                            if( aInnerJoinNodeInfoArr[j].mNode->mNodeID ==
                                ((qlncExprColumn*)(sFunction->mArgs[0]))->mNode->mNodeID )
                            {
                                STL_TRY( qlncAddColumnToRefColumnList( &(aCBOptParamInfo->mParamInfo),
                                                                       sFunction->mArgs[0],
                                                                       &sColListArr[j],
                                                                       aEnv )
                                         == STL_SUCCESS );
                                break;
                            }
                        }

                        for( j = 0; j < aNodeCount; j++ )
                        {
                            if( aInnerJoinNodeInfoArr[j].mNode->mNodeID ==
                                ((qlncExprColumn*)(sFunction->mArgs[1]))->mNode->mNodeID )
                            {
                                STL_TRY( qlncAddColumnToRefColumnList( &(aCBOptParamInfo->mParamInfo),
                                                                       sFunction->mArgs[1],
                                                                       &sColListArr[j],
                                                                       aEnv )
                                         == STL_SUCCESS );
                                break;
                            }
                        }

                        sEquiJoinCondArr[sEquiJoinCondCount] =
                            (qlncBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[0]);
                        sEquiJoinCondCount++;

                        continue;
                    }
                }
            }

            /* Pure Equi-Join 조건이 아닌 경우 */
            sResultRowCount = qlncGetExpectedOrFilterResultRowCount( sAndFilter->mOrFilters[i],
                                                                     STL_FALSE,
                                                                     sResultRowCount );
        }
    }

    /* Pure Equi-Join이 존재하는 경우 */
    sIsMatchedUnique = STL_FALSE;
    sEstCard = (stlFloat64)0.0;
    for( i = 0; i < aNodeCount; i++ )
    {
        if( sColListArr[i].mCount > 0 )
        {
            sTmpEstCard = qlncGetCardEquiJoinColumnList(
                              (qlncBaseTableNode*)(aInnerJoinNodeInfoArr[i].mNode),
                              0,
                              &sColListArr[i],
                              sEquiJoinCondCount,
                              sEquiJoinCondArr,
                              &sTmpResultRate,
                              &sTmpIsMatchedUnique );

            if( sTmpIsMatchedUnique == STL_TRUE )
            {
                if( i == (aNodeCount - 1) )
                {
                    /* Right Node에 존재하는 Unique Column */
                    sTmpEstCard = ((qlncBaseTableNode*)(aInnerJoinNodeInfoArr[i].mNode))->mTableStat->mRowCount;
                }
                else
                {
                    /* Left Node에 존재하는 Unique Column */
                    sTmpEstCard = aCBOptParamInfo->mRightNode->mBestCost->mResultRowCount / sTmpResultRate;
                    if( aInnerJoinNodeInfoArr[aNodeCount-1].mNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
                    {
                        sTmpEstCard *= ( ((qlncBaseTableNode*)(aInnerJoinNodeInfoArr[aNodeCount-1].mNode))->mTableStat->mRowCount / aCBOptParamInfo->mRightNode->mBestCost->mResultRowCount);
                    }
                }

                if( sIsMatchedUnique == STL_FALSE )
                {
                    sEstCard = sTmpEstCard;
                }
                else
                {
                    if( sTmpEstCard > sEstCard )
                    {
                        sEstCard = sTmpEstCard;
                    }
                }

                sIsMatchedUnique = sTmpIsMatchedUnique;
            }
            else
            {
                if( (sIsMatchedUnique == STL_FALSE) &&
                    (sTmpEstCard > sEstCard) )
                {
                    sEstCard = sTmpEstCard;
                }
            }
        }
    }

    if( sEstCard != (stlFloat64)0.0 )
    {
        sResultRowCount /= sEstCard;
    }

    /* Join Filter 체크 */
    if( aCBOptParamInfo->mFilter != NULL )
    {
        sAndFilter = aCBOptParamInfo->mFilter;
        for( i = 0; i < sAndFilter->mOrCount; i++ )
        {
            sResultRowCount = qlncGetExpectedOrFilterResultRowCount( sAndFilter->mOrFilters[i],
                                                                     STL_FALSE,
                                                                     sResultRowCount );
        }
    }

    /* Region Memory UnMark */
    STL_TRY( knlFreeUnmarkedRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                       &sRegionMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Output 설정
     */

    *aResultRowCount = sResultRowCount;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expected Outer Join Result Count를 계산한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aJoinDirect     Join Direct
 * @param[out]  aResultRowCount Result Row Count
 * @param[in]   aEnv            Environment
 */
stlStatus qlncGetExpectedOuterJoinResultRowCount( qlncCBOptParamInfo    * aCBOptParamInfo,
                                                  qlncJoinDirect          aJoinDirect,
                                                  stlFloat64            * aResultRowCount,
                                                  qllEnv                * aEnv )
{
    stlInt32              i;

    qlncRefColumnList     sLeftColList;
    qlncRefColumnList     sRightColList;

    qlncAndFilter       * sAndFilter;
    qlncExprFunction    * sFunction;

    stlFloat64            sResultRate;
    stlFloat64            sResultRowCount;
    stlFloat64            sLeftEstCard;
    stlFloat64            sRightEstCard;
    stlBool               sIsMatchedUnique;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* 초기 Result Row Count 계산 */
    STL_DASSERT( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE );
    STL_DASSERT( aCBOptParamInfo->mRightNode->mIsAnalyzedCBO == STL_TRUE );
    sResultRowCount = aCBOptParamInfo->mLeftNode->mBestCost->mResultRowCount *
        aCBOptParamInfo->mRightNode->mBestCost->mResultRowCount;


    /**
     * Left Node와 Right Node가 모두 Base Table Node인 경우이고
     * Column간 Equi-Join 조건인 경우에만 보다 정확한 Result Size를 구할 수 있다.
     */

    QLNC_INIT_LIST( &sLeftColList );
    QLNC_INIT_LIST( &sRightColList );

    /* Join Condition 체크 */
    if( aCBOptParamInfo->mJoinCond != NULL )
    {
        sAndFilter = aCBOptParamInfo->mJoinCond;
        for( i = 0; i < sAndFilter->mOrCount; i++ )
        {
            if( (sAndFilter->mOrFilters[i]->mFilterCount == 1) &&
                (sAndFilter->mOrFilters[i]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER) )
            {
                if( (((qlncBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[0]))->mRefNodeList.mCount == 2) &&
                    (((qlncBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[0]))->mFuncID ==
                     KNL_BUILTIN_FUNC_IS_EQUAL) )
                {
                    sFunction = (qlncExprFunction*)(((qlncBooleanFilter*)(sAndFilter->mOrFilters[i]->mFilters[0]))->mExpr);
                    if( (sFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_COLUMN) &&
                        (sFunction->mArgs[1]->mType == QLNC_EXPR_TYPE_COLUMN) )
                    {
                        if( qlncIsExistNode( aCBOptParamInfo->mLeftNode,
                                             ((qlncExprColumn*)(sFunction->mArgs[0]))->mNode ) )
                        {
                            if( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
                            {
                                STL_TRY( qlncAddColumnToRefColumnList( &(aCBOptParamInfo->mParamInfo),
                                                                       sFunction->mArgs[0],
                                                                       &sLeftColList,
                                                                       aEnv )
                                         == STL_SUCCESS );
                            }

                            if( aCBOptParamInfo->mRightNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
                            {
                                STL_TRY( qlncAddColumnToRefColumnList( &(aCBOptParamInfo->mParamInfo),
                                                                       sFunction->mArgs[1],
                                                                       &sRightColList,
                                                                       aEnv )
                                         == STL_SUCCESS );
                            }
                        }
                        else
                        {
                            if( aCBOptParamInfo->mRightNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
                            {
                                STL_TRY( qlncAddColumnToRefColumnList( &(aCBOptParamInfo->mParamInfo),
                                                                       sFunction->mArgs[0],
                                                                       &sRightColList,
                                                                       aEnv )
                                         == STL_SUCCESS );
                            }

                            if( aCBOptParamInfo->mLeftNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
                            {
                                STL_TRY( qlncAddColumnToRefColumnList( &(aCBOptParamInfo->mParamInfo),
                                                                       sFunction->mArgs[1],
                                                                       &sLeftColList,
                                                                       aEnv )
                                         == STL_SUCCESS );
                            }
                        }

                        continue;
                    }
                }
            }

            /* Pure Equi-Join 조건이 아닌 경우 */
            sResultRowCount = qlncGetExpectedOrFilterResultRowCount( sAndFilter->mOrFilters[i],
                                                                     STL_FALSE,
                                                                     sResultRowCount );
        }
    }

    /* Pure Equi-Join이 존재하는 경우 */
    if( sLeftColList.mCount > 0 )
    {
        sLeftEstCard = qlncGetCardEquiJoinColumnList(
                           (qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode),
                           0,
                           &sLeftColList,
                           0,
                           NULL,
                           &sResultRate,
                           &sIsMatchedUnique );

        if( sLeftEstCard > (stlFloat64)10.0 )
        {
            STL_DASSERT( aCBOptParamInfo->mLeftNode->mBestCost != NULL );
            sLeftEstCard = aCBOptParamInfo->mLeftNode->mBestCost->mResultRowCount;
        }
    }
    else
    {
        sLeftEstCard = (stlFloat64)0.0;
    }

    if( sRightColList.mCount > 0 )
    {
        sRightEstCard = qlncGetCardEquiJoinColumnList(
                            (qlncBaseTableNode*)(aCBOptParamInfo->mRightNode),
                            0,
                            &sRightColList,
                            0,
                            NULL,
                            &sResultRate,
                            &sIsMatchedUnique );

        if( sRightEstCard > (stlFloat64)10.0 )
        {
            STL_DASSERT( aCBOptParamInfo->mRightNode->mBestCost != NULL );
            sRightEstCard = aCBOptParamInfo->mRightNode->mBestCost->mResultRowCount;
        }
    }
    else
    {
        sRightEstCard = (stlFloat64)0.0;
    }

    if( (sLeftEstCard != (stlFloat64)0.0) ||
        (sRightEstCard != (stlFloat64)0.0) )
    {
        if( sLeftEstCard > sRightEstCard )
        {
            sResultRowCount /= sLeftEstCard;
        }
        else
        {
            sResultRowCount /= sRightEstCard;
        }
    }

    /* Join Filter 체크 */
    if( aCBOptParamInfo->mFilter != NULL )
    {
        sAndFilter = aCBOptParamInfo->mFilter;
        for( i = 0; i < sAndFilter->mOrCount; i++ )
        {
            sResultRowCount = qlncGetExpectedOrFilterResultRowCount( sAndFilter->mOrFilters[i],
                                                                     STL_FALSE,
                                                                     sResultRowCount );
        }
    }
    

    /**
     * Output 설정
     */

    if( aJoinDirect == QLNC_JOIN_DIRECT_LEFT )
    {
        *aResultRowCount =
            sResultRowCount +
            (aCBOptParamInfo->mLeftNode->mBestCost->mResultRowCount - sResultRowCount);
    }
    else if( aJoinDirect == QLNC_JOIN_DIRECT_RIGHT )
    {
        /* @todo 여기에 들어오는 경우가 없는 것으로 판단됨 */

        *aResultRowCount =
            sResultRowCount +
            (aCBOptParamInfo->mRightNode->mBestCost->mResultRowCount - sResultRowCount);
    }
    else
    {
        *aResultRowCount =
            sResultRowCount +
            (aCBOptParamInfo->mLeftNode->mBestCost->mResultRowCount - sResultRowCount) +
            (aCBOptParamInfo->mRightNode->mBestCost->mResultRowCount - sResultRowCount);
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expected Semi Join Result Count를 계산한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aJoinDirect     Join Direct
 * @param[out]  aResultRowCount Result Row Count
 * @param[in]   aEnv            Environment
 */
stlStatus qlncGetExpectedSemiJoinResultRowCount( qlncCBOptParamInfo     * aCBOptParamInfo,
                                                 qlncJoinDirect           aJoinDirect,
                                                 stlFloat64             * aResultRowCount,
                                                 qllEnv                 * aEnv )
{
    stlInt32              i;

    qlncAndFilter       * sAndFilter;

    stlFloat64            sResultRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* 초기 Result Row Count 계산 */
    STL_DASSERT( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE );
    STL_DASSERT( aCBOptParamInfo->mRightNode->mIsAnalyzedCBO == STL_TRUE );

    if( aJoinDirect == QLNC_JOIN_DIRECT_LEFT )
    {
        sResultRowCount = aCBOptParamInfo->mLeftNode->mBestCost->mResultRowCount;
    }
    else
    {
        /* @todo 여기에 들어오는 경우가 없는 것으로 판단됨 */

        STL_DASSERT( aJoinDirect == QLNC_JOIN_DIRECT_RIGHT );
        sResultRowCount = aCBOptParamInfo->mRightNode->mBestCost->mResultRowCount;
    }

    /* Join Condition 체크 */
    if( aCBOptParamInfo->mJoinCond != NULL )
    {
        sAndFilter = aCBOptParamInfo->mJoinCond;
        for( i = 0; i < sAndFilter->mOrCount; i++ )
        {
            sResultRowCount = qlncGetExpectedOrFilterResultRowCount( sAndFilter->mOrFilters[i],
                                                                     STL_FALSE,
                                                                     sResultRowCount );
        }
    }

    /* Join Filter 체크 */
    if( aCBOptParamInfo->mFilter != NULL )
    {
        sAndFilter = aCBOptParamInfo->mFilter;
        for( i = 0; i < sAndFilter->mOrCount; i++ )
        {
            sResultRowCount = qlncGetExpectedOrFilterResultRowCount( sAndFilter->mOrFilters[i],
                                                                     STL_FALSE,
                                                                     sResultRowCount );
        }
    }
    

    /**
     * Output 설정
     */

    *aResultRowCount = sResultRowCount;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expected Anti Semi Join Result Count를 계산한다.
 * @param[in]   aCBOptParamInfo Cost Based Optimizer Parameter Info
 * @param[in]   aJoinDirect     Join Direct
 * @param[out]  aResultRowCount Result Row Count
 * @param[in]   aEnv            Environment
 */
stlStatus qlncGetExpectedAntiSemiJoinResultRowCount( qlncCBOptParamInfo     * aCBOptParamInfo,
                                                     qlncJoinDirect           aJoinDirect,
                                                     stlFloat64             * aResultRowCount,
                                                     qllEnv                 * aEnv )
{
    stlInt32              i;

    qlncAndFilter       * sAndFilter;

    stlFloat64            sResultRowCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* 초기 Result Row Count 계산 */
    STL_DASSERT( aCBOptParamInfo->mLeftNode->mIsAnalyzedCBO == STL_TRUE );
    STL_DASSERT( aCBOptParamInfo->mRightNode->mIsAnalyzedCBO == STL_TRUE );

    if( aJoinDirect == QLNC_JOIN_DIRECT_LEFT )
    {
        sResultRowCount = aCBOptParamInfo->mLeftNode->mBestCost->mResultRowCount;
    }
    else
    {
        /* @todo 여기에 들어오는 경우가 없는 것으로 판단됨 */

        STL_DASSERT( aJoinDirect == QLNC_JOIN_DIRECT_RIGHT );
        sResultRowCount = aCBOptParamInfo->mRightNode->mBestCost->mResultRowCount;
    }

    /* Join Condition 체크 */
    if( aCBOptParamInfo->mJoinCond != NULL )
    {
        sAndFilter = aCBOptParamInfo->mJoinCond;
        for( i = 0; i < sAndFilter->mOrCount; i++ )
        {
            sResultRowCount = qlncGetExpectedOrFilterResultRowCount( sAndFilter->mOrFilters[i],
                                                                     STL_FALSE,
                                                                     sResultRowCount );
        }
    }

    /* Join Filter 체크 */
    if( aCBOptParamInfo->mFilter != NULL )
    {
        sAndFilter = aCBOptParamInfo->mFilter;
        for( i = 0; i < sAndFilter->mOrCount; i++ )
        {
            sResultRowCount = qlncGetExpectedOrFilterResultRowCount( sAndFilter->mOrFilters[i],
                                                                     STL_FALSE,
                                                                     sResultRowCount );
        }
    }
    

    /**
     * Output 설정
     */

    if( aJoinDirect == QLNC_JOIN_DIRECT_LEFT )
    {
        *aResultRowCount =
            aCBOptParamInfo->mLeftNode->mBestCost->mResultRowCount - sResultRowCount;
    }
    else
    {
        /* @todo 여기에 들어오는 경우가 없는 것으로 판단됨 */

        STL_DASSERT( aJoinDirect == QLNC_JOIN_DIRECT_RIGHT );
        *aResultRowCount =
            aCBOptParamInfo->mRightNode->mBestCost->mResultRowCount - sResultRowCount;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Equi-Join 조건에 포함된 컬럼 목록을 이용해 Max Cardinality 를 예측한다.
 * @param[in]   aCandBaseTable      Candidate Base Table Node
 * @param[in]   aFindIndexKeyCount  찾을 Index의 key 개수(0: 무제한)
 * @param[in]   aRefColumnList      Reference Column Expression List
 * @param[in]   aEquiJoinCondCount  Equi Join Condition Count
 * @param[in]   aEquiJoinCondArr    Equi Join Condition Array
 * @param[out]  aResultRate         Result Rate
 * @param[out]  aIsMatchedUnique    Unique에 Match 되었는지 여부
 */
stlFloat64 qlncGetCardEquiJoinColumnList( qlncBaseTableNode     * aCandBaseTable,
                                          stlInt32                aFindIndexKeyCount,
                                          qlncRefColumnList     * aRefColumnList,
                                          stlInt32                aEquiJoinCondCount,
                                          qlncBooleanFilter    ** aEquiJoinCondArr,
                                          stlFloat64            * aResultRate,
                                          stlBool               * aIsMatchedUnique )
{
    stlInt32              i, j;
    stlInt32              sIndexKeyCount;
    ellIndexKeyDesc     * sIndexKeyDesc     = NULL;
    qlncRefColumnItem   * sRefColumnItem    = NULL;
    stlFloat64            sEstCard;
    stlFloat64            sOuterNodeRowCount;
    stlBool               sIsMatchedUnique;


    STL_DASSERT( aRefColumnList->mCount > 0 );

    sIsMatchedUnique = STL_FALSE;
    sEstCard = (stlFloat64)0.0;
    
    for( i = 0; i < aCandBaseTable->mIndexCount; i++ )
    {
        /* Unique Index인 경우에 한해서만 체크 */
        if( (aCandBaseTable->mIndexStatArr[i].mIsUnique == STL_TRUE) &&
            ( (aFindIndexKeyCount == 0) ||
              (aCandBaseTable->mIndexStatArr[i].mIndexKeyCount <= aFindIndexKeyCount) ) )
        {
            sIndexKeyCount = aCandBaseTable->mIndexStatArr[i].mIndexKeyCount;
            sIndexKeyDesc  = aCandBaseTable->mIndexStatArr[i].mIndexKeyDesc;

            /* Index에 존재하는 Column이 모두 존재하는지 체크 */
            for( j = 0; j < sIndexKeyCount; j++ )
            {
                if( !qlncIsExistColumnIdxInRefColumnList(
                         aRefColumnList,
                         ellGetColumnIdx( &sIndexKeyDesc[j].mKeyColumnHandle ) ) )
                {
                    break;
                }
            }

            /* Index의 모든 Column이 존재하는 경우 */
            if( j == sIndexKeyCount )
            {
                STL_DASSERT( aCandBaseTable->mNodeCommon.mIsAnalyzedCBO == STL_TRUE );
                sEstCard = aCandBaseTable->mNodeCommon.mBestCost->mResultRowCount;
                sIsMatchedUnique = STL_TRUE;

                sOuterNodeRowCount = (stlFloat64)1.0;
                for( i = 0; i < sIndexKeyCount; i++ )
                {
                    for( j = 0; j < aEquiJoinCondCount; j++ )
                    {
                        if( aEquiJoinCondArr[j] == NULL )
                        {
                            continue;
                        }

                        STL_DASSERT( (aEquiJoinCondArr[j]->mLeftRefNodeList != NULL) &&
                                     (aEquiJoinCondArr[j]->mLeftRefNodeList->mCount == 1) );
                        STL_DASSERT( (aEquiJoinCondArr[j]->mRightRefNodeList != NULL) &&
                                     (aEquiJoinCondArr[j]->mRightRefNodeList->mCount == 1) );

                        if( (aCandBaseTable->mNodeCommon.mNodeID ==
                             aEquiJoinCondArr[j]->mLeftRefNodeList->mHead->mNode->mNodeID) &&
                            (qlncIsExistColumnIdxInRefColumnList(
                                 &(aEquiJoinCondArr[j]->mLeftRefNodeList->mHead->mRefColumnList),
                                 ellGetColumnIdx( &sIndexKeyDesc[i].mKeyColumnHandle ) )) )
                        {
                            STL_DASSERT( aEquiJoinCondArr[j]->mRightRefNodeList->mHead->mNode->mNodeType ==
                                         QLNC_NODE_TYPE_BASE_TABLE );
                            sOuterNodeRowCount *=
                                ((qlncBaseTableNode*)(aEquiJoinCondArr[j]->mRightRefNodeList->mHead->mNode))->mTableStat->mRowCount;
                            aEquiJoinCondArr[j] = NULL;
                        }
                        else if( (aCandBaseTable->mNodeCommon.mNodeID ==
                                  aEquiJoinCondArr[j]->mRightRefNodeList->mHead->mNode->mNodeID) &&
                                 (qlncIsExistColumnIdxInRefColumnList(
                                      &(aEquiJoinCondArr[j]->mRightRefNodeList->mHead->mRefColumnList),
                                      ellGetColumnIdx( &sIndexKeyDesc[i].mKeyColumnHandle ) )) )
                        {
                            STL_DASSERT( aEquiJoinCondArr[j]->mLeftRefNodeList->mHead->mNode->mNodeType ==
                                         QLNC_NODE_TYPE_BASE_TABLE );
                            sOuterNodeRowCount *=
                                ((qlncBaseTableNode*)(aEquiJoinCondArr[j]->mLeftRefNodeList->mHead->mNode))->mTableStat->mRowCount;
                            aEquiJoinCondArr[j] = NULL;
                        }
                    }
                }

                STL_DASSERT( sOuterNodeRowCount > (stlFloat64)0.0 );
                *aResultRate =  sOuterNodeRowCount / aCandBaseTable->mTableStat->mRowCount;

                break;
            }
        }
        else
        {
            /* nothing to do */
        }
    }

    if ( sIsMatchedUnique == STL_TRUE )
    {
        /* 이미 계산됨 */
    }
    else
    {
        /* Unique Index에 존재하는 Column만으로 구성되지 않은 경우 */
        sEstCard = (stlFloat64)1.0;
        sRefColumnItem = aRefColumnList->mHead;
        while( sRefColumnItem != NULL )
        {
            /**
             * 통계정보로부터 Column의 Cardinality를 얻는다.
             */
            
            sEstCard *= aCandBaseTable->mTableStat->mColumnStat[sRefColumnItem->mIdx].mColumnCard;

            sRefColumnItem = sRefColumnItem->mNext;
        }

        *aResultRate = (stlFloat64)0.0;
    }

    /* Output 설정 */
    *aIsMatchedUnique = sIsMatchedUnique;


    return sEstCard;
}


/**
 * @brief SubQuery를 가지고 있는지 판단한다.
 * @param[in]   aNode   Candidate Node
 */
stlBool qlncHasSubQuery( qlncNodeCommon * aNode )
{
    stlBool       sHasSubQuery;
    stlInt32      i;

#define _QLNC_CHECK_SUBQUERY_FILTER( _aFilter, _aHasSubQuery )                      \
    {                                                                               \
        if( (_aFilter) != NULL )                                                    \
        {                                                                           \
            (_aHasSubQuery) = STL_FALSE;                                            \
            for( i = 0; i < (_aFilter)->mOrCount; i++ )                             \
            {                                                                       \
                if( ((_aFilter)->mOrFilters[i]->mSubQueryExprList != NULL) &&       \
                    ((_aFilter)->mOrFilters[i]->mSubQueryExprList->mCount > 0) )    \
                {                                                                   \
                    (_aHasSubQuery) = STL_TRUE;                                     \
                    break;                                                          \
                }                                                                   \
            }                                                                       \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            (_aHasSubQuery) = STL_FALSE;                                            \
        }                                                                           \
    }

    /* @todo 현재 Base Table인 경우에만 들어오도록 상위에서 사용하므로
     * Base Table에 대한 코드만 남겨두고 향후 필요할때 확장하도록 한다. */
    if( aNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
    {
        _QLNC_CHECK_SUBQUERY_FILTER( ((qlncBaseTableNode*)aNode)->mFilter, sHasSubQuery );
    }
    else
    {
        STL_DASSERT( 0 );
        sHasSubQuery = STL_FALSE;
    }

#if 0
    switch( aNode->mNodeType )
    {
        case QLNC_NODE_TYPE_SET_OP:
            sHasSubQuery = STL_FALSE;
            break;
        case QLNC_NODE_TYPE_QUERY_SPEC:
            if( (((qlncQuerySpec*)aNode)->mNonFilterSubQueryExprList != NULL) &&
                (((qlncQuerySpec*)aNode)->mNonFilterSubQueryExprList->mCount > 0) )
            {
                sHasSubQuery = STL_TRUE;
            }
            else
            {
                sHasSubQuery = STL_FALSE;
            }
            break;
        case QLNC_NODE_TYPE_BASE_TABLE:
            _QLNC_CHECK_SUBQUERY_FILTER( ((qlncBaseTableNode*)aNode)->mFilter, sHasSubQuery );
            break;
        case QLNC_NODE_TYPE_SUB_TABLE:
            _QLNC_CHECK_SUBQUERY_FILTER( ((qlncSubTableNode*)aNode)->mFilter, sHasSubQuery );
            break;
        case QLNC_NODE_TYPE_JOIN_TABLE:
            _QLNC_CHECK_SUBQUERY_FILTER( ((qlncJoinTableNode*)aNode)->mFilter, sHasSubQuery );
            break;
        case QLNC_NODE_TYPE_FLAT_INSTANT:
            STL_DASSERT( 0 );
            sHasSubQuery = STL_FALSE;
            break;
        case QLNC_NODE_TYPE_SORT_INSTANT:
            if( (((qlncSortInstantNode*)aNode)->mNonFilterSubQueryExprList != NULL) &&
                (((qlncSortInstantNode*)aNode)->mNonFilterSubQueryExprList->mCount > 0) )
            {
                sHasSubQuery = STL_TRUE;
            }
            else
            {
                _QLNC_CHECK_SUBQUERY_FILTER( ((qlncSortInstantNode*)aNode)->mFilter, sHasSubQuery );
            }
            break;
        case QLNC_NODE_TYPE_HASH_INSTANT:
            if( (((qlncHashInstantNode*)aNode)->mNonFilterSubQueryExprList != NULL) &&
                (((qlncHashInstantNode*)aNode)->mNonFilterSubQueryExprList->mCount > 0) )
            {
                sHasSubQuery = STL_TRUE;
            }
            else
            {
                _QLNC_CHECK_SUBQUERY_FILTER( ((qlncHashInstantNode*)aNode)->mFilter, sHasSubQuery );
            }
            break;
        case QLNC_NODE_TYPE_INNER_JOIN_TABLE:
            _QLNC_CHECK_SUBQUERY_FILTER( ((qlncInnerJoinTableNode*)aNode)->mJoinFilter, sHasSubQuery );
            break;
        case QLNC_NODE_TYPE_OUTER_JOIN_TABLE:
            _QLNC_CHECK_SUBQUERY_FILTER( ((qlncOuterJoinTableNode*)aNode)->mJoinFilter, sHasSubQuery );
            break;
        default:
            STL_DASSERT( 0 );
            sHasSubQuery = STL_FALSE;
            break;
    }
#endif

    return sHasSubQuery;
}


/**
 * @brief Base Table Node에서 Sort Key로만 구성된 Index가 존재하는지 찾는다.
 * @param[in]   aBaseTableNode          Base Table Node
 * @param[in]   aSortKeyCount           Sort Key Count
 * @param[in]   aSortKeyArr             Sort Key Array
 * @param[in]   aNeedFullMatch          Index의 모든 키가 Sort Key와 1:1 매칭이 되어야 하는지 여부
 * @param[in]   aNeedOnlyUnique         Unique Index만 필요한지 여부
 * @param[out]  aUsableIndexScanInfo    사용 가능 Index Statistic 정보
 * @param[out]  aIndexOrdering          사용 가능 Index의 Ordering 정보
 */
void qlncFindIndexForSortKey( qlncBaseTableNode         * aBaseTableNode,
                              stlInt32                    aSortKeyCount,
                              qlncExprCommon           ** aSortKeyArr,
                              stlBool                     aNeedFullMatch,
                              stlBool                     aNeedOnlyUnique,
                              qlncIndexScanInfo        ** aUsableIndexScanInfo,
                              ellIndexColumnOrdering    * aIndexOrdering )
{
    stlInt32                  i, j;
    stlInt32                  sIndexScanCount;
    qlncIndexScanInfo       * sIndexScanInfoArr = NULL;
    qlncIndexStat           * sIndexStat        = NULL;

    qlncIndexScanInfo       * sFullMatchIndexScanInfo   = NULL;
    qlncIndexScanInfo       * sPartMatchIndexScanInfo   = NULL;
    ellIndexColumnOrdering    sFullMatchIndexOrdering   = ELL_INDEX_COLUMN_ASCENDING;
    ellIndexColumnOrdering    sPartMatchIndexOrdering   = ELL_INDEX_COLUMN_ASCENDING;
    ellIndexColumnOrdering    sIndexOrdering            = ELL_INDEX_COLUMN_ASCENDING;


#define _QLNC_IS_UNIQUE_INDEX( _aIndexStat )                            \
    ( ( (_aIndexStat)->mIndexType == QLNC_INDEX_TYPE_PRIMARY_KEY ) ||   \
      ( (_aIndexStat)->mIndexType == QLNC_INDEX_TYPE_UNIQUE_KEY ) ||    \
      ( (_aIndexStat)->mIndexType == QLNC_INDEX_TYPE_UNIQUE_INDEX ) )

#define _QLNC_SET_INDEX_SCAN_INFO( _aIndexScanInfo, _aIndexOrdering )                               \
    {                                                                                               \
        if( (_aIndexScanInfo) == NULL )                                                             \
        {                                                                                           \
            (_aIndexScanInfo) = &(sIndexScanInfoArr[i]);                                            \
            (_aIndexOrdering) = sIndexOrdering;                                                     \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            if( _QLNC_IS_UNIQUE_INDEX(                                                              \
                    &(aBaseTableNode->mIndexStatArr[(_aIndexScanInfo)->mOffset]) ) )                \
            {                                                                                       \
                /* Full Match Index Scan Info가 Unique Index인 경우 */                              \
                if( _QLNC_IS_UNIQUE_INDEX( sIndexStat ) )                                           \
                {                                                                                   \
                    if( sIndexStat->mLeafPageCount <                                                \
                        aBaseTableNode->mIndexStatArr[(_aIndexScanInfo)->mOffset].mLeafPageCount )  \
                    {                                                                               \
                        (_aIndexScanInfo) = &(sIndexScanInfoArr[i]);                                \
                        (_aIndexOrdering) = sIndexOrdering;                                         \
                    }                                                                               \
                }                                                                                   \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                /* Full Match Index Scan Info가 Unique Index가 아닌 경우 */                         \
                if( _QLNC_IS_UNIQUE_INDEX( sIndexStat ) )                                           \
                {                                                                                   \
                    (_aIndexScanInfo) = &(sIndexScanInfoArr[i]);                                    \
                    (_aIndexOrdering) = sIndexOrdering;                                             \
                }                                                                                   \
                else                                                                                \
                {                                                                                   \
                    if( sIndexStat->mLeafPageCount <                                                \
                        aBaseTableNode->mIndexStatArr[(_aIndexScanInfo)->mOffset].mLeafPageCount )  \
                    {                                                                               \
                        (_aIndexScanInfo) = &(sIndexScanInfoArr[i]);                                \
                        (_aIndexOrdering) = sIndexOrdering;                                         \
                    }                                                                               \
                }                                                                                   \
            }                                                                                       \
        }                                                                                           \
    }

    /* Output 변수 초기화 */
    *aUsableIndexScanInfo = NULL;

    /* Sort Key가 모두 존재하는 Index Scan이 존재하는지 체크 */
    sIndexScanCount = aBaseTableNode->mAccPathHint->mPossibleIndexScanCount; 
    sIndexScanInfoArr = aBaseTableNode->mAccPathHint->mPossibleIndexScanInfoArr; 
    for( i = 0; i < sIndexScanCount; i++ )
    {
        /* Scan Direction이 Ascending이어야 한다. */
        if( sIndexScanInfoArr[i].mIsAsc == STL_FALSE )
        {
            continue;
        }

        sIndexStat = &(aBaseTableNode->mIndexStatArr[sIndexScanInfoArr[i].mOffset]);

        /* Unique Index 여부 판단 */
        if( aNeedOnlyUnique == STL_TRUE )
        {
            /* Unique가 아닌 경우 Skip */
            if( !_QLNC_IS_UNIQUE_INDEX( sIndexStat ) )
            {
                continue;
            }
        }

        /* Sort Key와 Index Key의 개수에 대한 판단 */
        if( aNeedFullMatch == STL_TRUE )
        {
            if( aSortKeyCount == sIndexStat->mIndexKeyCount )
            {
                for( j = 0; j < aSortKeyCount; j++ )
                {
                    /* Sort Key의 순서와 Index Key의 순서가 모두 일치하여야 한다. */
                    if( ( ((qlncExprColumn*)aSortKeyArr[j])->mColumnID !=
                          ellGetColumnID( &(sIndexStat->mIndexKeyDesc[j].mKeyColumnHandle) ) ) ||
                        ( (j > 0) && (sIndexOrdering != sIndexStat->mIndexKeyDesc[j].mKeyOrdering) ) )
                    {
                        break;
                    }

                    sIndexOrdering = sIndexStat->mIndexKeyDesc[j].mKeyOrdering;
                }

                /* 모든 조건이 맞는 Index를 찾은 경우 */
                if( j == aSortKeyCount )
                {
                    _QLNC_SET_INDEX_SCAN_INFO( sFullMatchIndexScanInfo,
                                               sFullMatchIndexOrdering );
                }
            }
        }
        else
        {
            if( aSortKeyCount <= sIndexStat->mIndexKeyCount )
            {
                for( j = 0; j < aSortKeyCount; j++ )
                {
                    /* Sort Key의 순서와 Index Key의 순서가 모두 일치하여야 한다. */
                    if( ( ((qlncExprColumn*)aSortKeyArr[j])->mColumnID !=
                          ellGetColumnID( &(sIndexStat->mIndexKeyDesc[j].mKeyColumnHandle) ) ) ||
                        ( (j > 0) && (sIndexOrdering != sIndexStat->mIndexKeyDesc[j].mKeyOrdering) ) )
                    {
                        break;
                    }

                    sIndexOrdering = sIndexStat->mIndexKeyDesc[j].mKeyOrdering;
                }

                /* 모든 조건이 맞는 Index를 찾은 경우 */
                if( j == aSortKeyCount )
                {
                    if( aSortKeyCount == sIndexStat->mIndexKeyCount )
                    {
                        _QLNC_SET_INDEX_SCAN_INFO( sFullMatchIndexScanInfo,
                                                   sFullMatchIndexOrdering );
                    }
                    else
                    {
                        _QLNC_SET_INDEX_SCAN_INFO( sPartMatchIndexScanInfo,
                                                   sPartMatchIndexOrdering );
                    }
                }
            }
        }
    }

    /* Output 설정 */
    if( aNeedFullMatch == STL_TRUE )
    {
        *aUsableIndexScanInfo = sFullMatchIndexScanInfo;
        *aIndexOrdering = sFullMatchIndexOrdering;
    }
    else
    {
        if( sFullMatchIndexScanInfo != NULL )
        {
            if( sPartMatchIndexScanInfo != NULL )
            {
                /* Full Match / Part Match 중 Unique Index인쪽
                 * 그리고 Leaf Page Count가 적은쪽을 우선으로 한다. */
                if( _QLNC_IS_UNIQUE_INDEX(
                        &(aBaseTableNode->mIndexStatArr[sFullMatchIndexScanInfo->mOffset]) ) )
                {
                    if( _QLNC_IS_UNIQUE_INDEX(
                            &(aBaseTableNode->mIndexStatArr[sPartMatchIndexScanInfo->mOffset]) ) )
                    {
                        if( aBaseTableNode->mIndexStatArr[sPartMatchIndexScanInfo->mOffset].mLeafPageCount <
                            aBaseTableNode->mIndexStatArr[sFullMatchIndexScanInfo->mOffset].mLeafPageCount )
                        {
                            *aUsableIndexScanInfo = sPartMatchIndexScanInfo;
                            *aIndexOrdering = sPartMatchIndexOrdering;
                        }
                        else
                        {
                            *aUsableIndexScanInfo = sFullMatchIndexScanInfo;
                            *aIndexOrdering = sFullMatchIndexOrdering;
                        }
                    }
                    else
                    {
                        *aUsableIndexScanInfo = sFullMatchIndexScanInfo;
                        *aIndexOrdering = sFullMatchIndexOrdering;
                    }
                }
                else
                {
                    if( _QLNC_IS_UNIQUE_INDEX(
                            &(aBaseTableNode->mIndexStatArr[sPartMatchIndexScanInfo->mOffset]) ) )
                    {
                        *aUsableIndexScanInfo = sPartMatchIndexScanInfo;
                        *aIndexOrdering = sPartMatchIndexOrdering;
                    }
                    else
                    {
                        if( aBaseTableNode->mIndexStatArr[sPartMatchIndexScanInfo->mOffset].mLeafPageCount <
                            aBaseTableNode->mIndexStatArr[sFullMatchIndexScanInfo->mOffset].mLeafPageCount )
                        {
                            /* @todo 여기에 들어오는 테스트 케이스를 만들기 어려움
                             * (Index의 page들이 split과 merge등으로 인해 발생할 수 있다.) */
                            *aUsableIndexScanInfo = sPartMatchIndexScanInfo;
                            *aIndexOrdering = sPartMatchIndexOrdering;
                        }
                        else
                        {
                            *aUsableIndexScanInfo = sFullMatchIndexScanInfo;
                            *aIndexOrdering = sFullMatchIndexOrdering;
                        }
                    }
                }
            }
            else
            {
                *aUsableIndexScanInfo = sFullMatchIndexScanInfo;
                *aIndexOrdering = sFullMatchIndexOrdering;
            }
        }
        else
        {
            *aUsableIndexScanInfo = sPartMatchIndexScanInfo;
            *aIndexOrdering = sPartMatchIndexOrdering;
        }
    }
}


/**
 * @brief Merge Join을 위해 Join Condition을 Equi-Join Condition과 Non Equi-Join Condition으로 분리한다.
 * @param[in]   aParamInfo              Parameter Info
 * @param[in]   aJoinCondition          Join Condition
 * @param[out]  aEquiJoinCondition      Equi Join Condition
 * @param[out]  aNonEquiJoinCondition   Non Equi Join Condition
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncSeperateJoinConditionForMergeJoin( qlncParamInfo  * aParamInfo,
                                                 qlncAndFilter  * aJoinCondition,
                                                 qlncAndFilter ** aEquiJoinCondition,
                                                 qlncAndFilter ** aNonEquiJoinCondition,
                                                 qllEnv         * aEnv )
{
    stlInt32              i;
    qlncBooleanFilter   * sBooleanFilter        = NULL;
    qlncAndFilter       * sEquiJoinCondition    = NULL;
    qlncAndFilter       * sNonEquiJoinCondition = NULL;

    const dtlCompareType    * sCompType;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinCondition != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Join Condition 분리
     * ---------------------------
     *  Join Condition에는 Join 조건이 아닌 경우가 존재한다.
     *  (한쪽 테이블의 컬럼만 존재하거나 constant인 경우)
     *  이는 NonEquiJoinCondition으로 분리하도록 한다.
     */

    sEquiJoinCondition = NULL;
    sNonEquiJoinCondition = NULL;
    for( i = 0; i < aJoinCondition->mOrCount; i++ )
    {
        if( (aJoinCondition->mOrFilters[i]->mFilterCount > 1) ||
            (aJoinCondition->mOrFilters[i]->mFilters[0]->mType != QLNC_EXPR_TYPE_BOOLEAN_FILTER) )
        {
            /* Non Equi-Join Condition인 경우 */
            if( sNonEquiJoinCondition == NULL )
            {
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncAndFilter ),
                                            (void**) &sNonEquiJoinCondition,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                QLNC_INIT_AND_FILTER( sNonEquiJoinCondition );
            }

            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        sNonEquiJoinCondition,
                                                        1,
                                                        &(aJoinCondition->mOrFilters[i]),
                                                        aEnv )
                     == STL_SUCCESS );

            continue;
        }

        sBooleanFilter = (qlncBooleanFilter*)(aJoinCondition->mOrFilters[i]->mFilters[0]);
        if( sBooleanFilter->mFuncID == KNL_BUILTIN_FUNC_IS_EQUAL )
        {
            /* Equi-Join Condition인 경우 */
            if( (sBooleanFilter->mLeftRefNodeList != NULL) &&
                (sBooleanFilter->mRightRefNodeList != NULL) &&
                ( ( (sBooleanFilter->mLeftRefNodeList->mCount == 1) &&
                    (sBooleanFilter->mRightRefNodeList->mCount > 0) ) ||
                  ( (sBooleanFilter->mLeftRefNodeList->mCount > 0) &&
                    (sBooleanFilter->mRightRefNodeList->mCount == 1) ) ) &&
                (dtlCheckKeyCompareType( ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0]->mDataType ) == STL_TRUE) &&
                (dtlCheckKeyCompareType( ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1]->mDataType ) == STL_TRUE) )
            {
                STL_TRY( qlnoGetCompareType( aParamInfo->mSQLStmt->mRetrySQL,
                                             ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0]->mDataType,
                                             ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1]->mDataType,
                                             STL_FALSE,
                                             STL_FALSE,
                                             sBooleanFilter->mExpr->mPosition,
                                             &sCompType,
                                             aEnv )
                         == STL_SUCCESS );

                if( (sCompType->mLeftType != ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0]->mDataType) ||
                    (sCompType->mRightType != ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1]->mDataType) )
                {
                    /* Join 조건이 아닌 경우 */
                    if( sNonEquiJoinCondition == NULL )
                    {
                        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                    STL_SIZEOF( qlncAndFilter ),
                                                    (void**) &sNonEquiJoinCondition,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        QLNC_INIT_AND_FILTER( sNonEquiJoinCondition );
                    }

                    STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                                sNonEquiJoinCondition,
                                                                1,
                                                                &(aJoinCondition->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /* Join 조건인 경우 */
                    if( sEquiJoinCondition == NULL )
                    {
                        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                    STL_SIZEOF( qlncAndFilter ),
                                                    (void**) &sEquiJoinCondition,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        QLNC_INIT_AND_FILTER( sEquiJoinCondition );
                    }

                    STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                                sEquiJoinCondition,
                                                                1,
                                                                &(aJoinCondition->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                /* Join 조건이 아닌 경우 */
                if( sNonEquiJoinCondition == NULL )
                {
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncAndFilter ),
                                                (void**) &sNonEquiJoinCondition,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    QLNC_INIT_AND_FILTER( sNonEquiJoinCondition );
                }

                STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                            sNonEquiJoinCondition,
                                                            1,
                                                            &(aJoinCondition->mOrFilters[i]),
                                                            aEnv )
                         == STL_SUCCESS );
            }
        }
        else
        {
            /* Non Equi-Join Condition인 경우 */
            if( sNonEquiJoinCondition == NULL )
            {
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncAndFilter ),
                                            (void**) &sNonEquiJoinCondition,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                QLNC_INIT_AND_FILTER( sNonEquiJoinCondition );
            }

            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        sNonEquiJoinCondition,
                                                        1,
                                                        &(aJoinCondition->mOrFilters[i]),
                                                        aEnv )
                     == STL_SUCCESS );
        }
    }


    /**
     * Output 설정
     */

    *aEquiJoinCondition = sEquiJoinCondition;
    *aNonEquiJoinCondition = sNonEquiJoinCondition;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Join을 위해 Join Condition을 Equi-Join Condition과 Non Equi-Join Condition으로 분리한다.
 * @param[in]   aParamInfo              Parameter Info
 * @param[in]   aHashNode               Hash Candidate Node
 * @param[in]   aJoinCondition          Join Condition
 * @param[out]  aEquiJoinCondition      Equi Join Condition
 * @param[out]  aNonEquiJoinCondition   Non Equi Join Condition
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncSeperateJoinConditionForHashJoin( qlncParamInfo   * aParamInfo,
                                                qlncNodeCommon  * aHashNode,
                                                qlncAndFilter   * aJoinCondition,
                                                qlncAndFilter  ** aEquiJoinCondition,
                                                qlncAndFilter  ** aNonEquiJoinCondition,
                                                qllEnv          * aEnv )
{
    stlBool               sIsPossible;
    stlInt32              i;
    qlncAndFilter       * sEquiJoinCondition    = NULL;
    qlncAndFilter       * sNonEquiJoinCondition = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinCondition != NULL, QLL_ERROR_STACK(aEnv) );


    /* Join Condition 분리 */
    sEquiJoinCondition = NULL;
    sNonEquiJoinCondition = NULL;
    for( i = 0; i < aJoinCondition->mOrCount; i++ )
    {
        if( (aJoinCondition->mOrFilters[i]->mFilterCount > 1) ||
            (aJoinCondition->mOrFilters[i]->mFilters[0]->mType != QLNC_EXPR_TYPE_BOOLEAN_FILTER) )
        {
            /* Non Equi-Join Condition인 경우 */
            if( sNonEquiJoinCondition == NULL )
            {
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncAndFilter ),
                                            (void**) &sNonEquiJoinCondition,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                QLNC_INIT_AND_FILTER( sNonEquiJoinCondition );
            }

            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        sNonEquiJoinCondition,
                                                        1,
                                                        &(aJoinCondition->mOrFilters[i]),
                                                        aEnv )
                     == STL_SUCCESS );

            continue;
        }

        STL_TRY( qlncIsPossibleHashJoinCondition(
                     aParamInfo->mSQLStmt->mRetrySQL,
                     (qlncBooleanFilter*)(aJoinCondition->mOrFilters[i]->mFilters[0]),
                     aHashNode,
                     sEquiJoinCondition,
                     &sIsPossible,
                     aEnv )
                 == STL_SUCCESS );

        if( sIsPossible == STL_TRUE )
        {
            /* Equi-Join Condition인 경우 */
            if( sEquiJoinCondition == NULL )
            {
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncAndFilter ),
                                            (void**) &sEquiJoinCondition,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                QLNC_INIT_AND_FILTER( sEquiJoinCondition );
            }

            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        sEquiJoinCondition,
                                                        1,
                                                        &(aJoinCondition->mOrFilters[i]),
                                                        aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* Non Equi-Join Condition인 경우 */
            if( sNonEquiJoinCondition == NULL )
            {
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncAndFilter ),
                                            (void**) &sNonEquiJoinCondition,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                QLNC_INIT_AND_FILTER( sNonEquiJoinCondition );
            }

            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        sNonEquiJoinCondition,
                                                        1,
                                                        &(aJoinCondition->mOrFilters[i]),
                                                        aEnv )
                     == STL_SUCCESS );
        }
    }


    /**
     * Output 설정
     */

    *aEquiJoinCondition = sEquiJoinCondition;
    *aNonEquiJoinCondition = sNonEquiJoinCondition;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expression이 Range 적용 가능한 Key Col을 가지고 있는지 판단
 * @param[in]   aExpr       Expression
 * @param[in]   aTableNode  Physical Filter 평가를 위한 Table Node
 */
stlBool qlncHasKeyColForRange( qlncExprCommon  * aExpr,
                               qlncNodeCommon  * aTableNode )
{
    qlncExprFunction  * sFunction = NULL;
    stlInt32            sColCnt = 0;
    
    if( aExpr->mType == QLNC_EXPR_TYPE_FUNCTION )
    {
        sFunction = (qlncExprFunction *) aExpr;

        if( ( sFunction->mArgCount == 0 ) || ( sFunction->mArgCount > 2 ) )
        {
            return STL_FALSE;
        }

        if( sFunction->mArgCount == 1 )
        {
            if( ( sFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_COLUMN ) ||
                ( sFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_INNER_COLUMN ) )
            {
                return ( qlncIsExistColumnInExpr( sFunction->mArgs[0],
                                                  aTableNode )
                         == STL_TRUE );
            }
            else
            {
                return STL_FALSE;
            }
        }
        else
        {
            if( ( sFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_COLUMN ) ||
                ( sFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_INNER_COLUMN ) )
            {
                sColCnt += ( qlncIsExistColumnInExpr( sFunction->mArgs[0],
                                                      aTableNode )
                             == STL_TRUE );
            }
            else
            {
                /* Do Nothing */
            }

            if( sColCnt == 1 )
            {
                return ( qlncIsExistColumnInExpr( sFunction->mArgs[1],
                                                  aTableNode )
                         == STL_FALSE );
            }

            if( ( sFunction->mArgs[1]->mType == QLNC_EXPR_TYPE_COLUMN ) ||
                ( sFunction->mArgs[1]->mType == QLNC_EXPR_TYPE_INNER_COLUMN ) )
            {
                sColCnt += ( qlncIsExistColumnInExpr( sFunction->mArgs[1],
                                                      aTableNode )
                             == STL_TRUE );
            }
            else
            {
                /* Do Nothing */
            }

            if( sColCnt == 1 )
            {
                return ( qlncIsExistColumnInExpr( sFunction->mArgs[0],
                                                  aTableNode )
                         == STL_FALSE );
            }
            else
            {
                return STL_FALSE;
            }
        }
    }
    else
    {
        return STL_FALSE;
    }
}


/**
 * @brief Expression 내에 주어진 Node에 대한 Column이 존재하는지 여부 판단
 * @param[in]   aExpr       Expression
 * @param[in]   aTableNode  Physical Filter 평가를 위한 Table Node
 */
stlBool qlncIsExistColumnInExpr( qlncExprCommon  * aExpr,
                                 qlncNodeCommon  * aTableNode )
{
    stlInt32  sLoop;
    
    switch( aExpr->mType )
    {
        case QLNC_EXPR_TYPE_COLUMN :
            {
                switch( aTableNode->mNodeType )
                {
                    case QLNC_NODE_TYPE_BASE_TABLE :
                        return ( ((qlncExprColumn*)aExpr)->mNode->mNodeID == aTableNode->mNodeID );
                    case QLNC_NODE_TYPE_SORT_INSTANT :
                        return ( ((qlncExprColumn*)aExpr)->mNode->mNodeID ==
                                 ((qlncSortInstantNode *)aTableNode)->mChildNode->mNodeID );
                    case QLNC_NODE_TYPE_HASH_INSTANT :
                        return ( ((qlncExprColumn*)aExpr)->mNode->mNodeID ==
                                 ((qlncHashInstantNode *)aTableNode)->mChildNode->mNodeID );
                    default:
                        /* @todo 현재 여기에 들어오는 경우는 없는 것으로 판단됨 */

                        return STL_FALSE;
                }
            }
        case QLNC_EXPR_TYPE_INNER_COLUMN :
            {
                return ( ((qlncExprInnerColumn*)aExpr)->mNode->mNodeID == aTableNode->mNodeID );
            }
        case QLNC_EXPR_TYPE_ROWID_COLUMN :
            {
                switch( aTableNode->mNodeType )
                {
                    case QLNC_NODE_TYPE_BASE_TABLE :
                        return ( ((qlncExprRowidColumn*)aExpr)->mNode->mNodeID == aTableNode->mNodeID );
                    case QLNC_NODE_TYPE_SORT_INSTANT :
                        return ( ((qlncExprRowidColumn*)aExpr)->mNode->mNodeID ==
                                 ((qlncSortInstantNode *)aTableNode)->mChildNode->mNodeID );
                    case QLNC_NODE_TYPE_HASH_INSTANT :
                        return ( ((qlncExprRowidColumn*)aExpr)->mNode->mNodeID ==
                                 ((qlncHashInstantNode *)aTableNode)->mChildNode->mNodeID );
                    default:
                        /* @todo 현재 여기에 들어오는 경우는 없는 것으로 판단됨 */

                        return STL_FALSE;
                }
            }
        case QLNC_EXPR_TYPE_FUNCTION :
            {
                qlncExprFunction  * sFunction = (qlncExprFunction*) aExpr;
                for( sLoop = 0 ; sLoop < sFunction->mArgCount ; sLoop++ )
                {
                    if( qlncIsExistColumnInExpr( sFunction->mArgs[ sLoop ],
                                                 aTableNode )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_CAST :
            {
                qlncExprTypeCast  * sCast = (qlncExprTypeCast*) aExpr;
                for( sLoop = 0 ; sLoop < sCast->mArgCount ; sLoop++ )
                {
                    if( qlncIsExistColumnInExpr( sCast->mArgs[ sLoop ],
                                                 aTableNode )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_CASE_EXPR :
            {
                qlncExprCaseExpr  * sCase = (qlncExprCaseExpr*) aExpr;
                for( sLoop = 0 ; sLoop < sCase->mCount ; sLoop++ )
                {
                    if( qlncIsExistColumnInExpr( sCase->mWhenArr[ sLoop ],
                                                 aTableNode )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }

                    if( qlncIsExistColumnInExpr( sCase->mThenArr[ sLoop ],
                                                 aTableNode )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                return qlncIsExistColumnInExpr( sCase->mDefResult,
                                                aTableNode );
            }
        case QLNC_EXPR_TYPE_LIST_FUNCTION :
            {
                qlncExprListFunc  * sListFunc = (qlncExprListFunc*) aExpr;
                for( sLoop = 0 ; sLoop < sListFunc->mArgCount ; sLoop++ )
                {
                    if( qlncIsExistColumnInExpr( sListFunc->mArgs[ sLoop ],
                                                 aTableNode )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_LIST_COLUMN :
            {
                qlncExprListCol  * sListCol = (qlncExprListCol*) aExpr;
                for( sLoop = 0 ; sLoop < sListCol->mArgCount ; sLoop++ )
                {
                    if( qlncIsExistColumnInExpr( sListCol->mArgs[ sLoop ],
                                                 aTableNode )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                return STL_FALSE;
            }
        case QLNC_EXPR_TYPE_ROW_EXPR :
            {
                qlncExprRowExpr  * sRowExpr = (qlncExprRowExpr*) aExpr;
                for( sLoop = 0 ; sLoop < sRowExpr->mArgCount ; sLoop++ )
                {
                    if( qlncIsExistColumnInExpr( sRowExpr->mArgs[ sLoop ],
                                                 aTableNode )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                return STL_FALSE;
            }
        default :
            {
                return STL_FALSE;
            }
    }
}


/**
 * @brief And Filter에 대하여 Row Cardinality 및 Cost에 의하여 정렬한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aAndFilter  And Filter
 * @param[in]   aEnv        Environment
 */
stlStatus qlncAdjustAndFilterByCost( qlncParamInfo  * aParamInfo,
                                     qlncAndFilter  * aAndFilter,
                                     qllEnv         * aEnv )
{
    typedef struct _qlncPriorInfo
    {
        stlInt8           mType;        /* 0: No SubQuery 1: SubQuery */
        stlFloat64        mCardinality;
        stlFloat64        mSubQueryCost;
        stlFloat64        mSubQueryRowCount;
        qlncOrFilter    * mOrFilter;
    } _qlncPriorInfo;

    stlBool           sIsLess;
    stlBool           sHasNonStmtSubQuery;
    stlInt32          i;
    stlInt32          j;
    stlFloat64        sTmpRowCount;
    _qlncPriorInfo  * sPriorInfo    = NULL;
    _qlncPriorInfo    sTmpPriorInfo;
    qlncRefExprItem * sRefExprItem  = NULL;


#define _QLNC_IS_LESS_THAN_PRIOR_INFO( _aPriorInfo1, _aPriorInfo2, _aIsLess )               \
    {                                                                                       \
        if( (_aPriorInfo1).mType < (_aPriorInfo2).mType )                                   \
        {                                                                                   \
            (_aIsLess) = STL_TRUE;                                                          \
        }                                                                                   \
        else if( (_aPriorInfo1).mType == (_aPriorInfo2).mType )                             \
        {                                                                                   \
            if( (_aPriorInfo1).mCardinality < (_aPriorInfo2).mCardinality )                 \
            {                                                                               \
                (_aIsLess) = STL_TRUE;                                                      \
            }                                                                               \
            else if( (_aPriorInfo1).mCardinality == (_aPriorInfo2).mCardinality )           \
            {                                                                               \
                if( ( (_aPriorInfo1).mSubQueryRowCount * (_aPriorInfo2).mSubQueryCost )     \
                    < ( (_aPriorInfo2).mSubQueryRowCount * (_aPriorInfo1).mSubQueryCost ) ) \
                {                                                                           \
                    (_aIsLess) = STL_TRUE;                                                  \
                }                                                                           \
                else                                                                        \
                {                                                                           \
                    (_aIsLess) = STL_FALSE;                                                 \
                }                                                                           \
            }                                                                               \
            else                                                                            \
            {                                                                               \
                (_aIsLess) = STL_FALSE;                                                     \
            }                                                                               \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            (_aIsLess) = STL_FALSE;                                                         \
        }                                                                                   \
    }

#define _QLNC_SWAP_PRIOR_INFO( _aPriorInfo1, _aPriorInfo2 )                 \
    {                                                                       \
        sTmpPriorInfo.mCardinality = (_aPriorInfo1).mCardinality;           \
        sTmpPriorInfo.mSubQueryCost = (_aPriorInfo1).mSubQueryCost;         \
        sTmpPriorInfo.mSubQueryRowCount = (_aPriorInfo1).mSubQueryRowCount; \
        sTmpPriorInfo.mOrFilter = (_aPriorInfo1).mOrFilter;                 \
                                                                            \
        (_aPriorInfo1).mCardinality = (_aPriorInfo2).mCardinality;          \
        (_aPriorInfo1).mSubQueryCost = (_aPriorInfo2).mSubQueryCost;        \
        (_aPriorInfo1).mSubQueryRowCount = (_aPriorInfo2).mSubQueryRowCount;\
        (_aPriorInfo1).mOrFilter = (_aPriorInfo2).mOrFilter;                \
                                                                            \
        (_aPriorInfo2).mCardinality = sTmpPriorInfo.mCardinality;           \
        (_aPriorInfo2).mSubQueryCost = sTmpPriorInfo.mSubQueryCost;         \
        (_aPriorInfo2).mSubQueryRowCount = sTmpPriorInfo.mSubQueryRowCount; \
        (_aPriorInfo2).mOrFilter = sTmpPriorInfo.mOrFilter;                 \
    }

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter->mOrCount > 0, QLL_ERROR_STACK(aEnv) );


    /**
     * 정렬 순서
     * 1. 연산자에 의해 순서 결정
     * 2. 연산자가 동일한 경우 Column에 의한 Cardinality로 순서 결정
     * 3. SubQuery가 존재하는 경우 제일 마지막으로 순서 결정
     *
     * SubQuery가 존재하는 경우에 대한 순서 결정은 다음을 따름
     * 1. 연산자에 의해 순서 결정
     * 2. 연산자가 동일한 경우 SubQuery의 Cost에 의해 순서 결정
     */

    /* And Filter의 Compact 수행 */
    STL_TRY( qlncCompactAndFilter( aAndFilter, aEnv ) == STL_SUCCESS );

    /* Priority Info 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( _qlncPriorInfo ) * aAndFilter->mOrCount,
                                (void**) &sPriorInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* And Filter의 각 Or Filter들에 대해 Priority Info 설정 */
    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        /* Type 설정 및 SubQuery Cost값 설정 */
        if( (aAndFilter->mOrFilters[i]->mSubQueryExprList != NULL) &&
            (aAndFilter->mOrFilters[i]->mSubQueryExprList->mCount > 0) )
        {
            /* SubQuery */
            sPriorInfo[i].mSubQueryCost = (stlFloat64)0.0;
            sPriorInfo[i].mSubQueryRowCount = (stlFloat64)0.0;
            sHasNonStmtSubQuery = STL_FALSE;
            sRefExprItem = aAndFilter->mOrFilters[i]->mSubQueryExprList->mHead;
            while( sRefExprItem != NULL )
            {
                STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_SUB_QUERY );
                STL_DASSERT( ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode->mIsAnalyzedCBO ==
                             STL_TRUE );
                STL_DASSERT( ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode->mBestCost != NULL );

                sPriorInfo[i].mSubQueryCost +=
                    ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode->mBestCost->mTotalCost;
                sPriorInfo[i].mSubQueryRowCount +=
                    ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode->mBestCost->mResultRowCount;

                if( (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL) ||
                    (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mIsRelSubQuery == STL_TRUE) )
                {
                    sHasNonStmtSubQuery = STL_TRUE;
                }

                sRefExprItem = sRefExprItem->mNext;
            }

            if( sHasNonStmtSubQuery == STL_TRUE )
            {
                /* 매번 평가되어야 하는 SubQuery가 존재하는 경우 */
                sPriorInfo[i].mType = 1;
            }
            else
            {
                /* Statement단위로 한번만 평가 가능한 SubQuery인 경우 Non SubQuery Type으로 함 */
                sPriorInfo[i].mType = 0;
            }
        }
        else
        {
            /* No SubQuery */
            sPriorInfo[i].mType = 0;
            sPriorInfo[i].mSubQueryCost = (stlFloat64)0.0;
            sPriorInfo[i].mSubQueryRowCount = (stlFloat64)0.0;
        }

        /* Cardinality값 설정 */
        sTmpRowCount = (stlFloat64)0.0;
        for( j = 0; j < aAndFilter->mOrFilters[i]->mFilterCount; j++ )
        {
            sTmpRowCount +=
                qlncGetExpectedResultCountByFilter(
                    aAndFilter->mOrFilters[i]->mFilters[j],
                    STL_FALSE,
                    (1 - sTmpRowCount) );
        }
        sPriorInfo[i].mCardinality = sTmpRowCount;

        /* Or Filter 설정 */
        sPriorInfo[i].mOrFilter = aAndFilter->mOrFilters[i];
    }

    /* Priority Info에 대하여 정렬 수행 */
    for( i = 1; i < aAndFilter->mOrCount; i++ )
    {
        for( j = i; j > 0; j-- )
        {
            _QLNC_IS_LESS_THAN_PRIOR_INFO(sPriorInfo[j], sPriorInfo[j-1], sIsLess );
            if( sIsLess == STL_TRUE )
            {
                _QLNC_SWAP_PRIOR_INFO( sPriorInfo[j], sPriorInfo[j-1] );
            }
            else
            {
                break;
            }
        }
    }

    /* 정렬된 순서로 And Filter 설정 */
    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        aAndFilter->mOrFilters[i] = sPriorInfo[i].mOrFilter;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 현재 노드에서  SubQuery Filter를 하위 노드로 Push Down 가능한 경우 이를 처리한다.
 * @param[in]   aParamInfo  Parameter Information
 * @param[in]   aNode       Inner Join Candidate Node
 * @param[in]   aParentNode Parent Candidate Node
 * @param[in]   aEnv        Environment
 */
stlStatus qlncPushDownSubQueryFilter( qlncParamInfo     * aParamInfo,
                                      qlncNodeCommon    * aNode,
                                      qlncNodeCommon    * aParentNode,
                                      qllEnv            * aEnv )
{
    stlInt32              i, j;
    stlFloat64            sBestRowCount;
    qlncAndFilter       * sFilter       = NULL;
    qlncNodeCommon      * sPushDownNode = NULL;
    qlncNodeCommon      * sParentNode   = NULL;
    qlncNodeCommon      * sCopiedNode   = NULL;
    qlncPushSubqType      sPushSubqType;
    qlncCBOptParamInfo    sCBOptParamInfo;
    qlncInnerJoinNodeInfo sInnerJoinNodeInfoArr[2];
    qlncOrFilter        * sPushOrFilter = NULL;
    qlncRefExprItem     * sRefExprItem  = NULL;
    stlBool               sIsConstSubQuery;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) ||
                        (aNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE) ||
                        (aNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode->mBestCost != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode->mIsAnalyzedCBO == STL_TRUE, QLL_ERROR_STACK(aEnv) );


#define _QLNC_ADD_SUBQUERY_FILTER( _aAndFilter, _aSubQueryOrFilter )        \
    {                                                                       \
        if( (_aAndFilter) == NULL )                                         \
        {                                                                   \
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),         \
                                        STL_SIZEOF( qlncAndFilter ),        \
                                        (void**) &(_aAndFilter),            \
                                        KNL_ENV(aEnv) )                     \
                     == STL_SUCCESS );                                      \
            QLNC_INIT_AND_FILTER( (_aAndFilter) );                          \
        }                                                                   \
                                                                            \
        STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,             \
                                                    (_aAndFilter),          \
                                                    1,                      \
                                                    &(_aSubQueryOrFilter),  \
                                                    aEnv )                  \
                 == STL_SUCCESS );                                          \
    }

    /* Filter 찾기 */
    if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_NESTED_LOOPS )
    {
        sFilter = ((qlncNestedJoinCost*)(aNode->mBestCost))->mJoinFilter;
    }
    else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_MERGE )
    {
        sFilter = ((qlncMergeJoinCost*)(aNode->mBestCost))->mJoinFilter;
    }
    else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_HASH )
    {
        sFilter = ((qlncHashJoinCost*)(aNode->mBestCost))->mJoinFilter;
    }
    else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_FILTER )
    {
        if( ((qlncFilterCost*)(aNode->mBestCost))->mMaterializedNode != NULL )
        {
            /* @todo 여기에 들어오는 테스트 케이스를 만들기 어려움 */
            STL_THROW( RAMP_FINISH );
        }

        sFilter = ((qlncFilterCost*)(aNode->mBestCost))->mFilter;
    }
    else
    {
        STL_DASSERT( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_JOIN_COMBINE );
        STL_THROW( RAMP_FINISH );
    }

    STL_TRY_THROW( sFilter != NULL, RAMP_FINISH );
    STL_DASSERT( sFilter->mOrCount > 0 );


    /* SubQuery Filter 하나씩에 대하여 Push Down 수행 */
    for( i = 0; i < sFilter->mOrCount; i++ )
    {
        if( sFilter->mOrFilters[i]->mSubQueryExprList != NULL )
        {
            (void)qlncGetPushSubqType( sFilter->mOrFilters[i]->mSubQueryExprList,
                                       &sPushSubqType );

            STL_TRY_THROW( sPushSubqType != QLNC_PUSH_SUBQ_TYPE_NO,
                           RAMP_SKIP );

            /* Constant인 SubQuery인지 판단 */
            sRefExprItem = sFilter->mOrFilters[i]->mSubQueryExprList->mHead;
            while( sRefExprItem != NULL )
            {
                if( (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL) &&
                    (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList->mCount > 0) )
                {
                    break;
                }

                sRefExprItem = sRefExprItem->mNext;
            }

            if( sRefExprItem != NULL )
            {
                sIsConstSubQuery = STL_FALSE;
            }
            else
            {
                sIsConstSubQuery = STL_TRUE;
            }

            sBestRowCount = aNode->mBestCost->mResultRowCount;
            sPushDownNode = NULL;
            STL_TRY( qlncFindNodeForPushDownSubQueryFilter( aParamInfo,
                                                            aNode,
                                                            NULL,
                                                            sPushSubqType,
                                                            sIsConstSubQuery,
                                                            &sBestRowCount,
                                                            sFilter->mOrFilters[i],
                                                            &sPushDownNode,
                                                            &sParentNode,
                                                            &sPushOrFilter,
                                                            aEnv )
                     == STL_SUCCESS );

            /* Push Down을 수행할 노드를 찾지 못하면 Skip */
            STL_TRY_THROW( (sPushDownNode != NULL) &&
                           (sPushDownNode->mNodeID != aNode->mNodeID),
                           RAMP_SKIP );

            STL_DASSERT( sPushOrFilter != NULL );

            /* Push Down 노드가 존재하는 경우 해당 Node에 대하여 Cost를 새로 계산함 */
            if( sPushDownNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
            {
                STL_TRY( qlncCopyBaseNodeWithoutCBOInfo( aParamInfo,
                                                         sPushDownNode,
                                                         &sCopiedNode,
                                                         aEnv )
                         == STL_SUCCESS );

                /* Push Filter 설정 */
                if( ((qlncBaseTableNode*)sCopiedNode)->mFilter == NULL )
                {
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncAndFilter ),
                                                (void**) &(((qlncBaseTableNode*)sCopiedNode)->mFilter),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    QLNC_INIT_AND_FILTER( ((qlncBaseTableNode*)sCopiedNode)->mFilter );
                }

                STL_TRY( qlncAppendOrFilterToAndFilterTail(
                             aParamInfo,
                             ((qlncBaseTableNode*)sCopiedNode)->mFilter,
                             1,
                             &sPushOrFilter,
                             aEnv )
                         == STL_SUCCESS );

                if( sParentNode->mBestOptType == QLNC_BEST_OPT_TYPE_MERGE )
                {
                    STL_DASSERT( sPushDownNode->mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_SCAN );

                    /* copy of cost opt info */
                    sCopiedNode->mBestOptType = sPushDownNode->mBestOptType;
                    sCopiedNode->mBestCost = sPushDownNode->mBestCost;
                    sCopiedNode->mIsAnalyzedCBO = sPushDownNode->mIsAnalyzedCBO;
                    ((qlncBaseTableNode*)sCopiedNode)->mTableScanCost
                        = ((qlncBaseTableNode*)sPushDownNode)->mTableScanCost;
                    ((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostCount
                        = ((qlncBaseTableNode*)sPushDownNode)->mIndexScanCostCount;
                    ((qlncBaseTableNode*)sCopiedNode)->mIndexScanCostArr
                        = ((qlncBaseTableNode*)sPushDownNode)->mIndexScanCostArr;
                    ((qlncBaseTableNode*)sCopiedNode)->mIndexCombineCost
                        = ((qlncBaseTableNode*)sPushDownNode)->mIndexCombineCost;
                    ((qlncBaseTableNode*)sCopiedNode)->mRowidScanCost
                        = ((qlncBaseTableNode*)sPushDownNode)->mRowidScanCost;

                    /* 반드시 Index Scan을 해야 하므로 Plan을 변경하지 않는다. */
                    if( ((qlncIndexScanCost*)(sCopiedNode->mBestCost))->mTableAndFilter == NULL )
                    {
                        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                    STL_SIZEOF( qlncAndFilter ),
                                                    (void**) &(((qlncIndexScanCost*)(sCopiedNode->mBestCost))->mTableAndFilter),
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        QLNC_INIT_AND_FILTER( ((qlncIndexScanCost*)(sCopiedNode->mBestCost))->mTableAndFilter );
                    }

                    STL_TRY( qlncAppendOrFilterToAndFilterTail(
                                 aParamInfo,
                                 ((qlncIndexScanCost*)(sCopiedNode->mBestCost))->mTableAndFilter,
                                 1,
                                 &sPushOrFilter,
                                 aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /* Base Table Node Cost 계산 */
                    sCBOptParamInfo.mParamInfo = *aParamInfo;
                    sCBOptParamInfo.mLeftNode = sCopiedNode;
                    sCBOptParamInfo.mRightNode = NULL;
                    sCBOptParamInfo.mJoinCond = NULL;
                    sCBOptParamInfo.mFilter = NULL;
                    sCBOptParamInfo.mCost = NULL;

                    STL_TRY( qlncCBOptCopiedBaseTable( &sCBOptParamInfo,
                                                       aEnv )
                             == STL_SUCCESS );
                }
            }
            else if( sPushDownNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE )
            {
                STL_TRY( qlncCopySubNodeWithoutCBOInfo( aParamInfo,
                                                        sPushDownNode,
                                                        &sCopiedNode,
                                                        aEnv )
                         == STL_SUCCESS );

                /* Push Filter 설정 */
                if( ((qlncSubTableNode*)sCopiedNode)->mFilter == NULL )
                {
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncAndFilter ),
                                                (void**) &(((qlncSubTableNode*)sCopiedNode)->mFilter),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    QLNC_INIT_AND_FILTER( ((qlncSubTableNode*)sCopiedNode)->mFilter );
                }

                STL_TRY( qlncAppendOrFilterToAndFilterTail(
                             aParamInfo,
                             ((qlncSubTableNode*)sCopiedNode)->mFilter,
                             1,
                             &sPushOrFilter,
                             aEnv )
                         == STL_SUCCESS );

                /* Sub Table Node Cost 계산 */
                sCBOptParamInfo.mParamInfo = *aParamInfo;
                sCBOptParamInfo.mLeftNode = sCopiedNode;
                sCBOptParamInfo.mRightNode = NULL;
                sCBOptParamInfo.mJoinCond = NULL;
                sCBOptParamInfo.mFilter = NULL;
                sCBOptParamInfo.mCost = NULL;

                STL_TRY( qlncCBOptSubTable( &sCBOptParamInfo,
                                            aEnv )
                         == STL_SUCCESS );
            }
            else if( sPushDownNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE )
            {
                qlncAndFilter     sPushFilter;

                /* 임시로 만들어지는 Node이므로 별도의 Copy가 필요없음 */
                sCopiedNode = sPushDownNode;

                /* Push Filter 설정 */
                QLNC_INIT_AND_FILTER( &sPushFilter );

                /* Push Filter에 기존 Inner Join Table Node의 Join Condition을 추가 */
                if( ((qlncInnerJoinTableNode*)sCopiedNode)->mJoinCondition != NULL )
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail(
                                 aParamInfo,
                                 &sPushFilter,
                                 ((qlncInnerJoinTableNode*)sCopiedNode)->mJoinCondition->mOrCount,
                                 ((qlncInnerJoinTableNode*)sCopiedNode)->mJoinCondition->mOrFilters,
                                 aEnv )
                             == STL_SUCCESS );
                }

                /* Push Filter에 기존 Inner Join Table Node의 Join Filter를 추가 */
                if( ((qlncInnerJoinTableNode*)sCopiedNode)->mJoinFilter != NULL )
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail(
                                 aParamInfo,
                                 &sPushFilter,
                                 ((qlncInnerJoinTableNode*)sCopiedNode)->mJoinFilter->mOrCount,
                                 ((qlncInnerJoinTableNode*)sCopiedNode)->mJoinFilter->mOrFilters,
                                 aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncAppendOrFilterToAndFilterTail(
                             aParamInfo,
                             &sPushFilter,
                             1,
                             &sPushOrFilter,
                             aEnv )
                         == STL_SUCCESS );

                /* Inner Join Node Info 설정 */
                sInnerJoinNodeInfoArr[0].mNode = ((qlncInnerJoinTableNode*)sCopiedNode)->mLeftNode;
                sInnerJoinNodeInfoArr[0].mAnalyzedNode = NULL;
                sInnerJoinNodeInfoArr[0].mIsFixed = STL_FALSE;
                sInnerJoinNodeInfoArr[0].mIsAnalyzed = STL_FALSE;

                sInnerJoinNodeInfoArr[1].mNode = ((qlncInnerJoinTableNode*)sCopiedNode)->mRightNode;
                sInnerJoinNodeInfoArr[1].mAnalyzedNode = NULL;
                sInnerJoinNodeInfoArr[1].mIsFixed = STL_FALSE;
                sInnerJoinNodeInfoArr[1].mIsAnalyzed = STL_FALSE;

                /* Join Table Node Cost 계산 */
                sCBOptParamInfo.mParamInfo = *aParamInfo;
                sCBOptParamInfo.mLeftNode = ((qlncInnerJoinTableNode*)sCopiedNode)->mLeftNode;
                sCBOptParamInfo.mRightNode = ((qlncInnerJoinTableNode*)sCopiedNode)->mRightNode;
                sCBOptParamInfo.mJoinCond = (sPushFilter.mOrCount == 0 ? NULL : &sPushFilter);
                sCBOptParamInfo.mFilter = NULL;
                sCBOptParamInfo.mCost = NULL;

                STL_TRY( qlncCBOptInnerJoinCost( &sCBOptParamInfo,
                                                 sInnerJoinNodeInfoArr,
                                                 2,
                                                 1,
                                                 aEnv )
                         == STL_SUCCESS );

                stlMemcpy( sCopiedNode,
                           sCBOptParamInfo.mRightNode,
                           STL_SIZEOF( qlncInnerJoinTableNode ) );

                STL_THROW( RAMP_SKIP_SET_COPIED_NODE );
            }
            else if( sPushDownNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE )
            {
                /* 임시로 만들어지는 Node이므로 별도의 Copy가 필요없음 */
                sCopiedNode = sPushDownNode;

                /* Outer Join Table Node의 Join Filter에 Push Filter를 추가 */
                if( ((qlncOuterJoinTableNode*)sCopiedNode)->mJoinFilter == NULL )
                {
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncAndFilter ),
                                                (void**) &(((qlncOuterJoinTableNode*)sCopiedNode)->mJoinFilter),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    QLNC_INIT_AND_FILTER( ((qlncOuterJoinTableNode*)sCopiedNode)->mJoinFilter );
                }

                STL_TRY( qlncAppendOrFilterToAndFilterTail(
                             aParamInfo,
                             ((qlncOuterJoinTableNode*)sCopiedNode)->mJoinFilter,
                             1,
                             &sPushOrFilter,
                             aEnv )
                         == STL_SUCCESS );

                /* Join Table Node Cost 계산 */
                sCBOptParamInfo.mParamInfo = *aParamInfo;
                sCBOptParamInfo.mLeftNode = ((qlncOuterJoinTableNode*)sCopiedNode)->mLeftNode;
                sCBOptParamInfo.mRightNode = ((qlncOuterJoinTableNode*)sCopiedNode)->mRightNode;
                sCBOptParamInfo.mJoinCond = ((qlncOuterJoinTableNode*)sCopiedNode)->mJoinCondition;
                sCBOptParamInfo.mFilter = ((qlncOuterJoinTableNode*)sCopiedNode)->mJoinFilter;
                sCBOptParamInfo.mCost = NULL;

                STL_TRY( qlncCBOptOuterJoinCost( &sCBOptParamInfo,
                                                 ((qlncOuterJoinTableNode*)sCopiedNode)->mJoinDirect,
                                                 aEnv )
                         == STL_SUCCESS );

                stlMemcpy( sCopiedNode,
                           sCBOptParamInfo.mRightNode,
                           STL_SIZEOF( qlncOuterJoinTableNode ) );

                STL_THROW( RAMP_SKIP_SET_COPIED_NODE );
            }
            else if( sPushDownNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
            {
                /* @todo 여기에 들어오는 경우가 없을 것으로 판단됨 */

                /* Sort Instant Node 복사 */
                STL_TRY( qlncCopySortInstantNode( aParamInfo,
                                                  sPushDownNode,
                                                  &sCopiedNode,
                                                  aEnv )
                         == STL_SUCCESS );

                /* Sort Instant Node의 Filter에 Push Filter를 추가 */
                if( ((qlncSortInstantNode*)sCopiedNode)->mFilter == NULL )
                {
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncAndFilter ),
                                                (void**) &(((qlncSortInstantNode*)sCopiedNode)->mFilter),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    QLNC_INIT_AND_FILTER( ((qlncSortInstantNode*)sCopiedNode)->mFilter );

                }

                STL_TRY( qlncAppendOrFilterToAndFilterTail(
                             aParamInfo,
                             ((qlncSortInstantNode*)sCopiedNode)->mFilter,
                             1,
                             &sPushOrFilter,
                             aEnv )
                         == STL_SUCCESS );

                /* Sort Instant Node Cost 계산 */
                sCBOptParamInfo.mParamInfo = *aParamInfo;
                sCBOptParamInfo.mLeftNode = sCopiedNode;
                sCBOptParamInfo.mRightNode = NULL;
                sCBOptParamInfo.mJoinCond = NULL;
                sCBOptParamInfo.mFilter = NULL;
                sCBOptParamInfo.mCost = NULL;

                STL_TRY( qlncCBOptSortInstant( &sCBOptParamInfo,
                                               aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sPushDownNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT );

                /* Hash Instant Node 복사 */
                STL_TRY( qlncCopyHashInstantNode( aParamInfo,
                                                  sPushDownNode,
                                                  &sCopiedNode,
                                                  aEnv )
                         == STL_SUCCESS );

                /* Hash Instant Node의 Filter에 Push Filter를 추가 */
                if( ((qlncHashInstantNode*)sCopiedNode)->mFilter == NULL )
                {
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncAndFilter ),
                                                (void**) &(((qlncHashInstantNode*)sCopiedNode)->mFilter),
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    QLNC_INIT_AND_FILTER( ((qlncHashInstantNode*)sCopiedNode)->mFilter );

                }

                STL_TRY( qlncAppendOrFilterToAndFilterTail(
                             aParamInfo,
                             ((qlncHashInstantNode*)sCopiedNode)->mFilter,
                             1,
                             &sPushOrFilter,
                             aEnv )
                         == STL_SUCCESS );

                /* Hash Instant Node Cost 계산 */
                sCBOptParamInfo.mParamInfo = *aParamInfo;
                sCBOptParamInfo.mLeftNode = sCopiedNode;
                sCBOptParamInfo.mRightNode = NULL;
                sCBOptParamInfo.mJoinCond = NULL;
                sCBOptParamInfo.mFilter = NULL;
                sCBOptParamInfo.mCost = NULL;

                STL_TRY( qlncCBOptHashInstant( &sCBOptParamInfo,
                                               aEnv )
                         == STL_SUCCESS );
            }


            STL_RAMP( RAMP_RETRY_SET_COPIED_NODE );


            switch( sParentNode->mBestOptType )
            {
                case QLNC_BEST_OPT_TYPE_NESTED_LOOPS:
                    if( ((qlncNestedJoinCost*)(sParentNode->mBestCost))->mLeftNode->mNodeID
                        == sCopiedNode->mNodeID )
                    {
                        ((qlncNestedJoinCost*)(sParentNode->mBestCost))->mLeftNode = sCopiedNode;
                    }
                    else
                    {
                        STL_DASSERT( ((qlncNestedJoinCost*)(sParentNode->mBestCost))->mRightNode->mNodeID
                                     == sCopiedNode->mNodeID );
                        ((qlncNestedJoinCost*)(sParentNode->mBestCost))->mRightNode = sCopiedNode;
                    }
                    break;

                case QLNC_BEST_OPT_TYPE_MERGE:
                    if( ((qlncMergeJoinCost*)(sParentNode->mBestCost))->mLeftNode->mNodeID
                        == sCopiedNode->mNodeID )
                    {
                        ((qlncMergeJoinCost*)(sParentNode->mBestCost))->mLeftNode = sCopiedNode;
                    }
                    else
                    {
                        STL_DASSERT( ((qlncMergeJoinCost*)(sParentNode->mBestCost))->mRightNode->mNodeID
                                     == sCopiedNode->mNodeID );
                        ((qlncMergeJoinCost*)(sParentNode->mBestCost))->mRightNode = sCopiedNode;
                    }
                    break;

                case QLNC_BEST_OPT_TYPE_HASH:
                    if( ((qlncHashJoinCost*)(sParentNode->mBestCost))->mLeftNode->mNodeID
                        == sCopiedNode->mNodeID )
                    {
                        ((qlncHashJoinCost*)(sParentNode->mBestCost))->mLeftNode = sCopiedNode;
                    }
                    else
                    {
                        STL_DASSERT( ((qlncHashJoinCost*)(sParentNode->mBestCost))->mRightNode->mNodeID
                                     == sCopiedNode->mNodeID );
                        ((qlncHashJoinCost*)(sParentNode->mBestCost))->mRightNode = sCopiedNode;
                    }
                    break;
                case QLNC_BEST_OPT_TYPE_INNER_JOIN:
                case QLNC_BEST_OPT_TYPE_OUTER_JOIN:
                case QLNC_BEST_OPT_TYPE_SEMI_JOIN:
                case QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN:
                    STL_TRY( qlncCopyJoinNodeWithoutCBOInfo( aParamInfo,
                                                             sParentNode,
                                                             &sPushDownNode,
                                                             aEnv )
                             == STL_SUCCESS );
                    sPushDownNode->mBestOptType = sParentNode->mBestOptType;
                    sPushDownNode->mBestCost = sParentNode->mBestCost;
                    sPushDownNode->mIsAnalyzedCBO = sParentNode->mIsAnalyzedCBO;
                    ((qlncJoinTableNode*)sPushDownNode)->mBestInnerJoinTable
                        = ((qlncJoinTableNode*)sParentNode)->mBestInnerJoinTable;
                    ((qlncJoinTableNode*)sPushDownNode)->mBestOuterJoinTable
                        = ((qlncJoinTableNode*)sParentNode)->mBestOuterJoinTable;
                    ((qlncJoinTableNode*)sPushDownNode)->mBestSemiJoinTable
                        = ((qlncJoinTableNode*)sParentNode)->mBestSemiJoinTable;
                    ((qlncJoinTableNode*)sPushDownNode)->mBestAntiSemiJoinTable
                        = ((qlncJoinTableNode*)sParentNode)->mBestAntiSemiJoinTable;

                    for( j = 0; j < ((qlncJoinTableNode*)sPushDownNode)->mNodeArray.mCurCnt; j++ )
                    {
                        if( ((qlncJoinTableNode*)sPushDownNode)->mNodeArray.mNodeArr[i]->mNodeID
                            == sCopiedNode->mNodeID )
                        {
                            ((qlncJoinTableNode*)sPushDownNode)->mNodeArray.mNodeArr[i] = sCopiedNode;
                            break;
                        }
                    }
                    STL_DASSERT( j < ((qlncJoinTableNode*)sPushDownNode)->mNodeArray.mCurCnt );
                    sCopiedNode = sPushDownNode;
                    sParentNode = qlncFindParentNodeByCostOpt( aNode,
                                                               NULL,
                                                               sCopiedNode );
                    STL_THROW( RAMP_RETRY_SET_COPIED_NODE );
                    break;
                case QLNC_BEST_OPT_TYPE_SORT_INSTANT:
                    STL_TRY( qlncCopySortInstantNode( aParamInfo,
                                                      sParentNode,
                                                      &sPushDownNode,
                                                      aEnv )
                             == STL_SUCCESS );
                    ((qlncSortInstantNode*)sPushDownNode)->mChildNode = sCopiedNode;
                    sCopiedNode = sPushDownNode;
                    sParentNode = qlncFindParentNodeByCostOpt( aNode,
                                                               NULL,
                                                               sCopiedNode );
                    STL_THROW( RAMP_RETRY_SET_COPIED_NODE );
                    break;
                case QLNC_BEST_OPT_TYPE_HASH_INSTANT:
                case QLNC_BEST_OPT_TYPE_GROUP:
                    STL_TRY( qlncCopyHashInstantNode( aParamInfo,
                                                      sParentNode,
                                                      &sPushDownNode,
                                                      aEnv )
                             == STL_SUCCESS );
                    ((qlncHashInstantNode*)sPushDownNode)->mChildNode = sCopiedNode;
                    sCopiedNode = sPushDownNode;
                    sParentNode = qlncFindParentNodeByCostOpt( aNode,
                                                               NULL,
                                                               sCopiedNode );
                    STL_THROW( RAMP_RETRY_SET_COPIED_NODE );
                    break;
                case QLNC_BEST_OPT_TYPE_QUERY_SPEC:
                    ((qlncQuerySpecCost*)(sParentNode->mBestCost))->mChildNode = sCopiedNode;
                    break;
                case QLNC_BEST_OPT_TYPE_FILTER:
                    STL_TRY( qlncCopySubNodeWithoutCBOInfo( aParamInfo,
                                                            sParentNode,
                                                            &sPushDownNode,
                                                            aEnv )
                             == STL_SUCCESS );
                    sPushDownNode->mBestOptType = sParentNode->mBestOptType;
                    sPushDownNode->mBestCost = sParentNode->mBestCost;
                    sPushDownNode->mIsAnalyzedCBO = sParentNode->mIsAnalyzedCBO;
                    ((qlncSubTableNode*)sPushDownNode)->mFilterCost
                        = ((qlncSubTableNode*)sParentNode)->mFilterCost;

                    ((qlncSubTableNode*)sPushDownNode)->mQueryNode = sCopiedNode;
                    sCopiedNode = sPushDownNode;
                    sParentNode = qlncFindParentNodeByCostOpt( aNode,
                                                               (sCopiedNode->mNodeID == aNode->mNodeID
                                                                ? aParentNode : NULL),
                                                               sCopiedNode );
                    STL_THROW( RAMP_RETRY_SET_COPIED_NODE );
                    break;
                default:
                    STL_DASSERT( 0 );
                    break;
            }


            STL_RAMP( RAMP_SKIP_SET_COPIED_NODE );


            sFilter->mOrFilters[i] = NULL;
        }

        STL_RAMP( RAMP_SKIP );
    }

    /* Join Filter를 Compact함 */
    STL_TRY( qlncCompactAndFilter( sFilter,
                                   aEnv )
             == STL_SUCCESS );

    /* Filter가 비었다면 NULL로 처리 */
    if( sFilter->mOrCount == 0 )
    {
        if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_NESTED_LOOPS )
        {
            ((qlncNestedJoinCost*)(aNode->mBestCost))->mJoinFilter = NULL;
        }
        else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_MERGE )
        {
            ((qlncMergeJoinCost*)(aNode->mBestCost))->mJoinFilter = NULL;
        }
        else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_HASH )
        {
            ((qlncHashJoinCost*)(aNode->mBestCost))->mJoinFilter = NULL;
        }
        else
        {
            STL_DASSERT( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_FILTER );
            ((qlncFilterCost*)(aNode->mBestCost))->mFilter = NULL;
        }
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Filter가 Push Down 가능한 Node를 찾는다.
 * @param[in]   aParamInfo          Parameter Info
 * @param[in]   aCurNode            Current Candidate Node
 * @param[in]   aParentNode         Parent Candidate Node
 * @param[in]   aPushSubqType       Push Subq Type
 * @param[in]   aIsConstSubQuery    Constant SubQuery 여부
 * @param[in]   aBestRowCount       Best Result Row Count
 * @param[in]   aOrFilter           Or Filter
 * @param[out]  aFoundNode          Found Candidate Node
 * @param[out]  aFoundParentNode    Found Parent Candidate Node
 * @param[out]  aOutOrFilter        Push Node에 적용할 Or Filter
 * @param[in]   aEnv                Environment
 */
stlStatus qlncFindNodeForPushDownSubQueryFilter( qlncParamInfo      * aParamInfo,
                                                 qlncNodeCommon     * aCurNode,
                                                 qlncNodeCommon     * aParentNode,
                                                 qlncPushSubqType     aPushSubqType,
                                                 stlBool              aIsConstSubQuery,
                                                 stlFloat64         * aBestRowCount,
                                                 qlncOrFilter       * aOrFilter,
                                                 qlncNodeCommon    ** aFoundNode,
                                                 qlncNodeCommon    ** aFoundParentNode,
                                                 qlncOrFilter      ** aOutOrFilter,
                                                 qllEnv             * aEnv )
{
    qlncNodeCommon  * sPrevFoundNode    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCurNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrFilter != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPushSubqType != QLNC_PUSH_SUBQ_TYPE_NO, QLL_ERROR_STACK(aEnv) );


    /**
     * Current Node에 대한 체크
     */

    /* 더이상 진행 불가한 경우 처리 */
    STL_TRY_THROW( (aCurNode->mBestOptType != QLNC_BEST_OPT_TYPE_QUERY_SET) &&
                   (aCurNode->mBestOptType != QLNC_BEST_OPT_TYPE_SET_OP) &&
                   (aCurNode->mBestOptType != QLNC_BEST_OPT_TYPE_QUERY_SPEC) &&
                   (aCurNode->mBestOptType != QLNC_BEST_OPT_TYPE_INDEX_COMBINE) &&
                   (aCurNode->mBestOptType != QLNC_BEST_OPT_TYPE_JOIN_COMBINE) &&
                   (aCurNode->mBestOptType != QLNC_BEST_OPT_TYPE_SEMI_JOIN) &&
                   (aCurNode->mBestOptType != QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN),
                   RAMP_FINISH );

    /* 현재 노드가 Instant인 경우 Join Type이면 현재 노드에는 Push 불가 */
    STL_TRY_THROW( !( (aCurNode->mBestOptType == QLNC_BEST_OPT_TYPE_SORT_INSTANT) &&
                      (((qlncSortInstantNode*)aCurNode)->mInstantType == QLNC_INSTANT_TYPE_JOIN) ),
                   RAMP_SKIP_CURR_CHECK );

    STL_TRY_THROW( !( ((aCurNode->mBestOptType == QLNC_BEST_OPT_TYPE_HASH_INSTANT) ||
                       (aCurNode->mBestOptType == QLNC_BEST_OPT_TYPE_GROUP)) &&
                      (((qlncHashInstantNode*)aCurNode)->mInstantType == QLNC_INSTANT_TYPE_JOIN) ),
                   RAMP_SKIP_CURR_CHECK );

    /* 현재 노드가 Join Node인 경우 현재 노드에는 Push 불가 */
    STL_TRY_THROW( aCurNode->mNodeType != QLNC_NODE_TYPE_JOIN_TABLE,
                    RAMP_SKIP_CURR_CHECK );

    /* Push 가능여부 판단 */
    if( qlncIsUsableOrFilterOnNode( aCurNode, aOrFilter ) == STL_TRUE )
    {
        if( aPushSubqType == QLNC_PUSH_SUBQ_TYPE_YES )
        {
            /* Push Subq Hint가 있었으면 무조건 가능한 최하위 노드를 찾는다. */
            (*aBestRowCount) = aCurNode->mBestCost->mResultRowCount;
            *aFoundNode = aCurNode;
            *aFoundParentNode = aParentNode;
            *aOutOrFilter = aOrFilter;
        }
        else
        {
            /* Push Subq Hint가 Default이면 가장 좋은 노드를 찾는다. */
            if( !QLNC_LESS_THAN_DOUBLE( *aBestRowCount, aCurNode->mBestCost->mResultRowCount  ) )
            {
                (*aBestRowCount) = aCurNode->mBestCost->mResultRowCount;
                *aFoundNode = aCurNode;
                *aFoundParentNode = aParentNode;
                *aOutOrFilter = aOrFilter;
            }
        }
    }
    else
    {
        /* 현재 노드에서 사용 불가하므로 하위노드도 사용 불가 */
        STL_THROW( RAMP_FINISH );
    }


    STL_RAMP( RAMP_SKIP_CURR_CHECK );


    /**
     * 하위 노드 진행 탐색
     */

    switch( aCurNode->mBestOptType )
    {
        /* Sub Table */
        case QLNC_BEST_OPT_TYPE_FILTER:          /**< Filter */
            {
                /* 하위 Query Node가 Query Set이면 진행 불가 */
                STL_TRY_THROW( ((qlncSubTableNode*)aCurNode)->mQueryNode->mNodeType
                               != QLNC_NODE_TYPE_QUERY_SET,
                               RAMP_FINISH );

                STL_TRY( qlncDuplicateExpr( aParamInfo,
                                            (qlncExprCommon*)aOrFilter,
                                            (qlncExprCommon**)&aOrFilter,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlncFPDChangeInnerColumnToTargetExpr(
                             aParamInfo,
                             aCurNode->mNodeID,
                             ((qlncQuerySpec*)(((qlncSubTableNode*)aCurNode)->mQueryNode))->mTargetCount,
                             ((qlncQuerySpec*)(((qlncSubTableNode*)aCurNode)->mQueryNode))->mTargetArr,
                             (qlncExprCommon*)aOrFilter,
                             (qlncExprCommon**)&aOrFilter,
                             aEnv )
                         == STL_SUCCESS );

                if( ((qlncFilterCost*)(aCurNode->mBestCost))->mMaterializedNode != NULL )
                {
                    /* @todo 여기에 들어오는 테스트 케이스를 만들기 어려움 */
                    STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                                 aParamInfo,
                                 ((qlncFilterCost*)(aCurNode->mBestCost))->mMaterializedNode,
                                 aCurNode,
                                 aPushSubqType,
                                 aIsConstSubQuery,
                                 aBestRowCount,
                                 aOrFilter,
                                 aFoundNode,
                                 aFoundParentNode,
                                 aOutOrFilter,
                                 aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                                 aParamInfo,
                                 ((qlncQuerySpec*)(((qlncSubTableNode*)aCurNode)->mQueryNode))->mChildNode,
                                 ((qlncSubTableNode*)aCurNode)->mQueryNode,
                                 aPushSubqType,
                                 aIsConstSubQuery,
                                 aBestRowCount,
                                 aOrFilter,
                                 aFoundNode,
                                 aFoundParentNode,
                                 aOutOrFilter,
                                 aEnv )
                             == STL_SUCCESS );
                }

                break;
            }

        /* Join */
        case QLNC_BEST_OPT_TYPE_INNER_JOIN:      /**< Inner Join */
            {
                /* Best Opt Node 탐색 */
                STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                             aParamInfo,
                             (qlncNodeCommon*)(((qlncJoinTableNode*)aCurNode)->mBestInnerJoinTable),
                             aCurNode,
                             aPushSubqType,
                             aIsConstSubQuery,
                             aBestRowCount,
                             aOrFilter,
                             aFoundNode,
                             aFoundParentNode,
                             aOutOrFilter,
                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_BEST_OPT_TYPE_OUTER_JOIN:      /**< Outer Join */
            {
                /* Best Opt Node 탐색 */
                STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                             aParamInfo,
                             (qlncNodeCommon*)(((qlncJoinTableNode*)aCurNode)->mBestOuterJoinTable),
                             aCurNode,
                             aPushSubqType,
                             aIsConstSubQuery,
                             aBestRowCount,
                             aOrFilter,
                             aFoundNode,
                             aFoundParentNode,
                             aOutOrFilter,
                             aEnv )
                         == STL_SUCCESS );
                break;
            }

        /* Join Operation */
        case QLNC_BEST_OPT_TYPE_NESTED_LOOPS:    /**< Nested Loops Join */
            {
                if( aCurNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE )
                {
                    sPrevFoundNode = *aFoundNode;

                    /* Left Node에 대한 탐색 */
                    STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                                 aParamInfo,
                                 ((qlncNestedJoinCost*)(aCurNode->mBestCost))->mLeftNode,
                                 aCurNode,
                                 aPushSubqType,
                                 aIsConstSubQuery,
                                 aBestRowCount,
                                 aOrFilter,
                                 aFoundNode,
                                 aFoundParentNode,
                                 aOutOrFilter,
                                 aEnv )
                             == STL_SUCCESS );

                    /* Right Node에 대한 탐색 */
                    if( (sPrevFoundNode->mNodeID == (*aFoundNode)->mNodeID) &&
                        (aIsConstSubQuery == STL_TRUE) )
                    {
                        STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                                     aParamInfo,
                                     ((qlncNestedJoinCost*)(aCurNode->mBestCost))->mRightNode,
                                     aCurNode,
                                     aPushSubqType,
                                     aIsConstSubQuery,
                                     aBestRowCount,
                                     aOrFilter,
                                     aFoundNode,
                                     aFoundParentNode,
                                     aOutOrFilter,
                                     aEnv )
                                 == STL_SUCCESS );
                    }
                }
                else if( aCurNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE )
                {
                    if( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                        == QLNC_JOIN_DIRECT_LEFT )
                    {
                        /* Left Node에 대한 탐색 */
                        STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                                     aParamInfo,
                                     ((qlncNestedJoinCost*)(aCurNode->mBestCost))->mLeftNode,
                                     aCurNode,
                                     aPushSubqType,
                                     aIsConstSubQuery,
                                     aBestRowCount,
                                     aOrFilter,
                                     aFoundNode,
                                     aFoundParentNode,
                                     aOutOrFilter,
                                     aEnv )
                                 == STL_SUCCESS );
                    }
                    else if( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                             == QLNC_JOIN_DIRECT_RIGHT )
                    {
                        /* Right Node에 대한 탐색 */
                        STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                                     aParamInfo,
                                     ((qlncNestedJoinCost*)(aCurNode->mBestCost))->mRightNode,
                                     aCurNode,
                                     aPushSubqType,
                                     aIsConstSubQuery,
                                     aBestRowCount,
                                     aOrFilter,
                                     aFoundNode,
                                     aFoundParentNode,
                                     aOutOrFilter,
                                     aEnv )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        STL_DASSERT( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                                     == QLNC_JOIN_DIRECT_FULL );
                        STL_THROW( RAMP_FINISH );
                    }
                }
                else
                {
                    STL_DASSERT( (aCurNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE) ||
                                 (aCurNode->mNodeType == QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE) );

                    /* Left Node에 대한 탐색 */
                    STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                                 aParamInfo,
                                 ((qlncNestedJoinCost*)(aCurNode->mBestCost))->mLeftNode,
                                 aCurNode,
                                 aPushSubqType,
                                 aIsConstSubQuery,
                                 aBestRowCount,
                                 aOrFilter,
                                 aFoundNode,
                                 aFoundParentNode,
                                 aOutOrFilter,
                                 aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLNC_BEST_OPT_TYPE_MERGE:           /**< Merge Join */
            {
                if( aCurNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE )
                {
                    sPrevFoundNode = *aFoundNode;

                    /* Left Node에 대한 탐색 */
                    STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                                 aParamInfo,
                                 ((qlncMergeJoinCost*)(aCurNode->mBestCost))->mLeftNode,
                                 aCurNode,
                                 aPushSubqType,
                                 aIsConstSubQuery,
                                 aBestRowCount,
                                 aOrFilter,
                                 aFoundNode,
                                 aFoundParentNode,
                                 aOutOrFilter,
                                 aEnv )
                             == STL_SUCCESS );

                    /* Right Node에 대한 탐색 */
                    if( sPrevFoundNode->mNodeID == (*aFoundNode)->mNodeID )
                    {
                        STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                                     aParamInfo,
                                     ((qlncMergeJoinCost*)(aCurNode->mBestCost))->mRightNode,
                                     aCurNode,
                                     aPushSubqType,
                                     aIsConstSubQuery,
                                     aBestRowCount,
                                     aOrFilter,
                                     aFoundNode,
                                     aFoundParentNode,
                                     aOutOrFilter,
                                     aEnv )
                                 == STL_SUCCESS );
                    }
                }
                else
                {
                    STL_DASSERT( aCurNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE );

                    if( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                        == QLNC_JOIN_DIRECT_LEFT )
                    {
                        /* Left Node에 대한 탐색 */
                        STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                                     aParamInfo,
                                     ((qlncMergeJoinCost*)(aCurNode->mBestCost))->mLeftNode,
                                     aCurNode,
                                     aPushSubqType,
                                     aIsConstSubQuery,
                                     aBestRowCount,
                                     aOrFilter,
                                     aFoundNode,
                                     aFoundParentNode,
                                     aOutOrFilter,
                                     aEnv )
                                 == STL_SUCCESS );
                    }
                    else if( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                             == QLNC_JOIN_DIRECT_RIGHT )
                    {
                        /* Right Node에 대한 탐색 */
                        STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                                     aParamInfo,
                                     ((qlncMergeJoinCost*)(aCurNode->mBestCost))->mRightNode,
                                     aCurNode,
                                     aPushSubqType,
                                     aIsConstSubQuery,
                                     aBestRowCount,
                                     aOrFilter,
                                     aFoundNode,
                                     aFoundParentNode,
                                     aOutOrFilter,
                                     aEnv )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        STL_DASSERT( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                                     == QLNC_JOIN_DIRECT_FULL );
                        STL_THROW( RAMP_FINISH );
                    }
                }
                break;
            }
        case QLNC_BEST_OPT_TYPE_HASH:            /**< Hash Join */
            {
                if( aCurNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE )
                {
                    sPrevFoundNode = *aFoundNode;

                    /* Left Node에 대한 탐색 */
                    STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                                 aParamInfo,
                                 ((qlncHashJoinCost*)(aCurNode->mBestCost))->mLeftNode,
                                 aCurNode,
                                 aPushSubqType,
                                 aIsConstSubQuery,
                                 aBestRowCount,
                                 aOrFilter,
                                 aFoundNode,
                                 aFoundParentNode,
                                 aOutOrFilter,
                                 aEnv )
                             == STL_SUCCESS );

                    /* Right Node에 대한 탐색 */
                    if( sPrevFoundNode->mNodeID == (*aFoundNode)->mNodeID )
                    {
                        STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                                     aParamInfo,
                                     ((qlncHashJoinCost*)(aCurNode->mBestCost))->mRightNode,
                                     aCurNode,
                                     aPushSubqType,
                                     aIsConstSubQuery,
                                     aBestRowCount,
                                     aOrFilter,
                                     aFoundNode,
                                     aFoundParentNode,
                                     aOutOrFilter,
                                     aEnv )
                                 == STL_SUCCESS );
                    }
                }
                else
                {
                    STL_DASSERT( aCurNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE );

                    if( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                        == QLNC_JOIN_DIRECT_LEFT )
                    {
                        /* Left Node에 대한 탐색 */
                        STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                                     aParamInfo,
                                     ((qlncHashJoinCost*)(aCurNode->mBestCost))->mLeftNode,
                                     aCurNode,
                                     aPushSubqType,
                                     aIsConstSubQuery,
                                     aBestRowCount,
                                     aOrFilter,
                                     aFoundNode,
                                     aFoundParentNode,
                                     aOutOrFilter,
                                     aEnv )
                                 == STL_SUCCESS );
                    }
                    else if( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                             == QLNC_JOIN_DIRECT_RIGHT )
                    {
                        /* @todo 여기에 들어오는 경우가 없을 것으로 판단됨 */

                        /* Right Node에 대한 탐색 */
                        STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                                     aParamInfo,
                                     ((qlncHashJoinCost*)(aCurNode->mBestCost))->mRightNode,
                                     aCurNode,
                                     aPushSubqType,
                                     aIsConstSubQuery,
                                     aBestRowCount,
                                     aOrFilter,
                                     aFoundNode,
                                     aFoundParentNode,
                                     aOutOrFilter,
                                     aEnv )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        STL_DASSERT( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                                     == QLNC_JOIN_DIRECT_FULL );
                        STL_THROW( RAMP_FINISH );
                    }
                }
                break;
            }

        /* Instant */
        case QLNC_BEST_OPT_TYPE_SORT_INSTANT:    /**< Sort Instant */
            {
                STL_TRY( qlncDuplicateExpr( aParamInfo,
                                            (qlncExprCommon*)aOrFilter,
                                            (qlncExprCommon**)&aOrFilter,
                                            aEnv )
                         == STL_SUCCESS );

                if( (((qlncSortInstantNode*)aCurNode)->mInstantType != QLNC_INSTANT_TYPE_JOIN) &&
                    (((qlncSortInstantNode*)aCurNode)->mInstantType != QLNC_INSTANT_TYPE_NORMAL) )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToOrgExpr(
                                 aParamInfo,
                                 aCurNode,
                                 (qlncExprCommon*)aOrFilter,
                                 (qlncExprCommon**)&aOrFilter,
                                 aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                             aParamInfo,
                             ((qlncSortInstantNode*)aCurNode)->mChildNode,
                             aCurNode,
                             aPushSubqType,
                             aIsConstSubQuery,
                             aBestRowCount,
                             aOrFilter,
                             aFoundNode,
                             aFoundParentNode,
                             aOutOrFilter,
                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_BEST_OPT_TYPE_HASH_INSTANT:    /**< Hash Instant */
        case QLNC_BEST_OPT_TYPE_GROUP:           /**< Group */
            {
                if( (((qlncHashInstantNode*)aCurNode)->mInstantType == QLNC_INSTANT_TYPE_GROUP) &&
                    (qlncHasAggregationOnExpr( (qlncExprCommon*)aOrFilter,
                                               aCurNode->mNodeID,
                                               STL_FALSE )
                     == STL_TRUE) )
                {
                    STL_THROW( RAMP_FINISH );
                }

                STL_TRY( qlncDuplicateExpr( aParamInfo,
                                            (qlncExprCommon*)aOrFilter,
                                            (qlncExprCommon**)&aOrFilter,
                                            aEnv )
                         == STL_SUCCESS );

                if( ((qlncHashInstantNode*)aCurNode)->mInstantType != QLNC_INSTANT_TYPE_JOIN )
                {
                    STL_TRY( qlncFPDChangeInnerColumnToOrgExpr(
                                 aParamInfo,
                                 aCurNode,
                                 (qlncExprCommon*)aOrFilter,
                                 (qlncExprCommon**)&aOrFilter,
                                 aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncFindNodeForPushDownSubQueryFilter(
                             aParamInfo,
                             ((qlncHashInstantNode*)aCurNode)->mChildNode,
                             aCurNode,
                             aPushSubqType,
                             aIsConstSubQuery,
                             aBestRowCount,
                             aOrFilter,
                             aFoundNode,
                             aFoundParentNode,
                             aOutOrFilter,
                             aEnv )
                         == STL_SUCCESS );
                break;
            }
        default:
            /* 더이상 진행 안함 */
            break;
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Parent Node를 찾는다.
 * @param[in]   aCurNode    Current Candidate Node
 * @param[in]   aParentNode Parent Candidate Node
 * @param[in]   aFindNode   Candidate Node For Finding
 */
qlncNodeCommon * qlncFindParentNodeByCostOpt( qlncNodeCommon    * aCurNode,
                                              qlncNodeCommon    * aParentNode,
                                              qlncNodeCommon    * aFindNode)
{
    qlncNodeCommon  * sFoundNode    = NULL;


    if( aCurNode->mNodeID == aFindNode->mNodeID )
    {
        return aParentNode;
    }

    switch( aCurNode->mBestOptType )
    {
        /* Query Spec */
        case QLNC_BEST_OPT_TYPE_QUERY_SPEC:      /**< Query Spec */
            {
                sFoundNode = qlncFindParentNodeByCostOpt(
                                 ((qlncQuerySpecCost*)(aCurNode->mBestCost))->mChildNode,
                                 aCurNode,
                                 aFindNode );

                break;
            }

        /* Sub Table */
        case QLNC_BEST_OPT_TYPE_FILTER:          /**< Filter */
            {
                if( ((qlncFilterCost*)(aCurNode->mBestCost))->mMaterializedNode != NULL )
                {
                    sFoundNode = qlncFindParentNodeByCostOpt(
                                     ((qlncFilterCost*)(aCurNode->mBestCost))->mMaterializedNode,
                                     aCurNode,
                                     aFindNode );
                }
                else
                {
                    sFoundNode = qlncFindParentNodeByCostOpt(
                                     ((qlncSubTableNode*)aCurNode)->mQueryNode,
                                     aCurNode,
                                     aFindNode );
                }

                break;
            }

        /* Join */
        case QLNC_BEST_OPT_TYPE_INNER_JOIN:      /**< Inner Join */
            {
                sFoundNode = qlncFindParentNodeByCostOpt(
                                 (qlncNodeCommon*)(((qlncJoinTableNode*)aCurNode)->mBestInnerJoinTable),
                                 aCurNode,
                                 aFindNode );
                break;
            }
        case QLNC_BEST_OPT_TYPE_OUTER_JOIN:      /**< Outer Join */
            {
                sFoundNode = qlncFindParentNodeByCostOpt(
                                 (qlncNodeCommon*)(((qlncJoinTableNode*)aCurNode)->mBestOuterJoinTable),
                                 aCurNode,
                                 aFindNode );
                break;
            }

        /* Join Operation */
        case QLNC_BEST_OPT_TYPE_NESTED_LOOPS:    /**< Nested Loops Join */
            {
                if( aCurNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE )
                {
                    sFoundNode = qlncFindParentNodeByCostOpt(
                                     ((qlncNestedJoinCost*)(aCurNode->mBestCost))->mLeftNode,
                                     aCurNode,
                                     aFindNode );

                    if( sFoundNode == NULL )
                    {
                        sFoundNode = qlncFindParentNodeByCostOpt(
                                         ((qlncNestedJoinCost*)(aCurNode->mBestCost))->mRightNode,
                                         aCurNode,
                                         aFindNode );
                    }
                }
                else if( aCurNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE )
                {
                    if( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                        == QLNC_JOIN_DIRECT_LEFT )
                    {
                        sFoundNode = qlncFindParentNodeByCostOpt(
                                         ((qlncNestedJoinCost*)(aCurNode->mBestCost))->mLeftNode,
                                         aCurNode,
                                         aFindNode );
                    }
                    else if( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                             == QLNC_JOIN_DIRECT_RIGHT )
                    {
                        sFoundNode = qlncFindParentNodeByCostOpt(
                                         ((qlncNestedJoinCost*)(aCurNode->mBestCost))->mRightNode,
                                         aCurNode,
                                         aFindNode );
                    }
                    else
                    {
                        STL_DASSERT( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                                     == QLNC_JOIN_DIRECT_FULL );
                        sFoundNode = NULL;
                    }
                }
                else
                {
                    STL_DASSERT( (aCurNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE) ||
                                 (aCurNode->mNodeType == QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE) );

                    sFoundNode = qlncFindParentNodeByCostOpt(
                                     ((qlncNestedJoinCost*)(aCurNode->mBestCost))->mLeftNode,
                                     aCurNode,
                                     aFindNode );
                }
                break;
            }
        case QLNC_BEST_OPT_TYPE_MERGE:           /**< Merge Join */
            {
                if( aCurNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE )
                {
                    sFoundNode = qlncFindParentNodeByCostOpt(
                                     ((qlncMergeJoinCost*)(aCurNode->mBestCost))->mLeftNode,
                                     aCurNode,
                                     aFindNode );

                    if( sFoundNode == NULL )
                    {
                        sFoundNode = qlncFindParentNodeByCostOpt(
                                         ((qlncMergeJoinCost*)(aCurNode->mBestCost))->mRightNode,
                                         aCurNode,
                                         aFindNode );
                    }
                }
                else
                {
                    STL_DASSERT( aCurNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE );

                    if( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                        == QLNC_JOIN_DIRECT_LEFT )
                    {
                        sFoundNode = qlncFindParentNodeByCostOpt(
                                         ((qlncMergeJoinCost*)(aCurNode->mBestCost))->mLeftNode,
                                         aCurNode,
                                         aFindNode );
                    }
                    else if( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                             == QLNC_JOIN_DIRECT_RIGHT )
                    {
                        sFoundNode = qlncFindParentNodeByCostOpt(
                                         ((qlncMergeJoinCost*)(aCurNode->mBestCost))->mRightNode,
                                         aCurNode,
                                         aFindNode );
                    }
                    else
                    {
                        STL_DASSERT( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                                     == QLNC_JOIN_DIRECT_FULL );
                        sFoundNode = NULL;
                    }
                }
                break;
            }
        case QLNC_BEST_OPT_TYPE_HASH:            /**< Hash Join */
            {
                if( aCurNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE )
                {
                    sFoundNode = qlncFindParentNodeByCostOpt(
                                     ((qlncHashJoinCost*)(aCurNode->mBestCost))->mLeftNode,
                                     aCurNode,
                                     aFindNode );

                    if( sFoundNode == NULL )
                    {
                        sFoundNode = qlncFindParentNodeByCostOpt(
                                         ((qlncHashJoinCost*)(aCurNode->mBestCost))->mRightNode,
                                         aCurNode,
                                         aFindNode );
                    }
                }
                else
                {
                    STL_DASSERT( aCurNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE );

                    if( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                        == QLNC_JOIN_DIRECT_LEFT )
                    {
                        sFoundNode = qlncFindParentNodeByCostOpt(
                                         ((qlncHashJoinCost*)(aCurNode->mBestCost))->mLeftNode,
                                         aCurNode,
                                         aFindNode );
                    }
                    else if( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                             == QLNC_JOIN_DIRECT_RIGHT )
                    {
                        sFoundNode = qlncFindParentNodeByCostOpt(
                                         ((qlncHashJoinCost*)(aCurNode->mBestCost))->mRightNode,
                                         aCurNode,
                                         aFindNode );
                    }
                    else
                    {
                        STL_DASSERT( ((qlncOuterJoinTableNode*)aCurNode)->mJoinDirect
                                     == QLNC_JOIN_DIRECT_FULL );
                        sFoundNode = NULL;
                    }
                }
                break;
            }

        /* Instant */
        case QLNC_BEST_OPT_TYPE_SORT_INSTANT:    /**< Sort Instant */
            {
                sFoundNode = qlncFindParentNodeByCostOpt(
                                 ((qlncSortInstantNode*)aCurNode)->mChildNode,
                                 aCurNode,
                                 aFindNode );
                break;
            }
        case QLNC_BEST_OPT_TYPE_HASH_INSTANT:    /**< Hash Instant */
        case QLNC_BEST_OPT_TYPE_GROUP:           /**< Group */
            {
                sFoundNode = qlncFindParentNodeByCostOpt(
                                 ((qlncHashInstantNode*)aCurNode)->mChildNode,
                                 aCurNode,
                                 aFindNode );
                break;
            }
        default:
            sFoundNode = NULL;
            break;
    }


    return sFoundNode;
}


/**
 * @brief Node에서 처리 가능한 Or Filter인지 판단한다.
 * @param[in]   aNode       Candidate Node
 * @param[in]   aOrFilter   Or Filter
 */
stlBool qlncIsUsableOrFilterOnNode( qlncNodeCommon  * aNode,
                                    qlncOrFilter    * aOrFilter )
{
    stlInt32          i;
    qlncRefNodeItem * sRefNodeItem  = NULL;
    qlncRefExprItem * sSubQueryItem = NULL;
    qlncRefExprItem * sRefExprItem  = NULL;


    for( i = 0; i < aOrFilter->mFilterCount; i++ )
    {
        if( aOrFilter->mFilters[i]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER )
        {
            /* Filter의 Reference Node들이 모두 존재하는지 판단 */
            sRefNodeItem = ((qlncBooleanFilter*)(aOrFilter->mFilters[i]))->mRefNodeList.mHead;
            while( sRefNodeItem != NULL )
            {
                STL_TRY_THROW( qlncIsExistNode( aNode, sRefNodeItem->mNode ) == STL_TRUE,
                               RAMP_FALSE );

                sRefNodeItem = sRefNodeItem->mNext;
            }

            /* SubQuery Item 설정 */
            if( ((qlncBooleanFilter*)(aOrFilter->mFilters[i]))->mSubQueryExprList != NULL )
            {
                sSubQueryItem =
                    ((qlncBooleanFilter*)(aOrFilter->mFilters[i]))->mSubQueryExprList->mHead;
            }
            else
            {
                sSubQueryItem = NULL;
            }
        }
        else
        {
            STL_DASSERT( aOrFilter->mFilters[i]->mType ==
                         QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER );

            /* SubQuery Item 설정 */
            if( ((qlncConstBooleanFilter*)(aOrFilter->mFilters[i]))->mSubQueryExprList != NULL )
            {
                sSubQueryItem =
                    ((qlncConstBooleanFilter*)(aOrFilter->mFilters[i]))->mSubQueryExprList->mHead;
            }
            else
            {
                sSubQueryItem = NULL;
            }
        }

        /* SubQuery의 Reference Column들의 Node가 모두 존재하는지 판단 */
        while( sSubQueryItem != NULL )
        {
            STL_DASSERT( sSubQueryItem->mExpr->mType == QLNC_EXPR_TYPE_SUB_QUERY );
            if( ((qlncExprSubQuery*)(sSubQueryItem->mExpr))->mRefExprList != NULL )
            {
                sRefExprItem = ((qlncExprSubQuery*)(sSubQueryItem->mExpr))->mRefExprList->mHead;
                while( sRefExprItem != NULL )
                {
                    if( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_COLUMN )
                    {
                        STL_TRY_THROW( qlncIsExistNode(
                                           aNode,
                                           ((qlncExprColumn*)(sRefExprItem->mExpr))->mNode )
                                       == STL_TRUE,
                                       RAMP_FALSE );
                    }
                    else if( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN )
                    {
                        STL_TRY_THROW( qlncIsExistNode(
                                           aNode,
                                           ((qlncExprInnerColumn*)(sRefExprItem->mExpr))->mNode )
                                       == STL_TRUE,
                                       RAMP_FALSE );
                    }
                    else
                    {
                        STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );
                        STL_TRY_THROW( qlncIsExistNode(
                                           aNode,
                                           ((qlncExprRowidColumn*)(sRefExprItem->mExpr))->mNode )
                                       == STL_TRUE,
                                       RAMP_FALSE );
                    }

                    sRefExprItem = sRefExprItem->mNext;
                }
            }

            sSubQueryItem = sSubQueryItem->mNext;
        }
    }


    return STL_TRUE;


    STL_RAMP( RAMP_FALSE );


    return STL_FALSE;
}


/**
 * @brief Push SubQuery Type을 분석하여 반환한다.
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[out]  aPushSubqType       Push Subq Type
 */
void qlncGetPushSubqType( qlncRefExprList   * aSubQueryExprList,
                          qlncPushSubqType  * aPushSubqType )
{
    qlncRefExprItem * sRefExprItem;


    STL_DASSERT( aSubQueryExprList != NULL );
    *aPushSubqType = QLNC_PUSH_SUBQ_TYPE_DEFAULT;

    sRefExprItem = aSubQueryExprList->mHead;
    while( sRefExprItem != NULL )
    {
        STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_SUB_QUERY );
        if( ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC )
        {
            switch( ((qlncQuerySpec*)(((qlncExprSubQuery*)(sRefExprItem->mExpr))->mNode))->mOtherHint->mPushSubqType )
            {
                case QLNC_PUSH_SUBQ_TYPE_DEFAULT:
                    break;

                case QLNC_PUSH_SUBQ_TYPE_YES:
                    if( *aPushSubqType == QLNC_PUSH_SUBQ_TYPE_NO )
                    {
                        *aPushSubqType = QLNC_PUSH_SUBQ_TYPE_DEFAULT;
                        STL_THROW( RAMP_FINISH );
                    }
                    else
                    {
                        *aPushSubqType = QLNC_PUSH_SUBQ_TYPE_YES;
                    }
                    break;

                case QLNC_PUSH_SUBQ_TYPE_NO:
                    if( *aPushSubqType == QLNC_PUSH_SUBQ_TYPE_YES )
                    {
                        *aPushSubqType = QLNC_PUSH_SUBQ_TYPE_DEFAULT;
                        STL_THROW( RAMP_FINISH );
                    }
                    else
                    {
                        *aPushSubqType = QLNC_PUSH_SUBQ_TYPE_NO;
                    }
                    break;

                default:
                    STL_DASSERT( 0 );
                    break;
            }
        }

        sRefExprItem = sRefExprItem->mNext;
    }


    STL_RAMP( RAMP_FINISH );
}


/**
 * @brief Nested Loops Join을 위한 Sort Instant를 만든다.
 * @param[in]   aParamInfo              Parameter Info
 * @param[in]   aSortTableAttr          Sort Table Attribute
 * @param[in]   aChildNode              Child Candidate Node
 * @param[in]   aJoinCondition          Join Condition
 * @param[in]   aPushFilterToInstant    Join Condition을 Sort Instant에 Push할지 여부
 * @param[out]  aOutJoinCondition       Instant와의 조건으로 변경한 Join Condition의 Output
 * @param[out]  aOutNode                Output Candidate Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCBOptMakeSortInstantForNestedLoops( qlncParamInfo     * aParamInfo,
                                                  smlSortTableAttr  * aSortTableAttr,
                                                  qlncNodeCommon    * aChildNode,
                                                  qlncAndFilter     * aJoinCondition,
                                                  stlBool             aPushFilterToInstant,
                                                  qlncAndFilter    ** aOutJoinCondition,
                                                  qlncNodeCommon   ** aOutNode,
                                                  qllEnv            * aEnv )
{
    stlInt32              i;
    stlInt32              sSortKeyCount;
    qlncNodeCommon      * sSortInstant          = NULL;
    qlncExprCommon     ** sLeftSortKeyArr       = NULL;
    qlncExprCommon     ** sRightSortKeyArr      = NULL;
    qlncAndFilter         sSortKeyAndFilter;
    qlncAndFilter         sNonSortKeyAndFilter;
    qlncBooleanFilter   * sBooleanFilter        = NULL;
    qlncAndFilter       * sPushKeyFilter        = NULL;

    stlUInt8            * sSortKeyFlags         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aJoinCondition != NULL) && (aJoinCondition->mOrCount > 0),
                        QLL_ERROR_STACK(aEnv) );


    /* Sort Key 및 Non Sort Key And Filter 분리 */
    STL_TRY( qlncCBOptSeperateSortKeyFromJoinCondition( aParamInfo,
                                                        aChildNode,
                                                        aJoinCondition,
                                                        &sSortKeyCount,
                                                        &sLeftSortKeyArr,
                                                        &sRightSortKeyArr,
                                                        &sSortKeyAndFilter,
                                                        &sNonSortKeyAndFilter,
                                                        aEnv )
             == STL_SUCCESS );

    if( sSortKeyCount == 0 )
    {
        sSortInstant = NULL;
        STL_THROW( RAMP_FINISH );
    }

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( stlUInt8 ) * sSortKeyCount,
                                (void**) &sSortKeyFlags,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sSortKeyFlags, 0x00, STL_SIZEOF( stlInt8 ) * sSortKeyCount );

    for( i = 0; i < sSortKeyCount; i++ )
    {
        DTL_SET_INDEX_COLUMN_FLAG( sSortKeyFlags[i],
                                   DTL_KEYCOLUMN_INDEX_ORDER_ASC,
                                   DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE,
                                   DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST );
    }

    /* Sort Instant 생성 */
    STL_TRY( qlncCreateSortInstantForMergeJoin( aParamInfo,
                                                QLNC_INSTANT_TYPE_NORMAL,
                                                aSortTableAttr,
                                                ELL_CURSOR_SCROLLABILITY_NO_SCROLL,
                                                sSortKeyCount,
                                                sRightSortKeyArr,
                                                sSortKeyFlags,
                                                aChildNode,
                                                &sSortInstant,
                                                aEnv )
             == STL_SUCCESS );

    /* Sort Key에 대한 Expression을 생성하여 Push Filter에 추가 */
    for( i = 0; i < sSortKeyCount; i++ )
    {
        qlncExprFunction    * sFunction         = NULL;

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprFunction ),
                                    (void**) &sFunction,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sBooleanFilter = (qlncBooleanFilter*)(sSortKeyAndFilter.mOrFilters[i]->mFilters[0]);
        stlMemcpy( sFunction,
                   sBooleanFilter->mExpr,
                   STL_SIZEOF( qlncExprFunction ) );

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprCommon* ) * sFunction->mArgCount,
                                    (void**) &(sFunction->mArgs),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( (sBooleanFilter->mLeftRefNodeList != NULL) &&
            qlncIsExistNode( aChildNode,
                             sBooleanFilter->mLeftRefNodeList->mHead->mNode ) )
        {
            sFunction->mArgs[1] = sLeftSortKeyArr[i];
            sFunction->mArgs[0] = sRightSortKeyArr[i];
        }
        else
        {
            sFunction->mArgs[1] = sRightSortKeyArr[i];
            sFunction->mArgs[0] = sLeftSortKeyArr[i];
        }

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncBooleanFilter ),
                                    (void**) &sBooleanFilter,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sBooleanFilter,
                   sSortKeyAndFilter.mOrFilters[i]->mFilters[0],
                   STL_SIZEOF( qlncBooleanFilter ) );

        sBooleanFilter->mExpr = (qlncExprCommon*)sFunction;

        if( sPushKeyFilter == NULL )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncAndFilter ),
                                        (void**) &sPushKeyFilter,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            QLNC_INIT_AND_FILTER( sPushKeyFilter );

            QLNC_SET_EXPR_COMMON( &sPushKeyFilter->mCommon,
                                  &QLL_CANDIDATE_MEM( aEnv ),
                                  aEnv,
                                  QLNC_EXPR_TYPE_AND_FILTER,
                                  sBooleanFilter->mExpr->mExprPhraseFlag,
                                  sBooleanFilter->mExpr->mIterationTime,
                                  sBooleanFilter->mExpr->mPosition,
                                  sBooleanFilter->mExpr->mColumnName,
                                  DTL_TYPE_BOOLEAN );
        }

        STL_TRY( qlncAppendFilterToAndFilter( aParamInfo,
                                              sPushKeyFilter,
                                              (qlncExprCommon*)sBooleanFilter,
                                              aEnv )
                 == STL_SUCCESS );
    }

    if( aPushFilterToInstant == STL_TRUE )
    {
        /* Push Filter을 Sort Instant에 추가 */
        ((qlncSortInstantNode*)sSortInstant)->mKeyFilter = sPushKeyFilter;
        if( sNonSortKeyAndFilter.mOrCount > 0 )
        {
            STL_TRY( qlncCopyAndFilter( aParamInfo,
                                        &sNonSortKeyAndFilter,
                                        &(((qlncSortInstantNode*)sSortInstant)->mRecFilter),
                                        aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            ((qlncSortInstantNode*)sSortInstant)->mRecFilter = NULL;
        }
    }
    else
    {
        /* Output Join Condition에 새로 생성된 Filter를 달아준다. */
        STL_DASSERT( aOutJoinCondition != NULL );
        STL_DASSERT( sPushKeyFilter != NULL );

        if( sNonSortKeyAndFilter.mOrCount > 0 )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        sPushKeyFilter,
                                                        sNonSortKeyAndFilter.mOrCount,
                                                        sNonSortKeyAndFilter.mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );
        }

        *aOutJoinCondition = sPushKeyFilter;
    }


    STL_RAMP( RAMP_FINISH );


    /**
     * Output 설정
     */

    *aOutNode = sSortInstant;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief In Key Range에 포함된 모든 Column들이 Index Scan Cost의 Range Filter에 사용되었는지 판단한다.
 * @param[in]   aIndexScanCost  Index Scan Cost
 */
stlBool qlncIsPossibleInKeyRange( qlncIndexScanCost * aIndexScanCost )
{
    stlInt32              i;
    stlBool               sIsExists;
    qlncExprFunction    * sFunction = NULL;


#define _QLNC_IS_EXIST_IN_KEY_RANGE_ON_AND_FILTER( _aAndFilter, _aIsExists )                    \
    {                                                                                           \
        for( i = 0; i < (_aAndFilter)->mOrCount; i++ )                                          \
        {                                                                                       \
            if( (_aAndFilter)->mOrFilters[i]->mFilterCount != 1 )                               \
            {                                                                                   \
                continue;                                                                       \
            }                                                                                   \
                                                                                                \
            if( (_aAndFilter)->mOrFilters[i]->mFilters[0]->mType                                \
                == QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER )                                        \
            {                                                                                   \
                continue;                                                                       \
            }                                                                                   \
                                                                                                \
            STL_DASSERT( (_aAndFilter)->mOrFilters[i]->mFilters[0]->mType                       \
                         == QLNC_EXPR_TYPE_BOOLEAN_FILTER );                                    \
            if( ((qlncBooleanFilter*)((_aAndFilter)->mOrFilters[i]->mFilters[0]))->mExpr->mType \
                != QLNC_EXPR_TYPE_FUNCTION )                                                    \
            {                                                                                   \
                continue;                                                                       \
            }                                                                                   \
                                                                                                \
            sFunction =                                                                         \
                (qlncExprFunction*)(((qlncBooleanFilter*)((_aAndFilter)->mOrFilters[i]->mFilters[0]))->mExpr);  \
                                                                                                \
            /* In Key Range가 변경되면 항상 column = value 형태가 된다. */                      \
            /* 참고로 i1 = 1 같은 Filter는 column = const expr 형태이다.*/                      \
            if( (sFunction->mFuncId == KNL_BUILTIN_FUNC_IS_EQUAL) &&                            \
                (sFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_COLUMN) &&                        \
                (sFunction->mArgs[1]->mType == QLNC_EXPR_TYPE_VALUE) )                          \
            {                                                                                   \
                break;                                                                          \
            }                                                                                   \
        }                                                                                       \
                                                                                                \
        if( i < aIndexScanCost->mIndexRangeAndFilter->mOrCount )                                \
        {                                                                                       \
            (_aIsExists) = STL_TRUE;                                                            \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            (_aIsExists) = STL_FALSE;                                                           \
        }                                                                                       \
    }

    STL_DASSERT( aIndexScanCost != NULL );
    STL_DASSERT( aIndexScanCost->mListColMap != NULL );


    /**
     * Index Range Filter가 반드시 존재해야 한다.
     */

    if( (aIndexScanCost->mIndexRangeAndFilter == NULL) ||
        (aIndexScanCost->mIndexRangeAndFilter->mOrCount == 0) )
    {
        return STL_FALSE;
    }


    /**
     * Index Range Filter에는 In Key Range 관련 Column으로 구성된 Filter가
     * 반드시 하나 이상 존재해야 한다.
     */

    _QLNC_IS_EXIST_IN_KEY_RANGE_ON_AND_FILTER( aIndexScanCost->mIndexRangeAndFilter,
                                               sIsExists );

    if( sIsExists == STL_FALSE )
    {
        return STL_FALSE;
    }


    /**
     * Index Key Filter에는 In Key Range 관련 Column으로 구성된 Filter가
     * 존재하면 안된다.
     */

    if( aIndexScanCost->mIndexKeyAndFilter != NULL )
    {
        _QLNC_IS_EXIST_IN_KEY_RANGE_ON_AND_FILTER( aIndexScanCost->mIndexKeyAndFilter,
                                                   sIsExists );

        if( sIsExists == STL_TRUE )
        {
            return STL_FALSE;
        }
    }


    /**
     * Table Filter에는 In Key Range 관련 Column으로 구성된 Filter가
     * 존재하면 안된다.
     */

    if( aIndexScanCost->mTableAndFilter != NULL )
    {
        _QLNC_IS_EXIST_IN_KEY_RANGE_ON_AND_FILTER( aIndexScanCost->mTableAndFilter,
                                                   sIsExists );

        if( sIsExists == STL_TRUE )
        {
            return STL_FALSE;
        }
    }


    return STL_TRUE;
}


/**
 * @brief Instant의 Rebuild가 필요한지 여부를 체크한다.
 * @param[in]   aInstantNode    Instant Node
 * @param[out]  aNeedRebuild    Rebuild가 필요한지 여부
 * @param[in]   aEnv            Environment
 */
stlStatus qlncNeedRebuildInstant( qlncNodeCommon    * aInstantNode,
                                  stlBool           * aNeedRebuild,
                                  qllEnv            * aEnv )
{
    knlRegionMark     sRegionMemMark;
    qlncNodeArray     sNodeArray;
    qlncRefExprItem * sRefExprItem  = NULL;
    stlBool           sHasOuterColumn;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aInstantNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aInstantNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT) ||
                        (aInstantNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT),
                        QLL_ERROR_STACK(aEnv) );


#define _QLNC_CHECK_HAS_OUTER_COLUMN_ON_EXPR_LIST( _aExprList, _aHasOuterColumn )   \
    {                                                                               \
        (_aHasOuterColumn) = STL_FALSE;                                             \
        if( (_aExprList) != NULL )                                                  \
        {                                                                           \
            sRefExprItem = (_aExprList)->mHead;                                     \
            while( sRefExprItem != NULL )                                           \
            {                                                                       \
                if( qlncIsExistOuterColumnOnExpr( sRefExprItem->mExpr,              \
                                                  &sNodeArray )                     \
                    == STL_TRUE )                                                   \
                {                                                                   \
                    (_aHasOuterColumn) = STL_TRUE;                                  \
                    break;                                                          \
                }                                                                   \
                                                                                    \
                sRefExprItem = sRefExprItem->mNext;                                 \
            }                                                                       \
        }                                                                           \
    }

    /* Region Memory Mark */
    KNL_INIT_REGION_MARK( &sRegionMemMark );
    STL_TRY( knlMarkRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                               &sRegionMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Node Array 초기화 */
    STL_TRY( qlncInitializeNodeArray( &QLL_CANDIDATE_MEM( aEnv ),
                                      &sNodeArray,
                                      aEnv )
             == STL_SUCCESS );

    /* Node Array 생성 */
    STL_TRY( qlncMakeNodeArrayOnCurrNode( &sNodeArray,
                                          aInstantNode,
                                          aEnv )
             == STL_SUCCESS );

    if( aInstantNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
    {
        _QLNC_CHECK_HAS_OUTER_COLUMN_ON_EXPR_LIST(
            ((qlncSortInstantNode*)aInstantNode)->mKeyColList,
            sHasOuterColumn );

        if( sHasOuterColumn == STL_FALSE )
        {
            _QLNC_CHECK_HAS_OUTER_COLUMN_ON_EXPR_LIST(
                ((qlncSortInstantNode*)aInstantNode)->mRecColList,
                sHasOuterColumn );
        }
    }
    else
    {
        STL_DASSERT( aInstantNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT );
        _QLNC_CHECK_HAS_OUTER_COLUMN_ON_EXPR_LIST(
            ((qlncHashInstantNode*)aInstantNode)->mKeyColList,
            sHasOuterColumn );

        if( sHasOuterColumn == STL_FALSE )
        {
            _QLNC_CHECK_HAS_OUTER_COLUMN_ON_EXPR_LIST(
                ((qlncHashInstantNode*)aInstantNode)->mRecColList,
                sHasOuterColumn );
        }
    }

    /* Region Memory UnMark */
    STL_TRY( knlFreeUnmarkedRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                       &sRegionMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );

    *aNeedRebuild = sHasOuterColumn;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Pre-ordered 정보를 이용한 Group By Cost를 계산한다.
 * @param[in]   aParamInfo  Parameter Info
 * @param[in]   aNode       Candidate Node
 * @param[in]   aEnv        Environment
 */
stlStatus qlncCBOptHashInstantForPreorderedGroupBy( qlncParamInfo   * aParamInfo,
                                                    qlncNodeCommon  * aNode,
                                                    qllEnv          * aEnv )
{
    qlncCBOptParamInfo    sCBOptParamInfo;
    qlncGroupCost       * sGroupCost        = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT,
                        QLL_ERROR_STACK(aEnv) );


    /* Group Cost 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncGroupCost ),
                                (void**) &sGroupCost,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* CBOptParamInfo 설정 */
    sCBOptParamInfo.mParamInfo = *aParamInfo;
    sCBOptParamInfo.mLeftNode = aNode;
    sCBOptParamInfo.mRightNode = NULL;
    sCBOptParamInfo.mJoinCond = NULL;
    sCBOptParamInfo.mFilter = NULL;
    sCBOptParamInfo.mCost = (qlncCostCommon*)sGroupCost;

    STL_TRY( qlncCBOptGroupCost( &sCBOptParamInfo,
                                 aEnv )
             == STL_SUCCESS );

    /* Best Opt 설정 */
    aNode->mBestOptType = QLNC_BEST_OPT_TYPE_GROUP;
    aNode->mBestCost = (qlncCostCommon*)(((qlncHashInstantNode*)aNode)->mGroupCost);


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index Scan Cost를 재 계산한다.
 * @param[in]   aParamInfo          Parameter Info
 * @param[in]   aBaseTableNode      Base Table Candidate Node
 * @param[in]   aIndexScanInfoIdx   Index Scan Info Index
 * @param[in]   aEnv                Environment
 */
stlStatus qlncCBOptRecomputeIndexScanCost( qlncParamInfo        * aParamInfo,
                                           qlncBaseTableNode    * aBaseTableNode,
                                           stlInt32               aIndexScanInfoIdx,
                                           qllEnv               * aEnv )
{
    qlncCBOptParamInfo    sCBOptParamInfo;

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBaseTableNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* CBOptParamInfo 설정 */
    sCBOptParamInfo.mParamInfo = *aParamInfo;
    sCBOptParamInfo.mLeftNode = (qlncNodeCommon*)aBaseTableNode;
    sCBOptParamInfo.mRightNode = NULL;
    sCBOptParamInfo.mJoinCond = NULL;
    sCBOptParamInfo.mFilter = NULL;
    sCBOptParamInfo.mCost =
        (qlncCostCommon*)&(aBaseTableNode->mIndexScanCostArr[aIndexScanInfoIdx]);

    STL_TRY( qlncCBOptIndexScanCost(
                 &sCBOptParamInfo,
                 &(aBaseTableNode->mAccPathHint->mPossibleIndexScanInfoArr[aIndexScanInfoIdx]),
                 aBaseTableNode->mAccPathHint->mPossibleIndexScanInfoArr[aIndexScanInfoIdx].mIsAsc,
                 NULL,
                 aEnv )
             == STL_SUCCESS );

    aBaseTableNode->mIndexScanCostArr[aIndexScanInfoIdx].mListColMap = NULL;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Condition으로부터 Sort Key정보를 추출한다.
 * @param[in]   aParamInfo              Parameter Info
 * @param[in]   aNode                   Candidate Node
 * @param[in]   aJoinCondition          Join Condition
 * @param[out]  aSortKeyCount           Sort Key Count
 * @param[out]  aLeftSortKeyArr         Left Sort Key Array
 * @param[out]  aRightSortKeyArr        Right Sort Key Array
 * @param[out]  aSortKeyAndFilter       Sort Key And Filter
 * @param[out]  aNonSortKeyAndFilter    Non Sort Key And Filter
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncCBOptSeperateSortKeyFromJoinCondition( qlncParamInfo      * aParamInfo,
                                                     qlncNodeCommon     * aNode,
                                                     qlncAndFilter      * aJoinCondition,
                                                     stlInt32           * aSortKeyCount,
                                                     qlncExprCommon   *** aLeftSortKeyArr,
                                                     qlncExprCommon   *** aRightSortKeyArr,
                                                     qlncAndFilter      * aSortKeyAndFilter,
                                                     qlncAndFilter      * aNonSortKeyAndFilter,
                                                     qllEnv             * aEnv )
{
    stlInt32              i;
    stlInt32              sSortKeyCount;
    qlncExprCommon     ** sLeftSortKeyArr       = NULL;
    qlncExprCommon     ** sRightSortKeyArr      = NULL;
    qlncBooleanFilter   * sBooleanFilter        = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aJoinCondition != NULL) && (aJoinCondition->mOrCount > 0),
                        QLL_ERROR_STACK(aEnv) );


    /* Sort Key 및 Non Sort Key And Filter 분리 */
    QLNC_INIT_AND_FILTER( aSortKeyAndFilter );
    QLNC_INIT_AND_FILTER( aNonSortKeyAndFilter );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * aJoinCondition->mOrCount,
                                (void**) &sLeftSortKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * aJoinCondition->mOrCount,
                                (void**) &sRightSortKeyArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sSortKeyCount = 0;
    for( i = 0; i < aJoinCondition->mOrCount; i++ )
    {
        if( (aJoinCondition->mOrFilters[i]->mFilterCount == 1) &&
            (aJoinCondition->mOrFilters[i]->mSubQueryExprList == NULL) &&
            (aJoinCondition->mOrFilters[i]->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER) &&
            (((qlncBooleanFilter*)(aJoinCondition->mOrFilters[i]->mFilters[0]))->mPossibleJoinCond == STL_TRUE) )
        {
            sBooleanFilter =
                (qlncBooleanFilter*)(aJoinCondition->mOrFilters[i]->mFilters[0]);

            if( (sBooleanFilter->mLeftRefNodeList != NULL) &&
                qlncIsExistNode( aNode, sBooleanFilter->mLeftRefNodeList->mHead->mNode ) )
            {
                if( dtlCheckKeyCompareType( ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0]->mDataType ) )
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                                aSortKeyAndFilter,
                                                                1,
                                                                &(aJoinCondition->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );

                    sLeftSortKeyArr[sSortKeyCount] =
                        ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
                    sRightSortKeyArr[sSortKeyCount] =
                        ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
                    sSortKeyCount++;
                }
                else
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                                aNonSortKeyAndFilter,
                                                                1,
                                                                &(aJoinCondition->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }
            }
            else if( (sBooleanFilter->mRightRefNodeList != NULL) &&
                     qlncIsExistNode( aNode,
                                      sBooleanFilter->mRightRefNodeList->mHead->mNode ) )
            {
                if( dtlCheckKeyCompareType( ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1]->mDataType ) )
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                                aSortKeyAndFilter,
                                                                1,
                                                                &(aJoinCondition->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );

                    sLeftSortKeyArr[sSortKeyCount] =
                        ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0];
                    sRightSortKeyArr[sSortKeyCount] =
                        ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1];
                    sSortKeyCount++;
                }
                else
                {
                    STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                                aNonSortKeyAndFilter,
                                                                1,
                                                                &(aJoinCondition->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                            aNonSortKeyAndFilter,
                                                            1,
                                                            &(aJoinCondition->mOrFilters[i]),
                                                            aEnv )
                         == STL_SUCCESS );
            }
        }
        else
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( aParamInfo,
                                                        aNonSortKeyAndFilter,
                                                        1,
                                                        &(aJoinCondition->mOrFilters[i]),
                                                        aEnv )
                     == STL_SUCCESS );
        }
    }

    STL_DASSERT( aSortKeyAndFilter->mOrCount == sSortKeyCount );


    /* output 설정 */
    *aSortKeyCount = sSortKeyCount;
    *aLeftSortKeyArr = sLeftSortKeyArr;
    *aRightSortKeyArr = sRightSortKeyArr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Inverted Nested Loops Semi Join을 위해 Unique Check가 필요한지 여부 체크
 * @param[in]   aParamInfo      Parameter Info
 * @param[in]   aNode           Candidate Node
 * @param[in]   aJoinCondition  Join Condition
 * @param[out]  aNeed           Unique Check 필요 여부
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCBOptNeedUniqueCheckForINL( qlncParamInfo     * aParamInfo,
                                          qlncNodeCommon    * aNode,
                                          qlncAndFilter     * aJoinCondition,
                                          stlBool           * aNeed,
                                          qllEnv            * aEnv )
{
    stlBool               sNeed                 = STL_TRUE;
    stlInt32              sSortKeyCount;
    qlncExprCommon     ** sLeftSortKeyArr       = NULL;
    qlncExprCommon     ** sRightSortKeyArr      = NULL;
    qlncAndFilter         sSortKeyAndFilter;
    qlncAndFilter         sNonSortKeyAndFilter;

    stlInt32              i;
    stlInt32              j;
    stlInt32              k;
    stlInt32              sIndexCount;
    qlncIndexStat       * sIndexStatArr         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aJoinCondition != NULL) && (aJoinCondition->mOrCount > 0),
                        QLL_ERROR_STACK(aEnv) );


    /* Unique Check 필요 여부 초기화 */
    sNeed = STL_TRUE;

    /* Node가 Base Table Node이어야 한다. */
    STL_TRY_THROW( aNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                   RAMP_FINISH );

    /* Sort Key 및 Non Sort Key And Filter 분리 */
    STL_TRY( qlncCBOptSeperateSortKeyFromJoinCondition( aParamInfo,
                                                        aNode,
                                                        aJoinCondition,
                                                        &sSortKeyCount,
                                                        &sLeftSortKeyArr,
                                                        &sRightSortKeyArr,
                                                        &sSortKeyAndFilter,
                                                        &sNonSortKeyAndFilter,
                                                        aEnv )
             == STL_SUCCESS );

    /* Sort Key가 모두 column이어야 한다. */
    for( i = 0; i < sSortKeyCount; i++ )
    {
        STL_TRY_THROW( sRightSortKeyArr[i]->mType == QLNC_EXPR_TYPE_COLUMN,
                       RAMP_FINISH );
    }

    /* Left Sort Key들이 unique인지 여부 체크 */
    if( sSortKeyCount == 1 )
    {
        if( ((qlncBaseTableNode*)aNode)->mTableStat->mColumnStat[ ((qlncExprColumn*)sRightSortKeyArr[0])->mColumnPos ].mIsUnique == STL_TRUE )
        {
            sNeed = STL_FALSE;
        }
    }
    else
    {
        sIndexCount = ((qlncBaseTableNode*)aNode)->mIndexCount;
        sIndexStatArr = ((qlncBaseTableNode*)aNode)->mIndexStatArr;

        for( i = 0; i < sIndexCount; i++ )
        {
            /* unique index이어야 한다. */
            if( sIndexStatArr[i].mIsUnique == STL_FALSE )
            {
                continue;
            }

            /* key 개수가 같아야 한다. */
            if( sIndexStatArr[i].mIndexKeyCount != sSortKeyCount )
            {
                continue;
            }

            /* key column이 동일해야 한다. */
            for( j = 0; j < sIndexStatArr[i].mIndexKeyCount; j++ )
            {
                for( k = 0; k < sSortKeyCount; k++ )
                {
                    if( ellGetColumnIdx( &(sIndexStatArr[i].mIndexKeyDesc[j].mKeyColumnHandle) )
                        == ((qlncExprColumn*)sRightSortKeyArr[k])->mColumnPos )
                    {
                        break;
                    }
                }

                if( k >= sSortKeyCount )
                {
                    break;
                }
            }

            /* key column이 동일한 경우 */
            if( j >= sIndexStatArr[i].mIndexKeyCount )
            {
                sNeed = STL_FALSE;
                break;
            }
        }
    }


    STL_RAMP( RAMP_FINISH );


    /* output 설정 */
    *aNeed = sNeed;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */
