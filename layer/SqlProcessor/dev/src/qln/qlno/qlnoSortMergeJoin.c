/*******************************************************************************
 * qlnoSortMergeJoin.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
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
 * @file qlnoSortMergeJoin.c
 * @brief SQL Processor Layer Code Optimization Node - SORT MERGE JOIN
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlno
 * @{
 */


/****************************************************************************
 * Node Optimization Function
 ***************************************************************************/


/**
 * @brief Merge Join에 대한 Code Optimize를 수행한다.
 * @param[in]   aCodeOptParamInfo   Code Optimize Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlnoCodeOptimizeMerge( qloCodeOptParamInfo    * aCodeOptParamInfo,
                                 qllEnv                 * aEnv )
{
    qlncPlanMergeJoin       * sPlanMergeJoin        = NULL;

    qllOptimizationNode     * sJoinOptNode          = NULL;
    qlnoSortMergeJoin       * sOptSortMergeJoin     = NULL;
    qllEstimateCost         * sOptCost              = NULL;

    qlvRefExprList          * sRefColumnList        = NULL;

    
    /*
     * Paramter Validation
     */

    QLO_CODE_OPT_DEFAULT_PARAM_VALIDATE( aCodeOptParamInfo, aEnv );
    STL_PARAM_VALIDATE( aCodeOptParamInfo->mCandPlan->mPlanType == QLNC_PLAN_TYPE_MERGE_JOIN,
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Merge Join Plan 정보 획득
     ****************************************************************/

    sPlanMergeJoin = (qlncPlanMergeJoin*)(aCodeOptParamInfo->mCandPlan);


    /****************************************************************
     * Optimization Node 생성
     ****************************************************************/

    /**
     * SORT MERGE JOIN Optimization Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qlnoSortMergeJoin ),
                                (void **) &sOptSortMergeJoin,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sOptSortMergeJoin, 0x00, STL_SIZEOF( qlnoSortMergeJoin ) );


    /**
     * Optimization Cost 공간 할당
     */

    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                STL_SIZEOF( qllEstimateCost ),
                                (void **) &sOptCost,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sOptCost, 0x00, STL_SIZEOF( qllEstimateCost ) );


    /**
     * Common Optimization Node 생성
     */

    STL_TRY( qlnoCreateOptNode( aCodeOptParamInfo->mOptNodeList,
                                QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                                sOptSortMergeJoin,
                                aCodeOptParamInfo->mOptQueryNode,  /* Query Node */
                                sOptCost,
                                NULL,
                                &sJoinOptNode,
                                QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                                aEnv )
             == STL_SUCCESS );


    /**
     * Left Child Node에 대한 Optimization Node 생성
     */

    aCodeOptParamInfo->mCandPlan = sPlanMergeJoin->mLeftChildPlanNode;
    QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );
    sOptSortMergeJoin->mLeftChildNode = aCodeOptParamInfo->mOptNode;


    /**
     * Right Child Node에 대한 Optimization Node 생성
     */

    aCodeOptParamInfo->mCandPlan = sPlanMergeJoin->mRightChildPlanNode;
    QLNO_CODE_OPTIMIZE( aCodeOptParamInfo, aEnv );
    sOptSortMergeJoin->mRightChildNode = aCodeOptParamInfo->mOptNode;


    /****************************************************************
     * Read Column List 구성
     ****************************************************************/
 
    /**
     * Read Column List 구성
     */

    sRefColumnList = sPlanMergeJoin->mReadColList;


    /**
     * Statement Expression List에 Join Read Column 추가
     */

    STL_TRY( qlvAddExprListToRefExprList(
                 aCodeOptParamInfo->mQueryExprList->mStmtExprList->mColumnExprList,
                 sRefColumnList,
                 STL_FALSE,
                 QLL_CODE_PLAN( aCodeOptParamInfo->mDBCStmt ),
                 aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Optimize 정보 연결
     ****************************************************************/

    /**
     * Row Block이 필요한지 여부
     */

    sOptSortMergeJoin->mNeedRowBlock = sPlanMergeJoin->mCommon.mNeedRowBlock;


    /**
     * Join Type
     */

    sOptSortMergeJoin->mJoinType = sPlanMergeJoin->mJoinType;


    /**
     * Joined Column List
     */

    sOptSortMergeJoin->mJoinColList = sRefColumnList;


    /**
     * Join Table Node
     */

    sOptSortMergeJoin->mJoinTableNode = (qlvInitJoinTableNode*)(sPlanMergeJoin->mCommon.mInitNode);


    /**
     * Left Sort Key List
     */

    sOptSortMergeJoin->mLeftSortKeyList = sPlanMergeJoin->mLeftSortKeyList;


    /**
     * Right Sort Key List
     */

    sOptSortMergeJoin->mRightSortKeyList = sPlanMergeJoin->mRightSortKeyList;


    /**
     * Join Left Outer Column List
     */

    sOptSortMergeJoin->mLeftOuterColumnList = sPlanMergeJoin->mJoinLeftOuterColumnList;


    /**
     * Join Right Outer Column List
     */

    sOptSortMergeJoin->mRightOuterColumnList = sPlanMergeJoin->mJoinRightOuterColumnList;


    /**
     * Outer Column List
     */

    sOptSortMergeJoin->mOuterColumnList = sPlanMergeJoin->mOuterColumnList;


    /**
     * Where Clause Logical Filter 설정
     */

    sOptSortMergeJoin->mWhereFilterExpr = sPlanMergeJoin->mWhereFilterExpr;


    /**
     * Where Clause Logical Filter Predicate 설정
     */

    sOptSortMergeJoin->mWhereFilterPred = NULL;


    /**
     * Join Filiter Expression 설정
     */

    sOptSortMergeJoin->mJoinFilterExpr = sPlanMergeJoin->mJoinFilterExpr;


    /**
     * Join Filter Predicate 설정
     */

    sOptSortMergeJoin->mJoinFilterPred = NULL;


    /****************************************************************
     * 마무리
     ****************************************************************/

    /**
     * Output 설정
     */

    aCodeOptParamInfo->mOptNode = sJoinOptNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/****************************************************************************
 * Complete Building Optimization Node Functions
 ***************************************************************************/

/**
 * @brief SORT MERGE JOIN의 Optimization Node 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoCompleteSortMergeJoin( qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptNodeList        * aOptNodeList,
                                     qllOptimizationNode   * aOptNode,
                                     qloInitExprList       * aQueryExprList,
                                     qllEnv                * aEnv )
{
    qlnoSortMergeJoin   * sOptSortMergeJoin     = NULL;

    knlExprStack        * sLogicalFilterStack   = NULL;
    knlPredicateList    * sLogicalFilterPred    = NULL;

    knlExprStack        * sJoinFilterStack      = NULL;
    knlPredicateList    * sJoinFilterPred       = NULL;
    
    qlvInitExpression   * sCodeExpr             = NULL;
    knlExprEntry        * sExprEntry            = NULL;

    qlvInitDataTypeInfo   sDataTypeInfo;
    qlvRefExprItem      * sExprItem             = NULL;
    qlvInitExpression   * sOrgExpr              = NULL;
    qloDBType           * sDBType               = NULL;
    ellDictHandle       * sColumnHandle         = NULL;

    knlExprStack        * sTempCodeStack        = NULL;

    qlvRefExprItem      * sLeftSortKeyItem      = NULL;
    qlvRefExprItem      * sRightSortKeyItem     = NULL;

    stlInt32              sOffset;
    stlInt32              sIdx;

    stlUInt32             sFuncPtrIdx           = 0;
    dtlDataType           sResultType           = DTL_TYPE_NA;
    dtlDataTypeDefInfo    sResultTypeDefInfo;

    dtlBuiltInFuncInfo  * sFuncInfo             = NULL;
    stlBool               sIsConstantData[ 2 ];

    dtlDataType           sInputArgDataTypes[ 2 ];
    dtlDataTypeDefInfo    sInputArgDataTypeDefInfo[ 2 ];

    dtlDataType           sResultArgDataTypes[ 2 ];
    dtlDataTypeDefInfo    sResultArgDataTypeDefInfo[ 2 ];

    dtlDataType           sArgType                  = DTL_TYPE_NA;
    dtlDataTypeDefInfo    sArgTypeDefInfo;

    qloDBType          ** sLeftSortKeyDBTypePtrArr  = NULL;
    qloDBType          ** sRightSortKeyDBTypePtrArr = NULL;

    stlChar             * sLastError                = NULL;
    stlInt32              sErrCastPos;


    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/

    sOptSortMergeJoin = (qlnoSortMergeJoin *) aOptNode->mOptNode;


    /****************************************************************
     * 하위 node에 대한 Complete Node 수행
     ****************************************************************/

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptSortMergeJoin->mLeftChildNode,
                   aQueryExprList,
                   aEnv );

    QLNO_COMPLETE( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptSortMergeJoin->mRightChildNode,
                   aQueryExprList,
                   aEnv );

    
    /****************************************************************
     * Join Column에 대한 DB Type 설정
     ****************************************************************/

    sExprItem = sOptSortMergeJoin->mJoinColList->mHead;
    
    while( sExprItem != NULL )
    {
        STL_DASSERT( sExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
        
        sOrgExpr = sExprItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr;

        if( sOrgExpr->mType == QLV_EXPR_TYPE_COLUMN )
        {
            sColumnHandle = sOrgExpr->mExpr.mColumn->mColumnHandle;
            
            /* sOrgExpr 의 COLUMN 에 대한 DBType 설정 */
            STL_TRY( qloSetExprDBType( sOrgExpr,
                                       aQueryExprList->mStmtExprList->mTotalExprInfo,
                                       ellGetColumnDBType( sColumnHandle ),
                                       ellGetColumnPrecision( sColumnHandle ),
                                       ellGetColumnScale( sColumnHandle ),
                                       ellGetColumnStringLengthUnit( sColumnHandle ),
                                       ellGetColumnIntervalIndicator( sColumnHandle ),
                                       aEnv )
                     == STL_SUCCESS );
        }
        else if( sOrgExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN )
        {
            /* sOrgExpr 의 ROWID COLUMN 에 대한 DBType 설정 */
            STL_TRY( qloSetExprDBType( sOrgExpr,
                                       aQueryExprList->mStmtExprList->mTotalExprInfo,
                                       DTL_TYPE_ROWID,
                                       gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mArgNum1,
                                       gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mArgNum2,
                                       gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mStringLengthUnit,
                                       gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mIntervalIndicator,
                                       aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_DASSERT( sOrgExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
 
            /* sOrgExpr 에 대한 DBType 설정 */
            sDBType  = & aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sOrgExpr->mOffset ];
            STL_TRY( qloSetExprDBType( sOrgExpr,
                                       aQueryExprList->mStmtExprList->mTotalExprInfo,
                                       sDBType->mDBType,
                                       sDBType->mArgNum1,
                                       sDBType->mArgNum2,
                                       sDBType->mStringLengthUnit,
                                       sDBType->mIntervalIndicator,
                                       aEnv )
                     == STL_SUCCESS );
        }

        STL_TRY( qlvGetExprDataTypeInfo( aSQLStmt->mRetrySQL,
                                         QLL_CODE_PLAN( aDBCStmt ),
                                         sExprItem->mExprPtr,
                                         aSQLStmt->mBindContext,
                                         &sDataTypeInfo,
                                         aEnv )
                 == STL_SUCCESS );
        
        /* INNER COLUMN에 대한 DBType 설정 */
        STL_TRY( qloSetExprDBType( sExprItem->mExprPtr,
                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                   sDataTypeInfo.mDataType,
                                   sDataTypeInfo.mArgNum1,
                                   sDataTypeInfo.mArgNum2,
                                   sDataTypeInfo.mStringLengthUnit,
                                   sDataTypeInfo.mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );

        sExprItem = sExprItem->mNext;
    }


    /****************************************************************
     * Sort Key Column에 대한 DB Type 설정
     ****************************************************************/

    /**
     * 임시 Expression Code Stack 공간 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( knlExprStack ),
                                (void **) & sTempCodeStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sTempCodeStack->mMaxEntryCount = 0;


    /**
     * Left Sort Key
     */

    sExprItem = sOptSortMergeJoin->mLeftSortKeyList->mHead;
    while( sExprItem != NULL )
    {
        /* 임시 Expression Code Stack 생성 */
        if( sExprItem->mExprPtr->mIncludeExprCnt > sTempCodeStack->mMaxEntryCount )
        {
            STL_TRY( knlExprInit( sTempCodeStack,
                                  sExprItem->mExprPtr->mIncludeExprCnt,
                                  & QLL_CANDIDATE_MEM( aEnv ),
                                  KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
        else
        {
            sTempCodeStack->mEntryCount = 0;
        }

        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sExprItem->mExprPtr,
                                            aSQLStmt->mBindContext,
                                            sTempCodeStack,
                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            & sExprEntry,
                                            & QLL_CANDIDATE_MEM( aEnv ),
                                            aEnv )
                 == STL_SUCCESS );

        sExprItem = sExprItem->mNext;
    }


    /**
     * Right Sort Key
     */

    sExprItem = sOptSortMergeJoin->mRightSortKeyList->mHead;
    while( sExprItem != NULL )
    {
        /* 임시 Expression Code Stack 생성 */
        if( sExprItem->mExprPtr->mIncludeExprCnt > sTempCodeStack->mMaxEntryCount )
        {
            STL_TRY( knlExprInit( sTempCodeStack,
                                  sExprItem->mExprPtr->mIncludeExprCnt,
                                  & QLL_CANDIDATE_MEM( aEnv ),
                                  KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
        else
        {
            sTempCodeStack->mEntryCount = 0;
        }

        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sExprItem->mExprPtr,
                                            aSQLStmt->mBindContext,
                                            sTempCodeStack,
                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            & sExprEntry,
                                            & QLL_CANDIDATE_MEM( aEnv ),
                                            aEnv )
                 == STL_SUCCESS );

        sExprItem = sExprItem->mNext;
    }


    /****************************************************************
     * Join Condition Filter
     ****************************************************************/
 
    /**
     * Sort Key에 대한 정보 설정
     */

    STL_DASSERT( sOptSortMergeJoin->mLeftSortKeyList->mCount > 0 );

    sIsConstantData[ 0 ] = STL_FALSE;
    sIsConstantData[ 1 ] = STL_FALSE;

    STL_DASSERT( sOptSortMergeJoin->mLeftSortKeyList->mCount ==
                 sOptSortMergeJoin->mRightSortKeyList->mCount );

    /* Left Sort Key DBType Ptr Arr 할당 */
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qloDBType* ) * sOptSortMergeJoin->mLeftSortKeyList->mCount,
                                (void **) & sLeftSortKeyDBTypePtrArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sLeftSortKeyDBTypePtrArr,
               0x00,
               STL_SIZEOF( qloDBType* ) * sOptSortMergeJoin->mLeftSortKeyList->mCount );

    /* Right Sort Key DBType Ptr Arr 할당 */
    STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                STL_SIZEOF( qloDBType* ) * sOptSortMergeJoin->mRightSortKeyList->mCount,
                                (void **) & sRightSortKeyDBTypePtrArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sRightSortKeyDBTypePtrArr,
               0x00,
               STL_SIZEOF( qloDBType* ) * sOptSortMergeJoin->mRightSortKeyList->mCount );

    /* Sort Key에 대한 DBType 정보 수집 */
    sIdx = 0;
    sLeftSortKeyItem = sOptSortMergeJoin->mLeftSortKeyList->mHead;
    sRightSortKeyItem = sOptSortMergeJoin->mRightSortKeyList->mHead;
    while( sLeftSortKeyItem != NULL )
    {
        STL_DASSERT( sRightSortKeyItem != NULL );

        /* Left Sort Key Data Type 셋팅 */
        sOffset = sLeftSortKeyItem->mExprPtr->mOffset;
        sInputArgDataTypes[ 0 ] =
            aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sOffset ].mDBType;

        sInputArgDataTypeDefInfo[ 0 ].mArgNum1 =
            aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sOffset ].mArgNum1;
        sInputArgDataTypeDefInfo[ 0 ].mArgNum2 =
            aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sOffset ].mArgNum2;
        sInputArgDataTypeDefInfo[ 0 ].mStringLengthUnit =
            aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sOffset ].mStringLengthUnit;
        sInputArgDataTypeDefInfo[ 0 ].mIntervalIndicator =
            aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sOffset ].mIntervalIndicator;

        sLeftSortKeyDBTypePtrArr[sIdx] = & aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sOffset ];

        /* Right Sort Key Data Type 셋팅 */
        sOffset = sRightSortKeyItem->mExprPtr->mOffset;
        sInputArgDataTypes[ 1 ] =
            aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sOffset ].mDBType;

        sInputArgDataTypeDefInfo[ 1 ].mArgNum1 =
            aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sOffset ].mArgNum1;
        sInputArgDataTypeDefInfo[ 1 ].mArgNum2 =
            aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sOffset ].mArgNum2;
        sInputArgDataTypeDefInfo[ 1 ].mStringLengthUnit =
            aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sOffset ].mStringLengthUnit;
        sInputArgDataTypeDefInfo[ 1 ].mIntervalIndicator =
            aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sOffset ].mIntervalIndicator;

        sRightSortKeyDBTypePtrArr[sIdx] = & aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType[ sOffset ];

        /* Function의 Return Data Type 결정하기 */
        sFuncInfo = & gBuiltInFuncInfoArr[ KNL_BUILTIN_FUNC_IS_EQUAL ];

        STL_TRY( sFuncInfo->mGetFuncDetailInfo( 2 /* Arg Count */,
                                                sIsConstantData,
                                                sInputArgDataTypes,
                                                sInputArgDataTypeDefInfo,
                                                2 /* Arg Count */,
                                                sResultArgDataTypes,
                                                sResultArgDataTypeDefInfo,
                                                & sResultType,
                                                & sResultTypeDefInfo,
                                                & sFuncPtrIdx,
                                                KNL_DT_VECTOR(aEnv),
                                                aEnv,
                                                QLL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );

        /**
         * Set Cast : arguments의 DB Type 설정
         */

        /* Left Sort Key Cast 설정 */
        sArgType  = sResultArgDataTypes[ 0 ];
        sArgTypeDefInfo.mArgNum1 = sResultArgDataTypeDefInfo[ 0 ].mArgNum1;
        sArgTypeDefInfo.mArgNum2 = sResultArgDataTypeDefInfo[ 0 ].mArgNum2;
        sArgTypeDefInfo.mStringLengthUnit = sResultArgDataTypeDefInfo[ 0 ].mStringLengthUnit;
        sArgTypeDefInfo.mIntervalIndicator = sResultArgDataTypeDefInfo[ 0 ].mIntervalIndicator;

        sErrCastPos = sLeftSortKeyItem->mExprPtr->mPosition;
        STL_TRY_THROW( qloCastExprDBType( sLeftSortKeyItem->mExprPtr,
                                          aQueryExprList->mStmtExprList->mTotalExprInfo,
                                          sArgType,
                                          sArgTypeDefInfo.mArgNum1,
                                          sArgTypeDefInfo.mArgNum2,
                                          sArgTypeDefInfo.mStringLengthUnit,
                                          sArgTypeDefInfo.mIntervalIndicator,
                                          STL_TRUE,
                                          STL_FALSE,
                                          aEnv )
                       == STL_SUCCESS,
                       RAMP_ERR_CAST_DB_TYPE );

        /* Right Sort Key Cast 설정 */
        sArgType  = sResultArgDataTypes[ 1 ];
        sArgTypeDefInfo.mArgNum1 = sResultArgDataTypeDefInfo[ 1 ].mArgNum1;
        sArgTypeDefInfo.mArgNum2 = sResultArgDataTypeDefInfo[ 1 ].mArgNum2;
        sArgTypeDefInfo.mStringLengthUnit = sResultArgDataTypeDefInfo[ 1 ].mStringLengthUnit;
        sArgTypeDefInfo.mIntervalIndicator = sResultArgDataTypeDefInfo[ 1 ].mIntervalIndicator;

        sErrCastPos = sRightSortKeyItem->mExprPtr->mPosition;
        STL_TRY_THROW( qloCastExprDBType( sRightSortKeyItem->mExprPtr,
                                          aQueryExprList->mStmtExprList->mTotalExprInfo,
                                          sArgType,
                                          sArgTypeDefInfo.mArgNum1,
                                          sArgTypeDefInfo.mArgNum2,
                                          sArgTypeDefInfo.mStringLengthUnit,
                                          sArgTypeDefInfo.mIntervalIndicator,
                                          STL_TRUE,
                                          STL_FALSE,
                                          aEnv )
                       == STL_SUCCESS,
                       RAMP_ERR_CAST_DB_TYPE );

        sIdx++;
        sLeftSortKeyItem = sLeftSortKeyItem->mNext;
        sRightSortKeyItem = sRightSortKeyItem->mNext;
    }

    sOptSortMergeJoin->mLeftSortKeyDBTypePtrArr = sLeftSortKeyDBTypePtrArr;
    sOptSortMergeJoin->mRightSortKeyDBTypePtrArr = sRightSortKeyDBTypePtrArr;


    /**
     * Join Condition Filter를 Logical Filter 형태로 생성
     */

    if( sOptSortMergeJoin->mJoinFilterExpr == NULL )
    {
        sJoinFilterPred  = NULL;
    }
    else
    {
        /**
         * Join Filter Predicate 생성
         */

        /**
         * Join Expression Code Stack 공간 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprStack ),
                                    (void **) & sJoinFilterStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );


        /**
         * Join Expression Stack 생성
         */

        sCodeExpr = sOptSortMergeJoin->mJoinFilterExpr;

        STL_TRY( knlExprInit( sJoinFilterStack,
                              sCodeExpr->mIncludeExprCnt,
                              QLL_CODE_PLAN( aDBCStmt ),
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );


        /**
         * Logical Expression Stack 구성
         */

        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sCodeExpr,
                                            aSQLStmt->mBindContext,
                                            sJoinFilterStack,
                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            & sExprEntry,
                                            QLL_CODE_PLAN(aDBCStmt),
                                            aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( qloCastExprDBType(
                           sCodeExpr,
                           aQueryExprList->mStmtExprList->mTotalExprInfo,
                           DTL_TYPE_BOOLEAN,
                           gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                           gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                           gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                           gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                           STL_FALSE,
                           STL_FALSE,
                           aEnv )
                       == STL_SUCCESS,
                       RAMP_ERR_INVALID_WHERE_CLAUSE );


        /**
         * Logical Expression에 대한 Filter Predicate 구성
         */

        STL_TRY( knlMakeFilter( sJoinFilterStack,
                                & sJoinFilterPred,
                                QLL_CODE_PLAN( aDBCStmt ),
                                KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }


    /****************************************************************
     * Where Filter 처리
     ****************************************************************/
    
    if( sOptSortMergeJoin->mWhereFilterExpr == NULL )
    {
        sLogicalFilterPred  = NULL;
    }
    else
    {
        if( sOptSortMergeJoin->mWhereFilterExpr == NULL )
        {
            /* Do Nothing */
        }
        else
        {
            /**
             * Logical Filter Predicate 생성
             */

            /**
             * Logical Expression Code Stack 공간 할당
             */

            STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                        STL_SIZEOF( knlExprStack ),
                                        (void **) & sLogicalFilterStack,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );


            /**
             * Logical Expression Stack 생성
             */

            sCodeExpr = sOptSortMergeJoin->mWhereFilterExpr;

            STL_TRY( knlExprInit( sLogicalFilterStack,
                                  sCodeExpr->mIncludeExprCnt,
                                  QLL_CODE_PLAN( aDBCStmt ),
                                  KNL_ENV( aEnv ) )
                     == STL_SUCCESS );


            /**
             * Logical Expression Stack 구성
             */

            STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                sCodeExpr,
                                                aSQLStmt->mBindContext,
                                                sLogicalFilterStack,
                                                aQueryExprList->mStmtExprList->mConstExprStack,
                                                aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                & sExprEntry,
                                                QLL_CODE_PLAN(aDBCStmt),
                                                aEnv )
                     == STL_SUCCESS );

            STL_TRY_THROW( qloCastExprDBType(
                               sCodeExpr,
                               aQueryExprList->mStmtExprList->mTotalExprInfo,
                               DTL_TYPE_BOOLEAN,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                               STL_FALSE,
                               STL_FALSE,
                               aEnv )
                           == STL_SUCCESS,
                           RAMP_ERR_INVALID_WHERE_CLAUSE );


            /**
             * Logical Expression에 대한 Filter Predicate 구성
             */

            STL_TRY( knlMakeFilter( sLogicalFilterStack,
                                    & sLogicalFilterPred,
                                    QLL_CODE_PLAN( aDBCStmt ),
                                    KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
    }
        
    
    /****************************************************************
     * Node 정보 설정
     ****************************************************************/

    /**
     * Logical Filter Predicate
     */
    
    sOptSortMergeJoin->mWhereFilterPred = sLogicalFilterPred;


    /**
     * Join Filter Predicate
     */

    sOptSortMergeJoin->mJoinFilterPred = sJoinFilterPred;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_WHERE_CLAUSE )
    {
        (void)stlPopError( QLL_ERROR_STACK(aEnv) );

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BOOLEAN_EXPRESSION_EXPECTED,
                      qlgMakeErrPosString( aSQLStmt->mRetrySQL,
                                           sCodeExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_CAST_DB_TYPE )
    {
        if( aSQLStmt->mRetrySQL != NULL )
        {
            sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
            
            if( sLastError[0] == '\0' )
            {
                stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                              qlgMakeErrPosString( aSQLStmt->mRetrySQL,
                                                                   sErrCastPos,
                                                                   aEnv ) );
            }
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SORT MERGE JOIN의 All Expr 리스트 정보를 완성한다.
 * @param[in]     aDBCStmt        DBC Statement
 * @param[in]     aSQLStmt        SQL Statement 객체
 * @param[in]     aOptNodeList    Optimization Node List
 * @param[in,out] aOptNode        Optimization Node
 * @param[in,out] aQueryExprList  Query 단위 Expression 정보
 * @param[in]     aEnv            Environment
 */
stlStatus qlnoAddExprSortMergeJoin( qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptNodeList        * aOptNodeList,
                                    qllOptimizationNode   * aOptNode,
                                    qloInitExprList       * aQueryExprList,
                                    qllEnv                * aEnv )
{
    qlnoSortMergeJoin     * sOptSortMergeJoin   = NULL;
    qlvRefExprItem        * sRefColumnItem      = NULL;
    

    /*
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_MERGE_JOIN_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );    


    /****************************************************************
     * Optimization Node 얻기
     ****************************************************************/
    
    sOptSortMergeJoin = (qlnoSortMergeJoin *) aOptNode->mOptNode;


    /****************************************************************
     * Child Node의 Add Expression 수행
     ****************************************************************/

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptSortMergeJoin->mLeftChildNode,
                   aQueryExprList,
                   aEnv );

    QLNO_ADD_EXPR( aDBCStmt,
                   aSQLStmt,
                   aOptNodeList,
                   sOptSortMergeJoin->mRightChildNode,
                   aQueryExprList,
                   aEnv );


    /**
     * Add Expr Join Read Column
     */

    sRefColumnItem = sOptSortMergeJoin->mJoinColList->mHead;
    while( sRefColumnItem != NULL )
    {
        STL_DASSERT( sRefColumnItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );

        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /**
     * Add Expr Join Left Sort Key Column
     */

    sRefColumnItem = sOptSortMergeJoin->mLeftSortKeyList->mHead;
    while( sRefColumnItem != NULL )
    {
        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /**
     * Add Expr Join Right Sort Key Column
     */

    sRefColumnItem = sOptSortMergeJoin->mRightSortKeyList->mHead;
    while( sRefColumnItem != NULL )
    {
        STL_TRY( qloAddExpr( sRefColumnItem->mExprPtr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );

        sRefColumnItem = sRefColumnItem->mNext;
    }


    /**
     * Add Expr Join Condition Expression
     */

    if( sOptSortMergeJoin->mJoinFilterExpr != NULL )
    {
        STL_TRY( qloAddExpr( sOptSortMergeJoin->mJoinFilterExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Add Expr Where Clause Expression
     */

    if( sOptSortMergeJoin->mWhereFilterExpr != NULL )
    {
        STL_TRY( qloAddExpr( sOptSortMergeJoin->mWhereFilterExpr,
                             aQueryExprList->mStmtExprList->mAllExprList,
                             QLL_CODE_PLAN(aDBCStmt),
                             aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlno */

