/*******************************************************************************
 * qlncExpr.c
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
 * @file qlncExpr.c
 * @brief SQL Processor Layer SELECT statement candidate optimization
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Convert Expression Function Pointer List
 */
const qlncConvertExprFuncPtr qlncConvertExprFuncList[ QLV_EXPR_TYPE_MAX ] =
{
    qlncConvertExprValue,               /**< QLV_EXPR_TYPE_VALUE */
    qlncConvertExprBindParam,           /**< QLV_EXPR_TYPE_BIND_PARAM */
    qlncConvertExprColumn,              /**< QLV_EXPR_TYPE_COLUMN */
    qlncConvertExprFunction,            /**< QLV_EXPR_TYPE_FUNCTION */
    qlncConvertExprTypeCast,            /**< QLV_EXPR_TYPE_CAST */
    qlncConvertExprPseudoCol,           /**< QLV_EXPR_TYPE_PSEUDO_COLUMN */
    qlncConvertExprPseudoArg,           /**< QLV_EXPR_TYPE_PSEUDO_ARGUMENT */
    qlncConvertExprConstExpr,           /**< QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT */
    qlncConvertExprReference,           /**< QLV_EXPR_TYPE_REFERENCE */
    qlncConvertExprSubQuery,            /**< QLV_EXPR_TYPE_SUB_QUERY */
    qlncConvertExprInnerColumn,         /**< QLV_EXPR_TYPE_INNER_COLUMN */
    qlncConvertExprOuterColumn,         /**< QLV_EXPR_TYPE_OUTER_COLUMN */
    qlncConvertExprRowidColumn,         /**< QLV_EXPR_TYPE_ROWID_COLUMN */
    qlncConvertExprAggregation,         /**< QLV_EXPR_TYPE_AGGREGATION */
    qlncConvertExprCaseExpr,            /**< QLV_EXPR_TYPE_CASE_EXPR */
    qlncConvertExprListFunction,        /**< QLV_EXPR_TYPE_LIST_FUNCTION */
    qlncConvertExprListColumn,          /**< QLV_EXPR_TYPE_LIST_COLUMN */
    qlncConvertExprRowExpr              /**< QLV_EXPR_TYPE_ROW_EXPR */
};


/**
 * @brief Value Expression을 convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprValue( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                qllEnv                      * aEnv )
{
    qlvInitValue        * sInitValue    = NULL;
    qlncExprValue       * sCandValue    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_VALUE );
    sInitValue = aConvertExprParamInfo->mInitExpr->mExpr.mValue;

    /* Candidate Value Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprValue ),
                                (void**) &sCandValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Value Expression 정보 셋팅 */
    sCandValue->mValueType = (qlncValueType)sInitValue->mValueType;
    sCandValue->mInternalBindIdx = sInitValue->mInternalBindIdx;
    
    if( (sInitValue->mValueType == QLV_VALUE_TYPE_BOOLEAN) ||
        (sInitValue->mValueType == QLV_VALUE_TYPE_BINARY_INTEGER) )
    {
        sCandValue->mData.mInteger = sInitValue->mData.mInteger;
    }
    else
    {
        QLNC_ALLOC_AND_COPY_STRING( sCandValue->mData.mString,
                                    sInitValue->mData.mString,
                                    &QLL_CANDIDATE_MEM( aEnv ),
                                    aEnv );
    }

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandValue->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_VALUE,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          aConvertExprParamInfo->mInitExpr->mIterationTime,
                          aConvertExprParamInfo->mInitExpr->mPosition,
                          aConvertExprParamInfo->mInitExpr->mColumnName,
                          DTL_TYPE_NA );

    /* DB Type 설정 */
    STL_TRY( qloGetValueDataType( sInitValue,
                                  &(sCandValue->mCommon.mDataType),
                                  aEnv )
             == STL_SUCCESS );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandValue;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Bind Param Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprBindParam( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                    qllEnv                      * aEnv )
{
    qlvInitBindParam    * sInitBindParam    = NULL;
    qlncExprBindParam   * sCandBindParam    = NULL;
    knlValueBlockList   * sValueBlock       = NULL;

    knlBindType   sUserBindType = KNL_BIND_TYPE_INVALID;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_BIND_PARAM );
    sInitBindParam = aConvertExprParamInfo->mInitExpr->mExpr.mBindParam;

    /* Candidate Bind Param Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprBindParam ),
                                (void**) &sCandBindParam,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /* Bind Param Expression 정보 셋팅 */
    sCandBindParam->mBindType = sInitBindParam->mBindType;
    sCandBindParam->mBindParamIdx = sInitBindParam->mBindParamIdx;

    QLNC_ALLOC_AND_COPY_STRING( sCandBindParam->mHostName,
                                sInitBindParam->mHostName,
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );

    QLNC_ALLOC_AND_COPY_STRING( sCandBindParam->mIndicatorName,
                                sInitBindParam->mIndicatorName,
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandBindParam->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_BIND_PARAM,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          aConvertExprParamInfo->mInitExpr->mIterationTime,
                          aConvertExprParamInfo->mInitExpr->mPosition,
                          aConvertExprParamInfo->mInitExpr->mColumnName,
                          DTL_TYPE_NA );

    /* IN Bind Type 에 부합하는 Bind Type 이어야 함. */
    STL_TRY( knlGetBindParamType( aConvertExprParamInfo->mParamInfo.mSQLStmt->mBindContext,
                                  sInitBindParam->mBindParamIdx + 1,
                                  & sUserBindType,
                                  KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY_THROW( (sUserBindType == KNL_BIND_TYPE_IN) || (sUserBindType == KNL_BIND_TYPE_INOUT),
                   RAMP_BINDTYPE_MISMATCH );

    /* DB Type 설정 */
    STL_TRY( knlGetExecuteParamValueBlock( aConvertExprParamInfo->mParamInfo.mSQLStmt->mBindContext,
                                           sInitBindParam->mBindParamIdx + 1,
                                           &sValueBlock,
                                           KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sValueBlock != NULL, RAMP_ERR_BIND_NOT_EXIST );

    sCandBindParam->mCommon.mDataType = KNL_GET_BLOCK_DB_TYPE( sValueBlock );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandBindParam;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_BIND_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BIND_VARIABLE_NOT_EXIST,
                      qlgMakeErrPosString( aConvertExprParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                           aConvertExprParamInfo->mInitExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_BINDTYPE_MISMATCH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BIND_TYPE_MISMATCH,
                      qlgMakeErrPosString( aConvertExprParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                           aConvertExprParamInfo->mInitExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sInitBindParam->mBindParamIdx + 1 );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Column Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprColumn( qlncConvertExprParamInfo   * aConvertExprParamInfo,
                                 qllEnv                     * aEnv )
{
    qlvInitColumn   * sInitColumn   = NULL;
    qlncExprColumn  * sCandColumn   = NULL;
    stlBool           sIsOuterColumn;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_COLUMN );
    sInitColumn = aConvertExprParamInfo->mInitExpr->mExpr.mColumn;

    /* Candidate Column Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprColumn ),
                                (void**) &sCandColumn,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /* Column Expression 정보 셋팅 */
    sCandColumn->mNode =
        qlncGetCandNodePtrFromInitNodePtr( aConvertExprParamInfo->mTableMapArr,
                                           sInitColumn->mRelationNode );

    if( sCandColumn->mNode == NULL )
    {
        /* Outer Table을 참조하는 경우 */
        sCandColumn->mNode =
            qlncGetCandNodePtrFromInitNodePtr( aConvertExprParamInfo->mOuterTableMapArr,
                                               sInitColumn->mRelationNode );
        STL_DASSERT( sCandColumn->mNode != NULL );

        STL_TRY( qlncSetRefColumnInfoOnExprSubQuery(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     (qlncExprCommon*)sCandColumn,
                     aConvertExprParamInfo->mExprSubQuery,
                     aEnv )
                 == STL_SUCCESS );

        sIsOuterColumn = STL_TRUE;
    }
    else
    {
        sIsOuterColumn = STL_FALSE;
    }

    STL_DASSERT( sCandColumn->mNode != NULL );
    sCandColumn->mColumnHandle = sInitColumn->mColumnHandle;
    sCandColumn->mColumnID = ellGetColumnID( sInitColumn->mColumnHandle );
    sCandColumn->mColumnPos = ellGetColumnIdx( sInitColumn->mColumnHandle );

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandColumn->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_COLUMN,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          aConvertExprParamInfo->mInitExpr->mIterationTime,
                          aConvertExprParamInfo->mInitExpr->mPosition,
                          aConvertExprParamInfo->mInitExpr->mColumnName,
                          ellGetColumnDBType( sCandColumn->mColumnHandle ) );


    /**
     * Reference Column List 설정
     */

    if( (aConvertExprParamInfo->mRefNodeList != NULL) &&
        (sIsOuterColumn == STL_FALSE) )
    {
        STL_TRY( qlncAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                (qlncExprCommon*)sCandColumn,
                                                aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Base Table의 Read Column List에 등록
     */

    STL_TRY( qlncAddColumnToRefColumnList( &(aConvertExprParamInfo->mParamInfo),
                                           (qlncExprCommon*)sCandColumn,
                                           &(((qlncBaseTableNode*)(sCandColumn->mNode))->mReadColumnList),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandColumn;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Function Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprFunction( qlncConvertExprParamInfo * aConvertExprParamInfo,
                                   qllEnv                   * aEnv )
{
    stlInt32              i;
    qlvInitFunction     * sInitFunction     = NULL;
    qlncExprFunction    * sCandFunction     = NULL;
    qlvInitExpression   * sInitExpr         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_FUNCTION );
    sInitExpr = aConvertExprParamInfo->mInitExpr;
    sInitFunction = aConvertExprParamInfo->mInitExpr->mExpr.mFunction;

    /* Candidate Function Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprFunction ),
                                (void**) &sCandFunction,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Function Expression 정보 셋팅 */
    sCandFunction->mFuncId = sInitFunction->mFuncId;
    sCandFunction->mArgCount = sInitFunction->mArgCount;

    if( sCandFunction->mArgCount > 0 )
    {
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprCommon* ) * sCandFunction->mArgCount,
                                    (void**) &sCandFunction->mArgs,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( i = 0; i < sCandFunction->mArgCount; i++ )
        {
            aConvertExprParamInfo->mInitExpr = sInitFunction->mArgs[i];
            aConvertExprParamInfo->mCandExpr = NULL;

            STL_TRY( qlncConvertExprFuncList[ sInitFunction->mArgs[i]->mType ]( aConvertExprParamInfo,
                                                                                aEnv )
                     == STL_SUCCESS );
            STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

            sCandFunction->mArgs[i] = aConvertExprParamInfo->mCandExpr;
        }
    }

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandFunction->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_FUNCTION,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          sInitExpr->mIterationTime,
                          sInitExpr->mPosition,
                          sInitExpr->mColumnName,
                          DTL_TYPE_NA );

    /* DB Type 설정 */
    STL_TRY( qloGetDataType( aConvertExprParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                             sInitExpr,
                             aConvertExprParamInfo->mParamInfo.mSQLStmt->mBindContext,
                             &sCandFunction->mCommon.mDataType,
                             &QLL_CANDIDATE_MEM( aEnv ),
                             aEnv )
             == STL_SUCCESS );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandFunction;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Type Cast Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprTypeCast( qlncConvertExprParamInfo * aConvertExprParamInfo,
                                   qllEnv                   * aEnv )
{
    stlInt32              i;
    qlvInitTypeCast     * sInitTypeCast     = NULL;
    qlncExprTypeCast    * sCandTypeCast     = NULL;
    qlvInitExpression   * sInitExpr         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_CAST );
    sInitExpr = aConvertExprParamInfo->mInitExpr;
    sInitTypeCast = aConvertExprParamInfo->mInitExpr->mExpr.mTypeCast;

    /* Candidate Type Cast Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprTypeCast ),
                                (void**) &sCandTypeCast,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /* Type Cast  Expression 정보 셋팅 */
    sCandTypeCast->mTypeDef.mDBType = sInitTypeCast->mTypeDef.mDBType;

    QLNC_ALLOC_AND_COPY_STRING( sCandTypeCast->mTypeDef.mUserTypeName,
                                sInitTypeCast->mTypeDef.mUserTypeName,
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv );

    sCandTypeCast->mTypeDef.mArgNum1 = sInitTypeCast->mTypeDef.mArgNum1;
    sCandTypeCast->mTypeDef.mArgNum2 = sInitTypeCast->mTypeDef.mArgNum2;
    sCandTypeCast->mTypeDef.mStringLengthUnit = sInitTypeCast->mTypeDef.mStringLengthUnit;
    sCandTypeCast->mTypeDef.mIntervalIndicator = sInitTypeCast->mTypeDef.mIntervalIndicator;
    sCandTypeCast->mTypeDef.mNumericRadix = sInitTypeCast->mTypeDef.mNumericRadix;

    sCandTypeCast->mArgCount = DTL_CAST_INPUT_ARG_CNT;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * DTL_CAST_INPUT_ARG_CNT,
                                (void**) &sCandTypeCast->mArgs,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < DTL_CAST_INPUT_ARG_CNT; i++ )
    {
        aConvertExprParamInfo->mInitExpr = sInitTypeCast->mArgs[i];
        aConvertExprParamInfo->mCandExpr = NULL;

        STL_TRY( qlncConvertExprFuncList[ sInitTypeCast->mArgs[i]->mType ]( aConvertExprParamInfo,
                                                                        aEnv )
                 == STL_SUCCESS );
        STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

        sCandTypeCast->mArgs[i] = aConvertExprParamInfo->mCandExpr;
    }

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandTypeCast->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_CAST,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          sInitExpr->mIterationTime,
                          sInitExpr->mPosition,
                          sInitExpr->mColumnName,
                          sInitTypeCast->mTypeDef.mDBType );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandTypeCast;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Pseudo Column Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprPseudoCol( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                    qllEnv                      * aEnv )
{
    stlInt32              i;
    qlvInitPseudoCol    * sInitPseudoCol    = NULL;
    qlncExprPseudoCol   * sCandPseudoCol    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_PSEUDO_COLUMN );
    sInitPseudoCol = aConvertExprParamInfo->mInitExpr->mExpr.mPseudoCol;

    /* Candidate Pseudo Column Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprPseudoCol ),
                                (void**) &sCandPseudoCol,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /* Pseudo Column  Expression 정보 셋팅 */
    sCandPseudoCol->mPseudoId = sInitPseudoCol->mPseudoId;
    sCandPseudoCol->mArgCount = sInitPseudoCol->mArgCount;
    sCandPseudoCol->mSeqDictHandle = sInitPseudoCol->mSeqDictHandle;
    sCandPseudoCol->mPseudoOffset = sInitPseudoCol->mPseudoOffset;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( ellPseudoArg* ) * sCandPseudoCol->mArgCount,
                                (void**) &sCandPseudoCol->mArgs,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < sCandPseudoCol->mArgCount; i++ )
    {
        sCandPseudoCol->mArgs[i] = sInitPseudoCol->mArgs[i];
    }

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandPseudoCol->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_PSEUDO_COLUMN,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          aConvertExprParamInfo->mInitExpr->mIterationTime,
                          aConvertExprParamInfo->mInitExpr->mPosition,
                          aConvertExprParamInfo->mInitExpr->mColumnName,
                          gPseudoColInfoArr[ sInitPseudoCol->mPseudoId ].mDataType );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandPseudoCol;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Pseudo Argument Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprPseudoArg( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                    qllEnv                      * aEnv )
{
#if 0
    qlncExprPseudoArg   * sCandPseudoArg    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_PSEUDO_ARGUMENT );

    /* Candidate Pseudo Argument Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprPseudoArg ),
                                (void**) &sCandPseudoArg,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /* Pseudo Argument Expression 정보 셋팅 */
    sCandPseudoArg->mPseudoArg = aConvertExprParamInfo->mInitExpr->mExpr.mPseudoArg;

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandPseudoArg->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_PSEUDO_ARGUMENT,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          aConvertExprParamInfo->mInitExpr->mIterationTime,
                          aConvertExprParamInfo->mInitExpr->mPosition,
                          aConvertExprParamInfo->mInitExpr->mColumnName,
                          DTL_TYPE_NA );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandPseudoArg;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
#endif

    /* @todo 현재 Pseudo Argument는 없다. */

    return STL_FAILURE;
}


/**
 * @brief Const Expression Result Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprConstExpr( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                    qllEnv                      * aEnv )
{
    qlvInitConstExpr    * sInitConstExpr    = NULL;
    qlncExprConstExpr   * sCandConstExpr    = NULL;
    qlvInitExpression   * sInitExpr         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT );
    sInitExpr = aConvertExprParamInfo->mInitExpr;
    sInitConstExpr = aConvertExprParamInfo->mInitExpr->mExpr.mConstExpr;

    /* Candidate Constant Expression Result Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprConstExpr ),
                                (void**) &sCandConstExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Constant Expression Result Expression 정보 셋팅 */
    aConvertExprParamInfo->mInitExpr = sInitConstExpr->mOrgExpr;
    aConvertExprParamInfo->mCandExpr = NULL;

    STL_TRY( qlncConvertExprFuncList[ sInitConstExpr->mOrgExpr->mType ]( aConvertExprParamInfo,
                                                                         aEnv )
             == STL_SUCCESS );
    STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

    sCandConstExpr->mOrgExpr = aConvertExprParamInfo->mCandExpr;

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandConstExpr->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          sInitExpr->mIterationTime,
                          sInitExpr->mPosition,
                          sInitExpr->mColumnName,
                          DTL_TYPE_NA );

    /* DB Type 설정 */
    STL_TRY( qloGetDataType( aConvertExprParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                             sInitExpr,
                             aConvertExprParamInfo->mParamInfo.mSQLStmt->mBindContext,
                             &sCandConstExpr->mCommon.mDataType,
                             &QLL_CANDIDATE_MEM( aEnv ),
                             aEnv )
             == STL_SUCCESS );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandConstExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub-Query Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprSubQuery( qlncConvertExprParamInfo * aConvertExprParamInfo,
                                   qllEnv                   * aEnv )
{
    qlvInitSubQuery         * sInitSubQuery     = NULL;
    qlncExprSubQuery        * sCandSubQuery     = NULL;
    qlvInitExpression       * sInitExpr         = NULL;

    qlncCreateNodeParamInfo   sCreateNodeParamInfo;
    qlncTableMapArray       * sNewOuterTableMapArr  = NULL;
    qlncRefExprItem         * sRefExprItem          = NULL;

    stlBool                   sIsExistSubQuery;
    stlInt32                  sQBID;
    qlncNodeCommon          * sCandNode         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );

    
    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_SUB_QUERY );
    sInitExpr = aConvertExprParamInfo->mInitExpr;
    sInitSubQuery = aConvertExprParamInfo->mInitExpr->mExpr.mSubQuery;

    /* Candidate SubQuery Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprSubQuery ),
                                (void**) &sCandSubQuery,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* SubQuery Expression 정보 초기화 */
    stlMemset( sCandSubQuery,
               0x00,
               STL_SIZEOF( qlncExprSubQuery ) );

    /* SubQuery에 대한 Init Node가 이미 존재하는지 찾는다. */
    sIsExistSubQuery = qlncFindQueryBlockInQBMapArray( aConvertExprParamInfo->mQBMapArr,
                                                       sInitSubQuery->mInitNode,
                                                       &sQBID,
                                                       &sCandNode );

    /* Relation SubQuery 여부 셋팅 */
    sCandSubQuery->mIsRelSubQuery = ( sInitSubQuery->mCategory == QLV_SUB_QUERY_TYPE_RELATION );
    
    /* related function's ID 셋팅 */
    sCandSubQuery->mRelatedFuncId = sInitSubQuery->mRelatedFuncId;

    if( sIsExistSubQuery == STL_TRUE )
    {
        /* 이미 존재하는 경우 */
        sCandSubQuery->mNode = sCandNode;
        sCandSubQuery->mRefQBID = sQBID;
        sCandSubQuery->mRefIdx = sInitExpr->mExpr.mSubQuery->mRefIdx;
    }
    else
    {
        /* 존재하지 않는 경우 */

        /* Outer Table Map Array 생성 */
        STL_TRY( qlncMergeTableMapArray( &QLL_CANDIDATE_MEM( aEnv ),
                                         aConvertExprParamInfo->mTableMapArr,
                                         aConvertExprParamInfo->mOuterTableMapArr,
                                         &sNewOuterTableMapArr,
                                         aEnv )
                 == STL_SUCCESS );

        /* Create Node Parameter Info 설정 */
        QLNC_INIT_CREATE_NODE_PARAM_INFO( &sCreateNodeParamInfo,
                                          &(aConvertExprParamInfo->mParamInfo),
                                          sInitSubQuery->mInitNode,
                                          NULL,
                                          NULL,
                                          sNewOuterTableMapArr,
                                          sCandSubQuery,
                                          aConvertExprParamInfo->mQBMapArr );

        /* SubQuery의 Node에 대한 처리 */
        STL_TRY( qlncCreateQueryCandNode( &sCreateNodeParamInfo,
                                          aEnv )
                 == STL_SUCCESS );

        /* 상위에 SubQuery가 존재하면 Reference Expr List를 복사한다. */
        if( (sCandSubQuery->mRefExprList != NULL) &&
            (sCandSubQuery->mRefExprList->mCount > 0) &&
            (aConvertExprParamInfo->mExprSubQuery != NULL) )
        {
            sRefExprItem = sCandSubQuery->mRefExprList->mHead;
            while( sRefExprItem != NULL )
            {
                if( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_COLUMN )
                {
                    sCandNode = ((qlncExprColumn*)(sRefExprItem->mExpr))->mNode;
                }
                else if( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN )
                {
                    sCandNode = ((qlncExprRowidColumn*)(sRefExprItem->mExpr))->mNode;
                }
                else
                {
                    STL_DASSERT( sRefExprItem->mExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN );
                    sCandNode = ((qlncExprInnerColumn*)(sRefExprItem->mExpr))->mNode;
                }

                if( qlncIsExistCandNodeOnTableMap( aConvertExprParamInfo->mOuterTableMapArr,
                                                   sCandNode ) == STL_TRUE )
                {
                    STL_TRY( qlncSetRefColumnInfoOnExprSubQuery(
                                 &QLL_CANDIDATE_MEM( aEnv ),
                                 sRefExprItem->mExpr,
                                 aConvertExprParamInfo->mExprSubQuery,
                                 aEnv )
                             == STL_SUCCESS );
                }

                sRefExprItem = sRefExprItem->mNext;
            }
        }

        STL_DASSERT( sCreateNodeParamInfo.mCandNode != NULL );
        sCandSubQuery->mNode = sCreateNodeParamInfo.mCandNode;

        if( sCandSubQuery->mNode->mNodeType == QLNC_NODE_TYPE_QUERY_SET )
        {
            sCandSubQuery->mRefQBID = ((qlncQuerySet*)(sCandSubQuery->mNode))->mQBID;
        }
        else
        {
            STL_DASSERT( sCandSubQuery->mNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC );
            sCandSubQuery->mRefQBID = ((qlncQuerySpec*)(sCandSubQuery->mNode))->mQBID;
        }

        sCandSubQuery->mRefIdx = sInitExpr->mExpr.mSubQuery->mRefIdx;
    }
 
 
    /* Target 정보 셋팅 */
    if( sCandSubQuery->mNode->mNodeType == QLNC_NODE_TYPE_QUERY_SET )
    {
        sCandSubQuery->mTargetCount = ((qlncQuerySet*)(sCandSubQuery->mNode))->mSetTargetCount;
        sCandSubQuery->mTargetArr = ((qlncQuerySet*)(sCandSubQuery->mNode))->mSetTargetArr;
    }
    else
    {
        STL_DASSERT( sCandSubQuery->mNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC );
        sCandSubQuery->mTargetCount = ((qlncQuerySpec*)(sCandSubQuery->mNode))->mTargetCount;
        sCandSubQuery->mTargetArr = ((qlncQuerySpec*)(sCandSubQuery->mNode))->mTargetArr;
    }

    
    /**
     * DB Type 설정
     */

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandSubQuery->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_SUB_QUERY,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          sInitExpr->mIterationTime,
                          sInitExpr->mPosition,
                          sInitExpr->mColumnName,
                          DTL_TYPE_NA );

    /* DB Type 설정 */
    if( sCandSubQuery->mTargetCount == 1 )
    {
        STL_TRY( qloGetDataType( aConvertExprParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                 sInitExpr,
                                 aConvertExprParamInfo->mParamInfo.mSQLStmt->mBindContext,
                                 &sCandSubQuery->mCommon.mDataType,
                                 &QLL_CANDIDATE_MEM( aEnv ),
                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Scalar Type이 아닌 경우 Sub Query의 결과 Type은 임시로 Boolean Type으로 설정한다. */
        sCandSubQuery->mCommon.mDataType = DTL_TYPE_BOOLEAN;
    }

    /* SubQuery Expression List에 추가 */
    if( (sIsExistSubQuery == STL_FALSE) &&
        (aConvertExprParamInfo->mSubQueryExprList != NULL) )
    {
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncRefExprItem ),
                                    (void**) &sRefExprItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRefExprItem->mExpr = (qlncExprCommon*)sCandSubQuery;
        sRefExprItem->mNext = NULL;

        QLNC_APPEND_ITEM_TO_LIST( aConvertExprParamInfo->mSubQueryExprList, sRefExprItem );
    }

    if( (aConvertExprParamInfo->mRefNodeList != NULL) &&
        (sCandSubQuery->mRefExprList != NULL) )
    {
        sRefExprItem = sCandSubQuery->mRefExprList->mHead;
        while( sRefExprItem != NULL )
        {
            STL_TRY( qlncAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                    sRefExprItem->mExpr,
                                                    aEnv )
                     == STL_SUCCESS );

            sRefExprItem = sRefExprItem->mNext;
        }
    }


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandSubQuery;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Reference Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprReference( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                    qllEnv                      * aEnv )
{
#if 0
    qlvInitRefExpr      * sInitRefExpr      = NULL;
    qlncExprReference   * sCandReference    = NULL;
    qlvInitExpression   * sInitExpr         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_REFERENCE );
    sInitExpr = aConvertExprParamInfo->mInitExpr;
    sInitRefExpr = aConvertExprParamInfo->mInitExpr->mExpr.mReference;

    /* Candidate Reference Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprReference ),
                                (void**) &sCandReference,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /* Reference Expression 정보 셋팅 */
    aConvertExprParamInfo->mInitExpr = sInitRefExpr->mOrgExpr;
    aConvertExprParamInfo->mCandExpr = NULL;

    STL_TRY( qlncConvertExprFuncList[ sInitRefExpr->mOrgExpr->mType ]( aConvertExprParamInfo,
                                                                       aEnv )
             == STL_SUCCESS );
    STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

    sCandReference->mOrgExpr = aConvertExprParamInfo->mCandExpr;

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandReference->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_REFERENCE,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          sInitExpr->mIterationTime,
                          sInitExpr->mPosition,
                          sInitExpr->mColumnName,
                          DTL_TYPE_NA );

    /* DB Type 설정 */
    STL_TRY( qloGetDataType( aConvertExprParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                             sInitExpr,
                             aConvertExprParamInfo->mParamInfo.mSQLStmt->mBindContext,
                             &sCandReference->mCommon.mDataType,
                             &QLL_CANDIDATE_MEM( aEnv ),
                             aEnv )
             == STL_SUCCESS );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandReference;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
#endif

    /* 현재 Validation에서 Reference를 만드는 경우는 없다. */

    return STL_FAILURE;
}


/**
 * @brief Inner Column Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprInnerColumn( qlncConvertExprParamInfo  * aConvertExprParamInfo,
                                      qllEnv                    * aEnv )
{
    qlvInitInnerColumn  * sInitInnerColumn  = NULL;
    qlncExprInnerColumn * sCandInnerColumn  = NULL;
    qlvInitExpression   * sInitExpr         = NULL;
    stlBool               sIsOuterColumn;

    qlncRefExprItem     * sRefExprItem      = NULL;

    qlncSortInstantNode * sSortInstant      = NULL;
    qlncHashInstantNode * sHashInstant      = NULL;

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
    sInitExpr = aConvertExprParamInfo->mInitExpr;
    sInitInnerColumn = aConvertExprParamInfo->mInitExpr->mExpr.mInnerColumn;

    if( (sInitInnerColumn->mRelationNode->mType != QLV_NODE_TYPE_SUB_TABLE) &&
        (sInitInnerColumn->mRelationNode->mType != QLV_NODE_TYPE_SORT_INSTANT) &&
        (sInitInnerColumn->mRelationNode->mType != QLV_NODE_TYPE_GROUP_SORT_INSTANT) &&
        (sInitInnerColumn->mRelationNode->mType != QLV_NODE_TYPE_HASH_INSTANT) &&
        (sInitInnerColumn->mRelationNode->mType != QLV_NODE_TYPE_GROUP_HASH_INSTANT) )
    {
        aConvertExprParamInfo->mInitExpr = sInitInnerColumn->mOrgExpr;

        STL_TRY( qlncConvertExprFuncList[ sInitInnerColumn->mOrgExpr->mType ]( aConvertExprParamInfo,
                                                                               aEnv )
                 == STL_SUCCESS );
        STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

        if( sInitInnerColumn->mRelationNode->mType == QLV_NODE_TYPE_QUERY_SPEC )
        {
            aConvertExprParamInfo->mCandExpr->mDataType = 
                ((qlvInitQuerySpecNode *) sInitInnerColumn->mRelationNode)
                ->mTargets[ *sInitInnerColumn->mIdx ].mDataTypeInfo.mDataType;
        }
        else if( sInitInnerColumn->mRelationNode->mType == QLV_NODE_TYPE_QUERY_SET )
        {
            aConvertExprParamInfo->mCandExpr->mDataType = 
                ((qlvInitQuerySetNode *) sInitInnerColumn->mRelationNode)
                ->mSetTargets[ *sInitInnerColumn->mIdx ].mDataTypeInfo.mDataType;
        }
        else
        {
            STL_DASSERT( sInitInnerColumn->mRelationNode->mType == QLV_NODE_TYPE_JOIN_TABLE );
            
            /* Do Nothing */
        }
    }
    else
    {
        /* Candidate Inner Column Expression 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprInnerColumn ),
                                    (void**) &sCandInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Inner Column Expression 정보 셋팅 */
        sCandInnerColumn->mNode =
            qlncGetCandNodePtrFromInitNodePtr( aConvertExprParamInfo->mTableMapArr,
                                               sInitInnerColumn->mRelationNode );
        if( sCandInnerColumn->mNode == NULL )
        {
            /* Outer Table을 참조하는 경우 */
            sCandInnerColumn->mNode =
                qlncGetCandNodePtrFromInitNodePtr( aConvertExprParamInfo->mOuterTableMapArr,
                                                   sInitInnerColumn->mRelationNode );
            STL_DASSERT( sCandInnerColumn->mNode != NULL );

            STL_TRY( qlncSetRefColumnInfoOnExprSubQuery(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         (qlncExprCommon*)sCandInnerColumn,
                         aConvertExprParamInfo->mExprSubQuery,
                         aEnv )
                     == STL_SUCCESS );

            sIsOuterColumn = STL_TRUE;
        }
        else
        {
            sIsOuterColumn = STL_FALSE;
        }

        STL_DASSERT( sCandInnerColumn->mNode != NULL );
        sCandInnerColumn->mIdx = *(sInitInnerColumn->mIdx);

        if( sCandInnerColumn->mNode->mNodeType == QLNC_NODE_TYPE_FLAT_INSTANT )
        {
            STL_DASSERT( 0 );
        }
        else if( sCandInnerColumn->mNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
        {
            sSortInstant = (qlncSortInstantNode*) sCandInnerColumn->mNode;
            sCandInnerColumn->mOrgExpr = NULL;

            /* 상위에서 Read Column을 참조하는 경우 */
            STL_DASSERT( sSortInstant->mReadColList != NULL );
            sRefExprItem = sSortInstant->mReadColList->mHead;
            while( sRefExprItem != NULL )
            {
                if( sCandInnerColumn->mIdx ==
                    ((qlncExprInnerColumn *) sRefExprItem->mExpr)->mIdx )
                {
                    sCandInnerColumn->mOrgExpr = sRefExprItem->mExpr;
                    break;
                }

                sRefExprItem = sRefExprItem->mNext;
            }

            STL_DASSERT( sCandInnerColumn->mOrgExpr != NULL );
        }
        else if( sCandInnerColumn->mNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT )
        {
            sHashInstant = (qlncHashInstantNode*) sCandInnerColumn->mNode;
            sCandInnerColumn->mOrgExpr = NULL;

            if( sHashInstant->mReadColList == NULL )
            {
                /* Key 또는 Record Column에서 동일 레벨(Key/Record)의 Column을 참조하는 경우 */

                STL_DASSERT( sHashInstant->mKeyColList != NULL );
                if( sCandInnerColumn->mIdx < sHashInstant->mKeyColList->mCount )
                {
                    /* Key Column 찾음 */
                    sRefExprItem = sHashInstant->mKeyColList->mHead;
                    while( sRefExprItem != NULL )
                    {
                        if( sCandInnerColumn->mIdx ==
                            ((qlncExprInnerColumn *) sRefExprItem->mExpr)->mIdx )
                        {
                            sCandInnerColumn->mOrgExpr = sRefExprItem->mExpr;
                            break;
                        }

                        sRefExprItem = sRefExprItem->mNext;
                    }
                }
                else
                {
                    /* Validation에 의해 Record Column에서 찾는 경우가 없는 것으로 판단됨 */

                    /* Record Column 찾음 */
                    if( sHashInstant->mRecColList != NULL )
                    {
                        sRefExprItem = sHashInstant->mRecColList->mHead;
                        while( sRefExprItem != NULL )
                        {
                            if( sCandInnerColumn->mIdx ==
                                ((qlncExprInnerColumn *) sRefExprItem->mExpr)->mIdx )
                            {
                                sCandInnerColumn->mOrgExpr = sRefExprItem->mExpr;
                                break;
                            }

                            sRefExprItem = sRefExprItem->mNext;
                        }
                    }
                }
            }
            else
            {
                /* 상위에서 Read Column을 참조하는 경우 */
                sRefExprItem = sHashInstant->mReadColList->mHead;
                while( sRefExprItem != NULL )
                {
                    if( sCandInnerColumn->mIdx ==
                        ((qlncExprInnerColumn *) sRefExprItem->mExpr)->mIdx )
                    {
                        sCandInnerColumn->mOrgExpr = sRefExprItem->mExpr;
                        break;
                    }

                    sRefExprItem = sRefExprItem->mNext;
                }
            }

            STL_DASSERT( sCandInnerColumn->mOrgExpr != NULL );
        }
        else
        {
            STL_DASSERT( sCandInnerColumn->mNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE );

            /* Sub Table인 경우 Orginal Expression을 더이상 분석하지 않는다.
             * 다만 Idx의 값이 하위 Query Spec Node의 Target에 대한 Idx와 일치해야 한다. */
            sCandInnerColumn->mOrgExpr = NULL;
        }

        /* Expression Common 정보 셋팅 */
        QLNC_SET_EXPR_COMMON( &sCandInnerColumn->mCommon,
                              &QLL_CANDIDATE_MEM( aEnv ),
                              aEnv,
                              QLNC_EXPR_TYPE_INNER_COLUMN,
                              aConvertExprParamInfo->mExprPhraseFlag,
                              sInitExpr->mIterationTime,
                              sInitExpr->mPosition,
                              sInitExpr->mColumnName,
                              DTL_TYPE_NA );

        /* DB Type 설정 */
        STL_TRY( qloGetDataType( aConvertExprParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                 sInitExpr,
                                 aConvertExprParamInfo->mParamInfo.mSQLStmt->mBindContext,
                                 &sCandInnerColumn->mCommon.mDataType,
                                 &QLL_CANDIDATE_MEM( aEnv ),
                                 aEnv )
                 == STL_SUCCESS );


        /**
         * Reference Column List 설정
         */

        if( (aConvertExprParamInfo->mRefNodeList != NULL) &&
            (sIsOuterColumn == STL_FALSE) )
        {
            STL_TRY( qlncAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                    (qlncExprCommon*)sCandInnerColumn,
                                                    aEnv )
                     == STL_SUCCESS );
        }


        if( sInitInnerColumn->mRelationNode->mType == QLV_NODE_TYPE_SUB_TABLE )
        {
            /**
             * Sub Table의 Read Column List에 등록
             */

            STL_TRY( qlncAddColumnToRefColumnList(
                         &(aConvertExprParamInfo->mParamInfo),
                         (qlncExprCommon*)sCandInnerColumn,
                         &(((qlncSubTableNode*)(sCandInnerColumn->mNode))->mReadColumnList),
                         aEnv )
                     == STL_SUCCESS );
        }


        /**
         * Output 설정
         */

        aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandInnerColumn;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Outer Column Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprOuterColumn( qlncConvertExprParamInfo  * aConvertExprParamInfo,
                                      qllEnv                    * aEnv )
{
    qlvInitExpression   * sInitExpr         = NULL;
    qlvInitOuterColumn  * sInitOuterColumn  = NULL;
    

    /****************************************************************
     * Paramter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );

    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_OUTER_COLUMN );

    sInitExpr = aConvertExprParamInfo->mInitExpr;
    sInitOuterColumn = aConvertExprParamInfo->mInitExpr->mExpr.mOuterColumn;

    /* Outer Column Expression 정보 셋팅 */
    aConvertExprParamInfo->mInitExpr = sInitOuterColumn->mOrgExpr;
    aConvertExprParamInfo->mCandExpr = NULL;

    STL_TRY( qlncConvertExprFuncList[ sInitOuterColumn->mOrgExpr->mType ]( aConvertExprParamInfo,
                                                                           aEnv )
             == STL_SUCCESS );
    STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

    /* Position 정보 수정 */
    aConvertExprParamInfo->mCandExpr->mPosition = sInitExpr->mPosition;


    /**
     * Output 설정
     */

    /* Do Nothing */


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Rowid Column Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprRowidColumn( qlncConvertExprParamInfo  * aConvertExprParamInfo,
                                      qllEnv                    * aEnv )
{
    qlvInitRowIdColumn  * sInitRowidColumn  = NULL;
    qlncExprRowidColumn * sCandRowidColumn  = NULL;
    stlBool               sIsOuterColumn;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN );
    sInitRowidColumn = aConvertExprParamInfo->mInitExpr->mExpr.mRowIdColumn;

    /* Candidate Rowid Column Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprRowidColumn ),
                                (void**) &sCandRowidColumn,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /* Rowid Column Expression 정보 셋팅 */
    sCandRowidColumn->mNode =
        qlncGetCandNodePtrFromInitNodePtr( aConvertExprParamInfo->mTableMapArr,
                                           sInitRowidColumn->mRelationNode );

    if( sCandRowidColumn->mNode == NULL )
    {
        /* Outer Table을 참조하는 경우 */
        sCandRowidColumn->mNode =
            qlncGetCandNodePtrFromInitNodePtr( aConvertExprParamInfo->mOuterTableMapArr,
                                               sInitRowidColumn->mRelationNode );
        STL_DASSERT( sCandRowidColumn->mNode != NULL );

        STL_TRY( qlncSetRefColumnInfoOnExprSubQuery(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     (qlncExprCommon*)sCandRowidColumn,
                     aConvertExprParamInfo->mExprSubQuery,
                     aEnv )
                 == STL_SUCCESS );

        sIsOuterColumn = STL_TRUE;
    }
    else
    {
        sIsOuterColumn = STL_FALSE;
    }

    STL_DASSERT( sCandRowidColumn->mNode != NULL );

    sCandRowidColumn->mIdx = QLNC_ROWID_COLUMN_IDX;

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandRowidColumn->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_ROWID_COLUMN,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          aConvertExprParamInfo->mInitExpr->mIterationTime,
                          aConvertExprParamInfo->mInitExpr->mPosition,
                          aConvertExprParamInfo->mInitExpr->mColumnName,
                          DTL_TYPE_ROWID );


    /**
     * Reference Column List 설정
     */

    if( (aConvertExprParamInfo->mRefNodeList != NULL) &&
        (sIsOuterColumn == STL_FALSE) )
    {
        STL_TRY( qlncAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                (qlncExprCommon*)sCandRowidColumn,
                                                aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Base Table의 Read Column List에 등록
     */

    STL_TRY( qlncAddColumnToRefColumnList( &(aConvertExprParamInfo->mParamInfo),
                                           (qlncExprCommon*)sCandRowidColumn,
                                           &(((qlncBaseTableNode*)(sCandRowidColumn->mNode))->mReadColumnList),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandRowidColumn;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Aggregation Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprAggregation( qlncConvertExprParamInfo  * aConvertExprParamInfo,
                                      qllEnv                    * aEnv )
{
    stlInt32              i;
    qlvInitAggregation  * sInitAggregation  = NULL;
    qlncExprAggregation * sCandAggregation  = NULL;
    qlvInitExpression   * sInitExpr         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_AGGREGATION );
    sInitExpr = aConvertExprParamInfo->mInitExpr;
    sInitAggregation = aConvertExprParamInfo->mInitExpr->mExpr.mAggregation;

    /* Candidate Aggregation Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprAggregation ),
                                (void**) &sCandAggregation,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /* Aggregation Expression 정보 셋팅 */
    sCandAggregation->mAggrId = sInitAggregation->mAggrId;
    sCandAggregation->mIsDistinct = sInitAggregation->mIsDistinct;
    sCandAggregation->mHasNestedAggr = sInitAggregation->mHasNestedAggr;
    sCandAggregation->mArgCount = sInitAggregation->mArgCount;

    if( sCandAggregation->mArgCount > 0 )
    {
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprCommon* ) * sCandAggregation->mArgCount,
                                    (void**) &sCandAggregation->mArgs,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( i = 0; i < sCandAggregation->mArgCount; i++ )
        {
            aConvertExprParamInfo->mInitExpr = sInitAggregation->mArgs[i];
            aConvertExprParamInfo->mCandExpr = NULL;

            STL_TRY( qlncConvertExprFuncList[ sInitAggregation->mArgs[i]->mType ]( aConvertExprParamInfo,
                                                                                   aEnv )
                     == STL_SUCCESS );
            STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

            sCandAggregation->mArgs[i] = aConvertExprParamInfo->mCandExpr;
        }
    }
    else
    {
        sCandAggregation->mArgs = NULL;
    }

    if( sInitAggregation->mFilter != NULL )
    {
        /* @todo 향후 개발될 부분 */
        aConvertExprParamInfo->mInitExpr = sInitAggregation->mFilter;
        aConvertExprParamInfo->mCandExpr = NULL;

        STL_TRY( qlncConvertExprFuncList[ sInitAggregation->mFilter->mType ]( aConvertExprParamInfo,
                                                                              aEnv )
                 == STL_SUCCESS );
        STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

        sCandAggregation->mFilter = aConvertExprParamInfo->mCandExpr;
    }
    else
    {
        sCandAggregation->mFilter = NULL;
    }

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandAggregation->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_AGGREGATION,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          sInitExpr->mIterationTime,
                          sInitExpr->mPosition,
                          sInitExpr->mColumnName,
                          DTL_TYPE_NA );

    /* DB Type 설정 */
    STL_TRY( qloGetDataType( aConvertExprParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                             sInitExpr,
                             aConvertExprParamInfo->mParamInfo.mSQLStmt->mBindContext,
                             &sCandAggregation->mCommon.mDataType,
                             &QLL_CANDIDATE_MEM( aEnv ),
                             aEnv )
             == STL_SUCCESS );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandAggregation;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Case Expr Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprCaseExpr( qlncConvertExprParamInfo * aConvertExprParamInfo,
                                   qllEnv                   * aEnv )
{
    stlInt32              i;
    qlvInitCaseExpr     * sInitCaseExpr     = NULL;
    qlncExprCaseExpr    * sCandCaseExpr     = NULL;
    qlvInitExpression   * sInitExpr         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_CASE_EXPR );
    sInitExpr = aConvertExprParamInfo->mInitExpr;
    sInitCaseExpr = aConvertExprParamInfo->mInitExpr->mExpr.mCaseExpr;

    /* Candidate Case Expr Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCaseExpr ),
                                (void**) &sCandCaseExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /* Case Expr Expression 정보 셋팅 */
    sCandCaseExpr->mFuncId = sInitCaseExpr->mFuncId;
    sCandCaseExpr->mCount = sInitCaseExpr->mCount;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sCandCaseExpr->mCount,
                                (void**) &sCandCaseExpr->mWhenArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sCandCaseExpr->mCount,
                                (void**) &sCandCaseExpr->mThenArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < sCandCaseExpr->mCount; i++ )
    {
        /* When */
        aConvertExprParamInfo->mInitExpr = sInitCaseExpr->mWhenArr[i];
        aConvertExprParamInfo->mCandExpr = NULL;

        STL_TRY( qlncConvertExprFuncList[ sInitCaseExpr->mWhenArr[i]->mType ]( aConvertExprParamInfo,
                                                                           aEnv )
                 == STL_SUCCESS );
        STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

        sCandCaseExpr->mWhenArr[i] = aConvertExprParamInfo->mCandExpr;

        /* Then */
        aConvertExprParamInfo->mInitExpr = sInitCaseExpr->mThenArr[i];
        aConvertExprParamInfo->mCandExpr = NULL;

        STL_TRY( qlncConvertExprFuncList[ sInitCaseExpr->mThenArr[i]->mType ]( aConvertExprParamInfo,
                                                                           aEnv )
                 == STL_SUCCESS );
        STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

        sCandCaseExpr->mThenArr[i] = aConvertExprParamInfo->mCandExpr;
    }

    aConvertExprParamInfo->mInitExpr = sInitCaseExpr->mDefResult;
    aConvertExprParamInfo->mCandExpr = NULL;

    STL_TRY( qlncConvertExprFuncList[ sInitCaseExpr->mDefResult->mType ]( aConvertExprParamInfo,
                                                                      aEnv )
             == STL_SUCCESS );
    STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

    sCandCaseExpr->mDefResult = aConvertExprParamInfo->mCandExpr;

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandCaseExpr->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_CASE_EXPR,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          sInitExpr->mIterationTime,
                          sInitExpr->mPosition,
                          sInitExpr->mColumnName,
                          DTL_TYPE_NA );

    /* DB Type 설정 */
    STL_TRY( qloGetDataType( aConvertExprParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                             sInitExpr,
                             aConvertExprParamInfo->mParamInfo.mSQLStmt->mBindContext,
                             &sCandCaseExpr->mCommon.mDataType,
                             &QLL_CANDIDATE_MEM( aEnv ),
                             aEnv )
             == STL_SUCCESS );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandCaseExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief List Function Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprListFunction( qlncConvertExprParamInfo * aConvertExprParamInfo,
                                       qllEnv                   * aEnv )
{
    stlInt32              i;
    qlvInitListFunc     * sInitListFunc     = NULL;
    qlncExprListFunc    * sCandListFunc     = NULL;
    qlvInitExpression   * sInitExpr         = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_LIST_FUNCTION );
    sInitExpr = aConvertExprParamInfo->mInitExpr;
    sInitListFunc = aConvertExprParamInfo->mInitExpr->mExpr.mListFunc;

    /* Candidate List Function Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprListFunc ),
                                (void**) &sCandListFunc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /* List Function Expression 정보 셋팅 */
    sCandListFunc->mFuncId = sInitListFunc->mFuncId;
    sCandListFunc->mListFuncType = sInitListFunc->mListFuncType;
    sCandListFunc->mArgCount = sInitListFunc->mArgCount;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sCandListFunc->mArgCount,
                                (void**) &sCandListFunc->mArgs,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < sCandListFunc->mArgCount; i++ )
    {
        aConvertExprParamInfo->mInitExpr = sInitListFunc->mArgs[i];
        aConvertExprParamInfo->mCandExpr = NULL;

        STL_TRY( qlncConvertExprFuncList[ sInitListFunc->mArgs[i]->mType ]( aConvertExprParamInfo,
                                                                        aEnv )
                 == STL_SUCCESS );
        STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

        sCandListFunc->mArgs[i] = aConvertExprParamInfo->mCandExpr;
    }

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandListFunc->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_LIST_FUNCTION,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          sInitExpr->mIterationTime,
                          sInitExpr->mPosition,
                          sInitExpr->mColumnName,
                          DTL_TYPE_BOOLEAN );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandListFunc;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief List Column Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprListColumn( qlncConvertExprParamInfo   * aConvertExprParamInfo,
                                     qllEnv                     * aEnv )
{
    stlInt32              i;
    qlvInitListCol      * sInitListCol  = NULL;
    qlncExprListCol     * sCandListCol  = NULL;
    qlvInitExpression   * sInitExpr     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_LIST_COLUMN );
    sInitExpr = aConvertExprParamInfo->mInitExpr;
    sInitListCol = aConvertExprParamInfo->mInitExpr->mExpr.mListCol;

    /* Candidate List Column Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprListCol ),
                                (void**) &sCandListCol,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /* List Column Expression 정보 셋팅 */
    sCandListCol->mPredicate = sInitListCol->mPredicate;
    sCandListCol->mArgCount = sInitListCol->mArgCount;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sCandListCol->mArgCount,
                                (void**) &sCandListCol->mArgs,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( knlListEntry* ) * sCandListCol->mArgCount,
                                (void**) &sCandListCol->mEntry,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < sCandListCol->mArgCount; i++ )
    {
        aConvertExprParamInfo->mInitExpr = sInitListCol->mArgs[i];
        aConvertExprParamInfo->mCandExpr = NULL;

        STL_TRY( qlncConvertExprFuncList[ sInitListCol->mArgs[i]->mType ]( aConvertExprParamInfo,
                                                                       aEnv )
                 == STL_SUCCESS );
        STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

        sCandListCol->mArgs[i] = aConvertExprParamInfo->mCandExpr;

        sCandListCol->mEntry[i] = sInitListCol->mEntry[i];
    }

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandListCol->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_LIST_COLUMN,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          sInitExpr->mIterationTime,
                          sInitExpr->mPosition,
                          sInitExpr->mColumnName,
                          DTL_TYPE_NA );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandListCol;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Row Expr Expression을 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertExprRowExpr( qlncConvertExprParamInfo  * aConvertExprParamInfo,
                                  qllEnv                    * aEnv )
{
    stlInt32              i;
    qlvInitRowExpr      * sInitRowExpr  = NULL;
    qlncExprRowExpr     * sCandRowExpr  = NULL;
    qlvInitExpression   * sInitExpr     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_ROW_EXPR );
    sInitExpr = aConvertExprParamInfo->mInitExpr;
    sInitRowExpr = aConvertExprParamInfo->mInitExpr->mExpr.mRowExpr;

    /* Candidate Row Expr Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprRowExpr ),
                                (void**) &sCandRowExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Row Expr Expression 정보 셋팅 */
    sCandRowExpr->mArgCount = sInitRowExpr->mArgCount;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * sCandRowExpr->mArgCount,
                                (void**) &sCandRowExpr->mArgs,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( knlListEntry ) * sCandRowExpr->mArgCount,
                                (void**) &sCandRowExpr->mEntry,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < sCandRowExpr->mArgCount; i++ )
    {
        aConvertExprParamInfo->mInitExpr = sInitRowExpr->mArgs[i];
        aConvertExprParamInfo->mCandExpr = NULL;

        STL_TRY( qlncConvertExprFuncList[ sInitRowExpr->mArgs[i]->mType ]( aConvertExprParamInfo,
                                                                           aEnv )
                 == STL_SUCCESS );
        STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

        sCandRowExpr->mArgs[i] = aConvertExprParamInfo->mCandExpr;

        sCandRowExpr->mEntry[i] = sInitRowExpr->mEntry[i];

        /* Row의 각 Expression에 대하여 Nullable 설정 */
        sCandRowExpr->mArgs[i]->mNullable = qlvCheckNullable( NULL, sInitRowExpr->mArgs[i] );
    }

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandRowExpr->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_ROW_EXPR,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          sInitExpr->mIterationTime,
                          sInitExpr->mPosition,
                          sInitExpr->mColumnName,
                          DTL_TYPE_NA );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sCandRowExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief And Filter를 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertAndFilter( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                qllEnv                      * aEnv )
{
    stlInt32              i;
    qlvInitFunction     * sFunction     = NULL;
    qlncAndFilter       * sAndFilter    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* And Filter 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncAndFilter ),
                                (void**)&sAndFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sAndFilter->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_AND_FILTER,
                          aConvertExprParamInfo->mInitExpr->mPhraseType,        
                          aConvertExprParamInfo->mInitExpr->mIterationTime,         
                          aConvertExprParamInfo->mInitExpr->mPosition,              
                          aConvertExprParamInfo->mInitExpr->mColumnName,            
                          DTL_TYPE_BOOLEAN );

    if( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_FUNCTION )
    {
        sFunction = aConvertExprParamInfo->mInitExpr->mExpr.mFunction;
        if( sFunction->mFuncId == KNL_BUILTIN_FUNC_AND )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncOrFilter* ) * sFunction->mArgCount,
                                        (void**)&(sAndFilter->mOrFilters),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( i = 0; i < sFunction->mArgCount; i++ )
            {
                aConvertExprParamInfo->mInitExpr = sFunction->mArgs[i];
                aConvertExprParamInfo->mCandExpr = NULL;

                STL_TRY( qlncConvertOrFilter( aConvertExprParamInfo,
                                              aEnv )
                         == STL_SUCCESS );

                STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );
                sAndFilter->mOrFilters[i] = (qlncOrFilter*)(aConvertExprParamInfo->mCandExpr);
            }

            sAndFilter->mOrCount = sFunction->mArgCount;

            STL_THROW( RAMP_FINISH );
        }
    }

    /* And Function이 아닌 경우 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOrFilter* ),
                                (void**)&(sAndFilter->mOrFilters),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    aConvertExprParamInfo->mCandExpr = NULL;

    STL_TRY( qlncConvertOrFilter( aConvertExprParamInfo,
                                  aEnv )
             == STL_SUCCESS );

    STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );
    sAndFilter->mOrFilters[0] = (qlncOrFilter*)(aConvertExprParamInfo->mCandExpr);
    sAndFilter->mOrCount = 1;


    STL_RAMP( RAMP_FINISH );


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sAndFilter;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Or Filter를 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertOrFilter( qlncConvertExprParamInfo     * aConvertExprParamInfo,
                               qllEnv                       * aEnv )
{
    stlInt32              i;
    qlvInitFunction     * sFunction     = NULL;
    qlncOrFilter        * sOrFilter     = NULL;

    qlncRefExprList       sSubQueryExprList;
    qlncRefExprList     * sBackupSubQueryExprList   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /* Or Filter 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOrFilter ),
                                (void**)&sOrFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sOrFilter->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_OR_FILTER,
                          aConvertExprParamInfo->mInitExpr->mPhraseType,        
                          aConvertExprParamInfo->mInitExpr->mIterationTime,         
                          aConvertExprParamInfo->mInitExpr->mPosition,              
                          aConvertExprParamInfo->mInitExpr->mColumnName,            
                          DTL_TYPE_BOOLEAN );

    /* SubQuery Expression List 백업 및 초기화 */
    QLNC_INIT_LIST( &sSubQueryExprList );
    sBackupSubQueryExprList = aConvertExprParamInfo->mSubQueryExprList;
    aConvertExprParamInfo->mSubQueryExprList = &sSubQueryExprList;

    if( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_FUNCTION )
    {
        sFunction = aConvertExprParamInfo->mInitExpr->mExpr.mFunction;
        if( sFunction->mFuncId == KNL_BUILTIN_FUNC_OR )
        {
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * sFunction->mArgCount,
                                        (void**)&(sOrFilter->mFilters),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( i = 0; i < sFunction->mArgCount; i++ )
            {
                aConvertExprParamInfo->mInitExpr = sFunction->mArgs[i];
                aConvertExprParamInfo->mCandExpr = NULL;

                STL_TRY( qlncConvertBooleanFilter( aConvertExprParamInfo,
                                                   aEnv )
                         == STL_SUCCESS );

                STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );
                sOrFilter->mFilters[i] = aConvertExprParamInfo->mCandExpr;
            }

            sOrFilter->mFilterCount = sFunction->mArgCount;
            sOrFilter->mSubQueryExprList = NULL;

            STL_THROW( RAMP_FINISH );
        }
    }

    /* Or Function이 아닌 경우 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ),
                                (void**)&(sOrFilter->mFilters),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    aConvertExprParamInfo->mCandExpr = NULL;

    STL_TRY( qlncConvertBooleanFilter( aConvertExprParamInfo,
                                       aEnv )
             == STL_SUCCESS );

    STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );
    sOrFilter->mFilters[0] = aConvertExprParamInfo->mCandExpr;
    sOrFilter->mFilterCount = 1;
    sOrFilter->mSubQueryExprList = NULL;


    STL_RAMP( RAMP_FINISH );


    if( sSubQueryExprList.mCount > 0 )
    {
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncRefExprList ),
                                    (void**) &(sOrFilter->mSubQueryExprList),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        QLNC_COPY_LIST_INFO( &sSubQueryExprList, sOrFilter->mSubQueryExprList );

        if( sBackupSubQueryExprList != NULL )
        {
            STL_TRY( qlncAddExprListToRefExprList(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         sBackupSubQueryExprList,
                         &sSubQueryExprList,
                         aEnv )
                     == STL_SUCCESS );
        }
    }

    aConvertExprParamInfo->mSubQueryExprList = sBackupSubQueryExprList;


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sOrFilter;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Boolean값을 반환하는 Filter를 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertBooleanFilter( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                                    qllEnv                      * aEnv )
{
    qlncExprCommon      * sResultFilter      = NULL;
    qlncRefNodeList     * sBackupRefNodeList = NULL;
    qlncRefNodeItem     * sRefSrcNodeItem    = NULL;
    qlncRefNodeItem     * sRefDstNodeItem    = NULL;
    qlncRefNodeList       sRefNodeList;
    stlInt32              sLoop;
    qlncRefExprList     * sBackupSubQueryExprList   = NULL;
    qlncRefExprList       sSubQueryExprList;
    

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * MACRO
     ****************************************************************/

#define _QLNC_INIT_SUBQUERY_LIST( )                                         \
    {                                                                       \
        QLNC_INIT_LIST( &sSubQueryExprList );                               \
        sBackupSubQueryExprList = aConvertExprParamInfo->mSubQueryExprList; \
        aConvertExprParamInfo->mSubQueryExprList = &sSubQueryExprList;      \
    }

#define _QLNC_FINI_SUBQUERY_LIST( )                                         \
    {                                                                       \
        if( sSubQueryExprList.mCount > 0 )                                  \
        {                                                                   \
            if( sBackupSubQueryExprList != NULL )                           \
            {                                                               \
                STL_TRY( qlncAddExprListToRefExprList(                      \
                             &QLL_CANDIDATE_MEM( aEnv ),                    \
                             sBackupSubQueryExprList,                       \
                             &sSubQueryExprList,                            \
                             aEnv )                                         \
                         == STL_SUCCESS );                                  \
            }                                                               \
        }                                                                   \
                                                                            \
        aConvertExprParamInfo->mSubQueryExprList = sBackupSubQueryExprList; \
    }


    STL_DASSERT( aConvertExprParamInfo->mCandExpr == NULL );

    QLNC_INIT_LIST( &sRefNodeList );
    sBackupRefNodeList = aConvertExprParamInfo->mRefNodeList;
    aConvertExprParamInfo->mRefNodeList = &sRefNodeList;
    
    switch( aConvertExprParamInfo->mInitExpr->mType )
    {
        case QLV_EXPR_TYPE_FUNCTION:
            {
                STL_DASSERT( aConvertExprParamInfo->mInitExpr->mExpr.mFunction->mFuncId !=
                             KNL_BUILTIN_FUNC_OR );

                if( aConvertExprParamInfo->mInitExpr->mExpr.mFunction->mFuncId == KNL_BUILTIN_FUNC_AND )
                {
                    /* Or 연산 아래에 And 연산이 오는 경우이다. */

                    _QLNC_INIT_SUBQUERY_LIST( );

                    STL_TRY( qlncConvertAndFilter( aConvertExprParamInfo,
                                                   aEnv )
                             == STL_SUCCESS );
                    STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

                    STL_TRY( qlncMakeFilter( aConvertExprParamInfo,
                                             &sRefNodeList,
                                             KNL_BUILTIN_FUNC_AND,
                                             &sSubQueryExprList,
                                             NULL,
                                             &sResultFilter,
                                             aEnv )
                             == STL_SUCCESS );

                    _QLNC_FINI_SUBQUERY_LIST( );
                }
                else
                {
                    if( qlncIsPossibleJoinCondition(
                            aConvertExprParamInfo->mInitExpr->mExpr.mFunction->mFuncId ) )
                    {
                        STL_TRY( qlncConvertCompareBooleanFilter( aConvertExprParamInfo,
                                                                  aEnv )
                                 == STL_SUCCESS );

                        sResultFilter = aConvertExprParamInfo->mCandExpr;
                    }
                    else
                    {
                        knlBuiltInFunc        sFuncID;

                        sFuncID = aConvertExprParamInfo->mInitExpr->mExpr.mFunction->mFuncId;

                        _QLNC_INIT_SUBQUERY_LIST( );

                        STL_TRY( qlncConvertExprFunction( aConvertExprParamInfo,
                                                          aEnv )
                                 == STL_SUCCESS );
                        STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

                        STL_TRY( qlncMakeFilter( aConvertExprParamInfo,
                                                 &sRefNodeList,
                                                 sFuncID,
                                                 &sSubQueryExprList,
                                                 NULL,
                                                 &sResultFilter,
                                                 aEnv )
                                 == STL_SUCCESS );

                        _QLNC_FINI_SUBQUERY_LIST( );
                    }
                }

                break;
            }
        case QLV_EXPR_TYPE_CAST:
            {
                _QLNC_INIT_SUBQUERY_LIST( );

                STL_TRY( qlncConvertExprTypeCast( aConvertExprParamInfo,
                                                  aEnv )
                         == STL_SUCCESS );
                STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

                STL_TRY( qlncMakeFilter( aConvertExprParamInfo,
                                         &sRefNodeList,
                                         KNL_BUILTIN_FUNC_CAST,
                                         &sSubQueryExprList,
                                         NULL,
                                         &sResultFilter,
                                         aEnv )
                         == STL_SUCCESS );

                _QLNC_FINI_SUBQUERY_LIST( );

                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION:
            {
                knlBuiltInFunc        sFuncID;

                sFuncID = aConvertExprParamInfo->mInitExpr->mExpr.mListFunc->mFuncId;

                _QLNC_INIT_SUBQUERY_LIST( );

                STL_TRY( qlncConvertExprListFunction( aConvertExprParamInfo,
                                                      aEnv )
                         == STL_SUCCESS );
                STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

                STL_TRY( qlncMakeFilter( aConvertExprParamInfo,
                                         &sRefNodeList,
                                         sFuncID,
                                         &sSubQueryExprList,
                                         NULL,
                                         &sResultFilter,
                                         aEnv )
                         == STL_SUCCESS );

                _QLNC_FINI_SUBQUERY_LIST( );

                break;
            }
        case QLV_EXPR_TYPE_CASE_EXPR:
            {
                knlBuiltInFunc        sFuncID;

                sFuncID = aConvertExprParamInfo->mInitExpr->mExpr.mCaseExpr->mFuncId;

                _QLNC_INIT_SUBQUERY_LIST( );

                STL_TRY( qlncConvertExprCaseExpr( aConvertExprParamInfo,
                                                  aEnv )
                         == STL_SUCCESS );
                STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

                STL_TRY( qlncMakeFilter( aConvertExprParamInfo,
                                         &sRefNodeList,
                                         sFuncID,
                                         &sSubQueryExprList,
                                         NULL,
                                         &sResultFilter,
                                         aEnv )
                         == STL_SUCCESS );

                _QLNC_FINI_SUBQUERY_LIST( );

                break;
            }
        case QLV_EXPR_TYPE_VALUE:
        case QLV_EXPR_TYPE_BIND_PARAM:
        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT:
            {
                qlncConstBooleanFilter  * sConstBooleanFilter   = NULL;

                _QLNC_INIT_SUBQUERY_LIST( );

                /* Const Boolean Filter 할당 */
                STL_TRY( knlAllocRegionMem(
                             &QLL_CANDIDATE_MEM( aEnv ),
                             STL_SIZEOF( qlncConstBooleanFilter ),
                             (void**)&sConstBooleanFilter,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                QLNC_SET_EXPR_COMMON( &sConstBooleanFilter->mCommon,
                                      &QLL_CANDIDATE_MEM( aEnv ),
                                      aEnv,
                                      QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER,
                                      aConvertExprParamInfo->mInitExpr->mPhraseType,
                                      aConvertExprParamInfo->mInitExpr->mIterationTime,         
                                      aConvertExprParamInfo->mInitExpr->mPosition,              
                                      aConvertExprParamInfo->mInitExpr->mColumnName,            
                                      DTL_TYPE_BOOLEAN );                                       
                
                aConvertExprParamInfo->mRefNodeList = NULL;

                STL_TRY( qlncConvertExprFuncList[ aConvertExprParamInfo->mInitExpr->mType ]( aConvertExprParamInfo,
                                                                                             aEnv )
                         == STL_SUCCESS );
                STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

                sConstBooleanFilter->mExpr = aConvertExprParamInfo->mCandExpr;

                if( sSubQueryExprList.mCount > 0 )
                {
                    /* @todo 현재 constant expr result가 subquery를 갖는 경우는 없는 것으로 판단됨 */

                    STL_TRY( knlAllocRegionMem(
                                 &QLL_CANDIDATE_MEM( aEnv ),
                                 STL_SIZEOF( qlncRefExprList ),
                                 (void**)&(sConstBooleanFilter->mSubQueryExprList),
                                 KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    sConstBooleanFilter->mSubQueryExprList->mCount = sSubQueryExprList.mCount;
                    sConstBooleanFilter->mSubQueryExprList->mHead = sSubQueryExprList.mHead;
                    sConstBooleanFilter->mSubQueryExprList->mTail = sSubQueryExprList.mTail;
                }
                else
                {
                    sConstBooleanFilter->mSubQueryExprList = NULL;
                }

                sResultFilter = (qlncExprCommon*)sConstBooleanFilter;

                _QLNC_FINI_SUBQUERY_LIST( );

                break;
            }
        case QLV_EXPR_TYPE_COLUMN:
        case QLV_EXPR_TYPE_PSEUDO_COLUMN:
        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT:
        case QLV_EXPR_TYPE_INNER_COLUMN:
        case QLV_EXPR_TYPE_OUTER_COLUMN:
        case QLV_EXPR_TYPE_ROWID_COLUMN:
        case QLV_EXPR_TYPE_AGGREGATION:
        case QLV_EXPR_TYPE_SUB_QUERY:
            {
                _QLNC_INIT_SUBQUERY_LIST( );

                STL_TRY( qlncConvertExprFuncList[ aConvertExprParamInfo->mInitExpr->mType ]( aConvertExprParamInfo,
                                                                                             aEnv )
                         == STL_SUCCESS );
                STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

                STL_TRY( qlncMakeFilter( aConvertExprParamInfo,
                                         &sRefNodeList,
                                         KNL_BUILTIN_FUNC_INVALID,
                                         &sSubQueryExprList,
                                         NULL,
                                         &sResultFilter,
                                         aEnv )
                         == STL_SUCCESS );

                _QLNC_FINI_SUBQUERY_LIST( );

                break;
            }
        case QLV_EXPR_TYPE_LIST_COLUMN:
        case QLV_EXPR_TYPE_ROW_EXPR:
        default:
            STL_DASSERT( 0 );
            break;
    }


    /* Merge Ref Node List */
    if( ( sRefNodeList.mCount > 0 ) && ( sBackupRefNodeList != NULL ) )
    {
        /* Merge List */
        if( sBackupRefNodeList->mCount == 0 )
        {
            QLNC_COPY_LIST_INFO( &sRefNodeList, sBackupRefNodeList );
        }
        else
        {
            sRefSrcNodeItem = sRefNodeList.mHead;
            
            for( sLoop = 0 ; sLoop < sRefNodeList.mCount ; sLoop++ )
            {
                /* 새로 추가하는 경우 */
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncRefNodeItem ),
                                            (void*)&sRefDstNodeItem,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                /* Column Item 정보 설정 */
                sRefDstNodeItem->mNode = sRefSrcNodeItem->mNode;
                sRefDstNodeItem->mRefColumnList = sRefSrcNodeItem->mRefColumnList;
                sRefDstNodeItem->mNext = NULL;
                
                /* Column Item 리스트 구성 */
                sBackupRefNodeList->mTail->mNext = sRefDstNodeItem;
                sBackupRefNodeList->mTail = sRefDstNodeItem;
                
                sRefSrcNodeItem = sRefSrcNodeItem->mNext;
            }
            
            sBackupRefNodeList->mCount += sRefNodeList.mCount;
        }
    }
    
    aConvertExprParamInfo->mRefNodeList = sBackupRefNodeList;


    /**
     * Output 설정
     */

    aConvertExprParamInfo->mCandExpr = sResultFilter;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Compare Function Boolean Filter를 Convert한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertCompareBooleanFilter( qlncConvertExprParamInfo * aConvertExprParamInfo,
                                           qllEnv                   * aEnv )
{
    qlvInitExpression   * sInitExpr         = NULL;
    qlvInitFunction     * sInitFunction     = NULL;
    qlncExprFunction    * sCandFunction     = NULL;
    qlncRefNodeList       sLeftRefNodeList;
    qlncRefNodeList       sRightRefNodeList;
    stlBool               sIsJoinCondition;
    qlncRefNodeItem     * sRefNodeItem      = NULL;

    qlncConstBooleanFilter  * sConstBooleanFilter   = NULL;
    qlncBooleanFilter       * sBooleanFilter        = NULL;
    qlncRefColumnItem       * sRefColumnItem        = NULL;

    qlncRefNodeList     * sBackupRefNodeList = NULL;
    qlncRefNodeItem     * sRefSrcNodeItem    = NULL;
    qlncRefNodeItem     * sRefDstNodeItem    = NULL;
    stlInt32              sLoop;
    qlncRefExprList     * sBackupSubQueryExprList   = NULL;
    qlncRefExprList       sSubQueryExprList;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aConvertExprParamInfo->mInitExpr->mType == QLV_EXPR_TYPE_FUNCTION );
    sInitExpr = aConvertExprParamInfo->mInitExpr;
    sInitFunction = aConvertExprParamInfo->mInitExpr->mExpr.mFunction;

    /* Candidate Function Expression 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprFunction ),
                                (void**) &sCandFunction,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Function Expression 정보 셋팅 */
    sCandFunction->mFuncId = sInitFunction->mFuncId;
    sCandFunction->mArgCount = sInitFunction->mArgCount;

    STL_DASSERT( sInitFunction->mArgCount == 2 );
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ) * 2,
                                (void**) &sCandFunction->mArgs,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Backup Ref Node List */
    sBackupRefNodeList = aConvertExprParamInfo->mRefNodeList;

    QLNC_INIT_LIST( &sSubQueryExprList );
    sBackupSubQueryExprList = aConvertExprParamInfo->mSubQueryExprList;
    aConvertExprParamInfo->mSubQueryExprList = &sSubQueryExprList;

    /* Left Argument */
    QLNC_INIT_LIST( &sLeftRefNodeList );
    aConvertExprParamInfo->mRefNodeList = &sLeftRefNodeList;

    aConvertExprParamInfo->mInitExpr = sInitFunction->mArgs[0];
    aConvertExprParamInfo->mCandExpr = NULL;

    STL_TRY( qlncConvertExprFuncList[ sInitFunction->mArgs[0]->mType ]( aConvertExprParamInfo,
                                                                        aEnv )
             == STL_SUCCESS );
    STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

    sCandFunction->mArgs[0] = aConvertExprParamInfo->mCandExpr;

    /* Right Argument */
    QLNC_INIT_LIST( &sRightRefNodeList );
    aConvertExprParamInfo->mRefNodeList = &sRightRefNodeList;

    aConvertExprParamInfo->mInitExpr = sInitFunction->mArgs[1];
    aConvertExprParamInfo->mCandExpr = NULL;

    STL_TRY( qlncConvertExprFuncList[ sInitFunction->mArgs[1]->mType ]( aConvertExprParamInfo,
                                                                        aEnv )
             == STL_SUCCESS );
    STL_DASSERT( aConvertExprParamInfo->mCandExpr != NULL );

    sCandFunction->mArgs[1] = aConvertExprParamInfo->mCandExpr;

    /* Expression Common 정보 셋팅 */
    QLNC_SET_EXPR_COMMON( &sCandFunction->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_FUNCTION,
                          aConvertExprParamInfo->mExprPhraseFlag,
                          sInitExpr->mIterationTime,
                          sInitExpr->mPosition,
                          sInitExpr->mColumnName,
                          DTL_TYPE_NA );

    /* DB Type 설정 */
    STL_TRY( qloGetDataType( aConvertExprParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                             sInitExpr,
                             aConvertExprParamInfo->mParamInfo.mSQLStmt->mBindContext,
                             &sCandFunction->mCommon.mDataType,
                             &QLL_CANDIDATE_MEM( aEnv ),
                             aEnv )
             == STL_SUCCESS );


    /**
     * Boolean Filter 관련 처리
     */

    /* Join Condition으로 사용 가능한지 판단 */
    if( (sLeftRefNodeList.mCount == 1) &&
        (sRightRefNodeList.mCount > 0))
    {
        sIsJoinCondition = STL_TRUE;
        sRefNodeItem = sRightRefNodeList.mHead;
        while( sRefNodeItem != NULL )
        {
            if( sLeftRefNodeList.mHead->mNode ==
                sRefNodeItem->mNode )
            {
                sIsJoinCondition = STL_FALSE;
                break;
            }

            sRefNodeItem = sRefNodeItem->mNext;
        }
    }
    else if( (sRightRefNodeList.mCount == 1) &&
             (sLeftRefNodeList.mCount > 0))
    {
        sIsJoinCondition = STL_TRUE;
        sRefNodeItem = sLeftRefNodeList.mHead;
        while( sRefNodeItem != NULL )
        {
            if( sRightRefNodeList.mHead->mNode ==
                sRefNodeItem->mNode )
            {
                sIsJoinCondition = STL_FALSE;
                break;
            }

            sRefNodeItem = sRefNodeItem->mNext;
        }
    }
    else
    {
        sIsJoinCondition = STL_FALSE;
    }

    /* Filter 등록 */
    if( (sLeftRefNodeList.mCount == 0) &&
        (sRightRefNodeList.mCount == 0) )
    {
        STL_DASSERT( sIsJoinCondition == STL_FALSE );

        /* Const Boolean Filter 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncConstBooleanFilter ),
                                    (void**) &sConstBooleanFilter,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_SET_EXPR_COMMON( &sConstBooleanFilter->mCommon,
                              &QLL_CANDIDATE_MEM( aEnv ),
                              aEnv,
                              QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER,
                              sCandFunction->mCommon.mExprPhraseFlag,        
                              sCandFunction->mCommon.mIterationTime,         
                              sCandFunction->mCommon.mPosition,              
                              sCandFunction->mCommon.mColumnName,            
                              DTL_TYPE_BOOLEAN );                                       

        sConstBooleanFilter->mExpr = (qlncExprCommon*)sCandFunction;

        /* SubQuery Expression List 등록 */
        if( sSubQueryExprList.mCount > 0 )
        {
            STL_TRY( knlAllocRegionMem(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         STL_SIZEOF( qlncRefExprList ),
                         (void**)&(sConstBooleanFilter->mSubQueryExprList),
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            sConstBooleanFilter->mSubQueryExprList->mCount = sSubQueryExprList.mCount;
            sConstBooleanFilter->mSubQueryExprList->mHead = sSubQueryExprList.mHead;
            sConstBooleanFilter->mSubQueryExprList->mTail = sSubQueryExprList.mTail;
        }
        else
        {
            sConstBooleanFilter->mSubQueryExprList = NULL;
        }

        aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sConstBooleanFilter;
    }
    else
    {
        /* Boolean Filter 할당 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncBooleanFilter ),
                                    (void**) &sBooleanFilter,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_SET_EXPR_COMMON( &sBooleanFilter->mCommon,
                              &QLL_CANDIDATE_MEM( aEnv ),
                              aEnv,
                              QLNC_EXPR_TYPE_BOOLEAN_FILTER,
                              sCandFunction->mCommon.mExprPhraseFlag,        
                              sCandFunction->mCommon.mIterationTime,         
                              sCandFunction->mCommon.mPosition,              
                              sCandFunction->mCommon.mColumnName,            
                              DTL_TYPE_BOOLEAN );                                       

        sBooleanFilter->mPossibleJoinCond = sIsJoinCondition;
        sBooleanFilter->mFuncID = sInitFunction->mFuncId;
        sBooleanFilter->mExpr = (qlncExprCommon*)sCandFunction;

        /* SubQuery Expression List 등록 */
        if( sSubQueryExprList.mCount > 0 )
        {
            STL_TRY( knlAllocRegionMem(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         STL_SIZEOF( qlncRefExprList ),
                         (void**)&(sBooleanFilter->mSubQueryExprList),
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            sBooleanFilter->mSubQueryExprList->mCount = sSubQueryExprList.mCount;
            sBooleanFilter->mSubQueryExprList->mHead = sSubQueryExprList.mHead;
            sBooleanFilter->mSubQueryExprList->mTail = sSubQueryExprList.mTail;
        }
        else
        {
            sBooleanFilter->mSubQueryExprList = NULL;
        }

        /* Join 관련 Reference Node List 등록 */
        if( sIsJoinCondition == STL_TRUE )
        {
            /* Left Expression에 있는 Reference Node List 설정 */
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefNodeList ),
                                        (void**) &(sBooleanFilter->mLeftRefNodeList),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            QLNC_COPY_LIST_INFO( &(sLeftRefNodeList), sBooleanFilter->mLeftRefNodeList );

            /* Right Expression에 있는 Reference Node List 설정 */
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncRefNodeList ),
                                        (void**) &(sBooleanFilter->mRightRefNodeList),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            QLNC_COPY_LIST_INFO( &(sRightRefNodeList), sBooleanFilter->mRightRefNodeList );
        }
        else
        {
            sBooleanFilter->mLeftRefNodeList = NULL;
            sBooleanFilter->mRightRefNodeList = NULL;
        }

        QLNC_INIT_LIST( &(sBooleanFilter->mRefNodeList) );

        /* Append Left Reference Node List */
        aConvertExprParamInfo->mRefNodeList = &(sBooleanFilter->mRefNodeList);
        sRefNodeItem = sLeftRefNodeList.mHead;
        while( sRefNodeItem != NULL )
        {
            sRefColumnItem = sRefNodeItem->mRefColumnList.mHead;
            while( sRefColumnItem != NULL )
            {
                STL_TRY( qlncAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                        sRefColumnItem->mExpr,
                                                        aEnv )
                         == STL_SUCCESS );

                sRefColumnItem = sRefColumnItem->mNext;
            }

            sRefNodeItem = sRefNodeItem->mNext;
        }

        /* Append Right Reference Node List */
        aConvertExprParamInfo->mRefNodeList = &(sBooleanFilter->mRefNodeList);
        sRefNodeItem = sRightRefNodeList.mHead;
        while( sRefNodeItem != NULL )
        {
            sRefColumnItem = sRefNodeItem->mRefColumnList.mHead;
            while( sRefColumnItem != NULL )
            {
                STL_TRY( qlncAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                        sRefColumnItem->mExpr,
                                                        aEnv )
                         == STL_SUCCESS );

                sRefColumnItem = sRefColumnItem->mNext;
            }

            sRefNodeItem = sRefNodeItem->mNext;
        }

        aConvertExprParamInfo->mCandExpr = (qlncExprCommon*)sBooleanFilter;

        /* Merge Ref Node List */
        if( ( sBooleanFilter->mRefNodeList.mCount > 0 ) && ( sBackupRefNodeList != NULL ) )
        {
            /* Merge List */
            if( sBackupRefNodeList->mCount == 0 )
            {
                QLNC_COPY_LIST_INFO( &sBooleanFilter->mRefNodeList, sBackupRefNodeList );
            }
            else
            {
                /* @todo 현재 Backup Ref Node List가 1개 이상 존재하는 경우가 없는 것으로 판단됨 */

                sRefSrcNodeItem = sBooleanFilter->mRefNodeList.mHead;
            
                for( sLoop = 0 ; sLoop < sBooleanFilter->mRefNodeList.mCount ; sLoop++ )
                {
                    /* 새로 추가하는 경우 */
                    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                STL_SIZEOF( qlncRefNodeItem ),
                                                (void*)&sRefDstNodeItem,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    /* Column Item 정보 설정 */
                    sRefDstNodeItem->mNode = sRefSrcNodeItem->mNode;
                    sRefDstNodeItem->mRefColumnList = sRefSrcNodeItem->mRefColumnList;
                    sRefDstNodeItem->mNext = NULL;
                
                    /* Column Item 리스트 구성 */
                    sBackupRefNodeList->mTail->mNext = sRefDstNodeItem;
                    sBackupRefNodeList->mTail = sRefDstNodeItem;
                
                    sRefSrcNodeItem = sRefSrcNodeItem->mNext;
                }
            
                sBackupRefNodeList->mCount += sBooleanFilter->mRefNodeList.mCount;
            }
        }
    }

    if( sSubQueryExprList.mCount > 0 )
    {
        if( sBackupSubQueryExprList != NULL )
        {
            STL_TRY( qlncAddExprListToRefExprList(
                         &QLL_CANDIDATE_MEM( aEnv ),
                         sBackupSubQueryExprList,
                         &sSubQueryExprList,
                         aEnv )
                     == STL_SUCCESS );
        }
    }

    aConvertExprParamInfo->mSubQueryExprList = sBackupSubQueryExprList;
    aConvertExprParamInfo->mRefNodeList = sBackupRefNodeList;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Expression에 Reference Column 정보를 설정한다.
 * @param[in]       aRegionMem      Region Memory
 * @param[in]       aRefColumn      Reference Column
 * @param[in,out]   aExprSubQuery   SubQuery Expression
 * @param[in]       aEnv            Environment
 */
stlStatus qlncSetRefColumnInfoOnExprSubQuery( knlRegionMem      * aRegionMem,
                                              qlncExprCommon    * aRefColumn,
                                              qlncExprSubQuery  * aExprSubQuery,
                                              qllEnv            * aEnv )
{
    qlncRefExprItem * sRefExprItem  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefColumn != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSubQuery != NULL, QLL_ERROR_STACK(aEnv) );


    /* Reference Expression List가 존재하는지 판단 */
    if( aExprSubQuery->mRefExprList == NULL )
    {
        /* Reference Expression List 할당 */
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlncRefExprList ),
                                    (void**) &(aExprSubQuery->mRefExprList),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Reference Expression List 초기화 */
        QLNC_INIT_LIST( aExprSubQuery->mRefExprList );
    }

    /* Reference Expression Item 할당 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlncRefExprItem ),
                                (void**) &sRefExprItem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Reference Expression Item 정보 설정 */
    sRefExprItem->mExpr = aRefColumn;
    sRefExprItem->mNext = NULL;

    /* Reference Expression List에 등록 */
    QLNC_APPEND_ITEM_TO_LIST( aExprSubQuery->mRefExprList, sRefExprItem );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Validation Expression으로부터 And Filter를 생성한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aCondition              Validation Condition
 * @param[in]   aExprPhraseFlag         Expression Phrase Flag
 * @param[out]  aAndFilter              And Filter
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncConvertCondition( qlncCreateNodeParamInfo * aCreateNodeParamInfo,
                                qlvInitExpression       * aCondition,
                                stlInt32                  aExprPhraseFlag,
                                qlncAndFilter          ** aAndFilter,
                                qllEnv                  * aEnv )
{
    qlncConvertExprParamInfo      sConvertExprParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCondition != NULL, QLL_ERROR_STACK(aEnv) );


    sConvertExprParamInfo.mParamInfo = aCreateNodeParamInfo->mParamInfo;
    sConvertExprParamInfo.mTableMapArr = aCreateNodeParamInfo->mTableMapArr;
    sConvertExprParamInfo.mOuterTableMapArr = aCreateNodeParamInfo->mOuterTableMapArr;
    sConvertExprParamInfo.mInitExpr = aCondition;
    sConvertExprParamInfo.mCandExpr = NULL;
    sConvertExprParamInfo.mRefNodeList = NULL;
    sConvertExprParamInfo.mExprPhraseFlag = aExprPhraseFlag;
    sConvertExprParamInfo.mSubQueryExprList = NULL;
    sConvertExprParamInfo.mExprSubQuery = aCreateNodeParamInfo->mExprSubQuery;
    sConvertExprParamInfo.mQBMapArr = aCreateNodeParamInfo->mQBMapArr;


    /**
     * Convert Expression 함수 호출
     */

    STL_TRY( qlncConvertAndFilter( &sConvertExprParamInfo,
                                   aEnv )
             == STL_SUCCESS );
    STL_DASSERT( sConvertExprParamInfo.mCandExpr != NULL );


    /**
     * Output 설정
     */

    *aAndFilter = (qlncAndFilter*)(sConvertExprParamInfo.mCandExpr);


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Validation Expression으로부터 Candidate Expression을 생성한다.
 * @param[in]       aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]       aCondition              Validation Condition
 * @param[in]       aExprPhraseFlag         Expression Phrase Flag
 * @param[in,out]   aSubQueryExprList       SubQuery Expression List
 * @param[out]      aExpr                   Expression
 * @param[in]       aEnv                    Environment
 */
stlStatus qlncConvertExpression( qlncCreateNodeParamInfo    * aCreateNodeParamInfo,
                                 qlvInitExpression          * aCondition,
                                 stlInt32                     aExprPhraseFlag,
                                 qlncRefExprList            * aSubQueryExprList,
                                 qlncExprCommon            ** aExpr,
                                 qllEnv                     * aEnv )
{
    qlncConvertExprParamInfo      sConvertExprParamInfo;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCondition != NULL, QLL_ERROR_STACK(aEnv) );


    sConvertExprParamInfo.mParamInfo = aCreateNodeParamInfo->mParamInfo;
    sConvertExprParamInfo.mTableMapArr = aCreateNodeParamInfo->mTableMapArr;
    sConvertExprParamInfo.mOuterTableMapArr = aCreateNodeParamInfo->mOuterTableMapArr;
    sConvertExprParamInfo.mInitExpr = aCondition;
    sConvertExprParamInfo.mCandExpr = NULL;
    sConvertExprParamInfo.mRefNodeList = NULL;
    sConvertExprParamInfo.mExprPhraseFlag = aExprPhraseFlag;
    sConvertExprParamInfo.mSubQueryExprList = aSubQueryExprList;
    sConvertExprParamInfo.mExprSubQuery = aCreateNodeParamInfo->mExprSubQuery;
    sConvertExprParamInfo.mQBMapArr = aCreateNodeParamInfo->mQBMapArr;


    /**
     * Convert Expression 함수 호출
     */

    STL_TRY( qlncConvertExprFuncList[ aCondition->mType ]( &sConvertExprParamInfo,
                                                           aEnv )
             == STL_SUCCESS );
    STL_DASSERT( sConvertExprParamInfo.mCandExpr != NULL );


    /**
     * Output 설정
     */

    *aExpr = sConvertExprParamInfo.mCandExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Or Filter를 And Filter의 Head에 Append한다.
 * @param[in]       aParamInfo      Common Parameter Info
 * @param[in,out]   aAndFilter      And Filter
 * @param[in]       aOrFilterCount  Or Filter Count
 * @param[in]       aOrFilterPtrArr Or Filter Pointer Array
 * @param[in]       aEnv            Environment
 */
stlStatus qlncAppendOrFilterToAndFilterHead( qlncParamInfo  * aParamInfo,
                                             qlncAndFilter  * aAndFilter,
                                             stlInt32         aOrFilterCount,
                                             qlncOrFilter  ** aOrFilterPtrArr,
                                             qllEnv         * aEnv )
{
    stlInt32          sArgCount;
    qlncOrFilter   ** sOrFilter     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter->mCommon.mType == QLNC_EXPR_TYPE_AND_FILTER,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrFilterCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrFilterPtrArr != NULL, QLL_ERROR_STACK(aEnv) );


    sArgCount = aAndFilter->mOrCount + aOrFilterCount;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOrFilter* ) * sArgCount,
                                (void**) &sOrFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Or Filter 복사 */
    stlMemcpy( sOrFilter,
               aOrFilterPtrArr,
               STL_SIZEOF( qlncOrFilter* ) * aOrFilterCount );

    /* And Filter에 존재하던 Or Filter를 Append */
    if( aAndFilter->mOrCount > 0 )
    {
        stlMemcpy( (stlChar*)sOrFilter + STL_SIZEOF( qlncOrFilter* ) * aOrFilterCount,
                   aAndFilter->mOrFilters,
                   STL_SIZEOF( qlncOrFilter* ) * aAndFilter->mOrCount );
    }

    aAndFilter->mOrCount = sArgCount;
    aAndFilter->mOrFilters = sOrFilter;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Or Filter를 And Filter의 Tail에 Append한다.
 * @param[in]       aParamInfo      Common Parameter Info
 * @param[in,out]   aAndFilter      And Filter
 * @param[in]       aOrFilterCount  Or Filter Count
 * @param[in]       aOrFilterPtrArr Or Filter Pointer Array
 * @param[in]       aEnv            Environment
 */
stlStatus qlncAppendOrFilterToAndFilterTail( qlncParamInfo  * aParamInfo,
                                             qlncAndFilter  * aAndFilter,
                                             stlInt32         aOrFilterCount,
                                             qlncOrFilter  ** aOrFilterPtrArr,
                                             qllEnv         * aEnv )
{
    stlInt32          sArgCount;
    qlncOrFilter   ** sOrFilter     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter->mCommon.mType == QLNC_EXPR_TYPE_AND_FILTER,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrFilterCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOrFilterPtrArr != NULL, QLL_ERROR_STACK(aEnv) );


    sArgCount = aAndFilter->mOrCount + aOrFilterCount;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOrFilter* ) * sArgCount,
                                (void**) &sOrFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* And Filter에 존재하던 Or Filter 복사 */
    if( aAndFilter->mOrCount > 0 )
    {
        stlMemcpy( sOrFilter,
                   aAndFilter->mOrFilters,
                   STL_SIZEOF( qlncOrFilter* ) * aAndFilter->mOrCount );
    }

    /* Append Or Filter */
    stlMemcpy( (stlChar*)sOrFilter + STL_SIZEOF( qlncOrFilter* ) * aAndFilter->mOrCount,
               aOrFilterPtrArr,
               STL_SIZEOF( qlncOrFilter* ) * aOrFilterCount );

    aAndFilter->mOrCount = sArgCount;
    aAndFilter->mOrFilters = sOrFilter;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief And Filter에 Filter를 Append한다.
 * @param[in]       aParamInfo      Common Parameter Info
 * @param[in,out]   aAndFilter      And Filter
 * @param[in]       aFilter         Filter
 * @param[in]       aEnv            Environment
 */
stlStatus qlncAppendFilterToAndFilter( qlncParamInfo    * aParamInfo,
                                       qlncAndFilter    * aAndFilter,
                                       qlncExprCommon   * aFilter,
                                       qllEnv           * aEnv )
{
    stlInt32          sArgCount;
    qlncOrFilter   ** sOrFilter     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter->mCommon.mType == QLNC_EXPR_TYPE_AND_FILTER,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFilter != NULL, QLL_ERROR_STACK(aEnv) );


    sArgCount = aAndFilter->mOrCount + 1;

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOrFilter* ) * sArgCount,
                                (void**) &sOrFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* And Filter에 존재하던 Or Filter 복사 */
    stlMemcpy( sOrFilter,
               aAndFilter->mOrFilters,
               STL_SIZEOF( qlncOrFilter* ) * aAndFilter->mOrCount );

    /* Set Or Filter */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOrFilter ),
                                (void**)&sOrFilter[aAndFilter->mOrCount],
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLNC_SET_EXPR_COMMON( &sOrFilter[aAndFilter->mOrCount]->mCommon,
                          &QLL_CANDIDATE_MEM( aEnv ),
                          aEnv,
                          QLNC_EXPR_TYPE_OR_FILTER,
                          aFilter->mExprPhraseFlag,        
                          aFilter->mIterationTime,         
                          aFilter->mPosition,              
                          aFilter->mColumnName,
                          DTL_TYPE_BOOLEAN );                                       

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncExprCommon* ),
                                (void**)&(sOrFilter[aAndFilter->mOrCount]->mFilters),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sOrFilter[aAndFilter->mOrCount]->mFilterCount = 1;
    sOrFilter[aAndFilter->mOrCount]->mFilters[0] = aFilter;
    if( aFilter->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER )
    {
        sOrFilter[aAndFilter->mOrCount]->mSubQueryExprList =
            ((qlncBooleanFilter*)aFilter)->mSubQueryExprList;
    }
    else
    {
        sOrFilter[aAndFilter->mOrCount]->mSubQueryExprList =
            ((qlncConstBooleanFilter*)aFilter)->mSubQueryExprList;
    }
    
    
    aAndFilter->mOrCount = sArgCount;
    aAndFilter->mOrFilters = sOrFilter;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expression에서 등록할 컬럼을 찾아 Reference Node List에 추가한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aExpr                   Candidate Expression
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncFindAddRefColumnToRefNodeList( qlncConvertExprParamInfo   * aConvertExprParamInfo,
                                             qlncExprCommon             * aExpr,
                                             qllEnv                     * aEnv )
{
    stlInt32      i;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvertExprParamInfo->mRefNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
        case QLNC_EXPR_TYPE_BIND_PARAM:
        case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
        case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
            {
                break;
            }
        case QLNC_EXPR_TYPE_COLUMN:
        case QLNC_EXPR_TYPE_ROWID_COLUMN:
            {
                STL_TRY( qlncAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                        aExpr,
                                                        aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_FUNCTION:
            {
                qlncExprFunction    * sFunction = NULL;

                sFunction = (qlncExprFunction*)aExpr;
                for( i = 0; i < sFunction->mArgCount; i++ )
                {
                    STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                                sFunction->mArgs[i],
                                                                aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_CAST:
            {
                qlncExprTypeCast    * sTypeCast = NULL;

                sTypeCast = (qlncExprTypeCast*)aExpr;
                for( i = 0; i < sTypeCast->mArgCount; i++ )
                {
                    STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                                sTypeCast->mArgs[i],
                                                                aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
            {
                STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                            ((qlncExprConstExpr*)aExpr)->mOrgExpr,
                                                            aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_SUB_QUERY:
            {
                qlncRefExprItem * sRefExprItem  = NULL;

                if( ((qlncExprSubQuery*)aExpr)->mRefExprList != NULL )
                {
                    sRefExprItem = ((qlncExprSubQuery*)aExpr)->mRefExprList->mHead;
                    while( sRefExprItem != NULL )
                    {
                        STL_TRY( qlncAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                                sRefExprItem->mExpr,
                                                                aEnv )
                                 == STL_SUCCESS );

                        sRefExprItem = sRefExprItem->mNext;
                    }
                }
                break;
            }
        case QLNC_EXPR_TYPE_REFERENCE:
            {
                /* @todo Validation에서는 현재 reference를 만들지 않는다. */

                STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                            ((qlncExprReference*)aExpr)->mOrgExpr,
                                                            aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_INNER_COLUMN:
            {
                if( (((qlncExprInnerColumn*)aExpr)->mNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE) ||
                    (((qlncExprInnerColumn*)aExpr)->mNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT) ||
                    (((qlncExprInnerColumn*)aExpr)->mNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT) )
                {
                    STL_TRY( qlncAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                            aExpr,
                                                            aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /* @todo Validation에서는 현재 Sub Table 혹은 Instant를 제외하고는
                     * Filter에서 참조하는 다른 Inner Column이 없다. */

                    STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                                ((qlncExprInnerColumn*)aExpr)->mOrgExpr,
                                                                aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_OUTER_COLUMN:
            {
                /* @todo Validation에서는 현재 outer column을 만들지 않는다. */

                STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                            ((qlncExprOuterColumn*)aExpr)->mOrgExpr,
                                                            aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_AGGREGATION:
            {
                qlncExprAggregation * sAggregation  = NULL;

                sAggregation = (qlncExprAggregation*)aExpr;
                for( i = 0; i < sAggregation->mArgCount; i++ )
                {
                    STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                                sAggregation->mArgs[i],
                                                                aEnv )
                             == STL_SUCCESS );
                }

                if( sAggregation->mFilter != NULL )
                {
                    STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                                sAggregation->mFilter,
                                                                aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_CASE_EXPR:
            {
                qlncExprCaseExpr    * sCaseExpr = NULL;

                sCaseExpr = (qlncExprCaseExpr*)aExpr;
                for( i = 0; i < sCaseExpr->mCount; i++ )
                {
                    STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                                sCaseExpr->mWhenArr[i],
                                                                aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                                sCaseExpr->mThenArr[i],
                                                                aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                            sCaseExpr->mDefResult,
                                                            aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_EXPR_TYPE_LIST_FUNCTION:
            {
                qlncExprListFunc    * sListFunc = NULL;

                sListFunc = (qlncExprListFunc*)aExpr;
                for( i = 0; i < sListFunc->mArgCount; i++ )
                {
                    STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                                sListFunc->mArgs[i],
                                                                aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_LIST_COLUMN:
            {
                qlncExprListCol * sListCol  = NULL;

                sListCol = (qlncExprListCol*)aExpr;
                for( i = 0; i < sListCol->mArgCount; i++ )
                {
                    STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                                sListCol->mArgs[i],
                                                                aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_ROW_EXPR:
            {
                qlncExprRowExpr * sRowExpr  = NULL;

                sRowExpr = (qlncExprRowExpr*)aExpr;
                for( i = 0; i < sRowExpr->mArgCount; i++ )
                {
                    STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                                sRowExpr->mArgs[i],
                                                                aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_AND_FILTER:
            {
                qlncAndFilter   * sAndFilter    = NULL;

                sAndFilter = (qlncAndFilter*)aExpr;
                for( i = 0; i < sAndFilter->mOrCount; i++ )
                {
                    STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                                (qlncExprCommon*)(sAndFilter->mOrFilters[i]),
                                                                aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_OR_FILTER:
            {
                qlncOrFilter    * sOrFilter = NULL;

                sOrFilter = (qlncOrFilter*)aExpr;
                for( i = 0; i < sOrFilter->mFilterCount; i++ )
                {
                    STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                                sOrFilter->mFilters[i],
                                                                aEnv )
                             == STL_SUCCESS );
                }

                break;
            }
        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
            {
                STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                            ((qlncBooleanFilter*)aExpr)->mExpr,
                                                            aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
            {
                /* @todo Boolean Filter 아래에 Const Boolean Filter가 오는 경우가 없다. */

                STL_TRY( qlncFindAddRefColumnToRefNodeList( aConvertExprParamInfo,
                                                            ((qlncConstBooleanFilter*)aExpr)->mExpr,
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


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Reference Node List에 Reference Column을 추가한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aExpr                   Candidate Expression
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncAddRefColumnToRefNodeList( qlncConvertExprParamInfo   * aConvertExprParamInfo,
                                         qlncExprCommon             * aExpr,
                                         qllEnv                     * aEnv )
{
    stlInt32              sIdx;
    qlncNodeCommon      * sNode                 = NULL;
    qlncRefNodeItem     * sRefCurrNodeItem      = NULL;
    qlncRefNodeItem     * sRefPrevNodeItem      = NULL;
    qlncRefColumnItem   * sRefCurrColumnItem    = NULL;
    qlncRefColumnItem   * sRefPrevColumnItem    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvertExprParamInfo->mRefNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );


    /* Node와 Idx 찾기 */
    if( aExpr->mType == QLNC_EXPR_TYPE_COLUMN )
    {
        sNode = ((qlncExprColumn*)aExpr)->mNode;
        sIdx = ((qlncExprColumn*)aExpr)->mColumnPos;
    }
    else if( aExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN )
    {
        sNode = ((qlncExprInnerColumn*)aExpr)->mNode;
        sIdx = ((qlncExprInnerColumn*)aExpr)->mIdx;
    }
    else
    {
        STL_DASSERT( aExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );
        sNode = ((qlncExprRowidColumn*)aExpr)->mNode;
        sIdx = ((qlncExprRowidColumn*)aExpr)->mIdx;
    }

    /* Node 검색 및 추가 */
    sRefCurrNodeItem = aConvertExprParamInfo->mRefNodeList->mHead;
    sRefPrevNodeItem = NULL;
    while( sRefCurrNodeItem != NULL )
    {
        if( sNode->mNodeID <= sRefCurrNodeItem->mNode->mNodeID )
        {
            break;
        }

        sRefPrevNodeItem = sRefCurrNodeItem;
        sRefCurrNodeItem = sRefCurrNodeItem->mNext;
    }

    if( (sRefCurrNodeItem != NULL) && (sNode->mNodeID == sRefCurrNodeItem->mNode->mNodeID) )
    {
        /* 기존에 존재하는 경우 */
        /* Do Nothing */
    }
    else
    {
        /* 새로 추가하는 경우 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncRefNodeItem ),
                                    (void*)&sRefCurrNodeItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRefCurrNodeItem->mNode = sNode;
        sRefCurrNodeItem->mNext = NULL;
        QLNC_INIT_LIST( &(sRefCurrNodeItem->mRefColumnList) );

        /* List에 추가 */
        if( sRefPrevNodeItem == NULL )
        {
            sRefCurrNodeItem->mNext = aConvertExprParamInfo->mRefNodeList->mHead;
            aConvertExprParamInfo->mRefNodeList->mHead = sRefCurrNodeItem;

            if( aConvertExprParamInfo->mRefNodeList->mCount == 0 )
            {
                aConvertExprParamInfo->mRefNodeList->mTail = sRefCurrNodeItem;
            }
        }
        else
        {
            if( sRefPrevNodeItem->mNext == NULL )
            {
                aConvertExprParamInfo->mRefNodeList->mTail = sRefCurrNodeItem;
            }

            sRefCurrNodeItem->mNext = sRefPrevNodeItem->mNext;
            sRefPrevNodeItem->mNext = sRefCurrNodeItem;
        }
        aConvertExprParamInfo->mRefNodeList->mCount++;
    }

    /* Column 검색 및 추가 */
    sRefCurrColumnItem = sRefCurrNodeItem->mRefColumnList.mHead;
    sRefPrevColumnItem = NULL;
    while( sRefCurrColumnItem != NULL )
    {
        if( sIdx <= sRefCurrColumnItem->mIdx )
        {
            break;
        }

        sRefPrevColumnItem = sRefCurrColumnItem;
        sRefCurrColumnItem = sRefCurrColumnItem->mNext;
    }

    if( (sRefCurrColumnItem != NULL) && (sIdx == sRefCurrColumnItem->mIdx) )
    {
        /* 기존에 존재하는 경우 */
        sRefCurrColumnItem->mRefCount++;
    }
    else
    {
        /* 새로 추가하는 경우 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncRefColumnItem ),
                                    (void*)&sRefCurrColumnItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRefCurrColumnItem->mIdx = sIdx;
        sRefCurrColumnItem->mRefCount = 1;
        sRefCurrColumnItem->mExpr = aExpr;
        sRefCurrColumnItem->mNext = NULL;

        /* List에 추가 */
        if( sRefPrevColumnItem == NULL )
        {
            sRefCurrColumnItem->mNext = sRefCurrNodeItem->mRefColumnList.mHead;
            sRefCurrNodeItem->mRefColumnList.mHead = sRefCurrColumnItem;

            if( sRefCurrNodeItem->mRefColumnList.mCount == 0 )
            {
                sRefCurrNodeItem->mRefColumnList.mTail = sRefCurrColumnItem;
            }
        }
        else
        {
            if( sRefPrevColumnItem->mNext == NULL )
            {
                sRefCurrNodeItem->mRefColumnList.mTail = sRefCurrColumnItem;
            }

            sRefCurrColumnItem->mNext = sRefPrevColumnItem->mNext;
            sRefPrevColumnItem->mNext = sRefCurrColumnItem;
        }
        sRefCurrNodeItem->mRefColumnList.mCount++;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Reference Expr List에 Reference Expr Item을 추가한다.
 * @param[in]       aParamInfo      Convert Expression Parameter Info
 * @param[in]       aColumn         Candidate Column Expression
 * @param[in,out]   aRefColumnList  Reference Column Expression List
 * @param[in]       aEnv            Environment
 */
stlStatus qlncAddColumnToRefColumnList( qlncParamInfo       * aParamInfo,
                                        qlncExprCommon      * aColumn,
                                        qlncRefColumnList   * aRefColumnList,
                                        qllEnv              * aEnv )
{
    stlInt32              sIdx;
    qlncRefColumnItem   * sRefCurrColumnItem    = NULL;
    qlncRefColumnItem   * sRefPrevColumnItem    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumn != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefColumnList != NULL, QLL_ERROR_STACK(aEnv) );


    /* Idx 찾기 */
    if( aColumn->mType == QLNC_EXPR_TYPE_COLUMN )
    {
        sIdx = ((qlncExprColumn*)aColumn)->mColumnPos;
    }
    else if( aColumn->mType == QLNC_EXPR_TYPE_INNER_COLUMN )
    {
        sIdx = ((qlncExprInnerColumn*)aColumn)->mIdx;
    }
    else
    {
        STL_DASSERT( aColumn->mType == QLNC_EXPR_TYPE_ROWID_COLUMN );
        sIdx = ((qlncExprRowidColumn*)aColumn)->mIdx;
    }

    /* Column 검색 및 추가 */
    sRefCurrColumnItem = aRefColumnList->mHead;
    sRefPrevColumnItem = NULL;
    while( sRefCurrColumnItem != NULL )
    {
        if( sIdx <= sRefCurrColumnItem->mIdx )
        {
            break;
        }

        sRefPrevColumnItem = sRefCurrColumnItem;
        sRefCurrColumnItem = sRefCurrColumnItem->mNext;
    }

    if( (sRefCurrColumnItem != NULL) && (sIdx == sRefCurrColumnItem->mIdx) )
    {
        /* 기존에 존재하는 경우 */

        sRefCurrColumnItem->mExpr->mExprPhraseFlag |= aColumn->mExprPhraseFlag;
        sRefCurrColumnItem->mRefCount++;
    }
    else
    {
        /* 새로 추가하는 경우 */
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncRefColumnItem ),
                                    (void*)&sRefCurrColumnItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRefCurrColumnItem->mIdx = sIdx;
        sRefCurrColumnItem->mRefCount = 1;
        sRefCurrColumnItem->mExpr = aColumn;
        sRefCurrColumnItem->mNext = NULL;

        /* List에 추가 */
        if( sRefPrevColumnItem == NULL )
        {
            sRefCurrColumnItem->mNext = aRefColumnList->mHead;
            aRefColumnList->mHead = sRefCurrColumnItem;

            if( aRefColumnList->mCount == 0 )
            {
                aRefColumnList->mTail = sRefCurrColumnItem;
            }
        }
        else
        {
            if( sRefPrevColumnItem->mNext == NULL )
            {
                aRefColumnList->mTail = sRefCurrColumnItem;
            }

            sRefCurrColumnItem->mNext = sRefPrevColumnItem->mNext;
            sRefPrevColumnItem->mNext = sRefCurrColumnItem;
        }
        aRefColumnList->mCount++;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Column이 Reference Column List에 존재하는지 판별한다.
 * @param[in]   aRefColumnList  Reference Column List
 * @param[in]   aColumnIdx      Column Index
 */
stlBool qlncIsExistColumnIdxInRefColumnList( qlncRefColumnList  * aRefColumnList,
                                             stlInt32             aColumnIdx )
{
    qlncRefColumnItem   * sRefColumnItem    = NULL;


    STL_DASSERT( aRefColumnList != NULL );


    /* Column 찾기 */
    sRefColumnItem = aRefColumnList->mHead;
    while( sRefColumnItem != NULL )
    {
        if( sRefColumnItem->mIdx == aColumnIdx )
        {
            return STL_TRUE;
        }

        sRefColumnItem = sRefColumnItem->mNext;
    }

    return STL_FALSE;
}


/**
 * @brief And Filter를 Compact한다.
 * @param[in]   aAndFilter      And Filter
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCompactAndFilter( qlncAndFilter   * aAndFilter,
                                qllEnv          * aEnv )
{
    stlInt32      i;
    stlInt32      sIdx;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aAndFilter != NULL, QLL_ERROR_STACK(aEnv) );


    sIdx = 0;
    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        if( aAndFilter->mOrFilters[i] != NULL )
        {
            aAndFilter->mOrFilters[sIdx] = aAndFilter->mOrFilters[i];
            sIdx++;
        }
    }

    aAndFilter->mOrCount = sIdx;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief And Filter를 Copy한다.
 * @param[in]   aParamInfo          Parameter Information
 * @param[in]   aSrcAndFilter       Source And Filter
 * @param[out]  aCopiedAndFilter    Copied And Filter
 * @param[in]   aEnv                Environment
 */
stlStatus qlncCopyAndFilter( qlncParamInfo  * aParamInfo,
                             qlncAndFilter  * aSrcAndFilter,
                             qlncAndFilter ** aCopiedAndFilter,
                             qllEnv         * aEnv )
{
    qlncAndFilter   * sAndFilter    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSrcAndFilter != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCopiedAndFilter != NULL, QLL_ERROR_STACK(aEnv) );


    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncAndFilter ),
                                (void**)&sAndFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemcpy( sAndFilter, aSrcAndFilter, STL_SIZEOF( qlncAndFilter ) );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncOrFilter* ) * aSrcAndFilter->mOrCount,
                                (void**)&(sAndFilter->mOrFilters),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemcpy( sAndFilter->mOrFilters,
               aSrcAndFilter->mOrFilters,
               STL_SIZEOF( qlncOrFilter* ) * aSrcAndFilter->mOrCount );


    /**
     * Output 설정
     */

    *aCopiedAndFilter = sAndFilter;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief FuncID를 이용해 Join Condition이 가능한지를 판단한다.
 * @param[in]   aFuncID     Function ID
 */
stlBool qlncIsPossibleJoinCondition( knlBuiltInFunc aFuncID )
{
    switch( aFuncID )
    {
        case KNL_BUILTIN_FUNC_IS_EQUAL:
        case KNL_BUILTIN_FUNC_IS_NOT_EQUAL:
        case KNL_BUILTIN_FUNC_IS_LESS_THAN:
        case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL:
        case KNL_BUILTIN_FUNC_IS_GREATER_THAN:
        case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL:
        case KNL_BUILTIN_FUNC_IS:
        case KNL_BUILTIN_FUNC_IS_NOT:
            return STL_TRUE;
            break;
        default:
            return STL_FALSE;
            break;
    }

    return STL_FALSE;
}


/**
 * @brief Expression을 Inner Column으로 감싼다.
 * @param[in]   aRegionMem      RegionMemory
 * @param[in]   aExpr           Expression
 * @param[in]   aNode           Candidate Node
 * @param[in]   aIdx            Index Number
 * @param[out]  aWrappedExpr    Wrapped Expression
 * @param[in]   aEnv            Environment
 */
stlStatus qlncWrapExprInInnerColumn( knlRegionMem       * aRegionMem,
                                     qlncExprCommon     * aExpr,
                                     qlncNodeCommon     * aNode,
                                     stlInt32             aIdx,
                                     qlncExprCommon    ** aWrappedExpr,
                                     qllEnv             * aEnv )
{
    qlncExprInnerColumn * sInnerColumn  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );


    /* Inner Column 할당 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlncExprInnerColumn ),
                                (void**) &sInnerColumn,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Inner Column 정보 설정 */
    sInnerColumn->mNode = aNode;
    sInnerColumn->mIdx = aIdx;
    sInnerColumn->mOrgExpr = aExpr;

    /* Expression Common 정보 설정 */
    QLNC_SET_EXPR_COMMON( &sInnerColumn->mCommon,
                          aRegionMem,
                          aEnv,
                          QLNC_EXPR_TYPE_INNER_COLUMN,
                          aExpr->mExprPhraseFlag,
                          aExpr->mIterationTime,
                          aExpr->mPosition,
                          aExpr->mColumnName,
                          aExpr->mDataType );


    /**
     * Output 설정
     */

    *aWrappedExpr = (qlncExprCommon*)sInnerColumn;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Reference Expression List에 Expression 추가 
 * @param[in,out]  aDestExprList   Destination Expression List
 * @param[in]      aSrcExprList    Source Expression List 
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlncAddExprListToRefExprList( knlRegionMem     * aRegionMem,
                                        qlncRefExprList  * aDestExprList,
                                        qlncRefExprList  * aSrcExprList,
                                        qllEnv           * aEnv )
{
    stlInt32           sCount       = 0;
    qlncRefExprItem  * sNewExprItem = NULL;
    qlncRefExprItem  * sSrcExprItem = NULL;

    
    /****************************************************************
     * Paramter Validation
     ****************************************************************/
    
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDestExprList != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Source Expr List 내의 모든 Expression을 Dest Expr List에 삽입
     */

    if( aSrcExprList == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sSrcExprItem = aSrcExprList->mHead;
    
        for( sCount = 0 ; sCount < aSrcExprList->mCount ; sCount++ )
        {
            STL_DASSERT( sSrcExprItem != NULL );


            /**
             * 공간 할당
             */

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( qlncRefExprItem ),
                                        (void **) & sNewExprItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            
            /**
             * Item 초기화
             */
    
            sNewExprItem->mExpr = sSrcExprItem->mExpr;
            sNewExprItem->mNext = NULL;


            /**
             * List 설정
             */

            if( aDestExprList->mCount == 0 )
            {
                aDestExprList->mCount = 1;
                aDestExprList->mHead  = sNewExprItem;
                aDestExprList->mTail  = sNewExprItem;
            }
            else
            {
                aDestExprList->mCount++;
                aDestExprList->mTail->mNext = sNewExprItem;
                aDestExprList->mTail        = sNewExprItem;
            }
        
            sSrcExprItem = sSrcExprItem->mNext;
        }
    }
    

    STL_DASSERT( sSrcExprItem == NULL );
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Or Filter가 Rowid의 Boolean Filter 하나로 구성되었는지 판단한다.
 * @param[in]   aBaseTableNode  Rowid 대상이 되는 Base Table Node
 * @param[in]   aOrFilter       Or Filter
 */
stlBool qlncIsRowidORFilter( qlncBaseTableNode  * aBaseTableNode,
                             qlncOrFilter       * aOrFilter )
{
    stlBool               sIsRowidOrFilter;
    qlncExprFunction    * sFunction         = NULL;

//    qlncRefNodeItem     * sRefNodeItem      = NULL;
//    qlncRefColumnItem   * sRefColumnItem    = NULL;


    sIsRowidOrFilter = STL_FALSE;
    if( (aOrFilter->mFilterCount == 1) &&
        (aOrFilter->mFilters[0]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER) )
    {
        if( ((qlncBooleanFilter*)(aOrFilter->mFilters[0]))->mExpr->mType == QLNC_EXPR_TYPE_FUNCTION )
        {
            sFunction = (qlncExprFunction*)(((qlncBooleanFilter*)(aOrFilter->mFilters[0]))->mExpr);
            if( sFunction->mFuncId == KNL_BUILTIN_FUNC_IS_EQUAL )
            {
                if( (sFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_ROWID_COLUMN) &&
                    (aBaseTableNode->mNodeCommon.mNodeID == ((qlncExprRowidColumn*)(sFunction->mArgs[0]))->mNode->mNodeID) )
                {
                    if( (sFunction->mArgs[1]->mType == QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                        (sFunction->mArgs[1]->mType == QLNC_EXPR_TYPE_REFERENCE) ||
                        (sFunction->mArgs[1]->mType == QLNC_EXPR_TYPE_OUTER_COLUMN) )
                    {
                        sIsRowidOrFilter = STL_TRUE;
                    }
                    else if( sFunction->mArgs[1]->mType == QLNC_EXPR_TYPE_ROWID_COLUMN )
                    {
                        if( ((qlncExprRowidColumn *)sFunction->mArgs[0])->mNode->mNodeID !=
                            ((qlncExprRowidColumn *)sFunction->mArgs[1])->mNode->mNodeID  )
                        {
                            sIsRowidOrFilter = STL_TRUE;
                        }
                        else
                        {
                            sIsRowidOrFilter = STL_FALSE;
                        }
                    }
                }
                else if( (sFunction->mArgs[1]->mType == QLNC_EXPR_TYPE_ROWID_COLUMN) &&
                         (aBaseTableNode->mNodeCommon.mNodeID == ((qlncExprRowidColumn*)(sFunction->mArgs[1]))->mNode->mNodeID) )
                {
                    if( (sFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                        (sFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_REFERENCE) ||
                        (sFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_OUTER_COLUMN) )
                    {
                        sIsRowidOrFilter = STL_TRUE;
                    }
                    else if( sFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_ROWID_COLUMN )
                    {
                        STL_DASSERT( ((qlncExprRowidColumn *)sFunction->mArgs[0])->mNode->mNodeID !=
                                     ((qlncExprRowidColumn *)sFunction->mArgs[1])->mNode->mNodeID  );
                        sIsRowidOrFilter = STL_TRUE;
                    }
                }
            }
#if 0
            sRefNodeItem = ((qlncBooleanFilter*)(aOrFilter->mFilters[0]))->mRefNodeList.mHead;
            while( sRefNodeItem != NULL )
            {
                sRefColumnItem = sRefNodeItem->mRefColumnList.mHead;
                while( sRefColumnItem != NULL )
                {
                    if( sRefColumnItem->mExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN )
                    {
                        sIsRowidOrFilter = STL_TRUE;
                        break;
                    }

                    sRefColumnItem = sRefColumnItem->mNext;
                }

                if( sIsRowidOrFilter == STL_TRUE )
                {
                    break;
                }

                sRefNodeItem = sRefNodeItem->mNext;
            }
#endif
        }
    }

    return sIsRowidOrFilter;
}


/**
 * @brief 두 Expression이 동일한지 판단한다.
 * @param[in]   aExpr1  First Expression
 * @param[in]   aExpr2  Second Expression
 */
stlBool qlncIsSameExpr( qlncExprCommon  * aExpr1,
                        qlncExprCommon  * aExpr2 )
{
    STL_DASSERT( aExpr1 != NULL );
    STL_DASSERT( aExpr2 != NULL );

    if( aExpr1->mType == aExpr2->mType )
    {
        switch( aExpr1->mType )
        {
            case QLNC_EXPR_TYPE_VALUE:
                {
                    qlncExprValue   * sCandValue1   = (qlncExprValue*)aExpr1;
                    qlncExprValue   * sCandValue2   = (qlncExprValue*)aExpr2;

                    if( sCandValue1->mInternalBindIdx != sCandValue2->mInternalBindIdx )
                    {
                        /* @todo 현재 코드에서는 여기를 들어올 수 없다. */

                        return STL_FALSE;
                    }
                    else
                    {
                        if( sCandValue1->mInternalBindIdx != QLV_INTERNAL_BIND_VARIABLE_IDX_NA )
                        {
                            /* @todo 현재 코드에서는 여기를 들어올 수 없다. */

                            return ( aExpr1->mPosition == aExpr2->mPosition );
                        }
                    }
                    
                    if( sCandValue1->mValueType == sCandValue2->mValueType )
                    {
                        if( (sCandValue1->mValueType == QLNC_VALUE_TYPE_BOOLEAN) ||
                            (sCandValue1->mValueType == QLNC_VALUE_TYPE_BINARY_INTEGER) )
                        {
                            if( sCandValue1->mData.mInteger == sCandValue2->mData.mInteger )
                            {
                                return STL_TRUE;
                            }
                        }
                        else if( sCandValue1->mValueType == QLNC_VALUE_TYPE_NULL )
                        {
                            return STL_TRUE;
                        }
                        else
                        {
                            if( stlStrcmp( sCandValue1->mData.mString,
                                           sCandValue2->mData.mString )
                                == 0 )
                            {
                                return STL_TRUE;
                            }
                        }
                    }

                    return STL_FALSE;
                }
            case QLNC_EXPR_TYPE_BIND_PARAM:
                {
                    qlncExprBindParam   * sCandBindParam1   = (qlncExprBindParam*)aExpr1;
                    qlncExprBindParam   * sCandBindParam2   = (qlncExprBindParam*)aExpr2;

                    if( (sCandBindParam1->mBindType == sCandBindParam2->mBindType) &&
                        (sCandBindParam1->mBindParamIdx == sCandBindParam2->mBindParamIdx) )
                    {
                        return STL_TRUE;
                    }

                    return STL_FALSE;
                }
            case QLNC_EXPR_TYPE_COLUMN:
                {
                    qlncExprColumn  * sCandColumn1  = (qlncExprColumn*)aExpr1;
                    qlncExprColumn  * sCandColumn2  = (qlncExprColumn*)aExpr2;

                    if( (sCandColumn1->mNode->mNodeID == sCandColumn2->mNode->mNodeID) &&
                        (sCandColumn1->mColumnID == sCandColumn2->mColumnID) )
                    {
                        return STL_TRUE;
                    }

                    return STL_FALSE;
                }
            case QLNC_EXPR_TYPE_FUNCTION:
                {
                    qlncExprFunction    * sCandFunc1    = (qlncExprFunction*)aExpr1;
                    qlncExprFunction    * sCandFunc2    = (qlncExprFunction*)aExpr2;
                    stlInt32              i;

                    if( (sCandFunc1->mFuncId == sCandFunc2->mFuncId) &&
                        (sCandFunc1->mArgCount == sCandFunc2->mArgCount) )
                    {
                        for( i = 0; i < sCandFunc1->mArgCount; i++ )
                        {
                            if( qlncIsSameExpr( sCandFunc1->mArgs[i],
                                                sCandFunc2->mArgs[i] )
                                == STL_FALSE )
                            {
                                return STL_FALSE;
                            }
                        }

                        return STL_TRUE;
                    }

                    return STL_FALSE;
                }
            case QLNC_EXPR_TYPE_CAST:
                {
                    qlncExprTypeCast    * sCandCast1    = (qlncExprTypeCast*)aExpr1;
                    qlncExprTypeCast    * sCandCast2    = (qlncExprTypeCast*)aExpr2;

                    if( qlncIsSameExpr( sCandCast1->mArgs[0],
                                        sCandCast2->mArgs[0] )
                        == STL_TRUE )
                    {
                        if( (sCandCast1->mTypeDef.mDBType == sCandCast2->mTypeDef.mDBType) &&
                            (sCandCast1->mTypeDef.mArgNum1 == sCandCast2->mTypeDef.mArgNum1) &&
                            (sCandCast1->mTypeDef.mArgNum2 == sCandCast2->mTypeDef.mArgNum2) &&
                            (sCandCast1->mTypeDef.mStringLengthUnit == sCandCast2->mTypeDef.mStringLengthUnit) &&
                            (sCandCast1->mTypeDef.mIntervalIndicator == sCandCast2->mTypeDef.mIntervalIndicator) )
                        {
                            return STL_TRUE;
                        }
                    }

                    return STL_FALSE;
                }
            case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
                {
                    qlncExprPseudoCol   * sCandPseudoCol1   = (qlncExprPseudoCol*)aExpr1;
                    qlncExprPseudoCol   * sCandPseudoCol2   = (qlncExprPseudoCol*)aExpr2;

                    /* @todo 현재 pseudo column이 오는 경우가 없는 것으로 판단됨 */
                    if( sCandPseudoCol1->mPseudoOffset == sCandPseudoCol2->mPseudoOffset )
                    {
                        return STL_TRUE;
                    }

                    return STL_FALSE;
                }
            case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
                {
                    STL_DASSERT( 0 );
                    break;
                }
            case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
                {
                    return qlncIsSameExpr( ((qlncExprConstExpr*)aExpr1)->mOrgExpr,
                                           ((qlncExprConstExpr*)aExpr2)->mOrgExpr );
                }
            case QLNC_EXPR_TYPE_SUB_QUERY:
                {
                    qlncExprSubQuery    * sCandSubQuery1    = (qlncExprSubQuery*)aExpr1;
                    qlncExprSubQuery    * sCandSubQuery2    = (qlncExprSubQuery*)aExpr2;

                    /* @todo 현재 pseudo column이 오는 경우가 없는 것으로 판단됨 */

                    if( sCandSubQuery1->mNode->mNodeID == sCandSubQuery2->mNode->mNodeID )
                    {
                        return STL_TRUE;
                    }

                    return STL_FALSE;
                }
            case QLNC_EXPR_TYPE_REFERENCE:
                {
                    /* @todo 현재 pseudo column이 오는 경우가 없는 것으로 판단됨 */

                    return qlncIsSameExpr( ((qlncExprReference*)aExpr1)->mOrgExpr,
                                           ((qlncExprReference*)aExpr2)->mOrgExpr );
                }
            case QLNC_EXPR_TYPE_INNER_COLUMN:
                {
                    qlncExprInnerColumn * sCandInnerColumn1 = (qlncExprInnerColumn*)aExpr1;
                    qlncExprInnerColumn * sCandInnerColumn2 = (qlncExprInnerColumn*)aExpr2;

                    if( (sCandInnerColumn1->mNode->mNodeID == sCandInnerColumn2->mNode->mNodeID) &&
                        (sCandInnerColumn1->mIdx == sCandInnerColumn2->mIdx) )
                    {
                        if( (sCandInnerColumn1->mOrgExpr == NULL) &&
                            (sCandInnerColumn2->mOrgExpr == NULL) )
                        {
                            return STL_TRUE;
                        }
                        else
                        {
                            if( (sCandInnerColumn1->mOrgExpr != NULL) &&
                                (sCandInnerColumn2->mOrgExpr != NULL) )
                            {
                                /* @todo 현재 pseudo column이 오는 경우가 없는 것으로 판단됨 */

                                return qlncIsSameExpr( sCandInnerColumn1->mOrgExpr,
                                                       sCandInnerColumn2->mOrgExpr );
                            }
                        }
                    }

                    return STL_FALSE;
                }
            case QLNC_EXPR_TYPE_OUTER_COLUMN:
                {
                    /* @todo 현재 pseudo column이 오는 경우가 없는 것으로 판단됨 */

                    return qlncIsSameExpr( ((qlncExprOuterColumn*)aExpr1)->mOrgExpr,
                                           ((qlncExprOuterColumn*)aExpr2)->mOrgExpr );
                }
            case QLNC_EXPR_TYPE_ROWID_COLUMN:
                {
                    qlncExprRowidColumn * sCandRowidColumn1 = (qlncExprRowidColumn*)aExpr1;
                    qlncExprRowidColumn * sCandRowidColumn2 = (qlncExprRowidColumn*)aExpr2;

                    if( (sCandRowidColumn1->mNode->mNodeID == sCandRowidColumn2->mNode->mNodeID) &&
                        (sCandRowidColumn1->mIdx == sCandRowidColumn2->mIdx) )
                    {
                        return STL_TRUE;
                    }

                    return STL_FALSE;
                }
            case QLNC_EXPR_TYPE_AGGREGATION:
                {
                    qlncExprAggregation * sCandAggr1    = (qlncExprAggregation*)aExpr1;
                    qlncExprAggregation * sCandAggr2    = (qlncExprAggregation*)aExpr2;
                    stlInt32              i;

                    if( (sCandAggr1->mAggrId == sCandAggr2->mAggrId) &&
                        (sCandAggr1->mArgCount == sCandAggr2->mArgCount) &&
                        (sCandAggr1->mIsDistinct == sCandAggr2->mIsDistinct) )
                    {
                        for( i = 0; i < sCandAggr1->mArgCount; i++ )
                        {
                            if( qlncIsSameExpr( sCandAggr1->mArgs[i],
                                                sCandAggr2->mArgs[i] )
                                == STL_FALSE )
                            {
                                return STL_FALSE;
                            }
                        }

                        if( (sCandAggr1->mFilter == NULL) &&
                            (sCandAggr2->mFilter == NULL) )
                        {
                            return STL_TRUE;
                        }
                        else
                        {
                            /* @todo 현재 Aggregation은 Filter를 가지지 않으므로
                             * 여기에 들어오지 않는다. */
                            if( (sCandAggr1->mFilter != NULL) &&
                                (sCandAggr2->mFilter != NULL) )
                            {
                                return qlncIsSameExpr( sCandAggr1->mFilter,
                                                       sCandAggr2->mFilter );
                            }
                        }
                    }

                    return STL_FALSE;
                }
            case QLNC_EXPR_TYPE_CASE_EXPR:
                {
                    qlncExprCaseExpr    * sCandCaseExpr1    = (qlncExprCaseExpr*)aExpr1;
                    qlncExprCaseExpr    * sCandCaseExpr2    = (qlncExprCaseExpr*)aExpr2;
                    stlInt32              i;

                    if( (sCandCaseExpr1->mFuncId == sCandCaseExpr2->mFuncId) &&
                        (sCandCaseExpr1->mCount == sCandCaseExpr2->mCount) )
                    {
                        for( i = 0; i < sCandCaseExpr1->mCount; i++ )
                        {
                            if( qlncIsSameExpr( sCandCaseExpr1->mWhenArr[i],
                                                sCandCaseExpr2->mWhenArr[i] )
                                == STL_FALSE )
                            {
                                return STL_FALSE;
                            }

                            if( qlncIsSameExpr( sCandCaseExpr1->mThenArr[i],
                                                sCandCaseExpr2->mThenArr[i] )
                                == STL_FALSE )
                            {
                                return STL_FALSE;
                            }
                        }

                        return qlncIsSameExpr( sCandCaseExpr1->mDefResult,
                                               sCandCaseExpr2->mDefResult );
                    }

                    return STL_FALSE;
                }
            case QLNC_EXPR_TYPE_LIST_FUNCTION:
                {
                    qlncExprListFunc    * sCandListFunc1    = (qlncExprListFunc*)aExpr1;
                    qlncExprListFunc    * sCandListFunc2    = (qlncExprListFunc*)aExpr2;
                    stlInt32              i;

                    if( (sCandListFunc1->mFuncId == sCandListFunc2->mFuncId) &&
                        (sCandListFunc1->mArgCount == sCandListFunc2->mArgCount) )
                    {
                        for( i = 0; i < sCandListFunc1->mArgCount; i++ )
                        {
                            if( qlncIsSameExpr( sCandListFunc1->mArgs[i],
                                                sCandListFunc2->mArgs[i] )
                                == STL_FALSE )
                            {
                                return STL_FALSE;
                            }
                        }

                        /* @todo 현재 여기로 들어오는 경우가 없는 것으로 판단됨 */

                        return STL_TRUE;
                    }

                    return STL_FALSE;
                }
            case QLNC_EXPR_TYPE_LIST_COLUMN:
                {
                    qlncExprListCol * sCandListCol1 = (qlncExprListCol*)aExpr1;
                    qlncExprListCol * sCandListCol2 = (qlncExprListCol*)aExpr2;
                    stlInt32          i;

                    if( sCandListCol1->mArgCount == sCandListCol2->mArgCount )
                    {
                        for( i = 0; i < sCandListCol1->mArgCount; i++ )
                        {
                            if( qlncIsSameExpr( sCandListCol1->mArgs[i],
                                                sCandListCol2->mArgs[i] )
                                == STL_FALSE )
                            {
                                return STL_FALSE;
                            }
                        }

                        return STL_TRUE;
                    }

                    return STL_FALSE;
                }
            case QLNC_EXPR_TYPE_ROW_EXPR:
                {
                    qlncExprRowExpr * sCandRowExpr1 = (qlncExprRowExpr*)aExpr1;
                    qlncExprRowExpr * sCandRowExpr2 = (qlncExprRowExpr*)aExpr2;
                    stlInt32          i;

                    if( sCandRowExpr1->mArgCount == sCandRowExpr2->mArgCount )
                    {
                        for( i = 0; i < sCandRowExpr1->mArgCount; i++ )
                        {
                            if( qlncIsSameExpr( sCandRowExpr1->mArgs[i],
                                                sCandRowExpr2->mArgs[i] )
                                == STL_FALSE )
                            {
                                return STL_FALSE;
                            }
                        }

                        return STL_TRUE;
                    }

                    /* @todo 현재 여기로 들어오는 경우가 없는 것으로 판단됨 */

                    return STL_FALSE;
                }
            case QLNC_EXPR_TYPE_AND_FILTER:
            case QLNC_EXPR_TYPE_OR_FILTER:
            case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
            case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
            default:
                STL_DASSERT( 0 );
                break;
        }
    }

    return STL_FALSE;
}


/**
 * @brief Expression의 Duplicate을 수행한다.
 * @param[in]   aParamInfo  parameter Info
 * @param[in]   aSrcExpr    Source Expression
 * @param[out]  aDstExpr    Dest Expression
 * @param[in]   aEnv        Environment
 */
stlStatus qlncDuplicateExpr( qlncParamInfo      * aParamInfo,
                             qlncExprCommon     * aSrcExpr,
                             qlncExprCommon    ** aDstExpr,
                             qllEnv             * aEnv )
{
    stlInt32          i;
    qlncExprCommon  * sDstExpr  = NULL;

    qlncBooleanFilter           * sBooleanFilter    = NULL;
    qlncRefNodeItem             * sRefNodeItem      = NULL;
    qlncRefColumnItem           * sRefColumnItem    = NULL;

    qlncConvertExprParamInfo      sConvertExprParamInfo;

    /****************************************************************
     * Paramter Validation
     ****************************************************************/
    
    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


#define _QLNC_ALLOC_AND_COPY_EXPR( _aType )                     \
    {                                                           \
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ), \
                                    STL_SIZEOF( _aType ),       \
                                    (void**) &sDstExpr,         \
                                    KNL_ENV(aEnv) )             \
                 == STL_SUCCESS );                              \
                                                                \
        stlMemcpy( sDstExpr, aSrcExpr, STL_SIZEOF( _aType ) );  \
    }

    /* Source Expression이 NULL인 경우 체크 */
    if( aSrcExpr == NULL )
    {
        sDstExpr = NULL;
        STL_THROW( RAMP_FINISH );
    }

    /* Source Expression의 Type에 따라 처리 */
    switch( aSrcExpr->mType )
    {
        case QLNC_EXPR_TYPE_VALUE:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprValue );

            if( ((qlncExprValue *) aSrcExpr)->mInternalBindIdx !=
                QLV_INTERNAL_BIND_VARIABLE_IDX_NA )
            {
                ((qlncExprValue *) sDstExpr)->mInternalBindIdx
                    = ++(*aParamInfo->mGlobalInternalBindIdx);
            }

            break;

        case QLNC_EXPR_TYPE_BIND_PARAM:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprBindParam );

            break;

        case QLNC_EXPR_TYPE_COLUMN:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprColumn );

            break;

        case QLNC_EXPR_TYPE_FUNCTION:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprFunction );

            if( ((qlncExprFunction*)aSrcExpr)->mArgCount > 0 )
            {
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncExprCommon* ) * ((qlncExprFunction*)aSrcExpr)->mArgCount,
                                            (void**) &(((qlncExprFunction*)sDstExpr)->mArgs),
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                for( i = 0; i < ((qlncExprFunction*)aSrcExpr)->mArgCount; i++ )
                {
                    STL_TRY( qlncDuplicateExpr( aParamInfo,
                                                ((qlncExprFunction*)aSrcExpr)->mArgs[i],
                                                &(((qlncExprFunction*)sDstExpr)->mArgs[i]),
                                                aEnv )
                             == STL_SUCCESS );
                }
            }

            break;

        case QLNC_EXPR_TYPE_CAST:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprTypeCast );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * ((qlncExprTypeCast*)aSrcExpr)->mArgCount,
                                        (void**) &(((qlncExprTypeCast*)sDstExpr)->mArgs),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( i = 0; i < ((qlncExprTypeCast*)aSrcExpr)->mArgCount; i++ )
            {
                STL_TRY( qlncDuplicateExpr( aParamInfo,
                                            ((qlncExprTypeCast*)aSrcExpr)->mArgs[i],
                                            &(((qlncExprTypeCast*)sDstExpr)->mArgs[i]),
                                            aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_EXPR_TYPE_PSEUDO_COLUMN:
            /* @todo 현재 여기로 들어오는 경우가 없는 것으로 판단됨 */

            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprPseudoCol );

            break;

        case QLNC_EXPR_TYPE_PSEUDO_ARGUMENT:
            /* @todo 현재 여기로 들어오는 경우가 없음 */

            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprPseudoArg );

            break;

        case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprConstExpr );

            STL_TRY( qlncDuplicateExpr( aParamInfo,
                                        ((qlncExprConstExpr*)aSrcExpr)->mOrgExpr,
                                        &(((qlncExprConstExpr*)sDstExpr)->mOrgExpr),
                                        aEnv )
                     == STL_SUCCESS );

            break;

        case QLNC_EXPR_TYPE_SUB_QUERY:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprSubQuery );

            break;

        case QLNC_EXPR_TYPE_REFERENCE:
            /* @todo 현재 여기로 들어오는 경우가 없는 것으로 판단됨 */

            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprReference );

            STL_TRY( qlncDuplicateExpr( aParamInfo,
                                        ((qlncExprReference*)aSrcExpr)->mOrgExpr,
                                        &(((qlncExprReference*)sDstExpr)->mOrgExpr),
                                        aEnv )
                     == STL_SUCCESS );

            break;

        case QLNC_EXPR_TYPE_INNER_COLUMN:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprInnerColumn );

            if( ((qlncExprInnerColumn*)aSrcExpr)->mOrgExpr == NULL )
            {
                ((qlncExprInnerColumn*)sDstExpr)->mOrgExpr = NULL;
            }
            else
            {
                STL_TRY( qlncDuplicateExpr( aParamInfo,
                                            ((qlncExprInnerColumn*)aSrcExpr)->mOrgExpr,
                                            &(((qlncExprInnerColumn*)sDstExpr)->mOrgExpr),
                                            aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_EXPR_TYPE_OUTER_COLUMN:
            /* @todo 현재 여기로 들어오는 경우가 없는 것으로 판단됨 */

            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprOuterColumn );

            STL_TRY( qlncDuplicateExpr( aParamInfo,
                                        ((qlncExprOuterColumn*)aSrcExpr)->mOrgExpr,
                                        &(((qlncExprOuterColumn*)sDstExpr)->mOrgExpr),
                                        aEnv )
                     == STL_SUCCESS );

            break;

        case QLNC_EXPR_TYPE_ROWID_COLUMN:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprRowidColumn );

            break;

        case QLNC_EXPR_TYPE_AGGREGATION:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprAggregation );

            if( ((qlncExprAggregation*)aSrcExpr)->mArgCount > 0 )
            {
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncExprCommon* ) * ((qlncExprAggregation*)aSrcExpr)->mArgCount,
                                            (void**) &(((qlncExprAggregation*)sDstExpr)->mArgs),
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                for( i = 0; i < ((qlncExprAggregation*)aSrcExpr)->mArgCount; i++ )
                {
                    STL_TRY( qlncDuplicateExpr( aParamInfo,
                                                ((qlncExprAggregation*)aSrcExpr)->mArgs[i],
                                                &(((qlncExprAggregation*)sDstExpr)->mArgs[i]),
                                                aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                ((qlncExprAggregation*)sDstExpr)->mArgs = NULL;
            }

            STL_TRY( qlncDuplicateExpr( aParamInfo,
                                        ((qlncExprAggregation*)aSrcExpr)->mFilter,
                                        &(((qlncExprAggregation*)sDstExpr)->mFilter),
                                        aEnv )
                     == STL_SUCCESS );

            break;

        case QLNC_EXPR_TYPE_CASE_EXPR:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprCaseExpr );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * ((qlncExprCaseExpr*)aSrcExpr)->mCount,
                                        (void**) &(((qlncExprCaseExpr*)sDstExpr)->mWhenArr),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * ((qlncExprCaseExpr*)aSrcExpr)->mCount,
                                        (void**) &(((qlncExprCaseExpr*)sDstExpr)->mThenArr),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( i = 0; i < ((qlncExprCaseExpr*)aSrcExpr)->mCount; i++ )
            {
                STL_TRY( qlncDuplicateExpr( aParamInfo,
                                            ((qlncExprCaseExpr*)aSrcExpr)->mWhenArr[i],
                                            &(((qlncExprCaseExpr*)sDstExpr)->mWhenArr[i]),
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlncDuplicateExpr( aParamInfo,
                                            ((qlncExprCaseExpr*)aSrcExpr)->mThenArr[i],
                                            &(((qlncExprCaseExpr*)sDstExpr)->mThenArr[i]),
                                            aEnv )
                         == STL_SUCCESS );
            }

            STL_TRY( qlncDuplicateExpr( aParamInfo,
                                        ((qlncExprCaseExpr*)aSrcExpr)->mDefResult,
                                        &(((qlncExprCaseExpr*)sDstExpr)->mDefResult),
                                        aEnv )
                     == STL_SUCCESS );

            break;

        case QLNC_EXPR_TYPE_LIST_FUNCTION:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprListFunc );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * ((qlncExprListFunc*)aSrcExpr)->mArgCount,
                                        (void**) &(((qlncExprListFunc*)sDstExpr)->mArgs),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( i = 0; i < ((qlncExprListFunc*)aSrcExpr)->mArgCount; i++ )
            {
                STL_TRY( qlncDuplicateExpr( aParamInfo,
                                            ((qlncExprListFunc*)aSrcExpr)->mArgs[i],
                                            &(((qlncExprListFunc*)sDstExpr)->mArgs[i]),
                                            aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_EXPR_TYPE_LIST_COLUMN:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprListCol );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * ((qlncExprListCol*)aSrcExpr)->mArgCount,
                                        (void**) &(((qlncExprListCol*)sDstExpr)->mArgs),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( i = 0; i < ((qlncExprListCol*)aSrcExpr)->mArgCount; i++ )
            {
                STL_TRY( qlncDuplicateExpr( aParamInfo,
                                            ((qlncExprListCol*)aSrcExpr)->mArgs[i],
                                            &(((qlncExprListCol*)sDstExpr)->mArgs[i]),
                                            aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_EXPR_TYPE_ROW_EXPR:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncExprRowExpr );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * ((qlncExprRowExpr*)aSrcExpr)->mArgCount,
                                        (void**) &(((qlncExprRowExpr*)sDstExpr)->mArgs),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( i = 0; i < ((qlncExprRowExpr*)aSrcExpr)->mArgCount; i++ )
            {
                STL_TRY( qlncDuplicateExpr( aParamInfo,
                                            ((qlncExprRowExpr*)aSrcExpr)->mArgs[i],
                                            &(((qlncExprRowExpr*)sDstExpr)->mArgs[i]),
                                            aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_EXPR_TYPE_AND_FILTER:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncAndFilter );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncOrFilter* ) * ((qlncAndFilter*)aSrcExpr)->mOrCount,
                                        (void**) &(((qlncAndFilter*)sDstExpr)->mOrFilters),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( i = 0; i < ((qlncAndFilter*)aSrcExpr)->mOrCount; i++ )
            {
                STL_TRY( qlncDuplicateExpr( aParamInfo,
                                            (qlncExprCommon*)((qlncAndFilter*)aSrcExpr)->mOrFilters[i],
                                            (qlncExprCommon**)&(((qlncAndFilter*)sDstExpr)->mOrFilters[i]),
                                            aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_EXPR_TYPE_OR_FILTER:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncOrFilter );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * ((qlncOrFilter*)aSrcExpr)->mFilterCount,
                                        (void**) &(((qlncOrFilter*)sDstExpr)->mFilters),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            for( i = 0; i < ((qlncOrFilter*)aSrcExpr)->mFilterCount; i++ )
            {
                STL_TRY( qlncDuplicateExpr( aParamInfo,
                                            ((qlncOrFilter*)aSrcExpr)->mFilters[i],
                                            &(((qlncOrFilter*)sDstExpr)->mFilters[i]),
                                            aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_EXPR_TYPE_BOOLEAN_FILTER:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncBooleanFilter );

            STL_TRY( qlncDuplicateExpr( aParamInfo,
                                        ((qlncBooleanFilter*)aSrcExpr)->mExpr,
                                        &(((qlncBooleanFilter*)sDstExpr)->mExpr),
                                        aEnv )
                     == STL_SUCCESS );

            STL_DASSERT( sDstExpr->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER );

            sBooleanFilter = (qlncBooleanFilter*)sDstExpr;

            /* Node List 초기화 */
            QLNC_INIT_LIST( &(sBooleanFilter->mRefNodeList) );
            sBooleanFilter->mLeftRefNodeList = NULL;
            sBooleanFilter->mRightRefNodeList = NULL;

            /* Convert Expr Param Info 초기화 */
            sConvertExprParamInfo.mParamInfo = *aParamInfo;
            sConvertExprParamInfo.mTableMapArr = NULL;
            sConvertExprParamInfo.mOuterTableMapArr = NULL;
            sConvertExprParamInfo.mInitExpr = NULL;
            sConvertExprParamInfo.mCandExpr = NULL;
            sConvertExprParamInfo.mExprPhraseFlag = 0;
            sConvertExprParamInfo.mSubQueryExprList = NULL;
            sConvertExprParamInfo.mExprSubQuery = NULL;
            sConvertExprParamInfo.mRefNodeList = NULL;

            if( ((qlncBooleanFilter*)aSrcExpr)->mLeftRefNodeList != NULL )
            {
                STL_DASSERT( (sBooleanFilter->mExpr->mType == QLNC_EXPR_TYPE_FUNCTION) &&
                             (((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgCount == 2) );

                /* Left Expression에 있는 Reference Node List 설정 */
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncRefNodeList ),
                                            (void**) &(sBooleanFilter->mLeftRefNodeList),
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                QLNC_INIT_LIST( sBooleanFilter->mLeftRefNodeList );

                sConvertExprParamInfo.mRefNodeList = sBooleanFilter->mLeftRefNodeList;
                STL_TRY( qlncFindAddRefColumnToRefNodeList(
                             &sConvertExprParamInfo,
                             ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[0],
                             aEnv )
                         == STL_SUCCESS );

                /* Append Left Reference Node List */
                sConvertExprParamInfo.mRefNodeList = &(sBooleanFilter->mRefNodeList);
                sRefNodeItem = sBooleanFilter->mLeftRefNodeList->mHead;
                while( sRefNodeItem != NULL )
                {
                    sRefColumnItem = sRefNodeItem->mRefColumnList.mHead;
                    while( sRefColumnItem != NULL )
                    {
                        STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                                sRefColumnItem->mExpr,
                                                                aEnv )
                                 == STL_SUCCESS );

                        sRefColumnItem = sRefColumnItem->mNext;
                    }

                    sRefNodeItem = sRefNodeItem->mNext;
                }
            }

            if( ((qlncBooleanFilter*)aSrcExpr)->mRightRefNodeList != NULL )
            {
                STL_DASSERT( (sBooleanFilter->mExpr->mType == QLNC_EXPR_TYPE_FUNCTION) &&
                             (((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgCount == 2) );

                /* Right Expression에 있는 Reference Node List 설정 */
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncRefNodeList ),
                                            (void**) &(sBooleanFilter->mRightRefNodeList),
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                QLNC_INIT_LIST( sBooleanFilter->mRightRefNodeList );

                sConvertExprParamInfo.mRefNodeList = sBooleanFilter->mRightRefNodeList;
                STL_TRY( qlncFindAddRefColumnToRefNodeList(
                             &sConvertExprParamInfo,
                             ((qlncExprFunction*)(sBooleanFilter->mExpr))->mArgs[1],
                             aEnv )
                         == STL_SUCCESS );

                /* Append Right Reference Node List */
                sConvertExprParamInfo.mRefNodeList = &(sBooleanFilter->mRefNodeList);
                sRefNodeItem = sBooleanFilter->mRightRefNodeList->mHead;
                while( sRefNodeItem != NULL )
                {
                    sRefColumnItem = sRefNodeItem->mRefColumnList.mHead;
                    while( sRefColumnItem != NULL )
                    {
                        STL_TRY( qlncAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                                sRefColumnItem->mExpr,
                                                                aEnv )
                                 == STL_SUCCESS );

                        sRefColumnItem = sRefColumnItem->mNext;
                    }

                    sRefNodeItem = sRefNodeItem->mNext;
                }
            }

            if( (sBooleanFilter->mLeftRefNodeList == NULL) &&
                (sBooleanFilter->mRightRefNodeList == NULL) )
            {
                sConvertExprParamInfo.mRefNodeList = &(sBooleanFilter->mRefNodeList);
                STL_TRY( qlncFindAddRefColumnToRefNodeList(
                             &sConvertExprParamInfo,
                             sBooleanFilter->mExpr,
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER:
            _QLNC_ALLOC_AND_COPY_EXPR( qlncConstBooleanFilter );

            STL_TRY( qlncDuplicateExpr( aParamInfo,
                                        ((qlncConstBooleanFilter*)aSrcExpr)->mExpr,
                                        &(((qlncConstBooleanFilter*)sDstExpr)->mExpr),
                                        aEnv )
                     == STL_SUCCESS );

            break;

        default:
            STL_DASSERT( 0 );
            break;
    }


    STL_RAMP( RAMP_FINISH );


    /**
     * Output 설정
     */

    *aDstExpr = sDstExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief And Filter에 SubQuery Filter가 존재하는지 판단한다.
 */
stlBool qlncIsExistSubQueryFilter( qlncAndFilter    * aFilter )
{
    stlInt32      i;
    stlInt32      j;


    STL_DASSERT( aFilter != NULL );
    for( i = 0; i < aFilter->mOrCount; i++ )
    {
        if( (aFilter->mOrFilters[i]->mSubQueryExprList != NULL) &&
            (aFilter->mOrFilters[i]->mSubQueryExprList->mCount > 0) )
        {
            return STL_TRUE;
        }

        for( j = 0; j < aFilter->mOrFilters[i]->mFilterCount; j++ )
        {
            if( aFilter->mOrFilters[i]->mFilters[j]->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER )
            {
                qlncBooleanFilter   * sBooleanFilter    = NULL;

                sBooleanFilter = (qlncBooleanFilter*)(aFilter->mOrFilters[i]->mFilters[j]);

                if( sBooleanFilter->mFuncID == KNL_BUILTIN_FUNC_AND )
                {
                    if( qlncIsExistSubQueryFilter( (qlncAndFilter*)(sBooleanFilter->mExpr) )
                        == STL_TRUE )
                    {
                        /* @todo 현재 여기로 들어오는 경우가 없는 것으로 판단됨 */

                        return STL_TRUE;
                    }
                }
            }
        }
    }


    return STL_FALSE;
}


/**
 * @brief Physical Filter 여부를 확인한다.
 * @param[in]       aSQLString          SQL String
 * @param[in]       aTableNode          Physical Filter 평가를 위한 Table Node
 * @param[in,out]   aExpr               Expression
 * @param[out]      aIsPhysicalFilter   Physical Filter 여부
 * @param[in]       aEnv                Environment
 */
stlStatus qlncIsPhysicalFilter( stlChar         * aSQLString,
                                qlncNodeCommon  * aTableNode,
                                qlncExprCommon  * aExpr,
                                stlBool         * aIsPhysicalFilter,
                                qllEnv          * aEnv )
{
    stlBool                 sIsPhysicalFilter;
    stlBool                 sIsExistCol;
    qlncExprFunction      * sCandFunction     = NULL;
    const dtlCompareType  * sCompareType      = NULL;    

    STL_DASSERT( aExpr != NULL );

    sIsExistCol = STL_FALSE;
    
    if( aExpr->mType == QLNC_EXPR_TYPE_FUNCTION )
    {
        sCandFunction = (qlncExprFunction*)aExpr;
        switch( sCandFunction->mFuncId )
        {
            case KNL_BUILTIN_FUNC_IS_EQUAL:
            case KNL_BUILTIN_FUNC_IS_NOT_EQUAL:
            case KNL_BUILTIN_FUNC_IS_LESS_THAN:
            case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL:
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN:
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL:
                /* Arg 0 체크 */
                if( (sCandFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_COLUMN) ||
                    (sCandFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_ROWID_COLUMN) ||
                    (sCandFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_INNER_COLUMN ) )
                {
                    sIsExistCol = qlncIsExistColumnInExpr( sCandFunction->mArgs[0],
                                                           aTableNode );
                }
                else
                {
                    if( qlncIsExistColumnInExpr( sCandFunction->mArgs[1],
                                                 aTableNode )
                        == STL_FALSE )
                    {
                        sIsPhysicalFilter = STL_FALSE;
                        STL_THROW( RAMP_FINISH );
                    }
                }

                /* Arg 1 체크 */
                if( (sCandFunction->mArgs[1]->mType == QLNC_EXPR_TYPE_COLUMN) ||
                    (sCandFunction->mArgs[1]->mType == QLNC_EXPR_TYPE_ROWID_COLUMN) ||
                    (sCandFunction->mArgs[1]->mType == QLNC_EXPR_TYPE_INNER_COLUMN ) )
                {
                    if( sIsExistCol == STL_FALSE )
                    {
                        if( qlncIsExistColumnInExpr( sCandFunction->mArgs[1],
                                                     aTableNode )
                            == STL_TRUE )
                        {
                            STL_TRY( qlnoGetCompareType( aSQLString,
                                                         sCandFunction->mArgs[0]->mDataType,
                                                         sCandFunction->mArgs[1]->mDataType,
                                                         STL_TRUE,
                                                         STL_FALSE,
                                                         aExpr->mPosition,
                                                         &sCompareType,
                                                         aEnv )
                                     == STL_SUCCESS );

                            sIsPhysicalFilter = ( sCompareType->mRightType == sCandFunction->mArgs[1]->mDataType );
                        }
                        else
                        {
                            sIsPhysicalFilter = STL_FALSE;
                        }

                        STL_THROW( RAMP_FINISH );
                    }
                    else
                    {
                        if( qlncIsExistColumnInExpr( sCandFunction->mArgs[1],
                                                     aTableNode )
                            == STL_TRUE )
                        {
                            STL_TRY( qlnoGetCompareType( aSQLString,
                                                         sCandFunction->mArgs[0]->mDataType,
                                                         sCandFunction->mArgs[1]->mDataType,
                                                         STL_FALSE,
                                                         STL_FALSE,
                                                         aExpr->mPosition,
                                                         &sCompareType,
                                                         aEnv )
                                     == STL_SUCCESS );
                            
                            sIsPhysicalFilter = ( ( sCompareType->mLeftType == sCandFunction->mArgs[0]->mDataType ) &&
                                                  ( sCompareType->mRightType == sCandFunction->mArgs[1]->mDataType ) );
                        }
                        else
                        {
                            STL_TRY( qlnoGetCompareType( aSQLString,
                                                         sCandFunction->mArgs[0]->mDataType,
                                                         sCandFunction->mArgs[1]->mDataType,
                                                         STL_FALSE,
                                                         STL_TRUE,
                                                         aExpr->mPosition,
                                                         &sCompareType,
                                                         aEnv )
                                     == STL_SUCCESS );

                            sIsPhysicalFilter = ( sCompareType->mLeftType == sCandFunction->mArgs[0]->mDataType );
                        }

                        STL_THROW( RAMP_FINISH );
                    }
                }
                else
                {
                    if( qlncIsExistColumnInExpr( sCandFunction->mArgs[1],
                                                 aTableNode )
                        == STL_TRUE )
                    {
                        sIsPhysicalFilter = STL_FALSE;
                    }
                    else
                    {
                        if( sIsExistCol == STL_TRUE )
                        {
                            STL_TRY( qlnoGetCompareType( aSQLString,
                                                         sCandFunction->mArgs[0]->mDataType,
                                                         sCandFunction->mArgs[1]->mDataType,
                                                         STL_FALSE,
                                                         STL_TRUE,
                                                         aExpr->mPosition,
                                                         &sCompareType,
                                                         aEnv )
                                     == STL_SUCCESS );

                            sIsPhysicalFilter = ( sCompareType->mLeftType == sCandFunction->mArgs[0]->mDataType );
                        }
                        else
                        {
                            sIsPhysicalFilter = STL_FALSE;
                        }
                    }

                    STL_THROW( RAMP_FINISH );
                }

                break;

            case KNL_BUILTIN_FUNC_IS_NULL:
            case KNL_BUILTIN_FUNC_IS_NOT_NULL:
                /* Arg 0 체크 */
                if( (sCandFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_COLUMN) ||
                    (sCandFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_ROWID_COLUMN) ||
                    (sCandFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_INNER_COLUMN ) )
                {
                    sIsPhysicalFilter = qlncIsExistColumnInExpr( sCandFunction->mArgs[0],
                                                                 aTableNode );
                }
                else
                {
                    sIsPhysicalFilter = STL_FALSE;
                }
                STL_THROW( RAMP_FINISH );
                break;
                
            case KNL_BUILTIN_FUNC_NOT:
            case KNL_BUILTIN_FUNC_IS:
            case KNL_BUILTIN_FUNC_IS_NOT:
            case KNL_BUILTIN_FUNC_IS_UNKNOWN:
            case KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN:
                /* Arg 0 체크 */
                if( (sCandFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_COLUMN) ||
                    (sCandFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_ROWID_COLUMN) ||
                    (sCandFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_INNER_COLUMN ) )
                {
                    if( qlncIsExistColumnInExpr( sCandFunction->mArgs[0],
                                                 aTableNode )
                        == STL_TRUE )
                    {
                        if( sCandFunction->mArgs[0]->mDataType != DTL_TYPE_BOOLEAN )
                        {
                            sIsPhysicalFilter = STL_FALSE;
                            STL_THROW( RAMP_FINISH );
                        }
                    }
                    else
                    {
                        sIsPhysicalFilter = STL_FALSE;
                        STL_THROW( RAMP_FINISH );
                    }
                }
                else
                {
                    sIsPhysicalFilter = STL_FALSE;
                    STL_THROW( RAMP_FINISH );
                }
                break;

            case KNL_BUILTIN_FUNC_LIKE:
            case KNL_BUILTIN_FUNC_NOT_LIKE:
                /* Arg 0 체크 */
                if( (sCandFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_COLUMN) ||
                    (sCandFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_ROWID_COLUMN) ||
                    (sCandFunction->mArgs[0]->mType == QLNC_EXPR_TYPE_INNER_COLUMN ) )
                {
                    sIsExistCol = qlncIsExistColumnInExpr( sCandFunction->mArgs[0],
                                                           aTableNode );
                    if( sIsExistCol == STL_FALSE )
                    {
                        sIsPhysicalFilter = STL_FALSE;
                        STL_THROW( RAMP_FINISH );
                    }
                }
                else
                {
                    sIsPhysicalFilter = STL_FALSE;
                    STL_THROW( RAMP_FINISH );
                }

                /* Arg 1 체크 : Like Pattern */
                if( qlncIsExistColumnInExpr( sCandFunction->mArgs[1],
                                             aTableNode )
                    == STL_TRUE )
                {
                    sIsPhysicalFilter = STL_FALSE;
                }
                else
                {
                    STL_TRY( qlnoGetCompareType( aSQLString,
                                                 sCandFunction->mArgs[0]->mDataType,
                                                 DTL_TYPE_CHAR,
                                                 STL_FALSE,
                                                 STL_TRUE,
                                                 aExpr->mPosition,
                                                 &sCompareType,
                                                 aEnv )
                             == STL_SUCCESS );

                    sIsPhysicalFilter = ( sCompareType->mLeftType == sCandFunction->mArgs[0]->mDataType );
                }

                STL_THROW( RAMP_FINISH );

                break;
                
            default:
                /* Logical Filter */
                sIsPhysicalFilter =  STL_FALSE;

                break;
        }
    }
    else if( (aExpr->mType == QLNC_EXPR_TYPE_COLUMN) ||
             (aExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN) )
    {
        if( qlncIsExistColumnInExpr( aExpr,
                                     aTableNode )
            == STL_TRUE )
        {
            if( aExpr->mDataType != DTL_TYPE_BOOLEAN )
            {
                sIsPhysicalFilter = STL_FALSE;
                STL_THROW( RAMP_FINISH );
            }
        }
        else
        {
            sIsPhysicalFilter = STL_FALSE;
            STL_THROW( RAMP_FINISH );
        }
    }
    else
    {
        sIsPhysicalFilter = STL_FALSE;
        STL_THROW( RAMP_FINISH );
    }

    sIsPhysicalFilter = STL_TRUE;


    STL_RAMP( RAMP_FINISH );

    *aIsPhysicalFilter = sIsPhysicalFilter;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Filter로 부터 IN Key Range 후보 Expression List를 구성한다.
 * @param[in]   aCBOptParamInfo          Cost Based Optimizer Parameter Info
 * @param[in]   aIndexScanInfo           Index Scan Info
 * @param[in]   aAndFilter               Source AND-Filter Expression
 * @param[out]  aInKeyRangeExprList      Candidate IN Key Range Expression List
 * @param[out]  aValueExprList           Value Expression List (Right Operand Expression)
 * @param[in]   aEnv                     Environment
 */
stlStatus qlncGetInKeyRangeExprList( qlncCBOptParamInfo  * aCBOptParamInfo,
                                     qlncIndexScanInfo   * aIndexScanInfo,
                                     qlncAndFilter       * aAndFilter,
                                     qlncRefExprList     * aInKeyRangeExprList,
                                     qlncRefExprList     * aValueExprList,
                                     qllEnv              * aEnv )
{
    qlncBooleanFilter   * sBooleanFilter;
    qlncExprListFunc    * sListFuncExpr;
    qlncExprListCol     * sListColExpr;
    qlncExprRowExpr     * sLeftRowExpr;
    qlncExprRowExpr     * sRightRowExpr;
    qlncExprColumn      * sColumnExpr;
    qlncExprValue       * sCandValue;
    qlncRefExprItem     * sRefInitExprItem;
    qlncRefExprItem     * sSubQueryExprItem;
    qlncIndexStat       * sIndexStat;

    dtlDataType           sLeftDataType;
    dtlDataType           sRightDataType;
    dtlDataType           sResultDataType = DTL_TYPE_NA;

    const dtlCompareType    * sCompType;
    
    stlInt32              i;
    stlInt32              sColIdx;
    stlInt32              sLoop;
    stlBool               sIsRange;
    stlBool               sIsExistVarying;
    stlBool               sIsExistFixed[ QLL_MAX_ROW_EXPR_ARGS ];

    
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexScanInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilter != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInKeyRangeExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueExprList != NULL, QLL_ERROR_STACK(aEnv) );

    
    /* Expression List 초기화 */
    QLNC_INIT_LIST( aInKeyRangeExprList );
    QLNC_INIT_LIST( aValueExprList );

    sIndexStat = &(((qlncBaseTableNode*)(aCBOptParamInfo->mLeftNode))->mIndexStatArr[ aIndexScanInfo->mOffset ]);

    for( i = 0; i < aAndFilter->mOrCount; i++ )
    {
        if( aAndFilter->mOrFilters[ i ]->mFilterCount == 1 )
        {
            if( aAndFilter->mOrFilters[ i ]->mFilters[ 0 ]->mType !=
                QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER )
            {
                STL_DASSERT( aAndFilter->mOrFilters[ i ]->mFilters[ 0 ]->mType ==
                             QLNC_EXPR_TYPE_BOOLEAN_FILTER );
                    
                sBooleanFilter = (qlncBooleanFilter*)(aAndFilter->mOrFilters[ i ]->mFilters[ 0 ]);

                if( sBooleanFilter->mCommon.mIterationTime != DTL_ITERATION_TIME_FOR_EACH_EXPR )
                {
                    continue;
                }
                    
                if( sBooleanFilter->mSubQueryExprList != NULL )
                {
                    /* N형 Scalar/Row Sub Query인 경우 In Key Range 구성 가능 */
                    sSubQueryExprItem = sBooleanFilter->mSubQueryExprList->mHead;

                    while( sSubQueryExprItem != NULL )
                    {
                        if( ( ((qlncExprSubQuery *) sSubQueryExprItem->mExpr)->mRefExprList != NULL ) ||
                            ( ((qlncExprSubQuery *) sSubQueryExprItem->mExpr)->mIsRelSubQuery == STL_TRUE ) )
                        {
                            break;
                        }
                        sSubQueryExprItem = sSubQueryExprItem->mNext;
                    }

                    if( sSubQueryExprItem != NULL )
                    {
                        continue;
                    }
                }

                if( ( sBooleanFilter->mFuncID == KNL_BUILTIN_FUNC_IN ) ||
                    ( sBooleanFilter->mFuncID == KNL_BUILTIN_FUNC_EQUAL_ANY ) )
                {
                    /**
                     * Iteration Time이 Each Expr 또는 Aggregation 이면 안된다.
                     */

                    STL_DASSERT( sBooleanFilter->mExpr->mType == QLNC_EXPR_TYPE_LIST_FUNCTION );

                    sListFuncExpr = (qlncExprListFunc *) sBooleanFilter->mExpr;
                    STL_DASSERT( sListFuncExpr->mArgCount == 2 );
                        
                    sListColExpr = (qlncExprListCol *) sListFuncExpr->mArgs[ 1 ];
                    sLeftRowExpr = (qlncExprRowExpr *) sListColExpr->mArgs[ 0 ];

                    /* Key Column이 존재하여야 한다. */
                    if( sListColExpr->mCommon.mIterationTime != DTL_ITERATION_TIME_FOR_EACH_EXPR )
                    {
                        continue;
                    }

                    /* Left List Column은 Inner Column 또는 Base Column으로만 이루어져 있어야 한다. */
                    sIsRange = STL_FALSE;
                    for( sColIdx = 0 ; sColIdx < sLeftRowExpr->mArgCount ; sColIdx++ )
                    {
                        if( ( sLeftRowExpr->mArgs[ sColIdx ]->mType == QLNC_EXPR_TYPE_COLUMN ) ||
                            ( sLeftRowExpr->mArgs[ sColIdx ]->mType == QLNC_EXPR_TYPE_INNER_COLUMN ) )
                        {
                            sIsRange = STL_TRUE;
                            break;
                        }
                        else
                        {
                            /* Do Nothing */
                        }
                    }

                    if( sIsRange == STL_FALSE )
                    {
                        continue;
                    }

                    sListColExpr = (qlncExprListCol *) sListFuncExpr->mArgs[ 0 ];

                    /* 상수만 올 수 있다. */
                    if( sListColExpr->mCommon.mIterationTime == DTL_ITERATION_TIME_FOR_EACH_EXPR )
                    {
                        /* Check Iteration Time */
                        if( qlncIsExistColumnInExpr( (qlncExprCommon *) sListColExpr,
                                                     aCBOptParamInfo->mLeftNode )
                            == STL_TRUE )
                        {                 
                            continue;
                        }
                    }

                    for( sColIdx = 0 ; sColIdx < sLeftRowExpr->mArgCount ; sColIdx++ )
                    {
                        sLeftDataType = sLeftRowExpr->mArgs[ sColIdx ]->mDataType;

                        sRightRowExpr   = (qlncExprRowExpr *) sListColExpr->mArgs[ 0 ];
                        sResultDataType = sRightRowExpr->mArgs[ sColIdx ]->mDataType;

                        if( dtlDataTypeGroup[ sResultDataType ] == DTL_GROUP_BINARY_INTEGER )
                        {
                            sResultDataType = DTL_TYPE_NUMBER;
                        }

                        if( ( sResultDataType == DTL_TYPE_CHAR ) ||
                            ( sResultDataType == DTL_TYPE_BINARY ) )
                        {
                            sIsExistFixed[ sColIdx ] = STL_TRUE;
                        }
                        else
                        {
                            sIsExistFixed[ sColIdx ] = STL_FALSE;
                        }

                        if( ( sResultDataType == DTL_TYPE_VARCHAR ) ||
                            ( sResultDataType == DTL_TYPE_VARBINARY ) )
                        {
                            sIsExistVarying = STL_TRUE;
                        }
                        else
                        {
                            sIsExistVarying = STL_FALSE;
                        }
                        
                        for( sLoop = 1 ; sLoop < sListColExpr->mArgCount ; sLoop++ )
                        {
                            sRightRowExpr = (qlncExprRowExpr *) sListColExpr->mArgs[ sLoop ];
                            sRightDataType = sRightRowExpr->mArgs[ sColIdx ]->mDataType;

                            /**
                             * 각 컬럼은 하나의 data type으로 변환 가능해야 한다.
                             */

                            if( dtlDataTypeGroup[ sRightDataType ] == DTL_GROUP_BINARY_INTEGER )
                            {
                                sRightDataType = DTL_TYPE_NUMBER;
                            }

                            if( sLeftDataType == DTL_TYPE_CHAR )
                            {
                                if( sRightDataType == DTL_TYPE_CHAR )
                                {
                                    sIsExistFixed[ sColIdx ] = STL_TRUE;
                                }
                                else if( sRightDataType == DTL_TYPE_VARCHAR )
                                {
                                    sIsExistVarying = STL_TRUE;
                                }
                                
                                if( ( sIsExistFixed[ sColIdx ] == STL_TRUE ) &&
                                    ( sIsExistVarying == STL_TRUE ) )
                                {
                                    sIsRange = STL_FALSE;
                                    break;
                                }
                            }
                            else if( sLeftDataType == DTL_TYPE_BINARY )
                            {
                                if( sRightDataType == DTL_TYPE_BINARY )
                                {
                                    sIsExistFixed[ sColIdx ] = STL_TRUE;
                                }
                                else if( sRightDataType == DTL_TYPE_VARBINARY )
                                {
                                    sIsExistVarying = STL_TRUE;
                                }
                                
                                if( ( sIsExistFixed[ sColIdx ] == STL_TRUE ) &&
                                    ( sIsExistVarying == STL_TRUE ) )
                                {
                                    sIsRange = STL_FALSE;
                                    break;
                                }
                            }
                            else
                            {
                                /* Do Nothing */
                            }
                            
                            STL_TRY( qlnoGetCompareType( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                         sRightDataType,
                                                         sResultDataType,
                                                         STL_TRUE,
                                                         STL_TRUE,
                                                         sBooleanFilter->mExpr->mPosition,
                                                         &sCompType,
                                                         aEnv )
                                     == STL_SUCCESS );
                            
                            STL_DASSERT( ( sCompType->mLeftType != DTL_TYPE_NA ) &&
                                         ( sCompType->mRightType != DTL_TYPE_NA ) );

                            sResultDataType = dtlGroupRepresentType[ dtlDataTypeGroup[ sCompType->mRightType ] ];
                        }

                        if( sIsRange == STL_FALSE )
                        {
                            break;
                        }

                        if( dtlCheckKeyCompareType( sResultDataType ) == STL_FALSE )
                        {
                            /* @todo 현재 이 코드로 들어오는 경우는 없다고 판단됨 */
                            sIsRange = STL_FALSE;
                            break;
                        }

                        STL_TRY( qlnoGetCompareType( aCBOptParamInfo->mParamInfo.mSQLStmt->mRetrySQL,
                                                     sLeftDataType,
                                                     sResultDataType,
                                                     STL_FALSE,
                                                     STL_FALSE,
                                                     sBooleanFilter->mExpr->mPosition,
                                                     &sCompType,
                                                     aEnv )
                                 == STL_SUCCESS );
                            
                        if( ( sCompType->mLeftType == sLeftDataType ) &&
                            ( sCompType->mRightType == sResultDataType ) )
                        {
                            /* Do Nothing */
                        }
                        else
                        {
                            sIsRange = STL_FALSE;
                            break;
                        }
                    }

                    if( sIsRange == STL_FALSE )
                    {
                        continue;
                    }


                    /**
                     * Key Range 대상 List Function인지 판단
                     */

                    sIsRange = STL_FALSE;

                    for( sColIdx = 0 ; sColIdx < sLeftRowExpr->mArgCount ; sColIdx++ )
                    {
                        if( sLeftRowExpr->mArgs[ sColIdx ]->mType == QLNC_EXPR_TYPE_COLUMN )
                        {
                            sColumnExpr = (qlncExprColumn *) sLeftRowExpr->mArgs[ sColIdx ];
  
                            for( sLoop = 0; sLoop < sIndexStat->mIndexKeyCount; sLoop++ )
                            {
                                if( sColumnExpr->mColumnID ==
                                    ellGetColumnID( &(sIndexStat->mIndexKeyDesc[ sLoop ].mKeyColumnHandle) ) )
                                {
                                    sIsRange = STL_TRUE;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            /* Do Nothing */
                        }
                    }

                    if( sIsRange == STL_TRUE )
                    {
                        for( sColIdx = 0 ; sColIdx < sLeftRowExpr->mArgCount ; sColIdx++ )
                        {
                            /* List Function를 IN Key Range List 에 추가 */
                            STL_TRY( knlAllocRegionMem(
                                         &QLL_CANDIDATE_MEM( aEnv ),
                                         STL_SIZEOF( qlncRefExprItem ),
                                         (void**) &sRefInitExprItem,
                                         KNL_ENV(aEnv) )
                                     == STL_SUCCESS );

                            sRefInitExprItem->mExpr = (qlncExprCommon*)aAndFilter->mOrFilters[ i ];
                            sRefInitExprItem->mNext = NULL;

                            QLNC_APPEND_ITEM_TO_LIST( aInKeyRangeExprList, sRefInitExprItem );
                            
                            /* List Function를 Equal Function으로 변경하기 위한 Right Value Exression 생성 */
                            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                                        STL_SIZEOF( qlncExprValue ),
                                                        (void**) &sCandValue,
                                                        KNL_ENV(aEnv) )
                                     == STL_SUCCESS );
                            
                            if( dtlDataTypeGroup[ sLeftRowExpr->mArgs[ sColIdx ]->mDataType ] == DTL_GROUP_BINARY_INTEGER )
                            {
                                sResultDataType = DTL_TYPE_NUMBER;
                            }
                            else
                            {
                                if( sLeftRowExpr->mArgs[ sColIdx ]->mDataType == DTL_TYPE_CHAR )
                                {
                                    if( sIsExistFixed[ sColIdx ] == STL_TRUE )
                                    {
                                        sResultDataType = DTL_TYPE_CHAR;
                                    }
                                    else
                                    {
                                        sResultDataType = DTL_TYPE_VARCHAR;
                                    }
                                }
                                else if( sLeftRowExpr->mArgs[ sColIdx ]->mDataType == DTL_TYPE_BINARY )
                                {
                                    if( sIsExistFixed[ sColIdx ] == STL_TRUE )
                                    {
                                        sResultDataType = DTL_TYPE_BINARY;
                                    }
                                    else
                                    {
                                        sResultDataType = DTL_TYPE_VARBINARY;
                                    }
                                }
                                else
                                {
                                    sResultDataType =
                                        dtlGroupRepresentType[ dtlDataTypeGroup[ sLeftRowExpr->mArgs[ sColIdx ]->mDataType ] ];
                                }
                            }
                            
                            QLNC_SET_EXPR_COMMON( 
                                &sCandValue->mCommon,
                                &QLL_CANDIDATE_MEM( aEnv ),
                                aEnv, 
                                QLNC_EXPR_TYPE_VALUE,
                                sListColExpr->mCommon.mExprPhraseFlag,
                                sListColExpr->mCommon.mIterationTime, 
                                sListColExpr->mCommon.mPosition,
                                "?",
                                sResultDataType ); 

                            /* Data Type 중 각 Data Type Group의 대표 type만 올 수 있다. */
                            sCandValue->mInternalBindIdx = ++(*aCBOptParamInfo->mParamInfo.mGlobalInternalBindIdx);
                            sCandValue->mData.mString = NULL;  /* 값을 설정하지 않는다. */
                            
                            switch( sResultDataType )
                            {
                                case DTL_TYPE_BOOLEAN :
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_BOOLEAN;
                                    break;
                                case DTL_TYPE_NATIVE_BIGINT :
                                    /* @todo 현재 이 코드로 들어오는 경우는 없다고 판단됨 */
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_BINARY_INTEGER;
                                    break;
                                case DTL_TYPE_NUMBER :
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_NUMERIC;
                                    break;
                                case DTL_TYPE_NATIVE_DOUBLE :
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_BINARY_DOUBLE;
                                    break;
                                case DTL_TYPE_CHAR :
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_STRING_FIXED;
                                    break;
                                case DTL_TYPE_VARCHAR :
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_STRING_VARYING;
                                    break;
                                case DTL_TYPE_BINARY :
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_BITSTRING_FIXED;
                                    break;
                                case DTL_TYPE_VARBINARY :
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_BITSTRING_VARYING;
                                    break;
                                case DTL_TYPE_DATE :
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_DATE_LITERAL_STRING;
                                    break;
                                case DTL_TYPE_TIME :
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_TIME_LITERAL_STRING;
                                    break;
                                case DTL_TYPE_TIME_WITH_TIME_ZONE :
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_TIME_TZ_LITERAL_STRING;
                                    break;
                                case DTL_TYPE_TIMESTAMP :
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_TIMESTAMP_LITERAL_STRING;
                                    break;
                                case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE :
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_TIMESTAMP_TZ_LITERAL_STRING;
                                    break;
                                case DTL_TYPE_INTERVAL_YEAR_TO_MONTH :
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_INTERVAL_YEAR_TO_MONTH_STRING;
                                    break;
                                case DTL_TYPE_INTERVAL_DAY_TO_SECOND :
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_INTERVAL_DAY_TO_SECOND_STRING;
                                    break;
                                case DTL_TYPE_ROWID :
                                    sCandValue->mValueType = QLNC_VALUE_TYPE_ROWID_STRING;
                                    break;
                                default :
                                /* case DTL_TYPE_LONGVARCHAR : */
                                /* case DTL_TYPE_LONGVARBINARY : */
                                    STL_DASSERT( 0 );
                                    break;
                            }
                                
                            /* Value Expression을 Value Expression List 에 추가 */
                            STL_TRY( knlAllocRegionMem(
                                         &QLL_CANDIDATE_MEM( aEnv ),
                                         STL_SIZEOF( qlncRefExprItem ),
                                         (void**) &sRefInitExprItem,
                                         KNL_ENV(aEnv) )
                                     == STL_SUCCESS );

                            sRefInitExprItem->mExpr = (qlncExprCommon *) sCandValue;
                            sRefInitExprItem->mNext = NULL;

                            QLNC_APPEND_ITEM_TO_LIST( aValueExprList, sRefInitExprItem );
                        }   /* for */
                    }  
                }
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            /* Do Nothing */
        }
    }        

    STL_DASSERT( aInKeyRangeExprList->mCount == aValueExprList->mCount );
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Filter로 부터 IN Key Range 후보 AND-Filter List를 구성한다.
 * @param[in]   aCBOptParamInfo    Cost Based Optimizer Parameter Info
 * @param[in]   aIndexScanInfo     Index Scan Info
 * @param[in]   aAndFilter         Source AND-Filter Expression
 * @param[out]  aAndFilterList     AND-Filter Expression List for IN Key Range
 * @param[out]  aListColMapList    List Column Map List for IN Key Range
 * @param[in]   aEnv               Environment
 */
stlStatus qlncGetInKeyRangeAndFilter( qlncCBOptParamInfo    * aCBOptParamInfo,
                                      qlncIndexScanInfo     * aIndexScanInfo,
                                      qlncAndFilter         * aAndFilter,
                                      qlncRefExprList       * aAndFilterList,
                                      qlncListColMapList   ** aListColMapList,
                                      qllEnv                * aEnv )
{
    qlncExprListFunc    * sListFuncExpr;
    qlncExprFunction    * sNewFuncExpr;

    qlncBooleanFilter   * sNewBooleanFilter;

    qlncExprListCol     * sListColExpr;
    qlncExprListCol     * sNewListColExpr;

    qlncExprRowExpr     * sLeftRowExpr;
    qlncExprRowExpr     * sRightRowExpr;
    qlncExprRowExpr     * sNewRowExpr;
    
    qlncExprCommon      * sPrevExpr;
    qlncRefExprItem     * sRefExprItem;
    qlncRefExprItem     * sRefValueExprItem;
    qlncAndFilter       * sNewAndFilter;
    qlncRefExprList       sInKeyRangeExprList;
    qlncRefExprList       sValueExprList;
    qlncRefExprItem     * sRefListFuncExprItem;

    qlncListColMapList  * sListColMapList;
    
    stlInt32              sInKeyRangeExprCnt;
    stlInt32              sLoop;
    stlInt32              sRowIdx;

    qlncRefExprList            sSubQueryExprList;
    qlncConvertExprParamInfo   sConvertExprParamInfo;
    
    
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCBOptParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexScanInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAndFilterList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aListColMapList != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * OUTPUT 초기화
     */

    QLNC_INIT_LIST( aAndFilterList );
    *aListColMapList = NULL;

    if( aAndFilter == NULL )
    {
        STL_THROW( RAMP_FINISH );        
    }
    

    /**
     * IN Key Range 대상 List Function Expression 분석
     */
    
    STL_TRY( qlncGetInKeyRangeExprList( aCBOptParamInfo,
                                        aIndexScanInfo,
                                        aAndFilter,
                                        & sInKeyRangeExprList,
                                        & sValueExprList,
                                        aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( sInKeyRangeExprList.mCount > 0, RAMP_FINISH );


    /**
     * List Column Map List 공간 할당
     */

    sRefExprItem       = sInKeyRangeExprList.mHead;
    sPrevExpr          = sRefExprItem->mExpr;
    sInKeyRangeExprCnt = 1;
    
    while( sRefExprItem != NULL )
    {
        if( sPrevExpr != sRefExprItem->mExpr )
        {
            sInKeyRangeExprCnt++;
        }

        sPrevExpr    = sRefExprItem->mExpr;
        sRefExprItem = sRefExprItem->mNext;
    }
    

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncListColMapList ) * sInKeyRangeExprCnt,
                                (void**)&sListColMapList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    *aListColMapList = sListColMapList;

    for( sLoop = 1; sLoop < sInKeyRangeExprCnt; sLoop++ )
    {
        sListColMapList[ sLoop - 1 ].mNext = & sListColMapList[ sLoop ];
    }
    sListColMapList[ sInKeyRangeExprCnt - 1 ].mNext = NULL;
            
                
    /**
     * IN Key Range 대상 List Function Expression을 분석하여 AND-Filter 재구성
     */

    sConvertExprParamInfo.mParamInfo = aCBOptParamInfo->mParamInfo;
    sConvertExprParamInfo.mTableMapArr = NULL;
    sConvertExprParamInfo.mOuterTableMapArr = NULL;
    sConvertExprParamInfo.mInitExpr = NULL;
    sConvertExprParamInfo.mCandExpr = NULL;
    sConvertExprParamInfo.mExprPhraseFlag = 0;
    sConvertExprParamInfo.mSubQueryExprList = &sSubQueryExprList;
    sConvertExprParamInfo.mExprSubQuery = NULL;

    sRefExprItem      = sInKeyRangeExprList.mHead;
    sPrevExpr         = sRefExprItem->mExpr;
    sRefValueExprItem = sValueExprList.mHead;

    while( sRefExprItem != NULL )
    {
        STL_DASSERT( sRefValueExprItem != NULL );
        
        /**
         * Copy AND-Filter
         */
        
        STL_TRY( qlncCopyAndFilter( &aCBOptParamInfo->mParamInfo,
                                    aAndFilter,
                                    &sNewAndFilter,
                                    aEnv )
                 == STL_SUCCESS );


        /**
         * Remove OR-Filter : NULL 설정
         */

        for( sLoop = 0; sLoop < sNewAndFilter->mOrCount; sLoop++ )
        {
            if( (qlncExprCommon*)(sNewAndFilter->mOrFilters[ sLoop ]) == sRefExprItem->mExpr )
            {
                /* Remove OR Filter */
                sNewAndFilter->mOrFilters[ sLoop ] = NULL;

                /* compact AND filter */
                STL_TRY( qlncCompactAndFilter( sNewAndFilter, aEnv )
                         == STL_SUCCESS );
                break;
            }
        }

        
        /**
         * List Function 쪼개기
         */

        sListFuncExpr = (qlncExprListFunc *)(((qlncBooleanFilter*)(((qlncOrFilter*)(sRefExprItem->mExpr))->mFilters[0]))->mExpr);
        
        sListColExpr = (qlncExprListCol *) sListFuncExpr->mArgs[ 1 ];
        sLeftRowExpr = (qlncExprRowExpr *) sListColExpr->mArgs[ 0 ];
        sListColExpr = (qlncExprListCol *) sListFuncExpr->mArgs[ 0 ];

        sListColMapList->mListColMap.mMapCount = sLeftRowExpr->mArgCount;
        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprCommon * ) * sLeftRowExpr->mArgCount,
                                    (void**)&sListColMapList->mListColMap.mValueExpr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncExprCommon * ) * sLeftRowExpr->mArgCount,
                                    (void**)&sListColMapList->mListColMap.mListColExpr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Post Optimizer에서 Filter 복원시 필요한 원본 Expression 저장 */
        sListColMapList->mListColMap.mOrgFunc = sRefExprItem->mExpr;
        
        /* List Column 및 Row Expression 구성 */
        for( sLoop = 0; sLoop < sLeftRowExpr->mArgCount; sLoop++ )
        {
            /* List Column 구성 */
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprListCol ),
                                        (void**)&sNewListColExpr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
                        
            stlMemcpy( sNewListColExpr,
                       sListColExpr,
                       STL_SIZEOF( qlncExprListCol ) );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * sListColExpr->mArgCount,
                                        (void**) &sNewListColExpr->mArgs,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( knlListEntry* ) * sListColExpr->mArgCount,
                                        (void**) &sNewListColExpr->mEntry,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            /* List Col Map의 List Column Expression 설정 */
            sListColMapList->mListColMap.mListColExpr[ sLoop ] = (qlncExprCommon *)sNewListColExpr;

            /* List Col Map의 Value Expression 설정 */
            if( sLoop == 0 )
            {
                sListColMapList->mListColMap.mValueExpr[ sLoop ] = (qlncExprCommon *)sRefValueExprItem->mExpr;
            }
            else
            {
                STL_TRY( qlncDuplicateExpr( & aCBOptParamInfo->mParamInfo,
                                            (qlncExprCommon *)sRefValueExprItem->mExpr,
                                            & sListColMapList->mListColMap.mValueExpr[ sLoop ],
                                            aEnv )
                         == STL_SUCCESS );
            }

            /* Row Expression 구성 */
            for( sRowIdx = 0; sRowIdx < sListColExpr->mArgCount; sRowIdx++ )
            {
                sRightRowExpr = (qlncExprRowExpr *) sListColExpr->mArgs[ sRowIdx ];
                
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncExprRowExpr ),
                                            (void**)&sNewRowExpr,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( &sNewRowExpr->mCommon,
                           &sRightRowExpr->mCommon,
                           STL_SIZEOF( qlncExprCommon ) );

                sNewRowExpr->mArgCount = 1;
                
                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( qlncExprCommon* ),
                                            (void**) &sNewRowExpr->mArgs,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sNewRowExpr->mArgs[ 0 ] = sRightRowExpr->mArgs[ sLoop ];

                STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                            STL_SIZEOF( knlListEntry ),
                                            (void**) &sNewRowExpr->mEntry,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemset( sNewRowExpr->mEntry,
                           0x00,
                           STL_SIZEOF( knlListEntry ) );
                
                sNewListColExpr->mArgs[ sRowIdx ]  = (qlncExprCommon *) sNewRowExpr;
                sNewListColExpr->mEntry[ sRowIdx ] = sNewRowExpr->mEntry;
            }

            /* Equal Function 생성 */
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprFunction ),
                                        (void**) &sNewFuncExpr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemcpy( &sNewFuncExpr->mCommon,
                       &sListFuncExpr->mCommon,
                       STL_SIZEOF( qlncExprCommon ) );

            sNewFuncExpr->mCommon.mType = QLNC_EXPR_TYPE_FUNCTION;

            sNewFuncExpr->mFuncId = KNL_BUILTIN_FUNC_IS_EQUAL;
            sNewFuncExpr->mArgCount = 2;

            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncExprCommon* ) * 2,
                                        (void**) &sNewFuncExpr->mArgs,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sNewFuncExpr->mArgs[ 0 ] = sLeftRowExpr->mArgs[ sLoop ];
            sNewFuncExpr->mArgs[ 1 ] = sListColMapList->mListColMap.mValueExpr[ sLoop ];

            /* Boolean Filter 생성*/
            STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                        STL_SIZEOF( qlncBooleanFilter ),
                                        (void**) &sNewBooleanFilter,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemcpy( &sNewBooleanFilter->mCommon,
                       &sListFuncExpr->mCommon,
                       STL_SIZEOF( qlncExprCommon ) );

            sNewBooleanFilter->mCommon.mType = QLNC_EXPR_TYPE_BOOLEAN_FILTER;

            sNewBooleanFilter->mFuncID = KNL_BUILTIN_FUNC_IS_EQUAL;
            sNewBooleanFilter->mExpr = (qlncExprCommon*)sNewFuncExpr;
            QLNC_INIT_LIST( &(sNewBooleanFilter->mRefNodeList) );

            sNewBooleanFilter->mPossibleJoinCond = STL_FALSE;
            sNewBooleanFilter->mIsPhysicalFilter = STL_TRUE;
            sNewBooleanFilter->mSubQueryExprList = NULL;

            /* Node List 구성 */
            sNewBooleanFilter->mLeftRefNodeList = NULL;
            sNewBooleanFilter->mRightRefNodeList = NULL;
            
            /* Reference Node List 설정 */
            sConvertExprParamInfo.mRefNodeList = &(sNewBooleanFilter->mRefNodeList);
            STL_TRY( qlncFindAddRefColumnToRefNodeList( &sConvertExprParamInfo,
                                                        sNewBooleanFilter->mExpr,
                                                        aEnv )
                     == STL_SUCCESS );

            /* Add OR-Filter (OR Filter 생성 후 Add) */
            STL_TRY( qlncAppendFilterToAndFilter( &aCBOptParamInfo->mParamInfo,
                                                  sNewAndFilter,
                                                  (qlncExprCommon*)sNewBooleanFilter,
                                                  aEnv )
                     == STL_SUCCESS );

            sRefValueExprItem = sRefValueExprItem->mNext;
        }
        
        /* AND-Filter를 AND-Filter List 에 추가 */
        STL_TRY( knlAllocRegionMem(
                     &QLL_CANDIDATE_MEM( aEnv ),
                     STL_SIZEOF( qlncRefExprItem ),
                     (void**) &sRefListFuncExprItem,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRefListFuncExprItem->mExpr = (qlncExprCommon *) sNewAndFilter;
        sRefListFuncExprItem->mNext = NULL;

        QLNC_APPEND_ITEM_TO_LIST( aAndFilterList, sRefListFuncExprItem );

        /* Next Multi-Range */
        while( sPrevExpr == sRefExprItem->mExpr )
        {
            sRefExprItem = sRefExprItem->mNext;

            STL_TRY_THROW( sRefExprItem != NULL, RAMP_FINISH );
        }
        
        sPrevExpr       = sRefExprItem->mExpr;
        sListColMapList = sListColMapList->mNext;
    }

    STL_RAMP( RAMP_FINISH );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Boolean Filter를 생성한다.
 * @param[in]   aConvertExprParamInfo   Convert Expression Parameter Info
 * @param[in]   aRefNodeList            Reference Node List
 * @param[in]   aFuncID                 Built-In Function ID
 * @param[in]   aSubQueryExprList       Sub Query Expression List
 * @param[in]   aColumnName             Expression Name
 * @param[out]  aResultFilter           생성된 Filter
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncMakeFilter( qlncConvertExprParamInfo    * aConvertExprParamInfo,
                          qlncRefNodeList             * aRefNodeList,
                          knlBuiltInFunc                aFuncID,
                          qlncRefExprList             * aSubQueryExprList,
                          stlChar                     * aColumnName,
                          qlncExprCommon             ** aResultFilter,
                          qllEnv                      * aEnv )
{
    qlncExprCommon          * sResultFilter;
    qlncBooleanFilter       * sBooleanFilter;
    qlncConstBooleanFilter  * sConstBooleanFilter;

    
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aConvertExprParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConvertExprParamInfo->mCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefNodeList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultFilter != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Make Boolean Filter
     */

    aConvertExprParamInfo->mSubQueryExprList = aSubQueryExprList;
    
    if( aRefNodeList->mCount > 0 )
    {
        /* Boolean Filter 할당 */ 
        STL_TRY( knlAllocRegionMem( 
                     &QLL_CANDIDATE_MEM( aEnv ),
                     STL_SIZEOF( qlncBooleanFilter ), 
                     (void**)&sBooleanFilter,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_SET_EXPR_COMMON( 
            &sBooleanFilter->mCommon,
            &QLL_CANDIDATE_MEM( aEnv ), 
            aEnv, 
            QLNC_EXPR_TYPE_BOOLEAN_FILTER,
            aConvertExprParamInfo->mCandExpr->mExprPhraseFlag,
            aConvertExprParamInfo->mCandExpr->mIterationTime, 
            aConvertExprParamInfo->mCandExpr->mPosition,
            aColumnName,
            DTL_TYPE_BOOLEAN ); 

        sBooleanFilter->mPossibleJoinCond = STL_FALSE; 
        sBooleanFilter->mFuncID = aFuncID;
        sBooleanFilter->mExpr = aConvertExprParamInfo->mCandExpr;
        QLNC_COPY_LIST_INFO( aRefNodeList, &(sBooleanFilter->mRefNodeList) );
        sResultFilter = (qlncExprCommon*)sBooleanFilter;
        sBooleanFilter->mLeftRefNodeList = NULL; 
        sBooleanFilter->mRightRefNodeList = NULL;
        if( aSubQueryExprList->mCount > 0 )
        { 
            STL_TRY( knlAllocRegionMem( 
                         &QLL_CANDIDATE_MEM( aEnv ),
                         STL_SIZEOF( qlncRefExprList ), 
                         (void**)&(sBooleanFilter->mSubQueryExprList), 
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            sBooleanFilter->mSubQueryExprList->mCount = aSubQueryExprList->mCount;
            sBooleanFilter->mSubQueryExprList->mHead = aSubQueryExprList->mHead;
            sBooleanFilter->mSubQueryExprList->mTail = aSubQueryExprList->mTail;
        } 
        else
        { 
            sBooleanFilter->mSubQueryExprList = NULL;
        } 
    } 
    else
    { 
        STL_DASSERT( aRefNodeList->mCount == 0 ); 

        /* Const Boolean Filter 할당 */ 
        STL_TRY( knlAllocRegionMem( 
                     &QLL_CANDIDATE_MEM( aEnv ),
                     STL_SIZEOF( qlncConstBooleanFilter ),
                     (void**)&sConstBooleanFilter, 
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        QLNC_SET_EXPR_COMMON( 
            &sConstBooleanFilter->mCommon, 
            &QLL_CANDIDATE_MEM( aEnv ), 
            aEnv, 
            QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER,
            aConvertExprParamInfo->mCandExpr->mExprPhraseFlag,
            aConvertExprParamInfo->mCandExpr->mIterationTime, 
            aConvertExprParamInfo->mCandExpr->mPosition,
            aColumnName,
            DTL_TYPE_BOOLEAN );

        sConstBooleanFilter->mExpr = aConvertExprParamInfo->mCandExpr; 
        sResultFilter = (qlncExprCommon*)sConstBooleanFilter; 
        if( aSubQueryExprList->mCount > 0 )
        { 
            STL_TRY( knlAllocRegionMem( 
                         &QLL_CANDIDATE_MEM( aEnv ),
                         STL_SIZEOF( qlncRefExprList ), 
                         (void**)&(sConstBooleanFilter->mSubQueryExprList),
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            sConstBooleanFilter->mSubQueryExprList->mCount = aSubQueryExprList->mCount; 
            sConstBooleanFilter->mSubQueryExprList->mHead = aSubQueryExprList->mHead; 
            sConstBooleanFilter->mSubQueryExprList->mTail = aSubQueryExprList->mTail; 
        } 
        else
        { 
            sConstBooleanFilter->mSubQueryExprList = NULL; 
        } 
    }


    *aResultFilter = sResultFilter;
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlnc */
