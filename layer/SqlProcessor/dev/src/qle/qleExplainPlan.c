/*******************************************************************************
 * qleExplainPlan.c
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
 * @file qleExplainPlan.c
 * @brief SQL Processor Layer Explain Plan
 */

#include <qll.h>

#include <qlDef.h>



/**
 * @addtogroup qleExplainPlan
 * @{
 */


/**
 * @brief Sort Key Expression List을 explain하기 위한 Comma string을 구성한다.
 * @param[in]       aRegionMem      Region Memory
 * @param[in,out]   aExplainText    Explain Text
 * @param[in]       aRefExprList    Reference Expression List
 * @param[in]       aKeyColFlags    Key Column Flags
 * @param[in]       aIsVerbose      자세한 정보 출력 여부
 * @param[in]       aEnv            Environment
 */
stlStatus qleSetExplainSortExprList( knlRegionMem   * aRegionMem,
                                     qllExplainText * aExplainText,
                                     qlvRefExprList * aRefExprList,
                                     stlUInt8       * aKeyColFlags,
                                     stlBool          aIsVerbose,
                                     qllEnv         * aEnv )
{
    qlvRefExprItem  * sListItem = NULL;
    stlBool           sIsFirst  = STL_TRUE;
    
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExplainText != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );


    sListItem = aRefExprList->mHead;
    while( sListItem != NULL )
    {
        if( sIsFirst == STL_FALSE )
        {
            STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                                   aExplainText,
                                                   aEnv,
                                                   "\", \"" )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                                   aExplainText,
                                                   aEnv,
                                                   "\"" )
                     == STL_SUCCESS );

            sIsFirst = STL_FALSE;
        }

        STL_TRY( qleSetExplainExpr( aRegionMem,
                                    aExplainText,
                                    sListItem->mExprPtr,
                                    STL_FALSE,
                                    aIsVerbose,
                                    aEnv )
                 == STL_SUCCESS );

        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               aExplainText,
                                               aEnv,
                                               " %s %s",
                                               ( DTL_GET_KEY_FLAG_SORT_ORDER( *aKeyColFlags )
                                                 == DTL_KEYCOLUMN_INDEX_ORDER_ASC
                                                 ? "ASC" : "DESC" ),
                                               ( DTL_GET_KEY_FLAG_NULL_ORDER( *aKeyColFlags )
                                                 == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST
                                                 ? "NULLS FIRST" : "NULLS LAST" ) )
                 == STL_SUCCESS );

        aKeyColFlags++;
        sListItem = sListItem->mNext;
    }

    STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                           aExplainText,
                                           aEnv,
                                           "\"\0" )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expression List을 explain하기 위한 string을 구성한다.
 * @param[in]       aRegionMem      Region Memory
 * @param[in,out]   aExplainText    Explain Text
 * @param[in]       aRefExprList    Reference Expression List
 * @param[in]       aSepString      Expression 간의 구분을 위한 출력 문자열
 * @param[in]       aIsVerbose      자세한 정보 출력 여부
 * @param[in]       aEnv            Environment
 */
stlStatus qleSetExplainExprList( knlRegionMem   * aRegionMem,
                                 qllExplainText * aExplainText,
                                 qlvRefExprList * aRefExprList,
                                 stlChar        * aSepString,
                                 stlBool          aIsVerbose,
                                 qllEnv         * aEnv )
{
    qlvRefExprItem  * sListItem = NULL;
    
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExplainText != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSepString != NULL, QLL_ERROR_STACK(aEnv) );


    sListItem = aRefExprList->mHead;
    if( sListItem != NULL )
    {
        STL_TRY( qleSetExplainExpr( aRegionMem,
                                    aExplainText,
                                    sListItem->mExprPtr,
                                    STL_FALSE,
                                    aIsVerbose,
                                    aEnv )
                 == STL_SUCCESS );

        sListItem = sListItem->mNext;
    }

    while( sListItem != NULL )
    {
        /* Seperator 추가 */
        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               aExplainText,
                                               aEnv,
                                               aSepString )
                 == STL_SUCCESS );

        STL_TRY( qleSetExplainExpr( aRegionMem,
                                    aExplainText,
                                    sListItem->mExprPtr,
                                    STL_FALSE,
                                    aIsVerbose,
                                    aEnv )
                 == STL_SUCCESS );

        sListItem = sListItem->mNext;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Reference Expression을 explain하기 위한 string을 구성한다.
 * @param[in]       aRegionMem      Region Memory
 * @param[in,out]   aExplainText    Explain Text
 * @param[in]       aExpr           Expression
 * @param[in]       aCalledByFunc   Function/List Function 등에 의해 호출되었는지 여부
 * @param[in]       aIsVerbose      자세한 정보 출력 여부
 * @param[in]       aEnv            Environment
 */
stlStatus qleSetExplainExpr( knlRegionMem       * aRegionMem,
                             qllExplainText     * aExplainText,
                             qlvInitExpression  * aExpr,
                             stlBool              aCalledByFunc,
                             stlBool              aIsVerbose,
                             qllEnv             * aEnv )
{
    qlvInitExpression   * sExpr;
    qlvInitValue        * sValue;
    qlvInitColumn       * sColumn;
    qlvInitTypeCast     * sTypeCast;
    qlvInitPseudoCol    * sPseudoCol;
    qlvInitAggregation  * sAggr;
    qlvInitListCol      * sListCol;
    stlInt32              sCount   = 0;

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExplainText != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );


    sExpr = aExpr;
    while( STL_TRUE )
    {
        switch( sExpr->mType )
        {
            case QLV_EXPR_TYPE_VALUE :
                {
                    sValue = sExpr->mExpr.mValue;
                    if( sValue->mInternalBindIdx != QLV_INTERNAL_BIND_VARIABLE_IDX_NA )
                    {
                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     "?" )
                                 == STL_SUCCESS );
                        break;
                    }
                    
                    switch( sValue->mValueType )
                    {
                        case QLV_VALUE_TYPE_BOOLEAN :
                            {
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             (sValue->mData.mInteger == STL_TRUE
                                               ? DTL_BOOLEAN_STRING_TRUE
                                               : DTL_BOOLEAN_STRING_FALSE) )
                                         == STL_SUCCESS );
                                break;
                            }
                        case QLV_VALUE_TYPE_NUMERIC :
                        case QLV_VALUE_TYPE_BINARY_DOUBLE :
                        case QLV_VALUE_TYPE_BINARY_REAL :
                            {
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             sValue->mData.mString )
                                         == STL_SUCCESS );
                                break;
                            }
                        case QLV_VALUE_TYPE_BINARY_INTEGER :
                            {
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             "%ld",
                                             sValue->mData.mInteger )
                                         == STL_SUCCESS );
                                break;
                            }
                        case QLV_VALUE_TYPE_NULL :
                            {
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             "NULL" )
                                         == STL_SUCCESS );
                                break;
                            }
                        case QLV_VALUE_TYPE_DATE_LITERAL_STRING :
                            {
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             "DATE" )
                                         == STL_SUCCESS );
                                
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             "\'%s\'",
                                             sValue->mData.mString )
                                         == STL_SUCCESS );
                                break;
                            }
                        case QLV_VALUE_TYPE_TIME_LITERAL_STRING :
                            {
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             "TIME" )
                                         == STL_SUCCESS );
                                
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             "\'%s\'",
                                             sValue->mData.mString )
                                         == STL_SUCCESS );
                                break;
                            }
                        case QLV_VALUE_TYPE_TIME_TZ_LITERAL_STRING :
                            {
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             "TIME WITH TIME ZONE" ) 
                                         == STL_SUCCESS );
                                
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             "\'%s\'",
                                             sValue->mData.mString )
                                         == STL_SUCCESS );
                                break;
                            }
                        case QLV_VALUE_TYPE_TIMESTAMP_LITERAL_STRING :
                            {
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             "TIMESTAMP" )
                                         == STL_SUCCESS );
                                
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             "\'%s\'",
                                             sValue->mData.mString )
                                         == STL_SUCCESS );
                                break;
                            }                                    
                        case QLV_VALUE_TYPE_TIMESTAMP_TZ_LITERAL_STRING :
                            {
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             "TIMESTAMP WITH TIME ZONE" )
                                         == STL_SUCCESS );
                                
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             "\'%s\'",
                                             sValue->mData.mString )
                                         == STL_SUCCESS );
                                break;
                            }                                    
                        default :
                            {
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             "\'%s\'",
                                             sValue->mData.mString )
                                         == STL_SUCCESS );
                                break;
                            }
                    }
                    break;
                }
                
            case QLV_EXPR_TYPE_BIND_PARAM :
                {
                    stlInt32      sLen;
                    stlChar     * sHostName = NULL;

                    if( sExpr->mExpr.mBindParam->mHostName == NULL )
                    {
                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     "?" )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        sLen = stlStrlen( sExpr->mExpr.mBindParam->mHostName );
                        STL_TRY( knlAllocRegionMem( aRegionMem,
                                                    sLen + 1,
                                                    (void**) &sHostName,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        stlMemcpy( sHostName, sExpr->mExpr.mBindParam->mHostName, sLen );
                        sHostName[sLen] = '\0';

                        stlToupperString( sHostName, sHostName );

                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     ":%s",
                                     sHostName )
                                 == STL_SUCCESS );
                    }
                    
                    break;
                }
                
            case QLV_EXPR_TYPE_COLUMN :
                {
                    sColumn = sExpr->mExpr.mColumn;

                    if( aIsVerbose == STL_FALSE )
                    {
                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     "%s",
                                     sExpr->mColumnName )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     "%s.%s",
                                     ((qlvInitBaseTableNode *)sColumn->mRelationNode)->mRelationName->mTable,
                                     sExpr->mColumnName )
                                 == STL_SUCCESS );
                    }
                    break;
                }
                
            case QLV_EXPR_TYPE_FUNCTION :
                {
                    STL_TRY( qleSetExplainFuncExpr( aRegionMem,
                                                    aExplainText,
                                                    sExpr,
                                                    aCalledByFunc,
                                                    aIsVerbose,
                                                    aEnv )
                             == STL_SUCCESS );

                    break;
                }
                
            case QLV_EXPR_TYPE_CAST :
                {
                    sTypeCast = sExpr->mExpr.mTypeCast;

                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 "CAST( " )
                             == STL_SUCCESS );
                    
                    STL_TRY( qleSetExplainExpr( aRegionMem,
                                                aExplainText,
                                                sTypeCast->mArgs[ 0 ],
                                                STL_FALSE,
                                                aIsVerbose,
                                                aEnv )
                             == STL_SUCCESS );

                    switch( sTypeCast->mTypeDef.mDBType )
                    {
                        case DTL_TYPE_BOOLEAN :
                        case DTL_TYPE_NATIVE_SMALLINT :
                        case DTL_TYPE_NATIVE_INTEGER :
                        case DTL_TYPE_NATIVE_BIGINT :
                        case DTL_TYPE_NATIVE_REAL :
                        case DTL_TYPE_NATIVE_DOUBLE :
                        case DTL_TYPE_ROWID :
                        case DTL_TYPE_DATE :
                            {
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             " AS %s )",
                                             dtlDataTypeName[ sTypeCast->mTypeDef.mDBType ] )
                                         == STL_SUCCESS );
                                break;
                            }
                            
                        case DTL_TYPE_FLOAT :
                            {
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             " AS %s(%ld) )",
                                             dtlDataTypeName[ sTypeCast->mTypeDef.mDBType ],
                                             sTypeCast->mTypeDef.mArgNum1 )
                                         == STL_SUCCESS );
                                break;
                            }                                
                            
                        case DTL_TYPE_NUMBER :
                            {
                                if( sTypeCast->mTypeDef.mArgNum2 == DTL_SCALE_NA )
                                {
                                    STL_TRY( qleAppendStringToExplainText(
                                                 aRegionMem,
                                                 aExplainText,
                                                 aEnv,
                                                 " AS %s )",
                                                 dtlDataTypeName[ sTypeCast->mTypeDef.mDBType ] )
                                             == STL_SUCCESS );
                                }
                                else
                                {
                                    STL_TRY( qleAppendStringToExplainText(
                                                 aRegionMem,
                                                 aExplainText,
                                                 aEnv,
                                                 " AS %s(%ld, %ld) )",
                                                 dtlDataTypeName[ sTypeCast->mTypeDef.mDBType ],
                                                 sTypeCast->mTypeDef.mArgNum1,
                                                 sTypeCast->mTypeDef.mArgNum2 )
                                             == STL_SUCCESS );
                                }
                                break;
                            }
                            
                        case DTL_TYPE_CHAR :
                        case DTL_TYPE_VARCHAR :
                        case DTL_TYPE_LONGVARCHAR :
                            {
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             " AS %s(%ld, %s) )",
                                             dtlDataTypeName[ sTypeCast->mTypeDef.mDBType ],
                                             sTypeCast->mTypeDef.mArgNum1,
                                             gDtlLengthUnitString[ sTypeCast->mTypeDef.mStringLengthUnit ] )
                                         == STL_SUCCESS );
                                break;
                            }

                        case DTL_TYPE_BINARY :
                        case DTL_TYPE_VARBINARY :
                        case DTL_TYPE_LONGVARBINARY :
                        case DTL_TYPE_TIME :
                        case DTL_TYPE_TIME_WITH_TIME_ZONE :
                        case DTL_TYPE_TIMESTAMP :
                        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE :
                            {
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             " AS %s(%ld) )",
                                             dtlDataTypeName[ sTypeCast->mTypeDef.mDBType ],
                                             sTypeCast->mTypeDef.mArgNum1 )
                                         == STL_SUCCESS );
                                break;
                            }

                        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH :
                        case DTL_TYPE_INTERVAL_DAY_TO_SECOND :
                            {
                                STL_TRY( qleAppendStringToExplainText(
                                             aRegionMem,
                                             aExplainText,
                                             aEnv,
                                             " AS %s(%s) )",
                                             dtlDataTypeName[ sTypeCast->mTypeDef.mDBType ],
                                             gDtlIntervalIndicatorString[ sTypeCast->mTypeDef.mIntervalIndicator ] )
                                         == STL_SUCCESS );
                                break;
                            }
                            
                        case DTL_TYPE_CLOB :
                        case DTL_TYPE_BLOB :
                        case DTL_TYPE_DECIMAL :
                            {
                                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                                break;
                            }
                        default :
                            {
                                STL_DASSERT( 0 );
                                break;
                            }
                    }

                    break;
                }

            case QLV_EXPR_TYPE_PSEUDO_COLUMN :
                {
                    sPseudoCol = sExpr->mExpr.mPseudoCol;

                    if( sPseudoCol->mArgCount > 0 )
                    {
                        STL_DASSERT( ( sPseudoCol->mPseudoId == KNL_PSEUDO_COL_CURRVAL ) ||
                                     ( sPseudoCol->mPseudoId == KNL_PSEUDO_COL_NEXTVAL ) );

                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     "%s(%s)",
                                     gPseudoColInfoArr[ sPseudoCol->mPseudoId ].mName,
                                     ellGetSequenceName( sPseudoCol->mSeqDictHandle ) )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     "%s()",
                                     gPseudoColInfoArr[ sPseudoCol->mPseudoId ].mName )
                                 == STL_SUCCESS );
                    }

                    break;
                }

            case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
                {
                    /* Do Nothing */
                    break;
                }

            case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
                {
                    sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
                    continue;
                }

            case QLV_EXPR_TYPE_REFERENCE :
                {
                    sExpr = sExpr->mExpr.mReference->mOrgExpr;
                    continue;
                }

            case QLV_EXPR_TYPE_SUB_QUERY :
                {
                    STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                    break;
                }

            case QLV_EXPR_TYPE_INNER_COLUMN :
                {
                    switch( sExpr->mExpr.mInnerColumn->mRelationNode->mType )
                    {
                        case QLV_NODE_TYPE_QUERY_SET :
                        case QLV_NODE_TYPE_QUERY_SPEC :
                        case QLV_NODE_TYPE_BASE_TABLE :
                        case QLV_NODE_TYPE_JOIN_TABLE :
                        case QLV_NODE_TYPE_FLAT_INSTANT :
                        case QLV_NODE_TYPE_SORT_INSTANT :
                        case QLV_NODE_TYPE_HASH_INSTANT :
                        case QLV_NODE_TYPE_GROUP_HASH_INSTANT :
                        case QLV_NODE_TYPE_GROUP_SORT_INSTANT :
                        case QLV_NODE_TYPE_SORT_JOIN_INSTANT :
                        case QLV_NODE_TYPE_HASH_JOIN_INSTANT :
                        case QLV_NODE_TYPE_SUB_TABLE :
                            {
                                sExpr = sExpr->mExpr.mInnerColumn->mOrgExpr;
                                break;
                            }                            
                        default :
                            {
                                STL_DASSERT( 0 );
                                break;
                            }
                    }
                    
                    continue;
                }
                
            case QLV_EXPR_TYPE_OUTER_COLUMN :
                {
                    if ( sExpr->mColumnName != NULL )
                    {
                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     "{%s}",
                                     sExpr->mColumnName )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     "?" )
                                 == STL_SUCCESS );
                    }
                    break;
                }
                
            case QLV_EXPR_TYPE_ROWID_COLUMN :
                {
                    if( aIsVerbose == STL_FALSE )
                    {
                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     "ROWID" )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     "%s.ROWID",
                                     ((qlvInitBaseTableNode *)sExpr->mExpr.mRowIdColumn->mRelationNode)->mRelationName->mTable )
                                 == STL_SUCCESS );
                    }

                    break;
                }

            case QLV_EXPR_TYPE_AGGREGATION :
                {
                    sAggr = sExpr->mExpr.mAggregation;

                    if( sAggr->mAggrId == KNL_BUILTIN_AGGREGATION_COUNT_ASTERISK )
                    {
                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     "%s",
                                     qlneAggrFuncName[ sAggr->mAggrId ] )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        if( sAggr->mIsDistinct == STL_TRUE )
                        {
                            STL_TRY( qleAppendStringToExplainText(
                                         aRegionMem,
                                         aExplainText,
                                         aEnv,
                                         "%s( DISTINCT ",
                                         qlneAggrFuncName[ sAggr->mAggrId ] )
                                     == STL_SUCCESS );
                        }
                        else
                        {
                            STL_TRY( qleAppendStringToExplainText(
                                         aRegionMem,
                                         aExplainText,
                                         aEnv,
                                         "%s( ",
                                         qlneAggrFuncName[ sAggr->mAggrId ] )
                                     == STL_SUCCESS );
                        }
                        
                        STL_TRY( qleSetExplainExpr( aRegionMem,
                                                    aExplainText,
                                                    sAggr->mArgs[ 0 ],
                                                    STL_FALSE,
                                                    aIsVerbose,
                                                    aEnv )
                                 == STL_SUCCESS );

                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     " )" )
                                 == STL_SUCCESS );
                    }
                    
                    break;
                }
                
            case QLV_EXPR_TYPE_CASE_EXPR :
                {
                    STL_TRY( qleSetExplainCaseExpr( aRegionMem,
                                                    aExplainText,
                                                    sExpr,
                                                    aIsVerbose,
                                                    aEnv )
                             == STL_SUCCESS );
                    
                    break;
                }
            case QLV_EXPR_TYPE_LIST_FUNCTION :
                {
                    STL_TRY( qleSetExplainListFuncExpr( aRegionMem,
                                                        aExplainText,
                                                        sExpr,
                                                        aCalledByFunc,
                                                        aIsVerbose,
                                                        aEnv )
                             == STL_SUCCESS );
                    
                    break;
                }
            case QLV_EXPR_TYPE_LIST_COLUMN :
                {
                    sListCol = sExpr->mExpr.mListCol;

                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 "( " )
                             == STL_SUCCESS );

                    STL_TRY( qleSetExplainExpr( aRegionMem,
                                                aExplainText,
                                                sListCol->mArgs[ 0 ],
                                                STL_FALSE,
                                                aIsVerbose,
                                                aEnv )
                             == STL_SUCCESS );
                        
                       
                    if( sListCol->mPredicate == DTL_LIST_PREDICATE_LEFT )
                    {
                        /**
                         *  List Column Target 절에서는
                         *  List Column List만큼 Row수를 증가시키기 때문에
                         *  첫번재 argument만 출력한다.
                         *  ( Row 단위로 복사함. )
                         *  */
                    }
                    else
                    {
                        for( sCount = 1 ; sCount < sListCol->mArgCount ; sCount ++ )
                        {
                            STL_TRY( qleAppendStringToExplainText(
                                         aRegionMem,
                                         aExplainText,
                                         aEnv,
                                         ", " )
                                     == STL_SUCCESS );
                        
                            STL_TRY( qleSetExplainExpr( aRegionMem,
                                                        aExplainText,
                                                        sListCol->mArgs[ sCount ],
                                                        STL_FALSE,
                                                        aIsVerbose,
                                                        aEnv )
                                     == STL_SUCCESS );
                        }
                    }

                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 " )" )
                             == STL_SUCCESS );

                    break;
                }
            case QLV_EXPR_TYPE_ROW_EXPR :
                {
                    STL_TRY( qleSetExplainRowExpr( aRegionMem,
                                                   aExplainText,
                                                   sExpr,
                                                   aIsVerbose,
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

        break;
    }


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK( aEnv ),
                      "qleSetExplainExpr()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Function Expression을 explain하기 위한 string을 구성한다.
 * @param[in]       aRegionMem      Region Memory
 * @param[in,out]   aExplainText    Explain Text
 * @param[in]       aFuncExpr       Expression
 * @param[in]       aCalledByFunc   Function/List Function 등에 의해 호출되었는지 여부
 * @param[in]       aIsVerbose      자세한 정보 출력 여부
 * @param[in]       aEnv            Environment
 */
stlStatus qleSetExplainFuncExpr( knlRegionMem       * aRegionMem,
                                 qllExplainText     * aExplainText,
                                 qlvInitExpression  * aFuncExpr,
                                 stlBool              aCalledByFunc,
                                 stlBool              aIsVerbose,
                                 qllEnv             * aEnv )
{
    qlvInitExpression  * sExpr = NULL;
    qlvInitFunction    * sFunction;
    stlInt32             sLoop = 0;
    stlInt32             i;
    

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExplainText != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFuncExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFuncExpr->mType == QLV_EXPR_TYPE_FUNCTION, QLL_ERROR_STACK(aEnv) );


    /**
     * Function 기준 순회 방법에 따른 explain plan 구성
     */
    
    sFunction = aFuncExpr->mExpr.mFunction;
    switch( sFunction->mFuncId )
    {
        /* 전위 순회형 expression */
        case KNL_BUILTIN_FUNC_NOT :
            {
                STL_DASSERT( sFunction->mArgCount == 1 );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             "%s ",
                             qlneBuiltInFuncName[ sFunction->mFuncId ] )
                         == STL_SUCCESS );
                
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[0],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                break;
            }
        case KNL_BUILTIN_FUNC_POSITIVE :
        case KNL_BUILTIN_FUNC_NEGATIVE :
            {
                STL_DASSERT( sFunction->mArgCount == 1 );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             "%s",
                             qlneBuiltInFuncName[ sFunction->mFuncId ] )
                         == STL_SUCCESS );
                
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[0],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                break;
            }
        /* 후위 순회형 expression */
        case KNL_BUILTIN_FUNC_IS_NULL :
        case KNL_BUILTIN_FUNC_IS_NOT_NULL :
        case KNL_BUILTIN_FUNC_IS_UNKNOWN :
        case KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN :
            {
                STL_DASSERT( sFunction->mArgCount == 1 );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[0],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " %s",
                             qlneBuiltInFuncName[ sFunction->mFuncId ] )
                         == STL_SUCCESS );

                break;
            }
        /* 중위 순회형 expression */
        case KNL_BUILTIN_FUNC_IS_EQUAL :
        case KNL_BUILTIN_FUNC_IS_NOT_EQUAL :
        case KNL_BUILTIN_FUNC_IS_LESS_THAN :
        case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :
        case KNL_BUILTIN_FUNC_IS_GREATER_THAN :
        case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :
        case KNL_BUILTIN_FUNC_IS :
        case KNL_BUILTIN_FUNC_IS_NOT :
        case KNL_BUILTIN_FUNC_ADD :
        case KNL_BUILTIN_FUNC_SUB :
        case KNL_BUILTIN_FUNC_MUL :
        case KNL_BUILTIN_FUNC_DIV :
        case KNL_BUILTIN_FUNC_BITWISE_AND :
        case KNL_BUILTIN_FUNC_BITWISE_OR :
        case KNL_BUILTIN_FUNC_BITWISE_XOR :
        case KNL_BUILTIN_FUNC_BITWISE_SHIFTLEFT :
        case KNL_BUILTIN_FUNC_BITWISE_SHIFTRIGHT :
        case KNL_BUILTIN_FUNC_CONCATENATE :
            {
                STL_DASSERT( sFunction->mArgCount == 2 );

                if( aCalledByFunc == STL_TRUE )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 "( " )
                             == STL_SUCCESS );
                }

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[0],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " %s ",
                             qlneBuiltInFuncName[ sFunction->mFuncId ] )
                         == STL_SUCCESS );
                
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[1],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                if( aCalledByFunc == STL_TRUE )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 " )" )
                             == STL_SUCCESS );
                }

                break;
            }
        /* A and B and C ..., A or B or C ...*/
        case KNL_BUILTIN_FUNC_AND :
        case KNL_BUILTIN_FUNC_OR :
            {
                STL_DASSERT( sFunction->mArgCount >= 2 );

                if( aCalledByFunc == STL_TRUE )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 "( " )
                             == STL_SUCCESS );
                }

                for( i = 0; i < sFunction->mArgCount; i++ )
                {
                    STL_TRY( qleSetExplainExpr( aRegionMem,
                                                aExplainText,
                                                sFunction->mArgs[i],
                                                STL_FALSE,
                                                aIsVerbose,
                                                aEnv )
                             == STL_SUCCESS );

                    if( i != sFunction->mArgCount - 1 )
                    {
                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     " %s ",
                                     qlneBuiltInFuncName[ sFunction->mFuncId ] )
                                 == STL_SUCCESS );
                    }
                }

                if( aCalledByFunc == STL_TRUE )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 " )" )
                             == STL_SUCCESS );
                }

                break;
            }
        /* between A and B */
        case KNL_BUILTIN_FUNC_BETWEEN :
        case KNL_BUILTIN_FUNC_NOT_BETWEEN :
            {
                STL_DASSERT( sFunction->mArgCount == 3 );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[0],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " %s ",
                             qlneBuiltInFuncName[ sFunction->mFuncId ] )
                         == STL_SUCCESS );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[1],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " AND " )
                         == STL_SUCCESS );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[2],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                break;
            }
        case KNL_BUILTIN_FUNC_BETWEEN_SYMMETRIC :
        case KNL_BUILTIN_FUNC_NOT_BETWEEN_SYMMETRIC :
            {
                /* symmetric은 A between symmetric b and c 시
                 * A가 b 와 c 각각 cast를 붙여줘야 하므로
                 * A.1 A.2를 갖게 된다.
                 * 그러므로 argument는 3 이지만 실질적 값은 +1 인 4이다.
                 */ 
                STL_DASSERT( sFunction->mArgCount == 3 + 1 );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[0],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " %s ",
                             qlneBuiltInFuncName[ sFunction->mFuncId ] )
                         == STL_SUCCESS );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[2],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " AND " )
                         == STL_SUCCESS );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[3],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                break;
            }
        case KNL_BUILTIN_FUNC_LIKE :
        case KNL_BUILTIN_FUNC_NOT_LIKE :
            {
                STL_DASSERT( ( sFunction->mArgCount == 2 ) ||
                             ( sFunction->mArgCount == 3 ) );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[0],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " %s ",
                             qlneBuiltInFuncName[ sFunction->mFuncId ] )
                         == STL_SUCCESS );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[1],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                if( sFunction->mArgCount == 3 )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 " ESCAPE " )
                             == STL_SUCCESS );

                    STL_TRY( qleSetExplainExpr( aRegionMem,
                                                aExplainText,
                                                sFunction->mArgs[2],
                                                STL_TRUE,
                                                aIsVerbose,
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /* Do Nothing */
                }

                break;
            }
        case KNL_BUILTIN_FUNC_DATE_PART :            
        case KNL_BUILTIN_FUNC_EXTRACT :
            {
                STL_DASSERT( sFunction->mArgCount == 2 );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             "%s( ",
                             qlneBuiltInFuncName[ sFunction->mFuncId ] )
                         == STL_SUCCESS );

                sExpr = sFunction->mArgs[0];
                while( sExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
                {
                    sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
                }

                STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_VALUE );
                                        
                if( stlStrncmp( sExpr->mExpr.mValue->mData.mString,
                                "1",
                                1 ) == 0 )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 "YEAR" )
                             == STL_SUCCESS );
                }
                else if( stlStrncmp( sExpr->mExpr.mValue->mData.mString,
                                     "2",
                                     1 ) == 0 )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 "MONTH" )
                             == STL_SUCCESS );
                }
                else if( stlStrncmp( sExpr->mExpr.mValue->mData.mString,
                                     "3",
                                     1 ) == 0 )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 "DAY" )
                             == STL_SUCCESS );
                }
                else if( stlStrncmp( sExpr->mExpr.mValue->mData.mString,
                                     "4",
                                     1 ) == 0 )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 "HOUR" )
                             == STL_SUCCESS );
                }
                else if( stlStrncmp( sExpr->mExpr.mValue->mData.mString,
                                     "5",
                                     1 ) == 0 )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 "MINUTE" )
                             == STL_SUCCESS );
                }
                else if( stlStrncmp( sExpr->mExpr.mValue->mData.mString,
                                     "6",
                                     1 ) == 0 )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 "SECOND" )
                             == STL_SUCCESS );
                }
                else if( stlStrncmp( sExpr->mExpr.mValue->mData.mString,
                                     "7",
                                     1 ) == 0 )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 "TIMEZONE_HOUR" )
                             == STL_SUCCESS );
                }
                else if( stlStrncmp( sExpr->mExpr.mValue->mData.mString,
                                     "8",
                                     1 ) == 0 )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 "TIMEZONE_MINUTE" )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_DASSERT( 0 );
                }
                    
                if( sFunction->mFuncId == KNL_BUILTIN_FUNC_EXTRACT )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 " FROM " )
                             == STL_SUCCESS );
                }
                /* KNL_BUILTIN_FUNC_DATE_PART */
                else 
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 " , " )
                             == STL_SUCCESS );
                }
                
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[1],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );
 
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " )" )
                         == STL_SUCCESS );
                
                break;
            }
        case KNL_BUILTIN_FUNC_DATEADD :
        case KNL_BUILTIN_FUNC_DATEDIFF :            
            {
                STL_DASSERT( sFunction->mArgCount == 3 );
                
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             "%s( ",
                             qlneBuiltInFuncName[ sFunction->mFuncId ] )
                         == STL_SUCCESS );
                
                sExpr = sFunction->mArgs[0];
                while( sExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
                {
                    sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
                }
                
                STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_VALUE );
                
                if( sExpr->mExpr.mValue->mData.mString[1] == '\0' )
                {
                    i = sExpr->mExpr.mValue->mData.mString[0] - '0';
                }
                else
                {
                    i = ( sExpr->mExpr.mValue->mData.mString[0] - '0' ) * 10;
                    i = i + ( sExpr->mExpr.mValue->mData.mString[1] - '0' );
                    STL_DASSERT( sExpr->mExpr.mValue->mData.mString[2] == '\0' );
                }

                STL_DASSERT( i < DTL_DATETIME_PART_MAX );
                
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             dtlDatePartName[i] )
                         == STL_SUCCESS );                
                
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " , " )
                         == STL_SUCCESS );
                
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[1],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );
                
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " , " )
                         == STL_SUCCESS );
                
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[2],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );
                
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " )" )
                         == STL_SUCCESS );
                
                break;
            }
        case KNL_BUILTIN_FUNC_TO_CHAR :
        case KNL_BUILTIN_FUNC_TO_DATE :
        case KNL_BUILTIN_FUNC_TO_TIME :
        case KNL_BUILTIN_FUNC_TO_TIME_WITH_TIME_ZONE :
        case KNL_BUILTIN_FUNC_TO_TIMESTAMP :
        case KNL_BUILTIN_FUNC_TO_TIMESTAMP_WITH_TIME_ZONE :
        case KNL_BUILTIN_FUNC_TO_NUMBER :
        case KNL_BUILTIN_FUNC_TO_NATIVE_REAL :
        case KNL_BUILTIN_FUNC_TO_NATIVE_DOUBLE :
            {
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             "%s(",
                             qlneBuiltInFuncName[ sFunction->mFuncId ] )
                         == STL_SUCCESS );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[ 0 ],
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                if( sFunction->mArgCount > 1 )
                {
                    sExpr = sFunction->mArgs[ 1 ];
                    
                    while( sExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
                    {
                        sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
                    }
                    
                    STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_FUNCTION );
                    
                    sFunction = sExpr->mExpr.mFunction;
                    
                    if( ( gBuiltInFuncInfoArr[sFunction->mFuncId].mArgumentCntMin !=
                          gBuiltInFuncInfoArr[sFunction->mFuncId].mArgumentCntMax ) &&
                        ( sFunction->mArgCount == gBuiltInFuncInfoArr[sFunction->mFuncId].mArgumentCntMin ) )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     "," )
                                 == STL_SUCCESS );
                        
                        STL_TRY( qleSetExplainExpr(
                                     aRegionMem,
                                     aExplainText,
                                     sFunction->mArgs[ gBuiltInFuncInfoArr[sFunction->mFuncId].mArgumentCntMax - 1 ],
                                     STL_FALSE,
                                     aIsVerbose,
                                     aEnv )
                                 == STL_SUCCESS );
                    }
                }
                
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             ")" )
                         == STL_SUCCESS );
                
                break;
            }
        case KNL_BUILTIN_FUNC_LIKE_PATTERN :
            {
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             "%s",
                             qlneBuiltInFuncName[ sFunction->mFuncId ] )
                         == STL_SUCCESS );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[ 0 ],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );
                
                break;
            }
        case KNL_BUILTIN_FUNC_ASSIGN :
            {
                STL_DASSERT( sFunction->mArgCount == 2 );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[0],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );
                break;
            }
        case KNL_BUILTIN_FUNC_TRIM :
            {
                STL_DASSERT( ( sFunction->mArgCount >=
                               gBuiltInFuncInfoArr[ sFunction->mFuncId ].mArgumentCntMin ) &&
                             ( sFunction->mArgCount <=
                               gBuiltInFuncInfoArr[ sFunction->mFuncId ].mArgumentCntMax ) );
                
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             "%s( ",
                             qlneBuiltInFuncName[ sFunction->mFuncId ] )
                         == STL_SUCCESS );
                
                sExpr = sFunction->mArgs[0];
                while( sExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
                {
                    sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
                }
                
                STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_VALUE );
                
                i = sExpr->mExpr.mValue->mData.mString[0] - '0';

                STL_DASSERT( i < DTL_TRIM_SPEC_MAX );

                /* trim specification string */
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             dtlTrimSpecificationStr[i] )
                         == STL_SUCCESS );

                /* trim character */
                if( sFunction->mArgCount == 3 )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 " " )
                             == STL_SUCCESS );
                    
                    STL_TRY( qleSetExplainExpr( aRegionMem,
                                                aExplainText,
                                                sFunction->mArgs[2],
                                                STL_TRUE,
                                                aIsVerbose,
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /* Do Nothing */
                }
                
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " FROM " )
                         == STL_SUCCESS );
                
                /* trim_source */
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[1],
                                            STL_TRUE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );
                
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " )" )
                         == STL_SUCCESS );
                
                break;
            }
        /* 열거형 expression */
        default :
            {
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             "%s(",
                             qlneBuiltInFuncName[ sFunction->mFuncId ] )
                         == STL_SUCCESS );

                for( sLoop = 0 ; sLoop < sFunction->mArgCount ; sLoop++ )
                {
                    if( sLoop > 0 )
                    {
                        STL_TRY( qleAppendStringToExplainText(
                                     aRegionMem,
                                     aExplainText,
                                     aEnv,
                                     "," )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        /* Do Nothing */
                    }

                    STL_TRY( qleSetExplainExpr( aRegionMem,
                                                aExplainText,
                                                sFunction->mArgs[ sLoop ],
                                                STL_FALSE,
                                                aIsVerbose,
                                                aEnv )
                             == STL_SUCCESS );
                }
 
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
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
 * @brief CASE Expression을 explain하기 위한 string을 구성한다.
 * @param[in]       aRegionMem      Region Memory
 * @param[in,out]   aExplainText    Explain Text
 * @param[in]       aInitCaseExpr   Expression
 * @param[in]       aIsVerbose      자세한 정보 출력 여부
 * @param[in]       aEnv            Environment
 */
stlStatus qleSetExplainCaseExpr( knlRegionMem       * aRegionMem,
                                 qllExplainText     * aExplainText,
                                 qlvInitExpression  * aInitCaseExpr,
                                 stlBool              aIsVerbose,
                                 qllEnv             * aEnv )
{
    qlvInitCaseExpr    * sCaseExpr = NULL;
    qlvInitFunction    * sFunction = NULL;
    qlvInitExpression  * sExpr = NULL;
    stlInt32             sCnt = 0;

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExplainText != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitCaseExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitCaseExpr->mType == QLV_EXPR_TYPE_CASE_EXPR, QLL_ERROR_STACK(aEnv) );


    sCaseExpr = aInitCaseExpr->mExpr.mCaseExpr;

    switch( sCaseExpr->mFuncId )
    {
        case KNL_BUILTIN_FUNC_CASE :
            {
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             qlneBuiltInFuncName[ sCaseExpr->mFuncId ] )
                         == STL_SUCCESS );
                
                for( sCnt = 0; sCnt < sCaseExpr->mCount; sCnt++ )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 " WHEN " )
                             == STL_SUCCESS );
                    
                    STL_TRY( qleSetExplainExpr( aRegionMem,
                                                aExplainText,
                                                sCaseExpr->mWhenArr[sCnt],
                                                STL_FALSE,
                                                aIsVerbose,
                                                aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 " THEN " )
                             == STL_SUCCESS );

                    STL_TRY( qleSetExplainExpr( aRegionMem,
                                                aExplainText,
                                                sCaseExpr->mThenArr[sCnt],
                                                STL_FALSE,
                                                aIsVerbose,
                                                aEnv )
                             == STL_SUCCESS );
                }  
 
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " ELSE " )
                         == STL_SUCCESS );
                
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sCaseExpr->mDefResult,
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " END " )
                         == STL_SUCCESS );
                
                break;
            }
        case KNL_BUILTIN_FUNC_NULLIF :
            {
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             "%s( ",
                             qlneBuiltInFuncName[ sCaseExpr->mFuncId ] )
                         == STL_SUCCESS );
                
                sExpr = sCaseExpr->mWhenArr[0];
                while( sExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
                {
                    sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
                }

                STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_FUNCTION );
                
                sFunction = sExpr->mExpr.mFunction;

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[0],
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             ", " )
                         == STL_SUCCESS );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[1],
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " )" )
                         == STL_SUCCESS );
                
                break;
            }
        case KNL_BUILTIN_FUNC_COALESCE :
            {
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             "%s( ",
                             qlneBuiltInFuncName[ sCaseExpr->mFuncId ] )
                         == STL_SUCCESS );
                
                for( sCnt = 0; sCnt < sCaseExpr->mCount; sCnt++ )
                {
                    STL_TRY( qleSetExplainExpr( aRegionMem,
                                                aExplainText,
                                                sCaseExpr->mThenArr[sCnt],
                                                STL_FALSE,
                                                aIsVerbose,
                                                aEnv )
                             == STL_SUCCESS );
 
                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 ", " )
                             == STL_SUCCESS );
                }
                
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sCaseExpr->mDefResult,
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " )" )
                         == STL_SUCCESS );
                
                break;
            }
        case KNL_BUILTIN_FUNC_NVL :
            {
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             "%s( ",
                             qlneBuiltInFuncName[ sCaseExpr->mFuncId ] )
                         == STL_SUCCESS );
                
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sCaseExpr->mThenArr[0],
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             ", " )
                         == STL_SUCCESS );
                
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sCaseExpr->mDefResult,
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " )" )
                         == STL_SUCCESS );
                
                break;
            }
        case KNL_BUILTIN_FUNC_NVL2 :
            {
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             "%s( ",
                             qlneBuiltInFuncName[ sCaseExpr->mFuncId ] )
                         == STL_SUCCESS );

                sExpr = sCaseExpr->mWhenArr[0];
                while( sExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
                {
                    sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
                }
                
                STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_FUNCTION );
                
                sFunction = sExpr->mExpr.mFunction;
                
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[0],
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             ", " )
                         == STL_SUCCESS );
                
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sCaseExpr->mThenArr[0],
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             ", " )
                         == STL_SUCCESS );
                
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sCaseExpr->mDefResult,
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " )" )
                         == STL_SUCCESS );
                
                break;
            }
        case KNL_BUILTIN_FUNC_DECODE :
            {
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             "%s( ",
                             qlneBuiltInFuncName[ sCaseExpr->mFuncId ] )
                         == STL_SUCCESS );
                
                sExpr = sCaseExpr->mWhenArr[0];
                while( sExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
                {
                    sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
                }
                
                STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_FUNCTION );
                
                sFunction = sExpr->mExpr.mFunction;
                sFunction = sFunction->mArgs[0]->mExpr.mFunction;
                
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sFunction->mArgs[0],
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );
                
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             ", " )
                         == STL_SUCCESS );

                for( sCnt = 0; sCnt < sCaseExpr->mCount; sCnt++ )
                {
                    sExpr = sCaseExpr->mWhenArr[sCnt];
                    while( sExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
                    {
                        sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
                    }
                    
                    STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_FUNCTION );
                    
                    sFunction = sExpr->mExpr.mFunction;
                    sFunction = sFunction->mArgs[0]->mExpr.mFunction;

                    STL_TRY( qleSetExplainExpr( aRegionMem,
                                                aExplainText,
                                                sFunction->mArgs[1],
                                                STL_FALSE,
                                                aIsVerbose,
                                                aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 ", " )
                             == STL_SUCCESS );
                    
                    STL_TRY( qleSetExplainExpr( aRegionMem,
                                                aExplainText,
                                                sCaseExpr->mThenArr[sCnt],
                                                STL_FALSE,
                                                aIsVerbose,
                                                aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 ", " )
                             == STL_SUCCESS );
                }
                
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sCaseExpr->mDefResult,
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " )" )
                         == STL_SUCCESS );
                
                break;
            }
        case KNL_BUILTIN_FUNC_CASE2 :
            {
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             "%s( ",
                             qlneBuiltInFuncName[ sCaseExpr->mFuncId ] )
                         == STL_SUCCESS );

                for( sCnt = 0; sCnt < sCaseExpr->mCount; sCnt++ )
                {
                    STL_TRY( qleSetExplainExpr( aRegionMem,
                                                aExplainText,
                                                sCaseExpr->mWhenArr[sCnt],
                                                STL_FALSE,
                                                aIsVerbose,
                                                aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 ", " )
                             == STL_SUCCESS );

                    STL_TRY( qleSetExplainExpr( aRegionMem,
                                                aExplainText,
                                                sCaseExpr->mThenArr[sCnt],
                                                STL_FALSE,
                                                aIsVerbose,
                                                aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qleAppendStringToExplainText(
                                 aRegionMem,
                                 aExplainText,
                                 aEnv,
                                 ", " )
                             == STL_SUCCESS );
                }

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sCaseExpr->mDefResult,
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );
 
                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " )" )
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
 * @brief LIST FUNCTION Expression을 explain하기 위한 string을 구성한다.
 * @param[in]       aRegionMem      Region Memory
 * @param[in,out]   aExplainText    Explain Text
 * @param[in]       aInitListFunc   Expression
 * @param[in]       aCalledByFunc   Function/List Function 등에 의해 호출되었는지 여부
 * @param[in]       aIsVerbose      자세한 정보 출력 여부
 * @param[in]       aEnv            Environment
 */
stlStatus qleSetExplainListFuncExpr( knlRegionMem       * aRegionMem,
                                     qllExplainText     * aExplainText,
                                     qlvInitExpression  * aInitListFunc,
                                     stlBool              aCalledByFunc,
                                     stlBool              aIsVerbose,
                                     qllEnv             * aEnv )
{
    qlvInitListFunc     * sListFunc   = NULL;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExplainText != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitListFunc->mType == QLV_EXPR_TYPE_LIST_FUNCTION, QLL_ERROR_STACK(aEnv) );


    if( aCalledByFunc == STL_TRUE )
    {
        STL_TRY( qleAppendStringToExplainText(
                     aRegionMem,
                     aExplainText,
                     aEnv,
                     "( " )
                 == STL_SUCCESS );
    }

    sListFunc = aInitListFunc->mExpr.mListFunc;

    switch( sListFunc->mFuncId )
    {
        case KNL_BUILTIN_FUNC_IN :
        case KNL_BUILTIN_FUNC_NOT_IN :
        case KNL_BUILTIN_FUNC_IS_EQUAL_ROW :
        case KNL_BUILTIN_FUNC_IS_NOT_EQUAL_ROW :
        case KNL_BUILTIN_FUNC_IS_LESS_THAN_ROW :
        case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL_ROW :
        case KNL_BUILTIN_FUNC_IS_GREATER_THAN_ROW :
        case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL_ROW :
        case KNL_BUILTIN_FUNC_EQUAL_ANY :
        case KNL_BUILTIN_FUNC_NOT_EQUAL_ANY :
        case KNL_BUILTIN_FUNC_LESS_THAN_ANY :
        case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY :
        case KNL_BUILTIN_FUNC_GREATER_THAN_ANY :
        case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY :
        case KNL_BUILTIN_FUNC_EQUAL_ALL :
        case KNL_BUILTIN_FUNC_NOT_EQUAL_ALL :
        case KNL_BUILTIN_FUNC_LESS_THAN_ALL :
        case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL :
        case KNL_BUILTIN_FUNC_GREATER_THAN_ALL :
        case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL :
            {
                STL_DASSERT( sListFunc->mArgCount == 2 );
 
                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sListFunc->mArgs[1],
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " %s ",
                             qlneBuiltInFuncName[ sListFunc->mFuncId ] )
                         == STL_SUCCESS );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sListFunc->mArgs[0],
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                break;
            }
        case KNL_BUILTIN_FUNC_EXISTS :
        case KNL_BUILTIN_FUNC_NOT_EXISTS :
            {
                STL_DASSERT( sListFunc->mArgCount == 1 );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             "%s( ",
                             qlneBuiltInFuncName[ sListFunc->mFuncId ] )
                         == STL_SUCCESS );

                STL_TRY( qleSetExplainExpr( aRegionMem,
                                            aExplainText,
                                            sListFunc->mArgs[0],
                                            STL_FALSE,
                                            aIsVerbose,
                                            aEnv )
                         == STL_SUCCESS );

                STL_TRY( qleAppendStringToExplainText(
                             aRegionMem,
                             aExplainText,
                             aEnv,
                             " )" )
                         == STL_SUCCESS );

                break;
            }
        default :
            {
                 STL_DASSERT( 0 );
                 break;
            }
    }
 
    if( aCalledByFunc == STL_TRUE )
    {
        STL_TRY( qleAppendStringToExplainText(
                     aRegionMem,
                     aExplainText,
                     aEnv,
                     " )" )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ROW EXPR Expression을 explain하기 위한 string을 구성한다.
 * @param[in]       aRegionMem      Region Memory
 * @param[in,out]   aExplainText    Explain Text
 * @param[in]       aRowExpr        Expression
 * @param[in]       aIsVerbose      자세한 정보 출력 여부
 * @param[in]       aEnv            Environment
 */
stlStatus qleSetExplainRowExpr( knlRegionMem       * aRegionMem,
                                qllExplainText     * aExplainText,
                                qlvInitExpression  * aRowExpr,
                                stlBool              aIsVerbose,
                                qllEnv             * aEnv )
{
    qlvInitExpression   * sExpr       = NULL;
    qlvInitRowExpr      * sRowExpr    = NULL;
    stlInt32              sCount      = 0;
    stlBool               sIsFunction = STL_FALSE;
    
#define QLNE_IS_ROW_EXPR_FUNCTION( aExpr )                                  \
    {                                                                       \
        sIsFunction = STL_FALSE;                                            \
        sExpr = (aExpr);                                                    \
        while( aRowExpr->mRelatedFuncId != KNL_BUILTIN_FUNC_INVALID )       \
        {                                                                   \
            switch( sExpr->mType )                                          \
            {                                                               \
                case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :                   \
                    sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;              \
                    continue;                                               \
                case QLV_EXPR_TYPE_INNER_COLUMN :                           \
                    sExpr = sExpr->mExpr.mInnerColumn->mOrgExpr;            \
                    continue;                                               \
                case QLV_EXPR_TYPE_OUTER_COLUMN :                           \
                    sExpr = sExpr->mExpr.mOuterColumn->mOrgExpr;            \
                    continue;                                               \
                case QLV_EXPR_TYPE_FUNCTION :                               \
                    sIsFunction = STL_TRUE;                                 \
                    break;                                                  \
                case QLV_EXPR_TYPE_LIST_FUNCTION :                          \
                    sIsFunction = STL_TRUE;                                 \
                default :                                                   \
                    break;                                                  \
            }                                                               \
            break;                                                          \
        }                                                                   \
    }


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExplainText != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowExpr->mType == QLV_EXPR_TYPE_ROW_EXPR, QLL_ERROR_STACK(aEnv) );


    sRowExpr = aRowExpr->mExpr.mRowExpr;

    if( sRowExpr->mArgCount > 1 )
    {
        STL_TRY( qleAppendStringToExplainText(
                     aRegionMem,
                     aExplainText,
                     aEnv,
                     "( " )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    QLNE_IS_ROW_EXPR_FUNCTION( sRowExpr->mArgs[0] );
    if( sIsFunction == STL_TRUE )
    {
        STL_TRY( qleAppendStringToExplainText(
                     aRegionMem,
                     aExplainText,
                     aEnv,
                     "( " )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    STL_TRY( qleSetExplainExpr( aRegionMem,
                                aExplainText,
                                sRowExpr->mArgs[ 0 ],
                                STL_FALSE,
                                aIsVerbose,
                                aEnv )
             == STL_SUCCESS );

    if( sIsFunction == STL_TRUE )
    {
        STL_TRY( qleAppendStringToExplainText(
                     aRegionMem,
                     aExplainText,
                     aEnv,
                     " )" )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    for( sCount = 1 ; sCount < sRowExpr->mArgCount ; sCount ++ )
    {
        STL_TRY( qleAppendStringToExplainText(
                     aRegionMem,
                     aExplainText,
                     aEnv,
                     ", " )
                 == STL_SUCCESS );

        QLNE_IS_ROW_EXPR_FUNCTION( sRowExpr->mArgs[sCount] );

        if( sIsFunction == STL_TRUE )
        {
            STL_TRY( qleAppendStringToExplainText(
                         aRegionMem,
                         aExplainText,
                         aEnv,
                         "( " )
                     == STL_SUCCESS );
        }
        else
        {
            /* Do Nothing */
        }

        STL_TRY( qleSetExplainExpr( aRegionMem,
                                    aExplainText,
                                    sRowExpr->mArgs[ sCount ],
                                    STL_FALSE,
                                    aIsVerbose,
                                    aEnv )
                 == STL_SUCCESS );

        if( sIsFunction == STL_TRUE )
        {
            STL_TRY( qleAppendStringToExplainText(
                         aRegionMem,
                         aExplainText,
                         aEnv,
                         " )" )
                     == STL_SUCCESS );
        }
        else
        {
            /* Do Nothing */
        }
    }

    if( sRowExpr->mArgCount > 1 )
    {
        STL_TRY( qleAppendStringToExplainText(
                     aRegionMem,
                     aExplainText,
                     aEnv,
                     " )" )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }
 

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Range Expression을 explain하기 위한 string을 구성한다.
 * @param[in]       aRegionMem      Region Memory
 * @param[in,out]   aExplainText    Explain Text
 * @param[in]       aRangeExprList  Range Expression List
 * @param[in]       aKeyColCnt      Key Column Count
 * @param[in]       aIsVerbose      자세한 정보 출력 여부
 * @param[in]       aEnv            Environment
 */
stlStatus qleSetExplainRange( knlRegionMem      * aRegionMem,
                              qllExplainText    * aExplainText,
                              qlvRefExprList   ** aRangeExprList,
                              stlInt32            aKeyColCnt,
                              stlBool             aIsVerbose,
                              qllEnv            * aEnv )
{
    qlvRefExprItem  * sListItem    = NULL;
    stlBool           sIsFirst     = STL_TRUE;
    stlInt32          sIdx         = 0;
    
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExplainText != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRangeExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyColCnt > 0, QLL_ERROR_STACK(aEnv) );

    
    sIdx = 0;
    while( aKeyColCnt > 0 )
    {
        STL_DASSERT( aRangeExprList[sIdx] != NULL );
        
        if( aRangeExprList[sIdx]->mCount == 0 )
        {
            break;
        }
        else
        {
            /* Do Nothing */
        }
        
        sListItem = aRangeExprList[sIdx]->mHead;
        while( sListItem != NULL )
        {
            if( sIsFirst == STL_FALSE )
            {
                STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                                       aExplainText,
                                                       aEnv,
                                                       " AND " )
                         == STL_SUCCESS );
            }
            else
            {
                sIsFirst = STL_FALSE;
            }

            STL_TRY( qleSetExplainExpr( aRegionMem,
                                        aExplainText,
                                        sListItem->mExprPtr,
                                        STL_FALSE,
                                        aIsVerbose,
                                        aEnv )
                     == STL_SUCCESS );

            sListItem = sListItem->mNext;
        }

        sIdx++;
        aKeyColCnt--;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Statement의 실행 Plan 출력을 구성한다.
 * @param[in]   aTransID    Transaction ID
 * @param[in]   aDBCStmt    DBC Statement 
 * @param[in]   aSQLStmt    SQL Statement 객체
 * @param[out]  aPlanText   Plan Text
 * @param[in]   aEnv        Environment
 * @remarks
 */
stlStatus qleExplainPlan( smlTransId              aTransID,
                          qllDBCStmt            * aDBCStmt,
                          qllStatement          * aSQLStmt,
                          qllExplainNodeText   ** aPlanText,
                          qllEnv                * aEnv )
{
    qllExplainNodeText  * sPlanText         = NULL;
    qllExplainNodeText  * sHeadPlanText     = NULL;
    qllExplainNodeText  * sTailPlanText     = NULL;
    qllOptimizationNode * sCodePlan         = NULL;
    qllDataArea         * sDataArea         = NULL;

    stlInt32              sDescLen;
    stlInt32              sMaxDescLen;
    stlChar             * sTmpBuffer1       = NULL;
    stlChar             * sTmpBuffer2       = NULL;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Explain Plan 설정
     */

    /* Explain Node Text 생성 */
    STL_TRY( qleCreateExplainNodeText( &QLL_DATA_PLAN( aDBCStmt ),
                                       0,
                                       NULL,
                                       &sHeadPlanText,
                                       aEnv )
             == STL_SUCCESS );

    *aPlanText = sHeadPlanText;
    sPlanText = sHeadPlanText;


    /**
     * Plan Text 구성
     */  

    switch( aSQLStmt->mStmtType )
    {
        case QLL_STMT_SELECT_TYPE :
        case QLL_STMT_SELECT_FOR_UPDATE_TYPE :
        case QLL_STMT_SELECT_INTO_TYPE :
        case QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE :
            {
                /* Valid Plan 얻기 */
                sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
                sDataArea = (qllDataArea *) aSQLStmt->mDataPlan;

                /* explain SELECT Optimization Node */
                STL_TRY( qlneExplainNodeList[ sCodePlan->mType ] (
                             sCodePlan,
                             sDataArea,
                             & QLL_DATA_PLAN( aDBCStmt ),
                             0,
                             & sPlanText,
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
            
        case QLL_STMT_DELETE_TYPE :
        case QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE :
        case QLL_STMT_DELETE_RETURNING_QUERY_TYPE :
        case QLL_STMT_DELETE_RETURNING_INTO_TYPE :
            {
                /* Valid Plan 얻기 */
                sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
                sDataArea = (qllDataArea *) aSQLStmt->mDataPlan;

                /* explain DELETE Optimization Node */
                STL_TRY( qlneExplainNodeList[ sCodePlan->mType ] (
                             sCodePlan,
                             sDataArea,
                             & QLL_DATA_PLAN( aDBCStmt ),
                             0,
                             & sPlanText,
                             aEnv )
                         == STL_SUCCESS );

                break;
            }

        case QLL_STMT_UPDATE_TYPE :
        case QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE :
        case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE :
        case QLL_STMT_UPDATE_RETURNING_INTO_TYPE :
            {
                /* Valid Plan 얻기 */
                sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
                sDataArea = (qllDataArea *) aSQLStmt->mDataPlan;

                /* explain UPDATE Optimization Node */
                STL_TRY( qlneExplainNodeList[ sCodePlan->mType ] (
                             sCodePlan,
                             sDataArea,
                             & QLL_DATA_PLAN( aDBCStmt ),
                             0,
                             & sPlanText,
                             aEnv )
                         == STL_SUCCESS );

                break;
            }

        case QLL_STMT_INSERT_TYPE :
            {
                sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
                sDataArea = (qllDataArea *) aSQLStmt->mDataPlan;

                /* explain INSERT Optimization Node */
                STL_TRY( qlneExplainNodeList[ sCodePlan->mType ] (
                             sCodePlan,
                             sDataArea,
                             & QLL_DATA_PLAN( aDBCStmt ),
                             0,
                             & sPlanText,
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLL_STMT_INSERT_SELECT_TYPE :
            {
                /* Valid Plan 얻기 */
                sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
                sDataArea = (qllDataArea *) aSQLStmt->mDataPlan;

                /* explain INSERT Optimization Node */
                STL_TRY( qlneExplainNodeList[ sCodePlan->mType ] (
                             sCodePlan,
                             sDataArea,
                             & QLL_DATA_PLAN( aDBCStmt ),
                             0,
                             & sPlanText,
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLL_STMT_INSERT_RETURNING_INTO_TYPE :
        case QLL_STMT_INSERT_RETURNING_QUERY_TYPE :
            {
                /* Valid Plan 얻기 */
                sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
                sDataArea = (qllDataArea *) aSQLStmt->mDataPlan;
                
                /* explain INSERT Optimization Node */
                STL_TRY( qlneExplainNodeList[ sCodePlan->mType ] (
                             sCodePlan,
                             sDataArea,
                             & QLL_DATA_PLAN( aDBCStmt ),
                             0,
                             & sPlanText,
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        default :
            {
                STL_TRY( qleAppendStringToExplainText( &QLL_DATA_PLAN( aDBCStmt ),
                                                       &(sHeadPlanText->mLineBuffer),
                                                       aEnv,
                                                       "cannot make plan.\n" )
                         == STL_SUCCESS );

                sHeadPlanText->mLineText = sHeadPlanText->mLineBuffer.mText;
                sHeadPlanText->mNext = NULL;

                STL_THROW( RAMP_FINISH );
                break;
            }
    }

    /* Tail Explain Text 설정 */
    STL_TRY( qleCreateExplainNodeText( &QLL_DATA_PLAN( aDBCStmt ),
                                       0,
                                       sPlanText,
                                       &sTailPlanText,
                                       aEnv )
             == STL_SUCCESS );

    /* Node Description 길이 계산 */
    sMaxDescLen = 0;
    sPlanText = sHeadPlanText->mNext;
    while( (sPlanText != NULL) &&
           (sPlanText->mNext != NULL) )
    {
        sDescLen = 
            2/* head space */
            + (sPlanText->mDepth * 2)/* indent space */
            + sPlanText->mDescBuffer.mCurSize/* desc string length */
            + 2;/* tail space */

        if( sMaxDescLen < sDescLen )
        {
            sMaxDescLen = sDescLen;
        }

        sPlanText = sPlanText->mNext;
    }

    if( sMaxDescLen < 62 )
    {
        sMaxDescLen = 62;
    }

    /* Description을 위한 임시 Buffer 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_DATA_PLAN( aDBCStmt ),
                                sMaxDescLen + 1,
                                (void**) &sTmpBuffer1,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_DATA_PLAN( aDBCStmt ),
                                sMaxDescLen + 1,
                                (void**) &sTmpBuffer2,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sTmpBuffer1, '=', sMaxDescLen );
    sTmpBuffer1[sMaxDescLen] = '\0';
    stlMemset( sTmpBuffer2, '-', sMaxDescLen );
    sTmpBuffer2[sMaxDescLen] = '\0';

    /* Head 구성 */
    STL_TRY( qleAppendStringToExplainText( &QLL_DATA_PLAN( aDBCStmt ),
                                           &(sHeadPlanText->mLineBuffer),
                                           aEnv,
                                           "< Execution Plan >\n"
                                           "=========%*s==============\n"
                                           "|  IDX  |%s%*s|       ROWS |\n"
                                           "---------%*s--------------\n",
                                           sMaxDescLen, sTmpBuffer1,
                                           "  NODE DESCRIPTION",
                                           sMaxDescLen - 18, "",
                                           sMaxDescLen, sTmpBuffer2 )
             == STL_SUCCESS );

    /* Node들의 Description 설정 */
    sPlanText = sHeadPlanText->mNext;
    STL_TRY( qleAppendStringToExplainText( &QLL_DATA_PLAN( aDBCStmt ),
                                           &(sPlanText->mLineBuffer),
                                           aEnv,
                                           "| %4d  |  %*s%s%*s|            |\n",
                                           sPlanText->mIdx,
                                           sPlanText->mDepth * 2, "",
                                           sPlanText->mDescBuffer.mText,
                                           (sMaxDescLen - (sPlanText->mDepth * 2) - sPlanText->mDescBuffer.mCurSize - 1), "" )
             == STL_SUCCESS );

    sPlanText->mLineText = sPlanText->mLineBuffer.mText;

    sPlanText = sPlanText->mNext;

    while( (sPlanText != NULL) &&
           (sPlanText->mNext != NULL) )
    {
        STL_TRY( qleAppendStringToExplainText( &QLL_DATA_PLAN( aDBCStmt ),
                                               &(sPlanText->mLineBuffer),
                                               aEnv,
                                               "| %4d  |  %*s%s%*s| %10ld |\n",
                                               sPlanText->mIdx,
                                               sPlanText->mDepth * 2, "",
                                               sPlanText->mDescBuffer.mText,
                                               (sMaxDescLen - (sPlanText->mDepth * 2) - sPlanText->mDescBuffer.mCurSize - 1), "",
                                               sPlanText->mFetchRowCnt )
                 == STL_SUCCESS );

        sPlanText->mLineText = sPlanText->mLineBuffer.mText;

        sPlanText = sPlanText->mNext;
    }

    /* Tail 구성 */
    STL_TRY( qleAppendStringToExplainText( &QLL_DATA_PLAN( aDBCStmt ),
                                           &(sTailPlanText->mLineBuffer),
                                           aEnv,
                                           "=========%*s==============\n\n",
                                           sMaxDescLen, sTmpBuffer1 )
             == STL_SUCCESS );


    STL_RAMP( RAMP_FINISH );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Trace Log용 Statement의 실행 Plan 출력을 구성한다.
 * @param[in]   aTransID        Transaction ID
 * @param[in]   aDBCStmt        DBC Statement 
 * @param[in]   aSQLStmt        SQL Statement 객체
 * @param[in]   aSqlPlanLevel   SQL Plan Level
 * @param[out]  aPlanText       Plan Text
 * @param[in]   aEnv            Environment
 * @remarks
 */
stlStatus qleTraceExplainPlan( smlTransId             aTransID,
                               qllDBCStmt           * aDBCStmt,
                               qllStatement         * aSQLStmt,
                               stlInt32               aSqlPlanLevel,
                               qllExplainNodeText  ** aPlanText,
                               qllEnv               * aEnv )
{
    qllExplainNodeText  * sPlanText         = NULL;
    qllExplainNodeText  * sHeadPlanText     = NULL;
    qllExplainNodeText  * sTailPlanText     = NULL;
    qllOptimizationNode * sCodePlan         = NULL;
    qllDataArea         * sDataArea         = NULL;

    stlInt32              sDescLen;
    stlInt32              sMaxDescLen;
    stlChar             * sTmpBuffer1       = NULL;
    stlChar             * sTmpBuffer2       = NULL;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Explain Plan 설정
     */

    /* Explain Node Text 생성 */
    STL_TRY( qleCreateExplainNodeText( &QLL_DATA_PLAN( aDBCStmt ),
                                       0,
                                       NULL,
                                       &sHeadPlanText,
                                       aEnv )
             == STL_SUCCESS );

    *aPlanText = sHeadPlanText;
    sPlanText = sHeadPlanText;


    /**
     * Plan Text 구성
     */  

    switch( aSQLStmt->mStmtType )
    {
        case QLL_STMT_SELECT_TYPE :
        case QLL_STMT_SELECT_FOR_UPDATE_TYPE :
        case QLL_STMT_SELECT_INTO_TYPE :
        case QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE :
            {
                /* Valid Plan 얻기 */
                sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
                sDataArea = (qllDataArea *) aSQLStmt->mDataPlan;

                /* explain SELECT Optimization Node */
                STL_TRY( qlneExplainNodeList[ sCodePlan->mType ] (
                             sCodePlan,
                             sDataArea,
                             & QLL_DATA_PLAN( aDBCStmt ),
                             0,
                             & sPlanText,
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
            
        case QLL_STMT_DELETE_TYPE :
        case QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE :
        case QLL_STMT_DELETE_RETURNING_QUERY_TYPE :
        case QLL_STMT_DELETE_RETURNING_INTO_TYPE :
            {
                /* Valid Plan 얻기 */
                sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
                sDataArea = (qllDataArea *) aSQLStmt->mDataPlan;

                /* explain DELETE Optimization Node */
                STL_TRY( qlneExplainNodeList[ sCodePlan->mType ] (
                             sCodePlan,
                             sDataArea,
                             & QLL_DATA_PLAN( aDBCStmt ),
                             0,
                             & sPlanText,
                             aEnv )
                         == STL_SUCCESS );

                break;
            }

        case QLL_STMT_UPDATE_TYPE :
        case QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE :
        case QLL_STMT_UPDATE_RETURNING_QUERY_TYPE :
        case QLL_STMT_UPDATE_RETURNING_INTO_TYPE :
            {
                /* Valid Plan 얻기 */
                sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
                sDataArea = (qllDataArea *) aSQLStmt->mDataPlan;

                /* explain UPDATE Optimization Node */
                STL_TRY( qlneExplainNodeList[ sCodePlan->mType ] (
                             sCodePlan,
                             sDataArea,
                             & QLL_DATA_PLAN( aDBCStmt ),
                             0,
                             & sPlanText,
                             aEnv )
                         == STL_SUCCESS );

                break;
            }

        case QLL_STMT_INSERT_TYPE :
            {
                sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
                sDataArea = (qllDataArea *) aSQLStmt->mDataPlan;

                /* explain INSERT Optimization Node */
                STL_TRY( qlneExplainNodeList[ sCodePlan->mType ] (
                             sCodePlan,
                             sDataArea,
                             & QLL_DATA_PLAN( aDBCStmt ),
                             0,
                             & sPlanText,
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLL_STMT_INSERT_SELECT_TYPE :
            {
                /* Valid Plan 얻기 */
                sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
                sDataArea = (qllDataArea *) aSQLStmt->mDataPlan;

                /* explain INSERT Optimization Node */
                STL_TRY( qlneExplainNodeList[ sCodePlan->mType ] (
                             sCodePlan,
                             sDataArea,
                             & QLL_DATA_PLAN( aDBCStmt ),
                             0,
                             & sPlanText,
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLL_STMT_INSERT_RETURNING_INTO_TYPE :
        case QLL_STMT_INSERT_RETURNING_QUERY_TYPE :
            {
                /* Valid Plan 얻기 */
                sCodePlan = (qllOptimizationNode *) aSQLStmt->mCodePlan;
                sDataArea = (qllDataArea *) aSQLStmt->mDataPlan;
                
                /* explain INSERT Optimization Node */
                STL_TRY( qlneExplainNodeList[ sCodePlan->mType ] (
                             sCodePlan,
                             sDataArea,
                             & QLL_DATA_PLAN( aDBCStmt ),
                             0,
                             & sPlanText,
                             aEnv )
                         == STL_SUCCESS );

                break;
            }
        default :
            {
                STL_TRY( qleAppendStringToExplainText( &QLL_DATA_PLAN( aDBCStmt ),
                                                       &(sHeadPlanText->mLineBuffer),
                                                       aEnv,
                                                       "cannot make plan.\n" )
                         == STL_SUCCESS );

                sHeadPlanText->mLineText = sHeadPlanText->mLineBuffer.mText;
                sHeadPlanText->mNext = NULL;

                STL_THROW( RAMP_FINISH );
                break;
            }
    }

    /* Tail Explain Text 설정 */
    STL_TRY( qleCreateExplainNodeText( &QLL_DATA_PLAN( aDBCStmt ),
                                       0,
                                       sPlanText,
                                       &sTailPlanText,
                                       aEnv )
             == STL_SUCCESS );

    /* Node Description 길이 계산 */
    sMaxDescLen = 0;
    sPlanText = sHeadPlanText->mNext;
    while( (sPlanText != NULL) &&
           (sPlanText->mNext != NULL) )
    {
        sDescLen = 
            2/* head space */
            + (sPlanText->mDepth * 2)/* indent space */
            + sPlanText->mDescBuffer.mCurSize/* desc string length */
            + 2;/* tail space */

        if( sMaxDescLen < sDescLen )
        {
            sMaxDescLen = sDescLen;
        }

        sPlanText = sPlanText->mNext;
    }

    if( sMaxDescLen < 62 )
    {
        sMaxDescLen = 62;
    }

    /* Description을 위한 임시 Buffer 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_DATA_PLAN( aDBCStmt ),
                                sMaxDescLen + 1,
                                (void**) &sTmpBuffer1,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_DATA_PLAN( aDBCStmt ),
                                sMaxDescLen + 1,
                                (void**) &sTmpBuffer2,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sTmpBuffer1, '=', sMaxDescLen );
    sTmpBuffer1[sMaxDescLen] = '\0';
    stlMemset( sTmpBuffer2, '-', sMaxDescLen );
    sTmpBuffer2[sMaxDescLen] = '\0';

    if( knlGetPropertyBoolValueByID( KNL_PROPERTY_TRACE_LOG_TIME_DETAIL,
                                     KNL_ENV(aEnv) ) == STL_FALSE )
    {
        switch( aSqlPlanLevel )
        {
            case 1:
            case 2:
                /* Head 구성 */
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sHeadPlanText->mLineBuffer),
                             aEnv,
                             "< Execution Plan >\n"
                             "=========%*s===========================\n"
                             "|  IDX  |%s%*s|       ROWS | Total Time |\n"
                             "---------%*s---------------------------\n",
                             sMaxDescLen, sTmpBuffer1,
                             "  NODE DESCRIPTION",
                             sMaxDescLen - 18, "",
                             sMaxDescLen, sTmpBuffer2 )
                         == STL_SUCCESS );

                /* Node들의 Description 설정 */
                sPlanText = sHeadPlanText->mNext;
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sPlanText->mLineBuffer),
                             aEnv,
                             "| %4d  |  %*s%s%*s|            | %1d:%02d:%02d.%02d |\n",
                             sPlanText->mIdx,
                             sPlanText->mDepth * 2, "",
                             sPlanText->mDescBuffer.mText,
                             (sMaxDescLen - (sPlanText->mDepth * 2) - sPlanText->mDescBuffer.mCurSize - 1), "",
                             sPlanText->mFetchTime / 3600000000,
                             (sPlanText->mFetchTime / 60000000) % 60,
                             (sPlanText->mFetchTime / 1000000) % 60,
                             (sPlanText->mFetchTime % 1000000) / 10000 )
                         == STL_SUCCESS );

                sPlanText->mLineText = sPlanText->mLineBuffer.mText;

                sPlanText = sPlanText->mNext;

                while( (sPlanText != NULL) &&
                       (sPlanText->mNext != NULL) )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 &QLL_DATA_PLAN( aDBCStmt ),
                                 &(sPlanText->mLineBuffer),
                                 aEnv,
                                 "| %4d  |  %*s%s%*s| %10ld | %1d:%02d:%02d.%02d |\n",
                                 sPlanText->mIdx,
                                 sPlanText->mDepth * 2, "",
                                 sPlanText->mDescBuffer.mText,
                                 (sMaxDescLen - (sPlanText->mDepth * 2) - sPlanText->mDescBuffer.mCurSize - 1), "",
                                 sPlanText->mFetchRowCnt,
                                 (sPlanText->mBuildTime + sPlanText->mFetchTime) / 3600000000,
                                 ((sPlanText->mBuildTime + sPlanText->mFetchTime) / 60000000) % 60,
                                 ((sPlanText->mBuildTime + sPlanText->mFetchTime) / 1000000) % 60,
                                 ((sPlanText->mBuildTime + sPlanText->mFetchTime) % 1000000) / 10000 )
                             == STL_SUCCESS );

                    sPlanText->mLineText = sPlanText->mLineBuffer.mText;

                    sPlanText = sPlanText->mNext;
                }

                /* Tail 구성 */
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sTailPlanText->mLineBuffer),
                             aEnv,
                             "=========%*s===========================\n\n",
                             sMaxDescLen, sTmpBuffer1 )
                         == STL_SUCCESS );

                break;

            case 3:
                /* Head 구성 */
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sHeadPlanText->mLineBuffer),
                             aEnv,
                             "< Execution Plan >\n"
                             "=========%*s=====================================================\n"
                             "|  IDX  |%s%*s|       ROWS | Build Time | Fetch Time | Total Time |\n"
                             "---------%*s-----------------------------------------------------\n",
                             sMaxDescLen, sTmpBuffer1,
                             "  NODE DESCRIPTION",
                             sMaxDescLen - 18, "",
                             sMaxDescLen, sTmpBuffer2 )
                         == STL_SUCCESS );

                /* Node들의 Description 설정 */
                sPlanText = sHeadPlanText->mNext;
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sPlanText->mLineBuffer),
                             aEnv,
                             "| %4d  |  %*s%s%*s|            |            | %1d:%02d:%02d.%02d | %1d:%02d:%02d.%02d |\n",
                             sPlanText->mIdx,
                             sPlanText->mDepth * 2, "",
                             sPlanText->mDescBuffer.mText,
                             (sMaxDescLen - (sPlanText->mDepth * 2) - sPlanText->mDescBuffer.mCurSize - 1), "",
                             sPlanText->mFetchTime / 3600000000,
                             (sPlanText->mFetchTime / 60000000) % 60,
                             (sPlanText->mFetchTime / 1000000) % 60,
                             (sPlanText->mFetchTime % 1000000) / 10000,
                             sPlanText->mFetchTime / 3600000000,
                             (sPlanText->mFetchTime / 60000000) % 60,
                             (sPlanText->mFetchTime / 1000000) % 60,
                             (sPlanText->mFetchTime % 1000000) / 10000 )
                         == STL_SUCCESS );

                sPlanText->mLineText = sPlanText->mLineBuffer.mText;

                sPlanText = sPlanText->mNext;

                while( (sPlanText != NULL) &&
                       (sPlanText->mNext != NULL) )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 &QLL_DATA_PLAN( aDBCStmt ),
                                 &(sPlanText->mLineBuffer),
                                 aEnv,
                                 "| %4d  |  %*s%s%*s| %10ld | %1d:%02d:%02d.%02d | %1d:%02d:%02d.%02d | %1d:%02d:%02d.%02d |\n",
                                 sPlanText->mIdx,
                                 sPlanText->mDepth * 2, "",
                                 sPlanText->mDescBuffer.mText,
                                 (sMaxDescLen - (sPlanText->mDepth * 2) - sPlanText->mDescBuffer.mCurSize - 1), "",
                                 sPlanText->mFetchRowCnt,
                                 sPlanText->mBuildTime / 3600000000,
                                 (sPlanText->mBuildTime / 60000000) % 60,
                                 (sPlanText->mBuildTime / 1000000) % 60,
                                 (sPlanText->mBuildTime % 1000000) / 10000,
                                 sPlanText->mFetchTime / 3600000000,
                                 (sPlanText->mFetchTime / 60000000) % 60,
                                 (sPlanText->mFetchTime / 1000000) % 60,
                                 (sPlanText->mFetchTime % 1000000) / 10000,
                                 (sPlanText->mBuildTime + sPlanText->mFetchTime) / 3600000000,
                                 ((sPlanText->mBuildTime + sPlanText->mFetchTime) / 60000000) % 60,
                                 ((sPlanText->mBuildTime + sPlanText->mFetchTime) / 1000000) % 60,
                                 ((sPlanText->mBuildTime + sPlanText->mFetchTime) % 1000000) / 10000 )
                             == STL_SUCCESS );

                    sPlanText->mLineText = sPlanText->mLineBuffer.mText;

                    sPlanText = sPlanText->mNext;
                }

                /* Tail 구성 */
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sTailPlanText->mLineBuffer),
                             aEnv,
                             "=========%*s=====================================================\n\n",
                             sMaxDescLen, sTmpBuffer1 )
                         == STL_SUCCESS );

                break;

            default:
                /* Head 구성 */
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sHeadPlanText->mLineBuffer),
                             aEnv,
                             "< Execution Plan >\n"
                             "=========%*s=======================================================================================\n"
                             "|  IDX  |%s%*s|       ROWS | Build Time | Fetch Time | Total Time | Rec AVG Size | Fetch Call Count |\n"
                             "---------%*s---------------------------------------------------------------------------------------\n",
                             sMaxDescLen, sTmpBuffer1,
                             "  NODE DESCRIPTION",
                             sMaxDescLen - 18, "",
                             sMaxDescLen, sTmpBuffer2 )
                         == STL_SUCCESS );

                /* Node들의 Description 설정 */
                sPlanText = sHeadPlanText->mNext;
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sPlanText->mLineBuffer),
                             aEnv,
                             "| %4d  |  %*s%s%*s|            |            | %1d:%02d:%02d.%02d | %1d:%02d:%02d.%02d |              |                  |\n",
                             sPlanText->mIdx,
                             sPlanText->mDepth * 2, "",
                             sPlanText->mDescBuffer.mText,
                             (sMaxDescLen - (sPlanText->mDepth * 2) - sPlanText->mDescBuffer.mCurSize - 1), "",
                             sPlanText->mFetchTime / 3600000000,
                             (sPlanText->mFetchTime / 60000000) % 60,
                             (sPlanText->mFetchTime / 1000000) % 60,
                             (sPlanText->mFetchTime % 1000000) / 10000,
                             sPlanText->mFetchTime / 3600000000,
                             (sPlanText->mFetchTime / 60000000) % 60,
                             (sPlanText->mFetchTime / 1000000) % 60,
                             (sPlanText->mFetchTime % 1000000) / 10000 )
                         == STL_SUCCESS );

                sPlanText->mLineText = sPlanText->mLineBuffer.mText;

                sPlanText = sPlanText->mNext;

                while( (sPlanText != NULL) &&
                       (sPlanText->mNext != NULL) )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 &QLL_DATA_PLAN( aDBCStmt ),
                                 &(sPlanText->mLineBuffer),
                                 aEnv,
                                 "| %4d  |  %*s%s%*s| %10ld | %1d:%02d:%02d.%02d | %1d:%02d:%02d.%02d | %1d:%02d:%02d.%02d | %12.lf | %16d |\n",
                                 sPlanText->mIdx,
                                 sPlanText->mDepth * 2, "",
                                 sPlanText->mDescBuffer.mText,
                                 (sMaxDescLen - (sPlanText->mDepth * 2) - sPlanText->mDescBuffer.mCurSize - 1), "",
                                 sPlanText->mFetchRowCnt,
                                 sPlanText->mBuildTime / 3600000000,
                                 (sPlanText->mBuildTime / 60000000) % 60,
                                 (sPlanText->mBuildTime / 1000000) % 60,
                                 (sPlanText->mBuildTime % 1000000) / 10000,
                                 sPlanText->mFetchTime / 3600000000,
                                 (sPlanText->mFetchTime / 60000000) % 60,
                                 (sPlanText->mFetchTime / 1000000) % 60,
                                 (sPlanText->mFetchTime % 1000000) / 10000,
                                 (sPlanText->mBuildTime + sPlanText->mFetchTime) / 3600000000,
                                 ((sPlanText->mBuildTime + sPlanText->mFetchTime) / 60000000) % 60,
                                 ((sPlanText->mBuildTime + sPlanText->mFetchTime) / 1000000) % 60,
                                 ((sPlanText->mBuildTime + sPlanText->mFetchTime) % 1000000) / 10000,
                                 sPlanText->mFetchRecordAvgSize,
                                 sPlanText->mFetchCallCount )
                             == STL_SUCCESS );

                    sPlanText->mLineText = sPlanText->mLineBuffer.mText;

                    sPlanText = sPlanText->mNext;
                }

                /* Tail 구성 */
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sTailPlanText->mLineBuffer),
                             aEnv,
                             "=========%*s=======================================================================================\n\n",
                             sMaxDescLen, sTmpBuffer1 )
                         == STL_SUCCESS );

                break;
        }
    }
    else
    {
        switch( aSqlPlanLevel )
        {
            case 1:
            case 2:
                /* Head 구성 */
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sHeadPlanText->mLineBuffer),
                             aEnv,
                             "< Execution Plan >\n"
                             "=========%*s===============================\n"
                             "|  IDX  |%s%*s|       ROWS | Total Time     |\n"
                             "---------%*s-------------------------------\n",
                             sMaxDescLen, sTmpBuffer1,
                             "  NODE DESCRIPTION",
                             sMaxDescLen - 18, "",
                             sMaxDescLen, sTmpBuffer2 )
                         == STL_SUCCESS );

                /* Node들의 Description 설정 */
                sPlanText = sHeadPlanText->mNext;
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sPlanText->mLineBuffer),
                             aEnv,
                             "| %4d  |  %*s%s%*s|            | %1d:%02d:%02d.%06d |\n",
                             sPlanText->mIdx,
                             sPlanText->mDepth * 2, "",
                             sPlanText->mDescBuffer.mText,
                             (sMaxDescLen - (sPlanText->mDepth * 2) - sPlanText->mDescBuffer.mCurSize - 1), "",
                             sPlanText->mFetchTime / 3600000000,
                             (sPlanText->mFetchTime / 60000000) % 60,
                             (sPlanText->mFetchTime / 1000000) % 60,
                             sPlanText->mFetchTime % 1000000 )
                         == STL_SUCCESS );

                sPlanText->mLineText = sPlanText->mLineBuffer.mText;

                sPlanText = sPlanText->mNext;

                while( (sPlanText != NULL) &&
                       (sPlanText->mNext != NULL) )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 &QLL_DATA_PLAN( aDBCStmt ),
                                 &(sPlanText->mLineBuffer),
                                 aEnv,
                                 "| %4d  |  %*s%s%*s| %10ld | %1d:%02d:%02d.%06d |\n",
                                 sPlanText->mIdx,
                                 sPlanText->mDepth * 2, "",
                                 sPlanText->mDescBuffer.mText,
                                 (sMaxDescLen - (sPlanText->mDepth * 2) - sPlanText->mDescBuffer.mCurSize - 1), "",
                                 sPlanText->mFetchRowCnt,
                                 (sPlanText->mBuildTime + sPlanText->mFetchTime) / 3600000000,
                                 ((sPlanText->mBuildTime + sPlanText->mFetchTime) / 60000000) % 60,
                                 ((sPlanText->mBuildTime + sPlanText->mFetchTime) / 1000000) % 60,
                                 (sPlanText->mBuildTime + sPlanText->mFetchTime) % 1000000 )
                             == STL_SUCCESS );

                    sPlanText->mLineText = sPlanText->mLineBuffer.mText;

                    sPlanText = sPlanText->mNext;
                }

                /* Tail 구성 */
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sTailPlanText->mLineBuffer),
                             aEnv,
                             "=========%*s===============================\n\n",
                             sMaxDescLen, sTmpBuffer1 )
                         == STL_SUCCESS );

                break;

            case 3:
                /* Head 구성 */
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sHeadPlanText->mLineBuffer),
                             aEnv,
                             "< Execution Plan >\n"
                             "=========%*s=================================================================\n"
                             "|  IDX  |%s%*s|       ROWS | Build Time     | Fetch Time     | Total Time     |\n"
                             "---------%*s-----------------------------------------------------------------\n",
                             sMaxDescLen, sTmpBuffer1,
                             "  NODE DESCRIPTION",
                             sMaxDescLen - 18, "",
                             sMaxDescLen, sTmpBuffer2 )
                         == STL_SUCCESS );

                /* Node들의 Description 설정 */
                sPlanText = sHeadPlanText->mNext;
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sPlanText->mLineBuffer),
                             aEnv,
                             "| %4d  |  %*s%s%*s|            |                | %1d:%02d:%02d.%06d | %1d:%02d:%02d.%06d |\n",
                             sPlanText->mIdx,
                             sPlanText->mDepth * 2, "",
                             sPlanText->mDescBuffer.mText,
                             (sMaxDescLen - (sPlanText->mDepth * 2) - sPlanText->mDescBuffer.mCurSize - 1), "",
                             sPlanText->mFetchTime / 3600000000,
                             (sPlanText->mFetchTime / 60000000) % 60,
                             (sPlanText->mFetchTime / 1000000) % 60,
                             sPlanText->mFetchTime % 1000000,
                             sPlanText->mFetchTime / 3600000000,
                             (sPlanText->mFetchTime / 60000000) % 60,
                             (sPlanText->mFetchTime / 1000000) % 60,
                             sPlanText->mFetchTime % 1000000 )
                         == STL_SUCCESS );

                sPlanText->mLineText = sPlanText->mLineBuffer.mText;

                sPlanText = sPlanText->mNext;

                while( (sPlanText != NULL) &&
                       (sPlanText->mNext != NULL) )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 &QLL_DATA_PLAN( aDBCStmt ),
                                 &(sPlanText->mLineBuffer),
                                 aEnv,
                                 "| %4d  |  %*s%s%*s| %10ld | %1d:%02d:%02d.%06d | %1d:%02d:%02d.%06d | %1d:%02d:%02d.%06d |\n",
                                 sPlanText->mIdx,
                                 sPlanText->mDepth * 2, "",
                                 sPlanText->mDescBuffer.mText,
                                 (sMaxDescLen - (sPlanText->mDepth * 2) - sPlanText->mDescBuffer.mCurSize - 1), "",
                                 sPlanText->mFetchRowCnt,
                                 sPlanText->mBuildTime / 3600000000,
                                 (sPlanText->mBuildTime / 60000000) % 60,
                                 (sPlanText->mBuildTime / 1000000) % 60,
                                 sPlanText->mBuildTime % 1000000,
                                 sPlanText->mFetchTime / 3600000000,
                                 (sPlanText->mFetchTime / 60000000) % 60,
                                 (sPlanText->mFetchTime / 1000000) % 60,
                                 sPlanText->mFetchTime % 1000000,
                                 (sPlanText->mBuildTime + sPlanText->mFetchTime) / 3600000000,
                                 ((sPlanText->mBuildTime + sPlanText->mFetchTime) / 60000000) % 60,
                                 ((sPlanText->mBuildTime + sPlanText->mFetchTime) / 1000000) % 60,
                                 (sPlanText->mBuildTime + sPlanText->mFetchTime) % 1000000 )
                             == STL_SUCCESS );

                    sPlanText->mLineText = sPlanText->mLineBuffer.mText;

                    sPlanText = sPlanText->mNext;
                }

                /* Tail 구성 */
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sTailPlanText->mLineBuffer),
                             aEnv,
                             "=========%*s=================================================================\n\n",
                             sMaxDescLen, sTmpBuffer1 )
                         == STL_SUCCESS );

                break;

            default:
                /* Head 구성 */
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sHeadPlanText->mLineBuffer),
                             aEnv,
                             "< Execution Plan >\n"
                             "=========%*s===================================================================================================\n"
                             "|  IDX  |%s%*s|       ROWS | Build Time     | Fetch Time     | Total Time     | Rec AVG Size | Fetch Call Count |\n"
                             "---------%*s---------------------------------------------------------------------------------------------------\n",
                             sMaxDescLen, sTmpBuffer1,
                             "  NODE DESCRIPTION",
                             sMaxDescLen - 18, "",
                             sMaxDescLen, sTmpBuffer2 )
                         == STL_SUCCESS );

                /* Node들의 Description 설정 */
                sPlanText = sHeadPlanText->mNext;
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sPlanText->mLineBuffer),
                             aEnv,
                             "| %4d  |  %*s%s%*s|            |                | %1d:%02d:%02d.%06d | %1d:%02d:%02d.%06d |              |                  |\n",
                             sPlanText->mIdx,
                             sPlanText->mDepth * 2, "",
                             sPlanText->mDescBuffer.mText,
                             (sMaxDescLen - (sPlanText->mDepth * 2) - sPlanText->mDescBuffer.mCurSize - 1), "",
                             sPlanText->mFetchTime / 3600000000,
                             (sPlanText->mFetchTime / 60000000) % 60,
                             (sPlanText->mFetchTime / 1000000) % 60,
                             sPlanText->mFetchTime % 1000000,
                             sPlanText->mFetchTime / 3600000000,
                             (sPlanText->mFetchTime / 60000000) % 60,
                             (sPlanText->mFetchTime / 1000000) % 60,
                             sPlanText->mFetchTime % 1000000 )
                         == STL_SUCCESS );

                sPlanText->mLineText = sPlanText->mLineBuffer.mText;

                sPlanText = sPlanText->mNext;

                while( (sPlanText != NULL) &&
                       (sPlanText->mNext != NULL) )
                {
                    STL_TRY( qleAppendStringToExplainText(
                                 &QLL_DATA_PLAN( aDBCStmt ),
                                 &(sPlanText->mLineBuffer),
                                 aEnv,
                                 "| %4d  |  %*s%s%*s| %10ld | %1d:%02d:%02d.%06d | %1d:%02d:%02d.%06d | %1d:%02d:%02d.%06d | %12.lf | %16d |\n",
                                 sPlanText->mIdx,
                                 sPlanText->mDepth * 2, "",
                                 sPlanText->mDescBuffer.mText,
                                 (sMaxDescLen - (sPlanText->mDepth * 2) - sPlanText->mDescBuffer.mCurSize - 1), "",
                                 sPlanText->mFetchRowCnt,
                                 sPlanText->mBuildTime / 3600000000,
                                 (sPlanText->mBuildTime / 60000000) % 60,
                                 (sPlanText->mBuildTime / 1000000) % 60,
                                 sPlanText->mBuildTime % 1000000,
                                 sPlanText->mFetchTime / 3600000000,
                                 (sPlanText->mFetchTime / 60000000) % 60,
                                 (sPlanText->mFetchTime / 1000000) % 60,
                                 sPlanText->mFetchTime % 1000000,
                                 (sPlanText->mBuildTime + sPlanText->mFetchTime) / 3600000000,
                                 ((sPlanText->mBuildTime + sPlanText->mFetchTime) / 60000000) % 60,
                                 ((sPlanText->mBuildTime + sPlanText->mFetchTime) / 1000000) % 60,
                                 (sPlanText->mBuildTime + sPlanText->mFetchTime) % 1000000,
                                 sPlanText->mFetchRecordAvgSize,
                                 sPlanText->mFetchCallCount )
                             == STL_SUCCESS );

                    sPlanText->mLineText = sPlanText->mLineBuffer.mText;

                    sPlanText = sPlanText->mNext;
                }

                /* Tail 구성 */
                STL_TRY( qleAppendStringToExplainText(
                             &QLL_DATA_PLAN( aDBCStmt ),
                             &(sTailPlanText->mLineBuffer),
                             aEnv,
                             "=========%*s===================================================================================================\n\n",
                             sMaxDescLen, sTmpBuffer1 )
                         == STL_SUCCESS );

                break;
        }
    }


    STL_RAMP( RAMP_FINISH );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Explain Node Text를 생성한다.
 * @param[in]   aRegionMem          Region Memory
 * @param[in]   aDepth              Depth
 * @param[in]   aParentNodeText     Parent Explain Node Text
 * @param[out]  aCurrentNodeText    Current Explain Node Text
 * @param[in]   aEnv                Environment
 */
stlStatus qleCreateExplainNodeText( knlRegionMem        * aRegionMem,
                                    stlInt32              aDepth,
                                    qllExplainNodeText  * aParentNodeText,
                                    qllExplainNodeText ** aCurrentNodeText,
                                    qllEnv              * aEnv )
{
    qllExplainNodeText  * sCurText  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /* Explain Node Text 할당 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qllExplainNodeText ),
                                (void **) & sCurText,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Explain Node Text 초기 정보 설정 */
    if( aParentNodeText == NULL )
    {
        sCurText->mIdx = -1;
    }
    else
    {
        sCurText->mIdx = aParentNodeText->mIdx + 1;
        aParentNodeText->mNext = sCurText;
    }

    sCurText->mDepth = aDepth;
    sCurText->mFetchRowCnt = 0;
    sCurText->mBuildTime = 0;
    sCurText->mFetchTime = 0;
    sCurText->mFetchRecordAvgSize = 0;
    sCurText->mFetchCallCount = 0;
    sCurText->mDescBuffer.mMaxSize = 0;
    sCurText->mDescBuffer.mCurSize = 0;
    sCurText->mDescBuffer.mText = NULL;
    sCurText->mDescBuffer.mLineText = NULL;
    sCurText->mLineBuffer.mMaxSize = 0;
    sCurText->mLineBuffer.mCurSize = 0;
    sCurText->mLineBuffer.mText = NULL;
    sCurText->mLineBuffer.mLineText = &(sCurText->mLineText);
    sCurText->mLineText = NULL;
    sCurText->mPredText = NULL;
    sCurText->mLastPredText = NULL;
    sCurText->mNext = NULL;

    /* Output 설정 */
    *aCurrentNodeText = sCurText;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Explain Predicate Text를 생성한다.
 * @param[in]   aRegionMem  Region Memory
 * @param[in]   aDepth      Depth
 * @param[in]   aNodeText   Parent Explain Node Text
 * @param[out]  aPredText   Current Explain Node Text
 * @param[in]   aEnv        Environment
 */
stlStatus qleCreateExplainPredText( knlRegionMem        * aRegionMem,
                                    stlInt32              aDepth,
                                    qllExplainNodeText  * aNodeText,
                                    qllExplainPredText ** aPredText,
                                    qllEnv              * aEnv )
{
    qllExplainPredText  * sCurText  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNodeText != NULL, QLL_ERROR_STACK(aEnv) );


    /* Explain Node Text 할당 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qllExplainPredText ),
                                (void **) & sCurText,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Explain Text의 String 할당 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                QLL_EXPLAIN_PLAN_DEFAULT_LINE_SIZE,
                                (void **) &(sCurText->mLineBuffer.mText),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sCurText->mLineBuffer.mText[0] = '\0';
    sCurText->mLineBuffer.mMaxSize = QLL_EXPLAIN_PLAN_DEFAULT_LINE_SIZE;
    sCurText->mLineBuffer.mCurSize = 0;
    sCurText->mLineBuffer.mLineText = &(sCurText->mLineText);

    /* Explain Predicate Text 정보 설정 */
    sCurText->mLineText = NULL;
    sCurText->mNext = NULL;
    if( aNodeText->mPredText == NULL )
    {
        aNodeText->mPredText     = sCurText;
        aNodeText->mLastPredText = sCurText;

        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               &(sCurText->mLineBuffer),
                                               aEnv,
                                               "  %4d  -%*s",
                                               aNodeText->mIdx,
                                               aDepth * 2 + 2, "" )
                 == STL_SUCCESS );
    }
    else
    {
        aNodeText->mLastPredText->mNext = sCurText;
        aNodeText->mLastPredText        = sCurText;

        STL_TRY( qleAppendStringToExplainText( aRegionMem,
                                               &(sCurText->mLineBuffer),
                                               aEnv,
                                               " %*s",
                                               aDepth * 2 + 10, "" )
                 == STL_SUCCESS );
    }

    *(sCurText->mLineBuffer.mLineText) = sCurText->mLineBuffer.mText;

    /* Output 설정 */
    *aPredText = sCurText;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Explain Text에 String을 Set한다.
 * @param[in]   aRegionMem      Region Memory
 * @param[in]   aExplainText    Explain Text
 * @param[in]   aEnv            Environment
 * @param[in]   aFormat         Format
 * @param[in]   ...             Output Parameter
 */
stlStatus qleSetStringToExplainText( knlRegionMem   * aRegionMem,
                                     qllExplainText * aExplainText,
                                     qllEnv         * aEnv,
                                     const stlChar  * aFormat,
                                     ... )
{
    stlInt32      sStrLen;
    stlVarList    sVarArgList;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExplainText != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFormat != NULL, QLL_ERROR_STACK(aEnv) );


    /* String Size 설정 */
    stlVarStart( sVarArgList, aFormat );
    sStrLen = stlGetVprintfBufferSize( aFormat, sVarArgList );
    stlVarEnd( sVarArgList );

    /* String 공간 할당 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( stlChar ) * (sStrLen + 1),
                                (void**) &(aExplainText->mText),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    aExplainText->mMaxSize = sStrLen + 1;
    aExplainText->mCurSize = sStrLen + 1;
    if( aExplainText->mLineText != NULL )
    {
        *(aExplainText->mLineText) = aExplainText->mText;
    }

    /* Set String */
    stlVarStart( sVarArgList, aFormat );
    stlVsnprintf( aExplainText->mText,
                  sStrLen + 1,
                  aFormat,
                  sVarArgList );
    stlVarEnd( sVarArgList );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Explain Text에 String을 Append한다.
 * @param[in]   aRegionMem      Region Memory
 * @param[in]   aExplainText    Explain Text
 * @param[in]   aEnv            Environment
 * @param[in]   aFormat         Format
 * @param[in]   ...             Output Parameter
 */
stlStatus qleAppendStringToExplainText( knlRegionMem    * aRegionMem,
                                        qllExplainText  * aExplainText,
                                        qllEnv          * aEnv,
                                        const stlChar   * aFormat,
                                        ... )
{
    stlInt32      sStrLen;
    stlInt32      sMaxLen;
    stlVarList    sVarArgList;
    stlChar     * sNewString    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExplainText != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFormat != NULL, QLL_ERROR_STACK(aEnv) );


    /* String Size 설정 */
    stlVarStart( sVarArgList, aFormat );
    sStrLen = stlGetVprintfBufferSize( aFormat, sVarArgList );
    stlVarEnd( sVarArgList );

    /* Size 확인 */
    if( (aExplainText->mCurSize + sStrLen + 1/* 종료문자 */) > aExplainText->mMaxSize )
    {
        sMaxLen = (stlInt32)( ( aExplainText->mCurSize + sStrLen + 1 ) / QLL_EXPLAIN_PLAN_DEFAULT_LINE_SIZE );
        sMaxLen = ( sMaxLen + 1 ) * QLL_EXPLAIN_PLAN_DEFAULT_LINE_SIZE;
        
        /* String의 공간을 늘린다. */
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( stlChar ) * sMaxLen,
                                    (void**) &sNewString,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sNewString, aExplainText->mText, STL_SIZEOF( stlChar ) * aExplainText->mCurSize );

        aExplainText->mText = sNewString;
        aExplainText->mMaxSize = sMaxLen;

        if( aExplainText->mLineText != NULL )
        {
            *(aExplainText->mLineText) = aExplainText->mText;
        }
    }

    /* Append String */
    stlVarStart( sVarArgList, aFormat );
    stlVsnprintf( aExplainText->mText + aExplainText->mCurSize,
                  sStrLen + 1,
                  aFormat,
                  sVarArgList );
    stlVarEnd( sVarArgList );

    aExplainText->mCurSize += sStrLen;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qleExplainPlan */


