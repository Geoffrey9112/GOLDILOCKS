/*******************************************************************************
 * qlncMakeCandPlan.c
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
 * @file qlncMakeCandPlan.c
 * @brief SQL Processor Layer Make Candidate Plan
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Plan Node Map Dynamic Array를 생성한다.
 * @param[in]   aRegionMem          Region Memory
 * @param[out]  aPlanNodeMapDynArr  Plan Node Map Dynamic Array
 * @param[in]   aEnv                Environment
 */
stlStatus qlncCreatePlanNodeMapDynArray( knlRegionMem               * aRegionMem,
                                         qlncPlanNodeMapDynArray   ** aPlanNodeMapDynArr,
                                         qllEnv                     * aEnv )
{
    qlncPlanNodeMapDynArray * sPlanNodeMapDynArr    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /* Plan Node Map Dynamic Array 생성 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlncPlanNodeMapDynArray ),
                                (void**) &sPlanNodeMapDynArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Region Memory 설정 */
    sPlanNodeMapDynArr->mRegionMem = aRegionMem;

    /* Plan Node Map 개수 설정 */
    sPlanNodeMapDynArr->mCurCnt = 0;
    sPlanNodeMapDynArr->mMaxCnt = QLNC_PLAN_NODE_MAP_ARRAY_DEFAULT_SIZE;

    /* Plan Node Map 할당 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlncPlanCommon* ) * QLNC_PLAN_NODE_MAP_ARRAY_DEFAULT_SIZE,
                                (void**) &(sPlanNodeMapDynArr->mPlanNodeArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Output 설정
     */

    *aPlanNodeMapDynArr = sPlanNodeMapDynArr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Plan Node Map Dynamic Array에 Plan Node Map 정보를 Tail 부분에 추가한다.
 * @param[in,out]   aPlanNodeMapDynArr  Plan Node Map Dynamic Array
 * @param[in]       aPlanNode           Candidate Plan Node
 * @param[in]       aEnv                Environment
 */
stlStatus qlncAddTailPlanNodeMapDynArray( qlncPlanNodeMapDynArray   * aPlanNodeMapDynArr,
                                          qlncPlanCommon            * aPlanNode,
                                          qllEnv                    * aEnv )
{
    stlInt32          sNewCount;
    qlncPlanCommon ** sNewPlanNodeArr   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aPlanNodeMapDynArr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Size 체크 */
    if( aPlanNodeMapDynArr->mCurCnt == aPlanNodeMapDynArr->mMaxCnt )
    {
        sNewCount = aPlanNodeMapDynArr->mMaxCnt + QLNC_PLAN_NODE_MAP_ARRAY_INCREMENT_SIZE;

        /* 새로운 Plan Node Map Array 할당 */
        STL_TRY( knlAllocRegionMem( aPlanNodeMapDynArr->mRegionMem,
                                    STL_SIZEOF( qlncPlanCommon* ) * sNewCount,
                                    (void**) &sNewPlanNodeArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* 기존 Plan Node Array 정보 복사 */
        stlMemcpy( sNewPlanNodeArr,
                   aPlanNodeMapDynArr->mPlanNodeArr,
                   STL_SIZEOF( qlncPlanCommon* ) * aPlanNodeMapDynArr->mCurCnt );

        /* Max Count 조정 */
        aPlanNodeMapDynArr->mMaxCnt = sNewCount;

        /* 기존 Plan Node 정보 교체 */
        aPlanNodeMapDynArr->mPlanNodeArr = sNewPlanNodeArr;
    }

    STL_DASSERT( aPlanNodeMapDynArr->mCurCnt < aPlanNodeMapDynArr->mMaxCnt );

    /* Plan Node Map 추가 */
    aPlanNodeMapDynArr->mPlanNodeArr[aPlanNodeMapDynArr->mCurCnt] = aPlanNode;
    aPlanNodeMapDynArr->mCurCnt++;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Plan Node Map Dynamic Array에서 Tail Plan Node를 삭제한다.
 * @param[in,out]   aPlanNodeMapDynArr  Plan Node Map Dynamic Array
 * @param[in]       aDelCount           Delete Count
 * @param[in]       aEnv                Environment
 */
stlStatus qlncDelTailPlanNodeMapDynArray( qlncPlanNodeMapDynArray   * aPlanNodeMapDynArr,
                                          stlInt32                    aDelCount,
                                          qllEnv                    * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aPlanNodeMapDynArr != NULL, QLL_ERROR_STACK(aEnv) );


    /* Plan Node가 하나라도 존재하는 경우에만 삭제한다. */
    if( aPlanNodeMapDynArr->mCurCnt > aDelCount )
    {
        aPlanNodeMapDynArr->mCurCnt -= aDelCount;
    }
    else
    {
        aPlanNodeMapDynArr->mCurCnt = 0;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Plan Node Map Dynamic Array에서 Candidate Node에 해당하는 Candidate Plan Node를 찾는다.
 * @param[in]   aPlanNodeMapDynArr  Plan Node Map Dynamic Array
 * @param[in]   aNode               Candidate Node
 * @param[out]  aFoundPlanNode      Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncFindPlanNodeFromPlanNodeMapDynArray( qlncPlanNodeMapDynArray  * aPlanNodeMapDynArr,
                                                   qlncNodeCommon           * aNode,
                                                   qlncPlanCommon          ** aFoundPlanNode,
                                                   qllEnv                   * aEnv )
{
    stlInt32          i;
    qlncPlanCommon  * sPlanNode     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aPlanNodeMapDynArr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Plan Node를 반드시 뒤에서부터 찾는다.
     * 이유: map에 plan node를 추가할때 뒤에 추가하므로
     * 가장 가까운 노드부터 찾기 위해서는 뒤에서부터 찾아야 한다. */
    sPlanNode = NULL;
    for( i = aPlanNodeMapDynArr->mCurCnt - 1; i >= 0;  i-- )
    {
        if( qlncIsExistCandNodeOnPlanNode( aPlanNodeMapDynArr->mPlanNodeArr[i], aNode ) == STL_TRUE )
        {
            sPlanNode = aPlanNodeMapDynArr->mPlanNodeArr[i];
            break;
        }
    }


    /**
     * Output 설정
     */

    *aFoundPlanNode = sPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Plan Node로부터 Outer Column List를 찾는다.
 * @param[in]   aPlanNode           Candidate Plan Node
 * @param[out]  aOuterColumnList    Outer Column List
 */
void qlncFindOuterColumnListFromPlanNode( qlncPlanCommon    * aPlanNode,
                                          qlvRefExprList   ** aOuterColumnList )
{
    switch( aPlanNode->mPlanType )
    {
        case QLNC_PLAN_TYPE_TABLE_SCAN:
        case QLNC_PLAN_TYPE_INDEX_SCAN:
        case QLNC_PLAN_TYPE_ROWID_SCAN:
            *aOuterColumnList = ((qlvInitBaseTableNode*)(aPlanNode->mInitNode))->mOuterColumnList;
            break;

        case QLNC_PLAN_TYPE_INDEX_COMBINE:
            *aOuterColumnList = ((qlncPlanIndexCombine*)aPlanNode)->mOuterColumnList;
            break;

        case QLNC_PLAN_TYPE_SUB_TABLE:
            *aOuterColumnList = ((qlvInitSubTableNode*)(aPlanNode->mInitNode))->mOuterColumnList;
            break;

        case QLNC_PLAN_TYPE_NESTED_JOIN:
        case QLNC_PLAN_TYPE_MERGE_JOIN:
        case QLNC_PLAN_TYPE_HASH_JOIN:
            *aOuterColumnList = ((qlvInitJoinTableNode*)(aPlanNode->mInitNode))->mOuterColumnList;
            break;

        case QLNC_PLAN_TYPE_JOIN_COMBINE:
            *aOuterColumnList = ((qlncPlanJoinCombine*)aPlanNode)->mOuterColumnList;
            break;

        case QLNC_PLAN_TYPE_SORT_INSTANT:
        case QLNC_PLAN_TYPE_SORT_GROUP_INSTANT:
        case QLNC_PLAN_TYPE_SORT_JOIN_INSTANT:
        case QLNC_PLAN_TYPE_HASH_INSTANT:
        case QLNC_PLAN_TYPE_HASH_GROUP_INSTANT:
        case QLNC_PLAN_TYPE_HASH_JOIN_INSTANT:
        case QLNC_PLAN_TYPE_GROUP:
            *aOuterColumnList = ((qlvInitInstantNode*)(aPlanNode->mInitNode))->mOuterColumnList;
            break;

        case QLNC_PLAN_TYPE_SUB_QUERY_LIST:
            (void)qlncFindOuterColumnListFromPlanNode(
                      ((qlncPlanSubQueryList*)aPlanNode)->mChildPlanNode,
                      aOuterColumnList );
            break;

        case QLNC_PLAN_TYPE_SUB_QUERY_FILTER:
            (void)qlncFindOuterColumnListFromPlanNode(
                      ((qlncPlanSubQueryFilter*)aPlanNode)->mChildPlanNodes[ 0 ],
                      aOuterColumnList );
            break;

        /* case QLNC_PLAN_TYPE_QUERY_SET: */
        /* case QLNC_PLAN_TYPE_SET_OP: */
        /* case QLNC_PLAN_TYPE_QUERY_SPEC: */
        default:
            STL_DASSERT( 0 );
            *aOuterColumnList = NULL;
            break;
    }
}


/////////////////////////////////////////////////////////////////////////////

/**
 * @brief Candidate Plan을 생성한다.
 * @param[in]   aTransID            Transaction ID
 * @param[in]   aDBCStmt            DBC Statement
 * @param[in]   aSQLStmt            SQL Statement
 * @param[in]   aInitStmtExprList   Init Statement Expression List
 * @param[in]   aTableStatList      Table Stat List
 * @param[in]   aCandNode           Candidate Node
 * @param[in]   aRootPlan           Root Candidate Plan
 * @param[out]  aCandPlan           Candidate Plan
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlan( smlTransId            aTransID,
                            qllDBCStmt          * aDBCStmt,
                            qllStatement        * aSQLStmt,
                            qlvInitStmtExprList * aInitStmtExprList,
                            qloValidTblStatList * aTableStatList,
                            qlncNodeCommon      * aCandNode,
                            qlncPlanCommon      * aRootPlan,
                            qlncPlanCommon     ** aCandPlan,
                            qllEnv              * aEnv )
{
    qlncPlanBasicParamInfo    sPlanBasicParamInfo;
    qlncPlanDetailParamInfo   sPlanDetailParamInfo;
    qlncPlanNodeMapDynArray * sPlanNodeMapDynArr    = NULL;
    qlncPlanCommon          * sPlanNode             = NULL;
    qlvInitExprList           sInitExprList;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitStmtExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableStatList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Plan Node Map Dynamic Array 생성 */
    STL_TRY( qlncCreatePlanNodeMapDynArray( &QLL_CANDIDATE_MEM( aEnv ),
                                            &sPlanNodeMapDynArr,
                                            aEnv )
             == STL_SUCCESS );

    /* Candidate Plan Basic Parameter Info 설정 */
    sPlanBasicParamInfo.mRegionMem = QLL_CODE_PLAN( aDBCStmt );
    sPlanBasicParamInfo.mCandidateMem = &QLL_CANDIDATE_MEM( aEnv );
    sPlanBasicParamInfo.mPlanNodeMapDynArr = sPlanNodeMapDynArr;
    sPlanBasicParamInfo.mRootQuerySetPlanNode = NULL;
    sPlanBasicParamInfo.mInitQuerySetNode = NULL;
    sPlanBasicParamInfo.mNeedRowBlock = QLCR_NEED_TO_SET_ROWBLOCK( aSQLStmt );

    /* Basic Candidate Plan Node 생성 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ aCandNode->mBestOptType ](
                 &sPlanBasicParamInfo,
                 aCandNode,
                 &sPlanNode,
                 aEnv )
             == STL_SUCCESS );

    /* Init Expression List 설정 */
    sInitExprList.mStmtExprList = aInitStmtExprList;
    sInitExprList.mPseudoColExprList = NULL;
    sInitExprList.mAggrExprList = NULL;
    sInitExprList.mNestedAggrExprList = NULL;

    /* Candidate Plan Detail Parameter Info 설정 */
    sPlanDetailParamInfo.mParamInfo.mTransID = aTransID;
    sPlanDetailParamInfo.mParamInfo.mDBCStmt = aDBCStmt;
    sPlanDetailParamInfo.mParamInfo.mSQLStmt = aSQLStmt;
    sPlanDetailParamInfo.mRegionMem = QLL_CODE_PLAN( aDBCStmt );
    sPlanDetailParamInfo.mCandidateMem = &QLL_CANDIDATE_MEM( aEnv );
    sPlanDetailParamInfo.mPlanNodeMapDynArr = sPlanNodeMapDynArr;
    sPlanDetailParamInfo.mInitExprList = &sInitExprList;
    sPlanDetailParamInfo.mTableStatList = aTableStatList;
    sPlanDetailParamInfo.mJoinOuterInfoList.mCount = 0;
    sPlanDetailParamInfo.mJoinOuterInfoList.mHead = NULL;

    /* Detail Candidate Plan Node 생성 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanNode->mPlanType ](
                 &sPlanDetailParamInfo,
                 sPlanNode,
                 &sPlanNode,
                 aEnv )
             == STL_SUCCESS );

    /* Output 설정 */
    *aCandPlan = sPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Insert를 위한 Candidate Plan을 생성한다.
 * @param[in]       aTransID                    Transaction ID
 * @param[in]       aDBCStmt                    DBC Statement
 * @param[in]       aSQLStmt                    SQL Statement
 * @param[in]       aQueryExprList              Query Expression List
 * @param[in]       aTableStatList              Table Stat List
 * @param[in,out]   aPlanInsertStmtParamInfo    Insert Statement Parameter Info
 * @param[in]       aEnv                        Environment
 */
stlStatus qlncMakeCandPlanForInsert( smlTransId                       aTransID,
                                     qllDBCStmt                     * aDBCStmt,
                                     qllStatement                   * aSQLStmt,
                                     qloInitExprList                * aQueryExprList,
                                     qloValidTblStatList            * aTableStatList,
                                     qlncPlanInsertStmtParamInfo    * aPlanInsertStmtParamInfo,
                                     qllEnv                         * aEnv )
{
    stlInt32                  i;
    stlInt32                  j;
    qlncPlanBasicParamInfo    sPlanBasicParamInfo;
    qlncPlanDetailParamInfo   sPlanDetailParamInfo;
    qlncPlanNodeMapDynArray * sPlanNodeMapDynArr        = NULL;
    qlncPlanCommon          * sPlanWriteTable           = NULL;
    qlncPlanCommon          * sPlanRowSubQueryList      = NULL;
    qlncPlanCommon          * sPlanReturnSubQueryList   = NULL;
    qlncConvExprParamInfo     sConvExprParamInfo;
    qlvRefExprList            sTmpSubQueryExprList;
    qlvRefExprList            sSubQueryExprList;
    qlncRefExprItem         * sRefCandExprItem          = NULL;

    qlvInitExpression      ** sRowDefaultExprArr        = NULL;
    qlvRefExprList          * sInsertColExprList        = NULL;
    qlvRefExprList          * sReturnOuterColumnList    = NULL;
    qlvRefExprItem          * sRefInitExprItem          = NULL;
    qlvInitSingleRow        * sRowList                  = NULL;
    qlvInitSingleRow        * sSingleRow                = NULL;
    qlvInitTarget           * sReturnTargetArray        = NULL;

    qloValidTblStatList       sTmpTableStatList;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanInsertStmtParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* Plan Node Map Dynamic Array 생성 */
    STL_TRY( qlncCreatePlanNodeMapDynArray( &QLL_CANDIDATE_MEM( aEnv ),
                                            &sPlanNodeMapDynArr,
                                            aEnv )
             == STL_SUCCESS );


    /**
     * Write Table Node 처리
     */

    /* Candidate Plan Basic Parameter Info 설정 */
    sPlanBasicParamInfo.mRegionMem = QLL_CODE_PLAN( aDBCStmt );
    sPlanBasicParamInfo.mCandidateMem = &QLL_CANDIDATE_MEM( aEnv );
    sPlanBasicParamInfo.mPlanNodeMapDynArr = sPlanNodeMapDynArr;
    sPlanBasicParamInfo.mRootQuerySetPlanNode = NULL;
    sPlanBasicParamInfo.mInitQuerySetNode = NULL;
    sPlanBasicParamInfo.mNeedRowBlock = QLCR_NEED_TO_SET_ROWBLOCK( aSQLStmt );

    /* Basic Candidate Plan Node 생성 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ aPlanInsertStmtParamInfo->mWriteTableNode->mBestOptType ](
                 &sPlanBasicParamInfo,
                 aPlanInsertStmtParamInfo->mWriteTableNode,
                 &sPlanWriteTable,
                 aEnv )
             == STL_SUCCESS );

    /* Candidate Plan Detail Parameter Info 설정 */
    sTmpTableStatList.mCount = 0;
    sTmpTableStatList.mHead = NULL;
    sTmpTableStatList.mTail = NULL;
    sPlanDetailParamInfo.mParamInfo.mTransID = aTransID;
    sPlanDetailParamInfo.mParamInfo.mDBCStmt = aDBCStmt;
    sPlanDetailParamInfo.mParamInfo.mSQLStmt = aSQLStmt;
    sPlanDetailParamInfo.mRegionMem = QLL_CODE_PLAN( aDBCStmt );
    sPlanDetailParamInfo.mCandidateMem = &QLL_CANDIDATE_MEM( aEnv );
    sPlanDetailParamInfo.mPlanNodeMapDynArr = sPlanNodeMapDynArr;
    sPlanDetailParamInfo.mInitExprList = aQueryExprList->mInitExprList;
    sPlanDetailParamInfo.mTableStatList = &sTmpTableStatList;
    sPlanDetailParamInfo.mJoinOuterInfoList.mCount = 0;
    sPlanDetailParamInfo.mJoinOuterInfoList.mHead = NULL;

    /* Detail Candidate Plan Node 생성 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanWriteTable->mPlanType ](
                 &sPlanDetailParamInfo,
                 sPlanWriteTable,
                 &sPlanWriteTable,
                 aEnv )
             == STL_SUCCESS );

    /* Write Column List 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    sRefCandExprItem = aPlanInsertStmtParamInfo->mWriteColumnList->mHead;
    while( sRefCandExprItem != NULL )
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(sPlanDetailParamInfo.mParamInfo),
            QLL_CODE_PLAN( aDBCStmt ),
            &QLL_CANDIDATE_MEM( aEnv ),
            aQueryExprList->mInitExprList,
            aTableStatList,
            0,
            NULL,
            0,
            NULL,
            sPlanWriteTable,
            sPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            sRefCandExprItem->mExpr );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[ sRefCandExprItem->mExpr->mType ](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        sRefCandExprItem = sRefCandExprItem->mNext;
    }

    /* Insert의 경우 Validation Node로부터 Reference Column을 참조하므로 이에 대한 처리를 해준다. */
    switch( sPlanWriteTable->mPlanType )
    {
        case QLNC_PLAN_TYPE_TABLE_SCAN:
            ((qlvInitBaseTableNode*)(sPlanWriteTable->mInitNode))->mRefColumnList =
                ((qlncPlanTableScan*)sPlanWriteTable)->mReadColList;
            ((qlvInitBaseTableNode*)(sPlanWriteTable->mInitNode))->mRefRowIdColumn =
                ((qlncPlanTableScan*)sPlanWriteTable)->mRowidColumn;
            break;
        case QLNC_PLAN_TYPE_INDEX_SCAN:
            ((qlvInitBaseTableNode*)(sPlanWriteTable->mInitNode))->mRefColumnList =
                ((qlncPlanIndexScan*)sPlanWriteTable)->mReadColList;
            ((qlvInitBaseTableNode*)(sPlanWriteTable->mInitNode))->mRefRowIdColumn =
                ((qlncPlanIndexScan*)sPlanWriteTable)->mRowidColumn;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Row Default Expression Array 설정 */
    if( aPlanInsertStmtParamInfo->mRowDefaultExprArr != NULL )
    {
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlvInitExpression* ) * aPlanInsertStmtParamInfo->mInsertColumnCount,
                                    (void**) &sRowDefaultExprArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_INIT_LIST( &sTmpSubQueryExprList );
        for( i = 0; i < aPlanInsertStmtParamInfo->mInsertColumnCount; i++ )
        {
            if( aPlanInsertStmtParamInfo->mRowDefaultExprArr[i] != NULL )
            {
                QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                    &sConvExprParamInfo,
                    &(sPlanDetailParamInfo.mParamInfo),
                    QLL_CODE_PLAN( aDBCStmt ),
                    &QLL_CANDIDATE_MEM( aEnv ),
                    aQueryExprList->mInitExprList,
                    aTableStatList,
                    0,
                    NULL,
                    0,
                    NULL,
                    sPlanWriteTable,
                    sPlanNodeMapDynArr,
                    &sTmpSubQueryExprList,
                    STL_TRUE,
                    aPlanInsertStmtParamInfo->mRowDefaultExprArr[i] );

                STL_TRY( qlncConvertCandExprToInitExprFuncList[ aPlanInsertStmtParamInfo->mRowDefaultExprArr[i]->mType ](
                             &sConvExprParamInfo,
                             aEnv )
                         == STL_SUCCESS );

                sRowDefaultExprArr[i] = sConvExprParamInfo.mInitExpr;
            }
            else
            {
                sRowDefaultExprArr[i] = NULL;
            }
        }
    }
    else
    {
        sRowDefaultExprArr = NULL;
    }

    /* Insert Column Expression List 설정 */
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvRefExprList ),
                                (void**) &sInsertColExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_LIST( sInsertColExprList );
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    sRefCandExprItem = aPlanInsertStmtParamInfo->mInsertColExprList->mHead;
    while( sRefCandExprItem != NULL )
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(sPlanDetailParamInfo.mParamInfo),
            QLL_CODE_PLAN( aDBCStmt ),
            &QLL_CANDIDATE_MEM( aEnv ),
            aQueryExprList->mInitExprList,
            aTableStatList,
            0,
            NULL,
            0,
            NULL,
            sPlanWriteTable,
            sPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            sRefCandExprItem->mExpr );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[ sRefCandExprItem->mExpr->mType ](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlvRefExprItem ),
                                    (void**) &sRefInitExprItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRefInitExprItem->mExprPtr = sConvExprParamInfo.mInitExpr;
        sRefInitExprItem->mIsLeft = STL_TRUE;
        sRefInitExprItem->mNext = NULL;

        QLNC_APPEND_ITEM_TO_LIST( sInsertColExprList, sRefInitExprItem );

        sRefCandExprItem = sRefCandExprItem->mNext;
    }

    /* Row Value Array 설정 */
    if( aPlanInsertStmtParamInfo->mRowValueArray != NULL )
    {
        sRowList = NULL;
        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlvInitSingleRow ) * aPlanInsertStmtParamInfo->mMultiValueCount,
                                    (void**) &sRowList,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_LIST( &sSubQueryExprList );
        sSingleRow = sRowList;
        for( i = 0; i < aPlanInsertStmtParamInfo->mMultiValueCount; i++ )
        {
            QLNC_INIT_LIST( &sTmpSubQueryExprList );
            STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                        STL_SIZEOF( qlvInitExpression* ) * aPlanInsertStmtParamInfo->mInsertColumnCount,
                                        (void**) &(sSingleRow->mValueExpr),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( j = 0; j < aPlanInsertStmtParamInfo->mInsertColumnCount; j++ )
            {
                QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                    &sConvExprParamInfo,
                    &(sPlanDetailParamInfo.mParamInfo),
                    QLL_CODE_PLAN( aDBCStmt ),
                    &QLL_CANDIDATE_MEM( aEnv ),
                    aQueryExprList->mInitExprList,
                    aTableStatList,
                    0,
                    NULL,
                    0,
                    NULL,
                    sPlanWriteTable,
                    sPlanNodeMapDynArr,
                    &sTmpSubQueryExprList,
                    STL_TRUE,
                    aPlanInsertStmtParamInfo->mRowValueArray[i][j] );

                STL_TRY( qlncConvertCandExprToInitExprFuncList[ aPlanInsertStmtParamInfo->mRowValueArray[i][j]->mType ](
                             &sConvExprParamInfo,
                             aEnv )
                         == STL_SUCCESS );

                sSingleRow->mValueExpr[j] = sConvExprParamInfo.mInitExpr;
            }

            /* SubQuery Expression List가 존재하는지 판별 */
            if( sTmpSubQueryExprList.mCount > 0 )
            {
                if( sSubQueryExprList.mCount == 0 )
                {
                    sSubQueryExprList.mCount = sTmpSubQueryExprList.mCount;
                    sSubQueryExprList.mHead = sTmpSubQueryExprList.mHead;
                    sSubQueryExprList.mTail = sTmpSubQueryExprList.mTail;
                }
                else
                {
                    sSubQueryExprList.mCount += sTmpSubQueryExprList.mCount;
                    sSubQueryExprList.mTail->mNext = sTmpSubQueryExprList.mHead;
                    sSubQueryExprList.mTail = sTmpSubQueryExprList.mTail;
                }
            }

            if( (i + 1) < aPlanInsertStmtParamInfo->mMultiValueCount )
            {
                sSingleRow->mNext = &sRowList[i+1];
            }
            else
            {
                sSingleRow->mNext = NULL;
            }

            sSingleRow = sSingleRow->mNext;
        }

        /* SubQuery Expression List가 존재하는지 판별 */
        if( sSubQueryExprList.mCount > 0 )
        {
            STL_TRY( qlncMakeCandPlanSubQueryList( QLL_CODE_PLAN( aDBCStmt ),
                                                   &QLL_CANDIDATE_MEM( aEnv ),
                                                   NULL,
                                                   &sSubQueryExprList,
                                                   &sPlanRowSubQueryList,
                                                   aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sPlanRowSubQueryList = NULL;
        }
    }
    else
    {
        sRowList = NULL;
        sPlanRowSubQueryList = NULL;
    }

    /* Return Target 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( aPlanInsertStmtParamInfo->mReturnTargetCount > 0 )
    {
        /* Plan Node Map Dynamic Array에 Write Table Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( sPlanNodeMapDynArr,
                                                 sPlanWriteTable,
                                                 aEnv )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlvInitTarget ) * aPlanInsertStmtParamInfo->mReturnTargetCount,
                                    (void**) &sReturnTargetArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( i = 0; i < aPlanInsertStmtParamInfo->mReturnTargetCount; i++ )
        {
            sReturnTargetArray[i].mDisplayPos =
                aPlanInsertStmtParamInfo->mReturnTargetArray[i].mDisplayPos;

            QLNC_ALLOC_AND_COPY_STRING( sReturnTargetArray[i].mDisplayName,
                                        aPlanInsertStmtParamInfo->mReturnTargetArray[i].mDisplayName,
                                        QLL_CODE_PLAN( aDBCStmt ),
                                        aEnv );

            QLNC_ALLOC_AND_COPY_STRING( sReturnTargetArray[i].mAliasName,
                                        aPlanInsertStmtParamInfo->mReturnTargetArray[i].mAliasName,
                                        QLL_CODE_PLAN( aDBCStmt ),
                                        aEnv );

            QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                &sConvExprParamInfo,
                &(sPlanDetailParamInfo.mParamInfo),
                QLL_CODE_PLAN( aDBCStmt ),
                &QLL_CANDIDATE_MEM( aEnv ),
                aQueryExprList->mInitExprList,
                aTableStatList,
                0,
                NULL,
                0,
                NULL,
                sPlanWriteTable,
                sPlanNodeMapDynArr,
                &sTmpSubQueryExprList,
                STL_TRUE,
                aPlanInsertStmtParamInfo->mReturnTargetArray[i].mExpr );

            STL_TRY( qlncConvertCandExprToInitExprFuncList[ aPlanInsertStmtParamInfo->mReturnTargetArray[i].mExpr->mType ](
                         &sConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );

            sReturnTargetArray[i].mExpr = sConvExprParamInfo.mInitExpr;

            if( aSQLStmt->mBindCount > 0 )
            {
                sReturnTargetArray[i].mDataTypeInfo =
                    aPlanInsertStmtParamInfo->mReturnTargetArray[i].mDataTypeInfo;
            }
            else
            {
                STL_TRY( qlvGetExprDataTypeInfo( aSQLStmt->mRetrySQL,
                                                 &QLL_CANDIDATE_MEM( aEnv ),
                                                 sReturnTargetArray[i].mExpr,
                                                 aSQLStmt->mBindContext,
                                                 & sReturnTargetArray[i].mDataTypeInfo,
                                                 aEnv )
                         == STL_SUCCESS );
            }
        }

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( sPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sReturnTargetArray = NULL;
    }

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sTmpSubQueryExprList.mCount > 0 )
    {
        STL_TRY( qlncMakeCandPlanSubQueryList( QLL_CODE_PLAN( aDBCStmt ),
                                               &QLL_CANDIDATE_MEM( aEnv ),
                                               NULL,
                                               &sTmpSubQueryExprList,
                                               &sPlanReturnSubQueryList,
                                               aEnv )
                 == STL_SUCCESS );

        /* SubQuery가 참조하는 Outer Column을 찾아 Column의 Relation Node의 Pointer를 변경 */
        qlncFindOuterColumnListFromPlanNode( sPlanWriteTable,
                                             &sReturnOuterColumnList );
    }
    else
    {
        sPlanReturnSubQueryList = NULL;
        sReturnOuterColumnList = NULL;
    }

    /* Output 설정 */
    aPlanInsertStmtParamInfo->mOutReturnTargetArray = sReturnTargetArray;
    aPlanInsertStmtParamInfo->mOutReturnOuterColumnList = sReturnOuterColumnList;
    aPlanInsertStmtParamInfo->mOutRowDefaultExprArr = sRowDefaultExprArr;
    aPlanInsertStmtParamInfo->mOutRowList = sRowList;
    aPlanInsertStmtParamInfo->mOutInsertColExprList = sInsertColExprList;
    aPlanInsertStmtParamInfo->mOutPlanWriteTable = sPlanWriteTable;
    aPlanInsertStmtParamInfo->mOutPlanRowSubQueryList = sPlanRowSubQueryList;
    aPlanInsertStmtParamInfo->mOutPlanReturnSubQueryList = sPlanReturnSubQueryList;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Delete를 위한 Candidate Plan을 생성한다.
 * @param[in]   aTransID                Transaction ID
 * @param[in]   aDBCStmt                DBC Statement
 * @param[in]   aSQLStmt                SQL Statement
 * @param[in]   aQueryExprList          Query Expression List
 * @param[in]   aTableStatList          Table Stat List
 * @param[in]   aCandNode               Candidate Node
 * @param[in]   aReturnTargetCount      Return Target Count
 * @param[in]   aReturnTargetArray      Return Target Array
 * @param[in]   aReadColumnList         Read Column List
 * @param[out]  aInitReturnTargetArray  Validation Return Target Array
 * @param[out]  aOuterColumnList        Outer Column List
 * @param[out]  aPlanDelete             Candidate Delete Plan
 * @param[out]  aPlanReadTable          Delete Read Table Node
 * @param[out]  aPlanSubQueryList       Candidate SubQuery List Plan
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanForDelete( smlTransId               aTransID,
                                     qllDBCStmt             * aDBCStmt,
                                     qllStatement           * aSQLStmt,
                                     qloInitExprList        * aQueryExprList,
                                     qloValidTblStatList    * aTableStatList,
                                     qlncNodeCommon         * aCandNode,
                                     qlncRefExprList        * aReadColumnList,
                                     stlInt32                 aReturnTargetCount,
                                     qlncTarget             * aReturnTargetArray,
                                     qlvInitTarget         ** aInitReturnTargetArray,
                                     qlvRefExprList        ** aOuterColumnList,
                                     qlncPlanCommon        ** aPlanDelete,
                                     qlncPlanCommon        ** aPlanReadTable,
                                     qlncPlanCommon        ** aPlanSubQueryList,
                                     qllEnv                 * aEnv )
{
    stlInt32                  i;
    qlncPlanBasicParamInfo    sPlanBasicParamInfo;
    qlncPlanDetailParamInfo   sPlanDetailParamInfo;
    qlncPlanNodeMapDynArray * sPlanNodeMapDynArr    = NULL;
    qlncPlanCommon          * sPlanReadTable        = NULL;
    qlncPlanCommon          * sPlanRootNode         = NULL;
    qlncPlanCommon          * sPlanSubQueryList     = NULL;
    qlvInitTarget           * sInitReturnTargetArr  = NULL;
    qlncConvExprParamInfo     sConvExprParamInfo;
    qlvRefExprList            sTmpSubQueryExprList;
    qlncRefExprItem         * sRefCandExprItem      = NULL;
    qlvRefExprList          * sOuterColumnList      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableStatList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReadColumnList != NULL, QLL_ERROR_STACK(aEnv) );


    /* Plan Node Map Dynamic Array 생성 */
    STL_TRY( qlncCreatePlanNodeMapDynArray( &QLL_CANDIDATE_MEM( aEnv ),
                                            &sPlanNodeMapDynArr,
                                            aEnv )
             == STL_SUCCESS );

    /* Candidate Plan Basic Parameter Info 설정 */
    sPlanBasicParamInfo.mRegionMem = QLL_CODE_PLAN( aDBCStmt );
    sPlanBasicParamInfo.mCandidateMem = &QLL_CANDIDATE_MEM( aEnv );
    sPlanBasicParamInfo.mPlanNodeMapDynArr = sPlanNodeMapDynArr;
    sPlanBasicParamInfo.mRootQuerySetPlanNode = NULL;
    sPlanBasicParamInfo.mInitQuerySetNode = NULL;
    sPlanBasicParamInfo.mNeedRowBlock = STL_TRUE;

    /* Basic Candidate Plan Node 생성 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ aCandNode->mBestOptType ](
                 &sPlanBasicParamInfo,
                 aCandNode,
                 &sPlanReadTable,
                 aEnv )
             == STL_SUCCESS );

    /* Candidate Plan Detail Parameter Info 설정 */
    sPlanDetailParamInfo.mParamInfo.mTransID = aTransID;
    sPlanDetailParamInfo.mParamInfo.mDBCStmt = aDBCStmt;
    sPlanDetailParamInfo.mParamInfo.mSQLStmt = aSQLStmt;
    sPlanDetailParamInfo.mRegionMem = QLL_CODE_PLAN( aDBCStmt );
    sPlanDetailParamInfo.mCandidateMem = &QLL_CANDIDATE_MEM( aEnv );
    sPlanDetailParamInfo.mPlanNodeMapDynArr = sPlanNodeMapDynArr;
    sPlanDetailParamInfo.mInitExprList = aQueryExprList->mInitExprList;
    sPlanDetailParamInfo.mTableStatList = aTableStatList;
    sPlanDetailParamInfo.mJoinOuterInfoList.mCount = 0;
    sPlanDetailParamInfo.mJoinOuterInfoList.mHead = NULL;

    /* Detail Candidate Plan Node 생성 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanReadTable->mPlanType ](
                 &sPlanDetailParamInfo,
                 sPlanReadTable,
                 &sPlanRootNode,
                 aEnv )
             == STL_SUCCESS );

    /* Read Column List 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    sRefCandExprItem = aReadColumnList->mHead;
    while( sRefCandExprItem != NULL )
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(sPlanDetailParamInfo.mParamInfo),
            QLL_CODE_PLAN( aDBCStmt ),
            &QLL_CANDIDATE_MEM( aEnv ),
            aQueryExprList->mInitExprList,
            aTableStatList,
            0,
            NULL,
            0,
            NULL,
            sPlanReadTable,
            sPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            sRefCandExprItem->mExpr );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[ sRefCandExprItem->mExpr->mType ](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        sRefCandExprItem = sRefCandExprItem->mNext;
    }

    /* Delete의 경우 Validation Node로부터 Reference Column을 참조하므로 이에 대한 처리를 해준다. */
    switch( sPlanReadTable->mPlanType )
    {
        case QLNC_PLAN_TYPE_TABLE_SCAN:
            ((qlvInitBaseTableNode*)(sPlanReadTable->mInitNode))->mRefColumnList =
                ((qlncPlanTableScan*)sPlanReadTable)->mReadColList;
            break;
        case QLNC_PLAN_TYPE_INDEX_SCAN:
            ((qlvInitBaseTableNode*)(sPlanReadTable->mInitNode))->mRefColumnList =
                ((qlncPlanIndexScan*)sPlanReadTable)->mReadColList;
            break;
        case QLNC_PLAN_TYPE_INDEX_COMBINE:
            ((qlvInitBaseTableNode*)(sPlanReadTable->mInitNode))->mRefColumnList =
                ((qlncPlanIndexCombine*)sPlanReadTable)->mReadColList;
            break;
        case QLNC_PLAN_TYPE_ROWID_SCAN:
            ((qlvInitBaseTableNode*)(sPlanReadTable->mInitNode))->mRefColumnList =
                ((qlncPlanRowidScan*)sPlanReadTable)->mReadColList;
            break;
        case QLNC_PLAN_TYPE_NESTED_JOIN:
            ((qlvInitJoinTableNode*)(sPlanReadTable->mInitNode))->mRefColumnList =
                ((qlncPlanNestedJoin*)sPlanReadTable)->mReadColList;
            break;
        case QLNC_PLAN_TYPE_MERGE_JOIN:
            ((qlvInitJoinTableNode*)(sPlanReadTable->mInitNode))->mRefColumnList =
                ((qlncPlanMergeJoin*)sPlanReadTable)->mReadColList;
            break;
        case QLNC_PLAN_TYPE_HASH_JOIN:
            ((qlvInitJoinTableNode*)(sPlanReadTable->mInitNode))->mRefColumnList =
                ((qlncPlanHashJoin*)sPlanReadTable)->mReadColList;
            break;
        /* Delete구문의 Read Table에서는 Join Combine이 올 수 없다. */
        /* case QLNC_PLAN_TYPE_JOIN_COMBINE: */
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Return Target 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( aReturnTargetCount > 0 )
    {
        /* Plan Node Map Dynamic Array에 Write Table Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( sPlanNodeMapDynArr,
                                                 sPlanReadTable,
                                                 aEnv )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlvInitTarget ) * aReturnTargetCount,
                                    (void**) &sInitReturnTargetArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( i = 0; i < aReturnTargetCount; i++ )
        {
            sInitReturnTargetArr[i].mDisplayPos = aReturnTargetArray[i].mDisplayPos;
            QLNC_ALLOC_AND_COPY_STRING( sInitReturnTargetArr[i].mDisplayName,
                                        aReturnTargetArray[i].mDisplayName,
                                        QLL_CODE_PLAN( aDBCStmt ),
                                        aEnv );

            QLNC_ALLOC_AND_COPY_STRING( sInitReturnTargetArr[i].mAliasName,
                                        aReturnTargetArray[i].mAliasName,
                                        QLL_CODE_PLAN( aDBCStmt ),
                                        aEnv );

            QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                &sConvExprParamInfo,
                &(sPlanDetailParamInfo.mParamInfo),
                QLL_CODE_PLAN( aDBCStmt ),
                &QLL_CANDIDATE_MEM( aEnv ),
                aQueryExprList->mInitExprList,
                aTableStatList,
                0,
                NULL,
                0,
                NULL,
                sPlanReadTable,
                sPlanNodeMapDynArr,
                &sTmpSubQueryExprList,
                STL_TRUE,
                aReturnTargetArray[i].mExpr );

            STL_TRY( qlncConvertCandExprToInitExprFuncList[ aReturnTargetArray[i].mExpr->mType ](
                         &sConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );

            sInitReturnTargetArr[i].mExpr = sConvExprParamInfo.mInitExpr;

            if( aSQLStmt->mBindCount > 0 )
            {
                sInitReturnTargetArr[i].mDataTypeInfo = aReturnTargetArray[i].mDataTypeInfo;
            }
            else
            {
                STL_TRY( qlvGetExprDataTypeInfo( aSQLStmt->mRetrySQL,
                                                 &QLL_CANDIDATE_MEM( aEnv ),
                                                 sInitReturnTargetArr[i].mExpr,
                                                 aSQLStmt->mBindContext,
                                                 & sInitReturnTargetArr[i].mDataTypeInfo,
                                                 aEnv )
                         == STL_SUCCESS );
            }
        }

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( sPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sInitReturnTargetArr = NULL;
    }

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sTmpSubQueryExprList.mCount > 0 )
    {
        STL_TRY( qlncMakeCandPlanSubQueryList( QLL_CODE_PLAN( aDBCStmt ),
                                               &QLL_CANDIDATE_MEM( aEnv ),
                                               NULL,
                                               &sTmpSubQueryExprList,
                                               &sPlanSubQueryList,
                                               aEnv )
                 == STL_SUCCESS );

        /* SubQuery가 참조하는 Outer Column을 찾아 Column의 Relation Node의 Pointer를 변경 */
        qlncFindOuterColumnListFromPlanNode( sPlanReadTable,
                                             &sOuterColumnList );
    }
    else
    {
        sPlanSubQueryList = NULL;
        sOuterColumnList = NULL;
    }

    /* Output 설정 */
    *aInitReturnTargetArray = sInitReturnTargetArr;
    *aOuterColumnList = sOuterColumnList;
    *aPlanDelete = sPlanRootNode;
    *aPlanReadTable = sPlanReadTable;
    *aPlanSubQueryList = sPlanSubQueryList;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Update를 위한 Candidate Plan을 생성한다.
 * @param[in]       aTransID                    Transaction ID
 * @param[in]       aDBCStmt                    DBC Statement
 * @param[in]       aSQLStmt                    SQL Statement
 * @param[in]       aQueryExprList              Query Expression List
 * @param[in]       aTableStatList              Table Stat List
 * @param[in,out]   aPlanUpdateStmtParamInfo    Update Statement Parameter Info
 * @param[in]       aEnv                        Environment
 */
stlStatus qlncMakeCandPlanForUpdate( smlTransId                       aTransID,
                                     qllDBCStmt                     * aDBCStmt,
                                     qllStatement                   * aSQLStmt,
                                     qloInitExprList                * aQueryExprList,
                                     qloValidTblStatList            * aTableStatList,
                                     qlncPlanUpdateStmtParamInfo    * aPlanUpdateStmtParamInfo,
                                     qllEnv                         * aEnv )
{
    stlInt32                  i;
    qlncPlanBasicParamInfo    sPlanBasicParamInfo;
    qlncPlanDetailParamInfo   sPlanDetailParamInfo;
    qlncPlanNodeMapDynArray * sPlanNodeMapDynArr        = NULL;
    qlncPlanCommon          * sRootPlanNode             = NULL;
    qlncPlanCommon          * sPlanReadTable            = NULL;
    qlncPlanCommon          * sPlanWriteTable           = NULL;
    qlncPlanCommon          * sPlanSetSubQueryList      = NULL;
    qlncPlanCommon          * sPlanReturnSubQueryList   = NULL;
    qlncConvExprParamInfo     sConvExprParamInfo;
    qlvRefExprList            sTmpSubQueryExprList;
    qlncRefExprItem         * sRefCandExprItem          = NULL;

    qlvRefExprList          * sSetColExprList           = NULL;
    qlvRefExprList          * sShareReadColumnList      = NULL;
    qlvRefExprList          * sShareWriteColumnList     = NULL;
    qlvRefExprList          * sSetOuterColumnList       = NULL;
    qlvRefExprList          * sReturnOuterColumnList    = NULL;
    qlvRefExprItem          * sRefInitExprItem          = NULL;
    qlvInitExpression      ** sSetExprArray             = NULL;
    qlvInitTarget           * sReturnTargetArray        = NULL;

    qloValidTblStatList       sTmpTableStatList;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableStatList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanUpdateStmtParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* Plan Node Map Dynamic Array 생성 */
    STL_TRY( qlncCreatePlanNodeMapDynArray( &QLL_CANDIDATE_MEM( aEnv ),
                                            &sPlanNodeMapDynArr,
                                            aEnv )
             == STL_SUCCESS );

    /**
     * Read Table Node 처리
     */

    /* Candidate Plan Basic Parameter Info 설정 */
    sPlanBasicParamInfo.mRegionMem = QLL_CODE_PLAN( aDBCStmt );
    sPlanBasicParamInfo.mCandidateMem = &QLL_CANDIDATE_MEM( aEnv );
    sPlanBasicParamInfo.mPlanNodeMapDynArr = sPlanNodeMapDynArr;
    sPlanBasicParamInfo.mRootQuerySetPlanNode = NULL;
    sPlanBasicParamInfo.mInitQuerySetNode = NULL;
    sPlanBasicParamInfo.mNeedRowBlock = STL_TRUE;

    /* Basic Candidate Plan Node 생성 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ aPlanUpdateStmtParamInfo->mReadTableNode->mBestOptType ](
                 &sPlanBasicParamInfo,
                 aPlanUpdateStmtParamInfo->mReadTableNode,
                 &sPlanReadTable,
                 aEnv )
             == STL_SUCCESS );

    /* Candidate Plan Detail Parameter Info 설정 */
    sPlanDetailParamInfo.mParamInfo.mTransID = aTransID;
    sPlanDetailParamInfo.mParamInfo.mDBCStmt = aDBCStmt;
    sPlanDetailParamInfo.mParamInfo.mSQLStmt = aSQLStmt;
    sPlanDetailParamInfo.mRegionMem = QLL_CODE_PLAN( aDBCStmt );
    sPlanDetailParamInfo.mCandidateMem = &QLL_CANDIDATE_MEM( aEnv );
    sPlanDetailParamInfo.mPlanNodeMapDynArr = sPlanNodeMapDynArr;
    sPlanDetailParamInfo.mInitExprList = aQueryExprList->mInitExprList;
    sPlanDetailParamInfo.mTableStatList = aTableStatList;
    sPlanDetailParamInfo.mJoinOuterInfoList.mCount = 0;
    sPlanDetailParamInfo.mJoinOuterInfoList.mHead = NULL;

    /* Detail Candidate Plan Node 생성 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanReadTable->mPlanType ](
                 &sPlanDetailParamInfo,
                 sPlanReadTable,
                 &sRootPlanNode,
                 aEnv )
             == STL_SUCCESS );

    /* Read Column List 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    sRefCandExprItem = aPlanUpdateStmtParamInfo->mReadColumnList->mHead;
    while( sRefCandExprItem != NULL )
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(sPlanDetailParamInfo.mParamInfo),
            QLL_CODE_PLAN( aDBCStmt ),
            &QLL_CANDIDATE_MEM( aEnv ),
            aQueryExprList->mInitExprList,
            aTableStatList,
            0,
            NULL,
            0,
            NULL,
            sPlanReadTable,
            sPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            sRefCandExprItem->mExpr );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[ sRefCandExprItem->mExpr->mType ](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        sRefCandExprItem = sRefCandExprItem->mNext;
    }

    /* Update의 경우 Validation Node로부터 Reference Column을 참조하므로 이에 대한 처리를 해준다. */
    switch( sPlanReadTable->mPlanType )
    {
        case QLNC_PLAN_TYPE_TABLE_SCAN:
            ((qlvInitBaseTableNode*)(sPlanReadTable->mInitNode))->mRefColumnList =
                ((qlncPlanTableScan*)sPlanReadTable)->mReadColList;
            ((qlvInitBaseTableNode*)(sPlanReadTable->mInitNode))->mRefRowIdColumn =
                ((qlncPlanTableScan*)sPlanReadTable)->mRowidColumn;
            break;
        case QLNC_PLAN_TYPE_INDEX_SCAN:
            ((qlvInitBaseTableNode*)(sPlanReadTable->mInitNode))->mRefColumnList =
                ((qlncPlanIndexScan*)sPlanReadTable)->mReadColList;
            ((qlvInitBaseTableNode*)(sPlanReadTable->mInitNode))->mRefRowIdColumn =
                ((qlncPlanIndexScan*)sPlanReadTable)->mRowidColumn;
            break;
        case QLNC_PLAN_TYPE_INDEX_COMBINE:
            ((qlvInitBaseTableNode*)(sPlanReadTable->mInitNode))->mRefColumnList =
                ((qlncPlanIndexCombine*)sPlanReadTable)->mReadColList;
            ((qlvInitBaseTableNode*)(sPlanReadTable->mInitNode))->mRefRowIdColumn =
                ((qlncPlanIndexCombine*)sPlanReadTable)->mRowidColumn;
            break;
        case QLNC_PLAN_TYPE_ROWID_SCAN:
            ((qlvInitBaseTableNode*)(sPlanReadTable->mInitNode))->mRefColumnList =
                ((qlncPlanRowidScan*)sPlanReadTable)->mReadColList;
            ((qlvInitBaseTableNode*)(sPlanReadTable->mInitNode))->mRefRowIdColumn =
                ((qlncPlanRowidScan*)sPlanReadTable)->mRowidColumn;
            break;
        case QLNC_PLAN_TYPE_NESTED_JOIN:
            ((qlvInitJoinTableNode*)(sPlanReadTable->mInitNode))->mRefColumnList =
                ((qlncPlanNestedJoin*)sPlanReadTable)->mReadColList;
            break;
        case QLNC_PLAN_TYPE_MERGE_JOIN:
            ((qlvInitJoinTableNode*)(sPlanReadTable->mInitNode))->mRefColumnList =
                ((qlncPlanMergeJoin*)sPlanReadTable)->mReadColList;
            break;
        case QLNC_PLAN_TYPE_HASH_JOIN:
            ((qlvInitJoinTableNode*)(sPlanReadTable->mInitNode))->mRefColumnList =
                ((qlncPlanHashJoin*)sPlanReadTable)->mReadColList;
            break;
        /* Update구문에서 Read Table에 Join Combine이 올 수 없다. */
        /* case QLNC_PLAN_TYPE_JOIN_COMBINE: */
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Set Value Array 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvInitExpression* ) * aPlanUpdateStmtParamInfo->mSetColumnCount,
                                (void**) &sSetExprArray,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Plan Node Map Dynamic Array에 Table Plan Node를 추가 */
    STL_TRY( qlncAddTailPlanNodeMapDynArray( sPlanNodeMapDynArr,
                                             sPlanReadTable,
                                             aEnv )
             == STL_SUCCESS );
    
    for( i = 0; i < aPlanUpdateStmtParamInfo->mSetColumnCount; i++ )
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(sPlanDetailParamInfo.mParamInfo),
            QLL_CODE_PLAN( aDBCStmt ),
            &QLL_CANDIDATE_MEM( aEnv ),
            aQueryExprList->mInitExprList,
            aTableStatList,
            0,
            NULL,
            0,
            NULL,
            sPlanReadTable,
            sPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            aPlanUpdateStmtParamInfo->mSetValueArray[i] );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[ aPlanUpdateStmtParamInfo->mSetValueArray[i]->mType ](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        /* Row SubQuery인 경우 처리 */
        if( (aPlanUpdateStmtParamInfo->mSetValueArray[i]->mType == QLNC_EXPR_TYPE_SUB_QUERY) &&
            (sConvExprParamInfo.mInitExpr->mType == QLV_EXPR_TYPE_SUB_QUERY) )
        {
            STL_DASSERT( ((qlncExprSubQuery*)aPlanUpdateStmtParamInfo->mSetValueArray[i])->mRefIdx == 0 );
            STL_DASSERT( sConvExprParamInfo.mInitExpr->mExpr.mSubQuery->mRefColList != NULL );

            /* SubQuery의 Reference Column List에 존재하는 Expression 들을
             * 차례로 Set Expr Array에 넣는다. */
            sRefInitExprItem = sConvExprParamInfo.mInitExpr->mExpr.mSubQuery->mRefColList->mHead;
            while( sRefInitExprItem != NULL )
            {
                sSetExprArray[i] = sRefInitExprItem->mExprPtr;

                if( sRefInitExprItem->mNext != NULL )
                {
                    i++;
                }
                sRefInitExprItem = sRefInitExprItem->mNext;
            }
        }
        else
        {
            sSetExprArray[i] = sConvExprParamInfo.mInitExpr;
        }
    }

    /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
    STL_TRY( qlncDelTailPlanNodeMapDynArray( sPlanNodeMapDynArr,
                                             1,
                                             aEnv )
             == STL_SUCCESS );
    
    /* SubQuery Expression List가 존재하는지 판별 */
    if( sTmpSubQueryExprList.mCount > 0 )
    {
        STL_TRY( qlncMakeCandPlanSubQueryList( QLL_CODE_PLAN( aDBCStmt ),
                                               &QLL_CANDIDATE_MEM( aEnv ),
                                               NULL,
                                               &sTmpSubQueryExprList,
                                               &sPlanSetSubQueryList,
                                               aEnv )
                 == STL_SUCCESS );

        /* SubQuery가 참조하는 Outer Column을 찾아 Column의 Relation Node의 Pointer를 변경 */
        qlncFindOuterColumnListFromPlanNode( sPlanReadTable,
                                             &sSetOuterColumnList );
    }
    else
    {
        sPlanSetSubQueryList = NULL;
        sSetOuterColumnList = NULL;
    }

    /* Share Read Column List 설정 */
    STL_DASSERT( aPlanUpdateStmtParamInfo->mShareReadColumnList != NULL );
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvRefExprList ),
                                (void**) &sShareReadColumnList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_LIST( sShareReadColumnList );
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    sRefCandExprItem = aPlanUpdateStmtParamInfo->mShareReadColumnList->mHead;
    while( sRefCandExprItem != NULL )
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(sPlanDetailParamInfo.mParamInfo),
            QLL_CODE_PLAN( aDBCStmt ),
            &QLL_CANDIDATE_MEM( aEnv ),
            aQueryExprList->mInitExprList,
            aTableStatList,
            0,
            NULL,
            0,
            NULL,
            sPlanReadTable,
            sPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            sRefCandExprItem->mExpr );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[ sRefCandExprItem->mExpr->mType ](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlvRefExprItem ),
                                    (void**) &sRefInitExprItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRefInitExprItem->mExprPtr = sConvExprParamInfo.mInitExpr;
        sRefInitExprItem->mIsLeft = STL_TRUE;
        sRefInitExprItem->mNext = NULL;

        QLNC_APPEND_ITEM_TO_LIST( sShareReadColumnList, sRefInitExprItem );

        sRefCandExprItem = sRefCandExprItem->mNext;
    }


    /**
     * Write Table Node 처리
     */

    /* Candidate Plan Basic Parameter Info 설정 */
    sPlanBasicParamInfo.mRegionMem = QLL_CODE_PLAN( aDBCStmt );
    sPlanBasicParamInfo.mPlanNodeMapDynArr = sPlanNodeMapDynArr;
    sPlanBasicParamInfo.mRootQuerySetPlanNode = NULL;
    sPlanBasicParamInfo.mInitQuerySetNode = NULL;

    /* Basic Candidate Plan Node 생성 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ aPlanUpdateStmtParamInfo->mWriteTableNode->mBestOptType ](
                 &sPlanBasicParamInfo,
                 aPlanUpdateStmtParamInfo->mWriteTableNode,
                 &sPlanWriteTable,
                 aEnv )
             == STL_SUCCESS );

    /* Candidate Plan Detail Parameter Info 설정 */
    sTmpTableStatList.mCount = 0;
    sTmpTableStatList.mHead = NULL;
    sTmpTableStatList.mTail = NULL;
    sPlanDetailParamInfo.mParamInfo.mTransID = aTransID;
    sPlanDetailParamInfo.mParamInfo.mDBCStmt = aDBCStmt;
    sPlanDetailParamInfo.mParamInfo.mSQLStmt = aSQLStmt;
    sPlanDetailParamInfo.mRegionMem = QLL_CODE_PLAN( aDBCStmt );
    sPlanDetailParamInfo.mPlanNodeMapDynArr = sPlanNodeMapDynArr;
    sPlanDetailParamInfo.mInitExprList = aQueryExprList->mInitExprList;
    sPlanDetailParamInfo.mTableStatList = &sTmpTableStatList;
    sPlanDetailParamInfo.mJoinOuterInfoList.mCount = 0;
    sPlanDetailParamInfo.mJoinOuterInfoList.mHead = NULL;

    /* Detail Candidate Plan Node 생성 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanWriteTable->mPlanType ](
                 &sPlanDetailParamInfo,
                 sPlanWriteTable,
                 &sPlanWriteTable,
                 aEnv )
             == STL_SUCCESS );

    /* Write Column List 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    sRefCandExprItem = aPlanUpdateStmtParamInfo->mWriteColumnList->mHead;
    while( sRefCandExprItem != NULL )
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(sPlanDetailParamInfo.mParamInfo),
            QLL_CODE_PLAN( aDBCStmt ),
            &QLL_CANDIDATE_MEM( aEnv ),
            aQueryExprList->mInitExprList,
            aTableStatList,
            0,
            NULL,
            0,
            NULL,
            sPlanWriteTable,
            sPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            sRefCandExprItem->mExpr );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[ sRefCandExprItem->mExpr->mType ](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        sRefCandExprItem = sRefCandExprItem->mNext;
    }

    /* Update의 경우 Validation Node로부터 Reference Column을 참조하므로 이에 대한 처리를 해준다. */
    switch( sPlanWriteTable->mPlanType )
    {
        case QLNC_PLAN_TYPE_TABLE_SCAN:
            ((qlvInitBaseTableNode*)(sPlanWriteTable->mInitNode))->mRefColumnList =
                ((qlncPlanTableScan*)sPlanWriteTable)->mReadColList;
            ((qlvInitBaseTableNode*)(sPlanWriteTable->mInitNode))->mRefRowIdColumn =
                ((qlncPlanTableScan*)sPlanWriteTable)->mRowidColumn;
            break;
        case QLNC_PLAN_TYPE_INDEX_SCAN:
            ((qlvInitBaseTableNode*)(sPlanWriteTable->mInitNode))->mRefColumnList =
                ((qlncPlanIndexScan*)sPlanWriteTable)->mReadColList;
            ((qlvInitBaseTableNode*)(sPlanWriteTable->mInitNode))->mRefRowIdColumn =
                ((qlncPlanIndexScan*)sPlanWriteTable)->mRowidColumn;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    /* Set Column Expression List 설정 */
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvRefExprList ),
                                (void**) &sSetColExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_LIST( sSetColExprList );
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    sRefCandExprItem = aPlanUpdateStmtParamInfo->mSetColExprList->mHead;
    while( sRefCandExprItem != NULL )
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(sPlanDetailParamInfo.mParamInfo),
            QLL_CODE_PLAN( aDBCStmt ),
            &QLL_CANDIDATE_MEM( aEnv ),
            aQueryExprList->mInitExprList,
            aTableStatList,
            0,
            NULL,
            0,
            NULL,
            sPlanWriteTable,
            sPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            sRefCandExprItem->mExpr );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[ sRefCandExprItem->mExpr->mType ](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlvRefExprItem ),
                                    (void**) &sRefInitExprItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRefInitExprItem->mExprPtr = sConvExprParamInfo.mInitExpr;
        sRefInitExprItem->mIsLeft = STL_TRUE;
        sRefInitExprItem->mNext = NULL;

        QLNC_APPEND_ITEM_TO_LIST( sSetColExprList, sRefInitExprItem );

        sRefCandExprItem = sRefCandExprItem->mNext;
    }

    /* Share Write Column List 설정 */
    STL_DASSERT( aPlanUpdateStmtParamInfo->mShareWriteColumnList != NULL );
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlvRefExprList ),
                                (void**) &sShareWriteColumnList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_INIT_LIST( sShareWriteColumnList );
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    sRefCandExprItem = aPlanUpdateStmtParamInfo->mShareWriteColumnList->mHead;
    while( sRefCandExprItem != NULL )
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(sPlanDetailParamInfo.mParamInfo),
            QLL_CODE_PLAN( aDBCStmt ),
            &QLL_CANDIDATE_MEM( aEnv ),
            aQueryExprList->mInitExprList,
            aTableStatList,
            0,
            NULL,
            0,
            NULL,
            sPlanWriteTable,
            sPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            sRefCandExprItem->mExpr );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[ sRefCandExprItem->mExpr->mType ](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlvRefExprItem ),
                                    (void**) &sRefInitExprItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRefInitExprItem->mExprPtr = sConvExprParamInfo.mInitExpr;
        sRefInitExprItem->mIsLeft = STL_TRUE;
        sRefInitExprItem->mNext = NULL;

        QLNC_APPEND_ITEM_TO_LIST( sShareWriteColumnList, sRefInitExprItem );

        sRefCandExprItem = sRefCandExprItem->mNext;
    }

    /* Return Target 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( aPlanUpdateStmtParamInfo->mReturnTargetCount > 0 )
    {
        /* Plan Node Map Dynamic Array에 Table Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( sPlanNodeMapDynArr,
                                                 (aPlanUpdateStmtParamInfo->mIsReturnNew == STL_TRUE
                                                  ? sPlanWriteTable : sPlanReadTable),
                                                 aEnv )
                 == STL_SUCCESS );

        if( aPlanUpdateStmtParamInfo->mIsReturnNew == STL_FALSE )
        {
            STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                        STL_SIZEOF( qlvRefExprList ),
                                        (void**) &(((qlvInitBaseTableNode*)(sPlanReadTable->mInitNode))->mOuterColumnList),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            QLNC_INIT_LIST( ((qlvInitBaseTableNode*)(sPlanReadTable->mInitNode))->mOuterColumnList );
        }

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlvInitTarget ) * aPlanUpdateStmtParamInfo->mReturnTargetCount,
                                    (void**) &sReturnTargetArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( i = 0; i < aPlanUpdateStmtParamInfo->mReturnTargetCount; i++ )
        {
            sReturnTargetArray[i].mDisplayPos =
                aPlanUpdateStmtParamInfo->mReturnTargetArray[i].mDisplayPos;

            QLNC_ALLOC_AND_COPY_STRING( sReturnTargetArray[i].mDisplayName,
                                        aPlanUpdateStmtParamInfo->mReturnTargetArray[i].mDisplayName,
                                        QLL_CODE_PLAN( aDBCStmt ),
                                        aEnv );

            QLNC_ALLOC_AND_COPY_STRING( sReturnTargetArray[i].mAliasName,
                                        aPlanUpdateStmtParamInfo->mReturnTargetArray[i].mAliasName,
                                        QLL_CODE_PLAN( aDBCStmt ),
                                        aEnv );

            QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                &sConvExprParamInfo,
                &(sPlanDetailParamInfo.mParamInfo),
                QLL_CODE_PLAN( aDBCStmt ),
                &QLL_CANDIDATE_MEM( aEnv ),
                aQueryExprList->mInitExprList,
                aTableStatList,
                0,
                NULL,
                0,
                NULL,
                (aPlanUpdateStmtParamInfo->mIsReturnNew == STL_FALSE
                 ? sPlanReadTable : sPlanWriteTable),
                sPlanNodeMapDynArr,
                &sTmpSubQueryExprList,
                STL_TRUE,
                aPlanUpdateStmtParamInfo->mReturnTargetArray[i].mExpr );

            STL_TRY( qlncConvertCandExprToInitExprFuncList[ aPlanUpdateStmtParamInfo->mReturnTargetArray[i].mExpr->mType ](
                         &sConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );

            sReturnTargetArray[i].mExpr = sConvExprParamInfo.mInitExpr;

            if( aSQLStmt->mBindCount > 0 )
            {
                sReturnTargetArray[i].mDataTypeInfo =
                    aPlanUpdateStmtParamInfo->mReturnTargetArray[i].mDataTypeInfo;
            }
            else
            {
                STL_TRY( qlvGetExprDataTypeInfo( aSQLStmt->mRetrySQL,
                                                 &QLL_CANDIDATE_MEM( aEnv ),
                                                 sReturnTargetArray[i].mExpr,
                                                 aSQLStmt->mBindContext,
                                                 & sReturnTargetArray[i].mDataTypeInfo,
                                                 aEnv )
                         == STL_SUCCESS );
            }
        }

        /* Read / Write RowId Column이 중복으로 처리됨을 방지하기 위해
         * RETURNING NEW 에 쓰인 ROWID column 들의 relation node를 ReadTableNode로 변경한다. */
        if( (aPlanUpdateStmtParamInfo->mIsReturnNew == STL_TRUE) &&
            (((qlvInitBaseTableNode*)(sPlanWriteTable->mInitNode))->mRefRowIdColumn != NULL) &&
            (((qlvInitBaseTableNode*)(sPlanReadTable->mInitNode))->mRefRowIdColumn != NULL) )
        {
            for( i = 0; i < aPlanUpdateStmtParamInfo->mReturnTargetCount; i++ )
            {
                STL_TRY( qlvChangeRelationNodeForRowIdCol(
                             sPlanWriteTable->mInitNode,
                             sPlanReadTable->mInitNode,
                             sReturnTargetArray[ i ].mExpr,
                             aEnv )
                         == STL_SUCCESS );
            }
        }

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( sPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sReturnTargetArray = NULL;
    }

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sTmpSubQueryExprList.mCount > 0 )
    {
        STL_TRY( qlncMakeCandPlanSubQueryList( QLL_CODE_PLAN( aDBCStmt ),
                                               &QLL_CANDIDATE_MEM( aEnv ),
                                               NULL,
                                               &sTmpSubQueryExprList,
                                               &sPlanReturnSubQueryList,
                                               aEnv )
                 == STL_SUCCESS );

        /* SubQuery가 참조하는 Outer Column을 찾아 Column의 Relation Node의 Pointer를 변경 */
        qlncFindOuterColumnListFromPlanNode( (aPlanUpdateStmtParamInfo->mIsReturnNew == STL_FALSE
                                              ? sPlanReadTable : sPlanWriteTable),
                                             &sReturnOuterColumnList );
    }
    else
    {
        sPlanReturnSubQueryList = NULL;
        sReturnOuterColumnList = NULL;
    }

    /* Output 설정 */
    aPlanUpdateStmtParamInfo->mOutReturnTargetArray = sReturnTargetArray;
    aPlanUpdateStmtParamInfo->mOutSetOuterColumnList = sSetOuterColumnList;
    aPlanUpdateStmtParamInfo->mOutReturnOuterColumnList = sReturnOuterColumnList;
    aPlanUpdateStmtParamInfo->mOutSetExprArray = sSetExprArray;
    aPlanUpdateStmtParamInfo->mOutSetColExprList = sSetColExprList;
    aPlanUpdateStmtParamInfo->mOutShareReadColList = sShareReadColumnList;
    aPlanUpdateStmtParamInfo->mOutShareWriteColList = sShareWriteColumnList;
    aPlanUpdateStmtParamInfo->mOutPlanRootNode = sRootPlanNode;
    aPlanUpdateStmtParamInfo->mOutPlanReadTable = sPlanReadTable;
    aPlanUpdateStmtParamInfo->mOutPlanWriteTable = sPlanWriteTable;
    aPlanUpdateStmtParamInfo->mOutPlanSetSubQueryList = sPlanSetSubQueryList;
    aPlanUpdateStmtParamInfo->mOutPlanReturnSubQueryList = sPlanReturnSubQueryList;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Expression에서 호출되어 Candidate Plan을 생성한다.
 * @param[in]   aParamInfo          Parameter Info
 * @param[in]   aInitStmtExprList   Init Statement Expression List
 * @param[in]   aTableStatList      Table Stat List
 * @param[in]   aCandNode           Candidate Node
 * @param[in]   aJoinOuterInfoList  Join Outer Column Info List
 * @param[in]   aPlanNodeMapDynArr  Candidate Plan Node Map Dynamic Array
 * @param[out]  aCandPlan           Candidate Plan
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanForSubQueryExpr( qlncParamInfo            * aParamInfo,
                                           qlvInitStmtExprList      * aInitStmtExprList,
                                           qloValidTblStatList      * aTableStatList,
                                           qlncNodeCommon           * aCandNode,
                                           qlncJoinOuterInfoList    * aJoinOuterInfoList,
                                           qlncPlanNodeMapDynArray  * aPlanNodeMapDynArr,
                                           qlncPlanCommon          ** aCandPlan,
                                           qllEnv                   * aEnv )
{
    qlncPlanBasicParamInfo    sPlanBasicParamInfo;
    qlncPlanDetailParamInfo   sPlanDetailParamInfo;
    qlncPlanCommon          * sPlanNode             = NULL;
    qlvInitExprList           sInitExprList;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParamInfo->mDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParamInfo->mSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitStmtExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableStatList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinOuterInfoList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNodeMapDynArr != NULL, QLL_ERROR_STACK(aEnv) );


    /* Candidate Plan Basic Parameter Info 설정 */
    sPlanBasicParamInfo.mRegionMem = QLL_CODE_PLAN( aParamInfo->mDBCStmt );
    sPlanBasicParamInfo.mCandidateMem = &QLL_CANDIDATE_MEM( aEnv );
    sPlanBasicParamInfo.mPlanNodeMapDynArr = aPlanNodeMapDynArr;
    sPlanBasicParamInfo.mRootQuerySetPlanNode = NULL;
    sPlanBasicParamInfo.mInitQuerySetNode = NULL;
    sPlanBasicParamInfo.mNeedRowBlock = QLCR_NEED_TO_SET_ROWBLOCK( aParamInfo->mSQLStmt );

    /* Basic Candidate Plan Node 생성 */
    STL_TRY( qlncMakeCandPlanBasicFuncList[ aCandNode->mBestOptType ](
                 &sPlanBasicParamInfo,
                 aCandNode,
                 &sPlanNode,
                 aEnv )
             == STL_SUCCESS );

    /* Init Expression List 설정 */
    sInitExprList.mStmtExprList = aInitStmtExprList;
    sInitExprList.mPseudoColExprList = NULL;
    sInitExprList.mAggrExprList = NULL;
    sInitExprList.mNestedAggrExprList = NULL;

    /* Candidate Plan Detail Parameter Info 설정 */
    sPlanDetailParamInfo.mParamInfo = *aParamInfo;
    sPlanDetailParamInfo.mRegionMem = QLL_CODE_PLAN( aParamInfo->mDBCStmt );
    sPlanDetailParamInfo.mCandidateMem = &QLL_CANDIDATE_MEM( aEnv );
    sPlanDetailParamInfo.mPlanNodeMapDynArr = aPlanNodeMapDynArr;
    sPlanDetailParamInfo.mInitExprList = &sInitExprList;
    sPlanDetailParamInfo.mTableStatList = aTableStatList;
    sPlanDetailParamInfo.mJoinOuterInfoList.mCount = aJoinOuterInfoList->mCount;
    sPlanDetailParamInfo.mJoinOuterInfoList.mHead = aJoinOuterInfoList->mHead;

    /* Detail Candidate Plan Node 생성 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanNode->mPlanType ](
                 &sPlanDetailParamInfo,
                 sPlanNode,
                 &sPlanNode,
                 aEnv )
             == STL_SUCCESS );

    /* Output 설정 */
    *aCandPlan = sPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Aggregation을 처리할 Node의 AggrExprList 및 NestedAggrExprList를 설정한다.
 * @param[in]   aCandPlan               Candidate Plan
 * @param[out]  aOutAggrExprList        Aggregation Expression List
 * @param[out]  aOutNestedAggrExprList  Nested Aggregation Expression List
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMCPSetAggrExprList( qlncPlanCommon    * aCandPlan,
                                  qlvRefExprList   ** aOutAggrExprList,
                                  qlvRefExprList   ** aOutNestedAggrExprList,
                                  qllEnv            * aEnv )
{
    stlInt32                  i;
    stlInt32                  j;
    stlBool                   sHasSubQueryFilter;
    stlBool                   sIsPossible;
    qlncRefExprItem         * sRefExprItem          = NULL;
    qlncBooleanFilter       * sBooleanFilter        = NULL;
    qlncConstBooleanFilter  * sConstBooleanFilter   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCandPlan != NULL, QLL_ERROR_STACK(aEnv) );


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
                    for( j = 0; j < (_aFilter)->mOrFilters[i]->mFilterCount; j++ )  \
                    {                                                               \
                        if( (_aFilter)->mOrFilters[i]->mFilters[j]->mType ==        \
                            QLNC_EXPR_TYPE_BOOLEAN_FILTER )                         \
                        {                                                           \
                            sBooleanFilter =                                        \
                                (qlncBooleanFilter*)((_aFilter)->mOrFilters[i]->mFilters[j]);\
                            if( (sBooleanFilter->mSubQueryExprList != NULL) &&      \
                                (sBooleanFilter->mSubQueryExprList->mCount > 0) )   \
                            {                                                       \
                                if( sBooleanFilter->mExpr->mType                    \
                                    == QLNC_EXPR_TYPE_LIST_FUNCTION )               \
                                {                                                   \
                                    (_aHasSubQuery) = STL_TRUE;                     \
                                    break;                                          \
                                }                                                   \
                                                                                    \
                                sRefExprItem =                                      \
                                    sBooleanFilter->mSubQueryExprList->mHead;       \
                                while( sRefExprItem != NULL )                       \
                                {                                                   \
                                    STL_DASSERT( sRefExprItem->mExpr->mType         \
                                                 == QLNC_EXPR_TYPE_SUB_QUERY );     \
                                    if( (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL) &&   \
                                        (((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList->mCount > 0) )\
                                    {                                               \
                                        (_aHasSubQuery) = STL_TRUE;                 \
                                        break;                                      \
                                    }                                               \
                                                                                    \
                                    sRefExprItem = sRefExprItem->mNext;             \
                                }                                                   \
                                                                                    \
                                if( (_aHasSubQuery) == STL_TRUE )                   \
                                {                                                   \
                                    break;                                          \
                                }                                                   \
                            }                                                       \
                        }                                                           \
                        else                                                        \
                        {                                                           \
                            STL_DASSERT( (_aFilter)->mOrFilters[i]->mFilters[j]->mType ==   \
                                         QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER );     \
                            sConstBooleanFilter =                                   \
                                (qlncConstBooleanFilter*)((_aFilter)->mOrFilters[i]->mFilters[j]);\
                            if( (sConstBooleanFilter->mSubQueryExprList != NULL) &&     \
                                (sConstBooleanFilter->mSubQueryExprList->mCount > 0) && \
                                (sConstBooleanFilter->mExpr->mType == QLNC_EXPR_TYPE_LIST_FUNCTION) )\
                            {                                                       \
                                (_aHasSubQuery) = STL_TRUE;                         \
                                break;                                              \
                            }                                                       \
                        }                                                           \
                    }                                                               \
                                                                                    \
                    if( (_aHasSubQuery) == STL_TRUE )                               \
                    {                                                               \
                        break;                                                      \
                    }                                                               \
                }                                                                   \
            }                                                                       \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            (_aHasSubQuery) = STL_FALSE;                                            \
        }                                                                           \
    }


    switch( aCandPlan->mPlanType )
    {
        case QLNC_PLAN_TYPE_QUERY_SET:
        case QLNC_PLAN_TYPE_SET_OP:
            STL_DASSERT( 0 );
            break;
        case QLNC_PLAN_TYPE_QUERY_SPEC:
            *aOutAggrExprList = &(aCandPlan->mAggrExprList);
            *aOutNestedAggrExprList = &(aCandPlan->mNestedAggrExprList);

            /* Target에 Aggregation이 존재하고 해당 Aggregation에서
             * 현재 Query Spec의 Child Node에 해당하는 Column이 존재하는 경우
             * Aggregation Push 불가*/

            sIsPossible = STL_TRUE;
            if( ((qlncQuerySpec*)(aCandPlan->mCandNode))->mNonFilterSubQueryExprList != NULL )
            {
                sRefExprItem = ((qlncQuerySpec*)(aCandPlan->mCandNode))->mNonFilterSubQueryExprList->mHead;
                while( sRefExprItem != NULL )
                {
                    if( ((qlncExprSubQuery*)(sRefExprItem->mExpr))->mRefExprList != NULL )
                    {
                        sIsPossible = STL_FALSE;
                        break;
                    }

                    sRefExprItem = sRefExprItem->mNext;
                }
            }

            if( sIsPossible == STL_TRUE )
            {
                STL_TRY( qlncMCPSetAggrExprList( ((qlncPlanQuerySpec*)aCandPlan)->mChildPlanNode,
                                                 aOutAggrExprList,
                                                 aOutNestedAggrExprList,
                                                 aEnv )
                         == STL_SUCCESS );
            }

            break;
        case QLNC_PLAN_TYPE_TABLE_SCAN:
            _QLNC_CHECK_SUBQUERY_FILTER(
                ((qlncTableScanCost*)(aCandPlan->mCandNode->mBestCost))->mTableAndFilter,
                sHasSubQueryFilter );

            if( sHasSubQueryFilter == STL_FALSE )
            {
                *aOutAggrExprList = &(aCandPlan->mAggrExprList);
            }

            break;
        case QLNC_PLAN_TYPE_INDEX_SCAN:
            _QLNC_CHECK_SUBQUERY_FILTER(
                ((qlncIndexScanCost*)(aCandPlan->mCandNode->mBestCost))->mTableAndFilter,
                sHasSubQueryFilter );

            if( sHasSubQueryFilter == STL_FALSE )
            {
                *aOutAggrExprList = &(aCandPlan->mAggrExprList);
            }

            break;
        case QLNC_PLAN_TYPE_ROWID_SCAN:
            _QLNC_CHECK_SUBQUERY_FILTER(
                ((qlncRowidScanCost*)(aCandPlan->mCandNode->mBestCost))->mTableAndFilter,
                sHasSubQueryFilter );

            if( sHasSubQueryFilter == STL_FALSE )
            {
                *aOutAggrExprList = &(aCandPlan->mAggrExprList);
            }

            break;
        case QLNC_PLAN_TYPE_INDEX_COMBINE:
        case QLNC_PLAN_TYPE_SUB_TABLE:
        case QLNC_PLAN_TYPE_NESTED_JOIN:
        case QLNC_PLAN_TYPE_MERGE_JOIN:
        case QLNC_PLAN_TYPE_HASH_JOIN:
        case QLNC_PLAN_TYPE_JOIN_COMBINE:
            break;
        case QLNC_PLAN_TYPE_SORT_INSTANT:
            STL_DASSERT( aCandPlan->mCandNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT );
            _QLNC_CHECK_SUBQUERY_FILTER(
                ((qlncSortInstantNode*)(aCandPlan->mCandNode))->mFilter,
                sHasSubQueryFilter );

            if( sHasSubQueryFilter == STL_FALSE )
            {
                *aOutAggrExprList = &(aCandPlan->mAggrExprList);
            }

            STL_TRY( qlncMCPSetAggrExprList( ((qlncPlanSortInstant*)aCandPlan)->mChildPlanNode,
                                             aOutAggrExprList,
                                             aOutNestedAggrExprList,
                                             aEnv )
                     == STL_SUCCESS );

            break;
        case QLNC_PLAN_TYPE_SORT_GROUP_INSTANT:
        case QLNC_PLAN_TYPE_SORT_JOIN_INSTANT:
            STL_DASSERT( 0 );
            break;
        case QLNC_PLAN_TYPE_HASH_INSTANT:
            /* @todo 현재 Normal Hash Instant는 없다. */
            break;
        case QLNC_PLAN_TYPE_HASH_GROUP_INSTANT:
        case QLNC_PLAN_TYPE_GROUP:
            *aOutAggrExprList = NULL;
            *aOutNestedAggrExprList = &(aCandPlan->mNestedAggrExprList);

            break;
        case QLNC_PLAN_TYPE_HASH_JOIN_INSTANT:
            STL_DASSERT( 0 );
            break;
        /* case QLNC_PLAN_TYPE_SUB_QUERY_LIST: */
        /* case QLNC_PLAN_TYPE_SUB_QUERY_FUNCTION: */
        /* case QLNC_PLAN_TYPE_SUB_QUERY_FILTER: */
        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Table Stat 정보를 Table Stat List에 추가한다.
 * @param[in]   aTableStatList  Table Stat List
 * @param[in]   aPlanNode       Plan Node
 * @param[in]   aRegionMem      Region Memory
 * @param[in]   aEnv            Environment
 */
stlStatus qlncMCPAppendTableStat( qloValidTblStatList   * aTableStatList,
                                  qlncPlanCommon        * aPlanNode,
                                  knlRegionMem          * aRegionMem,
                                  qllEnv                * aEnv )
{
    ellDictHandle       * sTableHandle      = NULL;
    void                * sTablePhyHandle   = NULL;
    stlFloat64            sPageCount;
    qloValidTblStatItem * sTableStatItem    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aTableStatList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /* BASE TABLE에 대해서만 허용 */
    STL_TRY_THROW( ellGetTableType( ((qlncBaseTableNode*)(aPlanNode->mCandNode))->mTableInfo->mTableHandle )
                   == ELL_TABLE_TYPE_BASE_TABLE,
                   RAMP_FINISH );

    /* 정보 얻기 */
    if( aPlanNode->mPlanType == QLNC_PLAN_TYPE_TABLE_SCAN )
    {
        sTableHandle = ((qlncPlanTableScan*)aPlanNode)->mTableHandle;
        sTablePhyHandle = ((qlncPlanTableScan*)aPlanNode)->mPhysicalHandle;
        sPageCount = ((qlncBaseTableNode*)(aPlanNode->mCandNode))->mTableStat->mPageCount;
    }
    else if( aPlanNode->mPlanType == QLNC_PLAN_TYPE_INDEX_SCAN )
    {
        sTableHandle = ((qlncPlanIndexScan*)aPlanNode)->mTableHandle;
        sTablePhyHandle = ((qlncPlanIndexScan*)aPlanNode)->mTablePhysicalHandle;
        sPageCount = ((qlncBaseTableNode*)(aPlanNode->mCandNode))->mTableStat->mPageCount;
    }
    else if( aPlanNode->mPlanType == QLNC_PLAN_TYPE_INDEX_COMBINE )
    {
        sTableHandle = ((qlncPlanIndexCombine*)aPlanNode)->mTableHandle;
        sTablePhyHandle = ((qlncBaseTableNode*)(aPlanNode->mCandNode))->mTableInfo->mTablePhyHandle;
        sPageCount = ((qlncBaseTableNode*)(aPlanNode->mCandNode))->mTableStat->mPageCount;
    }
    else
    {
        STL_DASSERT( aPlanNode->mPlanType == QLNC_PLAN_TYPE_ROWID_SCAN );
        sTableHandle = ((qlncPlanRowidScan*)aPlanNode)->mTableHandle;
        sTablePhyHandle = ((qlncPlanRowidScan*)aPlanNode)->mPhysicalHandle;
        sPageCount = ((qlncBaseTableNode*)(aPlanNode->mCandNode))->mTableStat->mPageCount;
    }

    if( sPageCount == 0 )
    {
        sPageCount = 1; /* Default Page Count */
    }

    /* 이미 등록된 테이블 handle이 존재하는지 검사 */
    sTableStatItem = aTableStatList->mHead;
    while( sTableStatItem != NULL )
    {
        if( ellGetTableID( sTableStatItem->mTableHandle )
            == ellGetTableID( sTableHandle ) )
        {
            if( sTableStatItem->mPageCount < sPageCount )
            {
                sTableStatItem->mPageCount = sPageCount;
            }

            STL_THROW( RAMP_FINISH );
        }

        sTableStatItem = sTableStatItem->mNext;
    }

    /* 새로 추가 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qloValidTblStatItem ),
                                (void**) &sTableStatItem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTableStatItem->mTableHandle = sTableHandle;
    sTableStatItem->mTablePhyHandle = sTablePhyHandle;
    sTableStatItem->mPageCount = sPageCount;
    sTableStatItem->mNext = NULL;

    if( aTableStatList->mCount == 0 )
    {
        aTableStatList->mHead = sTableStatItem;
        aTableStatList->mTail = sTableStatItem;
    }
    else
    {
        aTableStatList->mTail->mNext = sTableStatItem;
        aTableStatList->mTail = sTableStatItem;
    }
    aTableStatList->mCount++;


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */

