/*******************************************************************************
 * qlncMakeCandPlanExpr.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlncMakeCandPlanExpr.c 9803 2013-10-07 03:37:40Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file qlncMakeCandPlanExpr.c
 * @brief SQL Processor Layer Make Candidate Plan Expression
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Convert Candidate Expression To Init Expression Function List
 */
const qlncConvertCandExprToInitExprFuncPtr qlncConvertCandExprToInitExprFuncList[ QLNC_EXPR_TYPE_MAX ] =
{
    qlncConvertCandExprToInitExprValue,             /**< QLNC_EXPR_TYPE_VALUE */
    qlncConvertCandExprToInitExprBindParam,         /**< QLNC_EXPR_TYPE_BIND_PARAM */
    qlncConvertCandExprToInitExprColumn,            /**< QLNC_EXPR_TYPE_COLUMN */
    qlncConvertCandExprToInitExprFunction,          /**< QLNC_EXPR_TYPE_FUNCTION */
    qlncConvertCandExprToInitExprCast,              /**< QLNC_EXPR_TYPE_CAST */
    qlncConvertCandExprToInitExprPseudoColumn,      /**< QLNC_EXPR_TYPE_PSEUDO_COLUMN */
    qlncConvertCandExprToInitExprPseudoArgument,    /**< QLNC_EXPR_TYPE_PSEUDO_ARGUMENT */
    qlncConvertCandExprToInitExprConstantExprResult,/**< QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT */
    qlncConvertCandExprToInitExprSubQuery,          /**< QLNC_EXPR_TYPE_SUB_QUERY */
    qlncConvertCandExprToInitExprReference,         /**< QLNC_EXPR_TYPE_REFERENCE */
    qlncConvertCandExprToInitExprInnerColumn,       /**< QLNC_EXPR_TYPE_INNER_COLUMN */
    qlncConvertCandExprToInitExprOuterColumn,       /**< QLNC_EXPR_TYPE_OUTER_COLUMN */
    qlncConvertCandExprToInitExprRowidColumn,       /**< QLNC_EXPR_TYPE_ROWID_COLUMN */
    qlncConvertCandExprToInitExprAggregation,       /**< QLNC_EXPR_TYPE_AGGREGATION */
    qlncConvertCandExprToInitExprCaseExpr,          /**< QLNC_EXPR_TYPE_CASE_EXPR */
    qlncConvertCandExprToInitExprListFunction,      /**< QLNC_EXPR_TYPE_LIST_FUNCTION */
    qlncConvertCandExprToInitExprListColumn,        /**< QLNC_EXPR_TYPE_LIST_COLUMN */
    qlncConvertCandExprToInitExprRowExpr,           /**< QLNC_EXPR_TYPE_ROW_EXPR */
    qlncConvertCandExprToInitExprAndFilter,         /**< QLNC_EXPR_TYPE_AND_FILTER */
    qlncConvertCandExprToInitExprOrFilter,          /**< QLNC_EXPR_TYPE_OR_FILTER */
    qlncConvertCandExprToInitExprBooleanFilter,     /**< QLNC_EXPR_TYPE_BOOLEAN_FILTER */
    qlncConvertCandExprToInitExprConstBooleanFilter /**< QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER */
};


/**
 * @brief Candidate Value Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprValue( qlncConvExprParamInfo * aConvExprParamInfo,
                                              qllEnv                * aEnv )
{
    qlncExprValue       * sCandValue    = NULL;
    qlvInitValue        * sInitValue    = NULL;
    qlvInitExpression   * sInitExpr     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_VALUE,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Candidate Value Expression 설정 */
    sCandValue = (qlncExprValue*)(aConvExprParamInfo->mCandExpr);

    /* Init Value 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitValue ),
                                (void**) &sInitValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Value Type 설정 */
    sInitValue->mValueType = (qlvValueType)(sCandValue->mValueType);
    sInitValue->mInternalBindIdx = sCandValue->mInternalBindIdx;

    /* Value Data 설정 */
    if( (sCandValue->mValueType == QLNC_VALUE_TYPE_BOOLEAN) ||
        (sCandValue->mValueType == QLNC_VALUE_TYPE_BINARY_INTEGER) )
    {
        sInitValue->mData.mInteger = sCandValue->mData.mInteger;
    }
    else
    {
        QLNC_ALLOC_AND_COPY_STRING( sInitValue->mData.mString,
                                    sCandValue->mData.mString,
                                    aConvExprParamInfo->mRegionMem,
                                    aEnv );
    }

    /* Init Expression 생성 */
    QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                               QLV_EXPR_TYPE_VALUE,
                               KNL_BUILTIN_FUNC_INVALID,
                               sCandValue->mCommon.mIterationTime,
                               sCandValue->mCommon.mPosition,
                               sCandValue->mCommon.mExprPhraseFlag,
                               0,
                               sCandValue->mCommon.mColumnName,
                               sInitValue,
                               sInitExpr,
                               aEnv );


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Bind Param Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprBindParam( qlncConvExprParamInfo * aConvExprParamInfo,
                                                  qllEnv                * aEnv )
{
    qlncExprBindParam   * sCandBindParam    = NULL;
    qlvInitBindParam    * sInitBindParam    = NULL;
    qlvInitExpression   * sInitExpr         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_BIND_PARAM,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Candidate Bind Param Expression 설정 */
    sCandBindParam = (qlncExprBindParam*)(aConvExprParamInfo->mCandExpr);

    /* Init Bind Param 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitBindParam ),
                                (void**) &sInitBindParam,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Bind Type 설정 */
    sInitBindParam->mBindType = sCandBindParam->mBindType;

    /* Bind Parameter Index 설정 */
    sInitBindParam->mBindParamIdx = sCandBindParam->mBindParamIdx;

    /* Host Name 복사 */
    QLNC_ALLOC_AND_COPY_STRING( sInitBindParam->mHostName,
                                sCandBindParam->mHostName,
                                aConvExprParamInfo->mRegionMem,
                                aEnv );

    /* Indicator Name 복사 */
    QLNC_ALLOC_AND_COPY_STRING( sInitBindParam->mIndicatorName,
                                sCandBindParam->mIndicatorName,
                                aConvExprParamInfo->mRegionMem,
                                aEnv );

    /* Init Expression 생성 */
    QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                               QLV_EXPR_TYPE_BIND_PARAM,
                               KNL_BUILTIN_FUNC_INVALID,
                               sCandBindParam->mCommon.mIterationTime,
                               sCandBindParam->mCommon.mPosition,
                               sCandBindParam->mCommon.mExprPhraseFlag,
                               0,
                               sCandBindParam->mCommon.mColumnName,
                               sInitBindParam,
                               sInitExpr,
                               aEnv );

    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Column Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprColumn( qlncConvExprParamInfo    * aConvExprParamInfo,
                                               qllEnv                   * aEnv )
{
    qlvInitExpression   * sInitExpr     = NULL;
    qlvInitColumn       * sInitColumn   = NULL;
    qlvInitExpression   * sOrgExpr      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_COLUMN,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Read Column을 Add하고 Add된 Column의 Init Expression을 가져옴 */
    STL_TRY( qlncAddReadColumnOnCurPlan( aConvExprParamInfo,
                                         aEnv )
             == STL_SUCCESS );
    sOrgExpr = aConvExprParamInfo->mInitExpr;

    if( sOrgExpr->mType == QLV_EXPR_TYPE_OUTER_COLUMN )
    {
        /* Outer Column인 경우 Expression을 복사하지 않는다. */
        sInitExpr = sOrgExpr;
    }
    else
    {
        if( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_MERGE_JOIN )
        {
            STL_DASSERT( (aConvExprParamInfo->mJoinLeftOuterInfoList.mCount == 0) ||
                         (aConvExprParamInfo->mJoinRightOuterInfoList.mCount == 0) );

            STL_DASSERT( sOrgExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

            STL_TRY( qlvCopyExpr( sOrgExpr,
                                  &sInitExpr,
                                  aConvExprParamInfo->mRegionMem,
                                  aEnv )
                     == STL_SUCCESS );

            sInitExpr->mPosition = aConvExprParamInfo->mCandExpr->mPosition;                    
        }
        else
        {
            /* Outer Column이 아닌 경우 Expression을 Copy한다. */

            if( sOrgExpr->mType == QLV_EXPR_TYPE_COLUMN )
            {
                /* Init Expression 할당 */
                STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                            STL_SIZEOF( qlvInitExpression ),
                                            (void**) &sInitExpr,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                /* Init Expression 복사 */
                stlMemcpy( sInitExpr, sOrgExpr, STL_SIZEOF( qlvInitExpression ) );
                sInitExpr->mPosition = aConvExprParamInfo->mCandExpr->mPosition;
                    
                /* Init Column 할당 */
                STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                            STL_SIZEOF( qlvInitColumn ),
                                            (void**) &sInitColumn,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                /* Init Column 정보 설정 */
                sInitColumn->mRelationNode = sOrgExpr->mExpr.mColumn->mRelationNode;

                QLNC_ALLOC_AND_COPY_DICT_HANDLE( aConvExprParamInfo->mRegionMem,
                                                 sOrgExpr->mExpr.mColumn->mColumnHandle,
                                                 sInitColumn->mColumnHandle,
                                                 aEnv );

                /* Init Column 연결 */
                sInitExpr->mExpr.mColumn = sInitColumn;
            }
            else
            {
                STL_TRY( qlvCopyExpr( sOrgExpr,
                                      &sInitExpr,
                                      aConvExprParamInfo->mRegionMem,
                                      aEnv )
                         == STL_SUCCESS );

                sInitExpr->mPosition = aConvExprParamInfo->mCandExpr->mPosition;
            }
        }
    }


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Function Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprFunction( qlncConvExprParamInfo  * aConvExprParamInfo,
                                                 qllEnv                 * aEnv )
{
    stlInt32              i;
    stlInt32              sIncludeExprCnt;
    qlncExprFunction    * sCandFunction  = NULL;
    qlvInitExpression   * sInitExpr      = NULL;
    qlvInitFunction     * sInitFunction  = NULL;
    dtlIterationTime      sIterationTime = DTL_ITERATION_TIME_INVALID;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_FUNCTION,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Candidate Function Expression 설정 */
    sCandFunction = (qlncExprFunction*)(aConvExprParamInfo->mCandExpr);

    /* Init Function 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitFunction ),
                                (void**) &sInitFunction,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Function Id  설정 */
    sInitFunction->mFuncId = sCandFunction->mFuncId;

    /* Argument Count 설정 */
    sInitFunction->mArgCount = sCandFunction->mArgCount;

    /* Argument 할당 */
    if( sCandFunction->mArgCount > 0 )
    {
        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitExpression* ) * sCandFunction->mArgCount,
                                    (void**) &(sInitFunction->mArgs),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    /* Argument 설정 */
    sIncludeExprCnt = 0;
    for( i = 0; i < sCandFunction->mArgCount; i++ )
    {
        aConvExprParamInfo->mCandExpr = sCandFunction->mArgs[i];
        STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandFunction->mArgs[i]->mType](
                     aConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );
        sInitFunction->mArgs[i] = aConvExprParamInfo->mInitExpr;

        /* Argument의 Related Function ID 변경 */
        sInitFunction->mArgs[i]->mRelatedFuncId = sCandFunction->mFuncId;

        /* Argument의 Include Expression Count 누적 */
        sIncludeExprCnt += sInitFunction->mArgs[i]->mIncludeExprCnt;

        /* Iteration Time 설정 */
        sIterationTime = ( sIterationTime < sInitFunction->mArgs[i]->mIterationTime )
            ? sIterationTime : sInitFunction->mArgs[i]->mIterationTime;
    }

    /* Function Init Expression 생성 */
    QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                               QLV_EXPR_TYPE_FUNCTION,
                               KNL_BUILTIN_FUNC_INVALID,
                               sIterationTime,
                               sCandFunction->mCommon.mPosition,
                               sCandFunction->mCommon.mExprPhraseFlag,
                               sIncludeExprCnt,
                               sCandFunction->mCommon.mColumnName,
                               sInitFunction,
                               sInitExpr,
                               aEnv );


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Cast Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprCast( qlncConvExprParamInfo  * aConvExprParamInfo,
                                             qllEnv                 * aEnv )
{
    stlInt32              i;
    stlInt32              sIncludeExprCnt;
    qlncExprTypeCast    * sCandTypeCast  = NULL;
    qlvInitTypeCast     * sInitTypeCast  = NULL;
    qlvInitExpression   * sInitExpr      = NULL;
    dtlIterationTime      sIterationTime = DTL_ITERATION_TIME_INVALID;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_CAST,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Candidate Type Cast Expression 설정 */
    sCandTypeCast = (qlncExprTypeCast*)(aConvExprParamInfo->mCandExpr);

    /* Type Cast 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitTypeCast ),
                                (void**) &sInitTypeCast,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Type Definition 설정 */
    sInitTypeCast->mTypeDef.mDBType = sCandTypeCast->mTypeDef.mDBType;
    QLNC_ALLOC_AND_COPY_STRING( sInitTypeCast->mTypeDef.mUserTypeName,
                                sCandTypeCast->mTypeDef.mUserTypeName,
                                aConvExprParamInfo->mRegionMem,
                                aEnv );
    sInitTypeCast->mTypeDef.mArgNum1 = sCandTypeCast->mTypeDef.mArgNum1;
    sInitTypeCast->mTypeDef.mArgNum2 = sCandTypeCast->mTypeDef.mArgNum2;
    sInitTypeCast->mTypeDef.mStringLengthUnit = sCandTypeCast->mTypeDef.mStringLengthUnit;
    sInitTypeCast->mTypeDef.mIntervalIndicator = sCandTypeCast->mTypeDef.mIntervalIndicator;
    sInitTypeCast->mTypeDef.mNumericRadix = sCandTypeCast->mTypeDef.mNumericRadix;

    /* Argument 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitExpression* ) * DTL_CAST_INPUT_ARG_CNT,
                                (void**) &(sInitTypeCast->mArgs),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Argument 설정 */
    sIncludeExprCnt = 0;
    for( i = 0; i < DTL_CAST_INPUT_ARG_CNT; i++ )
    {
        aConvExprParamInfo->mCandExpr = sCandTypeCast->mArgs[i];
        STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandTypeCast->mArgs[i]->mType](
                     aConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );
        sInitTypeCast->mArgs[i] = aConvExprParamInfo->mInitExpr;

        /* Argument의 Related Function ID 변경 */
        sInitTypeCast->mArgs[i]->mRelatedFuncId = KNL_BUILTIN_FUNC_CAST;

        /* Argument의 Include Expression Count 누적 */
        sIncludeExprCnt += sInitTypeCast->mArgs[i]->mIncludeExprCnt;

        /* Iteration Time 설정 */
        sIterationTime = ( sIterationTime < sInitTypeCast->mArgs[i]->mIterationTime )
            ? sIterationTime : sInitTypeCast->mArgs[i]->mIterationTime;
    }

    /* Type Cast Init Expression 생성 */
    QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                               QLV_EXPR_TYPE_CAST,
                               KNL_BUILTIN_FUNC_INVALID,
                               sIterationTime,
                               sCandTypeCast->mCommon.mPosition,
                               sCandTypeCast->mCommon.mExprPhraseFlag,
                               sIncludeExprCnt,
                               sCandTypeCast->mCommon.mColumnName,
                               sInitTypeCast,
                               sInitExpr,
                               aEnv );


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Pseudo Column Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprPseudoColumn( qlncConvExprParamInfo  * aConvExprParamInfo,
                                                     qllEnv                 * aEnv )
{
    stlInt32              i;
    qlncExprPseudoCol   * sCandPseudoCol    = NULL;
    qlvInitPseudoCol    * sInitPseudoCol    = NULL;
    qlvInitExpression   * sInitExpr         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_PSEUDO_COLUMN,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Candidate Pseudo Col Expression 설정 */
    sCandPseudoCol = (qlncExprPseudoCol*)(aConvExprParamInfo->mCandExpr);

    /* Pseudo Col 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitPseudoCol ),
                                (void**) &sInitPseudoCol,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Pseudo Id 설정 */
    sInitPseudoCol->mPseudoId = sCandPseudoCol->mPseudoId;

    /* Argument Count 설정 */
    sInitPseudoCol->mArgCount = sCandPseudoCol->mArgCount;

    /* Sequence Dictionary Handle 설정 */
    QLNC_ALLOC_AND_COPY_DICT_HANDLE( aConvExprParamInfo->mRegionMem,
                                     sCandPseudoCol->mSeqDictHandle,
                                     sInitPseudoCol->mSeqDictHandle,
                                     aEnv );

    /* Pseudo Offset 설정 */
    sInitPseudoCol->mPseudoOffset = sCandPseudoCol->mPseudoOffset;

    /* Argument 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitExpression* ) * sCandPseudoCol->mArgCount,
                                (void**) &(sInitPseudoCol->mArgs),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Argument 설정 */
    for( i = 0; i < sCandPseudoCol->mArgCount; i++ )
    {
        sInitPseudoCol->mArgs[i] = sCandPseudoCol->mArgs[i];
    }

    /* Pseudo Col Init Expression 생성 */
    QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                               QLV_EXPR_TYPE_PSEUDO_COLUMN,
                               KNL_BUILTIN_FUNC_INVALID,
                               sCandPseudoCol->mCommon.mIterationTime,
                               sCandPseudoCol->mCommon.mPosition,
                               sCandPseudoCol->mCommon.mExprPhraseFlag,
                               0,
                               sCandPseudoCol->mCommon.mColumnName,
                               sInitPseudoCol,
                               sInitExpr,
                               aEnv );

    /* Init Expression List 설정 */
    switch( sCandPseudoCol->mCommon.mIterationTime )
    {
        case DTL_ITERATION_TIME_FOR_EACH_EXPR:
            {  
                /**
                 * 현재 레벨의 Expression List내에 존재하는
                 * Pseudo Column Expression List를 가져옴
                 */

                /* Query Unit */
                STL_TRY( qlvAddPseudoColExprList( aConvExprParamInfo->mInitExprList->mPseudoColExprList,
                                                  sInitExpr,
                                                  aConvExprParamInfo->mRegionMem,
                                                  aEnv )
                         == STL_SUCCESS );

                /* Statement Unit */
                STL_TRY( qlvAddPseudoColExprList( aConvExprParamInfo->mInitExprList->mStmtExprList->mPseudoColExprList,
                                                  sInitExpr,
                                                  aConvExprParamInfo->mRegionMem,
                                                  aEnv )
                         == STL_SUCCESS );

                break;
            }
        case DTL_ITERATION_TIME_FOR_EACH_STATEMENT:
        case DTL_ITERATION_TIME_NONE:
            {
                /* @todo 현재 pseudo column은 each expr만 존재한다. */

                /**
                 * Root 레벨(STMT 레벨)의 Expression List내에 존재하는
                 * Pseudo Column Expression List를 가져옴
                 */

                /* Statement Unit */
                STL_TRY( qlvAddPseudoColExprList( aConvExprParamInfo->mInitExprList->mStmtExprList->mPseudoColExprList,
                                                  sInitExpr,
                                                  aConvExprParamInfo->mRegionMem,
                                                  aEnv )
                         == STL_SUCCESS );
                break;
            }  
        default:
            {
                STL_DASSERT( 0 );
                break;
            }
    }


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Pseudo Argument Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprPseudoArgument( qlncConvExprParamInfo    * aConvExprParamInfo,
                                                       qllEnv                   * aEnv )
{
#if 0
    qlncExprPseudoArg   * sCandPseudoArg    = NULL;
    qlvInitExpression   * sInitExpr         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_PSEUDO_ARGUMENT,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Candidate Pseudo Argument Expression 설정 */
    sCandPseudoArg = (qlncExprPseudoArg*)(aConvExprParamInfo->mCandExpr);

    /* Pseudo Argument Init Expression 생성 */
    QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                               QLV_EXPR_TYPE_PSEUDO_ARGUMENT,
                               KNL_BUILTIN_FUNC_INVALID,
                               sCandPseudoArg->mCommon.mIterationTime,
                               sCandPseudoArg->mCommon.mPosition,
                               sCandPseudoArg->mCommon.mExprPhraseFlag,
                               0,
                               sCandPseudoArg->mCommon.mColumnName,
                               sCandPseudoArg->mPseudoArg,
                               sInitExpr,
                               aEnv );


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;
#endif

    /* @todo 현재 Pseudo Argument는 없다. */

    return STL_FAILURE;
}


/**
 * @brief Candidate Constant Expr Result Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprConstantExprResult( qlncConvExprParamInfo    * aConvExprParamInfo,
                                                           qllEnv                   * aEnv )
{
    qlncExprConstExpr   * sCandConstExpr    = NULL;
    qlvInitExpression   * sInitExpr         = NULL;
    qlvInitConstExpr    * sInitConstExpr    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Candidate Constant Expr Result Expression 설정 */
    sCandConstExpr = (qlncExprConstExpr*)(aConvExprParamInfo->mCandExpr);

    /* Constant Expr Result 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitConstExpr ),
                                (void**) &sInitConstExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Original Expression 설정 */
    aConvExprParamInfo->mCandExpr = sCandConstExpr->mOrgExpr;
    STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandConstExpr->mOrgExpr->mType](
                 aConvExprParamInfo,
                 aEnv )
             == STL_SUCCESS );
    sInitConstExpr->mOrgExpr = aConvExprParamInfo->mInitExpr;

    /* Constant Expr Result Init Expression 할당 */
    QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                               QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT,
                               KNL_BUILTIN_FUNC_INVALID,
                               sCandConstExpr->mCommon.mIterationTime,
                               sCandConstExpr->mCommon.mPosition,
                               sCandConstExpr->mCommon.mExprPhraseFlag,
                               sInitConstExpr->mOrgExpr->mIncludeExprCnt,
                               sCandConstExpr->mCommon.mColumnName,
                               sInitConstExpr,
                               sInitExpr,
                               aEnv );

    /* Init Expression List 설정 */
    if( sCandConstExpr->mCommon.mIterationTime >= DTL_ITERATION_TIME_FOR_EACH_STATEMENT )
    {
        /* Constant Expression을 Constant Expression List에 추가 */
        STL_TRY( qlvAddExprToRefExprList( aConvExprParamInfo->mInitExprList->mStmtExprList->mConstExprList,
                                          sInitExpr,
                                          STL_FALSE,
                                          aConvExprParamInfo->mRegionMem,
                                          aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate SubQuery Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprSubQuery( qlncConvExprParamInfo  * aConvExprParamInfo,
                                                 qllEnv                 * aEnv )
{
    qlncExprSubQuery    * sCandSubQuery     = NULL;
    qlvInitExpression   * sInitExpr         = NULL;
    qlvInitExpression   * sTempExpr         = NULL;
    qlvInitSubQuery     * sInitSubQuery     = NULL;
    qlncPlanCommon      * sCandPlan         = NULL;
    qlvRefExprItem      * sExprItem         = NULL;
    stlInt32              sLoop             = 0;
    dtlIterationTime      sIterationTime    = DTL_ITERATION_TIME_INVALID;

    qlncJoinOuterInfoList     sTmpJoinOuterInfoList;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_SUB_QUERY,
                                                    aEnv );


#define _QLNC_APPEND_JOIN_OUTER_INFO_LIST( _aDstJoinOuterInfoList, _aSrcJoinOuterInfoList )     \
    {                                                                                           \
        qlncJoinOuterInfoItem   * _sJoinOuterInfoItem       = NULL;                             \
        qlncJoinOuterInfoItem   * _sTmpJoinOuterInfoItem    = NULL;                             \
                                                                                                \
        _sJoinOuterInfoItem = (_aSrcJoinOuterInfoList)->mHead;                                  \
        while( _sJoinOuterInfoItem != NULL )                                                    \
        {                                                                                       \
            STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,                         \
                                        STL_SIZEOF( qlncJoinOuterInfoItem ),                    \
                                        (void**) &_sTmpJoinOuterInfoItem,                       \
                                        KNL_ENV(aEnv) )                                         \
                     == STL_SUCCESS );                                                          \
            _sTmpJoinOuterInfoItem->mOuterPlanNode = _sJoinOuterInfoItem->mOuterPlanNode;       \
            _sTmpJoinOuterInfoItem->mOuterColumnList = _sJoinOuterInfoItem->mOuterColumnList;   \
                                                                                                \
            _sTmpJoinOuterInfoItem->mNext = (_aDstJoinOuterInfoList)->mHead;                    \
            (_aDstJoinOuterInfoList)->mHead = _sTmpJoinOuterInfoItem;                           \
            (_aDstJoinOuterInfoList)->mCount++;                                                 \
                                                                                                \
            _sJoinOuterInfoItem = _sJoinOuterInfoItem->mNext;                                   \
        }                                                                                       \
    }


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Candidate SubQuery Expression 설정 */
    sCandSubQuery = (qlncExprSubQuery*)(aConvExprParamInfo->mCandExpr);

    /* SubQuery 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitSubQuery ),
                                (void**) &sInitSubQuery,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Join Outer Info List 설정 */
    sTmpJoinOuterInfoList.mCount = 0;
    sTmpJoinOuterInfoList.mHead = NULL;

    /* Left Join Outer Info List 등록 */
    _QLNC_APPEND_JOIN_OUTER_INFO_LIST( &sTmpJoinOuterInfoList,
                                       &(aConvExprParamInfo->mJoinLeftOuterInfoList) );

    /* Right Join Outer Info List 등록 */
    _QLNC_APPEND_JOIN_OUTER_INFO_LIST( &sTmpJoinOuterInfoList,
                                       &(aConvExprParamInfo->mJoinRightOuterInfoList) );

    /* SubQuery의 Node에 대한 Candidate Plan 생성 */
    STL_TRY( qlncMakeCandPlanForSubQueryExpr( aConvExprParamInfo->mParamInfo,
                                              aConvExprParamInfo->mInitExprList->mStmtExprList,
                                              aConvExprParamInfo->mTableStatList,
                                              sCandSubQuery->mNode,
                                              &sTmpJoinOuterInfoList,
                                              aConvExprParamInfo->mPlanNodeMapDynArr,
                                              &sCandPlan,
                                              aEnv )
             == STL_SUCCESS );

    /* Init Node 설정 */
    sInitSubQuery->mInitNode = sCandPlan->mInitNode;

    /* Cand Plan 설정 */
    sInitSubQuery->mCandPlan = (void*)sCandPlan;    

    if( sCandPlan->mPlanType == QLNC_PLAN_TYPE_QUERY_SPEC )
    {
        sInitSubQuery->mTargetCount = ((qlncPlanQuerySpec *) sCandPlan)->mTargetCount;    
        sInitSubQuery->mTargetList  = ((qlncPlanQuerySpec *) sCandPlan)->mTargetColList;
    }
    else
    {
        STL_DASSERT( sCandPlan->mPlanType == QLNC_PLAN_TYPE_QUERY_SET );
        sInitSubQuery->mTargetCount = ((qlncPlanQuerySet *) sCandPlan)->mTargetCount;    
        sInitSubQuery->mTargetList  = ((qlncPlanQuerySet *) sCandPlan)->mTargetList;
    }

    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitTarget ) * sInitSubQuery->mTargetCount,
                                (void **) & sInitSubQuery->mTargets,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sInitSubQuery->mTargets,
               0x00,
               STL_SIZEOF( qlvInitTarget ) * sInitSubQuery->mTargetCount );

    STL_TRY( qlvCreateRefExprList( & sInitSubQuery->mRefColList,
                                   aConvExprParamInfo->mRegionMem,
                                   aEnv )
             == STL_SUCCESS );


    /* Set Sub Query Type */
    if( sCandSubQuery->mRefExprList == NULL )
    {
        sInitSubQuery->mSubQueryType = QLV_SUB_QUERY_N_TYPE;
    }
    else
    {
        sInitSubQuery->mSubQueryType = QLV_SUB_QUERY_J_TYPE;
    }

    /* Set Sub Query Category */
    if( sCandSubQuery->mIsRelSubQuery == STL_TRUE )
    {
        sInitSubQuery->mCategory = QLV_SUB_QUERY_TYPE_RELATION;
    }
    else
    {
        if( sInitSubQuery->mTargetCount == 1 )
        {
            sInitSubQuery->mCategory = QLV_SUB_QUERY_TYPE_SCALAR;
        }
        else
        {
            sInitSubQuery->mCategory = QLV_SUB_QUERY_TYPE_ROW;
        }
    }

    /* Set Related Func ID */
    sInitSubQuery->mRelatedFuncId = sCandSubQuery->mRelatedFuncId;

    /* Get Iteration Time */
    if( sInitSubQuery->mSubQueryType == QLV_SUB_QUERY_N_TYPE )
    {
        switch( sInitSubQuery->mRelatedFuncId )
        {
            case KNL_BUILTIN_FUNC_EXISTS :
            case KNL_BUILTIN_FUNC_NOT_EXISTS :
                sIterationTime = DTL_ITERATION_TIME_FOR_EACH_STATEMENT;
                break;
                
            default :
                if( sInitSubQuery->mCategory == QLV_SUB_QUERY_TYPE_RELATION )
                {
                    sIterationTime = DTL_ITERATION_TIME_FOR_EACH_QUERY;
                }
                else
                {
                    sIterationTime = DTL_ITERATION_TIME_FOR_EACH_STATEMENT;
                }
                break;
        }
    }
    else
    {
        sIterationTime = DTL_ITERATION_TIME_FOR_EACH_QUERY;
    }

    /* Set Target */
    sExprItem = sInitSubQuery->mTargetList->mHead;
    for( sLoop = 0 ; sLoop < sInitSubQuery->mTargetCount ; sLoop++ )
    {
        STL_DASSERT( sExprItem != NULL );
        
        sInitSubQuery->mTargets[ sLoop ].mDisplayName = NULL;
        sInitSubQuery->mTargets[ sLoop ].mDisplayPos = 0;
        sInitSubQuery->mTargets[ sLoop ].mAliasName = NULL;
        sInitSubQuery->mTargets[ sLoop ].mExpr = sExprItem->mExprPtr;

        sInitSubQuery->mTargets[ sLoop ].mDataTypeInfo = 
            sCandSubQuery->mTargetArr[ sLoop ].mDataTypeInfo;

        /* reference expression 구성 */
        sTempExpr = sExprItem->mExprPtr;
        STL_TRY( qlvAddReferenceExpr( & sTempExpr,
                                      aConvExprParamInfo->mRegionMem,
                                      aEnv )
                 == STL_SUCCESS );

        sTempExpr->mIterationTime = sIterationTime;
        
        /* reference expression list에 추가 */
        STL_TRY( qlvAddExprToRefExprList( sInitSubQuery->mRefColList,
                                          sTempExpr,
                                          STL_FALSE,
                                          aConvExprParamInfo->mRegionMem,
                                          aEnv )
                 == STL_SUCCESS );                                          

        sExprItem = sExprItem->mNext;
    }

    /* SubQuery Init Expression 할당 */
    QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                               QLV_EXPR_TYPE_SUB_QUERY,
                               KNL_BUILTIN_FUNC_INVALID,
                               sIterationTime,
                               sCandSubQuery->mCommon.mPosition,
                               sCandSubQuery->mCommon.mExprPhraseFlag,
                               0,
                               sCandSubQuery->mCommon.mColumnName,
                               sInitSubQuery,
                               sInitExpr,
                               aEnv );

    STL_DASSERT( aConvExprParamInfo->mSubQueryExprList != NULL );

    STL_TRY( qlvAddExprToRefExprList( aConvExprParamInfo->mSubQueryExprList,
                                      sInitExpr,
                                      STL_FALSE,
                                      aConvExprParamInfo->mRegionMem,
                                      aEnv )
             == STL_SUCCESS );
    
    
    /**
     * SubQuery Target이 한 개인 경우 Reference Expression을 반환
     */

    if( sInitSubQuery->mTargetCount == 1 )
    {
        sInitExpr = sInitSubQuery->mRefColList->mHead->mExprPtr;
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Reference Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprReference( qlncConvExprParamInfo * aConvExprParamInfo,
                                                  qllEnv                * aEnv )
{
#if 0
    qlncExprReference   * sCandRefExpr  = NULL;
    qlvInitExpression   * sInitExpr     = NULL;
    qlvInitRefExpr      * sInitRefExpr  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_REFERENCE,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Candidate Reference Expr Result Expression 설정 */
    sCandRefExpr = (qlncExprReference*)(aConvExprParamInfo->mCandExpr);

    /* Reference Expression 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitRefExpr ),
                                (void**) &sInitRefExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Original Expression 설정 */
    aConvExprParamInfo->mCandExpr = sCandRefExpr->mOrgExpr;
    STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandRefExpr->mOrgExpr->mType](
                 aConvExprParamInfo,
                 aEnv )
             == STL_SUCCESS );
    sInitRefExpr->mOrgExpr = aConvExprParamInfo->mInitExpr;

    /* Reference Expr Init Expression 할당 */
    QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                               QLV_EXPR_TYPE_REFERENCE,
                               KNL_BUILTIN_FUNC_INVALID,
                               sInitRefExpr->mOrgExpr->mIterationTime,
                               sCandRefExpr->mCommon.mPosition,
                               sCandRefExpr->mCommon.mExprPhraseFlag,
                               sInitRefExpr->mOrgExpr->mIncludeExprCnt,
                               sCandRefExpr->mCommon.mColumnName,
                               sInitRefExpr,
                               sInitExpr,
                               aEnv );


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;
#endif

    /* @todo 현재 reference는 Subquery에서 wrap 함수를 이용해서만 만들고 있다. */

    return STL_FAILURE;
}


/**
 * @brief Candidate Inner Column Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprInnerColumn( qlncConvExprParamInfo   * aConvExprParamInfo,
                                                    qllEnv                  * aEnv )
{
    qlvInitExpression   * sInitExpr     = NULL;
    qlvInitExpression   * sOrgExpr      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_INNER_COLUMN,
                                                    aEnv );
    STL_PARAM_VALIDATE( ( (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                           QLNC_NODE_TYPE_SUB_TABLE) ||
                          (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                           QLNC_NODE_TYPE_SORT_INSTANT) ||
                          (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                           QLNC_NODE_TYPE_HASH_INSTANT) ),
                        QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Read Column을 Add하고 Add된 Column의 Init Expression을 가져옴 */
    STL_TRY( qlncAddReadColumnOnCurPlan( aConvExprParamInfo,
                                         aEnv )
             == STL_SUCCESS );
    sOrgExpr = aConvExprParamInfo->mInitExpr;

    if( sOrgExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
    {
        /* Copy Expression */
        STL_TRY( qlvCopyExpr( sOrgExpr,
                              &sInitExpr,
                              aConvExprParamInfo->mRegionMem,
                              aEnv )
                 == STL_SUCCESS );

        if( aConvExprParamInfo->mCandExpr->mColumnName != NULL ) 
        {
            QLNC_ALLOC_AND_COPY_STRING( sInitExpr->mColumnName,
                                        aConvExprParamInfo->mCandExpr->mColumnName,
                                        aConvExprParamInfo->mRegionMem,
                                        aEnv );
        }
    }
    else
    {
        sInitExpr = sOrgExpr;
    }


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Outer Column Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprOuterColumn( qlncConvExprParamInfo   * aConvExprParamInfo,
                                                    qllEnv                  * aEnv )
{
#if 0
    qlncExprOuterColumn * sCandOuterColumn  = NULL;
    qlvInitExpression   * sInitExpr         = NULL;
    qlvInitOuterColumn  * sInitOuterColumn  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_OUTER_COLUMN,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Candidate Outer Column Expression 설정 */
    sCandOuterColumn = (qlncExprOuterColumn*)(aConvExprParamInfo->mCandExpr);

    /* Outer Column 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitOuterColumn ),
                                (void**) &sInitOuterColumn,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Original Expression 설정 */
    aConvExprParamInfo->mCandExpr = sCandOuterColumn->mOrgExpr;
    STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandOuterColumn->mOrgExpr->mType](
                 aConvExprParamInfo,
                 aEnv )
             == STL_SUCCESS );
    sInitOuterColumn->mOrgExpr = aConvExprParamInfo->mInitExpr;

    sInitOuterColumn->mRelationNode = 
        aConvExprParamInfo->mCurPlan->mInitNode;

    /* Outer Column Init Expression 할당 */
    QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                               QLV_EXPR_TYPE_OUTER_COLUMN,
                               KNL_BUILTIN_FUNC_INVALID,
                               DTL_ITERATION_TIME_FOR_EACH_QUERY,
                               sCandOuterColumn->mCommon.mPosition,
                               sCandOuterColumn->mCommon.mExprPhraseFlag,
                               sInitOuterColumn->mOrgExpr->mIncludeExprCnt,
                               sCandOuterColumn->mCommon.mColumnName,
                               sInitOuterColumn,
                               sInitExpr,
                               aEnv );


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;
#endif

    /* @todo 현재 Outer Column은 Add Column 함수내에서 만들고 있음 */

    return STL_FAILURE;
}


/**
 * @brief Candidate Rowid Column Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprRowidColumn( qlncConvExprParamInfo   * aConvExprParamInfo,
                                                    qllEnv                  * aEnv )
{
    qlvInitExpression   * sInitExpr     = NULL;
    qlvInitExpression   * sOrgExpr      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_ROWID_COLUMN,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Read Column을 Add하고 Add된 Column의 Init Expression을 가져옴 */
    STL_TRY( qlncAddReadColumnOnCurPlan( aConvExprParamInfo,
                                         aEnv )
             == STL_SUCCESS );
    sOrgExpr = aConvExprParamInfo->mInitExpr;

    if( sOrgExpr->mType == QLV_EXPR_TYPE_OUTER_COLUMN )
    {
        /* Outer Column인 경우 Expression을 복사하지 않는다. */
        sInitExpr = sOrgExpr;
    }
    else
    {
        if( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_MERGE_JOIN )
        {
            STL_DASSERT( (aConvExprParamInfo->mJoinLeftOuterInfoList.mCount == 0) ||
                         (aConvExprParamInfo->mJoinRightOuterInfoList.mCount == 0) );

            STL_TRY( qlvCopyExpr( sOrgExpr,
                                  &sInitExpr,
                                  aConvExprParamInfo->mRegionMem,
                                  aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* Outer Column이 아닌 경우 Expression을 Copy한다. */
            STL_TRY( qlvCopyExpr( sOrgExpr,
                                  &sInitExpr,
                                  aConvExprParamInfo->mRegionMem,
                                  aEnv )
                     == STL_SUCCESS );
        }
    }


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Aggregation Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprAggregation( qlncConvExprParamInfo   * aConvExprParamInfo,
                                                    qllEnv                  * aEnv )
{
    stlInt32              i;
    stlInt32              sIncludeExprCnt;
    qlncExprAggregation * sCandAggregation  = NULL;
    qlvInitExpression   * sInitExpr         = NULL;
    qlvInitAggregation  * sInitAggregation  = NULL;

    
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_AGGREGATION,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Candidate Aggregation Expression 설정 */
    sCandAggregation = (qlncExprAggregation*)(aConvExprParamInfo->mCandExpr);

    /* Aggregation 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitAggregation ),
                                (void**) &sInitAggregation,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Aggregation ID 설정 */
    sInitAggregation->mAggrId = sCandAggregation->mAggrId;

    /* Argument Count 설정 */
    sInitAggregation->mArgCount = sCandAggregation->mArgCount;

    /* Argument 설정 */
    sIncludeExprCnt = 0;
    if( sCandAggregation->mArgCount > 0 )
    {
        /* Argument Point Array 할당 */
        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitExpression* ) * sCandAggregation->mArgCount,
                                    (void**) &(sInitAggregation->mArgs),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* 각 Argument 설정 */
        for( i = 0; i < sCandAggregation->mArgCount; i++ )
        {
            aConvExprParamInfo->mCandExpr = sCandAggregation->mArgs[i];
            STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandAggregation->mArgs[i]->mType](
                         aConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );
            sInitAggregation->mArgs[i] = aConvExprParamInfo->mInitExpr;

            /* Include Expression Count 설정 */
            sIncludeExprCnt += sInitAggregation->mArgs[i]->mIncludeExprCnt;
        }
    }
    else
    {
        sInitAggregation->mArgs = NULL;
    }

    /* Distinct 여부 설정 */
    sInitAggregation->mIsDistinct = sCandAggregation->mIsDistinct;

    /* Nested Aggregation 포함 여부 설정 */
    sInitAggregation->mHasNestedAggr = sCandAggregation->mHasNestedAggr;

    /* Filter 설정 */
    if( sCandAggregation->mFilter != NULL )
    {
        /* @todo 현재 Aggregation은 Filter를 갖지 않음 */

        aConvExprParamInfo->mCandExpr = sCandAggregation->mFilter;
        STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandAggregation->mFilter->mType](
                     aConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );
        sInitAggregation->mFilter = aConvExprParamInfo->mInitExpr;

        /* Include Expression Count 설정 */
        sIncludeExprCnt += sInitAggregation->mFilter->mIncludeExprCnt;
    }
    else
    {
        sInitAggregation->mFilter = NULL;
    }

    /* Aggregation Init Expression 할당 */
    QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                               QLV_EXPR_TYPE_AGGREGATION,
                               KNL_BUILTIN_FUNC_INVALID,
                               sCandAggregation->mCommon.mIterationTime,
                               sCandAggregation->mCommon.mPosition,
                               sCandAggregation->mCommon.mExprPhraseFlag,
                               sIncludeExprCnt,
                               sCandAggregation->mCommon.mColumnName,
                               sInitAggregation,
                               sInitExpr,
                               aEnv );

    /* Init Expression List 설정 */
    if( sCandAggregation->mHasNestedAggr == STL_TRUE )
    {
        if( aConvExprParamInfo->mInitExprList->mNestedAggrExprList != NULL )
        {
            STL_TRY( qlvAddExprToRefExprList( aConvExprParamInfo->mInitExprList->mNestedAggrExprList,
                                              sInitExpr,
                                              STL_FALSE,
                                              aConvExprParamInfo->mRegionMem,
                                              aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        if( aConvExprParamInfo->mInitExprList->mAggrExprList != NULL )
        {
            STL_TRY( qlvAddExprToRefExprList( aConvExprParamInfo->mInitExprList->mAggrExprList,
                                              sInitExpr,
                                              STL_FALSE,
                                              aConvExprParamInfo->mRegionMem,
                                              aEnv )
                     == STL_SUCCESS );
        }
    }


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Case Expr Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprCaseExpr( qlncConvExprParamInfo  * aConvExprParamInfo,
                                                 qllEnv                 * aEnv )
{
    stlInt32              i;
    stlInt32              sIncludeExprCnt;
    qlncExprCaseExpr    * sCandCaseExpr  = NULL;
    qlvInitExpression   * sInitExpr      = NULL;
    qlvInitCaseExpr     * sInitCaseExpr  = NULL;
    dtlIterationTime      sIterationTime = DTL_ITERATION_TIME_INVALID;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_CASE_EXPR,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Candidate Case Expr Expression 설정 */
    sCandCaseExpr = (qlncExprCaseExpr*)(aConvExprParamInfo->mCandExpr);

    /* Case Expr 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitCaseExpr ),
                                (void**) &sInitCaseExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Function ID 설정 */
    sInitCaseExpr->mFuncId = sCandCaseExpr->mFuncId;

    /* Count 설정 */
    sInitCaseExpr->mCount = sCandCaseExpr->mCount;
    STL_DASSERT( sCandCaseExpr->mCount > 0 );

    /* When Expr Point Array 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitExpression* ) * sCandCaseExpr->mCount,
                                (void**)&(sInitCaseExpr->mWhenArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Then Expr Point Array 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitExpression* ) * sCandCaseExpr->mCount,
                                (void**)&(sInitCaseExpr->mThenArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sIncludeExprCnt = 0;
    for( i = 0; i < sCandCaseExpr->mCount; i++ )
    {
        /* When Expr 설정 */
        aConvExprParamInfo->mCandExpr = sCandCaseExpr->mWhenArr[i];
        STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandCaseExpr->mWhenArr[i]->mType](
                     aConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );
        sInitCaseExpr->mWhenArr[i] = aConvExprParamInfo->mInitExpr;

        /* Argument의 Related Function ID 변경 */
        sInitCaseExpr->mWhenArr[i]->mRelatedFuncId = KNL_BUILTIN_FUNC_CASE;

        /* Include Expression Count 설정 */
        sIncludeExprCnt += sInitCaseExpr->mWhenArr[i]->mIncludeExprCnt;

        /* Iteration Time 설정 */
        sIterationTime = ( sIterationTime < sInitCaseExpr->mWhenArr[i]->mIterationTime )
            ? sIterationTime : sInitCaseExpr->mWhenArr[i]->mIterationTime;

        /* Then Expr 설정 */
        aConvExprParamInfo->mCandExpr = sCandCaseExpr->mThenArr[i];
        STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandCaseExpr->mThenArr[i]->mType](
                     aConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );
        sInitCaseExpr->mThenArr[i] = aConvExprParamInfo->mInitExpr;

        /* Argument의 Related Function ID 변경 */
        sInitCaseExpr->mThenArr[i]->mRelatedFuncId = KNL_BUILTIN_FUNC_CASE;

        /* Include Expression Count 설정 */
        sIncludeExprCnt += sInitCaseExpr->mThenArr[i]->mIncludeExprCnt;

        /* Iteration Time 설정 */
        sIterationTime = ( sIterationTime < sInitCaseExpr->mThenArr[i]->mIterationTime )
            ? sIterationTime : sInitCaseExpr->mThenArr[i]->mIterationTime;
    }

    /* Default Result 설정 */
    STL_DASSERT( sCandCaseExpr->mDefResult != NULL );

    aConvExprParamInfo->mCandExpr = sCandCaseExpr->mDefResult;
    STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandCaseExpr->mDefResult->mType](
                 aConvExprParamInfo,
                 aEnv )
             == STL_SUCCESS );
    sInitCaseExpr->mDefResult = aConvExprParamInfo->mInitExpr;

    /* Argument의 Related Function ID 변경 */
    sInitCaseExpr->mDefResult->mRelatedFuncId = KNL_BUILTIN_FUNC_CASE;

    /* Include Expression Count 설정 */
    sIncludeExprCnt += sInitCaseExpr->mDefResult->mIncludeExprCnt;

    /* Iteration Time 설정 */
    sIterationTime = ( sIterationTime < sInitCaseExpr->mDefResult->mIterationTime )
        ? sIterationTime : sInitCaseExpr->mDefResult->mIterationTime;

    /* Case Expr Init Expression 할당 */
    QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                               QLV_EXPR_TYPE_CASE_EXPR,
                               KNL_BUILTIN_FUNC_INVALID,
                               sIterationTime,
                               sCandCaseExpr->mCommon.mPosition,
                               sCandCaseExpr->mCommon.mExprPhraseFlag,
                               sIncludeExprCnt,
                               sCandCaseExpr->mCommon.mColumnName,
                               sInitCaseExpr,
                               sInitExpr,
                               aEnv );


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate List Function Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprListFunction( qlncConvExprParamInfo  * aConvExprParamInfo,
                                                     qllEnv                 * aEnv )
{
    stlInt32              i;
    stlInt32              sIncludeExprCnt;
    qlncExprListFunc    * sCandListFunc  = NULL;
    qlncExprRowExpr     * sCandRowExpr   = NULL;
    qlvInitExpression   * sInitExpr      = NULL;
    qlvInitListFunc     * sInitListFunc  = NULL;
    qlvInitRowExpr      * sInitRowExpr   = NULL;
    dtlIterationTime      sIterationTime = DTL_ITERATION_TIME_INVALID;
    
    
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_LIST_FUNCTION,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Candidate List Func Expression 설정 */
    sCandListFunc = (qlncExprListFunc*)(aConvExprParamInfo->mCandExpr);

    /* List Function 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitListFunc ),
                                (void**) &sInitListFunc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Function ID 설정 */
    sInitListFunc->mFuncId = sCandListFunc->mFuncId;

    /* List Function Type 설정 */
    sInitListFunc->mListFuncType = sCandListFunc->mListFuncType;

    /* Argument Count 설정 */
    sInitListFunc->mArgCount = sCandListFunc->mArgCount;
    STL_DASSERT( sCandListFunc->mArgCount > 0 );

    /* Argument Pointer Array 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitExpression* ) * sCandListFunc->mArgCount,
                                (void**) &(sInitListFunc->mArgs),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Argument 설정 */
    sIncludeExprCnt = 0;
    for( i = 0; i < sCandListFunc->mArgCount; i++ )
    {
        aConvExprParamInfo->mCandExpr = sCandListFunc->mArgs[i];
        STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandListFunc->mArgs[i]->mType](
                     aConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );
        sInitListFunc->mArgs[i] = aConvExprParamInfo->mInitExpr;

        /* Argument의 Related Function ID 변경 */
        sInitListFunc->mArgs[i]->mRelatedFuncId = sCandListFunc->mFuncId;

        /* Include Expression Count 설정 */
        sIncludeExprCnt += sInitListFunc->mArgs[i]->mIncludeExprCnt;

        /* Iteration Time 설정 */
        sIterationTime = ( sIterationTime < sInitListFunc->mArgs[i]->mIterationTime )
            ? sIterationTime : sInitListFunc->mArgs[i]->mIterationTime;
    }

    /* List Function Init Expression 할당 */
    QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                               QLV_EXPR_TYPE_LIST_FUNCTION,
                               KNL_BUILTIN_FUNC_INVALID,
                               sIterationTime,
                               sCandListFunc->mCommon.mPosition,
                               sCandListFunc->mCommon.mExprPhraseFlag,
                               sIncludeExprCnt,
                               sCandListFunc->mCommon.mColumnName,
                               sInitListFunc,
                               sInitExpr,
                               aEnv );

    /**
     * Right Operand가 Sub Query를 포함하는 경우,
     * Sub Query Expression List에 Sub Query Function을 추가하고
     * Sub Query Function을 Reference Expression으로 감싼다.
     */
    
    sCandRowExpr = (qlncExprRowExpr*)(((qlncExprListCol*)sCandListFunc->mArgs[0])->mArgs[0]);

    if( ( sCandRowExpr->mArgCount == 1 ) &&
        ( sCandRowExpr->mArgs[0]->mType == QLNC_EXPR_TYPE_SUB_QUERY ) )
    {
        sInitRowExpr = sInitListFunc->mArgs[0]->mExpr.mListCol->mArgs[0]->mExpr.mRowExpr;
            
        switch( sCandListFunc->mFuncId )
        {
            case KNL_BUILTIN_FUNC_EXISTS :
            case KNL_BUILTIN_FUNC_NOT_EXISTS :
                sIterationTime = sInitRowExpr->mSubQueryExpr->mIterationTime;
                break;
            default :
                if( sInitListFunc->mArgs[1]->mIterationTime >=
                    DTL_ITERATION_TIME_FOR_EACH_STATEMENT )
                {
                    sIterationTime = DTL_ITERATION_TIME_FOR_EACH_STATEMENT;
                }
                else
                {
                    sIterationTime = DTL_ITERATION_TIME_FOR_EACH_QUERY;
                }
                break;
        }
        
        if( ((qlncExprSubQuery *)sCandRowExpr->mArgs[0])->mIsRelSubQuery == STL_TRUE )
        {
            STL_DASSERT( aConvExprParamInfo->mSubQueryExprList != NULL );
            
            STL_TRY( qlvAddReferenceExpr( & sInitExpr,
                                          aConvExprParamInfo->mRegionMem,
                                          aEnv )
                     == STL_SUCCESS );

            sInitExpr->mIterationTime = sIterationTime;

            STL_TRY( qlvAddExprToRefExprList( aConvExprParamInfo->mSubQueryExprList,
                                              sInitExpr,
                                              STL_FALSE,
                                              aConvExprParamInfo->mRegionMem,
                                              aEnv )
                     == STL_SUCCESS );
        }
    }

    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate List Column Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprListColumn( qlncConvExprParamInfo    * aConvExprParamInfo,
                                                   qllEnv                   * aEnv )
{
    stlInt32              i;
    stlInt32              sIncludeExprCnt;
    qlncExprListCol     * sCandListCol   = NULL;
    qlvInitExpression   * sInitExpr      = NULL;
    qlvInitListCol      * sInitListCol   = NULL;
    dtlIterationTime      sIterationTime = DTL_ITERATION_TIME_INVALID;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_LIST_COLUMN,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Candidate List Column Expression 설정 */
    sCandListCol = (qlncExprListCol*)(aConvExprParamInfo->mCandExpr);

    /* List Column 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitListCol ),
                                (void**) &sInitListCol,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* List Predicate 설정 */
    sInitListCol->mPredicate = sCandListCol->mPredicate;

    /* Argument Count 설정 */
    sInitListCol->mArgCount = sCandListCol->mArgCount;
    STL_DASSERT( sCandListCol->mArgCount > 0 );

    /* Argument Pointer Array 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitExpression* ) * sCandListCol->mArgCount,
                                (void**) &(sInitListCol->mArgs),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Entry Pointer Array 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( knlListEntry* ) * sCandListCol->mArgCount,
                                (void**) &(sInitListCol->mEntry),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Argument 및 Entry 설정 */
    sIncludeExprCnt = 0;
    for( i = 0; i < sCandListCol->mArgCount; i++ )
    {
        /* Argument 설정 */
        aConvExprParamInfo->mCandExpr = sCandListCol->mArgs[i];
        STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandListCol->mArgs[i]->mType](
                     aConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );
        sInitListCol->mArgs[i] = aConvExprParamInfo->mInitExpr;

        /* Entry 설정 */
        sInitListCol->mEntry[i] = sCandListCol->mEntry[i];

        /* Include Expression Count 설정 */
        sIncludeExprCnt += sInitListCol->mArgs[i]->mIncludeExprCnt;

        /* Iteration Time 설정 */
        sIterationTime = ( sIterationTime < sInitListCol->mArgs[i]->mIterationTime )
            ? sIterationTime : sInitListCol->mArgs[i]->mIterationTime;
    }

    /* List Column Init Expression 할당 */
    QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                               QLV_EXPR_TYPE_LIST_COLUMN,
                               KNL_BUILTIN_FUNC_INVALID,
                               sIterationTime,
                               sCandListCol->mCommon.mPosition,
                               sCandListCol->mCommon.mExprPhraseFlag,
                               sIncludeExprCnt,
                               sCandListCol->mCommon.mColumnName,
                               sInitListCol,
                               sInitExpr,
                               aEnv );


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Row Expr Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprRowExpr( qlncConvExprParamInfo   * aConvExprParamInfo,
                                                qllEnv                  * aEnv )
{
    stlInt32              i;
    stlInt32              sIncludeExprCnt;
    qlncExprRowExpr     * sCandRowExpr      = NULL;
    qlvInitExpression   * sInitExpr         = NULL;
    qlvInitRowExpr      * sInitRowExpr      = NULL;
    qlvInitSubQuery     * sInitSubQueryExpr = NULL;
    qlvRefExprItem      * sExprItem         = NULL;
    qlvRefExprList      * sSubQueryExprList = NULL;
    qlvRefExprList      * sBackupExprList   = NULL;
    dtlIterationTime      sIterationTime = DTL_ITERATION_TIME_INVALID;

    
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_ROW_EXPR,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Candidate Row Expr Expression 설정 */
    sCandRowExpr = (qlncExprRowExpr*)(aConvExprParamInfo->mCandExpr);

    /* Row Expression 할당 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitRowExpr ),
                                (void**) &sInitRowExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_DASSERT( sCandRowExpr->mArgCount > 0 );

    /* Argument 및 Entry 설정 */
    sIncludeExprCnt = 0;

    if( ( sCandRowExpr->mArgCount == 1 ) &&
        ( sCandRowExpr->mArgs[0]->mType == QLNC_EXPR_TYPE_SUB_QUERY ) )
    {
        /* Argument Count 설정 */
        sInitRowExpr->mArgCount = ((qlncExprSubQuery *)sCandRowExpr->mArgs[0])->mTargetCount;

        /* Argument Pointer Array 할당 */
        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitExpression* ) * sInitRowExpr->mArgCount,
                                    (void**) &(sInitRowExpr->mArgs),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Entry 할당 */
        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                    STL_SIZEOF( knlListEntry ) * sInitRowExpr->mArgCount,
                                    (void**) &(sInitRowExpr->mEntry),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Sub Query List 설정 */
        if( ((qlncExprSubQuery *)sCandRowExpr->mArgs[0])->mIsRelSubQuery == STL_TRUE )
        {
            STL_TRY( qlvCreateRefExprList( & sSubQueryExprList,
                                           aConvExprParamInfo->mRegionMem,
                                           aEnv )
                     == STL_SUCCESS );

            sBackupExprList = aConvExprParamInfo->mSubQueryExprList;
            aConvExprParamInfo->mSubQueryExprList = sSubQueryExprList;
        }

        /* Argument 설정 */
        aConvExprParamInfo->mCandExpr = sCandRowExpr->mArgs[0];
        STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandRowExpr->mArgs[0]->mType](
                     aConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );

        /* Iteration Time 설정 */
        sIterationTime = aConvExprParamInfo->mInitExpr->mIterationTime;
            
        if( sInitRowExpr->mArgCount > 1 )
        {
            STL_DASSERT( aConvExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_SUB_QUERY );
            
            sInitSubQueryExpr = aConvExprParamInfo->mInitExpr->mExpr.mSubQuery;
            sExprItem = sInitSubQueryExpr->mRefColList->mHead;

            for( i = 0; i < sInitSubQueryExpr->mTargetCount; i++ )
            {
                sInitRowExpr->mArgs[i] = sExprItem->mExprPtr;

                /* Entry 설정 */
                sInitRowExpr->mEntry[i] = sCandRowExpr->mEntry[0];

                /* Include Expression Count 설정 */
                sIncludeExprCnt += sInitRowExpr->mArgs[i]->mIncludeExprCnt;

                sExprItem = sExprItem->mNext;
            }
        }
        else
        {
            STL_DASSERT( sInitRowExpr->mArgCount == 1 );
            STL_DASSERT( aConvExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_REFERENCE );

            /* sInitRowExpr->mArgs[0] = aConvExprParamInfo->mInitExpr->mExpr.mReference->mOrgExpr; */
            sInitRowExpr->mArgs[0] = aConvExprParamInfo->mInitExpr;

            /* Entry 설정 */
            sInitRowExpr->mEntry[0] = sCandRowExpr->mEntry[0];
            
            /* Include Expression Count 설정 */
            sIncludeExprCnt += sInitRowExpr->mArgs[0]->mIncludeExprCnt;
        }
        
        if( ((qlncExprSubQuery *)sCandRowExpr->mArgs[0])->mIsRelSubQuery == STL_TRUE )
        {
            STL_DASSERT( sSubQueryExprList->mCount == 1 );
            
            sInitRowExpr->mSubQueryExpr = sSubQueryExprList->mHead->mExprPtr;
            aConvExprParamInfo->mSubQueryExprList = sBackupExprList;
        }
        else
        {
            sInitRowExpr->mSubQueryExpr = NULL;
        }
    }
    else
    {
        /* Argument Count 설정 */
        sInitRowExpr->mArgCount = sCandRowExpr->mArgCount;

        /* Argument Pointer Array 할당 */
        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitExpression* ) * sInitRowExpr->mArgCount,
                                    (void**) &(sInitRowExpr->mArgs),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Entry 할당 */
        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                    STL_SIZEOF( knlListEntry ) * sInitRowExpr->mArgCount,
                                    (void**) &(sInitRowExpr->mEntry),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        for( i = 0; i < sInitRowExpr->mArgCount; i++ )
        {
            /* Argument 설정 */
            aConvExprParamInfo->mCandExpr = sCandRowExpr->mArgs[i];
            STL_TRY( qlncConvertCandExprToInitExprFuncList[sCandRowExpr->mArgs[i]->mType](
                         aConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );
            sInitRowExpr->mArgs[i] = aConvExprParamInfo->mInitExpr;

            /* Entry 설정 */
            sInitRowExpr->mEntry[i] = sCandRowExpr->mEntry[i];

            /* Include Expression Count 설정 */
            sIncludeExprCnt += sInitRowExpr->mArgs[i]->mIncludeExprCnt;

            /* Iteration Time 설정 */
            sIterationTime = ( sIterationTime < sInitRowExpr->mArgs[i]->mIterationTime )
                ? sIterationTime : sInitRowExpr->mArgs[i]->mIterationTime;
        }

        sInitRowExpr->mSubQueryExpr = NULL;
    }
    
    /* Row Expr Init Expression 할당 */
    QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                               QLV_EXPR_TYPE_ROW_EXPR,
                               KNL_BUILTIN_FUNC_INVALID,
                               sIterationTime,
                               sCandRowExpr->mCommon.mPosition,
                               sCandRowExpr->mCommon.mExprPhraseFlag,
                               sIncludeExprCnt,
                               sCandRowExpr->mCommon.mColumnName,
                               sInitRowExpr,
                               sInitExpr,
                               aEnv );

    
    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate And Filter Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprAndFilter( qlncConvExprParamInfo * aConvExprParamInfo,
                                                  qllEnv                * aEnv )
{
    stlInt32                  i;
    stlInt32                  sExprCount         = 0;
    qlvInitExpression      ** sExprArr           = NULL;
    qlncAndFilter           * sAndFilter         = NULL;
    qlvInitExpression       * sAndExpr           = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_AND_FILTER,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* And Filter 설정 */
    sAndFilter = (qlncAndFilter*)(aConvExprParamInfo->mCandExpr);

    STL_DASSERT( sAndFilter->mOrCount > 1 );

    /* And Function Argument Expression Array 생성 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitExpression* ) * sAndFilter->mOrCount,
                                (void**) &sExprArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Function Argument 설정 */
    for( i = 0; i < sAndFilter->mOrCount; i++ )
    {
        aConvExprParamInfo->mCandExpr = (qlncExprCommon*)(sAndFilter->mOrFilters[i]);
        STL_TRY( qlncConvertCandExprToInitExprOrFilter( aConvExprParamInfo,
                                                        aEnv )
                 == STL_SUCCESS );

        /* SubQuery를 갖지 않는 Filter */
        sExprArr[sExprCount] = aConvExprParamInfo->mInitExpr;
        sExprCount++;
    }

    /* And Function Init Expression 생성 */
    STL_DASSERT( sExprCount > 1 );
    STL_TRY( qlvMakeFuncAndNArg( aConvExprParamInfo->mRegionMem,
                                 0,
                                 sExprCount,
                                 sExprArr,
                                 &sAndExpr,
                                 aEnv )
             == STL_SUCCESS );


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sAndExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Or Filter Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprOrFilter( qlncConvExprParamInfo  * aConvExprParamInfo,
                                                 qllEnv                 * aEnv )
{
    stlInt32              i;
    qlncOrFilter        * sOrFilter = NULL;
    qlvInitExpression   * sOrExpr   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_OR_FILTER,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Or Filter 설정 */
    sOrFilter = (qlncOrFilter*)(aConvExprParamInfo->mCandExpr);

    if( sOrFilter->mFilterCount == 1 )
    {
        aConvExprParamInfo->mCandExpr = (qlncExprCommon*)(sOrFilter->mFilters[0]);
        STL_TRY( qlncConvertCandExprToInitExprFuncList[sOrFilter->mFilters[0]->mType](
                     aConvExprParamInfo,
                     aEnv )
                 == STL_SUCCESS );
        sOrExpr = aConvExprParamInfo->mInitExpr;
    }
    else
    {
        qlvInitExpression  ** sExprArr  = NULL;

        STL_DASSERT( sOrFilter->mFilterCount > 1 );

        /* Or Function Argument Expression Array 생성 */
        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitExpression* ) * sOrFilter->mFilterCount,
                                    (void**) &sExprArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Function Argument 설정 */
        for( i = 0; i < sOrFilter->mFilterCount; i++ )
        {
            aConvExprParamInfo->mCandExpr = (qlncExprCommon*)(sOrFilter->mFilters[i]);
            STL_TRY( qlncConvertCandExprToInitExprFuncList[sOrFilter->mFilters[i]->mType](
                         aConvExprParamInfo,
                         aEnv )
                     == STL_SUCCESS );
            sExprArr[i] = aConvExprParamInfo->mInitExpr;
        }

        /* Or Function Init Expression 생성 */
        STL_TRY( qlvMakeFuncOrNArg( aConvExprParamInfo->mRegionMem,
                                    0,
                                    sOrFilter->mFilterCount,
                                    sExprArr,
                                    &sOrExpr,
                                    aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sOrExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Boolean Filter Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprBooleanFilter( qlncConvExprParamInfo * aConvExprParamInfo,
                                                      qllEnv                * aEnv )
{
    qlncBooleanFilter   * sBooleanFilter    = NULL;
    qlvInitExpression   * sInitExpr         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_BOOLEAN_FILTER,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Boolean Filter 설정 */
    sBooleanFilter = (qlncBooleanFilter*)(aConvExprParamInfo->mCandExpr);

    aConvExprParamInfo->mCandExpr = sBooleanFilter->mExpr;
    STL_TRY( qlncConvertCandExprToInitExprFuncList[sBooleanFilter->mExpr->mType](
                 aConvExprParamInfo,
                 aEnv )
             == STL_SUCCESS );

    STL_DASSERT( aConvExprParamInfo->mInitExpr != NULL );
    if( (sBooleanFilter->mExpr->mType == QLNC_EXPR_TYPE_AND_FILTER) &&
        (aConvExprParamInfo->mSubQueryAndFilter != NULL) )
    {
        stlInt32              sCount;
        qlvInitExpression  ** sExprArr  = NULL;

        /* @todo Boolean Filter의 Expr이 And Filter인 경우가 없을 것으로 판단됨 */

        /* Init Expr과 SubQuery Expr List의 Expression을 합친다.*/
        if( (aConvExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_FUNCTION) &&
            (aConvExprParamInfo->mInitExpr->mExpr.mFunction->mFuncId == KNL_BUILTIN_FUNC_AND) )
        {
            sCount =
                aConvExprParamInfo->mInitExpr->mExpr.mFunction->mArgCount +
                aConvExprParamInfo->mSubQueryAndFilter->mAndFilterCnt;

            /* And Function Argument Expression Array 생성 */
            STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitExpression* ) * sCount,
                                        (void**) &sExprArr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemcpy( sExprArr,
                       aConvExprParamInfo->mInitExpr->mExpr.mFunction->mArgs,
                       STL_SIZEOF( qlvInitExpression* ) * aConvExprParamInfo->mInitExpr->mExpr.mFunction->mArgCount );

            stlMemcpy( sExprArr + aConvExprParamInfo->mInitExpr->mExpr.mFunction->mArgCount,
                       aConvExprParamInfo->mSubQueryAndFilter->mInitSubQueryFilter,
                       STL_SIZEOF( qlvInitExpression* ) * aConvExprParamInfo->mSubQueryAndFilter->mAndFilterCnt );
        }
        else
        {
            sCount =
                aConvExprParamInfo->mSubQueryAndFilter->mAndFilterCnt + 1;

            /* And Function Argument Expression Array 생성 */
            STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitExpression* ) * sCount,
                                        (void**) &sExprArr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sExprArr[0] = aConvExprParamInfo->mInitExpr;

            stlMemcpy( sExprArr + 1,
                       aConvExprParamInfo->mSubQueryAndFilter->mInitSubQueryFilter,
                       STL_SIZEOF( qlvInitExpression* ) * aConvExprParamInfo->mSubQueryAndFilter->mAndFilterCnt );
        }

        /* And Function Init Expression 생성 */
        STL_TRY( qlvMakeFuncAndNArg( aConvExprParamInfo->mRegionMem,
                                     0,
                                     sCount,
                                     sExprArr,
                                     &sInitExpr,
                                     aEnv )
                 == STL_SUCCESS );

        aConvExprParamInfo->mSubQueryAndFilter = NULL;
    }
    else
    {
        sInitExpr = aConvExprParamInfo->mInitExpr;
    }


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Const Boolean Filter Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprConstBooleanFilter( qlncConvExprParamInfo    * aConvExprParamInfo,
                                                           qllEnv                   * aEnv )
{
    qlncConstBooleanFilter  * sConstBooleanFilter   = NULL;
    qlvInitExpression       * sInitExpr             = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* Const Boolean Filter 설정 */
    sConstBooleanFilter = (qlncConstBooleanFilter*)(aConvExprParamInfo->mCandExpr);

    aConvExprParamInfo->mCandExpr = sConstBooleanFilter->mExpr;
    STL_TRY( qlncConvertCandExprToInitExprFuncList[sConstBooleanFilter->mExpr->mType](
                 aConvExprParamInfo,
                 aEnv )
             == STL_SUCCESS );
    sInitExpr = aConvExprParamInfo->mInitExpr;


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 최상위 Candidate And Filter Expression을 Init Expression으로 변경한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncConvertCandExprToInitExprRootAndFilter( qlncConvExprParamInfo * aConvExprParamInfo,
                                                      qllEnv                * aEnv )
{
    stlInt32                  i;
    qlncAndFilter           * sAndFilter         = NULL;
    qlvInitExpression       * sAndExpr           = NULL;
    qlncSubQueryAndFilter   * sSubQueryAndFilter = NULL;
    qlvInitExpression      ** sSubQueryExprArr   = NULL;
    qlvRefExprList         ** sSubQueryListArr   = NULL;
    stlInt32                  sSubQueryExprCount = 0;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_AND_FILTER,
                                                    aEnv );


    /****************************************************************
     * Cand Expr -> Init Expr 변환
     ****************************************************************/

    /* And Filter 설정 */
    sAndFilter = (qlncAndFilter*)(aConvExprParamInfo->mCandExpr);

    /* And Function Argument Expression Array for SubQuery 생성 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvInitExpression* ) * sAndFilter->mOrCount,
                                (void**) &sSubQueryExprArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* And Function Argument Expression Array for SubQuery List 생성 */
    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                STL_SIZEOF( qlvRefExprList* ) * sAndFilter->mOrCount,
                                (void**) &sSubQueryListArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    if( sAndFilter->mOrCount == 1 )
    {
        if( aConvExprParamInfo->mSubQueryExprList == NULL )
        {
            aConvExprParamInfo->mCandExpr = (qlncExprCommon*)(sAndFilter->mOrFilters[0]);
            STL_TRY( qlncConvertCandExprToInitExprOrFilter( aConvExprParamInfo,
                                                            aEnv )
                     == STL_SUCCESS );

            /* SubQuery를 갖지 않는 Filter */
            sAndExpr = aConvExprParamInfo->mInitExpr;
        }
        else
        {
            aConvExprParamInfo->mCandExpr = (qlncExprCommon*)(sAndFilter->mOrFilters[0]);
            STL_TRY( qlncConvertCandExprToInitExprOrFilter( aConvExprParamInfo,
                                                            aEnv )
                     == STL_SUCCESS );
            
            if( aConvExprParamInfo->mSubQueryExprList->mCount > 0 )
            {
                qlvRefExprItem  * sRefExprItem  = NULL;

                /* SubQuery를 갖는 Filter */

                /* i번째 SubQuery가 Statement에서 처리 가능한 SubQuery들로만 구성되었는지 판단 */
                sRefExprItem = aConvExprParamInfo->mSubQueryExprList->mHead;
                while( sRefExprItem != NULL )
                {
                    if( sRefExprItem->mExprPtr->mIterationTime <
                        DTL_ITERATION_TIME_FOR_EACH_STATEMENT )
                    {
                        break;
                    }

                    sRefExprItem = sRefExprItem->mNext;
                }

                if( sRefExprItem != NULL )
                {
                    /* Statement에서 처리 가능하지 않은 SubQuery가 존재하는 경우 */
                    sSubQueryExprArr[0] = aConvExprParamInfo->mInitExpr;
                    sSubQueryListArr[0] = aConvExprParamInfo->mSubQueryExprList;
                    sSubQueryExprCount  = 1;

                    /* SubQuery And Filter 생성 */
                    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mCandidateMem,
                                                STL_SIZEOF( qlncSubQueryAndFilter ),
                                                (void**) &sSubQueryAndFilter,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    sSubQueryAndFilter->mInitSubQueryFilter = sSubQueryExprArr;
                    sSubQueryAndFilter->mSubQueryExprList   = sSubQueryListArr;
                    sSubQueryAndFilter->mAndFilterCnt       = sSubQueryExprCount;
                }
                else
                {
                    /* Statement에서 처리 가능한 SubQuery만 존재하는 경우 */
                    sSubQueryExprArr[0] = NULL;
                    sSubQueryListArr[0] = aConvExprParamInfo->mSubQueryExprList;
                    sSubQueryExprCount  = 1;

                    /* SubQuery And Filter 생성 */
                    STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mCandidateMem,
                                                STL_SIZEOF( qlncSubQueryAndFilter ),
                                                (void**) &sSubQueryAndFilter,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    sSubQueryAndFilter->mInitSubQueryFilter = sSubQueryExprArr;
                    sSubQueryAndFilter->mSubQueryExprList   = sSubQueryListArr;
                    sSubQueryAndFilter->mAndFilterCnt       = sSubQueryExprCount;

                    sAndExpr = aConvExprParamInfo->mInitExpr;
                }
            }
            else
            {
                /* SubQuery를 갖지 않는 Filter */
                sAndExpr = aConvExprParamInfo->mInitExpr;
            }
        }
    }
    else
    {
        stlInt32              sExprCount            = 0;
        qlvInitExpression  ** sExprArr              = NULL;

        STL_DASSERT( sAndFilter->mOrCount > 1 );

        /* And Function Argument Expression Array 생성 */
        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitExpression* ) * sAndFilter->mOrCount,
                                    (void**) &sExprArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Function Argument 설정 */
        if( aConvExprParamInfo->mSubQueryExprList == NULL )
        {
            for( i = 0; i < sAndFilter->mOrCount; i++ )
            {
                aConvExprParamInfo->mCandExpr = (qlncExprCommon*)(sAndFilter->mOrFilters[i]);
                STL_TRY( qlncConvertCandExprToInitExprOrFilter( aConvExprParamInfo,
                                                                aEnv )
                         == STL_SUCCESS );

                /* SubQuery를 갖지 않는 Filter */
                sExprArr[sExprCount] = aConvExprParamInfo->mInitExpr;
                sExprCount++;
            }
        }
        else
        {
            for( i = 0; i < sAndFilter->mOrCount; i++ )
            {
                aConvExprParamInfo->mCandExpr = (qlncExprCommon*)(sAndFilter->mOrFilters[i]);
                STL_TRY( qlncConvertCandExprToInitExprOrFilter( aConvExprParamInfo,
                                                                aEnv )
                         == STL_SUCCESS );

                if( aConvExprParamInfo->mSubQueryExprList->mCount > 0 )
                {
                    qlvRefExprItem  * sRefExprItem  = NULL;

                    /* SubQuery를 갖는 Filter */

                    /* i번째 SubQuery가 Statement에서 처리 가능한 SubQuery들로만 구성되었는지 판단 */
                    sRefExprItem = aConvExprParamInfo->mSubQueryExprList->mHead;
                    while( sRefExprItem != NULL )
                    {
                        if( sRefExprItem->mExprPtr->mIterationTime <
                            DTL_ITERATION_TIME_FOR_EACH_STATEMENT )
                        {
                            break;
                        }

                        sRefExprItem = sRefExprItem->mNext;
                    }

                    if( sRefExprItem != NULL )
                    {
                        /* Statement에서 처리 가능하지 않은 SubQuery가 존재하는 경우 */
                        sSubQueryExprArr[sSubQueryExprCount] = aConvExprParamInfo->mInitExpr;
                        sSubQueryListArr[sSubQueryExprCount] = aConvExprParamInfo->mSubQueryExprList;
                        sSubQueryExprCount++;

                        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                                    STL_SIZEOF( qlvRefExprList ),
                                                    (void**) &(aConvExprParamInfo->mSubQueryExprList),
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        QLNC_INIT_LIST( aConvExprParamInfo->mSubQueryExprList );
                    }
                    else
                    {
                        /* Statement에서 처리 가능한 SubQuery만 존재하는 경우 */
                        sSubQueryExprArr[sSubQueryExprCount] = NULL;
                        sSubQueryListArr[sSubQueryExprCount] = aConvExprParamInfo->mSubQueryExprList;
                        sSubQueryExprCount++;

                        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                                    STL_SIZEOF( qlvRefExprList ),
                                                    (void**) &(aConvExprParamInfo->mSubQueryExprList),
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        QLNC_INIT_LIST( aConvExprParamInfo->mSubQueryExprList );

                        sExprArr[sExprCount] = aConvExprParamInfo->mInitExpr;
                        sExprCount++;
                    }
                }
                else
                {
                    /* SubQuery를 갖지 않는 Filter */
                    sExprArr[sExprCount] = aConvExprParamInfo->mInitExpr;
                    sExprCount++;
                }
            }
        }
        
        /* And Function Init Expression 생성 */
        if( sExprCount > 1 )
        {
            STL_TRY( qlvMakeFuncAndNArg( aConvExprParamInfo->mRegionMem,
                                         0,
                                         sExprCount,
                                         sExprArr,
                                         &sAndExpr,
                                         aEnv )
                     == STL_SUCCESS );
        }
        else if( sExprCount == 1 )
        {
            sAndExpr = sExprArr[0];
        }
        else
        {
            sAndExpr = NULL;
        }

        /* And Function Init Expression for SubQuery 생성 */
        if( sSubQueryExprCount > 0 )
        {
            /* SubQuery And Filter 생성 */
            STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mCandidateMem,
                                        STL_SIZEOF( qlncSubQueryAndFilter ),
                                        (void**) &sSubQueryAndFilter,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sSubQueryAndFilter->mInitSubQueryFilter = sSubQueryExprArr;
            sSubQueryAndFilter->mSubQueryExprList   = sSubQueryListArr;
            sSubQueryAndFilter->mAndFilterCnt       = sSubQueryExprCount;
        }
        else
        {
            sSubQueryAndFilter = NULL;
        }
    }


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sAndExpr;
    aConvExprParamInfo->mSubQueryAndFilter = sSubQueryAndFilter;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Current Plan에 Expression을 추가한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncAddReadColumnOnCurPlan( qlncConvExprParamInfo * aConvExprParamInfo,
                                      qllEnv                * aEnv )
{
    stlBool               sIsOuterColumn;
    stlBool               sIsExceptCase;
    stlUInt32             sPosition;
    qlncNodeCommon      * sNodeOfExpr                   = NULL;
    qlncPlanCommon      * sBackupCurPlan                = NULL;
    qlvRefExprList      * sOuterColumnList              = NULL;
    qlvInitOuterColumn  * sInitOuterColumn              = NULL;
    qlvInitExpression   * sInitExpr                     = NULL;

    qlncPlanCommon      * sFoundPlan                    = NULL;
    qlvRefExprList      * sFoundOuterColumnList         = NULL;

    qlncJoinOuterInfoList     sBackupJoinLeftOuterInfoList = { 0, NULL };
    qlncJoinOuterInfoList     sBackupJoinRightOuterInfoList = { 0, NULL };

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvExprParamInfo->mRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCurPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN) ||
                        (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN) ||
                        (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN),
                        QLL_ERROR_STACK(aEnv) );


    /* 현재 Expression의 Position 정보 저장 */
    sPosition = aConvExprParamInfo->mCandExpr->mPosition;


    /**
     * 현재의 Instant Node에 대한 참조인지 체크
     */

    sIsExceptCase = STL_FALSE;
    if( aConvExprParamInfo->mIsRefReadCol == STL_FALSE )
    {
        STL_DASSERT( (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SORT_INSTANT) ||
                     (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SORT_GROUP_INSTANT) ||
                     (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SORT_JOIN_INSTANT) ||
                     (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_HASH_INSTANT) ||
                     (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_HASH_GROUP_INSTANT) ||
                     (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_HASH_JOIN_INSTANT) ||
                     (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_GROUP) );

        if( (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN) &&
            ( (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT) ||
              (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT) ) &&
            (aConvExprParamInfo->mCurPlan->mCandNode->mNodeID == ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID) )
        {
            /* 자신 노드에 대한 참조인 경우 */
            STL_TRY( qlncAddReadColumnOnCurPlan_InstantForSelfRef( aConvExprParamInfo,
                                                                   aEnv )
                     == STL_SUCCESS );
            sInitExpr = aConvExprParamInfo->mInitExpr;
            sInitExpr->mPosition = sPosition;

            STL_THROW( RAMP_FINISH );
        }
        else
        {
            aConvExprParamInfo->mIsRefReadCol = STL_TRUE;
            if( (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SORT_INSTANT) ||
                (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SORT_GROUP_INSTANT) ||
                (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SORT_JOIN_INSTANT) )
            {
                aConvExprParamInfo->mCurPlan =
                    ((qlncPlanSortInstant*)(aConvExprParamInfo->mCurPlan))->mChildPlanNode;

                /**
                 * 예외 상황 체크
                 * ------------------------
                 *  Sort Instant 아래에 SetOP가 있는 경우 RootQuerySet에서 Node를 찾도록 해야 한다.
                 */

                if( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SET_OP )
                {
                    /* @todo 현재 여기에 들어오는 경우가 없는 것으로 판단됨 */

                    sIsExceptCase = STL_TRUE;
                }
            }
            else
            {
                aConvExprParamInfo->mCurPlan =
                    ((qlncPlanHashInstant*)(aConvExprParamInfo->mCurPlan))->mChildPlanNode;
            }

        }
    }

    /* Candidate Expression의 Candidate Node 찾기 */
    if( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN )
    {
        sNodeOfExpr = ((qlncExprColumn*)(aConvExprParamInfo->mCandExpr))->mNode;
    }
    else if( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN )
    {
        sNodeOfExpr = ((qlncExprRowidColumn*)(aConvExprParamInfo->mCandExpr))->mNode;
    }
    else
    {
        STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
        sNodeOfExpr = ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode;
    }

    /* Outer Column 여부 체크 및 Outer Column 처리를 위한 정보 설정 */
    switch( aConvExprParamInfo->mCurPlan->mPlanType )
    {
        case QLNC_PLAN_TYPE_QUERY_SET:
        case QLNC_PLAN_TYPE_SET_OP:
        case QLNC_PLAN_TYPE_QUERY_SPEC:
        case QLNC_PLAN_TYPE_TABLE_SCAN:
        case QLNC_PLAN_TYPE_INDEX_SCAN:
        case QLNC_PLAN_TYPE_ROWID_SCAN:
        case QLNC_PLAN_TYPE_INDEX_COMBINE:
        case QLNC_PLAN_TYPE_SUB_TABLE:
        case QLNC_PLAN_TYPE_JOIN_COMBINE:
        case QLNC_PLAN_TYPE_SORT_INSTANT:
        case QLNC_PLAN_TYPE_SORT_GROUP_INSTANT:
        case QLNC_PLAN_TYPE_SORT_JOIN_INSTANT:
        case QLNC_PLAN_TYPE_HASH_INSTANT:
        case QLNC_PLAN_TYPE_HASH_GROUP_INSTANT:
        case QLNC_PLAN_TYPE_HASH_JOIN_INSTANT:
        case QLNC_PLAN_TYPE_GROUP:
            if( (qlncIsExistCandNodeOnPlanNode( aConvExprParamInfo->mCurPlan, sNodeOfExpr) == STL_TRUE) ||
                ( (sIsExceptCase == STL_TRUE) &&
                  (qlncIsExistCandNodeOnPlanNode( (qlncPlanCommon*)(((qlncPlanSetOP*)(aConvExprParamInfo->mCurPlan))->mRootQuerySet), sNodeOfExpr) == STL_TRUE) ) )
            {
                /* 현재 Plan Node에 존재하는 경우 */
                sIsOuterColumn = STL_FALSE;
            }
            else
            {
                sFoundPlan = NULL;
                sFoundOuterColumnList = NULL;

                if( aConvExprParamInfo->mJoinLeftOuterInfoList.mCount > 0 )
                {
                    qlncJoinOuterInfoItem   * sJoinOuterInfoItem    = NULL;

                    sJoinOuterInfoItem = aConvExprParamInfo->mJoinLeftOuterInfoList.mHead;
                    while( sJoinOuterInfoItem != NULL )
                    {
                        if( qlncIsExistCandNodeOnPlanNode( sJoinOuterInfoItem->mOuterPlanNode, sNodeOfExpr) == STL_TRUE )
                        {
                            sFoundPlan = sJoinOuterInfoItem->mOuterPlanNode;
                            sFoundOuterColumnList = sJoinOuterInfoItem->mOuterColumnList;
                            break;
                        }

                        sJoinOuterInfoItem = sJoinOuterInfoItem->mNext;
                    }
                }

                if( sFoundPlan != NULL )
                {
                    STL_DASSERT( sFoundOuterColumnList != NULL );

                    /* Left Outer Plan Node에 존재하는 경우 */
                    sIsOuterColumn = STL_TRUE;

                    /* Backup 정보 설정 */
                    sBackupCurPlan = aConvExprParamInfo->mCurPlan;
                    sBackupJoinLeftOuterInfoList.mCount =
                        aConvExprParamInfo->mJoinLeftOuterInfoList.mCount;
                    sBackupJoinLeftOuterInfoList.mHead =
                        aConvExprParamInfo->mJoinLeftOuterInfoList.mHead;
                    sBackupJoinRightOuterInfoList.mCount =
                        aConvExprParamInfo->mJoinRightOuterInfoList.mCount;
                    sBackupJoinRightOuterInfoList.mHead =
                        aConvExprParamInfo->mJoinRightOuterInfoList.mHead;

                    /* Convert Expression Parameter Info 재설정 */
                    aConvExprParamInfo->mCurPlan = sFoundPlan;
                    aConvExprParamInfo->mJoinLeftOuterInfoList.mCount = 0;
                    aConvExprParamInfo->mJoinLeftOuterInfoList.mHead = NULL;
                    aConvExprParamInfo->mJoinRightOuterInfoList.mCount = 0;
                    aConvExprParamInfo->mJoinRightOuterInfoList.mHead = NULL;

                    sOuterColumnList = sFoundOuterColumnList;
                }
                else
                {
                    /* 외부(상위 Level)에 존재하는 Plan Node에 존재하는 경우 */
                    STL_DASSERT( aConvExprParamInfo->mPlanNodeMapDynArr != NULL );

                    STL_TRY( qlncFindPlanNodeFromPlanNodeMapDynArray(
                                 aConvExprParamInfo->mPlanNodeMapDynArr,
                                 sNodeOfExpr,
                                 &sFoundPlan,
                                 aEnv )
                             == STL_SUCCESS );
                    STL_DASSERT( sFoundPlan != NULL );

                    (void)qlncFindOuterColumnListFromPlanNode( sFoundPlan,
                                                               &sFoundOuterColumnList );
                    STL_DASSERT( sFoundOuterColumnList != NULL );

                    sIsOuterColumn = STL_TRUE;

                    /* Backup 정보 설정 */
                    sBackupCurPlan = aConvExprParamInfo->mCurPlan;
                    sBackupJoinLeftOuterInfoList.mCount =
                        aConvExprParamInfo->mJoinLeftOuterInfoList.mCount;
                    sBackupJoinLeftOuterInfoList.mHead =
                        aConvExprParamInfo->mJoinLeftOuterInfoList.mHead;
                    sBackupJoinRightOuterInfoList.mCount =
                        aConvExprParamInfo->mJoinRightOuterInfoList.mCount;
                    sBackupJoinRightOuterInfoList.mHead =
                        aConvExprParamInfo->mJoinRightOuterInfoList.mHead;

                    /* Convert Expression Parameter Info 재설정 */
                    aConvExprParamInfo->mCurPlan = sFoundPlan;
                    aConvExprParamInfo->mJoinLeftOuterInfoList.mCount = 0;
                    aConvExprParamInfo->mJoinLeftOuterInfoList.mHead = NULL;
                    aConvExprParamInfo->mJoinRightOuterInfoList.mCount = 0;
                    aConvExprParamInfo->mJoinRightOuterInfoList.mHead = NULL;

                    sOuterColumnList = sFoundOuterColumnList;
                }
            }

            break;

        case QLNC_PLAN_TYPE_NESTED_JOIN:
        case QLNC_PLAN_TYPE_MERGE_JOIN:
        case QLNC_PLAN_TYPE_HASH_JOIN:
            /* Merge Join의 경우 Left와 Right 모두에 Outer Column을 필요로 하는 경우가 있다. */
            if( (aConvExprParamInfo->mJoinLeftOuterInfoList.mCount > 0) &&
                (aConvExprParamInfo->mJoinRightOuterInfoList.mCount > 0) )
            {
                sIsOuterColumn = STL_TRUE;

                /* Backup 정보 설정 */
                sBackupCurPlan = aConvExprParamInfo->mCurPlan;
                sBackupJoinLeftOuterInfoList.mCount =
                    aConvExprParamInfo->mJoinLeftOuterInfoList.mCount;
                sBackupJoinLeftOuterInfoList.mHead =
                    aConvExprParamInfo->mJoinLeftOuterInfoList.mHead;
                sBackupJoinRightOuterInfoList.mCount =
                    aConvExprParamInfo->mJoinRightOuterInfoList.mCount;
                sBackupJoinRightOuterInfoList.mHead =
                    aConvExprParamInfo->mJoinRightOuterInfoList.mHead;

                if( (sBackupJoinLeftOuterInfoList.mCount > 0) &&
                    (qlncIsExistCandNodeOnPlanNode( sBackupJoinLeftOuterInfoList.mHead->mOuterPlanNode, sNodeOfExpr ) == STL_TRUE) )
                {
                    /* Left Node에 있는 경우 */
                    /* Convert Expression Parameter Info 재설정 */
                    aConvExprParamInfo->mCurPlan = sBackupJoinLeftOuterInfoList.mHead->mOuterPlanNode;
                    aConvExprParamInfo->mJoinLeftOuterInfoList.mCount = 0;
                    aConvExprParamInfo->mJoinLeftOuterInfoList.mHead = NULL;
                    aConvExprParamInfo->mJoinRightOuterInfoList.mCount = 0;
                    aConvExprParamInfo->mJoinRightOuterInfoList.mHead = NULL;

                    sOuterColumnList = sBackupJoinLeftOuterInfoList.mHead->mOuterColumnList;
                }
                else if( (sBackupJoinRightOuterInfoList.mCount > 0) &&
                         (qlncIsExistCandNodeOnPlanNode( sBackupJoinRightOuterInfoList.mHead->mOuterPlanNode, sNodeOfExpr ) == STL_TRUE) )
                {
                    /* Right Node에 있는 경우 */
                    /* Convert Expression Parameter Info 재설정 */
                    aConvExprParamInfo->mCurPlan = sBackupJoinRightOuterInfoList.mHead->mOuterPlanNode;
                    aConvExprParamInfo->mJoinLeftOuterInfoList.mCount = 0;
                    aConvExprParamInfo->mJoinLeftOuterInfoList.mHead = NULL;
                    aConvExprParamInfo->mJoinRightOuterInfoList.mCount = 0;
                    aConvExprParamInfo->mJoinRightOuterInfoList.mHead = NULL;

                    sOuterColumnList = sBackupJoinRightOuterInfoList.mHead->mOuterColumnList;
                }
                else
                {
                    /* 외부(상위 Level)에 존재하는 Plan Node에 존재하는 경우 */
                    STL_DASSERT( aConvExprParamInfo->mPlanNodeMapDynArr != NULL );

                    STL_TRY( qlncFindPlanNodeFromPlanNodeMapDynArray(
                                 aConvExprParamInfo->mPlanNodeMapDynArr,
                                 sNodeOfExpr,
                                 &sFoundPlan,
                                 aEnv )
                             == STL_SUCCESS );
                    STL_DASSERT( sFoundPlan != NULL );

                    (void)qlncFindOuterColumnListFromPlanNode( sFoundPlan,
                                                               &sFoundOuterColumnList );
                    STL_DASSERT( sFoundOuterColumnList != NULL );

                    /* Convert Expression Parameter Info 재설정 */
                    aConvExprParamInfo->mCurPlan = sFoundPlan;
                    aConvExprParamInfo->mJoinLeftOuterInfoList.mCount = 0;
                    aConvExprParamInfo->mJoinLeftOuterInfoList.mHead = NULL;
                    aConvExprParamInfo->mJoinRightOuterInfoList.mCount = 0;
                    aConvExprParamInfo->mJoinRightOuterInfoList.mHead = NULL;

                    sOuterColumnList = sFoundOuterColumnList;
                }
            }
            else
            {
                if( qlncIsExistCandNodeOnPlanNode( aConvExprParamInfo->mCurPlan, sNodeOfExpr) == STL_TRUE )
                {
                    /* 현재 Plan Node에 존재하는 경우 */
                    sIsOuterColumn = STL_FALSE;
                }
                else
                {
                    sFoundPlan = NULL;
                    sFoundOuterColumnList = NULL;

                    if( aConvExprParamInfo->mJoinLeftOuterInfoList.mCount > 0 )
                    {
                        qlncJoinOuterInfoItem   * sJoinOuterInfoItem    = NULL;

                        sJoinOuterInfoItem = aConvExprParamInfo->mJoinLeftOuterInfoList.mHead;
                        while( sJoinOuterInfoItem != NULL )
                        {
                            if( qlncIsExistCandNodeOnPlanNode( sJoinOuterInfoItem->mOuterPlanNode, sNodeOfExpr) == STL_TRUE )
                            {
                                sFoundPlan = sJoinOuterInfoItem->mOuterPlanNode;
                                sFoundOuterColumnList = sJoinOuterInfoItem->mOuterColumnList;
                                break;
                            }

                            sJoinOuterInfoItem = sJoinOuterInfoItem->mNext;
                        }
                    }

                    if( sFoundPlan != NULL )
                    {
                        STL_DASSERT( sFoundOuterColumnList != NULL );

                        /* Left Outer Plan Node에 존재하는 경우 */
                        sIsOuterColumn = STL_TRUE;

                        /* Backup 정보 설정 */
                        sBackupCurPlan = aConvExprParamInfo->mCurPlan;
                        sBackupJoinLeftOuterInfoList.mCount =
                            aConvExprParamInfo->mJoinLeftOuterInfoList.mCount;
                        sBackupJoinLeftOuterInfoList.mHead =
                            aConvExprParamInfo->mJoinLeftOuterInfoList.mHead;
                        sBackupJoinRightOuterInfoList.mCount =
                            aConvExprParamInfo->mJoinRightOuterInfoList.mCount;
                        sBackupJoinRightOuterInfoList.mHead =
                            aConvExprParamInfo->mJoinRightOuterInfoList.mHead;

                        /* Convert Expression Parameter Info 재설정 */
                        aConvExprParamInfo->mCurPlan = sFoundPlan;
                        aConvExprParamInfo->mJoinLeftOuterInfoList.mCount = 0;
                        aConvExprParamInfo->mJoinLeftOuterInfoList.mHead = NULL;
                        aConvExprParamInfo->mJoinRightOuterInfoList.mCount = 0;
                        aConvExprParamInfo->mJoinRightOuterInfoList.mHead = NULL;

                        sOuterColumnList = sFoundOuterColumnList;
                    }
                    else
                    {
                        /* 외부(상위 Level)에 존재하는 Plan Node에 존재하는 경우 */
                        STL_DASSERT( aConvExprParamInfo->mPlanNodeMapDynArr != NULL );

                        STL_TRY( qlncFindPlanNodeFromPlanNodeMapDynArray(
                                     aConvExprParamInfo->mPlanNodeMapDynArr,
                                     sNodeOfExpr,
                                     &sFoundPlan,
                                     aEnv )
                                 == STL_SUCCESS );
                        STL_DASSERT( sFoundPlan != NULL );

                        (void)qlncFindOuterColumnListFromPlanNode( sFoundPlan,
                                                                   &sFoundOuterColumnList );
                        STL_DASSERT( sFoundOuterColumnList != NULL );

                        sIsOuterColumn = STL_TRUE;

                        /* Backup 정보 설정 */
                        sBackupCurPlan = aConvExprParamInfo->mCurPlan;
                        sBackupJoinLeftOuterInfoList.mCount =
                            aConvExprParamInfo->mJoinLeftOuterInfoList.mCount;
                        sBackupJoinLeftOuterInfoList.mHead =
                            aConvExprParamInfo->mJoinLeftOuterInfoList.mHead;
                        sBackupJoinRightOuterInfoList.mCount =
                            aConvExprParamInfo->mJoinRightOuterInfoList.mCount;
                        sBackupJoinRightOuterInfoList.mHead =
                            aConvExprParamInfo->mJoinRightOuterInfoList.mHead;

                        /* Convert Expression Parameter Info 재설정 */
                        aConvExprParamInfo->mCurPlan = sFoundPlan;
                        aConvExprParamInfo->mJoinLeftOuterInfoList.mCount = 0;
                        aConvExprParamInfo->mJoinLeftOuterInfoList.mHead = NULL;
                        aConvExprParamInfo->mJoinRightOuterInfoList.mCount = 0;
                        aConvExprParamInfo->mJoinRightOuterInfoList.mHead = NULL;

                        sOuterColumnList = sFoundOuterColumnList;
                    }
                }
            }

            break;

        case QLNC_PLAN_TYPE_SUB_QUERY_LIST:
        case QLNC_PLAN_TYPE_SUB_QUERY_FILTER:
            sIsOuterColumn = STL_FALSE;
            break;

        default:
            STL_DASSERT( 0 );
            sIsOuterColumn = STL_FALSE;
            break;
    }


    switch( aConvExprParamInfo->mCurPlan->mPlanType )
    {
        case QLNC_PLAN_TYPE_TABLE_SCAN:
        case QLNC_PLAN_TYPE_INDEX_SCAN:
        case QLNC_PLAN_TYPE_ROWID_SCAN:
            STL_TRY( qlncAddReadColumnOnCurPlan_BaseTable( aConvExprParamInfo,
                                                           aEnv )
                     == STL_SUCCESS );
            sInitExpr = aConvExprParamInfo->mInitExpr;

            break;

        case QLNC_PLAN_TYPE_INDEX_COMBINE:
            STL_TRY( qlncAddReadColumnOnCurPlan_IndexCombine( aConvExprParamInfo,
                                                              aEnv )
                     == STL_SUCCESS );
            sInitExpr = aConvExprParamInfo->mInitExpr;

            break;

        case QLNC_PLAN_TYPE_SUB_TABLE:
            STL_TRY( qlncAddReadColumnOnCurPlan_SubTable( aConvExprParamInfo,
                                                          aEnv )
                     == STL_SUCCESS );
            sInitExpr = aConvExprParamInfo->mInitExpr;

            break;

        case QLNC_PLAN_TYPE_JOIN_COMBINE:
            STL_TRY( qlncAddReadColumnOnCurPlan_JoinCombine( aConvExprParamInfo,
                                                             aEnv )
                     == STL_SUCCESS );
            sInitExpr = aConvExprParamInfo->mInitExpr;

            break;

        case QLNC_PLAN_TYPE_NESTED_JOIN:
        case QLNC_PLAN_TYPE_MERGE_JOIN:
        case QLNC_PLAN_TYPE_HASH_JOIN:
            STL_TRY( qlncAddReadColumnOnCurPlan_JoinTable( aConvExprParamInfo,
                                                           aEnv )
                     == STL_SUCCESS );
            sInitExpr = aConvExprParamInfo->mInitExpr;

            break;

        case QLNC_PLAN_TYPE_SORT_INSTANT:
        case QLNC_PLAN_TYPE_SORT_GROUP_INSTANT:
        case QLNC_PLAN_TYPE_SORT_JOIN_INSTANT:
        case QLNC_PLAN_TYPE_HASH_INSTANT:
        case QLNC_PLAN_TYPE_HASH_GROUP_INSTANT:
        case QLNC_PLAN_TYPE_HASH_JOIN_INSTANT:
        case QLNC_PLAN_TYPE_GROUP:
            STL_TRY( qlncAddReadColumnOnCurPlan_Instant( aConvExprParamInfo,
                                                         aEnv )
                     == STL_SUCCESS );
            sInitExpr = aConvExprParamInfo->mInitExpr;

            break;

        case QLNC_PLAN_TYPE_SUB_QUERY_LIST:
        case QLNC_PLAN_TYPE_SUB_QUERY_FILTER:
            STL_TRY( qlncAddReadColumnOnCurPlan_ByPass( aConvExprParamInfo,
                                                        aEnv )
                     == STL_SUCCESS );
            sInitExpr = aConvExprParamInfo->mInitExpr;

            break;

        /* case QLNC_PLAN_TYPE_QUERY_SET: */
        /* case QLNC_PLAN_TYPE_SET_OP: */
        /* case QLNC_PLAN_TYPE_QUERY_SPEC: */
        default:
            STL_DASSERT( 0 );
            break;
    }

    sInitExpr->mPosition = sPosition;

    if( sIsOuterColumn == STL_TRUE )
    {
        /* OuterColumn 할당 */
        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitOuterColumn ),
                                    (void**) &sInitOuterColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Relation Code 설정 */
        sInitOuterColumn->mRelationNode = aConvExprParamInfo->mCurPlan->mInitNode;

        /* Orginal Expression 설정 */
        sInitOuterColumn->mOrgExpr = sInitExpr;

        /* Init Expression 생성 */
        QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                                   QLV_EXPR_TYPE_OUTER_COLUMN,
                                   sInitOuterColumn->mOrgExpr->mRelatedFuncId,
                                   DTL_ITERATION_TIME_FOR_EACH_QUERY,
                                   sPosition,
                                   QLNC_EXPR_PHRASE_WHERE,
                                   sInitOuterColumn->mOrgExpr->mIncludeExprCnt,
                                   aConvExprParamInfo->mInitExpr->mColumnName,
                                   sInitOuterColumn,
                                   sInitExpr,
                                   aEnv );

        /* Outer Column List에 추가 */
        STL_TRY( qlvAddExprToRefExprList( sOuterColumnList,
                                          sInitExpr,
                                          STL_FALSE,
                                          aConvExprParamInfo->mRegionMem,
                                          aEnv )
                 == STL_SUCCESS );

        /* Backup 정보 원복 */
        aConvExprParamInfo->mCurPlan = sBackupCurPlan;
        aConvExprParamInfo->mJoinLeftOuterInfoList.mCount =
            sBackupJoinLeftOuterInfoList.mCount;
        aConvExprParamInfo->mJoinLeftOuterInfoList.mHead =
            sBackupJoinLeftOuterInfoList.mHead;
        aConvExprParamInfo->mJoinRightOuterInfoList.mCount =
            sBackupJoinRightOuterInfoList.mCount;
        aConvExprParamInfo->mJoinRightOuterInfoList.mHead =
            sBackupJoinRightOuterInfoList.mHead;
    }


    STL_RAMP( RAMP_FINISH );


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Set OP의 Read Column List에 Column Expression을 추가한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncAddReadColumnOnCurPlan_SetOP( qlncConvExprParamInfo   * aConvExprParamInfo,
                                            qllEnv                  * aEnv )
{
    qlncPlanSetOP       * sPlanSetOP        = NULL;
    qlvInitExpression   * sInitExpr         = NULL;
    qlncReadColMap      * sReadColMap       = NULL;
    qlncReadColMapList  * sReadColMapList   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvExprParamInfo->mRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCurPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SET_OP,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCandExpr != NULL, QLL_ERROR_STACK(aEnv) );


    /* Set OP Plan Node 설정 */
    sPlanSetOP = (qlncPlanSetOP*)(aConvExprParamInfo->mCurPlan);

    /* Read Column Map List 설정 */
    STL_DASSERT( sPlanSetOP->mReadColMapList != NULL );
    sReadColMapList = sPlanSetOP->mReadColMapList;

    /* Find Expression */
    sInitExpr = NULL;
    sReadColMap = sReadColMapList->mHead;
    while( sReadColMap != NULL )
    {
        if( ( sReadColMap->mCandExpr->mType == aConvExprParamInfo->mCandExpr->mType ) &&
            ( sReadColMap->mCandExpr->mPosition ==
              aConvExprParamInfo->mCandExpr->mPosition ) )
        {
            if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN )
            {
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN );
                if( ((qlncExprColumn*)(sReadColMap->mCandExpr))->mColumnID ==
                    ((qlncExprColumn*)(aConvExprParamInfo->mCandExpr))->mColumnID )
                {
                    /* 이미 등록된 Column Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }
            else if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN )
            {
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );
                if( ((qlncExprRowidColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                    ((qlncExprRowidColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID )
                {
                    /* 이미 등록된 Rowid Column Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }
            else if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN )
            {
                STL_DASSERT( (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_SUB_TABLE) ||
                             (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_SORT_INSTANT) ||
                             (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_HASH_INSTANT) );
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
                STL_DASSERT( (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                             QLNC_NODE_TYPE_SUB_TABLE) ||
                             (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                             QLNC_NODE_TYPE_SORT_INSTANT) ||
                             (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                             QLNC_NODE_TYPE_HASH_INSTANT) );

                if( ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mIdx ==
                      ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mIdx ) &&
                    ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                      ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID ) )
                {
                    /* 이미 등록된 Inner Column Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }
            else
            {
                if( qlncIsSameExpr( sReadColMap->mCandExpr,
                                    aConvExprParamInfo->mCandExpr )
                    == STL_TRUE )
                {
                    /* 이미 등록된 Inner Column Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }
        }

        sReadColMap = sReadColMap->mNext;
    }

    /* 반드시 등록되어 있어야 함 */
    STL_DASSERT( sInitExpr != NULL );


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Base Table의 Read Column List에 Column Expression을 추가한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncAddReadColumnOnCurPlan_BaseTable( qlncConvExprParamInfo   * aConvExprParamInfo,
                                                qllEnv                  * aEnv )
{
    qlvInitExpression   * sInitExpr         = NULL;
    qlncReadColMap      * sReadColMap       = NULL;
    qlncReadColMapList  * sReadColMapList   = NULL;
    qlvRefExprList      * sReadColList      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvExprParamInfo->mRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCurPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN) ||
                        (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN),
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Macro 정의
     */

#define _QLNC_SET_READ_COL_MAP_LIST_AND_READ_COL_LIST_ON_BASE_TABLE( _aStructName )             \
    {                                                                                           \
        if( ((_aStructName*)aConvExprParamInfo->mCurPlan)->mReadColMapList == NULL )            \
        {                                                                                       \
            STL_DASSERT( ((_aStructName*)aConvExprParamInfo->mCurPlan)->mReadColList != NULL ); \
            STL_DASSERT( ((_aStructName*)aConvExprParamInfo->mCurPlan)->mReadColList->mCount == 0 ); \
                                                                                                \
            /* Read Column Map List 할당 */                                                     \
            STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mCandidateMem,                      \
                                        STL_SIZEOF( qlncReadColMapList ),                       \
                                        (void**) &sReadColMapList,                              \
                                        KNL_ENV(aEnv) )                                         \
                     == STL_SUCCESS );                                                          \
            QLNC_INIT_LIST( sReadColMapList );                                                  \
            ((_aStructName*)aConvExprParamInfo->mCurPlan)->mReadColMapList = sReadColMapList;   \
            sReadColList = ((_aStructName*)aConvExprParamInfo->mCurPlan)->mReadColList;         \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            sReadColMapList = ((_aStructName*)aConvExprParamInfo->mCurPlan)->mReadColMapList;   \
            sReadColList = ((_aStructName*)aConvExprParamInfo->mCurPlan)->mReadColList;         \
        }                                                                                       \
    }


    /**
     * Plan Type 별 Read Column Map List와 Read Column List 설정
     */

    if( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_TABLE_SCAN )
    {
        _QLNC_SET_READ_COL_MAP_LIST_AND_READ_COL_LIST_ON_BASE_TABLE( qlncPlanTableScan );
    }
    else if( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_INDEX_SCAN )
    {
        _QLNC_SET_READ_COL_MAP_LIST_AND_READ_COL_LIST_ON_BASE_TABLE( qlncPlanIndexScan );
    }
    else
    {
        STL_DASSERT( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_ROWID_SCAN );
        _QLNC_SET_READ_COL_MAP_LIST_AND_READ_COL_LIST_ON_BASE_TABLE( qlncPlanRowidScan );
    }


    /* Find Expression */
    sInitExpr = NULL;
    sReadColMap = sReadColMapList->mHead;
    while( sReadColMap != NULL )
    {
        if( sReadColMap->mCandExpr->mType == aConvExprParamInfo->mCandExpr->mType )
        {
            if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN )
            {
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN );
                if( ((qlncExprColumn*)(sReadColMap->mCandExpr))->mColumnID ==
                    ((qlncExprColumn*)(aConvExprParamInfo->mCandExpr))->mColumnID )
                {
                    /* 이미 등록된 Column Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }
            else
            {
                STL_DASSERT( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );

                /* 이미 등록된 Rowid Column Expression */
                sInitExpr = sReadColMap->mInitExpr;
                break;
            }
        }

        sReadColMap = sReadColMap->mNext;
    }

    /* 등록되지 않았다면 등록함 */
    if( sInitExpr == NULL )
    {
        if( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN )
        {
            qlvInitColumn   * sInitColumn   = NULL;
            qlncExprColumn  * sCandColumn   = NULL;

            /* Candidate Column Expression 설정 */
            sCandColumn = (qlncExprColumn*)(aConvExprParamInfo->mCandExpr);

            /* Init Column 할당 */
            STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitColumn ),
                                        (void**) &sInitColumn,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /* Relation Node 설정 */
            sInitColumn->mRelationNode = aConvExprParamInfo->mCurPlan->mInitNode;

            /* Column Handle 설정 */
            QLNC_ALLOC_AND_COPY_DICT_HANDLE( aConvExprParamInfo->mRegionMem,
                                             sCandColumn->mColumnHandle,
                                             sInitColumn->mColumnHandle,
                                             aEnv );

            /* Init Expression 생성 */
            QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                                       QLV_EXPR_TYPE_COLUMN,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       sCandColumn->mCommon.mIterationTime,
                                       sCandColumn->mCommon.mPosition,
                                       sCandColumn->mCommon.mExprPhraseFlag,
                                       0,
                                       sCandColumn->mCommon.mColumnName,
                                       sInitColumn,
                                       sInitExpr,
                                       aEnv );

            /* Read Column List에 등록 */
            STL_TRY( qlvAddExprToRefColExprList( sReadColList,
                                                 sInitExpr,
                                                 aConvExprParamInfo->mRegionMem,
                                                 aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            qlvInitRowIdColumn  * sInitRowidColumn  = NULL;
            qlncExprRowidColumn * sCandRowidColumn  = NULL;

            STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );

            /* Candidate Rowid Column Expression 설정 */
            sCandRowidColumn = (qlncExprRowidColumn*)(aConvExprParamInfo->mCandExpr);

            /* Init Rowid Column 할당 */
            STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitRowIdColumn ),
                                        (void**) &sInitRowidColumn,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /* Relation Node 설정 */
            sInitRowidColumn->mRelationNode = aConvExprParamInfo->mCurPlan->mInitNode;

            /* Init Expression 생성 */
            QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                                       QLV_EXPR_TYPE_ROWID_COLUMN,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       sCandRowidColumn->mCommon.mIterationTime,
                                       sCandRowidColumn->mCommon.mPosition,
                                       sCandRowidColumn->mCommon.mExprPhraseFlag,
                                       0,
                                       sCandRowidColumn->mCommon.mColumnName,
                                       sInitRowidColumn,
                                       sInitExpr,
                                       aEnv );

            /* Rowid Column에 등록 */
            if( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_TABLE_SCAN )
            {
                ((qlncPlanTableScan*)(aConvExprParamInfo->mCurPlan))->mRowidColumn = sInitExpr;
            }
            else if( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_INDEX_SCAN )
            {
                ((qlncPlanIndexScan*)(aConvExprParamInfo->mCurPlan))->mRowidColumn = sInitExpr;
            }
            else
            {
                STL_DASSERT( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_ROWID_SCAN );
                ((qlncPlanRowidScan*)(aConvExprParamInfo->mCurPlan))->mRowidColumn = sInitExpr;
            }
        }

        /* Read Column Map List에 등록 */
        QLNC_ADD_READ_COLUMN_MAP( aConvExprParamInfo->mCandidateMem,
                                  sReadColMapList,
                                  aConvExprParamInfo->mCandExpr,
                                  sInitExpr,
                                  aEnv );
    }


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index Combine의 Read Column List에 Column Expression을 추가한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncAddReadColumnOnCurPlan_IndexCombine( qlncConvExprParamInfo    * aConvExprParamInfo,
                                                   qllEnv                   * aEnv )
{
    stlInt32                  i;
    qlncPlanIndexCombine    * sPlanIndexCombine = NULL;
    qlvInitExpression       * sInitExpr         = NULL;
    qlncReadColMap          * sReadColMap       = NULL;
    qlncReadColMapList      * sReadColMapList   = NULL;
    qlvRefExprList          * sReadColList      = NULL;
    qlncPlanCommon          * sBackupCurPlan    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvExprParamInfo->mRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCurPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_INDEX_COMBINE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN) ||
                        (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN),
                        QLL_ERROR_STACK(aEnv) );


    /* Plan Index Combine 설정 */
    sPlanIndexCombine = (qlncPlanIndexCombine*)(aConvExprParamInfo->mCurPlan);

    /* Read Column Map List와 Read Column List 설정 */
    if( sPlanIndexCombine->mReadColMapList == NULL )
    {
        STL_DASSERT( sPlanIndexCombine->mReadColList == NULL );

        /* Read Column Map List 할당 */
        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mCandidateMem,
                                    STL_SIZEOF( qlncReadColMapList ),
                                    (void**) &sReadColMapList,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_LIST( sReadColMapList );
        sPlanIndexCombine->mReadColMapList = sReadColMapList;

        /* Read Column List 할당 */
        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvRefExprList ),
                                    (void**) &sReadColList,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_LIST( sReadColList );
        sPlanIndexCombine->mReadColList = sReadColList;
    }
    else
    {
        sReadColMapList = sPlanIndexCombine->mReadColMapList;
        sReadColList = sPlanIndexCombine->mReadColList;
    }

    /* Find Expression */
    sInitExpr = NULL;
    sReadColMap = sReadColMapList->mHead;
    while( sReadColMap != NULL )
    {
        if( sReadColMap->mCandExpr->mType == aConvExprParamInfo->mCandExpr->mType )
        {
            if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN )
            {
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN );
                if( ((qlncExprColumn*)(sReadColMap->mCandExpr))->mColumnID ==
                    ((qlncExprColumn*)(aConvExprParamInfo->mCandExpr))->mColumnID )
                {
                    /* 이미 등록된 Column Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }
            else
            {
                STL_DASSERT( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );

                /* 이미 등록된 Rowid Column Expression */
                sInitExpr = sReadColMap->mInitExpr;
                break;
            }
        }

        sReadColMap = sReadColMap->mNext;
    }

    /* 등록되지 않았다면 등록함 */
    if( sInitExpr == NULL )
    {
        if( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN )
        {
            qlvInitColumn   * sInitColumn   = NULL;
            qlncExprColumn  * sCandColumn   = NULL;

            /* Candidate Column Expression 설정 */
            sCandColumn = (qlncExprColumn*)(aConvExprParamInfo->mCandExpr);

            /* Init Column 할당 */
            STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitColumn ),
                                        (void**) &sInitColumn,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /* Relation Node 설정 */
            sInitColumn->mRelationNode = aConvExprParamInfo->mCurPlan->mInitNode;

            /* Column Handle 설정 */
            QLNC_ALLOC_AND_COPY_DICT_HANDLE( aConvExprParamInfo->mRegionMem,
                                             sCandColumn->mColumnHandle,
                                             sInitColumn->mColumnHandle,
                                             aEnv );

            /* Init Expression 생성 */
            QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                                       QLV_EXPR_TYPE_COLUMN,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       sCandColumn->mCommon.mIterationTime,
                                       sCandColumn->mCommon.mPosition,
                                       sCandColumn->mCommon.mExprPhraseFlag,
                                       0,
                                       sCandColumn->mCommon.mColumnName,
                                       sInitColumn,
                                       sInitExpr,
                                       aEnv );

            /* Read Column List에 등록 */
            STL_TRY( qlvAddExprToRefColExprList( sReadColList,
                                                 sInitExpr,
                                                 aConvExprParamInfo->mRegionMem,
                                                 aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            qlvInitRowIdColumn  * sInitRowidColumn  = NULL;
            qlncExprRowidColumn * sCandRowidColumn  = NULL;

            STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );

            /* Candidate Rowid Column Expression 설정 */
            sCandRowidColumn = (qlncExprRowidColumn*)(aConvExprParamInfo->mCandExpr);

            /* Init Rowid Column 할당 */
            STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitRowIdColumn ),
                                        (void**) &sInitRowidColumn,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /* Relation Node 설정 */
            sInitRowidColumn->mRelationNode = aConvExprParamInfo->mCurPlan->mInitNode;

            /* Init Expression 생성 */
            QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
                                       QLV_EXPR_TYPE_ROWID_COLUMN,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       sCandRowidColumn->mCommon.mIterationTime,
                                       sCandRowidColumn->mCommon.mPosition,
                                       sCandRowidColumn->mCommon.mExprPhraseFlag,
                                       0,
                                       sCandRowidColumn->mCommon.mColumnName,
                                       sInitRowidColumn,
                                       sInitExpr,
                                       aEnv );

            /* Rowid Column에 등록 */
            sPlanIndexCombine->mRowidColumn = sInitExpr;
        }

        /* Read Column Map List에 등록 */
        QLNC_ADD_READ_COLUMN_MAP( aConvExprParamInfo->mCandidateMem,
                                  sReadColMapList,
                                  aConvExprParamInfo->mCandExpr,
                                  sInitExpr,
                                  aEnv );

        /* Child Plan Array에 모두 등록함 */
        sBackupCurPlan = aConvExprParamInfo->mCurPlan;
        for( i = 0; i < sPlanIndexCombine->mChildCount; i++ )
        {
            aConvExprParamInfo->mCurPlan = sPlanIndexCombine->mChildPlanArr[i];

            /* @todo 현재 SubQuery가 포함된 Filter인 경우 Index Combine이 불가함 */
            STL_DASSERT( ( aConvExprParamInfo->mCurPlan->mPlanType != QLNC_PLAN_TYPE_SUB_QUERY_LIST ) &&
                         ( aConvExprParamInfo->mCurPlan->mPlanType != QLNC_PLAN_TYPE_SUB_QUERY_FILTER ) );

            STL_TRY( qlncAddReadColumnOnCurPlan_BaseTable( aConvExprParamInfo,
                                                           aEnv )
                     == STL_SUCCESS );
        }
        aConvExprParamInfo->mCurPlan = sBackupCurPlan;
    }


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Table의 Read Column List에 Column Expression을 추가한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncAddReadColumnOnCurPlan_SubTable( qlncConvExprParamInfo * aConvExprParamInfo,
                                               qllEnv               * aEnv )
{
    qlncPlanSubTable    * sPlanSubTable     = NULL;
    qlvInitExpression   * sInitExpr         = NULL;
    qlvRefExprList      * sReadColList      = NULL;
    qlvRefExprItem      * sReadColItem      = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    QLNC_CONVERT_EXPRESSION_DEFAULT_PARAM_VALIDATE( aConvExprParamInfo,
                                                    QLNC_EXPR_TYPE_INNER_COLUMN,
                                                    aEnv );


    /* Plan Sub Table 설정 */
    sPlanSubTable = (qlncPlanSubTable*)(aConvExprParamInfo->mCurPlan);

    /* Read Column List 설정 */
    STL_DASSERT( sPlanSubTable->mReadColList != NULL );
    sReadColList = sPlanSubTable->mReadColList;

    /* Find Expression */
    sInitExpr = NULL;
    sReadColItem = sReadColList->mHead;
    while( sReadColItem != NULL )
    {
        STL_DASSERT( sReadColItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
        if( ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mIdx == *(sReadColItem->mExprPtr->mExpr.mInnerColumn->mIdx) )
        {
            sInitExpr = sReadColItem->mExprPtr;
            break;
        }

        sReadColItem = sReadColItem->mNext;
    }

    /* Sub Table에 반드시 존재해야 한다. */
    STL_DASSERT( sInitExpr != NULL );


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Combine의 Read Column List에 Column Expression을 추가한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncAddReadColumnOnCurPlan_JoinCombine( qlncConvExprParamInfo * aConvExprParamInfo,
                                                  qllEnv                * aEnv )
{
    stlInt32              i;
    qlncPlanJoinCombine * sPlanJoinCombine = NULL;
    qlvInitExpression   * sInitExpr         = NULL;
    qlvInitExpression   * sCopiedExpr       = NULL;
    qlncReadColMap      * sReadColMap       = NULL;
    qlncReadColMapList  * sReadColMapList   = NULL;
    qlvRefExprList      * sReadColList      = NULL;
    qlncPlanCommon      * sBackupCurPlan    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvExprParamInfo->mRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCurPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_JOIN_COMBINE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN) ||
                        (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN) ||
                        (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN),
                        QLL_ERROR_STACK(aEnv) );


    /* Plan Join Combine 설정 */
    sPlanJoinCombine = (qlncPlanJoinCombine*)(aConvExprParamInfo->mCurPlan);

    /* Read Column Map List와 Read Column List 설정 */
    if( sPlanJoinCombine->mReadColMapList == NULL )
    {
        STL_DASSERT( sPlanJoinCombine->mReadColList == NULL );

        /* Read Column Map List 할당 */
        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mCandidateMem,
                                    STL_SIZEOF( qlncReadColMapList ),
                                    (void**) &sReadColMapList,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_LIST( sReadColMapList );
        sPlanJoinCombine->mReadColMapList = sReadColMapList;

        /* Read Column List 할당 */
        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvRefExprList ),
                                    (void**) &sReadColList,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_INIT_LIST( sReadColList );
        sPlanJoinCombine->mReadColList = sReadColList;
    }
    else
    {
        sReadColMapList = sPlanJoinCombine->mReadColMapList;
        sReadColList = sPlanJoinCombine->mReadColList;
    }

    /* Find Expression */
    sInitExpr = NULL;
    sReadColMap = sReadColMapList->mHead;
    while( sReadColMap != NULL )
    {
        if( sReadColMap->mCandExpr->mType == aConvExprParamInfo->mCandExpr->mType )
        {
            if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN )
            {
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN );
                if( ( ((qlncExprColumn*)(sReadColMap->mCandExpr))->mColumnID ==
                      ((qlncExprColumn*)(aConvExprParamInfo->mCandExpr))->mColumnID ) &&
                    ( ((qlncExprColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                      ((qlncExprColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID ) )
                {
                    /* 이미 등록된 Column Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }
            else if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN )
            {
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );
                if( ((qlncExprRowidColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                    ((qlncExprRowidColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID )
                {
                    /* 이미 등록된 Rowid Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }
            else
            {
                STL_DASSERT( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
                STL_DASSERT( (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_SUB_TABLE) ||
                             (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_SORT_INSTANT) ||
                             (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_HASH_INSTANT) );
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
                STL_DASSERT( (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                             QLNC_NODE_TYPE_SUB_TABLE) ||
                             (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                             QLNC_NODE_TYPE_SORT_INSTANT) ||
                             (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                             QLNC_NODE_TYPE_HASH_INSTANT) );

                if( ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mIdx ==
                      ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mIdx ) &&
                    ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                      ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID ) )
                {
                    /* 이미 등록된 Inner Column Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }
        }

        sReadColMap = sReadColMap->mNext;
    }

    /* 등록되지 않았다면 등록함 */
    if( sInitExpr == NULL )
    {
        /* Child Plan Array에 모두 등록함 */
        sBackupCurPlan = aConvExprParamInfo->mCurPlan;
        for( i = 0; i < sPlanJoinCombine->mChildCount; i++ )
        {
            aConvExprParamInfo->mCurPlan = sPlanJoinCombine->mChildPlanArr[i];

            while( STL_TRUE )
            {
                if( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_LIST )
                {
                    aConvExprParamInfo->mCurPlan =
                        ((qlncPlanSubQueryList*)(aConvExprParamInfo->mCurPlan))->mChildPlanNode;
                }
                else if( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_FILTER )
                {
                    aConvExprParamInfo->mCurPlan =
                        ((qlncPlanSubQueryFilter*)(aConvExprParamInfo->mCurPlan))->mChildPlanNodes[ 0 ];
                    STL_DASSERT( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_LIST );
                    aConvExprParamInfo->mCurPlan =
                        ((qlncPlanSubQueryList*)(aConvExprParamInfo->mCurPlan))->mChildPlanNode;
                }
                else
                {
                    break;
                }
            }

            STL_TRY( qlncAddReadColumnOnCurPlan_JoinTable( aConvExprParamInfo,
                                                           aEnv )
                     == STL_SUCCESS );

            if( i == 0 )
            {
                sInitExpr = aConvExprParamInfo->mInitExpr;
            }
        }
        aConvExprParamInfo->mCurPlan = sBackupCurPlan;

        /* Read Column List에 등록 */
        STL_TRY( qlvCopyExpr( sInitExpr,
                              &sCopiedExpr,
                              aConvExprParamInfo->mRegionMem,
                              aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlvAddExprToRefColExprList( sReadColList,
                                             sCopiedExpr,
                                             aConvExprParamInfo->mRegionMem,
                                             aEnv )
                 == STL_SUCCESS );

        /* Read Column Map List에 등록 */
        QLNC_ADD_READ_COLUMN_MAP( aConvExprParamInfo->mCandidateMem,
                                  sReadColMapList,
                                  aConvExprParamInfo->mCandExpr,
                                  sInitExpr,
                                  aEnv );
    }


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Table의 Read Column List에 Column Expression을 추가한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncAddReadColumnOnCurPlan_JoinTable( qlncConvExprParamInfo * aConvExprParamInfo,
                                                qllEnv              * aEnv )
{
    qlvInitExpression   * sInitExpr         = NULL;
    qlncReadColMap      * sReadColMap       = NULL;
    qlncReadColMapList  * sReadColMapList   = NULL;
    qlvRefExprList      * sReadColList      = NULL;
    qlncPlanCommon      * sLeftPlanNode     = NULL;
    qlncPlanCommon      * sRightPlanNode    = NULL;
    qlncNodeCommon      * sCandNodeOfExpr   = NULL;
    qlvInitInnerColumn  * sInitInnerColumn  = NULL;
    qlncPlanCommon      * sBackupCurPlan    = NULL;
    stlBool               sIsLeft;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvExprParamInfo->mRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCurPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN) ||
                        (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN) ||
                        (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN),
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Macro 정의
     */

#define _QLNC_SET_READ_COL_MAP_LIST_AND_READ_COL_LIST_ON_JOIN_TABLE( _aStructName )             \
    {                                                                                           \
        if( ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mReadColMapList == NULL )          \
        {                                                                                       \
            /* Read Column Map List 할당 */                                                     \
            STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mCandidateMem,                      \
                                        STL_SIZEOF( qlncReadColMapList ),                       \
                                        (void**) &sReadColMapList,                              \
                                        KNL_ENV(aEnv) )                                         \
                     == STL_SUCCESS );                                                          \
            QLNC_INIT_LIST( sReadColMapList );                                                  \
            ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mReadColMapList = sReadColMapList; \
            sReadColList = ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mReadColList;       \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            sReadColMapList = ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mReadColMapList; \
            sReadColList = ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mReadColList;       \
        }                                                                                       \
                                                                                                \
        sLeftPlanNode = ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mLeftChildPlanNode;    \
        sRightPlanNode = ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mRightChildPlanNode;  \
    }


    /**
     * Plan Type 별 Read Column Map List와 Read Column List 설정
     */

    if( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_NESTED_JOIN )
    {
        _QLNC_SET_READ_COL_MAP_LIST_AND_READ_COL_LIST_ON_JOIN_TABLE( qlncPlanNestedJoin );
    }
    else if( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_MERGE_JOIN )
    {
        _QLNC_SET_READ_COL_MAP_LIST_AND_READ_COL_LIST_ON_JOIN_TABLE( qlncPlanMergeJoin );
    }
    else
    {
        STL_DASSERT( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_HASH_JOIN );
        _QLNC_SET_READ_COL_MAP_LIST_AND_READ_COL_LIST_ON_JOIN_TABLE( qlncPlanHashJoin );
    }

    /* Find Expression */
    sInitExpr = NULL;
    sReadColMap = sReadColMapList->mHead;
    while( sReadColMap != NULL )
    {
        if( sReadColMap->mCandExpr->mType == aConvExprParamInfo->mCandExpr->mType )
        {
            if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN )
            {
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN );
                if( ( ((qlncExprColumn*)(sReadColMap->mCandExpr))->mColumnID ==
                      ((qlncExprColumn*)(aConvExprParamInfo->mCandExpr))->mColumnID ) &&
                    ( ((qlncExprColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                      ((qlncExprColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID ) )
                {
                    /* 이미 등록된 Column Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }
            else if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN )
            {
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );
                if( ((qlncExprRowidColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                    ((qlncExprRowidColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID )
                {
                    /* 이미 등록된 Rowid Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }
            else
            {
                STL_DASSERT( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
                STL_DASSERT( (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_SUB_TABLE) ||
                             (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_SORT_INSTANT) ||
                             (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_HASH_INSTANT) );
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
                STL_DASSERT( (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                             QLNC_NODE_TYPE_SUB_TABLE) ||
                             (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                             QLNC_NODE_TYPE_SORT_INSTANT) ||
                             (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                             QLNC_NODE_TYPE_HASH_INSTANT) );

                if( ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mIdx ==
                      ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mIdx ) &&
                    ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                      ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID ) )
                {
                    /* 이미 등록된 Inner Column Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }
        }

        sReadColMap = sReadColMap->mNext;
    }

    /* 등록되지 않았다면 등록함 */
    if( sInitExpr == NULL )
    {
        /* Candidate Expression의 Node 찾기 */
        if( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN )
        {
            sCandNodeOfExpr = ((qlncExprColumn*)(aConvExprParamInfo->mCandExpr))->mNode;
        }
        else if( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN )
        {
            sCandNodeOfExpr = ((qlncExprRowidColumn*)(aConvExprParamInfo->mCandExpr))->mNode;
        }
        else
        {
            STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
            sCandNodeOfExpr = ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode;
        }

        /* 하위 노드에서 해당 Column의 Init Expression 가져오기 */
        if( qlncIsExistCandNodeOnPlanNode( sLeftPlanNode, sCandNodeOfExpr ) == STL_TRUE )
        {
            sBackupCurPlan = aConvExprParamInfo->mCurPlan;
            aConvExprParamInfo->mCurPlan = sLeftPlanNode;
            STL_TRY( qlncAddReadColumnOnCurPlan( aConvExprParamInfo,
                                                 aEnv )
                     == STL_SUCCESS );
            sInitExpr = aConvExprParamInfo->mInitExpr;
            aConvExprParamInfo->mCurPlan = sBackupCurPlan;

            sIsLeft = STL_TRUE;
        }
        else
        {
            STL_DASSERT( qlncIsExistCandNodeOnPlanNode( sRightPlanNode, sCandNodeOfExpr ) ==
                         STL_TRUE );

            sBackupCurPlan = aConvExprParamInfo->mCurPlan;
            aConvExprParamInfo->mCurPlan = sRightPlanNode;
            STL_TRY( qlncAddReadColumnOnCurPlan( aConvExprParamInfo,
                                                 aEnv )
                     == STL_SUCCESS );
            sInitExpr = aConvExprParamInfo->mInitExpr;
            aConvExprParamInfo->mCurPlan = sBackupCurPlan;

            sIsLeft = STL_FALSE;
        }

        /* Init Inner Column 할당 */
        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                    STL_SIZEOF( qlvInitInnerColumn ),
                                    (void**) &sInitInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Relation Node 설정 */
        sInitInnerColumn->mRelationNode = aConvExprParamInfo->mCurPlan->mInitNode;

        /* Column Idx 할당 */
        STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                    STL_SIZEOF( stlInt32 ),
                                    (void**) &(sInitInnerColumn->mIdx),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Column Idx 설정 */
        *(sInitInnerColumn->mIdx) = sReadColList->mCount;

        /* Original Expression 설정 */
        sInitInnerColumn->mOrgExpr = sInitExpr;

        /* Init Expression 생성 */
        QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
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
        STL_TRY( qlvAddExprToRefColExprList( sReadColList,
                                             sInitExpr,
                                             aConvExprParamInfo->mRegionMem,
                                             aEnv )
                 == STL_SUCCESS );

        /* Left/Right 여부 설정 (반드시 설정해야 한다!) */
        sReadColList->mTail->mIsLeft = sIsLeft;

        /* Read Column Map List에 등록 */
        QLNC_ADD_READ_COLUMN_MAP( aConvExprParamInfo->mCandidateMem,
                                  sReadColMapList,
                                  aConvExprParamInfo->mCandExpr,
                                  sInitExpr,
                                  aEnv );
    }


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Instant의 Read Column List에 Column Expression을 추가한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncAddReadColumnOnCurPlan_Instant( qlncConvExprParamInfo * aConvExprParamInfo,
                                              qllEnv                * aEnv )
{
    stlInt32              sNewIdx;
    qlvInitExpression   * sInitExpr         = NULL;
    qlvInitExpression   * sCopiedInitExpr   = NULL;
    qlncReadColMap      * sReadColMap       = NULL;
    qlncReadColMapList  * sKeyColMapList    = NULL;
    qlncReadColMapList  * sRecColMapList    = NULL;
    qlncReadColMapList  * sReadColMapList   = NULL;
    qlvRefExprList      * sRecColList       = NULL;
    qlvRefExprList      * sReadColList      = NULL;
    qlncPlanCommon      * sChildPlanNode    = NULL;
    qlvInitInnerColumn  * sInitInnerColumn  = NULL;
    qlncPlanCommon      * sBackupCurPlan    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvExprParamInfo->mRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCurPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN) ||
                        (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN) ||
                        (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN),
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Macro 정의
     */

#define _QLNC_SET_READ_COL_MAP_LIST_AND_READ_COL_LIST_ON_INSTANT( _aStructName )                \
    {                                                                                           \
        if( ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mReadColMapList == NULL )          \
        {                                                                                       \
            STL_DASSERT( ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mKeyColMapList != NULL );\
            STL_DASSERT( ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mKeyColList != NULL );\
                                                                                                \
            /* Record Column Map List 할당 */                                                   \
            if( ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mRecColMapList == NULL )       \
            {                                                                                   \
                STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mCandidateMem,                  \
                                            STL_SIZEOF( qlncReadColMapList ),                   \
                                            (void**) &sRecColMapList,                           \
                                            KNL_ENV(aEnv) )                                     \
                         == STL_SUCCESS );                                                      \
                QLNC_INIT_LIST( sRecColMapList );                                               \
                ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mRecColMapList = sRecColMapList;\
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                sRecColMapList = ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mRecColMapList;\
            }                                                                                   \
                                                                                                \
            /* Read Column Map List 할당 */                                                     \
            STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mCandidateMem,                      \
                                        STL_SIZEOF( qlncReadColMapList ),                       \
                                        (void**) &sReadColMapList,                              \
                                        KNL_ENV(aEnv) )                                         \
                     == STL_SUCCESS );                                                          \
            QLNC_INIT_LIST( sReadColMapList );                                                  \
            ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mReadColMapList = sReadColMapList; \
                                                                                                \
            /* Rec Column List 설정 */                                                          \
            sRecColList = ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mRecColList;         \
                                                                                                \
            /* Read Column List 설정 */                                                         \
            sReadColList = ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mReadColList;       \
                                                                                                \
            /* Key Column Map List 설정 */                                                      \
            sKeyColMapList = ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mKeyColMapList;   \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            sKeyColMapList = ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mKeyColMapList;   \
            sRecColMapList = ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mRecColMapList;   \
            sReadColMapList = ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mReadColMapList; \
            sRecColList = ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mRecColList;         \
            sReadColList = ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mReadColList;       \
        }                                                                                       \
                                                                                                \
        sChildPlanNode = ((_aStructName*)(aConvExprParamInfo->mCurPlan))->mChildPlanNode;       \
    }


    /**
     * Plan Type 별 Read Column Map List와 Read Column List 설정
     */

    if( (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SORT_INSTANT) ||
        (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SORT_GROUP_INSTANT) ||
        (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SORT_JOIN_INSTANT) )
    {
        _QLNC_SET_READ_COL_MAP_LIST_AND_READ_COL_LIST_ON_INSTANT( qlncPlanSortInstant );
        sNewIdx =
            ((qlncPlanSortInstant*)(aConvExprParamInfo->mCurPlan))->mKeyColList->mCount +
            ((qlncPlanSortInstant*)(aConvExprParamInfo->mCurPlan))->mRecColList->mCount;
    }
    else if( (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_HASH_INSTANT) ||
             (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_HASH_GROUP_INSTANT) ||
             (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_HASH_JOIN_INSTANT) )
    {
        _QLNC_SET_READ_COL_MAP_LIST_AND_READ_COL_LIST_ON_INSTANT( qlncPlanHashInstant );
        sNewIdx =
            ((qlncPlanHashInstant*)(aConvExprParamInfo->mCurPlan))->mKeyColList->mCount +
            ((qlncPlanHashInstant*)(aConvExprParamInfo->mCurPlan))->mRecColList->mCount;
    }
    else
    {
        STL_DASSERT( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_GROUP );
        _QLNC_SET_READ_COL_MAP_LIST_AND_READ_COL_LIST_ON_INSTANT( qlncPlanGroup );
        sNewIdx =
            ((qlncPlanGroup*)(aConvExprParamInfo->mCurPlan))->mKeyColList->mCount +
            ((qlncPlanGroup*)(aConvExprParamInfo->mCurPlan))->mRecColList->mCount;
    }

    /* Find Expression */
    sInitExpr = NULL;
    sReadColMap = sReadColMapList->mHead;
    while( sReadColMap != NULL )
    {
        if( sReadColMap->mCandExpr->mType == aConvExprParamInfo->mCandExpr->mType )
        {
            if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN )
            {
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN );
                if( ( ((qlncExprColumn*)(sReadColMap->mCandExpr))->mColumnID ==
                      ((qlncExprColumn*)(aConvExprParamInfo->mCandExpr))->mColumnID ) &&
                    ( ((qlncExprColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                      ((qlncExprColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID ) )
                {
                    /* 이미 등록된 Column Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }
            else if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN )
            {
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );
                if( ((qlncExprRowidColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                    ((qlncExprRowidColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID )
                {
                    /* 이미 등록된 Rowid Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }
            else
            {
                STL_DASSERT( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
                STL_DASSERT( (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_SUB_TABLE) ||
                             (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_SORT_INSTANT) ||
                             (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_HASH_INSTANT) );
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
                STL_DASSERT( (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                             QLNC_NODE_TYPE_SUB_TABLE) ||
                             (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                             QLNC_NODE_TYPE_SORT_INSTANT) ||
                             (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                             QLNC_NODE_TYPE_HASH_INSTANT) );

                if( ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mIdx ==
                      ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mIdx ) &&
                    ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                      ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID ) )
                {
                    /* 이미 등록된 Inner Column Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }
        }

        sReadColMap = sReadColMap->mNext;
    }

    /* 등록되지 않았다면 등록함 */
    if( sInitExpr == NULL )
    {
        /* Key Column List에 등록되어 있는지 체크 */
        sReadColMap = sKeyColMapList->mHead;
        while( sReadColMap != NULL )
        {
            if( sReadColMap->mCandExpr->mType == aConvExprParamInfo->mCandExpr->mType )
            {
                if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN )
                {
                    STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN );
                    if( ( ((qlncExprColumn*)(sReadColMap->mCandExpr))->mColumnID ==
                          ((qlncExprColumn*)(aConvExprParamInfo->mCandExpr))->mColumnID ) &&
                        ( ((qlncExprColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                          ((qlncExprColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID ) )
                    {
                        /* 이미 등록된 Column Expression */
                        sInitExpr = sReadColMap->mInitExpr;
                        break;
                    }
                }
                else if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN )
                {
                    STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );
                    if( ((qlncExprRowidColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                        ((qlncExprRowidColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID )
                    {
                        /* 이미 등록된 Rowid Expression */
                        sInitExpr = sReadColMap->mInitExpr;
                        break;
                    }
                }
                else if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN )
                {
                    STL_DASSERT( (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                                  QLNC_NODE_TYPE_SUB_TABLE) ||
                                 (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                                  QLNC_NODE_TYPE_SORT_INSTANT) ||
                                 (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                                  QLNC_NODE_TYPE_HASH_INSTANT) );
                    STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
                    STL_DASSERT( (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                                  QLNC_NODE_TYPE_SUB_TABLE) ||
                                 (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                                  QLNC_NODE_TYPE_SORT_INSTANT) ||
                                 (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                                  QLNC_NODE_TYPE_HASH_INSTANT) );

                    if( ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mIdx ==
                          ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mIdx ) &&
                        ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                          ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID ) )
                    {
                        /* 이미 등록된 Inner Column Expression */
                        sInitExpr = sReadColMap->mInitExpr;
                        break;
                    }
                }
            }

            sReadColMap = sReadColMap->mNext;
        }

        if( sInitExpr == NULL )
        {
            /* Record Column List에 등록되어 있는지 체크 */
            sReadColMap = sRecColMapList->mHead;
            while( sReadColMap != NULL )
            {
                if( sReadColMap->mCandExpr->mType == aConvExprParamInfo->mCandExpr->mType )
                {
                    if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN )
                    {
                        /* @todo 현재 여기에 들어올 수 없는 것으로 판단됨 */

                        STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN );
                        if( ( ((qlncExprColumn*)(sReadColMap->mCandExpr))->mColumnID ==
                              ((qlncExprColumn*)(aConvExprParamInfo->mCandExpr))->mColumnID ) &&
                            ( ((qlncExprColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                              ((qlncExprColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID ) )
                        {
                            /* 이미 등록된 Column Expression */
                            sInitExpr = sReadColMap->mInitExpr;
                            break;
                        }
                    }
                    else if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN )
                    {
                        /* @todo 현재 여기에 들어올 수 없는 것으로 판단됨 */

                        STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );
                        if( ((qlncExprRowidColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                            ((qlncExprRowidColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID )
                        {
                            /* 이미 등록된 Rowid Expression */
                            sInitExpr = sReadColMap->mInitExpr;
                            break;
                        }
                    }
                    else if( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN )
                    {
                        STL_DASSERT( (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                                      QLNC_NODE_TYPE_SUB_TABLE) ||
                                     (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                                      QLNC_NODE_TYPE_SORT_INSTANT) ||
                                     (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                                      QLNC_NODE_TYPE_HASH_INSTANT) );
                        STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
                        STL_DASSERT( (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                                      QLNC_NODE_TYPE_SUB_TABLE) ||
                                     (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                                      QLNC_NODE_TYPE_SORT_INSTANT) ||
                                     (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                                      QLNC_NODE_TYPE_HASH_INSTANT) );

                        if( ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mIdx ==
                              ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mIdx ) &&
                            ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                              ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID ) )
                        {
                            /* 이미 등록된 Inner Column Expression */
                            sInitExpr = sReadColMap->mInitExpr;
                            break;
                        }
                    }
                }

                sReadColMap = sReadColMap->mNext;
            }
        }

        if( sInitExpr == NULL )
        {
            /* Key Column List 및 Record Column List에 등록되지 않은 경우 */

            /* Child Node에 등록 */
            sBackupCurPlan = aConvExprParamInfo->mCurPlan;
            aConvExprParamInfo->mCurPlan = sChildPlanNode;
            STL_TRY( qlncAddReadColumnOnCurPlan( aConvExprParamInfo,
                                                 aEnv )
                     == STL_SUCCESS );
            sInitExpr = aConvExprParamInfo->mInitExpr;
            aConvExprParamInfo->mCurPlan = sBackupCurPlan;

            /* Init Inner Column 할당 */
            STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                        STL_SIZEOF( qlvInitInnerColumn ),
                                        (void**) &sInitInnerColumn,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /* Relation Node 설정 */
            sInitInnerColumn->mRelationNode = aConvExprParamInfo->mCurPlan->mInitNode;

            /* Column Idx 할당 */
            STL_TRY( knlAllocRegionMem( aConvExprParamInfo->mRegionMem,
                                        STL_SIZEOF( stlInt32 ),
                                        (void**) &(sInitInnerColumn->mIdx),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /* Column Idx 설정 */
            *(sInitInnerColumn->mIdx) = sNewIdx;

            /* Original Expression 설정 */
            sInitInnerColumn->mOrgExpr = sInitExpr;

            /* Init Expression 생성 */
            QLNC_MAKE_INIT_EXPRESSION( aConvExprParamInfo->mRegionMem,
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

            /* Record Column List에 등록 */
            STL_TRY( qlvAddExprToRefColExprList( sRecColList,
                                                 sInitExpr,
                                                 aConvExprParamInfo->mRegionMem,
                                                 aEnv )
                     == STL_SUCCESS );

            if( (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SORT_INSTANT) ||
                (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SORT_GROUP_INSTANT) ||
                (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SORT_JOIN_INSTANT) )
            {
                ((qlncPlanSortInstant*)(aConvExprParamInfo->mCurPlan))->mSortTableAttr.mTopDown = STL_FALSE;
                ((qlncPlanSortInstant*)(aConvExprParamInfo->mCurPlan))->mSortTableAttr.mDistinct = STL_FALSE;
            }
            else if( (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_HASH_INSTANT) ||
                     (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_HASH_GROUP_INSTANT) ||
                     (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_HASH_JOIN_INSTANT) )
            {
                ((qlncPlanHashInstant*)(aConvExprParamInfo->mCurPlan))->mHashTableAttr.mUniquenessFlag = STL_FALSE;
            }
            else
            {
                STL_DASSERT( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_GROUP );
                /* Do Nothing */
            }
        }
        else
        {
            /* Key Column List에 이미 등록된 경우 */
        }

        /* Init Expression 복사 */
        /* STL_TRY( qlvCopyExpr( sInitExpr, */
        /*                       &sCopiedInitExpr, */
        /*                       aConvExprParamInfo->mRegionMem, */
        /*                       aEnv ) */
        /*          == STL_SUCCESS ); */

        sCopiedInitExpr = sInitExpr;

        /* Read Column List에 등록 */
        STL_TRY( qlvAddExprToRefColExprList( sReadColList,
                                             sCopiedInitExpr,
                                             aConvExprParamInfo->mRegionMem,
                                             aEnv )
                 == STL_SUCCESS );

        /* Read Column Map List에 등록 */
        QLNC_ADD_READ_COLUMN_MAP( aConvExprParamInfo->mCandidateMem,
                                  sReadColMapList,
                                  aConvExprParamInfo->mCandExpr,
                                  sCopiedInitExpr,
                                  aEnv );

        sInitExpr = sCopiedInitExpr;
    }


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Instant의 Key/Record Column List에서 Expression을 찾는다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncAddReadColumnOnCurPlan_InstantForSelfRef( qlncConvExprParamInfo   * aConvExprParamInfo,
                                                        qllEnv                  * aEnv )
{
    qlvInitExpression   * sInitExpr         = NULL;
    qlncReadColMapList  * sKeyColMapList    = NULL;
    qlncReadColMapList  * sRecColMapList    = NULL;
    qlncReadColMap      * sReadColMap       = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvExprParamInfo->mRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCurPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Plan Type 별 Key Column Map List와 Record Column Map List 설정
     */

    if( (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SORT_INSTANT) ||
        (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SORT_GROUP_INSTANT) ||
        (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SORT_JOIN_INSTANT) )
    {
        /* @todo 현재 여기에 들어오는 경우가 없는 것으로 판단됨 */

        sKeyColMapList = ((qlncPlanSortInstant*)(aConvExprParamInfo->mCurPlan))->mKeyColMapList;
        sRecColMapList = ((qlncPlanSortInstant*)(aConvExprParamInfo->mCurPlan))->mRecColMapList;
    }
    else if( (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_HASH_INSTANT) ||
             (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_HASH_GROUP_INSTANT) ||
             (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_HASH_JOIN_INSTANT) )
    {
        sKeyColMapList = ((qlncPlanHashInstant*)(aConvExprParamInfo->mCurPlan))->mKeyColMapList;
        sRecColMapList = ((qlncPlanHashInstant*)(aConvExprParamInfo->mCurPlan))->mRecColMapList;
    }
    else
    {
        STL_DASSERT( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_GROUP );
        sKeyColMapList = ((qlncPlanGroup*)(aConvExprParamInfo->mCurPlan))->mKeyColMapList;
        sRecColMapList = ((qlncPlanGroup*)(aConvExprParamInfo->mCurPlan))->mRecColMapList;
    }

    STL_DASSERT( sKeyColMapList != NULL );
    sInitExpr = NULL;

    /* Key Column List에 등록되어 있는지 체크 */
    sReadColMap = sKeyColMapList->mHead;
    while( sReadColMap != NULL )
    {
        if( (sReadColMap->mCandExpr->mType == aConvExprParamInfo->mCandExpr->mType) &&
            (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType != QLNC_NODE_TYPE_SUB_TABLE) )
        {
            STL_DASSERT( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
            STL_DASSERT( (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                          QLNC_NODE_TYPE_SORT_INSTANT) ||
                         (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                          QLNC_NODE_TYPE_HASH_INSTANT) );
            STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
            STL_DASSERT( (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                          QLNC_NODE_TYPE_SORT_INSTANT) ||
                         (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                          QLNC_NODE_TYPE_HASH_INSTANT) );

            if( ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mIdx ==
                  ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mIdx ) &&
                ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                  ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID ) )
            {
                /* 이미 등록된 Inner Column Expression */
                sInitExpr = sReadColMap->mInitExpr;
                break;
            }
        }

        sReadColMap = sReadColMap->mNext;
    }

    if( (sInitExpr == NULL) &&
        (sRecColMapList != NULL) )
    {
        /* @todo 현재 여기에 들어오는 경우가 없는 것으로 판단됨 */

        /* Record Column List에 등록되어 있는지 체크 */
        sReadColMap = sRecColMapList->mHead;
        while( sReadColMap != NULL )
        {
            if( (sReadColMap->mCandExpr->mType == aConvExprParamInfo->mCandExpr->mType) &&
                (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType != QLNC_NODE_TYPE_SUB_TABLE) )
            {
                STL_DASSERT( sReadColMap->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
                STL_DASSERT( (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_SORT_INSTANT) ||
                             (((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_HASH_INSTANT) );
                STL_DASSERT( aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
                STL_DASSERT( (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_SORT_INSTANT) ||
                             (((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeType ==
                              QLNC_NODE_TYPE_HASH_INSTANT) );

                if( ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mIdx ==
                      ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mIdx ) &&
                    ( ((qlncExprInnerColumn*)(sReadColMap->mCandExpr))->mNode->mNodeID ==
                      ((qlncExprInnerColumn*)(aConvExprParamInfo->mCandExpr))->mNode->mNodeID ) )
                {
                    /* 이미 등록된 Inner Column Expression */
                    sInitExpr = sReadColMap->mInitExpr;
                    break;
                }
            }

            sReadColMap = sReadColMap->mNext;
        }
    }

    STL_DASSERT( sInitExpr != NULL );


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief By Pass 형태의 Plan Node에 Column Expression을 추가한다.
 * @param[in]   aConvExprParamInfo  Convert Expression Parameter Info
 * @param[in]   aEnv                Environment
 */
stlStatus qlncAddReadColumnOnCurPlan_ByPass( qlncConvExprParamInfo  * aConvExprParamInfo,
                                             qllEnv                 * aEnv )
{
    qlvInitExpression   * sInitExpr         = NULL;
    qlncPlanCommon      * sBackupCurPlan    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvExprParamInfo->mRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCurPlan != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_LIST) ||
                        (aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_FILTER),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvExprParamInfo->mCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_COLUMN) ||
                        (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN) ||
                        (aConvExprParamInfo->mCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN),
                        QLL_ERROR_STACK(aEnv) );


    if( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_LIST )
    {
        sBackupCurPlan = aConvExprParamInfo->mCurPlan;
        aConvExprParamInfo->mCurPlan =
            ((qlncPlanSubQueryList*)(aConvExprParamInfo->mCurPlan))->mChildPlanNode;
        STL_TRY( qlncAddReadColumnOnCurPlan( aConvExprParamInfo,
                                             aEnv )
                 == STL_SUCCESS );
        sInitExpr = aConvExprParamInfo->mInitExpr;
        aConvExprParamInfo->mCurPlan = sBackupCurPlan;
    }
    else
    {
        STL_DASSERT( aConvExprParamInfo->mCurPlan->mPlanType == QLNC_PLAN_TYPE_SUB_QUERY_FILTER );

        sBackupCurPlan = aConvExprParamInfo->mCurPlan;
        aConvExprParamInfo->mCurPlan =
            ((qlncPlanSubQueryFilter*)(aConvExprParamInfo->mCurPlan))->mChildPlanNodes[ 0 ];
        STL_TRY( qlncAddReadColumnOnCurPlan( aConvExprParamInfo,
                                             aEnv )
                 == STL_SUCCESS );
        sInitExpr = aConvExprParamInfo->mInitExpr;
        aConvExprParamInfo->mCurPlan = sBackupCurPlan;
    }

    /* 반드시 존재해야 한다. */
    STL_DASSERT( sInitExpr != NULL );


    /**
     * Output 설정
     */

    aConvExprParamInfo->mInitExpr = sInitExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */

