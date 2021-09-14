/*******************************************************************************
 * qlnvCommon.c
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
 * @file qlnvCommon.c
 * @brief SQL Processor Layer Validation - Common Functions
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnv
 * @{
 */


/****************************************************
 * Inline Functions
 ****************************************************/ 

/**
 * @brief Reference Table List를 생성한다.
 * @param[in]  aRegionMem           Region Memory
 * @param[in]  aParentRefTableList  Parent Reference Table List
 * @param[out] aRefTableList        Reference Table List
 * @param[in]  aEnv                 Environment
 */
inline stlStatus qlvCreateRefTableList( knlRegionMem       * aRegionMem,
                                        qlvRefTableList    * aParentRefTableList,
                                        qlvRefTableList   ** aRefTableList,
                                        qllEnv             * aEnv )
{
    qlvRefTableItem     * sRefTableItem = NULL;
    qlvRefTableList     * sRefTableList = NULL;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Create Reference Table List
     **********************************************************/

    /**
     * Reference Table Item 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvRefTableItem ),
                                (void **) & sRefTableItem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Reference Table List 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvRefTableList ),
                                (void **) & sRefTableList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Parent Reference Table List를 이용하여
     * Reference Table Item 및 Reference Table List 설정
     */

    if( aParentRefTableList == NULL )
    {
        sRefTableItem->mTableNode   = NULL;
        sRefTableItem->mTargetCount = 0;
        sRefTableItem->mTargets     = NULL;
        sRefTableItem->mNext        = NULL;

        sRefTableList->mHead        = sRefTableItem;
        sRefTableList->mCount       = 1;
    }
    else
    {
        sRefTableItem->mTableNode   = NULL;
        sRefTableItem->mTargetCount = 0;
        sRefTableItem->mTargets     = NULL;
        sRefTableItem->mNext        = aParentRefTableList->mHead;

        sRefTableList->mHead        = sRefTableItem;
        sRefTableList->mCount       = aParentRefTableList->mCount + 1;
    }


    /**********************************************************
     * 마무리
     **********************************************************/

    /**
     * Output 설정
     */

    *aRefTableList = sRefTableList;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Reference Table List에 Table Node를 추가한다.
 * @param[in]  aRefTableList        Reference Table List
 * @param[in]  aTableNode           Table Node
 * @param[in]  aEnv                 Environment
 */
inline stlStatus qlvSetNodeToRefTableItem( qlvRefTableList   * aRefTableList,
                                           qlvInitNode       * aTableNode,
                                           qllEnv            * aEnv )
{
    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aRefTableList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefTableList->mCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefTableList->mHead->mTableNode == NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Add Table Node To Reference Table List
     **********************************************************/

    aRefTableList->mHead->mTableNode = aTableNode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Reference Table List에 Target 정보를 추가한다.
 * @param[in]  aRefTableList        Reference Table List
 * @param[in]  aTargetCount         Target Count
 * @param[in]  aTargets             Target Array
 * @param[in]  aEnv                 Environment
 */
inline stlStatus qlvSetTargetToRefTableItem( qlvRefTableList   * aRefTableList,
                                             stlInt32            aTargetCount,
                                             qlvInitTarget     * aTargets,
                                             qllEnv            * aEnv )
{
    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aRefTableList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefTableList->mCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefTableList->mHead->mTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargets != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Add Targets To Reference Table List
     **********************************************************/

    aRefTableList->mHead->mTargetCount  = aTargetCount;
    aRefTableList->mHead->mTargets      = aTargets;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief And Function을 생성한다.
 * @param[in]  aRegionMem           Region Memory
 * @param[in]  aPosition            Position
 * @param[in]  aLeftExpr            Left Expression
 * @param[in]  aRightExpr           Right Expression
 * @param[out] aFuncExpr            Function Expression
 * @param[in]  aEnv                 Environment
 */
inline stlStatus qlvMakeFuncAnd2Arg( knlRegionMem         * aRegionMem,
                                     stlInt32               aPosition,
                                     qlvInitExpression    * aLeftExpr,
                                     qlvInitExpression    * aRightExpr,
                                     qlvInitExpression   ** aFuncExpr,
                                     qllEnv               * aEnv )
{
    qlvInitExpression  * sFuncCode       = NULL;
    qlvInitFunction    * sFunction       = NULL;




    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLeftExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRightExpr != NULL, QLL_ERROR_STACK(aEnv) );


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

    sFunction->mFuncId = KNL_BUILTIN_FUNC_AND;


    /**
     * Argument List 공간 할당
     */

    sFunction->mArgCount = 2;

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression* ) * sFunction->mArgCount,
                                (void **) & sFunction->mArgs,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sFunction->mArgs,
               0x00,
               STL_SIZEOF( qlvInitExpression* ) * sFunction->mArgCount );

    sFunction->mArgs[0] = aLeftExpr;
    sFunction->mArgs[1] = aRightExpr;


    /**
     * Left Expr과 Right Expr에 Related Function ID 설정
     */

    aLeftExpr->mRelatedFuncId = KNL_BUILTIN_FUNC_AND;
    aRightExpr->mRelatedFuncId = KNL_BUILTIN_FUNC_AND;


    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( aLeftExpr->mIncludeExprCnt + aRightExpr->mIncludeExprCnt + 1, aEnv );
 
    sFuncCode->mType           = QLV_EXPR_TYPE_FUNCTION;
    sFuncCode->mPosition       = aPosition;
    sFuncCode->mIncludeExprCnt = aLeftExpr->mIncludeExprCnt + aRightExpr->mIncludeExprCnt + 1;
    sFuncCode->mExpr.mFunction = sFunction;
    sFuncCode->mIterationTime  = DTL_ITERATION_TIME_FOR_EACH_EXPR;
    sFuncCode->mColumnName     = NULL;


    /**
     * OUTPUT 설정
     */

    *aFuncExpr = sFuncCode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Or Function을 생성한다.
 * @param[in]  aRegionMem           Region Memory
 * @param[in]  aPosition            Position
 * @param[in]  aExprCnt             Expression Count
 * @param[in]  aExprArr             Expression Array
 * @param[out] aFuncExpr            Function Expression
 * @param[in]  aEnv                 Environment
 */
inline stlStatus qlvMakeFuncOrNArg( knlRegionMem         * aRegionMem,
                                    stlInt32               aPosition,
                                    stlInt32               aExprCnt,
                                    qlvInitExpression   ** aExprArr,
                                    qlvInitExpression   ** aFuncExpr,
                                    qllEnv               * aEnv )
{
    qlvInitExpression  * sFuncCode       = NULL;
    qlvInitFunction    * sFunction       = NULL;
    stlInt32             sTotalExprCnt   = 0;
    stlInt32             i;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprCnt >= 2, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprArr != NULL, QLL_ERROR_STACK(aEnv) );


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

    sFunction->mFuncId = KNL_BUILTIN_FUNC_OR;


    /**
     * Argument List 세팅
     */

    sFunction->mArgCount = aExprCnt;
    sFunction->mArgs = aExprArr;


    /**
     * Expr에 Related Function ID 설정 및 전체 개수 조사
     */

    for( i = 0; i < aExprCnt; i++ )
    {
        aExprArr[i]->mRelatedFuncId = KNL_BUILTIN_FUNC_OR;
        sTotalExprCnt += aExprArr[i]->mIncludeExprCnt;
    }


    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( sTotalExprCnt + 1, aEnv );
    
    sFuncCode->mType           = QLV_EXPR_TYPE_FUNCTION;
    sFuncCode->mPosition       = aPosition;
    sFuncCode->mIncludeExprCnt = sTotalExprCnt + 1;
    sFuncCode->mExpr.mFunction = sFunction;
    sFuncCode->mIterationTime  = DTL_ITERATION_TIME_FOR_EACH_EXPR;
    sFuncCode->mColumnName     = NULL;

    
    /**
     * OUTPUT 설정
     */

    *aFuncExpr = sFuncCode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief And Function을 생성한다.
 * @param[in]  aRegionMem           Region Memory
 * @param[in]  aPosition            Position
 * @param[in]  aExprCnt             Expression Count
 * @param[in]  aExprArr             Expression Array
 * @param[out] aFuncExpr            Function Expression
 * @param[in]  aEnv                 Environment
 */
inline stlStatus qlvMakeFuncAndNArg( knlRegionMem         * aRegionMem,
                                     stlInt32               aPosition,
                                     stlInt32               aExprCnt,
                                     qlvInitExpression   ** aExprArr,
                                     qlvInitExpression   ** aFuncExpr,
                                     qllEnv               * aEnv )
{
    qlvInitExpression  * sFuncCode       = NULL;
    qlvInitFunction    * sFunction       = NULL;
    stlInt32             sTotalExprCnt   = 0;
    stlInt32             i;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprCnt >= 2, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprArr != NULL, QLL_ERROR_STACK(aEnv) );


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

    sFunction->mFuncId = KNL_BUILTIN_FUNC_AND;


    /**
     * Argument List 세팅
     */

    sFunction->mArgCount = aExprCnt;
    sFunction->mArgs = aExprArr;


    /**
     * Expr에 Related Function ID 설정 및 전체 개수 조사
     */

    for( i = 0; i < aExprCnt; i++ )
    {
        aExprArr[i]->mRelatedFuncId = KNL_BUILTIN_FUNC_AND;
        sTotalExprCnt += aExprArr[i]->mIncludeExprCnt;
    }


    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( sTotalExprCnt + 1, aEnv );

    sFuncCode->mType           = QLV_EXPR_TYPE_FUNCTION;
    sFuncCode->mPosition       = aPosition;
    sFuncCode->mIncludeExprCnt = sTotalExprCnt + 1;
    sFuncCode->mExpr.mFunction = sFunction;
    sFuncCode->mIterationTime  = DTL_ITERATION_TIME_FOR_EACH_EXPR;
    sFuncCode->mColumnName     = NULL;

    
    /**
     * OUTPUT 설정
     */

    *aFuncExpr = sFuncCode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Equal Function을 생성한다.
 * @param[in]  aRegionMem           Region Memory
 * @param[in]  aPosition            Position
 * @param[in]  aLeftExpr            Left Expression
 * @param[in]  aRightExpr           Right Expression
 * @param[out] aFuncExpr            Function Expression
 * @param[in]  aEnv                 Environment
 */
inline stlStatus qlvMakeFuncEqual( knlRegionMem         * aRegionMem,
                                   stlInt32               aPosition,
                                   qlvInitExpression    * aLeftExpr,
                                   qlvInitExpression    * aRightExpr,
                                   qlvInitExpression   ** aFuncExpr,
                                   qllEnv               * aEnv )
{
    qlvInitExpression  * sFuncCode       = NULL;
    qlvInitFunction    * sFunction       = NULL;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLeftExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRightExpr != NULL, QLL_ERROR_STACK(aEnv) );


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

    sFunction->mFuncId = KNL_BUILTIN_FUNC_IS_EQUAL;


    /**
     * Argument List 공간 할당
     */

    sFunction->mArgCount = 2;

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression* ) * sFunction->mArgCount,
                                (void **) & sFunction->mArgs,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sFunction->mArgs,
               0x00,
               STL_SIZEOF( qlvInitExpression* ) * sFunction->mArgCount );

    sFunction->mArgs[0] = aLeftExpr;
    sFunction->mArgs[1] = aRightExpr;


    /**
     * Left Expr과 Right Expr에 Related Function ID 설정
     */

    aLeftExpr->mRelatedFuncId = KNL_BUILTIN_FUNC_IS_EQUAL;
    aRightExpr->mRelatedFuncId = KNL_BUILTIN_FUNC_IS_EQUAL;


    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( aLeftExpr->mIncludeExprCnt + aRightExpr->mIncludeExprCnt + 1, aEnv );
    
    sFuncCode->mType           = QLV_EXPR_TYPE_FUNCTION;
    sFuncCode->mPosition       = aPosition;
    sFuncCode->mIncludeExprCnt = aLeftExpr->mIncludeExprCnt + aRightExpr->mIncludeExprCnt + 1;
    sFuncCode->mExpr.mFunction = sFunction;
    sFuncCode->mIterationTime  = DTL_ITERATION_TIME_FOR_EACH_EXPR;
    sFuncCode->mColumnName     = NULL;


    /**
     * Left Expr에 Related Function Id 설정
     */

    aLeftExpr->mRelatedFuncId = sFunction->mFuncId;


    /**
     * Right Expr에 Related Function Id 설정
     */

    aRightExpr->mRelatedFuncId = sFunction->mFuncId;


    /**
     * OUTPUT 설정
     */

    *aFuncExpr = sFuncCode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/****************************************************
 * Normal Functions
 ****************************************************/ 

/**
 * @brief catalog, schema, table이 Relation Name과 일치하는지 판별한다.
 * @param[in]  aRelationName        Relation Name
 * @param[in]  aCatalog             Catalog Name
 * @param[in]  aSchema              Schema Name
 * @param[in]  aTable               Table Name
 */
stlBool qlvIsSameTable( qlvRelationName   * aRelationName,
                        stlChar           * aCatalog,
                        stlChar           * aSchema,
                        stlChar           * aTable )
{
    stlBool sIsSame = STL_FALSE;

    if( ( aRelationName->mTable == NULL ) || ( aTable == NULL ) )
    {
        return STL_FALSE;
    }

    if( stlStrcmp( aTable, aRelationName->mTable ) == 0 )
    {
        if( aSchema != NULL )
        {
            if( aCatalog != NULL )
            {
                /* Catalog Name, Schema Name, Table Name이 존재하는 경우
                 * Relation Name에서 Catalog Name, Schema Name, Table Name이
                 * 모두 일치해야 한다.*/
                if( (aRelationName->mCatalog != NULL) &&
                    (aRelationName->mSchema != NULL) )
                {
                    if( (stlStrcmp( aSchema, aRelationName->mSchema ) == 0) &&
                        (stlStrcmp( aCatalog, aRelationName->mCatalog ) == 0) )
                    {
                        sIsSame = STL_TRUE;
                    }
                }
            }
            else
            {
                /* Schema Name, Table Name이 존재하는 경우
                 * Relation Name에서 Schema Name과 Table Name이
                 * 모두 일치해야 한다.
                 * Relation Name에 Catalog Name이 있더라도 무시한다. */
                if( (aRelationName->mSchema != NULL) &&
                    (stlStrcmp( aSchema, aRelationName->mSchema ) == 0) )
                {
                    sIsSame = STL_TRUE;
                }
            }
        }
        else
        {
            /* Table Name만 존재하는 경우
             * Table Name만 일치하면 된다.
             * Relation Name에 Schema Name, Catalog Name이 있더라도 무시한다. */
            sIsSame = STL_TRUE;
        }
    }

    return sIsSame;
}


/**
 * @brief 두 Init Node가 같은 Base Table인지 판별한다.
 */
stlBool qlvIsSameBaseTable( qlvInitNode     * aNode1,
                            qlvInitNode     * aNode2 )
{
    /* NULL 여부 체크 */
    if( (aNode1 == NULL) || (aNode2 == NULL) )
    {
        return STL_FALSE;
    }

    /* Base Table Node 여부 체크 */
    if( (aNode1->mType != QLV_NODE_TYPE_BASE_TABLE) ||
        (aNode2->mType != QLV_NODE_TYPE_BASE_TABLE) )
    {
        return STL_FALSE;
    }

    /* Table 동일한지 체크 */
    return ( aNode1 == aNode2 );
}


/**
 * @brief Table Node에서 catalog, schema, table에 해당하는
 * Table Node를 가져온다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aSourceTableNode     Table Node
 * @param[in]  aObjectName          Object Name
 * @param[out] aTableNode           Table Node
 * @param[out] aTableNodeCount      Table Node Count
 * @param[in]  aEnv                 Environment
 */
stlStatus qlvGetTableNodeForAsterisk( qlvStmtInfo       * aStmtInfo,
                                      qlvInitNode       * aSourceTableNode,
                                      qlpObjectName     * aObjectName,
                                      qlvInitNode      ** aTableNode,
                                      stlInt32          * aTableNodeCount,
                                      qllEnv            * aEnv )
{
    stlInt32                  i                 = 0;
    qlvInitNode             * sTableNode        = NULL;
    qlvInitNode             * sResTableNode     = NULL;
    qlvRelationName         * sRelationName     = NULL;
    stlInt32                  sCount            = 0;
    stlBool                   sIsSame           = STL_FALSE;

    qlvInitJoinTableNode    * sJoinTableNode    = NULL;
    qlvInitNode             * sTempTableNode    = NULL;
    stlInt32                  sTempCount        = 0;

    qlvNamedColumnItem      * sNamedColumnItem  = NULL;
    ellDictHandle             sTempColumnHandle;
    ellDictHandle           * sColumnHandle     = &sTempColumnHandle;
    stlBool                   sExist            = STL_FALSE;

    stlInt32                  sTargetCount      = 0;
    qlvInitTarget           * sTargets          = NULL;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSourceTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aObjectName != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Table Node에서 매칭되는 Table Node 찾기
     **********************************************************/

    sTableNode  = aSourceTableNode;
    sCount      = 0;

    switch( sTableNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
            sRelationName = ((qlvInitBaseTableNode*)sTableNode)->mRelationName;
            sIsSame = qlvIsSameTable( sRelationName,
                                      NULL, /* aObjectName->mCatalog */
                                      aObjectName->mSchema,
                                      aObjectName->mObject );
            if( sIsSame == STL_TRUE )
            {
                sResTableNode = sTableNode;
                sCount++;
            }
            break;

        case QLV_NODE_TYPE_SUB_TABLE:
            sRelationName = ((qlvInitSubTableNode*)sTableNode)->mRelationName;
            if( sRelationName->mTable != NULL )
            {
                if( stlStrcmp( aObjectName->mObject, sRelationName->mTable ) == 0 )
                {
                    if( aObjectName->mSchema != NULL )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        sResTableNode = sTableNode;
                        sCount++;
                    }
                }
                else
                {
                    /* Do Nothing */
                }
            }
            else
            {
                /* Sub-Query에 Alias Name이 없는 경우 */
                /* Do Nothing */
            }
            break;

        case QLV_NODE_TYPE_JOIN_TABLE:
            sJoinTableNode = (qlvInitJoinTableNode*)sTableNode;

            /**
             * Left Table Node 체크
             */

            sTempCount = 0;
            STL_TRY( qlvGetTableNodeForAsterisk( aStmtInfo,
                                                 sJoinTableNode->mLeftTableNode,
                                                 aObjectName,
                                                 &sTempTableNode,
                                                 &sTempCount,
                                                 aEnv )
                     == STL_SUCCESS );
            if( sTempTableNode != NULL )
            {
                sResTableNode = sTempTableNode;
            }
            sCount += sTempCount;


            /**
             * Right Table Node 체크
             */

            sTempCount = 0;
            STL_TRY( qlvGetTableNodeForAsterisk( aStmtInfo,
                                                 sJoinTableNode->mRightTableNode,
                                                 aObjectName,
                                                 &sTempTableNode,
                                                 &sTempCount,
                                                 aEnv )
                     == STL_SUCCESS );
            if( sTempTableNode != NULL )
            {
                sResTableNode = sTempTableNode;
            }
            sCount += sTempCount;

            /**
             * 동일한 테이블이 2개 이상이면 안된다.
             */

            STL_TRY_THROW( sCount < 2, RAMP_ERR_COLUMN_AMBIGUOUS );


            if( sCount == 1 )
            {
                /**
                 * 해당 Table Node를 하나 찾았을 경우
                 * ----------------------------------------------
                 *  Join Specification의 Named Column List가 있다면
                 *  해당 Column 중 하나라도 해당 테이블에 존재해서는 안된다.
                 */

                if( (sJoinTableNode->mJoinSpec != NULL) &&
                    (sJoinTableNode->mJoinSpec->mNamedColumnList != NULL) )
                {
                    /**
                     * Named Column List에 있는 컬럼들중
                     * 찾은 Table Node에 속한 컬럼이 있는지 판별
                     */

                    sNamedColumnItem = sJoinTableNode->mJoinSpec->mNamedColumnList->mHead;
                    while( sNamedColumnItem != NULL )
                    {
                        if( sResTableNode->mType == QLV_NODE_TYPE_BASE_TABLE )
                        {
                            /**
                             * Base Table Node인 경우
                             */

                            sExist = STL_FALSE;
                            STL_TRY( ellGetColumnDictHandle( aStmtInfo->mTransID,
                                                             aStmtInfo->mSQLStmt->mViewSCN,
                                                             & ((qlvInitBaseTableNode*)sResTableNode)->mTableHandle,
                                                             sNamedColumnItem->mName,
                                                             sColumnHandle,
                                                             &sExist,
                                                             ELL_ENV(aEnv) )
                                     == STL_SUCCESS );

                            STL_TRY_THROW( sExist == STL_FALSE, RAMP_ERR_COLUMN_PART_OF_USING_CLAUSE );
                        }
                        else
                        {
                            /**
                             * Sub Table Node인 경우
                             */

                            if( ((qlvInitSubTableNode*)sResTableNode)->mQueryNode->mType == QLV_NODE_TYPE_QUERY_SET )
                            {
                                sTargetCount    = ((qlvInitQuerySetNode*)(((qlvInitSubTableNode*)sResTableNode)->mQueryNode))->mSetTargetCount;
                                sTargets        = ((qlvInitQuerySetNode*)(((qlvInitSubTableNode*)sResTableNode)->mQueryNode))->mSetTargets;
                            }
                            else
                            {
                                sTargetCount    = ((qlvInitQuerySpecNode*)(((qlvInitSubTableNode*)sResTableNode)->mQueryNode))->mTargetCount;
                                sTargets        = ((qlvInitQuerySpecNode*)(((qlvInitSubTableNode*)sResTableNode)->mQueryNode))->mTargets;
                            }

                            sExist = STL_FALSE;
                            for( i = 0; i < sTargetCount; i++ )
                            {
                                if( stlStrcmp( sTargets[i].mDisplayName, sNamedColumnItem->mName ) == 0 )
                                {
                                    sExist = STL_TRUE;
                                    break;
                                }
                            }

                            STL_TRY_THROW( sExist == STL_FALSE, RAMP_ERR_COLUMN_PART_OF_USING_CLAUSE );
                        }

                        sNamedColumnItem = sNamedColumnItem->mNext;
                    }
                }
            }
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }


    /**********************************************************
     * 마무리
     **********************************************************/

    /**
     * Output 설정
     */

    *aTableNode         = sResTableNode;
    *aTableNodeCount    = sCount;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_AMBIGUOUS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_AMBIGUOUS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_COLUMN_PART_OF_USING_CLAUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_PART_OF_USING_CLAUSE,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Reference Table List에서 catalog, schema, table에 해당하는
 * Table Node를 가져온다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aPhraseType          Expr 이 사용된 Phrase 유형
 * @param[in]  aRefTableList        Reference Table List
 * @param[in]  aColumnNameSource    Column Name Source
 * @param[in]  aIsSetOuterMark      Outer Join Operator 설정 여부
 * @param[out] aColumnExpr          Column Expression
 * @param[out] aIsColumn            Is Column
 * @param[in,out]  aIsUpdatable     Updatable Column 여부 
 * @param[in]  aRelatedFuncId       Related Function ID
 * @param[in]  aIterationTime       상위 노드의 Iteration Time
 * @param[in]  aRegionMem           Region Memory
 * @param[in]  aEnv                 Environment
 */
stlStatus qlvGetTableNodeWithColumn( qlvStmtInfo        * aStmtInfo,
                                     qlvExprPhraseType    aPhraseType,
                                     qlvRefTableList    * aRefTableList,
                                     qlpColumnName      * aColumnNameSource,
                                     stlBool              aIsSetOuterMark,
                                     qlvInitExpression ** aColumnExpr,
                                     stlBool            * aIsColumn,
                                     stlBool            * aIsUpdatable,
                                     knlBuiltInFunc       aRelatedFuncId,
                                     dtlIterationTime     aIterationTime,
                                     knlRegionMem       * aRegionMem,
                                     qllEnv             * aEnv )
{
    qlvRefTableItem     * sRefTableItem     = NULL;
    qlvInitExpression   * sColumnExpr       = NULL;
    stlBool               sExist            = STL_FALSE;
    stlBool               sIsOuterColumn    = STL_FALSE;

    qlvInitOuterColumn  * sOuterColumn      = NULL;
    qlvInitExpression   * sTempColumnExpr   = NULL;

    stlInt32              i                 = 0;
    stlBool               sIsRootTableNode  = STL_TRUE;


    /**********************************************************
     * Paramter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefTableList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefTableList->mCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnNameSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Outer Join Operator Check #1
     * => Outer Join Operator는 Where절에만 올 수 있음을 판단한다.
     */
    if( (aIsSetOuterMark == STL_TRUE) &&
        (aPhraseType != QLV_EXPR_PHRASE_CONDITION) )
    {
        switch( aPhraseType )
        {
            case QLV_EXPR_PHRASE_TARGET:
                STL_THROW( RAMP_ERR_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN_SELECT_LIST );
                break;
            case QLV_EXPR_PHRASE_FROM:
                STL_THROW( RAMP_ERR_CANNOT_USE_OUTER_JOIN_OPERATOR_WITH_ANSI_JOINS );
                break;
            case QLV_EXPR_PHRASE_HAVING:
                STL_THROW( RAMP_ERR_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN_HAVING );
                break;
            case QLV_EXPR_PHRASE_GROUPBY:
                STL_THROW( RAMP_ERR_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN_GROUPBY );
                break;
            case QLV_EXPR_PHRASE_ORDERBY:
                STL_THROW( RAMP_ERR_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN_ORDERBY );
                break;
            case QLV_EXPR_PHRASE_OFFSETLIMIT:
                STL_DASSERT( 0 );
                break;
            case QLV_EXPR_PHRASE_RETURN:
                STL_THROW( RAMP_ERR_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN_RETURN );
                break;
            case QLV_EXPR_PHRASE_DEFAULT:
                STL_DASSERT( 0 );
                break;
            default:
                STL_DASSERT( 0 );
                break;
        }
    }

    /* Reference Table List에서 매칭되는 Table Reference 찾기 */
    sRefTableItem = aRefTableList->mHead;
    while( sRefTableItem != NULL )
    {
        /**
         * Reference Table Item의 Target이 존재하는 경우
         * Target에서 해당 컬럼이 존재하는지 먼저 찾아본다.
         * 이때 Column Name Source는 Column Name만 존재해야 한다.
         * (table.column 형식 안됨)
         */
        if( sRefTableItem->mTargetCount > 0 )
        {
            STL_DASSERT( sRefTableItem->mTargets != NULL );

            /**
             * Column Name만 존재해야 한다. table.column 형식 안됨
             */
            if( /* (aColumnNameSource->mCatalog == NULL) && */
                (aColumnNameSource->mSchema == NULL) &&
                (aColumnNameSource->mTable == NULL) )
            {
                sExist = STL_FALSE;
                for( i = 0; i < sRefTableItem->mTargetCount; i++ )
                {
                    if( stlStrcmp( aColumnNameSource->mColumn,
                                   sRefTableItem->mTargets[i].mDisplayName )
                        == 0 )

                    {
                        STL_TRY_THROW( sExist == STL_FALSE, RAMP_ERR_COLUMN_AMBIGUOUS );

                        sExist = STL_TRUE;

                        STL_TRY( qlvCopyExpr( sRefTableItem->mTargets[i].mExpr,
                                              & sColumnExpr,
                                              aRegionMem,
                                              aEnv )
                                 == STL_SUCCESS );

                        sColumnExpr->mRelatedFuncId = aRelatedFuncId;

                    }
                }

                if( sExist == STL_TRUE )
                {
                    STL_THROW( RAMP_FINISH );
                }
            }
        }

        STL_TRY( qlvGetTableNodeWithColumnInternal( aStmtInfo,
                                                    aPhraseType,
                                                    sRefTableItem->mTableNode,
                                                    aColumnNameSource,
                                                    STL_TRUE,
                                                    sIsRootTableNode,
                                                    &sColumnExpr,
                                                    &sExist,
                                                    aIsUpdatable,
                                                    aRegionMem,
                                                    aEnv )
                 == STL_SUCCESS );

        if( sExist == STL_TRUE )
        {
            /* Set Outer Mark 여부를 설정 */
            if( sColumnExpr->mType == QLV_EXPR_TYPE_COLUMN )
            {
                sColumnExpr->mExpr.mColumn->mIsSetOuterMark = aIsSetOuterMark;
            }
            else if( sColumnExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
            {
                sColumnExpr->mExpr.mInnerColumn->mIsSetOuterMark = aIsSetOuterMark;
            }
            else
            {
                STL_DASSERT( sColumnExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN );
                sColumnExpr->mExpr.mRowIdColumn->mIsSetOuterMark = aIsSetOuterMark;
            }


            /**
             * 현재 Level이 아닌 상위 Level의 Table에 존재하는
             * Column을 접근하는 경우 Outer Column Expression으로
             * Column Expr을 만들어준다.
             */

            if( sIsOuterColumn == STL_TRUE )
            {
                /**
                 * Outer Join Operator Check #2
                 * => Outer Join Operator는 외부 Query Block의 Column을 참조할 수 없음을 판단한다.
                 */
                STL_TRY_THROW( aIsSetOuterMark == STL_FALSE,
                               RAMP_ERR_OUTER_JOIN_CANNOT_BE_SPECIFIED_ON_CORRELATION_COLUMN );

                /**
                 * Target Expr 설정 : OUTER COLUMN 생성
                 */

                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            STL_SIZEOF( qlvInitOuterColumn ),
                                            (void **) & sOuterColumn,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                /**
                 * sColumnExpr은 Column을 참조하는 것이므로
                 * Column 또는 Inner Column 중 하나이다.
                 */

                if( sColumnExpr->mType == QLV_EXPR_TYPE_COLUMN )
                {
                    sOuterColumn->mRelationNode = sColumnExpr->mExpr.mColumn->mRelationNode;

                    STL_TRY( qlvCopyExpr( sColumnExpr,
                                          & sOuterColumn->mOrgExpr,
                                          aRegionMem,
                                          aEnv )
                             == STL_SUCCESS );
                }
                else if( sColumnExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
                {
                    sOuterColumn->mRelationNode = sColumnExpr->mExpr.mInnerColumn->mRelationNode;

                    STL_TRY( qlvCopyExpr( sColumnExpr,
                                          & sOuterColumn->mOrgExpr,
                                          aRegionMem,
                                          aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_DASSERT( sColumnExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN );

                    sOuterColumn->mRelationNode = sColumnExpr->mExpr.mRowIdColumn->mRelationNode;

                    STL_TRY( qlvCopyExpr( sColumnExpr,
                                          & sOuterColumn->mOrgExpr,
                                          aRegionMem,
                                          aEnv )
                             == STL_SUCCESS );
                }
                
                /**
                 * Target Expr 설정 : CODE EXPRESSION 생성
                 */

                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            STL_SIZEOF( qlvInitExpression ),
                                            (void **) & sTempColumnExpr,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemset( sTempColumnExpr,
                           0x00,
                           STL_SIZEOF( qlvInitExpression ) );

                QLV_VALIDATE_INCLUDE_EXPR_CNT( sColumnExpr->mIncludeExprCnt + 1, aEnv );
                
                sTempColumnExpr->mType              = QLV_EXPR_TYPE_OUTER_COLUMN;
                sTempColumnExpr->mPosition          = aColumnNameSource->mNodePos;
                sTempColumnExpr->mIncludeExprCnt    = sColumnExpr->mIncludeExprCnt + 1;
                sTempColumnExpr->mExpr.mOuterColumn = sOuterColumn;
                sTempColumnExpr->mRelatedFuncId     = aRelatedFuncId;
                sTempColumnExpr->mIterationTime     = sColumnExpr->mIterationTime;
                sTempColumnExpr->mColumnName        = NULL;


                /**
                 * Outer Column의 Expression을 결과 Column Expr로 설정
                 */

                sColumnExpr = sTempColumnExpr;
            }
            else
            {
                /**
                 * 정해진 Iteration Time은 유지한다.
                 * => sColumnExpr으로 어떠한 타입의 expression이 올 수도 있기 때문
                 */
                
                sColumnExpr->mRelatedFuncId = aRelatedFuncId;
            }
            break;
        }

        sIsOuterColumn   = STL_TRUE;
        sRefTableItem    = sRefTableItem->mNext;
        sIsRootTableNode = STL_FALSE;
    }

    STL_RAMP( RAMP_FINISH );


    /**
     * Outer Join Operator Check #3
     * => Outer Join Operator는 ANSI join에서 기술할 수 없음을 판단한다.
     */
    if( (aIsSetOuterMark == STL_TRUE) &&
        (sExist == STL_TRUE) )
    {
        STL_TRY_THROW( qlvHasANSIJoin( sRefTableItem->mTableNode ) == STL_FALSE,
                       RAMP_ERR_CANNOT_USE_OUTER_JOIN_OPERATOR_WITH_ANSI_JOINS );
    }


    /**********************************************************
     * 마무리
     **********************************************************/
    
    /**
     * Output 설정
     */

    *aColumnExpr    = sColumnExpr;
    *aIsColumn      = sExist;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_AMBIGUOUS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_AMBIGUOUS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnNameSource->mColumnPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_CANNOT_USE_OUTER_JOIN_OPERATOR_WITH_ANSI_JOINS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_USE_OUTER_JOIN_OPERATOR_WITH_ANSI_JOINS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnNameSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN_SELECT_LIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnNameSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "select-list" );
    }

    STL_CATCH( RAMP_ERR_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN_HAVING )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnNameSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "HAVING clause" );
    }

    STL_CATCH( RAMP_ERR_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN_GROUPBY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnNameSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "GROUPBY clause" );
    }

    STL_CATCH( RAMP_ERR_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN_ORDERBY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnNameSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "ORDERBY clause" );
    }

    STL_CATCH( RAMP_ERR_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN_RETURN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_OUTER_JOIN_OPERATOR_NOT_ALLOWED_IN,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnNameSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "RETURNING INTO clause" );
    }

    STL_CATCH( RAMP_ERR_OUTER_JOIN_CANNOT_BE_SPECIFIED_ON_CORRELATION_COLUMN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_OUTER_JOIN_CANNOT_BE_SPECIFIED_ON_CORRELATION_COLUMN,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnNameSource->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;

}

/**
 * @brief Table Node를 탐색하며 catalog, schema, table에 해당하는
 * Table Node를 가져온다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aPhraseType          Expr 이 사용된 Phrase 유형
 * @param[in]  aTableNode           Table Node
 * @param[in]  aColumnNameSource    Column Name Source
 * @param[in]  aNeedCopy            Expr의 copy 필요여부
 * @param[in]  aIsRootTableNode     Root Table Node 인지 여부
 * @param[out] aColumnExpr          Column Expression
 * @param[out] aExist               존재 여부
 * @param[in,out]  aIsUpdatable     Updatable Column 여부 
 * @param[in]  aRegionMem           Region Memory
 * @param[in]  aEnv                 Environment
 */
stlStatus qlvGetTableNodeWithColumnInternal( qlvStmtInfo        * aStmtInfo,
                                             qlvExprPhraseType    aPhraseType,
                                             qlvInitNode        * aTableNode,
                                             qlpColumnName      * aColumnNameSource,
                                             stlBool              aNeedCopy,
                                             stlBool              aIsRootTableNode,
                                             qlvInitExpression ** aColumnExpr,
                                             stlBool            * aExist,
                                             stlBool            * aIsUpdatable,
                                             knlRegionMem       * aRegionMem,
                                             qllEnv             * aEnv )
{
    stlBool                   sExist            = STL_FALSE;

    qlvRefExprItem          * sRefColumnItem    = NULL;

    qlvInitExpression       * sColumnExpr       = NULL;
    qlvInitExpression       * sTempExpr         = NULL;

    stlBool                   sLeftExist        = STL_FALSE;
    stlBool                   sRightExist       = STL_FALSE;

    qlvInitExpression       * sLeftColumnExpr   = NULL;
    qlvInitExpression       * sRightColumnExpr  = NULL;

    qlvInitBaseTableNode    * sBaseTableNode    = NULL;
    qlvInitSubTableNode     * sSubTableNode     = NULL;
    qlvInitJoinTableNode    * sJoinTableNode    = NULL;

    qlvNamedColumnItem      * sNamedColumnItem  = NULL;


    /**********************************************************
     * Paramter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnNameSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Table Node에서 해당 Column이 존재하는지 찾기
     **********************************************************/

    STL_TRY_THROW( aTableNode != NULL, RAMP_FINISH );

    switch( aTableNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
            sBaseTableNode = (qlvInitBaseTableNode*)aTableNode;

            if( aColumnNameSource->mTable != NULL )
            {
                /**
                 * Table Name이 존재하는 경우
                 */

                sExist = qlvIsSameTable( sBaseTableNode->mRelationName,
                                         NULL, /* aColumnNameSource->mCatalog */
                                         aColumnNameSource->mSchema,
                                         aColumnNameSource->mTable );

                if( sExist == STL_TRUE )
                {
                    STL_TRY( qlvFindOrAddColumnOnBaseTable( aStmtInfo,
                                                            aPhraseType,
                                                            aRegionMem,
                                                            aColumnNameSource->mColumn,
                                                            aColumnNameSource->mColumnPos,
                                                            sBaseTableNode,
                                                            aNeedCopy,
                                                            &sExist,
                                                            aIsUpdatable,
                                                            &sRefColumnItem,
                                                            aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_THROW( RAMP_FINISH );
                }
            }
            else
            {
                /**
                 * Column Name만 있는 경우
                 */

                STL_TRY( qlvFindOrAddColumnOnBaseTable( aStmtInfo,
                                                        aPhraseType,
                                                        aRegionMem,
                                                        aColumnNameSource->mColumn,
                                                        aColumnNameSource->mColumnPos,
                                                        sBaseTableNode,
                                                        aNeedCopy,
                                                        &sExist,
                                                        aIsUpdatable,
                                                        &sRefColumnItem,
                                                        aEnv )
                         == STL_SUCCESS );
            }

            if( sExist == STL_TRUE )
            {
                sColumnExpr = sRefColumnItem->mExprPtr;
            }
            else
            {
                /**
                 * ROWID Pseudo Column 인 경우,
                 */

                if( stlStrcmp( aColumnNameSource->mColumn, "ROWID" ) == 0 )
                {
                    *aIsUpdatable = STL_FALSE;
                    
                    STL_TRY( qlvMakeAndAddRowIdColumnOnBaseTable( aStmtInfo,
                                                                  aPhraseType,
                                                                  aRegionMem,
                                                                  aColumnNameSource->mColumnPos,
                                                                  sBaseTableNode,
                                                                  aNeedCopy,
                                                                  & sExist,
                                                                  & sColumnExpr,
                                                                  aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /* Do Nothing */
                }
            }

            break;

        case QLV_NODE_TYPE_SUB_TABLE:

            sSubTableNode = (qlvInitSubTableNode*)aTableNode;
            if( aColumnNameSource->mTable != NULL )
            {
                if( aColumnNameSource->mSchema != NULL )
                {
                    /* Schema Name 및 Table Name이 존재하는 경우
                     * Sub-Table은 Alias만 가질 수 있으므로
                     * Schema Name은 올 수 없다. */

                    /* Do Nothing */
                }
                else
                {
                    /**
                     * Schema Name은 존재하지 않고 Table Name만 존재하는 경우
                     */

                    if( sSubTableNode->mRelationName->mTable == NULL )
                    {
                        sExist = STL_FALSE;
                    }
                    else
                    {
                        if( stlStrcmp( aColumnNameSource->mTable, sSubTableNode->mRelationName->mTable ) == 0 )
                        {
                            /* Table Name이 일치하는 경우
                             * Sub-Table의 Target들로부터 Column을 찾는다. */

                            STL_TRY( qlvFindOrAddColumnOnSubTable( aStmtInfo,
                                                                   aPhraseType,
                                                                   aRegionMem,
                                                                   aColumnNameSource->mColumn,
                                                                   aColumnNameSource->mColumnPos,
                                                                   sSubTableNode,
                                                                   aNeedCopy,
                                                                   &sExist,
                                                                   aIsUpdatable,
                                                                   &sRefColumnItem,
                                                                   aEnv )
                                     == STL_SUCCESS );
                        }
                    }
                }
            }
            else
            {
                /* Column Name만 존재하는 경우
                 * Sub-Table의 Target들로부터 Column을 찾는다. */

                STL_TRY( qlvFindOrAddColumnOnSubTable( aStmtInfo,
                                                       aPhraseType,
                                                       aRegionMem,
                                                       aColumnNameSource->mColumn,
                                                       aColumnNameSource->mColumnPos,
                                                       sSubTableNode,
                                                       aNeedCopy,
                                                       &sExist,
                                                       aIsUpdatable,
                                                       &sRefColumnItem,
                                                       aEnv )
                         == STL_SUCCESS );
            }

            if( sExist == STL_TRUE )
            {
                sColumnExpr = sRefColumnItem->mExprPtr;
            }

            break;

        case QLV_NODE_TYPE_JOIN_TABLE:
            sJoinTableNode = (qlvInitJoinTableNode*)aTableNode;

            if( aColumnNameSource->mTable != NULL )
            {
                /**
                 * Table Name이 존재하는 경우
                 */

                /**
                 * Using 절에 column이 존재하는지 체크
                 */

                if( (sJoinTableNode->mJoinSpec != NULL) &&
                    (sJoinTableNode->mJoinSpec->mNamedColumnList != NULL) )
                {
                    sNamedColumnItem = sJoinTableNode->mJoinSpec->mNamedColumnList->mHead;
                    while( sNamedColumnItem != NULL )
                    {
                        /**
                         * Using 절에 column이 존재하는 경우 에러임
                         */

                        STL_TRY_THROW( stlStrcmp( sNamedColumnItem->mName, aColumnNameSource->mColumn ) != 0,
                                       RAMP_ERR_COLUMN_PART_OF_USING_CLAUSE );

                        sNamedColumnItem = sNamedColumnItem->mNext;
                    }
                }

                if( ( aPhraseType != QLV_EXPR_PHRASE_FROM ) &&
                    ( ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_RIGHT_SEMI ) ||
                      ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI ) ||
                      ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_RIGHT_ANTI_SEMI ) ||
                      ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA ) ) )
                {
                    /* Do Nothing */
                }
                else
                {
                    /* Left Table Node */
                    STL_TRY( qlvGetTableNodeWithColumnInternal( aStmtInfo,
                                                                aPhraseType,
                                                                sJoinTableNode->mLeftTableNode,
                                                                aColumnNameSource,
                                                                STL_FALSE,
                                                                aIsRootTableNode,
                                                                &sLeftColumnExpr,
                                                                &sLeftExist,
                                                                aIsUpdatable,
                                                                aRegionMem,
                                                                aEnv )
                             == STL_SUCCESS );
                }
                
                if( ( aPhraseType != QLV_EXPR_PHRASE_FROM ) &&
                    ( ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_LEFT_SEMI ) ||
                      ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_INVERTED_LEFT_SEMI ) ||
                      ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_LEFT_ANTI_SEMI ) ||
                      ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA ) ) )
                {
                    /* Do Nothing */
                }
                else
                {
                    /* Right Table Node */
                    STL_TRY( qlvGetTableNodeWithColumnInternal( aStmtInfo,
                                                                aPhraseType,
                                                                sJoinTableNode->mRightTableNode,
                                                                aColumnNameSource,
                                                                STL_FALSE,
                                                                aIsRootTableNode,
                                                                &sRightColumnExpr,
                                                                &sRightExist,
                                                                aIsUpdatable,
                                                                aRegionMem,
                                                                aEnv )
                             == STL_SUCCESS );
                }
                
                /**
                 * Left와 Right에 모두 컬럼이 존재하면 Ambiguous
                 */

                STL_TRY_THROW( ((sLeftExist == STL_FALSE) || (sRightExist == STL_FALSE)),
                               RAMP_ERR_COLUMN_AMBIGUOUS );


                /**
                 * Left 혹은 Right에 존재하는 Column Expression을
                 * Join Table Node의 Reference Column List에 넣는다.
                 */

                if( sLeftExist == STL_TRUE )
                {
                    sExist = STL_TRUE;

                    /**
                     * Join Reference Column List에 추가
                     */

                    if( ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_RIGHT_SEMI ) ||
                        ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI ) ||
                        ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_RIGHT_ANTI_SEMI ) ||
                        ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA ) )
                    {
                        sColumnExpr = sLeftColumnExpr;
                        STL_THROW( RAMP_FINISH );
                    }
                    else
                    {
                        STL_TRY( qlvAddRefColumnItemOnJoinTable( aRegionMem,
                                                                 aPhraseType,
                                                                 aColumnNameSource->mNodePos,
                                                                 sJoinTableNode,
                                                                 STL_TRUE,
                                                                 sLeftColumnExpr,
                                                                 aNeedCopy,
                                                                 &sRefColumnItem,
                                                                 aEnv )
                                 == STL_SUCCESS );
                    }
                }
                else if( sRightExist == STL_TRUE )
                {
                    sExist = STL_TRUE;
                    
                    /**
                     * Join Reference Column List에 추가
                     */

                    if( ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_LEFT_SEMI ) ||
                        ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_INVERTED_LEFT_SEMI ) ||
                        ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_LEFT_ANTI_SEMI ) ||
                        ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA ) )
                    {
                        sColumnExpr = sRightColumnExpr;
                        STL_THROW( RAMP_FINISH );
                    }
                    else
                    {
                        STL_TRY( qlvAddRefColumnItemOnJoinTable( aRegionMem,
                                                                 aPhraseType,
                                                                 aColumnNameSource->mNodePos,
                                                                 sJoinTableNode,
                                                                 STL_FALSE,
                                                                 sRightColumnExpr,
                                                                 aNeedCopy,
                                                                 &sRefColumnItem,
                                                                 aEnv )
                                 == STL_SUCCESS );
                    }
                }
            }
            else
            {
                /**
                 * Column Name만 있는 경우
                 */

                STL_TRY( qlvFindOrAddColumnOnJoinTable( aStmtInfo,
                                                        aPhraseType,
                                                        aRegionMem,
                                                        aColumnNameSource->mColumn,
                                                        aColumnNameSource->mColumnPos,
                                                        sJoinTableNode,
                                                        aNeedCopy,
                                                        &sExist,
                                                        aIsUpdatable,
                                                        &sRefColumnItem,
                                                        aEnv )
                         == STL_SUCCESS );
            }

            if( sExist == STL_TRUE )
            {
                switch ( sJoinTableNode->mJoinType )
                {
                    case QLV_JOIN_TYPE_NONE:
                    case QLV_JOIN_TYPE_CROSS:
                        {
                            break;
                        }
                    case QLV_JOIN_TYPE_INNER:
                        {
                            if ( sJoinTableNode->mJoinSpec != NULL )
                            {
                                if ( sJoinTableNode->mJoinSpec->mNamedColumnList != NULL )
                                {
                                    /**
                                     * INNER JOIN .. USING
                                     */
                                    
                                    *aIsUpdatable = STL_FALSE;
                                }
                            }
                            else
                            {
                                /* nothing to do */
                            }
                            break;
                        }
                    case QLV_JOIN_TYPE_LEFT_OUTER:
                    case QLV_JOIN_TYPE_RIGHT_OUTER:
                    case QLV_JOIN_TYPE_FULL_OUTER:
                    case QLV_JOIN_TYPE_LEFT_SEMI:
                    case QLV_JOIN_TYPE_RIGHT_SEMI:
                    case QLV_JOIN_TYPE_INVERTED_LEFT_SEMI:
                    case QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI:
                    case QLV_JOIN_TYPE_LEFT_ANTI_SEMI:
                    case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI:
                    case QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA:
                    case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA:
                        {
                            *aIsUpdatable = STL_FALSE;
                            break;
                        }
                    default:
                        {
                            STL_DASSERT(0);
                            break;
                        }
                }
                
                sColumnExpr = sRefColumnItem->mExprPtr;
            }
            break;
        case QLV_NODE_TYPE_QUERY_SET:
            {
                /**
                 * SET 절의 ORDER BY 만 expresion 을 validation 한다.
                 */
                STL_DASSERT( aPhraseType == QLV_EXPR_PHRASE_ORDERBY );

                STL_THROW( RAMP_ERR_COLUMN_AMBIGUOUS );
            }
            break;
        case QLV_NODE_TYPE_GROUP_HASH_INSTANT:
            {
                STL_TRY( qlvGetTableNodeWithColumnInternal( aStmtInfo,
                                                            aPhraseType,
                                                            ((qlvInitInstantNode*)aTableNode)->mTableNode,
                                                            aColumnNameSource,
                                                            aNeedCopy,
                                                            aIsRootTableNode,
                                                            & sColumnExpr,
                                                            & sExist,
                                                            aIsUpdatable,
                                                            aRegionMem,
                                                            aEnv )
                         == STL_SUCCESS );

                

                if( ( sExist == STL_TRUE ) && ( aIsRootTableNode == STL_FALSE ) )
                {
                    STL_TRY( qlnoIsExistExprFromInstant( ((qlvInitInstantNode*)aTableNode),
                                                         sColumnExpr,
                                                         STL_TRUE,
                                                         & sExist,
                                                         & sTempExpr,
                                                         aRegionMem,
                                                         aEnv )
                             == STL_SUCCESS );

                    STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_NOT_A_GROUP_FUNCTION );
                }
                
                break;
            }
        default:
            STL_DASSERT( 0 );
            break;
    }

    STL_RAMP( RAMP_FINISH );
    
    *aColumnExpr    = sColumnExpr;
    *aExist         = sExist;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_AMBIGUOUS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_AMBIGUOUS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnNameSource->mColumnPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_COLUMN_PART_OF_USING_CLAUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_PART_OF_USING_CLAUSE,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnNameSource->mColumnPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_NOT_A_GROUP_FUNCTION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_A_GROUP_FUNCTION,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnNameSource->mColumnPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Multi-Tables에 대하여 모든 Target의 개수를 구한다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aTableNode           Table Node
 * @param[in]  aExceptNameCount     Except Name Count
 * @param[in]  aExceptNameArr       Except Name Array
 * @param[out] aTargetCount         Total Target Count
 * @param[in]  aEnv                 Environment
 */
stlStatus qlvGetAsteriskTargetCountMultiTables( qlvStmtInfo     * aStmtInfo,
                                                qlvInitNode     * aTableNode,
                                                stlInt32          aExceptNameCount,
                                                stlChar        ** aExceptNameArr,
                                                stlInt32        * aTargetCount,
                                                qllEnv          * aEnv )
{
    stlInt32                  i                 = 0;
    stlInt32                  j                 = 0;
    stlBool                   sExist            = STL_FALSE;
    ellDictHandle             sTempColumnHandle;
    ellDictHandle           * sColumnHandle     = &sTempColumnHandle;
    ellDictHandle           * sTableColumnHandle = NULL;

    stlInt32                  sTargetCount      = 0;
    stlInt32                  sValidTargetCount = 0;
    qlvInitTarget           * sTargets          = NULL;

    qlvInitJoinTableNode    * sJoinTableNode    = NULL;

    stlInt32                  sExceptNameCount  = 0;
    stlChar                ** sExceptNameArr    = NULL;
    qlvNamedColumnItem      * sNamedColumnItem  = NULL;


    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**********************************************************
     * Table Node의 Target 개수를 찾아 더한다.
     **********************************************************/

    switch( aTableNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
            /**
             * Base Table Node인 경우
             * ---------------------------------------------------
             *  전체 컬럼 개수에서 Except Name Array에 컬럼명이 
             *  존재하는 컬럼의 개수를 제외한 수를 더한다.
             *  이유) Except Name Array에는 Using 구문에 의해
             *        Target으로 설정된 컬럼명이 들어있기 때문이다.
             */

            sTargetCount = ellGetTableColumnCnt( & ((qlvInitBaseTableNode*)aTableNode)->mTableHandle );
            sTableColumnHandle = ellGetTableColumnDictHandle( & ((qlvInitBaseTableNode*)aTableNode)->mTableHandle );

            sValidTargetCount = 0;
            for ( i = 0; i < sTargetCount; i++ )
            {
                if ( ellGetColumnUnused( & sTableColumnHandle[i] ) == STL_TRUE )
                {
                    /**
                     * UNUSED column 임
                     */
                }
                else
                {
                    sValidTargetCount++;
                }
            }
            
            *aTargetCount += sValidTargetCount;
            
            for( i = 0; i < aExceptNameCount; i++ )
            {
                STL_TRY( ellGetColumnDictHandle( aStmtInfo->mTransID,
                                                 aStmtInfo->mSQLStmt->mViewSCN,
                                                 & ((qlvInitBaseTableNode*)aTableNode)->mTableHandle,
                                                 aExceptNameArr[i],
                                                 sColumnHandle,
                                                 &sExist,
                                                 ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                if( sExist == STL_TRUE )
                {
                    *aTargetCount -= 1;
                }
            }
            break;

        case QLV_NODE_TYPE_SUB_TABLE:
            /**
             * Sub Table Node인 경우
             * ----------------------------------------------------
             *  Subquery의 Target에 있는 개수에서 Except Name Array에 컬럼명이 
             *  존재하는 컬럼의 개수를 제외한 수를 더한다.
             *  이유) Except Name Array에는 Using 구문에 의해
             *        Target으로 설정된 컬럼명이 들어있기 때문이다.
             */

            sTargetCount    = ((qlvInitSubTableNode*)aTableNode)->mColumnCount;
            sTargets        = ((qlvInitSubTableNode*)aTableNode)->mColumns;

            *aTargetCount += sTargetCount;

            for( i = 0; i < aExceptNameCount; i++ )
            {
                for( j = 0; j < sTargetCount; j++ )
                {
                    if( stlStrcmp( sTargets[j].mDisplayName, aExceptNameArr[i] ) == 0 )
                    {
                        *aTargetCount -= 1;
                        break;
                    }
                }
            }
            break;

        case QLV_NODE_TYPE_JOIN_TABLE:
            /**
             * Join Table Node인 경우
             * -----------------------------------------------------
             *  Using 구문이 있는 경우 Named Column List의
             *  컬럼 개수만큼 Target Count를 증가시키고,
             *  Left Table Node와 Right Table Node에 대하여
             *  Target Count를 구한다.
             *  이때 Except Name List에 Named Column List의 컬럼들을
             *  추가하여 넘겨준다.
             */

            sExceptNameArr      = aExceptNameArr;
            sExceptNameCount    = aExceptNameCount;

            sJoinTableNode = (qlvInitJoinTableNode*)aTableNode;
            if( (sJoinTableNode->mJoinSpec != NULL) &&
                (sJoinTableNode->mJoinSpec->mNamedColumnList != NULL) )
            {
                /**
                 * Named Column List가 존재하는 경우
                 */

                /**
                 * ExceptName List에 Named ColumnList를 추가
                 */

                sNamedColumnItem = sJoinTableNode->mJoinSpec->mNamedColumnList->mHead;
                while( sNamedColumnItem != NULL )
                {
                    /* 이미 동일한 이름이 존재하는지 검사 */
                    for( i = 0; i < sExceptNameCount; i++ )
                    {
                        if( stlStrcmp( sExceptNameArr[i], sNamedColumnItem->mName ) == 0 )
                        {
                            break;
                        }
                    }

                    if( i == sExceptNameCount )
                    {
                        sExceptNameArr[sExceptNameCount] = sNamedColumnItem->mName;
                        sExceptNameCount++;
                        *aTargetCount += 1;
                   }

                    sNamedColumnItem = sNamedColumnItem->mNext;
                }
            }


            /**
             * Left Table Node
             */

            if( (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_RIGHT_SEMI) &&
                (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI) &&
                (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_RIGHT_ANTI_SEMI) &&
                (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA) )
            {
                STL_TRY( qlvGetAsteriskTargetCountMultiTables( aStmtInfo,
                                                               sJoinTableNode->mLeftTableNode,
                                                               sExceptNameCount,
                                                               sExceptNameArr,
                                                               aTargetCount,
                                                               aEnv )
                         == STL_SUCCESS );
            }


            /**
             * Right Table Node
             */

            if( (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_LEFT_SEMI) &&
                (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_INVERTED_LEFT_SEMI) &&
                (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_LEFT_ANTI_SEMI) &&
                (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA) )
            {
                STL_TRY( qlvGetAsteriskTargetCountMultiTables( aStmtInfo,
                                                               sJoinTableNode->mRightTableNode,
                                                               sExceptNameCount,
                                                               sExceptNameArr,
                                                               aTargetCount,
                                                               aEnv )
                         == STL_SUCCESS );
            }
            break;

        default:
            STL_DASSERT(0);
            break;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Target Source List에 포함되는 모든 Target의 개수를 구한다.
 *        <BR> table_name.*와 같이 특정 테이블의 모든 컬럼을 Target으로 할 수 있다.
 * @param[in]  aStmtInfo        Stmt Information
 * @param[in]  aTargetSource    Target Source List
 * @param[in]  aTableNode       Table Node
 * @param[in]  aExceptNameCount Except Column Name Count
 * @param[in]  aExceptNameArr   Except Column Name Array
 * @param[out] aTargetCount     Total Target Count
 * @param[in]  aEnv             Environment
 */
stlStatus qlvGetTargetCountFromTargetList( qlvStmtInfo      * aStmtInfo,
                                           qlpList          * aTargetSource,
                                           qlvInitNode      * aTableNode,
                                           stlInt32           aExceptNameCount,
                                           stlChar         ** aExceptNameArr,
                                           stlInt32         * aTargetCount,
                                           qllEnv           * aEnv )
{
    stlInt32          sCount            = 0;
    stlInt32          sTargetCount      = 0;
    qllNode         * sTargetNode       = NULL;
    qlpListElement  * sListElem         = NULL;
    qlpObjectName   * sObjectName       = NULL;
    qlvInitNode     * sTableNode        = NULL;
    stlInt32          sTableNodeCount   = 0;


    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**********************************************************
     * Target List의 Target 개수를 찾아 더한다.
     **********************************************************/

    sTargetCount = 0;
    QLP_LIST_FOR_EACH( aTargetSource, sListElem )
    {
        sTargetNode = (qllNode *) QLP_LIST_GET_POINTER_VALUE( sListElem );

        if( sTargetNode->mType == QLL_TARGET_TYPE )
        {
            sTargetCount++;
        }
        else if( sTargetNode->mType == QLL_TARGET_ASTERISK_TYPE )
        {
            /**
             * TableName.* 인 경우
             */

            sCount          = 0;
            sObjectName     = ((qlpTarget*)sTargetNode)->mRelName;
            sTableNodeCount = 0;
            STL_TRY( qlvGetTableNodeForAsterisk( aStmtInfo,
                                                 aTableNode,
                                                 sObjectName,
                                                 &sTableNode,
                                                 &sTableNodeCount,
                                                 aEnv )
                     == STL_SUCCESS );

            STL_TRY_THROW( sTableNode != NULL, RAMP_ERR_TABLE_NOT_EXIST );
            STL_DASSERT( sTableNodeCount == 1 );

            STL_TRY( qlvGetAsteriskTargetCountMultiTables( aStmtInfo,
                                                           sTableNode,
                                                           aExceptNameCount,
                                                           aExceptNameArr,
                                                           &sCount,
                                                           aEnv )
                     == STL_SUCCESS );

            sTargetCount += sCount;
        }
        else
        {
            STL_DASSERT( 0 );
        }
    }


    /**********************************************************
     * 마무리
     **********************************************************/

    /**
     * Output 설정
     */

    *aTargetCount = sTargetCount;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TABLE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_IDENTIFIER,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sTargetNode->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mObject );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Multi-Tables의 모든 컬럼들에 대하여 validation 한다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aPhraseType          Expr이 사용된 Phrase 유형
 * @param[in]  aTableNode           Table Node
 * @param[in]  aTargetSource        Target Source
 * @param[in]  aExceptNameCount     Except Column Name Count
 * @param[in]  aExceptNameArr       Except Columy Name Array
 * @param[in]  aTargets             Target List
 * @param[out] aTargetCount         Total Target Count
 * @param[in]  aEnv                 Environment
 * @remarks TargetSource가 QLL_TARGET_ASTERISK_TYPE인 경우
 *          Relation Name에 해당하는 테이블의 Column들만 모두 추가한다.
 */
stlStatus qlvValidateAsteriskMultiTables( qlvStmtInfo         * aStmtInfo,
                                          qlvExprPhraseType     aPhraseType,
                                          qlvInitNode         * aTableNode,
                                          qlpTarget           * aTargetSource,
                                          stlInt32              aExceptNameCount,
                                          stlChar            ** aExceptNameArr,
                                          qlvInitTarget       * aTargets,
                                          stlInt32            * aTargetCount,
                                          qllEnv              * aEnv )
{
    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetSource != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetSource->mType == QLL_TARGET_ASTERISK_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Table Node별 Asterisk Validation
     **********************************************************/

    switch( aTableNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
            /**
             * Base Table이면
             * 해당 테이블에 존재하는 모든 컬럼들에서
             * Except Column Name List의 컬럼들을 제외하고 target에 추가한다.
             */
            STL_TRY( qlvValidateAsteriskBaseTable( aStmtInfo,
                                                   aPhraseType,
                                                   aTableNode,
                                                   aTargetSource,
                                                   aExceptNameCount,
                                                   aExceptNameArr,
                                                   aTargets,
                                                   aTargetCount,
                                                   aEnv )
                     == STL_SUCCESS );
            break;

        case QLV_NODE_TYPE_SUB_TABLE:
            /**
             * Sub Table은 해당 테이블에 존재하는
             * Target들을 target에 추가한다.
             */

            STL_TRY( qlvValidateAsteriskSubTable( aStmtInfo,
                                                  aPhraseType,
                                                  aTableNode,
                                                  aTargetSource,
                                                  aExceptNameCount,
                                                  aExceptNameArr,
                                                  aTargets,
                                                  aTargetCount,
                                                  aEnv )
                     == STL_SUCCESS );
            break;

        case QLV_NODE_TYPE_JOIN_TABLE:
            /**
             * Using 구문이 있는 경우 Named Column List의 컬럼들을
             * Target으로 하고, 해당 컬럼들을 Except Column Name Array에
             * 추가한다.
             */

            STL_TRY( qlvValidateAsteriskJoinTable( aStmtInfo,
                                                   aPhraseType,
                                                   aTableNode,
                                                   aTargetSource,
                                                   aExceptNameCount,
                                                   aExceptNameArr,
                                                   aTargets,
                                                   aTargetCount,
                                                   aEnv )
                     == STL_SUCCESS );
            break;

        default:
            STL_DASSERT(0);
            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Base Table의 모든 컬럼들에 대하여 validation 한다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aPhraseType          Expr이 사용된 Phrase 유형
 * @param[in]  aTableNode           Table Node
 * @param[in]  aTargetSource        Target Source
 * @param[in]  aExceptNameCount     Except Column Name Count
 * @param[in]  aExceptNameArr       Except Columy Name Array
 * @param[in]  aTargets             Target List
 * @param[out] aTargetCount         Total Target Count
 * @param[in]  aEnv                 Environment
 * @remarks TargetSource가 QLL_TARGET_ASTERISK_TYPE인 경우
 *          Relation Name에 해당하는 테이블의 Column들만 모두 추가한다.
 */
stlStatus qlvValidateAsteriskBaseTable( qlvStmtInfo         * aStmtInfo,
                                        qlvExprPhraseType     aPhraseType,
                                        qlvInitNode         * aTableNode,
                                        qlpTarget           * aTargetSource,
                                        stlInt32              aExceptNameCount,
                                        stlChar            ** aExceptNameArr,
                                        qlvInitTarget       * aTargets,
                                        stlInt32            * aTargetCount,
                                        qllEnv              * aEnv )
{
    stlInt32                  i                 = 0;
    stlInt32                  j                 = 0;
    stlInt32                  sTargetCount      = 0;
    stlInt32                  sColumnCount      = 0;
    stlBool                   sExist            = STL_FALSE;

    qlvInitBaseTableNode    * sBaseTableNode    = NULL;
    qlvRefExprItem          * sRefColumnItem    = NULL;

    ellDictHandle           * sTableHandle      = NULL;
    stlChar                 * sColumnName       = NULL;
    ellDictHandle           * sColumnHandle     = NULL;
    qlvInitTarget           * sCodeTarget       = NULL;
    

    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode->mType == QLV_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Base Table Node의 컬럼들을 Target으로 설정
     **********************************************************/

    sBaseTableNode = (qlvInitBaseTableNode*)aTableNode;


    /**
     * TargetSource에 Relation Name이 존재하는 경우 (ex: t1.*)
     * 동일한 Relation Name인 경우에만 Column들을 추가한다.
     */

    if( ( aTargetSource->mType == QLL_TARGET_ASTERISK_TYPE ) &&
        ( aTargetSource->mRelName != NULL ) )
    {
        if( qlvIsSameTable( sBaseTableNode->mRelationName,
                            NULL, /* aTargetSource->mRelName->mCatalog */
                            aTargetSource->mRelName->mSchema,
                            aTargetSource->mRelName->mObject )
            == STL_FALSE )
        {
            STL_THROW( RAMP_FINISH );
        }
    }

    /**
     * Table Handle 가져오기
     */

    sTableHandle = & sBaseTableNode->mTableHandle;


    /**
     * Column Handle 얻기
     */

    sTargetCount    = 0;
    sColumnCount    = ellGetTableColumnCnt( sTableHandle ); 
    sColumnHandle   = ellGetTableColumnDictHandle( sTableHandle );
    sCodeTarget     = aTargets;
    
    for( i = 0 ; i < sColumnCount ; i++ )
    {
        if ( ellGetColumnUnused( & sColumnHandle[i] ) == STL_TRUE )
        {
            /**
             * UNUSED Column 임
             */
            continue;
        }
        else
        {
            sColumnName = ellGetColumnName( & sColumnHandle[i] );

        
            /**
             * Except Column Name List에 존재하는지 판별
             */

            sExist = STL_FALSE;
            for( j = 0; j < aExceptNameCount; j++ )
            {
                if( stlStrcmp( sColumnName, aExceptNameArr[j] ) == 0 )
                {
                    /**
                     * Column이 Except Column Name에 존재하는 경우
                     */

                    sExist = STL_TRUE;
                    break;
                }
            }

            if( sExist == STL_TRUE )
            {
                /**
                 * Column이 Except Column Name에 존재하므로
                 * Target에 등록하지 않고 다음 컬럼으로 넘어간다.
                 */

                continue;
            }


            /**
             * Target Name 설정
             */

        
            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                        stlStrlen( sColumnName ) + 1,
                                        (void **) & sCodeTarget->mDisplayName,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        
            stlStrcpy( sCodeTarget->mDisplayName, sColumnName );
            sCodeTarget->mDisplayPos = aTargetSource->mNodePos;
            sCodeTarget->mAliasName = NULL;


            /**
             * Reference Column Item 얻어오기
             */

            STL_TRY( qlvAddRefColumnItemOnBaseTable( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                     aPhraseType,
                                                     aTargetSource->mNodePos,
                                                     sBaseTableNode,
                                                     &sColumnHandle[i],
                                                     STL_TRUE,
                                                     &sRefColumnItem,
                                                     aEnv )
                     == STL_SUCCESS );


            /**
             * Target Expr 설정
             */

            sCodeTarget->mExpr = sRefColumnItem->mExprPtr;
            sCodeTarget++;
            sTargetCount++;
        }
    }

    STL_RAMP( RAMP_FINISH );


    /**********************************************************
     * 마무리
     **********************************************************/

    /**
     * Output 설정
     */

    *aTargetCount = sTargetCount;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;

}

/**
 * @brief Sub Table의 모든 컬럼들에 대하여 validation 한다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aPhraseType          Expr이 사용된 Phrase 유형
 * @param[in]  aTableNode           Table Node
 * @param[in]  aTargetSource        Target Source
 * @param[in]  aExceptNameCount     Except Column Name Count
 * @param[in]  aExceptNameArr       Except Columy Name Array
 * @param[in]  aTargets             Target List
 * @param[out] aTargetCount         Total Target Count
 * @param[in]  aEnv                 Environment
 * @remarks TargetSource가 QLL_TARGET_ASTERISK_TYPE인 경우
 *          Relation Name에 해당하는 테이블의 Column들만 모두 추가한다.
 */
stlStatus qlvValidateAsteriskSubTable( qlvStmtInfo         * aStmtInfo,
                                       qlvExprPhraseType     aPhraseType,
                                       qlvInitNode         * aTableNode,
                                       qlpTarget           * aTargetSource,
                                       stlInt32              aExceptNameCount,
                                       stlChar            ** aExceptNameArr,
                                       qlvInitTarget       * aTargets,
                                       stlInt32            * aTargetCount,
                                       qllEnv              * aEnv )
{
    stlInt32              i                     = 0;
    stlInt32              j                     = 0;
    stlBool               sExist                = STL_FALSE;
    stlInt32              sTargetCount          = 0;
    stlInt32              sSrcTargetCount       = 0;

    qlvInitTarget       * sTargets              = NULL;
    qlvInitTarget       * sSrcTargets           = NULL;

    qlvInitSubTableNode * sSubTableNode         = NULL;
    qlvRefExprItem      * sRefColumnItem        = NULL;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode->mType == QLV_NODE_TYPE_SUB_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Sub Table Node의 컬럼들을 Target으로 설정
     **********************************************************/

    sSubTableNode = (qlvInitSubTableNode*)aTableNode;


    /**
     * TargetSource에 Relation Name이 존재하는 경우 (ex: t1.*)
     * 동일한 Relation Name인 경우에만 Column들을 추가한다.
     */

    if( ( aTargetSource->mType == QLL_TARGET_ASTERISK_TYPE ) &&
        ( aTargetSource->mRelName != NULL ) )
    {
        STL_DASSERT( aTargetSource->mRelName != NULL );
        if( qlvIsSameTable( sSubTableNode->mRelationName,
                            NULL, /* aTargetSource->mRelName->mCatalog */
                            aTargetSource->mRelName->mSchema,
                            aTargetSource->mRelName->mObject )
            == STL_FALSE )
        {
            STL_THROW( RAMP_FINISH );
        }
    }

    sTargetCount    = 0;
    sTargets        = aTargets;

    sSrcTargetCount = sSubTableNode->mColumnCount;
    sSrcTargets     = sSubTableNode->mColumns;
        
    for( i = 0; i < sSrcTargetCount; i++ )
    {
        /**
         * Except Column Name List에 존재하는지 판별
         */

        sExist = STL_FALSE;
        for( j = 0; j < aExceptNameCount; j++ )
        {
            if( sSrcTargets[i].mAliasName == NULL )
            {
                if( stlStrcmp( sSrcTargets[i].mDisplayName, aExceptNameArr[j] ) == 0 )
                {
                    sExist = STL_TRUE;
                    break;
                }
            }
            else
            {
                if( stlStrcmp( sSrcTargets[i].mAliasName, aExceptNameArr[j] ) == 0 )
                {
                    sExist = STL_TRUE;
                    break;
                }
            }
        }

        if( sExist == STL_TRUE )
        {
            /**
             * Except Column Name List에 존재하는 경우
             */

            continue;
        }


        /**
         * Reference Column Item 얻어오기
         */

        STL_TRY( qlvAddRefColumnItemOnSubTable( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                aPhraseType,
                                                aTargetSource->mNodePos,
                                                sSubTableNode,
                                                i,
                                                sSrcTargets[i].mExpr,
                                                STL_TRUE,
                                                &sRefColumnItem,
                                                aEnv )
                 == STL_SUCCESS );

        sSrcTargets[i].mExpr = sRefColumnItem->mExprPtr;


        /**
         * Target Expr 설정
         */

        sTargets[sTargetCount].mDisplayName = sSrcTargets[i].mDisplayName;
        sTargets[sTargetCount].mDisplayPos  = sSrcTargets[i].mDisplayPos;
        sTargets[sTargetCount].mAliasName   = sSrcTargets[i].mAliasName;
        
        sTargets[sTargetCount].mExpr = sRefColumnItem->mExprPtr;
        sTargetCount++;
    }

    STL_RAMP( RAMP_FINISH );


    /**********************************************************
     * 마무리
     **********************************************************/

    /**
     * Output 설정
     */

    *aTargetCount = sTargetCount;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Table의 모든 컬럼들에 대하여 validation 한다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aPhraseType          Expr이 사용된 Phrase 유형
 * @param[in]  aTableNode           Table Node
 * @param[in]  aTargetSource        Target Source
 * @param[in]  aExceptNameCount     Except Column Name Count
 * @param[in]  aExceptNameArr       Except Columy Name Array
 * @param[in]  aTargets             Target List
 * @param[out] aTargetCount         Total Target Count
 * @param[in]  aEnv                 Environment
 * @remarks TargetSource가 QLL_TARGET_ASTERISK_TYPE인 경우
 *          Relation Name에 해당하는 테이블의 Column들만 모두 추가한다.
 */
stlStatus qlvValidateAsteriskJoinTable( qlvStmtInfo         * aStmtInfo,
                                        qlvExprPhraseType     aPhraseType,
                                        qlvInitNode         * aTableNode,
                                        qlpTarget           * aTargetSource,
                                        stlInt32              aExceptNameCount,
                                        stlChar            ** aExceptNameArr,
                                        qlvInitTarget       * aTargets,
                                        stlInt32            * aTargetCount,
                                        qllEnv              * aEnv )
{
    qlvInitJoinTableNode    * sJoinTableNode        = NULL;
    qlvNamedColumnItem      * sNamedColumnItem      = NULL;

    stlInt32                  sTempTargetCount      = 0;
    stlInt32                  sTargetCount          = 0;
    qlvInitTarget           * sTargets              = NULL;

    stlInt32                  sExceptNameCount      = 0;
    stlChar                ** sExceptNameArr        = NULL;

    stlInt32                  i                     = 0;
    qlvRefExprItem          * sLeftRefColumnItem    = NULL;
    qlvRefExprItem          * sRightRefColumnItem   = NULL;
    qlvRefExprItem          * sTempRefColumnItem    = NULL;
    

    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode->mType == QLV_NODE_TYPE_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Join Table Node의 컬럼들을 Target으로 설정
     **********************************************************/

    /**
     * Using 구문이 있는 경우 Named Column List의 컬럼들을
     * Target으로 하고, 해당 컬럼들을 Except Column Name Array에
     * 추가한다.
     */

    sJoinTableNode      = (qlvInitJoinTableNode*)aTableNode;
    sTargetCount        = 0;
    sTargets            = aTargets;
    sExceptNameCount    = aExceptNameCount;
    sExceptNameArr      = aExceptNameArr;

    if( (aTargetSource->mType == QLL_TARGET_ASTERISK_TYPE) &&
        (aTargetSource->mRelName == NULL ) &&
        (sJoinTableNode->mJoinSpec != NULL) &&
        (sJoinTableNode->mJoinSpec->mNamedColumnList != NULL) )
    {
        /**
         * Named Column List가 있는 경우
         */

        sNamedColumnItem    = sJoinTableNode->mJoinSpec->mNamedColumnList->mHead;
        while( sNamedColumnItem != NULL )
        {
            /* 상위 Named Column List에 존재하는 경우 체크 */
            for( i = 0; i < aExceptNameCount; i++ )
            {
                if( stlStrcmp( aExceptNameArr[i], sNamedColumnItem->mName ) == 0 )
                {
                    break;
                }
            }


            if( i == aExceptNameCount )
            {
                /**
                 * Left Expr의 Reference Column Item 추가
                 */

                STL_TRY( qlvAddRefColumnItemOnJoinTable( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                         aPhraseType,
                                                         aTargetSource->mNodePos,
                                                         sJoinTableNode,
                                                         STL_TRUE,
                                                         sNamedColumnItem->mLeftExpr,
                                                         STL_TRUE,
                                                         &sLeftRefColumnItem,
                                                         aEnv )
                         == STL_SUCCESS );


                /**
                 * Right Expr의 Reference Column Item 추가
                 */

                STL_TRY( qlvAddRefColumnItemOnJoinTable( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                         aPhraseType,
                                                         aTargetSource->mNodePos,
                                                         sJoinTableNode,
                                                         STL_FALSE,
                                                         sNamedColumnItem->mRightExpr,
                                                         STL_TRUE,
                                                         &sRightRefColumnItem,
                                                         aEnv )
                         == STL_SUCCESS );


                /**
                 * Target의 Name과 Expr의 Pointer를 지정해준다.
                 */

                sTargets[sTargetCount].mDisplayName = sNamedColumnItem->mName;
                sTargets[sTargetCount].mDisplayPos  = aTargetSource->mNodePos;
                sTargets[sTargetCount].mExpr = sLeftRefColumnItem->mExprPtr;
                sTargetCount++;


                /**
                 * Except Column Name List에 추가
                 */

                sExceptNameArr[sExceptNameCount] = sNamedColumnItem->mName;
                sExceptNameCount++;
            }

            sNamedColumnItem = sNamedColumnItem->mNext;
        }
    }


    /**
     * Join의 Left Table Node
     */

    if( (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_RIGHT_SEMI) &&
        (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI) &&
        (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_RIGHT_ANTI_SEMI) &&
        (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA) )
    {
        sTempTargetCount = 0;
        STL_TRY( qlvValidateAsteriskMultiTables( aStmtInfo,
                                                 aPhraseType,
                                                 sJoinTableNode->mLeftTableNode,
                                                 aTargetSource,
                                                 sExceptNameCount,
                                                 sExceptNameArr,
                                                 &sTargets[sTargetCount],
                                                 &sTempTargetCount,
                                                 aEnv )
                 == STL_SUCCESS );

        if( sTempTargetCount > 0 )
        {
            for( i = 0; i < sTempTargetCount; i++ )
            {
                /**
                 * Reference Column Item 추가
                 */

                STL_TRY( qlvAddRefColumnItemOnJoinTable( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                         aPhraseType,
                                                         aTargetSource->mNodePos,
                                                         sJoinTableNode,
                                                         STL_TRUE,
                                                         sTargets[sTargetCount + i].mExpr,
                                                         STL_TRUE,
                                                         &sTempRefColumnItem,
                                                         aEnv )
                         == STL_SUCCESS );
                sTargets[sTargetCount + i].mExpr = sTempRefColumnItem->mExprPtr;
            }

            sTargetCount += sTempTargetCount;
        }
        else
        {
            if( (sJoinTableNode->mJoinType == QLV_JOIN_TYPE_LEFT_SEMI) ||
                (sJoinTableNode->mJoinType == QLV_JOIN_TYPE_INVERTED_LEFT_SEMI) ||
                (sJoinTableNode->mJoinType == QLV_JOIN_TYPE_LEFT_ANTI_SEMI) ||
                (sJoinTableNode->mJoinType == QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA) )
            {
                STL_THROW( RAMP_ERR_INVALID_IDENTIFIER );
            }
        }
    }


    /**
     * Join의 Right Table Node
     */

    if( (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_LEFT_SEMI) &&
        (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_INVERTED_LEFT_SEMI) &&
        (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_LEFT_ANTI_SEMI) &&
        (sJoinTableNode->mJoinType != QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA) )
    {
        sTempTargetCount = 0;
        STL_TRY( qlvValidateAsteriskMultiTables( aStmtInfo,
                                                 aPhraseType,
                                                 sJoinTableNode->mRightTableNode,
                                                 aTargetSource,
                                                 sExceptNameCount,
                                                 sExceptNameArr,
                                                 &sTargets[sTargetCount],
                                                 &sTempTargetCount,
                                                 aEnv )
                 == STL_SUCCESS );

        if( sTempTargetCount > 0 )
        {
            for( i = 0; i < sTempTargetCount; i++ )
            {
                /**
                 * Reference Column Item 추가
                 */

                STL_TRY( qlvAddRefColumnItemOnJoinTable( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                         aPhraseType,
                                                         aTargetSource->mNodePos,
                                                         sJoinTableNode,
                                                         STL_FALSE,
                                                         sTargets[sTargetCount + i].mExpr,
                                                         STL_TRUE,
                                                         &sTempRefColumnItem,
                                                         aEnv )
                         == STL_SUCCESS );
                sTargets[sTargetCount + i].mExpr = sTempRefColumnItem->mExprPtr;
            }

            sTargetCount += sTempTargetCount;
        }
        else
        {
            if( (sJoinTableNode->mJoinType == QLV_JOIN_TYPE_RIGHT_SEMI) ||
                (sJoinTableNode->mJoinType == QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI) ||
                (sJoinTableNode->mJoinType == QLV_JOIN_TYPE_RIGHT_ANTI_SEMI) ||
                (sJoinTableNode->mJoinType == QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA) )
            {
                STL_THROW( RAMP_ERR_INVALID_IDENTIFIER );
            }
        }
    }


    /**********************************************************
     * 마무리
     **********************************************************/

    /**
     * Output 설정
     */

    *aTargetCount = sTargetCount;

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_IDENTIFIER )
    {
        if( aTargetSource->mRelName == NULL )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_INVALID_IDENTIFIER,
                          qlgMakeErrPosString( aStmtInfo->mSQLString,
                                               aTargetSource->mNamePos,
                                               aEnv ),
                          QLL_ERROR_STACK(aEnv),
                          aTargetSource->mName );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_INVALID_IDENTIFIER,
                          qlgMakeErrPosString( aStmtInfo->mSQLString,
                                               aTargetSource->mRelName->mObjectPos,
                                               aEnv ),
                          QLL_ERROR_STACK(aEnv),
                          aTargetSource->mRelName->mObject );
        }
    }

    STL_FINISH;

    return STL_FAILURE;

}


/**
 * @brief Expression을 기반으로 하는 Inner Column Expresion을 추가한다.
 * @param[in,out]  aInitExpr        Inner Column Expression
 * @param[in]      aRelationNode    Relation Node Pointer
 * @param[in]      aIdx             Index
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvAddInnerColumnExpr( qlvInitExpression  ** aInitExpr,
                                 qlvInitNode         * aRelationNode,
                                 stlInt32              aIdx,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv )
{
    qlvInitExpression   * sSrcExpr     = NULL;
    qlvInitExpression   * sNewExpr     = NULL;
    qlvInitInnerColumn  * sInnerColumn = NULL;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( *aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * INNER COLUMN 정보 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitInnerColumn ),
                                (void **) & sInnerColumn,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( stlInt32 ),
                                (void **) & sInnerColumn->mIdx,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * INNER COLUMN 정보 설정
     */

    switch( (*aInitExpr)->mType )
    {
        case QLV_EXPR_TYPE_COLUMN :
        case QLV_EXPR_TYPE_ROWID_COLUMN :
            {
                STL_TRY( qlvCopyExpr( *aInitExpr,
                                      & sSrcExpr,
                                      aRegionMem,
                                      aEnv )
                         == STL_SUCCESS );
                break;
            }
        default :
            {
                sSrcExpr = *aInitExpr;
                break;
            }
    }
    
    
    sInnerColumn->mRelationNode = aRelationNode;
    *sInnerColumn->mIdx         = aIdx;
    sInnerColumn->mOrgExpr      = sSrcExpr;


    /**
     * INNER COLUMN Expression 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sNewExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sNewExpr,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );


    /**
     * INNER COLUMN Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( sSrcExpr->mIncludeExprCnt + 1, aEnv );
    
    sNewExpr->mType              = QLV_EXPR_TYPE_INNER_COLUMN;
    sNewExpr->mPosition          = sSrcExpr->mPosition;
    sNewExpr->mIncludeExprCnt    = sSrcExpr->mIncludeExprCnt + 1;
    sNewExpr->mExpr.mInnerColumn = sInnerColumn;
    sNewExpr->mRelatedFuncId     = sSrcExpr->mRelatedFuncId;
    sNewExpr->mIterationTime     = sSrcExpr->mIterationTime;
    sNewExpr->mColumnName        = sSrcExpr->mColumnName;


    /**
     * OUTPUT 설정
     */

    *aInitExpr = sNewExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expression을 기반으로 하는 Reference Expresion을 추가한다.
 * @param[in,out]  aInitExpr        Inner Column Expression
 * @param[in]      aRegionMem       Region Memory
 * @param[in]      aEnv             Environment
 */
stlStatus qlvAddReferenceExpr( qlvInitExpression  ** aInitExpr,
                               knlRegionMem        * aRegionMem,
                               qllEnv              * aEnv )
{
    qlvInitExpression  * sSrcExpr = NULL;
    qlvInitExpression  * sNewExpr = NULL;
    qlvInitRefExpr     * sRefExpr = NULL;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( *aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * REFERENCE COLUMN 정보 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitRefExpr ),
                                (void **) & sRefExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * REFERENCE COLUMN 정보 설정
     */

    sSrcExpr = *aInitExpr;
    sRefExpr->mOrgExpr = sSrcExpr;


    /**
     * REFERENCE COLUMN Expression 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sNewExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sNewExpr,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );


    /**
     * REFERENCE COLUMN Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );
    
    sNewExpr->mType              = QLV_EXPR_TYPE_REFERENCE;
    sNewExpr->mPosition          = sSrcExpr->mPosition;
    sNewExpr->mIncludeExprCnt    = 1;
    sNewExpr->mExpr.mReference   = sRefExpr;
    sNewExpr->mRelatedFuncId     = sSrcExpr->mRelatedFuncId;
    sNewExpr->mIterationTime     = DTL_ITERATION_TIME_INVALID;  /* Iteration Time 재설정이 반드시 필요 */ 
    sNewExpr->mColumnName        = NULL;


    /**
     * OUTPUT 설정
     */

    *aInitExpr = sNewExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Ref Expr List로 부터 And Function을 생성한다.
 * @param[in]  aRegionMem           Region Memory
 * @param[in]  aPosition            Position
 * @param[in]  aAggrExprList        Reference Expression List
 * @param[out] aFuncExpr            Function Expression
 * @param[in]  aEnv                 Environment
 */
inline stlStatus qlvMakeFuncAndFromExprList( knlRegionMem         * aRegionMem,
                                             stlInt32               aPosition,
                                             qlvRefExprList       * aAggrExprList,
                                             qlvInitExpression   ** aFuncExpr,
                                             qllEnv               * aEnv )
{
    qlvInitExpression  * sFuncCode       = NULL;
    qlvInitFunction    * sFunction       = NULL;
    qlvRefExprItem     * sRefExprItem    = NULL;
    stlInt32             sTotalExprCnt   = 0;
    stlInt32             sLoop;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFuncExpr != NULL, QLL_ERROR_STACK(aEnv) );


    if( aAggrExprList->mCount == 0 )
    {
        sFuncCode = NULL;
    }
    else if( aAggrExprList->mCount == 1)
    {
        sFuncCode = aAggrExprList->mHead->mExprPtr;
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

        sFunction->mFuncId = KNL_BUILTIN_FUNC_AND;


        /**
         * Argument List 세팅
         */

        sFunction->mArgCount = aAggrExprList->mCount;

        /* Arg Array 공간 할당 */
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvInitExpression * ) * aAggrExprList->mCount,
                                    (void **) & sFunction->mArgs,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /* Arg Array 설정 */
        sRefExprItem = aAggrExprList->mHead;

        for( sLoop = 0 ; sLoop < aAggrExprList->mCount ; sLoop++ )
        {
            sFunction->mArgs[ sLoop ] = sRefExprItem->mExprPtr;
            sRefExprItem->mExprPtr->mRelatedFuncId = KNL_BUILTIN_FUNC_AND;
            sTotalExprCnt += sRefExprItem->mExprPtr->mIncludeExprCnt;
            
            sRefExprItem = sRefExprItem->mNext;
        }
        
        
        /**
         * Code Expression 설정
         */

        QLV_VALIDATE_INCLUDE_EXPR_CNT( sTotalExprCnt + 1, aEnv );
        
        sFuncCode->mType           = QLV_EXPR_TYPE_FUNCTION;
        sFuncCode->mPosition       = aPosition;
        sFuncCode->mIncludeExprCnt = sTotalExprCnt + 1;
        sFuncCode->mExpr.mFunction = sFunction;
        sFuncCode->mIterationTime  = DTL_ITERATION_TIME_FOR_EACH_EXPR;
        sFuncCode->mColumnName     = NULL;
    }

    
    /**
     * OUTPUT 설정
     */

    *aFuncExpr = sFuncCode;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ANSI Join Node를 가지고 있는지 판단한다.
 * @param[in]   aTableNode  Validation Table Node
 */
inline stlBool qlvHasANSIJoin( qlvInitNode  * aTableNode )
{
    switch( aTableNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
        case QLV_NODE_TYPE_SUB_TABLE:
            break;
        case QLV_NODE_TYPE_JOIN_TABLE:
            if( ((qlvInitJoinTableNode*)aTableNode)->mJoinType == QLV_JOIN_TYPE_NONE )
            {
                if( qlvHasANSIJoin( ((qlvInitJoinTableNode*)aTableNode)->mLeftTableNode )
                    == STL_FALSE )
                {
                    return qlvHasANSIJoin( ((qlvInitJoinTableNode*)aTableNode)->mRightTableNode );
                }
                else
                {
                    return STL_TRUE;
                }
            }
            else
            {
                return STL_TRUE;
            }
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    return STL_FALSE;
}


/**
 * @brief Outer Join Operator에 대한 Validation을 수행하고, 이에 맞게 Table Node를 수정한다.
 * @param[in]   aStmtInfo       Stmt Information
 * @param[in]   aWhereExpr      Where Expression
 * @param[in]   aTableNode      Validation Table Node
 * @param[out]  aOutWhereExpr   Output Where Expression
 * @param[out]  aOutTableNode   Output Validation Table Node
 * @param[in]   aEnv            Environment
 */
inline stlStatus qlvValidateOuterJoinOperator( qlvStmtInfo          * aStmtInfo,
                                               qlvInitExpression    * aWhereExpr,
                                               qlvInitNode          * aTableNode,
                                               qlvInitExpression   ** aOutWhereExpr,
                                               qlvInitNode         ** aOutTableNode,
                                               qllEnv               * aEnv )
{
    stlInt32              i;
    stlInt32              j;
    stlInt32              sErrPos;
    stlInt32              sAndItemCount;
    stlInt32              sTableCount;
    stlInt32              sCurNodeIdx;
    stlInt32              sRefCount;
    stlInt32              sLeftNodeIdx          = 0;
    stlInt32              sRightNodeIdx         = 0;
    qlvInitExpression  ** sAndItemPtrArr        = NULL;
    qlvRefTableColList  * sRefTableColListArr   = NULL;
    qlvRefTableColItem  * sRefTableColItem      = NULL;
    qlvInitNode        ** sTableNodeArr         = NULL;
    stlInt32           ** sCycleDetectMap       = NULL;

    qlvInitNode             * sLeftNode         = NULL;
    qlvInitNode             * sRightNode        = NULL;
    qlvInitNode             * sLeftBaseNode     = NULL;
    qlvInitNode             * sRightBaseNode    = NULL;
    qlvInitNode             * sTmpNode          = NULL;
    qlvInitJoinTableNode    * sJoinTableNode    = NULL;
    qlvInitExpression       * sTmpItemPtr       = NULL;
    qlvRefTableColList        sTmpTableColList;

    qlvInitFunction         * sFunction         = NULL;
    qlvInitExpression      ** sArgs             = NULL;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aWhereExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Outer Join Operator에 대하여 Validation해야 하는 항목들
     * 1. Join Condition에서 하나의 comparison에 쓰였을 경우
     *  1-1. 한 테이블에 대해서만 Outer Join Operator를 사용해야 하며,
     *  1-2. 해당 comparison에 나타나는 동일 테이블의 모든 컬럼에는 Outer Join Operator가 존재해야 한다.
     *  ex) T1.I1(+) = T2.I1(+)          => X (1-1 위배)
     *      T1.I1 = T2.I1(+) + T3.I1(+)  => X (1-1 위배)
     *      T1.I1 = T2.I1(+) + T2.I1     => X (1-2 위배)
     *
     * 2. 한 테이블에 대한 Self Outer-Join으로 사용이 불가능한다.
     *  ex) T1.I1 = T1.I1(+)             => X
     *
     * 3. 3개 이상의 테이블들의 컬럼이 포함된 조건에는 사용될 수 없다.
     *  ex) T1.I1(+) = T2.I1 + T3.I1     => X
     *      T1.I1 = T2.I1(+) + T3.I1     => X
     *
     * 4. 서로 다른 2 테이블에 대하여 서로가 Outer Join이 될 수 없다.
     *  ex) T1.I1(+) = T2.I1 AND T1.I1 = T2.I1(+)
     *
     * 5. 서로 다른 2 테이블에 대하여 Outer Join 조건과 Inner Join 조건이 혼재된 경우 Inner Join으로 처리된다.
     *  ex) T1.I1 = T2.I1(+) AND T1.I2 = T2.I2  => Inner Join으로 수행
     */


    /**
     * Where Expression 분석
     */

    /* And Item Count 얻기 */
    if( (aWhereExpr->mType == QLV_EXPR_TYPE_FUNCTION) &&
        (aWhereExpr->mExpr.mFunction->mFuncId == KNL_BUILTIN_FUNC_AND) )
    {
        sAndItemCount = aWhereExpr->mExpr.mFunction->mArgCount;
    }
    else
    {
        sAndItemCount = 1;
    }

    /* And Item Pointer Array 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_VALIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlvInitExpression* ) * sAndItemCount,
                                (void**) &sAndItemPtrArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* And Item Pointer Array 설정 */
    if( sAndItemCount == 1 )
    {
        sAndItemPtrArr[0] = aWhereExpr;
    }
    else
    {
        for( i = 0; i < sAndItemCount; i++ )
        {
            sAndItemPtrArr[i] = aWhereExpr->mExpr.mFunction->mArgs[i];
        }
    }

    /* Reference Table Column Expression List Array 할당 */
    STL_TRY( knlAllocRegionMem( &QLL_VALIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlvRefTableColList ) * sAndItemCount,
                                (void**) &sRefTableColListArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* 각 And Item별 Reference Column Expression List 구성 */
    for( i = 0; i < sAndItemCount; i++ )
    {
        sRefTableColListArr[i].mCount = 0;
        sRefTableColListArr[i].mOuterJoinOperColCount = 0;
        sRefTableColListArr[i].mOuterColumnCount = 0;
        sRefTableColListArr[i].mHead = NULL;
        sRefTableColListArr[i].mTail = NULL;

        STL_TRY( qlvGetRefTableColList( &QLL_VALIDATE_MEM( aEnv ),
                                        sAndItemPtrArr[i],
                                        &sRefTableColListArr[i],
                                        aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Outer Join Operator Check #1
     * 1. Join Condition에서 하나의 comparison에 쓰였을 경우
     *  1-1. 한 테이블에 대해서만 Outer Join Operator를 사용해야 하며,
     *  1-2. 해당 comparison에 나타나는 동일 테이블의 모든 컬럼에는 Outer Join Operator가 존재해야 한다.
     */

    for( i = 0; i < sAndItemCount; i++ )
    {
        if( sRefTableColListArr[i].mOuterJoinOperColCount == 0 )
        {
            continue;
        }

        sErrPos = sAndItemPtrArr[i]->mPosition;

        /* Outer Join Operator Check #1-1 */
        STL_TRY_THROW( sRefTableColListArr[i].mOuterJoinOperColCount == 1,
                       RAMP_ERR_PREDICATE_MAY_REFERENCED_ONLY_ONE_OUTER_JOIN_TABLE );

        sRefTableColItem = sRefTableColListArr[i].mHead;
        while( sRefTableColItem != NULL )
        {
            /* Outer Join Operator Check */
            if( (sRefTableColItem->mOuterJoinOperCount > 0) &&
                (sRefTableColItem->mOuterJoinOperCount != sRefTableColItem->mColumnCount) )
            {
                if( sRefTableColListArr[i].mCount == 1 )
                {
                    /* Outer Join Operator Check #2 */
                    STL_THROW( RAMP_ERR_CANNOT_BE_OUTER_JOINED_TO_EACH_OTHER );
                }
                else
                {
                    /* Outer Join Operator Check #1-2 */
                    STL_THROW( RAMP_ERR_MAY_BE_OUTER_JOINED_TO_AT_MOST_ONE_OTHER_TABLE );
                }
            }

            sRefTableColItem = sRefTableColItem->mNext;
        }
    }


    /**
     * Outer Join Operator가 존재하는 노드를 기준으로 Outer Node들의 정보를 수집
     */

    /* Table Node Count 얻기 */
    sTableCount = 0;
    qlvGetTableCount( aTableNode, &sTableCount );

    /* Table Node Array 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_VALIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlvInitNode* ) * sTableCount,
                                (void**) &sTableNodeArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Table Node Array 셋팅 */
    sCurNodeIdx = 0;
    qlvSetTableNodeArray( aTableNode, sTableNodeArr, &sCurNodeIdx );

    /* Cycle Detect Map 생성 */
    STL_TRY( knlAllocRegionMem( &QLL_VALIDATE_MEM( aEnv ),
                                STL_SIZEOF( stlInt32* ) * sTableCount,
                                (void**) &sCycleDetectMap,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    for( i = 0; i < sTableCount; i++ )
    {
        STL_TRY( knlAllocRegionMem( &QLL_VALIDATE_MEM( aEnv ),
                                    STL_SIZEOF( stlInt32 ) * sTableCount,
                                    (void**) &sCycleDetectMap[i],
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sCycleDetectMap[i], 0x00, STL_SIZEOF( stlInt32 ) * sTableCount );
    }

    /* Cycle Detect Map 설정 */
    for( i = 0; i < sAndItemCount; i++ )
    {
        /* Outer Join Operator가 없으면 skip */
        if( sRefTableColListArr[i].mOuterJoinOperColCount == 0 )
        {
            continue;
        }

        /* Outer Query Block을 참조하면 Outer Join Operator를 무시 */
        if( sRefTableColListArr[i].mOuterColumnCount > 0 )
        {
            continue;
        }

        /* Outer Join Operator가 Set된 Relation Node의 Idx 찾기 */
        sRefTableColItem = sRefTableColListArr[i].mHead;
        while( sRefTableColItem != NULL )
        {
            if( sRefTableColItem->mOuterJoinOperCount > 0 )
            {
                break;
            }

            sRefTableColItem = sRefTableColItem->mNext;
        }
        STL_DASSERT( sRefTableColItem != NULL );

        for( sCurNodeIdx = 0; sCurNodeIdx < sTableCount; sCurNodeIdx++ )
        {
            if( sRefTableColItem->mRelationNode == sTableNodeArr[sCurNodeIdx] )
            {
                break;
            }
        }
        STL_DASSERT( sCurNodeIdx < sTableCount );

        /* Outer Join Operator가 아닌 모든 Relation Node에 대하여 Flag 셋팅 */
        sRefTableColItem = sRefTableColListArr[i].mHead;
        while( sRefTableColItem != NULL )
        {
            if( sRefTableColItem->mOuterJoinOperCount == 0 )
            {
                for( j = 0; j < sTableCount; j++ )
                {
                    if( sRefTableColItem->mRelationNode == sTableNodeArr[j] )
                    {
                        break;
                    }
                }
                STL_DASSERT( j < sTableCount );

                if( sCycleDetectMap[sCurNodeIdx][j] == 0 )
                {
                    STL_DASSERT( sAndItemPtrArr[i]->mPosition > 0 );
                    sCycleDetectMap[sCurNodeIdx][j] = sAndItemPtrArr[i]->mPosition;
                }
            }

            sRefTableColItem = sRefTableColItem->mNext;
        }
    }

    /* Outer Join Operator Check #3 */
    for( i = 0; i < sTableCount; i++ )
    {
        sRefCount = 0;
        sErrPos = 0;
        for( j = 0; j < sTableCount; j++ )
        {
            if( sCycleDetectMap[i][j] > 0 )
            {
                if( sErrPos == 0 )
                {
                    sErrPos = sCycleDetectMap[i][j];
                }

                sRefCount++;
            }
        }

        STL_TRY_THROW( sRefCount < 2, RAMP_ERR_MAY_BE_OUTER_JOINED_TO_AT_MOST_ONE_OTHER_TABLE );
    }

    /* Outer Join Operator Check #4 */
    for( sCurNodeIdx = 0; sCurNodeIdx < sTableCount; sCurNodeIdx++ )
    {
        sRefCount = 0;
        sErrPos = 0;
        i = sCurNodeIdx;
        while( STL_TRUE )
        {
            for( j = 0; j < sTableCount; j++ )
            {
                if( sCycleDetectMap[i][j] > 0 )
                {
                    if( i == sCurNodeIdx )
                    {
                        sErrPos = sCycleDetectMap[i][j];
                    }

                    break;
                }
            }

            if( j < sTableCount )
            {
                STL_TRY_THROW( (j != sCurNodeIdx) && (sRefCount < sTableCount),
                               RAMP_ERR_CANNOT_BE_OUTER_JOINED_TO_EACH_OTHER );

                sRefCount++;
                i = j;
            }
            else
            {
                break;
            }
        }
    }


    /**
     * Outer Join Operator를 적용 가능한 Filter만 선별
     * (Only Two Table's Filter)
     */

    sRefCount = 0;  /* Outer Join Operator Count */
    for( i = 0; i < sAndItemCount; i++ )
    {
        if( sRefTableColListArr[i].mOuterJoinOperColCount == 0 )
        {
            /* Outer Join Operator가 없는 조건 */
            continue;
        }
        else
        {
            if( sRefTableColListArr[i].mOuterColumnCount > 0 )
            {
                /* Outer Query Block을 참조하는 경우 Outer Join Operator 무시 */
                sRefTableColListArr[i].mOuterJoinOperColCount = 0;
                continue;
            }
            else if( sRefTableColListArr[i].mCount == 1 )
            {
                /* one table에 대한 조건인 경우 */
                continue;
            }
            else
            {
                sRefTableColItem = sRefTableColListArr[i].mHead;
                while( sRefTableColItem != NULL )
                {
                    if( sRefTableColItem->mOuterJoinOperCount > 0 )
                    {
                        break;
                    }

                    sRefTableColItem = sRefTableColItem->mNext;
                }
                STL_DASSERT( sRefTableColItem != NULL );
                sLeftNode = sRefTableColItem->mRelationNode;

                /* Non Outer Join Operator에 해당 Relation이 존재하는지 파악 */
                for( j = sRefCount; j < sAndItemCount; j++ )
                {
                    /* Outer Join Operator가 존재하는 경우 Skip */
                    if( sRefTableColListArr[j].mOuterJoinOperColCount > 0 )
                    {
                        continue;
                    }
                }

                STL_DASSERT( sRefTableColListArr[i].mOuterJoinOperColCount != 0 );
            }
        }

        /* Outer Join Operator를 앞으로 이동 */
        sTmpItemPtr = sAndItemPtrArr[i];
        sTmpTableColList = sRefTableColListArr[i];
        for( j = i; j > sRefCount; j-- )
        {
            sAndItemPtrArr[j] = sAndItemPtrArr[j-1];
            sRefTableColListArr[j] = sRefTableColListArr[j-1];
        }
        sAndItemPtrArr[sRefCount] = sTmpItemPtr;
        sRefTableColListArr[sRefCount] = sTmpTableColList;
        sRefCount++;
    }

    /* Outer Join Operator가 남아 있는지 판단 */
    if( sRefCount == 0 )
    {
        *aOutWhereExpr = aWhereExpr;
        *aOutTableNode = aTableNode;

        STL_THROW( RAMP_FINISH );
    }


    /**
     * Outer Join Operator를 적용 가능한 Filter만 선별
     * (Only One Table's Filter)
     */

    for( i = sRefCount; i < sAndItemCount; i++ )
    {
        if( sRefTableColListArr[i].mOuterJoinOperColCount == 0 )
        {
            /* Outer Join Operator가 없는 조건 */
            continue;
        }
        else
        {
            if( sRefTableColListArr[i].mOuterColumnCount > 0 )
            {
                /* Outer Query Block을 참조하는 경우 Outer Join Operator 무시 */
                sRefTableColListArr[i].mOuterJoinOperColCount = 0;
                continue;
            }
            else if( sRefTableColListArr[i].mCount == 1 )
            {
                /* one table에 대한 조건인 경우
                 * (해당 Table에 Two Table에 대한 Outer Join Condition이 존재해야 함) */
                for( j = 0; j < sRefCount; j++ )
                {
                    STL_DASSERT( sRefTableColListArr[j].mOuterJoinOperColCount > 0 );
                    if( sRefTableColListArr[j].mCount > 1 )
                    {
                        sRefTableColItem = sRefTableColListArr[j].mHead;
                        while( sRefTableColItem != NULL )
                        {
                            if( (sRefTableColItem->mOuterJoinOperCount > 0) &&
                                (sRefTableColItem->mRelationNode == sRefTableColListArr[i].mHead->mRelationNode) )
                            {
                                break;
                            }

                            sRefTableColItem = sRefTableColItem->mNext;
                        }

                        if( sRefTableColItem != NULL )
                        {
                            break;
                        }
                    }
                }

                if( sRefTableColItem == NULL )
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        }

        /* Outer Join Operator를 앞으로 이동 */
        sTmpItemPtr = sAndItemPtrArr[i];
        sTmpTableColList = sRefTableColListArr[i];
        for( j = i; j > sRefCount; j-- )
        {
            sAndItemPtrArr[j] = sAndItemPtrArr[j-1];
            sRefTableColListArr[j] = sRefTableColListArr[j-1];
        }
        sAndItemPtrArr[sRefCount] = sTmpItemPtr;
        sRefTableColListArr[sRefCount] = sTmpTableColList;
        sRefCount++;
    }


    /**
     * Outer Join Operator에 의한 새로운 Table Node 생성 및 Where Expression 조정
     */

    for( i = 0; i < sRefCount; i++ )
    {
        sLeftBaseNode = NULL;
        sRightBaseNode = NULL;
        sRefTableColItem = sRefTableColListArr[i].mHead;
        while( sRefTableColItem != NULL )
        {
            if( sRefTableColItem->mOuterJoinOperCount > 0 )
            {
                STL_DASSERT( sRightBaseNode == NULL );
                sRightBaseNode = sRefTableColItem->mRelationNode;
            }
            else
            {
                STL_DASSERT( sLeftBaseNode == NULL );
                sLeftBaseNode = sRefTableColItem->mRelationNode;
            }

            sRefTableColItem = sRefTableColItem->mNext;
        }

        STL_DASSERT( ((sLeftBaseNode != NULL) && (sRightBaseNode != NULL)) ||
                     ((sLeftBaseNode == NULL) && (sRightBaseNode != NULL)) );

        /* Left Base Node가 존재하는 Node를 찾음 */
        if( sLeftBaseNode != NULL )
        {
            sLeftNodeIdx = -1;
            for( j = 0; j < sTableCount; j++ )
            {
                if( sTableNodeArr[j] == NULL )
                {
                    continue;
                }

                if( qlvIsExistBaseNodeInTableNode( sLeftBaseNode,
                                                   sTableNodeArr[j] )
                    == STL_TRUE )
                {
                    sLeftNode = sTableNodeArr[j];
                    sTableNodeArr[j] = NULL;
                    sLeftNodeIdx = j;
                    break;
                }
            }
            STL_DASSERT( j < sTableCount );
        }

        /* Right Base Node가 존재하는 Node를 찾음 */
        sRightNodeIdx = -1;
        for( j = 0; j < sTableCount; j++ )
        {
            if( sTableNodeArr[j] == NULL )
            {
                continue;
            }

            if( qlvIsExistBaseNodeInTableNode( sRightBaseNode,
                                               sTableNodeArr[j] )
                == STL_TRUE )
            {
                sRightNode = sTableNodeArr[j];
                sTableNodeArr[j] = NULL;
                sRightNodeIdx = j;
                break;
            }
        }

        /* One Table Outer Join Operator */
        if( sRefTableColListArr[i].mCount == 1 )
        {
            STL_DASSERT( j < sTableCount );

            /* Right Base Node를 가진 최하위 Join Node를 찾아사 Filter를 Set한다. */
            sTmpNode = sRightNode;
            while( STL_TRUE )
            {
                STL_DASSERT( sTmpNode->mType == QLV_NODE_TYPE_JOIN_TABLE );

                if( qlvIsExistBaseNodeInTableNode(
                        sRightBaseNode,
                        ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode )
                    == STL_TRUE )
                {
                    if( ((qlvInitJoinTableNode*)sTmpNode)->mJoinType == QLV_JOIN_TYPE_RIGHT_OUTER )
                    {
                        break;
                    }

                    if( ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode->mType
                        == QLV_NODE_TYPE_JOIN_TABLE )
                    {
                        sTmpNode = ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode;
                    }
                    else
                    {
                        break;
                    }
                }
                else if( qlvIsExistBaseNodeInTableNode(
                             sRightBaseNode,
                             ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode )
                         == STL_TRUE )
                {
                    if( ((qlvInitJoinTableNode*)sTmpNode)->mJoinType == QLV_JOIN_TYPE_LEFT_OUTER )
                    {
                        break;
                    }

                    if( ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode->mType
                        == QLV_NODE_TYPE_JOIN_TABLE )
                    {
                        sTmpNode = ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
            STL_DASSERT( (sTmpNode != NULL) &&
                         (sTmpNode->mType == QLV_NODE_TYPE_JOIN_TABLE) );

            if( (((qlvInitJoinTableNode*)sTmpNode)->mJoinSpec->mJoinCondition->mType == QLV_EXPR_TYPE_FUNCTION) &&
                (((qlvInitJoinTableNode*)sTmpNode)->mJoinSpec->mJoinCondition->mExpr.mFunction->mFuncId == KNL_BUILTIN_FUNC_AND) )
            {
                /* And Function에 Argument를 추가 */
                sFunction = ((qlvInitJoinTableNode*)sTmpNode)->mJoinSpec->mJoinCondition->mExpr.mFunction;
                STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                            STL_SIZEOF( qlvInitExpression* ) * (sFunction->mArgCount + 1),
                                            (void**) &sArgs,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sArgs, sFunction->mArgs, STL_SIZEOF( qlvInitExpression*) * sFunction->mArgCount );
                sArgs[sFunction->mArgCount] = sAndItemPtrArr[i];
                sFunction->mArgs = sArgs;
                sFunction->mArgCount += 1;
            }
            else
            {
                /* 두 Expression을 And Function으로 묶는다. */
                STL_TRY( qlvMakeFuncAnd2Arg( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                             aWhereExpr->mPosition,
                                             ((qlvInitJoinTableNode*)sTmpNode)->mJoinSpec->mJoinCondition,
                                             sAndItemPtrArr[i],
                                             &(((qlvInitJoinTableNode*)sTmpNode)->mJoinSpec->mJoinCondition),
                                             aEnv )
                         == STL_SUCCESS );
            }

            /* Right Node를 Table Node Array에 넣는다. */
            for( j = 0; j < sTableCount; j++ )
            {
                if( sTableNodeArr[j] == NULL )
                {
                    sTableNodeArr[j] = sRightNode;
                    break;
                }
            }
            STL_DASSERT( j < sTableCount );

            continue;
        }

        if( j >= sTableCount )
        {
            /* 못찾은 경우 Left Node에 같이 존재한다. */
            STL_DASSERT( qlvIsExistBaseNodeInTableNode( sRightBaseNode, sLeftNode )
                         == STL_TRUE );

            /* Left Base Node와 Right Base Node를 모두 가진 Node를 찾아사 Filter를 Set한다. */
            sTmpNode = sLeftNode;
            while( STL_TRUE )
            {
                STL_DASSERT( sTmpNode->mType == QLV_NODE_TYPE_JOIN_TABLE );

                if( (qlvIsExistBaseNodeInTableNode(
                         sLeftBaseNode,
                         ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode )
                     == STL_TRUE) &&
                    (qlvIsExistBaseNodeInTableNode(
                         sRightBaseNode,
                         ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode )
                     == STL_TRUE) )
                {
                    sTmpNode = ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode;
                }
                else if( (qlvIsExistBaseNodeInTableNode(
                              sLeftBaseNode,
                              ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode )
                          == STL_TRUE) &&
                         (qlvIsExistBaseNodeInTableNode(
                              sRightBaseNode,
                              ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode )
                          == STL_TRUE) )
                {
                    sTmpNode = ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode;
                }
                else
                {
                    break;
                }
            }
            STL_DASSERT( (sTmpNode != NULL) &&
                         (sTmpNode->mType == QLV_NODE_TYPE_JOIN_TABLE) );

            if( (((qlvInitJoinTableNode*)sTmpNode)->mJoinSpec->mJoinCondition->mType == QLV_EXPR_TYPE_FUNCTION) &&
                (((qlvInitJoinTableNode*)sTmpNode)->mJoinSpec->mJoinCondition->mExpr.mFunction->mFuncId == KNL_BUILTIN_FUNC_AND) )
            {
                /* And Function에 Argument를 추가 */
                sFunction = ((qlvInitJoinTableNode*)sTmpNode)->mJoinSpec->mJoinCondition->mExpr.mFunction;
                STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                            STL_SIZEOF( qlvInitExpression* ) * (sFunction->mArgCount + 1),
                                            (void**) &sArgs,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                stlMemcpy( sArgs, sFunction->mArgs, STL_SIZEOF( qlvInitExpression*) * sFunction->mArgCount );
                sArgs[sFunction->mArgCount] = sAndItemPtrArr[i];
                sFunction->mArgs = sArgs;
                sFunction->mArgCount += 1;
            }
            else
            {
                /* 두 Expression을 And Function으로 묶는다. */
                STL_TRY( qlvMakeFuncAnd2Arg( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                             aWhereExpr->mPosition,
                                             ((qlvInitJoinTableNode*)sTmpNode)->mJoinSpec->mJoinCondition,
                                             sAndItemPtrArr[i],
                                             &(((qlvInitJoinTableNode*)sTmpNode)->mJoinSpec->mJoinCondition),
                                             aEnv )
                         == STL_SUCCESS );
            }

            /* Left Node를 Table Node Array에 넣는다. */
            for( j = 0; j < sTableCount; j++ )
            {
                if( sTableNodeArr[j] == NULL )
                {
                    sTableNodeArr[j] = sLeftNode;
                    break;
                }
            }
            STL_DASSERT( j < sTableCount );

            continue;
        }


        /**
         * Left Node와 Right Node의 구성에 따른 처리
         * 1. Base/Sub - Base/Sub
         *   Left Outer Join Node를 만듬
         * 2. Base/Sub - Join
         *   Right Node의 최좌측 Base/Sub 노드를 Left노드와 Left Outer Join Node를 만들고
         *   이를 Right Node의 최좌측 노드에 set한다.
         * 3. Join - Base/Sub
         *   Left Outer Join Node를 만듬
         * 4. Join - Join
         *   Right Node의 최좌측 Join 노드를 Left노드와 Left Outer Join Node를 만들고
         *   이를 Right Node의 최좌측 노드에 set한다.
         */

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF( qlvInitJoinTableNode ),
                                    (void**) &sJoinTableNode,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sJoinTableNode->mType = QLV_NODE_TYPE_JOIN_TABLE;

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF( qlvInitJoinSpec ),
                                    (void**) &(sJoinTableNode->mJoinSpec),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sJoinTableNode->mJoinSpec->mJoinCondition = NULL;
        sJoinTableNode->mJoinSpec->mNamedColumnList = NULL;

        STL_TRY( qlvCreateRefExprList( &(sJoinTableNode->mRefColumnList),
                                       QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        STL_TRY( qlvCreateRefExprList( &(sJoinTableNode->mOuterColumnList),
                                       QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        if( sLeftNodeIdx < sRightNodeIdx )
        {
            sJoinTableNode->mJoinType = QLV_JOIN_TYPE_LEFT_OUTER;

            if( sRightNode->mType == QLV_NODE_TYPE_JOIN_TABLE )
            {
                sTmpNode = sRightNode;
                while( STL_TRUE )
                {
                    if( qlvIsExistBaseNodeInTableNode(
                            sRightBaseNode,
                            ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode )
                         == STL_TRUE )
                    {
                        if( ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode->mType ==
                            QLV_NODE_TYPE_JOIN_TABLE )
                        {
                            sTmpNode = ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode;
                        }
                        else
                        {
                            sJoinTableNode->mLeftTableNode = sLeftNode;
                            sJoinTableNode->mRightTableNode = ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode;
                            ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode = (qlvInitNode*)sJoinTableNode;

                            break;
                        }
                    }
                    else
                    {
                        STL_DASSERT( qlvIsExistBaseNodeInTableNode(
                                         sRightBaseNode,
                                         ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode )
                                     == STL_TRUE );
                        if( ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode->mType ==
                            QLV_NODE_TYPE_JOIN_TABLE )
                        {
                            sTmpNode = ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode;
                        }
                        else
                        {
                            sJoinTableNode->mLeftTableNode = sLeftNode;
                            sJoinTableNode->mRightTableNode = ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode;
                            ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode = (qlvInitNode*)sJoinTableNode;

                            break;
                        }
                    }
                }
            }
            else
            {
                STL_DASSERT( (sRightNode->mType == QLV_NODE_TYPE_BASE_TABLE) ||
                             (sRightNode->mType == QLV_NODE_TYPE_SUB_TABLE) );

                sJoinTableNode->mLeftTableNode = sLeftNode;
                sJoinTableNode->mRightTableNode = sRightNode;
                sRightNode = (qlvInitNode*)sJoinTableNode;
            }
        }
        else
        {
            sJoinTableNode->mJoinType = QLV_JOIN_TYPE_RIGHT_OUTER;

            if( sLeftNode->mType == QLV_NODE_TYPE_JOIN_TABLE )
            {
                sTmpNode = sLeftNode;
                while( STL_TRUE )
                {
                    if( qlvIsExistBaseNodeInTableNode(
                            sLeftBaseNode,
                            ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode )
                         == STL_TRUE )
                    {
                        if( ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode->mType ==
                         QLV_NODE_TYPE_JOIN_TABLE )
                        {
                            sTmpNode = ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode;
                        }
                        else
                        {
                            sJoinTableNode->mLeftTableNode = sRightNode;
                            sJoinTableNode->mRightTableNode = ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode;
                            ((qlvInitJoinTableNode*)sTmpNode)->mLeftTableNode = (qlvInitNode*)sJoinTableNode;

                            break;
                        }
                    }
                    else
                    {
                        STL_DASSERT( qlvIsExistBaseNodeInTableNode(
                                         sLeftBaseNode,
                                         ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode )
                                     == STL_TRUE );
                        if( ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode->mType ==
                            QLV_NODE_TYPE_JOIN_TABLE )
                        {
                            sTmpNode = ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode;
                        }
                        else
                        {
                            sJoinTableNode->mLeftTableNode = sRightNode;
                            sJoinTableNode->mRightTableNode = ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode;
                            ((qlvInitJoinTableNode*)sTmpNode)->mRightTableNode = (qlvInitNode*)sJoinTableNode;

                            break;
                        }
                    }
                }

                sRightNode = sLeftNode;
            }
            else
            {
                STL_DASSERT( (sLeftNode->mType == QLV_NODE_TYPE_BASE_TABLE) ||
                             (sLeftNode->mType == QLV_NODE_TYPE_SUB_TABLE) );

                sJoinTableNode->mLeftTableNode = sRightNode;
                sJoinTableNode->mRightTableNode = sLeftNode;
                sRightNode = (qlvInitNode*)sJoinTableNode;
            }

        }

        /* Filter를 셋팅 */
        sJoinTableNode->mJoinSpec->mJoinCondition = sAndItemPtrArr[i];

        /* Right Node를 Table Node Array에 넣는다. */
        for( j = 0; j < sTableCount; j++ )
        {
            if( sTableNodeArr[j] == NULL )
            {
                sTableNodeArr[j] = sRightNode;
                break;
            }
        }
        STL_DASSERT( j < sTableCount );
    }


    /**
     * Table Node Array에 남은 Node들을 Join Table Node로 묶는다.
     */

    sLeftNode = NULL;
    for( i = 0; i < sTableCount; i++ )
    {
        if( sTableNodeArr[i] == NULL )
        {
            continue;
        }

        if( sLeftNode == NULL )
        {
            sLeftNode = sTableNodeArr[i];
            sTableNodeArr[i] = NULL;
        }
        else
        {
            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                        STL_SIZEOF( qlvInitJoinTableNode ),
                                        (void**) &sJoinTableNode,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sJoinTableNode->mType = QLV_NODE_TYPE_JOIN_TABLE;
            sJoinTableNode->mJoinType = QLV_JOIN_TYPE_CROSS;
            sJoinTableNode->mJoinSpec = NULL;

            STL_TRY( qlvCreateRefExprList( &(sJoinTableNode->mRefColumnList),
                                           QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                           aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlvCreateRefExprList( &(sJoinTableNode->mOuterColumnList),
                                           QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                           aEnv )
                     == STL_SUCCESS );

            sJoinTableNode->mLeftTableNode = sLeftNode;
            sJoinTableNode->mRightTableNode = sTableNodeArr[i];

            sLeftNode = (qlvInitNode*)sJoinTableNode;

            sTableNodeArr[i] = NULL;
        }
    }
    STL_DASSERT( sLeftNode != NULL );

    /* Output Table Node 설정 */
    *aOutTableNode = sLeftNode;


    /**
     * 남은 Expression들을 묶어서 Where Expression으로 만든다.
     */

    if( sRefCount == sAndItemCount )
    {
        /* Output Where Expression 설정 */
        *aOutWhereExpr = NULL;
    }
    else
    {
        sFunction = aWhereExpr->mExpr.mFunction;
        for( i = sRefCount, j = 0; i < sAndItemCount; i++, j++ )
        {
            sFunction->mArgs[j] = sAndItemPtrArr[i];
        }
        sFunction->mArgCount = j;

        /* Output Where Expression 설정 */
        *aOutWhereExpr = aWhereExpr;
    }


    /**
     * 변경된 Table Node에 따른 Filter의 Column 재조정
     */

    STL_TRY( qlvAdjustFilterByTableNode( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                         *aOutTableNode,
                                         *aOutWhereExpr,
                                         aOutWhereExpr,
                                         aEnv )
             == STL_SUCCESS );


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_PREDICATE_MAY_REFERENCED_ONLY_ONE_OUTER_JOIN_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PREDICATE_MAY_REFERENCED_ONLY_ONE_OUTER_JOIN_TABLE,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sErrPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_MAY_BE_OUTER_JOINED_TO_AT_MOST_ONE_OTHER_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MAY_BE_OUTER_JOINED_TO_AT_MOST_ONE_OTHER_TABLE,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sErrPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_CANNOT_BE_OUTER_JOINED_TO_EACH_OTHER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CANNOT_BE_OUTER_JOINED_TO_EACH_OTHER,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sErrPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expression을 분석하여 참조하는 Column List를 구성한다.
 * @param[in]     aRegionMem        Region Memory
 * @param[in]     aExpr             Init Expression
 * @param[in,out] aTableColList     Expression이 참조하고 있는 Table Column List
 * @param[in]     aEnv              Environment
 */
inline stlStatus qlvGetRefTableColList( knlRegionMem        * aRegionMem,
                                        qlvInitExpression   * aExpr,
                                        qlvRefTableColList  * aTableColList,
                                        qllEnv              * aEnv )
{
    stlInt32             sLoop              = 0;

    qlvInitColumn       * sInitColumn       = NULL;
    qlvInitInnerColumn  * sInitInnerColumn  = NULL;
    qlvInitRowIdColumn  * sInitRowidColumn  = NULL;
    qlvInitFunction     * sInitFunction     = NULL;
    qlvInitTypeCast     * sInitTypeCast     = NULL;
    qlvInitCaseExpr     * sInitCaseExpr     = NULL;
    qlvRefTableColItem  * sTableColItem     = NULL;
    qlvInitListFunc     * sInitListFunc     = NULL;
    qlvInitListCol      * sInitListCol      = NULL;
    qlvInitRowExpr      * sInitRowExpr      = NULL;

    qlvInitNode         * sRelationNode     = NULL;
    stlBool               sIsSetOuterMark;
    
        
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableColList != NULL, QLL_ERROR_STACK(aEnv) );


#define _QLV_FIND_OR_ADD_TABLE_COLUMN_ITEM( _aRelationNode )                \
    {                                                                       \
        sTableColItem = aTableColList->mHead;                               \
        while( sTableColItem != NULL )                                      \
        {                                                                   \
            if( sTableColItem->mRelationNode == (_aRelationNode) )          \
            {                                                               \
                break;                                                      \
            }                                                               \
            else                                                            \
            {                                                               \
                sTableColItem = sTableColItem->mNext;                       \
            }                                                               \
        }                                                                   \
                                                                            \
        if( sTableColItem == NULL )                                         \
        {                                                                   \
            STL_TRY( knlAllocRegionMem( aRegionMem,                         \
                                        STL_SIZEOF( qlvRefTableColItem ),   \
                                        (void **) & sTableColItem,          \
                                        KNL_ENV(aEnv) )                     \
                     == STL_SUCCESS );                                      \
                                                                            \
            sTableColItem->mOuterJoinOperCount  = 0;                        \
            sTableColItem->mColumnCount         = 0;                        \
            sTableColItem->mRelationNode        = (_aRelationNode);         \
            sTableColItem->mNext                = NULL;                     \
                                                                            \
            if( aTableColList->mCount == 0 )                                \
            {                                                               \
                aTableColList->mHead = sTableColItem;                       \
                aTableColList->mTail = sTableColItem;                       \
            }                                                               \
            else                                                            \
            {                                                               \
                aTableColList->mTail->mNext = sTableColItem;                \
                aTableColList->mTail = sTableColItem;                       \
            }                                                               \
            aTableColList->mCount++;                                        \
        }                                                                   \
        else                                                                \
        {                                                                   \
            /* Do Nothing */                                                \
        }                                                                   \
    }

    /**
     * Column List 구성
     */

    switch( aExpr->mType )
    {
        case QLV_EXPR_TYPE_COLUMN :
            {
                /**
                 * Column List에 Column 추가
                 */

                sInitColumn = aExpr->mExpr.mColumn;

                _QLV_FIND_OR_ADD_TABLE_COLUMN_ITEM( sInitColumn->mRelationNode );
                sTableColItem->mColumnCount++;

                if( sInitColumn->mIsSetOuterMark == STL_TRUE )
                {
                    if( sTableColItem->mOuterJoinOperCount == 0 )
                    {
                        aTableColList->mOuterJoinOperColCount++;
                    }

                    sTableColItem->mOuterJoinOperCount++;
                }

                break;
            }

        case QLV_EXPR_TYPE_FUNCTION :
            {
                sInitFunction = aExpr->mExpr.mFunction;
                
                for( sLoop = 0 ; sLoop < sInitFunction->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvGetRefTableColList( aRegionMem,
                                                    sInitFunction->mArgs[ sLoop ],
                                                    aTableColList,
                                                    aEnv )
                             == STL_SUCCESS );
                }
                
                break;
            }
            
        case QLV_EXPR_TYPE_CAST :
            {
                sInitTypeCast = aExpr->mExpr.mTypeCast;
                
                for( sLoop = 0 ; sLoop < DTL_CAST_INPUT_ARG_CNT ; sLoop++ )
                {
                    STL_TRY( qlvGetRefTableColList( aRegionMem,
                                                    sInitTypeCast->mArgs[ sLoop ],
                                                    aTableColList,
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

                sInitInnerColumn = aExpr->mExpr.mInnerColumn;
                sIsSetOuterMark = sInitInnerColumn->mIsSetOuterMark;

                while( STL_TRUE )
                {
                    if( sInitInnerColumn->mRelationNode->mType == QLV_NODE_TYPE_SUB_TABLE )
                    {
                        sRelationNode = sInitInnerColumn->mRelationNode;
                        break;
                    }
                    else
                    {
                        if( sInitInnerColumn->mOrgExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
                        {
                            sInitInnerColumn = sInitInnerColumn->mOrgExpr->mExpr.mInnerColumn;
                        }
                        else
                        {
                            if( sInitInnerColumn->mOrgExpr->mType == QLV_EXPR_TYPE_COLUMN )
                            {
                                sRelationNode = sInitInnerColumn->mOrgExpr->mExpr.mColumn->mRelationNode;
                            }
                            else
                            {
                                STL_DASSERT( sInitInnerColumn->mOrgExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN );
                                sRelationNode = sInitInnerColumn->mOrgExpr->mExpr.mRowIdColumn->mRelationNode;
                            }

                            break;
                        }
                    }
                }


                _QLV_FIND_OR_ADD_TABLE_COLUMN_ITEM( sRelationNode );
                sTableColItem->mColumnCount++;

                if( sIsSetOuterMark == STL_TRUE )
                {
                    if( sTableColItem->mOuterJoinOperCount == 0 )
                    {
                        aTableColList->mOuterJoinOperColCount++;
                    }

                    sTableColItem->mOuterJoinOperCount++;
                }

                break;
            }

        case QLV_EXPR_TYPE_OUTER_COLUMN :
            {
                aTableColList->mOuterColumnCount++;

                break;
            }

        case QLV_EXPR_TYPE_ROWID_COLUMN :
            {
                /**
                 * Column List에 Rowid Column 추가
                 */

                sInitRowidColumn = aExpr->mExpr.mRowIdColumn;

                _QLV_FIND_OR_ADD_TABLE_COLUMN_ITEM( sInitRowidColumn->mRelationNode );
                sTableColItem->mColumnCount++;

                if( sInitRowidColumn->mIsSetOuterMark == STL_TRUE )
                {
                    if( sTableColItem->mOuterJoinOperCount == 0 )
                    {
                        aTableColList->mOuterJoinOperColCount++;
                    }

                    sTableColItem->mOuterJoinOperCount++;
                }

                break;
            }

        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                sInitCaseExpr = aExpr->mExpr.mCaseExpr;
                
                for( sLoop = 0 ; sLoop < sInitCaseExpr->mCount ; sLoop++ )
                {
                    STL_TRY( qlvGetRefTableColList( aRegionMem,
                                                    sInitCaseExpr->mWhenArr[ sLoop ],
                                                    aTableColList,
                                                    aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlvGetRefTableColList( aRegionMem,
                                                    sInitCaseExpr->mThenArr[ sLoop ],
                                                    aTableColList,
                                                    aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlvGetRefTableColList( aRegionMem,
                                                sInitCaseExpr->mDefResult,
                                                aTableColList,
                                                aEnv )
                         == STL_SUCCESS );
                
                break;
            }

        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                sInitRowExpr = aExpr->mExpr.mRowExpr;

                for( sLoop = 0 ; sLoop < sInitRowExpr->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvGetRefTableColList( aRegionMem,
                                                    sInitRowExpr->mArgs[ sLoop ],
                                                    aTableColList,
                                                    aEnv )
                             == STL_SUCCESS );
                }

                break;
            }

        case QLV_EXPR_TYPE_LIST_COLUMN :
            {
                sInitListCol = aExpr->mExpr.mListCol;

                for( sLoop = 0 ; sLoop < sInitListCol->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvGetRefTableColList( aRegionMem,
                                                    sInitListCol->mArgs[ sLoop ],
                                                    aTableColList,
                                                    aEnv )
                             == STL_SUCCESS );
                }

                break;
            }

        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                sInitListFunc = aExpr->mExpr.mListFunc;

                for( sLoop = 0 ; sLoop < sInitListFunc->mArgCount ; sLoop++ )
                {
                    STL_TRY( qlvGetRefTableColList( aRegionMem,
                                                    sInitListFunc->mArgs[ sLoop ],
                                                    aTableColList,
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
 * @brief Table Node에 존재하는 Table Count를 반환한다.
 * @param[in]   aTableNode  Table Node
 * @param[out]  aTableCount Table Count
 */
inline void qlvGetTableCount( qlvInitNode   * aTableNode,
                              stlInt32      * aTableCount )
{
    STL_DASSERT( aTableNode != NULL );

    switch( aTableNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
        case QLV_NODE_TYPE_SUB_TABLE:
            (*aTableCount)++;
            break;

        case QLV_NODE_TYPE_JOIN_TABLE:
            qlvGetTableCount( ((qlvInitJoinTableNode*)aTableNode)->mLeftTableNode,
                              aTableCount );

            qlvGetTableCount( ((qlvInitJoinTableNode*)aTableNode)->mRightTableNode,
                              aTableCount );
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }
}


/**
 * @brief Table Node를 순차적으로 Array에 셋팅한다.
 * @param[in]       aTableNode      Table Node
 * @param[in,out]   aTableNodeArray Table Node Array
 * @param[in,out]   aCurNodeIdx     Current Node Idx
 */
inline void qlvSetTableNodeArray( qlvInitNode   * aTableNode,
                                  qlvInitNode  ** aTableNodeArray,
                                  stlInt32      * aCurNodeIdx )
{
    STL_DASSERT( aTableNode != NULL );

    switch( aTableNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
        case QLV_NODE_TYPE_SUB_TABLE:
            aTableNodeArray[*aCurNodeIdx] = aTableNode;
            (*aCurNodeIdx)++;
            break;

        case QLV_NODE_TYPE_JOIN_TABLE:
            qlvSetTableNodeArray( ((qlvInitJoinTableNode*)aTableNode)->mLeftTableNode,
                                  aTableNodeArray,
                                  aCurNodeIdx );

            qlvSetTableNodeArray( ((qlvInitJoinTableNode*)aTableNode)->mRightTableNode,
                                  aTableNodeArray,
                                  aCurNodeIdx );
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }
}


/**
 * @brief Srouce Table Node에 Base Table Node가 존재하는지 판단한다.
 * @param[in]   aBaseTableNode  Base Table Node
 * @param[in]   aSrcTableNode   Source Table Node
 */
inline stlBool qlvIsExistBaseNodeInTableNode( qlvInitNode   * aBaseTableNode,
                                              qlvInitNode   * aSrcTableNode )
{
    STL_DASSERT( aBaseTableNode != NULL );
    STL_DASSERT( aSrcTableNode != NULL );


    switch( aSrcTableNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
        case QLV_NODE_TYPE_SUB_TABLE:
            if( aBaseTableNode == aSrcTableNode )
            {
                return STL_TRUE;
            }
            else
            {
                return STL_FALSE;
            }

            break;

        case QLV_NODE_TYPE_JOIN_TABLE:
            if( qlvIsExistBaseNodeInTableNode( aBaseTableNode,
                                               ((qlvInitJoinTableNode*)aSrcTableNode)->mLeftTableNode )
                == STL_TRUE )
            {
                return STL_TRUE;
            }
            else
            {
                return qlvIsExistBaseNodeInTableNode(
                           aBaseTableNode,
                           ((qlvInitJoinTableNode*)aSrcTableNode)->mRightTableNode );

            }

            break;

        default:
            STL_DASSERT( 0 );
            break;
    }

    return STL_FALSE;
}


/**
 * @brief 재구성된 Table Node에 의해 Filter의 Column 정보를 재조정 한다.
 * @param[in]   aRegionMem      Region Memory
 * @param[in]   aTableNode      Table Node
 * @param[in]   aExpr           Expression
 * @param[out]  aOutExpr        Output Expression
 * @param[in]   aEnv            Environment
 */
inline stlStatus qlvAdjustFilterByTableNode( knlRegionMem       * aRegionMem,
                                             qlvInitNode        * aTableNode,
                                             qlvInitExpression  * aExpr,
                                             qlvInitExpression ** aOutExpr,
                                             qllEnv             * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );


    if( aTableNode->mType != QLV_NODE_TYPE_JOIN_TABLE )
    {
        if( aExpr != NULL )
        {
            *aOutExpr = aExpr;
        }

        STL_THROW( RAMP_FINISH );
    }

    /* 하위 노드부터 처리 */
    STL_TRY( qlvAdjustFilterByTableNode( aRegionMem,
                                         ((qlvInitJoinTableNode*)aTableNode)->mLeftTableNode,
                                         NULL,
                                         NULL,
                                         aEnv )
             == STL_SUCCESS );

    STL_TRY( qlvAdjustFilterByTableNode( aRegionMem,
                                         ((qlvInitJoinTableNode*)aTableNode)->mRightTableNode,
                                         NULL,
                                         NULL,
                                         aEnv )
             == STL_SUCCESS );

    /* 현재 노드에 Filter가 존재하는 경우 이에 대한 처리 */
    if( ((qlvInitJoinTableNode*)aTableNode)->mJoinSpec != NULL )
    {
        STL_DASSERT( ((qlvInitJoinTableNode*)aTableNode)->mJoinSpec->mJoinCondition != NULL );

        STL_TRY( qlvAdjustColumnByTableNode(
                     aRegionMem,
                     aTableNode,
                     ((qlvInitJoinTableNode*)aTableNode)->mJoinSpec->mJoinCondition,
                     &(((qlvInitJoinTableNode*)aTableNode)->mJoinSpec->mJoinCondition),
                     aEnv )
                 == STL_SUCCESS );
    }

    /* 상위노드에서 보낸 Filter에 대한 처리 */
    if( aExpr != NULL )
    {
        STL_TRY( qlvAdjustColumnByTableNode(
                     aRegionMem,
                     aTableNode,
                     aExpr,
                     aOutExpr,
                     aEnv )
                 == STL_SUCCESS );
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 재구성된 Table Node에 의해 Filter의 Column 정보를 재조정 한다.
 * @param[in]   aRegionMem      Region Memory
 * @param[in]   aTableNode      Table Node
 * @param[in]   aExpr           Expression
 * @param[out]  aOutExpr        Output Expression
 * @param[in]   aEnv            Environment
 */
inline stlStatus qlvAdjustColumnByTableNode( knlRegionMem       * aRegionMem,
                                             qlvInitNode        * aTableNode,
                                             qlvInitExpression  * aExpr,
                                             qlvInitExpression ** aOutExpr,
                                             qllEnv             * aEnv )
{
    stlInt32      i;
    stlInt32      sColumnPos;
    qlvInitNode * sRelationNode = NULL;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aExpr->mType )
    {
        case QLV_EXPR_TYPE_FUNCTION :
            {
                qlvInitFunction * sInitFunction = NULL;

                sInitFunction = aExpr->mExpr.mFunction;
                
                for( i = 0 ; i < sInitFunction->mArgCount ; i++ )
                {
                    STL_TRY( qlvAdjustColumnByTableNode( aRegionMem,
                                                         aTableNode,
                                                         sInitFunction->mArgs[i],
                                                         &(sInitFunction->mArgs[i]),
                                                         aEnv )
                             == STL_SUCCESS );
                }

                *aOutExpr = aExpr;
                
                break;
            }
            
        case QLV_EXPR_TYPE_CAST :
            {
                qlvInitTypeCast * sInitTypeCast = NULL;

                sInitTypeCast = aExpr->mExpr.mTypeCast;
                
                for( i = 0 ; i < DTL_CAST_INPUT_ARG_CNT ; i++ )
                {
                    STL_TRY( qlvAdjustColumnByTableNode( aRegionMem,
                                                         aTableNode,
                                                         sInitTypeCast->mArgs[i],
                                                         &(sInitTypeCast->mArgs[i]),
                                                         aEnv )
                             == STL_SUCCESS );
                }

                *aOutExpr = aExpr;
                
                break;
            }

        case QLV_EXPR_TYPE_INNER_COLUMN : 
            {
                qlvInitInnerColumn  * sInitInnerColumn  = NULL;

                sInitInnerColumn = aExpr->mExpr.mInnerColumn;

                while( STL_TRUE )
                {
                    if( (sInitInnerColumn->mOrgExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN) &&
                        (sInitInnerColumn->mOrgExpr->mExpr.mInnerColumn->mRelationNode->mType != QLV_NODE_TYPE_SUB_TABLE) )
                    {
                        sInitInnerColumn = sInitInnerColumn->mOrgExpr->mExpr.mInnerColumn;
                    }
                    else 
                    {
                        break;
                    }
                }

                if( sInitInnerColumn->mOrgExpr->mType == QLV_EXPR_TYPE_COLUMN )
                {
                    sRelationNode = sInitInnerColumn->mOrgExpr->mExpr.mColumn->mRelationNode;
                    sColumnPos = sInitInnerColumn->mOrgExpr->mPosition;
                }
                else if( sInitInnerColumn->mOrgExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN )
                {
                    sRelationNode = sInitInnerColumn->mOrgExpr->mExpr.mRowIdColumn->mRelationNode;
                    sColumnPos = sInitInnerColumn->mOrgExpr->mPosition;
                }
                else
                {
                    STL_DASSERT( sInitInnerColumn->mOrgExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
                    sRelationNode = sInitInnerColumn->mOrgExpr->mExpr.mInnerColumn->mRelationNode;
                    sColumnPos = sInitInnerColumn->mOrgExpr->mPosition;
                }

                STL_TRY( qlvAddColumnInJoinTableNode( aRegionMem,
                                                      aTableNode,
                                                      sRelationNode,
                                                      sColumnPos,
                                                      sInitInnerColumn->mOrgExpr,
                                                      aOutExpr,
                                                      aEnv )
                         == STL_SUCCESS );

                break;
            }

        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                qlvInitCaseExpr * sInitCaseExpr = NULL;

                sInitCaseExpr = aExpr->mExpr.mCaseExpr;
                
                for( i = 0 ; i < sInitCaseExpr->mCount ; i++ )
                {
                    STL_TRY( qlvAdjustColumnByTableNode( aRegionMem,
                                                         aTableNode,
                                                         sInitCaseExpr->mWhenArr[i],
                                                         &(sInitCaseExpr->mWhenArr[i]),
                                                         aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlvAdjustColumnByTableNode( aRegionMem,
                                                         aTableNode,
                                                         sInitCaseExpr->mThenArr[i],
                                                         &(sInitCaseExpr->mThenArr[i]),
                                                         aEnv )
                             == STL_SUCCESS );
                }

                STL_TRY( qlvAdjustColumnByTableNode( aRegionMem,
                                                     aTableNode,
                                                     sInitCaseExpr->mDefResult,
                                                     &(sInitCaseExpr->mDefResult),
                                                     aEnv )
                         == STL_SUCCESS );

                *aOutExpr = aExpr;

                break;
            }

        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                qlvInitRowExpr  * sInitRowExpr  = NULL;

                sInitRowExpr = aExpr->mExpr.mRowExpr;

                for( i = 0 ; i < sInitRowExpr->mArgCount ; i++ )
                {
                    STL_TRY( qlvAdjustColumnByTableNode( aRegionMem,
                                                         aTableNode,
                                                         sInitRowExpr->mArgs[i],
                                                         &(sInitRowExpr->mArgs[i]),
                                                         aEnv )
                             == STL_SUCCESS );
                }

                *aOutExpr = aExpr;

                break;
            }

        case QLV_EXPR_TYPE_LIST_COLUMN :
            {
                qlvInitListCol  * sInitListCol  = NULL;

                sInitListCol = aExpr->mExpr.mListCol;

                for( i = 0 ; i < sInitListCol->mArgCount ; i++ )
                {
                    STL_TRY( qlvAdjustColumnByTableNode( aRegionMem,
                                                         aTableNode,
                                                         sInitListCol->mArgs[i],
                                                         &(sInitListCol->mArgs[i]),
                                                         aEnv )
                             == STL_SUCCESS );
                }

                *aOutExpr = aExpr;

                break;
            }

        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                qlvInitListFunc * sInitListFunc = NULL;

                sInitListFunc = aExpr->mExpr.mListFunc;

                for( i = 0 ; i < sInitListFunc->mArgCount ; i++ )
                {
                    STL_TRY( qlvAdjustColumnByTableNode( aRegionMem,
                                                         aTableNode,
                                                         sInitListFunc->mArgs[i],
                                                         &(sInitListFunc->mArgs[i]),
                                                         aEnv )
                             == STL_SUCCESS );
                }

                *aOutExpr = aExpr;

                break;
            }
        /* 나머지 Expression들은 내부에 Column Expr을 포함할 수 없다 */
        default :   
            {
                *aOutExpr = aExpr;

                break;
            }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Table Node에 대하여 Column을 추가한다.
 * @param[in]   aRegionMem      Region Memory
 * @param[in]   aTableNode      Table Node
 * @param[in]   aNodeOfExpr     Expression의 Table Node
 * @param[in]   aColumnPos      Column Position
 * @param[in]   aExpr           Expression
 * @param[out]  aOutExpr        Output Expression
 * @param[in]   aEnv            Environment
 */
inline stlStatus qlvAddColumnInJoinTableNode( knlRegionMem          * aRegionMem,
                                              qlvInitNode           * aTableNode,
                                              qlvInitNode           * aNodeOfExpr,
                                              stlInt32                aColumnPos,
                                              qlvInitExpression     * aExpr,
                                              qlvInitExpression    ** aOutExpr,
                                              qllEnv                * aEnv )
{
    qlvInitExpression   * sExpr         = NULL;
    qlvRefExprItem      * sRefExprItem  = NULL;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNodeOfExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aTableNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
            STL_DASSERT( (aExpr->mType == QLV_EXPR_TYPE_COLUMN) ||
                         (aExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN) );
            *aOutExpr = aExpr;
            break;
        case QLV_NODE_TYPE_SUB_TABLE:
            STL_DASSERT( aExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
            *aOutExpr = aExpr;
            break;
        case QLV_NODE_TYPE_JOIN_TABLE:
            if( qlvIsExistBaseNodeInTableNode( aNodeOfExpr,
                                               ((qlvInitJoinTableNode*)aTableNode)->mLeftTableNode )
                == STL_TRUE )
            {
                STL_TRY( qlvAddColumnInJoinTableNode(
                             aRegionMem,
                             ((qlvInitJoinTableNode*)aTableNode)->mLeftTableNode,
                             aNodeOfExpr,
                             aColumnPos,
                             aExpr,
                             &sExpr,
                             aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlvAddRefColumnItemOnJoinTable( aRegionMem,
                                                         QLV_EXPR_PHRASE_CONDITION,
                                                         aColumnPos,
                                                         (qlvInitJoinTableNode*)aTableNode,
                                                         STL_TRUE,
                                                         sExpr,
                                                         STL_FALSE,
                                                         &sRefExprItem,
                                                         aEnv )
                         == STL_SUCCESS );

                *aOutExpr = sRefExprItem->mExprPtr;
            }
            else
            {
                STL_DASSERT( qlvIsExistBaseNodeInTableNode(
                                 aNodeOfExpr,
                                 ((qlvInitJoinTableNode*)aTableNode)->mRightTableNode )
                             == STL_TRUE );

                STL_TRY( qlvAddColumnInJoinTableNode(
                             aRegionMem,
                             ((qlvInitJoinTableNode*)aTableNode)->mRightTableNode,
                             aNodeOfExpr,
                             aColumnPos,
                             aExpr,
                             &sExpr,
                             aEnv )
                         == STL_SUCCESS );

                STL_TRY( qlvAddRefColumnItemOnJoinTable( aRegionMem,
                                                         QLV_EXPR_PHRASE_CONDITION,
                                                         aColumnPos,
                                                         (qlvInitJoinTableNode*)aTableNode,
                                                         STL_FALSE,
                                                         sExpr,
                                                         STL_FALSE,
                                                         &sRefExprItem,
                                                         aEnv )
                         == STL_SUCCESS );

                *aOutExpr = sRefExprItem->mExprPtr;
            }
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
 * @brief Expression의 Data Type 정보를 얻어온다. (Target의 Original Expression에 대해 평가)
 * @param[in]  aSQLString           SQL String
 * @param[in]  aRegionMem           Region Memory
 * @param[in]  aTargetExpr          Target Expression
 * @param[in]  aBindContext         Bind Context 
 * @param[out] aDataTypeInfo        Data Type Info
 * @param[in]  aEnv                 Environment
 * @remark Bind Variable이 있으나 Bind Type 이전에 호출된 경우,
 *    <BR> Bind Variable은 평가 대상에서 제외시킨다.
 *    <BR> 만약 Query Set의 child node 의 양쪽 Target 모두 Bind Variable인 경우 Varchar로 간주한다.
 *    <BR> Query Set 노드마다 이 함수를 호출하여 Set Target Column을 구성한다.
 *    <BR> Candiate Plan 단계에서는 Bind Variable이 존재하는 경우에 한하여 이 함수를 호출한다.
 */
stlStatus qlvGetExprDataTypeInfo( stlChar              * aSQLString,
                                  knlRegionMem         * aRegionMem,
                                  qlvInitExpression    * aTargetExpr,
                                  knlBindContext       * aBindContext,
                                  qlvInitDataTypeInfo  * aDataTypeInfo,
                                  qllEnv               * aEnv )
{
    qlvInitExpression     * sExpr         = aTargetExpr;
    qlvInitExpression     * sTmpExpr      = NULL;
    ellDictHandle         * sColumnHandle = NULL;
    knlValueBlockList     * sValueBlock   = NULL;
    dtlDataType             sDataType     = DTL_TYPE_NA;
    stlInt32                sLoop         = 0;
    stlBool                 sIsValid      = STL_FALSE;
    stlBool                 sIsNullable   = STL_FALSE;
    
    qlvInitNode           * sInitNode     = NULL;
    qlvInitTarget         * sTargets      = NULL;
    qlvInitJoinTableNode  * sJoinNode     = NULL;
    qlvNamedColumnItem    * sNamedColItem = NULL;
    
    qlvInitDataTypeInfo     sTmpDataTypeInfo1;
    qlvInitDataTypeInfo     sTmpDataTypeInfo2;
    
    
    /**
     * qlvMakeSetTargets() 과 CreateCandNode()에서 호출함
     */

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataTypeInfo != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Value Expression 을 구성한다.
     */

    STL_RAMP( RAMP_RETRY );

    if( sExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
    {
        sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
    }

    switch( sExpr->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
            {
                STL_TRY( qloGetValueDataType( sExpr->mExpr.mValue,
                                              & sDataType,
                                              aEnv )
                         == STL_SUCCESS );
                
                if( sExpr->mExpr.mValue->mValueType == QLV_VALUE_TYPE_NULL )
                {
                    aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                    aDataTypeInfo->mNullable     = STL_TRUE;
                    aDataTypeInfo->mUpdatable    = STL_FALSE;
                    aDataTypeInfo->mIsIgnore     = STL_TRUE;

                    aDataTypeInfo->mDataType          = sDataType;
                    aDataTypeInfo->mArgNum1           = 1;
                    aDataTypeInfo->mArgNum2           = gResultDataTypeDef[ sDataType ].mArgNum2;
                    aDataTypeInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_OCTETS;
                    aDataTypeInfo->mIntervalIndicator = gResultDataTypeDef[ sDataType ].mIntervalIndicator;
                }
                else
                {
                    aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                    aDataTypeInfo->mNullable     = STL_FALSE;
                    aDataTypeInfo->mUpdatable    = STL_FALSE;
                    aDataTypeInfo->mIsIgnore     = STL_FALSE;
                    
                    aDataTypeInfo->mDataType = sDataType;
                    
                    switch( sDataType )
                    {
                        case DTL_TYPE_CHAR:
                        case DTL_TYPE_VARCHAR:

                            if( sExpr->mExpr.mValue->mInternalBindIdx != QLV_INTERNAL_BIND_VARIABLE_IDX_NA )
                            {
                                aDataTypeInfo->mArgNum1 = 1;
                                break;
                            }
                            else
                            {
                                aDataTypeInfo->mArgNum1 = stlStrlen( sExpr->mExpr.mValue->mData.mString );
                
                                if( aDataTypeInfo->mArgNum1 == 0 )
                                {
                                    aDataTypeInfo->mArgNum1  = 1;
                                    aDataTypeInfo->mNullable = STL_TRUE;
                                }
                                else
                                {
                                    STL_TRY( dtlGetMbstrlenWithLen( (KNL_DT_VECTOR(aEnv))->mGetCharSetIDFunc(aEnv),
                                                                    sExpr->mExpr.mValue->mData.mString,
                                                                    aDataTypeInfo->mArgNum1,
                                                                    & aDataTypeInfo->mArgNum1,
                                                                    KNL_DT_VECTOR(aEnv),
                                                                    aEnv,
                                                                    QLL_ERROR_STACK(aEnv) )
                                             == STL_SUCCESS );
                                    
                                    if( sDataType == DTL_TYPE_CHAR )
                                    {
                                        STL_TRY_THROW( aDataTypeInfo->mArgNum1 <= DTL_VARCHAR_MAX_PRECISION,
                                                       RAMP_ERR_INVALID_CHAR_LENGTH );
                                    }
                                    else
                                    {
                                        STL_TRY_THROW( aDataTypeInfo->mArgNum1 <= DTL_VARCHAR_MAX_PRECISION,
                                                       RAMP_ERR_INVALID_VARCHAR_LENGTH );
                                    }
                                }
                            }
                            
                            aDataTypeInfo->mArgNum2           = gResultDataTypeDef[ sDataType ].mArgNum2;
                            aDataTypeInfo->mStringLengthUnit  = DTL_STRING_LENGTH_UNIT_CHARACTERS;
                            aDataTypeInfo->mIntervalIndicator = gResultDataTypeDef[ sDataType ].mIntervalIndicator;
                            break;
                            
                        case DTL_TYPE_BINARY:
                        case DTL_TYPE_VARBINARY:

                            if( sExpr->mExpr.mValue->mInternalBindIdx != QLV_INTERNAL_BIND_VARIABLE_IDX_NA )
                            {
                                aDataTypeInfo->mArgNum1 = 1;
                                break;
                            }
                            else
                            {
                                STL_TRY( dtlValidateBitString( sExpr->mExpr.mValue->mData.mString,
                                                               & sIsValid,
                                                               & aDataTypeInfo->mArgNum1,
                                                               QLL_ERROR_STACK(aEnv) )
                                         == STL_SUCCESS );
                    
                                if( aDataTypeInfo->mArgNum1 == 0 )
                                {
                                    aDataTypeInfo->mArgNum1 = 1;
                                }
                                else
                                {
                                    /* Do Nothing */
                                }
                    
                                STL_DASSERT( sIsValid == STL_TRUE );
                            }
                
                            if ( sDataType == DTL_TYPE_BINARY )
                            {
                                STL_TRY_THROW( aDataTypeInfo->mArgNum1 <= DTL_VARBINARY_MAX_PRECISION,
                                               RAMP_ERR_INVALID_BINARY_LENGTH );
                            }
                            else
                            {
                                STL_TRY_THROW( aDataTypeInfo->mArgNum1 <= DTL_VARBINARY_MAX_PRECISION,
                                               RAMP_ERR_INVALID_VARBINARY_LENGTH );
                            }
                
                            aDataTypeInfo->mArgNum2           = gResultDataTypeDef[ sDataType ].mArgNum2;
                            aDataTypeInfo->mStringLengthUnit  = gResultDataTypeDef[ sDataType ].mStringLengthUnit;
                            aDataTypeInfo->mIntervalIndicator = gResultDataTypeDef[ sDataType ].mIntervalIndicator;
                            break;
                            
                        default:
                            if( sExpr->mExpr.mValue->mInternalBindIdx != QLV_INTERNAL_BIND_VARIABLE_IDX_NA )
                            {
                                aDataTypeInfo->mArgNum1 = 1;
                            }
                            else
                            {
                                aDataTypeInfo->mArgNum1 = gResultDataTypeDef[ sDataType ].mArgNum1;
                            }
                            
                            aDataTypeInfo->mArgNum2           = gResultDataTypeDef[ sDataType ].mArgNum2;
                            aDataTypeInfo->mStringLengthUnit  = gResultDataTypeDef[ sDataType ].mStringLengthUnit;
                            aDataTypeInfo->mIntervalIndicator = gResultDataTypeDef[ sDataType ].mIntervalIndicator;
                            
                            break;
                    }
                }
                
                break;
            }
        case QLV_EXPR_TYPE_BIND_PARAM :
            {
                aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                aDataTypeInfo->mNullable     = STL_TRUE;
                aDataTypeInfo->mUpdatable    = STL_FALSE;
                
                if( aBindContext == NULL )
                {
                    aDataTypeInfo->mIsIgnore = STL_TRUE;
                    
                    aDataTypeInfo->mDataType          = DTL_TYPE_VARCHAR;
                    aDataTypeInfo->mArgNum1           = gResultDataTypeDef[ DTL_TYPE_VARCHAR ].mArgNum1;
                    aDataTypeInfo->mArgNum2           = gResultDataTypeDef[ DTL_TYPE_VARCHAR ].mArgNum2;
                    aDataTypeInfo->mStringLengthUnit  = gResultDataTypeDef[ DTL_TYPE_VARCHAR ].mStringLengthUnit;
                    aDataTypeInfo->mIntervalIndicator = gResultDataTypeDef[ DTL_TYPE_VARCHAR ].mIntervalIndicator;
                }
                else
                {
                    aDataTypeInfo->mIsIgnore = STL_FALSE;
                    
                    STL_TRY( knlGetParamValueBlock( aBindContext,
                                                    sExpr->mExpr.mBindParam->mBindParamIdx + 1,
                                                    & sValueBlock,
                                                    NULL,
                                                    KNL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );

                    aDataTypeInfo->mDataType          = KNL_GET_BLOCK_DB_TYPE( sValueBlock );
                    aDataTypeInfo->mArgNum1           = KNL_GET_BLOCK_ARG_NUM1( sValueBlock );
                    aDataTypeInfo->mArgNum2           = KNL_GET_BLOCK_ARG_NUM2( sValueBlock );
                    aDataTypeInfo->mStringLengthUnit  = KNL_GET_BLOCK_STRING_LENGTH_UNIT( sValueBlock );
                    aDataTypeInfo->mIntervalIndicator = KNL_GET_BLOCK_INTERVAL_INDICATOR( sValueBlock );
                }
                
                break;
            }
        case QLV_EXPR_TYPE_COLUMN :
            {
                sColumnHandle = sExpr->mExpr.mColumn->mColumnHandle;

                aDataTypeInfo->mIsBaseColumn = STL_TRUE;
                aDataTypeInfo->mNullable     = ellGetColumnNullable( sColumnHandle );
                aDataTypeInfo->mUpdatable    = ellGetColumnUpdatable( sColumnHandle );
                aDataTypeInfo->mIsIgnore     = STL_FALSE;

                aDataTypeInfo->mDataType          = ellGetColumnDBType( sColumnHandle );
                aDataTypeInfo->mArgNum1           = ellGetColumnPrecision( sColumnHandle );
                aDataTypeInfo->mArgNum2           = ellGetColumnScale( sColumnHandle );
                aDataTypeInfo->mStringLengthUnit  = ellGetColumnStringLengthUnit( sColumnHandle );
                aDataTypeInfo->mIntervalIndicator = ellGetColumnIntervalIndicator( sColumnHandle );

                break;                
            }
        case QLV_EXPR_TYPE_FUNCTION :
            {
                dtlDataTypeDefInfo   sDataTypeDefInfo;
                dtlBuiltInFuncInfo * sFuncInfo     = NULL;
                qlvInitFunction    * sFuncCode     = NULL;
                stlInt32             sArgCount     = 0;
                stlUInt32            sFuncPtrIdx   = 0;
                stlUInt32            sCastPtrIdx   = 0;
                dtlDataType          sTempType     = DTL_TYPE_NA;
                
                stlBool              sTmpIsConstantData[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
                dtlDataType          sTmpInputArgDataTypes[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
                dtlDataType          sTmpCastArgDataTypes[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
                dtlDataType          sTmpResultArgDataTypes[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];

                dtlDataTypeDefInfo   sTmpCastArgDataTypeDefInfo[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
                dtlDataTypeDefInfo   sTmpInputArgDataTypeDefInfo[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
                dtlDataTypeDefInfo   sTmpResultArgDataTypeDefInfo[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];

                stlBool            * sIsConstantData = sTmpIsConstantData;
                dtlDataType        * sCastArgDataTypes = sTmpCastArgDataTypes;
                dtlDataType        * sInputArgDataTypes = sTmpInputArgDataTypes;
                dtlDataType        * sResultArgDataTypes = sTmpResultArgDataTypes;

                dtlDataTypeDefInfo * sCastArgDataTypeDefInfo = sTmpCastArgDataTypeDefInfo;
                dtlDataTypeDefInfo * sInputArgDataTypeDefInfo = sTmpInputArgDataTypeDefInfo;
                dtlDataTypeDefInfo * sResultArgDataTypeDefInfo = sTmpResultArgDataTypeDefInfo;
                
                aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                aDataTypeInfo->mNullable     = qlvCheckNullable( NULL, sExpr );
                aDataTypeInfo->mUpdatable    = STL_FALSE;
                aDataTypeInfo->mIsIgnore     = STL_FALSE;

                
                /**
                 * Check Precision & Scale 
                 */

                sFuncCode = sExpr->mExpr.mFunction;
                sArgCount = sFuncCode->mArgCount;
                
                if( sFuncCode->mArgCount > DTL_FUNC_INPUTARG_VARIABLE_CNT )
                {
                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                STL_SIZEOF( stlBool ) * sArgCount,
                                                (void**) &sIsConstantData,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                STL_SIZEOF( dtlDataType ) * sArgCount,
                                                (void**) &sInputArgDataTypes,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                STL_SIZEOF( dtlDataType ) * sArgCount,
                                                (void**) &sResultArgDataTypes,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                STL_SIZEOF( dtlDataTypeDefInfo ) * sArgCount,
                                                (void**) &sInputArgDataTypeDefInfo,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                STL_SIZEOF( dtlDataTypeDefInfo ) * sArgCount,
                                                (void**) &sResultArgDataTypeDefInfo,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }

                for( sLoop = 0; sLoop < sArgCount ; sLoop++ )
                {
                    STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                                     aRegionMem,
                                                     sFuncCode->mArgs[ sLoop ],
                                                     aBindContext,
                                                     & sTmpDataTypeInfo1,
                                                     aEnv )
                             == STL_SUCCESS );

                    sInputArgDataTypes[ sLoop ] = sTmpDataTypeInfo1.mDataType;
                    
                    sInputArgDataTypeDefInfo[ sLoop ].mArgNum1 =
                        sTmpDataTypeInfo1.mArgNum1;
                    sInputArgDataTypeDefInfo[ sLoop ].mArgNum2 =
                        sTmpDataTypeInfo1.mArgNum2;
                    sInputArgDataTypeDefInfo[ sLoop ].mStringLengthUnit =
                        sTmpDataTypeInfo1.mStringLengthUnit;
                    sInputArgDataTypeDefInfo[ sLoop ].mIntervalIndicator =
                        sTmpDataTypeInfo1.mIntervalIndicator;

                    switch( sFuncCode->mArgs[ sLoop ]->mIterationTime )
                    {
                        case DTL_ITERATION_TIME_FOR_EACH_QUERY :
                        case DTL_ITERATION_TIME_FOR_EACH_STATEMENT :
                        case DTL_ITERATION_TIME_NONE :
                            {
                                sIsConstantData[ sLoop ] = STL_TRUE;
                                break;
                            }
                        default :
                            {
                                sIsConstantData[ sLoop ] = STL_FALSE;
                                break;
                            }
                    }
                }

                    
                /**
                 * Function의 Return Data Type 결정하기
                 */

                sFuncInfo = & gBuiltInFuncInfoArr[ sFuncCode->mFuncId ];
    
                STL_TRY( sFuncInfo->mGetFuncDetailInfo( sArgCount,
                                                        sIsConstantData,
                                                        sInputArgDataTypes,
                                                        sInputArgDataTypeDefInfo,
                                                        sArgCount,
                                                        sResultArgDataTypes,
                                                        sResultArgDataTypeDefInfo,
                                                        & sDataType,
                                                        & sDataTypeDefInfo,
                                                        & sFuncPtrIdx,
                                                        KNL_DT_VECTOR(aEnv),
                                                        aEnv,
                                                        QLL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );

                for( sLoop = 0 ; sLoop < sArgCount ; sLoop++ )
                {
                    sExpr = sFuncCode->mArgs[ sLoop ];
                    sCastArgDataTypes[ 0 ] = sInputArgDataTypes[ sLoop ];
                    sCastArgDataTypes[ 1 ] = sResultArgDataTypes[ sLoop ];

                    sCastArgDataTypeDefInfo[ 0 ] = sInputArgDataTypeDefInfo[ sLoop ];
                    sCastArgDataTypeDefInfo[ 1 ] = sResultArgDataTypeDefInfo[ sLoop ];

                    if( dtlGetFuncInfoCast( 2,
                                            sIsConstantData,
                                            sCastArgDataTypes,
                                            sCastArgDataTypeDefInfo,
                                            1,
                                            sResultArgDataTypes,
                                            sResultArgDataTypeDefInfo,
                                            & sTempType,
                                            sResultArgDataTypeDefInfo,
                                            & sCastPtrIdx,
                                            KNL_DT_VECTOR(aEnv),
                                            aEnv,
                                            QLL_ERROR_STACK( aEnv ) )
                        != STL_SUCCESS )
                    {
                        sDataType = sResultArgDataTypes[ sLoop ];
                        (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                        STL_THROW( RAMP_ERR_NOT_APPLICABLE_INTERNAL_CAST );
                    }
                }

                aDataTypeInfo->mDataType          = sDataType;
                aDataTypeInfo->mArgNum1           = sDataTypeDefInfo.mArgNum1;
                aDataTypeInfo->mArgNum2           = sDataTypeDefInfo.mArgNum2;
                aDataTypeInfo->mStringLengthUnit  = sDataTypeDefInfo.mStringLengthUnit;
                aDataTypeInfo->mIntervalIndicator = sDataTypeDefInfo.mIntervalIndicator;

                break;
            }
        case QLV_EXPR_TYPE_CAST :
            {
                dtlDataTypeDefInfo   sDataTypeDefInfo;
                stlUInt32            sCastPtrIdx   = 0;

                stlBool              sTmpIsConstantData[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
                dtlDataType          sTmpInputArgDataTypes[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
                dtlDataType          sTmpResultArgDataTypes[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];

                dtlDataTypeDefInfo   sTmpInputArgDataTypeDefInfo[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
                dtlDataTypeDefInfo   sTmpResultArgDataTypeDefInfo[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];

                stlBool            * sIsConstantData = sTmpIsConstantData;
                dtlDataType        * sInputArgDataTypes = sTmpInputArgDataTypes;
                dtlDataType        * sResultArgDataTypes = sTmpResultArgDataTypes;

                dtlDataTypeDefInfo * sInputArgDataTypeDefInfo = sTmpInputArgDataTypeDefInfo;
                dtlDataTypeDefInfo * sResultArgDataTypeDefInfo = sTmpResultArgDataTypeDefInfo;
                
                aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                aDataTypeInfo->mNullable     = qlvCheckNullable( NULL, sExpr );
                aDataTypeInfo->mUpdatable    = STL_FALSE;
                aDataTypeInfo->mIsIgnore     = STL_FALSE;

                STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                                 aRegionMem,
                                                 sExpr->mExpr.mTypeCast->mArgs[0],
                                                 aBindContext,
                                                 & sTmpDataTypeInfo1,
                                                 aEnv )
                         == STL_SUCCESS );

                sInputArgDataTypes[ sLoop ] = sTmpDataTypeInfo1.mDataType;
                    
                sInputArgDataTypeDefInfo[ sLoop ].mArgNum1 =
                    sTmpDataTypeInfo1.mArgNum1;
                sInputArgDataTypeDefInfo[ sLoop ].mArgNum2 =
                    sTmpDataTypeInfo1.mArgNum2;
                sInputArgDataTypeDefInfo[ sLoop ].mStringLengthUnit =
                    sTmpDataTypeInfo1.mStringLengthUnit;
                sInputArgDataTypeDefInfo[ sLoop ].mIntervalIndicator =
                    sTmpDataTypeInfo1.mIntervalIndicator;

                sInputArgDataTypes[ 1 ] = sExpr->mExpr.mTypeCast->mTypeDef.mDBType;

                sInputArgDataTypeDefInfo[1].mArgNum1 = sExpr->mExpr.mTypeCast->mTypeDef.mArgNum1;
                sInputArgDataTypeDefInfo[1].mArgNum2 = sExpr->mExpr.mTypeCast->mTypeDef.mArgNum2;
                sInputArgDataTypeDefInfo[1].mStringLengthUnit =
                    sExpr->mExpr.mTypeCast->mTypeDef.mStringLengthUnit;
                sInputArgDataTypeDefInfo[1].mIntervalIndicator =
                    sExpr->mExpr.mTypeCast->mTypeDef.mIntervalIndicator;
                    
                STL_TRY( dtlGetFuncInfoCast( 2,
                                             sIsConstantData,
                                             sInputArgDataTypes,
                                             sInputArgDataTypeDefInfo,
                                             1,
                                             sResultArgDataTypes,
                                             sResultArgDataTypeDefInfo,
                                             & sDataType,
                                             & sDataTypeDefInfo,
                                             & sCastPtrIdx,
                                             KNL_DT_VECTOR(aEnv),
                                             aEnv,
                                             QLL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );

                aDataTypeInfo->mDataType          = sDataType;
                aDataTypeInfo->mArgNum1           = sDataTypeDefInfo.mArgNum1;
                aDataTypeInfo->mArgNum2           = sDataTypeDefInfo.mArgNum2;
                aDataTypeInfo->mStringLengthUnit  = sDataTypeDefInfo.mStringLengthUnit;
                aDataTypeInfo->mIntervalIndicator = sDataTypeDefInfo.mIntervalIndicator;
                
                break;
            }
        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
            {
                aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                aDataTypeInfo->mNullable     = STL_FALSE;
                aDataTypeInfo->mUpdatable    = STL_FALSE;
                aDataTypeInfo->mIsIgnore     = STL_FALSE;

                aDataTypeInfo->mDataType          =
                    gPseudoColInfoArr[ sExpr->mExpr.mPseudoCol->mPseudoId ].mDataType;
                aDataTypeInfo->mArgNum1           =
                    gPseudoColInfoArr[ sExpr->mExpr.mPseudoCol->mPseudoId ].mArgNum1;
                aDataTypeInfo->mArgNum2           =
                    gPseudoColInfoArr[ sExpr->mExpr.mPseudoCol->mPseudoId ].mArgNum2;
                aDataTypeInfo->mStringLengthUnit  =
                    gPseudoColInfoArr[ sExpr->mExpr.mPseudoCol->mPseudoId ].mStringLengthUnit;
                aDataTypeInfo->mIntervalIndicator =
                    gPseudoColInfoArr[ sExpr->mExpr.mPseudoCol->mPseudoId ].mIntervalIndicator;
                
                break;
            }
        case QLV_EXPR_TYPE_REFERENCE :
            {
                STL_DASSERT( sExpr->mExpr.mReference->mOrgExpr->mType != QLV_EXPR_TYPE_SUB_QUERY );

                STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                                 aRegionMem,
                                                 sExpr->mExpr.mReference->mOrgExpr,
                                                 aBindContext,
                                                 aDataTypeInfo,
                                                 aEnv )
                         == STL_SUCCESS );

                aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                aDataTypeInfo->mUpdatable    = STL_FALSE;

                break;
            }
        case QLV_EXPR_TYPE_INNER_COLUMN :
            {
                STL_DASSERT( sExpr->mExpr.mInnerColumn->mRelationNode != NULL );

                sInitNode = sExpr->mExpr.mInnerColumn->mRelationNode;

                STL_RAMP( RAMP_CHECK_INSTANT );
                switch( sInitNode->mType )
                {
                    case QLV_NODE_TYPE_FLAT_INSTANT :
                    case QLV_NODE_TYPE_SORT_INSTANT :
                    case QLV_NODE_TYPE_HASH_INSTANT :
                    case QLV_NODE_TYPE_GROUP_HASH_INSTANT :
                    case QLV_NODE_TYPE_GROUP_SORT_INSTANT :
                    case QLV_NODE_TYPE_SORT_JOIN_INSTANT :
                    case QLV_NODE_TYPE_HASH_JOIN_INSTANT :
                        sExpr = sExpr->mExpr.mInnerColumn->mOrgExpr;

                        if( sExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
                        {
                            sInitNode = sExpr->mExpr.mInnerColumn->mRelationNode;
                            STL_THROW( RAMP_CHECK_INSTANT );
                        }
                        else
                        {
                            STL_THROW( RAMP_RETRY );
                        }
                        
                    default :
                        break;
                }   

                switch( sInitNode->mType )
                {
                    case QLV_NODE_TYPE_QUERY_SET :
                        sTargets = ((qlvInitQuerySetNode *)sInitNode)->mSetTargets;
                        *aDataTypeInfo = sTargets[ *sExpr->mExpr.mInnerColumn->mIdx ].mDataTypeInfo;
                        aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                        break;
                    case QLV_NODE_TYPE_QUERY_SPEC :
                        sTargets = ((qlvInitQuerySpecNode *)sInitNode)->mTargets;
                        *aDataTypeInfo = sTargets[ *sExpr->mExpr.mInnerColumn->mIdx ].mDataTypeInfo;
                        break;
                    case QLV_NODE_TYPE_SUB_TABLE :
                        sTargets = ((qlvInitSubTableNode *)sInitNode)->mColumns;
                        *aDataTypeInfo = sTargets[ *sExpr->mExpr.mInnerColumn->mIdx ].mDataTypeInfo;
                        aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                        break;
                    case QLV_NODE_TYPE_JOIN_TABLE :
                        sJoinNode = ((qlvInitJoinTableNode *)sInitNode);

                        if( sJoinNode->mJoinSpec != NULL )
                        {
                            if( sJoinNode->mJoinSpec->mNamedColumnList != NULL )
                            {
                                if( sJoinNode->mJoinSpec->mNamedColumnList->mCount > 0 )
                                {
                                    /* Result Type Combination */
                                    sNamedColItem = sJoinNode->mJoinSpec->mNamedColumnList->mHead;

                                    while( sNamedColItem != NULL )
                                    {
                                        if( ( qlvIsSameExpression( sNamedColItem->mLeftExpr,
                                                                   sExpr->mExpr.mInnerColumn->mOrgExpr ) == STL_TRUE ) ||
                                            ( qlvIsSameExpression( sNamedColItem->mRightExpr,
                                                                   sExpr->mExpr.mInnerColumn->mOrgExpr ) == STL_TRUE ) )
                                        {
                                            STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                                                             aRegionMem,
                                                                             sNamedColItem->mLeftExpr,
                                                                             aBindContext,
                                                                             & sTmpDataTypeInfo1,
                                                                             aEnv )
                                                     == STL_SUCCESS );

                                            STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                                                             aRegionMem,
                                                                             sNamedColItem->mRightExpr,
                                                                             aBindContext,
                                                                             & sTmpDataTypeInfo2,
                                                                             aEnv )
                                                     == STL_SUCCESS );

                                            if( sTmpDataTypeInfo1.mIsIgnore == STL_TRUE )
                                            {
                                                if( sTmpDataTypeInfo2.mIsIgnore == STL_TRUE )
                                                {
                                                    aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                                                    aDataTypeInfo->mNullable     = STL_TRUE;
                                                    aDataTypeInfo->mUpdatable    = STL_FALSE;
                                                    aDataTypeInfo->mIsIgnore     = STL_TRUE;
                    
                                                    aDataTypeInfo->mDataType          = DTL_TYPE_VARCHAR;
                                                    aDataTypeInfo->mArgNum1           = 1;
                                                    aDataTypeInfo->mArgNum2           =
                                                        gResultDataTypeDef[ DTL_TYPE_VARCHAR ].mArgNum2;
                                                    aDataTypeInfo->mStringLengthUnit  =
                                                        gResultDataTypeDef[ DTL_TYPE_VARCHAR ].mStringLengthUnit;
                                                    aDataTypeInfo->mIntervalIndicator =
                                                        gResultDataTypeDef[ DTL_TYPE_VARCHAR ].mIntervalIndicator;
                                                }
                                                else
                                                {
                                                    *aDataTypeInfo = sTmpDataTypeInfo2;
                                                }
                                            }
                                            else
                                            {
                                                if( sTmpDataTypeInfo2.mIsIgnore == STL_TRUE )
                                                {
                                                    *aDataTypeInfo = sTmpDataTypeInfo1;
                                                }
                                                else
                                                {
                                                    /* Result Set Combination */
                                                    STL_TRY( qlvGetResultTypeCombiantion(
                                                                 aSQLString,
                                                                 aBindContext,
                                                                 sExpr->mExpr.mInnerColumn->mOrgExpr->mPosition,
                                                                 STL_TRUE,
                                                                 & sTmpDataTypeInfo1,
                                                                 & sTmpDataTypeInfo2,
                                                                 aDataTypeInfo,
                                                                 aEnv )
                                                             == STL_SUCCESS );
                                                }
                                            }

                                            aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                                            aDataTypeInfo->mUpdatable    = STL_FALSE;

                                            STL_THROW( RAMP_FINISH );
                                        }

                                        sNamedColItem = sNamedColItem->mNext;
                                    }
                                }
                            }
                        }

                        
                        /**
                         * join type에 따른 updatable 체크 필요
                         */

                        STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                                         aRegionMem,
                                                         sExpr->mExpr.mInnerColumn->mOrgExpr,
                                                         aBindContext,
                                                         aDataTypeInfo,
                                                         aEnv )
                                 == STL_SUCCESS );

                        if( aDataTypeInfo->mIsBaseColumn == STL_TRUE )
                        {
                            switch( sJoinNode->mJoinType )
                            {
                                case QLV_JOIN_TYPE_FULL_OUTER :
                                    {
                                        aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                                        aDataTypeInfo->mNullable     = STL_TRUE;
                                        aDataTypeInfo->mUpdatable    = STL_FALSE;
                                        break;
                                    }
                                case QLV_JOIN_TYPE_LEFT_OUTER:
                                case QLV_JOIN_TYPE_LEFT_SEMI:
                                case QLV_JOIN_TYPE_LEFT_ANTI_SEMI:
                                case QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA:
                                case QLV_JOIN_TYPE_INVERTED_LEFT_SEMI:
                                    {
                                        if( sExpr->mExpr.mInnerColumn->mOrgExpr->mType == QLV_EXPR_TYPE_COLUMN )
                                        {
                                            if( sJoinNode->mRightTableNode ==
                                                sExpr->mExpr.mInnerColumn->mOrgExpr->mExpr.mColumn->mRelationNode )
                                            {
                                                aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                                                aDataTypeInfo->mNullable     = STL_TRUE;
                                                aDataTypeInfo->mUpdatable    = STL_FALSE;
                                            }
                                        }
                                        else if( sExpr->mExpr.mInnerColumn->mOrgExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
                                        {
                                            if( sJoinNode->mRightTableNode ==
                                                sExpr->mExpr.mInnerColumn->mOrgExpr->mExpr.mInnerColumn->mRelationNode )
                                            {
                                                aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                                                aDataTypeInfo->mNullable     = STL_TRUE;
                                                aDataTypeInfo->mUpdatable    = STL_FALSE;
                                            }
                                        }
                                        else
                                        {
                                            /* Do Nothing */
                                        }
                                        break;
                                    }
                                case QLV_JOIN_TYPE_RIGHT_OUTER:
                                case QLV_JOIN_TYPE_RIGHT_SEMI:
                                case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI:
                                case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA:
                                case QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI:
                                    {
                                        if( sExpr->mExpr.mInnerColumn->mOrgExpr->mType == QLV_EXPR_TYPE_COLUMN )
                                        {
                                            if( sJoinNode->mLeftTableNode ==
                                                sExpr->mExpr.mInnerColumn->mOrgExpr->mExpr.mColumn->mRelationNode )
                                            {
                                                aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                                                aDataTypeInfo->mNullable     = STL_TRUE;
                                                aDataTypeInfo->mUpdatable    = STL_FALSE;
                                            }
                                        }
                                        else if( sExpr->mExpr.mInnerColumn->mOrgExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
                                        {
                                            if( sJoinNode->mLeftTableNode ==
                                                sExpr->mExpr.mInnerColumn->mOrgExpr->mExpr.mInnerColumn->mRelationNode )
                                            {
                                                aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                                                aDataTypeInfo->mNullable     = STL_TRUE;
                                                aDataTypeInfo->mUpdatable    = STL_FALSE;
                                            }
                                        }
                                        else
                                        {
                                            /* Do Nothing */
                                        }
                                        break;
                                    }
                                default :
                                    {
                                        aDataTypeInfo->mNullable = qlvCheckNullable( NULL, sExpr );
                                        break;
                                    }
                            }
                        }
                        else
                        {
                            aDataTypeInfo->mNullable = qlvCheckNullable( NULL, sExpr );
                        }

                        STL_THROW( RAMP_FINISH );

                    case QLV_NODE_TYPE_BASE_TABLE :
                        sExpr = sExpr->mExpr.mInnerColumn->mOrgExpr;
                        STL_THROW( RAMP_RETRY );

                    default:
                        STL_DASSERT( 0 );
                        break;
                }

                break;
            }
        case QLV_EXPR_TYPE_OUTER_COLUMN :
            {
                STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                                 aRegionMem,
                                                 sExpr->mExpr.mOuterColumn->mOrgExpr,
                                                 aBindContext,
                                                 aDataTypeInfo,
                                                 aEnv )
                         == STL_SUCCESS );

                aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                aDataTypeInfo->mUpdatable    = STL_FALSE;

                break;
            }
        case QLV_EXPR_TYPE_ROWID_COLUMN :
            {
                aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                aDataTypeInfo->mNullable     = STL_FALSE;
                aDataTypeInfo->mUpdatable    = STL_FALSE;
                aDataTypeInfo->mIsIgnore     = STL_FALSE;
                    
                aDataTypeInfo->mDataType          = DTL_TYPE_ROWID;
                aDataTypeInfo->mArgNum1           = gResultDataTypeDef[ DTL_TYPE_ROWID ].mArgNum1;
                aDataTypeInfo->mArgNum2           = gResultDataTypeDef[ DTL_TYPE_ROWID ].mArgNum2;
                aDataTypeInfo->mStringLengthUnit  = gResultDataTypeDef[ DTL_TYPE_ROWID ].mStringLengthUnit;
                aDataTypeInfo->mIntervalIndicator = gResultDataTypeDef[ DTL_TYPE_ROWID ].mIntervalIndicator;

                break;
            }
        case QLV_EXPR_TYPE_AGGREGATION :
            {
                switch( sExpr->mExpr.mAggregation->mAggrId )
                {
                    case KNL_BUILTIN_AGGREGATION_SUM :
                        {
                            STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                                             aRegionMem,
                                                             sExpr->mExpr.mAggregation->mArgs[ 0 ],
                                                             aBindContext,
                                                             aDataTypeInfo,
                                                             aEnv )
                                     == STL_SUCCESS );

                            sDataType = aDataTypeInfo->mDataType;
                            
                            switch( dtlDataTypeGroup[ sDataType ] )
                            {
                                case DTL_GROUP_BINARY_INTEGER :
                                case DTL_GROUP_BINARY_REAL :
                                case DTL_GROUP_NUMBER :
                                case DTL_GROUP_INTERVAL_YEAR_TO_MONTH :     
                                case DTL_GROUP_INTERVAL_DAY_TO_SECOND :
                                    {
                                        sDataType = dtlGroupRepresentType[ dtlDataTypeGroup[ sDataType ] ];
                                        break;
                                    }
                                case DTL_GROUP_CHAR_STRING :
                                case DTL_GROUP_LONGVARCHAR_STRING :
                                    {
                                        sDataType = dtlGroupRepresentType[ DTL_GROUP_NUMBER ];
                                        break;
                                    }

                                case DTL_GROUP_BOOLEAN :
                                case DTL_GROUP_BINARY_STRING :
                                case DTL_GROUP_LONGVARBINARY_STRING :
                                case DTL_GROUP_DATE :
                                case DTL_GROUP_TIME :
                                case DTL_GROUP_TIMESTAMP :
                                case DTL_GROUP_TIME_WITH_TIME_ZONE :
                                case DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE :
                                case DTL_GROUP_ROWID :
                                    {
                                        STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                                        break;
                                    }
                                default :
                                    {
                                        STL_DASSERT( 0 );
                                        STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                                        break;
                                    }
                            }   

                            aDataTypeInfo->mDataType          = sDataType;
                            aDataTypeInfo->mArgNum1           = gResultDataTypeDef[ sDataType ].mArgNum1;
                            aDataTypeInfo->mArgNum2           = gResultDataTypeDef[ sDataType ].mArgNum2;
                            aDataTypeInfo->mStringLengthUnit  = gResultDataTypeDef[ sDataType ].mStringLengthUnit;
                            aDataTypeInfo->mIntervalIndicator = gResultDataTypeDef[ sDataType ].mIntervalIndicator;
                            
                            break;
                        }
                    case KNL_BUILTIN_AGGREGATION_COUNT :
                    case KNL_BUILTIN_AGGREGATION_COUNT_ASTERISK :
                        {
                            sDataType = DTL_TYPE_NATIVE_BIGINT;
                            aDataTypeInfo->mNullable = STL_FALSE;

                            aDataTypeInfo->mDataType          = sDataType;
                            aDataTypeInfo->mArgNum1           = gResultDataTypeDef[ sDataType ].mArgNum1;
                            aDataTypeInfo->mArgNum2           = gResultDataTypeDef[ sDataType ].mArgNum2;
                            aDataTypeInfo->mStringLengthUnit  = gResultDataTypeDef[ sDataType ].mStringLengthUnit;
                            aDataTypeInfo->mIntervalIndicator = gResultDataTypeDef[ sDataType ].mIntervalIndicator;
                            
                            break;
                        }
                    case KNL_BUILTIN_AGGREGATION_MIN :
                    case KNL_BUILTIN_AGGREGATION_MAX :
                        {
                            STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                                             aRegionMem,
                                                             sExpr->mExpr.mAggregation->mArgs[ 0 ],
                                                             aBindContext,
                                                             aDataTypeInfo,
                                                             aEnv )
                                     == STL_SUCCESS );
                            break;
                        }
                    default :
                        {
                            sDataType = DTL_TYPE_NA;
                            STL_DASSERT( 0 );
                            break;
                        }
                }

                aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                aDataTypeInfo->mUpdatable    = STL_FALSE;
                aDataTypeInfo->mIsIgnore     = STL_FALSE;

                break;
            }
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                stlBool                 sIsCompRule;
                stlUInt16               sArgCount = 0;
                dtlDataTypeDefInfo      sDataTypeDefInfo;
                dtlDataType           * sArgDataTypeArr;
                dtlDataTypeDefInfo    * sArgDataTypeDefInfoArr;

                /* Comparison Result Type Combination */
                sIsNullable = STL_FALSE;
                
                if( ( sExpr->mExpr.mCaseExpr->mFuncId == KNL_BUILTIN_FUNC_NVL ) ||
                    ( sExpr->mExpr.mCaseExpr->mFuncId == KNL_BUILTIN_FUNC_NVL2 ) ||
                    ( sExpr->mExpr.mCaseExpr->mFuncId == KNL_BUILTIN_FUNC_DECODE ) ||
                    ( sExpr->mExpr.mCaseExpr->mFuncId == KNL_BUILTIN_FUNC_CASE2 ) )
                {
                    sIsCompRule = STL_TRUE;

                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                STL_SIZEOF( dtlDataType ) * ( sExpr->mExpr.mCaseExpr->mCount + 1 ),
                                                (void**) &sArgDataTypeArr,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                STL_SIZEOF( dtlDataTypeDefInfo ) * ( sExpr->mExpr.mCaseExpr->mCount + 1 ),
                                                (void**) &sArgDataTypeDefInfoArr,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    sIsCompRule = STL_FALSE;
                }
                    
                /* Standard Result Type Combination */
                sTmpExpr = sExpr->mExpr.mCaseExpr->mDefResult;

                STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                                 aRegionMem,
                                                 sTmpExpr,
                                                 aBindContext,
                                                 & sTmpDataTypeInfo2,
                                                 aEnv )
                         == STL_SUCCESS );

                if( sTmpDataTypeInfo2.mIsIgnore == STL_FALSE )
                {
                    *aDataTypeInfo = sTmpDataTypeInfo2;

                    if( sIsCompRule == STL_TRUE )
                    {
                        sArgDataTypeArr[ sArgCount ] = aDataTypeInfo->mDataType;
                        
                        sArgDataTypeDefInfoArr[ sArgCount ].mArgNum1 =
                            aDataTypeInfo->mArgNum1;
                        sArgDataTypeDefInfoArr[ sArgCount ].mArgNum2 =
                            aDataTypeInfo->mArgNum2;
                        sArgDataTypeDefInfoArr[ sArgCount ].mStringLengthUnit =
                            aDataTypeInfo->mStringLengthUnit;
                        sArgDataTypeDefInfoArr[ sArgCount ].mIntervalIndicator =
                            aDataTypeInfo->mIntervalIndicator;
                        sArgDataTypeDefInfoArr[ sArgCount ].mDataValueSize = 0;
                        sArgDataTypeDefInfoArr[ sArgCount ].mStringBufferSize = 0;
                    }
                    sArgCount++;
                }
                else
                {
                    sIsNullable = STL_TRUE;
                }

                for( sLoop = 0 ; sLoop < sExpr->mExpr.mCaseExpr->mCount ; sLoop++ )
                {
                    sTmpExpr = sExpr->mExpr.mCaseExpr->mThenArr[ sLoop ];
                    
                    STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                                     aRegionMem,
                                                     sTmpExpr,
                                                     aBindContext,
                                                     & sTmpDataTypeInfo1,
                                                     aEnv )
                             == STL_SUCCESS );

                    if( sTmpDataTypeInfo1.mIsIgnore == STL_FALSE )
                    {
                        if( sIsCompRule == STL_TRUE )
                        {
                            sArgDataTypeArr[ sArgCount ] = sTmpDataTypeInfo1.mDataType;
                        
                            sArgDataTypeDefInfoArr[ sArgCount ].mArgNum1 =
                                sTmpDataTypeInfo1.mArgNum1;
                            sArgDataTypeDefInfoArr[ sArgCount ].mArgNum2 =
                                sTmpDataTypeInfo1.mArgNum2;
                            sArgDataTypeDefInfoArr[ sArgCount ].mStringLengthUnit =
                                sTmpDataTypeInfo1.mStringLengthUnit;
                            sArgDataTypeDefInfoArr[ sArgCount ].mIntervalIndicator =
                                sTmpDataTypeInfo1.mIntervalIndicator;
                            sArgDataTypeDefInfoArr[ sArgCount ].mDataValueSize = 0;
                            sArgDataTypeDefInfoArr[ sArgCount ].mStringBufferSize = 0;
                        }
                        else
                        {
                            if( sArgCount == 0 )
                            {
                                *aDataTypeInfo = sTmpDataTypeInfo1;
                            }
                            else
                            {
                                STL_TRY( qlvGetResultTypeCombiantion( aSQLString,
                                                                      aBindContext,
                                                                      sTmpExpr->mPosition,
                                                                      STL_FALSE,
                                                                      aDataTypeInfo,
                                                                      & sTmpDataTypeInfo1,
                                                                      aDataTypeInfo,
                                                                      aEnv )
                                         == STL_SUCCESS );
                            }
                        }
                        
                        sArgCount++;
                    }
                    else
                    {
                        sIsNullable = STL_TRUE;
                    }

                    if( sIsNullable == STL_TRUE )
                    {
                        aDataTypeInfo->mNullable = STL_TRUE;
                    }
                }

                aDataTypeInfo->mIsBaseColumn = STL_FALSE;

                if( sArgCount == 0 )
                {
                    aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                    aDataTypeInfo->mNullable     = STL_TRUE;
                    aDataTypeInfo->mUpdatable    = STL_FALSE;
                    aDataTypeInfo->mIsIgnore     = STL_TRUE;
                    
                    aDataTypeInfo->mDataType          = DTL_TYPE_VARCHAR;
                    aDataTypeInfo->mArgNum1           = 1;
                    aDataTypeInfo->mArgNum2           = gResultDataTypeDef[ DTL_TYPE_VARCHAR ].mArgNum2;
                    aDataTypeInfo->mStringLengthUnit  = gResultDataTypeDef[ DTL_TYPE_VARCHAR ].mStringLengthUnit;
                    aDataTypeInfo->mIntervalIndicator = gResultDataTypeDef[ DTL_TYPE_VARCHAR ].mIntervalIndicator;

                    break;
                }
                
                if( sIsCompRule == STL_TRUE )
                {
                    STL_TRY( dtlGetResultTypeForComparisonRule( sArgCount,
                                                                sArgDataTypeArr,
                                                                sArgDataTypeDefInfoArr,
                                                                & sDataType,
                                                                & sDataTypeDefInfo,
                                                                QLL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );

                    aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                    aDataTypeInfo->mUpdatable    = STL_FALSE;
                    aDataTypeInfo->mIsIgnore     = STL_FALSE;
                    
                    aDataTypeInfo->mDataType          = sDataType;
                    aDataTypeInfo->mArgNum1           = sDataTypeDefInfo.mArgNum1;
                    aDataTypeInfo->mArgNum2           = sDataTypeDefInfo.mArgNum2;
                    aDataTypeInfo->mStringLengthUnit  = sDataTypeDefInfo.mStringLengthUnit;
                    aDataTypeInfo->mIntervalIndicator = sDataTypeDefInfo.mIntervalIndicator;
                }
                
                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                aDataTypeInfo->mNullable     = qlvCheckNullable( NULL, sExpr );
                aDataTypeInfo->mUpdatable    = STL_FALSE;
                aDataTypeInfo->mIsIgnore     = STL_FALSE;
                    
                aDataTypeInfo->mDataType          = DTL_TYPE_BOOLEAN;
                aDataTypeInfo->mArgNum1           = gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1;
                aDataTypeInfo->mArgNum2           = gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2;
                aDataTypeInfo->mStringLengthUnit  = gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit;
                aDataTypeInfo->mIntervalIndicator = gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator;
                
                break;
            }
        case QLV_EXPR_TYPE_SUB_QUERY :
            {
                STL_DASSERT( sExpr->mExpr.mSubQuery->mTargetCount == 1 );
                *aDataTypeInfo = sExpr->mExpr.mSubQuery->mTargets[ 0 ].mDataTypeInfo;
                aDataTypeInfo->mIsBaseColumn = STL_FALSE;
                
                break;
            }
        /* case QLV_EXPR_TYPE_LIST_COLUMN : */
        /* case QLV_EXPR_TYPE_ROW_EXPR : */
        /* case QLV_EXPR_TYPE_PSEUDO_ARGUMENT : */
        default :
            STL_DASSERT( 0 );
            break;
    }

    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_CHAR_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CHARACTER_LENGTH_INVALID,
                      qlgMakeErrPosString( aSQLString,
                                           sExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_VARCHAR_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_VARCHAR_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CHARACTER_VARYING_LENGTH_INVALID,
                      qlgMakeErrPosString( aSQLString,
                                           sExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_VARCHAR_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_BINARY_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BINARY_LENGTH_INVALID,
                      qlgMakeErrPosString( aSQLString,
                                           sExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_VARBINARY_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_VARBINARY_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BINARY_VARYING_LENGTH_INVALID,
                      qlgMakeErrPosString( aSQLString,
                                           sExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_VARBINARY_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_NOT_APPLICABLE_INTERNAL_CAST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INCONSISTENT_DATATYPES,
                      qlgMakeErrPosString( aSQLString,
                                           sExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      gDataTypeDefinition[ sDataType ].mSqlNormalTypeName );
    }

    STL_CATCH( RAMP_ERR_NOT_APPLICABLE_CAST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INCONSISTENT_DATATYPES,
                      qlgMakeErrPosString( aSQLString,
                                           sExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      "NUMBER" );
    }

    STL_FINISH;
    
    if( aSQLString != NULL )
    {
        stlChar * sLastError = NULL;
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
        if ( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                          qlgMakeErrPosString( aSQLString,
                                                               sExpr->mPosition,
                                                               aEnv ) );
        }
    }

    return STL_FAILURE;
}



/**
 * @brief Expression의 Nullable 여부를 검사한다.
 * @param[in]  aTableNode    Init Table Node
 * @param[in]  aInitExpr     Init Expression
 * @return Nullable 여부 
 */
stlBool qlvCheckNullable( qlvInitNode        * aTableNode,
                          qlvInitExpression  * aInitExpr )
{
    qlvInitExpression     * sInitExpr = aInitExpr;
    qlvInitNode           * sTableNode = aTableNode;
    qlvInitInnerColumn    * sInitInnerCol;
    qlvInitFunction       * sInitFunc;
    qlvInitJoinTableNode  * sJoinTableNode;
    qlvNamedColumnItem    * sNamedColItem;
    qlvRefExprItem        * sRefExprItem;
    stlInt32                sLoop;

    
    STL_RAMP( RAMP_RETRY );
    
    STL_TRY_THROW( sTableNode != NULL, RAMP_CHECK_EXPR );

    if( sTableNode->mType == QLV_NODE_TYPE_JOIN_TABLE )
    {
        sJoinTableNode = (qlvInitJoinTableNode*) sTableNode;

        if( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_FULL_OUTER )
        {
            return STL_TRUE;
        }

        if( ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_NONE ) ||
            ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_CROSS ) )
        {
            STL_THROW( RAMP_CHECK_EXPR );
        }

        if( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_INNER )
        {
            /* Named Column List 를 검사 */
            if( sJoinTableNode->mJoinSpec == NULL )
            {
                STL_THROW( RAMP_CHECK_EXPR );
            }
                        
            if( sJoinTableNode->mJoinSpec->mNamedColumnList == NULL )
            {
                STL_THROW( RAMP_CHECK_EXPR );
            }

            sNamedColItem = sJoinTableNode->mJoinSpec->mNamedColumnList->mHead;

            while( sNamedColItem != NULL )
            {
                if( ( qlvIsSameExpression( sNamedColItem->mLeftExpr, sInitExpr ) == STL_TRUE ) ||
                    ( qlvIsSameExpression( sNamedColItem->mRightExpr, sInitExpr ) == STL_TRUE ) )
                {
                    if( ( qlvCheckNullable( NULL, sNamedColItem->mLeftExpr ) == STL_TRUE ) ||
                        ( qlvCheckNullable( NULL, sNamedColItem->mRightExpr ) == STL_TRUE ) )
                    {
                        return STL_TRUE;
                    }
                    else
                    {
                        return STL_FALSE;
                    }
                }

                sNamedColItem = sNamedColItem->mNext;
            }

            STL_THROW( RAMP_CHECK_EXPR );
        }


        if( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_LEFT_OUTER )
        {
            /* RefColumnList 에서 해당 Expression 찾기 */
            sRefExprItem = sJoinTableNode->mRefColumnList->mHead;
            while( sRefExprItem != NULL )
            {
                if( qlvIsSameExpression( sInitExpr, sRefExprItem->mExprPtr ) == STL_TRUE )
                {
                    if( sRefExprItem->mIsLeft == STL_TRUE )
                    {
                        STL_THROW( RAMP_CHECK_EXPR );
                    }
                    else
                    {
                        return STL_TRUE;
                    }
                }
                sRefExprItem = sRefExprItem->mNext;
            }
        }
                    
        if( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_RIGHT_OUTER )
        {
            /* RefColumnList 에서 해당 Expression 찾기 */
            sRefExprItem = sJoinTableNode->mRefColumnList->mHead;
            while( sRefExprItem != NULL )
            {
                if( qlvIsSameExpression( sInitExpr, sRefExprItem->mExprPtr ) == STL_TRUE )
                {
                    if( sRefExprItem->mIsLeft == STL_FALSE )
                    {
                        STL_THROW( RAMP_CHECK_EXPR );
                    }
                    else
                    {
                        return STL_TRUE;
                    }
                }
                sRefExprItem = sRefExprItem->mNext;
            }
        }
    }

    STL_RAMP( RAMP_CHECK_EXPR );

    STL_DASSERT( sInitExpr != NULL );

    switch( sInitExpr->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
            return ( sInitExpr->mExpr.mValue->mValueType == QLV_VALUE_TYPE_NULL );

        case QLV_EXPR_TYPE_BIND_PARAM :
            return STL_TRUE;

        case QLV_EXPR_TYPE_COLUMN :
            return ellGetColumnNullable( sInitExpr->mExpr.mColumn->mColumnHandle );

        case QLV_EXPR_TYPE_FUNCTION :
            sInitFunc = sInitExpr->mExpr.mFunction;
            
            switch( gBuiltInFuncInfoArr[ sInitFunc->mFuncId ].mReturnNullable )
            {
                case DTL_RETURN_NULLABLE_ALWAYS :
                    return STL_TRUE;

                case DTL_RETURN_NULLABLE_NEVER :
                    return STL_FALSE;

                case DTL_RETURN_NULLABLE_FROM_ANY_NULLABLE_INPUT :
                    for( sLoop = 0 ; sLoop < sInitFunc->mArgCount ; sLoop++ )
                    {
                        if( qlvCheckNullable( NULL, sInitFunc->mArgs[ sLoop ] ) == STL_TRUE )
                        {
                            return STL_TRUE;
                        }
                    }
                    return STL_FALSE;
                    
                case DTL_RETURN_NULLABLE_FROM_ALL_NULLABLE_INPUT :
                    for( sLoop = 0 ; sLoop < sInitFunc->mArgCount ; sLoop++ )
                    {
                        if( qlvCheckNullable( NULL, sInitFunc->mArgs[ sLoop ] ) == STL_FALSE )
                        {
                            return STL_FALSE;
                        }
                    }
                    return STL_TRUE;

                case DTL_RETURN_NULLABLE_CHECK :
                    /* Case Expression 에 의해 분기 함수들은 걸러짐 */
                    if( ( sInitFunc->mFuncId == KNL_BUILTIN_FUNC_LPAD ) ||
                        ( sInitFunc->mFuncId == KNL_BUILTIN_FUNC_RPAD ) )
                    {
                        if( qlvCheckNullable( NULL, sInitFunc->mArgs[ 0 ] ) == STL_TRUE )
                        {
                            return STL_TRUE;
                        }
                    }
                    else
                    {
                        STL_DASSERT( 0 );
                    }
                    return STL_FALSE;

                default :
                    STL_DASSERT( 0 );
                    break;
            }

        case QLV_EXPR_TYPE_CAST :
            sInitExpr = sInitExpr->mExpr.mTypeCast->mArgs[ 0 ];
            STL_THROW( RAMP_RETRY );
            
        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
            return STL_FALSE;
            
        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
            sInitExpr = sInitExpr->mExpr.mConstExpr->mOrgExpr;
            STL_THROW( RAMP_RETRY );

        case QLV_EXPR_TYPE_REFERENCE :
            sInitExpr = sInitExpr->mExpr.mReference->mOrgExpr;
            STL_THROW( RAMP_RETRY );

        case QLV_EXPR_TYPE_INNER_COLUMN :
            /* Join 고려 */
            sInitInnerCol = sInitExpr->mExpr.mInnerColumn;

            switch( sInitInnerCol->mRelationNode->mType )
            {
                case QLV_NODE_TYPE_QUERY_SPEC :
                    return ((qlvInitQuerySpecNode*) sInitInnerCol->mRelationNode)->
                        mTargets[ *sInitInnerCol->mIdx ].mDataTypeInfo.mNullable;

                case QLV_NODE_TYPE_QUERY_SET :
                    return ((qlvInitQuerySetNode*) sInitInnerCol->mRelationNode)->
                        mSetTargets[ *sInitInnerCol->mIdx ].mDataTypeInfo.mNullable;
                    
                case QLV_NODE_TYPE_SUB_TABLE :
                    return ((qlvInitSubTableNode*) sInitInnerCol->mRelationNode)->
                        mColumns[ *sInitInnerCol->mIdx ].mDataTypeInfo.mNullable;
                    
                case QLV_NODE_TYPE_JOIN_TABLE :
                    sJoinTableNode = ((qlvInitJoinTableNode*) sInitInnerCol->mRelationNode);

                    if( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_FULL_OUTER )
                    {
                        return STL_TRUE;
                    }

                    if( ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_NONE ) ||
                        ( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_CROSS ) )
                    {
                        sInitExpr = sInitInnerCol->mOrgExpr;
                        STL_THROW( RAMP_RETRY );
                    }

                    if( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_INNER )
                    {
                        /* Named Column List 를 검사 */
                        if( sJoinTableNode->mJoinSpec == NULL )
                        {
                            sInitExpr = sInitInnerCol->mOrgExpr;
                            STL_THROW( RAMP_RETRY );
                        }
                        
                        if( sJoinTableNode->mJoinSpec->mNamedColumnList == NULL )
                        {
                            sInitExpr = sInitInnerCol->mOrgExpr;
                            STL_THROW( RAMP_RETRY );
                        }

                        sNamedColItem = sJoinTableNode->mJoinSpec->mNamedColumnList->mHead;

                        while( sNamedColItem != NULL )
                        {
                            /* Named Column List 구성시 Expression Copy 를 하지 않았으므로
                             * pointer 비교를 한다. */
                            
                            if( ( qlvIsSameExpression( sNamedColItem->mLeftExpr, sInitExpr ) == STL_TRUE ) ||
                                ( qlvIsSameExpression( sNamedColItem->mRightExpr, sInitExpr ) == STL_TRUE ) )
                            {
                                if( ( qlvCheckNullable( NULL, sNamedColItem->mLeftExpr ) == STL_TRUE ) ||
                                    ( qlvCheckNullable( NULL, sNamedColItem->mRightExpr ) == STL_TRUE ) )
                                {
                                    return STL_TRUE;
                                }
                                else
                                {
                                    return STL_FALSE;
                                }
                            }

                            sNamedColItem = sNamedColItem->mNext;
                        }

                        sInitExpr = sInitInnerCol->mOrgExpr;
                        STL_THROW( RAMP_RETRY );
                    }


                    if( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_LEFT_OUTER )
                    {
                        /* RefColumnList 에서 해당 Expression 찾기 */
                        sRefExprItem = sJoinTableNode->mRefColumnList->mHead;
                        while( sRefExprItem != NULL )
                        {
                            if( qlvIsSameExpression( sInitExpr, sRefExprItem->mExprPtr ) == STL_TRUE )
                            {
                                if( sRefExprItem->mIsLeft == STL_TRUE )
                                {
                                    sInitExpr = sInitInnerCol->mOrgExpr;
                                    STL_THROW( RAMP_RETRY );
                                }
                                else
                                {
                                    return STL_TRUE;
                                }
                            }
                            sRefExprItem = sRefExprItem->mNext;
                        }
                    }
                    
                    if( sJoinTableNode->mJoinType == QLV_JOIN_TYPE_RIGHT_OUTER )
                    {
                        /* RefColumnList 에서 해당 Expression 찾기 */
                        sRefExprItem = sJoinTableNode->mRefColumnList->mHead;
                        while( sRefExprItem != NULL )
                        {
                            if( qlvIsSameExpression( sInitExpr, sRefExprItem->mExprPtr ) == STL_TRUE )
                            {
                                if( sRefExprItem->mIsLeft == STL_FALSE )
                                {
                                    sInitExpr = sInitInnerCol->mOrgExpr;
                                    STL_THROW( RAMP_RETRY );
                                }
                                else
                                {
                                    return STL_TRUE;
                                }
                            }
                            sRefExprItem = sRefExprItem->mNext;
                        }
                    }
                    
                    /* QLV_JOIN_TYPE_LEFT_SEMI */
                    /* QLV_JOIN_TYPE_RIGHT_SEMI */
                    /* QLV_JOIN_TYPE_LEFT_ANTI_SEMI */
                    /* QLV_JOIN_TYPE_RIGHT_ANTI_SEMI */
                    /* QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA */
                    /* QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA */
                    /* QLV_JOIN_TYPE_INVERTED_LEFT_SEMI */
                    /* QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI */
                    sInitExpr = sInitInnerCol->mOrgExpr;
                    STL_THROW( RAMP_RETRY );
                            
                case QLV_NODE_TYPE_BASE_TABLE :
                    STL_DASSERT( 0 );
                    return STL_TRUE;
                    
                default :
                    /* Instant Table */
                    sInitExpr = sInitInnerCol->mOrgExpr;
                    STL_THROW( RAMP_RETRY );
            }
            break;
            
        case QLV_EXPR_TYPE_OUTER_COLUMN :
            sInitExpr = sInitExpr->mExpr.mOuterColumn->mOrgExpr;
            STL_THROW( RAMP_RETRY );

        case QLV_EXPR_TYPE_ROWID_COLUMN :
            return STL_FALSE;
            
        case QLV_EXPR_TYPE_AGGREGATION :
            switch( sInitExpr->mExpr.mAggregation->mAggrId )
            {
                case KNL_BUILTIN_AGGREGATION_COUNT :
                case KNL_BUILTIN_AGGREGATION_COUNT_ASTERISK :
                    {
                        return STL_FALSE;
                    }
                case KNL_BUILTIN_AGGREGATION_SUM :
                case KNL_BUILTIN_AGGREGATION_MIN :
                case KNL_BUILTIN_AGGREGATION_MAX :
                    {
                        sInitExpr = sInitExpr->mExpr.mAggregation->mArgs[ 0 ];
                        STL_THROW( RAMP_RETRY );
                    }
                default :
                    {
                        STL_DASSERT( 0 );
                        break;
                    }
            }
            
        case QLV_EXPR_TYPE_CASE_EXPR :
            if( qlvCheckNullable( NULL, sInitExpr->mExpr.mCaseExpr->mDefResult ) == STL_TRUE )
            {
                return STL_TRUE;
            }
            
            for( sLoop = 0 ; sLoop < sInitExpr->mExpr.mCaseExpr->mCount ; sLoop++ )
            {
                if( qlvCheckNullable( NULL, sInitExpr->mExpr.mCaseExpr->mThenArr[ sLoop ] ) == STL_TRUE )
                {
                    return STL_TRUE;
                }
            }
            return STL_FALSE;
            
        case QLV_EXPR_TYPE_LIST_FUNCTION :
            if( gBuiltInFuncInfoArr[ sInitExpr->mExpr.mListFunc->mFuncId ].mReturnNullable ==
                DTL_RETURN_NULLABLE_NEVER )
            {
                return STL_FALSE;
            }
            
            for( sLoop = 0 ; sLoop < sInitExpr->mExpr.mListFunc->mArgCount ; sLoop++ )
            {
                if( qlvCheckNullable( NULL, sInitExpr->mExpr.mListFunc->mArgs[ sLoop ] ) == STL_TRUE )
                {
                    return STL_TRUE;
                }
            }
            return STL_FALSE;
            
        case QLV_EXPR_TYPE_LIST_COLUMN :
            for( sLoop = 0 ; sLoop < sInitExpr->mExpr.mListCol->mArgCount ; sLoop++ )
            {
                if( qlvCheckNullable( NULL, sInitExpr->mExpr.mListCol->mArgs[ sLoop ] ) == STL_TRUE )
                {
                    return STL_TRUE;
                }
            }
            return STL_FALSE;
            
        case QLV_EXPR_TYPE_ROW_EXPR :
            for( sLoop = 0 ; sLoop < sInitExpr->mExpr.mRowExpr->mArgCount ; sLoop++ )
            {
                if( qlvCheckNullable( NULL, sInitExpr->mExpr.mRowExpr->mArgs[ sLoop ] ) == STL_TRUE )
                {
                    return STL_TRUE;
                }
            }
            return STL_FALSE;

        case QLV_EXPR_TYPE_SUB_QUERY :
            return STL_TRUE;
            
        /* case QLV_EXPR_TYPE_PSEUDO_ARGUMENT : */
        default :
            STL_DASSERT( 0 );
            break;
    }
    
    return STL_TRUE;
}


/**
 * @brief 두 Data Type 간의 Result Type Combination을 수행한다.
 * @param[in]  aSQLString           SQL String
 * @param[in]  aBindContext         Bind Context 
 * @param[in]  aExprPos             Expression Position 
 * @param[in]  aUseCompRule         Comparison 을 위한 Result Type Combination Map 사용 여부
 * @param[in]  aLeftDataTypeInfo    Left Data Type Info
 * @param[in]  aRightDataTypeInfo   Right Data Type Info
 * @param[out] aResDataTypeInfo     Result Data Type Info
 * @param[in]  aEnv                 Environment
 */
stlStatus qlvGetResultTypeCombiantion( stlChar              * aSQLString,
                                       knlBindContext       * aBindContext,
                                       stlInt32               aExprPos,
                                       stlBool                aUseCompRule,
                                       qlvInitDataTypeInfo  * aLeftDataTypeInfo,
                                       qlvInitDataTypeInfo  * aRightDataTypeInfo,
                                       qlvInitDataTypeInfo  * aResDataTypeInfo,
                                       qllEnv               * aEnv )
{
    dtlDataType   sDataType     = DTL_TYPE_NA;
    stlInt64      sLeftArgNum1  = 0;           
    stlInt64      sLeftArgNum2  = 0;
    stlInt64      sRightArgNum1 = 0;           
    stlInt64      sRightArgNum2 = 0;
    stlInt64      sScaleDiff    = 0;

    stlInt64              sArgNum1;
    stlInt64              sArgNum2;
    dtlStringLengthUnit   sStringLengthUnit;
    dtlIntervalIndicator  sIntervalIndicator;


    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLeftDataTypeInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRightDataTypeInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResDataTypeInfo != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Data Type 외 정보 설정
     */

    aResDataTypeInfo->mIsBaseColumn = STL_FALSE;
    aResDataTypeInfo->mNullable     = ( aLeftDataTypeInfo->mNullable || aRightDataTypeInfo->mNullable );
    aResDataTypeInfo->mUpdatable    = STL_FALSE;
    aResDataTypeInfo->mIsIgnore     = STL_FALSE;
    

    /**
     * Data Type 설정
     */

    if( aUseCompRule == STL_TRUE )
    {
        sDataType = dtlComparisonResultTypeCombination[ aLeftDataTypeInfo->mDataType ][ aRightDataTypeInfo->mDataType ];
    }
    else
    {
        sDataType = dtlResultTypeCombination[ aLeftDataTypeInfo->mDataType ][ aRightDataTypeInfo->mDataType ];
    }
        
    STL_TRY_THROW( sDataType != DTL_TYPE_NA, RAMP_ERR_NOT_COMPATIBLE_TYPE );

    sArgNum1           = DTL_PRECISION_NA;
    sArgNum2           = DTL_SCALE_NA;
    sStringLengthUnit  = DTL_STRING_LENGTH_UNIT_NA;
    sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    
    /**
     * Data Type 부가 정보 설정
     */
    
    switch( sDataType )
    {
        case DTL_TYPE_BOOLEAN:
        case DTL_TYPE_NATIVE_SMALLINT:
        case DTL_TYPE_NATIVE_INTEGER:
        case DTL_TYPE_NATIVE_BIGINT:
        case DTL_TYPE_NATIVE_REAL:
        case DTL_TYPE_NATIVE_DOUBLE:
        case DTL_TYPE_LONGVARCHAR:
        case DTL_TYPE_LONGVARBINARY:
        case DTL_TYPE_DATE:
        case DTL_TYPE_ROWID:
            sArgNum1           = gResultDataTypeDef[ sDataType ].mArgNum1;
            sArgNum2           = gResultDataTypeDef[ sDataType ].mArgNum2;
            sStringLengthUnit  = gResultDataTypeDef[ sDataType ].mStringLengthUnit;
            sIntervalIndicator = gResultDataTypeDef[ sDataType ].mIntervalIndicator;
            break;
            
        case DTL_TYPE_FLOAT:
            switch( aLeftDataTypeInfo->mDataType )
            {
                case DTL_TYPE_NATIVE_SMALLINT:
                    sLeftArgNum1 = DTL_NATIVE_SMALLINT_DEFAULT_PRECISION;
                    break;
                case DTL_TYPE_NATIVE_INTEGER:
                    sLeftArgNum1 = DTL_NATIVE_INTEGER_DEFAULT_PRECISION;
                    break;
                case DTL_TYPE_NATIVE_BIGINT:
                    sLeftArgNum1 = DTL_NATIVE_BIGINT_DEFAULT_PRECISION;
                    break;
                case DTL_TYPE_FLOAT:
                    sLeftArgNum1 = aLeftDataTypeInfo->mArgNum1;
                    break;
                case DTL_TYPE_NUMBER:
                    sLeftArgNum1 = dtlDecimalToBinaryPrecision[ aLeftDataTypeInfo->mArgNum1 ];
                    break;
                default:
                    sLeftArgNum1 = gResultDataTypeDef[ sDataType ].mArgNum1;
                    break;
            }

            switch( aRightDataTypeInfo->mDataType )
            {
                case DTL_TYPE_NATIVE_SMALLINT:
                    sRightArgNum1 = DTL_NATIVE_SMALLINT_DEFAULT_PRECISION;
                    break;
                case DTL_TYPE_NATIVE_INTEGER:
                    sRightArgNum1 = DTL_NATIVE_INTEGER_DEFAULT_PRECISION;
                    break;
                case DTL_TYPE_NATIVE_BIGINT:
                    sRightArgNum1 = DTL_NATIVE_BIGINT_DEFAULT_PRECISION;
                    break;
                case DTL_TYPE_FLOAT:
                    sRightArgNum1 = aRightDataTypeInfo->mArgNum1;
                    break;
                case DTL_TYPE_NUMBER:
                    sRightArgNum1 = dtlDecimalToBinaryPrecision[ aRightDataTypeInfo->mArgNum1 ];
                    break;
                default:
                    sRightArgNum1 = gResultDataTypeDef[ sDataType ].mArgNum1;
                    break;
            }

            sArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );
            sArgNum2 = DTL_SCALE_NA;
            
            break;

        case DTL_TYPE_NUMBER:
            switch( aLeftDataTypeInfo->mDataType )
            {
                case DTL_TYPE_NATIVE_SMALLINT:
                    sLeftArgNum1 = DTL_DECIMAL_SMALLINT_DEFAULT_PRECISION;
                    sLeftArgNum2 = 0;
                    break;
                case DTL_TYPE_NATIVE_INTEGER:
                    sLeftArgNum1 = DTL_DECIMAL_INTEGER_DEFAULT_PRECISION;
                    sLeftArgNum2 = 0;
                    break;
                case DTL_TYPE_NATIVE_BIGINT:
                    sLeftArgNum1 = DTL_DECIMAL_BIGINT_DEFAULT_PRECISION;
                    sLeftArgNum2 = 0;
                    break;
                case DTL_TYPE_FLOAT:
                    sLeftArgNum1 = dtlBinaryToDecimalPrecision[ aLeftDataTypeInfo->mArgNum1 ];
                    sLeftArgNum2 = DTL_SCALE_NA;
                    break;
                case DTL_TYPE_NUMBER:
                    sLeftArgNum1 = aLeftDataTypeInfo->mArgNum1;
                    sLeftArgNum2 = aLeftDataTypeInfo->mArgNum2;
                    break;
                default:
                    sLeftArgNum1 = gResultDataTypeDef[ sDataType ].mArgNum1;
                    sLeftArgNum2 = gResultDataTypeDef[ sDataType ].mArgNum2;
                    break;
            }

            switch( aRightDataTypeInfo->mDataType )
            {
                case DTL_TYPE_NATIVE_SMALLINT:
                    sRightArgNum1 = DTL_DECIMAL_SMALLINT_DEFAULT_PRECISION;
                    sRightArgNum2 = 0;
                    break;
                case DTL_TYPE_NATIVE_INTEGER:
                    sRightArgNum1 = DTL_DECIMAL_INTEGER_DEFAULT_PRECISION;
                    sRightArgNum2 = 0;
                    break;
                case DTL_TYPE_NATIVE_BIGINT:
                    sRightArgNum1 = DTL_DECIMAL_BIGINT_DEFAULT_PRECISION;
                    sRightArgNum2 = 0;
                    break;
                case DTL_TYPE_FLOAT:
                    sRightArgNum1 = dtlBinaryToDecimalPrecision[ aRightDataTypeInfo->mArgNum1 ];
                    sRightArgNum2 = DTL_SCALE_NA;
                    break;
                case DTL_TYPE_NUMBER:
                    sRightArgNum1 = aRightDataTypeInfo->mArgNum1;
                    sRightArgNum2 = aRightDataTypeInfo->mArgNum2;
                    break;
                default:
                    sRightArgNum1 = gResultDataTypeDef[ sDataType ].mArgNum1;
                    sRightArgNum2 = gResultDataTypeDef[ sDataType ].mArgNum2;
                    break;
            }

            if( ( sLeftArgNum2 == DTL_SCALE_NA ) ||
                ( sRightArgNum2 == DTL_SCALE_NA ) )
            {
                sArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );
                sArgNum2 = DTL_SCALE_NA;
            }
            else
            {
                /**
                 * 두 Type 을 모두 수용할 수 있는 Precision, Scale 을 결정한다.
                 */
                    
                sArgNum2 = STL_MAX( sLeftArgNum2, sRightArgNum2 );
                    
                sScaleDiff = stlAbsInt64( sRightArgNum2 - sLeftArgNum2 );

                if( sRightArgNum2 > sLeftArgNum2 )
                {
                    sRightArgNum1 -= sScaleDiff;
                }
                else
                {
                    sLeftArgNum1 -= sScaleDiff;
                }

                sArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 ) + sScaleDiff;

                if( sArgNum1 > DTL_NUMERIC_MAX_PRECISION )
                {
                    sArgNum1 = DTL_NUMERIC_MAX_PRECISION;
                    sArgNum2 = DTL_SCALE_NA;
                }
                else
                {
                    /* nothing to do */
                }
            }   
            break;
            
        case DTL_TYPE_CHAR:
            sArgNum1 = STL_MAX( aLeftDataTypeInfo->mArgNum1, aRightDataTypeInfo->mArgNum1 );

            if( ( aLeftDataTypeInfo->mArgNum1 == aRightDataTypeInfo->mArgNum1 ) &&
                ( aLeftDataTypeInfo->mStringLengthUnit == aRightDataTypeInfo->mStringLengthUnit ) )
            {
                sDataType = DTL_TYPE_CHAR;
            }
            else
            {
                sDataType = DTL_TYPE_VARCHAR;
            }

            if( ( aLeftDataTypeInfo->mStringLengthUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS ) ||
                ( aRightDataTypeInfo->mStringLengthUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS ) )
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_CHARACTERS;
            }
            else
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
            }
            break;
            
        case DTL_TYPE_VARCHAR:
            sArgNum1 = STL_MAX( aLeftDataTypeInfo->mArgNum1, aRightDataTypeInfo->mArgNum1 );

            if( ( aLeftDataTypeInfo->mStringLengthUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS ) ||
                ( aRightDataTypeInfo->mStringLengthUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS ) )
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_CHARACTERS;
            }
            else
            {
                sStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
            }
            break;
            
        case DTL_TYPE_BINARY:
            sArgNum1 = STL_MAX( aLeftDataTypeInfo->mArgNum1, aRightDataTypeInfo->mArgNum1 );
            
            if( aLeftDataTypeInfo->mArgNum1 == aRightDataTypeInfo->mArgNum1 )
            {
                sDataType = DTL_TYPE_BINARY;
            }
            else
            {
                sDataType = DTL_TYPE_VARBINARY;
            }
            break;
            
        case DTL_TYPE_VARBINARY:
            sArgNum1 = STL_MAX( aLeftDataTypeInfo->mArgNum1, aRightDataTypeInfo->mArgNum1 );
            break;
            
        case DTL_TYPE_TIME:
        case DTL_TYPE_TIMESTAMP:
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            sArgNum1 = STL_MAX( aLeftDataTypeInfo->mArgNum1, aRightDataTypeInfo->mArgNum1 );
            break;
            
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
            if( aLeftDataTypeInfo->mDataType != DTL_TYPE_INTERVAL_YEAR_TO_MONTH )
            {
                STL_DASSERT( aRightDataTypeInfo->mDataType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH );

                switch( aRightDataTypeInfo->mIntervalIndicator )
                {
                    case DTL_INTERVAL_INDICATOR_YEAR :
                    case DTL_INTERVAL_INDICATOR_MONTH :
                        break;
                    case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH :
                        STL_THROW( RAMP_ERR_NOT_COMPATIBLE_TYPE );
                        break;
                    default :
                        STL_DASSERT( 0 );
                        break;
                }

                switch( aLeftDataTypeInfo->mDataType )
                {
                    case DTL_TYPE_NATIVE_SMALLINT :
                    case DTL_TYPE_NATIVE_INTEGER :
                    case DTL_TYPE_NATIVE_BIGINT :
                    case DTL_TYPE_FLOAT :
                    case DTL_TYPE_NUMBER :
                    case DTL_TYPE_CHAR :
                    case DTL_TYPE_VARCHAR :
                    case DTL_TYPE_LONGVARCHAR :
                        sArgNum1 = aRightDataTypeInfo->mArgNum1;
                        sIntervalIndicator = aRightDataTypeInfo->mIntervalIndicator;
                        break;
                    default :
                        STL_THROW( RAMP_ERR_NOT_COMPATIBLE_TYPE );
                        break;
                }
                break;
            }
            else if( aRightDataTypeInfo->mDataType != DTL_TYPE_INTERVAL_YEAR_TO_MONTH )
            {
                STL_DASSERT( aLeftDataTypeInfo->mDataType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH );

                switch( aLeftDataTypeInfo->mIntervalIndicator )
                {
                    case DTL_INTERVAL_INDICATOR_YEAR :
                    case DTL_INTERVAL_INDICATOR_MONTH :
                        break;
                    case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH :
                        STL_THROW( RAMP_ERR_NOT_COMPATIBLE_TYPE );
                        break;
                    default :
                        STL_DASSERT( 0 );
                        break;
                }

                switch( aRightDataTypeInfo->mDataType )
                {
                    case DTL_TYPE_NATIVE_SMALLINT :
                    case DTL_TYPE_NATIVE_INTEGER :
                    case DTL_TYPE_NATIVE_BIGINT :
                    case DTL_TYPE_FLOAT :
                    case DTL_TYPE_NUMBER :
                    case DTL_TYPE_CHAR :
                    case DTL_TYPE_VARCHAR :
                    case DTL_TYPE_LONGVARCHAR :
                        sArgNum1 = aLeftDataTypeInfo->mArgNum1;
                        sIntervalIndicator = aLeftDataTypeInfo->mIntervalIndicator;
                        break;
                    default :
                        STL_THROW( RAMP_ERR_NOT_COMPATIBLE_TYPE );
                        break;
                }
                break;
            }

            sIntervalIndicator =
                dtlIntervalIndicatorContainArgIndicator
                [ aLeftDataTypeInfo->mIntervalIndicator ][ aRightDataTypeInfo->mIntervalIndicator ];

            switch( sIntervalIndicator )
            {
                case DTL_INTERVAL_INDICATOR_YEAR:
                case DTL_INTERVAL_INDICATOR_MONTH:
                    sArgNum1 = STL_MAX( aLeftDataTypeInfo->mArgNum1, aRightDataTypeInfo->mArgNum1 );
                    break;
                case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
                    {
                        switch( aLeftDataTypeInfo->mIntervalIndicator )
                        {
                            case DTL_INTERVAL_INDICATOR_YEAR:
                            case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
                                sLeftArgNum1 = aLeftDataTypeInfo->mArgNum1;
                                break;
                            default:
                                sLeftArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                                break;
                        }

                        switch( aRightDataTypeInfo->mIntervalIndicator )
                        {
                            case DTL_INTERVAL_INDICATOR_YEAR:
                            case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
                                sRightArgNum1 = aRightDataTypeInfo->mArgNum1;
                                break;
                            default:
                                sRightArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                                break;
                        }
                            
                        sArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );
                        break;
                    }
                default:
                    STL_DASSERT(0);
                    break;
            }
            break;

        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            if( aLeftDataTypeInfo->mDataType != DTL_TYPE_INTERVAL_DAY_TO_SECOND )
            {
                STL_DASSERT( aRightDataTypeInfo->mDataType == DTL_TYPE_INTERVAL_DAY_TO_SECOND );

                switch( aRightDataTypeInfo->mIntervalIndicator )
                {
                    case DTL_INTERVAL_INDICATOR_DAY :
                    case DTL_INTERVAL_INDICATOR_HOUR :
                    case DTL_INTERVAL_INDICATOR_MINUTE :
                    case DTL_INTERVAL_INDICATOR_SECOND :
                        break;
                    case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR :
                    case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE :
                    case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND :
                    case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE :
                    case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND :
                    case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND :
                        STL_THROW( RAMP_ERR_NOT_COMPATIBLE_TYPE );
                        break;
                    default :
                        STL_DASSERT( 0 );
                        break;
                }

                switch( aLeftDataTypeInfo->mDataType )
                {
                    case DTL_TYPE_NATIVE_SMALLINT :
                    case DTL_TYPE_NATIVE_INTEGER :
                    case DTL_TYPE_NATIVE_BIGINT :
                    case DTL_TYPE_FLOAT :
                    case DTL_TYPE_NUMBER :
                    case DTL_TYPE_CHAR :
                    case DTL_TYPE_VARCHAR :
                    case DTL_TYPE_LONGVARCHAR :
                        sArgNum1 = aRightDataTypeInfo->mArgNum1;
                        sArgNum2 = aRightDataTypeInfo->mArgNum2;
                        sIntervalIndicator = aRightDataTypeInfo->mIntervalIndicator;
                        break;
                    default :
                        STL_THROW( RAMP_ERR_NOT_COMPATIBLE_TYPE );
                        break;
                }
                break;
            }
            else if( aRightDataTypeInfo->mDataType != DTL_TYPE_INTERVAL_DAY_TO_SECOND )
            {
                STL_DASSERT( aLeftDataTypeInfo->mDataType == DTL_TYPE_INTERVAL_DAY_TO_SECOND );

                switch( aLeftDataTypeInfo->mIntervalIndicator )
                {
                    case DTL_INTERVAL_INDICATOR_DAY :
                    case DTL_INTERVAL_INDICATOR_HOUR :
                    case DTL_INTERVAL_INDICATOR_MINUTE :
                    case DTL_INTERVAL_INDICATOR_SECOND :
                        break;
                    case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR :
                    case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE :
                    case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND :
                    case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE :
                    case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND :
                    case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND :
                        STL_THROW( RAMP_ERR_NOT_COMPATIBLE_TYPE );
                        break;
                    default :
                        STL_DASSERT( 0 );
                        break;
                }

                switch( aRightDataTypeInfo->mDataType )
                {
                    case DTL_TYPE_NATIVE_SMALLINT :
                    case DTL_TYPE_NATIVE_INTEGER :
                    case DTL_TYPE_NATIVE_BIGINT :
                    case DTL_TYPE_FLOAT :
                    case DTL_TYPE_NUMBER :
                    case DTL_TYPE_CHAR :
                    case DTL_TYPE_VARCHAR :
                    case DTL_TYPE_LONGVARCHAR :
                        sArgNum1 = aLeftDataTypeInfo->mArgNum1;
                        sArgNum2 = aLeftDataTypeInfo->mArgNum2;
                        sIntervalIndicator = aLeftDataTypeInfo->mIntervalIndicator;
                        break;
                    default :
                        STL_THROW( RAMP_ERR_NOT_COMPATIBLE_TYPE );
                        break;
                }
                break;
            }

            sIntervalIndicator =
                dtlIntervalIndicatorContainArgIndicator
                [aLeftDataTypeInfo->mIntervalIndicator][aRightDataTypeInfo->mIntervalIndicator];
                
            switch( sIntervalIndicator )
            {
                case DTL_INTERVAL_INDICATOR_DAY:
                case DTL_INTERVAL_INDICATOR_HOUR:
                case DTL_INTERVAL_INDICATOR_MINUTE:
                    sArgNum1 = STL_MAX( aLeftDataTypeInfo->mArgNum1, aRightDataTypeInfo->mArgNum1 );
                    break;
                case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR:
                case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE:
                    {
                        if( (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                            (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                            (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) )
                        {
                            sLeftArgNum1 = aLeftDataTypeInfo->mArgNum1;
                        }
                        else
                        {
                            sLeftArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                        }

                        if( (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                            (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                            (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) )
                        {
                            sRightArgNum1 = aRightDataTypeInfo->mArgNum1;
                        }
                        else
                        {
                            sRightArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                        }
                            
                        sArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );
                        break;
                    }
                case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:
                    {
                        if( (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                            (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) )
                        {
                            sLeftArgNum1 = aLeftDataTypeInfo->mArgNum1;
                        }
                        else
                        {
                            sLeftArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                        }

                        if( (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                            (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) )
                        {
                            sRightArgNum1 = aRightDataTypeInfo->mArgNum1;
                        }
                        else
                        {
                            sRightArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                        }
                            
                        sArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );
                        break;
                    }
                case DTL_INTERVAL_INDICATOR_SECOND:
                    sArgNum1 = STL_MAX( aLeftDataTypeInfo->mArgNum1, aRightDataTypeInfo->mArgNum1 );
                    sArgNum2 = STL_MAX( aLeftDataTypeInfo->mArgNum2, aRightDataTypeInfo->mArgNum2 );
                    break;
                case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND:
                    {
                        if( (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                            (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                            (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                            (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) )
                        {
                            sLeftArgNum1 = aLeftDataTypeInfo->mArgNum1;
                        }
                        else
                        {
                            sLeftArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                        }

                        if( (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                            (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                            (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                            (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) )
                        {
                            sRightArgNum1 = aRightDataTypeInfo->mArgNum1;
                        }
                        else
                        {
                            sRightArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                        }
                            
                        sArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );

                        if( (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                            (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                            (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                            (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                        {
                            sLeftArgNum2 = aLeftDataTypeInfo->mArgNum2;
                        }
                        else
                        {
                            sLeftArgNum2 = 0;
                        }

                        if( (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                            (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                            (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                            (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                        {
                            sRightArgNum2 = aRightDataTypeInfo->mArgNum2;
                        }
                        else
                        {
                            sRightArgNum2 = 0;
                        }

                        sArgNum2 = STL_MAX( sLeftArgNum2, sRightArgNum2 );
                            
                        break;
                    }
                case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:
                    {
                        if( (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                            (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                            (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) )
                        {
                            sLeftArgNum1 = aLeftDataTypeInfo->mArgNum1;
                        }
                        else
                        {
                            sLeftArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                        }

                        if( (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                            (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                            (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) )
                        {
                            sRightArgNum1 = aRightDataTypeInfo->mArgNum1;
                        }
                        else
                        {
                            sRightArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                        }
                            
                        sArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );

                        if( (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                            (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                            (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                        {
                            sLeftArgNum2 = aLeftDataTypeInfo->mArgNum2;
                        }
                        else
                        {
                            sLeftArgNum2 = 0;
                        }

                        if( (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                            (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                            (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                        {
                            sRightArgNum2 = aRightDataTypeInfo->mArgNum2;
                        }
                        else
                        {
                            sRightArgNum2 = 0;
                        }

                        sArgNum2 = STL_MAX( sLeftArgNum2, sRightArgNum2 );
                            
                        break;
                    }
                case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
                    {
                        if( (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                            (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                        {
                            sLeftArgNum1 = aLeftDataTypeInfo->mArgNum1;
                        }
                        else
                        {
                            sLeftArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                        }

                        if( (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                            (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                        {
                            sRightArgNum1 = aRightDataTypeInfo->mArgNum1;
                        }
                        else
                        {
                            sRightArgNum1 = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
                        }
                            
                        sArgNum1 = STL_MAX( sLeftArgNum1, sRightArgNum1 );

                        if( (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                            (aLeftDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                        {
                            sLeftArgNum2 = aLeftDataTypeInfo->mArgNum2;
                        }
                        else
                        {
                            sLeftArgNum2 = 0;
                        }

                        if( (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                            (aRightDataTypeInfo->mIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                        {
                            sRightArgNum2 = aRightDataTypeInfo->mArgNum2;
                        }
                        else
                        {
                            sRightArgNum2 = 0;
                        }

                        sArgNum2 = STL_MAX( sLeftArgNum2, sRightArgNum2 );
                            
                        break;
                    }
                default:
                    STL_DASSERT(0);
                    break;
            }
            break;
                
        /* case DTL_TYPE_DECIMAL: */
        /* case DTL_TYPE_CLOB: */
        /* case DTL_TYPE_BLOB: */
        default:
            STL_DASSERT(0);
            break;
            
    }


    /**
     * OUTPUT 설정
     */
    
    aResDataTypeInfo->mDataType          = sDataType;
    aResDataTypeInfo->mArgNum1           = sArgNum1;
    aResDataTypeInfo->mArgNum2           = sArgNum2;
    aResDataTypeInfo->mStringLengthUnit  = (stlUInt8) sStringLengthUnit;
    aResDataTypeInfo->mIntervalIndicator = (stlUInt8) sIntervalIndicator;

    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_NOT_COMPATIBLE_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TYPE_MISMATCH,
                      qlgMakeErrPosString( aSQLString,
                                           aExprPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      dtlDataTypeName[ aLeftDataTypeInfo->mDataType ],
                      dtlDataTypeName[ aRightDataTypeInfo->mDataType ] );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} qlnv */
