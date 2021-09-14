/*******************************************************************************
 * qlncHintAnalyzer.c
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
 * @file qlncCreateCandNode.c
 * @brief SQL Processor Layer SELECT statement Hint Analyzer
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/*******************************************************
 * Candidate Plan
 *******************************************************/


/**
 * @brief Access Path Hint를 분석한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncAnalyzeAccessPathHint( qlncCreateNodeParamInfo    * aCreateNodeParamInfo,
                                     qllEnv                     * aEnv )
{
    stlInt32                  i;
    stlInt32                  sCount;

    qlvInitNode            ** sTableNodePtrArr  = NULL;
    qlncBaseTableNode       * sCandBaseTable    = NULL;
    qlvAccessPathHint       * sInitAccPathHint  = NULL;
    qlncAccPathHint         * sCandAccPathHint  = NULL;
    qlvIndexScanItem        * sIndexScanItem    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    sCandBaseTable = (qlncBaseTableNode*)(aCreateNodeParamInfo->mCandNode);


    /**
     * 현재 Access Path Hint 설정
     */

    sCandAccPathHint = sCandBaseTable->mAccPathHint;


    if( aCreateNodeParamInfo->mHintInfo == NULL )
    {
        sCandAccPathHint->mPossibleTableScan = STL_TRUE;

        if( sCandBaseTable->mIndexCount > 0 )
        {
            sCount = sCandBaseTable->mIndexCount;
            sCandAccPathHint->mPossibleIndexScanCount = sCount;
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncIndexScanInfo ) * sCount,
                                        (void**) &(sCandAccPathHint->mPossibleIndexScanInfoArr),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( i = 0; i < sCount; i++ )
            {
                sCandAccPathHint->mPossibleIndexScanInfoArr[i].mOffset = i;
                sCandAccPathHint->mPossibleIndexScanInfoArr[i].mIsAsc = STL_TRUE;
            }
            sCandAccPathHint->mPossibleIndexScan = STL_TRUE;
            sCandAccPathHint->mPossibleIndexCombine = STL_TRUE;
            sCandAccPathHint->mPossibleInKeyRange = STL_TRUE;
        }
        else
        {
            sCandAccPathHint->mPossibleIndexScanCount = 0;
            sCandAccPathHint->mPossibleIndexScanInfoArr = NULL;
            sCandAccPathHint->mPossibleIndexScan = STL_FALSE;
            sCandAccPathHint->mPossibleIndexCombine = STL_FALSE;
            sCandAccPathHint->mPossibleInKeyRange = STL_FALSE;
        }

        sCandAccPathHint->mPossibleRowidScan = STL_TRUE;

        STL_THROW( RAMP_FINISH );
    }


    /**
     * 현재 Table에 해당하는 Access Path Hint 찾기
     */

    sCount = aCreateNodeParamInfo->mHintInfo->mTableCount;
    sTableNodePtrArr = aCreateNodeParamInfo->mHintInfo->mTableNodePtrArr;
    for( i = 0; i < sCount; i++ )
    {
        if( sTableNodePtrArr[i] == aCreateNodeParamInfo->mInitNode )
        {
            sInitAccPathHint = &(aCreateNodeParamInfo->mHintInfo->mAccPathHintArr[i]);
            break;
        }
    }

    STL_DASSERT( sInitAccPathHint != NULL );


    /**
     * Access Path Hint 분석
     */

    switch( sInitAccPathHint->mScanMethod )
    {
        case QLV_HINT_SCAN_METHOD_DEFAULT:
            sCandAccPathHint->mPossibleTableScan = STL_TRUE;

            if( sCandBaseTable->mIndexCount > 0 )
            {
                sCount = sCandBaseTable->mIndexCount;
                sCandAccPathHint->mPossibleIndexScanCount = sCount;
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncIndexScanInfo ) * sCount,
                                            (void**) &(sCandAccPathHint->mPossibleIndexScanInfoArr),
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                for( i = 0; i < sCount; i++ )
                {
                    sCandAccPathHint->mPossibleIndexScanInfoArr[i].mOffset = i;
                    sCandAccPathHint->mPossibleIndexScanInfoArr[i].mIsAsc = STL_TRUE;
                }
                sCandAccPathHint->mPossibleIndexScan = STL_TRUE;
                sCandAccPathHint->mPossibleIndexCombine = STL_TRUE;
                sCandAccPathHint->mPossibleInKeyRange = STL_TRUE;
            }
            else
            {
                sCandAccPathHint->mPossibleIndexScanCount = 0;
                sCandAccPathHint->mPossibleIndexScanInfoArr = NULL;
                sCandAccPathHint->mPossibleIndexScan = STL_FALSE;
                sCandAccPathHint->mPossibleIndexCombine = STL_FALSE;
                sCandAccPathHint->mPossibleInKeyRange = STL_FALSE;
            }

            sCandAccPathHint->mPossibleRowidScan = STL_TRUE;

            break;

        case QLV_HINT_SCAN_METHOD_TABLE:
            sCandAccPathHint->mPossibleTableScan = STL_TRUE;
            sCandAccPathHint->mPossibleIndexScanCount = 0;
            sCandAccPathHint->mPossibleIndexScanInfoArr = NULL;
            sCandAccPathHint->mPossibleIndexScan = STL_FALSE;
            sCandAccPathHint->mPossibleIndexCombine = STL_FALSE;
            sCandAccPathHint->mPossibleRowidScan = STL_FALSE;
            sCandAccPathHint->mPossibleInKeyRange = STL_FALSE;

            break;

        case QLV_HINT_SCAN_METHOD_USE_INDEX:
            STL_DASSERT( sInitAccPathHint->mUseIndexScanList.mCount > 0 );

            sCount = 0;
            if( sCandBaseTable->mIndexCount > 0 )
            {
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncIndexScanInfo ) * sCandBaseTable->mIndexCount,
                                            (void**) &(sCandAccPathHint->mPossibleIndexScanInfoArr),
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sIndexScanItem = sInitAccPathHint->mUseIndexScanList.mHead;
                while( sIndexScanItem != NULL )
                {
                    for( i = 0; i < sCandBaseTable->mIndexCount; i++ )
                    {
                        if( ellGetIndexID( sCandBaseTable->mIndexStatArr[i].mIndexHandle )
                            == ellGetIndexID( &sIndexScanItem->mIndexHandle ) )
                        {
                            sCandAccPathHint->mPossibleIndexScanInfoArr[sCount].mOffset = i;

                            if( sIndexScanItem->mAsc == QLV_HINT_DEFAULT )
                            {
                                sCandAccPathHint->mPossibleIndexScanInfoArr[sCount].mIsAsc = STL_TRUE;
                            }
                            else if( sIndexScanItem->mAsc == QLV_HINT_YES )
                            {
                                sCandAccPathHint->mPossibleIndexScanInfoArr[sCount].mIsAsc = STL_TRUE;
                            }
                            else
                            {
                                STL_DASSERT( sIndexScanItem->mAsc == QLV_HINT_NO );
                                sCandAccPathHint->mPossibleIndexScanInfoArr[sCount].mIsAsc = STL_FALSE;
                            }

                            sCount++;
                            break;
                        }
                    }

                    sIndexScanItem = sIndexScanItem->mNext;
                }
            }

            if( sCount == 0 )
            {
                /* @todo Validation에 의해 여기에 들어오는 일이 없을 것으로 판단됨 */

                sCandAccPathHint->mPossibleIndexScanCount = 0;
                sCandAccPathHint->mPossibleIndexScanInfoArr = NULL;

                sCandAccPathHint->mPossibleTableScan = STL_TRUE;
                sCandAccPathHint->mPossibleIndexScan = STL_FALSE;
            }
            else
            {
                sCandAccPathHint->mPossibleIndexScanCount = sCount;

                sCandAccPathHint->mPossibleTableScan = STL_FALSE;
                sCandAccPathHint->mPossibleIndexScan = STL_TRUE;
            }
            sCandAccPathHint->mPossibleIndexCombine = STL_FALSE;
            sCandAccPathHint->mPossibleRowidScan = STL_FALSE;
            sCandAccPathHint->mPossibleInKeyRange = STL_FALSE;

            break;

        case QLV_HINT_SCAN_METHOD_INDEX_COMBINE:
            STL_DASSERT( sInitAccPathHint->mUseIndexScanList.mCount > 0 );

            sCount = 0;
            if( sCandBaseTable->mIndexCount > 0 )
            {
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncIndexScanInfo ) * sCandBaseTable->mIndexCount,
                                            (void**) &(sCandAccPathHint->mPossibleIndexScanInfoArr),
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sIndexScanItem = sInitAccPathHint->mUseIndexScanList.mHead;
                while( sIndexScanItem != NULL )
                {
                    for( i = 0; i < sCandBaseTable->mIndexCount; i++ )
                    {
                        if( ellGetIndexID( sCandBaseTable->mIndexStatArr[i].mIndexHandle )
                            == ellGetIndexID( &sIndexScanItem->mIndexHandle ) )
                        {
                            sCandAccPathHint->mPossibleIndexScanInfoArr[sCount].mOffset = i;

                            /* Validation에서 Index Combine Hint는 항상 default로 설정되어 있다. */
                            STL_DASSERT( sIndexScanItem->mAsc == QLV_HINT_DEFAULT );

                            sCandAccPathHint->mPossibleIndexScanInfoArr[sCount].mIsAsc = STL_TRUE;

                            sCount++;
                            break;
                        }
                    }

                    sIndexScanItem = sIndexScanItem->mNext;
                }
            }

            if( sCount == 0 )
            {
                /* @todo Validation에 의해 여기에 들어오는 일이 없을 것으로 판단됨 */

                sCandAccPathHint->mPossibleIndexScanCount = 0;
                sCandAccPathHint->mPossibleIndexScanInfoArr = NULL;

                sCandAccPathHint->mPossibleTableScan = STL_TRUE;
                sCandAccPathHint->mPossibleIndexCombine = STL_FALSE;
            }
            else
            {
                sCandAccPathHint->mPossibleIndexScanCount = sCount;

                sCandAccPathHint->mPossibleTableScan = STL_FALSE;
                sCandAccPathHint->mPossibleIndexCombine = STL_TRUE;
            }
            sCandAccPathHint->mPossibleIndexScan = STL_FALSE;
            sCandAccPathHint->mPossibleRowidScan = STL_FALSE;
            sCandAccPathHint->mPossibleInKeyRange = STL_FALSE;

            break;

        case QLV_HINT_SCAN_METHOD_NO_USE_INDEX:
            STL_DASSERT( sInitAccPathHint->mNoUseIndexScanList.mCount > 0 );

            sCount = 0;
            if( sCandBaseTable->mIndexCount > 0 )
            {
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncIndexScanInfo ) * sCandBaseTable->mIndexCount,
                                            (void**) &(sCandAccPathHint->mPossibleIndexScanInfoArr),
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                for( i = 0; i < sCandBaseTable->mIndexCount; i++ )
                {
                    sIndexScanItem = sInitAccPathHint->mNoUseIndexScanList.mHead;
                    while( sIndexScanItem != NULL )
                    {
                        if( ellGetIndexID( sCandBaseTable->mIndexStatArr[i].mIndexHandle )
                            == ellGetIndexID( &sIndexScanItem->mIndexHandle ) )
                        {
                            break;
                        }

                        sIndexScanItem = sIndexScanItem->mNext;
                    }

                    if( sIndexScanItem == NULL )
                    {
                        sCandAccPathHint->mPossibleIndexScanInfoArr[sCount].mOffset = i;
                        sCandAccPathHint->mPossibleIndexScanInfoArr[sCount].mIsAsc = STL_TRUE;
                        sCount++;
                    }
                }
            }

            if( sCount == 0 )
            {
                sCandAccPathHint->mPossibleIndexScanCount = 0;
                sCandAccPathHint->mPossibleIndexScanInfoArr = NULL;

                sCandAccPathHint->mPossibleIndexScan = STL_FALSE;
                sCandAccPathHint->mPossibleIndexCombine = STL_FALSE;
                sCandAccPathHint->mPossibleInKeyRange = STL_FALSE;
            }
            else
            {
                sCandAccPathHint->mPossibleIndexScanCount = sCount;

                sCandAccPathHint->mPossibleIndexScan = STL_TRUE;
                sCandAccPathHint->mPossibleIndexCombine = STL_TRUE;
                sCandAccPathHint->mPossibleInKeyRange = STL_TRUE;
            }

            sCandAccPathHint->mPossibleTableScan = STL_TRUE;
            sCandAccPathHint->mPossibleRowidScan = STL_TRUE;

            break;

        case QLV_HINT_SCAN_METHOD_ROWID:
            sCandAccPathHint->mPossibleTableScan = STL_FALSE;
            sCandAccPathHint->mPossibleIndexScanCount = 0;
            sCandAccPathHint->mPossibleIndexScanInfoArr = NULL;
            sCandAccPathHint->mPossibleIndexScan = STL_FALSE;
            sCandAccPathHint->mPossibleIndexCombine = STL_FALSE;
            sCandAccPathHint->mPossibleInKeyRange = STL_FALSE;
            sCandAccPathHint->mPossibleRowidScan = STL_TRUE;

            break;

        case QLV_HINT_SCAN_METHOD_IN_KEY_RANGE:
            STL_DASSERT( sInitAccPathHint->mUseIndexScanList.mCount > 0 );

            sCount = 0;
            if( sCandBaseTable->mIndexCount > 0 )
            {
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncIndexScanInfo ) * sCandBaseTable->mIndexCount,
                                            (void**) &(sCandAccPathHint->mPossibleIndexScanInfoArr),
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sIndexScanItem = sInitAccPathHint->mUseIndexScanList.mHead;
                while( sIndexScanItem != NULL )
                {
                    for( i = 0; i < sCandBaseTable->mIndexCount; i++ )
                    {
                        if( ellGetIndexID( sCandBaseTable->mIndexStatArr[i].mIndexHandle )
                            == ellGetIndexID( &sIndexScanItem->mIndexHandle ) )
                        {
                            sCandAccPathHint->mPossibleIndexScanInfoArr[sCount].mOffset = i;

                            /* Validation에서 In Key Range Hint는 항상 default로 설정되어 있다. */
                            STL_DASSERT( sIndexScanItem->mAsc == QLV_HINT_DEFAULT );

                            sCandAccPathHint->mPossibleIndexScanInfoArr[sCount].mIsAsc = STL_TRUE;

                            sCount++;
                            break;
                        }
                    }

                    sIndexScanItem = sIndexScanItem->mNext;
                }
            }

            if( sCount == 0 )
            {
                /* @todo Validation에 의해 여기에 들어오는 일이 없을 것으로 판단됨 */

                sCandAccPathHint->mPossibleIndexScanCount = 0;
                sCandAccPathHint->mPossibleIndexScanInfoArr = NULL;

                sCandAccPathHint->mPossibleTableScan = STL_TRUE;
                sCandAccPathHint->mPossibleInKeyRange = STL_FALSE;
            }
            else
            {
                sCandAccPathHint->mPossibleIndexScanCount = sCount;

                sCandAccPathHint->mPossibleTableScan = STL_FALSE;
                sCandAccPathHint->mPossibleInKeyRange = STL_TRUE;
            }
            sCandAccPathHint->mPossibleIndexScan = STL_FALSE;
            sCandAccPathHint->mPossibleIndexCombine = STL_FALSE;
            sCandAccPathHint->mPossibleRowidScan = STL_FALSE;

            break;

        default:
            STL_DASSERT( 0 );
            break;
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Order Hint를 분석한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncAnalyzeJoinOrderHint( qlncCreateNodeParamInfo     * aCreateNodeParamInfo,
                                    qllEnv                      * aEnv )
{
    stlInt32              i, j;
    stlInt32              sIdx;
    qlncNodeArray       * sNodeArray    = NULL;
    qlncNodeCommon      * sNode         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mHintInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /* Join Node의 Child Node Array 설정 */
    STL_DASSERT( aCreateNodeParamInfo->mCandNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE );
    sNodeArray = &(((qlncJoinTableNode*)(aCreateNodeParamInfo->mCandNode))->mNodeArray);

    /* Join Order Hint 분석 */
    sIdx = 0;
    for( i = 0; i < aCreateNodeParamInfo->mHintInfo->mJoinOrderCount; i++ )
    {
        /* 현재 Join Order Hint에 설정된 Node 찾기 */
        sNode = qlncGetCandNodePtrFromInitNodePtr(
                    aCreateNodeParamInfo->mTableMapArr,
                    aCreateNodeParamInfo->mHintInfo->mTableNodePtrArr[aCreateNodeParamInfo->mHintInfo->mJoinOrderArr[i].mJoinOrderIdx] );

        if( sNode != NULL )
        {
            STL_DASSERT( (sNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE) ||
                         (sNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) );

            /* Node Array로부터 해당 Node가 있는지 찾기 */
            for( j = 0; j < sNodeArray->mCurCnt; j++ )
            {
                if( sNodeArray->mNodeArr[j] == sNode )
                {
                    break;
                }
            }

            /* Node를 찾은 경우 */
            if( j < sNodeArray->mCurCnt )
            {
                /* Order Hint 적용 */
                if( sNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
                {
                    ((qlncBaseTableNode*)sNode)->mJoinHint->mFixedOrder =
                        aCreateNodeParamInfo->mHintInfo->mJoinOrderArr[i].mIsFixedOrder;
                    ((qlncBaseTableNode*)sNode)->mJoinHint->mFixedPos =
                        aCreateNodeParamInfo->mHintInfo->mJoinOrderArr[i].mIsFixedPosition;
                    ((qlncBaseTableNode*)sNode)->mJoinHint->mIsLeftPos =
                        aCreateNodeParamInfo->mHintInfo->mJoinOrderArr[i].mIsLeft;
                }
                else
                {
                    STL_DASSERT( sNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE );
                    ((qlncSubTableNode*)sNode)->mJoinHint->mFixedOrder =
                        aCreateNodeParamInfo->mHintInfo->mJoinOrderArr[i].mIsFixedOrder;
                    ((qlncSubTableNode*)sNode)->mJoinHint->mFixedPos =
                        aCreateNodeParamInfo->mHintInfo->mJoinOrderArr[i].mIsFixedPosition;
                    ((qlncSubTableNode*)sNode)->mJoinHint->mIsLeftPos =
                        aCreateNodeParamInfo->mHintInfo->mJoinOrderArr[i].mIsLeft;
                }

                /* 지정된 위치로 Node를 이동 */
                for( ; j > sIdx; j-- )
                {
                    sNodeArray->mNodeArr[j] = sNodeArray->mNodeArr[j-1];
                }

                sNodeArray->mNodeArr[sIdx] = sNode;
                sIdx++;
            }
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Operation Hint를 분석한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncAnalyzeJoinOperationHint( qlncCreateNodeParamInfo     * aCreateNodeParamInfo,
                                        qllEnv                      * aEnv )
{
    qlvJoinOperItem     * sJoinOperItem     = NULL;

    stlInt32              i, j;
    qlncNodeArray       * sNodeArray        = NULL;
    qlncNodeCommon      * sNode             = NULL;
    qlncJoinHint        * sJoinHint         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mHintInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /* Join Node의 Child Node Array 설정 */
    STL_DASSERT( aCreateNodeParamInfo->mCandNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE );
    sNodeArray = &(((qlncJoinTableNode*)(aCreateNodeParamInfo->mCandNode))->mNodeArray);

    /* Hint Operation 분석 */
    sJoinOperItem = aCreateNodeParamInfo->mHintInfo->mJoinOperList.mHead;
    while( sJoinOperItem != NULL )
    {
        /* Join Operation에 대한 Table Node 찾기 */
        for( i = 0; i < sJoinOperItem->mJoinTableCount; i++ )
        {
            /* 현재 Join Operation Hint에 설정된 Node 찾기 */
            sNode = qlncGetCandNodePtrFromInitNodePtr(
                        aCreateNodeParamInfo->mTableMapArr,
                        aCreateNodeParamInfo->mHintInfo->mTableNodePtrArr[sJoinOperItem->mJoinTableArr[i]] );

            if( sNode != NULL )
            {
                STL_DASSERT( (sNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE) ||
                             (sNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) );

                /* Node Array로부터 해당 Node가 있는지 찾기 */
                for( j = 0; j < sNodeArray->mCurCnt; j++ )
                {
                    if( sNodeArray->mNodeArr[j]->mNodeID == sNode->mNodeID )
                    {
                        break;
                    }
                }

                /* Node를 찾은 경우 */
                if( j < sNodeArray->mCurCnt )
                {
                    /* Operation Hint 적용 */
                    if( sNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
                    {
                        sJoinHint = ((qlncBaseTableNode*)sNode)->mJoinHint;
                    }
                    else
                    {
                        STL_DASSERT( sNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE );
                        sJoinHint = ((qlncSubTableNode*)sNode)->mJoinHint;
                    }

                    if( sJoinHint->mJoinOperation == QLNC_JOIN_OPER_TYPE_DEFAULT )
                    {
                        switch( sJoinOperItem->mJoinOperType )
                        {
                            case QLV_HINT_JOIN_OPER_TYPE_USE_NL:
                                sJoinHint->mJoinOperation = QLNC_JOIN_OPER_TYPE_USE_NESTED;
                                break;
                            case QLV_HINT_JOIN_OPER_TYPE_NO_USE_NL:
                                sJoinHint->mJoinOperation = QLNC_JOIN_OPER_TYPE_NO_USE_NESTED;
                                break;
                            case QLV_HINT_JOIN_OPER_TYPE_USE_INL:
                                sJoinHint->mJoinOperation = QLNC_JOIN_OPER_TYPE_USE_INDEX_NESTED;
                                break;
                            case QLV_HINT_JOIN_OPER_TYPE_NO_USE_INL:
                                sJoinHint->mJoinOperation = QLNC_JOIN_OPER_TYPE_NO_USE_INDEX_NESTED;
                                break;
                            case QLV_HINT_JOIN_OPER_TYPE_USE_MERGE:
                                sJoinHint->mJoinOperation = QLNC_JOIN_OPER_TYPE_USE_MERGE;
                                break;
                            case QLV_HINT_JOIN_OPER_TYPE_NO_USE_MERGE:
                                sJoinHint->mJoinOperation = QLNC_JOIN_OPER_TYPE_NO_USE_MERGE;
                                break;
                            case QLV_HINT_JOIN_OPER_TYPE_USE_HASH:
                                sJoinHint->mJoinOperation = QLNC_JOIN_OPER_TYPE_USE_HASH;
                                break;
                            case QLV_HINT_JOIN_OPER_TYPE_NO_USE_HASH:
                                sJoinHint->mJoinOperation = QLNC_JOIN_OPER_TYPE_NO_USE_HASH;
                                break;
                            default:
                                STL_DASSERT( 0 );
                                break;
                        }
                    }
                }
            }
        }

        sJoinOperItem = sJoinOperItem->mNext;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Transformation Hint를 분석한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncAnalyzeQueryTransformHint( qlncCreateNodeParamInfo    * aCreateNodeParamInfo,
                                         qllEnv                     * aEnv )
{
    qlvQueryTransformHint   * sValidateQTHint   = NULL;
    qlncQueryTransformHint  * sCandQTHint       = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mHintInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC,
                        QLL_ERROR_STACK(aEnv) );


    /* Validate Query Transformation Hint 설정 */
    sValidateQTHint = &(aCreateNodeParamInfo->mHintInfo->mQueryTransformHint);

    /* Candidate Query Transformation Hint 설정 */
    sCandQTHint = ((qlncQuerySpec*)(aCreateNodeParamInfo->mCandNode))->mQueryTransformHint;

    /* Query Transformation Type 분석 */
    switch( sValidateQTHint->mQueryTransformType )
    {
        case QLV_HINT_QUERY_TRANSFORM_TYPE_DEFAULT:
        case QLV_HINT_QUERY_TRANSFORM_TYPE_ALLOW:
            sCandQTHint->mAllowedQueryTransform = STL_TRUE;
            break;
        case QLV_HINT_QUERY_TRANSFORM_TYPE_FORBID:
            sCandQTHint->mAllowedQueryTransform = STL_FALSE;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* SubQuery Unnest Type 분석 */
    switch( sValidateQTHint->mSubQueryUnnestType )
    {
        case QLV_HINT_SUBQUERY_UNNEST_TYPE_DEFAULT:
        case QLV_HINT_SUBQUERY_UNNEST_TYPE_ALLOW:
            sCandQTHint->mAllowedUnnest = STL_TRUE;
            break;
        case QLV_HINT_SUBQUERY_UNNEST_TYPE_FORBID:
            sCandQTHint->mAllowedUnnest = STL_FALSE;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Unnest Operation Type 분석 */
    switch( sValidateQTHint->mUnnestOperType )
    {
        case QLV_HINT_UNNEST_OPER_TYPE_DEFAULT:
            sCandQTHint->mUnnestOperType = QLNC_UNNEST_OPER_TYPE_DEFAULT;
            break;
        case QLV_HINT_UNNEST_OPER_TYPE_NL_ISJ:
            sCandQTHint->mUnnestOperType = QLNC_UNNEST_OPER_TYPE_NL_ISJ;
            break;
        case QLV_HINT_UNNEST_OPER_TYPE_NL_SJ:
            sCandQTHint->mUnnestOperType = QLNC_UNNEST_OPER_TYPE_NL_SJ;
            break;
        case QLV_HINT_UNNEST_OPER_TYPE_NL_AJ:
            sCandQTHint->mUnnestOperType = QLNC_UNNEST_OPER_TYPE_NL_AJ;
            break;
        case QLV_HINT_UNNEST_OPER_TYPE_MERGE_SJ:
            sCandQTHint->mUnnestOperType = QLNC_UNNEST_OPER_TYPE_MERGE_SJ;
            break;
        case QLV_HINT_UNNEST_OPER_TYPE_MERGE_AJ:
            sCandQTHint->mUnnestOperType = QLNC_UNNEST_OPER_TYPE_MERGE_AJ;
            break;
        case QLV_HINT_UNNEST_OPER_TYPE_HASH_ISJ:
            sCandQTHint->mUnnestOperType = QLNC_UNNEST_OPER_TYPE_HASH_ISJ;
            break;
        case QLV_HINT_UNNEST_OPER_TYPE_HASH_SJ:
            sCandQTHint->mUnnestOperType = QLNC_UNNEST_OPER_TYPE_HASH_SJ;
            break;
        case QLV_HINT_UNNEST_OPER_TYPE_HASH_AJ:
            sCandQTHint->mUnnestOperType = QLNC_UNNEST_OPER_TYPE_HASH_AJ;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Other Hint를 분석한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncAnalyzeOtherHint( qlncCreateNodeParamInfo * aCreateNodeParamInfo,
                                qllEnv                  * aEnv )
{
    qlvOtherHint    * sValidateOtherHint    = NULL;
    qlncOtherHint   * sCandOtherHint        = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mHintInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC,
                        QLL_ERROR_STACK(aEnv) );


    /* Validate Other Hint 설정 */
    sValidateOtherHint = &(aCreateNodeParamInfo->mHintInfo->mOtherHint);

    /* Candidate Other Hint 설정 */
    sCandOtherHint = ((qlncQuerySpec*)(aCreateNodeParamInfo->mCandNode))->mOtherHint;

    /* Push Subq Type 분석 */
    switch( sValidateOtherHint->mPushSubq )
    {
        case QLV_HINT_DEFAULT:
            sCandOtherHint->mPushSubqType = QLNC_PUSH_SUBQ_TYPE_DEFAULT;
            break;
        case QLV_HINT_YES:
            sCandOtherHint->mPushSubqType = QLNC_PUSH_SUBQ_TYPE_YES;
            break;
        case QLV_HINT_NO:
            sCandOtherHint->mPushSubqType = QLNC_PUSH_SUBQ_TYPE_NO;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Distinct Operation Type 분석 */
    switch( sValidateOtherHint->mUseDistinctHash )
    {
        case QLV_HINT_DEFAULT:
            sCandOtherHint->mDistinctOperType = QLNC_DISTINCT_OPER_TYPE_DEFAULT;
            break;
        case QLV_HINT_YES:
            sCandOtherHint->mDistinctOperType = QLNC_DISTINCT_OPER_TYPE_HASH;
            break;
        case QLV_HINT_NO:
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Group Operation Type 분석 */
    switch( sValidateOtherHint->mUseGroupHash )
    {
        case QLV_HINT_DEFAULT:
            sCandOtherHint->mGroupOperType = QLNC_GROUP_OPER_TYPE_DEFAULT;
            break;
        case QLV_HINT_YES:
            sCandOtherHint->mGroupOperType = QLNC_GROUP_OPER_TYPE_HASH;
            break;
        case QLV_HINT_NO:
        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */
