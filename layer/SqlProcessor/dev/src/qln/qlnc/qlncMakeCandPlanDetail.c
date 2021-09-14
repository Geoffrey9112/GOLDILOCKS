/*******************************************************************************
 * qlncMakeCandPlanDetail.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlncMakeCandPlanDetail.c 9847 2013-10-10 03:02:05Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file qlncMakeCandPlanDetail.c
 * @brief SQL Processor Layer Make Candidate Plan Detail Info
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Make Candidate Plan Detail Function List
 */
const qlncMakeCandPlanDetailFuncPtr qlncMakeCandPlanDetailFuncList[ QLNC_PLAN_TYPE_MAX ] =
{
    qlncMakeCandPlanDetailQuerySet,         /**< QLNC_PLAN_TYPE_QUERY_SET */
    qlncMakeCandPlanDetailSetOP,            /**< QLNC_PLAN_TYPE_SET_OP */
    qlncMakeCandPlanDetailQuerySpec,        /**< QLNC_PLAN_TYPE_QUERY_SPEC */

    qlncMakeCandPlanDetailTableScan,        /**< QLNC_PLAN_TYPE_TABLE_SCAN */
    qlncMakeCandPlanDetailIndexScan,        /**< QLNC_PLAN_TYPE_INDEX_SCAN */
    qlncMakeCandPlanDetailIndexCombine,     /**< QLNC_PLAN_TYPE_INDEX_COMBINE */
    qlncMakeCandPlanDetailRowidScan,        /**< QLNC_PLAN_TYPE_ROWID_SCAN */

    qlncMakeCandPlanDetailSubTable,         /**< QLNC_PLAN_TYPE_FILTER */

    qlncMakeCandPlanDetailNestedJoin,       /**< QLNC_PLAN_TYPE_NESTED_JOIN */
    qlncMakeCandPlanDetailMergeJoin,        /**< QLNC_PLAN_TYPE_MERGE_JOIN */
    qlncMakeCandPlanDetailHashJoin,         /**< QLNC_PLAN_TYPE_HASH_JOIN */
    qlncMakeCandPlanDetailJoinCombine,      /**< QLNC_PLAN_TYPE_JOIN_COMBINE */

    qlncMakeCandPlanDetailSortInstant,      /**< QLNC_PLAN_TYPE_SORT_INSTANT */
    qlncMakeCandPlanDetailSortInstant,      /**< QLNC_PLAN_TYPE_SORT_GROUP_INSTANT */
    qlncMakeCandPlanDetailSortInstant,      /**< QLNC_PLAN_TYPE_SORT_JOIN_INSTANT */

    qlncMakeCandPlanDetailHashInstant,      /**< QLNC_PLAN_TYPE_HASH_INSTANT */
    qlncMakeCandPlanDetailHashInstant,      /**< QLNC_PLAN_TYPE_HASH_GROUP_INSTANT */
    qlncMakeCandPlanDetailHashInstant,      /**< QLNC_PLAN_TYPE_HASH_JOIN_INSTANT */

    NULL /* qlncMakeCandPlanDetailSubQueryList */,     /**< QLNC_PLAN_TYPE_SUB_QUERY_LIST */
    NULL /* qlncMakeCandPlanDetailSubQueryFunc */,     /**< QLNC_PLAN_TYPE_SUB_QUERY_FUNCTION */
    NULL /* qlncMakeCandPlanDetailSubQueryFilter */,   /**< QLNC_PLAN_TYPE_SUB_QUERY_FILTER */

    qlncMakeCandPlanDetailGroup             /**< QLNC_PLAN_TYPE_GROUP */
};


/**
 * @brief Query Set Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailQuerySet( qlncPlanDetailParamInfo   * aPlanDetailParamInfo,
                                          qlncPlanCommon            * aPlanNode,
                                          qlncPlanCommon           ** aOutPlanNode,
                                          qllEnv                    * aEnv )
{
    stlInt32                  i;
    qlncQuerySet            * sNodeQuerySet         = NULL;
    qlncPlanQuerySet        * sPlanQuerySet         = NULL;
    qlncPlanQuerySet        * sTempPlanQuerySet     = NULL;
    qlncPlanSetOP           * sPlanSetOP            = NULL;

    qlvRefExprItem          * sRefExprItem          = NULL;

    qlvInitExpression       * sInitExpr             = NULL;
    qlvInitExpression       * sOrgExpr              = NULL;
    qlvInitInnerColumn      * sInnerColumn          = NULL;
    qlvInitQuerySetNode     * sInitQuerySet         = NULL;
    qlncConvExprParamInfo     sConvExprParamInfo;

    qlvInitTarget           * sTarget               = NULL;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_QUERY_SET,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Query Set Node 설정 */
    sNodeQuerySet = (qlncQuerySet*)(aPlanNode->mCandNode);

    /* Query Set Plan 설정 */
    sPlanQuerySet = (qlncPlanQuerySet*)aPlanNode;

    /* Child Node 수행 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanQuerySet->mChildPlanNode->mPlanType ](
                 aPlanDetailParamInfo,
                 sPlanQuerySet->mChildPlanNode,
                 &(sPlanQuerySet->mChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    /* Plan Node Map Dynamic Array에 현재 Plan Node를 추가 */
    STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             aPlanNode,
                                             aEnv )
             == STL_SUCCESS );

    /* Read Column Map List 설정 */
    STL_DASSERT( sNodeQuerySet->mSetTargetCount > 0 );
    STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mCandidateMem,
                                STL_SIZEOF( qlncReadColMapList ),
                                (void**) &(sPlanQuerySet->mReadColMapList),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_LIST( sPlanQuerySet->mReadColMapList );

    /* Target Count 설정 */
    sPlanQuerySet->mTargetCount = sNodeQuerySet->mSetTargetCount;

    /* Column List 정보 설정 */
    if( sPlanQuerySet->mRootQuerySet->mColumnList == NULL )
    {
        /* @todo Set OP쪽에서 처리하기 때문에 여기에 들어오는 경우가 없을 것으로 판단됨 */

        /* Root Query Set에 Column List를 생성 */
        STL_TRY( qlvCreateRefExprList( &(sPlanQuerySet->mRootQuerySet->mColumnList),
                                       aPlanDetailParamInfo->mRegionMem,
                                       aEnv )
                 == STL_SUCCESS );

        /* Internal Read Column Map List 설정 */
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mCandidateMem,
                                    STL_SIZEOF( qlncReadColMapList ),
                                    (void**) &(sPlanQuerySet->mRootQuerySet->mInternalReadColMapList),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_LIST( sPlanQuerySet->mRootQuerySet->mInternalReadColMapList );

        /* Left Most Query Spec 찾기 */
        if( sPlanQuerySet->mRootQuerySet->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SET_OP )
        {
            sPlanSetOP = (qlncPlanSetOP*)(sPlanQuerySet->mRootQuerySet->mChildPlanNode);
        }
        else
        {
            STL_DASSERT( sPlanQuerySet->mRootQuerySet->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SORT_INSTANT );
            STL_DASSERT( ((qlncPlanSortInstant*)(sPlanQuerySet->mRootQuerySet->mChildPlanNode))->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SET_OP );
            sPlanSetOP = (qlncPlanSetOP*)(((qlncPlanSortInstant*)(sPlanQuerySet->mRootQuerySet->mChildPlanNode))->mChildPlanNode);
        }

        while( STL_TRUE )
        {
            if( sPlanSetOP->mChildPlanNodeArr[0]->mPlanType == QLNC_PLAN_TYPE_SET_OP )
            {
                sPlanSetOP = (qlncPlanSetOP*)(sPlanSetOP->mChildPlanNodeArr[0]);
            }
            else if( sPlanSetOP->mChildPlanNodeArr[0]->mPlanType == QLNC_PLAN_TYPE_QUERY_SET )
            {
                sTempPlanQuerySet = (qlncPlanQuerySet*)(sPlanSetOP->mChildPlanNodeArr[0]);
 
                if( sTempPlanQuerySet->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SET_OP )
                {
                    sPlanSetOP = (qlncPlanSetOP*)(sTempPlanQuerySet->mChildPlanNode);
                }
                else
                {
                    STL_DASSERT( sTempPlanQuerySet->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SORT_INSTANT );
                    STL_DASSERT( ((qlncPlanSortInstant*)(sTempPlanQuerySet->mChildPlanNode))->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SET_OP );
                    sPlanSetOP = (qlncPlanSetOP*)(((qlncPlanSortInstant*)(sTempPlanQuerySet->mChildPlanNode))->mChildPlanNode);
                }
            }
            else
            {
                break;
            }
        }

        STL_DASSERT( sPlanSetOP->mChildPlanNodeArr[0]->mPlanType == QLNC_PLAN_TYPE_QUERY_SPEC );

        /* Query Spec의 Target을 Inner Column으로 감싸서 Set Column으로 만듬 */
        i = 0;
        sRefExprItem = ((qlncPlanQuerySpec*)(sPlanSetOP->mChildPlanNodeArr[0]))->mTargetColList->mHead;
        while( sRefExprItem != NULL )
        {
            /* Inner Column 생성 */
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitInnerColumn ),
                                        (void**) &sInnerColumn,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sInnerColumn->mRelationNode = sPlanQuerySet->mRootQuerySet->mCommon.mInitNode;
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( stlInt32 ),
                                        (void**) &(sInnerColumn->mIdx),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            *(sInnerColumn->mIdx) = i;
            sInnerColumn->mOrgExpr = sRefExprItem->mExprPtr;

            /* Expression 할당 및 설정 */
            QLNC_MAKE_INIT_EXPRESSION( aPlanDetailParamInfo->mRegionMem,
                                       QLV_EXPR_TYPE_INNER_COLUMN,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       sRefExprItem->mExprPtr->mIterationTime,
                                       sRefExprItem->mExprPtr->mPosition,
                                       sRefExprItem->mExprPtr->mPhraseType,
                                       0,
                                       sRefExprItem->mExprPtr->mColumnName,
                                       sInnerColumn,
                                       sInitExpr,
                                       aEnv );

            /* Column List에 등록 */
            STL_TRY( qlvAddExprToRefExprList( sPlanQuerySet->mRootQuerySet->mColumnList,
                                              sInitExpr,
                                              STL_FALSE,
                                              aPlanDetailParamInfo->mRegionMem,
                                              aEnv )
                     == STL_SUCCESS );

            /* Read Column Map List에 등록 */
            QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                      sPlanQuerySet->mRootQuerySet->mInternalReadColMapList,
                                      ((qlncQuerySet*)(sPlanQuerySet->mRootQuerySet->mCommon.mCandNode))->mSetColumnArr[i].mExpr,
                                      sInitExpr,
                                      aEnv );

            i++;
            sRefExprItem = sRefExprItem->mNext;
        }
    }

    STL_DASSERT( sPlanQuerySet->mRootQuerySet->mColumnList != NULL );
    STL_DASSERT( sPlanQuerySet->mRootQuerySet->mColumnList->mCount > 0 );
    sPlanQuerySet->mColumnList = sPlanQuerySet->mRootQuerySet->mColumnList;

    /* Target List 정보 설정 */
    STL_TRY( qlvCreateRefExprList( &(sPlanQuerySet->mTargetList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    if( sPlanQuerySet->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SET_OP )
    {
        /* Column List들을 Inner Column으로 감싸서 Target List를 생성 */
        i = 0;
        sRefExprItem = sPlanQuerySet->mColumnList->mHead;
        while( sRefExprItem != NULL )
        {
            /* Inner Column 생성 */
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitInnerColumn ),
                                        (void**) &sInnerColumn,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sInnerColumn->mRelationNode = sPlanQuerySet->mCommon.mInitNode;
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( stlInt32 ),
                                        (void**) &(sInnerColumn->mIdx),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            *(sInnerColumn->mIdx) = i;
            sInnerColumn->mOrgExpr = sRefExprItem->mExprPtr;

            /* Expression 할당 및 설정 */
            QLNC_MAKE_INIT_EXPRESSION( aPlanDetailParamInfo->mRegionMem,
                                       QLV_EXPR_TYPE_INNER_COLUMN,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       sRefExprItem->mExprPtr->mIterationTime,
                                       sRefExprItem->mExprPtr->mPosition,
                                       sRefExprItem->mExprPtr->mPhraseType,
                                       0,
                                       sRefExprItem->mExprPtr->mColumnName,
                                       sInnerColumn,
                                       sInitExpr,
                                       aEnv );

            /* Target List에 등록 */
            STL_TRY( qlvAddExprToRefExprList( sPlanQuerySet->mTargetList,
                                              sInitExpr,
                                              STL_FALSE,
                                              aPlanDetailParamInfo->mRegionMem,
                                              aEnv )
                     == STL_SUCCESS );

            /* Read Column Map List에 등록 */
            QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                      sPlanQuerySet->mReadColMapList,
                                      sNodeQuerySet->mSetTargetArr[i].mExpr,
                                      sInitExpr,
                                      aEnv );

            i++;
            sRefExprItem = sRefExprItem->mNext;
        }
    }
    else
    {
        /* 하위 노드에 Order By를 위한 Sort Instant가 존재 */
        STL_DASSERT( sPlanQuerySet->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SORT_INSTANT );

        for( i = 0; i < sPlanQuerySet->mTargetCount; i++ )
        {
            QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                &sConvExprParamInfo,
                &(aPlanDetailParamInfo->mParamInfo),
                aPlanDetailParamInfo->mRegionMem,
                aPlanDetailParamInfo->mCandidateMem,
                aPlanDetailParamInfo->mInitExprList,
                aPlanDetailParamInfo->mTableStatList,
                aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
                aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
                0,
                NULL,
                (qlncPlanCommon*)(sPlanQuerySet->mChildPlanNode),
                aPlanDetailParamInfo->mPlanNodeMapDynArr,
                NULL,
                STL_TRUE,
                sNodeQuerySet->mSetTargetArr[i].mExpr );

            STL_TRY( qlncConvertCandExprToInitExprFuncList[sNodeQuerySet->mSetTargetArr[i].mExpr->mType](
                         &sConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );
            sOrgExpr = sConvExprParamInfo.mInitExpr;

            /* Inner Column 생성 */
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitInnerColumn ),
                                        (void**) &sInnerColumn,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sInnerColumn->mRelationNode = sPlanQuerySet->mCommon.mInitNode;
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( stlInt32 ),
                                        (void**) &(sInnerColumn->mIdx),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            *(sInnerColumn->mIdx) = i;
            sInnerColumn->mOrgExpr = sOrgExpr;

            /* Expression 할당 및 설정 */
            QLNC_MAKE_INIT_EXPRESSION( aPlanDetailParamInfo->mRegionMem,
                                       QLV_EXPR_TYPE_INNER_COLUMN,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       sOrgExpr->mIterationTime,
                                       sOrgExpr->mPosition,
                                       sOrgExpr->mPhraseType,
                                       0,
                                       sOrgExpr->mColumnName,
                                       sInnerColumn,
                                       sInitExpr,
                                       aEnv );

            /* Target Column List에 추가 */
            STL_TRY( qlvAddExprToRefExprList( sPlanQuerySet->mTargetList,
                                              sInitExpr,
                                              STL_FALSE,
                                              aPlanDetailParamInfo->mRegionMem,
                                              aEnv )
                     == STL_SUCCESS );

            /* Read Column Map List에 등록 */
            QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                      sPlanQuerySet->mReadColMapList,
                                      sNodeQuerySet->mSetTargetArr[i].mExpr,
                                      sInitExpr,
                                      aEnv );
        }
    }

    /* Offset 정보 설정 */
    if( sNodeQuerySet->mOffset == NULL )
    {
        sPlanQuerySet->mOffset = NULL;
    }
    else
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanQuerySet,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            NULL,
            STL_TRUE,
            sNodeQuerySet->mOffset );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[sNodeQuerySet->mOffset->mType](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );
        sPlanQuerySet->mOffset = sConvExprParamInfo.mInitExpr;
    }

    /* Limit 정보 설정 */
    if( sNodeQuerySet->mLimit == NULL )
    {
        sPlanQuerySet->mLimit = NULL;
    }
    else
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanQuerySet,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            NULL,
            STL_TRUE,
            sNodeQuerySet->mLimit );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[sNodeQuerySet->mLimit->mType](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );
        sPlanQuerySet->mLimit = sConvExprParamInfo.mInitExpr;
    }

    /* target 정보 설정 */
    STL_DASSERT( sNodeQuerySet->mSetTargetCount > 0 );
    
    STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitTarget ) * sNodeQuerySet->mSetTargetCount,
                                (void**) &sTarget,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sRefExprItem = sPlanQuerySet->mTargetList->mHead;
    for( i = 0; i < sNodeQuerySet->mSetTargetCount; i++ )
    {
        QLNC_ALLOC_AND_COPY_STRING( sTarget[i].mDisplayName,
                                    sNodeQuerySet->mSetTargetArr[i].mDisplayName,
                                    aPlanDetailParamInfo->mRegionMem,
                                    aEnv );

        QLNC_ALLOC_AND_COPY_STRING( sTarget[i].mAliasName,
                                    sNodeQuerySet->mSetTargetArr[i].mAliasName,
                                    aPlanDetailParamInfo->mRegionMem,
                                    aEnv );
        
        sTarget[i].mDisplayPos = sNodeQuerySet->mSetTargetArr[i].mDisplayPos;
        sTarget[i].mExpr = sRefExprItem->mExprPtr;

        if( aPlanDetailParamInfo->mParamInfo.mSQLStmt->mBindCount == 0 )
        {
            sTarget[i].mDataTypeInfo = sNodeQuerySet->mSetTargetArr[i].mDataTypeInfo;

            sTarget[i].mDataTypeInfo.mIsBaseColumn = STL_FALSE;
            sTarget[i].mDataTypeInfo.mUpdatable    = STL_FALSE;
        }
        else
        {
            sTarget[i].mDataTypeInfo.mIsBaseColumn = STL_FALSE;
            sTarget[i].mDataTypeInfo.mNullable     = STL_TRUE;
            sTarget[i].mDataTypeInfo.mUpdatable    = STL_FALSE;
            sTarget[i].mDataTypeInfo.mIsIgnore     = STL_TRUE;

            sTarget[i].mDataTypeInfo.mDataType          = DTL_TYPE_VARCHAR;
            sTarget[i].mDataTypeInfo.mArgNum1           = 1;
            sTarget[i].mDataTypeInfo.mArgNum2           = gResultDataTypeDef[ DTL_TYPE_VARCHAR ].mArgNum2;
            sTarget[i].mDataTypeInfo.mStringLengthUnit  = gResultDataTypeDef[ DTL_TYPE_VARCHAR ].mStringLengthUnit;
            sTarget[i].mDataTypeInfo.mIntervalIndicator = gResultDataTypeDef[ DTL_TYPE_VARCHAR ].mIntervalIndicator;
        }
        
        sRefExprItem = sRefExprItem->mNext;
    }

    /* 하위 SET OP 노드 탐색해가며 Result Type Combiantion 수행 */
    if( aPlanDetailParamInfo->mParamInfo.mSQLStmt->mBindCount > 0 )
    {
        STL_TRY( qlncGetQuerySetTargetDataTypeInfo( aPlanDetailParamInfo,
                                                    sPlanQuerySet->mChildPlanNode,
                                                    sNodeQuerySet->mSetTargetCount,
                                                    sTarget,
                                                    aEnv )
                 == STL_SUCCESS );
    }

    /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
    STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             1,
                                             aEnv )
             == STL_SUCCESS );

    /* Target 정보 설정 */
    sPlanQuerySet->mSetTargets = sTarget;
    
    /* Init Node 정보 설정 */
    sInitQuerySet = (qlvInitQuerySetNode *) sPlanQuerySet->mCommon.mInitNode;
    sInitQuerySet->mIsRootSet = sPlanQuerySet->mIsRootSet;
    sInitQuerySet->mIsPureSet = STL_TRUE;  /* default */

    sInitQuerySet->mSetType = QLV_SET_TYPE_NA;  /* default */
    sInitQuerySet->mSetOption = QLV_SET_OPTION_NA;  /* default */
    
    sInitQuerySet->mSameSetChildCnt = 1;  /* default */
    
    sInitQuerySet->mQueryExprList = NULL;  /* default */

    sInitQuerySet->mSetTargetCount = sPlanQuerySet->mTargetCount;
    sInitQuerySet->mSetTargets = sTarget;
    sInitQuerySet->mSetColumns = NULL;  /* default */
    sInitQuerySet->mLeftQueryNode = NULL;  /* default */
    sInitQuerySet->mRightQueryNode = NULL;  /* default */

    sInitQuerySet->mCorrespondingSpec = NULL;  /* default */

    sInitQuerySet->mWithExpr = NULL;  /* default */
    sInitQuerySet->mOrderBy = NULL;  /* default */
    sInitQuerySet->mResultSkip = sPlanQuerySet->mOffset;
    sInitQuerySet->mResultLimit = sPlanQuerySet->mLimit;
    sInitQuerySet->mSkipCnt = 0;  /* default */
    sInitQuerySet->mFetchCnt = QLL_FETCH_COUNT_MAX;  /* default */


    /**
     * Output 설정
     */

    *aOutPlanNode = (qlncPlanCommon*)sPlanQuerySet;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Set OP Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailSetOP( qlncPlanDetailParamInfo      * aPlanDetailParamInfo,
                                       qlncPlanCommon               * aPlanNode,
                                       qlncPlanCommon              ** aOutPlanNode,
                                       qllEnv                       * aEnv )
{
    stlInt32              i;
    qlncPlanSetOP       * sPlanSetOP        = NULL;
    qlncPlanQuerySet    * sTempPlanQuerySet = NULL;
    qlncPlanSetOP       * sTmpPlanSetOP     = NULL;
    qlvRefExprItem      * sRefExprItem      = NULL;
    qlvInitExpression   * sInitExpr         = NULL;
    qlvInitInnerColumn  * sInnerColumn      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_SET_OP,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Set OP Plan 설정 */
    sPlanSetOP = (qlncPlanSetOP*)aPlanNode;

    /* Child Node Array 수행 */
    for( i = 0; i < sPlanSetOP->mChildCount; i++ )
    {
        STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanSetOP->mChildPlanNodeArr[i]->mPlanType ](
                     aPlanDetailParamInfo,
                     sPlanSetOP->mChildPlanNodeArr[i],
                     &(sPlanSetOP->mChildPlanNodeArr[i]),
                     aEnv )
                 == STL_SUCCESS );
    }

    /* Column List 정보 설정 */
    if( sPlanSetOP->mRootQuerySet->mColumnList == NULL )
    {
        /* Root Query Set에 Column List를 생성 */
        STL_TRY( qlvCreateRefExprList( &(sPlanSetOP->mRootQuerySet->mColumnList),
                                       aPlanDetailParamInfo->mRegionMem,
                                       aEnv )
                 == STL_SUCCESS );

        /* Internal Read Column Map List 설정 */
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mCandidateMem,
                                    STL_SIZEOF( qlncReadColMapList ),
                                    (void**) &(sPlanSetOP->mRootQuerySet->mInternalReadColMapList),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_LIST( sPlanSetOP->mRootQuerySet->mInternalReadColMapList );

        /* Left Most Query Spec 찾기 */
        if( sPlanSetOP->mRootQuerySet->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SET_OP )
        {
            sTmpPlanSetOP = (qlncPlanSetOP*)(sPlanSetOP->mRootQuerySet->mChildPlanNode);
        }
        else
        {
            STL_DASSERT( sPlanSetOP->mRootQuerySet->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SORT_INSTANT );
            STL_DASSERT( ((qlncPlanSortInstant*)(sPlanSetOP->mRootQuerySet->mChildPlanNode))->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SET_OP );
            sTmpPlanSetOP = (qlncPlanSetOP*)(((qlncPlanSortInstant*)(sPlanSetOP->mRootQuerySet->mChildPlanNode))->mChildPlanNode);
        }

        while( STL_TRUE )
        {
            if( sTmpPlanSetOP->mChildPlanNodeArr[0]->mPlanType == QLNC_PLAN_TYPE_SET_OP )
            {
                sTmpPlanSetOP = (qlncPlanSetOP*)(sTmpPlanSetOP->mChildPlanNodeArr[0]);
            }
            else if( sTmpPlanSetOP->mChildPlanNodeArr[0]->mPlanType == QLNC_PLAN_TYPE_QUERY_SET )
            {
                sTempPlanQuerySet = (qlncPlanQuerySet*)(sTmpPlanSetOP->mChildPlanNodeArr[0]);
 
                if( sTempPlanQuerySet->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SET_OP )
                {
                    sTmpPlanSetOP = (qlncPlanSetOP*)(sTempPlanQuerySet->mChildPlanNode);
                }
                else
                {
                    STL_DASSERT( sTempPlanQuerySet->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SORT_INSTANT );
                    STL_DASSERT( ((qlncPlanSortInstant*)(sTempPlanQuerySet->mChildPlanNode))->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SET_OP );
                    sTmpPlanSetOP = (qlncPlanSetOP*)(((qlncPlanSortInstant*)(sTempPlanQuerySet->mChildPlanNode))->mChildPlanNode);
                }
            }
            else
            {
                break;
            }
        }

        STL_DASSERT( sTmpPlanSetOP->mChildPlanNodeArr[0]->mPlanType == QLNC_PLAN_TYPE_QUERY_SPEC );

        /* Query Spec의 Target을 Inner Column으로 감싸서 Set Column으로 만듬 */
        i = 0;
        sRefExprItem = ((qlncPlanQuerySpec*)(sTmpPlanSetOP->mChildPlanNodeArr[0]))->mTargetColList->mHead;
        while( sRefExprItem != NULL )
        {
            /* Inner Column 생성 */
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitInnerColumn ),
                                        (void**) &sInnerColumn,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sInnerColumn->mRelationNode = sPlanSetOP->mRootQuerySet->mCommon.mInitNode;
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( stlInt32 ),
                                        (void**) &(sInnerColumn->mIdx),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            *(sInnerColumn->mIdx) = i;
            sInnerColumn->mOrgExpr = sRefExprItem->mExprPtr;

            /* Expression 할당 및 설정 */
            QLNC_MAKE_INIT_EXPRESSION( aPlanDetailParamInfo->mRegionMem,
                                       QLV_EXPR_TYPE_INNER_COLUMN,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       sRefExprItem->mExprPtr->mIterationTime,
                                       sRefExprItem->mExprPtr->mPosition,
                                       sRefExprItem->mExprPtr->mPhraseType,
                                       0,
                                       sRefExprItem->mExprPtr->mColumnName,
                                       sInnerColumn,
                                       sInitExpr,
                                       aEnv );

            /* Column List에 등록 */
            STL_TRY( qlvAddExprToRefExprList( sPlanSetOP->mRootQuerySet->mColumnList,
                                              sInitExpr,
                                              STL_FALSE,
                                              aPlanDetailParamInfo->mRegionMem,
                                              aEnv )
                     == STL_SUCCESS );

            /* Read Column Map List에 등록 */
            QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                      sPlanSetOP->mRootQuerySet->mInternalReadColMapList,
                                      ((qlncQuerySet*)(sPlanSetOP->mRootQuerySet->mCommon.mCandNode))->mSetColumnArr[i].mExpr,
                                      sInitExpr,
                                      aEnv );

            i++;
            sRefExprItem = sRefExprItem->mNext;
        }
    }

    /* Read Column Map List 설정 */
    sPlanSetOP->mReadColMapList = sPlanSetOP->mRootQuerySet->mInternalReadColMapList;

    /* Init Node 정보 설정 */
    /* Do Nothing */


    /**
     * Output 설정
     */

    *aOutPlanNode = (qlncPlanCommon*)sPlanSetOP;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Spec Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailQuerySpec( qlncPlanDetailParamInfo  * aPlanDetailParamInfo,
                                           qlncPlanCommon           * aPlanNode,
                                           qlncPlanCommon          ** aOutPlanNode,
                                           qllEnv                   * aEnv )
{
    stlInt32                  i;
    qlncQuerySpec           * sNodeQuerySpec        = NULL;
    qlncPlanQuerySpec       * sPlanQuerySpec        = NULL;
    qlvInitExpression       * sInitExpr             = NULL;
    qlvInitExpression       * sOrgExpr              = NULL;
    qlvInitInnerColumn      * sInnerColumn          = NULL;

    qlvInitExprList         * sInitExprList         = NULL;
    qlvInitExprList         * sBackupPrevExprList   = NULL;
    qlvInitQuerySpecNode    * sInitQuerySpec        = NULL;
    qlncPlanCommon          * sChildPlanNode        = NULL;
    qlncConvExprParamInfo     sConvExprParamInfo;
    qlvRefExprList            sTmpSubQueryExprList;
    qlvRefExprItem          * sRefExprItem          = NULL;

    qlvInitTarget           * sTarget               = NULL;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_QUERY_SPEC,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Query Spec Node 설정 */
    sNodeQuerySpec = (qlncQuerySpec*)(aPlanNode->mCandNode);

    /* Query Spec Plan 설정 */
    sPlanQuerySpec = (qlncPlanQuerySpec*)aPlanNode;

    /* Init Expression List 생성 및 초기화 */
    STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitExprList ),
                                (void**) &sInitExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( &sInitExprList->mPseudoColExprList,
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Aggregation Node 찾기 */
    STL_TRY( qlncMCPSetAggrExprList( (qlncPlanCommon*)sPlanQuerySpec,
                                     &(sInitExprList->mAggrExprList),
                                     &(sInitExprList->mNestedAggrExprList),
                                     aEnv )
             == STL_SUCCESS );

    sInitExprList->mStmtExprList = aPlanDetailParamInfo->mInitExprList->mStmtExprList;

    sPlanQuerySpec->mInitExprList = sInitExprList;

    /* 기존 Init Expression List 백업 및 새로운 Init Expression List 교체 */
    sBackupPrevExprList = aPlanDetailParamInfo->mInitExprList;
    aPlanDetailParamInfo->mInitExprList = sInitExprList;

    /* Child Node 수행 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanQuerySpec->mChildPlanNode->mPlanType ](
                 aPlanDetailParamInfo,
                 sPlanQuerySpec->mChildPlanNode,
                 &(sPlanQuerySpec->mChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    /* Plan Node Map Dynamic Array에 Child Plan Node를 추가 */
    STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             sPlanQuerySpec->mChildPlanNode,
                                             aEnv )
             == STL_SUCCESS );

    /* Read Column Map List 설정 */
    STL_DASSERT( sNodeQuerySpec->mTargetCount > 0 );
    STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mCandidateMem,
                                STL_SIZEOF( qlncReadColMapList ),
                                (void**) &(sPlanQuerySpec->mReadColMapList),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_LIST( sPlanQuerySpec->mReadColMapList );

    /* Target 정보 설정 */
    STL_TRY( qlvCreateRefExprList( &(sPlanQuerySpec->mTargetColList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    sPlanQuerySpec->mTargetCount = sNodeQuerySpec->mTargetCount;
    for( i = 0; i < sPlanQuerySpec->mTargetCount; i++ )
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)(sPlanQuerySpec->mChildPlanNode),
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            sNodeQuerySpec->mTargetArr[i].mExpr );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[sNodeQuerySpec->mTargetArr[i].mExpr->mType](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );
        sOrgExpr = sConvExprParamInfo.mInitExpr;

        /* Inner Column 생성 */
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitInnerColumn ),
                                    (void**) &sInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sInnerColumn->mRelationNode = sPlanQuerySpec->mCommon.mInitNode;
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( stlInt32 ),
                                    (void**) &(sInnerColumn->mIdx),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        *(sInnerColumn->mIdx) = i;
        sInnerColumn->mOrgExpr = sOrgExpr;

        /* Expression 할당 및 설정 */
        QLNC_MAKE_INIT_EXPRESSION( aPlanDetailParamInfo->mRegionMem,
                                   QLV_EXPR_TYPE_INNER_COLUMN,
                                   KNL_BUILTIN_FUNC_INVALID,
                                   sOrgExpr->mIterationTime,
                                   sOrgExpr->mPosition,
                                   sOrgExpr->mPhraseType,
                                   0,
                                   sOrgExpr->mColumnName,
                                   sInnerColumn,
                                   sInitExpr,
                                   aEnv );

        /* Target Column List에 추가 */
        STL_TRY( qlvAddExprToRefExprList( sPlanQuerySpec->mTargetColList,
                                          sInitExpr,
                                          STL_FALSE,
                                          aPlanDetailParamInfo->mRegionMem,
                                          aEnv )
                 == STL_SUCCESS );

        /* Read Column Map List에 등록 */
        QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                  sPlanQuerySpec->mReadColMapList,
                                  sNodeQuerySpec->mTargetArr[i].mExpr,
                                  sInitExpr,
                                  aEnv );
    }

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sTmpSubQueryExprList.mCount > 0 )
    {
        /* 하위 노드로 SubQuery Plan을 생성 */
        STL_TRY( qlncMakeCandPlanSubQueryList( aPlanDetailParamInfo->mRegionMem,
                                               aPlanDetailParamInfo->mCandidateMem,
                                               sPlanQuerySpec->mChildPlanNode,
                                               &sTmpSubQueryExprList,
                                               &(sPlanQuerySpec->mChildPlanNode),
                                               aEnv )
                 == STL_SUCCESS );
    }

    /* Offset 정보 설정 */
    if( sNodeQuerySpec->mOffset == NULL )
    {
        sPlanQuerySpec->mOffset = NULL;
    }
    else
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanQuerySpec,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            NULL,
            STL_TRUE,
            sNodeQuerySpec->mOffset );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[sNodeQuerySpec->mOffset->mType](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );
        sPlanQuerySpec->mOffset = sConvExprParamInfo.mInitExpr;
    }

    /* Limit 정보 설정 */
    if( sNodeQuerySpec->mLimit == NULL )
    {
        sPlanQuerySpec->mLimit = NULL;
    }
    else
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanQuerySpec,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            NULL,
            STL_TRUE,
            sNodeQuerySpec->mLimit );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[sNodeQuerySpec->mLimit->mType](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );
        sPlanQuerySpec->mLimit = sConvExprParamInfo.mInitExpr;
    }

    /* target 정보 설정 */
    STL_DASSERT( sNodeQuerySpec->mTargetCount > 0 );
    
    STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitTarget ) * sNodeQuerySpec->mTargetCount,
                                (void**) &sTarget,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sRefExprItem = sPlanQuerySpec->mTargetColList->mHead;
    for( i = 0; i < sNodeQuerySpec->mTargetCount; i++ )
    {
        QLNC_ALLOC_AND_COPY_STRING( sTarget[i].mDisplayName,
                                    sNodeQuerySpec->mTargetArr[i].mDisplayName,
                                    aPlanDetailParamInfo->mRegionMem,
                                    aEnv );

        QLNC_ALLOC_AND_COPY_STRING( sTarget[i].mAliasName,
                                    sNodeQuerySpec->mTargetArr[i].mAliasName,
                                    aPlanDetailParamInfo->mRegionMem,
                                    aEnv );
        
        sTarget[i].mDisplayPos = sNodeQuerySpec->mTargetArr[i].mDisplayPos;
        sTarget[i].mExpr = sRefExprItem->mExprPtr;

        if( aPlanDetailParamInfo->mParamInfo.mSQLStmt->mBindCount == 0 )
        {
            sTarget[i].mDataTypeInfo = sNodeQuerySpec->mTargetArr[i].mDataTypeInfo;
        }
        else
        {
            STL_TRY( qlvGetExprDataTypeInfo( aPlanDetailParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                             aPlanDetailParamInfo->mCandidateMem,
                                             sTarget[i].mExpr->mExpr.mInnerColumn->mOrgExpr,
                                             aPlanDetailParamInfo->mParamInfo.mSQLStmt->mBindContext,
                                             & sTarget[i].mDataTypeInfo,
                                             aEnv )
                     == STL_SUCCESS );
        }
        
        sRefExprItem = sRefExprItem->mNext;
    }

    /* Init Expression List 재설정 */
    aPlanDetailParamInfo->mInitExprList = sBackupPrevExprList;

    /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
    STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             1,
                                             aEnv )
             == STL_SUCCESS );

    /* Init Node 정보 설정 */
    sInitQuerySpec = (qlvInitQuerySpecNode *) sPlanQuerySpec->mCommon.mInitNode;
    
    sChildPlanNode = sPlanQuerySpec->mChildPlanNode;
    while( 1 )
    {
        switch( sChildPlanNode->mPlanType )
        {
            case QLNC_PLAN_TYPE_TABLE_SCAN :
            case QLNC_PLAN_TYPE_INDEX_SCAN :
            case QLNC_PLAN_TYPE_ROWID_SCAN :
            case QLNC_PLAN_TYPE_SUB_TABLE :
            case QLNC_PLAN_TYPE_NESTED_JOIN :
            case QLNC_PLAN_TYPE_MERGE_JOIN :
            case QLNC_PLAN_TYPE_HASH_JOIN :
            case QLNC_PLAN_TYPE_JOIN_COMBINE :
                STL_THROW( RAMP_SEARCHED_TABLE_NODE );
                break;
            case QLNC_PLAN_TYPE_INDEX_COMBINE :
                sChildPlanNode = ((qlncPlanIndexCombine *)sChildPlanNode)->mChildPlanArr[0];
                break;
            case QLNC_PLAN_TYPE_SORT_INSTANT :
            case QLNC_PLAN_TYPE_SORT_GROUP_INSTANT :
            case QLNC_PLAN_TYPE_SORT_JOIN_INSTANT :
                sChildPlanNode = ((qlncPlanSortInstant *)sChildPlanNode)->mChildPlanNode;
                break;
            case QLNC_PLAN_TYPE_HASH_INSTANT :
            case QLNC_PLAN_TYPE_HASH_GROUP_INSTANT :
            case QLNC_PLAN_TYPE_HASH_JOIN_INSTANT :
            case QLNC_PLAN_TYPE_GROUP :
                sChildPlanNode = ((qlncPlanHashInstant *)sChildPlanNode)->mChildPlanNode;
                break;
            case QLNC_PLAN_TYPE_SUB_QUERY_FILTER :
                sChildPlanNode = ((qlncPlanSubQueryFilter *)sChildPlanNode)->mChildPlanNodes[ 0 ];
                break;
            case QLNC_PLAN_TYPE_SUB_QUERY_LIST :
                sChildPlanNode = ((qlncPlanSubQueryList *)sChildPlanNode)->mChildPlanNode;
                break;
            default :
                STL_DASSERT( 0 );
                break;
        }
    }
    STL_RAMP( RAMP_SEARCHED_TABLE_NODE );

    sInitQuerySpec->mTableNode = sChildPlanNode->mInitNode;
    
    sInitQuerySpec->mTableCount = 1;  /* default */

    sInitQuerySpec->mQueryExprList = sPlanQuerySpec->mInitExprList;  /* default */

    sInitQuerySpec->mHintInfo = NULL;  /* default */

    sInitQuerySpec->mTargetCount = sPlanQuerySpec->mTargetCount;
    sInitQuerySpec->mTargets = sTarget;
    sInitQuerySpec->mDistinct = QLV_SET_TYPE_NA;  /* default */
    sInitQuerySpec->mIsDistinct = STL_FALSE;  /* default */

    sInitQuerySpec->mHasAggrDistinct = STL_FALSE;  /* default */
    sInitQuerySpec->mHasNestedAggrDistinct = STL_FALSE;  /* default */
    
    /* sInitQuerySpec->mWhereExpr = NULL;  /\* default *\/ */
    
    sInitQuerySpec->mGroupBy = NULL;  /* default */
    sInitQuerySpec->mHasGroupOper = STL_FALSE;  /* default */
    
    /* sInitQuerySpec->mHavingExpr = NULL;  /\* default *\/ */

    sInitQuerySpec->mOrderBy = NULL;  /* default */

    sInitQuerySpec->mWindowExpr = NULL;  /* default */

    sInitQuerySpec->mWithExpr = NULL;  /* default */
    sInitQuerySpec->mResultSkip = sPlanQuerySpec->mOffset;
    sInitQuerySpec->mResultLimit = sPlanQuerySpec->mLimit;
    sInitQuerySpec->mSkipCnt = 0;  /* default */
    sInitQuerySpec->mFetchCnt = QLL_FETCH_COUNT_MAX;  /* default */


    /**
     * Output 설정
     */

    *aOutPlanNode = (qlncPlanCommon*)sPlanQuerySpec;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Table Scan Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailTableScan( qlncPlanDetailParamInfo  * aPlanDetailParamInfo,
                                           qlncPlanCommon           * aPlanNode,
                                           qlncPlanCommon          ** aOutPlanNode,
                                           qllEnv                   * aEnv )
{
    qlncPlanCommon          * sOutPlanNode          = NULL;
    qlncPlanTableScan       * sPlanTableScan        = NULL;
    qlvRefExprList            sTmpSubQueryExprList;
    qlvInitBaseTableNode    * sInitBaseTable        = NULL;
    qlncSubQueryAndFilter   * sSubQueryFilter       = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_TABLE_SCAN,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Table Scan Plan 설정 */
    sPlanTableScan = (qlncPlanTableScan*)aPlanNode;

    /* 기타 변수 초기화 */
    STL_TRY( qlvCreateRefExprList( &(sPlanTableScan->mReadColList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    sPlanTableScan->mRowidColumn = NULL;
    sPlanTableScan->mReadColMapList = NULL;

    /* Table Stat 설정 */
    STL_DASSERT( aPlanNode->mCandNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE );
    STL_TRY( qlncMCPAppendTableStat( aPlanDetailParamInfo->mTableStatList,
                                     aPlanNode,
                                     aPlanDetailParamInfo->mRegionMem,
                                     aEnv )
             == STL_SUCCESS );

    /* Filter Expression 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( ((qlncTableScanCost*)(sPlanTableScan->mCommon.mCandNode->mBestCost))->mTableAndFilter != NULL )
    {
        qlncConvExprParamInfo     sConvExprParamInfo;

        /* Plan Node Map Dynamic Array에 현재 Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 aPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanTableScan,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            (qlncExprCommon*)(((qlncTableScanCost*)(sPlanTableScan->mCommon.mCandNode->mBestCost))->mTableAndFilter) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanTableScan->mFilterExpr = sConvExprParamInfo.mInitExpr;
        sSubQueryFilter = sConvExprParamInfo.mSubQueryAndFilter;

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sPlanTableScan->mFilterExpr = NULL;
        sSubQueryFilter = NULL;
    }

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sSubQueryFilter != NULL )
    {
        /* 상위 노드로 SubQuery Filter Plan과 SubQuery Plan을 생성 */
        STL_TRY( qlncMakeCandPlanSubQueryCondition( aPlanDetailParamInfo->mRegionMem,
                                                    aPlanDetailParamInfo->mCandidateMem,
                                                    (qlncPlanCommon*)sPlanTableScan,
                                                    sSubQueryFilter,
                                                    &sOutPlanNode,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOutPlanNode = (qlncPlanCommon*)sPlanTableScan;
    }


    /* Init Node 정보 설정 : 주석 부분은 MakeCandBasic에서 이미 설정한 부분임 */
    sInitBaseTable = (qlvInitBaseTableNode *) sPlanTableScan->mCommon.mInitNode;
    /* sInitBaseTable->mIsIndexScanAsc = STL_FALSE; */
    /* sInitBaseTable->mTableHandle = sPlanTableScan->mTableHandle;   */
    /* sInitBaseTable->mTablePhyHandle = sPlanTableScan->mPhysicalHandle; */
    /* sInitBaseTable->mIndexHandle = NULL; */
    /* sInitBaseTable->mIndexPhyHandle = NULL; */

    /* sInitBaseTable->mRelationName = NULL; */
    sInitBaseTable->mRefColumnList = sPlanTableScan->mReadColList;
    /* sInitBaseTable->mDumpOption = sPlanTableScan->mDumpOption; */
    /* sInitBaseTable->mOuterColumnList = sPlanTableScan->mOuterColumnList; */
    sInitBaseTable->mRefRowIdColumn = sPlanTableScan->mRowidColumn;
    

    /**
     * Output 설정
     */

    *aOutPlanNode = sOutPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index Scan Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailIndexScan( qlncPlanDetailParamInfo  * aPlanDetailParamInfo,
                                           qlncPlanCommon           * aPlanNode,
                                           qlncPlanCommon          ** aOutPlanNode,
                                           qllEnv                   * aEnv )
{
    qlncPlanCommon          * sOutPlanNode          = NULL;
    qlncPlanIndexScan       * sPlanIndexScan        = NULL;
    qlncIndexScanCost       * sIndexScanCost        = NULL;
    qlncAndFilter             sTmpAndFilter;
    qlvRefExprList            sTmpSubQueryExprList;
    qlncExprValue           * sCandValueExpr;
    qlvInitBaseTableNode    * sInitBaseTable        = NULL;
    qlncSubQueryAndFilter   * sSubQueryFilter       = NULL;
    qlncConvExprParamInfo     sConvExprParamInfo;
    stlInt32                  sLoop;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_INDEX_SCAN,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Index Scan Plan 설정 */
    sPlanIndexScan = (qlncPlanIndexScan*)aPlanNode;

    /* Index Scan Cost 설정 */
    sIndexScanCost = (qlncIndexScanCost*)(sPlanIndexScan->mCommon.mCandNode->mBestCost);

    /* 기타 변수 초기화 */
    STL_TRY( qlvCreateRefExprList( &(sPlanIndexScan->mReadColList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );
    sPlanIndexScan->mRowidColumn = NULL;
    sPlanIndexScan->mReadColMapList = NULL;

    /* Table Stat 설정 */
    STL_DASSERT( aPlanNode->mCandNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE );
    STL_TRY( qlncMCPAppendTableStat( aPlanDetailParamInfo->mTableStatList,
                                     aPlanNode,
                                     aPlanDetailParamInfo->mRegionMem,
                                     aEnv )
             == STL_SUCCESS );

    /* Index Scan Cost의 Filter들 통합 */
    QLNC_INIT_AND_FILTER( &sTmpAndFilter );

    if( sIndexScanCost->mIndexRangeAndFilter != NULL )
    {
        STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aPlanDetailParamInfo->mParamInfo),
                                                    &sTmpAndFilter,
                                                    sIndexScanCost->mIndexRangeAndFilter->mOrCount,
                                                    sIndexScanCost->mIndexRangeAndFilter->mOrFilters,
                                                    aEnv )
                 == STL_SUCCESS );
    }

    if( sIndexScanCost->mIndexKeyAndFilter != NULL )
    {
        STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aPlanDetailParamInfo->mParamInfo),
                                                    &sTmpAndFilter,
                                                    sIndexScanCost->mIndexKeyAndFilter->mOrCount,
                                                    sIndexScanCost->mIndexKeyAndFilter->mOrFilters,
                                                    aEnv )
                 == STL_SUCCESS );
    }

    if( sIndexScanCost->mTableAndFilter != NULL )
    {
        STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aPlanDetailParamInfo->mParamInfo),
                                                    &sTmpAndFilter,
                                                    sIndexScanCost->mTableAndFilter->mOrCount,
                                                    sIndexScanCost->mTableAndFilter->mOrFilters,
                                                    aEnv )
                 == STL_SUCCESS );
    }

    /* Plan Node Map Dynamic Array에 현재 Plan Node를 추가 */
    STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             aPlanNode,
                                             aEnv )
             == STL_SUCCESS );

    /* Filter Expression 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( sTmpAndFilter.mOrCount > 0 )
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO( &sConvExprParamInfo,
                                                &(aPlanDetailParamInfo->mParamInfo),
                                                aPlanDetailParamInfo->mRegionMem,
                                                aPlanDetailParamInfo->mCandidateMem,
                                                aPlanDetailParamInfo->mInitExprList,
                                                aPlanDetailParamInfo->mTableStatList,
                                                aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
                                                aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
                                                0,
                                                NULL,
                                                (qlncPlanCommon*)sPlanIndexScan,
                                                aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                &sTmpSubQueryExprList,
                                                STL_TRUE,
                                                (qlncExprCommon*)(&sTmpAndFilter) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanIndexScan->mFilterExpr = sConvExprParamInfo.mInitExpr;
        sSubQueryFilter = sConvExprParamInfo.mSubQueryAndFilter;
    }
    else
    {
        sPlanIndexScan->mFilterExpr = NULL;
        sSubQueryFilter = NULL;
    }

    /* List Column Expression Map 구성 */
    if( ( sIndexScanCost->mIndexRangeAndFilter != NULL ) &&
        ( sIndexScanCost->mListColMap != NULL ) )
    {
        STL_DASSERT( sPlanIndexScan->mFilterExpr != NULL );
        
        sPlanIndexScan->mListColMapCount = sIndexScanCost->mListColMap->mMapCount;

        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitExpression * ) * sPlanIndexScan->mListColMapCount,
                                    (void**) &sPlanIndexScan->mValueExpr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitExpression * ) * sPlanIndexScan->mListColMapCount,
                                    (void**) &sPlanIndexScan->mListColExpr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( sLoop = 0 ; sLoop < sPlanIndexScan->mListColMapCount ; sLoop++ )
        {
            /* Value Expression 찾기 */
            sCandValueExpr = (qlncExprValue *)sIndexScanCost->mListColMap->mValueExpr[ sLoop ];

            sPlanIndexScan->mValueExpr[ sLoop ] =
                qlvFindInternalBindValExprInExpr( sPlanIndexScan->mFilterExpr,
                                                  sCandValueExpr->mInternalBindIdx );

            STL_DASSERT( sPlanIndexScan->mValueExpr[ sLoop ] != NULL );

            /* List Column Expression 설정 */
            QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO( &sConvExprParamInfo,
                                                    &(aPlanDetailParamInfo->mParamInfo),
                                                    aPlanDetailParamInfo->mRegionMem,
                                                    aPlanDetailParamInfo->mCandidateMem,
                                                    aPlanDetailParamInfo->mInitExprList,
                                                    aPlanDetailParamInfo->mTableStatList,
                                                    aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
                                                    aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
                                                    0,
                                                    NULL,
                                                    (qlncPlanCommon*)sPlanIndexScan,
                                                    aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                    &sTmpSubQueryExprList,
                                                    STL_TRUE,
                                                    sIndexScanCost->mListColMap->mListColExpr[ sLoop ] );

            STL_TRY( qlncConvertCandExprToInitExprFuncList[ QLNC_EXPR_TYPE_LIST_COLUMN ](
                         &sConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );
            
            sPlanIndexScan->mListColExpr[ sLoop ] = sConvExprParamInfo.mInitExpr;
        }
    }
    else
    {
        sPlanIndexScan->mListColMapCount = 0;
        sPlanIndexScan->mValueExpr       = NULL;
        sPlanIndexScan->mListColExpr     = NULL;
    }
    
    /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
    STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             1,
                                             aEnv )
             == STL_SUCCESS );

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sSubQueryFilter != NULL )
    {
        /* 상위 노드로 SubQuery Filter Plan과 SubQuery Plan을 생성 */
        STL_TRY( qlncMakeCandPlanSubQueryCondition( aPlanDetailParamInfo->mRegionMem,
                                                    aPlanDetailParamInfo->mCandidateMem,
                                                    (qlncPlanCommon*)sPlanIndexScan,
                                                    sSubQueryFilter,
                                                    &sOutPlanNode,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* SubQuery Expression List가 존재하는지 판별 */
        if( sTmpSubQueryExprList.mCount > 0 )
        {
            /* 하위 노드로 SubQuery Plan을 생성 */
            STL_TRY( qlncMakeCandPlanSubQueryList( aPlanDetailParamInfo->mRegionMem,
                                                   aPlanDetailParamInfo->mCandidateMem,
                                                   (qlncPlanCommon*)sPlanIndexScan,
                                                   &sTmpSubQueryExprList,
                                                   &(sOutPlanNode),
                                                   aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sOutPlanNode = (qlncPlanCommon*)sPlanIndexScan;
        }
    }

    /* Init Node 정보 설정 : 주석 부분은 MakeCandBasic에서 이미 설정한 부분임 */
    sInitBaseTable = (qlvInitBaseTableNode *) sPlanIndexScan->mCommon.mInitNode;
    /* sInitBaseTable->mIsIndexScanAsc = STL_FALSE; */
    /* sInitBaseTable->mTableHandle = sPlanIndexScan->mTableHandle;   */
    /* sInitBaseTable->mTablePhyHandle = sPlanIndexScan->mPhysicalHandle; */
    /* sInitBaseTable->mIndexHandle = NULL; */
    /* sInitBaseTable->mIndexPhyHandle = NULL; */

    /* sInitBaseTable->mRelationName = NULL; */
    sInitBaseTable->mRefColumnList = sPlanIndexScan->mReadColList;
    /* sInitBaseTable->mDumpOption = sPlanIndexScan->mDumpOption; */
    /* sInitBaseTable->mOuterColumnList = sPlanIndexScan->mOuterColumnList; */
    sInitBaseTable->mRefRowIdColumn = sPlanIndexScan->mRowidColumn;

    
    /**
     * Output 설정
     */

    *aOutPlanNode = sOutPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index Combine Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailIndexCombine( qlncPlanDetailParamInfo   * aPlanDetailParamInfo,
                                              qlncPlanCommon            * aPlanNode,
                                              qlncPlanCommon           ** aOutPlanNode,
                                              qllEnv                    * aEnv )
{
    stlInt32                  i;
    qlncPlanIndexCombine    * sPlanIndexCombine     = NULL;
    qlncIndexCombineCost    * sIndexCombineScanCost = NULL;

    qlncPlanIndexScan       * sPlanIndexScan        = NULL;
    qlncIndexScanCost       * sIndexScanCost        = NULL;
    qlncAndFilter             sTmpAndFilter;
    qlvRefExprList            sTmpSubQueryExprList;

    qlncExprValue           * sCandValueExpr;
    qlvInitBaseTableNode    * sInitBaseTable        = NULL;
    qlncConvExprParamInfo     sConvExprParamInfo;
    stlInt32                  sLoop;



    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_INDEX_COMBINE,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Index Combine Plan 설정 */
    sPlanIndexCombine = (qlncPlanIndexCombine*)aPlanNode;

    /* 기타 변수 초기화 */
    sPlanIndexCombine->mReadColList = NULL;
    sPlanIndexCombine->mRowidColumn = NULL;
    sPlanIndexCombine->mReadColMapList = NULL;

    /* Table Stat 설정 */
    STL_DASSERT( aPlanNode->mCandNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE );
    STL_TRY( qlncMCPAppendTableStat( aPlanDetailParamInfo->mTableStatList,
                                     aPlanNode,
                                     aPlanDetailParamInfo->mRegionMem,
                                     aEnv )
             == STL_SUCCESS );

    /* Child Node 수행 */
    sIndexCombineScanCost = (qlncIndexCombineCost*)(sPlanIndexCombine->mCommon.mCandNode->mBestCost);
    for( i = 0; i < sPlanIndexCombine->mChildCount; i++ )
    {
        /****************************************************************
         * Detail Plan Info 설정
         ****************************************************************/

        /* Index Scan Plan 설정 */
        sPlanIndexScan = (qlncPlanIndexScan*)sPlanIndexCombine->mChildPlanArr[i];

        /* Index Scan Cost 설정 */
        sIndexScanCost = (qlncIndexScanCost*)(&sIndexCombineScanCost->mIndexScanCostArr[i]);

        /* 기타 변수 초기화 */
        STL_TRY( qlvCreateRefExprList( &(sPlanIndexScan->mReadColList),
                                       aPlanDetailParamInfo->mRegionMem,
                                       aEnv )
                 == STL_SUCCESS );
        sPlanIndexScan->mRowidColumn = NULL;
        sPlanIndexScan->mReadColMapList = NULL;

        /* Index Scan Cost의 Filter들 통합 */
        QLNC_INIT_AND_FILTER( &sTmpAndFilter );

        if( sIndexScanCost->mIndexRangeAndFilter != NULL )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aPlanDetailParamInfo->mParamInfo),
                                                        &sTmpAndFilter,
                                                        sIndexScanCost->mIndexRangeAndFilter->mOrCount,
                                                        sIndexScanCost->mIndexRangeAndFilter->mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );
        }

        if( sIndexScanCost->mIndexKeyAndFilter != NULL )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aPlanDetailParamInfo->mParamInfo),
                                                        &sTmpAndFilter,
                                                        sIndexScanCost->mIndexKeyAndFilter->mOrCount,
                                                        sIndexScanCost->mIndexKeyAndFilter->mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );
        }

        if( sIndexScanCost->mTableAndFilter != NULL )
        {
            STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aPlanDetailParamInfo->mParamInfo),
                                                        &sTmpAndFilter,
                                                        sIndexScanCost->mTableAndFilter->mOrCount,
                                                        sIndexScanCost->mTableAndFilter->mOrFilters,
                                                        aEnv )
                     == STL_SUCCESS );
        }

        /* Plan Node Map Dynamic Array에 현재 Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 sPlanIndexCombine->mChildPlanArr[i],
                                                 aEnv )
                 == STL_SUCCESS );
        
        /* Filter Expression 설정 */
        STL_DASSERT( sTmpAndFilter.mOrCount > 0 );
        QLNC_INIT_LIST( &sTmpSubQueryExprList );
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO( &sConvExprParamInfo,
                                                &(aPlanDetailParamInfo->mParamInfo),
                                                aPlanDetailParamInfo->mRegionMem,
                                                aPlanDetailParamInfo->mCandidateMem,
                                                aPlanDetailParamInfo->mInitExprList,
                                                aPlanDetailParamInfo->mTableStatList,
                                                aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
                                                aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
                                                0,
                                                NULL,
                                                (qlncPlanCommon*)sPlanIndexScan,
                                                aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                &sTmpSubQueryExprList,
                                                STL_TRUE,
                                                (qlncExprCommon*)(&sTmpAndFilter) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanIndexScan->mFilterExpr = sConvExprParamInfo.mInitExpr;
        /* SubQuery Expression List가 존재할 수 없다.
         * (Cost 계산시 Index Combine 조건이 Filter에 SubQuery가 존재하지 않아야 함이 있음) */
        STL_DASSERT( sConvExprParamInfo.mSubQueryAndFilter == NULL );

        /* List Column Expression Map 구성 */
        if( ( sIndexScanCost->mIndexRangeAndFilter != NULL ) &&
            ( sIndexScanCost->mListColMap != NULL ) )
        {
            STL_DASSERT( sPlanIndexScan->mFilterExpr != NULL );
        
            sPlanIndexScan->mListColMapCount = sIndexScanCost->mListColMap->mMapCount;

            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitExpression * ) * sPlanIndexScan->mListColMapCount,
                                        (void**) &sPlanIndexScan->mValueExpr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitExpression * ) * sPlanIndexScan->mListColMapCount,
                                        (void**) &sPlanIndexScan->mListColExpr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( sLoop = 0 ; sLoop < sPlanIndexScan->mListColMapCount ; sLoop++ )
            {
                /* Value Expression 찾기 */
                sCandValueExpr = (qlncExprValue *)sIndexScanCost->mListColMap->mValueExpr[ sLoop ];

                sPlanIndexScan->mValueExpr[ sLoop ] =
                    qlvFindInternalBindValExprInExpr( sPlanIndexScan->mFilterExpr,
                                                      sCandValueExpr->mInternalBindIdx );

                STL_DASSERT( sPlanIndexScan->mValueExpr[ sLoop ] != NULL );

                /* List Column Expression 설정 */
                QLNC_INIT_LIST( &sTmpSubQueryExprList );
                QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO( &sConvExprParamInfo,
                                                        &(aPlanDetailParamInfo->mParamInfo),
                                                        aPlanDetailParamInfo->mRegionMem,
                                                        aPlanDetailParamInfo->mCandidateMem,
                                                        aPlanDetailParamInfo->mInitExprList,
                                                        aPlanDetailParamInfo->mTableStatList,
                                                        aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
                                                        aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
                                                        0,
                                                        NULL,
                                                        (qlncPlanCommon*)sPlanIndexScan,
                                                        aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                        &sTmpSubQueryExprList,
                                                        STL_TRUE,
                                                        sIndexScanCost->mListColMap->mListColExpr[ sLoop ] );

                STL_TRY( qlncConvertCandExprToInitExprFuncList[ QLNC_EXPR_TYPE_LIST_COLUMN ](
                             &sConvExprParamInfo,
                             aEnv )
                         == STL_SUCCESS );
            
                sPlanIndexScan->mListColExpr[ sLoop ] = sConvExprParamInfo.mInitExpr;
            }
        }
        else
        {
            sPlanIndexScan->mListColMapCount = 0;
            sPlanIndexScan->mValueExpr       = NULL;
            sPlanIndexScan->mListColExpr     = NULL;
        }
    
        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }

    /* Init Node 정보 설정 : 주석 부분은 MakeCandBasic에서 이미 설정한 부분임 */
    sInitBaseTable = (qlvInitBaseTableNode *) sPlanIndexCombine->mCommon.mInitNode;
    /* sInitBaseTable->mIsIndexScanAsc = STL_FALSE; */
    /* sInitBaseTable->mTableHandle = sPlanIndexCombine->mTableHandle;   */
    /* sInitBaseTable->mTablePhyHandle = sPlanIndexCombine->mPhysicalHandle; */
    /* sInitBaseTable->mIndexHandle = NULL; */
    /* sInitBaseTable->mIndexPhyHandle = NULL; */

    /* sInitBaseTable->mRelationName = NULL; */
    sInitBaseTable->mRefColumnList = sPlanIndexCombine->mReadColList;
    /* sInitBaseTable->mDumpOption = sPlanIndexCombine->mDumpOption; */
    /* sInitBaseTable->mOuterColumnList = sPlanIndexCombine->mOuterColumnList; */
    sInitBaseTable->mRefRowIdColumn = sPlanIndexCombine->mRowidColumn;

    
    /**
     * Output 설정
     */

    *aOutPlanNode = (qlncPlanCommon*)sPlanIndexCombine;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Rowid Scan Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailRowidScan( qlncPlanDetailParamInfo  * aPlanDetailParamInfo,
                                           qlncPlanCommon           * aPlanNode,
                                           qlncPlanCommon          ** aOutPlanNode,
                                           qllEnv                   * aEnv )
{
    qlncPlanCommon          * sOutPlanNode          = NULL;
    qlncPlanRowidScan       * sPlanRowidScan        = NULL;
    qlncRowidScanCost       * sRowidScanCost        = NULL;
    qlncAndFilter             sTmpAndFilter;
    qlvRefExprList            sTmpSubQueryExprList;
    qlvInitBaseTableNode    * sInitBaseTable        = NULL;
    qlncSubQueryAndFilter   * sSubQueryFilter       = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_ROWID_SCAN,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Rowid Scan Plan 설정 */
    sPlanRowidScan = (qlncPlanRowidScan*)aPlanNode;

    /* Rowid Scan Cost 설정 */
    sRowidScanCost = (qlncRowidScanCost*)(aPlanNode->mCandNode->mBestCost);

    /* 기타 변수 초기화 */
    STL_TRY( qlvCreateRefExprList( &(sPlanRowidScan->mReadColList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );
    sPlanRowidScan->mRowidColumn = NULL;
    sPlanRowidScan->mReadColMapList = NULL;

    /* Table Stat 설정 */
    STL_DASSERT( aPlanNode->mCandNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE );
    STL_TRY( qlncMCPAppendTableStat( aPlanDetailParamInfo->mTableStatList,
                                     aPlanNode,
                                     aPlanDetailParamInfo->mRegionMem,
                                     aEnv )
             == STL_SUCCESS );

    /* Rowid Scan Cost의 Filter들 통합 */
    QLNC_INIT_AND_FILTER( &sTmpAndFilter );

    if( sRowidScanCost->mRowidAndFilter != NULL )
    {
        STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aPlanDetailParamInfo->mParamInfo),
                                                    &sTmpAndFilter,
                                                    sRowidScanCost->mRowidAndFilter->mOrCount,
                                                    sRowidScanCost->mRowidAndFilter->mOrFilters,
                                                    aEnv )
                 == STL_SUCCESS );
    }

    if( sRowidScanCost->mTableAndFilter != NULL )
    {
        STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aPlanDetailParamInfo->mParamInfo),
                                                    &sTmpAndFilter,
                                                    sRowidScanCost->mTableAndFilter->mOrCount,
                                                    sRowidScanCost->mTableAndFilter->mOrFilters,
                                                    aEnv )
                 == STL_SUCCESS );
    }

    /* Filter Expression 설정 */
    STL_DASSERT( sTmpAndFilter.mOrCount > 0 );
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    {
        qlncConvExprParamInfo     sConvExprParamInfo;

        /* Plan Node Map Dynamic Array에 현재 Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 aPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO( &sConvExprParamInfo,
                                                &(aPlanDetailParamInfo->mParamInfo),
                                                aPlanDetailParamInfo->mRegionMem,
                                                aPlanDetailParamInfo->mCandidateMem,
                                                aPlanDetailParamInfo->mInitExprList,
                                                aPlanDetailParamInfo->mTableStatList,
                                                aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
                                                aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
                                                0,
                                                NULL,
                                                (qlncPlanCommon*)sPlanRowidScan,
                                                aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                &sTmpSubQueryExprList,
                                                STL_TRUE,
                                                (qlncExprCommon*)(&sTmpAndFilter) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanRowidScan->mFilterExpr = sConvExprParamInfo.mInitExpr;
        sSubQueryFilter = sConvExprParamInfo.mSubQueryAndFilter;

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sSubQueryFilter != NULL )
    {
        /* 상위 노드로 SubQuery Filter Plan과 SubQuery Plan을 생성 */
        STL_TRY( qlncMakeCandPlanSubQueryCondition( aPlanDetailParamInfo->mRegionMem,
                                                    aPlanDetailParamInfo->mCandidateMem,
                                                    (qlncPlanCommon*)sPlanRowidScan,
                                                    sSubQueryFilter,
                                                    &sOutPlanNode,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOutPlanNode = (qlncPlanCommon*)sPlanRowidScan;
    }

    /* Init Node 정보 설정 : 주석 부분은 MakeCandBasic에서 이미 설정한 부분임 */
    sInitBaseTable = (qlvInitBaseTableNode *) sPlanRowidScan->mCommon.mInitNode;
    /* sInitBaseTable->mIsIndexScanAsc = STL_FALSE; */
    /* sInitBaseTable->mTableHandle = sPlanRowidScan->mTableHandle;   */
    /* sInitBaseTable->mTablePhyHandle = sPlanRowidScan->mPhysicalHandle; */
    /* sInitBaseTable->mIndexHandle = NULL; */
    /* sInitBaseTable->mIndexPhyHandle = NULL; */

    /* sInitBaseTable->mRelationName = NULL; */
    sInitBaseTable->mRefColumnList = sPlanRowidScan->mReadColList;
    /* sInitBaseTable->mDumpOption = sPlanRowidScan->mDumpOption; */
    /* sInitBaseTable->mOuterColumnList = sPlanRowidScan->mOuterColumnList; */
    sInitBaseTable->mRefRowIdColumn = sPlanRowidScan->mRowidColumn;

    
    /**
     * Output 설정
     */

    *aOutPlanNode = sOutPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Table Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailSubTable( qlncPlanDetailParamInfo   * aPlanDetailParamInfo,
                                          qlncPlanCommon            * aPlanNode,
                                          qlncPlanCommon           ** aOutPlanNode,
                                          qllEnv                    * aEnv )
{
    qlncPlanCommon          * sOutPlanNode      = NULL;
    qlncPlanSubTable        * sPlanSubTable     = NULL;
    qlncFilterCost          * sFilterCost       = NULL;
    qlvRefExprList            sTmpSubQueryExprList;
    qlvInitSubTableNode     * sInitSubTable     = NULL;
    qlncSubQueryAndFilter   * sSubQueryFilter   = NULL;
    qlvRefExprItem          * sRefExprItem      = NULL;
    qlvInitInnerColumn      * sInitInnerColumn  = NULL;
    qlvInitExpression       * sInitExpr         = NULL;
    qlvInitQuerySetNode     * sInitQuerySet     = NULL;
    qlvInitQuerySpecNode    * sInitQuerySpec    = NULL;

    
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_SUB_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Sub Table Plan 설정 */
    sPlanSubTable = (qlncPlanSubTable*)aPlanNode;

    /* Filter Cost 설정 */
    sFilterCost = (qlncFilterCost*)(sPlanSubTable->mCommon.mCandNode->mBestCost);

    /* Child Plan 처리 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanSubTable->mChildPlanNode->mPlanType ](
                 aPlanDetailParamInfo,
                 sPlanSubTable->mChildPlanNode,
                 &(sPlanSubTable->mChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    /* 기타 변수 초기화 */
    sPlanSubTable->mReadColList = NULL;
    sPlanSubTable->mReadColMapList = NULL;

    /* Reference Column List 설정 */
    if( sPlanSubTable->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_QUERY_SPEC )
    {
        STL_DASSERT( ((qlncPlanQuerySpec*)(sPlanSubTable->mChildPlanNode))->mTargetCount > 0 );
        sRefExprItem = ((qlncPlanQuerySpec*)(sPlanSubTable->mChildPlanNode))->mTargetColList->mHead;
    }
    else
    {
        STL_DASSERT( sPlanSubTable->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_QUERY_SET );
        sRefExprItem = ((qlncPlanQuerySet*)(sPlanSubTable->mChildPlanNode))->mTargetList->mHead;
    }

    /* Read Column List 할당 */
    STL_TRY( qlvCreateRefExprList( &(sPlanSubTable->mReadColList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Read Column List 등록 */
    while( sRefExprItem != NULL )
    {
        /* Init Inner Column 할당 */
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitInnerColumn ),
                                    (void**) &sInitInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Relation Node 설정 */
        sInitInnerColumn->mRelationNode = sPlanSubTable->mCommon.mInitNode;

        /* Column Idx 할당 */
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( stlInt32 ),
                                    (void**) &(sInitInnerColumn->mIdx),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Column Idx 설정 */
        *(sInitInnerColumn->mIdx) = sPlanSubTable->mReadColList->mCount;

        /* Original Expression 설정 */
        sInitInnerColumn->mOrgExpr = sRefExprItem->mExprPtr;

        /* Init Expression 생성 */
        QLNC_MAKE_INIT_EXPRESSION( aPlanDetailParamInfo->mRegionMem,
                                   QLV_EXPR_TYPE_INNER_COLUMN,
                                   KNL_BUILTIN_FUNC_INVALID,
                                   sInitInnerColumn->mOrgExpr->mIterationTime,
                                   sInitInnerColumn->mOrgExpr->mPosition,
                                   sInitInnerColumn->mOrgExpr->mPhraseType,
                                   sInitInnerColumn->mOrgExpr->mIncludeExprCnt,
                                   sInitInnerColumn->mOrgExpr->mColumnName,
                                   sInitInnerColumn,
                                   sInitExpr,
                                   aEnv );

        /* Read Column List에 등록 */
        STL_TRY( qlvAddExprToRefColExprList( sPlanSubTable->mReadColList,
                                             sInitExpr,
                                             aPlanDetailParamInfo->mRegionMem,
                                             aEnv )
                 == STL_SUCCESS );

        /* Left 여부 설정 */
        sPlanSubTable->mReadColList->mTail->mIsLeft = STL_TRUE;

        sRefExprItem = sRefExprItem->mNext;
    }

    /* Filter Expression 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( sFilterCost->mFilter != NULL)
    {
        qlncConvExprParamInfo     sConvExprParamInfo;

        /* Plan Node Map Dynamic Array에 현재 Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 aPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanSubTable,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            (qlncExprCommon*)(sFilterCost->mFilter) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanSubTable->mFilterExpr = sConvExprParamInfo.mInitExpr;
        sSubQueryFilter = sConvExprParamInfo.mSubQueryAndFilter;

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sPlanSubTable->mFilterExpr = NULL;
        sSubQueryFilter = NULL;
    }

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sSubQueryFilter != NULL )
    {
        /* 상위 노드로 SubQuery Filter Plan과 SubQuery Plan을 생성 */
        STL_TRY( qlncMakeCandPlanSubQueryCondition( aPlanDetailParamInfo->mRegionMem,
                                                    aPlanDetailParamInfo->mCandidateMem,
                                                    (qlncPlanCommon*)sPlanSubTable,
                                                    sSubQueryFilter,
                                                    &sOutPlanNode,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOutPlanNode = (qlncPlanCommon*)sPlanSubTable;
    }

    /* Init Node 정보 설정 : 주석 부분은 MakeCandBasic에서 이미 설정한 부분임 */
    sInitSubTable = (qlvInitSubTableNode *) sPlanSubTable->mCommon.mInitNode;
    /* sInitSubTable->mIsView = STL_TRUE; */

    /* sInitSubTable->mHasHandle = STL_FALSE; */
    /* sInitSubTable->mViewHandle; */
    
    /* sInitSubTable->mQueryNode = NULL; */
    /* sInitSubTable->mRelationName = NULL; */
    sInitSubTable->mRefColumnList = sPlanSubTable->mReadColList;
    /* sInitSubTable->mOuterColumnList = sPlanSubTable->mOuterColumnList; */

    if( ((qlncPlanCommon*)sPlanSubTable->mChildPlanNode)->mPlanType == QLNC_PLAN_TYPE_QUERY_SET )
    {
        sInitQuerySet = (qlvInitQuerySetNode *)
            (((qlncPlanQuerySet *)sPlanSubTable->mChildPlanNode)->mCommon.mInitNode);

        sInitSubTable->mColumnCount = sInitQuerySet->mSetTargetCount;
        sInitSubTable->mColumns = sInitQuerySet->mSetTargets;
    }
    else
    {
        STL_DASSERT( ((qlncPlanCommon*)sPlanSubTable->mChildPlanNode)->mPlanType ==
                     QLNC_PLAN_TYPE_QUERY_SPEC );

        sInitQuerySpec = (qlvInitQuerySpecNode *)
            (((qlncPlanQuerySpec *)sPlanSubTable->mChildPlanNode)->mCommon.mInitNode);

        sInitSubTable->mColumnCount = sInitQuerySpec->mTargetCount;
        sInitSubTable->mColumns = sInitQuerySpec->mTargets;
    }
    
    sInitSubTable->mFilterExpr = sPlanSubTable->mFilterExpr;

    
    /**
     * Output 설정
     */

    *aOutPlanNode = sOutPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Nested Loops Join Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailNestedJoin( qlncPlanDetailParamInfo * aPlanDetailParamInfo,
                                            qlncPlanCommon          * aPlanNode,
                                            qlncPlanCommon         ** aOutPlanNode,
                                            qllEnv                  * aEnv )
{
    stlInt32                  i;
    qlncPlanCommon          * sOutPlanNode          = NULL;
    qlncPlanNestedJoin      * sPlanNestedJoin       = NULL;
    qlncNestedJoinCost      * sNestedJoinCost       = NULL;
    qlvRefExprList            sTmpSubQueryExprList;
    qlncSubQueryAndFilter   * sSubQueryFilter       = NULL;
    qlvInitJoinTableNode    * sInitJoinTable        = NULL;

    qlvRefExprList          * sBackupAggrExprList       = NULL;
    qlvRefExprList          * sBackupNestedAggrExprList = NULL;

    qlncJoinOuterInfoItem     sJoinLeftOuterInfoItem;
    qlncJoinOuterInfoItem     sJoinRightOuterInfoItem;
    qlncJoinOuterInfoList     sTmpJoinOuterInfoList;

    stlBool                   sIsSamePosNode;
    qlncNodeCommon          * sNode                     = NULL;
    qlncNamedColumnItem     * sCandNamedColumnItem      = NULL;
    qlvNamedColumnItem      * sInitNamedColumnItem      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_NESTED_JOIN,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Nested Join Plan 설정 */
    sPlanNestedJoin = (qlncPlanNestedJoin*)aPlanNode;

    /* Nested Join Cost 설정 */
    sNestedJoinCost = (qlncNestedJoinCost*)(aPlanNode->mCandNode->mBestCost);

    /* Join Left Outer Column List 할당 */
    STL_TRY( qlvCreateRefExprList( &(sPlanNestedJoin->mJoinLeftOuterColumnList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Join Right Outer Column List 할당 */
    STL_TRY( qlvCreateRefExprList( &(sPlanNestedJoin->mJoinRightOuterColumnList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Where Filter Outer Column List 할당 */
    STL_TRY( qlvCreateRefExprList( &(sPlanNestedJoin->mWhereFilterOuterColumnList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Plan Node Map Dynamic Array에 Right Plan Node를 추가 */
    STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             sPlanNestedJoin->mRightChildPlanNode,
                                             aEnv )
             == STL_SUCCESS );

    /* Left Plan 설정 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanNestedJoin->mLeftChildPlanNode->mPlanType ](
                 aPlanDetailParamInfo,
                 sPlanNestedJoin->mLeftChildPlanNode,
                 &(sPlanNestedJoin->mLeftChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
    STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             1,
                                             aEnv )
             == STL_SUCCESS );

    /* Aggregation List 정보 Backup */
    if( (aPlanNode->mCandNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE) &&
        (aPlanDetailParamInfo->mInitExprList != NULL) )
    {
        sBackupAggrExprList = aPlanDetailParamInfo->mInitExprList->mAggrExprList;
        sBackupNestedAggrExprList = aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList;

        /* Aggregation Node 찾기 */
        STL_TRY( qlncMCPSetAggrExprList( sPlanNestedJoin->mRightChildPlanNode,
                                         &(aPlanDetailParamInfo->mInitExprList->mAggrExprList),
                                         &(aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList),
                                         aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sBackupAggrExprList = NULL;
        sBackupNestedAggrExprList = NULL;
    }

    /* Plan Node Map Dynamic Array에 Left Plan Node를 추가 */
    STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             sPlanNestedJoin->mLeftChildPlanNode,
                                             aEnv )
             == STL_SUCCESS );

    /* Right Plan 설정 */
    sJoinLeftOuterInfoItem.mOuterPlanNode = sPlanNestedJoin->mLeftChildPlanNode;
    sJoinLeftOuterInfoItem.mOuterColumnList = sPlanNestedJoin->mJoinLeftOuterColumnList;
    sJoinLeftOuterInfoItem.mNext = aPlanDetailParamInfo->mJoinOuterInfoList.mHead;

    aPlanDetailParamInfo->mJoinOuterInfoList.mHead = &sJoinLeftOuterInfoItem;
    aPlanDetailParamInfo->mJoinOuterInfoList.mCount++;

    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanNestedJoin->mRightChildPlanNode->mPlanType ](
                 aPlanDetailParamInfo,
                 sPlanNestedJoin->mRightChildPlanNode,
                 &(sPlanNestedJoin->mRightChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    aPlanDetailParamInfo->mJoinOuterInfoList.mHead =
        aPlanDetailParamInfo->mJoinOuterInfoList.mHead->mNext;
    aPlanDetailParamInfo->mJoinOuterInfoList.mCount--;

    /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
    STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             1,
                                             aEnv )
             == STL_SUCCESS );

    /* 기타 변수 초기화 */
    STL_TRY( qlvCreateRefExprList( &(sPlanNestedJoin->mReadColList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );
    sPlanNestedJoin->mReadColMapList = NULL;

    /* Left Join Outer Info와 Right Join Outer Info 설정 */
    sJoinLeftOuterInfoItem.mOuterPlanNode = sPlanNestedJoin->mLeftChildPlanNode;
    sJoinLeftOuterInfoItem.mOuterColumnList = sPlanNestedJoin->mJoinLeftOuterColumnList;
    sJoinLeftOuterInfoItem.mNext = NULL;
    sJoinRightOuterInfoItem.mOuterPlanNode = sPlanNestedJoin->mRightChildPlanNode;
    sJoinRightOuterInfoItem.mOuterColumnList = sPlanNestedJoin->mJoinRightOuterColumnList;
    sJoinRightOuterInfoItem.mNext = NULL;

    /* Join Condition Expression 설정 */
    if( sNestedJoinCost->mJoinCondition != NULL)
    {
        qlncConvExprParamInfo     sConvExprParamInfo;

        /* Plan Node Map Dynamic Array에 Left Plan Node와 Right Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 sPlanNestedJoin->mLeftChildPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 sPlanNestedJoin->mRightChildPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            1,
            &sJoinLeftOuterInfoItem,
            1,
            &sJoinRightOuterInfoItem,
            (qlncPlanCommon*)sPlanNestedJoin,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            NULL,
            STL_TRUE,
            (qlncExprCommon*)(sNestedJoinCost->mJoinCondition) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanNestedJoin->mJoinFilterExpr = sConvExprParamInfo.mInitExpr;

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 2,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sPlanNestedJoin->mJoinFilterExpr = NULL;
    }

    /* Where Filter Outer Info 설정 */
    sJoinLeftOuterInfoItem.mOuterPlanNode = (qlncPlanCommon*)sPlanNestedJoin;
    sJoinLeftOuterInfoItem.mOuterColumnList = sPlanNestedJoin->mWhereFilterOuterColumnList;
    sJoinLeftOuterInfoItem.mNext = aPlanDetailParamInfo->mJoinOuterInfoList.mHead;

    sTmpJoinOuterInfoList.mCount = aPlanDetailParamInfo->mJoinOuterInfoList.mCount + 1;
    sTmpJoinOuterInfoList.mHead = &sJoinLeftOuterInfoItem;

    /* SubQuery Filter Expression 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( sNestedJoinCost->mSubQueryFilter != NULL)
    {
        qlncConvExprParamInfo     sConvExprParamInfo;

        /* Plan Node Map Dynamic Array에 현재 Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 aPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            sTmpJoinOuterInfoList.mCount,
            sTmpJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanNestedJoin,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            (qlncExprCommon*)(sNestedJoinCost->mSubQueryFilter) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanNestedJoin->mSubQueryFilterExpr = sConvExprParamInfo.mInitExpr;
        sSubQueryFilter = sConvExprParamInfo.mSubQueryAndFilter;

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sPlanNestedJoin->mSubQueryFilterExpr = NULL;
        sSubQueryFilter = NULL;
    }

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sSubQueryFilter != NULL )
    {
        /* SubQuery Filter Plan과 SubQuery Plan을 생성 */
        STL_TRY( qlncMakeCandPlanSubQueryCondition( aPlanDetailParamInfo->mRegionMem,
                                                    aPlanDetailParamInfo->mCandidateMem,
                                                    (qlncPlanCommon*)sPlanNestedJoin,
                                                    sSubQueryFilter,
                                                    &sOutPlanNode,
                                                    aEnv )
                 == STL_SUCCESS );

        /* Statement SubQuery List가 있으면 이를 찾아
         * Nested Loops Join의 Stmt SubQuery List Plan Node로 설정 */
        if( sOutPlanNode->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_LIST )
        {
            sPlanNestedJoin->mStmtSubQueryPlanNode = sOutPlanNode;
            sOutPlanNode =
                ((qlncPlanSubQueryList*)(sPlanNestedJoin->mStmtSubQueryPlanNode))->mChildPlanNode;
            ((qlncPlanSubQueryList*)(sPlanNestedJoin->mStmtSubQueryPlanNode))->mChildPlanNode =
                NULL;
        }
        else
        {
            sPlanNestedJoin->mStmtSubQueryPlanNode = NULL;
        }

        /* SubQuery Filter가 존재하면 해당 Filter Node의 정보들을 Nested Loops Join에 설정 */
        if( sOutPlanNode->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_FILTER )
        {
            sPlanNestedJoin->mSubQueryPlanNodes =
                ((qlncPlanSubQueryFilter*)(sOutPlanNode))->mChildPlanNodes;
            sPlanNestedJoin->mSubQueryAndFilter =
                ((qlncPlanSubQueryFilter*)(sOutPlanNode))->mAndFilter;

            for( i = 0; i < sPlanNestedJoin->mSubQueryAndFilter->mAndFilterCnt; i++ )
            {
                STL_DASSERT( sPlanNestedJoin->mSubQueryPlanNodes[i]->mPlanType ==
                             QLNC_PLAN_TYPE_SUB_QUERY_LIST );
                ((qlncPlanSubQueryList*)(sPlanNestedJoin->mSubQueryPlanNodes[i]))->mChildPlanNode = NULL;
            }
        }
        else
        {
            sPlanNestedJoin->mSubQueryPlanNodes = NULL;
            sPlanNestedJoin->mSubQueryAndFilter = NULL;
        }
    }
    else
    {
        sPlanNestedJoin->mStmtSubQueryPlanNode = NULL;
        sPlanNestedJoin->mSubQueryPlanNodes = NULL;
        sPlanNestedJoin->mSubQueryAndFilter = NULL;
    }

    /* Join Filter Expression 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( sNestedJoinCost->mJoinFilter != NULL)
    {
        qlncConvExprParamInfo     sConvExprParamInfo;

        /* Plan Node Map Dynamic Array에 현재 Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 aPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanNestedJoin,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            (qlncExprCommon*)(sNestedJoinCost->mJoinFilter) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanNestedJoin->mWhereFilterExpr = sConvExprParamInfo.mInitExpr;
        sSubQueryFilter = sConvExprParamInfo.mSubQueryAndFilter;

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sPlanNestedJoin->mWhereFilterExpr = NULL;
        sSubQueryFilter = NULL;
    }

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sSubQueryFilter != NULL )
    {
        /* 상위 노드로 SubQuery Filter Plan과 SubQuery Plan을 생성 */
        STL_TRY( qlncMakeCandPlanSubQueryCondition( aPlanDetailParamInfo->mRegionMem,
                                                    aPlanDetailParamInfo->mCandidateMem,
                                                    (qlncPlanCommon*)sPlanNestedJoin,
                                                    sSubQueryFilter,
                                                    &sOutPlanNode,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOutPlanNode = (qlncPlanCommon*)sPlanNestedJoin;
    }

    /* Init Node 정보 설정 : 주석 부분은 MakeCandBasic에서 이미 설정한 부분임 */
    sInitJoinTable = (qlvInitJoinTableNode *) sPlanNestedJoin->mCommon.mInitNode;
    sInitJoinTable->mJoinType = sPlanNestedJoin->mJoinType;
    sInitJoinTable->mLeftTableNode = sPlanNestedJoin->mLeftChildPlanNode->mInitNode;
    sInitJoinTable->mRightTableNode = sPlanNestedJoin->mRightChildPlanNode->mInitNode;
    sInitJoinTable->mJoinSpec = NULL;   /* default */
    sInitJoinTable->mRefColumnList = sPlanNestedJoin->mReadColList;
    /* sInitJoinTable->mOuterColumnList = sPlanNestedJoin->mOuterColumnList; */

    /* Named Column List 설정 */
    if( (aPlanNode->mCandNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE) &&
        (((qlncInnerJoinTableNode*)(aPlanNode->mCandNode))->mNamedColumnList != NULL) )
    {
        qlncConvExprParamInfo     sConvExprParamInfo;

        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitJoinSpec ),
                                    (void**)&(sInitJoinTable->mJoinSpec),
                                    KNL_ENV(aEnv) )
             == STL_SUCCESS );

        sInitJoinTable->mJoinSpec->mJoinCondition = NULL;

        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvNamedColumnList ),
                                    (void**)&(sInitJoinTable->mJoinSpec->mNamedColumnList),
                                    KNL_ENV(aEnv) )
             == STL_SUCCESS );

        sInitJoinTable->mJoinSpec->mNamedColumnList->mCount = 0;
        sInitJoinTable->mJoinSpec->mNamedColumnList->mHead = NULL;
        sInitJoinTable->mJoinSpec->mNamedColumnList->mTail = NULL;

        sCandNamedColumnItem =
            ((qlncInnerJoinTableNode*)(aPlanNode->mCandNode))->mNamedColumnList->mHead;
        while( sCandNamedColumnItem != NULL )
        {
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvNamedColumnItem ),
                                        (void**)&(sInitNamedColumnItem),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sInitNamedColumnItem->mName = NULL;
            sInitNamedColumnItem->mPosition = 0;
            sInitNamedColumnItem->mNext = NULL;

            if( sCandNamedColumnItem->mLeftExpr->mType == QLNC_EXPR_TYPE_COLUMN )
            {
                sNode = ((qlncExprColumn*)(sCandNamedColumnItem->mLeftExpr))->mNode;
            }
            else if( sCandNamedColumnItem->mLeftExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN )
            {
                sNode = ((qlncExprInnerColumn*)(sCandNamedColumnItem->mLeftExpr))->mNode;
            }
            else
            {
                STL_DASSERT( sCandNamedColumnItem->mLeftExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );
                sNode = ((qlncExprRowidColumn*)(sCandNamedColumnItem->mLeftExpr))->mNode;
            }

            if( qlncIsExistCandNodeOnPlanNode( sPlanNestedJoin->mLeftChildPlanNode,
                                               sNode )
                == STL_TRUE )
            {
                STL_DASSERT( qlncIsExistCandNodeOnPlanNode( sPlanNestedJoin->mRightChildPlanNode,
                                                            sNode )
                             == STL_FALSE );
                sIsSamePosNode = STL_TRUE;
            }
            else
            {
                STL_DASSERT( qlncIsExistCandNodeOnPlanNode( sPlanNestedJoin->mRightChildPlanNode,
                                                            sNode )
                             == STL_TRUE );

                sIsSamePosNode = STL_FALSE;
            }

            QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                &sConvExprParamInfo,
                &(aPlanDetailParamInfo->mParamInfo),
                aPlanDetailParamInfo->mRegionMem,
                aPlanDetailParamInfo->mCandidateMem,
                aPlanDetailParamInfo->mInitExprList,
                aPlanDetailParamInfo->mTableStatList,
                0,
                NULL,
                0,
                NULL,
                ( sIsSamePosNode == STL_TRUE
                  ? sPlanNestedJoin->mLeftChildPlanNode
                  : sPlanNestedJoin->mRightChildPlanNode ),
                aPlanDetailParamInfo->mPlanNodeMapDynArr,
                NULL,
                STL_TRUE,
                sCandNamedColumnItem->mLeftExpr );

            STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandNamedColumnItem->mLeftExpr->mType](
                         &sConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );
            sInitNamedColumnItem->mLeftExpr = sConvExprParamInfo.mInitExpr;

            QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                &sConvExprParamInfo,
                &(aPlanDetailParamInfo->mParamInfo),
                aPlanDetailParamInfo->mRegionMem,
                aPlanDetailParamInfo->mCandidateMem,
                aPlanDetailParamInfo->mInitExprList,
                aPlanDetailParamInfo->mTableStatList,
                0,
                NULL,
                0,
                NULL,
                ( sIsSamePosNode == STL_TRUE
                  ? sPlanNestedJoin->mRightChildPlanNode
                  : sPlanNestedJoin->mLeftChildPlanNode ),
                aPlanDetailParamInfo->mPlanNodeMapDynArr,
                NULL,
                STL_TRUE,
                sCandNamedColumnItem->mRightExpr );

            STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandNamedColumnItem->mRightExpr->mType](
                         &sConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );
            sInitNamedColumnItem->mRightExpr = sConvExprParamInfo.mInitExpr;

            if( sInitJoinTable->mJoinSpec->mNamedColumnList->mCount == 0 )
            {
                sInitJoinTable->mJoinSpec->mNamedColumnList->mHead = sInitNamedColumnItem;
                sInitJoinTable->mJoinSpec->mNamedColumnList->mTail = sInitNamedColumnItem;
            }
            else
            {
                sInitJoinTable->mJoinSpec->mNamedColumnList->mTail->mNext = sInitNamedColumnItem;
                sInitJoinTable->mJoinSpec->mNamedColumnList->mTail = sInitNamedColumnItem;
            }
            sInitJoinTable->mJoinSpec->mNamedColumnList->mCount++;

            sCandNamedColumnItem = sCandNamedColumnItem->mNext;
        }
    }

    /* Aggregation List Backup 정보 원복 */
    if( (aPlanNode->mCandNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE) &&
        (aPlanDetailParamInfo->mInitExprList != NULL) )
    {
        aPlanDetailParamInfo->mInitExprList->mAggrExprList = sBackupAggrExprList;
        aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList = sBackupNestedAggrExprList;
    }

    
    /**
     * Output 설정
     */

    *aOutPlanNode = sOutPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Merge Join Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailMergeJoin( qlncPlanDetailParamInfo  * aPlanDetailParamInfo,
                                           qlncPlanCommon           * aPlanNode,
                                           qlncPlanCommon          ** aOutPlanNode,
                                           qllEnv                   * aEnv )
{
    stlInt32                  i;
    qlncPlanCommon          * sOutPlanNode          = NULL;
    qlncPlanMergeJoin       * sPlanMergeJoin        = NULL;
    qlncMergeJoinCost       * sMergeJoinCost        = NULL;
    qlncConvExprParamInfo     sConvExprParamInfo;
    qlvRefExprList            sTmpSubQueryExprList;
    qlncSubQueryAndFilter   * sSubQueryFilter       = NULL;
    qlvInitJoinTableNode    * sInitJoinTable        = NULL;

    qlvRefExprList          * sBackupAggrExprList       = NULL;
    qlvRefExprList          * sBackupNestedAggrExprList = NULL;

    qlncJoinOuterInfoItem     sJoinLeftOuterInfoItem;
    qlncJoinOuterInfoItem     sJoinRightOuterInfoItem;

    stlBool                   sIsSamePosNode;
    qlncNodeCommon          * sNode                     = NULL;
    qlncNamedColumnItem     * sCandNamedColumnItem      = NULL;
    qlvNamedColumnItem      * sInitNamedColumnItem      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_MERGE_JOIN,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Merge Join Plan 설정 */
    sPlanMergeJoin = (qlncPlanMergeJoin*)aPlanNode;

    /* Merge Join Cost 설정 */
    sMergeJoinCost = (qlncMergeJoinCost*)(aPlanNode->mCandNode->mBestCost);

    /* Plan Node Map Dynamic Array에 Right Plan Node를 추가 */
    STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             sPlanMergeJoin->mRightChildPlanNode,
                                             aEnv )
             == STL_SUCCESS );

    /* Left Plan 설정 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanMergeJoin->mLeftChildPlanNode->mPlanType ](
                 aPlanDetailParamInfo,
                 sPlanMergeJoin->mLeftChildPlanNode,
                 &(sPlanMergeJoin->mLeftChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
    STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             1,
                                             aEnv )
             == STL_SUCCESS );

    /* Aggregation List 정보 Backup */
    if( (aPlanNode->mCandNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE) &&
        (aPlanDetailParamInfo->mInitExprList != NULL) )
    {
        sBackupAggrExprList = aPlanDetailParamInfo->mInitExprList->mAggrExprList;
        sBackupNestedAggrExprList = aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList;

        /* Aggregation Node 찾기 */
        if( sPlanMergeJoin->mRightChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SORT_JOIN_INSTANT )
        {
            STL_TRY( qlncMCPSetAggrExprList( ((qlncPlanSortInstant*)(sPlanMergeJoin->mRightChildPlanNode))->mChildPlanNode,
                                             &(aPlanDetailParamInfo->mInitExprList->mAggrExprList),
                                             &(aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList),
                                             aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_DASSERT( sPlanMergeJoin->mRightChildPlanNode->mPlanType == QLNC_PLAN_TYPE_INDEX_SCAN );
            STL_TRY( qlncMCPSetAggrExprList( sPlanMergeJoin->mRightChildPlanNode,
                                             &(aPlanDetailParamInfo->mInitExprList->mAggrExprList),
                                             &(aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList),
                                             aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        sBackupAggrExprList = NULL;
        sBackupNestedAggrExprList = NULL;
    }

    /* Plan Node Map Dynamic Array에 Left Plan Node를 추가 */
    STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             sPlanMergeJoin->mLeftChildPlanNode,
                                             aEnv )
             == STL_SUCCESS );

    /* Right Plan 설정 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanMergeJoin->mRightChildPlanNode->mPlanType ](
                 aPlanDetailParamInfo,
                 sPlanMergeJoin->mRightChildPlanNode,
                 &(sPlanMergeJoin->mRightChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
    STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             1,
                                             aEnv )
             == STL_SUCCESS );

    /* Join Left Outer Column List 할당 */
    STL_TRY( qlvCreateRefExprList( &(sPlanMergeJoin->mJoinLeftOuterColumnList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Join Right Outer Column List 할당 */
    STL_TRY( qlvCreateRefExprList( &(sPlanMergeJoin->mJoinRightOuterColumnList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* 기타 변수 초기화 */
    STL_TRY( qlvCreateRefExprList( &(sPlanMergeJoin->mReadColList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );
    sPlanMergeJoin->mReadColMapList = NULL;

    /* Left Join Outer Info와 Right Join Outer Info 설정 */
    sJoinLeftOuterInfoItem.mOuterPlanNode = sPlanMergeJoin->mLeftChildPlanNode;
    sJoinLeftOuterInfoItem.mOuterColumnList = sPlanMergeJoin->mJoinLeftOuterColumnList;
    sJoinLeftOuterInfoItem.mNext = NULL;
    sJoinRightOuterInfoItem.mOuterPlanNode = sPlanMergeJoin->mRightChildPlanNode;
    sJoinRightOuterInfoItem.mOuterColumnList = sPlanMergeJoin->mJoinRightOuterColumnList;
    sJoinRightOuterInfoItem.mNext = NULL;

    /* Left Sort Key Expression 설정 */
    STL_DASSERT( sMergeJoinCost->mLeftSortKeyArr != NULL );

    STL_TRY( qlvCreateRefExprList( &(sPlanMergeJoin->mLeftSortKeyList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    for( i = 0; i < sMergeJoinCost->mSortKeyCount; i++ )
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            0,
            NULL,
            0,
            NULL,
            (qlncPlanCommon*)sPlanMergeJoin->mLeftChildPlanNode,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            NULL,
            STL_TRUE,
            (qlncExprCommon*)(sMergeJoinCost->mLeftSortKeyArr[i]) );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[ sMergeJoinCost->mLeftSortKeyArr[i]->mType ](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlvAddExprToRefExprList( sPlanMergeJoin->mLeftSortKeyList,
                                          sConvExprParamInfo.mInitExpr,
                                          STL_FALSE,
                                          aPlanDetailParamInfo->mRegionMem,
                                          aEnv )
                 == STL_SUCCESS );
    }

    /* Right Sort Key Expression 설정 */
    STL_DASSERT( sMergeJoinCost->mRightSortKeyArr != NULL );

    STL_TRY( qlvCreateRefExprList( &(sPlanMergeJoin->mRightSortKeyList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    for( i = 0; i < sMergeJoinCost->mSortKeyCount; i++ )
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            0,
            NULL,
            0,
            NULL,
            (qlncPlanCommon*)sPlanMergeJoin->mRightChildPlanNode,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            NULL,
            STL_TRUE,
            (qlncExprCommon*)(sMergeJoinCost->mRightSortKeyArr[i]) );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[ sMergeJoinCost->mRightSortKeyArr[i]->mType ](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlvAddExprToRefExprList( sPlanMergeJoin->mRightSortKeyList,
                                          sConvExprParamInfo.mInitExpr,
                                          STL_FALSE,
                                          aPlanDetailParamInfo->mRegionMem,
                                          aEnv )
                 == STL_SUCCESS );
    }

    /* Non Equi Join Condition Expression 설정 */
    if( sMergeJoinCost->mNonEquiJoinCondition != NULL)
    {
        /* Plan Node Map Dynamic Array에 Left Plan Node와 Right Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 sPlanMergeJoin->mLeftChildPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 sPlanMergeJoin->mRightChildPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            1,
            &sJoinLeftOuterInfoItem,
            1,
            &sJoinRightOuterInfoItem,
            (qlncPlanCommon*)sPlanMergeJoin,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            NULL,
            STL_TRUE,
            (qlncExprCommon*)(sMergeJoinCost->mNonEquiJoinCondition) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanMergeJoin->mJoinFilterExpr = sConvExprParamInfo.mInitExpr;

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 2,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sPlanMergeJoin->mJoinFilterExpr = NULL;
    }

    /* Join Filter Expression 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( sMergeJoinCost->mJoinFilter != NULL)
    {
        /* Plan Node Map Dynamic Array에 현재 Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 aPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanMergeJoin,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            (qlncExprCommon*)(sMergeJoinCost->mJoinFilter) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanMergeJoin->mWhereFilterExpr = sConvExprParamInfo.mInitExpr;
        sSubQueryFilter = sConvExprParamInfo.mSubQueryAndFilter;

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sPlanMergeJoin->mWhereFilterExpr = NULL;
        sSubQueryFilter = NULL;
    }

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sSubQueryFilter != NULL )
    {
        /* 상위 노드로 SubQuery Filter Plan과 SubQuery Plan을 생성 */
        STL_TRY( qlncMakeCandPlanSubQueryCondition( aPlanDetailParamInfo->mRegionMem,
                                                    aPlanDetailParamInfo->mCandidateMem,
                                                    (qlncPlanCommon*)sPlanMergeJoin,
                                                    sSubQueryFilter,
                                                    &sOutPlanNode,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOutPlanNode = (qlncPlanCommon*)sPlanMergeJoin;
    }

    /* Init Node 정보 설정 : 주석 부분은 MakeCandBasic에서 이미 설정한 부분임 */
    sInitJoinTable = (qlvInitJoinTableNode *) sPlanMergeJoin->mCommon.mInitNode;
    sInitJoinTable->mJoinType = sPlanMergeJoin->mJoinType;
    sInitJoinTable->mLeftTableNode = sPlanMergeJoin->mLeftChildPlanNode->mInitNode;
    sInitJoinTable->mRightTableNode = sPlanMergeJoin->mRightChildPlanNode->mInitNode;
    sInitJoinTable->mJoinSpec = NULL;   /* default */
    sInitJoinTable->mRefColumnList = sPlanMergeJoin->mReadColList;
    /* sInitJoinTable->mOuterColumnList = sPlanMergeJoin->mOuterColumnList; */

    /* Named Column List 설정 */
    if( (aPlanNode->mCandNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE) &&
        (((qlncInnerJoinTableNode*)(aPlanNode->mCandNode))->mNamedColumnList != NULL) )
    {
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitJoinSpec ),
                                    (void**)&(sInitJoinTable->mJoinSpec),
                                    KNL_ENV(aEnv) )
             == STL_SUCCESS );

        sInitJoinTable->mJoinSpec->mJoinCondition = NULL;

        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvNamedColumnList ),
                                    (void**)&(sInitJoinTable->mJoinSpec->mNamedColumnList),
                                    KNL_ENV(aEnv) )
             == STL_SUCCESS );

        sInitJoinTable->mJoinSpec->mNamedColumnList->mCount = 0;
        sInitJoinTable->mJoinSpec->mNamedColumnList->mHead = NULL;
        sInitJoinTable->mJoinSpec->mNamedColumnList->mTail = NULL;

        sCandNamedColumnItem =
            ((qlncInnerJoinTableNode*)(aPlanNode->mCandNode))->mNamedColumnList->mHead;
        while( sCandNamedColumnItem != NULL )
        {
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvNamedColumnItem ),
                                        (void**)&(sInitNamedColumnItem),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sInitNamedColumnItem->mName = NULL;
            sInitNamedColumnItem->mPosition = 0;
            sInitNamedColumnItem->mNext = NULL;

            if( sCandNamedColumnItem->mLeftExpr->mType == QLNC_EXPR_TYPE_COLUMN )
            {
                sNode = ((qlncExprColumn*)(sCandNamedColumnItem->mLeftExpr))->mNode;
            }
            else if( sCandNamedColumnItem->mLeftExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN )
            {
                sNode = ((qlncExprInnerColumn*)(sCandNamedColumnItem->mLeftExpr))->mNode;
            }
            else
            {
                STL_DASSERT( sCandNamedColumnItem->mLeftExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );
                sNode = ((qlncExprRowidColumn*)(sCandNamedColumnItem->mLeftExpr))->mNode;
            }

            if( qlncIsExistCandNodeOnPlanNode( sPlanMergeJoin->mLeftChildPlanNode,
                                               sNode )
                == STL_TRUE )
            {
                STL_DASSERT( qlncIsExistCandNodeOnPlanNode( sPlanMergeJoin->mRightChildPlanNode,
                                                            sNode )
                             == STL_FALSE );
                sIsSamePosNode = STL_TRUE;
            }
            else
            {
                STL_DASSERT( qlncIsExistCandNodeOnPlanNode( sPlanMergeJoin->mRightChildPlanNode,
                                                            sNode )
                             == STL_TRUE );

                sIsSamePosNode = STL_FALSE;
            }

            QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                &sConvExprParamInfo,
                &(aPlanDetailParamInfo->mParamInfo),
                aPlanDetailParamInfo->mRegionMem,
                aPlanDetailParamInfo->mCandidateMem,
                aPlanDetailParamInfo->mInitExprList,
                aPlanDetailParamInfo->mTableStatList,
                0,
                NULL,
                0,
                NULL,
                ( sIsSamePosNode == STL_TRUE
                  ? sPlanMergeJoin->mLeftChildPlanNode
                  : sPlanMergeJoin->mRightChildPlanNode ),
                aPlanDetailParamInfo->mPlanNodeMapDynArr,
                NULL,
                STL_TRUE,
                sCandNamedColumnItem->mLeftExpr );

            STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandNamedColumnItem->mLeftExpr->mType](
                         &sConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );
            sInitNamedColumnItem->mLeftExpr = sConvExprParamInfo.mInitExpr;

            QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                &sConvExprParamInfo,
                &(aPlanDetailParamInfo->mParamInfo),
                aPlanDetailParamInfo->mRegionMem,
                aPlanDetailParamInfo->mCandidateMem,
                aPlanDetailParamInfo->mInitExprList,
                aPlanDetailParamInfo->mTableStatList,
                0,
                NULL,
                0,
                NULL,
                ( sIsSamePosNode == STL_TRUE
                  ? sPlanMergeJoin->mRightChildPlanNode
                  : sPlanMergeJoin->mLeftChildPlanNode ),
                aPlanDetailParamInfo->mPlanNodeMapDynArr,
                NULL,
                STL_TRUE,
                sCandNamedColumnItem->mRightExpr );

            STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandNamedColumnItem->mRightExpr->mType](
                         &sConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );
            sInitNamedColumnItem->mRightExpr = sConvExprParamInfo.mInitExpr;

            if( sInitJoinTable->mJoinSpec->mNamedColumnList->mCount == 0 )
            {
                sInitJoinTable->mJoinSpec->mNamedColumnList->mHead = sInitNamedColumnItem;
                sInitJoinTable->mJoinSpec->mNamedColumnList->mTail = sInitNamedColumnItem;
            }
            else
            {
                sInitJoinTable->mJoinSpec->mNamedColumnList->mTail->mNext = sInitNamedColumnItem;
                sInitJoinTable->mJoinSpec->mNamedColumnList->mTail = sInitNamedColumnItem;
            }
            sInitJoinTable->mJoinSpec->mNamedColumnList->mCount++;

            sCandNamedColumnItem = sCandNamedColumnItem->mNext;
        }
    }

    /* Aggregation List Backup 정보 원복 */
    if( (aPlanNode->mCandNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE) &&
        (aPlanDetailParamInfo->mInitExprList != NULL) )
    {
        aPlanDetailParamInfo->mInitExprList->mAggrExprList = sBackupAggrExprList;
        aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList = sBackupNestedAggrExprList;
    }

    
    /**
     * Output 설정
     */

    *aOutPlanNode = sOutPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Join Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailHashJoin( qlncPlanDetailParamInfo   * aPlanDetailParamInfo,
                                          qlncPlanCommon            * aPlanNode,
                                          qlncPlanCommon           ** aOutPlanNode,
                                          qllEnv                    * aEnv )
{
    qlncPlanCommon          * sOutPlanNode          = NULL;
    qlncPlanHashJoin        * sPlanHashJoin         = NULL;
    qlncHashJoinCost        * sHashJoinCost         = NULL;
    qlncConvExprParamInfo     sConvExprParamInfo;
    qlvRefExprList            sSubQueryExprList;
    qlvRefExprList            sTmpSubQueryExprList;
    qlncSubQueryAndFilter   * sSubQueryFilter       = NULL;
    qlvInitJoinTableNode    * sInitJoinTable        = NULL;
 
    qlvRefExprList          * sBackupAggrExprList       = NULL;
    qlvRefExprList          * sBackupNestedAggrExprList = NULL;

    qlncJoinOuterInfoItem     sJoinLeftOuterInfoItem;
    qlncJoinOuterInfoItem     sJoinRightOuterInfoItem;

    stlBool                   sIsSamePosNode;
    qlncNodeCommon          * sNode                     = NULL;
    qlncNamedColumnItem     * sCandNamedColumnItem      = NULL;
    qlvNamedColumnItem      * sInitNamedColumnItem      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_HASH_JOIN,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Hash Join Plan 설정 */
    sPlanHashJoin = (qlncPlanHashJoin*)aPlanNode;

    /* Hash Join Cost 설정 */
    sHashJoinCost = (qlncHashJoinCost*)(aPlanNode->mCandNode->mBestCost);

    /* Join Left Outer Column List 할당 */
    STL_TRY( qlvCreateRefExprList( &(sPlanHashJoin->mJoinLeftOuterColumnList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Join Right Outer Column List 할당 */
    STL_TRY( qlvCreateRefExprList( &(sPlanHashJoin->mJoinRightOuterColumnList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Plan Node Map Dynamic Array에 Right Plan Node를 추가 */
    STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             sPlanHashJoin->mRightChildPlanNode,
                                             aEnv )
             == STL_SUCCESS );

    /* Left Plan 설정 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanHashJoin->mLeftChildPlanNode->mPlanType ](
                 aPlanDetailParamInfo,
                 sPlanHashJoin->mLeftChildPlanNode,
                 &(sPlanHashJoin->mLeftChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
    STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             1,
                                             aEnv )
             == STL_SUCCESS );

    /* Aggregation List 정보 Backup */
    if( (aPlanNode->mCandNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE) &&
        (aPlanDetailParamInfo->mInitExprList != NULL) )
    {
        sBackupAggrExprList = aPlanDetailParamInfo->mInitExprList->mAggrExprList;
        sBackupNestedAggrExprList = aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList;

        /* Aggregation Node 찾기 */
        STL_DASSERT( sPlanHashJoin->mRightChildPlanNode->mPlanType == QLNC_PLAN_TYPE_HASH_JOIN_INSTANT );
        STL_TRY( qlncMCPSetAggrExprList( ((qlncPlanHashInstant*)(sPlanHashJoin->mRightChildPlanNode))->mChildPlanNode,
                                         &(aPlanDetailParamInfo->mInitExprList->mAggrExprList),
                                         &(aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList),
                                         aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sBackupAggrExprList = NULL;
        sBackupNestedAggrExprList = NULL;
    }

    /* Plan Node Map Dynamic Array에 Left Plan Node를 추가 */
    STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             sPlanHashJoin->mLeftChildPlanNode,
                                             aEnv )
             == STL_SUCCESS );

    /* Right Plan 설정 */
    sJoinLeftOuterInfoItem.mOuterPlanNode = sPlanHashJoin->mLeftChildPlanNode;
    sJoinLeftOuterInfoItem.mOuterColumnList = sPlanHashJoin->mJoinLeftOuterColumnList;
    sJoinLeftOuterInfoItem.mNext = aPlanDetailParamInfo->mJoinOuterInfoList.mHead;

    aPlanDetailParamInfo->mJoinOuterInfoList.mHead = &sJoinLeftOuterInfoItem;
    aPlanDetailParamInfo->mJoinOuterInfoList.mCount++;

    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanHashJoin->mRightChildPlanNode->mPlanType ](
                 aPlanDetailParamInfo,
                 sPlanHashJoin->mRightChildPlanNode,
                 &(sPlanHashJoin->mRightChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    aPlanDetailParamInfo->mJoinOuterInfoList.mHead =
        aPlanDetailParamInfo->mJoinOuterInfoList.mHead->mNext;
    aPlanDetailParamInfo->mJoinOuterInfoList.mCount--;

    /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
    STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             1,
                                             aEnv )
             == STL_SUCCESS );

    /* 기타 변수 초기화 */
    STL_TRY( qlvCreateRefExprList( &(sPlanHashJoin->mReadColList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );
    sPlanHashJoin->mReadColMapList = NULL;

    /* Left Join Outer Info와 Right Join Outer Info 설정 */
    sJoinLeftOuterInfoItem.mOuterPlanNode = sPlanHashJoin->mLeftChildPlanNode;
    sJoinLeftOuterInfoItem.mOuterColumnList = sPlanHashJoin->mJoinLeftOuterColumnList;
    sJoinLeftOuterInfoItem.mNext = NULL;
    sJoinRightOuterInfoItem.mOuterPlanNode = sPlanHashJoin->mRightChildPlanNode;
    sJoinRightOuterInfoItem.mOuterColumnList = sPlanHashJoin->mJoinRightOuterColumnList;
    sJoinRightOuterInfoItem.mNext = NULL;

    /* Left Hash Key Expression 설정 */
#if 0
    STL_DASSERT( sHashJoinCost->mLeftHashKeyArr != NULL );

    STL_TRY( qlvCreateRefExprList( &(sPlanHashJoin->mLeftHashKeyList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Join Outer Info 설정 */
    for( i = 0; i < sHashJoinCost->mHashKeyCount; i++ )
    {
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            0,
            NULL,
            0,
            NULL,
            (qlncPlanCommon*)(sPlanHashJoin->mRightChildPlanNode),
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            NULL,
            STL_TRUE,
            (qlncExprCommon*)(sHashJoinCost->mLeftHashKeyArr[i]) );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[ sHashJoinCost->mLeftHashKeyArr[i]->mType ](
                     &sConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlvAddExprToRefExprList( sPlanHashJoin->mLeftHashKeyList,
                                          sConvExprParamInfo.mInitExpr,
                                          STL_FALSE,
                                          aPlanDetailParamInfo->mRegionMem,
                                          aEnv )
                 == STL_SUCCESS );
    }
#else
    sPlanHashJoin->mLeftHashKeyList = NULL;
#endif

    QLNC_INIT_LIST( &sSubQueryExprList );

    /* Non Equi-Join Condition Expression 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( sHashJoinCost->mNonEquiJoinCondition != NULL)
    {
        /* Plan Node Map Dynamic Array에 Left Plan Node와 Right Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 sPlanHashJoin->mLeftChildPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 sPlanHashJoin->mRightChildPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            1,
            &sJoinLeftOuterInfoItem,
            1,
            &sJoinRightOuterInfoItem,
            (qlncPlanCommon*)sPlanHashJoin,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            (qlncExprCommon*)(sHashJoinCost->mNonEquiJoinCondition) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanHashJoin->mJoinFilterExpr = sConvExprParamInfo.mInitExpr;
        sSubQueryFilter = sConvExprParamInfo.mSubQueryAndFilter;

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 2,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sPlanHashJoin->mJoinFilterExpr = NULL;
        sSubQueryFilter = NULL;
    }

    /* SubQuery가 존재하는 Filter는 Condition이 아닌 Filter로 분류되어 있다. */
    STL_DASSERT( sSubQueryFilter == NULL );

    QLNC_COPY_LIST_INFO( &sTmpSubQueryExprList, &sSubQueryExprList );

    /* Join Filter Expression 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( sHashJoinCost->mJoinFilter != NULL)
    {
        /* Plan Node Map Dynamic Array에 현재 Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 aPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanHashJoin,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            (qlncExprCommon*)(sHashJoinCost->mJoinFilter) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanHashJoin->mWhereFilterExpr = sConvExprParamInfo.mInitExpr;

        STL_DASSERT( sSubQueryFilter == NULL );
        sSubQueryFilter = sConvExprParamInfo.mSubQueryAndFilter;

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sPlanHashJoin->mWhereFilterExpr = NULL;
    }

    /* List Merge */
    if( sTmpSubQueryExprList.mCount > 0 )
    {
        if( sSubQueryExprList.mCount == 0 )
        {
            QLNC_COPY_LIST_INFO( &sTmpSubQueryExprList, &sSubQueryExprList );
        }
        else
        {
            /* @todo 현재 여기에 들어오는 경우는 없음 */

            sSubQueryExprList.mTail->mNext  = sTmpSubQueryExprList.mHead;
            sSubQueryExprList.mTail         = sTmpSubQueryExprList.mTail;
            sSubQueryExprList.mCount       += sTmpSubQueryExprList.mCount;
        }
    }

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sSubQueryFilter != NULL )
    {
        /* 상위 노드로 SubQuery Filter Plan과 SubQuery Plan을 생성 */
        STL_TRY( qlncMakeCandPlanSubQueryCondition( aPlanDetailParamInfo->mRegionMem,
                                                    aPlanDetailParamInfo->mCandidateMem,
                                                    (qlncPlanCommon*)sPlanHashJoin,
                                                    sSubQueryFilter,
                                                    &sOutPlanNode,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOutPlanNode = (qlncPlanCommon*)sPlanHashJoin;
    }

    /* Init Node 정보 설정 : 주석 부분은 MakeCandBasic에서 이미 설정한 부분임 */
    sInitJoinTable = (qlvInitJoinTableNode *) sPlanHashJoin->mCommon.mInitNode;
    sInitJoinTable->mJoinType = sPlanHashJoin->mJoinType;
    sInitJoinTable->mLeftTableNode = sPlanHashJoin->mLeftChildPlanNode->mInitNode;
    sInitJoinTable->mRightTableNode = sPlanHashJoin->mRightChildPlanNode->mInitNode;
    sInitJoinTable->mJoinSpec = NULL;   /* default */
    sInitJoinTable->mRefColumnList = sPlanHashJoin->mReadColList;
    /* sInitJoinTable->mOuterColumnList = sPlanHashJoin->mOuterColumnList; */

    /* Named Column List 설정 */
    if( (aPlanNode->mCandNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE) &&
        (((qlncInnerJoinTableNode*)(aPlanNode->mCandNode))->mNamedColumnList != NULL) )
    {
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitJoinSpec ),
                                    (void**)&(sInitJoinTable->mJoinSpec),
                                    KNL_ENV(aEnv) )
             == STL_SUCCESS );

        sInitJoinTable->mJoinSpec->mJoinCondition = NULL;

        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvNamedColumnList ),
                                    (void**)&(sInitJoinTable->mJoinSpec->mNamedColumnList),
                                    KNL_ENV(aEnv) )
             == STL_SUCCESS );

        sInitJoinTable->mJoinSpec->mNamedColumnList->mCount = 0;
        sInitJoinTable->mJoinSpec->mNamedColumnList->mHead = NULL;
        sInitJoinTable->mJoinSpec->mNamedColumnList->mTail = NULL;

        sCandNamedColumnItem =
            ((qlncInnerJoinTableNode*)(aPlanNode->mCandNode))->mNamedColumnList->mHead;
        while( sCandNamedColumnItem != NULL )
        {
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvNamedColumnItem ),
                                        (void**)&(sInitNamedColumnItem),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sInitNamedColumnItem->mName = NULL;
            sInitNamedColumnItem->mPosition = 0;
            sInitNamedColumnItem->mNext = NULL;

            if( sCandNamedColumnItem->mLeftExpr->mType == QLNC_EXPR_TYPE_COLUMN )
            {
                sNode = ((qlncExprColumn*)(sCandNamedColumnItem->mLeftExpr))->mNode;
            }
            else if( sCandNamedColumnItem->mLeftExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN )
            {
                sNode = ((qlncExprInnerColumn*)(sCandNamedColumnItem->mLeftExpr))->mNode;
            }
            else
            {
                STL_DASSERT( sCandNamedColumnItem->mLeftExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );
                sNode = ((qlncExprRowidColumn*)(sCandNamedColumnItem->mLeftExpr))->mNode;
            }

            if( qlncIsExistCandNodeOnPlanNode( sPlanHashJoin->mLeftChildPlanNode,
                                               sNode )
                == STL_TRUE )
            {
                STL_DASSERT( qlncIsExistCandNodeOnPlanNode( sPlanHashJoin->mRightChildPlanNode,
                                                            sNode )
                             == STL_FALSE );
                sIsSamePosNode = STL_TRUE;
            }
            else
            {
                STL_DASSERT( qlncIsExistCandNodeOnPlanNode( sPlanHashJoin->mRightChildPlanNode,
                                                            sNode )
                             == STL_TRUE );

                sIsSamePosNode = STL_FALSE;
            }

            QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                &sConvExprParamInfo,
                &(aPlanDetailParamInfo->mParamInfo),
                aPlanDetailParamInfo->mRegionMem,
                aPlanDetailParamInfo->mCandidateMem,
                aPlanDetailParamInfo->mInitExprList,
                aPlanDetailParamInfo->mTableStatList,
                0,
                NULL,
                0,
                NULL,
                ( sIsSamePosNode == STL_TRUE
                  ? sPlanHashJoin->mLeftChildPlanNode
                  : sPlanHashJoin->mRightChildPlanNode ),
                aPlanDetailParamInfo->mPlanNodeMapDynArr,
                NULL,
                STL_TRUE,
                sCandNamedColumnItem->mLeftExpr );

            STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandNamedColumnItem->mLeftExpr->mType](
                         &sConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );
            sInitNamedColumnItem->mLeftExpr = sConvExprParamInfo.mInitExpr;

            QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                &sConvExprParamInfo,
                &(aPlanDetailParamInfo->mParamInfo),
                aPlanDetailParamInfo->mRegionMem,
                aPlanDetailParamInfo->mCandidateMem,
                aPlanDetailParamInfo->mInitExprList,
                aPlanDetailParamInfo->mTableStatList,
                0,
                NULL,
                0,
                NULL,
                ( sIsSamePosNode == STL_TRUE
                  ? sPlanHashJoin->mRightChildPlanNode
                  : sPlanHashJoin->mLeftChildPlanNode ),
                aPlanDetailParamInfo->mPlanNodeMapDynArr,
                NULL,
                STL_TRUE,
                sCandNamedColumnItem->mRightExpr );

            STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandNamedColumnItem->mRightExpr->mType](
                         &sConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );
            sInitNamedColumnItem->mRightExpr = sConvExprParamInfo.mInitExpr;

            if( sInitJoinTable->mJoinSpec->mNamedColumnList->mCount == 0 )
            {
                sInitJoinTable->mJoinSpec->mNamedColumnList->mHead = sInitNamedColumnItem;
                sInitJoinTable->mJoinSpec->mNamedColumnList->mTail = sInitNamedColumnItem;
            }
            else
            {
                sInitJoinTable->mJoinSpec->mNamedColumnList->mTail->mNext = sInitNamedColumnItem;
                sInitJoinTable->mJoinSpec->mNamedColumnList->mTail = sInitNamedColumnItem;
            }
            sInitJoinTable->mJoinSpec->mNamedColumnList->mCount++;

            sCandNamedColumnItem = sCandNamedColumnItem->mNext;
        }
    }

    /* Aggregation List Backup 정보 원복 */
    if( (aPlanNode->mCandNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE) &&
        (aPlanDetailParamInfo->mInitExprList != NULL) )
    {
        aPlanDetailParamInfo->mInitExprList->mAggrExprList = sBackupAggrExprList;
        aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList = sBackupNestedAggrExprList;
    }

    
    /**
     * Output 설정
     */

    *aOutPlanNode = sOutPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Combine Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailJoinCombine( qlncPlanDetailParamInfo    * aPlanDetailParamInfo,
                                             qlncPlanCommon             * aPlanNode,
                                             qlncPlanCommon            ** aOutPlanNode,
                                             qllEnv                     * aEnv )
{
    stlInt32                  i;
    qlncPlanJoinCombine     * sPlanJoinCombine  = NULL;
    qlvInitJoinTableNode    * sInitJoinTable    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_JOIN_COMBINE,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Join Combine Plan 설정 */
    sPlanJoinCombine = (qlncPlanJoinCombine*)aPlanNode;

    /* 기타 변수 초기화 */
    sPlanJoinCombine->mReadColList = NULL;
    sPlanJoinCombine->mReadColMapList = NULL;

    /* Child Node 수행 */
    for( i = 0; i < sPlanJoinCombine->mChildCount; i++ )
    {
        STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanJoinCombine->mChildPlanArr[i]->mPlanType ](
                     aPlanDetailParamInfo,
                     sPlanJoinCombine->mChildPlanArr[i],
                     &(sPlanJoinCombine->mChildPlanArr[i]),
                     aEnv )
                 == STL_SUCCESS );
    }

    /* Init Node 정보 설정 : 주석 부분은 MakeCandBasic에서 이미 설정한 부분임 */
    sInitJoinTable = (qlvInitJoinTableNode *) sPlanJoinCombine->mCommon.mInitNode;
    sInitJoinTable->mJoinType = QLV_JOIN_TYPE_INNER;
    sInitJoinTable->mLeftTableNode = sPlanJoinCombine->mChildPlanArr[0]->mInitNode;
    sInitJoinTable->mRightTableNode = sPlanJoinCombine->mChildPlanArr[0]->mInitNode;
    sInitJoinTable->mJoinSpec = NULL;   /* default */
    sInitJoinTable->mRefColumnList = sPlanJoinCombine->mReadColList;
    /* sInitJoinTable->mOuterColumnList = sPlanHashJoin->mOuterColumnList; */

    
    /**
     * Output 설정
     */

    *aOutPlanNode = (qlncPlanCommon*)sPlanJoinCombine;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sort Instant Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailSortInstant( qlncPlanDetailParamInfo    * aPlanDetailParamInfo,
                                             qlncPlanCommon             * aPlanNode,
                                             qlncPlanCommon            ** aOutPlanNode,
                                             qllEnv                     * aEnv )
{
    stlInt32                  sIdx;
    qlncPlanCommon          * sOutPlanNode          = NULL;
    qlncSortInstantNode     * sNodeSortInstant      = NULL;
    qlncPlanSortInstant     * sPlanSortInstant      = NULL;
    qlncConvExprParamInfo     sConvExprParamInfo;
    qlncRefExprItem         * sCandRefExprItem      = NULL;
    qlncExprInnerColumn     * sCandInnerColumn      = NULL;
    qlvInitInnerColumn      * sInitInnerColumn      = NULL;
    qlvInitExpression       * sInitExpr             = NULL;
    qlvRefExprList            sTmpSubQueryExprList;
    qlncSubQueryAndFilter   * sSubQueryFilter       = NULL;
    qlvInitInstantNode      * sInitInstant          = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aPlanNode->mPlanType == QLNC_PLAN_TYPE_SORT_INSTANT) ||
                        (aPlanNode->mPlanType == QLNC_PLAN_TYPE_SORT_GROUP_INSTANT) ||
                        (aPlanNode->mPlanType == QLNC_PLAN_TYPE_SORT_JOIN_INSTANT),
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Sort Instant Plan 설정 */
    sPlanSortInstant = (qlncPlanSortInstant*)aPlanNode;

    /* Sort Instant Node 설정 */
    sNodeSortInstant = (qlncSortInstantNode*)(aPlanNode->mCandNode);

    /* Child Plan 설정 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanSortInstant->mChildPlanNode->mPlanType ](
                 aPlanDetailParamInfo,
                 sPlanSortInstant->mChildPlanNode,
                 &(sPlanSortInstant->mChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    /* 기타 변수 초기화 */
    sPlanSortInstant->mKeyColList = NULL;
    sPlanSortInstant->mRecColList = NULL;
    sPlanSortInstant->mReadColList = NULL;
    sPlanSortInstant->mKeyColMapList = NULL;
    sPlanSortInstant->mRecColMapList = NULL;
    sPlanSortInstant->mReadColMapList = NULL;

    QLNC_INIT_LIST( &sTmpSubQueryExprList );

    /* Plan Node Map Dynamic Array에 Child Plan Node를 추가 */
    STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             sPlanSortInstant->mChildPlanNode,
                                             aEnv )
             == STL_SUCCESS );

    /* Key Column List 설정 */
    STL_DASSERT( sNodeSortInstant->mKeyColList != NULL );

    /* Key Flags 할당 */
    STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                STL_SIZEOF( stlUInt8 ) * sNodeSortInstant->mKeyColList->mCount,
                                (void**) &(sPlanSortInstant->mKeyFlags),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Key Column List 할당 */
    STL_TRY( qlvCreateRefExprList( &(sPlanSortInstant->mKeyColList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Key Column Map List 할당 */
    STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mCandidateMem,
                                STL_SIZEOF( qlncReadColMapList ),
                                (void**) &(sPlanSortInstant->mKeyColMapList),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_LIST( sPlanSortInstant->mKeyColMapList );

    sIdx = 0;
    sCandRefExprItem = sNodeSortInstant->mKeyColList->mHead;
    while( sCandRefExprItem != NULL )
    {
        STL_DASSERT( sCandRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
        sCandInnerColumn = (qlncExprInnerColumn*)(sCandRefExprItem->mExpr);

        STL_DASSERT( sCandInnerColumn->mNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT );

        /* 하위 노드에 대한 Expression 처리 */
        if( sPlanSortInstant->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SET_OP )
        {
            /* Set Operation에서의 Order By인 경우 */
            QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                &sConvExprParamInfo,
                &(aPlanDetailParamInfo->mParamInfo),
                aPlanDetailParamInfo->mRegionMem,
                aPlanDetailParamInfo->mCandidateMem,
                aPlanDetailParamInfo->mInitExprList,
                aPlanDetailParamInfo->mTableStatList,
                aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
                aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
                0,
                NULL,
                sPlanSortInstant->mChildPlanNode,
                aPlanDetailParamInfo->mPlanNodeMapDynArr,
                &sTmpSubQueryExprList,
                STL_FALSE,
                sCandInnerColumn->mOrgExpr );

            STL_TRY( qlncAddReadColumnOnCurPlan_SetOP(
                         &sConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                &sConvExprParamInfo,
                &(aPlanDetailParamInfo->mParamInfo),
                aPlanDetailParamInfo->mRegionMem,
                aPlanDetailParamInfo->mCandidateMem,
                aPlanDetailParamInfo->mInitExprList,
                aPlanDetailParamInfo->mTableStatList,
                aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
                aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
                0,
                NULL,
                (qlncPlanCommon*)sPlanSortInstant,
                aPlanDetailParamInfo->mPlanNodeMapDynArr,
                &sTmpSubQueryExprList,
                STL_FALSE,
                sCandInnerColumn->mOrgExpr );

            STL_TRY( qlncConvertCandExprToInitExprFuncList[ sCandInnerColumn->mOrgExpr->mType ](
                         &sConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );
        }

        /* Init Inner Column 생성 */
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitInnerColumn ),
                                    (void**) &sInitInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Relation Node 설정 */
        sInitInnerColumn->mRelationNode = sPlanSortInstant->mCommon.mInitNode;

        /* Original Expression 설정 */
        sInitInnerColumn->mOrgExpr = sConvExprParamInfo.mInitExpr;

        /* Idx 설정 */
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( stlInt32 ),
                                    (void**) &(sInitInnerColumn->mIdx),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        *(sInitInnerColumn->mIdx) = sIdx;

        /* Init Expression 생성 */
        QLNC_MAKE_INIT_EXPRESSION( aPlanDetailParamInfo->mRegionMem,
                                   QLV_EXPR_TYPE_INNER_COLUMN,
                                   sInitInnerColumn->mOrgExpr->mRelatedFuncId,
                                   DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                   sConvExprParamInfo.mInitExpr->mPosition,
                                   QLNC_EXPR_PHRASE_WHERE,
                                   sInitInnerColumn->mOrgExpr->mIncludeExprCnt,
                                   sInitInnerColumn->mOrgExpr->mColumnName,
                                   sInitInnerColumn,
                                   sInitExpr,
                                   aEnv );

        /* Key Column List에 추가 */
        STL_TRY( qlvAddExprToRefExprList( sPlanSortInstant->mKeyColList,
                                          sInitExpr,
                                          STL_FALSE,
                                          aPlanDetailParamInfo->mRegionMem,
                                          aEnv )
                 == STL_SUCCESS );

        /* Key Column Map List에 추가 (Original Expression에 대해) */
        QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                  sPlanSortInstant->mKeyColMapList,
                                  sCandInnerColumn->mOrgExpr,
                                  sInitExpr,
                                  aEnv );

        /* Key Column Map List에 추가 (Inner Column Expression에 대해) */
        QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                  sPlanSortInstant->mKeyColMapList,
                                  sCandRefExprItem->mExpr,
                                  sInitExpr,
                                  aEnv );

        /* Key Flags 설정 */
        sPlanSortInstant->mKeyFlags[sIdx] = sNodeSortInstant->mKeyFlags[sIdx];

        sIdx++;

        sCandRefExprItem = sCandRefExprItem->mNext;
    }

    /* Record Column List 설정 */
    if( sNodeSortInstant->mRecColList != NULL )
    {
        /* Record Column List 할당 */
        STL_TRY( qlvCreateRefExprList( &(sPlanSortInstant->mRecColList),
                                       aPlanDetailParamInfo->mRegionMem,
                                       aEnv )
                 == STL_SUCCESS );

        /* Record Column Map List 할당 */
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mCandidateMem,
                                    STL_SIZEOF( qlncReadColMapList ),
                                    (void**) &(sPlanSortInstant->mRecColMapList),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_LIST( sPlanSortInstant->mRecColMapList );

        sCandRefExprItem = sNodeSortInstant->mRecColList->mHead;
        while( sCandRefExprItem != NULL )
        {
            STL_DASSERT( sCandRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
            sCandInnerColumn = (qlncExprInnerColumn*)(sCandRefExprItem->mExpr);

            STL_DASSERT( sCandInnerColumn->mNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT );

            /* 하위 노드에 대한 Expression 처리 */
            if( sPlanSortInstant->mChildPlanNode->mPlanType == QLNC_PLAN_TYPE_SET_OP )
            {
                /* 하위 노드에 대한 Expression 처리 */
                QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                    &sConvExprParamInfo,
                    &(aPlanDetailParamInfo->mParamInfo),
                    aPlanDetailParamInfo->mRegionMem,
                    aPlanDetailParamInfo->mCandidateMem,
                    aPlanDetailParamInfo->mInitExprList,
                    aPlanDetailParamInfo->mTableStatList,
                    aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
                    aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
                    0,
                    NULL,
                    sPlanSortInstant->mChildPlanNode,
                    aPlanDetailParamInfo->mPlanNodeMapDynArr,
                    &sTmpSubQueryExprList,
                    STL_FALSE,
                    sCandInnerColumn->mOrgExpr );

                    STL_TRY( qlncAddReadColumnOnCurPlan_SetOP(
                                 &sConvExprParamInfo,
                                 aEnv )
                             == STL_SUCCESS );
            }
            else
            {
                /* 하위 노드에 대한 Expression 처리 */
                QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                    &sConvExprParamInfo,
                    &(aPlanDetailParamInfo->mParamInfo),
                    aPlanDetailParamInfo->mRegionMem,
                    aPlanDetailParamInfo->mCandidateMem,
                    aPlanDetailParamInfo->mInitExprList,
                    aPlanDetailParamInfo->mTableStatList,
                    aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
                    aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
                    0,
                    NULL,
                    (qlncPlanCommon*)sPlanSortInstant,
                    aPlanDetailParamInfo->mPlanNodeMapDynArr,
                    &sTmpSubQueryExprList,
                    STL_FALSE,
                    sCandInnerColumn->mOrgExpr );

                STL_TRY( qlncConvertCandExprToInitExprFuncList[ sCandInnerColumn->mOrgExpr->mType ](
                             &sConvExprParamInfo,
                             aEnv )
                         == STL_SUCCESS );
            }

            /* Init Inner Column 생성 */
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitInnerColumn ),
                                        (void**) &sInitInnerColumn,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /* Relation Node 설정 */
            sInitInnerColumn->mRelationNode = sPlanSortInstant->mCommon.mInitNode;

            /* Original Expression 설정 */
            sInitInnerColumn->mOrgExpr = sConvExprParamInfo.mInitExpr;

            /* Idx 설정 */
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( stlInt32 ),
                                        (void**) &(sInitInnerColumn->mIdx),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            *(sInitInnerColumn->mIdx) = sIdx;

            /* Init Expression 생성 */
            QLNC_MAKE_INIT_EXPRESSION( aPlanDetailParamInfo->mRegionMem,
                                       QLV_EXPR_TYPE_INNER_COLUMN,
                                       sInitInnerColumn->mOrgExpr->mRelatedFuncId,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       sConvExprParamInfo.mInitExpr->mPosition,
                                       QLNC_EXPR_PHRASE_WHERE,
                                       sInitInnerColumn->mOrgExpr->mIncludeExprCnt,
                                       sInitInnerColumn->mOrgExpr->mColumnName,
                                       sInitInnerColumn,
                                       sInitExpr,
                                       aEnv );

            /* Record Column List에 추가 */
            STL_TRY( qlvAddExprToRefExprList( sPlanSortInstant->mRecColList,
                                              sInitExpr,
                                              STL_FALSE,
                                              aPlanDetailParamInfo->mRegionMem,
                                              aEnv )
                     == STL_SUCCESS );

            /* Record Column Map List에 추가 (Original Expression에 대해) */
            QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                      sPlanSortInstant->mRecColMapList,
                                      sCandInnerColumn->mOrgExpr,
                                      sInitExpr,
                                      aEnv );

            /* Record Column Map List에 추가 (Inner Column Expression에 대해) */
            QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                      sPlanSortInstant->mRecColMapList,
                                      sCandRefExprItem->mExpr,
                                      sInitExpr,
                                      aEnv );

            sIdx++;

            sCandRefExprItem = sCandRefExprItem->mNext;
        }
    }
    else
    {
        STL_TRY( qlvCreateRefExprList( &(sPlanSortInstant->mRecColList),
                                       aPlanDetailParamInfo->mRegionMem,
                                       aEnv )
                 == STL_SUCCESS );
    }

    /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
    STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             1,
                                             aEnv )
             == STL_SUCCESS );

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sTmpSubQueryExprList.mCount > 0 )
    {
        /* 하위 노드로 SubQuery Plan을 생성 */
        STL_TRY( qlncMakeCandPlanSubQueryList( aPlanDetailParamInfo->mRegionMem,
                                               aPlanDetailParamInfo->mCandidateMem,
                                               sPlanSortInstant->mChildPlanNode,
                                               &sTmpSubQueryExprList,
                                               &(sPlanSortInstant->mChildPlanNode),
                                           aEnv )
                 == STL_SUCCESS );
    }

    /* Read Column List 할당 */
    STL_TRY( qlvCreateRefExprList( &(sPlanSortInstant->mReadColList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Filter Expression 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( sNodeSortInstant->mFilter != NULL)
    {
        /* Plan Node Map Dynamic Array에 현재 Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 aPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanSortInstant,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            (qlncExprCommon*)(sNodeSortInstant->mFilter) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanSortInstant->mFilterExpr = sConvExprParamInfo.mInitExpr;
        sSubQueryFilter = sConvExprParamInfo.mSubQueryAndFilter;

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sPlanSortInstant->mFilterExpr = NULL;
        sSubQueryFilter = NULL;
    }

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sSubQueryFilter != NULL )
    {
        /* 상위 노드로 SubQuery Filter Plan과 SubQuery Plan을 생성 */
        STL_TRY( qlncMakeCandPlanSubQueryCondition( aPlanDetailParamInfo->mRegionMem,
                                                    aPlanDetailParamInfo->mCandidateMem,
                                                    (qlncPlanCommon*)sPlanSortInstant,
                                                    sSubQueryFilter,
                                                    &sOutPlanNode,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOutPlanNode = (qlncPlanCommon*)sPlanSortInstant;
    }

    /* Init Node 정보 설정 : 주석 부분은 MakeCandBasic에서 이미 설정한 부분임 */
    sInitInstant = (qlvInitInstantNode *) sPlanSortInstant->mCommon.mInitNode;
    sInitInstant->mRelationName = NULL;   /* default */
    /* sInitInstant->mTableNode = NULL; */

    sInitInstant->mKeyColList = sPlanSortInstant->mKeyColList;
    sInitInstant->mRecColList = sPlanSortInstant->mRecColList;
    sInitInstant->mReadColList = sPlanSortInstant->mReadColList;
    sInitInstant->mKeyColCnt = sPlanSortInstant->mKeyColList->mCount;
    sInitInstant->mRecColCnt = sPlanSortInstant->mRecColList->mCount;
    sInitInstant->mReadColCnt = sPlanSortInstant->mReadColList->mCount;

    /* sInitInstant->mOuterColumnList = sPlanSortInstant->mOuterColumnList; */

    STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                STL_SIZEOF( stlUInt8 ) * sInitInstant->mKeyColCnt,
                                (void**) &(sInitInstant->mKeyFlags),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemcpy( sInitInstant->mKeyFlags,
               sPlanSortInstant->mKeyFlags,
               STL_SIZEOF( stlUInt8 ) * sInitInstant->mKeyColCnt );
    
    
    /**
     * Output 설정
     */

    *aOutPlanNode = sOutPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Instant Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailHashInstant( qlncPlanDetailParamInfo    * aPlanDetailParamInfo,
                                             qlncPlanCommon             * aPlanNode,
                                             qlncPlanCommon            ** aOutPlanNode,
                                             qllEnv                     * aEnv )
{
    stlInt32                  sIdx;
    qlncPlanCommon          * sOutPlanNode          = NULL;
    qlncHashInstantNode     * sNodeHashInstant      = NULL;
    qlncPlanHashInstant     * sPlanHashInstant      = NULL;
    qlncConvExprParamInfo     sConvExprParamInfo;
    qlncRefExprItem         * sCandRefExprItem      = NULL;
    qlncExprInnerColumn     * sCandInnerColumn      = NULL;
    qlvInitInnerColumn      * sInitInnerColumn      = NULL;
    qlvInitExpression       * sInitExpr             = NULL;
    qlvRefExprList            sTmpSubQueryExprList;
    qlncSubQueryAndFilter   * sSubQueryFilter       = NULL;
    qlvInitInstantNode      * sInitInstant          = NULL;

    qlvRefExprList          * sBackupAggrExprList       = NULL;
    qlvRefExprList          * sBackupNestedAggrExprList = NULL;
    qlncAndFilter             sTmpAndFilter;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aPlanNode->mPlanType == QLNC_PLAN_TYPE_HASH_INSTANT) ||
                        (aPlanNode->mPlanType == QLNC_PLAN_TYPE_HASH_GROUP_INSTANT) ||
                        (aPlanNode->mPlanType == QLNC_PLAN_TYPE_HASH_JOIN_INSTANT),
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Aggregation List 정보 Backup */
    if( (aPlanNode->mPlanType == QLNC_PLAN_TYPE_HASH_GROUP_INSTANT) &&
        (aPlanDetailParamInfo->mInitExprList != NULL) )
    {
        sBackupAggrExprList = aPlanDetailParamInfo->mInitExprList->mAggrExprList;
        sBackupNestedAggrExprList = aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList;

        /* Aggregation Node 찾기 */
        STL_TRY( qlncMCPSetAggrExprList( aPlanNode,
                                         &(aPlanDetailParamInfo->mInitExprList->mAggrExprList),
                                         &(aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList),
                                         aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sBackupAggrExprList = NULL;
        sBackupNestedAggrExprList = NULL;
    }

    /* Hash Instant Plan 설정 */
    sPlanHashInstant = (qlncPlanHashInstant*)aPlanNode;

    /* Hash Instant Node 설정 */
    sNodeHashInstant = (qlncHashInstantNode*)(aPlanNode->mCandNode);

    /* Child Plan 설정 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanHashInstant->mChildPlanNode->mPlanType ](
                 aPlanDetailParamInfo,
                 sPlanHashInstant->mChildPlanNode,
                 &(sPlanHashInstant->mChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    /* 기타 변수 초기화 */
    sPlanHashInstant->mKeyColList = NULL;
    sPlanHashInstant->mRecColList = NULL;
    sPlanHashInstant->mReadColList = NULL;
    sPlanHashInstant->mKeyColMapList = NULL;
    sPlanHashInstant->mRecColMapList = NULL;
    sPlanHashInstant->mReadColMapList = NULL;

    QLNC_INIT_LIST( &sTmpSubQueryExprList );

    /* Plan Node Map Dynamic Array에 Child Plan Node를 추가 */
    STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             sPlanHashInstant->mChildPlanNode,
                                             aEnv )
             == STL_SUCCESS );

    /* Key Column List 설정 */
    STL_DASSERT( sNodeHashInstant->mKeyColList != NULL );

    /* Key Column List 할당 */
    STL_TRY( qlvCreateRefExprList( &(sPlanHashInstant->mKeyColList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Key Column Map List 할당 */
    STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mCandidateMem,
                                STL_SIZEOF( qlncReadColMapList ),
                                (void**) &(sPlanHashInstant->mKeyColMapList),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_LIST( sPlanHashInstant->mKeyColMapList );

    sIdx = 0;
    sCandRefExprItem = sNodeHashInstant->mKeyColList->mHead;
    while( sCandRefExprItem != NULL )
    {
        STL_DASSERT( sCandRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
        sCandInnerColumn = (qlncExprInnerColumn*)(sCandRefExprItem->mExpr);

        STL_DASSERT( sCandInnerColumn->mNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT );

        /* 하위 노드에 대한 Expression 처리 */
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanHashInstant,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_FALSE,
            sCandInnerColumn->mOrgExpr );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[ sCandInnerColumn->mOrgExpr->mType ]( &sConvExprParamInfo,
                                                                                             aEnv )
                 == STL_SUCCESS );

        /* Init Inner Column 생성 */
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitInnerColumn ),
                                    (void**) &sInitInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Relation Node 설정 */
        sInitInnerColumn->mRelationNode = sPlanHashInstant->mCommon.mInitNode;

        /* Original Expression 설정 */
        sInitInnerColumn->mOrgExpr = sConvExprParamInfo.mInitExpr;

        /* Idx 설정 */
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( stlInt32 ),
                                    (void**) &(sInitInnerColumn->mIdx),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        *(sInitInnerColumn->mIdx) = sIdx;

        /* Init Expression 생성 */
        QLNC_MAKE_INIT_EXPRESSION( aPlanDetailParamInfo->mRegionMem,
                                   QLV_EXPR_TYPE_INNER_COLUMN,
                                   sInitInnerColumn->mOrgExpr->mRelatedFuncId,
                                   DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                   sConvExprParamInfo.mInitExpr->mPosition,
                                   QLNC_EXPR_PHRASE_WHERE,
                                   sInitInnerColumn->mOrgExpr->mIncludeExprCnt,
                                   sInitInnerColumn->mOrgExpr->mColumnName,
                                   sInitInnerColumn,
                                   sInitExpr,
                                   aEnv );

        /* Key Column List에 추가 */
        STL_TRY( qlvAddExprToRefExprList( sPlanHashInstant->mKeyColList,
                                          sInitExpr,
                                          STL_FALSE,
                                          aPlanDetailParamInfo->mRegionMem,
                                          aEnv )
                 == STL_SUCCESS );

        /* Key Column Map List에 추가 (Original Expression에 대해) */
        QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                  sPlanHashInstant->mKeyColMapList,
                                  sCandInnerColumn->mOrgExpr,
                                  sInitExpr,
                                  aEnv );

        /* Key Column Map List에 추가 (Inner Column Expression에 대해) */
        QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                  sPlanHashInstant->mKeyColMapList,
                                  sCandRefExprItem->mExpr,
                                  sInitExpr,
                                  aEnv );

        sIdx++;

        sCandRefExprItem = sCandRefExprItem->mNext;
    }

    /* Record Column List 설정 */
    if( sNodeHashInstant->mRecColList != NULL )
    {
        /* Record Column List 할당 */
        STL_TRY( qlvCreateRefExprList( &(sPlanHashInstant->mRecColList),
                                       aPlanDetailParamInfo->mRegionMem,
                                       aEnv )
                 == STL_SUCCESS );

        /* Record Column Map List 할당 */
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mCandidateMem,
                                    STL_SIZEOF( qlncReadColMapList ),
                                    (void**) &(sPlanHashInstant->mRecColMapList),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_LIST( sPlanHashInstant->mRecColMapList );

        sCandRefExprItem = sNodeHashInstant->mRecColList->mHead;
        while( sCandRefExprItem != NULL )
        {
            STL_DASSERT( sCandRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
            sCandInnerColumn = (qlncExprInnerColumn*)(sCandRefExprItem->mExpr);

            STL_DASSERT( sCandInnerColumn->mNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT );

            /* 하위 노드에 대한 Expression 처리 */
            QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                &sConvExprParamInfo,
                &(aPlanDetailParamInfo->mParamInfo),
                aPlanDetailParamInfo->mRegionMem,
                aPlanDetailParamInfo->mCandidateMem,
                aPlanDetailParamInfo->mInitExprList,
                aPlanDetailParamInfo->mTableStatList,
                aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
                aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
                0,
                NULL,
                (qlncPlanCommon*)sPlanHashInstant,
                aPlanDetailParamInfo->mPlanNodeMapDynArr,
                &sTmpSubQueryExprList,
                STL_FALSE,
                sCandInnerColumn->mOrgExpr );

            STL_TRY( qlncConvertCandExprToInitExprFuncList[ sCandInnerColumn->mOrgExpr->mType ]( &sConvExprParamInfo,
                                                                                                 aEnv )
                     == STL_SUCCESS );

            /* Init Inner Column 생성 */
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitInnerColumn ),
                                        (void**) &sInitInnerColumn,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /* Relation Node 설정 */
            sInitInnerColumn->mRelationNode = sPlanHashInstant->mCommon.mInitNode;

            /* Original Expression 설정 */
            sInitInnerColumn->mOrgExpr = sConvExprParamInfo.mInitExpr;

            /* Idx 설정 */
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( stlInt32 ),
                                        (void**) &(sInitInnerColumn->mIdx),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            *(sInitInnerColumn->mIdx) = sIdx;

            /* Init Expression 생성 */
            QLNC_MAKE_INIT_EXPRESSION( aPlanDetailParamInfo->mRegionMem,
                                       QLV_EXPR_TYPE_INNER_COLUMN,
                                       sInitInnerColumn->mOrgExpr->mRelatedFuncId,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       sConvExprParamInfo.mInitExpr->mPosition,
                                       QLNC_EXPR_PHRASE_WHERE,
                                       sInitInnerColumn->mOrgExpr->mIncludeExprCnt,
                                       sInitInnerColumn->mOrgExpr->mColumnName,
                                       sInitInnerColumn,
                                       sInitExpr,
                                       aEnv );

            /* Record Column List에 추가 */
            STL_TRY( qlvAddExprToRefExprList( sPlanHashInstant->mRecColList,
                                              sInitExpr,
                                              STL_FALSE,
                                              aPlanDetailParamInfo->mRegionMem,
                                              aEnv )
                     == STL_SUCCESS );

            /* Record Column Map List에 추가 (Original Expression에 대해) */
            QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                      sPlanHashInstant->mRecColMapList,
                                      sCandInnerColumn->mOrgExpr,
                                      sInitExpr,
                                      aEnv );

            /* Record Column Map List에 추가 (Inner Column Expression에 대해) */
            QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                      sPlanHashInstant->mRecColMapList,
                                      sCandRefExprItem->mExpr,
                                      sInitExpr,
                                      aEnv );

            sIdx++;

            sCandRefExprItem = sCandRefExprItem->mNext;
        }
    }
    else
    {
        STL_TRY( qlvCreateRefExprList( &(sPlanHashInstant->mRecColList),
                                       aPlanDetailParamInfo->mRegionMem,
                                       aEnv )
                 == STL_SUCCESS );
    }

    /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
    STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             1,
                                             aEnv )
             == STL_SUCCESS );

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sTmpSubQueryExprList.mCount > 0 )
    {
        /* 하위 노드로 SubQuery Plan을 생성 */
        STL_TRY( qlncMakeCandPlanSubQueryList( aPlanDetailParamInfo->mRegionMem,
                                               aPlanDetailParamInfo->mCandidateMem,
                                               sPlanHashInstant->mChildPlanNode,
                                               &sTmpSubQueryExprList,
                                               &(sPlanHashInstant->mChildPlanNode),
                                               aEnv )
                 == STL_SUCCESS );
    }

    /* Read Column List 할당 */
    STL_TRY( qlvCreateRefExprList( &(sPlanHashInstant->mReadColList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Hash Key Filter Expression 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( sNodeHashInstant->mKeyFilter != NULL )
    {
        /* Plan Node Map Dynamic Array에 현재 Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 aPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanHashInstant,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            (qlncExprCommon*)(sNodeHashInstant->mKeyFilter) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanHashInstant->mHashKeyFilterExpr = sConvExprParamInfo.mInitExpr;
        sSubQueryFilter = sConvExprParamInfo.mSubQueryAndFilter;

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( ( sNodeHashInstant->mInstantType == QLNC_INSTANT_TYPE_GROUP ) ||
                     ( sNodeHashInstant->mInstantType == QLNC_INSTANT_TYPE_DISTINCT ) );
        sPlanHashInstant->mHashKeyFilterExpr = NULL;
        sSubQueryFilter = NULL;
    }

    STL_DASSERT( sSubQueryFilter == NULL );

    /* Record Filter와 Filter를 합침 */
    QLNC_INIT_AND_FILTER( &sTmpAndFilter );
    if( sNodeHashInstant->mRecFilter != NULL )
    {
        STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aPlanDetailParamInfo->mParamInfo),
                                                    &sTmpAndFilter,
                                                    sNodeHashInstant->mRecFilter->mOrCount,
                                                    sNodeHashInstant->mRecFilter->mOrFilters,
                                                    aEnv )
                 == STL_SUCCESS );
    }

    if( sNodeHashInstant->mFilter != NULL )
    {
        STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aPlanDetailParamInfo->mParamInfo),
                                                    &sTmpAndFilter,
                                                    sNodeHashInstant->mFilter->mOrCount,
                                                    sNodeHashInstant->mFilter->mOrFilters,
                                                    aEnv )
                 == STL_SUCCESS );
    }

    /* Filter Expression 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( sTmpAndFilter.mOrCount > 0 )
    {
        /* Plan Node Map Dynamic Array에 현재 Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 aPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanHashInstant,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            (qlncExprCommon*)(&sTmpAndFilter) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanHashInstant->mNonHashKeyFilterExpr = sConvExprParamInfo.mInitExpr;
        sSubQueryFilter = sConvExprParamInfo.mSubQueryAndFilter;

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sPlanHashInstant->mNonHashKeyFilterExpr = NULL;
        sSubQueryFilter = NULL;
    }

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sSubQueryFilter != NULL )
    {
        /* 상위 노드로 SubQuery Filter Plan과 SubQuery Plan을 생성 */
        STL_TRY( qlncMakeCandPlanSubQueryCondition( aPlanDetailParamInfo->mRegionMem,
                                                    aPlanDetailParamInfo->mCandidateMem,
                                                    (qlncPlanCommon*)sPlanHashInstant,
                                                    sSubQueryFilter,
                                                    &sOutPlanNode,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOutPlanNode = (qlncPlanCommon*)sPlanHashInstant;
    }

    /* Init Node 정보 설정 : 주석 부분은 MakeCandBasic에서 이미 설정한 부분임 */
    sInitInstant = (qlvInitInstantNode *) sPlanHashInstant->mCommon.mInitNode;
    sInitInstant->mRelationName = NULL;   /* default */
    /* sInitInstant->mTableNode = NULL; */

    sInitInstant->mKeyColList = sPlanHashInstant->mKeyColList;
    sInitInstant->mRecColList = sPlanHashInstant->mRecColList;
    sInitInstant->mReadColList = sPlanHashInstant->mReadColList;
    sInitInstant->mKeyColCnt = sPlanHashInstant->mKeyColList->mCount;
    sInitInstant->mRecColCnt = sPlanHashInstant->mRecColList->mCount;
    sInitInstant->mReadColCnt = sPlanHashInstant->mReadColList->mCount;

    /* sInitInstant->mOuterColumnList = sPlanHashInstant->mOuterColumnList; */

    sInitInstant->mKeyFlags = NULL;
    
    /* Aggregation List Backup 정보 원복 */
    if( (aPlanNode->mPlanType == QLNC_PLAN_TYPE_HASH_GROUP_INSTANT) &&
        (aPlanDetailParamInfo->mInitExprList != NULL) )
    {
        aPlanDetailParamInfo->mInitExprList->mAggrExprList = sBackupAggrExprList;
        aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList = sBackupNestedAggrExprList;
    }

    
    /**
     * Output 설정
     */

    *aOutPlanNode = sOutPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


#if 0
/**
 * @brief SubQuery List Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailSubQueryList( qlncPlanDetailParamInfo   * aPlanDetailParamInfo,
                                              qlncPlanCommon            * aPlanNode,
                                              qlncPlanCommon           ** aOutPlanNode,
                                              qllEnv                    * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_LIST,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Output 설정
     */

    *aOutPlanNode = aPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Function Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailSubQueryFunc( qlncPlanDetailParamInfo * aPlanDetailParamInfo,
                                              qlncPlanCommon          * aPlanNode,
                                              qlncPlanCommon         ** aOutPlanNode,
                                              qllEnv                  * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_FUNCTION,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Output 설정
     */

    *aOutPlanNode = aPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Filter Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailSubQueryFilter( qlncPlanDetailParamInfo * aPlanDetailParamInfo,
                                                qlncPlanCommon          * aPlanNode,
                                                qlncPlanCommon         ** aOutPlanNode,
                                                qllEnv                  * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_FILTER,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Output 설정
     */

    *aOutPlanNode = aPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
#endif


/**
 * @brief Group Candidate Plan의 Detail 정보를 처리한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Plan Node
 * @param[out]  aOutPlanNode            Candidate Plan Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeCandPlanDetailGroup( qlncPlanDetailParamInfo  * aPlanDetailParamInfo,
                                       qlncPlanCommon           * aPlanNode,
                                       qlncPlanCommon          ** aOutPlanNode,
                                       qllEnv                   * aEnv )
{
    stlInt32                  sIdx;
    qlncPlanCommon          * sOutPlanNode          = NULL;
    qlncHashInstantNode     * sNodeHashInstant      = NULL;
    qlncPlanGroup           * sPlanGroup            = NULL;
    qlncConvExprParamInfo     sConvExprParamInfo;
    qlncRefExprItem         * sCandRefExprItem      = NULL;
    qlncExprInnerColumn     * sCandInnerColumn      = NULL;
    qlvInitInnerColumn      * sInitInnerColumn      = NULL;
    qlvInitExpression       * sInitExpr             = NULL;
    qlvRefExprList            sTmpSubQueryExprList;
    qlncSubQueryAndFilter   * sSubQueryFilter       = NULL;
    qlvInitInstantNode      * sInitInstant          = NULL;

    qlvRefExprList          * sBackupAggrExprList       = NULL;
    qlvRefExprList          * sBackupNestedAggrExprList = NULL;
    qlncAndFilter             sTmpAndFilter;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPlanNode->mPlanType == QLNC_PLAN_TYPE_GROUP,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Aggregation List 정보 Backup */
    if( (aPlanNode->mPlanType == QLNC_PLAN_TYPE_GROUP) &&
        (aPlanDetailParamInfo->mInitExprList != NULL) )
    {
        sBackupAggrExprList = aPlanDetailParamInfo->mInitExprList->mAggrExprList;
        sBackupNestedAggrExprList = aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList;

        /* Aggregation Node 찾기 */
        STL_TRY( qlncMCPSetAggrExprList( aPlanNode,
                                         &(aPlanDetailParamInfo->mInitExprList->mAggrExprList),
                                         &(aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList),
                                         aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sBackupAggrExprList = NULL;
        sBackupNestedAggrExprList = NULL;
    }

    /* Group Plan 설정 */
    sPlanGroup = (qlncPlanGroup*)aPlanNode;

    /* Hash Instant Node 설정 */
    sNodeHashInstant = (qlncHashInstantNode*)(aPlanNode->mCandNode);

    /* Child Plan 설정 */
    STL_TRY( qlncMakeCandPlanDetailFuncList[ sPlanGroup->mChildPlanNode->mPlanType ](
                 aPlanDetailParamInfo,
                 sPlanGroup->mChildPlanNode,
                 &(sPlanGroup->mChildPlanNode),
                 aEnv )
             == STL_SUCCESS );

    /* 기타 변수 초기화 */
    sPlanGroup->mKeyColList = NULL;
    sPlanGroup->mRecColList = NULL;
    sPlanGroup->mReadColList = NULL;
    sPlanGroup->mKeyColMapList = NULL;
    sPlanGroup->mRecColMapList = NULL;
    sPlanGroup->mReadColMapList = NULL;

    QLNC_INIT_LIST( &sTmpSubQueryExprList );

    /* Plan Node Map Dynamic Array에 Child Plan Node를 추가 */
    STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             sPlanGroup->mChildPlanNode,
                                             aEnv )
             == STL_SUCCESS );

    /* Key Column List 설정 */
    STL_DASSERT( sNodeHashInstant->mKeyColList != NULL );

    /* Key Column List 할당 */
    STL_TRY( qlvCreateRefExprList( &(sPlanGroup->mKeyColList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Key Column Map List 할당 */
    STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mCandidateMem,
                                STL_SIZEOF( qlncReadColMapList ),
                                (void**) &(sPlanGroup->mKeyColMapList),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    QLNC_INIT_LIST( sPlanGroup->mKeyColMapList );

    sIdx = 0;
    sCandRefExprItem = sNodeHashInstant->mKeyColList->mHead;
    while( sCandRefExprItem != NULL )
    {
        STL_DASSERT( sCandRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
        sCandInnerColumn = (qlncExprInnerColumn*)(sCandRefExprItem->mExpr);

        STL_DASSERT( sCandInnerColumn->mNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT );

        /* 하위 노드에 대한 Expression 처리 */
        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanGroup,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_FALSE,
            sCandInnerColumn->mOrgExpr );

        STL_TRY( qlncConvertCandExprToInitExprFuncList[ sCandInnerColumn->mOrgExpr->mType ]( &sConvExprParamInfo,
                                                                                             aEnv )
                 == STL_SUCCESS );

        /* Init Inner Column 생성 */
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitInnerColumn ),
                                    (void**) &sInitInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Relation Node 설정 */
        sInitInnerColumn->mRelationNode = sPlanGroup->mCommon.mInitNode;

        /* Original Expression 설정 */
        sInitInnerColumn->mOrgExpr = sConvExprParamInfo.mInitExpr;

        /* Idx 설정 */
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                    STL_SIZEOF( stlInt32 ),
                                    (void**) &(sInitInnerColumn->mIdx),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        *(sInitInnerColumn->mIdx) = sIdx;

        /* Init Expression 생성 */
        QLNC_MAKE_INIT_EXPRESSION( aPlanDetailParamInfo->mRegionMem,
                                   QLV_EXPR_TYPE_INNER_COLUMN,
                                   sInitInnerColumn->mOrgExpr->mRelatedFuncId,
                                   DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                   sConvExprParamInfo.mInitExpr->mPosition,
                                   QLNC_EXPR_PHRASE_WHERE,
                                   sInitInnerColumn->mOrgExpr->mIncludeExprCnt,
                                   sInitInnerColumn->mOrgExpr->mColumnName,
                                   sInitInnerColumn,
                                   sInitExpr,
                                   aEnv );

        /* Key Column List에 추가 */
        STL_TRY( qlvAddExprToRefExprList( sPlanGroup->mKeyColList,
                                          sInitExpr,
                                          STL_FALSE,
                                          aPlanDetailParamInfo->mRegionMem,
                                          aEnv )
                 == STL_SUCCESS );

        /* Key Column Map List에 추가 (Original Expression에 대해) */
        QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                  sPlanGroup->mKeyColMapList,
                                  sCandInnerColumn->mOrgExpr,
                                  sInitExpr,
                                  aEnv );

        /* Key Column Map List에 추가 (Inner Column Expression에 대해) */
        QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                  sPlanGroup->mKeyColMapList,
                                  sCandRefExprItem->mExpr,
                                  sInitExpr,
                                  aEnv );

        sIdx++;

        sCandRefExprItem = sCandRefExprItem->mNext;
    }

    /* Record Column List 설정 */
    if( sNodeHashInstant->mRecColList != NULL )
    {
        /* Record Column List 할당 */
        STL_TRY( qlvCreateRefExprList( &(sPlanGroup->mRecColList),
                                       aPlanDetailParamInfo->mRegionMem,
                                       aEnv )
                 == STL_SUCCESS );

        /* Record Column Map List 할당 */
        STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mCandidateMem,
                                    STL_SIZEOF( qlncReadColMapList ),
                                    (void**) &(sPlanGroup->mRecColMapList),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_LIST( sPlanGroup->mRecColMapList );

        sCandRefExprItem = sNodeHashInstant->mRecColList->mHead;
        while( sCandRefExprItem != NULL )
        {
            STL_DASSERT( sCandRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
            sCandInnerColumn = (qlncExprInnerColumn*)(sCandRefExprItem->mExpr);

            STL_DASSERT( sCandInnerColumn->mNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT );

            /* 하위 노드에 대한 Expression 처리 */
            QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
                &sConvExprParamInfo,
                &(aPlanDetailParamInfo->mParamInfo),
                aPlanDetailParamInfo->mRegionMem,
                aPlanDetailParamInfo->mCandidateMem,
                aPlanDetailParamInfo->mInitExprList,
                aPlanDetailParamInfo->mTableStatList,
                aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
                aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
                0,
                NULL,
                (qlncPlanCommon*)sPlanGroup,
                aPlanDetailParamInfo->mPlanNodeMapDynArr,
                &sTmpSubQueryExprList,
                STL_FALSE,
                sCandInnerColumn->mOrgExpr );

            STL_TRY( qlncConvertCandExprToInitExprFuncList[ sCandInnerColumn->mOrgExpr->mType ]( &sConvExprParamInfo,
                                                                                                 aEnv )
                     == STL_SUCCESS );

            /* Init Inner Column 생성 */
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitInnerColumn ),
                                        (void**) &sInitInnerColumn,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /* Relation Node 설정 */
            sInitInnerColumn->mRelationNode = sPlanGroup->mCommon.mInitNode;

            /* Original Expression 설정 */
            sInitInnerColumn->mOrgExpr = sConvExprParamInfo.mInitExpr;

            /* Idx 설정 */
            STL_TRY( knlAllocRegionMem( aPlanDetailParamInfo->mRegionMem,
                                        STL_SIZEOF( stlInt32 ),
                                        (void**) &(sInitInnerColumn->mIdx),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            *(sInitInnerColumn->mIdx) = sIdx;

            /* Init Expression 생성 */
            QLNC_MAKE_INIT_EXPRESSION( aPlanDetailParamInfo->mRegionMem,
                                       QLV_EXPR_TYPE_INNER_COLUMN,
                                       sInitInnerColumn->mOrgExpr->mRelatedFuncId,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       sConvExprParamInfo.mInitExpr->mPosition,
                                       QLNC_EXPR_PHRASE_WHERE,
                                       sInitInnerColumn->mOrgExpr->mIncludeExprCnt,
                                       sInitInnerColumn->mOrgExpr->mColumnName,
                                       sInitInnerColumn,
                                       sInitExpr,
                                       aEnv );

            /* Record Column List에 추가 */
            STL_TRY( qlvAddExprToRefExprList( sPlanGroup->mRecColList,
                                              sInitExpr,
                                              STL_FALSE,
                                              aPlanDetailParamInfo->mRegionMem,
                                              aEnv )
                     == STL_SUCCESS );

            /* Record Column Map List에 추가 (Original Expression에 대해) */
            QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                      sPlanGroup->mRecColMapList,
                                      sCandInnerColumn->mOrgExpr,
                                      sInitExpr,
                                      aEnv );

            /* Record Column Map List에 추가 (Inner Column Expression에 대해) */
            QLNC_ADD_READ_COLUMN_MAP( aPlanDetailParamInfo->mCandidateMem,
                                      sPlanGroup->mRecColMapList,
                                      sCandRefExprItem->mExpr,
                                      sInitExpr,
                                      aEnv );

            sIdx++;

            sCandRefExprItem = sCandRefExprItem->mNext;
        }
    }
    else
    {
        STL_TRY( qlvCreateRefExprList( &(sPlanGroup->mRecColList),
                                       aPlanDetailParamInfo->mRegionMem,
                                       aEnv )
                 == STL_SUCCESS );
    }

    /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
    STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                             1,
                                             aEnv )
             == STL_SUCCESS );

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sTmpSubQueryExprList.mCount > 0 )
    {
        /* 하위 노드로 SubQuery Plan을 생성 */
        STL_TRY( qlncMakeCandPlanSubQueryList( aPlanDetailParamInfo->mRegionMem,
                                               aPlanDetailParamInfo->mCandidateMem,
                                               sPlanGroup->mChildPlanNode,
                                               &sTmpSubQueryExprList,
                                               &(sPlanGroup->mChildPlanNode),
                                               aEnv )
                 == STL_SUCCESS );
    }

    /* Read Column List 할당 */
    STL_TRY( qlvCreateRefExprList( &(sPlanGroup->mReadColList),
                                   aPlanDetailParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Hash Key Filter Expression 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( sNodeHashInstant->mKeyFilter != NULL )
    {
        /* Plan Node Map Dynamic Array에 현재 Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 aPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanGroup,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            (qlncExprCommon*)(sNodeHashInstant->mKeyFilter) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanGroup->mGroupKeyFilterExpr = sConvExprParamInfo.mInitExpr;
        sSubQueryFilter = sConvExprParamInfo.mSubQueryAndFilter;

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( (sNodeHashInstant->mInstantType == QLNC_INSTANT_TYPE_GROUP) ||
                     (sNodeHashInstant->mInstantType == QLNC_INSTANT_TYPE_DISTINCT) );
        sPlanGroup->mGroupKeyFilterExpr = NULL;
        sSubQueryFilter = NULL;
    }

    STL_DASSERT( sSubQueryFilter == NULL );

    /* Record Filter와 Filter를 합침 */
    QLNC_INIT_AND_FILTER( &sTmpAndFilter );
    if( sNodeHashInstant->mRecFilter != NULL )
    {
        STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aPlanDetailParamInfo->mParamInfo),
                                                    &sTmpAndFilter,
                                                    sNodeHashInstant->mRecFilter->mOrCount,
                                                    sNodeHashInstant->mRecFilter->mOrFilters,
                                                    aEnv )
                 == STL_SUCCESS );
    }

    if( sNodeHashInstant->mFilter != NULL )
    {
        STL_TRY( qlncAppendOrFilterToAndFilterTail( &(aPlanDetailParamInfo->mParamInfo),
                                                    &sTmpAndFilter,
                                                    sNodeHashInstant->mFilter->mOrCount,
                                                    sNodeHashInstant->mFilter->mOrFilters,
                                                    aEnv )
                 == STL_SUCCESS );
    }

    /* Filter Expression 설정 */
    QLNC_INIT_LIST( &sTmpSubQueryExprList );
    if( sTmpAndFilter.mOrCount > 0 )
    {
        /* Plan Node Map Dynamic Array에 현재 Plan Node를 추가 */
        STL_TRY( qlncAddTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 aPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );

        QLNC_SET_CONVERT_EXPRESSION_PARAM_INFO(
            &sConvExprParamInfo,
            &(aPlanDetailParamInfo->mParamInfo),
            aPlanDetailParamInfo->mRegionMem,
            aPlanDetailParamInfo->mCandidateMem,
            aPlanDetailParamInfo->mInitExprList,
            aPlanDetailParamInfo->mTableStatList,
            aPlanDetailParamInfo->mJoinOuterInfoList.mCount,
            aPlanDetailParamInfo->mJoinOuterInfoList.mHead,
            0,
            NULL,
            (qlncPlanCommon*)sPlanGroup,
            aPlanDetailParamInfo->mPlanNodeMapDynArr,
            &sTmpSubQueryExprList,
            STL_TRUE,
            (qlncExprCommon*)(&sTmpAndFilter) );

        STL_TRY( qlncConvertCandExprToInitExprRootAndFilter( &sConvExprParamInfo,
                                                             aEnv )
                 == STL_SUCCESS );
        sPlanGroup->mNonGroupKeyFilterExpr = sConvExprParamInfo.mInitExpr;
        sSubQueryFilter = sConvExprParamInfo.mSubQueryAndFilter;

        /* Plan Node Map Dynamic Array에서 Plan Node를 삭제 */
        STL_TRY( qlncDelTailPlanNodeMapDynArray( aPlanDetailParamInfo->mPlanNodeMapDynArr,
                                                 1,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sPlanGroup->mNonGroupKeyFilterExpr = NULL;
        sSubQueryFilter = NULL;
    }

    /* SubQuery Expression List가 존재하는지 판별 */
    if( sSubQueryFilter != NULL )
    {
        /* 상위 노드로 SubQuery Filter Plan과 SubQuery Plan을 생성 */
        STL_TRY( qlncMakeCandPlanSubQueryCondition( aPlanDetailParamInfo->mRegionMem,
                                                    aPlanDetailParamInfo->mCandidateMem,
                                                    (qlncPlanCommon*)sPlanGroup,
                                                    sSubQueryFilter,
                                                    &sOutPlanNode,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOutPlanNode = (qlncPlanCommon*)sPlanGroup;
    }

    /* Init Node 정보 설정 : 주석 부분은 MakeCandBasic에서 이미 설정한 부분임 */
    sInitInstant = (qlvInitInstantNode *) sPlanGroup->mCommon.mInitNode;
    sInitInstant->mRelationName = NULL;   /* default */
    /* sInitInstant->mTableNode = NULL; */

    sInitInstant->mKeyColList = sPlanGroup->mKeyColList;
    sInitInstant->mRecColList = sPlanGroup->mRecColList;
    sInitInstant->mReadColList = sPlanGroup->mReadColList;
    sInitInstant->mKeyColCnt = sPlanGroup->mKeyColList->mCount;
    sInitInstant->mRecColCnt = sPlanGroup->mRecColList->mCount;
    sInitInstant->mReadColCnt = sPlanGroup->mReadColList->mCount;

    /* sInitInstant->mOuterColumnList = sPlanGroup->mOuterColumnList; */

    sInitInstant->mKeyFlags = NULL;
    
    /* Aggregation List Backup 정보 원복 */
    if( (aPlanNode->mPlanType == QLNC_PLAN_TYPE_GROUP) &&
        (aPlanDetailParamInfo->mInitExprList != NULL) )
    {
        aPlanDetailParamInfo->mInitExprList->mAggrExprList = sBackupAggrExprList;
        aPlanDetailParamInfo->mInitExprList->mNestedAggrExprList = sBackupNestedAggrExprList;
    }

    
    /**
     * Output 설정
     */

    *aOutPlanNode = sOutPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Candidate Plan을 생성한다.
 * @param[in]   aRegionMem          Region Memory
 * @param[in]   aCandidateMem       Candidate Region Memory
 * @param[in]   aChildPlanNode      Child Candidate Plan Node
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[out]  aOutPlanNode        Output Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanSubQueryList( knlRegionMem    * aRegionMem,
                                        knlRegionMem    * aCandidateMem,
                                        qlncPlanCommon  * aChildPlanNode,
                                        qlvRefExprList  * aSubQueryExprList,
                                        qlncPlanCommon ** aOutPlanNode,
                                        qllEnv          * aEnv )
{
    qlncPlanSubQueryList  * sPlanSubQueryList = NULL;
    qlvInitSubTableNode   * sValidateSubTable = NULL;
    qlvRefExprItem        * sExprItem         = NULL;
    stlInt32                sIdx;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandidateMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryExprList->mCount > 0, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* SubQuery Candidate Plan 생성 */
    STL_TRY( knlAllocRegionMem( aCandidateMem,
                                STL_SIZEOF( qlncPlanSubQueryList ),
                                (void**) &sPlanSubQueryList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sPlanSubQueryList, 0x00, STL_SIZEOF( qlncPlanSubQueryList ) );

    /* Sub Table Validation Node 생성 및 설정 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitSubTableNode ),
                                (void**) &sValidateSubTable,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sValidateSubTable, 0x00, STL_SIZEOF( qlvInitSubTableNode ) );
    sValidateSubTable->mType = QLV_NODE_TYPE_SUB_TABLE;

    STL_TRY( qlvCreateRefExprList( &(sValidateSubTable->mOuterColumnList),
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    sValidateSubTable->mIsView = STL_FALSE;

    sValidateSubTable->mHasHandle = STL_FALSE;
    ellInitDictHandle( & sValidateSubTable->mViewHandle );
    
    sValidateSubTable->mQueryNode = NULL;
    sValidateSubTable->mRelationName = NULL;
    sValidateSubTable->mRefColumnList = NULL;

    sValidateSubTable->mColumnCount = 0;
    sValidateSubTable->mColumns = NULL;
    
    sValidateSubTable->mFilterExpr = NULL;

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanSubQueryList->mCommon),
                          QLNC_PLAN_TYPE_SUB_QUERY_LIST,
                          NULL,
                          (qlvInitNode*)sValidateSubTable,
                          (aChildPlanNode == NULL
                           ? STL_FALSE : aChildPlanNode->mNeedRowBlock) );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Child Plan Node 설정 */
    sPlanSubQueryList->mChildPlanNode = aChildPlanNode;

    /* SubQuery Expression List 설정 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvRefExprList ),
                                (void**) &(sPlanSubQueryList->mSubQueryExprList),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_COPY_LIST_INFO( aSubQueryExprList, sPlanSubQueryList->mSubQueryExprList );

    /* Sub Query Plan Node 설정 */
    sPlanSubQueryList->mSubQueryCnt = sPlanSubQueryList->mSubQueryExprList->mCount;

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlncPlanCommon * ) * sPlanSubQueryList->mSubQueryCnt,
                                (void**) &(sPlanSubQueryList->mSubQueryPlanNodes),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sExprItem = sPlanSubQueryList->mSubQueryExprList->mHead;

    sIdx = 0;
    while( sExprItem != NULL )
    {
        if( sExprItem->mExprPtr->mType == QLV_EXPR_TYPE_SUB_QUERY )
        {
            sPlanSubQueryList->mSubQueryPlanNodes[ sIdx ] =
                (qlncPlanCommon *) sExprItem->mExprPtr->mExpr.mSubQuery->mCandPlan;
        }
        else
        {
            STL_DASSERT( sExprItem->mExprPtr->mType == QLV_EXPR_TYPE_REFERENCE );

            STL_TRY( qlncMakeCandPlanSubQueryFunc( aRegionMem,
                                                   aCandidateMem,
                                                   sExprItem->mExprPtr,
                                                   &(sPlanSubQueryList->mSubQueryPlanNodes[ sIdx ]),
                                                   aEnv )
                     == STL_SUCCESS );
        }

        sIdx++;
        sExprItem = sExprItem->mNext;
    }
    
    
    /* Outer Column List 설정 */
    sPlanSubQueryList->mOuterColumnList = sValidateSubTable->mOuterColumnList;


    /****************************************************************
     * Output 설정
     ****************************************************************/

    *aOutPlanNode = (qlncPlanCommon*)sPlanSubQueryList;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Function Candidate Plan을 생성한다.
 * @param[in]   aRegionMem          Region Memory
 * @param[in]   aCandidateMem       Candidate Region Memory
 * @param[in]   aFuncExpr           Sub Query Function Expression
 * @param[out]  aOutPlanNode        Output Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanSubQueryFunc( knlRegionMem          * aRegionMem,
                                        knlRegionMem          * aCandidateMem,
                                        qlvInitExpression     * aFuncExpr,
                                        qlncPlanCommon       ** aOutPlanNode,
                                        qllEnv                * aEnv )
{
    qlncPlanSubQueryFunc  * sPlanSubQueryFunc = NULL;
    qlvInitRowExpr        * sInitRowExpr      = NULL;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandidateMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFuncExpr != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* SubQuery Candidate Plan 생성 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlncPlanSubQueryFunc ),
                                (void**) &sPlanSubQueryFunc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sPlanSubQueryFunc, 0x00, STL_SIZEOF( qlncPlanSubQueryFunc ) );

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanSubQueryFunc->mCommon),
                          QLNC_PLAN_TYPE_SUB_QUERY_FUNCTION,
                          NULL,
                          NULL,
                          STL_FALSE );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Function Expression 설정 */
    STL_DASSERT( aFuncExpr->mType == QLV_EXPR_TYPE_REFERENCE );
    STL_DASSERT( aFuncExpr->mExpr.mReference->mOrgExpr->mType == QLV_EXPR_TYPE_LIST_FUNCTION );
    
    sPlanSubQueryFunc->mFuncExpr = aFuncExpr->mExpr.mReference->mOrgExpr;
    sPlanSubQueryFunc->mResultExpr = aFuncExpr;
    
    /* Function Operand Plan Node 설정 */
    sInitRowExpr = sPlanSubQueryFunc->mFuncExpr->mExpr.mListFunc->mArgs[0]->
        mExpr.mListCol->mArgs[0]->mExpr.mRowExpr;

    STL_DASSERT( sInitRowExpr->mSubQueryExpr != NULL );

    sPlanSubQueryFunc->mRightOperandPlanNode =
        sInitRowExpr->mSubQueryExpr->mExpr.mSubQuery->mCandPlan;
    

    /****************************************************************
     * Output 설정
     ****************************************************************/

    *aOutPlanNode = (qlncPlanCommon*)sPlanSubQueryFunc;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Filter Candidate Plan을 생성한다.
 * @param[in]   aRegionMem          Region Memory
 * @param[in]   aCandidateMem       Candidate Region Memory
 * @param[in]   aChildPlanNode      Child Candidate Plan Node
 * @param[in]   aSubQueryFilter     SubQuery Filter
 * @param[out]  aOutPlanNode        Output Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanSubQueryFilter( knlRegionMem              * aRegionMem,
                                          knlRegionMem              * aCandidateMem,
                                          qlncPlanCommon            * aChildPlanNode,
                                          qlncSubQueryAndFilter     * aSubQueryFilter,
                                          qlncPlanCommon           ** aOutPlanNode,
                                          qllEnv                    * aEnv )
{
    stlInt32                  sLoop;
    qlncPlanSubQueryFilter  * sPlanSubQueryFilter       = NULL;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandidateMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryFilter != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryFilter->mAndFilterCnt > 0, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Basic Plan Info 설정
     ****************************************************************/

    /* SubQuery Candidate Plan 생성 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlncPlanSubQueryFilter ),
                                (void**) &sPlanSubQueryFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sPlanSubQueryFilter, 0x00, STL_SIZEOF( qlncPlanSubQueryFilter ) );

    /* Plan Common 정보 설정 */
    QLNC_SET_PLAN_COMMON( &(sPlanSubQueryFilter->mCommon),
                          QLNC_PLAN_TYPE_SUB_QUERY_FILTER,
                          NULL,
                          aChildPlanNode->mInitNode,
                          aChildPlanNode->mNeedRowBlock );


    /****************************************************************
     * Detail Plan Info 설정
     ****************************************************************/

    /* Child Plan Node 설정 */
    STL_TRY( knlAllocRegionMem( aCandidateMem,
                                STL_SIZEOF( qlncPlanCommon * ) * aSubQueryFilter->mAndFilterCnt,
                                (void**) &sPlanSubQueryFilter->mChildPlanNodes,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sPlanSubQueryFilter->mChildPlanNodes,
               0x00,
               STL_SIZEOF( qlncPlanCommon * ) * aSubQueryFilter->mAndFilterCnt );

    STL_DASSERT( aSubQueryFilter->mSubQueryExprList[ 0 ] != NULL );
    STL_DASSERT( aSubQueryFilter->mSubQueryExprList[ 0 ]->mCount > 0 );

    STL_TRY( qlncMakeCandPlanSubQueryList( aRegionMem,
                                           aCandidateMem,
                                           aChildPlanNode,
                                           aSubQueryFilter->mSubQueryExprList[ 0 ],
                                           &(sPlanSubQueryFilter->mChildPlanNodes[ 0 ]),
                                           aEnv )
             == STL_SUCCESS );

    for( sLoop = 1 ; sLoop < aSubQueryFilter->mAndFilterCnt ; sLoop++ )
    {
        STL_DASSERT( aSubQueryFilter->mSubQueryExprList[ sLoop ] != NULL );
        STL_DASSERT( aSubQueryFilter->mSubQueryExprList[ sLoop ]->mCount > 0 );

        STL_TRY( qlncMakeCandPlanSubQueryList( aRegionMem,
                                               aCandidateMem,
                                               NULL,
                                               aSubQueryFilter->mSubQueryExprList[ sLoop ],
                                               &(sPlanSubQueryFilter->mChildPlanNodes[ sLoop ]),
                                               aEnv )
                 == STL_SUCCESS );
    }
    
    /* Filter Expression 설정 */
    sPlanSubQueryFilter->mAndFilter = aSubQueryFilter;


    /****************************************************************
     * Output 설정
     ****************************************************************/

    *aOutPlanNode = (qlncPlanCommon*)sPlanSubQueryFilter;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery가 존재하는 Condition을 분석하여 Candidate Plan을 생성한다.
 * @param[in]   aRegionMem          Region Memory
 * @param[in]   aCandidateMem       Candidate Region Memory
 * @param[in]   aChildPlanNode      Child Candidate Plan Node
 * @param[in]   aSubQueryFilter     SubQuery Filter
 * @param[out]  aOutPlanNode        Output Candidate Plan Node
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMakeCandPlanSubQueryCondition( knlRegionMem           * aRegionMem,
                                             knlRegionMem           * aCandidateMem,
                                             qlncPlanCommon         * aChildPlanNode,
                                             qlncSubQueryAndFilter  * aSubQueryFilter,
                                             qlncPlanCommon        ** aOutPlanNode,
                                             qllEnv                 * aEnv )
{
    stlInt32                  i;
    qlvRefExprList            sStmtSubQueryExprList;

    qlvRefExprItem          * sRefExprItem  = NULL;
    qlncPlanCommon          * sOutPlanNode  = NULL;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandidateMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aChildPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryFilter != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryFilter->mAndFilterCnt > 0, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * SubQuery Filter에 대한 분석
     ****************************************************************/

    /* SubQuery And Filter를 Statement에서 처리할 것과 그렇지 않은 것으로 분리한다. */
    QLNC_INIT_LIST( &sStmtSubQueryExprList );
    for( i = 0; i < aSubQueryFilter->mAndFilterCnt; i++ )
    {
        if( aSubQueryFilter->mInitSubQueryFilter[i] == NULL )
        {
            /* Statement단위로 처리 가능한 SubQuery인 경우 */
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( qlvRefExprItem ),
                                        (void**) &sRefExprItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY( qlvAddExprListToRefExprList( &sStmtSubQueryExprList,
                                                  aSubQueryFilter->mSubQueryExprList[i],
                                                  STL_FALSE,
                                                  aRegionMem,
                                                  aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* Statement단위로 처리 가능한 SubQuery가 아닌 경우 */
            if( sStmtSubQueryExprList.mCount > 0 )
            {
                /* SubQuery를 앞으로 이동 (Statement단위로 처리 가능한 SubQuery가 빠진 공간이 있음) */
                aSubQueryFilter->mInitSubQueryFilter[i-sStmtSubQueryExprList.mCount] =
                    aSubQueryFilter->mInitSubQueryFilter[i];
                aSubQueryFilter->mSubQueryExprList[i-sStmtSubQueryExprList.mCount] =
                    aSubQueryFilter->mSubQueryExprList[i];
            }
        }
    }

    aSubQueryFilter->mAndFilterCnt -= sStmtSubQueryExprList.mCount;

    /* And Filter SubQuery에 대한 처리 */
    if( aSubQueryFilter->mAndFilterCnt > 0 )
    {
        STL_TRY( qlncMakeCandPlanSubQueryFilter( aRegionMem,
                                                 aCandidateMem,
                                                 aChildPlanNode,
                                                 aSubQueryFilter,
                                                 &sOutPlanNode,
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sOutPlanNode = aChildPlanNode;
    }

    /* Statement단위로 처리 가능한 SubQuery에 대한 처리 */
    if( sStmtSubQueryExprList.mCount > 0 )
    {
        STL_TRY( qlncMakeCandPlanSubQueryList( aRegionMem,
                                               aCandidateMem,
                                               sOutPlanNode,
                                               &sStmtSubQueryExprList,
                                               &sOutPlanNode,
                                               aEnv )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Output 설정
     ****************************************************************/

    *aOutPlanNode = sOutPlanNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Set Target의 Data Type 정보를 구축한다.
 * @param[in]   aPlanDetailParamInfo    Candidate Plan Detail Parameter Info
 * @param[in]   aPlanNode               Candidate Node
 * @param[in]   aTargetCnt              Init Target Count
 * @param[out]  aTarget                 Init Target
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncGetQuerySetTargetDataTypeInfo( qlncPlanDetailParamInfo   * aPlanDetailParamInfo,
                                             qlncPlanCommon            * aPlanNode,
                                             stlInt32                    aTargetCnt,
                                             qlvInitTarget             * aTarget,
                                             qllEnv                    * aEnv )
{
    qlncPlanCommon          * sPlanNode    = NULL;
    qlncPlanSetOP           * sPlanQueryOP = NULL;
    qlvInitTarget           * sLeftTarget  = NULL;
    qlvInitTarget           * sRightTarget = NULL;
    stlInt32                  i;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_MAKE_CAND_PLAN_DETAIL_PARAM_VALIDATE( aPlanDetailParamInfo, aEnv );
    STL_PARAM_VALIDATE( aPlanNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetCnt > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTarget != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Query Spec Node 얻기
     ****************************************************************/
        
    sPlanNode   = aPlanNode;
    sLeftTarget = aTarget;

    STL_DASSERT( sPlanNode->mPlanType != QLNC_PLAN_TYPE_QUERY_SET );
    if( sPlanNode->mPlanType == QLNC_PLAN_TYPE_QUERY_SPEC )
    {
        STL_DASSERT( ((qlncPlanQuerySpec*) sPlanNode)->mCommon.mInitNode != NULL );
        
        sRightTarget =
            ((qlvInitQuerySpecNode *) ((qlncPlanQuerySpec*) sPlanNode)->mCommon.mInitNode )->mTargets;

        STL_THROW( RAMP_GET_DATA_TYPE_INFO );
    }

    if( sPlanNode->mPlanType == QLNC_PLAN_TYPE_SORT_INSTANT )
    {
        sPlanNode = ((qlncPlanSortInstant*) sPlanNode)->mChildPlanNode;
        
        STL_DASSERT( sPlanNode->mPlanType == QLNC_PLAN_TYPE_SET_OP );
    }

    if( sPlanNode->mPlanType == QLNC_PLAN_TYPE_SET_OP )
    {
        sPlanQueryOP = ((qlncPlanSetOP*) sPlanNode);
        
        for( i = 0 ; i < sPlanQueryOP->mChildCount ; i++ )
        {
            STL_TRY( qlncGetQuerySetTargetDataTypeInfo( aPlanDetailParamInfo,
                                                        sPlanQueryOP->mChildPlanNodeArr[ i ],
                                                        aTargetCnt,
                                                        aTarget,
                                                        aEnv )
                     == STL_SUCCESS );
        }
        
        STL_THROW( RAMP_FINISH );
    }


    /****************************************************************
     * Data Type Info 분석
     ****************************************************************/

    STL_RAMP( RAMP_GET_DATA_TYPE_INFO );

    STL_DASSERT( sRightTarget != NULL );
    
    for( i = 0 ; i < aTargetCnt ; i++ )
    {
        /* Left Data Type Info 설정 */
        if( sLeftTarget[i].mDataTypeInfo.mIsIgnore == STL_TRUE )
        {
            if( sRightTarget[i].mDataTypeInfo.mIsIgnore == STL_TRUE )
            {
                /* Do Nothing */
            }
            else
            {
                sLeftTarget[i].mDataTypeInfo = sRightTarget[i].mDataTypeInfo;
            }
        }
        else
        {
            if( sRightTarget[i].mDataTypeInfo.mIsIgnore == STL_TRUE )
            {
                /* Do Nothing */
            }
            else
            {
                STL_TRY( qlvGetResultTypeCombiantion( aPlanDetailParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                      aPlanDetailParamInfo->mParamInfo.mSQLStmt->mBindContext,
                                                      sRightTarget[i].mExpr->mPosition,
                                                      STL_FALSE,
                                                      & sLeftTarget[i].mDataTypeInfo,
                                                      & sRightTarget[i].mDataTypeInfo,
                                                      & sLeftTarget[i].mDataTypeInfo,
                                                      aEnv )
                         == STL_SUCCESS );
            }
        }

        sLeftTarget[i].mDataTypeInfo.mIsBaseColumn = STL_FALSE;
        sLeftTarget[i].mDataTypeInfo.mUpdatable    = STL_FALSE;
    }
    
    
    STL_RAMP( RAMP_FINISH );
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */

