/*******************************************************************************
 * qlvCommon.c
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
 * @file qlvCommon.c
 * @brief SQL Processor Layer Common Validation
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlv
 * @{
 */


/**
 * @brief Query 또는 DML 의 Init Plan 을 초기화한다.
 * @param[in] aDBCStmt   DBC Statement
 * @param[in] aSQLStmt   SQL Statement
 * @param[in] aInitPlan  Init Plan of Query or DML
 * @param[in] aEnv       Environment
 * @remarks
 */
stlStatus qlvSetInitPlan( qllDBCStmt   * aDBCStmt,
                          qllStatement * aSQLStmt,
                          qlvInitPlan  * aInitPlan,
                          qllEnv       * aEnv )
{
    ellDictHandle * sAuthHandle = NULL;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitPlan != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Validation Object List 초기화
     */
    
    STL_TRY( ellInitObjectList( & aInitPlan->mValidationObjList,
                                QLL_INIT_PLAN(aDBCStmt),
                                ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Plan Cache 의 User Handle 의 최신 검사를 위해 추가
     */
    
    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );

    STL_TRY( ellAppendObjectItem( aInitPlan->mValidationObjList,
                                  sAuthHandle,
                                  QLL_INIT_PLAN(aDBCStmt),
                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Binding Memory 할당
     */

    aInitPlan->mBindCount = aSQLStmt->mBindCount;

    if ( aInitPlan->mBindCount > 0 )
    {
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlInt32) * aInitPlan->mBindCount,
                                    (void **) & aInitPlan->mBindNodePos,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlInt32) * aInitPlan->mBindCount,
                                    (void **) & aInitPlan->mBindIOType,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        aInitPlan->mBindNodePos = NULL;
        aInitPlan->mBindIOType  = NULL;
    }
        
    /**
     * Internal Bind Parameter Count 초기화
     */
    
    aInitPlan->mInternalBindCount = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    
    /**
     * Hint Error 초기화
     */
    
    aInitPlan->mHasHintError = STL_FALSE;

    /**
     * Result Cursor Property 정보 초기화 
     */

    aInitPlan->mResultCursorProperty.mStandardType  = ELL_CURSOR_STANDARD_NA;
    
    aInitPlan->mResultCursorProperty.mSensitivity   = ELL_CURSOR_SENSITIVITY_NA;
    aInitPlan->mResultCursorProperty.mScrollability = ELL_CURSOR_SCROLLABILITY_NA;
    aInitPlan->mResultCursorProperty.mHoldability   = ELL_CURSOR_HOLDABILITY_NA;
    aInitPlan->mResultCursorProperty.mUpdatability  = ELL_CURSOR_UPDATABILITY_NA;
    aInitPlan->mResultCursorProperty.mReturnability = ELL_CURSOR_RETURNABILITY_NA;

    /**
     * SQL Statement 에 Init Plan 연결
     */

    aSQLStmt->mInitPlan = (void *) aInitPlan;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Source Init Plan 정보로부터 Destin InitPlan 을 설정한다.
 * @param[out]  aDesPlan   Destination Init Plan
 * @param[in]   aSrcPlan   Source Init Plan
 */
void qlvSetInitPlanByInitPlan( qlvInitPlan  * aDesPlan,
                               qlvInitPlan  * aSrcPlan )
{
    aDesPlan->mValidationObjList = aSrcPlan->mValidationObjList;

    aDesPlan->mBindCount   = aSrcPlan->mBindCount;
    aDesPlan->mBindNodePos = aSrcPlan->mBindNodePos;
    aDesPlan->mBindIOType  = aSrcPlan->mBindIOType;
    
    aDesPlan->mInternalBindCount = aSrcPlan->mInternalBindCount;

    aDesPlan->mHasHintError = aSrcPlan->mHasHintError;

    stlMemcpy( & aDesPlan->mResultCursorProperty,
               & aSrcPlan->mResultCursorProperty,
               STL_SIZEOF(ellCursorProperty) ) ;
}



/**
 * @brief SQL 문장이 Query 의 공통 Init Plan 획득
 * @param[in] aSQLStmt    SQL Statement
 */
qlvInitPlan * qlvGetQueryInitPlan( qllStatement * aSQLStmt )
{
    qlvInitPlan * sInitPlan = NULL;

    if ( qllNeedFetch( aSQLStmt ) == STL_TRUE )
    {
        sInitPlan = (qlvInitPlan *) qllGetInitPlan( aSQLStmt );
    }
    else
    {
        sInitPlan = NULL;
    }

    return sInitPlan;
}



/**
 * @brief Code Expression 복사
 * @param[in]     aSourceExpr  Source Expression 
 * @param[out]    aDestExpr    Dest Expression
 * @param[in]     aRegionMem   Region Memory
 * @param[in]     aEnv         Environment
 * @remarks
 */
stlStatus qlvCopyExpr( qlvInitExpression   * aSourceExpr,
                       qlvInitExpression  ** aDestExpr,
                       knlRegionMem        * aRegionMem,
                       qllEnv              * aEnv )
{
    qlvInitExpression   * sNewExpr   = NULL;
    qlvInitValue        * sOldValue  = NULL;
    qlvInitValue        * sNewValue  = NULL;

    stlUInt32             sAllocSize = 0;
    stlInt32              sArgCount  = 0;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSourceExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDestExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * 새로운 Expression 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem(
                 aRegionMem,
                 STL_SIZEOF( qlvInitExpression ),
                 (void **) & sNewExpr,
                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    

    /**
     * Expression 복사
     */
    
    stlMemcpy( sNewExpr, aSourceExpr, STL_SIZEOF( qlvInitExpression ) );
    
    switch( aSourceExpr->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
            {
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             STL_SIZEOF( qlvInitValue ),
                             (void **) & sNewExpr->mExpr.mValue,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sOldValue = aSourceExpr->mExpr.mValue;
                sNewValue = sNewExpr->mExpr.mValue;
                
                sNewValue->mValueType = sOldValue->mValueType;
                sNewValue->mInternalBindIdx = sOldValue->mInternalBindIdx;

                switch( sOldValue->mValueType )
                {
                    case QLV_VALUE_TYPE_BOOLEAN :
                    case QLV_VALUE_TYPE_BINARY_INTEGER :
                        {
                            sNewValue->mData.mInteger = sOldValue->mData.mInteger;
                            break;
                        }
                        
                    case QLV_VALUE_TYPE_BINARY_REAL :
                    case QLV_VALUE_TYPE_BINARY_DOUBLE :
                    case QLV_VALUE_TYPE_NUMERIC :
                    case QLV_VALUE_TYPE_STRING_FIXED :
                    case QLV_VALUE_TYPE_STRING_VARYING :
                    case QLV_VALUE_TYPE_BITSTRING_FIXED :
                    case QLV_VALUE_TYPE_BITSTRING_VARYING :
                    case QLV_VALUE_TYPE_DATE_LITERAL_STRING :
                    case QLV_VALUE_TYPE_TIME_LITERAL_STRING :
                    case QLV_VALUE_TYPE_TIME_TZ_LITERAL_STRING :
                    case QLV_VALUE_TYPE_TIMESTAMP_LITERAL_STRING :
                    case QLV_VALUE_TYPE_TIMESTAMP_TZ_LITERAL_STRING :
                    case QLV_VALUE_TYPE_INTERVAL_YEAR_TO_MONTH_STRING :
                    case QLV_VALUE_TYPE_INTERVAL_DAY_TO_SECOND_STRING :
                    case QLV_VALUE_TYPE_ROWID_STRING :
                        {
                            STL_TRY( knlAllocRegionMem(
                                         aRegionMem,
                                         stlStrlen( sOldValue->mData.mString ) + 1,
                                         (void **) & sNewValue->mData,
                                         KNL_ENV(aEnv) )
                                     == STL_SUCCESS );

                            stlStrcpy( sNewValue->mData.mString,
                                       sOldValue->mData.mString );
                            break;
                        }
                        
                    case QLV_VALUE_TYPE_NULL :
                        {
                            sNewValue->mData.mString = NULL;
                            break;
                        }
                    default :
                        {
                            STL_DASSERT( 0 );
                            break;
                        }
                };

                break;
            }
            
        case QLV_EXPR_TYPE_BIND_PARAM :
            {
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             STL_SIZEOF( qlvInitBindParam ),
                             (void **) & sNewExpr->mExpr.mBindParam,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sNewExpr->mExpr.mBindParam,
                           aSourceExpr->mExpr.mBindParam,
                           STL_SIZEOF( qlvInitBindParam ) );

                if( aSourceExpr->mExpr.mBindParam->mHostName != NULL )
                {
                    sAllocSize = stlStrlen( aSourceExpr->mExpr.mBindParam->mHostName ) + 1;
                    STL_TRY( knlAllocRegionMem(
                                 aRegionMem,
                                 sAllocSize,
                                 (void **) & sNewExpr->mExpr.mBindParam->mHostName,
                                 KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    stlMemcpy( sNewExpr->mExpr.mBindParam->mHostName,
                               aSourceExpr->mExpr.mBindParam->mHostName,
                               sAllocSize );
                }

                if( aSourceExpr->mExpr.mBindParam->mIndicatorName != NULL )
                {
                    sAllocSize = stlStrlen( aSourceExpr->mExpr.mBindParam->mIndicatorName ) + 1;
                    STL_TRY( knlAllocRegionMem(
                                 aRegionMem,
                                 sAllocSize,
                                 (void **) & sNewExpr->mExpr.mBindParam->mIndicatorName,
                                 KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                    stlMemcpy( sNewExpr->mExpr.mBindParam->mIndicatorName,
                               aSourceExpr->mExpr.mBindParam->mIndicatorName,
                               sAllocSize );
                }

                break;
            }

        case QLV_EXPR_TYPE_COLUMN :
            {
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             STL_SIZEOF( qlvInitColumn ),
                             (void **) & sNewExpr->mExpr.mColumn,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sNewExpr->mExpr.mColumn,
                           aSourceExpr->mExpr.mColumn,
                           STL_SIZEOF( qlvInitColumn ) );
                break;
            }

        case QLV_EXPR_TYPE_FUNCTION :
            {
                sArgCount = aSourceExpr->mExpr.mFunction->mArgCount;
                if( sArgCount > 0 )
                {
                    sAllocSize = STL_SIZEOF( qlvInitFunction ) +
                        ( STL_SIZEOF( qlvInitExpression* ) * sArgCount );
                }
                else
                {
                    sAllocSize = STL_SIZEOF( qlvInitFunction );
                }
                
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             sAllocSize,
                             (void **) & sNewExpr->mExpr.mFunction,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sNewExpr->mExpr.mFunction->mFuncId =
                    aSourceExpr->mExpr.mFunction->mFuncId;

                sNewExpr->mExpr.mFunction->mArgCount =
                    aSourceExpr->mExpr.mFunction->mArgCount;

                sNewExpr->mExpr.mFunction->mArgs =
                    (qlvInitExpression **) ( ((stlChar*) sNewExpr->mExpr.mFunction) + STL_SIZEOF( qlvInitFunction ) );
                    
                while( sArgCount > 0 )
                {
                    sArgCount--;
                    sNewExpr->mExpr.mFunction->mArgs[ sArgCount ] =
                        aSourceExpr->mExpr.mFunction->mArgs[ sArgCount ];
                }

                break;
            }

        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
            {
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             STL_SIZEOF( qlvInitPseudoCol ),
                             (void **) & sNewExpr->mExpr.mPseudoCol,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sNewExpr->mExpr.mPseudoCol,
                           aSourceExpr->mExpr.mPseudoCol,
                           STL_SIZEOF( qlvInitPseudoCol ) );
                break;
            }

        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
            {
                sNewExpr->mExpr.mPseudoArg     = aSourceExpr->mExpr.mPseudoArg;
                break;
            }

        case QLV_EXPR_TYPE_CAST :
            {
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             STL_SIZEOF( qlvInitTypeCast ),
                             (void **) & sNewExpr->mExpr.mTypeCast,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sNewExpr->mExpr.mTypeCast,
                           aSourceExpr->mExpr.mTypeCast,
                           STL_SIZEOF( qlvInitTypeCast ) );
                break;
            }

        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
            {
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             STL_SIZEOF( qlvInitConstExpr ),
                             (void **) & sNewExpr->mExpr.mConstExpr,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sNewExpr->mExpr.mConstExpr,
                           aSourceExpr->mExpr.mConstExpr,
                           STL_SIZEOF( qlvInitConstExpr ) );
                break;
            }

        case QLV_EXPR_TYPE_REFERENCE :
            {
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             STL_SIZEOF( qlvInitRefExpr ),
                             (void **) & sNewExpr->mExpr.mReference,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sNewExpr->mExpr.mReference,
                           aSourceExpr->mExpr.mReference,
                           STL_SIZEOF( qlvInitRefExpr ) );
                break;
            }

        case QLV_EXPR_TYPE_SUB_QUERY :
            {
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             STL_SIZEOF( qlvInitSubQuery ),
                             (void **) & sNewExpr->mExpr.mSubQuery,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sNewExpr->mExpr.mSubQuery,
                           aSourceExpr->mExpr.mSubQuery,
                           STL_SIZEOF( qlvInitSubQuery ) );
                break;
            }
            
        case QLV_EXPR_TYPE_INNER_COLUMN :
            {
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             STL_SIZEOF( qlvInitInnerColumn ),
                             (void **) & sNewExpr->mExpr.mInnerColumn,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sNewExpr->mExpr.mInnerColumn,
                           aSourceExpr->mExpr.mInnerColumn,
                           STL_SIZEOF( qlvInitInnerColumn ) );
                break;
            }

        case QLV_EXPR_TYPE_OUTER_COLUMN :
            {
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             STL_SIZEOF( qlvInitOuterColumn ),
                             (void **) & sNewExpr->mExpr.mOuterColumn,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sNewExpr->mExpr.mOuterColumn,
                           aSourceExpr->mExpr.mOuterColumn,
                           STL_SIZEOF( qlvInitOuterColumn ) );
                break;
            }
        case QLV_EXPR_TYPE_ROWID_COLUMN :
            {
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             STL_SIZEOF( qlvInitRowIdColumn ),
                             (void **) & sNewExpr->mExpr.mRowIdColumn,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sNewExpr->mExpr.mRowIdColumn,
                           aSourceExpr->mExpr.mRowIdColumn,
                           STL_SIZEOF( qlvInitRowIdColumn ) );
                
                break;
            }
        case QLV_EXPR_TYPE_AGGREGATION :
            {
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             STL_SIZEOF( qlvInitAggregation ),
                             (void **) & sNewExpr->mExpr.mAggregation,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sNewExpr->mExpr.mAggregation,
                           aSourceExpr->mExpr.mAggregation,
                           STL_SIZEOF( qlvInitAggregation ) );
                break;
            }
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                sArgCount = aSourceExpr->mExpr.mCaseExpr->mCount;

                /* When ...  Then ...  Else ... */
                sAllocSize = STL_SIZEOF( qlvInitCaseExpr ) +
                    ( STL_SIZEOF( qlvInitExpression* ) * ( sArgCount * 2 + 1 ) );

                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             sAllocSize,
                             (void **) & sNewExpr->mExpr.mCaseExpr,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sNewExpr->mExpr.mCaseExpr->mFuncId =
                    aSourceExpr->mExpr.mCaseExpr->mFuncId;

                sNewExpr->mExpr.mCaseExpr->mCount =
                    aSourceExpr->mExpr.mCaseExpr->mCount;

                sNewExpr->mExpr.mCaseExpr->mWhenArr =
                    (qlvInitExpression **)( ((stlChar*) sNewExpr->mExpr.mCaseExpr) + STL_SIZEOF( qlvInitCaseExpr ) );
                sNewExpr->mExpr.mCaseExpr->mThenArr =
                    (qlvInitExpression **)( ((stlChar*) sNewExpr->mExpr.mCaseExpr->mWhenArr)
                                            + STL_SIZEOF( qlvInitExpression* ) * sArgCount );
                sNewExpr->mExpr.mCaseExpr->mDefResult =
                    (qlvInitExpression *)( ((stlChar*) sNewExpr->mExpr.mCaseExpr->mThenArr)
                                           + STL_SIZEOF( qlvInitExpression* ) * sArgCount );

                while( sArgCount > 0 )
                {
                    sArgCount--;

                    sNewExpr->mExpr.mCaseExpr->mWhenArr[ sArgCount ] =
                        aSourceExpr->mExpr.mCaseExpr->mWhenArr[ sArgCount ];

                    sNewExpr->mExpr.mCaseExpr->mThenArr[ sArgCount ] =
                        aSourceExpr->mExpr.mCaseExpr->mThenArr[ sArgCount ];
                }

                sNewExpr->mExpr.mCaseExpr->mDefResult =
                    aSourceExpr->mExpr.mCaseExpr->mDefResult;
                
                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                sArgCount = aSourceExpr->mExpr.mListFunc->mArgCount;

                STL_DASSERT( sArgCount > 0 );

                sAllocSize = STL_SIZEOF( qlvInitListFunc ) +
                    ( STL_SIZEOF( qlvInitExpression* ) * sArgCount );
                
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             sAllocSize,
                             (void **) & sNewExpr->mExpr.mListFunc,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sNewExpr->mExpr.mListFunc->mFuncId =
                    aSourceExpr->mExpr.mListFunc->mFuncId;

                sNewExpr->mExpr.mListFunc->mListFuncType =
                    aSourceExpr->mExpr.mListFunc->mListFuncType;

                sNewExpr->mExpr.mListFunc->mArgCount =
                    aSourceExpr->mExpr.mListFunc->mArgCount;

                sNewExpr->mExpr.mListFunc->mArgs =
                    (qlvInitExpression **) ( ((stlChar*) sNewExpr->mExpr.mListFunc) + STL_SIZEOF( qlvInitListFunc ) );
                    
                while( sArgCount > 0 )
                {
                    sArgCount--;
                    sNewExpr->mExpr.mListFunc->mArgs[ sArgCount ] =
                        aSourceExpr->mExpr.mListFunc->mArgs[ sArgCount ];
                }

                break;
            }
            
        case QLV_EXPR_TYPE_LIST_COLUMN :
            { 
                sArgCount = aSourceExpr->mExpr.mListCol->mArgCount;
                
                STL_DASSERT( sArgCount > 0 );

                sAllocSize = STL_SIZEOF( qlvInitListCol ) +
                    ( STL_SIZEOF( qlvInitExpression* ) * sArgCount ) +
                    ( STL_SIZEOF( knlListEntry* ) * sArgCount );
                
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             sAllocSize,
                             (void **) & sNewExpr->mExpr.mListCol,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sNewExpr->mExpr.mListCol->mPredicate =
                    aSourceExpr->mExpr.mListCol->mPredicate;

                sNewExpr->mExpr.mListCol->mArgCount =
                    aSourceExpr->mExpr.mListCol->mArgCount;

                sNewExpr->mExpr.mListCol->mArgs =
                    (qlvInitExpression **) ( ((stlChar*) sNewExpr->mExpr.mListCol) + STL_SIZEOF( qlvInitListCol ) );
            
                sNewExpr->mExpr.mListCol->mEntry =
                    (knlListEntry **) ( ((stlChar*) sNewExpr->mExpr.mListCol->mArgs)
                                        + STL_SIZEOF( qlvInitExpression* ) * sArgCount );
                
                while( sArgCount > 0 )
                {
                    sArgCount--;
                    sNewExpr->mExpr.mListCol->mArgs[ sArgCount ] =
                        aSourceExpr->mExpr.mListCol->mArgs[ sArgCount ];

                    sNewExpr->mExpr.mListCol->mEntry[ sArgCount ] =
                        aSourceExpr->mExpr.mListCol->mEntry[ sArgCount ];

                }

                break;
            }
            
        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                sArgCount = aSourceExpr->mExpr.mRowExpr->mArgCount;
                STL_DASSERT( sArgCount > 0 );

                sAllocSize = STL_SIZEOF( qlvInitRowExpr ) +
                    ( STL_SIZEOF( qlvInitExpression* ) * sArgCount ) +
                    ( STL_SIZEOF( knlListEntry ) * sArgCount );
                
                STL_TRY( knlAllocRegionMem(
                             aRegionMem,
                             sAllocSize,
                             (void **) & sNewExpr->mExpr.mRowExpr,
                             KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sNewExpr->mExpr.mRowExpr->mArgCount =
                    aSourceExpr->mExpr.mRowExpr->mArgCount;
                
                sNewExpr->mExpr.mRowExpr->mIsRowSubQuery =
                    aSourceExpr->mExpr.mRowExpr->mIsRowSubQuery;

                sNewExpr->mExpr.mRowExpr->mArgs =
                    (qlvInitExpression **) ( ((stlChar*) sNewExpr->mExpr.mRowExpr) + STL_SIZEOF( qlvInitRowExpr ) );

                sNewExpr->mExpr.mRowExpr->mEntry =
                    (knlListEntry *) ( ((stlChar*) sNewExpr->mExpr.mRowExpr->mArgs )
                                       + STL_SIZEOF( qlvInitExpression* ) * sArgCount );

                sNewExpr->mExpr.mRowExpr->mSubQueryExpr =
                    aSourceExpr->mExpr.mRowExpr->mSubQueryExpr;
                                
                while( sArgCount > 0 )
                {
                    sArgCount--;
                    sNewExpr->mExpr.mRowExpr->mArgs[ sArgCount ] =
                        aSourceExpr->mExpr.mRowExpr->mArgs[ sArgCount ];
                
                    sNewExpr->mExpr.mRowExpr->mEntry[ sArgCount ] =
                        aSourceExpr->mExpr.mRowExpr->mEntry[ sArgCount ];
                }
                
                break;
            }
            
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    /**
     * OUTPUT 설정
     */

    *aDestExpr = sNewExpr;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Code Expression 복사본 생성
 * @param[in]     aSourceExpr  Source Expression 
 * @param[out]    aDestExpr    Dest Expression
 * @param[in]     aRegionMem   Region Memory
 * @param[in]     aEnv         Environment
 * @remarks
 */
stlStatus qlvDuplicateExpr( qlvInitExpression   * aSourceExpr,
                            qlvInitExpression  ** aDestExpr,
                            knlRegionMem        * aRegionMem,
                            qllEnv              * aEnv )
{
    qlvInitExpression   * sSourceExpr   = NULL;
    qlvInitExpression   * sNewExpr      = NULL;
    qlvInitExpression   * sTempExpr     = NULL;

    qlvInitTypeCast     * sNewCast      = NULL;
    qlvInitTypeCast     * sOldCast      = NULL;
    qlvInitFunction     * sNewFunc      = NULL;
    qlvInitFunction     * sOldFunc      = NULL;
    qlvInitAggregation  * sNewAggr      = NULL;
    qlvInitAggregation  * sOldAggr      = NULL;
    qlvInitCaseExpr     * sNewCaseExpr  = NULL;
    qlvInitCaseExpr     * sOldCaseExpr  = NULL;
    
    qlvInitListFunc     * sNewListFunc  = NULL;
    qlvInitListFunc     * sOldListFunc  = NULL;
    qlvInitListCol      * sNewListCol   = NULL;
    qlvInitListCol      * sOldListCol   = NULL;
    qlvInitRowExpr      * sNewRowExpr   = NULL;
    qlvInitRowExpr      * sOldRowExpr   = NULL;
    stlInt32              sLoop         = 0;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSourceExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDestExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    sSourceExpr = aSourceExpr;


    /**
     * 현재 Expression 복사
     */

    STL_TRY( qlvCopyExpr( sSourceExpr,
                          & sNewExpr,
                          aRegionMem,
                          aEnv )
             == STL_SUCCESS );
    

    /**
     * 각 Expression 의 종류에 따라 Expression 내용 재구성
     */
    
    switch( sSourceExpr->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
            {
                /* Do Nothing */
                break;
            }
            
        case QLV_EXPR_TYPE_BIND_PARAM :
            {
                /* Do Nothing */
                break;
            }

        case QLV_EXPR_TYPE_COLUMN :
            {
                /* Do Nothing */
                break;
            }

        case QLV_EXPR_TYPE_FUNCTION :
            {
                sNewFunc = sNewExpr->mExpr.mFunction;
                sOldFunc = sSourceExpr->mExpr.mFunction;
                    
                /**
                 * Argument List 공간 할당
                 */

                if( sOldFunc->mArgCount == 0 )
                {
                    sNewFunc->mArgs = NULL;
                }
                else
                {
                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                STL_SIZEOF( qlvInitExpression* ) * sOldFunc->mArgCount,
                                                (void **) & sNewFunc->mArgs,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                
                for( sLoop = 0 ; sLoop < sOldFunc->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvDuplicateExpr( sOldFunc->mArgs[ sLoop ],
                                               & sTempExpr,
                                               aRegionMem,
                                               aEnv )
                             == STL_SUCCESS );

                    sNewFunc->mArgs[ sLoop ] = sTempExpr;
                }
                break;
            }

        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
            {
                /* Do Nothing */
                break;
            }

        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
            {
                /* Do Nothing */
                break;
            }

        case QLV_EXPR_TYPE_CAST :
            {
                sNewCast = sNewExpr->mExpr.mTypeCast;
                sOldCast = sSourceExpr->mExpr.mTypeCast;
                    
                /**
                 * Argument List 공간 할당
                 */

                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            STL_SIZEOF( qlvInitExpression* ) * DTL_CAST_INPUT_ARG_CNT,
                                            (void **) & sNewCast->mArgs,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
        
                for( sLoop = 0 ; sLoop < DTL_CAST_INPUT_ARG_CNT ; sLoop++ )
                {
                    STL_TRY( qlvDuplicateExpr( sOldCast->mArgs[ sLoop ],
                                               & sTempExpr,
                                               aRegionMem,
                                               aEnv )
                             == STL_SUCCESS );

                    sNewCast->mArgs[ sLoop ] = sTempExpr;
                }
                break;
            }

        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
            {
                /* Do Nothing */
                break;
            }

        case QLV_EXPR_TYPE_REFERENCE :
            {
                /* Do Nothing */
                break;
            }

        case QLV_EXPR_TYPE_SUB_QUERY :
            {
                /* Do Nothing */
                break;
            }
            
        case QLV_EXPR_TYPE_INNER_COLUMN :
            {
                STL_TRY( qlvDuplicateExpr( sSourceExpr->mExpr.mInnerColumn->mOrgExpr,
                                           & sTempExpr,
                                           aRegionMem,
                                           aEnv )
                         == STL_SUCCESS );

                sNewExpr->mExpr.mInnerColumn->mOrgExpr = sTempExpr;
                break;
            }

        case QLV_EXPR_TYPE_OUTER_COLUMN :
            {
                STL_TRY( qlvDuplicateExpr( sSourceExpr->mExpr.mOuterColumn->mOrgExpr,
                                           & sTempExpr,
                                           aRegionMem,
                                           aEnv )
                         == STL_SUCCESS );

                sNewExpr->mExpr.mOuterColumn->mOrgExpr = sTempExpr;
                break;
            }
        case QLV_EXPR_TYPE_ROWID_COLUMN :
            {
                /* Do Nothing */
                break;
            }

        case QLV_EXPR_TYPE_AGGREGATION :
            {
                sNewAggr = sNewExpr->mExpr.mAggregation;
                sOldAggr = sSourceExpr->mExpr.mAggregation;
                    
                /**
                 * Argument List 공간 할당
                 */

                if( sOldAggr->mArgCount > 0 )
                {
                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                STL_SIZEOF( qlvInitExpression* ) * sOldAggr->mArgCount,
                                                (void **) & sNewAggr->mArgs,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
        
                    for( sLoop = 0 ; sLoop < sOldAggr->mArgCount ; sLoop++ )
                    {
                        STL_TRY( qlvDuplicateExpr( sOldAggr->mArgs[ sLoop ],
                                                   & sTempExpr,
                                                   aRegionMem,
                                                   aEnv )
                                 == STL_SUCCESS );

                        sNewAggr->mArgs[ sLoop ] = sTempExpr;
                    }
                }
                else
                {
                    sNewAggr->mArgs = NULL;
                }
                break;
            }

        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                sNewCaseExpr = sNewExpr->mExpr.mCaseExpr;
                sOldCaseExpr = sSourceExpr->mExpr.mCaseExpr;

                /**
                 * Argument List 공간 할당
                 */

                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            STL_SIZEOF( qlvInitExpression* ) *
                                            ( sOldCaseExpr->mCount * 2 + 1 ),
                                            (void **) & sNewCaseExpr->mWhenArr,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                sNewCaseExpr->mThenArr =
                    (qlvInitExpression **)( ((stlChar*) sNewCaseExpr->mWhenArr)
                                            + STL_SIZEOF( qlvInitExpression* ) * sOldCaseExpr->mCount );
                
                sNewCaseExpr->mDefResult =
                    (qlvInitExpression *)( ((stlChar*) sNewCaseExpr->mThenArr)
                                           + STL_SIZEOF( qlvInitExpression* ) * sOldCaseExpr->mCount );

                for( sLoop = 0; sLoop < sOldCaseExpr->mCount; sLoop++ )
                {
                    STL_TRY( qlvDuplicateExpr( sOldCaseExpr->mWhenArr[sLoop],
                                               & sTempExpr,
                                               aRegionMem,
                                               aEnv )
                             == STL_SUCCESS );

                    sNewCaseExpr->mWhenArr[sLoop] = sTempExpr;

                    STL_TRY( qlvDuplicateExpr( sOldCaseExpr->mThenArr[sLoop],
                                               & sTempExpr,
                                               aRegionMem,
                                               aEnv )
                             == STL_SUCCESS );

                    sNewCaseExpr->mThenArr[sLoop] = sTempExpr;
                }

                STL_TRY( qlvDuplicateExpr( sOldCaseExpr->mDefResult,
                                           & sTempExpr,
                                           aRegionMem,
                                           aEnv )
                         == STL_SUCCESS );
                
                sNewCaseExpr->mDefResult = sTempExpr;
                
                break;
            }

        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                sNewListFunc = sNewExpr->mExpr.mListFunc;
                sOldListFunc = sSourceExpr->mExpr.mListFunc;
                    
                /**
                 * Argument List 공간 할당
                 */

                STL_DASSERT( sOldListFunc->mArgCount > 0 );

                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            STL_SIZEOF( qlvInitExpression* ) * sOldListFunc->mArgCount,
                                            (void **) & sNewListFunc->mArgs,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                for( sLoop = 0 ; sLoop < sOldListFunc->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvDuplicateExpr( sOldListFunc->mArgs[ sLoop ],
                                               & sTempExpr,
                                               aRegionMem,
                                               aEnv )
                             == STL_SUCCESS );

                    sNewListFunc->mArgs[ sLoop ] = sTempExpr;
                }
                break;
            }

        case QLV_EXPR_TYPE_LIST_COLUMN :
            {
                sNewListCol = sNewExpr->mExpr.mListCol;
                sOldListCol = sSourceExpr->mExpr.mListCol;
                    
                /**
                 * Argument List 공간 할당
                 */

                STL_DASSERT( sOldListCol->mArgCount > 0 );

                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            STL_SIZEOF( qlvInitExpression* ) * sOldListCol->mArgCount,
                                            (void **) & sNewListCol->mArgs,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                    
                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            STL_SIZEOF( knlListEntry* ) * sOldListCol->mArgCount,
                                            (void **) & sNewListCol->mEntry,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                for( sLoop = 0 ; sLoop < sOldListCol->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvDuplicateExpr( sOldListCol->mArgs[ sLoop ],
                                               & sTempExpr,
                                               aRegionMem,
                                               aEnv )
                             == STL_SUCCESS );

                    sNewListCol->mArgs[ sLoop ] = sTempExpr;
                }
                break;
            }
            
        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                sNewRowExpr = sNewExpr->mExpr.mRowExpr;
                sOldRowExpr = sSourceExpr->mExpr.mRowExpr;
                    
                /**
                 * Argument List 공간 할당
                 */

                STL_DASSERT( sOldRowExpr->mArgCount > 0 );

                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            STL_SIZEOF( qlvInitExpression* ) * sOldRowExpr->mArgCount,
                                            (void **) & sNewRowExpr->mArgs,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                                        
                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            STL_SIZEOF( knlListEntry ) * sOldRowExpr->mArgCount,
                                            (void *) & sNewRowExpr->mEntry,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );
                
                for( sLoop = 0 ; sLoop < sOldRowExpr->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvDuplicateExpr( sOldRowExpr->mArgs[ sLoop ],
                                               & sTempExpr,
                                               aRegionMem,
                                               aEnv )
                             == STL_SUCCESS );

                    sNewRowExpr->mArgs[ sLoop ] = sTempExpr;
                }
                break;
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }


    /**
     * OUTPUT 설정
     */
    
    *aDestExpr = sNewExpr;
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Reference Expression List 생성
 * @param[out]     aRefExprList    생성된 Reference Expression List
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlvCreateRefExprList( qlvRefExprList  ** aRefExprList,
                                knlRegionMem     * aRegionMem,
                                qllEnv           * aEnv )
{
    qlvRefExprList  * sRefExprList = NULL;

    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvRefExprList ),
                                (void **) & sRefExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * 초기화
     */
    
    sRefExprList->mCount = 0;
    sRefExprList->mHead  = NULL;
    sRefExprList->mTail  = NULL;


    /**
     * OUTPUT 설정
     */
    
    *aRefExprList = sRefExprList;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Reference Expression List에 Expression 추가 
 * @param[in,out]  aRefExprList    Reference Expression List
 * @param[in]      aInitExpr       Init Expression
 * @param[in]      aIsSetOffset    Expression의 Offset 설정 여부
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 *
 * @remark Statement 단위의 Expression List를 구성할 경우에만
 *    <BR> aIsSetOffset을 STL_TRUE로 설정하여 Expression의 Offset 설정한다.
 */
stlStatus qlvAddExprToRefExprList( qlvRefExprList     * aRefExprList,
                                   qlvInitExpression  * aInitExpr,
                                   stlBool              aIsSetOffset,
                                   knlRegionMem       * aRegionMem,
                                   qllEnv             * aEnv )
{
    qlvRefExprItem  * sRefExprItem = NULL;

    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvRefExprItem ),
                                (void **) & sRefExprItem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Expression Idx 설정
     */

    if( aIsSetOffset == STL_TRUE )
    {
        aInitExpr->mOffset = aRefExprList->mCount;
        aInitExpr->mIsSetOffset = STL_TRUE;
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Item 초기화
     */
    
    sRefExprItem->mExprPtr = aInitExpr;
    sRefExprItem->mNext    = NULL;


    /**
     * List 설정
     */

    if( aRefExprList->mCount == 0 )
    {
        aRefExprList->mCount = 1;
        aRefExprList->mHead  = sRefExprItem;
        aRefExprList->mTail  = sRefExprItem;
    }
    else
    {
        aRefExprList->mCount++;
        aRefExprList->mTail->mNext = sRefExprItem;
        aRefExprList->mTail        = sRefExprItem;
    }
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Reference Expression List에 Expression 을 제거한다.
 * @param[in,out]  aRefExprList    Reference Expression List
 * @param[in]      aInitExpr       Init Expression
 * @param[in]      aEnv            Environment
 */
stlStatus qlvRemoveExprToRefExprList( qlvRefExprList     * aRefExprList,
                                      qlvInitExpression  * aInitExpr,
                                      qllEnv             * aEnv )
{
    qlvRefExprItem  * sRefExprItem  = NULL;
    qlvRefExprItem  * sPrevExprItem = NULL;

    STL_PARAM_VALIDATE( aRefExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Item 초기화
     */

    sRefExprItem = aRefExprList->mHead;

    while( sRefExprItem != NULL )
    {
        if( sRefExprItem->mExprPtr == aInitExpr )
        {
            if( aRefExprList->mCount == 1 )
            {
                aRefExprList->mHead  = NULL;
                aRefExprList->mTail  = NULL;
                aRefExprList->mCount = 0;
            }
            else
            {
                if( sPrevExprItem == NULL )
                {
                    aRefExprList->mHead = sRefExprItem->mNext;
                }
                else
                {
                    if( sPrevExprItem->mNext == NULL )
                    {
                        aRefExprList->mTail  = sPrevExprItem;
                        sPrevExprItem->mNext = NULL;
                    }
                    else
                    {
                        sPrevExprItem->mNext = sRefExprItem->mNext;
                    }
                }
                
                aRefExprList->mCount--;
            }
            break;
        }
        else
        {
            sPrevExprItem = sRefExprItem;
            sRefExprItem  = sRefExprItem->mNext;
        }
    }
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Column Expression List에 Column Expression 추가 
 * @param[in,out]  aRefColExprList Column Expression List
 * @param[in]      aInitExpr       Init Column Expression
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 *
 * @remark Expression의 Offset 설정하지 않는다.
 */
stlStatus qlvAddExprToRefColExprList( qlvRefExprList     * aRefColExprList,
                                      qlvInitExpression  * aInitExpr,
                                      knlRegionMem       * aRegionMem,
                                      qllEnv             * aEnv )
{
    qlvRefExprItem     * sPrevExprItem = NULL;
    qlvRefExprItem     * sCurExprItem  = NULL;
    qlvRefExprItem     * sNewExprItem  = NULL;

    stlInt32             sSrcIdx       = 0;
    stlInt32             sCurIdx       = 0;

    /*
     * Param Validation
     */
    
    STL_PARAM_VALIDATE( aRefColExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( (aInitExpr->mType == QLV_EXPR_TYPE_COLUMN) ||
                          (aInitExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN) ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Expression 이 삽입될 List 내의 위치 찾기
     */

    if( aRefColExprList->mHead == NULL )
    {
        /**
         * 공간 할당
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvRefExprItem ),
                                    (void **) & sNewExprItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        
        /**
         * List 구성
         */

        sNewExprItem->mExprPtr  = aInitExpr;
        sNewExprItem->mNext     = NULL;
        aRefColExprList->mHead  = sNewExprItem;
        aRefColExprList->mTail  = sNewExprItem;
        aRefColExprList->mCount = 1;
    }
    else
    {
        if( aInitExpr->mType == QLV_EXPR_TYPE_COLUMN )
        {
            /**
             * Expr List에는 column만 존재한다는 것이 보장된다.
             */

            sPrevExprItem = NULL;
            sCurExprItem  = aRefColExprList->mHead;
            sSrcIdx       = ellGetColumnIdx( aInitExpr->mExpr.mColumn->mColumnHandle );

            while( sCurExprItem != NULL )
            {
                STL_DASSERT( sCurExprItem->mExprPtr->mType == QLV_EXPR_TYPE_COLUMN );

                sCurIdx = ellGetColumnIdx( sCurExprItem->mExprPtr->mExpr.mColumn->mColumnHandle );
                STL_DASSERT( ellGetColumnTableID( aInitExpr->mExpr.mColumn->mColumnHandle )
                             == ellGetColumnTableID( sCurExprItem->mExprPtr->mExpr.mColumn->mColumnHandle ) );

                if( sSrcIdx <= sCurIdx )
                {
                    break;
                }

                sPrevExprItem = sCurExprItem;
                sCurExprItem  = sCurExprItem->mNext;
            }
        }
        else
        {
            /**
             * Expr List에는 inner column만 존재한다는 것이 보장된다.
             */

            STL_DASSERT( aInitExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

            sPrevExprItem = NULL;
            sCurExprItem  = aRefColExprList->mHead;
            sSrcIdx       = *(aInitExpr->mExpr.mInnerColumn->mIdx);

            while( sCurExprItem != NULL )
            {
                STL_DASSERT( sCurExprItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

                sCurIdx = *(sCurExprItem->mExprPtr->mExpr.mInnerColumn->mIdx);
                STL_DASSERT( aInitExpr->mExpr.mInnerColumn->mRelationNode
                             == sCurExprItem->mExprPtr->mExpr.mInnerColumn->mRelationNode );

                if( sSrcIdx <= sCurIdx )
                {
                    break;
                }

                sPrevExprItem = sCurExprItem;
                sCurExprItem  = sCurExprItem->mNext;
            }
        }


        /**
         * 동일한 Column ID를 갖는 Expression 존재하는지 판단
         */

        if( sSrcIdx == sCurIdx )
        {
            /* 동일한 Column ID 갖는 Expression 존재하는 경우 */
            /* Do Nothing */
        }
        else
        {
            /* 동일한 Column ID 갖는 Expression 존재하지 않는 경우 */

            /**
             * 공간 할당
             */

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( qlvRefExprItem ),
                                        (void **) & sNewExprItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );


            /**
             * List 구성
             */

            if( sCurExprItem == NULL )
            {
                /* list 맨 마지막에 추가 */
                sNewExprItem->mExprPtr = aInitExpr;
                sNewExprItem->mNext    = NULL;

                aRefColExprList->mTail->mNext = sNewExprItem;
                aRefColExprList->mTail        = sNewExprItem;
            }
            else
            {
                if( sPrevExprItem == NULL )
                {
                    /* list의 맨 앞에 추가 */
                    sNewExprItem->mExprPtr = aInitExpr;
                    sNewExprItem->mNext    = aRefColExprList->mHead;

                    aRefColExprList->mHead = sNewExprItem;
                }
                else
                {
                    /* prev item 이후에 추가 */
                    sNewExprItem->mExprPtr = aInitExpr;
                    sNewExprItem->mNext    = sCurExprItem;

                    sPrevExprItem->mNext = sNewExprItem;
                }
            }

            aRefColExprList->mCount++;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Reference Expression List에 Expression 추가 
 * @param[in,out]  aDestExprList   Destination Expression List
 * @param[in]      aSrcExprList    Source Expression List 
 * @param[in]      aIsSetOffset    Expression의 Offset 설정 여부
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlvAddExprListToRefExprList( qlvRefExprList     * aDestExprList,
                                       qlvRefExprList     * aSrcExprList,
                                       stlBool              aIsSetOffset,
                                       knlRegionMem       * aRegionMem,
                                       qllEnv             * aEnv )
{
    stlInt32          sCount       = 0;
    qlvRefExprItem  * sNewExprItem = NULL;
    qlvRefExprItem  * sSrcExprItem = NULL;


    /*
     * Param Validation
     */
    
    STL_PARAM_VALIDATE( aDestExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSrcExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Source Expr List 내의 모든 Expression을 Dest Expr List에 삽입
     */
    
    sSrcExprItem = aSrcExprList->mHead;
    
    for( sCount = 0 ; sCount < aSrcExprList->mCount ; sCount++ )
    {
        STL_DASSERT( sSrcExprItem != NULL );


        /**
         * 공간 할당
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvRefExprItem ),
                                    (void **) & sNewExprItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        

        /**
         * Expression Idx 설정
         */

        if( aIsSetOffset == STL_TRUE )
        {
            /* 현재는 list 단위로 list element 로 추가하면서 offset을 설정하는 경우는 없다. */
            sSrcExprItem->mExprPtr->mOffset = aDestExprList->mCount;
            sSrcExprItem->mExprPtr->mIsSetOffset = STL_TRUE;
        }
        else
        {
            /* Do Nothing */
        }


        /**
         * Item 초기화
         */
    
        sNewExprItem->mExprPtr = sSrcExprItem->mExprPtr;
        sNewExprItem->mNext    = NULL;


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


    STL_DASSERT( sSrcExprItem == NULL );
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Column List 내의 Column 값을 변경시 Index 가 영향을 받는지 검사
 * @param[in]  aIndexDictHandle  Index Dictionary Handle
 * @param[in]  aWriteColumnList  Write Column List
 * @param[out] aIsAffected       영향을 받는지의 여부
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus qlvIsAffectedIndexByColumnList( ellDictHandle   * aIndexDictHandle,
                                          qlvRefExprList  * aWriteColumnList,
                                          stlBool         * aIsAffected,
                                          qllEnv          * aEnv )
{
    stlInt32             sKeyCnt   = 0;
    ellIndexKeyDesc    * sKeyDesc  = NULL;

    qlvRefExprItem     * sExprItem = NULL;
    qlvInitExpression  * sExpr     = NULL;
    qlvInitColumn      * sColumn   = NULL;
    
    stlInt32             sLoop     = 0;
    stlBool              sFound    = STL_FALSE;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aIndexDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aWriteColumnList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIsAffected != NULL, ELL_ERROR_STACK(aEnv) );

    
    /**
     * Index Key 정보 획득
     */
    
    sKeyCnt  = ellGetIndexKeyCount( aIndexDictHandle );
    sKeyDesc = ellGetIndexKeyDesc( aIndexDictHandle );

    
    /**
     * Table Value List 와 동일한 Index Key 정보가 있는지 검사 
     */

    sFound    = STL_FALSE;
    sExprItem = aWriteColumnList->mHead;

    while( sExprItem != NULL )
    {
        /**
         * Column Expression 가져오기
         */
        
        sExpr = sExprItem->mExprPtr;
        
        STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_COLUMN );

        sColumn = sExpr->mExpr.mColumn;
        
        STL_DASSERT( sColumn->mRelationNode->mType == QLV_NODE_TYPE_BASE_TABLE );
        STL_DASSERT( ellGetTableID( & ((qlvInitBaseTableNode *) sColumn->mRelationNode)->mTableHandle ) ==
                     ellGetColumnTableID( sColumn->mColumnHandle ) );


        /**
         * Index Key Column으로 해당 Column을 가지는지 여부 확인
         */

        for( sLoop = 0 ; sLoop < sKeyCnt ; sLoop++ )
        {
            /**
             * Column ID 가 동일한 지 검사 
             */

            if ( ellGetColumnID( sColumn->mColumnHandle )
                 == ellGetColumnID( & sKeyDesc[ sLoop ].mKeyColumnHandle ) )
            {
                /* Index Key 와 동일한 Column 의 Value List 가 존재함 */
                sFound = STL_TRUE;
                break;
            }
            else
            {
                continue;
            }
        }
        
        if( sFound == STL_TRUE )
        {
            break;
        }
        else
        {
            sExprItem = sExprItem->mNext;
        }
    }
    

    /**
     * Output 설정
     */

    *aIsAffected = sFound;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index내의 Key Column들을 Base Table Node의 Reference Column List에 추가
 * @param[in,out]  aBaseTableNode   Base Table Node
 * @param[in]      aIndexDictHandle Index Handle
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvAddRefColumnOnBaseTableByIndex( qlvInitBaseTableNode  * aBaseTableNode,
                                             ellDictHandle         * aIndexDictHandle,
                                             knlRegionMem          * aRegionMem,
                                             qllEnv                * aEnv )
{
    ellDictHandle       * sColumnHandle      = NULL;

    stlInt32              sLoop              = 0;
    stlInt32              sKeyCnt            = 0;
    ellIndexKeyDesc     * sKeyDesc           = NULL;

    qlvRefExprItem      * sRefColumnItem     = NULL;
    qlvRefExprItem      * sPrevRefColumnItem = NULL;

    qlvInitColumn       * sCodeColumn        = NULL;
    qlvInitExpression   * sCodeExpr          = NULL;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aBaseTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexDictHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Reference Column List에 Index Key Column들 추가
     **********************************************************/

    /**
     * Index Key 정보 획득
     */
    
    sKeyCnt  = ellGetIndexKeyCount( aIndexDictHandle );
    sKeyDesc = ellGetIndexKeyDesc( aIndexDictHandle );


    /**
     * Reference Column List에 Index Key Column 추가
     */

    for( sLoop = 0 ; sLoop < sKeyCnt ; sLoop++ )
    {
        /**
         * Reference Column List에서 Column Handle을 찾음
         */

        sColumnHandle       = & sKeyDesc[ sLoop ].mKeyColumnHandle;
        sRefColumnItem      = aBaseTableNode->mRefColumnList->mHead;
        sPrevRefColumnItem  = NULL;
        
        while( sRefColumnItem != NULL )
        {
            if( ellGetColumnID( sColumnHandle ) <=
                ellGetColumnID( sRefColumnItem->mExprPtr->mExpr.mColumn->mColumnHandle ) )
            {
                break;
            }

            sPrevRefColumnItem  = sRefColumnItem;
            sRefColumnItem      = sRefColumnItem->mNext;
        }

    
        /**
         * Reference Column List 구성
         */
    
        if( sRefColumnItem != NULL )
        {
            if( ellGetColumnID( sColumnHandle ) ==
                ellGetColumnID( sRefColumnItem->mExprPtr->mExpr.mColumn->mColumnHandle ) )
            {
                /**
                 * 이미 동일한 Column이 List에 존재하는 경우
                 */

                /* 다음 key column 평가 */
                continue;
            }
            else
            {
                /**
                 * Reference Column List 에 Column 추가
                 */

                /* Do Nothing */
            }
        }
        else
        {


            /**
             * Reference Column List 에 Column 추가
             */

            /* Do Nothing */
        }


        /**
         * Reference Column List 에 Column 추가하기
         */

        /* Column Expr 생성 */
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvInitColumn ),
                                    (void **) & sCodeColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Column Expr 설정 */
        sCodeColumn->mColumnHandle = sColumnHandle;
        sCodeColumn->mRelationNode = (qlvInitNode *) aBaseTableNode;


        /**
         * Target Expr 설정 : CODE EXPRESSION 생성
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvInitExpression ),
                                    (void **) & sCodeExpr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sCodeExpr,
                   0x00,
                   STL_SIZEOF( qlvInitExpression ) );

        QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );
        
        sCodeExpr->mType           = QLV_EXPR_TYPE_COLUMN;
        sCodeExpr->mPosition       = 0;
        sCodeExpr->mIncludeExprCnt = 1;
        sCodeExpr->mExpr.mColumn   = sCodeColumn;
        sCodeExpr->mIterationTime  = DTL_ITERATION_TIME_FOR_EACH_EXPR;

        /* unused column 은 참조할 수 없다. */
        STL_DASSERT( ellGetColumnName( sColumnHandle ) != NULL );

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    stlStrlen( ellGetColumnName( sColumnHandle ) ) + 1,
                                    (void **) & sCodeExpr->mColumnName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlStrcpy( sCodeExpr->mColumnName, ellGetColumnName( sColumnHandle ) );

        
        /**
         * Reference Column Item 생성
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvRefExprItem ),
                                    (void **) & sRefColumnItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRefColumnItem->mExprPtr = sCodeExpr;
        sRefColumnItem->mNext    = NULL;


        /**
         * New Reference Column Item을
         * Previous Reference Column Item 다음에 추가
         */

        if( sPrevRefColumnItem == NULL )
        {
            /**
             * Head에 추가해야 되는 경우
             */

            if( aBaseTableNode->mRefColumnList->mCount == 0 )
            {
                aBaseTableNode->mRefColumnList->mHead = sRefColumnItem;
                aBaseTableNode->mRefColumnList->mTail = sRefColumnItem;
            }
            else
            {
                sRefColumnItem->mNext                 = aBaseTableNode->mRefColumnList->mHead;
                aBaseTableNode->mRefColumnList->mHead = sRefColumnItem;
            }
        }
        else
        {
            /**
             * 중간에 추가해야 되는 경우
             */

            if( sPrevRefColumnItem->mNext == NULL )
            {
                sPrevRefColumnItem->mNext               = sRefColumnItem;
                aBaseTableNode->mRefColumnList->mTail   = sRefColumnItem;
            }
            else
            {
                sRefColumnItem->mNext       = sPrevRefColumnItem->mNext;
                sPrevRefColumnItem->mNext   = sRefColumnItem;
            }
        }

        aBaseTableNode->mRefColumnList->mCount++;
    }
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief NOT NULL, CHECK constrint 내의 Column들을 Base Table Node의 Reference Column List에 추가
 * @param[in,out]  aBaseTableNode   Base Table Node
 * @param[in]      aColumnHandle    Constraint Column Handle
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvAddRefColumnOnBaseTableByConstraintColumn( qlvInitBaseTableNode  * aBaseTableNode,
                                                        ellDictHandle         * aColumnHandle,
                                                        knlRegionMem          * aRegionMem,
                                                        qllEnv                * aEnv )
{
    qlvRefExprItem      * sRefColumnItem     = NULL;
    qlvRefExprItem      * sPrevRefColumnItem = NULL;

    qlvInitColumn       * sCodeColumn        = NULL;
    qlvInitExpression   * sCodeExpr          = NULL;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aBaseTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Reference Column List에 Constraint Column들 추가
     **********************************************************/

    /**
     * Reference Column List에서 Column Handle을 찾음
     */
    
    sRefColumnItem      = aBaseTableNode->mRefColumnList->mHead;
    sPrevRefColumnItem  = NULL;
        
    while( sRefColumnItem != NULL )
    {
        if( ellGetColumnID( aColumnHandle ) <=
            ellGetColumnID( sRefColumnItem->mExprPtr->mExpr.mColumn->mColumnHandle ) )
        {
            break;
        }
        
        sPrevRefColumnItem  = sRefColumnItem;
        sRefColumnItem      = sRefColumnItem->mNext;
    }
    
    /**
     * Reference Column List 구성
     */
    
    if( sRefColumnItem != NULL )
    {
        if( ellGetColumnID( aColumnHandle ) ==
            ellGetColumnID( sRefColumnItem->mExprPtr->mExpr.mColumn->mColumnHandle ) )
        {
            /**
             * 이미 동일한 Column이 List에 존재하는 경우
             */

            STL_THROW( RAMP_FINISH );
        }
        else
        {
            /**
             * Reference Column List 에 Column 추가
             */

            /* Do Nothing */
        }
    }
    else
    {
        /**
         * Reference Column List 에 Column 추가
         */

        /* Do Nothing */
    }

    /**
     * Reference Column List 에 Column 추가하기
     */

    /* Column Expr 생성 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitColumn ),
                                (void **) & sCodeColumn,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Column Expr 설정 */
    sCodeColumn->mColumnHandle = aColumnHandle;
    sCodeColumn->mRelationNode = (qlvInitNode *) aBaseTableNode;


    /**
     * Target Expr 설정 : CODE EXPRESSION 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sCodeExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sCodeExpr,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

    QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );
    
    sCodeExpr->mType           = QLV_EXPR_TYPE_COLUMN;
    sCodeExpr->mPosition       = 0;
    sCodeExpr->mIncludeExprCnt = 1;
    sCodeExpr->mExpr.mColumn   = sCodeColumn;
    sCodeExpr->mIterationTime  = DTL_ITERATION_TIME_FOR_EACH_EXPR;

    /* unused column 은 참조할 수 없다. */
    STL_DASSERT( ellGetColumnName( aColumnHandle ) != NULL );

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                stlStrlen( ellGetColumnName( aColumnHandle ) ) + 1,
                                (void **) & sCodeExpr->mColumnName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlStrcpy( sCodeExpr->mColumnName, ellGetColumnName( aColumnHandle ) );

        
    /**
     * Reference Column Item 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvRefExprItem ),
                                (void **) & sRefColumnItem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sRefColumnItem->mExprPtr = sCodeExpr;
    sRefColumnItem->mNext    = NULL;


    /**
     * New Reference Column Item을
     * Previous Reference Column Item 다음에 추가
     */

    if( sPrevRefColumnItem == NULL )
    {
        /**
         * Head에 추가해야 되는 경우
         */

        if( aBaseTableNode->mRefColumnList->mCount == 0 )
        {
            aBaseTableNode->mRefColumnList->mHead = sRefColumnItem;
            aBaseTableNode->mRefColumnList->mTail = sRefColumnItem;
        }
        else
        {
            sRefColumnItem->mNext                 = aBaseTableNode->mRefColumnList->mHead;
            aBaseTableNode->mRefColumnList->mHead = sRefColumnItem;
        }
    }
    else
    {
        /**
         * 중간에 추가해야 되는 경우
         */

        if( sPrevRefColumnItem->mNext == NULL )
        {
            sPrevRefColumnItem->mNext               = sRefColumnItem;
            aBaseTableNode->mRefColumnList->mTail   = sRefColumnItem;
        }
        else
        {
            sRefColumnItem->mNext       = sPrevRefColumnItem->mNext;
            sPrevRefColumnItem->mNext   = sRefColumnItem;
        }
    }

    aBaseTableNode->mRefColumnList->mCount++;

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expression을 분석하여 참조하는 Column List를 구성한다.
 * @param[in]     aOrgExpr        Init Original Expression
 * @param[in]     aCurExpr        Init Current Expression
 * @param[in,out] aColExprList    Expression이 참조하고 있는 Column List
 * @param[in]     aRegionMem      Region Memory
 * @param[in]     aEnv            Environment
 */
stlStatus qlvGetRefColList( qlvInitExpression   * aOrgExpr,
                            qlvInitExpression   * aCurExpr,
                            qlvRefColExprList   * aColExprList,
                            knlRegionMem        * aRegionMem,
                            qllEnv              * aEnv )
{
    qlvInitExpression  * sInitExprCode      = NULL;
    stlInt32             sLoop              = 0;

    qlvInitColumn      * sInitColumn        = NULL;
    qlvInitInnerColumn * sInitInnerColumn   = NULL;
    qlvInitFunction    * sInitFunction      = NULL;
    qlvInitTypeCast    * sInitTypeCast      = NULL;
    qlvInitCaseExpr    * sInitCaseExpr      = NULL;
    qlvRefColListItem  * sColExprItem       = NULL;
    qlvInitListFunc    * sInitListFunc      = NULL;
    qlvInitListCol     * sInitListCol       = NULL;
    qlvInitRowExpr     * sInitRowExpr       = NULL;    
    
        
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aOrgExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCurExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    sInitExprCode = aCurExpr;
    STL_DASSERT( sInitExprCode->mType < QLV_EXPR_TYPE_MAX );

    
    /**
     * Column List 구성
     */

    switch( sInitExprCode->mType )
    {
        case QLV_EXPR_TYPE_COLUMN :
            {
                /**
                 * Column List에 Column 추가
                 */

                sInitColumn = sInitExprCode->mExpr.mColumn;

                sColExprItem = aColExprList->mHead;
                while( sColExprItem != NULL )
                {
                    if( sColExprItem->mRelationNode == sInitColumn->mRelationNode )
                    {
                        break;
                    }
                    else
                    {
                        sColExprItem = sColExprItem->mNext;
                    }
                }

                if( sColExprItem == NULL )
                {
                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                STL_SIZEOF( qlvRefColListItem ),
                                                (void **) & sColExprItem,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    STL_TRY( qlvCreateRefExprList( & sColExprItem->mColExprList,
                                                   aRegionMem,
                                                   aEnv )
                             == STL_SUCCESS );

                    sColExprItem->mRelationNode = sInitColumn->mRelationNode;
                    sColExprItem->mNext         = aColExprList->mHead;
                    
                    aColExprList->mHead         = sColExprItem;
                }
                else
                {
                    /* Do Nothing */
                }

                STL_TRY( qlvAddExprToRefColExprList( sColExprItem->mColExprList,
                                                     sInitExprCode,
                                                     aRegionMem,
                                                     aEnv )
                         == STL_SUCCESS );
                    
                break;
            }
            
        case QLV_EXPR_TYPE_FUNCTION :
            {
                sInitFunction = sInitExprCode->mExpr.mFunction;
                
                for( sLoop = 0 ; sLoop < sInitFunction->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvGetRefColList( sInitFunction->mArgs[ sLoop ],
                                               sInitFunction->mArgs[ sLoop ],
                                               aColExprList,
                                               aRegionMem,
                                               aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }
            
        case QLV_EXPR_TYPE_CAST :
            {
                sInitTypeCast = sInitExprCode->mExpr.mTypeCast;
                
                for( sLoop = 0 ; sLoop < DTL_CAST_INPUT_ARG_CNT ; sLoop++ )
                {
                    STL_TRY( qlvGetRefColList( sInitTypeCast->mArgs[ sLoop ],
                                               sInitTypeCast->mArgs[ sLoop ],
                                               aColExprList,
                                               aRegionMem,
                                               aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }

        case QLV_EXPR_TYPE_INNER_COLUMN : 
            {
                /**
                 * Column List에 Inner Column 추가
                 */

                sInitInnerColumn = sInitExprCode->mExpr.mInnerColumn;

                if( sInitInnerColumn->mRelationNode->mType == QLV_NODE_TYPE_SORT_INSTANT )
                {
                    STL_TRY( qlvGetRefColList( sInitExprCode,
                                               sInitInnerColumn->mOrgExpr,
                                               aColExprList,
                                               aRegionMem,
                                               aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    sColExprItem = aColExprList->mHead;
                    while( sColExprItem != NULL )
                    {
                        if( sColExprItem->mRelationNode == sInitInnerColumn->mRelationNode )
                        {
                            break;
                        }
                        else
                        {
                            sColExprItem = sColExprItem->mNext;
                        }
                    }

                    if( sColExprItem == NULL )
                    {
                        STL_TRY( knlAllocRegionMem( aRegionMem,
                                                    STL_SIZEOF( qlvRefColListItem ),
                                                    (void **) & sColExprItem,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        STL_TRY( qlvCreateRefExprList( & sColExprItem->mColExprList,
                                                       aRegionMem,
                                                       aEnv )
                                 == STL_SUCCESS );

                        sColExprItem->mRelationNode = sInitInnerColumn->mRelationNode;
                        sColExprItem->mNext         = aColExprList->mHead;
                    
                        aColExprList->mHead         = sColExprItem;
                    }
                    else
                    {
                        /* Do Nothing */
                    }

                    STL_TRY( qlvAddExprToRefColExprList( sColExprItem->mColExprList,
                                                         sInitExprCode,
                                                         aRegionMem,
                                                         aEnv )
                             == STL_SUCCESS );
                }
                break;
            }

        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                sInitCaseExpr = sInitExprCode->mExpr.mCaseExpr;
                
                for( sLoop = 0 ; sLoop < sInitCaseExpr->mCount ; sLoop++ )
                {
                    STL_TRY( qlvGetRefColList( sInitCaseExpr->mWhenArr[ sLoop ],
                                               sInitCaseExpr->mWhenArr[ sLoop ],
                                               aColExprList,
                                               aRegionMem,
                                               aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlvGetRefColList( sInitCaseExpr->mThenArr[ sLoop ],
                                               sInitCaseExpr->mThenArr[ sLoop ],
                                               aColExprList,
                                               aRegionMem,
                                               aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlvGetRefColList( sInitCaseExpr->mDefResult,
                                           sInitCaseExpr->mDefResult,
                                           aColExprList,
                                           aRegionMem,
                                           aEnv )
                         == STL_SUCCESS );
                
                break;
            }
            
        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                sInitRowExpr = sInitExprCode->mExpr.mRowExpr;
                
                for( sLoop = 0 ; sLoop < sInitRowExpr->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvGetRefColList( sInitRowExpr->mArgs[ sLoop ],
                                               sInitRowExpr->mArgs[ sLoop ],
                                               aColExprList,
                                               aRegionMem,
                                               aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }

        case QLV_EXPR_TYPE_LIST_COLUMN :
            {
                sInitListCol = sInitExprCode->mExpr.mListCol;
                
                for( sLoop = 0 ; sLoop < sInitListCol->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvGetRefColList( sInitListCol->mArgs[ sLoop ],
                                               sInitListCol->mArgs[ sLoop ],
                                               aColExprList,
                                               aRegionMem,
                                               aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }
            
        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                sInitListFunc = sInitExprCode->mExpr.mListFunc;
                
                for( sLoop = 0 ; sLoop < sInitListFunc->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvGetRefColList( sInitListFunc->mArgs[ sLoop ],
                                               sInitListFunc->mArgs[ sLoop ],
                                               aColExprList,
                                               aRegionMem,
                                               aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }
        /* 나머지 Expression들은 내부에 Column Expr을 포함할 수 없다 */
        default :   
            {
                /* Do Nothing */
                break;
            }
    }
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief RETURN node validation
 * @param[in]      aStmtInfo             Stmt Information
 * @param[in]      aValidationObjList    Validation Object List
 * @param[in]      aRefTableList         Reference Table List
 * @param[in]      aAllowedAggrDepth     Allowed Aggregation Depth
 * @param[in]      aReturnSource         Return Source
 * @param[in,out]  aReturnCode           Return Code
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime        상위 노드의 Iteration Time
 * @param[in]      aEnv                  Environment
 * @remark RETURN 노드는 TARGET와 동일한 구조체를 사용한다.
 */
stlStatus qlvValidateReturn( qlvStmtInfo       * aStmtInfo,
                             ellObjectList     * aValidationObjList,
                             qlvRefTableList   * aRefTableList,
                             stlInt32            aAllowedAggrDepth,
                             qlpTarget         * aReturnSource,
                             qlvInitTarget     * aReturnCode,
                             dtlIterationTime    aIterationTimeScope,
                             dtlIterationTime    aIterationTime,
                             qllEnv            * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefTableList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReturnSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReturnSource->mExpr->mType == QLL_VALUE_EXPR_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * Return Name 설정
     */

    STL_TRY( qlvSetTargetName( aStmtInfo,
                               aReturnSource,
                               aReturnCode,
                               aEnv )
             == STL_SUCCESS );
    
    /**
     * Valdate Value Expression
     */

    STL_TRY( qlvValidateValueExpr( aStmtInfo,
                                   QLV_EXPR_PHRASE_RETURN,
                                   aValidationObjList,
                                   aRefTableList,
                                   STL_FALSE,  /* Atomic */
                                   STL_FALSE,  /* Row Expr */
                                   aAllowedAggrDepth,
                                   aReturnSource->mExpr,
                                   & aReturnCode->mExpr,
                                   NULL,
                                   KNL_BUILTIN_FUNC_INVALID,
                                   aIterationTimeScope,
                                   aIterationTime,
                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    /**
     * Rewrite Expression
     */

    STL_TRY( qlvRewriteExpr( aReturnCode->mExpr,
                             DTL_ITERATION_TIME_FOR_EACH_AGGREGATION,
                             aStmtInfo->mQueryExprList,
                             & aReturnCode->mExpr,
                             QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                             aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Target Expression 내에 Sequence 관련 Expression이 허용되지 않음을 validate 함
 * @param[in]      aSQLString    Query String
 * @param[in]      aTargetExpr   Target Expression
 * @param[in]      aEnv          Environment
 */
stlStatus qlvValidateSequenceInTarget( stlChar            * aSQLString,
                                       qlvInitExpression  * aTargetExpr,
                                       qllEnv             * aEnv )
{
    qlvInitExpression   * sExpr        = NULL;
    
    qlvInitFunction     * sFunction    = NULL;
    qlvInitCaseExpr     * sCaseExpr    = NULL;
    qlvInitListFunc     * sListFunc    = NULL;
    qlvInitListCol      * sListCol     = NULL;
    qlvInitRowExpr      * sRowExpr     = NULL;

    stlInt32              sLoop        = 0;
    
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetExpr != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Sequece 가 존재하면 에러 반환
     */

    sExpr = aTargetExpr;
        
    STL_RAMP( RAMP_FIRST );

    switch( sExpr->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
        case QLV_EXPR_TYPE_BIND_PARAM :
        case QLV_EXPR_TYPE_COLUMN :
        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
        case QLV_EXPR_TYPE_SUB_QUERY :
        case QLV_EXPR_TYPE_AGGREGATION :
        case QLV_EXPR_TYPE_OUTER_COLUMN :
        case QLV_EXPR_TYPE_ROWID_COLUMN :
            {
                break;
            }
        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
            {
                STL_THROW( RAMP_ERR_PSEUDO_COLUMN_NOT_ALLOWED );
                break;
            }
        case QLV_EXPR_TYPE_FUNCTION :
            {
                sFunction = sExpr->mExpr.mFunction;

                for( sLoop = 0 ; sLoop < sFunction->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvValidateSequenceInTarget( aSQLString,
                                                          sFunction->mArgs[ sLoop ],
                                                          aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLV_EXPR_TYPE_CAST :
            {
                sExpr = sExpr->mExpr.mTypeCast->mArgs[ 0 ];
                STL_THROW( RAMP_FIRST );
                break;
            }
        case QLV_EXPR_TYPE_INNER_COLUMN :
            {
                sExpr = sExpr->mExpr.mInnerColumn->mOrgExpr;
                STL_THROW( RAMP_FIRST );
                break;
            }
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                sCaseExpr = sExpr->mExpr.mCaseExpr;

                for( sLoop = 0 ; sLoop < sCaseExpr->mCount ; sLoop++ )
                {
                    STL_TRY( qlvValidateSequenceInTarget( aSQLString,
                                                          sCaseExpr->mWhenArr[ sLoop ],
                                                          aEnv )
                             == STL_SUCCESS );
                    
                    STL_TRY( qlvValidateSequenceInTarget( aSQLString,
                                                          sCaseExpr->mThenArr[ sLoop ],
                                                          aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlvValidateSequenceInTarget( aSQLString,
                                                      sCaseExpr->mDefResult,
                                                      aEnv )
                         == STL_SUCCESS );
                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                sListFunc = sExpr->mExpr.mListFunc;

                for( sLoop = 0 ; sLoop < sListFunc->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvValidateSequenceInTarget( aSQLString,
                                                          sListFunc->mArgs[ sLoop ],
                                                          aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLV_EXPR_TYPE_LIST_COLUMN :
            {
                sListCol = sExpr->mExpr.mListCol;

                for( sLoop = 0 ; sLoop < sListCol->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvValidateSequenceInTarget( aSQLString,
                                                          sListCol->mArgs[ sLoop ],
                                                          aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                sRowExpr = sExpr->mExpr.mRowExpr;

                for( sLoop = 0 ; sLoop < sRowExpr->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvValidateSequenceInTarget( aSQLString,
                                                          sRowExpr->mArgs[ sLoop ],
                                                          aEnv )
                             == STL_SUCCESS );
                }
                break;
            }
        case QLV_EXPR_TYPE_REFERENCE :
            /* Sub Query의 Target으로 Sequence가 올 수 없기 때문에 reference 될 수 없다. */
        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_PSEUDO_COLUMN_NOT_ALLOWED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SEQUENCE_NUMBER_NOT_ALLOWED,
                      qlgMakeErrPosString( aSQLString,
                                           sExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Synonym의 (Objct Schema Name, Object Name)을 이용해 Table Dictionary Handle 을 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aDBCStmt             DBC Statement
 * @param[in]  aSQLStmt             SQL Statement
 * @param[in]  aSQLString           Query String
 * @param[in]  aTableNamePos        Table Name Position
 * @param[in]  aSchemaName          Schema Name
 * @param[in]  aObjectName          Object Name
 * @param[out] aTableDictHandle     Table Dictionary Handle
 * @param[out] aExist               존재 여부
 * @param[out] aSynonymList         Synonym List
 * @param[out] aPublicSynonymList   Public Synonym List
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 *  Implementation
 *  Synonym의 object schema name, object name으로 table handle을 찾을 때 사용하도록 설계됨
 *  (1) (Schema Name, Object Name)로 Table Handle 획득 
 *  (2) Table Handle을 찾지 못한 경우, (Schema Name, Object Name)으로 Synonym Handle 획득 
 *  (3) Synonym Handle을 찾지 못한 경우, Object Name으로 Public Synonym Handle 획득 
 */
stlStatus qlvGetTblDictHandleBySchNameNObjNameOfSynonym( smlTransId          aTransID,
                                                         qllDBCStmt        * aDBCStmt,
                                                         qllStatement      * aSQLStmt,
                                                         stlChar           * aSQLString,
                                                         stlInt32            aTableNamePos,
                                                         stlChar           * aSchemaName,
                                                         stlChar           * aObjectName,
                                                         ellDictHandle     * aTableDictHandle,
                                                         stlBool           * aExist,
                                                         ellObjectList     * aSynonymList,
                                                         ellObjectList     * aPublicSynonymList,
                                                         qllEnv            * aEnv )
{
    ellDictHandle    sSchemaDictHandle;
    stlBool          sSchemaExist = STL_FALSE;

    ellDictHandle    sSynonymDictHandle;
    stlBool          sSynonymExist = STL_FALSE;

    stlChar        * sObjectSchemaName = NULL;
    stlChar        * sObjectName = NULL;

    stlBool          sIsCycle = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Synonym을 쓸 수 있는 구문인지 검사  
     */

    if ( (aSQLStmt->mStmtAttr & QLL_STMT_ATTR_CAN_REFER_SYNONYM_MASK)
         == QLL_STMT_ATTR_CAN_REFER_SYNONYM_NO )
    {
        /* Synonym 사용할 수 없는 구문 */ 
        STL_THROW( RAMP_ERR_SYNONYM_TRANSLATION_IS_NO_LONGER_VALID );
    }
    else
    {
        /* Synonym 사용할 수 있는 구문 */ 
    }

    /**
     * Table Dictionary Handle 획득 
     */

    ellInitDictHandle( & sSchemaDictHandle );
    
    STL_TRY( ellGetSchemaDictHandle( aTransID,
                                     aSQLStmt->mViewSCN,
                                     aSchemaName,
                                     & sSchemaDictHandle,
                                     & sSchemaExist,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sSchemaExist == STL_TRUE )
    {   
        ellInitDictHandle( aTableDictHandle );

        STL_TRY( ellGetTableDictHandleWithSchema( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  & sSchemaDictHandle,
                                                  aObjectName,
                                                  aTableDictHandle,
                                                  aExist,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* (Schema Name, Object Name)에 해당하는 Table 찾으면 종료,
         * 찾지 못하면 Private Synonym 찾음*/
        
        STL_TRY_THROW( *aExist == STL_FALSE, RAMP_FINISH );
    }
    else
    {
        /* (Schema Name, Object Name)에 해당하는 Table 없음 */
    }

    /**
     * Synonym Dictionary Handle 획득 
     */

    ellInitDictHandle( & sSynonymDictHandle );

    if ( sSchemaExist == STL_TRUE )
    {
        STL_TRY( ellGetSynonymDictHandleWithSchema( aTransID,
                                                    aSQLStmt->mViewSCN,
                                                    & sSchemaDictHandle,
                                                    aObjectName,
                                                    & sSynonymDictHandle,
                                                    & sSynonymExist,
                                                    ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        if ( sSynonymExist == STL_TRUE )
        {
            /* (Schema Name, Object Name)에 해당하는 Synonym 존재 */

            /* Synonym Cycle Check */
            STL_TRY( ellAddNewObjectItem( aSynonymList,
                                          & sSynonymDictHandle,
                                          & sIsCycle,
                                          QLL_INIT_PLAN(aDBCStmt),
                                          ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            STL_TRY_THROW( sIsCycle == STL_FALSE, RAMP_ERR_LOOPING_CHAIN_SYNONYM_NAME );
            
            sObjectSchemaName = ellGetSynonymObjectSchemaName( & sSynonymDictHandle );
            sObjectName       = ellGetSynonymObjectName( & sSynonymDictHandle );
            
            STL_TRY( qlvGetTblDictHandleBySchNameNObjNameOfSynonym( aTransID,
                                                                    aDBCStmt, 
                                                                    aSQLStmt,
                                                                    aSQLString,
                                                                    aTableNamePos,
                                                                    sObjectSchemaName,
                                                                    sObjectName,
                                                                    aTableDictHandle,
                                                                    aExist,
                                                                    aSynonymList,
                                                                    aPublicSynonymList,
                                                                    aEnv )
                     == STL_SUCCESS );            

            STL_THROW( RAMP_FINISH );
        }
        else
        {
            /* (Schema Name, Object Name)에 해당하는 Synonym 없음 */ 
        }
    }
    else
    {
        /* (Schema Name, Object Name)에 해당하는 Synonym 없음 */ 
    }
        
    /**
     * Public Synonym Dictionary Handle 획득
     */   

    ellInitDictHandle( & sSynonymDictHandle );
    
    STL_TRY( ellGetPublicSynonymDictHandle( aTransID,
                                            aSQLStmt->mViewSCN,
                                            aObjectName,
                                            & sSynonymDictHandle,
                                            & sSynonymExist,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    if ( sSynonymExist == STL_TRUE )
    {
        /* Public Synonym Cycle Check */
        STL_TRY( ellAddNewObjectItem( aPublicSynonymList,
                                      & sSynonymDictHandle,
                                      & sIsCycle,
                                      QLL_INIT_PLAN(aDBCStmt),
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sIsCycle == STL_FALSE, RAMP_ERR_LOOPING_CHAIN_SYNONYM_NAME );
    
        sObjectSchemaName = ellGetPublicSynonymObjectSchemaName( & sSynonymDictHandle );
        sObjectName       = ellGetPublicSynonymObjectName( & sSynonymDictHandle );
        
        STL_TRY( qlvGetTblDictHandleBySchNameNObjNameOfSynonym( aTransID,
                                                                aDBCStmt, 
                                                                aSQLStmt,
                                                                aSQLString,
                                                                aTableNamePos,
                                                                sObjectSchemaName,
                                                                sObjectName,
                                                                aTableDictHandle,
                                                                aExist,
                                                                aSynonymList,
                                                                aPublicSynonymList,
                                                                aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Public Synonym 없음 */
    }
    
    /**
     * 작업 종료
     */

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SYNONYM_TRANSLATION_IS_NO_LONGER_VALID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYNONYM_TRANSLATION_IS_NO_LONGER_VALID,
                      qlgMakeErrPosString( aSQLString,
                                           aTableNamePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_LOOPING_CHAIN_SYNONYM_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_LOOPING_CHAIN_SYNONYM_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           aTableNamePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief (Schema Handle, Table Name) 을 이용해 Table Dictionary Handle 을 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aDBCStmt             DBC Statement
 * @param[in]  aSQLStmt             SQL Statement
 * @param[in]  aSQLString           Query String
 * @param[in]  aSchemaDictHandle    Schema Dictionary Handle
 * @param[in]  aTableName           Table Name
 * @param[in]  aTableNamePos        Table Name Position
 * @param[out] aTableDictHandle     Table Dictionary Handle
 * @param[out] aExist               존재 여부
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus qlvGetTableDictHandleBySchHandleNTblName( smlTransId          aTransID,
                                                    qllDBCStmt        * aDBCStmt,
                                                    qllStatement      * aSQLStmt,
                                                    stlChar           * aSQLString,
                                                    ellDictHandle     * aSchemaDictHandle,
                                                    stlChar           * aTableName,
                                                    stlInt32            aTableNamePos,
                                                    ellDictHandle     * aTableDictHandle,
                                                    stlBool           * aExist,
                                                    qllEnv            * aEnv )
{
    ellObjectList * sSynonymList = NULL;
    ellObjectList * sPublicSynonymList = NULL;

    ellDictHandle   sSynonymDictHandle;
    stlBool         sSynonymExist = STL_FALSE;

    stlChar       * sObjectSchemaName = NULL;
    stlChar       * sObjectName = NULL;
    
    stlBool         sIsCycle = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 초기화
     */
    
    ellInitDictHandle( & sSynonymDictHandle );
    ellInitObjectList( & sSynonymList,
                       QLL_INIT_PLAN(aDBCStmt),
                       ELL_ENV(aEnv) );
    ellInitObjectList( & sPublicSynonymList,
                       QLL_INIT_PLAN(aDBCStmt),
                       ELL_ENV(aEnv) );

    /**
     * Table Dictionary Handle 획득
     */

    ellInitDictHandle( aTableDictHandle );

    STL_TRY( ellGetTableDictHandleWithSchema( aTransID,
                                              aSQLStmt->mViewSCN,
                                              aSchemaDictHandle,
                                              aTableName,
                                              aTableDictHandle,
                                              aExist,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( *aExist == STL_FALSE )
    {
        /**
         * Table Dictionary Handle 획득 실패하면 
         * Synonym Dictionary Handle 획득 시도 
         */

        STL_TRY( ellGetSynonymDictHandleWithSchema( aTransID,
                                                    aSQLStmt->mViewSCN,
                                                    aSchemaDictHandle,
                                                    aTableName,
                                                    & sSynonymDictHandle,
                                                    & sSynonymExist,
                                                    ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sSynonymExist == STL_TRUE )
        {
            STL_TRY( ellAddNewObjectItem( sSynonymList,
                                          & sSynonymDictHandle,
                                          & sIsCycle,
                                          QLL_INIT_PLAN(aDBCStmt),
                                          ELL_ENV(aEnv) )
                     == STL_SUCCESS );

             /* Synonym List를 초기화 한 후, 첫 add 이므로 cycle이 발생할 수 없음 */
            STL_DASSERT( sIsCycle == STL_FALSE );
            
            sObjectSchemaName = ellGetSynonymObjectSchemaName( & sSynonymDictHandle );
            sObjectName       = ellGetSynonymObjectName( & sSynonymDictHandle );
            
            STL_TRY( qlvGetTblDictHandleBySchNameNObjNameOfSynonym( aTransID,
                                                                    aDBCStmt, 
                                                                    aSQLStmt,
                                                                    aSQLString,
                                                                    aTableNamePos,
                                                                    sObjectSchemaName,
                                                                    sObjectName,
                                                                    aTableDictHandle,
                                                                    aExist,
                                                                    sSynonymList,
                                                                    sPublicSynonymList,
                                                                    aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* 스키마명을 명시한 경우이므로, Public Synonym을 찾지 않음 */
        }
    }
    else
    {
        /* Table Dictionary Handle 획득 성공 */ 
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief (Auth Handle, Table Name) 을 이용해 Table Dictionary Handle 을 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aDBCStmt             DBC Statement
 * @param[in]  aSQLStmt             SQL Statement
 * @param[in]  aSQLString           Query String
 * @param[in]  aAuthDictHandle      Authorization Dictionary Handle
 * @param[in]  aTableName           Table Name
 * @param[in]  aTableNamePos        Table Name Position
 * @param[out] aTableDictHandle     Table Dictionary Handle
 * @param[out] aExist               존재 여부
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus qlvGetTableDictHandleByAuthHandleNTblName( smlTransId          aTransID,
                                                     qllDBCStmt        * aDBCStmt,
                                                     qllStatement      * aSQLStmt,
                                                     stlChar           * aSQLString,
                                                     ellDictHandle     * aAuthDictHandle,
                                                     stlChar           * aTableName,
                                                     stlInt32            aTableNamePos,
                                                     ellDictHandle     * aTableDictHandle,
                                                     stlBool           * aExist,
                                                     qllEnv            * aEnv )
{
    ellObjectList * sSynonymList = NULL;
    ellObjectList * sPublicSynonymList = NULL;
    
    stlBool         sSynonymExist = STL_FALSE;
    ellDictHandle   sSynonymDictHandle;

    stlChar       * sObjectSchemaName = NULL;
    stlChar       * sObjectName = NULL;

    stlBool         sIsCycle = STL_FALSE;


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 초기화
     */    

    ellInitObjectList( & sSynonymList,
                       QLL_INIT_PLAN(aDBCStmt),
                       ELL_ENV(aEnv) );
    ellInitObjectList( & sPublicSynonymList,
                       QLL_INIT_PLAN(aDBCStmt),
                       ELL_ENV(aEnv) );

    /**
     * Table Dictionary Handle 을 획득
     */

    STL_TRY( ellGetTableDictHandleWithAuth( aTransID,
                                            aSQLStmt->mViewSCN,
                                            aAuthDictHandle,
                                            aTableName,
                                            aTableDictHandle,
                                            aExist,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( *aExist == STL_FALSE )
    {
        /**
         * Table Dictionary Handle 획득 실패하면, 
         * Synonym Dictionary Handle 을 획득
         */

        ellInitDictHandle( & sSynonymDictHandle );
        
        STL_TRY( ellGetSynonymDictHandleWithAuth( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  aAuthDictHandle,
                                                  aTableName,
                                                  & sSynonymDictHandle,
                                                  & sSynonymExist,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sSynonymExist == STL_TRUE )
        {
            /* For Synonym Cycle Check */
            STL_TRY( ellAddNewObjectItem( sSynonymList,
                                          & sSynonymDictHandle,
                                          & sIsCycle,
                                          QLL_INIT_PLAN(aDBCStmt),
                                          ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            /* Synonym List를 초기화 한 후, 첫 add 이므로 cycle이 발생할 수 없음 */
            STL_DASSERT( sIsCycle == STL_FALSE );
            
            sObjectSchemaName = ellGetSynonymObjectSchemaName( & sSynonymDictHandle );
            sObjectName       = ellGetSynonymObjectName( & sSynonymDictHandle );

            STL_TRY( qlvGetTblDictHandleBySchNameNObjNameOfSynonym( aTransID,
                                                                    aDBCStmt, 
                                                                    aSQLStmt,
                                                                    aSQLString,
                                                                    aTableNamePos,
                                                                    sObjectSchemaName,
                                                                    sObjectName,
                                                                    aTableDictHandle,
                                                                    aExist,
                                                                    sSynonymList,
                                                                    sPublicSynonymList,
                                                                    aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * Synonym Dictionary Handle 획득 실패하면, 
             * Public Synonym Dictionary Handle 을 획득
             */

            ellInitDictHandle( & sSynonymDictHandle );

            STL_TRY( ellGetPublicSynonymDictHandle( aTransID,
                                                    aSQLStmt->mViewSCN,
                                                    aTableName,
                                                    & sSynonymDictHandle,
                                                    & sSynonymExist,
                                                    ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            if ( sSynonymExist == STL_TRUE )
            {
                /* For Public Synonym Cycle Check */
                STL_TRY( ellAddNewObjectItem( sPublicSynonymList,
                                              & sSynonymDictHandle,
                                              & sIsCycle,
                                              QLL_INIT_PLAN(aDBCStmt),
                                              ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                 /* Public Synonym List를 초기화 한 후, 첫 add 이므로 cycle이 발생할 수 없음 */
                STL_DASSERT( sIsCycle == STL_FALSE );
                
                sObjectSchemaName = ellGetPublicSynonymObjectSchemaName( & sSynonymDictHandle );
                sObjectName       = ellGetPublicSynonymObjectName( & sSynonymDictHandle ); 

                STL_TRY( qlvGetTblDictHandleBySchNameNObjNameOfSynonym( aTransID,
                                                                        aDBCStmt, 
                                                                        aSQLStmt,
                                                                        aSQLString,
                                                                        aTableNamePos,
                                                                        sObjectSchemaName,
                                                                        sObjectName,
                                                                        aTableDictHandle,
                                                                        aExist,
                                                                        sSynonymList,
                                                                        sPublicSynonymList,
                                                                        aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /* 해당 name의 Public Synonym 없음 */
            }
        }
    }
    else
    {
        /* Table Dictionary Handle 획득 성공 */ 
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief (Schema Name, Object Name)을 이용해 Sequence Dictionary Handle 을 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aDBCStmt             DBC Statement
 * @param[in]  aSQLStmt             SQL Statement
 * @param[in]  aSQLString           Query String
 * @param[in]  aSeqNamePos          Sequence Name Position
 * @param[in]  aSchemaName          Schema Name
 * @param[in]  aObjectName          Object Name
 * @param[out] aSeqDictHandle       Sequence Dictionary Handle
 * @param[out] aExist               존재 여부
 * @param[out] aSynonymList         Synonym List
 * @param[out] aPublicSynonymList   Public Synonym List
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 *  Implementation 
 *  (1) (Schema Name, Object Name)로 Table Handle 획득 
 *  (2) Sequence Handle을 찾지 못한 경우, (Schema Name, Object Name)으로 Synonym Handle 획득 
 *  (3) Synonym Handle을 찾지 못한 경우, Object Name으로 Public Synonym Handle 획득 
 */
stlStatus qlvGetSequenceDictHandleBySchNameNObjName( smlTransId          aTransID,
                                                     qllDBCStmt        * aDBCStmt,
                                                     qllStatement      * aSQLStmt,
                                                     stlChar           * aSQLString,
                                                     stlInt32            aSeqNamePos,
                                                     stlChar           * aSchemaName,
                                                     stlChar           * aObjectName,
                                                     ellDictHandle     * aSeqDictHandle,
                                                     stlBool           * aExist,
                                                     ellObjectList     * aSynonymList,
                                                     ellObjectList     * aPublicSynonymList,
                                                     qllEnv            * aEnv )
{
    ellDictHandle    sSchemaDictHandle;
    stlBool          sSchemaExist = STL_FALSE;

    ellDictHandle    sSynonymDictHandle;
    stlBool          sSynonymExist = STL_FALSE;

    stlChar        * sObjectSchemaName = NULL;
    stlChar        * sObjectName = NULL;

    stlBool          sIsCycle = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Sequence Dictionary Handle 획득 
     */

    ellInitDictHandle( & sSchemaDictHandle );
    
    STL_TRY( ellGetSchemaDictHandle( aTransID,
                                     aSQLStmt->mViewSCN,
                                     aSchemaName,
                                     & sSchemaDictHandle,
                                     & sSchemaExist,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( sSchemaExist == STL_TRUE )
    {   
        ellInitDictHandle( aSeqDictHandle );

        STL_TRY( ellGetSequenceDictHandleWithSchema( aTransID,
                                                     aSQLStmt->mViewSCN,
                                                     & sSchemaDictHandle,
                                                     aObjectName,
                                                     aSeqDictHandle,
                                                     aExist,
                                                     ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* (Schema Name, Object Name)에 해당하는 Seqeunce 찾으면 종료,
         * 찾지 못하면 Private Synonym 찾음*/
        
        STL_TRY_THROW( *aExist == STL_FALSE, RAMP_FINISH );
    }
    else
    {
        /* (Schema Name, Object Name)에 해당하는 Table 없음 */
    }

    /**
     * Synonym Dictionary Handle 획득 
     */

    ellInitDictHandle( & sSynonymDictHandle );

    if ( sSchemaExist == STL_TRUE )
    {
        STL_TRY( ellGetSynonymDictHandleWithSchema( aTransID,
                                                    aSQLStmt->mViewSCN,
                                                    & sSchemaDictHandle,
                                                    aObjectName,
                                                    & sSynonymDictHandle,
                                                    & sSynonymExist,
                                                    ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        if ( sSynonymExist == STL_TRUE )
        {
            /* (Schema Name, Object Name)에 해당하는 Synonym 존재 */

            /* Synonym Cycle Check */
            STL_TRY( ellAddNewObjectItem( aSynonymList,
                                          & sSynonymDictHandle,
                                          & sIsCycle,
                                          QLL_INIT_PLAN(aDBCStmt),
                                          ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            STL_TRY_THROW( sIsCycle == STL_FALSE, RAMP_ERR_LOOPING_CHAIN_SYNONYM_NAME );
            
            sObjectSchemaName = ellGetSynonymObjectSchemaName( & sSynonymDictHandle );
            sObjectName       = ellGetSynonymObjectName( & sSynonymDictHandle );
            
            STL_TRY( qlvGetSequenceDictHandleBySchNameNObjName( aTransID,
                                                                aDBCStmt, 
                                                                aSQLStmt,
                                                                aSQLString,
                                                                aSeqNamePos,
                                                                sObjectSchemaName,
                                                                sObjectName,
                                                                aSeqDictHandle,
                                                                aExist,
                                                                aSynonymList,
                                                                aPublicSynonymList,
                                                                aEnv )
                     == STL_SUCCESS );            

            STL_THROW( RAMP_FINISH );
        }
        else
        {
            /* (Schema Name, Object Name)에 해당하는 Synonym 없음 */ 
        }
    }
    else
    {
        /* (Schema Name, Object Name)에 해당하는 Synonym 없음 */ 
    }
        
    /**
     * Public Synonym Dictionary Handle 획득
     */   

    ellInitDictHandle( & sSynonymDictHandle );
    
    STL_TRY( ellGetPublicSynonymDictHandle( aTransID,
                                            aSQLStmt->mViewSCN,
                                            aObjectName,
                                            & sSynonymDictHandle,
                                            & sSynonymExist,
                                            ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    if ( sSynonymExist == STL_TRUE )
    {
        /* Public Synonym Cycle Check */
        STL_TRY( ellAddNewObjectItem( aPublicSynonymList,
                                      & sSynonymDictHandle,
                                      & sIsCycle,
                                      QLL_INIT_PLAN(aDBCStmt),
                                      ELL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sIsCycle == STL_FALSE, RAMP_ERR_LOOPING_CHAIN_SYNONYM_NAME );
    
        sObjectSchemaName = ellGetPublicSynonymObjectSchemaName( & sSynonymDictHandle );
        sObjectName       = ellGetPublicSynonymObjectName( & sSynonymDictHandle );
        
        STL_TRY( qlvGetSequenceDictHandleBySchNameNObjName( aTransID,
                                                            aDBCStmt, 
                                                            aSQLStmt,
                                                            aSQLString,
                                                            aSeqNamePos,
                                                            sObjectSchemaName,
                                                            sObjectName,
                                                            aSeqDictHandle,
                                                            aExist,
                                                            aSynonymList,
                                                            aPublicSynonymList,
                                                            aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Public Synonym 없음 */
    }
    
    /**
     * 작업 종료
     */

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_LOOPING_CHAIN_SYNONYM_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_LOOPING_CHAIN_SYNONYM_NAME,
                      qlgMakeErrPosString( aSQLString,
                                           aSeqNamePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief (Schema Handle, Sequence Name) 을 이용해 Sequence Dictionary Handle 을 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aDBCStmt             DBC Statement
 * @param[in]  aSQLStmt             SQL Statement
 * @param[in]  aSQLString           Query String
 * @param[in]  aSchemaDictHandle    Schema Dictionary Handle
 * @param[in]  aSeqName             Sequence Name
 * @param[in]  aSeqNamePos          Sequence Name Position
 * @param[out] aSeqDictHandle       Sequence Dictionary Handle
 * @param[out] aExist               존재 여부
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus qlvGetSequenceDictHandleBySchHandleNSeqName( smlTransId          aTransID,
                                                       qllDBCStmt        * aDBCStmt,
                                                       qllStatement      * aSQLStmt,
                                                       stlChar           * aSQLString,
                                                       ellDictHandle     * aSchemaDictHandle,
                                                       stlChar           * aSeqName,
                                                       stlInt32            aSeqNamePos,
                                                       ellDictHandle     * aSeqDictHandle,
                                                       stlBool           * aExist,
                                                       qllEnv            * aEnv )
{
    ellObjectList * sSynonymList = NULL;
    ellObjectList * sPublicSynonymList = NULL;

    ellDictHandle   sSynonymDictHandle;
    stlBool         sSynonymExist = STL_FALSE;

    stlChar       * sObjectSchemaName = NULL;
    stlChar       * sObjectName = NULL;
    
    stlBool         sIsCycle = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 초기화
     */
    
    ellInitDictHandle( & sSynonymDictHandle );
    ellInitObjectList( & sSynonymList,
                       QLL_INIT_PLAN(aDBCStmt),
                       ELL_ENV(aEnv) );
    ellInitObjectList( & sPublicSynonymList,
                       QLL_INIT_PLAN(aDBCStmt),
                       ELL_ENV(aEnv) );

    /**
     * Sequence Dictionary Handle 획득
     */

    ellInitDictHandle( aSeqDictHandle );

    STL_TRY( ellGetSequenceDictHandleWithSchema( aTransID,
                                                 aSQLStmt->mViewSCN,
                                                 aSchemaDictHandle,
                                                 aSeqName,
                                                 aSeqDictHandle,
                                                 aExist,
                                                 ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( *aExist == STL_FALSE )
    {
        /**
         * Sequence Dictionary Handle 획득 실패하면, 
         * Synonym Dictionary Handle 획득 시도 
         */

        STL_TRY( ellGetSynonymDictHandleWithSchema( aTransID,
                                                    aSQLStmt->mViewSCN,
                                                    aSchemaDictHandle,
                                                    aSeqName,
                                                    & sSynonymDictHandle,
                                                    & sSynonymExist,
                                                    ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sSynonymExist == STL_TRUE )
        {
            STL_TRY( ellAddNewObjectItem( sSynonymList,
                                          & sSynonymDictHandle,
                                          & sIsCycle,
                                          QLL_INIT_PLAN(aDBCStmt),
                                          ELL_ENV(aEnv) )
                     == STL_SUCCESS );

             /* Synonym List를 초기화 한 후, 첫 add 이므로 cycle이 발생할 수 없음 */
            STL_DASSERT( sIsCycle == STL_FALSE );
            
            sObjectSchemaName = ellGetSynonymObjectSchemaName( & sSynonymDictHandle );
            sObjectName       = ellGetSynonymObjectName( & sSynonymDictHandle );
            
            STL_TRY( qlvGetSequenceDictHandleBySchNameNObjName( aTransID,
                                                                aDBCStmt, 
                                                                aSQLStmt,
                                                                aSQLString,
                                                                aSeqNamePos,
                                                                sObjectSchemaName,
                                                                sObjectName,
                                                                aSeqDictHandle,
                                                                aExist,
                                                                sSynonymList,
                                                                sPublicSynonymList,
                                                                aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* 스키마명을 명시한 경우이므로, Public Synonym을 찾지 않음 */
        }
    }
    else
    {
        /* Sequence Dictionary Handle 획득 성공 */ 
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief (Auth Handle, Sequence Name) 을 이용해 Sequence Dictionary Handle 을 얻는다.
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aDBCStmt             DBC Statement
 * @param[in]  aSQLStmt             SQL Statement
 * @param[in]  aSQLString           Query String
 * @param[in]  aAuthDictHandle      Authorization Dictionary Handle
 * @param[in]  aSeqName             Sequence Name
 * @param[in]  aSeqNamePos          Sequence Name Position
 * @param[out] aSeqDictHandle       Sequence Dictionary Handle
 * @param[out] aExist               존재 여부
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus qlvGetSequenceDictHandleByAuthHandleNSeqName( smlTransId          aTransID,
                                                        qllDBCStmt        * aDBCStmt,
                                                        qllStatement      * aSQLStmt,
                                                        stlChar           * aSQLString,
                                                        ellDictHandle     * aAuthDictHandle,
                                                        stlChar           * aSeqName,
                                                        stlInt32            aSeqNamePos,
                                                        ellDictHandle     * aSeqDictHandle,
                                                        stlBool           * aExist,
                                                        qllEnv            * aEnv )
{
    ellObjectList * sSynonymList = NULL;
    ellObjectList * sPublicSynonymList = NULL;
    
    stlBool         sSynonymExist = STL_FALSE;
    ellDictHandle   sSynonymDictHandle;

    stlChar       * sObjectSchemaName = NULL;
    stlChar       * sObjectName = NULL;

    stlBool         sIsCycle = STL_FALSE;


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * 초기화
     */    

    ellInitObjectList( & sSynonymList,
                       QLL_INIT_PLAN(aDBCStmt),
                       ELL_ENV(aEnv) );
    ellInitObjectList( & sPublicSynonymList,
                       QLL_INIT_PLAN(aDBCStmt),
                       ELL_ENV(aEnv) );

    /**
     * Sequence Dictionary Handle 을 획득
     */

    STL_TRY( ellGetSequenceDictHandleWithAuth( aTransID,
                                               aSQLStmt->mViewSCN,
                                               aAuthDictHandle,
                                               aSeqName,
                                               aSeqDictHandle,
                                               aExist,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    if ( *aExist == STL_FALSE )
    {
        /**
         * Sequence Dictionary Handle 획득 실패하면, 
         * Synonym Dictionary Handle 을 획득
         */

        ellInitDictHandle( & sSynonymDictHandle );
        
        STL_TRY( ellGetSynonymDictHandleWithAuth( aTransID,
                                                  aSQLStmt->mViewSCN,
                                                  aAuthDictHandle,
                                                  aSeqName,
                                                  & sSynonymDictHandle,
                                                  & sSynonymExist,
                                                  ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        if ( sSynonymExist == STL_TRUE )
        {
            /* For Synonym Cycle Check */
            STL_TRY( ellAddNewObjectItem( sSynonymList,
                                          & sSynonymDictHandle,
                                          & sIsCycle,
                                          QLL_INIT_PLAN(aDBCStmt),
                                          ELL_ENV(aEnv) )
                     == STL_SUCCESS );

            /* Synonym List를 초기화 한 후, 첫 add 이므로 cycle이 발생할 수 없음 */
            STL_DASSERT( sIsCycle == STL_FALSE );
            
            sObjectSchemaName = ellGetSynonymObjectSchemaName( & sSynonymDictHandle );
            sObjectName       = ellGetSynonymObjectName( & sSynonymDictHandle );

            STL_TRY( qlvGetSequenceDictHandleBySchNameNObjName( aTransID,
                                                                aDBCStmt, 
                                                                aSQLStmt,
                                                                aSQLString,
                                                                aSeqNamePos,
                                                                sObjectSchemaName,
                                                                sObjectName,
                                                                aSeqDictHandle,
                                                                aExist,
                                                                sSynonymList,
                                                                sPublicSynonymList,
                                                                aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * Synonym Dictionary Handle 획득 실패하면, 
             * Public Synonym Dictionary Handle 을 획득
             */

            ellInitDictHandle( & sSynonymDictHandle );

            STL_TRY( ellGetPublicSynonymDictHandle( aTransID,
                                                    aSQLStmt->mViewSCN,
                                                    aSeqName,
                                                    & sSynonymDictHandle,
                                                    & sSynonymExist,
                                                    ELL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            if ( sSynonymExist == STL_TRUE )
            {
                /* For Public Synonym Cycle Check */
                STL_TRY( ellAddNewObjectItem( sPublicSynonymList,
                                              & sSynonymDictHandle,
                                              & sIsCycle,
                                              QLL_INIT_PLAN(aDBCStmt),
                                              ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                 /* Public Synonym List를 초기화 한 후, 첫 add 이므로 cycle이 발생할 수 없음 */
                STL_DASSERT( sIsCycle == STL_FALSE );
                
                sObjectSchemaName = ellGetPublicSynonymObjectSchemaName( & sSynonymDictHandle );
                sObjectName       = ellGetPublicSynonymObjectName( & sSynonymDictHandle ); 

                STL_TRY( qlvGetSequenceDictHandleBySchNameNObjName( aTransID,
                                                                    aDBCStmt, 
                                                                    aSQLStmt,
                                                                    aSQLString,
                                                                    aSeqNamePos,
                                                                    sObjectSchemaName,
                                                                    sObjectName,
                                                                    aSeqDictHandle,
                                                                    aExist,
                                                                    sSynonymList,
                                                                    sPublicSynonymList,
                                                                    aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /* 해당 name의 Public Synonym 없음 */
            }
        }
    }
    else
    {
        /* Sequence Dictionary Handle 획득 성공 */ 
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Parse Node로 부터 상수 Expression에 대한 DataValue 값을 얻는다.
 * @param[in]      aSQLStmt        SQL Statement
 * @param[in]      aSQLString      Query String
 * @param[in]      aExprSource     Expression Source
 * @param[in]      aDataTypeDef    Target Data Type Definition
 * @param[out]     aDataValue      Data Value
 * @param[in]      aRegionMem      Region Memory
 * @param[in]      aEnv            Environment
 */
stlStatus qlvGetConstDataValueFromParseNode( qllStatement     * aSQLStmt,
                                             stlChar          * aSQLString,
                                             qllNode          * aExprSource,
                                             qlvInitTypeDef   * aDataTypeDef,
                                             dtlDataValue    ** aDataValue,
                                             knlRegionMem     * aRegionMem,
                                             qllEnv           * aEnv )
{
    ellObjectList        * sValidObjList   = NULL;
    qlvInitExpression    * sExprCode       = NULL;
    qlvInitExpression    * sConstExpr      = NULL;
    qlvInitConstExpr     * sConstant       = NULL;
    stlChar              * sTmpStr         = NULL;
    stlChar              * sDetailErrMsg   = NULL;

    qloExprInfo          * sTotalExprInfo  = NULL;
    knlExprStack         * sConstExprStack = NULL;
    
    knlExprEntry         * sExprEntry      = NULL;
    qloDBType            * sDBType         = NULL;
    dtlStringLengthUnit    sStrLengthUnit  = 0;
    stlInt64               sPrecision      = 0;
    stlInt64               sBufferSize     = 0;

    qloInitStmtExprList    sStmtExprList;
    qlvStmtInfo            sStmtInfo;
    qllDataArea            sDataArea;
    qloDataOptExprInfo     sDataOptInfo;

#ifdef STL_DEBUG
    stlInt32               sLongValueCount = 0;
#endif

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataTypeDef != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataValue != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Expression Validation
     ****************************************************************/

    /**
     * Expression Validation 위한 Statemen Info 초기화
     */
    
    stlMemset( & sStmtInfo, 0x00, STL_SIZEOF( qlvStmtInfo ) );

    sStmtInfo.mSQLString = aSQLString;

#ifdef STL_DEBUG
    sLongValueCount = aSQLStmt->mLongTypeValueList.mCount;
#endif    

    /**
     * Parse Node Validation
     */
    
    STL_TRY( ellInitObjectList( & sValidObjList,
                                aRegionMem,
                                ELL_ENV( aEnv ) )
             == STL_SUCCESS );

    if( qlvValidateValueExpr( & sStmtInfo,
                              QLV_EXPR_PHRASE_DEFAULT,
                              sValidObjList,
                              NULL, /* aRefTableList */
                              STL_FALSE, /* aIsAtomic */
                              STL_FALSE, /* aIsRowExpr */
                              0, /* aAllowedAggrDepth */
                              aExprSource,
                              & sExprCode,
                              NULL, /* aHasOuterJoinOperator */
                              KNL_BUILTIN_FUNC_INVALID, /* aRelatedFuncId */
                              DTL_ITERATION_TIME_NONE, /* aIterationTimeScope */
                              DTL_ITERATION_TIME_NONE, /* aIterationTime */
                              aRegionMem,
                              aEnv )
        == STL_SUCCESS )
    {
        /* Do Nothing */
    }
    else
    {
        if( ( stlGetLastErrorCode( QLL_ERROR_STACK( aEnv ) ) ==
              QLL_ERRCODE_ITERATION_TIME_SCOPE_INVALID ) ||
            ( stlGetLastErrorCode( QLL_ERROR_STACK( aEnv ) ) ==
              QLL_ERRCODE_DEFAULT_EXPRESSION_HAS_ERRORS ) )
        {
            sTmpStr = stlGetLastDetailErrorMessage( QLL_ERROR_STACK( aEnv ) );

            if( sTmpStr == NULL )
            {
                sDetailErrMsg = NULL;
            }
            else
            {
                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            stlStrlen( sTmpStr ) + 1,
                                            (void **) & sDetailErrMsg,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlStrcpy( sDetailErrMsg, sTmpStr );
            }

            stlPopError( QLL_ERROR_STACK( aEnv ) );

            STL_THROW( RAMP_ERR_NOT_CONSTANT_VALUE_EXPRESSION );
        }
        else
        {
            STL_TRY( 0 );
        }
    }

    
    /**
     * Constant Expression으로 감싸기
     */

    /* 현재 Expression 복사하여 새로운 Expression 생성 */
    STL_TRY( qlvCopyExpr( sExprCode,
                          & sConstExpr,
                          aRegionMem,
                          aEnv )
             == STL_SUCCESS );

    /* qlvInitConstExpr 공간 할당 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitConstExpr ),
                                (void **) & sConstant,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* qlvInitConstExpr 설정 */
    sConstant->mOrgExpr = sExprCode;
        
    /* 현재 Expression을 Constant Expression으로 변경 */
    sConstExpr->mType = QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT;
    sConstExpr->mExpr.mConstExpr = sConstant;

    QLV_VALIDATE_INCLUDE_EXPR_CNT( sConstExpr->mIncludeExprCnt + 1, aEnv );

    sConstExpr->mIncludeExprCnt++;


    /****************************************************************
     * Expression List 구성
     ****************************************************************/

    /**
     * Statement 단위 Expression List 생성
     */
    
    STL_TRY( qlvCreateRefExprList( & sStmtExprList.mAllExprList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList.mColumnExprList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList.mRowIdColumnExprList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    sStmtExprList.mConstExprStack = NULL;
    sStmtExprList.mTotalExprInfo  = NULL;

    
    /**
     * Init Statement Expression List 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitStmtExprList ),
                                (void**) & sStmtExprList.mInitExprList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList.mInitExprList->mPseudoColExprList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvCreateRefExprList( & sStmtExprList.mInitExprList->mConstExprList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    sStmtExprList.mInitExprList->mHasSubQuery = STL_FALSE;

    
    /****************************************************************
     * Add Expression 수행
     ****************************************************************/

    /**
     * Add Expr
     */

    STL_TRY( qloAddExpr( sExprCode,
                         sStmtExprList.mAllExprList,
                         aRegionMem,
                         aEnv )
             == STL_SUCCESS );

    STL_TRY( qloAddExpr( sConstExpr,
                         sStmtExprList.mAllExprList,
                         aRegionMem,
                         aEnv )
             == STL_SUCCESS );


    /****************************************************************
     * Expression 을 위한 DB type 정보 공간 설정
     ****************************************************************/
    
    /**
     * Expression Info 생성
     */

    STL_TRY( qloCreateExprInfo( sStmtExprList.mAllExprList->mCount,
                                & sTotalExprInfo,
                                aRegionMem,
                                aEnv )
             == STL_SUCCESS );

    sStmtExprList.mTotalExprInfo = sTotalExprInfo;

    
    /****************************************************************
     * Constant Expression 공간 설정
     ****************************************************************/

    /**
     * Constant Expression Stack 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlExprStack ),
                                (void **) & sConstExprStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Constant Expression Stack 초기화
     */
        
    STL_TRY( knlExprInit( sConstExprStack,
                          sConstExpr->mIncludeExprCnt,
                          aRegionMem,
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    
    /**
     * Statement Expression List의 Constant Expression Stack 설정
     */

    sStmtExprList.mConstExprStack = sConstExprStack;


    /****************************************************************
     * Expression Stack 구성
     ****************************************************************/

    /**
     * 각 Expression을 하나의 Stack으로 구성
     */

    STL_TRY( qloAddExprStackEntry( aSQLString,
                                   sExprCode,
                                   NULL, /* Bind Context */
                                   sConstExprStack,
                                   sConstExprStack,
                                   sConstExprStack->mEntryCount + 1,
                                   KNL_BUILTIN_FUNC_INVALID,
                                   sTotalExprInfo,
                                   & sExprEntry,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Cast 설정 */
    if( aDataTypeDef->mNumericRadix == DTL_NUMERIC_PREC_RADIX_BINARY )
    {
        sPrecision = dtlBinaryToDecimalPrecision[ aDataTypeDef->mArgNum1 ];
    }
    else
    {
        sPrecision = aDataTypeDef->mArgNum1;
    }

    if( aDataTypeDef->mStringLengthUnit == DTL_STRING_LENGTH_UNIT_NA )
    {
        sStrLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
    }
    else
    {
        sStrLengthUnit = aDataTypeDef->mStringLengthUnit;
    }

    STL_TRY_THROW( ( aDataTypeDef->mDBType != DTL_TYPE_LONGVARCHAR ) &&
                   ( aDataTypeDef->mDBType != DTL_TYPE_LONGVARBINARY ),
                   RAMP_ERR_CONSTANT_NOT_SUPPORT_DATA_TYPE );

    STL_TRY( qloCastExprDBType( sExprCode,
                                sTotalExprInfo,
                                aDataTypeDef->mDBType,
                                sPrecision,
                                aDataTypeDef->mArgNum2,
                                sStrLengthUnit,
                                aDataTypeDef->mIntervalIndicator,
                                STL_FALSE,
                                STL_FALSE,
                                aEnv )
             == STL_SUCCESS );

    sDBType = & sTotalExprInfo->mExprDBType[ sExprCode->mOffset ];

    STL_TRY_THROW( ( sDBType->mDBType != DTL_TYPE_LONGVARCHAR ) &&
                   ( sDBType->mDBType != DTL_TYPE_LONGVARBINARY ),
                   RAMP_ERR_CONSTANT_NOT_SUPPORT_DATA_TYPE );
        
    STL_TRY( qloSetExprDBType( sConstExpr,
                               sTotalExprInfo,
                               sDBType->mDBType,
                               sDBType->mArgNum1,
                               sDBType->mArgNum2,
                               sDBType->mStringLengthUnit,
                               sDBType->mIntervalIndicator,
                               aEnv )
             == STL_SUCCESS );

    STL_TRY( knlAnalyzeExprStack( sConstExprStack, KNL_ENV(aEnv) ) == STL_SUCCESS );

    
    /****************************************************************
     * Context 구성
     ****************************************************************/

    /**
     * Data Area 공간 할당
     */

    stlMemset( & sDataArea, 0x00, STL_SIZEOF( qllDataArea ) );


    /**
     * Block Allocation Count 설정 => Block Alloc Count로 설정
     */

    sDataArea.mBlockAllocCnt = knlGetPropertyBigIntValueByID( KNL_PROPERTY_BLOCK_READ_COUNT,
                                                               KNL_ENV(aEnv) );
    
    /**
     * Execution 수행시 첫번째 Execution 인지 여부
     */

    sDataArea.mIsFirstExecution = STL_TRUE;
    
        
    /**
     * Expression Context Data 공간 할당
     */

    STL_TRY( knlCreateContextInfo( sStmtExprList.mTotalExprInfo->mExprCnt,
                                   & sDataArea.mExprDataContext,
                                   aRegionMem,
                                   KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Data Optimization Info 설정
     */

    sDataOptInfo.mStmtExprList      = & sStmtExprList;
    sDataOptInfo.mDataArea          = & sDataArea;
    sDataOptInfo.mSQLStmt           = aSQLStmt; 
    sDataOptInfo.mWriteRelationNode = NULL;
    sDataOptInfo.mBindContext       = NULL;
    sDataOptInfo.mRegionMem         = aRegionMem;


    /**
     * Expression 을 위한 Data Context 생성 
     * - Expression Data Context 는
     *   Expression 의 Code Stack 이 참조할 수 있도록
     *   Value Block 과 Context 의 관계를 형성한다.
     */

    STL_TRY( qloDataOptimizeExpressionAll( & sDataOptInfo,
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Expression Result Column 공간 확보
     */

    STL_DASSERT( sStmtExprList.mAllExprList->mCount > 0 );

    STL_TRY( knlInitShareBlockFromBlock(
                 & sDataArea.mConstResultBlock,
                 sDataArea.mExprDataContext->mContexts[ sConstExpr->mOffset ].mInfo.mValueInfo,
                 aRegionMem,
                 KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Code Stack 수행
     */
    
    STL_TRY( qlxEvaluateConstExpr( sStmtExprList.mConstExprStack,
                                   & sDataArea,
                                   KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * OUTPUT 설정
     */

    STL_TRY( qlndAllocDataValueArray( & aSQLStmt->mLongTypeValueList,
                                      *aDataValue,
                                      sDBType->mDBType,
                                      sDBType->mArgNum1,
                                      sDBType->mStringLengthUnit,
                                      1, /* aArraySize */
                                      aRegionMem,
                                      & sBufferSize,
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_FIRST_DATA_VALUE( sDataArea.mConstResultBlock ),
                               *aDataValue,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

#ifdef STL_DEBUG
    STL_DASSERT( sLongValueCount == aSQLStmt->mLongTypeValueList.mCount );
#endif
    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_CONSTANT_VALUE_EXPRESSION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CONSTANT_VALUE_EXPRESSION_EXPECTED,
                      sDetailErrMsg,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_CONSTANT_NOT_SUPPORT_DATA_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CONSTANT_NOT_SUPPORT_DATA_TYPE,
                      qlgMakeErrPosString( aSQLString,
                                           sConstExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    if( stlGetErrorStackDepth( QLL_ERROR_STACK( aEnv ) ) > 0 )
    {
        if( stlGetLastDetailErrorMessage( QLL_ERROR_STACK( aEnv ) ) == NULL )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK( aEnv ),
                                          qlgMakeErrPosString( aSQLString,
                                                               aExprSource->mNodePos,
                                                               aEnv ) );
        }
    }
    
    return STL_FAILURE;
}


/** @} qlv */

