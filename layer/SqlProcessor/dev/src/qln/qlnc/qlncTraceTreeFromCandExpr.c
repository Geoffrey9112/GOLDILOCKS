/*******************************************************************************
 * qlncTraceTreeFromCandExpr.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlncTraceTreeFromCandExpr.c 9689 2013-09-26 02:42:15Z bsyou $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file qlncTraceTreeFromCandExpr.c
 * @brief SQL Processor Layer Optimizer Trace Tree From Candidate Expression
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Trace Tree From Candidate Expression Function List
 */
const qlncTraceTreeFromCandExprFuncPtr qlncTraceTreeFromCandExprFuncList[ QLNC_EXPR_TYPE_MAX ] =
{
    qlncTraceTreeFromCandExprValue,                 /**< QLNC_EXPR_TYPE_VALUE */
    qlncTraceTreeFromCandExprBindParam,             /**< QLNC_EXPR_TYPE_BIND_PARAM */
    qlncTraceTreeFromCandExprColumn,                /**< QLNC_EXPR_TYPE_COLUMN */
    qlncTraceTreeFromCandExprFunction,              /**< QLNC_EXPR_TYPE_FUNCTION */
    qlncTraceTreeFromCandExprCast,                  /**< QLNC_EXPR_TYPE_CAST */
    qlncTraceTreeFromCandExprPseudoColumn,          /**< QLNC_EXPR_TYPE_PSEUDO_COLUMN */
    qlncTraceTreeFromCandExprPseudoArgument,        /**< QLNC_EXPR_TYPE_PSEUDO_ARGUMENT */
    qlncTraceTreeFromCandExprConstantExprResult,    /**< QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT */
    qlncTraceTreeFromCandExprSubQuery,              /**< QLNC_EXPR_TYPE_SUB_QUERY */
    qlncTraceTreeFromCandExprReference,             /**< QLNC_EXPR_TYPE_REFERENCE */
    qlncTraceTreeFromCandExprInnerColumn,           /**< QLNC_EXPR_TYPE_INNER_COLUMN */
    qlncTraceTreeFromCandExprOuterColumn,           /**< QLNC_EXPR_TYPE_OUTER_COLUMN */
    qlncTraceTreeFromCandExprRowidColumn,           /**< QLNC_EXPR_TYPE_ROWID_COLUMN */
    qlncTraceTreeFromCandExprAggregation,           /**< QLNC_EXPR_TYPE_AGGREGATION */
    qlncTraceTreeFromCandExprCaseExpr,              /**< QLNC_EXPR_TYPE_CASE_EXPR */
    qlncTraceTreeFromCandExprListFunction,          /**< QLNC_EXPR_TYPE_LIST_FUNCTION */
    qlncTraceTreeFromCandExprListColumn,            /**< QLNC_EXPR_TYPE_LIST_COLUMN */
    qlncTraceTreeFromCandExprRowExpr,               /**< QLNC_EXPR_TYPE_ROW_EXPR */
    qlncTraceTreeFromCandExprAndFilter,             /**< QLNC_EXPR_TYPE_AND_FILTER */
    qlncTraceTreeFromCandExprOrFilter,              /**< QLNC_EXPR_TYPE_OR_FILTER */
    qlncTraceTreeFromCandExprBooleanFilter,         /**< QLNC_EXPR_TYPE_BOOLEAN_FILTER */
    qlncTraceTreeFromCandExprConstBooleanFilter     /**< QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER */
};


/**
 * @brief Value Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprValue( qlncStringBlock       * aStringBlock,
                                          qlncExprCommon            * aCandExpr,
                                          qlncTraceSubQueryExprList * aSubQueryExprList,
                                          qllEnv                    * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_VALUE, QLL_ERROR_STACK(aEnv) );


    if( ((qlncExprValue*)aCandExpr)->mInternalBindIdx != QLV_INTERNAL_BIND_VARIABLE_IDX_NA )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "?" )
                 == STL_SUCCESS );
    }
    else
    {
        switch( ((qlncExprValue*)aCandExpr)->mValueType )
        {
            case QLNC_VALUE_TYPE_BOOLEAN:
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "%s",
                                                             ( ((qlncExprValue*)aCandExpr)->mData.mInteger == STL_TRUE
                                                               ? DTL_BOOLEAN_STRING_TRUE
                                                               : DTL_BOOLEAN_STRING_FALSE ) )
                         == STL_SUCCESS );
                break;

            case QLNC_VALUE_TYPE_BINARY_INTEGER:
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "%ld",
                                                             ((qlncExprValue*)aCandExpr)->mData.mInteger )
                         == STL_SUCCESS );
                break;

            case QLNC_VALUE_TYPE_NULL:
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "NULL" )
                         == STL_SUCCESS );
                break;

            default:
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "%s",
                                                             ((qlncExprValue*)aCandExpr)->mData.mString )
                         == STL_SUCCESS );
                break;
        }
    }
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Bind Param Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprBindParam( qlncStringBlock           * aStringBlock,
                                              qlncExprCommon            * aCandExpr,
                                              qlncTraceSubQueryExprList * aSubQueryExprList,
                                              qllEnv                    * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_BIND_PARAM, QLL_ERROR_STACK(aEnv) );

    
    if( ((qlncExprBindParam*)aCandExpr)->mHostName == NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "?" )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     ":%s",
                                                     ((qlncExprBindParam*)aCandExpr)->mHostName )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Column Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprColumn( qlncStringBlock              * aStringBlock,
                                           qlncExprCommon               * aCandExpr,
                                           qlncTraceSubQueryExprList    * aSubQueryExprList,
                                           qllEnv                       * aEnv )
{
    qlncRelationName    * sRelationName = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_COLUMN, QLL_ERROR_STACK(aEnv) );


    /* Relation Name 설정 */
    STL_DASSERT( ((qlncExprColumn*)aCandExpr)->mNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE );
    sRelationName = &(((qlncBaseTableNode*)(((qlncExprColumn*)aCandExpr)->mNode))->mTableInfo->mRelationName);

    /* Catalog Name 출력 */
    if( sRelationName->mCatalog != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s.",
                                                     sRelationName->mCatalog )
                 == STL_SUCCESS );
    }

    /* Schema Name 출력 */
    if( sRelationName->mSchema != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s.",
                                                     sRelationName->mSchema )
                 == STL_SUCCESS );
    }

    /* Table Name 출력 */
    if( sRelationName->mTable != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s.",
                                                     sRelationName->mTable )
                 == STL_SUCCESS );
    }

    /* Column Name 출력 */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%s",
                                                 ellGetColumnName( ((qlncExprColumn*)aCandExpr)->mColumnHandle ) )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Function Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprFunction( qlncStringBlock            * aStringBlock,
                                             qlncExprCommon             * aCandExpr,
                                             qlncTraceSubQueryExprList  * aSubQueryExprList,
                                             qllEnv                     * aEnv )
{
    stlBool               sIsFunction;
    stlInt32              i;
    qlncExprCommon      * sCandExpr         = NULL;
    qlncExprFunction    * sCandFunction     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_FUNCTION, QLL_ERROR_STACK(aEnv) );


#define QLNC_TRACE_IS_FUNCTION_EXPR( _aCandExpr )                               \
    {                                                                           \
        sIsFunction = STL_FALSE;                                                \
        sCandExpr = (_aCandExpr);                                               \
        while( STL_TRUE )                                                       \
        {                                                                       \
            switch( sCandExpr->mType )                                          \
            {                                                                   \
                case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:                       \
                    sCandExpr = ((qlncExprConstExpr*)sCandExpr)->mOrgExpr;      \
                    continue;                                                   \
                case QLNC_EXPR_TYPE_INNER_COLUMN:                               \
                    if( ((qlncExprInnerColumn*)sCandExpr)->mNode->mNodeType     \
                        == QLNC_NODE_TYPE_SUB_TABLE )                           \
                    {                                                           \
                        sIsFunction = STL_FALSE;                                \
                        break;                                                  \
                    }                                                           \
                    else                                                        \
                    {                                                           \
                        sCandExpr = ((qlncExprInnerColumn*)sCandExpr)->mOrgExpr;\
                        continue;                                               \
                    }                                                           \
                case QLNC_EXPR_TYPE_OUTER_COLUMN:                               \
                    sCandExpr = ((qlncExprOuterColumn*)sCandExpr)->mOrgExpr;    \
                    continue;                                                   \
                case QLNC_EXPR_TYPE_FUNCTION:                                   \
                    if( ((qlncExprFunction*)sCandExpr)->mFuncId ==              \
                        KNL_BUILTIN_FUNC_LIKE_PATTERN )                         \
                    {                                                           \
                        sIsFunction = STL_FALSE;                                \
                    }                                                           \
                    else                                                        \
                    {                                                           \
                        sIsFunction = STL_TRUE;                                 \
                    }                                                           \
                    break;                                                      \
                case QLNC_EXPR_TYPE_LIST_FUNCTION:                              \
                    if( ((qlncExprListFunc*)sCandExpr)->mFuncId ==              \
                        KNL_BUILTIN_FUNC_EXISTS )                               \
                    {                                                           \
                        sIsFunction = STL_FALSE;                                \
                    }                                                           \
                    else if( ((qlncExprListFunc*)sCandExpr)->mFuncId ==         \
                             KNL_BUILTIN_FUNC_NOT_EXISTS )                      \
                    {                                                           \
                        sIsFunction = STL_FALSE;                                \
                    }                                                           \
                    else                                                        \
                    {                                                           \
                        sIsFunction = STL_TRUE;                                 \
                    }                                                           \
                    break;                                                      \
                default:                                                        \
                    break;                                                      \
            }                                                                   \
            break;                                                              \
        }                                                                       \
    }

#define QLNC_TRACE_SET_ARG_STRING( _aArg )                                  \
    {                                                                       \
        QLNC_TRACE_IS_FUNCTION_EXPR( (_aArg) );                             \
        if( sIsFunction == STL_TRUE )                                       \
        {                                                                   \
            STL_TRY( qlncTraceAppendStringToStringBlock(                    \
                         aStringBlock,                                      \
                         aEnv,                                              \
                         "(" )                                              \
                     == STL_SUCCESS );                                      \
                                                                            \
            STL_TRY( qlncTraceTreeFromCandExprFuncList[ (_aArg)->mType ](   \
                         aStringBlock,                                      \
                         (_aArg),                                           \
                         aSubQueryExprList,                                 \
                         aEnv )                                             \
                     == STL_SUCCESS );                                      \
                                                                            \
            STL_TRY( qlncTraceAppendStringToStringBlock(                    \
                         aStringBlock,                                      \
                         aEnv,                                              \
                         ")" )                                              \
                     == STL_SUCCESS );                                      \
        }                                                                   \
        else                                                                \
        {                                                                   \
            STL_TRY( qlncTraceTreeFromCandExprFuncList[ (_aArg)->mType ](   \
                         aStringBlock,                                      \
                         (_aArg),                                           \
                         aSubQueryExprList,                                 \
                         aEnv )                                             \
                     == STL_SUCCESS );                                      \
        }                                                                   \
    }


    /**
     * Function 기준 순회 방법에 따른 String 구성
     */

    sCandFunction = (qlncExprFunction*)aCandExpr;
    switch( sCandFunction->mFuncId )
    {
        /* 전위 순회형 Expression */
        case KNL_BUILTIN_FUNC_NOT:
        case KNL_BUILTIN_FUNC_POSITIVE:
        case KNL_BUILTIN_FUNC_NEGATIVE:
        case KNL_BUILTIN_FUNC_BITWISE_NOT:
            {
                STL_DASSERT( sCandFunction->mArgCount == 1 );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             "%s",
                             QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandFunction->mFuncId ) )
                         == STL_SUCCESS );

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[0] );

                break;
            }

        /* 후위 순회형 Expression */
        case KNL_BUILTIN_FUNC_IS_NULL:
        case KNL_BUILTIN_FUNC_IS_NOT_NULL:
        case KNL_BUILTIN_FUNC_IS_UNKNOWN:
        case KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN:
            {
                STL_DASSERT( sCandFunction->mArgCount == 1 );

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[0] );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " %s",
                             QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandFunction->mFuncId ) )
                         == STL_SUCCESS );

                break;
            }

        /* 중위 순회형 Expression */
        case KNL_BUILTIN_FUNC_IS_EQUAL:
        case KNL_BUILTIN_FUNC_IS_NOT_EQUAL:
        case KNL_BUILTIN_FUNC_IS_LESS_THAN:
        case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL:
        case KNL_BUILTIN_FUNC_IS_GREATER_THAN:
        case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL:
        case KNL_BUILTIN_FUNC_IS:
        case KNL_BUILTIN_FUNC_IS_NOT:
        case KNL_BUILTIN_FUNC_ADD:
        case KNL_BUILTIN_FUNC_SUB:
        case KNL_BUILTIN_FUNC_MUL:
        case KNL_BUILTIN_FUNC_DIV:
        case KNL_BUILTIN_FUNC_BITWISE_AND:
        case KNL_BUILTIN_FUNC_BITWISE_OR:
        case KNL_BUILTIN_FUNC_BITWISE_XOR:
        case KNL_BUILTIN_FUNC_BITWISE_SHIFTLEFT:
        case KNL_BUILTIN_FUNC_BITWISE_SHIFTRIGHT:
        case KNL_BUILTIN_FUNC_CONCATENATE:
            {
                STL_DASSERT( sCandFunction->mArgCount == 2 );

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[0] );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " %s ",
                             QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandFunction->mFuncId ) )
                         == STL_SUCCESS );

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[1] );

                break;
            }

        /* A and B and C ..., A or B or C ... */
        case KNL_BUILTIN_FUNC_AND:
        case KNL_BUILTIN_FUNC_OR:
            {
                STL_DASSERT( sCandFunction->mArgCount >= 2 );

                for( i = 0; i < sCandFunction->mArgCount; i++ )
                {
                    QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[i] );

                    if( (i + 1) < sCandFunction->mArgCount )
                    {
                        STL_TRY( qlncTraceAppendStringToStringBlock(
                                     aStringBlock,
                                     aEnv,
                                     " %s ",
                                     QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandFunction->mFuncId ) )
                                 == STL_SUCCESS );
                    }
                }
                break;
            }

        /* between A and B */
        case KNL_BUILTIN_FUNC_BETWEEN:
        case KNL_BUILTIN_FUNC_NOT_BETWEEN:
            {
                STL_DASSERT( sCandFunction->mArgCount == 3 );

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[0] );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " %s",
                             QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandFunction->mFuncId ) )
                         == STL_SUCCESS );

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[1] );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " AND " )
                         == STL_SUCCESS );

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[2] );

                break;
            }

        /* between symmetric A and B */
        case KNL_BUILTIN_FUNC_BETWEEN_SYMMETRIC:
        case KNL_BUILTIN_FUNC_NOT_BETWEEN_SYMMETRIC:
            {
                STL_DASSERT( sCandFunction->mArgCount == 3 + 1 );

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[0] );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " %s",
                             QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandFunction->mFuncId ) )
                         == STL_SUCCESS );

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[2] );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " AND " )
                         == STL_SUCCESS );

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[3] );

                break;
            }

        /* like */
        case KNL_BUILTIN_FUNC_LIKE:
        case KNL_BUILTIN_FUNC_NOT_LIKE:
            {
                STL_DASSERT( (sCandFunction->mArgCount == 2) ||
                             (sCandFunction->mArgCount == 3) );

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[0] );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " %s ",
                             QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandFunction->mFuncId ) )
                         == STL_SUCCESS );

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[1] );

                if( sCandFunction->mArgCount == 3 )
                {
                    STL_TRY( qlncTraceAppendStringToStringBlock(
                                 aStringBlock,
                                 aEnv,
                                 " ESCAPE " )
                             == STL_SUCCESS );

                    QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[2] );
                }

                break;
            }

        case KNL_BUILTIN_FUNC_DATE_PART:
        case KNL_BUILTIN_FUNC_EXTRACT:
            {
                STL_DASSERT( sCandFunction->mArgCount == 2 );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " %s(",
                             QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandFunction->mFuncId ) )
                         == STL_SUCCESS );

                QLNC_TRACE_IS_FUNCTION_EXPR( sCandFunction->mArgs[0] );
                if( sIsFunction == STL_TRUE )
                {
                    STL_TRY( qlncTraceAppendStringToStringBlock(
                                 aStringBlock,
                                 aEnv,
                                 "(" )
                             == STL_SUCCESS );
                }

                sCandExpr = sCandFunction->mArgs[0];
                while( sCandExpr->mType == QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT )
                {
                    sCandExpr = ((qlncExprConstExpr*)sCandExpr)->mOrgExpr;
                }

                STL_DASSERT( sCandExpr->mType == QLNC_EXPR_TYPE_VALUE );
                STL_DASSERT( ((qlncExprValue*)sCandExpr)->mData.mString[1] == '\0' );

                switch( ((qlncExprValue*)sCandExpr)->mData.mString[0] )
                {
                    case '1':
                        STL_TRY( qlncTraceAppendStringToStringBlock(
                                     aStringBlock,
                                     aEnv,
                                     " YEAR" )
                                 == STL_SUCCESS );
                        break;
                    case '2':
                        STL_TRY( qlncTraceAppendStringToStringBlock(
                                     aStringBlock,
                                     aEnv,
                                     " MONTH" )
                                 == STL_SUCCESS );
                        break;
                    case '3':
                        STL_TRY( qlncTraceAppendStringToStringBlock(
                                     aStringBlock,
                                     aEnv,
                                     " DAY" )
                                 == STL_SUCCESS );
                        break;
                    case '4':
                        STL_TRY( qlncTraceAppendStringToStringBlock(
                                     aStringBlock,
                                     aEnv,
                                     " HOUR" )
                                 == STL_SUCCESS );
                        break;
                    case '5':
                        STL_TRY( qlncTraceAppendStringToStringBlock(
                                     aStringBlock,
                                     aEnv,
                                     " MINUTE" )
                                 == STL_SUCCESS );
                        break;
                    case '6':
                        STL_TRY( qlncTraceAppendStringToStringBlock(
                                     aStringBlock,
                                     aEnv,
                                     " SECOND" )
                                 == STL_SUCCESS );
                        break;
                    case '7':
                        STL_TRY( qlncTraceAppendStringToStringBlock(
                                     aStringBlock,
                                     aEnv,
                                     " TIMEZONE_HOUR" )
                                 == STL_SUCCESS );
                        break;
                    case '8':
                        STL_TRY( qlncTraceAppendStringToStringBlock(
                                     aStringBlock,
                                     aEnv,
                                     " TIMEZONE_MINUTE" )
                                 == STL_SUCCESS );
                        break;
                    default:
                        STL_DASSERT( 0 );
                        break;
                }

                if( sCandFunction->mFuncId == KNL_BUILTIN_FUNC_EXTRACT )
                {
                    STL_TRY( qlncTraceAppendStringToStringBlock(
                                 aStringBlock,
                                 aEnv,
                                 " FROM " )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( qlncTraceAppendStringToStringBlock(
                                 aStringBlock,
                                 aEnv,
                                 " , " )
                             == STL_SUCCESS );
                }

                if( sIsFunction == STL_TRUE )
                {
                    STL_TRY( qlncTraceAppendStringToStringBlock(
                                 aStringBlock,
                                 aEnv,
                                 ")" )
                             == STL_SUCCESS );
                }

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[1] );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             ")" )
                         == STL_SUCCESS );

                break;
            }
        case KNL_BUILTIN_FUNC_DATEADD:
        case KNL_BUILTIN_FUNC_DATEDIFF:
            {
                STL_DASSERT( sCandFunction->mArgCount == 3 );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             "%s(",
                             QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandFunction->mFuncId ) )
                         == STL_SUCCESS );

                sCandExpr = sCandFunction->mArgs[0];
                while( sCandExpr->mType == QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT )
                {
                    sCandExpr = ((qlncExprConstExpr*)sCandExpr)->mOrgExpr;
                }

                STL_DASSERT( sCandExpr->mType == QLNC_EXPR_TYPE_VALUE );

                if( ((qlncExprValue*)sCandExpr)->mData.mString[1] == '\0' )
                {
                    i = ((qlncExprValue*)sCandExpr)->mData.mString[0] - '0';
                }
                else
                {
                    i = ( ((qlncExprValue*)sCandExpr)->mData.mString[0] - '0' ) * 10;
                    i += ( ((qlncExprValue*)sCandExpr)->mData.mString[1] - '0' );
                    STL_DASSERT( ((qlncExprValue*)sCandExpr)->mData.mString[2] == '\0' );
                }

                STL_DASSERT( i < DTL_DATETIME_PART_MAX );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             "%s, ",
                             dtlDatePartName[i] )
                         == STL_SUCCESS );

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[1] );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             ", " )
                         == STL_SUCCESS );

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[2] );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             ")" )
                         == STL_SUCCESS );

                break;
            }
        case KNL_BUILTIN_FUNC_TO_CHAR:
        case KNL_BUILTIN_FUNC_TO_DATE:
        case KNL_BUILTIN_FUNC_TO_TIME:
        case KNL_BUILTIN_FUNC_TO_TIME_WITH_TIME_ZONE:
        case KNL_BUILTIN_FUNC_TO_TIMESTAMP:
        case KNL_BUILTIN_FUNC_TO_TIMESTAMP_WITH_TIME_ZONE:
        case KNL_BUILTIN_FUNC_TO_NUMBER:
        case KNL_BUILTIN_FUNC_TO_NATIVE_REAL:
        case KNL_BUILTIN_FUNC_TO_NATIVE_DOUBLE:
            {
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             "%s",
                             QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandFunction->mFuncId ) )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             "(" )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandFunction->mArgs[0]->mType ](
                             aStringBlock,
                             sCandFunction->mArgs[0],
                             aSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );

                if( sCandFunction->mArgCount > 1 )
                {
                    sCandExpr = sCandFunction->mArgs[1];
                    while( sCandExpr->mType == QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT )
                    {
                        sCandExpr = ((qlncExprConstExpr*)sCandExpr)->mOrgExpr;
                    }

                    STL_DASSERT( sCandExpr->mType == QLNC_EXPR_TYPE_FUNCTION );

                    sCandFunction = (qlncExprFunction*)sCandExpr;
                    if( ( gBuiltInFuncInfoArr[sCandFunction->mFuncId].mArgumentCntMin !=
                          gBuiltInFuncInfoArr[sCandFunction->mFuncId].mArgumentCntMax ) &&
                        ( sCandFunction->mArgCount == gBuiltInFuncInfoArr[sCandFunction->mFuncId].mArgumentCntMin ) )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        STL_TRY( qlncTraceAppendStringToStringBlock(
                                     aStringBlock,
                                     aEnv,
                                     "," )
                                 == STL_SUCCESS );

                        STL_TRY( qlncTraceQueryFromCandExprFuncList[ sCandFunction->mArgs[gBuiltInFuncInfoArr[sCandFunction->mFuncId].mArgumentCntMax - 1]->mType ](
                                     aStringBlock,
                                     sCandFunction->mArgs[gBuiltInFuncInfoArr[sCandFunction->mFuncId].mArgumentCntMax - 1],
                                     aEnv )
                                 == STL_SUCCESS );
                    }
                }

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             ")" )
                         == STL_SUCCESS );

                break;
            }
        case KNL_BUILTIN_FUNC_LIKE_PATTERN:
            {
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " %s ",
                             QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandFunction->mFuncId ) )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandFunction->mArgs[0]->mType ](
                             aStringBlock,
                             sCandFunction->mArgs[0],
                             aSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case KNL_BUILTIN_FUNC_ASSIGN :
            {
                STL_DASSERT( sCandFunction->mArgCount == 2 );

                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[0] );
                break;
            }
        case KNL_BUILTIN_FUNC_TRIM:
            {
                STL_DASSERT( ( sCandFunction->mArgCount >=
                               gBuiltInFuncInfoArr[sCandFunction->mFuncId].mArgumentCntMin ) &&
                             ( sCandFunction->mArgCount <=
                               gBuiltInFuncInfoArr[sCandFunction->mFuncId].mArgumentCntMax ) );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             "%s(",
                             QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandFunction->mFuncId ) )
                         == STL_SUCCESS );

                sCandExpr = sCandFunction->mArgs[0];
                while( sCandExpr->mType == QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT )
                {
                    sCandExpr = ((qlncExprConstExpr*)sCandExpr)->mOrgExpr;
                }

                STL_DASSERT( sCandExpr->mType == QLNC_EXPR_TYPE_VALUE );

                i = ((qlncExprValue*)sCandExpr)->mData.mString[0] - '0';
                STL_DASSERT( i < DTL_TRIM_SPEC_MAX );

                /* trim specification string */
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             "%s",
                             dtlTrimSpecificationStr[i] )
                         == STL_SUCCESS );

                /* trim character */
                if( sCandFunction->mArgCount == 3 )
                {
                    QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[2] );
                }

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " FROM " )
                         == STL_SUCCESS );

                /* trim source */
                QLNC_TRACE_SET_ARG_STRING( sCandFunction->mArgs[1] );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             ")" )
                         == STL_SUCCESS );

                break;
            }
        default:
            {
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " %s(",
                             QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandFunction->mFuncId ) )
                         == STL_SUCCESS );

                for( i = 0; i < sCandFunction->mArgCount; i++ )
                {
                    if( i > 0 )
                    {
                        STL_TRY( qlncTraceAppendStringToStringBlock(
                                     aStringBlock,
                                     aEnv,
                                     "," )
                                 == STL_SUCCESS );
                    }

                    STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandFunction->mArgs[i]->mType ](
                                 aStringBlock,
                                 sCandFunction->mArgs[i],
                                 aSubQueryExprList,
                                 aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             ")" )
                         == STL_SUCCESS );

                break;
            }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Cast Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprCast( qlncStringBlock            * aStringBlock,
                                         qlncExprCommon             * aCandExpr,
                                         qlncTraceSubQueryExprList  * aSubQueryExprList,
                                         qllEnv                     * aEnv )
{
    qlncExprTypeCast    * sCandTypeCast = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_CAST, QLL_ERROR_STACK(aEnv) );


    /* Cast 설정 */
    sCandTypeCast = (qlncExprTypeCast*)aCandExpr;

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "CAST(" )
             == STL_SUCCESS );

    STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandTypeCast->mArgs[0]->mType ](
                 aStringBlock,
                 sCandTypeCast->mArgs[0],
                 aSubQueryExprList,
                 aEnv )
             == STL_SUCCESS );

    switch( sCandTypeCast->mTypeDef.mDBType )
    {
        case DTL_TYPE_BOOLEAN:
        case DTL_TYPE_NATIVE_SMALLINT:
        case DTL_TYPE_NATIVE_INTEGER:
        case DTL_TYPE_NATIVE_BIGINT:
        case DTL_TYPE_NATIVE_REAL:
        case DTL_TYPE_NATIVE_DOUBLE:
        case DTL_TYPE_ROWID:
        case DTL_TYPE_DATE:
            {
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " AS %s )",
                             dtlDataTypeName[sCandTypeCast->mTypeDef.mDBType] )
                         == STL_SUCCESS );

                break;
            }
        case DTL_TYPE_FLOAT:
            {
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " AS %s(%ld) )",
                             dtlDataTypeName[sCandTypeCast->mTypeDef.mDBType],
                             sCandTypeCast->mTypeDef.mArgNum1 )
                         == STL_SUCCESS );
                
                break;
            }
        case DTL_TYPE_NUMBER:
            {
                if( sCandTypeCast->mTypeDef.mArgNum2 == DTL_SCALE_NA )
                {
                    STL_TRY( qlncTraceAppendStringToStringBlock(
                                 aStringBlock,
                                 aEnv,
                                 " AS %s )",
                                 dtlDataTypeName[sCandTypeCast->mTypeDef.mDBType] )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( qlncTraceAppendStringToStringBlock(
                                 aStringBlock,
                                 aEnv,
                                 " AS %s(%ld, %ld) )",
                                 dtlDataTypeName[sCandTypeCast->mTypeDef.mDBType],
                                 sCandTypeCast->mTypeDef.mArgNum1,
                                 sCandTypeCast->mTypeDef.mArgNum2 )
                             == STL_SUCCESS );
                }
                
                break;
            }
        case DTL_TYPE_CHAR:
        case DTL_TYPE_VARCHAR:
        case DTL_TYPE_LONGVARCHAR:
            {
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " AS %s(%ld, %s) )",
                             dtlDataTypeName[sCandTypeCast->mTypeDef.mDBType],
                             sCandTypeCast->mTypeDef.mArgNum1,
                             gDtlLengthUnitString[sCandTypeCast->mTypeDef.mStringLengthUnit] )
                         == STL_SUCCESS );

                break;
            }
        case DTL_TYPE_BINARY:
        case DTL_TYPE_VARBINARY:
        case DTL_TYPE_LONGVARBINARY:
        case DTL_TYPE_TIME:
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
        case DTL_TYPE_TIMESTAMP:
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " AS %s(%ld) )",
                             dtlDataTypeName[sCandTypeCast->mTypeDef.mDBType],
                             sCandTypeCast->mTypeDef.mArgNum1 )
                         == STL_SUCCESS );

                break;
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " AS %s(%s) )",
                             dtlDataTypeName[sCandTypeCast->mTypeDef.mDBType],
                             gDtlIntervalIndicatorString[sCandTypeCast->mTypeDef.mIntervalIndicator] )
                         == STL_SUCCESS );

                break;
            }
        case DTL_TYPE_CLOB:
        case DTL_TYPE_BLOB:
        case DTL_TYPE_DECIMAL:
            {
                STL_DASSERT( 0 );
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
 * @brief Pseudo Column Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprPseudoColumn( qlncStringBlock            * aStringBlock,
                                                 qlncExprCommon             * aCandExpr,
                                                 qlncTraceSubQueryExprList  * aSubQueryExprList,
                                                 qllEnv                     * aEnv )
{
    qlncExprPseudoCol   * sCandPseudoCol    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_PSEUDO_COLUMN, QLL_ERROR_STACK(aEnv) );


    /* Pseudo Column 설정 */
    sCandPseudoCol = (qlncExprPseudoCol*)aCandExpr;

    if( sCandPseudoCol->mArgCount > 0 )
    {
        STL_DASSERT( (sCandPseudoCol->mPseudoId == KNL_PSEUDO_COL_CURRVAL) ||
                     (sCandPseudoCol->mPseudoId == KNL_PSEUDO_COL_NEXTVAL) );

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%s(%s)",
                     gPseudoColInfoArr[sCandPseudoCol->mPseudoId].mName,
                     ellGetSequenceName( sCandPseudoCol->mSeqDictHandle ) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%s()",
                     gPseudoColInfoArr[sCandPseudoCol->mPseudoId].mName )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Pseudo Argument Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprPseudoArgument( qlncStringBlock              * aStringBlock,
                                                   qlncExprCommon               * aCandExpr,
                                                   qlncTraceSubQueryExprList    * aSubQueryExprList,
                                                   qllEnv                       * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_PSEUDO_ARGUMENT, QLL_ERROR_STACK(aEnv) );


    /* Do Nothing */


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Constant Expr Result Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprConstantExprResult( qlncStringBlock              * aStringBlock,
                                                       qlncExprCommon               * aCandExpr,
                                                       qlncTraceSubQueryExprList    * aSubQueryExprList,
                                                       qllEnv                       * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT,
                        QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlncTraceTreeFromCandExprFuncList[ ((qlncExprConstExpr*)aCandExpr)->mOrgExpr->mType ](
                 aStringBlock,
                 ((qlncExprConstExpr*)aCandExpr)->mOrgExpr,
                 aSubQueryExprList,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprSubQuery( qlncStringBlock            * aStringBlock,
                                             qlncExprCommon             * aCandExpr,
                                             qlncTraceSubQueryExprList  * aSubQueryExprList,
                                             qllEnv                     * aEnv )
{
    qlncTraceSubQueryExprItem   * sSubQueryExprItem = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_SUB_QUERY, QLL_ERROR_STACK(aEnv) );


    /* SubQuery Expression Item 생성 */
    STL_TRY( knlAllocRegionMem( aSubQueryExprList->mRegionMem,
                                STL_SIZEOF( qlncTraceSubQueryExprItem ),
                                (void**) &sSubQueryExprItem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* SubQuery Name 생성 */
    stlSnprintf( sSubQueryExprItem->mSubQueryName,
                 QLNC_TRACE_SUBQUERY_NAME_MAX_SIZE,
                 "%s%03d",
                 QLNC_TRACE_SUBQUERY_NAME_PREFIX,
                 aSubQueryExprList->mSubQueryID );

    /* SubQuery Expression Item 설정 */
    sSubQueryExprItem->mSubQueryExpr = (qlncExprSubQuery*)aCandExpr;
    sSubQueryExprItem->mNext = NULL;

    /* SubQuery Expression List 등록 */
    if( aSubQueryExprList->mCount == 0 )
    {
        aSubQueryExprList->mHead = sSubQueryExprItem;
        aSubQueryExprList->mTail = sSubQueryExprItem;
    }
    else
    {
        aSubQueryExprList->mTail->mNext = sSubQueryExprItem;
        aSubQueryExprList->mTail = sSubQueryExprItem;
    }
    aSubQueryExprList->mCount++;
    aSubQueryExprList->mSubQueryID++;

    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 sSubQueryExprItem->mSubQueryName )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Reference Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprReference( qlncStringBlock           * aStringBlock,
                                              qlncExprCommon            * aCandExpr,
                                              qlncTraceSubQueryExprList * aSubQueryExprList,
                                              qllEnv                    * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_REFERENCE, QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlncTraceTreeFromCandExprFuncList[ ((qlncExprReference*)aCandExpr)->mOrgExpr->mType ](
                 aStringBlock,
                 ((qlncExprReference*)aCandExpr)->mOrgExpr,
                 aSubQueryExprList,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Inner Column Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprInnerColumn( qlncStringBlock             * aStringBlock,
                                                qlncExprCommon              * aCandExpr,
                                                qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                qllEnv                      * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_INNER_COLUMN, QLL_ERROR_STACK(aEnv) );


    switch( ((qlncExprInnerColumn*)aCandExpr)->mNode->mNodeType )
    {
        case QLNC_NODE_TYPE_QUERY_SET:
        case QLNC_NODE_TYPE_QUERY_SPEC:
        case QLNC_NODE_TYPE_JOIN_TABLE:
        case QLNC_NODE_TYPE_FLAT_INSTANT:
        case QLNC_NODE_TYPE_SORT_INSTANT:
        case QLNC_NODE_TYPE_HASH_INSTANT:
        case QLNC_NODE_TYPE_INNER_JOIN_TABLE:
        case QLNC_NODE_TYPE_OUTER_JOIN_TABLE:
            {
                STL_DASSERT( ((qlncExprInnerColumn*)aCandExpr)->mOrgExpr != NULL );
                STL_TRY( qlncTraceTreeFromCandExprFuncList[ ((qlncExprInnerColumn*)aCandExpr)->mOrgExpr->mType ](
                             aStringBlock,
                             ((qlncExprInnerColumn*)aCandExpr)->mOrgExpr,
                             aSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLNC_NODE_TYPE_SUB_TABLE:
            {
                qlncSubTableNode    * sSubTableNode     = NULL;
                qlncTarget          * sTarget           = NULL;

                STL_DASSERT( ((qlncExprInnerColumn*)aCandExpr)->mOrgExpr == NULL );

                sSubTableNode = (qlncSubTableNode*)(((qlncExprInnerColumn*)aCandExpr)->mNode);

                if( sSubTableNode->mQueryNode->mNodeType == QLNC_NODE_TYPE_QUERY_SET )
                {
                    STL_DASSERT( ((qlncExprInnerColumn*)aCandExpr)->mIdx <
                                 ((qlncQuerySet*)(sSubTableNode->mQueryNode))->mSetTargetCount );
                    sTarget = &(((qlncQuerySet*)(sSubTableNode->mQueryNode))->mSetTargetArr[((qlncExprInnerColumn*)aCandExpr)->mIdx]);
                }
                else
                {
                    STL_DASSERT( sSubTableNode->mQueryNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC );
                    STL_DASSERT( ((qlncExprInnerColumn*)aCandExpr)->mIdx <
                                 ((qlncQuerySpec*)(sSubTableNode->mQueryNode))->mTargetCount );
                    sTarget = &(((qlncQuerySpec*)(sSubTableNode->mQueryNode))->mTargetArr[((qlncExprInnerColumn*)aCandExpr)->mIdx]);
                }

                /* Relation Name 출력 */
                if( sSubTableNode->mRelationName != NULL )
                {
                    if( sSubTableNode->mRelationName->mTable != NULL )
                    {
                        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                     aEnv,
                                     " %s",
                                     sSubTableNode->mRelationName->mTable )
                                 == STL_SUCCESS );
                    }
                }

                /* Target Name 출력 */
                if( sTarget->mDisplayName != NULL )
                {
                    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                                 aEnv,
                                                                 "%s",
                                                                 sTarget->mDisplayName )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( qlncTraceTreeFromCandExprFuncList[ sTarget->mExpr->mType ](
                                 aStringBlock,
                                 sTarget->mExpr,
                                 aSubQueryExprList,
                                 aEnv )
                             == STL_SUCCESS );
                }

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
 * @brief Outer Column Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprOuterColumn( qlncStringBlock             * aStringBlock,
                                                qlncExprCommon              * aCandExpr,
                                                qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                qllEnv                      * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_OUTER_COLUMN, QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "?" )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Rowid Column Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprRowidColumn( qlncStringBlock             * aStringBlock,
                                                qlncExprCommon              * aCandExpr,
                                                qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                qllEnv                      * aEnv )
{
    qlncRelationName    * sRelationName = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_ROWID_COLUMN, QLL_ERROR_STACK(aEnv) );


    /* Relation Name 설정 */
    STL_DASSERT( ((qlncExprRowidColumn*)aCandExpr)->mNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE );
    sRelationName = &(((qlncBaseTableNode*)(((qlncExprRowidColumn*)aCandExpr)->mNode))->mTableInfo->mRelationName);

    /* Catalog Name 출력 */
    if( sRelationName->mCatalog != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s.",
                                                     sRelationName->mCatalog )
                 == STL_SUCCESS );
    }

    /* Schema Name 출력 */
    if( sRelationName->mSchema != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s.",
                                                     sRelationName->mSchema )
                 == STL_SUCCESS );
    }

    /* Table Name 출력 */
    if( sRelationName->mTable != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s.",
                                                     sRelationName->mTable )
                 == STL_SUCCESS );
    }

    /* Rowid 출력 */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "ROWID" )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Aggregation Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprAggregation( qlncStringBlock             * aStringBlock,
                                                qlncExprCommon              * aCandExpr,
                                                qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                qllEnv                      * aEnv )
{
    qlncExprAggregation * sCandAggregation  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_AGGREGATION, QLL_ERROR_STACK(aEnv) );


    /* Aggregation 설정 */
    sCandAggregation = (qlncExprAggregation*)aCandExpr;

    if( sCandAggregation->mAggrId == KNL_BUILTIN_AGGREGATION_COUNT_ASTERISK )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%s(*)",
                     QLNC_TRACE_GET_AGGREGATION_FUNC_NAME( sCandAggregation->mAggrId ) )
                 == STL_SUCCESS );
    }
    else
    {
        if( sCandAggregation->mIsDistinct == STL_TRUE )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "%s( DISTINCT",
                         QLNC_TRACE_GET_AGGREGATION_FUNC_NAME( sCandAggregation->mAggrId ) )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "%s(",
                         QLNC_TRACE_GET_AGGREGATION_FUNC_NAME( sCandAggregation->mAggrId ) )
                     == STL_SUCCESS );
        }

        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandAggregation->mArgs[0]->mType ](
                     aStringBlock,
                     sCandAggregation->mArgs[0],
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );

         STL_TRY( qlncTraceAppendStringToStringBlock(
                      aStringBlock,
                      aEnv,
                      ")" )
                  == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Case Expr Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprCaseExpr( qlncStringBlock            * aStringBlock,
                                             qlncExprCommon             * aCandExpr,
                                             qlncTraceSubQueryExprList  * aSubQueryExprList,
                                             qllEnv                     * aEnv )
{
    stlInt32              i;
    qlncExprCaseExpr    * sCandCaseExpr = NULL;
    qlncExprFunction    * sCandFunction = NULL;
    qlncExprCommon      * sCandExpr     = NULL;    


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_CASE_EXPR, QLL_ERROR_STACK(aEnv) );


    /* Case Expression 설정 */
    sCandCaseExpr = (qlncExprCaseExpr*)aCandExpr;

    if( sCandCaseExpr->mFuncId == KNL_BUILTIN_FUNC_CASE )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%s",
                     QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandCaseExpr->mFuncId ) )
                 == STL_SUCCESS );

        for( i = 0; i < sCandCaseExpr->mCount; i++ )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         " WHEN " )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandCaseExpr->mWhenArr[i]->mType ](
                         aStringBlock,
                         sCandCaseExpr->mWhenArr[i],
                         aSubQueryExprList,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         " THEN " )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandCaseExpr->mThenArr[i]->mType ](
                         aStringBlock,
                         sCandCaseExpr->mThenArr[i],
                         aSubQueryExprList,
                         aEnv )
                     == STL_SUCCESS );
        }

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     " ELSE " )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandCaseExpr->mDefResult->mType ](
                     aStringBlock,
                     sCandCaseExpr->mDefResult,
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     " END " )
                 == STL_SUCCESS );
    }
    else if( sCandCaseExpr->mFuncId == KNL_BUILTIN_FUNC_NULLIF )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%s( ",
                     QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandCaseExpr->mFuncId ) )
                 == STL_SUCCESS );
        
        sCandExpr = sCandCaseExpr->mWhenArr[0];
        while( sCandExpr->mType == QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT )
        {
            sCandExpr = ((qlncExprConstExpr*)sCandExpr)->mOrgExpr;
        }        
        
        STL_DASSERT( sCandExpr->mType == QLNC_EXPR_TYPE_FUNCTION );
        
        sCandFunction = (qlncExprFunction*)(sCandExpr);
        
        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandFunction->mArgs[0]->mType ](
                     aStringBlock,
                     sCandFunction->mArgs[0],
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     ", " )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandFunction->mArgs[1]->mType ](
                     aStringBlock,
                     sCandFunction->mArgs[1],
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     " )" )
                 == STL_SUCCESS );
    }
    else if( sCandCaseExpr->mFuncId == KNL_BUILTIN_FUNC_COALESCE )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%s( ",
                     QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandCaseExpr->mFuncId ) )
                 == STL_SUCCESS );

        for( i = 0; i < sCandCaseExpr->mCount; i++ )
        {
            STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandCaseExpr->mThenArr[i]->mType ](
                         aStringBlock,
                         sCandCaseExpr->mThenArr[i],
                         aSubQueryExprList,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         ", " )
                     == STL_SUCCESS );
        }

        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandCaseExpr->mDefResult->mType ](
                     aStringBlock,
                     sCandCaseExpr->mDefResult,
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     " )" )
                 == STL_SUCCESS );
    }
    else if( sCandCaseExpr->mFuncId == KNL_BUILTIN_FUNC_NVL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%s( ",
                     QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandCaseExpr->mFuncId ) )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandCaseExpr->mThenArr[0]->mType ](
                     aStringBlock,
                     sCandCaseExpr->mThenArr[0],
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     ", " )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandCaseExpr->mDefResult->mType ](
                     aStringBlock,
                     sCandCaseExpr->mDefResult,
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     " )" )
                 == STL_SUCCESS );
    }
    else if( sCandCaseExpr->mFuncId == KNL_BUILTIN_FUNC_NVL2 )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%s( ",
                     QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandCaseExpr->mFuncId ) )
                 == STL_SUCCESS );
        
        sCandExpr = sCandCaseExpr->mWhenArr[0];
        while( sCandExpr->mType == QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT )
        {
            sCandExpr = ((qlncExprConstExpr*)sCandExpr)->mOrgExpr;
        }        
        
        STL_DASSERT( sCandExpr->mType == QLNC_EXPR_TYPE_FUNCTION );
        
        sCandFunction = (qlncExprFunction*)(sCandExpr);
        
        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandFunction->mArgs[0]->mType ](
                     aStringBlock,
                     sCandFunction->mArgs[0],
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     ", " )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandCaseExpr->mThenArr[0]->mType ](
                     aStringBlock,
                     sCandCaseExpr->mThenArr[0],
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     ", " )
                 == STL_SUCCESS );
        
        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandCaseExpr->mDefResult->mType ](
                     aStringBlock,
                     sCandCaseExpr->mDefResult,
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     " )" )
                 == STL_SUCCESS );
    }
    else if( sCandCaseExpr->mFuncId == KNL_BUILTIN_FUNC_DECODE )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%s( ",
                     QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandCaseExpr->mFuncId ) )
                 == STL_SUCCESS );
        
        sCandExpr = sCandCaseExpr->mWhenArr[0];
        while( sCandExpr->mType == QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT )
        {
            sCandExpr = ((qlncExprConstExpr*)sCandExpr)->mOrgExpr;
        }        
        
        STL_DASSERT( sCandExpr->mType == QLNC_EXPR_TYPE_FUNCTION );
        
        sCandFunction = (qlncExprFunction*)(sCandExpr);
        sCandFunction = (qlncExprFunction*)(sCandFunction->mArgs[0]);
        
        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandFunction->mArgs[0]->mType ](
                     aStringBlock,
                     sCandFunction->mArgs[0],
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );
        
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     ", " )
                 == STL_SUCCESS );
        
        for( i = 0; i < sCandCaseExpr->mCount; i++ )
        {
            sCandExpr = sCandCaseExpr->mWhenArr[i];
            while( sCandExpr->mType == QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT )
            {
                sCandExpr = ((qlncExprConstExpr*)sCandExpr)->mOrgExpr;
            }        
            
            STL_DASSERT( sCandExpr->mType == QLNC_EXPR_TYPE_FUNCTION );
            
            sCandFunction = (qlncExprFunction*)(sCandExpr);
            sCandFunction = (qlncExprFunction*)(sCandFunction->mArgs[0]);
            
            STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandFunction->mArgs[1]->mType ](
                         aStringBlock,
                         sCandFunction->mArgs[1],
                         aSubQueryExprList,
                         aEnv )
                     == STL_SUCCESS );
            
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         ", " )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandCaseExpr->mThenArr[i]->mType ](
                         aStringBlock,
                         sCandCaseExpr->mThenArr[i],
                         aSubQueryExprList,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         ", " )
                     == STL_SUCCESS );
        }


        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandCaseExpr->mDefResult->mType ](
                     aStringBlock,
                     sCandCaseExpr->mDefResult,
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     " )" )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( sCandCaseExpr->mFuncId == KNL_BUILTIN_FUNC_CASE2 );

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%s( ",
                     QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandCaseExpr->mFuncId ) )
                 == STL_SUCCESS );

        for( i = 0; i < sCandCaseExpr->mCount; i++ )
        {
            STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandCaseExpr->mWhenArr[i]->mType ](
                         aStringBlock,
                         sCandCaseExpr->mWhenArr[i],
                         aSubQueryExprList,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         ", " )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandCaseExpr->mThenArr[i]->mType ](
                         aStringBlock,
                         sCandCaseExpr->mThenArr[i],
                         aSubQueryExprList,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         ", " )
                     == STL_SUCCESS );
        }

        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandCaseExpr->mDefResult->mType ](
                     aStringBlock,
                     sCandCaseExpr->mDefResult,
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     " )" )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief List Function Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprListFunction( qlncStringBlock            * aStringBlock,
                                                 qlncExprCommon             * aCandExpr,
                                                 qlncTraceSubQueryExprList  * aSubQueryExprList,
                                                 qllEnv                     * aEnv )
{
    qlncExprListFunc    * sCandListFunc = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_LIST_FUNCTION, QLL_ERROR_STACK(aEnv) );


    /* List Function 설정 */
    sCandListFunc = (qlncExprListFunc*)aCandExpr;

    switch( sCandListFunc->mFuncId )
    {
        case KNL_BUILTIN_FUNC_IN:
        case KNL_BUILTIN_FUNC_NOT_IN:
        case KNL_BUILTIN_FUNC_IS_EQUAL_ROW:
        case KNL_BUILTIN_FUNC_IS_NOT_EQUAL_ROW:
        case KNL_BUILTIN_FUNC_IS_LESS_THAN_ROW:
        case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL_ROW:
        case KNL_BUILTIN_FUNC_IS_GREATER_THAN_ROW:
        case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL_ROW:
        case KNL_BUILTIN_FUNC_EQUAL_ANY:
        case KNL_BUILTIN_FUNC_NOT_EQUAL_ANY:
        case KNL_BUILTIN_FUNC_LESS_THAN_ANY:
        case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY:
        case KNL_BUILTIN_FUNC_GREATER_THAN_ANY:
        case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY:
        case KNL_BUILTIN_FUNC_EQUAL_ALL:
        case KNL_BUILTIN_FUNC_NOT_EQUAL_ALL:
        case KNL_BUILTIN_FUNC_LESS_THAN_ALL:
        case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL:
        case KNL_BUILTIN_FUNC_GREATER_THAN_ALL:
        case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL:
            {
                STL_DASSERT( sCandListFunc->mArgCount == 2 );

                STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandListFunc->mArgs[1]->mType ](
                             aStringBlock,
                             sCandListFunc->mArgs[1],
                             aSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " %s ",
                             QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandListFunc->mFuncId ) )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandListFunc->mArgs[0]->mType ](
                             aStringBlock,
                             sCandListFunc->mArgs[0],
                             aSubQueryExprList,
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case KNL_BUILTIN_FUNC_EXISTS:
        case KNL_BUILTIN_FUNC_NOT_EXISTS:
            {
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             "%s ",
                             QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCandListFunc->mFuncId ) )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandListFunc->mArgs[0]->mType ](
                             aStringBlock,
                             sCandListFunc->mArgs[0],
                             aSubQueryExprList,
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
 * @brief List Column Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprListColumn( qlncStringBlock              * aStringBlock,
                                               qlncExprCommon               * aCandExpr,
                                               qlncTraceSubQueryExprList    * aSubQueryExprList,
                                               qllEnv                       * aEnv )
{
    stlInt32          i;
    qlncExprListCol * sCandListCol  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_LIST_COLUMN, QLL_ERROR_STACK(aEnv) );


    /* List Column 설정 */
    sCandListCol = (qlncExprListCol*)aCandExpr;


    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "( " )
             == STL_SUCCESS );

    STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandListCol->mArgs[0]->mType ](
                 aStringBlock,
                 sCandListCol->mArgs[0],
                 aSubQueryExprList,
                 aEnv )
             == STL_SUCCESS );

    if( sCandListCol->mPredicate == DTL_LIST_PREDICATE_LEFT )
    {
        /* Do Nothing */
    }
    else
    {
        for( i = 1; i < sCandListCol->mArgCount; i++ )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         ", " )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandListCol->mArgs[i]->mType ](
                         aStringBlock,
                         sCandListCol->mArgs[i],
                         aSubQueryExprList,
                         aEnv )
                     == STL_SUCCESS );
        }
    }

    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 " )" )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Row Expr Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprRowExpr( qlncStringBlock             * aStringBlock,
                                            qlncExprCommon              * aCandExpr,
                                            qlncTraceSubQueryExprList   * aSubQueryExprList,
                                            qllEnv                      * aEnv )
{
    stlBool           sIsFunction;
    stlInt32          i;
    qlncExprCommon  * sCandExpr     = NULL;
    qlncExprRowExpr * sCandRowExpr  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_ROW_EXPR, QLL_ERROR_STACK(aEnv) );


#define QLNC_TRACE_IS_ROW_EXPR_FUNCTION( _aCandExpr )                           \
    {                                                                           \
        sIsFunction = STL_FALSE;                                                \
        sCandExpr = (_aCandExpr);                                               \
        while( sCandExpr != NULL )                                              \
        {                                                                       \
            switch( sCandExpr->mType )                                          \
            {                                                                   \
                case QLNC_EXPR_TYPE_CONSTANT_EXPR_RESULT:                       \
                    sCandExpr = ((qlncExprConstExpr*)sCandExpr)->mOrgExpr;      \
                    continue;                                                   \
                case QLNC_EXPR_TYPE_INNER_COLUMN:                               \
                    sCandExpr = ((qlncExprInnerColumn*)sCandExpr)->mOrgExpr;    \
                    continue;                                                   \
                case QLNC_EXPR_TYPE_OUTER_COLUMN:                               \
                    sCandExpr = ((qlncExprOuterColumn*)sCandExpr)->mOrgExpr;    \
                    continue;                                                   \
                case QLNC_EXPR_TYPE_FUNCTION:                                   \
                case QLNC_EXPR_TYPE_LIST_FUNCTION:                              \
                    sIsFunction = STL_TRUE;                                     \
                    break;                                                      \
                default:                                                        \
                    break;                                                      \
            }                                                                   \
                                                                                \
            break;                                                              \
        }                                                                       \
    }

#define QLNC_TRACE_SET_ROW_ARG_STRING( _aArgExpr )                              \
    {                                                                           \
        QLNC_TRACE_IS_ROW_EXPR_FUNCTION( (_aArgExpr) );                         \
        if( sIsFunction == STL_TRUE )                                           \
        {                                                                       \
            STL_TRY( qlncTraceAppendStringToStringBlock(                        \
                         aStringBlock,                                          \
                         aEnv,                                                  \
                         "( " )                                                 \
                     == STL_SUCCESS );                                          \
                                                                                \
            STL_TRY( qlncTraceTreeFromCandExprFuncList[ (_aArgExpr)->mType ](   \
                         aStringBlock,                                          \
                         (_aArgExpr),                                           \
                         aSubQueryExprList,                                     \
                         aEnv )                                                 \
                     == STL_SUCCESS );                                          \
                                                                                \
            STL_TRY( qlncTraceAppendStringToStringBlock(                        \
                         aStringBlock,                                          \
                         aEnv,                                                  \
                         " )" )                                                 \
                     == STL_SUCCESS );                                          \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            STL_TRY( qlncTraceTreeFromCandExprFuncList[ (_aArgExpr)->mType ](   \
                         aStringBlock,                                          \
                         (_aArgExpr),                                           \
                         aSubQueryExprList,                                     \
                         aEnv )                                                 \
                     == STL_SUCCESS );                                          \
        }                                                                       \
    }


    /* Row Expression 설정 */
    sCandRowExpr = (qlncExprRowExpr*)aCandExpr;

    if( sCandRowExpr->mArgCount > 1 )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "( " )
                 == STL_SUCCESS );
    }

    QLNC_TRACE_SET_ROW_ARG_STRING( sCandRowExpr->mArgs[0] );

    for( i = 1; i < sCandRowExpr->mArgCount; i++ )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     ", " )
                 == STL_SUCCESS );

        QLNC_TRACE_SET_ROW_ARG_STRING( sCandRowExpr->mArgs[i] );
    }

    if( sCandRowExpr->mArgCount > 1 )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     " )" )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief And Filter Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprAndFilter( qlncStringBlock           * aStringBlock,
                                              qlncExprCommon            * aCandExpr,
                                              qlncTraceSubQueryExprList * aSubQueryExprList,
                                              qllEnv                    * aEnv )
{
    stlInt32          i;
    qlncAndFilter   * sAndFilter    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_AND_FILTER, QLL_ERROR_STACK(aEnv) );


    /* And Filter 설정 */
    sAndFilter = (qlncAndFilter*)aCandExpr;

    if( sAndFilter->mOrCount == 1 )
    {
        STL_TRY( qlncTraceTreeFromCandExprOrFilter( aStringBlock,
                                                    (qlncExprCommon*)(sAndFilter->mOrFilters[0]),
                                                    aSubQueryExprList,
                                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        for( i = 0; i < sAndFilter->mOrCount; i++ )
        {
            if( i > 0 )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " AND " )
                         == STL_SUCCESS );
            }

            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "( " )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCandExprOrFilter( aStringBlock,
                                                        (qlncExprCommon*)(sAndFilter->mOrFilters[i]),
                                                        aSubQueryExprList,
                                                        aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         " )" )
                     == STL_SUCCESS );
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Or Filter Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprOrFilter( qlncStringBlock            * aStringBlock,
                                             qlncExprCommon             * aCandExpr,
                                             qlncTraceSubQueryExprList  * aSubQueryExprList,
                                             qllEnv                     * aEnv )
{
    stlInt32          i;
    qlncOrFilter    * sOrFilter     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_OR_FILTER, QLL_ERROR_STACK(aEnv) );


    /* Or Filter 설정 */
    sOrFilter = (qlncOrFilter*)aCandExpr;

    if( sOrFilter->mFilterCount == 1 )
    {
        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sOrFilter->mFilters[0]->mType ](
                     aStringBlock,
                     sOrFilter->mFilters[0],
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        for( i = 0; i < sOrFilter->mFilterCount; i++ )
        {
            if( i > 0 )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " OR " )
                         == STL_SUCCESS );
            }

            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "( " )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCandExprFuncList[ sOrFilter->mFilters[i]->mType ](
                         aStringBlock,
                         sOrFilter->mFilters[i],
                         aSubQueryExprList,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         " )" )
                     == STL_SUCCESS );
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Boolean Filter Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprBooleanFilter( qlncStringBlock           * aStringBlock,
                                                  qlncExprCommon            * aCandExpr,
                                                  qlncTraceSubQueryExprList * aSubQueryExprList,
                                                  qllEnv                    * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_BOOLEAN_FILTER, QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlncTraceTreeFromCandExprFuncList[ ((qlncBooleanFilter*)aCandExpr)->mExpr->mType ](
                 aStringBlock,
                 ((qlncBooleanFilter*)aCandExpr)->mExpr,
                 aSubQueryExprList,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Const Boolean Filter Candidate Expression로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandExpr           Candidate Expression
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandExprConstBooleanFilter( qlncStringBlock              * aStringBlock,
                                                       qlncExprCommon               * aCandExpr,
                                                       qlncTraceSubQueryExprList    * aSubQueryExprList,
                                                       qllEnv                       * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandExpr->mType == QLNC_EXPR_TYPE_CONST_BOOLEAN_FILTER,
                        QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlncTraceTreeFromCandExprFuncList[ ((qlncConstBooleanFilter*)aCandExpr)->mExpr->mType ](
                 aStringBlock,
                 ((qlncConstBooleanFilter*)aCandExpr)->mExpr,
                 aSubQueryExprList,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */
