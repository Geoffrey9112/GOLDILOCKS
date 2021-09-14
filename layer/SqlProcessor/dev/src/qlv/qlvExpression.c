/*******************************************************************************
 * qlvExpression.c
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
 * @file qlvExpression.c
 * @brief SQL Processor Layer Expression Validation
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlv
 * @{
 */


/**
 * @brief Expression validation
 * @param[in]      aStmtInfo             Stmt Information
 * @param[in]      aPhraseType           Expr 이 사용된 Phrase 유형 
 * @param[in]      aValidationObjList    Validation Object List
 * @param[in]      aRefTableList         Reference Table List
 * @param[in]      aIsAtomic             Atomic Statement인지 여부
 * @param[in]      aIsRowExpr            Row Expression인지 여부
 * @param[in]      aAllowedAggrDepth     Allowed Aggregation Depth
 * @param[in]      aExprSource           Expression Source
 * @param[out]     aExprCode             Expression Code
 * @param[out]     aHasOuterJoinOperator Outer Join Operator를 가졌는지 여부
 * @param[in]      aRelatedFuncId        Related Function ID
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime        상위 노드의 Iteration Time
 * @param[in]      aRegionMem            Region Memory
 * @param[in]      aEnv                  Environment
 *
 * @remark Iteration Time에 따른 constant expression node 구성 여부
 *    <BR> (상위\현재)   EACH_EXPR  BEFORE_FETCH  AFTER_FETCH  EACH_STMT  NONE
 *    <BR> EACH_EXPR          X            X            X         ( O )    ( O )
 *    <BR> BEFORE_FETCH       X            X            X         ( O )    ( O )
 *    <BR> AFTER_FETCH        X            X            X         ( O )    ( O )
 *    <BR> EACH_STMT          X            X            X           X        X
 *    <BR> NONE               X            X            X           X        X  
 */
stlStatus qlvValidateValueExpr( qlvStmtInfo         * aStmtInfo,
                                qlvExprPhraseType     aPhraseType,
                                ellObjectList       * aValidationObjList,
                                qlvRefTableList     * aRefTableList,
                                stlBool               aIsAtomic,
                                stlBool               aIsRowExpr,
                                stlInt32              aAllowedAggrDepth,
                                qllNode             * aExprSource,
                                qlvInitExpression  ** aExprCode,
                                stlBool             * aHasOuterJoinOperator,
                                knlBuiltInFunc        aRelatedFuncId,
                                dtlIterationTime      aIterationTimeScope,
                                dtlIterationTime      aIterationTime,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv )
{
    qllNode            * sNode        = NULL;
    stlBool              sIsValueExpr = STL_TRUE;
    qlvInitExpression  * sExprCode    = NULL;
    stlBool              sIsUpdatable = STL_FALSE;
    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTimeScope >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTimeScope < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTime >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTime < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * Expression Validation
     */

    sNode = aExprSource;

    while( sIsValueExpr == STL_TRUE )
    {
        switch( sNode->mType )
        {
            case QLL_VALUE_EXPR_TYPE :
                {
                    /* value expression */
                    sNode = (qllNode *) ((qlpValueExpr *) sNode)->mExpr;
                    continue;
                }
                
            case QLL_BNF_CONSTANT_TYPE :
                {
                    /* constant value */
                    sNode = (qllNode *) ((qlpConstantValue *) sNode)->mValue;
                    continue;
                }
                
            case QLL_BNF_INTPARAM_CONSTANT_TYPE :
                {
                    /* value */
                    STL_TRY( qlvValidateIntParamBNF( aStmtInfo,
                                                     (qlpValue *) sNode,
                                                     & sExprCode,
                                                     aRelatedFuncId,
                                                     aIterationTimeScope,
                                                     aIterationTime,
                                                     aRegionMem,
                                                     aEnv )
                             == STL_SUCCESS );
                    break;
                }

            case QLL_BNF_NUMBER_CONSTANT_TYPE :
                {
                    /* value */
                    STL_TRY( qlvValidateNumberBNF( aStmtInfo,
                                                   (qlpValue *) sNode,
                                                   & sExprCode,
                                                   aRelatedFuncId,
                                                   aIterationTimeScope,
                                                   aIterationTime,
                                                   aRegionMem,
                                                   aEnv )
                             == STL_SUCCESS );
                    break;
                }
                
            case QLL_BNF_STRING_CONSTANT_TYPE :
                {
                    /* value */
                    STL_TRY( qlvValidateStringBNF( aStmtInfo,
                                                   (qlpValue *) sNode,
                                                   & sExprCode,
                                                   aRelatedFuncId,
                                                   aIterationTimeScope,
                                                   aIterationTime,
                                                   aRegionMem,
                                                   aEnv )
                             == STL_SUCCESS );
                    break;
                }
                
            case QLL_BNF_BITSTR_CONSTANT_TYPE :
                {
                    /* value */
                    STL_TRY( qlvValidateBitStringBNF( aStmtInfo,
                                                      (qlpValue *) sNode,
                                                      & sExprCode,
                                                      aRelatedFuncId,
                                                      aIterationTimeScope,
                                                      aIterationTime,
                                                      aRegionMem,
                                                      aEnv )
                             == STL_SUCCESS );
                    break;
                }

            case QLL_BNF_NULL_CONSTANT_TYPE :
                {
                    /* value */
                    STL_TRY( qlvValidateNullBNF( aStmtInfo,
                                                 (qlpValue *) sNode,
                                                 & sExprCode,
                                                 aRelatedFuncId,
                                                 aIterationTimeScope,
                                                 aIterationTime,
                                                 aRegionMem,
                                                 aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case QLL_BNF_DATE_CONSTANT_TYPE :
            case QLL_BNF_TIME_CONSTANT_TYPE :
            case QLL_BNF_TIME_TZ_CONSTANT_TYPE :
            case QLL_BNF_TIMESTAMP_CONSTANT_TYPE :
            case QLL_BNF_TIMESTAMP_TZ_CONSTANT_TYPE :
                {
                    /* value */
                    STL_TRY( qlvValidateDateTimeBNF( aStmtInfo,
                                                     (qlpValue *) sNode,
                                                     & sExprCode,
                                                     aRelatedFuncId,
                                                     aIterationTimeScope,
                                                     aIterationTime,
                                                     aRegionMem,
                                                     aEnv )
                             == STL_SUCCESS );
                    break;
                }

            case QLL_COLUMN_REF_TYPE :
                {
                    /* column */
                    sIsUpdatable = STL_TRUE;
                    STL_TRY( qlvValidateColumnRef( aStmtInfo,
                                                   aPhraseType,
                                                   aValidationObjList,
                                                   aRefTableList,
                                                   aIsAtomic,
                                                   aAllowedAggrDepth,
                                                   (qlpColumnRef *) sNode,
                                                   & sExprCode,
                                                   aHasOuterJoinOperator,
                                                   & sIsUpdatable,
                                                   aRelatedFuncId,
                                                   aIterationTimeScope,
                                                   aIterationTime,
                                                   aRegionMem,
                                                   aEnv )
                             == STL_SUCCESS );
                    break;
                }

            case QLL_PARAM_TYPE :
                {
                    /* bind */
                    STL_TRY( qlvValidateBindParam( aStmtInfo,
                                                   aPhraseType,
                                                   aIsAtomic,
                                                   (qlpParameter *) sNode,
                                                   & sExprCode,
                                                   aRelatedFuncId,
                                                   aIterationTimeScope,
                                                   aIterationTime,
                                                   aRegionMem,
                                                   aEnv )
                             == STL_SUCCESS );
                    break;
                }

            case QLL_FUNCTION_TYPE :
                {
                    /* built-in function */
                    STL_TRY( qlvValidateFunction( aStmtInfo,
                                                  aPhraseType,
                                                  aValidationObjList,
                                                  aRefTableList,
                                                  aIsAtomic,
                                                  aAllowedAggrDepth,
                                                  (qlpFunction *) sNode,
                                                  & sExprCode,
                                                  aHasOuterJoinOperator,
                                                  aRelatedFuncId,
                                                  aIterationTimeScope,
                                                  aIterationTime,
                                                  aRegionMem,
                                                  aEnv )
                             == STL_SUCCESS );
                    break;
                }
                
            case QLL_TYPECAST_TYPE :
                {
                    /* cast */
                    STL_TRY( qlvValidateCast( aStmtInfo,
                                              aPhraseType,
                                              aValidationObjList,
                                              aRefTableList,
                                              aIsAtomic,
                                              aAllowedAggrDepth,
                                              (qlpTypeCast *) sNode,
                                              & sExprCode,
                                              aHasOuterJoinOperator,
                                              aRelatedFuncId,
                                              aIterationTimeScope,
                                              aIterationTime,
                                              aRegionMem,
                                              aEnv )
                             == STL_SUCCESS );
                    break;
                }

            case QLL_PSEUDO_COLUMN_TYPE :
                {
                    /* pseudo column */
                    STL_TRY( qlvValidatePseudoCol( aStmtInfo,
                                                   aPhraseType,
                                                   aValidationObjList,
                                                   aRefTableList,
                                                   aIsAtomic,
                                                   aAllowedAggrDepth,
                                                   (qlpPseudoCol *) sNode,
                                                   & sExprCode,
                                                   aHasOuterJoinOperator,
                                                   aRelatedFuncId,
                                                   aIterationTimeScope,
                                                   aIterationTime,
                                                   aRegionMem,
                                                   aEnv )
                             == STL_SUCCESS );
                    break;
                }

            case QLL_SEQUENCE_NAME_TYPE :
                {
                    /* sequence name */
                    STL_TRY( qlvValidateSequenceRef( aStmtInfo,
                                                     aValidationObjList,
                                                     (qlpObjectName *) sNode,
                                                     & sExprCode,
                                                     aRelatedFuncId,
                                                     aIterationTimeScope,
                                                     aIterationTime,
                                                     aRegionMem,
                                                     aEnv )
                             == STL_SUCCESS );
                    
                    break;
                }

            case QLL_AGGREGATION_TYPE :
                {
                    /* aggregation */
                    STL_TRY( qlvValidateAggregation( aStmtInfo,
                                                     aPhraseType,
                                                     aValidationObjList,
                                                     aRefTableList,
                                                     aIsAtomic,
                                                     aAllowedAggrDepth,
                                                     (qlpAggregation *) sNode,
                                                     & sExprCode,
                                                     aHasOuterJoinOperator,
                                                     aRelatedFuncId,
                                                     aIterationTimeScope,
                                                     aIterationTime,
                                                     aRegionMem,
                                                     aEnv )
                             == STL_SUCCESS );

                    break;
                }

            case QLL_CASE_SEARCHED_EXPR :
            case QLL_CASE_SIMPLE_EXPR :
            case QLL_NULLIF_EXPR :
            case QLL_COALESCE_EXPR :
            case QLL_NVL_EXPR :
            case QLL_NVL2_EXPR :
            case QLL_DECODE_EXPR :
            case QLL_CASE2_EXPR :
                {
                    STL_TRY( qlvValidateCaseExpr( aStmtInfo,
                                                  aPhraseType,
                                                  aValidationObjList,
                                                  aRefTableList,
                                                  aIsAtomic,
                                                  aAllowedAggrDepth,
                                                  (qlpCaseExpr *) sNode,
                                                  & sExprCode,
                                                  aHasOuterJoinOperator,
                                                  aRelatedFuncId,
                                                  aIterationTimeScope,
                                                  aIterationTime,
                                                  aRegionMem,
                                                  aEnv )
                             == STL_SUCCESS );
                    break;
                }

            case QLL_LIST_FUNCTION_TYPE:
                {
                    STL_TRY( qlvValidateListFunction( aStmtInfo,
                                                      aPhraseType,
                                                      aValidationObjList,
                                                      aRefTableList,
                                                      aIsAtomic,
                                                      aAllowedAggrDepth,
                                                      (qlpListFunc *) sNode,
                                                      & sExprCode,
                                                      aHasOuterJoinOperator,
                                                      aRelatedFuncId,
                                                      aIterationTimeScope,
                                                      aIterationTime,
                                                      aRegionMem,
                                                      aEnv )
                             == STL_SUCCESS );
                    break;
                }

            case QLL_LIST_COLUMN_TYPE:
                {
                    STL_TRY( qlvValidateListColumn( aStmtInfo,
                                                    aPhraseType,
                                                    aValidationObjList,
                                                    aRefTableList,
                                                    aIsAtomic,
                                                    aAllowedAggrDepth,
                                                    (qlpListCol *) sNode,
                                                    & sExprCode,
                                                    aHasOuterJoinOperator,
                                                    aRelatedFuncId,
                                                    aIterationTimeScope,
                                                    aIterationTime,
                                                    aRegionMem,
                                                    aEnv )
                             == STL_SUCCESS );
                    break;
                }

            case QLL_ROW_EXPR_TYPE:
                {
                    STL_TRY( qlvValidateRowExpr( aStmtInfo,
                                                 aPhraseType,
                                                 aValidationObjList,
                                                 aRefTableList,
                                                 aIsAtomic,
                                                 aAllowedAggrDepth,
                                                 (qlpRowExpr *) sNode,
                                                 & sExprCode,
                                                 aHasOuterJoinOperator,
                                                 aRelatedFuncId,
                                                 aIterationTimeScope,
                                                 aIterationTime,
                                                 aRegionMem,
                                                 aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case QLL_SUB_TABLE_NODE_TYPE:
                {
                    STL_TRY( qlvValidateSubQuery( aStmtInfo,
                                                  aPhraseType,
                                                  aValidationObjList,
                                                  aRefTableList,
                                                  aIsAtomic,
                                                  aIsRowExpr,
                                                  aAllowedAggrDepth,
                                                  (qlpSubTableNode *) sNode,
                                                  & sExprCode,
                                                  aRelatedFuncId,
                                                  aIterationTimeScope,
                                                  aIterationTime,
                                                  aRegionMem,
                                                  aEnv )
                             == STL_SUCCESS );
                    
                    break;
                }
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }

        sIsValueExpr = STL_FALSE;
    }


    /**
     * OUTPUT 설정
     */

    STL_TRY_THROW( aIterationTimeScope <= sExprCode->mIterationTime,
                   RAMP_ERR_INVALID_ITERATION_TIME_SCOPE );

    sExprCode->mPosition = aExprSource->mNodePos;
    
    *aExprCode = sExprCode;

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ITERATION_TIME_SCOPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ITERATION_TIME_SCOPE_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aExprSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Bind Parameter validation
 * @param[in]      aStmtInfo      Stmt Information
 * @param[in]      aPhraseType    Expr 이 사용된 Phrase 유형
 * @param[in]      aIsAtomic      Atomic Statement인지 여부
 * @param[in]      aExprSource    Expression Source
 * @param[out]     aExprCode      Expression Code
 * @param[in]      aRelatedFuncId Related Function ID
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime 상위 노드의 Iteration Time
 * @param[in]      aRegionMem     Region Memory
 * @param[in]      aEnv           Environment
 */
stlStatus qlvValidateBindParam( qlvStmtInfo         * aStmtInfo,
                                qlvExprPhraseType     aPhraseType,
                                stlBool               aIsAtomic,
                                qlpParameter        * aExprSource,
                                qlvInitExpression  ** aExprCode,
                                knlBuiltInFunc        aRelatedFuncId,
                                dtlIterationTime      aIterationTimeScope,
                                dtlIterationTime      aIterationTime,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv )
{
    qlvInitExpression  * sExprCode  = NULL;
    qlvInitBindParam   * sParameter = NULL;
    stlChar            * sValueStr  = NULL;
    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource->mType == QLL_PARAM_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTimeScope >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTimeScope < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTime >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTime < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Bind Parameter 변수는 DEFAULT Phrase에서 사용할 수 없다.
     */

    STL_TRY_THROW( aPhraseType != QLV_EXPR_PHRASE_DEFAULT,
                   RAMP_ERR_DEFAULT_HAS_ERRORS );

    
    /**
     * Code Parameter Value 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitBindParam ),
                                (void **) & sParameter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sParameter,
               0x00,
               STL_SIZEOF( qlvInitBindParam ) );


    /**
     * Code Null Value 설정
     */

    sParameter->mBindType     = aExprSource->mInOutType;
    sParameter->mBindParamIdx = aExprSource->mParamId;

    /**
     * Bind I/O Type 정보 설정
     * - PSM 등이 되면 이에 대한 처리가 필요함.
     */
    
    qllSetBindIOType( aStmtInfo->mSQLStmt,
                      sParameter->mBindParamIdx,
                      aExprSource->mNodePos,
                      sParameter->mBindType );
    
    /* Host Name */
    if( aExprSource->mHostName == NULL )
    {
        sParameter->mHostName = NULL;
    }
    else
    {
        sValueStr = QLP_GET_PTR_VALUE( aExprSource->mHostName );

        STL_DASSERT( sValueStr != NULL );
        
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    stlStrlen( sValueStr ) + 1,
                                    (void **) & sParameter->mHostName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( sParameter->mHostName, sValueStr );
    }
    
    /* Indecator Name */
    if( aExprSource->mIndicatorName == NULL )
    {
        sParameter->mIndicatorName = NULL;
    }
    else
    {
        sValueStr = QLP_GET_PTR_VALUE( aExprSource->mIndicatorName );

        STL_DASSERT( sValueStr != NULL );

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    stlStrlen( sValueStr ) + 1,
                                    (void **) & sParameter->mIndicatorName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlStrcpy( sParameter->mIndicatorName, sValueStr );
    }


    /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sExprCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sExprCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

    
    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );
    
    sExprCode->mType            = QLV_EXPR_TYPE_BIND_PARAM;
    sExprCode->mPosition        = aExprSource->mNodePos;
    sExprCode->mIncludeExprCnt  = 1;
    sExprCode->mExpr.mBindParam = sParameter;
    sExprCode->mRelatedFuncId   = aRelatedFuncId;
    sExprCode->mColumnName      = NULL;

    if( aIsAtomic == STL_TRUE )
    {
        sExprCode->mIterationTime = DTL_ITERATION_TIME_FOR_EACH_EXPR;
    }
    else
    {
        sExprCode->mIterationTime = DTL_ITERATION_TIME_FOR_EACH_STATEMENT;
    }


    /**
     * OUTPUT 설정
     */
        
    *aExprCode = sExprCode;

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DEFAULT_HAS_ERRORS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DEFAULT_EXPRESSION_HAS_ERRORS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aExprSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief RETURNING phrase 를 validation 함.
 * @param[in]  aStmtInfo             Stmt Information
 * @param[in]  aValidationObjList    Validation Object List
 * @param[in]  aTableNode            Table Node
 * @param[in]  aParseReturnTarget    RETURN target parse tree
 * @param[out] aReturnTargetCnt      RETURN target count
 * @param[out] aInitReturnTarget     RETURN target init plan
 * @param[in]  aEnv                  Environment
 * @remarks
 */
stlStatus qlvValidateReturningPhrase( qlvStmtInfo     * aStmtInfo,
                                      ellObjectList   * aValidationObjList,
                                      qlvInitNode     * aTableNode,
                                      qlpList         * aParseReturnTarget,
                                      stlInt32        * aReturnTargetCnt,
                                      qlvInitTarget  ** aInitReturnTarget,
                                      qllEnv          * aEnv )
{
    stlInt32          sReturnCnt        = 0;
    qlvInitTarget   * sInitTargetArray  = NULL;

    qlpListElement  * sListElem         = NULL;
    qlpTarget       * sReturnNode       = NULL;

    stlInt32          i                 = 0;
    stlInt32          sCount            = 0;

    qlvRefTableList   sTempTableList;
    qlvRefTableItem   sTempTableItem;

    stlChar        ** sExceptNameArr    = NULL;

    qlvRefColExprList  * sPrivColList = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseReturnTarget != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReturnTargetCnt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitReturnTarget != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * RETURN 절 Validation
     */

    sListElem = QLP_LIST_GET_FIRST( aParseReturnTarget );
    sReturnNode = (qlpTarget *) QLP_LIST_GET_POINTER_VALUE( sListElem );

    if ( ( sReturnNode->mType == QLL_TARGET_ASTERISK_TYPE ) &&
         ( sReturnNode->mRelName == NULL ) )
    {   
        /**
         * RETURNING * 인 경우
         */

        STL_TRY( qlvGetAsteriskTargetCountMultiTables( aStmtInfo,
                                                       aTableNode,
                                                       0,
                                                       NULL,
                                                       &sReturnCnt,
                                                       aEnv )
                 == STL_SUCCESS );


        /**
         * RETURNING 절의 Init Expression 공간 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(qlvInitTarget) * sReturnCnt,
                                    (void **) & sInitTargetArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sInitTargetArray, 0x00, STL_SIZEOF(qlvInitTarget) * sReturnCnt );

        /**
         * Asterisk Validation
         */

        STL_TRY( qlvValidateAsteriskMultiTables( aStmtInfo,
                                                 QLV_EXPR_PHRASE_RETURN,
                                                 aTableNode,
                                                 sReturnNode,
                                                 0,
                                                 NULL,
                                                 sInitTargetArray,
                                                 &sCount,
                                                 aEnv )
                 == STL_SUCCESS );

        /* expression의 position 정보 설정 */
        sCount = sReturnCnt;
        while( sCount > 0 )
        {
            sCount--;
            STL_DASSERT( sInitTargetArray[sCount].mExpr->mPosition != 0 );
        }
    }
    else
    {   
        /**
         * RETURNING expr, ... 인 경우
         */

        STL_TRY( qlvGetTargetCountFromTargetList( aStmtInfo,
                                                  aParseReturnTarget,
                                                  aTableNode,
                                                  0,
                                                  sExceptNameArr,
                                                  &sReturnCnt,
                                                  aEnv )
                 == STL_SUCCESS );

        /**
         * RETURNING 절의 Init Expression 공간 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF(qlvInitTarget) * sReturnCnt,
                                    (void **) & sInitTargetArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sInitTargetArray, 0x00, STL_SIZEOF(qlvInitTarget) * sReturnCnt );


        /**
         * Temp Reference Table List 초기화
         */

        sTempTableItem.mTableNode   = aTableNode;
        sTempTableItem.mTargetCount = 0;
        sTempTableItem.mTargets     = NULL;
        sTempTableItem.mNext        = NULL;

        sTempTableList.mCount       = 1;
        sTempTableList.mHead        = &sTempTableItem;


        /**
         * Target 설정
         */

        i = 0;

        QLP_LIST_FOR_EACH( aParseReturnTarget, sListElem )
        {   
            sReturnNode = (qlpTarget *) QLP_LIST_GET_POINTER_VALUE( sListElem );

            if( sReturnNode->mType == QLL_TARGET_ASTERISK_TYPE )
            {
                STL_DASSERT( sReturnNode->mRelName != NULL );

                STL_TRY( qlvValidateAsteriskMultiTables( aStmtInfo,
                                                         QLV_EXPR_PHRASE_RETURN,
                                                         aTableNode,
                                                         sReturnNode,
                                                         0,
                                                         NULL,
                                                         & sInitTargetArray[i],
                                                         &sCount,
                                                         aEnv )
                         == STL_SUCCESS );
                
                i += sCount;
            }
            else
            {
                STL_TRY( qlvValidateReturn( aStmtInfo,
                                            aValidationObjList,
                                            &sTempTableList,
                                            0, /* Allowed Aggregation Depth */
                                            sReturnNode,
                                            & sInitTargetArray[i],
                                            DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                            DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                            aEnv )
                         == STL_SUCCESS );
                i++;
            }
        }
    }

    /**
     * Privilege Column List 구성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF( qlvRefColExprList ),
                                (void **) & sPrivColList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sPrivColList->mHead = NULL;
    
    for ( i = 0; i < sReturnCnt; i++ )
    {
        STL_TRY( qlvGetRefColList( sInitTargetArray[i].mExpr,
                                   sInitTargetArray[i].mExpr,
                                   sPrivColList,
                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                   aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Data Type Info 설정
     */

    for( i = 0 ; i < sReturnCnt ; i++ )
    {
        STL_TRY( qlvGetExprDataTypeInfo( aStmtInfo->mSQLString,
                                         & QLL_VALIDATE_MEM( aEnv ),
                                         sInitTargetArray[i].mExpr,
                                         NULL,
                                         & sInitTargetArray[i].mDataTypeInfo,
                                         aEnv )
                 == STL_SUCCESS );
    }

    /**
     * RETURN target 에 대한 SELECT 권한 검사
     */

    if ( sPrivColList->mHead == NULL )
    {
        /**
         * RETURN 절은 있으나, 읽는 컬럼이 없음.
         * SELECT 권한 검사 필요 없음
         */
    }
    else
    {
        STL_TRY( qlaCheckTablePriv( aStmtInfo->mTransID,
                                    aStmtInfo->mDBCStmt,
                                    aStmtInfo->mSQLStmt,
                                    ELL_TABLE_PRIV_ACTION_SELECT,
                                    & ((qlvInitBaseTableNode*) aTableNode)->mTableHandle,
                                    sPrivColList->mHead->mColExprList,
                                    aEnv )
                 == STL_SUCCESS );
    }

    /**
     * Output 설정
     */

    *aReturnTargetCnt  = sReturnCnt;
    *aInitReturnTarget = sInitTargetArray;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief INTO phrase 를 validation 한다 (OUT Parameter 대상임)
 * @param[in]      aStmtInfo        Stmt Information
 * @param[in]      aTargetCount     Target 개수
 * @param[in]      aIntoClause      INTO 구문의 Parse Tree
 * @param[out]     aInitIntoArray   INTO 구문의 init plan array
 * @param[in]      aEnv             Environment
 * @remarks
 */
stlStatus qlvValidateIntoPhrase( qlvStmtInfo         * aStmtInfo,
                                 stlInt32              aTargetCount,
                                 qlpIntoClause       * aIntoClause,
                                 qlvInitExpression   * aInitIntoArray,
                                 qllEnv              * aEnv )
{
    qlpList            * sParamList = NULL;
    qlpListElement     * sParamElem = NULL;
    qlpParameter       * sParamNode = NULL;

    qlvInitExpression  * sExprCode  = NULL;
    stlInt32             i;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIntoClause != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitIntoArray != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Target 개수와 INTO 절의 호스트 변수 개수가 동일한 지 확인
     */

    sParamList = aIntoClause->mParameters;

    STL_TRY_THROW( aTargetCount == QLP_LIST_GET_COUNT( sParamList ),
                   RAMP_ERR_INTO_PARAMETER_COUNT_MISMATCH );
    
    /**
     * INTO parameter 를 Validation 한다.
     */

    i = 0;
    
    QLP_LIST_FOR_EACH( sParamList, sParamElem )
    {
        sParamNode = (qlpParameter *) QLP_LIST_GET_POINTER_VALUE( sParamElem );

        STL_TRY( qlvValidateBindParam( aStmtInfo,
                                       QLV_EXPR_PHRASE_TARGET,
                                       STL_FALSE,
                                       sParamNode,
                                       & sExprCode,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
                                       DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
                                       QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        aInitIntoArray[i] = *sExprCode;
        i++;
    }
    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTO_PARAMETER_COUNT_MISMATCH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NUMBER_OF_ELEMENTS_INTO_LIST_MISMATCH,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aIntoClause->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
                        
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Function validation
 * @param[in]      aStmtInfo        Stmt Information
 * @param[in]      aPhraseType      Expr 이 사용된 Phrase 유형 
 * @param[in]      aValidationObjList    Validation Object List
 * @param[in]      aRefTableList         Reference Table List
 * @param[in]      aIsAtomic        Atomic Statement인지 여부
 * @param[in]      aAllowedAggrDepth     Allowed Aggregation Depth
 * @param[in]      aFunctionSource  Function
 * @param[out]     aExprCode        Expression 
 * @param[out]     aHasOuterJoinOperator Outer Join Operator를 가졌는지 여부
 * @param[in]      aRelatedFuncId   Related Function ID
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime  상위 노드의 Iteration Time
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvValidateFunction( qlvStmtInfo         * aStmtInfo,
                               qlvExprPhraseType     aPhraseType,
                               ellObjectList       * aValidationObjList,
                               qlvRefTableList     * aRefTableList,
                               stlBool               aIsAtomic,
                               stlInt32              aAllowedAggrDepth,
                               qlpFunction         * aFunctionSource,
                               qlvInitExpression  ** aExprCode,
                               stlBool             * aHasOuterJoinOperator,
                               knlBuiltInFunc        aRelatedFuncId,
                               dtlIterationTime      aIterationTimeScope,
                               dtlIterationTime      aIterationTime,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv )
{
    qlvInitExpression  * sFuncCode       = NULL;
    qlvInitExpression  * sArgCode        = NULL;
    qlvInitFunction    * sFunction       = NULL;
    qlpListElement     * sListElem       = NULL;   
    qllNode            * sNode           = NULL;
    stlInt64             sIncludeExprCnt = 0;
    stlInt32             sArgCount       = 0; 
    dtlIterationTime     sIterTime       = DTL_ITERATION_TIME_INVALID;

    stlBool              sHasOuterJoinOperator  = STL_FALSE;
    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFunctionSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFunctionSource->mType == QLL_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Iteration Time Validation
     */

    STL_TRY_THROW( aIterationTimeScope <= gBuiltInFuncInfoArr[ aFunctionSource->mFuncId ].mIterationTime,
                   RAMP_ERR_INVALID_ITERATION_TIME_SCOPE );

        
    /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sFuncCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sFuncCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

    /**
     * Code Function 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitFunction ),
                                (void **) & sFunction,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sFunction,
               0x00,
               STL_SIZEOF( qlvInitFunction ) );


    /**
     * Code Function 설정
     */

    sFunction->mFuncId = aFunctionSource->mFuncId;


    /**
     * Arguments 설정
     */
    
    if( aFunctionSource->mArgs != NULL )
    {
        /**
         * Argument List 공간 할당
         */
        
        sFunction->mArgCount = aFunctionSource->mArgs->mCount;

        STL_TRY( knlAllocRegionMem(
                     aRegionMem,
                     STL_SIZEOF( qlvInitExpression* ) * sFunction->mArgCount,
                     (void **) & sFunction->mArgs,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( sFunction->mArgs,
                   0x00,
                   STL_SIZEOF( qlvInitExpression* ) * sFunction->mArgCount );
        
        
        /**
         * Argument List 구성
         */

        sIterTime = gBuiltInFuncInfoArr[ sFunction->mFuncId ].mIterationTime;
    
        QLP_LIST_FOR_EACH( aFunctionSource->mArgs, sListElem )
        {
            sNode = (qllNode*) QLP_LIST_GET_POINTER_VALUE( sListElem );

            sHasOuterJoinOperator = STL_FALSE;
            STL_TRY( qlvValidateValueExpr(
                         aStmtInfo,
                         aPhraseType,
                         aValidationObjList,
                         aRefTableList,
                         aIsAtomic,
                         STL_FALSE,  /* Row Expr */
                         aAllowedAggrDepth,
                         (qllNode *) sNode,
                         & sArgCode,
                         &sHasOuterJoinOperator,
                         sFunction->mFuncId,
                         aIterationTimeScope,
                         gBuiltInFuncInfoArr[ sFunction->mFuncId ].mIterationTime,
                         aRegionMem,
                         aEnv )
                     == STL_SUCCESS );

            /**
             * Outer Join Operator Check
             * => OR Function의 Operand에는 Outer Join Operator를 기술할 수 없다.
             */

            if( aRelatedFuncId == KNL_BUILTIN_FUNC_OR )
            {
                STL_TRY_THROW( sHasOuterJoinOperator == STL_FALSE,
                               RAMP_ERR_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN_OR_OPERAND );
            }
            else
            {
                if( (aHasOuterJoinOperator != NULL) &&
                    (sHasOuterJoinOperator == STL_TRUE) )
                {
                    *aHasOuterJoinOperator = STL_TRUE;
                }
            }

            sIterTime = ( sIterTime < sArgCode->mIterationTime )
                ? sIterTime : sArgCode->mIterationTime;

            sIncludeExprCnt += sArgCode->mIncludeExprCnt;
            sFunction->mArgs[ sArgCount ] = sArgCode;

            sArgCount++;
        }
    }
    else
    {
        sFunction->mArgCount = 0;
        sFunction->mArgs     = NULL;
        sIterTime            = gBuiltInFuncInfoArr[ sFunction->mFuncId ].mIterationTime;
    }
    
    STL_DASSERT( sFunction->mArgCount == sArgCount );


    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( sIncludeExprCnt + 1, aEnv );
    
    sFuncCode->mType           = QLV_EXPR_TYPE_FUNCTION;
    sFuncCode->mPosition       = aFunctionSource->mNodePos;
    sFuncCode->mIncludeExprCnt = sIncludeExprCnt + 1;
    sFuncCode->mExpr.mFunction = sFunction;
    sFuncCode->mRelatedFuncId  = aRelatedFuncId;
    sFuncCode->mIterationTime  = sIterTime;
    sFuncCode->mColumnName     = NULL;


    /**
     * OUTPUT 설정
     */

    *aExprCode = sFuncCode;

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ITERATION_TIME_SCOPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ITERATION_TIME_SCOPE_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aFunctionSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN_OR_OPERAND )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aFunctionSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "operand of OR or the quantified comparison" );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}



/**
 * @brief Referenced Column validation
 * @param[in]      aStmtInfo             Stmt Information
 * @param[in]      aPhraseType           Expr 이 사용된 Phrase 유형 
 * @param[in]      aValidationObjList    Validation Object List
 * @param[in]      aRefTableList         Reference Table List
 * @param[in]      aIsAtomic             Atomic Statement인지 여부
 * @param[in]      aAllowedAggrDepth     Allowed Aggregation Depth
 * @param[in]      aColumnSource         Column Source
 * @param[out]     aColumnCode           Expression Code
 * @param[in,out]  aHasOuterJoinOperator Outer Join Operator를 가졌는지 여부
 * @param[in,out]  aIsUpdatable          Updatable Column 여부 
 * @param[in]      aRelatedFuncId        Related Function ID
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime        상위 노드의 Iteration Time
 * @param[in]      aRegionMem            Region Memory
 * @param[in]      aEnv                  Environment
 */
stlStatus qlvValidateColumnRef( qlvStmtInfo         * aStmtInfo,
                                qlvExprPhraseType     aPhraseType,
                                ellObjectList       * aValidationObjList,
                                qlvRefTableList     * aRefTableList,
                                stlBool               aIsAtomic,
                                stlInt32              aAllowedAggrDepth,
                                qlpColumnRef        * aColumnSource,
                                qlvInitExpression  ** aColumnCode,
                                stlBool             * aHasOuterJoinOperator,
                                stlBool             * aIsUpdatable,
                                knlBuiltInFunc        aRelatedFuncId,
                                dtlIterationTime      aIterationTimeScope,
                                dtlIterationTime      aIterationTime,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv )
{
    qlvInitExpression  * sExprCode    = NULL;
    qlpPseudoCol       * sMaySequence = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnSource->mType == QLL_COLUMN_REF_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Iteration Time Validation
     */

    STL_TRY_THROW( aIterationTimeScope <= DTL_ITERATION_TIME_FOR_EACH_EXPR,
                   RAMP_ERR_INVALID_ITERATION_TIME_SCOPE );

    
    /**
     * Column Name Validation
     * - 컬럼이 확실한 경우, column expression 을 획득함.
     * - 컬럼이 아니나 시퀀스 함수일 가능성이 있는 경우, sMaySequence 획득함.
     */

    STL_TRY( qlvValidateColumnName( aStmtInfo,
                                    aPhraseType,
                                    aRefTableList,
                                    aColumnSource->mName,
                                    aColumnSource->mIsSetOuterMark,
                                    & sMaySequence,
                                    & sExprCode,
                                    aIsUpdatable,
                                    aRelatedFuncId,
                                    aIterationTime,
                                    aRegionMem,
                                    aEnv )
             == STL_SUCCESS );
    
    if ( sMaySequence != NULL )
    {
        /**
         * - 임의로 생성한 Sequence Pseudo Function(NEXTVAL or CURRVAL) 을 Validation 
         */

        *aIsUpdatable = STL_FALSE;
        STL_TRY( qlvValidatePseudoCol( aStmtInfo,
                                       aPhraseType,
                                       aValidationObjList,
                                       aRefTableList,
                                       aIsAtomic,
                                       aAllowedAggrDepth,
                                       sMaySequence,
                                       & sExprCode,
                                       aHasOuterJoinOperator,
                                       aRelatedFuncId,
                                       aIterationTimeScope,
                                       aIterationTime,
                                       aRegionMem,
                                       aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /**
         * Column 인 경우
         */

        /* Do Nothing */
    }

    /* Constant Expression은 존재하지 않음 */
    
    /**
     * OUTPUT 설정
     */

    *aColumnCode = sExprCode;

    if( aHasOuterJoinOperator != NULL )
    {
        if( aColumnSource->mIsSetOuterMark == STL_TRUE )
        {
            *aHasOuterJoinOperator = STL_TRUE;
        }
    }
    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ITERATION_TIME_SCOPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ITERATION_TIME_SCOPE_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Column Name validation
 * @param[in]      aStmtInfo      Stmt Information
 * @param[in]      aPhraseType        Expr 이 사용된 Phrase 유형 
 * @param[in]      aRefTableList      Reference Table List
 * @param[in]      aColumnNameSource  Column Source
 * @param[in]      aIsSetOuterMark    Outer Join Operator 설정 여부
 * @param[out]     aMaySequence       가상의 Sequence Pseudo Parse Tree
 * @param[out]     aColumnExpr        Column Expression
 * @param[in,out]  aIsUpdatable       Updatable Column 여부 
 * @param[in]      aRelatedFuncId     Related Function ID
 * @param[in]      aIterationTime     상위 노드의 Iteration Time
 * @param[in]      aRegionMem         Region Memory
 * @param[in]      aEnv               Environment
 * @remarks
 * 에러가 발생하지 않는다면, out parameter 는 다음과 같이 결정된다.
 * - 컬럼인 경우 : aMaySeqPseudo(NULL), aColumnHandle(not null)
 * - 시퀀스 가능성이 있는 경우 : aMaySeqPseudo(not null), aColumnHandle(NULL)
 * - 모두 아닌 경우 : Validation Error 발생, aMaySeqPseudo(NULL), aColumnHandle(NULL)
 */
stlStatus qlvValidateColumnName( qlvStmtInfo        * aStmtInfo,
                                 qlvExprPhraseType    aPhraseType,        
                                 qlvRefTableList    * aRefTableList,
                                 qlpColumnName      * aColumnNameSource,
                                 stlBool              aIsSetOuterMark,
                                 qlpPseudoCol      ** aMaySequence,
                                 qlvInitExpression ** aColumnExpr,
                                 stlBool            * aIsUpdatable,
                                 knlBuiltInFunc       aRelatedFuncId,
                                 dtlIterationTime     aIterationTime,
                                 knlRegionMem       * aRegionMem,
                                 qllEnv             * aEnv )
{
    qlpPseudoCol          * sMaySeq        = NULL;
    qlpList               * sMayList       = NULL;
    qlpListElement        * sMayElement    = NULL;
    qlpObjectName         * sMayObjName    = NULL;

    stlBool                 sIsColumn      = STL_FALSE;
    stlBool                 sCanBeSeq      = STL_FALSE;
    qlvInitExpression     * sColumnExpr    = NULL;
    
    qlvRefTableItem       * sTableItem     = NULL;
    qlvInitTarget         * sTartget       = NULL;
    qlvInitBaseTableNode  * sBaseTableNode = NULL;
    qlvInitExpression     * sSearchedExpr  = NULL;
    
    stlInt32                sRefCount      = 0;
    stlInt32                sLoop          = 0;
    stlInt32                sLen           = 0;
    stlChar               * sErrString     = NULL;

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnNameSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnNameSource->mType == QLL_COLUMN_NAME_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnNameSource->mColumn != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aMaySequence != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnExpr != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Table Reference List가 없으면 Table Node에서 찾을 수 없다.
     */
    
    if( aRefTableList == NULL )
    {
        /**
         * Pseudo column 중 ROWID 는 올 수 없다.
         * @todo LEVEL, ROWNUM 도 마찬가지.
         */
        
        if( stlStrcmp( aColumnNameSource->mColumn, "ROWID" ) == 0 )
        {
            STL_THROW( RAMP_ERR_PSEUDO_COLUMN_NOT_ALLOWED );
        }
        else
        {
            sCanBeSeq = STL_TRUE;
        }
    }
    else
    {
        /**
         * @todo Target Clause에서 Name Validation
         */

        if( aPhraseType == QLV_EXPR_PHRASE_ORDERBY )
        {
            sTableItem = aRefTableList->mHead;

            while( sTableItem != NULL )
            {
                sRefCount = 0;
                sSearchedExpr = NULL;
                
                /* table 별로 Name Validation */
                for( sLoop = 0 ; sLoop < sTableItem->mTargetCount ; sLoop++ )
                {
                    sTartget = & sTableItem->mTargets[ sLoop ];

                    if( sTartget->mAliasName == NULL )
                    {
                        if( sTartget->mExpr->mColumnName == NULL )
                        {
                            continue;
                        }
                        
                        if( stlStrcmp( sTartget->mExpr->mColumnName,
                                       aColumnNameSource->mColumn ) != 0 )
                        {
                            continue;
                        }

                        if( aColumnNameSource->mTable == NULL )
                        {
                            if( sRefCount == 0 )
                            {
                                sRefCount++;
                                sSearchedExpr = sTartget->mExpr;
                            }
                            else
                            {
                                STL_TRY_THROW( ( qlvIsSameExpression( sSearchedExpr, sTartget->mExpr )
                                                 == STL_TRUE ),
                                               RAMP_ERR_COLUMN_AMBIGUOUS );
                            }
                            continue;
                        }
                        
                        STL_DASSERT( sTableItem->mTableNode != NULL );

                        if( sTableItem->mTableNode->mType != QLV_NODE_TYPE_BASE_TABLE )
                        {
                            continue;
                        }
                        sBaseTableNode = (qlvInitBaseTableNode *) sTableItem->mTableNode;
                        
                        if( stlStrcmp( sBaseTableNode->mRelationName->mTable,
                                       aColumnNameSource->mTable ) != 0 )
                        {
                            continue;
                        }

                        if( aColumnNameSource->mSchema == NULL )
                        {
                            if( sRefCount == 0 )
                            {
                                sRefCount++;
                                sSearchedExpr = sTartget->mExpr;
                            }
                            else
                            {
                                STL_TRY_THROW( ( qlvIsSameExpression( sSearchedExpr, sTartget->mExpr )
                                                 == STL_TRUE ),
                                               RAMP_ERR_COLUMN_AMBIGUOUS );
                            }
                            continue;
                        }

                        if( sBaseTableNode->mRelationName->mSchema != NULL )
                        {
                            if( stlStrcmp( sBaseTableNode->mRelationName->mSchema,
                                           aColumnNameSource->mSchema ) == 0 )
                            {
                                STL_DASSERT( sRefCount == 0 );

                                sRefCount++;
                                sSearchedExpr = sTartget->mExpr;
                                continue;
                            }
                        }
                    }
                    else
                    {
                        if( ( aColumnNameSource->mSchema == NULL ) &&
                            ( aColumnNameSource->mTable == NULL ) )
                        {                        
                            if( stlStrcmp( sTartget->mAliasName, aColumnNameSource->mColumn ) == 0 )
                            {
                                if( sRefCount == 0 )
                                {
                                    sRefCount++;
                                    sSearchedExpr = sTartget->mExpr;
                                }
                                else
                                {
                                    STL_TRY_THROW( ( qlvIsSameExpression( sSearchedExpr, sTartget->mExpr )
                                                     == STL_TRUE ),
                                                   RAMP_ERR_COLUMN_AMBIGUOUS );
                                }
                                continue;
                            }
                        }
                    }
                } /* for */

                if( sRefCount == 1 )
                {
                    /* 찾았으면 Expression 복사 */
                    STL_TRY( qlvCopyExpr( sSearchedExpr,
                                          & sColumnExpr,
                                          aRegionMem,
                                          aEnv )
                             == STL_SUCCESS );

                    sMaySeq = NULL;
                    
                    *aIsUpdatable = STL_TRUE;    /* ignore */
                    
                    STL_THROW( RAMP_FINISH );
                }
                else
                {                
                    /* 못찾으면 하위 table로 이동 */
                    sTableItem = sTableItem->mNext;
                }
            } /* while */
        }

        
        /**
         * Name Validation
         */

        STL_TRY( qlvGetTableNodeWithColumn( aStmtInfo,
                                            aPhraseType,
                                            aRefTableList,
                                            aColumnNameSource,
                                            aIsSetOuterMark,
                                            &sColumnExpr,
                                            &sIsColumn,
                                            aIsUpdatable,
                                            aRelatedFuncId,
                                            aIterationTime,
                                            aRegionMem,
                                            aEnv )
                 == STL_SUCCESS );

        if( sIsColumn == STL_FALSE )
        {
            sColumnExpr = NULL;
            
            /**
             * Column이 아닌 경우
             * NEXTVAL 또는 SEQ.NEXTVAL일 수 있다.
             */
            sCanBeSeq = STL_TRUE;
        }
        else
        {
            sCanBeSeq = STL_FALSE;
        }
    }
    

    /**
     * Table 이 존재하지 않을 경우, Sequence 함수일 가능성을 살핀다.
     */
    
    if ( sCanBeSeq == STL_TRUE )
    {
        /**
         * TABLE_NAME.COLUMN_NAME 이 seq.NEXTVAL, seq.CURRVAL 일 수 있는지 확인
         * - Oracle 호환성을 위해 Name Space 를 Table 과 Sequence 를 같이 사용한다.
         * - 호환성을 위해 Dictionary 체계를 바꾸는 것보다 QP 단에서 검사한다.
         * - 따라서 seq.NEXTVAL 이 테이블 컬럼이 아닐 경우
         *   NEXTVAL(seq) 에 해당하는 Parse Tree 를 만든다.
         */

        if ( ( aColumnNameSource->mTable != NULL ) &&
             ( (stlStrcmp( aColumnNameSource->mColumn, "NEXTVAL" ) == 0) ||
               (stlStrcmp( aColumnNameSource->mColumn, "CURRVAL" ) == 0) ) )
        {
            /**
             * 코드 재사용을 위해 임의의 Pseudo Column Parse Tree 구조를 생성한다.
             * SEQ.NEXTVAL ==> NEXTVAL(SEQ)
             */

            /**
             * Sequence Name 구성
             */

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF(qlpObjectName),
                                        (void **) & sMayObjName,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            stlMemset( sMayObjName, 0x00, STL_SIZEOF(qlpObjectName) );

            sMayObjName->mType      = QLL_SEQUENCE_NAME_TYPE;
            sMayObjName->mNodePos   = aColumnNameSource->mNodePos;
            sMayObjName->mSchemaPos = aColumnNameSource->mSchemaPos;
            sMayObjName->mObjectPos = aColumnNameSource->mTablePos;
            sMayObjName->mSchema    = aColumnNameSource->mSchema;
            sMayObjName->mObject    = aColumnNameSource->mTable;

            /**
             * Argument List 구성
             */

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF(qlpListElement),
                                        (void **) & sMayElement,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            stlMemset( sMayElement, 0x00, STL_SIZEOF(qlpListElement) );

            sMayElement->mData.mPtrValue = (void *) sMayObjName;
            sMayElement->mPrev = NULL;
            sMayElement->mNext = NULL;

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF(qlpList),
                                        (void **) & sMayList,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            stlMemset( sMayList, 0x00, STL_SIZEOF(qlpList) );

            sMayList->mType = QLL_POINTER_LIST_TYPE;
            sMayList->mCount = 1;
            sMayList->mHead = sMayElement;
            sMayList->mTail = sMayElement;

            /**
             * Pseudo Function 구성
             */
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF(qlpPseudoCol),
                                        (void **) & sMaySeq,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            stlMemset( sMaySeq, 0x00, STL_SIZEOF(qlpPseudoCol) );

            sMaySeq->mType = QLL_PSEUDO_COLUMN_TYPE;
            sMaySeq->mPseudoId =
                (stlStrcmp( aColumnNameSource->mColumn, "NEXTVAL" ) == 0) ?
                KNL_PSEUDO_COL_NEXTVAL :
                KNL_PSEUDO_COL_CURRVAL;

            sMaySeq->mNodePos = aColumnNameSource->mColumnPos;
            sMaySeq->mNodeLen = stlStrlen( aColumnNameSource->mColumn );
            sMaySeq->mArgs    = sMayList;

            /**
             * Sequence 일 가능성이 있음.
             */

            sColumnExpr = NULL;
        }
        else
        {
            /**
             * Sequence 함수 NEXTVAL, CURRVAL 이 아님
             */

            sMaySeq = NULL;

            STL_THROW( RAMP_ERR_COLUMN_NOT_EXISTS );
        }
    }
    else
    {
        /**
         * Sequence 일 가능성이 없음 
         */

        sMaySeq = NULL;
    }

    STL_RAMP( RAMP_FINISH );
    

    /**
     * OUTPUT 설정
     */

    *aMaySequence  = sMaySeq;
    *aColumnExpr   = sColumnExpr;
    
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_AMBIGUOUS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_AMBIGUOUS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnNameSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sErrString );
    }
    
    STL_CATCH( RAMP_ERR_COLUMN_NOT_EXISTS )
    {
        sLen = ( ( aColumnNameSource->mColumnPos +
                   stlStrlen( aColumnNameSource->mColumn ) ) -
                 aColumnNameSource->mNodePos ) + 5;

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    sLen,
                                    (void **) & sErrString,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

/*        if( aColumnNameSource->mCatalog != NULL )
        {
            stlSnprintf( sErrString,
                         sLen,
                         "%s.%s.%s.%s",
                         aColumnNameSource->mCatalog,
                         aColumnNameSource->mSchema,
                         aColumnNameSource->mTable,
                         aColumnNameSource->mColumn );
        }
        else */
        if( aColumnNameSource->mSchema != NULL )
        {
            stlSnprintf( sErrString,
                         sLen,
                         "%s.%s.%s",
                         aColumnNameSource->mSchema,
                         aColumnNameSource->mTable,
                         aColumnNameSource->mColumn );
        }
        else if( aColumnNameSource->mTable != NULL )
        {
            stlSnprintf( sErrString,
                         sLen,
                         "%s.%s",
                         aColumnNameSource->mTable,
                         aColumnNameSource->mColumn );
        }
        else
        {
            stlSnprintf( sErrString,
                         sLen,
                         "%s",
                         aColumnNameSource->mColumn );
        }

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_IDENTIFIER,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnNameSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sErrString );
    }

    STL_CATCH( RAMP_ERR_PSEUDO_COLUMN_NOT_ALLOWED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PSEUDO_COLUMN_NOT_ALLOWED,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnNameSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Referenced Sequence validation
 * @param[in]      aStmtInfo            Stmt Information
 * @param[in]      aValidationObjList   Validation Object List
 * @param[in]      aSeqName             Sequence Name Source
 * @param[out]     aSeqExprCode         Sequence Expression Code
 * @param[in]      aRelatedFuncId       Related Function ID
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime       상위 노드의 Iteration Time
 * @param[in]      aRegionMem           Region Memory
 * @param[in]      aEnv                 Environment
 */
stlStatus qlvValidateSequenceRef( qlvStmtInfo         * aStmtInfo,
                                  ellObjectList       * aValidationObjList,
                                  qlpObjectName       * aSeqName,
                                  qlvInitExpression  ** aSeqExprCode,
                                  knlBuiltInFunc        aRelatedFuncId,
                                  dtlIterationTime      aIterationTimeScope,
                                  dtlIterationTime      aIterationTime,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv )
{
    stlBool             sObjectExist = STL_FALSE;
    ellDictHandle     * sSeqHandle = NULL;
    ellDictHandle       sSchemaHandle;
    
    qlvInitExpression  * sExprCode = NULL;

    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( ellDictHandle ),
                                (void **) & sSeqHandle,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    ellInitDictHandle( sSeqHandle );
    ellInitDictHandle( & sSchemaHandle );
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqName->mType == QLL_SEQUENCE_NAME_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqExprCode != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Iteration Time Validation
     */

    STL_TRY_THROW( aIterationTimeScope <= DTL_ITERATION_TIME_FOR_EACH_EXPR,
                   RAMP_ERR_INVALID_ITERATION_TIME_SCOPE );


    /**
     * Sequence Name Validation
     */

    if( aSeqName->mSchema == NULL )
    {
        /**
         * Schema 가 명시되지 않은 경우
         */

        STL_TRY( qlvGetSequenceDictHandleByAuthHandleNSeqName( aStmtInfo->mTransID,
                                                               aStmtInfo->mDBCStmt,
                                                               aStmtInfo->mSQLStmt,
                                                               aStmtInfo->mSQLString,
                                                               ellGetCurrentUserHandle( ELL_ENV(aEnv) ),
                                                               aSeqName->mObject,
                                                               aSeqName->mObjectPos,
                                                               sSeqHandle,
                                                               & sObjectExist,
                                                               aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SEQ_NOT_EXIST );
    }
    else
    {
        /**
         * Schema 가 명시된 경우
         */

        STL_TRY( ellGetSchemaDictHandle( aStmtInfo->mTransID,
                                         aStmtInfo->mSQLStmt->mViewSCN,
                                         aSeqName->mSchema,
                                         & sSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXIST );

        STL_TRY( qlvGetSequenceDictHandleBySchHandleNSeqName( aStmtInfo->mTransID,
                                                              aStmtInfo->mDBCStmt,
                                                              aStmtInfo->mSQLStmt,
                                                              aStmtInfo->mSQLString,
                                                              & sSchemaHandle,
                                                              aSeqName->mObject,
                                                              aSeqName->mObjectPos,
                                                              sSeqHandle,
                                                              & sObjectExist,
                                                              aEnv )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SEQ_NOT_EXIST );
    }
    
    /**
     * USAGE ON SEQUENCE 권한 검사
     */

    STL_TRY( qlaCheckSeqPriv( aStmtInfo->mTransID,
                              aStmtInfo->mDBCStmt,
                              aStmtInfo->mSQLStmt,
                              sSeqHandle,
                              aEnv )
             == STL_SUCCESS );
                               
    /**
     * Access Object List 에 등록
     * Execution 시점에 Sequence Handle 을 Validation 해야 함.
     */

    STL_TRY( ellAddNewObjectItemWithQuantifiedName( aValidationObjList,
                                                    sSeqHandle,
                                                    NULL,
                                                    aRegionMem,
                                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sExprCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sExprCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( 0, aEnv );
    
    sExprCode->mType                = QLV_EXPR_TYPE_PSEUDO_ARGUMENT;
    sExprCode->mPosition            = aSeqName->mNodePos;
    sExprCode->mRelatedFuncId       = aRelatedFuncId;
    sExprCode->mIncludeExprCnt      = 0;                                /* 연산 대상이 아님 */
    sExprCode->mIterationTime       = DTL_ITERATION_TIME_FOR_EACH_EXPR;
    sExprCode->mExpr.mPseudoArg     = sSeqHandle;
    sExprCode->mColumnName          = NULL;

    /**
     * 생성된 Code Expression을 Expression List에 추가하지 않는다.
     * Sequence Name 은 Expression 대상이 아님.
     */


    /**
     * OUTPUT 설정
     */

    *aSeqExprCode = sExprCode;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ITERATION_TIME_SCOPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ITERATION_TIME_SCOPE_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aSeqName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SEQ_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SEQUENCE_NOT_EXIST,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aSeqName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aSeqName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      aSeqName->mSchema );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief aggregatioin validation
 * @param[in]      aStmtInfo      Stmt Information
 * @param[in]      aPhraseType          Expr 이 사용된 Phrase 유형 
 * @param[in]      aValidationObjList   Validation Object List
 * @param[in]      aRefTableList        Reference Table List
 * @param[in]      aIsAtomic            Atomic Statement인지 여부
 * @param[in]      aAllowedAggrDepth     Allowed Aggregation Depth
 * @param[in]      aAggrSource          Aggregation Source
 * @param[out]     aExprCode            Aggregation Expression Code
 * @param[out]     aHasOuterJoinOperator Outer Join Operator를 가졌는지 여부
 * @param[in]      aRelatedFuncId       Related Function ID
 * @param[in]      aIterationTimeScope  Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime       상위 노드의 Iteration Time
 * @param[in]      aRegionMem           Region Memory
 * @param[in]      aEnv                 Environment
 */
stlStatus qlvValidateAggregation( qlvStmtInfo         * aStmtInfo,
                                  qlvExprPhraseType     aPhraseType,
                                  ellObjectList       * aValidationObjList,
                                  qlvRefTableList     * aRefTableList,
                                  stlBool               aIsAtomic,
                                  stlInt32              aAllowedAggrDepth,
                                  qlpAggregation      * aAggrSource,
                                  qlvInitExpression  ** aExprCode,
                                  stlBool             * aHasOuterJoinOperator,
                                  knlBuiltInFunc        aRelatedFuncId,
                                  dtlIterationTime      aIterationTimeScope,
                                  dtlIterationTime      aIterationTime,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv )
{
    qlvInitExpression   * sAggrCode         = NULL;
    qlvInitExpression   * sArgCode          = NULL;
    qlvInitExpression   * sFilter           = NULL;
    qlvInitAggregation  * sAggregation      = NULL;
    qlpListElement      * sListElem         = NULL;   
    qllNode             * sNode             = NULL;
    stlInt64              sIncludeExprCnt   = 0;
    stlInt32              sArgCount         = 0; 
    dtlIterationTime      sIterTime         = DTL_ITERATION_TIME_NONE;
    stlInt32              sAllowedArgCount  = 0;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrSource->mType == QLL_AGGREGATION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprCode != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Aggregation Validation
     */

    /**
     * Aggregation은 Target Clause, Having Clause, Order By Clause에만 올 수 있다.
     */

    STL_TRY_THROW( ( (aPhraseType == QLV_EXPR_PHRASE_TARGET) ||
                     (aPhraseType == QLV_EXPR_PHRASE_HAVING) ||
                     (aPhraseType == QLV_EXPR_PHRASE_ORDERBY) ),
                   RAMP_ERR_GROUP_FUNCTION_NOT_ALLOWED );


    /**
     * Aggregation의 Iteration Time Validation
     */

    STL_TRY_THROW( aIterationTimeScope <= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION,
                   RAMP_ERR_GROUP_FUNCTION_NOT_ALLOWED );


    /**
     * Aggregation의 Depth Validation
     */

    STL_TRY_THROW( aAllowedAggrDepth > 0,
                   RAMP_ERR_NESTED_GROUP_IS_NESTED_TOO_DEEPLY );


    /**
     * Aggregation ID 체크 및 Aggregation의 Allowed Argument Count 계산
     */

    if( (aAggrSource->mAggrId > KNL_BUILTIN_AGGREGATION_UNARY_MIN) &&
        (aAggrSource->mAggrId < KNL_BUILTIN_AGGREGATION_UNARY_MAX) )
    {
        sAllowedArgCount = KNL_UNARY_AGGREGATION_ARG_COUNT;
    }
    else
    {
        STL_ASSERT( 0 );
    }


    /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sAggrCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sAggrCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

    /**
     * Code Aggregation 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitAggregation ),
                                (void **) & sAggregation,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sAggregation,
               0x00,
               STL_SIZEOF( qlvInitAggregation ) );


    /**
     * Code Aggregation 설정
     */

    sAggregation->mAggrId = aAggrSource->mAggrId;


    /**
     * Arguments 설정
     */
    
    if( aAggrSource->mArgs != NULL )
    {
        /**
         * Argument Count Validation
         */

        /* coverage : aggregation 구현 확장시 필요할 수 있다. */
        STL_TRY_THROW( sAllowedArgCount == aAggrSource->mArgs->mCount,
                       RAMP_ERR_INVALID_NUMBER_OF_ARGUMENTS );


        /**
         * Argument List 공간 할당
         */
        
        sAggregation->mArgCount = aAggrSource->mArgs->mCount;

        STL_TRY( knlAllocRegionMem(
                     aRegionMem,
                     STL_SIZEOF( qlvInitExpression* ) * sAggregation->mArgCount,
                     (void **) & sAggregation->mArgs,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sAggregation->mArgs,
                   0x00,
                   STL_SIZEOF( qlvInitExpression* ) * sAggregation->mArgCount );


        /**
         * Argument List 구성
         */

        sIterTime = DTL_ITERATION_TIME_NONE;
    
        QLP_LIST_FOR_EACH( aAggrSource->mArgs, sListElem )
        {
            sNode = (qllNode*) QLP_LIST_GET_POINTER_VALUE( sListElem );

            STL_TRY( qlvValidateValueExpr(
                         aStmtInfo,
                         aPhraseType,
                         aValidationObjList,
                         aRefTableList,
                         aIsAtomic,
                         STL_FALSE,  /* Row Expr */
                         aAllowedAggrDepth - 1,
                         (qllNode *) sNode,
                         & sArgCode,
                         aHasOuterJoinOperator,
                         KNL_BUILTIN_FUNC_INVALID,
                         DTL_ITERATION_TIME_FOR_EACH_AGGREGATION,
                         DTL_ITERATION_TIME_FOR_EACH_AGGREGATION,
                         aRegionMem,
                         aEnv )
                     == STL_SUCCESS );

            sIterTime = ( sIterTime < sArgCode->mIterationTime )
                ? sIterTime : sArgCode->mIterationTime;

            sIncludeExprCnt += sArgCode->mIncludeExprCnt;
            sAggregation->mArgs[ sArgCount ] = sArgCode;

            sArgCount++;
        }
    }
    else
    {
        sAggregation->mArgCount = 0;
        sAggregation->mArgs     = NULL;
    }
    
    STL_DASSERT( sAggregation->mArgCount == sArgCount );

    
    /**
     * Distinct 여부 설정
     */

    if( aAggrSource->mSetQuantifier == QLP_SET_QUANTIFIER_ALL )
    {
        sAggregation->mIsDistinct = STL_FALSE;
    }
    else
    {
        STL_DASSERT( aAggrSource->mSetQuantifier == QLP_SET_QUANTIFIER_DISTINCT );

        switch( sAggregation->mAggrId )
        {
            case KNL_BUILTIN_AGGREGATION_SUM :
            case KNL_BUILTIN_AGGREGATION_COUNT :
                {
                    sAggregation->mIsDistinct = STL_TRUE;
                    break;
                }
            case KNL_BUILTIN_AGGREGATION_COUNT_ASTERISK :
            case KNL_BUILTIN_AGGREGATION_MIN :
            case KNL_BUILTIN_AGGREGATION_MAX :
                {
                    sAggregation->mIsDistinct = STL_FALSE;
                    break;
                }
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
    }


    /**
     * Nested Aggregation 여부 설정
     */

    sAggregation->mHasNestedAggr = ( sIterTime == DTL_ITERATION_TIME_FOR_EACH_AGGREGATION );
    

    /**
     * Filter 구성
     */

    if( aAggrSource->mFilter != NULL )
    {
        /**
         * @todo (LONG TODO) Aggregation Filter
         */
        
        STL_TRY( qlvValidateValueExpr( aStmtInfo,
                                       aPhraseType,
                                       aValidationObjList,
                                       aRefTableList,
                                       aIsAtomic,
                                       STL_FALSE,  /* Row Expr */
                                       aAllowedAggrDepth - 1,
                                       aAggrSource->mFilter,
                                       & sFilter,
                                       aHasOuterJoinOperator,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                       aRegionMem,
                                       aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sFilter = NULL;
    }

    sAggregation->mFilter = sFilter;


    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( sIncludeExprCnt + 1, aEnv );
    
    sAggrCode->mType                = QLV_EXPR_TYPE_AGGREGATION;
    sAggrCode->mPosition            = aAggrSource->mNodePos;
    sAggrCode->mIncludeExprCnt      = sIncludeExprCnt + 1;
    sAggrCode->mExpr.mAggregation   = sAggregation;
    sAggrCode->mRelatedFuncId       = aRelatedFuncId;
    sAggrCode->mIterationTime       = DTL_ITERATION_TIME_FOR_EACH_AGGREGATION;
    sAggrCode->mColumnName          = NULL;


    /**
     * OUTPUT 설정
     */

    *aExprCode = sAggrCode;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_GROUP_FUNCTION_NOT_ALLOWED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_GROUP_FUNCTION_NOT_ALLOWED,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aAggrSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    /* coverage : aggregation 구현 확장시 필요할 수 있다. */
    STL_CATCH( RAMP_ERR_INVALID_NUMBER_OF_ARGUMENTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_NUMBER_OF_ARGUMENTS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aAggrSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NESTED_GROUP_IS_NESTED_TOO_DEEPLY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_GROUP_FUNCTION_IS_NESTED_TOO_DEEPLY,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aAggrSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief case expression validation
 * @param[in]      aStmtInfo            Stmt Information
 * @param[in]      aPhraseType          Expr 이 사용된 Phrase 유형 
 * @param[in]      aValidationObjList   Validation Object List
 * @param[in]      aRefTableList        Reference Table List
 * @param[in]      aIsAtomic            Atomic Statement인지 여부
 * @param[in]      aAllowedAggrDepth    Allowed Aggregation Depth
 * @param[in]      aCaseSource          Case expr Source
 * @param[out]     aExprCode            Expression Code
 * @param[out]     aHasOuterJoinOperator Outer Join Operator를 가졌는지 여부
 * @param[in]      aRelatedFuncId       Related Function ID
 * @param[in]      aIterationTimeScope  Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime       상위 노드의 Iteration Time
 * @param[in]      aRegionMem           Region Memory
 * @param[in]      aEnv                 Environment
 */
stlStatus qlvValidateCaseExpr( qlvStmtInfo         * aStmtInfo,
                               qlvExprPhraseType     aPhraseType,
                               ellObjectList       * aValidationObjList,
                               qlvRefTableList     * aRefTableList,
                               stlBool               aIsAtomic,
                               stlInt32              aAllowedAggrDepth,
                               qlpCaseExpr         * aCaseSource,
                               qlvInitExpression  ** aExprCode,
                               stlBool             * aHasOuterJoinOperator,
                               knlBuiltInFunc        aRelatedFuncId,
                               dtlIterationTime      aIterationTimeScope,
                               dtlIterationTime      aIterationTime,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv )
{
    qlvInitExpression  * sCaseCode       = NULL;
    qlvInitExpression  * sCode           = NULL;
    qlvInitCaseExpr    * sCaseExpr       = NULL;
    qlpCaseWhenClause  * sWhenClause     = NULL;
    qlpListElement     * sListElem       = NULL;   
    stlInt64             sIncludeExprCnt = 0;
    stlInt32             sCount          = 0;
    dtlIterationTime     sIterTime       = DTL_ITERATION_TIME_INVALID;   
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCaseSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprCode != NULL, QLL_ERROR_STACK(aEnv) );

    /******************************************************
     * NULLIF, COALESCE, SIMPLE CASE, DECODE, CASE2, NVL, NVL2 함수는
     * 파싱단계에서 SEARCHED CASE로 변형해서 넘어온다.
     * 각 함수들의 argument count validate
     ******************************************************/

    if( (aCaseSource->mType == QLL_NULLIF_EXPR)   ||
        (aCaseSource->mType == QLL_COALESCE_EXPR) ||
        (aCaseSource->mType == QLL_NVL_EXPR)      ||
        (aCaseSource->mType == QLL_NVL2_EXPR)     ||
        (aCaseSource->mType == QLL_DECODE_EXPR)   ||
        (aCaseSource->mType == QLL_CASE2_EXPR) )
    {
        STL_TRY_THROW( aCaseSource->mExpr->mCount >=
                       gBuiltInFuncInfoArr[aCaseSource->mFuncId].mArgumentCntMin,
                       RAMP_ERR_INVALID_NUMBER_OF_ARGUMENTS );
        
        STL_TRY_THROW( aCaseSource->mExpr->mCount <=
                       gBuiltInFuncInfoArr[aCaseSource->mFuncId].mArgumentCntMax,
                       RAMP_ERR_MAXIMUM_ARGUMENTS_EXCEEDED );
    }
    
    /******************************************************
     * Case Expr Validation
     ******************************************************/

    /* syntax error에 의해 적어도 하나 이상의 when 구문을 갖는것을 보장한다. */
    STL_DASSERT( aCaseSource->mWhenClause->mCount >=
                 gBuiltInFuncInfoArr[KNL_BUILTIN_FUNC_CASE].mArgumentCntMin );
    
    STL_TRY_THROW( aCaseSource->mWhenClause->mCount <=
                   gBuiltInFuncInfoArr[KNL_BUILTIN_FUNC_CASE].mArgumentCntMax,
                   RAMP_ERR_MAXIMUM_ARGUMENTS_EXCEEDED );                
    
    /**
     * Code Expression 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sCaseCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sCaseCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

    /**
     * Code CaseExpr 생성
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitCaseExpr ),
                                (void **) & sCaseExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sCaseExpr,
               0x00,
               STL_SIZEOF( qlvInitCaseExpr ) );

    /**
     * Code CaseExpr 설정
     */

    sCaseExpr->mFuncId   = aCaseSource->mFuncId;
    

    /**
     * When ... Then  expr 생성
     */

    /**
     * When ... Then ...  List 공간 할당
     */

    sCaseExpr->mCount = aCaseSource->mWhenClause->mCount;

    STL_TRY( knlAllocRegionMem(
                 aRegionMem,
                 STL_SIZEOF( qlvInitExpression* ) * sCaseExpr->mCount,
                 (void **) & sCaseExpr->mWhenArr,
                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sCaseExpr->mWhenArr,
               0x00,
               STL_SIZEOF( qlvInitExpression* ) * sCaseExpr->mCount );

    STL_TRY( knlAllocRegionMem(
                 aRegionMem,
                 STL_SIZEOF( qlvInitExpression* ) * sCaseExpr->mCount,
                 (void **) & sCaseExpr->mThenArr,
                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sCaseExpr->mThenArr,
               0x00,
               STL_SIZEOF( qlvInitExpression* ) * sCaseExpr->mCount );

    /**
     * When ... Then ...  List 구성
     */
    
    sIterTime = gBuiltInFuncInfoArr[ sCaseExpr->mFuncId ].mIterationTime;

    QLP_LIST_FOR_EACH( aCaseSource->mWhenClause, sListElem )
    {
        sWhenClause = (qlpCaseWhenClause *) QLP_LIST_GET_POINTER_VALUE( sListElem );

        /*
         * WHEN expr
         */
        STL_TRY( qlvValidateValueExpr(
                     aStmtInfo,
                     aPhraseType,
                     aValidationObjList,
                     aRefTableList,
                     aIsAtomic,
                     STL_FALSE,  /* Row Expr */
                     aAllowedAggrDepth,
                     (qllNode *)(sWhenClause->mWhen),
                     & sCode,
                     aHasOuterJoinOperator,
                     sCaseExpr->mFuncId,
                     aIterationTimeScope,
                     gBuiltInFuncInfoArr[ sCaseExpr->mFuncId ].mIterationTime,
                     aRegionMem,
                     aEnv )
                 == STL_SUCCESS );

        sIterTime = ( sIterTime < sCode->mIterationTime )
            ? sIterTime : sCode->mIterationTime;
        
        sIncludeExprCnt += sCode->mIncludeExprCnt;
        sCaseExpr->mWhenArr[ sCount ] = sCode;

        /*
         * THEN expr
         */ 
        STL_TRY( qlvValidateValueExpr(
                     aStmtInfo,
                     aPhraseType,
                     aValidationObjList,
                     aRefTableList,
                     aIsAtomic,
                     STL_FALSE,  /* Row Expr */
                     aAllowedAggrDepth,
                     (qllNode *)(sWhenClause->mThen),
                     & sCode,
                     aHasOuterJoinOperator,
                     sCaseExpr->mFuncId,
                     aIterationTimeScope,
                     gBuiltInFuncInfoArr[ sCaseExpr->mFuncId ].mIterationTime,
                     aRegionMem,
                     aEnv )
                 == STL_SUCCESS );

        sIterTime = ( sIterTime < sCode->mIterationTime )
            ? sIterTime : sCode->mIterationTime;
        
        sIncludeExprCnt += sCode->mIncludeExprCnt;
        sCaseExpr->mThenArr[ sCount ] = sCode;
        
        sCount++;
    }

    STL_DASSERT( sCaseExpr->mCount == sCount );    

    STL_TRY( qlvValidateValueExpr(
                 aStmtInfo,
                 aPhraseType,
                 aValidationObjList,
                 aRefTableList,
                 aIsAtomic,
                 STL_FALSE,  /* Row Expr */
                 aAllowedAggrDepth,
                 (qllNode *)(aCaseSource->mDefResult),
                 & sCode,
                 aHasOuterJoinOperator,
                 sCaseExpr->mFuncId,
                 aIterationTimeScope,
                 gBuiltInFuncInfoArr[ sCaseExpr->mFuncId ].mIterationTime,
                 aRegionMem,
                 aEnv )
             == STL_SUCCESS );
    
    sIterTime = ( sIterTime < sCode->mIterationTime )
        ? sIterTime : sCode->mIterationTime;
    
    sIncludeExprCnt += sCode->mIncludeExprCnt;
    sCaseExpr->mDefResult = sCode;

    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( sIncludeExprCnt + 1, aEnv );
    
    sCaseCode->mType           = QLV_EXPR_TYPE_CASE_EXPR;
    sCaseCode->mPosition       = aCaseSource->mNodePos;
    sCaseCode->mIncludeExprCnt = sIncludeExprCnt + 1;
    sCaseCode->mExpr.mCaseExpr = sCaseExpr;
    sCaseCode->mRelatedFuncId  = aRelatedFuncId;
    sCaseCode->mIterationTime  = sIterTime;
    sCaseCode->mColumnName     = NULL;

    /**
     * OUTPUT 설정
     */

    *aExprCode = sCaseCode;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_NUMBER_OF_ARGUMENTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_NUMBER_OF_ARGUMENTS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aCaseSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_MAXIMUM_ARGUMENTS_EXCEEDED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MAXIMUM_NUMBER_OF_ARGUMENTS_EXCEEDED,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aCaseSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }   


    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief List Function validation
 * @param[in]      aStmtInfo      Stmt Information
 * @param[in]      aPhraseType          Expr 이 사용된 Phrase 유형
 * @param[in]      aValidationObjList   Validation Object List
 * @param[in]      aRefTableList        Reference Table List
 * @param[in]      aIsAtomic            Atomic Statement인지 여부
 * @param[in]      aAllowedAggrDepth    Allowed Aggregation Depth
 * @param[in]      aListSource          List Function Source
 * @param[out]     aExprCode            List Function Expression Code
 * @param[out]     aHasOuterJoinOperator Outer Join Operator를 가졌는지 여부
 * @param[in]      aRelatedFuncId       Related Function ID
 * @param[in]      aIterationTimeScope  Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime       상위 노드의 Iteration Time
 * @param[in]      aRegionMem           Region Memory
 * @param[in]      aEnv                 Environment
 */
stlStatus qlvValidateListFunction( qlvStmtInfo         * aStmtInfo,
                                   qlvExprPhraseType     aPhraseType,
                                   ellObjectList       * aValidationObjList,
                                   qlvRefTableList     * aRefTableList,
                                   stlBool               aIsAtomic,
                                   stlInt32              aAllowedAggrDepth,
                                   qlpListFunc         * aListSource,
                                   qlvInitExpression  ** aExprCode,
                                   stlBool             * aHasOuterJoinOperator,
                                   knlBuiltInFunc        aRelatedFuncId,
                                   dtlIterationTime      aIterationTimeScope,
                                   dtlIterationTime      aIterationTime,
                                   knlRegionMem        * aRegionMem,
                                   qllEnv              * aEnv )
{
    qlvInitExpression   * sFuncCode       = NULL;
    qlvInitExpression   * sArgCode        = NULL;
    qlvInitListFunc     * sListFunc       = NULL;
    qlvInitFunction     * sFunction       = NULL;
    qllNode             * sNode           = NULL;
    stlInt64              sIncludeExprCnt = 0;
    stlInt32              sArgCount       = 0;
    dtlIterationTime      sIterTime       = DTL_ITERATION_TIME_INVALID;

    qlpListElement      * sListElem       = NULL;

    qlvInitListCol      * sListCol        = NULL;
    stlUInt32             sRowCount       = 0;
    stlUInt32             sRowArgCount    = 0;

    stlBool               sHasOuterJoinOperator = STL_FALSE;
    stlBool               sIsFirstListCol = STL_TRUE;
    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aListSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aListSource->mType == QLL_LIST_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Code List Function 생성
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitListFunc ),
                                (void **) & sListFunc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sListFunc,
               0x00,
               STL_SIZEOF( qlvInitListFunc ) );


    /**
     * Code List Function argument  설정
     */
    if( ( aListSource->mFuncId >= KNL_BUILTIN_FUNC_IS_EQUAL ) &&
        ( aListSource->mFuncId <= KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL ) )
    {
        switch( aListSource->mFuncId )
        {
            case KNL_BUILTIN_FUNC_IS_EQUAL :
                aListSource->mFuncId = KNL_BUILTIN_FUNC_IS_EQUAL_ROW;
                break;
            case KNL_BUILTIN_FUNC_IS_NOT_EQUAL :
                aListSource->mFuncId = KNL_BUILTIN_FUNC_IS_NOT_EQUAL_ROW;
                break;
            case KNL_BUILTIN_FUNC_IS_LESS_THAN :
                aListSource->mFuncId = KNL_BUILTIN_FUNC_IS_LESS_THAN_ROW;
                break;
            case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :
                aListSource->mFuncId = KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL_ROW;
                break;
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN :
                aListSource->mFuncId = KNL_BUILTIN_FUNC_IS_GREATER_THAN_ROW;
                break;
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :
                aListSource->mFuncId = KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL_ROW;
                break;
     
            default:
                STL_DASSERT( 0 );
                break;
        }
    }
    
    STL_DASSERT( ( KNL_LIST_FUNC_START_ID <= aListSource->mFuncId ) &&
                 ( aListSource->mFuncId <= KNL_LIST_FUNC_END_ID ) );
    
    sListFunc->mFuncId = aListSource->mFuncId;

    if( aListSource->mArgs != NULL )
    {
        /**
         * argument list 공간 할당
         */
        sListFunc->mArgCount = aListSource->mArgs->mCount;

        STL_TRY( knlAllocRegionMem(
                     aRegionMem,
                     STL_SIZEOF( qlvInitExpression* ) * sListFunc->mArgCount,
                     (void **) & sListFunc->mArgs,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( sListFunc->mArgs,
                   0x00,
                   STL_SIZEOF( qlvInitExpression* ) * sListFunc->mArgCount );

        /**
         * Argument List 구성
         */

        sIterTime = gBuiltInFuncInfoArr[ sListFunc->mFuncId ].mIterationTime;
        
        QLP_LIST_FOR_EACH( aListSource->mArgs, sListElem )
        {
            sNode = (qllNode*) QLP_LIST_GET_POINTER_VALUE( sListElem );

            /**
             *  List Function은 List Column만 올 수 있다.
             */
            STL_DASSERT( sNode->mType == QLL_LIST_COLUMN_TYPE );

            if( ((qlpListCol*)sNode)->mPredicate == DTL_LIST_PREDICATE_RIGHT )
            {
                sRowCount = ((qlpListCol*)sNode)->mArgCount;
            }
            else if( ((qlpListCol*)sNode)->mPredicate == DTL_LIST_PREDICATE_LEFT )
            {
                ((qlpListCol*)sNode)->mArgCount = sRowCount;
            }
            else
            {
                STL_ASSERT( 0 );
            }

            sHasOuterJoinOperator = STL_FALSE;
            STL_TRY( qlvValidateValueExpr(
                         aStmtInfo,
                         aPhraseType,
                         aValidationObjList,
                         aRefTableList,
                         aIsAtomic,
                         STL_FALSE,  /* Row Expr */
                         aAllowedAggrDepth,
                         (qllNode *) sNode,
                         & sArgCode,
                         &sHasOuterJoinOperator,
                         sListFunc->mFuncId,
                         aIterationTimeScope,
                         gBuiltInFuncInfoArr[ sListFunc->mFuncId ].mIterationTime,
                         aRegionMem,
                         aEnv )
                     == STL_SUCCESS );

            /**
             * Outer Join Operator Check
             * => Quantified Comparison에는 Outer Join Operator를 기술할 수 없다.
             */

            STL_TRY_THROW( sHasOuterJoinOperator == STL_FALSE,
                           RAMP_ERR_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN_QUANTIFIED_COMPARISON );

            sIterTime = ( sIterTime < sArgCode->mIterationTime )
                ? sIterTime : sArgCode->mIterationTime;

            sIncludeExprCnt += sArgCode->mIncludeExprCnt;

            /**
             * 현재는 LIST COLUMN만 올 수 있다. 나중에 확장
             * if( sArgCode->mType == QLV_EXPR_TYPE_LIST_COLUMN )
             */
            
            sListCol     = sArgCode->mExpr.mListCol;            
            STL_DASSERT( sListCol->mArgCount == sRowCount );

            /**
             * @todo List Column 과 Row Expr의 관계.
             * List Column안의 RowExpr의 argument 개수는 모두 동일해야 한다.
             * 현재는 List Column은 반드시 Row Expr를 갖고 있다. 
             */

            if( sListCol->mArgs[ 0 ]->mExpr.mRowExpr->mIsRowSubQuery == STL_TRUE )
            {
                STL_TRY_THROW( ( sRowArgCount ==
                                 sListCol->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ]->mExpr.mSubQuery->mTargetCount ) ||
                               ( sRowArgCount == 0 ),
                               RAMP_INVALID_NUMBER_OF_ARGUMENTS );
                sRowArgCount = sListCol->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ]->mExpr.mSubQuery->mTargetCount;

                if( ( sIsFirstListCol == STL_FALSE ) &&
                    ( sListCol->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ]->mExpr.mSubQuery->mCategory ==
                      QLV_SUB_QUERY_TYPE_RELATION ) )
                {
                    if( sRowArgCount == 1 )
                    {
                        sListCol->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ]->mExpr.mSubQuery->mCategory =
                            QLV_SUB_QUERY_TYPE_SCALAR;
                    }
                    else
                    {
                        sListCol->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ]->mExpr.mSubQuery->mCategory =
                            QLV_SUB_QUERY_TYPE_ROW;
                    }
                }
            }
            else
            {
                STL_TRY_THROW( ( sRowArgCount == sListCol->mArgs[ 0 ]->mExpr.mRowExpr->mArgCount ) ||
                               ( sRowArgCount == 0 ),
                               RAMP_INVALID_NUMBER_OF_ARGUMENTS );
                sRowArgCount = sListCol->mArgs[ 0 ]->mExpr.mRowExpr->mArgCount;
            }


            sListFunc->mArgs[ sArgCount ] = sArgCode;

            sIsFirstListCol = STL_FALSE;
            sArgCount++;
        }

        STL_DASSERT( sListFunc->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_LIST_COLUMN );
    }
    else
    {
        /* coverage : parser에 의해 이미 평가되었다. */
        STL_DASSERT( 0 );
    }


    if( ( sRowArgCount == 1 ) &&
        ( sListFunc->mArgCount == 2 ) &&
        ( sListFunc->mArgs[ 0 ]->mExpr.mListCol->mArgCount == 1 ) &&
        ( sListFunc->mArgs[ 0 ]->mExpr.mListCol->mArgs[ 0 ]->mExpr.mRowExpr->mIsRowSubQuery == STL_FALSE ) )
    {
        /**
         * Code List Function 생성
         */
    
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvInitFunction ),
                                    (void **) & sFunction,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sFunction,
                   0x00,
                   STL_SIZEOF( qlvInitFunction ) );

        STL_TRY( knlAllocRegionMem(
                     aRegionMem,
                     STL_SIZEOF( qlvInitExpression* ) * 2,
                     (void **) & sFunction->mArgs,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sFunction->mArgs,
                   0x00,
                   STL_SIZEOF( qlvInitExpression* ) * sFunction->mArgCount );

        /* Function ID 설정 */
        switch( sListFunc->mFuncId )
        {
            case KNL_BUILTIN_FUNC_IS_EQUAL_ROW :
            case KNL_BUILTIN_FUNC_IN :
            case KNL_BUILTIN_FUNC_EQUAL_ANY :
            case KNL_BUILTIN_FUNC_EQUAL_ALL :
                {
                    sFunction->mFuncId = KNL_BUILTIN_FUNC_IS_EQUAL;
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_NOT_EQUAL_ROW :
            case KNL_BUILTIN_FUNC_NOT_IN :
            case KNL_BUILTIN_FUNC_NOT_EQUAL_ANY :
            case KNL_BUILTIN_FUNC_NOT_EQUAL_ALL :
                {
                    sFunction->mFuncId = KNL_BUILTIN_FUNC_IS_NOT_EQUAL;
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_LESS_THAN_ROW :
            case KNL_BUILTIN_FUNC_LESS_THAN_ANY :
            case KNL_BUILTIN_FUNC_LESS_THAN_ALL :
                {
                    sFunction->mFuncId = KNL_BUILTIN_FUNC_IS_LESS_THAN;
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL_ROW :
            case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY :
            case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL :
                {
                    sFunction->mFuncId = KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL;
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN_ROW :
            case KNL_BUILTIN_FUNC_GREATER_THAN_ANY :
            case KNL_BUILTIN_FUNC_GREATER_THAN_ALL :
                {
                    sFunction->mFuncId = KNL_BUILTIN_FUNC_IS_GREATER_THAN;
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL_ROW :
            case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY :
            case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL :
                {
                    sFunction->mFuncId = KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL;
                    break;
                }
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }

        sFunction->mArgCount = 2;

        sFunction->mArgs[ 0 ] = sListFunc->mArgs[ 1 ]->mExpr.mListCol->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ];
        sFunction->mArgs[ 1 ] = sListFunc->mArgs[ 0 ]->mExpr.mListCol->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ];

        sIncludeExprCnt = ( sFunction->mArgs[ 0 ]->mIncludeExprCnt +
                            sFunction->mArgs[ 1 ]->mIncludeExprCnt );
        
        
        /**
         * Code Expression 공간 할당
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvInitExpression ),
                                    (void **) & sFuncCode,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sFuncCode,
                   0x00,
                   STL_SIZEOF( qlvInitExpression ) );


        /**
         * Code Expression 설정
         */

        QLV_VALIDATE_INCLUDE_EXPR_CNT( sIncludeExprCnt + 1, aEnv );
        
        sFuncCode->mType           = QLV_EXPR_TYPE_FUNCTION;
        sFuncCode->mPosition       = aListSource->mNodePos;
        sFuncCode->mIncludeExprCnt = sIncludeExprCnt + 1;
        sFuncCode->mExpr.mFunction = sFunction;
        sFuncCode->mRelatedFuncId  = aRelatedFuncId;
        sFuncCode->mIterationTime  = sIterTime;
        sFuncCode->mColumnName     = NULL;
    }
    else
    {
        /**
         * Code Expression 공간 할당
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvInitExpression ),
                                    (void **) & sFuncCode,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sFuncCode,
                   0x00,
                   STL_SIZEOF( qlvInitExpression ) );


        /**
         * Code Expression 설정
         */

        QLV_VALIDATE_INCLUDE_EXPR_CNT( sIncludeExprCnt + 1, aEnv );
        
        sFuncCode->mType           = QLV_EXPR_TYPE_LIST_FUNCTION;
        sFuncCode->mPosition       = aListSource->mNodePos;
        sFuncCode->mIncludeExprCnt = sIncludeExprCnt + 1;
        sFuncCode->mExpr.mListFunc = sListFunc;
        sFuncCode->mRelatedFuncId  = aRelatedFuncId;
        sFuncCode->mIterationTime  = sIterTime;
        sFuncCode->mColumnName     = NULL;
    }
    

    /**
     * OUTPUT 설정
     */

    *aExprCode = sFuncCode;

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_INVALID_NUMBER_OF_ARGUMENTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_NUMBER_OF_ARGUMENTS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aListSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_CATCH( RAMP_ERR_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN_QUANTIFIED_COMPARISON )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aListSource->mOperatorPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "operand of OR or the quantified comparison" );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief List Column validation
 * @param[in]      aStmtInfo      Stmt Information
 * @param[in]      aPhraseType          Expr 이 사용된 Phrase 유형
 * @param[in]      aValidationObjList   Validation Object List
 * @param[in]      aRefTableList        Reference Table List
 * @param[in]      aIsAtomic            Atomic Statement인지 여부
 * @param[in]      aAllowedAggrDepth    Allowed Aggregation Depth
 * @param[in]      aListSource          List Function Source
 * @param[out]     aExprCode            List Function Expression Code
 * @param[out]     aHasOuterJoinOperator Outer Join Operator를 가졌는지 여부
 * @param[in]      aRelatedFuncId       Related Function ID
 * @param[in]      aIterationTimeScope  Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime       상위 노드의 Iteration Time
 * @param[in]      aRegionMem           Region Memory
 * @param[in]      aEnv                 Environment
 */
stlStatus qlvValidateListColumn( qlvStmtInfo         * aStmtInfo,
                                 qlvExprPhraseType     aPhraseType,
                                 ellObjectList       * aValidationObjList,
                                 qlvRefTableList     * aRefTableList,
                                 stlBool               aIsAtomic,
                                 stlInt32              aAllowedAggrDepth,
                                 qlpListCol          * aListSource,
                                 qlvInitExpression  ** aExprCode,
                                 stlBool             * aHasOuterJoinOperator,
                                 knlBuiltInFunc        aRelatedFuncId,
                                 dtlIterationTime      aIterationTimeScope,
                                 dtlIterationTime      aIterationTime,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv )
{
    qlvInitExpression   * sExprCode       = NULL;
    qlvInitExpression   * sArgCode        = NULL;
    qlvInitListCol      * sListCol        = NULL;
    qllNode             * sNode           = NULL;
    stlInt64              sIncludeExprCnt = 0;
    stlInt32              sArgCount       = 0;
    stlInt32              sRowArgCount    = 0;
    dtlIterationTime      sIterTime       = DTL_ITERATION_TIME_INVALID;

    qlpListElement      * sListElem       = NULL;
    qlvInitRowExpr      * sNewRowExpr     = NULL;
    qlvInitRowExpr      * sOldRowExpr     = NULL;
    stlInt32              sLoop           = 0;
    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aListSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aListSource->mType == QLL_LIST_COLUMN_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprCode != NULL, QLL_ERROR_STACK(aEnv) );

     /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sExprCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sExprCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

     /**
     * Code ListCol 생성
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitListCol ),
                                (void **) & sListCol,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sListCol,
               0x00,
               STL_SIZEOF( qlvInitListCol ) );

    /**
     * Code ListCol 구성 설정 
     */

    sListCol->mPredicate = aListSource->mPredicate;

    if( aListSource->mArgs != NULL )
    {
         STL_TRY_THROW( aListSource->mArgCount <= QLL_MAX_LIST_COLUMN_ARGS,
                       RAMP_MAXIMUM_NUMBER_OF_ARGUMENTS_EXCEEDED );
        /**
         * List Column Argument List 공간 할당
         */
        sListCol->mArgCount = aListSource->mArgCount;

        STL_TRY( knlAllocRegionMem(
                     aRegionMem,
                     STL_SIZEOF( qlvInitExpression* ) * sListCol->mArgCount,
                     (void **) & sListCol->mArgs,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( sListCol->mArgs,
                   0x00,
                   STL_SIZEOF( qlvInitExpression* ) * sListCol->mArgCount );

        /**
         * DataType Item
         */
        
        STL_TRY( knlAllocRegionMem(
                     aRegionMem,
                     STL_SIZEOF( knlListEntry* ) * sListCol->mArgCount,
                     (void **) & sListCol->mEntry,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( sListCol->mEntry,
                   0x00,
                   STL_SIZEOF( knlListEntry* ) * sListCol->mArgCount );
        
        /**
         * Argument List 구성
         */
        
        sIterTime  = aIterationTime;

        aRelatedFuncId = ( ( aListSource->mArgCount == 1 ) && ( sArgCount == 0 )
                           ? aListSource->mFuncId
                           : KNL_BUILTIN_FUNC_INVALID );
        
        QLP_LIST_FOR_EACH( aListSource->mArgs, sListElem )
        {
            sNode = (qllNode*) QLP_LIST_GET_POINTER_VALUE( sListElem );

            /**
             *  List Column은 Row Expr 단위로 생성된다.
             */
            STL_DASSERT( sNode->mType == QLL_ROW_EXPR_TYPE );

            /**
             * List Column안의 RowExpr의 argument 개수는 모두 동일해야 한다.
             */
            
            STL_TRY( qlvValidateValueExpr(
                         aStmtInfo,
                         aPhraseType,
                         aValidationObjList,
                         aRefTableList,
                         aIsAtomic,
                         STL_FALSE,  /* Row Expr */
                         aAllowedAggrDepth,
                         (qllNode *) sNode,
                         & sArgCode,
                         aHasOuterJoinOperator,
                         aRelatedFuncId, /* Rigth Row Expression만 relation subquery 가 될 수 있다. */
                         aIterationTimeScope,
                         gBuiltInFuncInfoArr[ aListSource->mFuncId ].mIterationTime,
                         aRegionMem,
                         aEnv )
                     == STL_SUCCESS );

            STL_DASSERT( sArgCode->mType == QLV_EXPR_TYPE_ROW_EXPR );

            if( ( sArgCode->mExpr.mRowExpr->mArgCount == 1 ) &&
                ( sArgCode->mExpr.mRowExpr->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_SUB_QUERY ) )
            {
                STL_TRY_THROW( ( sRowArgCount ==
                                   sArgCode->mExpr.mRowExpr->mArgs[ 0 ]->mExpr.mSubQuery->mTargetCount ) ||
                               ( sRowArgCount == 0 ),
                               RAMP_INVALID_NUMBER_OF_ARGUMENTS );

                sRowArgCount = sArgCode->mExpr.mRowExpr->mArgs[ 0 ]->mExpr.mSubQuery->mTargetCount;
            }
            else
            {
                STL_TRY_THROW( ( sRowArgCount == sArgCode->mExpr.mRowExpr->mArgCount ) ||
                               ( sRowArgCount == 0 ),
                               RAMP_INVALID_NUMBER_OF_ARGUMENTS );
                
                sRowArgCount = sArgCode->mExpr.mRowExpr->mArgCount;
            }
            
            sIterTime = ( sIterTime < sArgCode->mIterationTime )
                ? sIterTime : sArgCode->mIterationTime;

            sIncludeExprCnt += sArgCode->mIncludeExprCnt;
            
            sListCol->mArgs[ sArgCount ] = sArgCode;
            sArgCount++;
        }

        /**
         * Left column 의 경우 Right column의 row 수 만큼
         * row를 생성해 주어야 한다.
         */
        while( sArgCount < sListCol->mArgCount )
        {
            /**
             * 현재 Expression 복사
             */

            STL_TRY( qlvCopyExpr( sArgCode,
                                  & sListCol->mArgs[ sArgCount ],
                                  aRegionMem,
                                  aEnv )
                     == STL_SUCCESS );

            sNewRowExpr = sListCol->mArgs[ sArgCount ]->mExpr.mRowExpr;
            sOldRowExpr = sArgCode->mExpr.mRowExpr;

            STL_DASSERT( sOldRowExpr->mArgCount > 0 );


            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( qlvInitExpression* ) * sOldRowExpr->mArgCount,
                                        (void **) &( sNewRowExpr->mArgs ),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
                                        
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( knlListEntry ) * sOldRowExpr->mArgCount,
                                        (void *) &( sNewRowExpr->mEntry ),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
                
            for( sLoop = 0 ; sLoop < sOldRowExpr->mArgCount ; sLoop++ )
            {
                STL_TRY( qlvCopyExpr( sOldRowExpr->mArgs[ sLoop ],
                                      &( sNewRowExpr->mArgs[ sLoop ] ),
                                      aRegionMem,
                                      aEnv )
                         == STL_SUCCESS );
            }

            sIterTime = ( sIterTime < sArgCode->mIterationTime )
                ? sIterTime : sArgCode->mIterationTime;

            sIncludeExprCnt += sArgCode->mIncludeExprCnt;
            sArgCount++;
        }
    }
    else
    {
        /* coverage : parser에 의해 이미 평가되었다. */
        STL_DASSERT( 0 );
    }
    
    
    STL_DASSERT( sListCol->mArgCount == sArgCount );

    QLV_VALIDATE_INCLUDE_EXPR_CNT( sIncludeExprCnt + 1, aEnv );
    
    sExprCode->mType           = QLV_EXPR_TYPE_LIST_COLUMN;
    sExprCode->mPosition       = aListSource->mNodePos;
    sExprCode->mIncludeExprCnt = sIncludeExprCnt + 1;
    sExprCode->mExpr.mListCol  = sListCol;
    sExprCode->mRelatedFuncId  = aRelatedFuncId;
    sExprCode->mIterationTime  = sIterTime;
    sExprCode->mColumnName     = NULL;
    
    /**
     * OUTPUT 설정
     */

    *aExprCode = sExprCode;

    return STL_SUCCESS;

    STL_CATCH( RAMP_MAXIMUM_NUMBER_OF_ARGUMENTS_EXCEEDED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MAXIMUM_NUMBER_OF_ARGUMENTS_EXCEEDED,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aListSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_INVALID_NUMBER_OF_ARGUMENTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_NUMBER_OF_ARGUMENTS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sArgCode->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
     
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Row Expr validation
 * @param[in]      aStmtInfo      Stmt Information
 * @param[in]      aPhraseType          Expr 이 사용된 Phrase 유형
 * @param[in]      aValidationObjList   Validation Object List
 * @param[in]      aRefTableList        Reference Table List
 * @param[in]      aIsAtomic            Atomic Statement인지 여부
 * @param[in]      aAllowedAggrDepth    Allowed Aggregation Depth
 * @param[in]      aRowSource           Row Expr Source
 * @param[out]     aExprCode            Row Expr Expression Code
 * @param[out]     aHasOuterJoinOperator Outer Join Operator를 가졌는지 여부
 * @param[in]      aRelatedFuncId       Related Function ID
 * @param[in]      aIterationTimeScope  Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime       상위 노드의 Iteration Time
 * @param[in]      aRegionMem           Region Memory
 * @param[in]      aEnv                 Environment
 */
stlStatus qlvValidateRowExpr( qlvStmtInfo         * aStmtInfo,
                              qlvExprPhraseType     aPhraseType,
                              ellObjectList       * aValidationObjList,
                              qlvRefTableList     * aRefTableList,
                              stlBool               aIsAtomic,
                              stlInt32              aAllowedAggrDepth,
                              qlpRowExpr          * aRowSource,
                              qlvInitExpression  ** aExprCode,
                              stlBool             * aHasOuterJoinOperator,
                              knlBuiltInFunc        aRelatedFuncId,
                              dtlIterationTime      aIterationTimeScope,
                              dtlIterationTime      aIterationTime,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv )
{
    qlvInitExpression   * sExprCode       = NULL;
    qlvInitExpression   * sArgCode        = NULL;
    qlvInitRowExpr      * sRowExpr        = NULL;
    qllNode             * sNode           = NULL;
    stlInt64              sIncludeExprCnt = 0;
    stlInt32              sArgCount       = 0;
    stlBool               sHasSingleExpr  = STL_TRUE;
    dtlIterationTime      sIterTime       = DTL_ITERATION_TIME_INVALID;

    qlpListElement      * sListElem       = NULL;
        

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowSource->mType == QLL_ROW_EXPR_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sExprCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sExprCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

     /**
     * Code RowExpr 생성
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitRowExpr ),
                                (void **) & sRowExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sRowExpr,
               0x00,
               STL_SIZEOF( qlvInitRowExpr ) );

    /**
     * Code Row Expr argument  설정
     */
    
    if( aRowSource->mArgs != NULL )
    {
        /**
         * Value list 공간 할당
         */
        
        sIterTime = aIterationTime;

        QLP_LIST_FOR_EACH( aRowSource->mArgs, sListElem )
        {
            sArgCount++;
        }

        STL_TRY_THROW( sArgCount <= QLL_MAX_ROW_EXPR_ARGS,
                       RAMP_MAXIMUM_NUMBER_OF_ARGUMENTS_EXCEEDED );

        sHasSingleExpr = ( sArgCount == 1 );

        if( sHasSingleExpr == STL_FALSE )
        {
            sRowExpr->mArgCount = aRowSource->mArgs->mCount;
                
            STL_TRY( knlAllocRegionMem(
                         aRegionMem,
                         STL_SIZEOF( qlvInitExpression* ) * sRowExpr->mArgCount,
                         (void **) & sRowExpr->mArgs,
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        
            stlMemset( sRowExpr->mArgs,
                       0x00,
                       STL_SIZEOF( qlvInitExpression* ) * sRowExpr->mArgCount );

        
            STL_TRY( knlAllocRegionMem(
                         aRegionMem,
                         STL_SIZEOF( knlListEntry ) * sRowExpr->mArgCount,
                         (void **) & sRowExpr->mEntry,
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        
            stlMemset( sRowExpr->mEntry,
                       0x00,
                       STL_SIZEOF( knlListEntry ) * sRowExpr->mArgCount );

            aRelatedFuncId = KNL_BUILTIN_FUNC_INVALID;
        }
        else
        {
            /* Do Nothing */
        }


        sArgCount = 0;
        QLP_LIST_FOR_EACH( aRowSource->mArgs, sListElem )
        {
            sNode = (qllNode*) QLP_LIST_GET_POINTER_VALUE( sListElem );


            /**
             * Subquery 가 단독으로 오는 경우 Related Function Id를 Push한다.
             */
            
            STL_TRY( qlvValidateValueExpr(
                         aStmtInfo,
                         aPhraseType,
                         aValidationObjList,
                         aRefTableList,
                         aIsAtomic,
                         STL_TRUE,  /* Row Expr */
                         aAllowedAggrDepth,
                         (qllNode *) sNode,
                         & sArgCode,
                         aHasOuterJoinOperator,
                         aRelatedFuncId,
                         aIterationTimeScope,
                         sIterTime,
                         aRegionMem,
                         aEnv )
                     == STL_SUCCESS );

            sIterTime = ( sIterTime < sArgCode->mIterationTime )
                ? sIterTime : sArgCode->mIterationTime;

            if( sArgCode->mType == QLV_EXPR_TYPE_SUB_QUERY )
            {
                if( sHasSingleExpr == STL_TRUE )
                {
                    sIncludeExprCnt = 1;
                    sRowExpr->mArgCount = 1;
                    sArgCount = 1;

                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                STL_SIZEOF( qlvInitExpression* ),
                                                (void **) & sRowExpr->mArgs,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
        
                    stlMemset( sRowExpr->mArgs,
                               0x00,
                               STL_SIZEOF( qlvInitExpression* ) );

                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                STL_SIZEOF( knlListEntry ),
                                                (void **) & sRowExpr->mEntry,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
        
                    stlMemset( sRowExpr->mEntry,
                               0x00,
                               STL_SIZEOF( knlListEntry ) );

                    sRowExpr->mIsRowSubQuery =
                        ( sArgCode->mExpr.mSubQuery->mCategory != QLV_SUB_QUERY_TYPE_SCALAR );
                    sRowExpr->mSubQueryExpr = NULL;

                    sRowExpr->mArgs[ 0 ] = sArgCode;
                }
                else
                {
                    STL_TRY_THROW( sArgCode->mExpr.mSubQuery->mTargetCount == 1,
                                   RAMP_INVALID_NUMBER_OF_ARGUMENTS );

                    sIncludeExprCnt += 1;
                    sRowExpr->mArgs[ sArgCount ] = sArgCode;
                    sArgCount++;

                    sRowExpr->mIsRowSubQuery = STL_FALSE;
                    sRowExpr->mSubQueryExpr = NULL;
                }
            }
            else
            {
                if( sHasSingleExpr == STL_TRUE )
                {
                    sRowExpr->mArgCount = 1;
                
                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                STL_SIZEOF( qlvInitExpression* ),
                                                (void **) & sRowExpr->mArgs,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
        
                    stlMemset( sRowExpr->mArgs,
                               0x00,
                               STL_SIZEOF( qlvInitExpression* ) );

                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                STL_SIZEOF( knlListEntry ),
                                                (void **) & sRowExpr->mEntry,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
        
                    stlMemset( sRowExpr->mEntry,
                               0x00,
                               STL_SIZEOF( knlListEntry ) );
                }
                else
                {
                    /* Do Nothing */
                }
                
                sIncludeExprCnt += sArgCode->mIncludeExprCnt;
                sRowExpr->mArgs[ sArgCount ] = sArgCode;
                sRowExpr->mSubQueryExpr = NULL;
                sRowExpr->mIsRowSubQuery = STL_FALSE;

                sArgCount++;
            }
        }
    }
    else
    {
        /* coverage : parser에 의해 이미 평가되었다. */
        STL_DASSERT( 0 );
    }
    
    STL_DASSERT( sRowExpr->mArgCount == sArgCount );

     /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( sIncludeExprCnt + 1, aEnv );
    
    sExprCode->mType           = QLV_EXPR_TYPE_ROW_EXPR;
    sExprCode->mPosition       = aRowSource->mNodePos;
    sExprCode->mIncludeExprCnt = sIncludeExprCnt + 1;
    sExprCode->mExpr.mRowExpr  = sRowExpr;
    sExprCode->mRelatedFuncId  = aRelatedFuncId;
    sExprCode->mIterationTime  = sIterTime;
    sExprCode->mColumnName     = NULL;

    /**
     * OUTPUT 설정
     */

    *aExprCode = sExprCode;

    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_MAXIMUM_NUMBER_OF_ARGUMENTS_EXCEEDED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MAXIMUM_NUMBER_OF_ARGUMENTS_EXCEEDED,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aRowSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_INVALID_NUMBER_OF_ARGUMENTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_NUMBER_OF_ARGUMENTS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sArgCode->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Null BNF validation
 * @param[in]      aStmtInfo      Stmt Information
 * @param[in]      aExprSource    Expression Source
 * @param[out]     aExprCode      Expression Code
 * @param[in]      aRelatedFuncId Related Function ID
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime 상위 노드의 Iteration Time
 * @param[in]      aRegionMem     Region Memory
 * @param[in]      aEnv           Environment
 */
stlStatus qlvValidateNullBNF( qlvStmtInfo         * aStmtInfo,
                              qlpValue            * aExprSource,
                              qlvInitExpression  ** aExprCode,
                              knlBuiltInFunc        aRelatedFuncId,
                              dtlIterationTime      aIterationTimeScope,
                              dtlIterationTime      aIterationTime,
                              knlRegionMem        * aRegionMem,
                              qllEnv              * aEnv )
{
    qlvInitExpression  * sExprCode  = NULL;
    qlvInitValue       * sNullValue = NULL;
    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource->mType == QLL_BNF_NULL_CONSTANT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTimeScope >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTimeScope < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTime >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTime < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * Code Null Value 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitValue ),
                                (void **) & sNullValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sNullValue,
               0x00,
               STL_SIZEOF( qlvInitValue ) );


    /**
     * Code Null Value 설정
     */

    sNullValue->mValueType       = QLV_VALUE_TYPE_NULL;
    sNullValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    sNullValue->mData.mString    = NULL;
    

    /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sExprCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sExprCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

    
    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );
    
    sExprCode->mType           = QLV_EXPR_TYPE_VALUE;
    sExprCode->mPosition       = aExprSource->mNodePos;
    sExprCode->mIncludeExprCnt = 1;
    sExprCode->mExpr.mValue    = sNullValue;
    sExprCode->mRelatedFuncId  = aRelatedFuncId;
    sExprCode->mIterationTime  = DTL_ITERATION_TIME_NONE;
    sExprCode->mColumnName     = NULL;
    
    
    /**
     * OUTPUT 설정
     */

    *aExprCode = sExprCode;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief String literal BNF validation
 * @param[in]      aStmtInfo      Stmt Information
 * @param[in]      aExprSource    Expression Source
 * @param[out]     aExprCode      Expression Code
 * @param[in]      aRelatedFuncId Related Function ID
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime 상위 노드의 Iteration Time
 * @param[in]      aRegionMem     Region Memory
 * @param[in]      aEnv           Environment
 */
stlStatus qlvValidateStringBNF( qlvStmtInfo         * aStmtInfo,
                                qlpValue            * aExprSource,
                                qlvInitExpression  ** aExprCode,
                                knlBuiltInFunc        aRelatedFuncId,
                                dtlIterationTime      aIterationTimeScope,
                                dtlIterationTime      aIterationTime,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv )
{
    qlvInitExpression  * sExprCode    = NULL;
    qlvInitValue       * sStringValue = NULL;
    stlChar            * sStr         = NULL;
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource->mType == QLL_BNF_STRING_CONSTANT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTimeScope >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTimeScope < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTime >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTime < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * Code String Value 생성
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitValue ),
                                (void **) & sStringValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sStringValue,
               0x00,
               STL_SIZEOF( qlvInitValue ) );


    /**
     * Code Value 설정
     */
    
    sStr = QLP_GET_PTR_VALUE( aExprSource );
    
    STL_DASSERT( sStr != NULL );

    if( sStr[0] == '\0' )
    {
        /**
         * Code Null Value 설정
         */

        sStringValue->mValueType       = QLV_VALUE_TYPE_STRING_FIXED;
        sStringValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
        sStringValue->mData.mString    = QLV_PREDEFINE_NULL_STRING;
        
    }
    else
    {
        /**
         * Code String Value 설정
         */

        sStringValue->mValueType       = QLV_VALUE_TYPE_STRING_FIXED;
        sStringValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    stlStrlen( sStr ) + 1,
                                    (void **) & sStringValue->mData.mString,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlStrcpy( sStringValue->mData.mString, sStr );
    }


    /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sExprCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sExprCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

    
    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );
    
    sExprCode->mType           = QLV_EXPR_TYPE_VALUE;
    sExprCode->mPosition       = aExprSource->mNodePos;
    sExprCode->mIncludeExprCnt = 1;
    sExprCode->mExpr.mValue    = sStringValue;
    sExprCode->mRelatedFuncId  = aRelatedFuncId;
    sExprCode->mIterationTime  = DTL_ITERATION_TIME_NONE;
    sExprCode->mColumnName     = NULL;
    

    /**
     * OUTPUT 설정
     */

    *aExprCode = sExprCode;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Bit String Literal BNF validation (hex string)
 * @param[in]      aStmtInfo      Stmt Information
 * @param[in]      aExprSource    Expression Source
 * @param[out]     aExprCode      Expression Code
 * @param[in]      aRelatedFuncId Related Function ID
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime 상위 노드의 Iteration Time
 * @param[in]      aRegionMem     Region Memory
 * @param[in]      aEnv           Environment
 */
stlStatus qlvValidateBitStringBNF( qlvStmtInfo         * aStmtInfo,
                                   qlpValue            * aExprSource,
                                   qlvInitExpression  ** aExprCode,
                                   knlBuiltInFunc        aRelatedFuncId,
                                   dtlIterationTime      aIterationTimeScope,
                                   dtlIterationTime      aIterationTime,
                                   knlRegionMem        * aRegionMem,
                                   qllEnv              * aEnv )
{
    qlvInitExpression  * sExprCode    = NULL;
    qlvInitValue       * sStringValue = NULL;
    stlChar            * sBitStr      = NULL;
    stlBool              sIsValid     = STL_FALSE;
    stlInt64             sPrecision   = 0;
    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource->mType == QLL_BNF_BITSTR_CONSTANT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTimeScope >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTimeScope < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTime >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTime < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * Code Bit String Value 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitValue ),
                                (void **) & sStringValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sStringValue,
               0x00,
               STL_SIZEOF( qlvInitValue ) );


    /**
     * Code Value 설정
     */
    
    sBitStr = QLP_GET_PTR_VALUE( aExprSource );
    
    STL_DASSERT( sBitStr != NULL );
    
    if( sBitStr[0] == '\0' )
    {
        /**
         * Code Null Value 설정
         */

        sStringValue->mValueType       = QLV_VALUE_TYPE_BITSTRING_FIXED;
        sStringValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
        sStringValue->mData.mString    = QLV_PREDEFINE_NULL_STRING;

    }
    else
    {
        /**
         * Binary String 값 Validation (1byte = 16진수 2자리)
         */

        STL_TRY( dtlValidateBitString( sBitStr, & sIsValid, & sPrecision, QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
        
        STL_DASSERT( sIsValid == STL_TRUE );


        /**
         * Code String Value 설정
         */

        sStringValue->mValueType       = QLV_VALUE_TYPE_BITSTRING_FIXED;
        sStringValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    stlStrlen( sBitStr ) + 1,
                                    (void **) & sStringValue->mData.mString,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlStrcpy( sStringValue->mData.mString, sBitStr );
    }    


    /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sExprCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sExprCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

    
    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );
    
    sExprCode->mType           = QLV_EXPR_TYPE_VALUE;
    sExprCode->mPosition       = aExprSource->mNodePos;
    sExprCode->mIncludeExprCnt = 1;
    sExprCode->mExpr.mValue    = sStringValue;
    sExprCode->mRelatedFuncId  = aRelatedFuncId;
    sExprCode->mIterationTime  = DTL_ITERATION_TIME_NONE;
    sExprCode->mColumnName     = NULL;


    /**
     * OUTPUT 설정
     */

    *aExprCode = sExprCode;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Number Literal BNF validation
 * @param[in]      aStmtInfo      Stmt Information
 * @param[in]      aExprSource    Expression Source
 * @param[out]     aExprCode      Expression Code
 * @param[in]      aRelatedFuncId Related Function ID
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime 상위 노드의 Iteration Time
 * @param[in]      aRegionMem     Region Memory
 * @param[in]      aEnv           Environment
 */
stlStatus qlvValidateNumberBNF( qlvStmtInfo         * aStmtInfo,
                                qlpValue            * aExprSource,
                                qlvInitExpression  ** aExprCode,
                                knlBuiltInFunc        aRelatedFuncId,
                                dtlIterationTime      aIterationTimeScope,
                                dtlIterationTime      aIterationTime,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv )
{
    qlvInitExpression  * sExprCode   = NULL;
    qlvInitValue       * sFloatValue = NULL;
    stlChar            * sStr        = NULL;
    stlSize              sStrSize;
    /* stlBool              sIsValid    = STL_FALSE; */

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource->mType == QLL_BNF_NUMBER_CONSTANT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTimeScope >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTimeScope < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTime >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTime < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * Code Float Value 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitValue ),
                                (void **) & sFloatValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sFloatValue,
               0x00,
               STL_SIZEOF( qlvInitValue ) );


    /**
     * Code Value 설정
     */
    
    sStr = QLP_GET_PTR_VALUE( aExprSource );

    STL_DASSERT( sStr != NULL );
        

    /**
     * Code Float Value 설정
     */

    sStrSize = stlStrlen( sStr );

    switch( sStr[ sStrSize - 1 ] )
    {
        case 'd' :
        case 'D' :
            {
                sFloatValue->mValueType = QLV_VALUE_TYPE_BINARY_DOUBLE;
                --sStrSize;
                break;
            }
            
        case 'f' :
        case 'F' :
            {
                sFloatValue->mValueType = QLV_VALUE_TYPE_BINARY_REAL;
                --sStrSize;
                break;
            }

        default :
            {
                sFloatValue->mValueType = QLV_VALUE_TYPE_NUMERIC;
                break;
            }
    }

    sFloatValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                sStrSize + 1,
                                (void **) & sFloatValue->mData.mString,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemcpy( sFloatValue->mData.mString, sStr, sStrSize );
    sFloatValue->mData.mString[ sStrSize ] = '\0';


    /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sExprCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sExprCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

    
    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );
    
    sExprCode->mType           = QLV_EXPR_TYPE_VALUE;
    sExprCode->mPosition       = aExprSource->mNodePos;
    sExprCode->mIncludeExprCnt = 1;
    sExprCode->mExpr.mValue    = sFloatValue;
    sExprCode->mRelatedFuncId  = aRelatedFuncId;
    sExprCode->mIterationTime  = DTL_ITERATION_TIME_NONE;
    sExprCode->mColumnName     = NULL;
    

    /**
     * OUTPUT 설정
     */

    *aExprCode = sExprCode;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief DateTime Literal BNF validation
 * @param[in]      aStmtInfo      Stmt Information
 * @param[in]      aExprSource    Expression Source
 * @param[out]     aExprCode      Expression Code
 * @param[in]      aRelatedFuncId Related Function ID
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime 상위 노드의 Iteration Time
 * @param[in]      aRegionMem     Region Memory
 * @param[in]      aEnv           Environment
 */
stlStatus qlvValidateDateTimeBNF( qlvStmtInfo         * aStmtInfo,
                                  qlpValue            * aExprSource,
                                  qlvInitExpression  ** aExprCode,
                                  knlBuiltInFunc        aRelatedFuncId,
                                  dtlIterationTime      aIterationTimeScope,
                                  dtlIterationTime      aIterationTime,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv )
{
    qlvInitExpression  * sExprCode   = NULL;
    qlvInitValue       * sDateTimeValue = NULL;
    stlChar            * sStr        = NULL;
    qlvValueType         sValueType  = QLV_VALUE_TYPE_INVALID;    
    stlSize              sStrSize;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aExprSource->mType == QLL_BNF_DATE_CONSTANT_TYPE) ||
                        (aExprSource->mType == QLL_BNF_TIME_CONSTANT_TYPE) ||
                        (aExprSource->mType == QLL_BNF_TIME_TZ_CONSTANT_TYPE) ||
                        (aExprSource->mType == QLL_BNF_TIMESTAMP_CONSTANT_TYPE) ||
                        (aExprSource->mType == QLL_BNF_TIMESTAMP_TZ_CONSTANT_TYPE),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTimeScope >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTimeScope < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTime >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTime < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * Code Date Value 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitValue ),
                                (void **) & sDateTimeValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sDateTimeValue,
               0x00,
               STL_SIZEOF( qlvInitValue ) );


    /**
     * Code Value 설정
     */
    
    sStr = QLP_GET_PTR_VALUE( aExprSource );
    
    STL_DASSERT( sStr != NULL );
    
    /* DATE'' TIME'' TIMESTAMP'' 는 syntax error로 이미 처리되었음. */
    STL_DASSERT( sStr[0] != '\0' );
    
    
    /**
     * Code Date Value 설정
     */
    
    sStrSize = stlStrlen( sStr );

    switch( ((qllNode*)(aExprSource))->mType )
    {
        case QLL_BNF_DATE_CONSTANT_TYPE :
            {
                sValueType = QLV_VALUE_TYPE_DATE_LITERAL_STRING;
                break;
            }
        case QLL_BNF_TIME_CONSTANT_TYPE :
            {
                if( dtlIsExistTimeZoneStr( sStr, sStrSize, STL_FALSE ) == STL_FALSE )
                {
                    sValueType = QLV_VALUE_TYPE_TIME_LITERAL_STRING;
                }
                else
                {
                    sValueType = QLV_VALUE_TYPE_TIME_TZ_LITERAL_STRING;
                }
                
                break;                
            }
        case QLL_BNF_TIME_TZ_CONSTANT_TYPE :
            {
                sValueType = QLV_VALUE_TYPE_TIME_TZ_LITERAL_STRING;
                break;
            }
        case QLL_BNF_TIMESTAMP_CONSTANT_TYPE :
            {
                if( dtlIsExistTimeZoneStr( sStr, sStrSize, STL_TRUE ) == STL_FALSE )
                {
                    sValueType = QLV_VALUE_TYPE_TIMESTAMP_LITERAL_STRING;
                }
                else
                {
                    sValueType = QLV_VALUE_TYPE_TIMESTAMP_TZ_LITERAL_STRING;
                }
                
                break;                
            }
        case QLL_BNF_TIMESTAMP_TZ_CONSTANT_TYPE :
            {
                sValueType = QLV_VALUE_TYPE_TIMESTAMP_TZ_LITERAL_STRING;
                break;
            }
        default:
            {
                STL_DASSERT( STL_FALSE );
                break;
            }
    }
    
    sDateTimeValue->mValueType = sValueType;
    sDateTimeValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                sStrSize + 1,
                                (void **) & sDateTimeValue->mData.mString,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlStrcpy( sDateTimeValue->mData.mString, sStr );
    
    /**
     * Code Expression 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sExprCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sExprCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );
    
    /**
     * Code Expression 설정
     */
    
    QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );

    sExprCode->mType           = QLV_EXPR_TYPE_VALUE;
    sExprCode->mPosition       = aExprSource->mNodePos;
    sExprCode->mIncludeExprCnt = 1;
    sExprCode->mExpr.mValue    = sDateTimeValue;
    sExprCode->mRelatedFuncId  = aRelatedFuncId;
    sExprCode->mIterationTime  = DTL_ITERATION_TIME_NONE;
    sExprCode->mColumnName     = NULL;
    
    /**
     * OUTPUT 설정
     */

    *aExprCode = sExprCode;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Parameter Integer BNF validation
 * @param[in]      aStmtInfo      Stmt Information
 * @param[in]      aExprSource    Expression Source
 * @param[out]     aExprCode      Expression Code
 * @param[in]      aRelatedFuncId Related Function ID
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime 상위 노드의 Iteration Time
 * @param[in]      aRegionMem     Region Memory
 * @param[in]      aEnv           Environment
 */
stlStatus qlvValidateIntParamBNF( qlvStmtInfo         * aStmtInfo,
                                  qlpValue            * aExprSource,
                                  qlvInitExpression  ** aExprCode,
                                  knlBuiltInFunc        aRelatedFuncId,
                                  dtlIterationTime      aIterationTimeScope,
                                  dtlIterationTime      aIterationTime,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv )
{
    qlvInitExpression  * sExprCode     = NULL;
    qlvInitValue       * sIntegerValue = NULL;
    stlBool              sIsValid      = STL_FALSE;
    stlInt64             sInteger      = 0;

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource->mType == QLL_BNF_INTPARAM_CONSTANT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTimeScope >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTimeScope < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTime >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTime < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * Code Integer Value 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitValue ),
                                (void **) & sIntegerValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sIntegerValue,
               0x00,
               STL_SIZEOF( qlvInitValue ) );


    /**
     * Code Value 설정
     */
    
    sInteger = QLP_GET_INT_VALUE( aExprSource );
    
    
    /**
     * Integer 값 범위 Validation ( Boolean Type )
     */

    STL_TRY( dtlValidateBoolean( sInteger, & sIsValid, QLL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
        
    STL_DASSERT( sIsValid == STL_TRUE );
        

    /**
     * Code Integer Value 설정
     */

    sIntegerValue->mValueType       = QLV_VALUE_TYPE_BOOLEAN;
    sIntegerValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    sIntegerValue->mData.mInteger   = sInteger;


    /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sExprCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sExprCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

    
    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );

    sExprCode->mType           = QLV_EXPR_TYPE_VALUE;
    sExprCode->mPosition       = aExprSource->mNodePos;
    sExprCode->mIncludeExprCnt = 1;
    sExprCode->mExpr.mValue    = sIntegerValue;
    sExprCode->mRelatedFuncId  = aRelatedFuncId;
    sExprCode->mIterationTime  = DTL_ITERATION_TIME_NONE;
    sExprCode->mColumnName     = NULL;


    /**
     * OUTPUT 설정
     */

    *aExprCode = sExprCode;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief DEFAULT BNF 에 해당하는 Code Value Expression 을 생성 
 * @param[in]  aStmtInfo           Stmt Information
 * @param[in]  aValidationObjList  Validation Object List
 * @param[in]  aExprSource         Default Expression Source
 * @param[in]  aColumnHandle       Column Handle
 * @param[out] aExprCode           Expression Code
 * @param[in]  aRelatedFuncId      Related Function ID
 * @param[in]  aIterationTime      상위 노드의 Iteration Time
 * @param[in]  aRegionMem          Region Memory
 * @param[in]  aEnv                Environment
 */
stlStatus qlvValidateDefaultBNF( qlvStmtInfo         * aStmtInfo,
                                 ellObjectList       * aValidationObjList,
                                 qllNode             * aExprSource,
                                 ellDictHandle       * aColumnHandle,
                                 qlvInitExpression  ** aExprCode,
                                 knlBuiltInFunc        aRelatedFuncId,
                                 dtlIterationTime      aIterationTime,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv )
{
    stlStatus sStatus;
    stlChar * sOrgString = NULL;
    
    qlvInitExpression  * sExprCode     = NULL;
    qlvInitValue       * sCodeValue    = NULL;
    qlvInitPseudoCol   * sCodePseudoCol = NULL;

    stlChar * sDefaultString = NULL;
    
    qllNode * sParseTree = NULL;
    qlpPhraseDefaultExpr * sPhraseDefault = NULL;
    
    stlInt32  sBindCount = 0;
    
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aIterationTime >= DTL_ITERATION_TIME_FOR_EACH_AGGREGATION ) &&
                        ( aIterationTime < DTL_ITERATION_TIME_MAX ),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */

    sDefaultString = ellGetColumnDefaultValue( aColumnHandle );

    /**
     * DEFAULT 유형별 Value Expression 생성 
     */
    
    if ( sDefaultString != NULL )
    {
        /**
         * DEFAULT 구문 Parsing
         */

        qlgSetQueryPhaseMemMgr( aStmtInfo->mDBCStmt, QLL_PHASE_PARSING, aEnv );
        
        sStatus = qlgParseInternal( aStmtInfo->mDBCStmt,
                                    sDefaultString,
                                    & sParseTree,
                                    & sBindCount,
                                    aEnv );

        qlgSetQueryPhaseMemMgr( aStmtInfo->mDBCStmt, QLL_PHASE_VALIDATION, aEnv );
        
        STL_TRY_THROW( sStatus == STL_SUCCESS, RAMP_ERR_DEFAULT_HAS_ERRORS );
        
        STL_DASSERT( sBindCount == 0 );
        STL_DASSERT( sParseTree->mType == QLL_PHRASE_DEFAULT_EXPR_TYPE );

        sPhraseDefault = (qlpPhraseDefaultExpr *) sParseTree;
        
        /**
         * DEFAULT 구문 Validation
         */

        sOrgString = aStmtInfo->mSQLString;
        aStmtInfo->mSQLString = sDefaultString;
        
        sStatus = qlvValidateValueExpr( aStmtInfo,
                                        QLV_EXPR_PHRASE_DEFAULT,
                                        aValidationObjList,
                                        NULL, /* aRefTableList */
                                        STL_FALSE,  /* aIsAtomicInsert */
                                        STL_FALSE,  /* Row Expr */
                                        0, /* Allowed Aggregation Depth */
                                        (qllNode*) sPhraseDefault->mDefaultExpr,
                                        & sExprCode,
                                        NULL, /* aHasOuterJoinOperator */
                                        KNL_BUILTIN_FUNC_INVALID,
                                        DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                        DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                        QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                        aEnv );
        aStmtInfo->mSQLString = sOrgString;
        STL_TRY_THROW( sStatus == STL_SUCCESS, RAMP_ERR_DEFAULT_HAS_ERRORS );
    }
    else
    {
        if ( ellGetColumnIdentityHandle( aColumnHandle ) != NULL )
        {
            /**
             * Identity Column 의 Nextval 함수를 설정해야 함.
             * - NEXTVAL(column) 에 해당하는 Expression 생성 
             */

            /**
             * NEXTVAL Init Pseudo Column 생성
             */
            
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( qlvInitPseudoCol ),
                                        (void **) & sCodePseudoCol,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemset( sCodePseudoCol,
                       0x00,
                       STL_SIZEOF( qlvInitPseudoCol ) );

            sCodePseudoCol->mPseudoId = KNL_PSEUDO_COL_NEXTVAL;
            sCodePseudoCol->mArgCount = 1;

            /**
             * Identity Handle 을 Argument 설정 
             */
            
            STL_TRY( knlAllocRegionMem(
                         aRegionMem,
                         STL_SIZEOF(ellPseudoArg),
                         (void **) & sCodePseudoCol->mArgs,
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemset( sCodePseudoCol->mArgs, 0x00, STL_SIZEOF(ellPseudoArg) );

            sCodePseudoCol->mSeqDictHandle = NULL;
            sCodePseudoCol->mArgs[0].mPseudoArg =
                ellGetColumnIdentityHandle( aColumnHandle );
            
            /**
             * Code Expression 설정 
             */
            
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( qlvInitExpression ),
                                        (void **) & sExprCode,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemset( sExprCode, 0x00, STL_SIZEOF( qlvInitExpression ) );

            QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );
            
            sExprCode->mType            = QLV_EXPR_TYPE_PSEUDO_COLUMN; 
            sExprCode->mPosition        = aExprSource->mNodePos;
            sExprCode->mRelatedFuncId   = aRelatedFuncId;
            sExprCode->mColumnName      = NULL;
            sExprCode->mIncludeExprCnt  = 1;
            sExprCode->mIterationTime   =
                gPseudoColInfoArr[KNL_PSEUDO_COL_NEXTVAL].mIterationTime;
            sExprCode->mExpr.mPseudoCol = sCodePseudoCol;
        }
        else
        {
            /**
             * DEFAULT NULL 임
             */

            /**
             * Code NULL Value 생성
             */
            
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( qlvInitValue ),
                                        (void **) & sCodeValue,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemset( sCodeValue,
                       0x00,
                       STL_SIZEOF( qlvInitValue ) );
            
            sCodeValue->mValueType       = QLV_VALUE_TYPE_NULL;
            sCodeValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
            sCodeValue->mData.mString    = NULL;

            /**
             * Code Expression 공간 할당
             */
            
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( qlvInitExpression ),
                                        (void **) & sExprCode,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            stlMemset( sExprCode,
                       0x00,
                       STL_SIZEOF( qlvInitExpression ) );
            
            /**
             * Code Expression 설정
             */
            
            sExprCode->mType           = QLV_EXPR_TYPE_VALUE;
            sExprCode->mPosition       = aExprSource->mNodePos;
            sExprCode->mRelatedFuncId  = aRelatedFuncId;
            sExprCode->mColumnName     = NULL;
            sExprCode->mIncludeExprCnt = 1;
            sExprCode->mIterationTime  = DTL_ITERATION_TIME_NONE;
            sExprCode->mExpr.mValue    = sCodeValue;
        }
    }


    /**
     * OUTPUT 설정
     */

    *aExprCode = sExprCode;

    return STL_SUCCESS;

    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   qlgMakeErrPosString( aStmtInfo->mSQLString, */
    /*                                        aExprSource->mNodePos, */
    /*                                        aEnv ), */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlvValidateDefaultValue()" ); */
    /* } */

    STL_CATCH( RAMP_ERR_DEFAULT_HAS_ERRORS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DEFAULT_EXPRESSION_HAS_ERRORS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aExprSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Cast Validation
 * @param[in]      aStmtInfo             Stmt Information
 * @param[in]      aPhraseType           Expr 이 사용된 Phrase 유형 
 * @param[in]      aValidationObjList    Validation Object List
 * @param[in]      aRefTableList         Reference Table List
 * @param[in]      aIsAtomic             Atomic Statement인지 여부
 * @param[in]      aAllowedAggrDepth     Allowed Aggregation Depth
 * @param[in]      aCastSource           Cast Source
 * @param[out]     aExprCode             Expression 
 * @param[out]     aHasOuterJoinOperator Outer Join Operator를 가졌는지 여부
 * @param[in]      aRelatedFuncId        Related Function ID
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime        상위 노드의 Iteration Time
 * @param[in]      aRegionMem            Region Memory
 * @param[in]      aEnv                  Environment
 */
stlStatus qlvValidateCast( qlvStmtInfo         * aStmtInfo,
                           qlvExprPhraseType     aPhraseType,
                           ellObjectList       * aValidationObjList,
                           qlvRefTableList     * aRefTableList,
                           stlBool               aIsAtomic,
                           stlInt32              aAllowedAggrDepth,
                           qlpTypeCast         * aCastSource,
                           qlvInitExpression  ** aExprCode,
                           stlBool             * aHasOuterJoinOperator,
                           knlBuiltInFunc        aRelatedFuncId,
                           dtlIterationTime      aIterationTimeScope,
                           dtlIterationTime      aIterationTime,
                           knlRegionMem        * aRegionMem,
                           qllEnv              * aEnv )
{
    qlvInitExpression  * sExprCode       = NULL;
    qlvInitExpression  * sArgCode        = NULL;
    qlvInitTypeCast    * sCast           = NULL;
    stlInt64             sIncludeExprCnt = 0;
    dtlIterationTime     sIterTime       = DTL_ITERATION_TIME_INVALID;

    qllNode            * sCastSrc        = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCastSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCastSource->mType == QLL_TYPECAST_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    sCastSrc = aCastSource->mExpr;

    /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sExprCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sExprCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );


    /**
     * Code Type Cast 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitTypeCast ),
                                (void **) & sCast,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sCast,
               0x00,
               STL_SIZEOF( qlvInitTypeCast ) );


    /**
     * Code Cast의 TypeName 설정
     */
    
    STL_TRY( qlvSetColumnTypeForCast( aStmtInfo,
                                      & sCast->mTypeDef,
                                      aCastSource->mTypeName,
                                      aEnv )
             == STL_SUCCESS );

    
    /**
     * Argument List 공간 할당
     */

    STL_TRY( knlAllocRegionMem(
                 aRegionMem,
                 STL_SIZEOF( qlvInitExpression* ) * DTL_CAST_INPUT_ARG_CNT,
                 (void **) & sCast->mArgs,
                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
        
    stlMemset( sCast->mArgs,
               0x00,
               STL_SIZEOF( qlvInitExpression* ) * DTL_CAST_INPUT_ARG_CNT );

    
    /**
     * Code Cast의 Argument 설정
     */

    /* target value */
    STL_TRY( qlvValidateValueExpr(
                 aStmtInfo,
                 aPhraseType,
                 aValidationObjList,
                 aRefTableList,
                 aIsAtomic,
                 STL_FALSE,  /* Row Expr */
                 aAllowedAggrDepth,
                 sCastSrc,
                 & sArgCode,
                 aHasOuterJoinOperator,
                 KNL_BUILTIN_FUNC_CAST,
                 aIterationTimeScope,
                 gBuiltInFuncInfoArr[ KNL_BUILTIN_FUNC_CAST ].mIterationTime,
                 aRegionMem,
                 aEnv )
             == STL_SUCCESS );

    sIncludeExprCnt += sArgCode->mIncludeExprCnt;
    sCast->mArgs[ 0 ] = sArgCode;

    sIterTime = sArgCode->mIterationTime;
    
    /* ArgNum1 */
    STL_TRY( qlvAddIntegerValueExpr(
                 aStmtInfo,
                 sCast->mTypeDef.mArgNum1,
                 & sArgCode,
                 KNL_BUILTIN_FUNC_CAST,
                 gBuiltInFuncInfoArr[ KNL_BUILTIN_FUNC_CAST ].mIterationTime,
                 aRegionMem,
                 aEnv )
             == STL_SUCCESS );

    sIncludeExprCnt++;
    sCast->mArgs[ 1 ] = sArgCode;

    /* ArgNum2 */
    STL_TRY( qlvAddIntegerValueExpr(
                 aStmtInfo,
                 sCast->mTypeDef.mArgNum2,
                 & sArgCode,
                 KNL_BUILTIN_FUNC_CAST,
                 gBuiltInFuncInfoArr[ KNL_BUILTIN_FUNC_CAST ].mIterationTime,
                 aRegionMem,
                 aEnv )
             == STL_SUCCESS );

    sIncludeExprCnt++;
    sCast->mArgs[ 2 ] = sArgCode;

    /* StringLengthUnit */
    STL_TRY( qlvAddIntegerValueExpr(
                 aStmtInfo,
                 sCast->mTypeDef.mStringLengthUnit,
                 & sArgCode,
                 KNL_BUILTIN_FUNC_CAST,
                 gBuiltInFuncInfoArr[ KNL_BUILTIN_FUNC_CAST ].mIterationTime,
                 aRegionMem,
                 aEnv )
             == STL_SUCCESS );

    sIncludeExprCnt++;
    sCast->mArgs[ 3 ] = sArgCode;
    
    /* IntervalIndicator */
    STL_TRY( qlvAddIntegerValueExpr(
                 aStmtInfo,
                 sCast->mTypeDef.mIntervalIndicator,
                 & sArgCode,
                 KNL_BUILTIN_FUNC_CAST,
                 gBuiltInFuncInfoArr[ KNL_BUILTIN_FUNC_CAST ].mIterationTime,
                 aRegionMem,
                 aEnv )
             == STL_SUCCESS );

    sIncludeExprCnt++;
    sCast->mArgs[ 4 ] = sArgCode;
    
    
    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( sIncludeExprCnt + 1, aEnv );
    
    sExprCode->mType           = QLV_EXPR_TYPE_CAST;
    sExprCode->mPosition       = aCastSource->mNodePos;
    sExprCode->mIncludeExprCnt = sIncludeExprCnt + 1;
    sExprCode->mExpr.mTypeCast = sCast;
    sExprCode->mRelatedFuncId  = aRelatedFuncId;
    sExprCode->mIterationTime  = sIterTime;
    sExprCode->mColumnName     = NULL;
    

    /**
     * OUTPUT 설정
     */

    *aExprCode = sExprCode;

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Pseudo Column Validation
 * @param[in]      aStmtInfo             Stmt Information
 * @param[in]      aPhraseType           Expr 이 사용된 Phrase 유형 
 * @param[in]      aValidationObjList    Validation Object List
 * @param[in]      aRefTableList         Reference Table List
 * @param[in]      aIsAtomic             Atomic Statement인지 여부
 * @param[in]      aAllowedAggrDepth     Allowed Aggregation Depth
 * @param[in]      aPseudoCol            Cast Source
 * @param[out]     aExprCode             Expression 
 * @param[out]     aHasOuterJoinOperator Outer Join Operator를 가졌는지 여부
 * @param[in]      aRelatedFuncId        Related Function ID
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime        상위 노드의 Iteration Time
 * @param[in]      aRegionMem            Region Memory
 * @param[in]      aEnv                  Environment
 */
stlStatus qlvValidatePseudoCol( qlvStmtInfo         * aStmtInfo,
                                qlvExprPhraseType     aPhraseType,
                                ellObjectList       * aValidationObjList,
                                qlvRefTableList     * aRefTableList,
                                stlBool               aIsAtomic,
                                stlInt32              aAllowedAggrDepth,
                                qlpPseudoCol        * aPseudoCol,
                                qlvInitExpression  ** aExprCode,
                                stlBool             * aHasOuterJoinOperator,
                                knlBuiltInFunc        aRelatedFuncId,
                                dtlIterationTime      aIterationTimeScope,
                                dtlIterationTime      aIterationTime,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv )
{
    qlvInitExpression  * sExprCode       = NULL;
    qlvInitExpression  * sArgCode        = NULL;
    qlvInitPseudoCol   * sCodePseudoCol  = NULL;
    qlpListElement     * sListElem       = NULL;   
    qllNode            * sNode           = NULL;
    stlInt64             sIncludeExprCnt = 0;
    stlInt32             sArgCount       = 0;
    stlInt32             sStrLen         = 0;
    dtlIterationTime     sIterTime       = DTL_ITERATION_TIME_INVALID;
    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPseudoCol != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPseudoCol->mType == QLL_PSEUDO_COLUMN_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Iteration Time Validation
     */

    STL_TRY_THROW( aIterationTimeScope <= DTL_ITERATION_TIME_FOR_EACH_EXPR,
                   RAMP_ERR_INVALID_ITERATION_TIME_SCOPE );


    /**
     * Pseudo Column의 Iteration Time에 대한 허용 범위 체크
     * ------------------------------
     *  Pseudo column 중 ROWID 는 올 수 없다.
     *  @todo LEVEL, ROWNUM 도 마찬가지.
     */

    if( (aPseudoCol->mPseudoId == KNL_PSEUDO_COL_CURRVAL) ||
        (aPseudoCol->mPseudoId == KNL_PSEUDO_COL_NEXTVAL) )
    {
        /**
         * Sequence는 iteration time scope 처리에서 예외임.
         */

        STL_TRY_THROW( aIterationTime != DTL_ITERATION_TIME_FOR_EACH_AGGREGATION,
                       RAMP_ERR_SEQUENCE_POSITION );
    }
    else
    {
        /* coverage : pseudo column 확장시 고려할 사항임. */
        STL_TRY_THROW( gPseudoColInfoArr[aPseudoCol->mPseudoId].mIterationTime >= aIterationTimeScope,
                       RAMP_ERR_PSEUDO_COLUMN_NOT_ALLOWED );
    }
    
    /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sExprCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sExprCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );


    /**
     * Pseudo Column Code 구성
     */

    sIterTime = gPseudoColInfoArr[aPseudoCol->mPseudoId].mIterationTime;    
        
    /**
     * Init Pseudo Column 생성
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitPseudoCol ),
                                (void **) & sCodePseudoCol,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sCodePseudoCol,
               0x00,
               STL_SIZEOF( qlvInitPseudoCol ) );


    /**
     * Code Function 설정
     */

    if ( (aPseudoCol->mPseudoId == KNL_PSEUDO_COL_CURRVAL) ||
         (aPseudoCol->mPseudoId == KNL_PSEUDO_COL_NEXTVAL) )
    {
        /**
         * Sequence 는 Target 에만 사용할 수 있다.
         * - INSERT VALUES seq
         * - UPDATE SET seq
         * - SELECT seq
         */
        STL_TRY_THROW( (aPhraseType == QLV_EXPR_PHRASE_TARGET) ||
                       (aPhraseType == QLV_EXPR_PHRASE_DEFAULT) 
                       , RAMP_ERR_SEQUENCE_POSITION );
    }
    
    sCodePseudoCol->mPseudoId = aPseudoCol->mPseudoId;

    
    /**
     * Arguments 설정
     */

    if( aPseudoCol->mArgs != NULL )
    {
        /**
         * Argument List 공간 할당
         */
        
        sCodePseudoCol->mArgCount = aPseudoCol->mArgs->mCount;

        STL_TRY( knlAllocRegionMem(
                     aRegionMem,
                     STL_SIZEOF(ellPseudoArg) * sCodePseudoCol->mArgCount,
                     (void **) & sCodePseudoCol->mArgs,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( sCodePseudoCol->mArgs,
                   0x00,
                   STL_SIZEOF(ellPseudoArg) * sCodePseudoCol->mArgCount );

        
        /**
         * Argument List 구성
         */

        QLP_LIST_FOR_EACH( aPseudoCol->mArgs, sListElem )
        {
            sNode = (qllNode*) QLP_LIST_GET_POINTER_VALUE( sListElem );

            STL_TRY( qlvValidateValueExpr(
                         aStmtInfo,
                         aPhraseType,
                         aValidationObjList,
                         aRefTableList,
                         aIsAtomic,
                         STL_FALSE,  /* Row Expr */
                         aAllowedAggrDepth,
                         (qllNode *) sNode,
                         & sArgCode,
                         aHasOuterJoinOperator,
                         KNL_BUILTIN_FUNC_INVALID,
                         aIterationTimeScope,
                         gPseudoColInfoArr[aPseudoCol->mPseudoId].mIterationTime,
                         aRegionMem,
                         aEnv )
                     == STL_SUCCESS );

            STL_DASSERT( sArgCode->mType == QLV_EXPR_TYPE_PSEUDO_ARGUMENT );
                
            sIterTime = ( sIterTime < sArgCode->mIterationTime )
                ? sIterTime : sArgCode->mIterationTime;

            sIncludeExprCnt += sArgCode->mIncludeExprCnt;

            if ( (aPseudoCol->mPseudoId == KNL_PSEUDO_COL_CURRVAL) ||
                 (aPseudoCol->mPseudoId == KNL_PSEUDO_COL_NEXTVAL) )
            {
                sCodePseudoCol->mSeqDictHandle = sArgCode->mExpr.mPseudoArg;
                sArgCode->mExpr.mPseudoArg = (void *) ellGetSequenceHandle( sArgCode->mExpr.mPseudoArg );
            }
            else
            {
                /* coverage : 현재 sequence 만 구현되어 있어 해당 사항 없음. */
                sCodePseudoCol->mSeqDictHandle = NULL;
            }

            sCodePseudoCol->mArgs[sArgCount].mPseudoArg = sArgCode->mExpr.mPseudoArg;
            sArgCount++;
        }
    }
    else
    {
        /* coverage : 현재 sequence 만 구현되어 있어 해당 사항 없음. */
        sCodePseudoCol->mArgCount = 0;
        sCodePseudoCol->mArgs     = NULL;
    }
    
    STL_DASSERT( sCodePseudoCol->mArgCount == sArgCount );
                 

    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( sIncludeExprCnt + 1, aEnv );

    sExprCode->mType            = QLV_EXPR_TYPE_PSEUDO_COLUMN;
    sExprCode->mPosition        = aPseudoCol->mNodePos;
    sExprCode->mIncludeExprCnt  = sIncludeExprCnt + 1;
    sExprCode->mExpr.mPseudoCol = sCodePseudoCol;
    sExprCode->mRelatedFuncId   = aRelatedFuncId;
    sExprCode->mIterationTime   = sIterTime;

    sStrLen = stlStrlen( gPseudoColInfoArr[ aPseudoCol->mPseudoId ].mName );
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                sStrLen + 1,
                                (void**) &(sExprCode->mColumnName),
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    stlMemcpy( sExprCode->mColumnName,
               gPseudoColInfoArr[ aPseudoCol->mPseudoId ].mName,
               sStrLen );
    sExprCode->mColumnName[ sStrLen ] = '\0';

    
    /**
     * OUTPUT 설정
     */

    *aExprCode = sExprCode;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_ITERATION_TIME_SCOPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ITERATION_TIME_SCOPE_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aPseudoCol->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SEQUENCE_POSITION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SEQUENCE_NUMBER_NOT_ALLOWED,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aPseudoCol->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    /* coverage : 현재 sequence 만 구현되어 있어 해당 사항 없음. */
    STL_CATCH( RAMP_ERR_PSEUDO_COLUMN_NOT_ALLOWED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PSEUDO_COLUMN_NOT_ALLOWED,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aPseudoCol->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Sub Query Validation
 * @param[in]      aStmtInfo             Stmt Information
 * @param[in]      aPhraseType           Expr 이 사용된 Phrase 유형 
 * @param[in]      aValidationObjList    Validation Object List
 * @param[in]      aRefTableList         Reference Table List
 * @param[in]      aIsAtomic             Atomic Statement인지 여부
 * @param[in]      aIsRowExpr            Row Expression인지 여부
 * @param[in]      aAllowedAggrDepth     Allowed Aggregation Depth
 * @param[in]      aSubQuery             Sub Query
 * @param[out]     aExprCode             Expression 
 * @param[in]      aRelatedFuncId        Related Function ID
 * @param[in]      aIterationTimeScope   Iteration Time Scope (허용 가능한 Iteration Time)
 * @param[in]      aIterationTime        상위 노드의 Iteration Time
 * @param[in]      aRegionMem            Region Memory
 * @param[in]      aEnv                  Environment
 */
stlStatus qlvValidateSubQuery( qlvStmtInfo         * aStmtInfo,
                               qlvExprPhraseType     aPhraseType,
                               ellObjectList       * aValidationObjList,
                               qlvRefTableList     * aRefTableList,
                               stlBool               aIsAtomic,
                               stlBool               aIsRowExpr,
                               stlInt32              aAllowedAggrDepth,
                               qlpSubTableNode     * aSubQuery,
                               qlvInitExpression  ** aExprCode,
                               knlBuiltInFunc        aRelatedFuncId,
                               dtlIterationTime      aIterationTimeScope,
                               dtlIterationTime      aIterationTime,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv )
{
    qlvInitExpression  * sExprCode     = NULL;
    qlvInitSubQuery    * sSubQuery     = NULL;
    qlvInitNode        * sInitNode     = NULL;
    qlvSubQueryCategory  sCategory     = QLV_SUB_QUERY_TYPE_RELATION;
    qlvSubQueryType      sSubQueryType = QLV_SUB_QUERY_N_TYPE;
    dtlIterationTime     sIterTime     = DTL_ITERATION_TIME_INVALID;
    stlInt32             sLoop         = 0;
    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQuery != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQuery->mType == QLL_SUB_TABLE_NODE_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sExprCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sExprCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );


    /**
     * Init Subquery 생성
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitSubQuery ),
                                (void **) & sSubQuery,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sSubQuery,
               0x00,
               STL_SIZEOF( qlvInitSubQuery ) );


    /**
     * @todo Phrase에 따른 Validation
     */

    switch( aPhraseType )
    {
        case QLV_EXPR_PHRASE_TARGET :
        case QLV_EXPR_PHRASE_FROM :
        case QLV_EXPR_PHRASE_CONDITION :
        case QLV_EXPR_PHRASE_HAVING :
        case QLV_EXPR_PHRASE_ORDERBY :
        case QLV_EXPR_PHRASE_RETURN :
            {
                sSubQueryType = QLV_SUB_QUERY_N_TYPE;
                sIterTime     = DTL_ITERATION_TIME_FOR_EACH_QUERY;

                break;
            }
        case QLV_EXPR_PHRASE_GROUPBY :
        case QLV_EXPR_PHRASE_OFFSETLIMIT :
        case QLV_EXPR_PHRASE_DEFAULT:
            {
                STL_THROW( RAMP_ERR_SUB_QUERY_NOT_ALLOWED);
                break;
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }


    /**
     * Subquery의 하위 Query Node Validation
     */

    STL_TRY( qlvQueryNodeValidation( aStmtInfo,
                                     ( ( ( aPhraseType == QLV_EXPR_PHRASE_CONDITION ) ||
                                         ( aPhraseType == QLV_EXPR_PHRASE_HAVING ) )
                                      ? STL_TRUE : STL_FALSE ),
                                     aValidationObjList,
                                     aRefTableList,
                                     aSubQuery->mQueryNode,
                                     & sSubQuery->mInitNode,
                                     aEnv)
             == STL_SUCCESS );

    
    /**
     * Target Info 구성
     */

    sInitNode = sSubQuery->mInitNode;
    switch( sInitNode->mType )
    {
        case QLV_NODE_TYPE_QUERY_SET :
            {
                sSubQuery->mTargetCount = ((qlvInitQuerySetNode *) sInitNode)->mSetTargetCount;
                sSubQuery->mTargets     = ((qlvInitQuerySetNode *) sInitNode)->mSetTargets;
                break;
            }
        case QLV_NODE_TYPE_QUERY_SPEC :
            {
                sSubQuery->mTargetCount = ((qlvInitQuerySpecNode *) sInitNode)->mTargetCount;
                sSubQuery->mTargets     = ((qlvInitQuerySpecNode *) sInitNode)->mTargets;
                break;
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    
    /**
     * Target Count Validation
     */
    
    if( aIsRowExpr == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        STL_TRY_THROW( sSubQuery->mTargetCount == 1,
                       RAMP_INVALID_NUMBER_OF_ARGUMENTS );
    }


    /**
     * Sub Query Category 설정
     */
    
    /* parser 에서 row 관련 비교연산은 scalar 비교 연산으로 간주하여 구성한다.
     * KNL_BUILTIN_FUNC_IS_EQUAL_ROW ~ KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL_ROW 은 올 수 없다. */
    STL_DASSERT( ( aRelatedFuncId < KNL_BUILTIN_FUNC_IS_EQUAL_ROW ) ||
                 ( aRelatedFuncId > KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL_ROW ) );
    
    if( ( aRelatedFuncId >= KNL_LIST_FUNC_START_ID ) &&
        ( aRelatedFuncId <= KNL_LIST_FUNC_END_ID ) )
    {
        sCategory = QLV_SUB_QUERY_TYPE_RELATION;
    }
    else
    {
        if( sSubQuery->mTargetCount == 1 )
        {
            sCategory = QLV_SUB_QUERY_TYPE_SCALAR;
        }
        else
        {
            sCategory = QLV_SUB_QUERY_TYPE_ROW;
        }
    }


    /**
     * Target Expression 내에 Sequence 가 있는지 확인
     */

    for( sLoop = 0 ; sLoop < sSubQuery->mTargetCount ; sLoop++ )
    {
        STL_TRY( qlvValidateSequenceInTarget( aStmtInfo->mSQLString,
                                              sSubQuery->mTargets[ sLoop ].mExpr,
                                              aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Related Function Id 설정
     */

    sSubQuery->mRelatedFuncId = aRelatedFuncId;
    
    
    /**
     * Phrase에 따른 Subquery Type 설정
     */

    sSubQuery->mSubQueryType = sSubQueryType;


    /**
     * Phrase에 따른 Subquery Category 설정
     */

    sSubQuery->mCategory = sCategory;


    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );
    
    sExprCode->mType           = QLV_EXPR_TYPE_SUB_QUERY;
    sExprCode->mPosition       = aSubQuery->mNodePos;
    sExprCode->mIncludeExprCnt = 1;
    sExprCode->mExpr.mSubQuery = sSubQuery;
    sExprCode->mRelatedFuncId  = aRelatedFuncId;
    sExprCode->mIterationTime  = sIterTime;
    sExprCode->mColumnName     = NULL;
    sExprCode->mPhraseType     = aPhraseType;

    
    /**
     * OUTPUT 설정
     */

    *aExprCode = sExprCode;

    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_SUB_QUERY_NOT_ALLOWED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SUB_QUERY_NOT_ALLOWED,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aSubQuery->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_INVALID_NUMBER_OF_ARGUMENTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_NUMBER_OF_ARGUMENTS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aSubQuery->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}
    

/**
 * @brief Column Init Plan 에 DB Type 관련 정보를 설정한다.
 * @param[in]     aStmtInfo    Stmt Information
 * @param[in,out] aCodeTypeDef Column Init Plan's Type Definition
 * @param[in]     aColumnType  Column Type Parse Tree
 * @param[in]     aEnv         Environment
 * @remarks
 */
stlStatus qlvSetColumnType( qlvStmtInfo      * aStmtInfo,
                            qlvInitTypeDef   * aCodeTypeDef,
                            qlpTypeName      * aColumnType,
                            qllEnv           * aEnv )
{
    stlInt64  sBigintValue = 0;

    stlInt32  sErrPos = 0;
    
    qlpQualifier * sInterval = NULL;
    dtlIntervalPrimaryField sStartField = DTL_INTERVAL_PRIMARY_NONE;
    dtlIntervalPrimaryField sEndField   = DTL_INTERVAL_PRIMARY_NONE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodeTypeDef != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnType != NULL, QLL_ERROR_STACK(aEnv) );

    
    aCodeTypeDef->mDBType = aColumnType->mDBType;

    switch( aColumnType->mDBType )
    {
        case DTL_TYPE_BOOLEAN:
        case DTL_TYPE_NATIVE_SMALLINT:
        case DTL_TYPE_NATIVE_INTEGER:
        case DTL_TYPE_NATIVE_BIGINT:
        case DTL_TYPE_NATIVE_REAL:
        case DTL_TYPE_NATIVE_DOUBLE:
            {
                aCodeTypeDef->mArgNum1 = 0;
                aCodeTypeDef->mArgNum2 = 0;
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_FLOAT:
            {
                if ( aColumnType->mNumericPrecision == NULL )
                {
                    /* DTL_FLOAT_DEFAULT_PRECISION */
                    aCodeTypeDef->mArgNum1 =
                        gDataTypeDefinition[aColumnType->mDBType].mDefNumericPrec;
                }
                else
                {
                    STL_TRY_THROW(
                        dtlGetIntegerFromString(
                            QLP_GET_PTR_VALUE(aColumnType->mNumericPrecision),
                            stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mNumericPrecision) ),
                            & sBigintValue,
                            QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                        RAMP_ERR_INVALID_FLOAT_PRECISION );

                    STL_TRY_THROW( (sBigintValue >= DTL_FLOAT_MIN_PRECISION) &&
                                   (sBigintValue <= DTL_FLOAT_DEFAULT_PRECISION),
                                   RAMP_ERR_INVALID_FLOAT_PRECISION );
                    
                    aCodeTypeDef->mArgNum1 = sBigintValue;
                }

                /* DTL_SCALE_NA */
                aCodeTypeDef->mArgNum2 = gDataTypeDefinition[aColumnType->mDBType].mDefNumericScale;
                aCodeTypeDef->mNumericRadix = aColumnType->mNumericPrecRadix;  
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_NUMBER:
            {
                /**
                 * Scale이 N/A인 경우,
                 * NUMBER, FLOAT
                 * NUMERIC은 파싱단계에서 Precision이 NULL일 때, 38로 Setting해줌.
                 * 그러므로 NUMBER인 경우에는 Precision이 NULL
                 */

                if ( aColumnType->mNumericPrecision == NULL )
                {
                    /* DTL_NUMERIC_DEFAULT_PRECISION, DTL_SCALE_NA */
                    aCodeTypeDef->mArgNum1 =
                        gDataTypeDefinition[aColumnType->mDBType].mDefNumericPrec;
                    aCodeTypeDef->mArgNum2 =
                        gDataTypeDefinition[aColumnType->mDBType].mDefNumericScale;
                }
                else
                {
                    STL_TRY_THROW(
                        dtlGetIntegerFromString(
                            QLP_GET_PTR_VALUE(aColumnType->mNumericPrecision),
                            stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mNumericPrecision) ),
                            & sBigintValue,
                            QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                        RAMP_ERR_INVALID_NUMERIC_PRECISION );

                    STL_TRY_THROW( (sBigintValue >= DTL_NUMERIC_MIN_PRECISION) &&
                                   (sBigintValue <= DTL_NUMERIC_DEFAULT_PRECISION),
                                   RAMP_ERR_INVALID_NUMERIC_PRECISION );
                    
                    aCodeTypeDef->mArgNum1 = sBigintValue;

                    if ( aColumnType->mNumericScale == NULL )
                    {
                        aCodeTypeDef->mArgNum2 = DTL_NUMERIC_DEFAULT_SCALE;
                    }
                    else
                    {
                        STL_TRY_THROW(
                            dtlGetIntegerFromString(
                                QLP_GET_PTR_VALUE(aColumnType->mNumericScale),
                                stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mNumericScale) ),
                                & sBigintValue,
                                QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                            RAMP_ERR_INVALID_NUMERIC_SCALE );

                        STL_TRY_THROW( (sBigintValue >= DTL_NUMERIC_MIN_SCALE) &&
                                       (sBigintValue <= DTL_NUMERIC_MAX_SCALE),
                                       RAMP_ERR_INVALID_NUMERIC_SCALE );
                    
                        aCodeTypeDef->mArgNum2 = sBigintValue;
                    }
                    
                }
                aCodeTypeDef->mNumericRadix = aColumnType->mNumericPrecRadix;  
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_DECIMAL:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case DTL_TYPE_CHAR:
            {
                if( aColumnType->mStringLength == NULL )
                {
                    sBigintValue = DTL_LENGTH_NA;

                    aCodeTypeDef->mStringLengthUnit =
                        DTL_STRING_LENGTH_UNIT_NA;
                }
                else
                {
                    STL_TRY_THROW(
                        dtlGetIntegerFromString(
                            QLP_GET_PTR_VALUE(aColumnType->mStringLength->mLength),
                            stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mStringLength->mLength) ),
                            & sBigintValue,
                            QLL_ERROR_STACK(aEnv) )
                        == STL_SUCCESS,
                        RAMP_ERR_INVALID_CHAR_LENGTH );

                    STL_TRY_THROW( (sBigintValue >= 1) &&
                                   (sBigintValue <= DTL_CHAR_MAX_PRECISION),
                                   RAMP_ERR_INVALID_CHAR_LENGTH );

                    aCodeTypeDef->mStringLengthUnit =
                        aColumnType->mStringLength->mLengthUnit;
                }

                aCodeTypeDef->mArgNum1 = sBigintValue;
                aCodeTypeDef->mArgNum2 = 0;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                
                break;
            }
        case DTL_TYPE_VARCHAR:
            {
                STL_DASSERT( aColumnType->mStringLength != NULL );
                
                STL_TRY_THROW(
                    dtlGetIntegerFromString(
                        QLP_GET_PTR_VALUE(aColumnType->mStringLength->mLength),
                        stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mStringLength->mLength) ),
                        & sBigintValue,
                        QLL_ERROR_STACK(aEnv) )
                    == STL_SUCCESS,
                    RAMP_ERR_INVALID_VARCHAR_LENGTH );
                
                STL_TRY_THROW( (sBigintValue >= 1) &&
                               (sBigintValue <= DTL_VARCHAR_MAX_PRECISION),
                               RAMP_ERR_INVALID_VARCHAR_LENGTH );
                
                aCodeTypeDef->mStringLengthUnit =
                    aColumnType->mStringLength->mLengthUnit;

                aCodeTypeDef->mArgNum1 = sBigintValue;
                aCodeTypeDef->mArgNum2 = 0;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

                break;
            }
        case DTL_TYPE_LONGVARCHAR:
            {
                STL_DASSERT( aColumnType->mStringLength == NULL );
                
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
                
                aCodeTypeDef->mArgNum1 = DTL_LONGVARCHAR_MAX_PRECISION;
                aCodeTypeDef->mArgNum2 = 0;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

                break;
            }
        case DTL_TYPE_CLOB:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                
                break;
            }
        case DTL_TYPE_BINARY:
            {
                if( aColumnType->mBinaryLength == NULL )
                {
                    sBigintValue = DTL_LENGTH_NA;
                }
                else
                {
                    STL_TRY_THROW(
                        dtlGetIntegerFromString(
                            QLP_GET_PTR_VALUE(aColumnType->mBinaryLength),
                            stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mBinaryLength) ),
                            & sBigintValue,
                            QLL_ERROR_STACK(aEnv) )
                        == STL_SUCCESS,
                        RAMP_ERR_INVALID_BINARY_LENGTH );

                    STL_TRY_THROW( (sBigintValue >= 1) &&
                                   (sBigintValue <= DTL_BINARY_MAX_PRECISION),
                                   RAMP_ERR_INVALID_BINARY_LENGTH );
                }

                aCodeTypeDef->mArgNum1 = sBigintValue;
                aCodeTypeDef->mArgNum2 = 0;
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                
                break;
            }
        case DTL_TYPE_VARBINARY:
            {
                STL_DASSERT( aColumnType->mBinaryLength != NULL );
                
                STL_TRY_THROW(
                    dtlGetIntegerFromString(
                        QLP_GET_PTR_VALUE(aColumnType->mBinaryLength),
                        stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mBinaryLength) ),
                        & sBigintValue,
                        QLL_ERROR_STACK(aEnv) )
                    == STL_SUCCESS,
                    RAMP_ERR_INVALID_VARBINARY_LENGTH );
                
                STL_TRY_THROW( (sBigintValue >= 1) &&
                               (sBigintValue <= DTL_VARBINARY_MAX_PRECISION),
                               RAMP_ERR_INVALID_VARBINARY_LENGTH );
                
                aCodeTypeDef->mArgNum1 = sBigintValue;
                aCodeTypeDef->mArgNum2 = 0;
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                
                break;
            }
        case DTL_TYPE_LONGVARBINARY:
            {
                aCodeTypeDef->mArgNum1 = DTL_LONGVARBINARY_MAX_PRECISION;
                aCodeTypeDef->mArgNum2 = 0;
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                
                break;
            }
        case DTL_TYPE_BLOB:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                
                break;
            }
        case DTL_TYPE_DATE:
            {
                aCodeTypeDef->mArgNum1 = 0;
                aCodeTypeDef->mArgNum2 = 0;
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_TIME:
        case DTL_TYPE_TIMESTAMP:
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                if( aColumnType->mFractionalSecondPrec == NULL )
                {
                    aCodeTypeDef->mArgNum1 = DTL_PRECISION_NA;
                }
                else
                {
                    STL_TRY_THROW(
                        dtlGetIntegerFromString(
                            QLP_GET_PTR_VALUE(aColumnType->mFractionalSecondPrec),
                            stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mFractionalSecondPrec) ),
                            & sBigintValue,
                            QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                        RAMP_ERR_INVALID_FRACTIONAL_PRECISION );

                    STL_TRY_THROW( (sBigintValue >= DTL_TIME_MIN_PRECISION) &&
                                   (sBigintValue <= DTL_TIME_MAX_PRECISION),
                                   RAMP_ERR_INVALID_FRACTIONAL_PRECISION );
                    
                    aCodeTypeDef->mArgNum1 = sBigintValue;
                }

                aCodeTypeDef->mArgNum2 = 0;
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            {
                sInterval = (qlpQualifier *) aColumnType->mQualifier;
                STL_DASSERT( sInterval->mType == QLL_QUALIFIER_TYPE );

                if ( sInterval->mSingleFieldDesc != NULL )
                {
                    /**
                     * Single Field INTERVAL 인 경우
                     */
                    
                    sStartField =
                        QLP_GET_INT_VALUE(sInterval->mSingleFieldDesc->mPrimaryFieldType);

                    switch ( sStartField )
                    {
                        case DTL_INTERVAL_PRIMARY_YEAR:
                            aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_YEAR;
                            break;
                        case DTL_INTERVAL_PRIMARY_MONTH:
                            aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_MONTH;
                            break;
                        default:
                            STL_DASSERT( 1 == 0 );
                            break;
                    }

                    /**
                     * Leading Precision
                     */
                    
                    if( sInterval->mSingleFieldDesc->mLeadingFieldPrecision == NULL )
                    {
                        aCodeTypeDef->mArgNum1 = DTL_PRECISION_NA;
                    }
                    else
                    {
                        sErrPos =
                            sInterval->mSingleFieldDesc->mLeadingFieldPrecision->mNodePos;
                        
                        STL_TRY_THROW(
                            dtlGetIntegerFromString(
                                QLP_GET_PTR_VALUE(sInterval->mSingleFieldDesc->mLeadingFieldPrecision),
                                stlStrlen( QLP_GET_PTR_VALUE(sInterval->mSingleFieldDesc->mLeadingFieldPrecision) ),
                                & sBigintValue,
                                QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        STL_TRY_THROW(
                            (sBigintValue >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                            (sBigintValue <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        aCodeTypeDef->mArgNum1 = sBigintValue;
                    }

                    /**
                     * Fractional Second Precision
                     */
                    
                    aCodeTypeDef->mArgNum2 = DTL_PRECISION_NA;
                }
                else
                {
                    /**
                     * Double Field INTERVAL 인 경우
                     */

                    sStartField =
                        QLP_GET_INT_VALUE(sInterval->mStartFieldDesc->mPrimaryFieldType);
                    sEndField =
                        QLP_GET_INT_VALUE(sInterval->mEndFieldDesc->mPrimaryFieldType);

                    switch ( sStartField )
                    {
                        case DTL_INTERVAL_PRIMARY_YEAR:
                            {
                                switch( sEndField )
                                {
                                    case DTL_INTERVAL_PRIMARY_YEAR:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                    case DTL_INTERVAL_PRIMARY_MONTH:
                                        aCodeTypeDef->mIntervalIndicator
                                            = DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH;
                                        break;
                                    case DTL_INTERVAL_PRIMARY_DAY:
                                    case DTL_INTERVAL_PRIMARY_HOUR:
                                    case DTL_INTERVAL_PRIMARY_MINUTE:
                                    case DTL_INTERVAL_PRIMARY_SECOND:
                                    default:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                }
                                break;
                            }        
                        default:
                            STL_DASSERT( 1 == 0 );
                            break;
                    }

                    /**
                     * Leading Precision
                     */
                    
                    if( sInterval->mStartFieldDesc->mLeadingFieldPrecision == NULL )
                    {
                        aCodeTypeDef->mArgNum1 = DTL_PRECISION_NA;
                    }
                    else
                    {
                        sErrPos =
                            sInterval->mStartFieldDesc->mLeadingFieldPrecision->mNodePos;
                        
                        STL_TRY_THROW(
                            dtlGetIntegerFromString(
                                QLP_GET_PTR_VALUE(sInterval->mStartFieldDesc->mLeadingFieldPrecision),
                                stlStrlen( QLP_GET_PTR_VALUE(sInterval->mStartFieldDesc->mLeadingFieldPrecision) ),
                                & sBigintValue,
                                QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        STL_TRY_THROW(
                            (sBigintValue >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                            (sBigintValue <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        aCodeTypeDef->mArgNum1 = sBigintValue;
                    }

                    /**
                     * Fractional Second Precision
                     */

                    aCodeTypeDef->mArgNum2 = DTL_PRECISION_NA;
                }
                
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                sInterval = (qlpQualifier *) aColumnType->mQualifier;
                STL_DASSERT( sInterval->mType == QLL_QUALIFIER_TYPE );

                if ( sInterval->mSingleFieldDesc != NULL )
                {
                    /**
                     * Single Field INTERVAL 인 경우
                     */
                    
                    sStartField =
                        QLP_GET_INT_VALUE(sInterval->mSingleFieldDesc->mPrimaryFieldType);

                    switch ( sStartField )
                    {
                        case DTL_INTERVAL_PRIMARY_DAY:
                            aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_DAY;
                            break;
                        case DTL_INTERVAL_PRIMARY_HOUR:
                            aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_HOUR;
                            break;
                        case DTL_INTERVAL_PRIMARY_MINUTE:
                            aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_MINUTE;
                            break;
                        case DTL_INTERVAL_PRIMARY_SECOND:
                            aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_SECOND;
                            break;
                        default:
                            STL_DASSERT( 1 == 0 );
                            break;
                    }

                    /**
                     * Leading Precision
                     */
                    
                    if( sInterval->mSingleFieldDesc->mLeadingFieldPrecision == NULL )
                    {
                        aCodeTypeDef->mArgNum1 = DTL_PRECISION_NA;
                    }
                    else
                    {
                        sErrPos =
                            sInterval->mSingleFieldDesc->mLeadingFieldPrecision->mNodePos;
                        
                        STL_TRY_THROW(
                            dtlGetIntegerFromString(
                                QLP_GET_PTR_VALUE(sInterval->mSingleFieldDesc->mLeadingFieldPrecision),
                                stlStrlen( QLP_GET_PTR_VALUE(sInterval->mSingleFieldDesc->mLeadingFieldPrecision) ),
                                & sBigintValue,
                                QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        STL_TRY_THROW(
                            (sBigintValue >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                            (sBigintValue <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        aCodeTypeDef->mArgNum1 = sBigintValue;
                    }

                    /**
                     * Fractional Second Precision
                     */

                    if( sInterval->mSingleFieldDesc->mFractionalSecondsPrecision == NULL )
                    {
                        aCodeTypeDef->mArgNum2 = DTL_PRECISION_NA;
                    }
                    else
                    {
                        STL_DASSERT( sStartField == DTL_INTERVAL_PRIMARY_SECOND );

                        sErrPos =
                            sInterval->mSingleFieldDesc->mFractionalSecondsPrecision->mNodePos;
                        
                        STL_TRY_THROW(
                            dtlGetIntegerFromString(
                                QLP_GET_PTR_VALUE(sInterval->mSingleFieldDesc->mFractionalSecondsPrecision),
                                stlStrlen( QLP_GET_PTR_VALUE(sInterval->mSingleFieldDesc->mFractionalSecondsPrecision) ),
                                & sBigintValue,
                                QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                            RAMP_ERR_INVALID_INTERVAL_FRACTIONAL_SECOND_PRECISION );
                        
                        STL_TRY_THROW(
                            (sBigintValue >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION)
                            &&
                            (sBigintValue <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION),
                            RAMP_ERR_INVALID_INTERVAL_FRACTIONAL_SECOND_PRECISION );
                        
                        aCodeTypeDef->mArgNum2 = sBigintValue;
                    }
                    
                }
                else
                {
                    /**
                     * Double Field INTERVAL 인 경우
                     */

                    sStartField =
                        QLP_GET_INT_VALUE(sInterval->mStartFieldDesc->mPrimaryFieldType);
                    sEndField =
                        QLP_GET_INT_VALUE(sInterval->mEndFieldDesc->mPrimaryFieldType);

                    switch ( sStartField )
                    {
                        case DTL_INTERVAL_PRIMARY_DAY:
                            {
                                switch( sEndField )
                                {
                                    case DTL_INTERVAL_PRIMARY_YEAR:
                                    case DTL_INTERVAL_PRIMARY_MONTH:
                                    case DTL_INTERVAL_PRIMARY_DAY:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                    case DTL_INTERVAL_PRIMARY_HOUR:
                                        aCodeTypeDef->mIntervalIndicator
                                            = DTL_INTERVAL_INDICATOR_DAY_TO_HOUR;
                                        break;
                                    case DTL_INTERVAL_PRIMARY_MINUTE:
                                        aCodeTypeDef->mIntervalIndicator
                                            = DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE;
                                        break;
                                    case DTL_INTERVAL_PRIMARY_SECOND:
                                        aCodeTypeDef->mIntervalIndicator
                                            = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
                                        break;
                                    default:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                }
                                break;
                            }        
                        case DTL_INTERVAL_PRIMARY_HOUR:
                            {
                                switch( sEndField )
                                {
                                    case DTL_INTERVAL_PRIMARY_YEAR:
                                    case DTL_INTERVAL_PRIMARY_MONTH:
                                    case DTL_INTERVAL_PRIMARY_DAY:
                                    case DTL_INTERVAL_PRIMARY_HOUR:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                    case DTL_INTERVAL_PRIMARY_MINUTE:
                                        aCodeTypeDef->mIntervalIndicator
                                            = DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE;
                                        break;
                                    case DTL_INTERVAL_PRIMARY_SECOND:
                                        aCodeTypeDef->mIntervalIndicator
                                            = DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND;
                                        break;
                                    default:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                }
                                break;
                            }        
                        case DTL_INTERVAL_PRIMARY_MINUTE:
                            {
                                switch( sEndField )
                                {
                                    case DTL_INTERVAL_PRIMARY_YEAR:
                                    case DTL_INTERVAL_PRIMARY_MONTH:
                                    case DTL_INTERVAL_PRIMARY_DAY:
                                    case DTL_INTERVAL_PRIMARY_HOUR:
                                    case DTL_INTERVAL_PRIMARY_MINUTE:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                    case DTL_INTERVAL_PRIMARY_SECOND:
                                        aCodeTypeDef->mIntervalIndicator
                                            = DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND;
                                        break;
                                    default:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                }
                                break;
                            }        
                        case DTL_INTERVAL_PRIMARY_SECOND:
                            {
                                STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                break;
                            }
                        default:
                            STL_DASSERT( 1 == 0 );
                            break;
                    }

                    /**
                     * Leading Precision
                     */
                    
                    if( sInterval->mStartFieldDesc->mLeadingFieldPrecision == NULL )
                    {
                        aCodeTypeDef->mArgNum1 = DTL_PRECISION_NA;
                    }
                    else
                    {
                        sErrPos =
                            sInterval->mStartFieldDesc->mLeadingFieldPrecision->mNodePos;
                        
                        STL_TRY_THROW(
                            dtlGetIntegerFromString(
                                QLP_GET_PTR_VALUE(sInterval->mStartFieldDesc->mLeadingFieldPrecision),
                                stlStrlen( QLP_GET_PTR_VALUE(sInterval->mStartFieldDesc->mLeadingFieldPrecision) ),
                                & sBigintValue,
                                QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        STL_TRY_THROW(
                            (sBigintValue >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                            (sBigintValue <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        aCodeTypeDef->mArgNum1 = sBigintValue;
                    }

                    /**
                     * Fractional Second Precision
                     */

                    if( sInterval->mEndFieldDesc->mFractionalSecondsPrecision == NULL )
                    {
                        aCodeTypeDef->mArgNum2 = DTL_PRECISION_NA;
                    }
                    else
                    {
                        STL_DASSERT( sEndField == DTL_INTERVAL_PRIMARY_SECOND );

                        sErrPos =
                            sInterval->mEndFieldDesc->mFractionalSecondsPrecision->mNodePos;
                        
                        STL_TRY_THROW(
                            dtlGetIntegerFromString(
                                QLP_GET_PTR_VALUE(sInterval->mEndFieldDesc->mFractionalSecondsPrecision),
                                stlStrlen( QLP_GET_PTR_VALUE(sInterval->mEndFieldDesc->mFractionalSecondsPrecision) ),
                                & sBigintValue,
                                QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                            RAMP_ERR_INVALID_INTERVAL_FRACTIONAL_SECOND_PRECISION );
                        
                        STL_TRY_THROW(
                            (sBigintValue >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION)
                            &&
                            (sBigintValue <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION),
                            RAMP_ERR_INVALID_INTERVAL_FRACTIONAL_SECOND_PRECISION );
                        
                        aCodeTypeDef->mArgNum2 = sBigintValue;
                    }
                    
                }
                
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_ROWID:
            {
                aCodeTypeDef->mArgNum1 = gDefaultDataTypeDef[DTL_TYPE_ROWID].mArgNum1;
                aCodeTypeDef->mArgNum2 = gDefaultDataTypeDef[DTL_TYPE_ROWID].mArgNum2;
                aCodeTypeDef->mStringLengthUnit = gDefaultDataTypeDef[DTL_TYPE_ROWID].mStringLengthUnit;
                aCodeTypeDef->mIntervalIndicator = gDefaultDataTypeDef[DTL_TYPE_ROWID].mIntervalIndicator;

                break;
            }
        default:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
    }


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_NUMERIC_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NUMERIC_PRECISION_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mNumericPrecision->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_NUMERIC_MIN_PRECISION,
                      DTL_NUMERIC_DEFAULT_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_NUMERIC_SCALE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NUMERIC_SCALE_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mNumericScale->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_NUMERIC_MIN_SCALE,
                      DTL_NUMERIC_MAX_SCALE );
    }
    
    STL_CATCH( RAMP_ERR_INVALID_FLOAT_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_FLOAT_PRECISION_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mNumericPrecision->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_FLOAT_MIN_PRECISION,
                      DTL_FLOAT_DEFAULT_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_CHAR_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CHARACTER_LENGTH_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mStringLength->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_CHAR_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_VARCHAR_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CHARACTER_VARYING_LENGTH_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mStringLength->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_VARCHAR_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_BINARY_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BINARY_LENGTH_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mBinaryLength->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_BINARY_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_VARBINARY_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BINARY_VARYING_LENGTH_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mBinaryLength->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_VARBINARY_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_FRACTIONAL_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_FRACTIONAL_SECOND_PRECISION_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mFractionalSecondPrec->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_TIME_MIN_PRECISION,
                      DTL_TIME_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_INTERVAL_INDICATOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_INTERVAL_INDICATOR,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sInterval->mEndFieldDesc->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_INTERVAL_LEADING_PRECISION,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sErrPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,
                      DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_INTERVAL_FRACTIONAL_SECOND_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_INTERVAL_FRACTIONAL_SECONDS_PRECISION,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sErrPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION,
                      DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "qlvSetColumnType()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Column Init Plan 에 DB Type 관련 정보를 설정한다.
 * @param[in]     aStmtInfo    Stmt Information
 * @param[in,out] aCodeTypeDef Column Init Plan's Type Definition
 * @param[in]     aColumnType  Column Type Parse Tree
 * @param[in]     aEnv         Environment
 * @remarks
 */
stlStatus qlvSetColumnTypeForCast( qlvStmtInfo      * aStmtInfo,
                                   qlvInitTypeDef   * aCodeTypeDef,
                                   qlpTypeName      * aColumnType,
                                   qllEnv           * aEnv )
{
    stlInt64  sBigintValue = 0;

    stlInt32  sErrPos = 0;
    
    qlpQualifier * sInterval = NULL;
    dtlIntervalPrimaryField sStartField = DTL_INTERVAL_PRIMARY_NONE;
    dtlIntervalPrimaryField sEndField   = DTL_INTERVAL_PRIMARY_NONE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodeTypeDef != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnType != NULL, QLL_ERROR_STACK(aEnv) );

    
    aCodeTypeDef->mDBType = aColumnType->mDBType;

    switch( aColumnType->mDBType )
    {
        case DTL_TYPE_BOOLEAN:
        case DTL_TYPE_NATIVE_SMALLINT:
        case DTL_TYPE_NATIVE_INTEGER:
        case DTL_TYPE_NATIVE_BIGINT:
        case DTL_TYPE_NATIVE_REAL:
        case DTL_TYPE_NATIVE_DOUBLE:
            {
                aCodeTypeDef->mArgNum1 = gDataTypeDefinition[aColumnType->mDBType].mDefNumericPrec;
                aCodeTypeDef->mArgNum2 = DTL_SCALE_NA;
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_FLOAT:
            {
                if ( aColumnType->mNumericPrecision == NULL )
                {
                    /* DTL_FLOAT_DEFAULT_PRECISION */
                    aCodeTypeDef->mArgNum1 = 
                        gDataTypeDefinition[aColumnType->mDBType].mDefNumericPrec;
                }
                else
                {
                    STL_TRY_THROW(
                        dtlGetIntegerFromString(
                            QLP_GET_PTR_VALUE(aColumnType->mNumericPrecision),
                            stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mNumericPrecision) ),
                            & sBigintValue,
                            QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                        RAMP_ERR_INVALID_FLOAT_PRECISION );

                    STL_TRY_THROW( (sBigintValue >= DTL_FLOAT_MIN_PRECISION) &&
                                   (sBigintValue <= DTL_FLOAT_DEFAULT_PRECISION),
                                   RAMP_ERR_INVALID_FLOAT_PRECISION );
                    
                    aCodeTypeDef->mArgNum1 = sBigintValue;
                }
                aCodeTypeDef->mArgNum2 = gDataTypeDefinition[aColumnType->mDBType].mDefNumericScale;
                aCodeTypeDef->mNumericRadix = aColumnType->mNumericPrecRadix;
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_NUMBER:
            {
                /**
                 * Scale이 N/A인 경우,
                 * NUMBER, FLOAT
                 * NUMERIC은 파싱단계에서 Precision이 NULL일 때, 38로 Setting해줌.
                 * 그러므로 NUMBER인 경우에는 Precision이 NULL
                 */

                if ( aColumnType->mNumericPrecision == NULL )
                {
                    /* DTL_NUMERIC_DEFAULT_PRECISION, DTL_SCALE_NA */
                    aCodeTypeDef->mArgNum1 =
                        gDataTypeDefinition[aColumnType->mDBType].mDefNumericPrec;
                    aCodeTypeDef->mArgNum2 =
                        gDataTypeDefinition[aColumnType->mDBType].mDefNumericScale;
                }
                else
                {
                    STL_TRY_THROW(
                        dtlGetIntegerFromString(
                            QLP_GET_PTR_VALUE(aColumnType->mNumericPrecision),
                            stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mNumericPrecision) ),
                            & sBigintValue,
                            QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                        RAMP_ERR_INVALID_NUMERIC_PRECISION );

                    STL_TRY_THROW( (sBigintValue >= DTL_NUMERIC_MIN_PRECISION) &&
                                   (sBigintValue <= DTL_NUMERIC_DEFAULT_PRECISION),
                                   RAMP_ERR_INVALID_NUMERIC_PRECISION );
                    
                    aCodeTypeDef->mArgNum1 = sBigintValue;
                    
                    if ( aColumnType->mNumericScale == NULL )
                    {
                        aCodeTypeDef->mArgNum2 = DTL_NUMERIC_DEFAULT_SCALE;
                    }
                    else
                    {
                        STL_TRY_THROW(
                            dtlGetIntegerFromString(
                                QLP_GET_PTR_VALUE(aColumnType->mNumericScale),
                                stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mNumericScale) ),
                                & sBigintValue,
                                QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                            RAMP_ERR_INVALID_NUMERIC_SCALE );

                        STL_TRY_THROW( (sBigintValue >= DTL_NUMERIC_MIN_SCALE) &&
                                       (sBigintValue <= DTL_NUMERIC_MAX_SCALE),
                                       RAMP_ERR_INVALID_NUMERIC_SCALE );
                    
                        aCodeTypeDef->mArgNum2 = sBigintValue;
                    }
                }
                aCodeTypeDef->mNumericRadix = aColumnType->mNumericPrecRadix;
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_DECIMAL:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case DTL_TYPE_CHAR:
            {
                if( aColumnType->mStringLength == NULL )
                {
                    sBigintValue = gDataTypeDefinition[aColumnType->mDBType].mDefStringLength;

                    aCodeTypeDef->mStringLengthUnit = ellGetDbCharLengthUnit();
                }
                else
                {
                    STL_TRY_THROW(
                        dtlGetIntegerFromString(
                            QLP_GET_PTR_VALUE(aColumnType->mStringLength->mLength),
                            stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mStringLength->mLength) ),
                            & sBigintValue,
                            QLL_ERROR_STACK(aEnv) )
                        == STL_SUCCESS,
                        RAMP_ERR_INVALID_CHAR_LENGTH );

                    STL_TRY_THROW( (sBigintValue >= 1) &&
                                   (sBigintValue <= DTL_CHAR_MAX_PRECISION),
                                   RAMP_ERR_INVALID_CHAR_LENGTH );

                    if( aColumnType->mStringLength->mLengthUnit == DTL_STRING_LENGTH_UNIT_NA )
                    {
                        aCodeTypeDef->mStringLengthUnit = ellGetDbCharLengthUnit();
                    }
                    else
                    {
                        aCodeTypeDef->mStringLengthUnit =
                            aColumnType->mStringLength->mLengthUnit;
                    }
                }

                aCodeTypeDef->mArgNum1 = sBigintValue;
                aCodeTypeDef->mArgNum2 = 0;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                
                break;
            }
        case DTL_TYPE_VARCHAR:
            {
                STL_DASSERT( aColumnType->mStringLength != NULL );
                
                STL_TRY_THROW(
                    dtlGetIntegerFromString(
                        QLP_GET_PTR_VALUE(aColumnType->mStringLength->mLength),
                        stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mStringLength->mLength) ),
                        & sBigintValue,
                        QLL_ERROR_STACK(aEnv) )
                    == STL_SUCCESS,
                    RAMP_ERR_INVALID_VARCHAR_LENGTH );
                
                STL_TRY_THROW( (sBigintValue >= 1) &&
                               (sBigintValue <= DTL_VARCHAR_MAX_PRECISION),
                               RAMP_ERR_INVALID_VARCHAR_LENGTH );
                
                aCodeTypeDef->mArgNum1 = sBigintValue;
                aCodeTypeDef->mArgNum2 = 0;
                
                if( aColumnType->mStringLength->mLengthUnit == DTL_STRING_LENGTH_UNIT_NA )
                {
                    aCodeTypeDef->mStringLengthUnit = ellGetDbCharLengthUnit();
                }
                else
                {
                    aCodeTypeDef->mStringLengthUnit =
                        aColumnType->mStringLength->mLengthUnit;
                }
                
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                
                break;
            }
        case DTL_TYPE_LONGVARCHAR:
            {
                aCodeTypeDef->mArgNum1 = DTL_LONGVARCHAR_MAX_PRECISION;
                aCodeTypeDef->mArgNum2 = 0;
                
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                
                break;
            }
        case DTL_TYPE_CLOB:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                
                break;
            }
        case DTL_TYPE_BINARY:
            {
                if( aColumnType->mBinaryLength == NULL )
                {
                    sBigintValue = gDefaultDataTypeDef[aColumnType->mDBType].mArgNum1;
                }
                else
                {
                    STL_TRY_THROW(
                        dtlGetIntegerFromString(
                            QLP_GET_PTR_VALUE(aColumnType->mBinaryLength),
                            stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mBinaryLength) ),
                            & sBigintValue,
                            QLL_ERROR_STACK(aEnv) )
                        == STL_SUCCESS,
                        RAMP_ERR_INVALID_BINARY_LENGTH );
                }

                STL_TRY_THROW( (sBigintValue >= 1) &&
                               (sBigintValue <= DTL_BINARY_MAX_PRECISION),
                               RAMP_ERR_INVALID_BINARY_LENGTH );

                aCodeTypeDef->mArgNum1 = sBigintValue;
                aCodeTypeDef->mArgNum2 = 0;
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                
                break;
            }
        case DTL_TYPE_VARBINARY:
            {
                STL_DASSERT( aColumnType->mBinaryLength != NULL );

                STL_TRY_THROW(
                    dtlGetIntegerFromString(
                        QLP_GET_PTR_VALUE(aColumnType->mBinaryLength),
                        stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mBinaryLength) ),
                        & sBigintValue,
                        QLL_ERROR_STACK(aEnv) )
                    == STL_SUCCESS,
                    RAMP_ERR_INVALID_VARBINARY_LENGTH );

                STL_TRY_THROW( (sBigintValue >= 1) &&
                               (sBigintValue <= DTL_VARBINARY_MAX_PRECISION),
                               RAMP_ERR_INVALID_VARBINARY_LENGTH );
                
                aCodeTypeDef->mArgNum1 = sBigintValue;
                aCodeTypeDef->mArgNum2 = 0;
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                
                break;
            }
        case DTL_TYPE_LONGVARBINARY:
            {
                aCodeTypeDef->mArgNum1 = DTL_LONGVARBINARY_MAX_PRECISION;
                aCodeTypeDef->mArgNum2 = 0;
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                
                break;
            }
        case DTL_TYPE_BLOB:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                
                break;
            }
        case DTL_TYPE_DATE:
            {
                aCodeTypeDef->mArgNum1 = 0;
                aCodeTypeDef->mArgNum2 = 0;
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_TIME:
        case DTL_TYPE_TIMESTAMP:
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                if( aColumnType->mFractionalSecondPrec == NULL )
                {
                    aCodeTypeDef->mArgNum1 =
                        gDefaultDataTypeDef[aColumnType->mDBType].mArgNum1;
                }
                else
                {
                    STL_TRY_THROW(
                        dtlGetIntegerFromString(
                            QLP_GET_PTR_VALUE(aColumnType->mFractionalSecondPrec),
                            stlStrlen( QLP_GET_PTR_VALUE(aColumnType->mFractionalSecondPrec) ),
                            & sBigintValue,
                            QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                        RAMP_ERR_INVALID_FRACTIONAL_PRECISION );

                    STL_TRY_THROW( (sBigintValue >= DTL_TIME_MIN_PRECISION) &&
                                   (sBigintValue <= DTL_TIME_MAX_PRECISION),
                                   RAMP_ERR_INVALID_FRACTIONAL_PRECISION );
                    
                    aCodeTypeDef->mArgNum1 = sBigintValue;
                }

                aCodeTypeDef->mArgNum2 = 0;
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                break;
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            {
                sInterval = (qlpQualifier *) aColumnType->mQualifier;
                STL_DASSERT( sInterval->mType == QLL_QUALIFIER_TYPE );

                if ( sInterval->mSingleFieldDesc != NULL )
                {
                    /**
                     * Single Field INTERVAL 인 경우
                     */
                    
                    sStartField =
                        QLP_GET_INT_VALUE(sInterval->mSingleFieldDesc->mPrimaryFieldType);

                    switch ( sStartField )
                    {
                        case DTL_INTERVAL_PRIMARY_YEAR:
                            aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_YEAR;
                            break;
                        case DTL_INTERVAL_PRIMARY_MONTH:
                            aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_MONTH;
                            break;
                        default:
                            STL_DASSERT( 1 == 0 );
                            break;
                    }

                    /**
                     * Leading Precision
                     */
                    
                    if( sInterval->mSingleFieldDesc->mLeadingFieldPrecision == NULL )
                    {
                        aCodeTypeDef->mArgNum1 =
                            gDataTypeDefinition[aColumnType->mDBType].mDefIntervalPrec;
                    }
                    else
                    {
                        sErrPos =
                            sInterval->mSingleFieldDesc->mLeadingFieldPrecision->mNodePos;
                        
                        STL_TRY_THROW(
                            dtlGetIntegerFromString(
                                QLP_GET_PTR_VALUE(sInterval->mSingleFieldDesc->mLeadingFieldPrecision),
                                stlStrlen( QLP_GET_PTR_VALUE(sInterval->mSingleFieldDesc->mLeadingFieldPrecision) ),
                                & sBigintValue,
                                QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        STL_TRY_THROW(
                            (sBigintValue >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                            (sBigintValue <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        aCodeTypeDef->mArgNum1 = sBigintValue;
                    }

                    /**
                     * Fractional Second Precision
                     */
                    
                    aCodeTypeDef->mArgNum2 = DTL_PRECISION_NA;
                }
                else
                {
                    /**
                     * Double Field INTERVAL 인 경우
                     */

                    sStartField =
                        QLP_GET_INT_VALUE(sInterval->mStartFieldDesc->mPrimaryFieldType);
                    sEndField =
                        QLP_GET_INT_VALUE(sInterval->mEndFieldDesc->mPrimaryFieldType);

                    switch ( sStartField )
                    {
                        case DTL_INTERVAL_PRIMARY_YEAR:
                            {
                                switch( sEndField )
                                {
                                    case DTL_INTERVAL_PRIMARY_YEAR:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                    case DTL_INTERVAL_PRIMARY_MONTH:
                                        aCodeTypeDef->mIntervalIndicator
                                            = DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH;
                                        break;
                                    case DTL_INTERVAL_PRIMARY_DAY:
                                    case DTL_INTERVAL_PRIMARY_HOUR:
                                    case DTL_INTERVAL_PRIMARY_MINUTE:
                                    case DTL_INTERVAL_PRIMARY_SECOND:
                                    default:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                }
                                break;
                            }        
                        case DTL_INTERVAL_PRIMARY_MONTH:
                            {
                                STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                break;
                            }        
                        default:
                            STL_DASSERT( 1 == 0 );
                            break;
                    }

                    /**
                     * Leading Precision
                     */
                    
                    if( sInterval->mStartFieldDesc->mLeadingFieldPrecision == NULL )
                    {
                        aCodeTypeDef->mArgNum1 =
                            gDataTypeDefinition[aColumnType->mDBType].mDefIntervalPrec;
                    }
                    else
                    {
                        sErrPos =
                            sInterval->mStartFieldDesc->mLeadingFieldPrecision->mNodePos;
                        
                        STL_TRY_THROW(
                            dtlGetIntegerFromString(
                                QLP_GET_PTR_VALUE(sInterval->mStartFieldDesc->mLeadingFieldPrecision),
                                stlStrlen( QLP_GET_PTR_VALUE(sInterval->mStartFieldDesc->mLeadingFieldPrecision) ),
                                & sBigintValue,
                                QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        STL_TRY_THROW(
                            (sBigintValue >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                            (sBigintValue <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        aCodeTypeDef->mArgNum1 = sBigintValue;
                    }

                    /**
                     * Fractional Second Precision
                     */

                    aCodeTypeDef->mArgNum2 = DTL_PRECISION_NA;
                }
                
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                sInterval = (qlpQualifier *) aColumnType->mQualifier;
                STL_DASSERT( sInterval->mType == QLL_QUALIFIER_TYPE );

                if ( sInterval->mSingleFieldDesc != NULL )
                {
                    /**
                     * Single Field INTERVAL 인 경우
                     */
                    
                    sStartField =
                        QLP_GET_INT_VALUE(sInterval->mSingleFieldDesc->mPrimaryFieldType);

                    switch ( sStartField )
                    {
                        case DTL_INTERVAL_PRIMARY_DAY:
                            aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_DAY;
                            break;
                        case DTL_INTERVAL_PRIMARY_HOUR:
                            aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_HOUR;
                            break;
                        case DTL_INTERVAL_PRIMARY_MINUTE:
                            aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_MINUTE;
                            break;
                        case DTL_INTERVAL_PRIMARY_SECOND:
                            aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_SECOND;
                            break;
                        default:
                            STL_DASSERT( 1 == 0 );
                            break;
                    }

                    /**
                     * Leading Precision
                     */
                    
                    if( sInterval->mSingleFieldDesc->mLeadingFieldPrecision == NULL )
                    {
                        aCodeTypeDef->mArgNum1 =
                            gDataTypeDefinition[aColumnType->mDBType].mDefIntervalPrec;
                    }
                    else
                    {
                        sErrPos =
                            sInterval->mSingleFieldDesc->mLeadingFieldPrecision->mNodePos;
                        
                        STL_TRY_THROW(
                            dtlGetIntegerFromString(
                                QLP_GET_PTR_VALUE(sInterval->mSingleFieldDesc->mLeadingFieldPrecision),
                                stlStrlen( QLP_GET_PTR_VALUE(sInterval->mSingleFieldDesc->mLeadingFieldPrecision) ),
                                & sBigintValue,
                                QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        STL_TRY_THROW(
                            (sBigintValue >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                            (sBigintValue <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        aCodeTypeDef->mArgNum1 = sBigintValue;
                    }

                    /**
                     * Fractional Second Precision
                     */

                    if( sInterval->mSingleFieldDesc->mFractionalSecondsPrecision == NULL )
                    {
                        aCodeTypeDef->mArgNum2 =
                            gDataTypeDefinition[aColumnType->mDBType].mDefFractionalSecondPrec;
                    }
                    else
                    {
                        STL_DASSERT( sStartField == DTL_INTERVAL_PRIMARY_SECOND );

                        sErrPos =
                            sInterval->mSingleFieldDesc->mFractionalSecondsPrecision->mNodePos;
                        
                        STL_TRY_THROW(
                            dtlGetIntegerFromString(
                                QLP_GET_PTR_VALUE(sInterval->mSingleFieldDesc->mFractionalSecondsPrecision),
                                stlStrlen( QLP_GET_PTR_VALUE(sInterval->mSingleFieldDesc->mFractionalSecondsPrecision) ),
                                & sBigintValue,
                                QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                            RAMP_ERR_INVALID_INTERVAL_FRACTIONAL_SECOND_PRECISION );
                        
                        STL_TRY_THROW(
                            (sBigintValue >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION)
                            &&
                            (sBigintValue <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION),
                            RAMP_ERR_INVALID_INTERVAL_FRACTIONAL_SECOND_PRECISION );
                        
                        aCodeTypeDef->mArgNum2 = sBigintValue;
                    }
                    
                }
                else
                {
                    /**
                     * Double Field INTERVAL 인 경우
                     */

                    sStartField =
                        QLP_GET_INT_VALUE(sInterval->mStartFieldDesc->mPrimaryFieldType);
                    sEndField =
                        QLP_GET_INT_VALUE(sInterval->mEndFieldDesc->mPrimaryFieldType);

                    switch ( sStartField )
                    {
                        case DTL_INTERVAL_PRIMARY_DAY:
                            {
                                switch( sEndField )
                                {
                                    case DTL_INTERVAL_PRIMARY_YEAR:
                                    case DTL_INTERVAL_PRIMARY_MONTH:
                                    case DTL_INTERVAL_PRIMARY_DAY:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                    case DTL_INTERVAL_PRIMARY_HOUR:
                                        aCodeTypeDef->mIntervalIndicator
                                            = DTL_INTERVAL_INDICATOR_DAY_TO_HOUR;
                                        break;
                                    case DTL_INTERVAL_PRIMARY_MINUTE:
                                        aCodeTypeDef->mIntervalIndicator
                                            = DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE;
                                        break;
                                    case DTL_INTERVAL_PRIMARY_SECOND:
                                        aCodeTypeDef->mIntervalIndicator
                                            = DTL_INTERVAL_INDICATOR_DAY_TO_SECOND;
                                        break;
                                    default:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                }
                                break;
                            }        
                        case DTL_INTERVAL_PRIMARY_HOUR:
                            {
                                switch( sEndField )
                                {
                                    case DTL_INTERVAL_PRIMARY_YEAR:
                                    case DTL_INTERVAL_PRIMARY_MONTH:
                                    case DTL_INTERVAL_PRIMARY_DAY:
                                    case DTL_INTERVAL_PRIMARY_HOUR:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                    case DTL_INTERVAL_PRIMARY_MINUTE:
                                        aCodeTypeDef->mIntervalIndicator
                                            = DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE;
                                        break;
                                    case DTL_INTERVAL_PRIMARY_SECOND:
                                        aCodeTypeDef->mIntervalIndicator
                                            = DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND;
                                        break;
                                    default:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                }
                                break;
                            }        
                        case DTL_INTERVAL_PRIMARY_MINUTE:
                            {
                                switch( sEndField )
                                {
                                    case DTL_INTERVAL_PRIMARY_YEAR:
                                    case DTL_INTERVAL_PRIMARY_MONTH:
                                    case DTL_INTERVAL_PRIMARY_DAY:
                                    case DTL_INTERVAL_PRIMARY_HOUR:
                                    case DTL_INTERVAL_PRIMARY_MINUTE:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                    case DTL_INTERVAL_PRIMARY_SECOND:
                                        aCodeTypeDef->mIntervalIndicator
                                            = DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND;
                                        break;
                                    default:
                                        STL_THROW( RAMP_ERR_INVALID_INTERVAL_INDICATOR );
                                        break;
                                }
                                break;
                            }        
                        case DTL_INTERVAL_PRIMARY_SECOND:
                            {
                                /* parser에 의해 올 수 없음 */
                                STL_DASSERT( 0) ;
                                break;
                            }
                        default:
                            STL_DASSERT( 1 == 0 );
                            break;
                    }

                    /**
                     * Leading Precision
                     */
                    
                    if( sInterval->mStartFieldDesc->mLeadingFieldPrecision == NULL )
                    {
                        aCodeTypeDef->mArgNum1 =
                            gDataTypeDefinition[aColumnType->mDBType].mDefIntervalPrec;
                    }
                    else
                    {
                        sErrPos =
                            sInterval->mStartFieldDesc->mLeadingFieldPrecision->mNodePos;
                        
                        STL_TRY_THROW(
                            dtlGetIntegerFromString(
                                QLP_GET_PTR_VALUE(sInterval->mStartFieldDesc->mLeadingFieldPrecision),
                                stlStrlen( QLP_GET_PTR_VALUE(sInterval->mStartFieldDesc->mLeadingFieldPrecision) ),
                                & sBigintValue,
                                QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        STL_TRY_THROW(
                            (sBigintValue >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                            (sBigintValue <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                            RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION );
                        
                        aCodeTypeDef->mArgNum1 = sBigintValue;
                    }

                    /**
                     * Fractional Second Precision
                     */

                    if( sInterval->mEndFieldDesc->mFractionalSecondsPrecision == NULL )
                    {
                        aCodeTypeDef->mArgNum2 =
                            gDataTypeDefinition[aColumnType->mDBType].mDefFractionalSecondPrec;
                    }
                    else
                    {
                        STL_DASSERT( sEndField == DTL_INTERVAL_PRIMARY_SECOND );

                        sErrPos =
                            sInterval->mEndFieldDesc->mFractionalSecondsPrecision->mNodePos;
                        
                        STL_TRY_THROW(
                            dtlGetIntegerFromString(
                                QLP_GET_PTR_VALUE(sInterval->mEndFieldDesc->mFractionalSecondsPrecision),
                                stlStrlen( QLP_GET_PTR_VALUE(sInterval->mEndFieldDesc->mFractionalSecondsPrecision) ),
                                & sBigintValue,
                                QLL_ERROR_STACK(aEnv) ) == STL_SUCCESS,
                            RAMP_ERR_INVALID_INTERVAL_FRACTIONAL_SECOND_PRECISION );
                        
                        STL_TRY_THROW(
                            (sBigintValue >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION)
                            &&
                            (sBigintValue <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION),
                            RAMP_ERR_INVALID_INTERVAL_FRACTIONAL_SECOND_PRECISION );
                        
                        aCodeTypeDef->mArgNum2 = sBigintValue;
                    }
                    
                }
                
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                break;
            }
        case DTL_TYPE_ROWID:
            {
                aCodeTypeDef->mArgNum1 = DTL_PRECISION_NA;
                aCodeTypeDef->mArgNum2 = DTL_SCALE_NA;
                aCodeTypeDef->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
                aCodeTypeDef->mIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

                break;
            }
        default:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
    }


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_NUMERIC_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NUMERIC_PRECISION_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mNumericPrecision->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_NUMERIC_MIN_PRECISION,
                      DTL_NUMERIC_DEFAULT_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_NUMERIC_SCALE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NUMERIC_SCALE_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mNumericScale->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_NUMERIC_MIN_SCALE,
                      DTL_NUMERIC_MAX_SCALE );
    }

     STL_CATCH( RAMP_ERR_INVALID_FLOAT_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_FLOAT_PRECISION_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mNumericPrecision->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_FLOAT_MIN_PRECISION,
                      DTL_FLOAT_DEFAULT_PRECISION );
    }
     
    STL_CATCH( RAMP_ERR_INVALID_CHAR_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CHARACTER_LENGTH_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mStringLength->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_CHAR_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_VARCHAR_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CHARACTER_VARYING_LENGTH_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mStringLength->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_VARCHAR_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_BINARY_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BINARY_LENGTH_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mBinaryLength->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_BINARY_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_VARBINARY_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BINARY_VARYING_LENGTH_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mBinaryLength->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_VARBINARY_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_FRACTIONAL_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_FRACTIONAL_SECOND_PRECISION_INVALID,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mFractionalSecondPrec->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_TIME_MIN_PRECISION,
                      DTL_TIME_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_INTERVAL_INDICATOR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_INTERVAL_INDICATOR,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sInterval->mEndFieldDesc->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_INVALID_INTERVAL_LEADING_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_INTERVAL_LEADING_PRECISION,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sErrPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION,
                      DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_INTERVAL_FRACTIONAL_SECOND_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_INTERVAL_FRACTIONAL_SECONDS_PRECISION,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sErrPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION,
                      DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION );
    }
    
    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnType->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "qlvSetColumnType()" );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}
  

/**
 * @brief Expression List에 Integer Value Expression을 추가
 * @param[in]      aStmtInfo      Stmt Information
 * @param[in]      aInteger       Integer
 * @param[out]     aExprCode      Expression Code
 * @param[in]      aRelatedFuncId Related Function ID
 * @param[in]      aIterationTime 상위 노드의 Iteration Time
 * @param[in]      aRegionMem     Region Memory
 * @param[in]      aEnv           Environment
 *
 * @remark Validation 과정에서 생성된 Expression을 추가하는 함수임
 */
stlStatus qlvAddIntegerValueExpr( qlvStmtInfo         * aStmtInfo,
                                  stlInt64              aInteger,
                                  qlvInitExpression  ** aExprCode,
                                  knlBuiltInFunc        aRelatedFuncId,
                                  dtlIterationTime      aIterationTime,
                                  knlRegionMem        * aRegionMem,
                                  qllEnv              * aEnv )
{
    qlvInitExpression  * sExprCode   = NULL;
    qlvInitValue       * sIntegerValue = NULL;

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( *aExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    
    
    /**
     * Code Integer Value 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitValue ),
                                (void **) & sIntegerValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Code Integer Value 설정
     */

    sIntegerValue->mValueType       = QLV_VALUE_TYPE_BINARY_INTEGER;
    sIntegerValue->mInternalBindIdx = QLV_INTERNAL_BIND_VARIABLE_IDX_NA;
    sIntegerValue->mData.mInteger   = aInteger;


    /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sExprCode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sExprCode,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

    
    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );
    
    sExprCode->mType           = QLV_EXPR_TYPE_VALUE;
    sExprCode->mPosition       = 0;
    sExprCode->mIncludeExprCnt = 1;
    sExprCode->mExpr.mValue    = sIntegerValue;
    sExprCode->mRelatedFuncId  = aRelatedFuncId;
    sExprCode->mIterationTime  = DTL_ITERATION_TIME_NONE;
    sExprCode->mColumnName     = NULL;

    /**
     * OUTPUT 설정
     */

    *aExprCode = sExprCode;

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;    
}


/**
 * @brief result OFFSET/LIMIT 에 사용된 상수 표현식이 올바른지 검사한다.
 * @param[in] aSkipLimit  OFFSET/LIMIT init code plan
 * @param[in] aEnv        Environment
 * @return Valid or Not
 * @remarks
 */
stlBool qlvIsValidSkipLimit( qlvInitExpression * aSkipLimit, qllEnv * aEnv )
{
    stlBool              sIsValid = STL_FALSE;
    qlvInitExpression  * sSkipLimit = NULL;

    STL_DASSERT( aSkipLimit != NULL );

    if( aSkipLimit->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
    {
        sSkipLimit = aSkipLimit->mExpr.mConstExpr->mOrgExpr;
    }
    else
    {
        sSkipLimit = aSkipLimit;
    }
        
    switch( sSkipLimit->mType )
    {
        case QLV_EXPR_TYPE_VALUE:
            {
                /**
                 * 상수의 유형을 확인
                 */
                
                switch (sSkipLimit->mExpr.mValue->mValueType)
                {
                    case QLV_VALUE_TYPE_NUMERIC:
                    case QLV_VALUE_TYPE_BINARY_DOUBLE:
                    case QLV_VALUE_TYPE_BINARY_REAL:
                        {
                            /**
                             * 숫자임
                             */
                            
                            sIsValid = STL_TRUE;
                            break;
                        }
                    case QLV_VALUE_TYPE_STRING_FIXED:
                    case QLV_VALUE_TYPE_STRING_VARYING:
                        {
                            /**
                             * 숫자형 문자열인지 확인 
                             */
                            
                            dtlValidateFloat( sSkipLimit->mExpr.mValue->mData.mString,
                                              & sIsValid,
                                              QLL_ERROR_STACK(aEnv) );
                            break;
                        }
                    default:
                        {
                            /**
                             * 숫자도 문자도 아님
                             */
                            
                            sIsValid = STL_FALSE;
                            break;
                        }
                }
                
                break;
            }
        case QLV_EXPR_TYPE_BIND_PARAM:
            {
                /**
                 * Host 변수라면 Validation 단계에서 판단할 수 없다.
                 */
                sIsValid = STL_TRUE;
                break;
            }
        default:
            {
                sIsValid = STL_FALSE;
                break;
            }
    }
    
    return sIsValid;
}


/**
 * @brief 두 Expression이 동일한 의미를 갖는지 판단
 * @param[in] aExpr1        Expression 1
 * @param[in] aExpr2        Expression 2
 * @return Same or Not
 */
stlBool qlvIsSameExpression( qlvInitExpression  * aExpr1,
                             qlvInitExpression  * aExpr2 )
{
    qlvInitExpression  * sExpr1;
    qlvInitExpression  * sExpr2;

    qlvInitTypeDef     * sCastTypeDef1;
    qlvInitTypeDef     * sCastTypeDef2;
    qlvInitFunction    * sFunc1;
    qlvInitFunction    * sFunc2;
    qlvInitAggregation * sAggr1;
    qlvInitAggregation * sAggr2;
    qlvInitCaseExpr    * sCaseExpr1;
    qlvInitCaseExpr    * sCaseExpr2;
    
    qlvInitListFunc    * sListFunc1;
    qlvInitListFunc    * sListFunc2;
    qlvInitListCol     * sListCol1;
    qlvInitListCol     * sListCol2;
    qlvInitRowExpr     * sRowExpr1;
    qlvInitRowExpr     * sRowExpr2;

    stlInt32             i;
    
    STL_DASSERT( aExpr1 != NULL );
    STL_DASSERT( aExpr2 != NULL );
    
    if( aExpr1 == aExpr2 )
    {
        return STL_TRUE;
    }
    else
    {
        /* Do Nothing */
    }

    sExpr1 = aExpr1;
    sExpr2 = aExpr2;

    
    /**
     * Expression1 원본 Expression 얻기
     */

    while( 1 )
    {
        switch( sExpr1->mType )
        {
            case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
                {
                    sExpr1 = sExpr1->mExpr.mConstExpr->mOrgExpr;
                    continue;
                }
            case QLV_EXPR_TYPE_REFERENCE :
                {
                    sExpr1 = sExpr1->mExpr.mReference->mOrgExpr;
                    continue;
                }
            case QLV_EXPR_TYPE_INNER_COLUMN :
                {
                    /* Query Spec Node인 경우 Sub Table인 경우이다. */
                    if( sExpr1->mExpr.mInnerColumn->mRelationNode->mType != QLV_NODE_TYPE_SUB_TABLE )
                    {
                        sExpr1 = sExpr1->mExpr.mInnerColumn->mOrgExpr;
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
            case QLV_EXPR_TYPE_OUTER_COLUMN :
                {
                    sExpr1 = sExpr1->mExpr.mOuterColumn->mOrgExpr;
                    continue;
                }
            case QLV_EXPR_TYPE_VALUE :
            case QLV_EXPR_TYPE_BIND_PARAM :
            case QLV_EXPR_TYPE_FUNCTION :
            case QLV_EXPR_TYPE_CAST :
            case QLV_EXPR_TYPE_COLUMN :
            case QLV_EXPR_TYPE_PSEUDO_COLUMN :
            case QLV_EXPR_TYPE_ROWID_COLUMN :
            case QLV_EXPR_TYPE_AGGREGATION :
            case QLV_EXPR_TYPE_CASE_EXPR :
            case QLV_EXPR_TYPE_LIST_FUNCTION :
            case QLV_EXPR_TYPE_LIST_COLUMN :
            case QLV_EXPR_TYPE_ROW_EXPR :
            case QLV_EXPR_TYPE_SUB_QUERY :
                {
                    break;
                }
            case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
    
        break;
    }

    
    /**
     * Expression2 원본 Expression 얻기
     */

    while( 1 )
    {
        switch( sExpr2->mType )
        {
            case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
                {
                    sExpr2 = sExpr2->mExpr.mConstExpr->mOrgExpr;
                    continue;
                }
            case QLV_EXPR_TYPE_REFERENCE :
                {
                    sExpr2 = sExpr2->mExpr.mReference->mOrgExpr;
                    continue;
                }
            case QLV_EXPR_TYPE_INNER_COLUMN :
                {
                    /* Query Spec Node인 경우 Sub Table인 경우이다. */
                    if( sExpr2->mExpr.mInnerColumn->mRelationNode->mType != QLV_NODE_TYPE_SUB_TABLE )
                    {
                        sExpr2 = sExpr2->mExpr.mInnerColumn->mOrgExpr;
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
            case QLV_EXPR_TYPE_OUTER_COLUMN :
                {
                    sExpr2 = sExpr2->mExpr.mOuterColumn->mOrgExpr;
                    continue;
                }
            case QLV_EXPR_TYPE_VALUE :
            case QLV_EXPR_TYPE_BIND_PARAM :
            case QLV_EXPR_TYPE_FUNCTION :
            case QLV_EXPR_TYPE_CAST :
            case QLV_EXPR_TYPE_COLUMN :
            case QLV_EXPR_TYPE_PSEUDO_COLUMN :
            case QLV_EXPR_TYPE_ROWID_COLUMN :
            case QLV_EXPR_TYPE_AGGREGATION :
            case QLV_EXPR_TYPE_CASE_EXPR :
            case QLV_EXPR_TYPE_LIST_FUNCTION :
            case QLV_EXPR_TYPE_LIST_COLUMN :
            case QLV_EXPR_TYPE_ROW_EXPR :
            case QLV_EXPR_TYPE_SUB_QUERY :
                {
                    break;
                }
            case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
    
        break;
    }
    

    /**
     * Expression 비교
     */

    if( sExpr1 == sExpr2 )
    {
        return STL_TRUE;
    }
    else
    {
        if( sExpr1->mType != sExpr2->mType )
        {
            return STL_FALSE;
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    switch( sExpr1->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
            {
                if( sExpr1->mExpr.mValue->mInternalBindIdx !=
                    sExpr2->mExpr.mValue->mInternalBindIdx )
                {
                    return STL_FALSE;
                }
                else
                {
                    if( sExpr1->mExpr.mValue->mInternalBindIdx != QLV_INTERNAL_BIND_VARIABLE_IDX_NA )
                    {
                        return ( sExpr1->mPosition == sExpr2->mPosition );
                    }
                }

                if( sExpr1->mExpr.mValue->mValueType ==
                    sExpr2->mExpr.mValue->mValueType )
                {
                    switch( sExpr1->mExpr.mValue->mValueType )
                    {
                        case QLV_VALUE_TYPE_NULL :
                            {
                                return STL_TRUE;
                            }
                        case QLV_VALUE_TYPE_BOOLEAN :
                        case QLV_VALUE_TYPE_BINARY_INTEGER :
                            {
                                return ( sExpr1->mExpr.mValue->mData.mInteger ==
                                         sExpr2->mExpr.mValue->mData.mInteger );
                            }
                        default :
                            {
                                return ( stlStrcmp( sExpr1->mExpr.mValue->mData.mString,
                                                    sExpr2->mExpr.mValue->mData.mString ) == 0 );
                            }
                    }
                }
                else
                {
                    return STL_FALSE;
                }
            }
        case QLV_EXPR_TYPE_BIND_PARAM :
            {
                return ( ( sExpr1->mExpr.mBindParam->mBindType ==
                           sExpr2 ->mExpr.mBindParam->mBindType ) &&
                         ( sExpr1->mExpr.mBindParam->mBindParamIdx ==
                           sExpr2 ->mExpr.mBindParam->mBindParamIdx ) );
            }
        case QLV_EXPR_TYPE_COLUMN :
            {
                return ( ( sExpr1->mExpr.mColumn->mRelationNode ==
                           sExpr2->mExpr.mColumn->mRelationNode ) &&
                         ( ellGetColumnID( sExpr1->mExpr.mColumn->mColumnHandle ) ==
                           ellGetColumnID( sExpr2->mExpr.mColumn->mColumnHandle ) ) );
            }
        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
            {
                return ( sExpr1->mExpr.mPseudoCol->mPseudoOffset ==
                         sExpr2->mExpr.mPseudoCol->mPseudoOffset );
            }
        case QLV_EXPR_TYPE_ROWID_COLUMN :
            {
                return ( sExpr1->mExpr.mRowIdColumn->mRelationNode ==
                         sExpr2->mExpr.mRowIdColumn->mRelationNode );
            }
        case QLV_EXPR_TYPE_INNER_COLUMN :
            {
                STL_DASSERT( sExpr1->mExpr.mInnerColumn->mRelationNode->mType == QLV_NODE_TYPE_SUB_TABLE );
                STL_DASSERT( sExpr2->mExpr.mInnerColumn->mRelationNode->mType == QLV_NODE_TYPE_SUB_TABLE );
                return ( ( sExpr1->mExpr.mColumn->mRelationNode ==
                           sExpr2->mExpr.mColumn->mRelationNode ) &&
                         ( *(sExpr1->mExpr.mInnerColumn->mIdx) ==
                           *(sExpr2->mExpr.mInnerColumn->mIdx) ) );
            }

        case QLV_EXPR_TYPE_FUNCTION :
            {
                sFunc1 = sExpr1->mExpr.mFunction;
                sFunc2 = sExpr2->mExpr.mFunction;

                if( ( sFunc1->mFuncId ==
                      sFunc2->mFuncId ) &&
                    ( sFunc1->mArgCount ==
                      sFunc2->mArgCount ) )
                {
                    /**
                     * @todo Argument 순서가 변경되더라도
                     *  <BR> 의미상 동치인 Expression을 판단할 수 있어야 한다.
                     */
                    
                    /* arguments */
                    for( i = 0 ; i < sFunc1->mArgCount ; i++ )
                    {
                        if( qlvIsSameExpression( sFunc1->mArgs[ i ],
                                                 sFunc2->mArgs[ i ] )
                            == STL_TRUE )
                        {
                            /* Do Nothing */
                        }
                        else
                        {
                            return STL_FALSE;
                        }
                    }
                }
                else
                {
                    return STL_FALSE;
                }
                return STL_TRUE;
            }
        case QLV_EXPR_TYPE_CAST :
            {
                if( qlvIsSameExpression( sExpr1->mExpr.mTypeCast->mArgs[0],
                                         sExpr2->mExpr.mTypeCast->mArgs[0] )
                    == STL_TRUE )
                {                    
                    sCastTypeDef1 = & sExpr1->mExpr.mTypeCast->mTypeDef;
                    sCastTypeDef2 = & sExpr2->mExpr.mTypeCast->mTypeDef;

                    return ( ( sCastTypeDef1->mDBType == sCastTypeDef2->mDBType ) &&
                             ( sCastTypeDef1->mArgNum1 == sCastTypeDef2->mArgNum1 ) &&
                             ( sCastTypeDef1->mArgNum2 == sCastTypeDef2->mArgNum2 ) &&
                             ( sCastTypeDef1->mStringLengthUnit == sCastTypeDef2->mStringLengthUnit ) &&
                             ( sCastTypeDef1->mIntervalIndicator == sCastTypeDef2->mIntervalIndicator ) );
                }
                else
                {
                    return STL_FALSE;
                }
            }
        case QLV_EXPR_TYPE_AGGREGATION :
            {
                sAggr1 = sExpr1->mExpr.mAggregation;
                sAggr2 = sExpr2->mExpr.mAggregation;

                if( ( sAggr1->mAggrId ==
                      sAggr2->mAggrId ) &&
                    ( sAggr1->mArgCount ==
                      sAggr2->mArgCount ) &&
                    ( sAggr1->mIsDistinct ==
                      sAggr2->mIsDistinct ) )
                {
                    /* arguments */
                    for( i = 0 ; i < sAggr1->mArgCount ; i++ )
                    {
                        if( qlvIsSameExpression( sAggr1->mArgs[ i ],
                                                 sAggr2->mArgs[ i ] )
                            == STL_TRUE )
                        {
                            /* Do Nothing */
                        }
                        else
                        {
                            return STL_FALSE;
                        }
                    }

                    /* filter */
                    /**
                     * @todo (LONG TODO)
                     */
                    if( sAggr1->mFilter == NULL )
                    {
                        return ( sAggr2->mFilter == NULL );
                    }
                    else
                    {
                        if( sAggr2->mFilter == NULL )
                        {
                            return STL_FALSE;
                        }
                        else
                        {
                            return ( qlvIsSameExpression( sAggr1->mFilter,
                                                          sAggr2->mFilter ) );
                        }
                    }
                }
                else
                {
                    return STL_FALSE;
                }
                return STL_TRUE;
            }

        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                sCaseExpr1 = sExpr1->mExpr.mCaseExpr;
                sCaseExpr2 = sExpr2->mExpr.mCaseExpr;

                if( ( sCaseExpr1->mFuncId ==
                      sCaseExpr2->mFuncId ) &&
                    ( sCaseExpr1->mCount ==
                      sCaseExpr2->mCount ) )
                {
                    /**
                     * @todo Argument 순서가 변경되더라도
                     *  <BR> 의미상 동치인 Expression을 판단할 수 있어야 한다.
                     */
                    
                    /* arguments */
                    for( i = 0 ; i < sCaseExpr1->mCount ; i++ )
                    {
                        if( qlvIsSameExpression( sCaseExpr1->mWhenArr[ i ],
                                                 sCaseExpr2->mWhenArr[ i ] )
                            == STL_TRUE )
                        {
                            /* Do Nothing */
                        }
                        else
                        {
                            return STL_FALSE;
                        }

                        if( qlvIsSameExpression( sCaseExpr1->mThenArr[ i ],
                                                 sCaseExpr2->mThenArr[ i ] )
                            == STL_TRUE )
                        {
                            /* Do Nothing */
                        }
                        else
                        {
                            return STL_FALSE;
                        }                        
                    }

                    if( qlvIsSameExpression( sCaseExpr1->mDefResult,
                                             sCaseExpr2->mDefResult )
                        == STL_TRUE )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        return STL_FALSE;
                    }                    
                }
                else
                {
                    return STL_FALSE;
                }

                return STL_TRUE;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                sListFunc1 = sExpr1->mExpr.mListFunc;
                sListFunc2 = sExpr2->mExpr.mListFunc;

                if( ( sListFunc1->mFuncId ==
                      sListFunc2->mFuncId ) &&
                    ( sListFunc1->mArgCount ==
                      sListFunc2->mArgCount ) )
                {
                    /**
                     * @todo Argument 순서가 변경되더라도
                     *  <BR> 의미상 동치인 Expression을 판단할 수 있어야 한다.
                     */
                    
                    /* arguments */
                    for( i = 0 ; i < sListFunc1->mArgCount ; i++ )
                    {
                        if( qlvIsSameExpression( sListFunc1->mArgs[ i ],
                                                 sListFunc2->mArgs[ i ] )
                            == STL_TRUE )
                        {
                            /* Do Nothing */
                        }
                        else
                        {
                            return STL_FALSE;
                        }
                    }
                }
                else
                {
                    return STL_FALSE;
                }
                return STL_TRUE;
            }
        case QLV_EXPR_TYPE_LIST_COLUMN :
            {
                sListCol1 = sExpr1->mExpr.mListCol;
                sListCol2 = sExpr2->mExpr.mListCol;

                if( ( sListCol1->mArgCount ==
                      sListCol2->mArgCount ) )
                {
                    /**
                     * @todo Argument 순서가 변경되더라도
                     *  <BR> 의미상 동치인 Expression을 판단할 수 있어야 한다.
                     */
                    
                    /* arguments */
                    for( i = 0 ; i < sListCol1->mArgCount ; i++ )
                    {
                        if( qlvIsSameExpression( sListCol1->mArgs[ i ],
                                                 sListCol2->mArgs[ i ] )
                            == STL_TRUE )
                        {
                            /* Do Nothing */
                        }
                        else
                        {
                            return STL_FALSE;
                        }
                    }
                }
                else
                {
                    return STL_FALSE;
                }
                return STL_TRUE;
            }            

        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                sRowExpr1 = sExpr1->mExpr.mRowExpr;
                sRowExpr2 = sExpr2->mExpr.mRowExpr;

                if( ( sRowExpr1->mArgCount ==
                      sRowExpr2->mArgCount ) )
                {
                    /**
                     * @todo Argument 순서가 변경되더라도
                     *  <BR> 의미상 동치인 Expression을 판단할 수 있어야 한다.
                     */
                    
                    /* arguments */
                    for( i = 0 ; i < sRowExpr1->mArgCount ; i++ )
                    {
                        if( qlvIsSameExpression( sRowExpr1->mArgs[ i ],
                                                 sRowExpr2->mArgs[ i ] )
                            == STL_TRUE )
                        {
                            /* Do Nothing */
                        }
                        else
                        {
                            return STL_FALSE;
                        }
                    }
                }
                else
                {
                    return STL_FALSE;
                }
                return STL_TRUE;
            }            
        case QLV_EXPR_TYPE_SUB_QUERY :
            {
                return ( sExpr1->mExpr.mSubQuery->mInitNode ==
                         sExpr2->mExpr.mSubQuery->mInitNode );
            }
            
            
        /* invalid */
        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
        case QLV_EXPR_TYPE_REFERENCE :
        case QLV_EXPR_TYPE_OUTER_COLUMN :
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }
    
    return STL_FALSE;
}


/**
 * @brief Expression 내에 찾고자 하는 Expression이 있는지 검사한다.
 * @param[in] aExpr         Expression
 * @param[in] aSearchExpr   찾으려는 Expression
 * @return Exist or Not
 */
stlBool qlvIsExistInExpression( qlvInitExpression  * aExpr,
                                qlvInitExpression  * aSearchExpr )
{
    qlvInitExpression  * sExpr1;
    qlvInitExpression  * sExpr2;
    
    STL_DASSERT( aExpr != NULL );
    STL_DASSERT( aSearchExpr != NULL );
    
    if( aExpr == aSearchExpr )
    {
        return STL_TRUE;
    }
    else
    {
        /* Do Nothing */
    }

    sExpr1 = aExpr;
    sExpr2 = aSearchExpr;

    
    /**
     * Expression1 원본 Expression 얻기
     */

    while( 1 )
    {
        switch( sExpr1->mType )
        {
            case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
                {
                    sExpr1 = sExpr1->mExpr.mConstExpr->mOrgExpr;
                    continue;
                }
            case QLV_EXPR_TYPE_REFERENCE :
                {
                    sExpr1 = sExpr1->mExpr.mReference->mOrgExpr;
                    continue;
                }
            case QLV_EXPR_TYPE_INNER_COLUMN :
                {
                    /* Query Spec Node인 경우 Sub Table인 경우이다. */
                    if( sExpr1->mExpr.mInnerColumn->mRelationNode->mType != QLV_NODE_TYPE_SUB_TABLE )
                    {
                        sExpr1 = sExpr1->mExpr.mInnerColumn->mOrgExpr;
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
            case QLV_EXPR_TYPE_OUTER_COLUMN :
                {
                    sExpr1 = sExpr1->mExpr.mOuterColumn->mOrgExpr;
                    continue;
                }
            case QLV_EXPR_TYPE_VALUE :
            case QLV_EXPR_TYPE_BIND_PARAM :
            case QLV_EXPR_TYPE_FUNCTION :
            case QLV_EXPR_TYPE_CAST :
            case QLV_EXPR_TYPE_COLUMN :
            case QLV_EXPR_TYPE_PSEUDO_COLUMN :
            case QLV_EXPR_TYPE_ROWID_COLUMN :
            case QLV_EXPR_TYPE_AGGREGATION :
            case QLV_EXPR_TYPE_CASE_EXPR :
            case QLV_EXPR_TYPE_LIST_FUNCTION :
            case QLV_EXPR_TYPE_LIST_COLUMN :
            case QLV_EXPR_TYPE_ROW_EXPR :
            case QLV_EXPR_TYPE_SUB_QUERY :
                {
                    break;
                }
            case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
    
        break;
    }

    
    /**
     * Expression2 원본 Expression 얻기
     */

    while( 1 )
    {
        switch( sExpr2->mType )
        {
            case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
                {
                    sExpr2 = sExpr2->mExpr.mConstExpr->mOrgExpr;
                    continue;
                }
            case QLV_EXPR_TYPE_REFERENCE :
                {
                    sExpr2 = sExpr2->mExpr.mReference->mOrgExpr;
                    continue;
                }
            case QLV_EXPR_TYPE_INNER_COLUMN :
                {
                    /* Query Spec Node인 경우 Sub Table인 경우이다. */
                    if( sExpr2->mExpr.mInnerColumn->mRelationNode->mType != QLV_NODE_TYPE_SUB_TABLE )
                    {
                        sExpr2 = sExpr2->mExpr.mInnerColumn->mOrgExpr;
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
            case QLV_EXPR_TYPE_OUTER_COLUMN :
                {
                    sExpr2 = sExpr2->mExpr.mOuterColumn->mOrgExpr;
                    continue;
                }
            case QLV_EXPR_TYPE_VALUE :
            case QLV_EXPR_TYPE_BIND_PARAM :
            case QLV_EXPR_TYPE_FUNCTION :
            case QLV_EXPR_TYPE_CAST :
            case QLV_EXPR_TYPE_COLUMN :
            case QLV_EXPR_TYPE_PSEUDO_COLUMN :
            case QLV_EXPR_TYPE_ROWID_COLUMN :
            case QLV_EXPR_TYPE_AGGREGATION :
            case QLV_EXPR_TYPE_CASE_EXPR :
            case QLV_EXPR_TYPE_LIST_FUNCTION :
            case QLV_EXPR_TYPE_LIST_COLUMN :
            case QLV_EXPR_TYPE_ROW_EXPR :
            case QLV_EXPR_TYPE_SUB_QUERY :
                {
                    break;
                }
            case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
    
        break;
    }
    

    /**
     * Expression 비교
     */

    return ( sExpr1 == sExpr2 );
}

/**
 * @brief Expression 내에서 주어진 Idx에 해당하는 Internal Bind Value Expression을 찾는다.
 * @param[in]      aExpr                 Input Expression
 * @param[in]      aIdx                  Internal Bind Value Idx
 */
qlvInitExpression *  qlvFindInternalBindValExprInExpr( qlvInitExpression   * aExpr,
                                                       stlInt32              aIdx )
{
    qlvInitExpression  * sExpr;
    qlvInitExpression  * sResExpr;
    stlInt32             sLoop;

    STL_DASSERT( aExpr != NULL );
    STL_DASSERT( aIdx != QLV_INTERNAL_BIND_VARIABLE_IDX_NA );

    sExpr = aExpr;
    while( 1 )
    {
        switch( sExpr->mType )
        {
            case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
                {
                    sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
                    continue;
                }
            case QLV_EXPR_TYPE_VALUE :
                {
                    if( aIdx == sExpr->mExpr.mValue->mInternalBindIdx )
                    {
                        return sExpr;
                    }
                    else
                    {
                        return NULL;
                    }
                }
            case QLV_EXPR_TYPE_FUNCTION :
                {
                    for( sLoop = 0 ; sLoop < sExpr->mExpr.mFunction->mArgCount ; sLoop++ )
                    {
                        sResExpr = qlvFindInternalBindValExprInExpr( sExpr->mExpr.mFunction->mArgs[ sLoop ],
                                                                     aIdx );
                        
                        if( sResExpr != NULL )
                        {
                            return sResExpr;
                        }
                    }
                    return NULL;
                }
            case QLV_EXPR_TYPE_COLUMN :
            case QLV_EXPR_TYPE_PSEUDO_COLUMN :
            case QLV_EXPR_TYPE_ROWID_COLUMN :
            case QLV_EXPR_TYPE_BIND_PARAM :
            case QLV_EXPR_TYPE_SUB_QUERY :
            case QLV_EXPR_TYPE_REFERENCE :
            case QLV_EXPR_TYPE_INNER_COLUMN :
            case QLV_EXPR_TYPE_OUTER_COLUMN :
            case QLV_EXPR_TYPE_CAST :
            case QLV_EXPR_TYPE_AGGREGATION :
            case QLV_EXPR_TYPE_CASE_EXPR :
            case QLV_EXPR_TYPE_LIST_FUNCTION :
            case QLV_EXPR_TYPE_LIST_COLUMN :
            case QLV_EXPR_TYPE_ROW_EXPR :
                {
                    return NULL;
                }
            case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
    
        break;
    }
    
    return NULL;
}


/**
 * @brief Parse Function을 구성한다.
 * @param[in]      aFunctionId           function ID
 * @param[in]      aArg                  function operand
 * @param[in]      aNodePos              node position
 * @param[out]     aFunction             function 
 * @param[in]      aRegionMem            Region Memory
 * @param[in]      aEnv                  Environment
 */
stlStatus qlvMakeParseFunctionArgs1( knlBuiltInFunc    aFunctionId,
                                     void            * aArg,
                                     stlInt32          aNodePos,
                                     qlpFunction    ** aFunction,
                                     knlRegionMem    * aRegionMem,
                                     qllEnv          * aEnv )
{
    qlpList         * sList        = NULL;
    qlpFunction     * sFunction    = NULL;
    qlpListElement  * sListElement = NULL;
    
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aArg != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFunction != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /* Function */                
    STL_TRY( knlAllocRegionMem( aRegionMem, 
                                STL_SIZEOF(qlpFunction),
                                (void **) & sFunction,
                                KNL_ENV(aEnv) ) 
             == STL_SUCCESS );
    stlMemset( sFunction, 0x00, STL_SIZEOF(qlpFunction) );

    /* Function Argument List Left Argument */
    STL_TRY( knlAllocRegionMem( aRegionMem, 
                                STL_SIZEOF(qlpListElement), 
                                (void **) & sListElement, 
                                KNL_ENV(aEnv) ) 
             == STL_SUCCESS );
    stlMemset( sListElement, 0x00, STL_SIZEOF(qlpListElement) );

    sListElement->mData.mPtrValue = aArg; 
    sListElement->mPrev = NULL; 
    sListElement->mNext = NULL; 

    /* Function Argument List */
    STL_TRY( knlAllocRegionMem( aRegionMem, 
                                STL_SIZEOF(qlpList),
                                (void **) & sList,
                                KNL_ENV(aEnv) ) 
             == STL_SUCCESS );
    stlMemset( sList, 0x00, STL_SIZEOF(qlpList) );

    sList->mType  = QLL_POINTER_LIST_TYPE; 
    sList->mCount = 1;
    sList->mHead  = sListElement;
    sList->mTail  = sListElement;

    /* Function 구성. */
    sFunction->mType    = QLL_FUNCTION_TYPE;
    sFunction->mFuncId  = aFunctionId;
    sFunction->mNodePos = aNodePos;
    sFunction->mNodeLen = 0;
    sFunction->mArgs    = sList;

    /* OUTPUT 설정 */
    *aFunction = sFunction;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Parse Function을 구성한다.
 * @param[in]      aFunctionId           function ID
 * @param[in]      aLeftArg              function left operand
 * @param[in]      aRightArg             function right operand
 * @param[in]      aNodePos              node position
 * @param[out]     aFunction             function 
 * @param[in]      aRegionMem            Region Memory
 * @param[in]      aEnv                  Environment
 */
stlStatus qlvMakeParseFunctionArgs2( knlBuiltInFunc    aFunctionId,
                                     void            * aLeftArg,
                                     void            * aRightArg,
                                     stlInt32          aNodePos,
                                     qlpFunction    ** aFunction,
                                     knlRegionMem    * aRegionMem,
                                     qllEnv          * aEnv )
{
    qlpList         * sList             = NULL;
    qlpFunction     * sFunction         = NULL;
    qlpListElement  * sListElementLeft  = NULL;
    qlpListElement  * sListElementRight = NULL;
    
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aLeftArg != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRightArg != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFunction != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /* Function */                
    STL_TRY( knlAllocRegionMem( aRegionMem, 
                                STL_SIZEOF(qlpFunction),
                                (void **) & sFunction,
                                KNL_ENV(aEnv) ) 
             == STL_SUCCESS );
    stlMemset( sFunction, 0x00, STL_SIZEOF(qlpFunction) );

    /* Function Argument List Left Argument */
    STL_TRY( knlAllocRegionMem( aRegionMem, 
                                STL_SIZEOF(qlpListElement), 
                                (void **) & sListElementLeft, 
                                KNL_ENV(aEnv) ) 
             == STL_SUCCESS );
    stlMemset( sListElementLeft, 0x00, STL_SIZEOF(qlpListElement) );

    sListElementLeft->mData.mPtrValue = aLeftArg; 
    sListElementLeft->mPrev = NULL; 
    sListElementLeft->mNext = NULL; 

    /* Function Argument List Right Argument */ 
    STL_TRY( knlAllocRegionMem( aRegionMem, 
                                STL_SIZEOF(qlpListElement), 
                                (void **) & sListElementRight,
                                KNL_ENV(aEnv) ) 
             == STL_SUCCESS );
    stlMemset( sListElementRight, 0x00, STL_SIZEOF(qlpListElement) ); 

    sListElementRight->mData.mPtrValue = aRightArg; 
    sListElementRight->mPrev = NULL;
    sListElementRight->mNext = NULL;

    /* Function Argument List */
    STL_TRY( knlAllocRegionMem( aRegionMem, 
                                STL_SIZEOF(qlpList),
                                (void **) & sList,
                                KNL_ENV(aEnv) ) 
             == STL_SUCCESS );
    stlMemset( sList, 0x00, STL_SIZEOF(qlpList) );

    sList->mType  = QLL_POINTER_LIST_TYPE; 
    sList->mCount = 2;
    sList->mHead  = sListElementLeft;
    sList->mTail  = sListElementLeft;

    sList->mTail->mNext      = sListElementRight; 
    sListElementRight->mPrev = sList->mTail;
    sList->mTail             = sListElementRight; 

    /* Function 구성. */
    sFunction->mType    = QLL_FUNCTION_TYPE;
    sFunction->mFuncId  = aFunctionId;
    sFunction->mNodePos = aNodePos;
    sFunction->mNodeLen = 0;
    sFunction->mArgs    = sList;

    /* OUTPUT 설정 */
    *aFunction = sFunction;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlv */


