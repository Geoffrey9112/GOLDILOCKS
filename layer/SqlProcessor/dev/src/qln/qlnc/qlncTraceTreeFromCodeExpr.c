/*******************************************************************************
 * qlncTraceTreeFromCodeExpr.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlncTraceTreeFromCodeExpr.c 9556 2013-09-09 10:32:50Z bsyou $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file qlncTraceTreeFromCodeExpr.c
 * @brief SQL Processor Layer Optimizer Trace Tree From Code Expression
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Trace Tree From Code Expr Function List
 */
const qlncTraceTreeFromCodeExprFuncPtr qlncTraceTreeFromCodeExprFuncList[ QLV_EXPR_TYPE_MAX ] =
{
    qlncTraceTreeFromCodeExpr_Value,                /**< QLV_EXPR_TYPE_VALUE */
    qlncTraceTreeFromCodeExpr_BindParam,            /**< QLV_EXPR_TYPE_BIND_PARAM */
    qlncTraceTreeFromCodeExpr_Column,               /**< QLV_EXPR_TYPE_COLUMN */
    qlncTraceTreeFromCodeExpr_Function,             /**< QLV_EXPR_TYPE_FUNCTION */
    qlncTraceTreeFromCodeExpr_Cast,                 /**< QLV_EXPR_TYPE_CAST */
    qlncTraceTreeFromCodeExpr_PseudoColumn,         /**< QLV_EXPR_TYPE_PSEUDO_COLUMN */
    qlncTraceTreeFromCodeExpr_PseudoArgument,       /**< QLV_EXPR_TYPE_PSEUDO_ARGUMENT */
    qlncTraceTreeFromCodeExpr_ConstantExprResult,   /**< QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT */
    qlncTraceTreeFromCodeExpr_Reference,            /**< QLV_EXPR_TYPE_REFERENCE */
    qlncTraceTreeFromCodeExpr_SubQuery,             /**< QLV_EXPR_TYPE_SUB_QUERY */
    qlncTraceTreeFromCodeExpr_InnerColumn,          /**< QLV_EXPR_TYPE_INNER_COLUMN */
    qlncTraceTreeFromCodeExpr_OuterColumn,          /**< QLV_EXPR_TYPE_OUTER_COLUMN */
    qlncTraceTreeFromCodeExpr_RowidColumn,          /**< QLV_EXPR_TYPE_ROWID_COLUMN */
    qlncTraceTreeFromCodeExpr_Aggregation,          /**< QLV_EXPR_TYPE_AGGREGATION */
    qlncTraceTreeFromCodeExpr_CaseExpr,             /**< QLV_EXPR_TYPE_CASE_EXPR */
    qlncTraceTreeFromCodeExpr_ListFunction,         /**< QLV_EXPR_TYPE_LIST_FUNCTION */
    qlncTraceTreeFromCodeExpr_ListColumn,           /**< QLV_EXPR_TYPE_LIST_COLUMN */
    qlncTraceTreeFromCodeExpr_RowExpr               /**< QLV_EXPR_TYPE_ROW_EXPR */
};


/**
 * @brief Value Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_Value( qlncStringBlock      * aStringBlock,
                                           qlvInitExpression    * aExpr,
                                           stlInt32               aLevel,
                                           qllEnv               * aEnv )
{
    qlvInitValue    * sValue    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_VALUE, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    /* Value 설정 */
    sValue = aExpr->mExpr.mValue;

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " => " )
             == STL_SUCCESS );

    if( sValue->mInternalBindIdx != QLV_INTERNAL_BIND_VARIABLE_IDX_NA )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "(bind) " )
                     == STL_SUCCESS );
    }

    switch( sValue->mValueType )
    {
        case QLV_VALUE_TYPE_BOOLEAN:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: BOOL VAL: %s",
                                                         (sValue->mData.mInteger == STL_TRUE
                                                          ? DTL_BOOLEAN_STRING_TRUE
                                                          : DTL_BOOLEAN_STRING_FALSE) )
                     == STL_SUCCESS );
            break;

        case QLV_VALUE_TYPE_NUMERIC:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: NUM VAL: %s",
                                                         sValue->mData.mString )
                     == STL_SUCCESS );
            break;

        case QLV_VALUE_TYPE_BINARY_DOUBLE:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: DBL VAL: %s",
                                                         sValue->mData.mString )
                     == STL_SUCCESS );
            break;

        case QLV_VALUE_TYPE_BINARY_REAL:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: REL VAL: %s",
                                                         sValue->mData.mString )
                     == STL_SUCCESS );
            break;

        case QLV_VALUE_TYPE_BINARY_INTEGER:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: INT VAL: %ld",
                                                         sValue->mData.mInteger )
                     == STL_SUCCESS );
            break;

        case QLV_VALUE_TYPE_STRING_FIXED:
        case QLV_VALUE_TYPE_STRING_VARYING:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: STR VAL: \'%s\'",
                                                         sValue->mData.mString )
                     == STL_SUCCESS );
            break;

        case QLV_VALUE_TYPE_BITSTRING_FIXED:
        case QLV_VALUE_TYPE_BITSTRING_VARYING:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: BITSTR VAL: \'%s\'",
                                                         sValue->mData.mString )
                     == STL_SUCCESS );
            break;

        case QLV_VALUE_TYPE_DATE_LITERAL_STRING:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: DATE VAL: \'%s\'",
                                                         sValue->mData.mString )
                     == STL_SUCCESS );
            break;

        case QLV_VALUE_TYPE_TIME_LITERAL_STRING:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: TIME VAL: \'%s\'",
                                                         sValue->mData.mString )
                     == STL_SUCCESS );
            break;

        case QLV_VALUE_TYPE_TIME_TZ_LITERAL_STRING:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: TIME_TZ VAL: \'%s\'",
                                                         sValue->mData.mString )
                     == STL_SUCCESS );
            break;
            
        case QLV_VALUE_TYPE_TIMESTAMP_LITERAL_STRING:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: TIMESTAMP VAL: \'%s\'",
                                                         sValue->mData.mString )
                     == STL_SUCCESS );
            break;

        case QLV_VALUE_TYPE_TIMESTAMP_TZ_LITERAL_STRING:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: TIMESTAMP_TZ VAL: \'%s\'",
                                                         sValue->mData.mString )
                     == STL_SUCCESS );
            break;

        case QLV_VALUE_TYPE_INTERVAL_YEAR_TO_MONTH_STRING:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: INTERVAL YM VAL: \'%s\'",
                                                         sValue->mData.mString )
                     == STL_SUCCESS );
            break;

        case QLV_VALUE_TYPE_INTERVAL_DAY_TO_SECOND_STRING:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: INTERVAL DS VAL: \'%s\'",
                                                         sValue->mData.mString )
                     == STL_SUCCESS );
            break;

        case QLV_VALUE_TYPE_ROWID_STRING:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: ROWID VAL: \'%s\'",
                                                         sValue->mData.mString )
                     == STL_SUCCESS );
            break;

        case QLV_VALUE_TYPE_NULL:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "VALTYP: NULL VAL: NULL" )
                     == STL_SUCCESS );
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    /* Value */


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Bind Param Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_BindParam( qlncStringBlock      * aStringBlock,
                                               qlvInitExpression    * aExpr,
                                               stlInt32               aLevel,
                                               qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_BIND_PARAM, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " => " )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "HN: %s IDX: %d INDNAM: %s",
                                                 (aExpr->mExpr.mBindParam->mHostName == NULL
                                                  ? "?"
                                                  : aExpr->mExpr.mBindParam->mHostName),
                                                 aExpr->mExpr.mBindParam->mBindParamIdx,
                                                 (aExpr->mExpr.mBindParam->mIndicatorName == NULL
                                                  ? "NULL"
                                                  : aExpr->mExpr.mBindParam->mIndicatorName) )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Column Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_Column( qlncStringBlock     * aStringBlock,
                                            qlvInitExpression   * aExpr,
                                            stlInt32              aLevel,
                                            qllEnv              * aEnv )
{
    qlvInitColumn   * sColumn   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_COLUMN, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    /* Column 설정 */
    sColumn = aExpr->mExpr.mColumn;

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " => " )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "RELPTR: 0x%p RELTYP: %s COLNAM: %s COLHDLPTR: 0x%p",
                                                 sColumn->mRelationNode,
                                                 qlncInitNodeTypeName[sColumn->mRelationNode->mType],
                                                 ellGetColumnName( sColumn->mColumnHandle ),
                                                 sColumn->mColumnHandle )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Function Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_Function( qlncStringBlock   * aStringBlock,
                                              qlvInitExpression * aExpr,
                                              stlInt32            aLevel,
                                              qllEnv            * aEnv )
{
    stlInt32              i;
    qlvInitFunction     * sFunction = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_FUNCTION, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    /* Function 설정 */
    sFunction = aExpr->mExpr.mFunction;

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " => " )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "FUNC: %s ARGCNG: %d",
                                                 QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sFunction->mFuncId ),
                                                 sFunction->mArgCount )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    for( i = 0; i < sFunction->mArgCount; i++ )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s<ARG-%03d>\n",
                                                     qlncTraceTreePrefix[aLevel+1],
                                                     i )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sFunction->mArgs[i]->mType ](
                     aStringBlock,
                     sFunction->mArgs[i],
                     aLevel + 2,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Cast Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_Cast( qlncStringBlock   * aStringBlock,
                                          qlvInitExpression * aExpr,
                                          stlInt32            aLevel,
                                          qllEnv            * aEnv )
{
    qlvInitTypeCast * sTypeCast = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_CAST, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    /* Cast 설정 */
    sTypeCast = aExpr->mExpr.mTypeCast;

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " => " )
             == STL_SUCCESS );

    switch( sTypeCast->mTypeDef.mDBType )
    {
        case DTL_TYPE_BOOLEAN:
        case DTL_TYPE_NATIVE_SMALLINT:
        case DTL_TYPE_NATIVE_INTEGER:
        case DTL_TYPE_NATIVE_BIGINT:
        case DTL_TYPE_NATIVE_REAL:
        case DTL_TYPE_NATIVE_DOUBLE:
        case DTL_TYPE_ROWID:
        case DTL_TYPE_DATE:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "DSTTYP: %s",
                         QLNC_TRACE_GET_DATA_TYPE_NAME( sTypeCast->mTypeDef.mDBType ) )
                     == STL_SUCCESS );
            break;

        case DTL_TYPE_FLOAT:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "DSTTYP: %s(%ld)",
                         QLNC_TRACE_GET_DATA_TYPE_NAME( sTypeCast->mTypeDef.mDBType ),
                         sTypeCast->mTypeDef.mArgNum1 )
                     == STL_SUCCESS );
            
            break;
            
        case DTL_TYPE_NUMBER:
            if( sTypeCast->mTypeDef.mArgNum2 == DTL_SCALE_NA )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             "DSTTYP: %s",
                             QLNC_TRACE_GET_DATA_TYPE_NAME( sTypeCast->mTypeDef.mDBType ) )
                         == STL_SUCCESS );
            }
            else
            {
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             "DSTTYP: %s(%ld, %s)",
                             QLNC_TRACE_GET_DATA_TYPE_NAME( sTypeCast->mTypeDef.mDBType ),
                             sTypeCast->mTypeDef.mArgNum1,
                             QLNC_TRACE_GET_STRING_LENGTH_UNIT_NAME( sTypeCast->mTypeDef.mStringLengthUnit ) )
                         == STL_SUCCESS );
            }
            break;

        case DTL_TYPE_CHAR:
        case DTL_TYPE_VARCHAR:
        case DTL_TYPE_LONGVARCHAR:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "DSTTYP: %s(%ld, %s)",
                         QLNC_TRACE_GET_DATA_TYPE_NAME( sTypeCast->mTypeDef.mDBType ),
                         sTypeCast->mTypeDef.mArgNum1,
                         QLNC_TRACE_GET_STRING_LENGTH_UNIT_NAME( sTypeCast->mTypeDef.mStringLengthUnit ) )
                     == STL_SUCCESS );
            break;

        case DTL_TYPE_BINARY:
        case DTL_TYPE_VARBINARY:
        case DTL_TYPE_LONGVARBINARY:
        case DTL_TYPE_TIME:
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
        case DTL_TYPE_TIMESTAMP:
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "DSTTYP: %s(%ld)",
                         QLNC_TRACE_GET_DATA_TYPE_NAME( sTypeCast->mTypeDef.mDBType ),
                         sTypeCast->mTypeDef.mArgNum1 )
                     == STL_SUCCESS );
            break;

        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "DSTTYP: %s(%s)",
                         QLNC_TRACE_GET_DATA_TYPE_NAME( sTypeCast->mTypeDef.mDBType ),
                         QLNC_TRACE_GET_INTERVAL_INDICATOR_NAME( sTypeCast->mTypeDef.mIntervalIndicator ) )
                     == STL_SUCCESS );
            break;

        case DTL_TYPE_CLOB:
        case DTL_TYPE_BLOB:
        case DTL_TYPE_DECIMAL:
            STL_DASSERT( 0 );
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%s<ARG-000>\n",
                                                 qlncTraceTreePrefix[aLevel+1] )
             == STL_SUCCESS );

    STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sTypeCast->mArgs[0]->mType ](
                 aStringBlock,
                 sTypeCast->mArgs[0],
                 aLevel + 2,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Pseudo Column Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_PseudoColumn( qlncStringBlock   * aStringBlock,
                                                  qlvInitExpression * aExpr,
                                                  stlInt32            aLevel,
                                                  qllEnv            * aEnv )
{
    qlvInitPseudoCol    * sPseudoCol    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_PSEUDO_COLUMN, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    /* Pseudo Column 설정 */
    sPseudoCol = aExpr->mExpr.mPseudoCol;

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " => " )
             == STL_SUCCESS );

    if( sPseudoCol->mArgCount > 0 )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                     aEnv,
                     "PSENAM: %s(%s)",
                     gPseudoColInfoArr[ sPseudoCol->mPseudoId ].mName,
                     ( sPseudoCol->mSeqDictHandle == NULL
                       ? "NONE"
                       : ellGetSequenceName( sPseudoCol->mSeqDictHandle ) ) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                     aEnv,
                     "PSENAM: %s()",
                     gPseudoColInfoArr[ sPseudoCol->mPseudoId ].mName )
                 == STL_SUCCESS );
    }

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Pseudo Argument Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_PseudoArgument( qlncStringBlock     * aStringBlock,
                                                    qlvInitExpression   * aExpr,
                                                    stlInt32              aLevel,
                                                    qllEnv              * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_PSEUDO_ARGUMENT, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Constant Expr Result Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_ConstantExprResult( qlncStringBlock     * aStringBlock,
                                                        qlvInitExpression   * aExpr,
                                                        stlInt32              aLevel,
                                                        qllEnv              * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT,
                        QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    STL_TRY( qlncTraceTreeFromCodeExprFuncList[ aExpr->mExpr.mConstExpr->mOrgExpr->mType ](
                 aStringBlock,
                 aExpr->mExpr.mConstExpr->mOrgExpr,
                 aLevel + 1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Reference Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_Reference( qlncStringBlock      * aStringBlock,
                                               qlvInitExpression    * aExpr,
                                               stlInt32               aLevel,
                                               qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_REFERENCE, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    STL_TRY( qlncTraceTreeFromCodeExprFuncList[ aExpr->mExpr.mReference->mOrgExpr->mType ](
                 aStringBlock,
                 aExpr->mExpr.mReference->mOrgExpr,
                 aLevel + 1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Query Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_SubQuery( qlncStringBlock   * aStringBlock,
                                              qlvInitExpression * aExpr,
                                              stlInt32            aLevel,
                                              qllEnv            * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_SUB_QUERY, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Inner Column Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_InnerColumn( qlncStringBlock    * aStringBlock,
                                                 qlvInitExpression  * aExpr,
                                                 stlInt32             aLevel,
                                                 qllEnv             * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " => " )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "RELPTR:0x%p RELTYP: %s IDX: %d",
                 aExpr->mExpr.mInnerColumn->mRelationNode,
                 qlncInitNodeTypeName[aExpr->mExpr.mInnerColumn->mRelationNode->mType],
                 *(aExpr->mExpr.mInnerColumn->mIdx) )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    /* Original Expression */
    STL_TRY( qlncTraceTreeFromCodeExprFuncList[ aExpr->mExpr.mInnerColumn->mOrgExpr->mType ](
                 aStringBlock,
                 aExpr->mExpr.mInnerColumn->mOrgExpr,
                 aLevel + 2,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Outer Column Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_OuterColumn( qlncStringBlock    * aStringBlock,
                                                 qlvInitExpression  * aExpr,
                                                 stlInt32             aLevel,
                                                 qllEnv             * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_OUTER_COLUMN, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " => " )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "RELPTR:0x%p RELTYP: %s",
                 aExpr->mExpr.mOuterColumn->mRelationNode,
                 qlncInitNodeTypeName[aExpr->mExpr.mOuterColumn->mRelationNode->mType] )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    STL_TRY( qlncTraceTreeFromCodeExprFuncList[ aExpr->mExpr.mOuterColumn->mOrgExpr->mType ](
                 aStringBlock,
                 aExpr->mExpr.mOuterColumn->mOrgExpr,
                 aLevel + 1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Rowid Column Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_RowidColumn( qlncStringBlock    * aStringBlock,
                                                 qlvInitExpression  * aExpr,
                                                 stlInt32             aLevel,
                                                 qllEnv             * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " => " )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "RELPTR:0x%p RELTYP: %s",
                 aExpr->mExpr.mRowIdColumn->mRelationNode,
                 qlncInitNodeTypeName[aExpr->mExpr.mRowIdColumn->mRelationNode->mType] )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Aggregation Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_Aggregation( qlncStringBlock    * aStringBlock,
                                                 qlvInitExpression  * aExpr,
                                                 stlInt32             aLevel,
                                                 qllEnv             * aEnv )
{
    stlInt32              i;
    qlvInitAggregation  * sAggregation  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_AGGREGATION, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    /* Aggregation 설정 */
    sAggregation = aExpr->mExpr.mAggregation;

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " => " )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "AGGNAM: %s DIST: %c NESTAGG: %c ARGCNT: %d",
                 QLNC_TRACE_GET_AGGREGATION_FUNC_NAME( sAggregation->mAggrId ),
                 (sAggregation->mIsDistinct == STL_TRUE ? 'Y' : 'N'),
                 (sAggregation->mHasNestedAggr == STL_TRUE ? 'Y' : 'N'),
                 sAggregation->mArgCount )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    for( i = 0; i < sAggregation->mArgCount; i++ )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s<ARG-%03d>\n",
                                                     qlncTraceTreePrefix[aLevel+1],
                                                     i )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sAggregation->mArgs[i]->mType ](
                     aStringBlock,
                     sAggregation->mArgs[i],
                     aLevel + 2,
                     aEnv )
                 == STL_SUCCESS );
    }

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%s<AGGR FILTER>\n",
                                                 qlncTraceTreePrefix[aLevel+1] )
             == STL_SUCCESS );

    if( sAggregation->mFilter != NULL )
    {
        STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sAggregation->mFilter->mType ](
                     aStringBlock,
                     sAggregation->mFilter,
                     aLevel + 2,
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%sNULL\n",
                                                     qlncTraceTreePrefix[aLevel+2] )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Case Expr Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_CaseExpr( qlncStringBlock   * aStringBlock,
                                              qlvInitExpression * aExpr,
                                              stlInt32            aLevel,
                                              qllEnv            * aEnv )
{
    stlInt32             i;
    qlvInitCaseExpr    * sCaseExpr = NULL;
    qlvInitFunction    * sFunction = NULL;
    qlvInitExpression  * sExpr = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_CASE_EXPR, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    /* Case Expr 설정 */
    sCaseExpr = aExpr->mExpr.mCaseExpr;

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " => " )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "FUNCNAM: %s",
                                                 QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sCaseExpr->mFuncId ) )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    switch( sCaseExpr->mFuncId )
    {
        case KNL_BUILTIN_FUNC_CASE:
            for( i = 0; i < sCaseExpr->mCount; i++ )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "%s<WHEN-%3d>\n",
                                                             qlncTraceTreePrefix[aLevel+1],
                                                             i )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sCaseExpr->mWhenArr[i]->mType ](
                             aStringBlock,
                             sCaseExpr->mWhenArr[i],
                             aLevel + 2,
                             aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "%s<THEN-%3d>\n",
                                                             qlncTraceTreePrefix[aLevel+1],
                                                             i )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sCaseExpr->mThenArr[i]->mType ](
                             aStringBlock,
                             sCaseExpr->mThenArr[i],
                             aLevel + 2,
                             aEnv )
                         == STL_SUCCESS );
            }

            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s<ELSE>\n",
                                                         qlncTraceTreePrefix[aLevel+1] )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sCaseExpr->mDefResult->mType ](
                         aStringBlock,
                         sCaseExpr->mDefResult,
                         aLevel + 2,
                         aEnv )
                     == STL_SUCCESS );

            break;

        case KNL_BUILTIN_FUNC_NULLIF:
            sExpr = sCaseExpr->mWhenArr[0];
            while( sExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
            {
                sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
            }
            
            STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_FUNCTION );
            
            sFunction = sExpr->mExpr.mFunction;
            
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s<WHEN-%3d>\n",
                                                         qlncTraceTreePrefix[aLevel+1],
                                                         0 )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sFunction->mArgs[0]->mType ](
                         aStringBlock,
                         sFunction->mArgs[0],
                         aLevel + 2,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s<WHEN-%3d>\n",
                                                         qlncTraceTreePrefix[aLevel+1],
                                                         1 )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sFunction->mArgs[1]->mType ](
                         aStringBlock,
                         sFunction->mArgs[1],
                         aLevel + 2,
                         aEnv )
                     == STL_SUCCESS );

            break;

        case KNL_BUILTIN_FUNC_COALESCE:
            for( i = 0; i < sCaseExpr->mCount; i++ )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "%s<THEN-%3d>\n",
                                                             qlncTraceTreePrefix[aLevel+1],
                                                             i )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sCaseExpr->mThenArr[i]->mType ](
                             aStringBlock,
                             sCaseExpr->mThenArr[i],
                             aLevel + 2,
                             aEnv )
                         == STL_SUCCESS );
            }

            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s<DEFAULT>\n",
                                                         qlncTraceTreePrefix[aLevel+1] )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sCaseExpr->mDefResult->mType ](
                         aStringBlock,
                         sCaseExpr->mDefResult,
                         aLevel + 2,
                         aEnv )
                     == STL_SUCCESS );

            break;

        case KNL_BUILTIN_FUNC_NVL:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s<THEN-%3d>\n",
                                                         qlncTraceTreePrefix[aLevel+1],
                                                         0 )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sCaseExpr->mThenArr[0]->mType ](
                         aStringBlock,
                         sCaseExpr->mThenArr[0],
                         aLevel + 2,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s<DEFAULT>\n",
                                                         qlncTraceTreePrefix[aLevel+1] )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sCaseExpr->mDefResult->mType ](
                         aStringBlock,
                         sCaseExpr->mDefResult,
                         aLevel + 2,
                         aEnv )
                     == STL_SUCCESS );

            break;

        case KNL_BUILTIN_FUNC_NVL2:
            sExpr = sCaseExpr->mWhenArr[0];
            while( sExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
            {
                sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
            }
            
            STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_FUNCTION );
            
            sFunction = sExpr->mExpr.mFunction;

            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s<WHEN-%3d>\n",
                                                         qlncTraceTreePrefix[aLevel+1],
                                                         0 )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sFunction->mArgs[0]->mType ](
                         aStringBlock,
                         sFunction->mArgs[0],
                         aLevel + 2,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s<THEN-%3d>\n",
                                                         qlncTraceTreePrefix[aLevel+1],
                                                         0 )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sCaseExpr->mThenArr[0]->mType ](
                         aStringBlock,
                         sCaseExpr->mThenArr[0],
                         aLevel + 2,
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s<DEFAULT>\n",
                                                         qlncTraceTreePrefix[aLevel+1] )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sCaseExpr->mDefResult->mType ](
                         aStringBlock,
                         sCaseExpr->mDefResult,
                         aLevel + 2,
                         aEnv )
                     == STL_SUCCESS );

            break;

        case KNL_BUILTIN_FUNC_DECODE:
            
            sExpr = sCaseExpr->mWhenArr[0];
            while( sExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
            {
                sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
            }
            
            STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_FUNCTION );
            
            sFunction = sExpr->mExpr.mFunction;
            sFunction = sFunction->mArgs[0]->mExpr.mFunction;
            
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s<WHEN-%3d>\n",
                                                         qlncTraceTreePrefix[aLevel+1],
                                                         0 )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sFunction->mArgs[0]->mType ](
                         aStringBlock,
                         sFunction->mArgs[0],
                         aLevel + 2,
                         aEnv )
                     == STL_SUCCESS );
            
            for( i = 0; i < sCaseExpr->mCount; i++ )
            {
                sExpr = sCaseExpr->mWhenArr[i];
                while( sExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
                {
                    sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
                }
                
                STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_FUNCTION );
                
                sFunction = sExpr->mExpr.mFunction;
                sFunction = sFunction->mArgs[0]->mExpr.mFunction;
                
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "%s<WHEN-%3d>\n",
                                                             qlncTraceTreePrefix[aLevel+1],
                                                             i + 1 )
                         == STL_SUCCESS );
                
                STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sFunction->mArgs[1]->mType ](
                             aStringBlock,
                             sFunction->mArgs[1],
                             aLevel + 2,
                             aEnv )
                         == STL_SUCCESS );
                
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "%s<THEN-%3d>\n",
                                                             qlncTraceTreePrefix[aLevel+1],
                                                             i )
                         == STL_SUCCESS );
                
                STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sCaseExpr->mThenArr[i]->mType ](
                             aStringBlock,
                             sCaseExpr->mThenArr[i],
                             aLevel + 2,
                             aEnv )
                         == STL_SUCCESS );
            }
            
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s<DEFAULT>\n",
                                                         qlncTraceTreePrefix[aLevel+1] )
                     == STL_SUCCESS );
            
            STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sCaseExpr->mDefResult->mType ](
                         aStringBlock,
                         sCaseExpr->mDefResult,
                         aLevel + 2,
                         aEnv )
                     == STL_SUCCESS );

            break;

        case KNL_BUILTIN_FUNC_CASE2:
            for( i = 0; i < sCaseExpr->mCount; i++ )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "%s<WHEN-%3d>\n",
                                                             qlncTraceTreePrefix[aLevel+1],
                                                             i )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sCaseExpr->mWhenArr[i]->mType ](
                             aStringBlock,
                             sCaseExpr->mWhenArr[i],
                             aLevel + 2,
                             aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "%s<THEN-%3d>\n",
                                                             qlncTraceTreePrefix[aLevel+1],
                                                             i )
                         == STL_SUCCESS );

                STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sCaseExpr->mThenArr[i]->mType ](
                             aStringBlock,
                             sCaseExpr->mThenArr[i],
                             aLevel + 2,
                             aEnv )
                         == STL_SUCCESS );
            }

            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s<ELSE>\n",
                                                         qlncTraceTreePrefix[aLevel+1] )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sCaseExpr->mDefResult->mType ](
                         aStringBlock,
                         sCaseExpr->mDefResult,
                         aLevel + 2,
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


/**
 * @brief List Function Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_ListFunction( qlncStringBlock   * aStringBlock,
                                                  qlvInitExpression * aExpr,
                                                  stlInt32            aLevel,
                                                  qllEnv            * aEnv )
{
    stlInt32          i;
    qlvInitListFunc * sListFunc = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_LIST_FUNCTION, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    /* List Function 설정 */
    sListFunc = aExpr->mExpr.mListFunc;

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " => " )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "FUNCNAM: %s, LISTFUNCTYP: %s ARGCNG: %d",
                 QLNC_TRACE_GET_BUILTIN_FUNC_NAME( sListFunc->mFuncId ),
                 (sListFunc->mListFuncType == QLV_LIST_FUNC_NESTED_TYPE
                  ? "NESTED" : "CORRELATED"),
                 sListFunc->mArgCount)
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    for( i = 0; i < sListFunc->mArgCount; i++ )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s<LIST FUNC ARG-%03d>\n",
                                                     qlncTraceTreePrefix[aLevel+1],
                                                     i )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sListFunc->mArgs[i]->mType ](
                     aStringBlock,
                     sListFunc->mArgs[i],
                     aLevel + 2,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief List Column Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_ListColumn( qlncStringBlock     * aStringBlock,
                                                qlvInitExpression   * aExpr,
                                                stlInt32              aLevel,
                                                qllEnv              * aEnv )
{
    stlInt32          i;
    qlvInitListCol  * sListCol  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_LIST_COLUMN, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    /* List Column 설정 */
    sListCol = aExpr->mExpr.mListCol;

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " => " )
             == STL_SUCCESS );

    switch( sListCol->mPredicate )
    {
        case DTL_LIST_PREDICATE_RIGHT:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "PRED: RIGHT ARGCNT: %d",
                                                         sListCol->mArgCount )
                     == STL_SUCCESS );
            break;

        case DTL_LIST_PREDICATE_LEFT:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "PRED: LEFT ARGCNT: 1" )
                     == STL_SUCCESS );
            break;

        case DTL_LIST_PREDICATE_FUNCTION:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "PRED: FUNC ARGCNT: %d",
                                                         sListCol->mArgCount )
                     == STL_SUCCESS );
            break;

        case DTL_LIST_PREDICATE_NA:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "PRED: N/A ARGCNT: %d",
                                                         sListCol->mArgCount )
                     == STL_SUCCESS );
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    if( sListCol->mPredicate == DTL_LIST_PREDICATE_LEFT )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s<LIST COL ARG-%03d>\n",
                                                     qlncTraceTreePrefix[aLevel+1],
                                                     0 )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sListCol->mArgs[0]->mType ](
                     aStringBlock,
                     sListCol->mArgs[0],
                     aLevel + 2,
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        for( i = 0; i < sListCol->mArgCount; i++ )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s<LIST COL ARG-%03d>\n",
                                                         qlncTraceTreePrefix[aLevel+1],
                                                         i )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sListCol->mArgs[i]->mType ](
                         aStringBlock,
                         sListCol->mArgs[i],
                         aLevel + 2,
                         aEnv )
                     == STL_SUCCESS );
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Row Expr Code Expression으로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aExpr           Init Expression
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceTreeFromCodeExpr_RowExpr( qlncStringBlock    * aStringBlock,
                                             qlvInitExpression  * aExpr,
                                             stlInt32             aLevel,
                                             qllEnv             * aEnv )
{
    stlInt32          i;
    qlvInitRowExpr  * sRowExpr  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr->mType == QLV_EXPR_TYPE_ROW_EXPR, QLL_ERROR_STACK(aEnv) );


    /* Code Expr Info */
    QLNC_TRACE_INIT_EXPR_DEFAULT_INFO( aStringBlock, aExpr, aLevel, aEnv );

    /* Row Expr 설정 */
    sRowExpr = aExpr->mExpr.mRowExpr;

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 " => " )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "ARGCNT: %d",
                                                 sRowExpr->mArgCount )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    if( sRowExpr->mSubQueryExpr != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s<SUB QUERY>\n",
                                                     qlncTraceTreePrefix[aLevel+1] )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sRowExpr->mSubQueryExpr->mType ](
                     aStringBlock,
                     sRowExpr->mSubQueryExpr,
                     aLevel + 2,
                     aEnv )
                 == STL_SUCCESS );
    }

    for( i = 0; i < sRowExpr->mArgCount; i++ )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s<ROW EXPR ARG-%03d>\n",
                                                     qlncTraceTreePrefix[aLevel+1],
                                                     i )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceTreeFromCodeExprFuncList[ sRowExpr->mArgs[i]->mType ](
                     aStringBlock,
                     sRowExpr->mArgs[i],
                     aLevel + 2,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */
