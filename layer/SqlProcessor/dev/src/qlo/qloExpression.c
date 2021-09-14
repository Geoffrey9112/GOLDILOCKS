/*******************************************************************************
 * qloExpression.c
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
 * @file qloExpression.c
 * @brief SQL Processor Layer EXPRESSION optimization
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qlo
 * @{
 */


/*******************************************************************************
 * Expression : Code Optimizaion
 ******************************************************************************/


/**
 * @brief Exression Stack Entry를 구성한다.
 * @param[in]     aSQLString       SQL String
 * @param[in]     aInitExprCode    Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext     Bind Context 
 * @param[in,out] aExeCodeExpr     Optimization 적용한 Code Expression
 * @param[in,out] aConstExpr       Optimization 적용한 Constant Expression
 * @param[in,out] aExprInfo        All Expression's Information
 * @param[out]    aExprEntry       Expresson Stack의 Root-Entry
 * @param[in]     aRegionMem       Region Memory
 * @param[in]     aEnv             Environment
 *
 * @remark Expression에 대한 Optimize 초기 수행시 (본 함수 처음 호출시)
 *    <BR> aGroupEntryNo의 값을 "QLL_INIT_GROUP_ENTRY_NO" 로 주어야 한다.
 */
stlStatus qloCodeOptimizeExpression( stlChar             * aSQLString,
                                     qlvInitExpression   * aInitExprCode,
                                     knlBindContext      * aBindContext,
                                     knlExprStack        * aExeCodeExpr,
                                     knlExprStack        * aConstExpr,
                                     qloExprInfo         * aExprInfo,
                                     knlExprEntry       ** aExprEntry,
                                     knlRegionMem        * aRegionMem,
                                     qllEnv              * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExeCodeExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Expression Code Stack Entry 구성 : Bottom-Up 방식으로 Data Type이 설정됨.
     */

    STL_TRY( qloAddExprStackEntry( aSQLString,
                                   aInitExprCode,
                                   aBindContext,
                                   aExeCodeExpr,
                                   aConstExpr,
                                   QLL_INIT_GROUP_ENTRY_NO,
                                   KNL_BUILTIN_FUNC_INVALID,
                                   aExprInfo,
                                   aExprEntry,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /**
     * expression stack composition 설정
     */

    STL_TRY( knlAnalyzeExprStack( aExeCodeExpr, KNL_ENV(aEnv) ) == STL_SUCCESS );


    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief COLUMN 의 Code Area를 구성한다.
 * @param[in]     aSQLString          SQL String
 * @param[in]     aInitColumnCode     Parse/Validation 단계의 Code Expression
 * @param[in,out] aExeExprCodeStack   Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo       Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId      현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[out]    aExprEntry          추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloCodeOptimizeColumn( stlChar            * aSQLString,
                                 qlvInitExpression  * aInitColumnCode,
                                 knlExprStack       * aExeExprCodeStack,
                                 knlExprStack       * aConstExprCodeStack,
                                 stlUInt32            aGroupEntryNo,
                                 knlBuiltInFunc       aRelatedFuncId,
                                 qloExprInfo        * aExprInfo,
                                 knlExprEntry      ** aExprEntry,
                                 knlRegionMem       * aRegionMem,
                                 qllEnv             * aEnv )
{
    knlExprStack        * sCodeStack  = aExeExprCodeStack;
    qlvInitColumn       * sInitColumn = NULL;
    dtlBuiltInFuncInfo  * sFuncInfo   = NULL;
    dtlDataType           sDataType   = DTL_TYPE_NA;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitColumnCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitColumnCode->mType == QLV_EXPR_TYPE_COLUMN,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Expression Code Stack에 column entry 추가
     */

    *aExprEntry = & sCodeStack->mEntries[ sCodeStack->mEntryCount ];
    sInitColumn = aInitColumnCode->mExpr.mColumn;
    sFuncInfo   = & gBuiltInFuncInfoArr[ aRelatedFuncId ];
    sDataType   = ellGetColumnDBType( sInitColumn->mColumnHandle );
    
    STL_TRY( knlExprAddValue( (knlExprStack *) sCodeStack,
                              aGroupEntryNo,
                              aInitColumnCode->mOffset,
                              sFuncInfo->mExceptionType,
                              sFuncInfo->mExceptionResult,
                              aGroupEntryNo - 1,
                              aRegionMem,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    

    /**
     * Set DB Type
     */
    
    STL_TRY( qloSetExprDBType( aInitColumnCode,
                               aExprInfo,
                               sDataType,
                               ellGetColumnPrecision( sInitColumn->mColumnHandle ),
                               ellGetColumnScale( sInitColumn->mColumnHandle ),
                               ellGetColumnStringLengthUnit( sInitColumn->mColumnHandle ),
                               ellGetColumnIntervalIndicator( sInitColumn->mColumnHandle ),
                               aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;
    
    STL_FINISH;

    if ( aSQLString != NULL )
    {
        stlChar * sLastError = NULL;
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
        if ( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                          qlgMakeErrPosString( aSQLString,
                                                               aInitColumnCode->mPosition,
                                                               aEnv ) );
        }
    }
    
    return STL_FAILURE;
}


/**
 * @brief VALUE 의 Code Area를 구성한다.
 * @param[in]     aSQLString          SQL String
 * @param[in]     aInitValueCode      Parse/Validation 단계의 Code Expression
 * @param[in,out] aExeExprCodeStack   Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo       Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId      현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[out]    aExprEntry          추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloCodeOptimizeValue( stlChar            * aSQLString,
                                qlvInitExpression  * aInitValueCode,
                                knlExprStack       * aExeExprCodeStack,
                                knlExprStack       * aConstExprCodeStack,
                                stlUInt32            aGroupEntryNo,
                                knlBuiltInFunc       aRelatedFuncId,
                                qloExprInfo        * aExprInfo,
                                knlExprEntry      ** aExprEntry,
                                knlRegionMem       * aRegionMem,
                                qllEnv             * aEnv )
{
    knlExprStack        * sCodeStack = aExeExprCodeStack;
    dtlBuiltInFuncInfo  * sFuncInfo  = NULL;
    dtlDataType           sDataType  = DTL_TYPE_NA;

    stlInt64              sArg1;
    stlInt64              sArg2;
    dtlStringLengthUnit   sLenUnit;  
    dtlIntervalIndicator  sIntervalIndicator;

    stlBool               sIsValid = STL_FALSE;    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitValueCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitValueCode->mType == QLV_EXPR_TYPE_VALUE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
            

    /**
     * Expression Code Stack에 value entry 추가
     */

    *aExprEntry = & sCodeStack->mEntries[ sCodeStack->mEntryCount ];
    sFuncInfo   = & gBuiltInFuncInfoArr[ aRelatedFuncId ];
        
    STL_TRY( knlExprAddValue( (knlExprStack *) sCodeStack,
                              aGroupEntryNo,
                              aInitValueCode->mOffset,
                              sFuncInfo->mExceptionType,
                              sFuncInfo->mExceptionResult,
                              aGroupEntryNo - 1,
                              aRegionMem,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    
    /**
     * Data Type 정하기
     */

    STL_TRY( qloGetValueDataType( aInitValueCode->mExpr.mValue,
                                  & sDataType,
                                  aEnv )
             == STL_SUCCESS );

    STL_DASSERT( ( sDataType >= 0 ) && ( sDataType < DTL_TYPE_MAX )  );
    
    
    /**
     * Set DB Type
     */
    if( aInitValueCode->mExpr.mValue->mInternalBindIdx != QLV_INTERNAL_BIND_VARIABLE_IDX_NA )
    {
        STL_TRY( qloSetExprDBType( aInitValueCode,
                                   aExprInfo,
                                   sDataType,
                                   1,
                                   gResultDataTypeDef[ sDataType ].mArgNum2,
                                   gResultDataTypeDef[ sDataType ].mStringLengthUnit,
                                   gResultDataTypeDef[ sDataType ].mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );
    }
    else if( aInitValueCode->mExpr.mValue->mValueType == QLV_VALUE_TYPE_NULL )
    {
        STL_TRY( qloSetExprDBType( aInitValueCode,
                                   aExprInfo,
                                   sDataType,
                                   1,
                                   gResultDataTypeDef[ sDataType ].mArgNum2,
                                   DTL_STRING_LENGTH_UNIT_OCTETS,
                                   gResultDataTypeDef[ sDataType ].mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        switch ( sDataType )
        {
            case DTL_TYPE_CHAR:
            case DTL_TYPE_VARCHAR:
                
                sArg1 = stlStrlen( aInitValueCode->mExpr.mValue->mData.mString );
                
                if( sArg1 == 0 )
                {
                    sArg1 = 1;
                }
                else
                {
                    STL_TRY( dtlGetMbstrlenWithLen( (KNL_DT_VECTOR(aEnv))->mGetCharSetIDFunc(aEnv),
                                                    aInitValueCode->mExpr.mValue->mData.mString,
                                                    sArg1,
                                                    & sArg1,
                                                    KNL_DT_VECTOR(aEnv),
                                                    aEnv,
                                                    QLL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );
                    
                    if ( sDataType == DTL_TYPE_CHAR )
                    {
                        STL_TRY_THROW( sArg1 <= DTL_VARCHAR_MAX_PRECISION, RAMP_ERR_INVALID_CHAR_LENGTH );
                    }
                    else
                    {
                        STL_TRY_THROW( sArg1 <= DTL_VARCHAR_MAX_PRECISION, RAMP_ERR_INVALID_VARCHAR_LENGTH );
                    }
                }
                
                sArg2    = gResultDataTypeDef[ sDataType ].mArgNum2;
                sLenUnit = DTL_STRING_LENGTH_UNIT_CHARACTERS;
                sIntervalIndicator = gResultDataTypeDef[ sDataType ].mIntervalIndicator;
                
                break;
            case DTL_TYPE_BINARY:
            case DTL_TYPE_VARBINARY:

                sArg1 = stlStrlen( aInitValueCode->mExpr.mValue->mData.mString );
                
                if( sArg1 == 0 )
                {
                    sArg1 = 1;
                }
                else
                {
                    STL_TRY( dtlValidateBitString( aInitValueCode->mExpr.mValue->mData.mString,
                                                   & sIsValid,
                                                   & sArg1,
                                                   QLL_ERROR_STACK(aEnv) )
                             == STL_SUCCESS );

                    sArg1 = STL_MAX( sArg1, 1 );
                    
                    STL_DASSERT( sIsValid == STL_TRUE );
                }
                
                if ( sDataType == DTL_TYPE_BINARY )
                {
                    STL_TRY_THROW( sArg1 <= DTL_VARBINARY_MAX_PRECISION, RAMP_ERR_INVALID_BINARY_LENGTH );
                }
                else
                {
                    STL_TRY_THROW( sArg1 <= DTL_VARBINARY_MAX_PRECISION, RAMP_ERR_INVALID_VARBINARY_LENGTH );
                }
                
                sArg2    = gResultDataTypeDef[ sDataType ].mArgNum2;
                sLenUnit = gResultDataTypeDef[ sDataType ].mStringLengthUnit;
                sIntervalIndicator = gResultDataTypeDef[ sDataType ].mIntervalIndicator;
                
                break;
            default:
                sArg1    = gResultDataTypeDef[ sDataType ].mArgNum1;
                sArg2    = gResultDataTypeDef[ sDataType ].mArgNum2;
                sLenUnit = gResultDataTypeDef[ sDataType ].mStringLengthUnit;
                sIntervalIndicator = gResultDataTypeDef[ sDataType ].mIntervalIndicator;
                break;
        }
            
        STL_TRY( qloSetExprDBType( aInitValueCode,
                                   aExprInfo,
                                   sDataType,
                                   sArg1,
                                   sArg2,
                                   sLenUnit,
                                   sIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_CHAR_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CHARACTER_LENGTH_INVALID,
                      qlgMakeErrPosString( aSQLString,
                                           aInitValueCode->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_VARCHAR_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_VARCHAR_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CHARACTER_VARYING_LENGTH_INVALID,
                      qlgMakeErrPosString( aSQLString,
                                           aInitValueCode->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_VARCHAR_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_BINARY_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BINARY_LENGTH_INVALID,
                      qlgMakeErrPosString( aSQLString,
                                           aInitValueCode->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_VARBINARY_MAX_PRECISION );
    }

    STL_CATCH( RAMP_ERR_INVALID_VARBINARY_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BINARY_VARYING_LENGTH_INVALID,
                      qlgMakeErrPosString( aSQLString,
                                           aInitValueCode->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      DTL_VARBINARY_MAX_PRECISION );
    }
    
    STL_FINISH;

    if ( aSQLString != NULL )
    {
        stlChar * sLastError = NULL;
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
        if ( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                          qlgMakeErrPosString( aSQLString,
                                                               aInitValueCode->mPosition,
                                                               aEnv ) );
        }
    }
    
    return STL_FAILURE;
}


/**
 * @brief Bind Parameter 의 Code Area를 구성한다.
 * @param[in]     aSQLString         SQL String
 * @param[in]     aInitParamCode     Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext       Bind Context
 * @param[in,out] aExeExprCodeStack  Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo      Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId     현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo          All Expression's Information
 * @param[out]    aExprEntry         추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem         Region Memory
 * @param[in]     aEnv               Environment
 */
stlStatus qloCodeOptimizeBindParam( stlChar            * aSQLString,
                                    qlvInitExpression  * aInitParamCode,
                                    knlBindContext     * aBindContext,
                                    knlExprStack       * aExeExprCodeStack,
                                    knlExprStack       * aConstExprCodeStack,
                                    stlUInt32            aGroupEntryNo,
                                    knlBuiltInFunc       aRelatedFuncId,
                                    qloExprInfo        * aExprInfo,
                                    knlExprEntry      ** aExprEntry,
                                    knlRegionMem       * aRegionMem,
                                    qllEnv             * aEnv )
{
    knlExprStack        * sCodeStack     = aExeExprCodeStack;
    qlvInitBindParam    * sInitBindParam = NULL;
    knlValueBlockList   * sValueBlock    = NULL;
    dtlBuiltInFuncInfo  * sFuncInfo      = NULL;
    dtlDataType           sDataType      = DTL_TYPE_NA;

    knlBindType   sUserBindType = KNL_BIND_TYPE_INVALID;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitParamCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitParamCode->mType == QLV_EXPR_TYPE_BIND_PARAM,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Expression Code Stack에 bind parameter entry 추가
     */

    *aExprEntry    = & sCodeStack->mEntries[ sCodeStack->mEntryCount ];
    sInitBindParam = aInitParamCode->mExpr.mBindParam;
    sFuncInfo      = & gBuiltInFuncInfoArr[ aRelatedFuncId ];

    
    /**
     * Bind Parameter 갯수 체크
     */

    STL_TRY_THROW( aBindContext != NULL, RAMP_ERR_BIND_NOT_EXIST );
    STL_TRY_THROW( sInitBindParam->mBindParamIdx < knlGetBindContextSize(aBindContext),
                   RAMP_ERR_BIND_NOT_EXIST );
    
    /**
     * IN Bind Type 에 부합하는 Bind Type 이어야 함.
     */

    STL_TRY( knlGetBindParamType( aBindContext,
                                  sInitBindParam->mBindParamIdx + 1,
                                  & sUserBindType,
                                  KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY_THROW( (sUserBindType == KNL_BIND_TYPE_IN) || (sUserBindType == KNL_BIND_TYPE_INOUT),
                   RAMP_BINDTYPE_MISMATCH );

    STL_TRY( knlGetExecuteParamValueBlock( aBindContext,
                                           sInitBindParam->mBindParamIdx + 1,
                                           & sValueBlock,
                                           KNL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );

    STL_TRY_THROW( sValueBlock != NULL, RAMP_ERR_BIND_NOT_EXIST );

    /**
     * Expression Stack 에 Value 추가 
     */
    
    STL_TRY( knlExprAddValue( (knlExprStack *) sCodeStack,
                              aGroupEntryNo,
                              aInitParamCode->mOffset,
                              sFuncInfo->mExceptionType,
                              sFuncInfo->mExceptionResult,
                              aGroupEntryNo - 1,
                              aRegionMem,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );
        

    /**
     * Set DB Type
     */

    sDataType = KNL_GET_BLOCK_DB_TYPE( sValueBlock );
        
    STL_TRY( qloSetExprDBType( aInitParamCode,
                               aExprInfo,
                               sDataType,
                               KNL_GET_BLOCK_ARG_NUM1( sValueBlock ),
                               KNL_GET_BLOCK_ARG_NUM2( sValueBlock ),
                               KNL_GET_BLOCK_STRING_LENGTH_UNIT( sValueBlock ),
                               KNL_GET_BLOCK_INTERVAL_INDICATOR( sValueBlock ),
                               aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_BIND_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BIND_VARIABLE_NOT_EXIST,
                      qlgMakeErrPosString( aSQLString,
                                           aInitParamCode->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_BINDTYPE_MISMATCH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_BIND_TYPE_MISMATCH,
                      qlgMakeErrPosString( aSQLString,
                                           aInitParamCode->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sInitBindParam->mBindParamIdx + 1 );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief FUNCTION 의 Code Area를 구성한다.
 * @param[in]     aSQLString          SQL String 
 * @param[in]     aInitFuncCode       Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext        Bind Context
 * @param[in,out] aExeExprCodeStack   Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo       Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId      현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[out]    aExprEntry          추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloCodeOptimizeFunction( stlChar            * aSQLString,
                                   qlvInitExpression  * aInitFuncCode,
                                   knlBindContext     * aBindContext,
                                   knlExprStack       * aExeExprCodeStack,
                                   knlExprStack       * aConstExprCodeStack,
                                   stlUInt32            aGroupEntryNo,
                                   knlBuiltInFunc       aRelatedFuncId,
                                   qloExprInfo        * aExprInfo,
                                   knlExprEntry      ** aExprEntry,
                                   knlRegionMem       * aRegionMem,
                                   qllEnv             * aEnv )
{
    knlExprStack        * sCodeStack  = aExeExprCodeStack;
    qlvInitFunction     * sInitFunc   = NULL;
    dtlBuiltInFuncInfo  * sFuncInfo   = NULL;
    stlInt32              sArgIdx     = 0;
    
    qlvInitExpression   * sCodeExpr     = NULL;
    knlExprEntry        * sFuncEntry    = NULL;
    stlUInt16             sArgCount     = 0;
    stlUInt32             sFuncPtrIdx   = 0;
    dtlDataType           sResultType   = DTL_TYPE_NA;
    dtlDataType           sArgType      = DTL_TYPE_NA;
    stlUInt32             sJumpEntryNo  = 0;
    stlUInt32             sGroupEntryNo = 0;
    stlUInt32             sUnitEntryCnt = 0;

    knlExprEntry        * sTmpExprEntry[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
    stlBool               sTmpIsConstantData[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
    dtlDataType           sTmpInputArgDataTypes[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
    dtlDataType           sTmpResultArgDataTypes[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];

    knlExprEntry       ** sExprEntry = sTmpExprEntry;
    stlBool             * sIsConstantData = sTmpIsConstantData;
    dtlDataType         * sInputArgDataTypes = sTmpInputArgDataTypes;
    dtlDataType         * sResultArgDataTypes = sTmpResultArgDataTypes;

    dtlDataTypeDefInfo    sTmpInputArgDataTypeDefInfo[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
    dtlDataTypeDefInfo    sTmpResultArgDataTypeDefInfo[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];

    dtlDataTypeDefInfo    sResultTypeDefInfo;
    dtlDataTypeDefInfo  * sInputArgDataTypeDefInfo = sTmpInputArgDataTypeDefInfo;
    dtlDataTypeDefInfo  * sResultArgDataTypeDefInfo = sTmpResultArgDataTypeDefInfo;
    dtlDataTypeDefInfo    sArgTypeDefInfo;

    stlInt64              sLength = 0;

    stlChar             * sLastError = NULL;
    stlBool               sIsSetErrorPos = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitFuncCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitFuncCode->mType == QLV_EXPR_TYPE_FUNCTION,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
        

    /**
     * Expression Code Stack에 function entry 추가 ( 기본 정보 )
     */

    sFuncEntry   = & sCodeStack->mEntries[ sCodeStack->mEntryCount + 1 ];
    sInitFunc    = aInitFuncCode->mExpr.mFunction;
    sFuncInfo    = & gBuiltInFuncInfoArr[ aRelatedFuncId ];
    sJumpEntryNo = aGroupEntryNo - 1;
    sArgCount    = sInitFunc->mArgCount;

    if( sArgCount > DTL_FUNC_INPUTARG_VARIABLE_CNT )
    {
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlExprEntry* ) * sArgCount,
                                    (void**) &sExprEntry,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
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
    
    STL_TRY( knlExprAddFunction( (knlExprStack *) sCodeStack,
                                 sInitFunc->mFuncId,
                                 gBuiltInFuncInfoArr[ sInitFunc->mFuncId ].mIsExprCompositionComplex,
                                 aInitFuncCode->mOffset,
                                 sArgCount,
                                 0,    /* unit entry count : entry 재설정 필요 */
                                 sFuncInfo->mExceptionType,
                                 sFuncInfo->mExceptionResult,
                                 sJumpEntryNo,
                                 & sGroupEntryNo,
                                 aRegionMem,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );
                                 
    sUnitEntryCnt++;


    /**
     * AND / OR 연산은 구분하여 처리한다.
     */

    if( ( sInitFunc->mFuncId == KNL_BUILTIN_FUNC_AND ) ||
        ( sInitFunc->mFuncId == KNL_BUILTIN_FUNC_OR ) )
    {
        STL_THROW( RAMP_AND_OR_EXPR );
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Expression Code Stack에 argument entries 추가
     */

    for( sArgIdx = sArgCount ; sArgIdx > 0 ; )
    {
        sArgIdx--;
        
        sCodeExpr = sInitFunc->mArgs[ sArgIdx ];
        
        STL_TRY( qloAddExprStackEntry( aSQLString,
                                       sCodeExpr,
                                       aBindContext,
                                       aExeExprCodeStack,
                                       aConstExprCodeStack,
                                       sGroupEntryNo,
                                       sInitFunc->mFuncId,
                                       aExprInfo,
                                       & sExprEntry[ sArgIdx ],
                                       aRegionMem,
                                       aEnv )
                 == STL_SUCCESS );

        if( sExprEntry[ sArgIdx ]->mExprType == KNL_EXPR_TYPE_FUNCTION )
        {
            /* KNL_EXPR_TYPE_FUNCTION */
            sUnitEntryCnt += sExprEntry[ sArgIdx ]->mExpr.mFuncInfo.mUnitEntryCount;
        }
        else
        {
            sUnitEntryCnt++;
        }

        switch( sCodeExpr->mIterationTime )
        {
            case DTL_ITERATION_TIME_FOR_EACH_QUERY :
            case DTL_ITERATION_TIME_FOR_EACH_STATEMENT :
            case DTL_ITERATION_TIME_NONE :
                {
                    sIsConstantData[ sArgIdx ] = STL_TRUE;
                    break;
                }
            default :
                {
                    sIsConstantData[ sArgIdx ] = STL_FALSE;
                    break;
                }
        }
        
        sInputArgDataTypes[ sArgIdx ] =
            aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mDBType;
        
        sInputArgDataTypeDefInfo[ sArgIdx ].mArgNum1 =
            aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mArgNum1;
        sInputArgDataTypeDefInfo[ sArgIdx ].mArgNum2 =
            aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mArgNum2;
        sInputArgDataTypeDefInfo[ sArgIdx ].mStringLengthUnit =
            aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mStringLengthUnit;
        sInputArgDataTypeDefInfo[ sArgIdx ].mIntervalIndicator =
            aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mIntervalIndicator;
    }

    /**
     * Function의 Return Data Type 결정하기
     */

    sFuncInfo = & gBuiltInFuncInfoArr[ sInitFunc->mFuncId ];
    
    STL_TRY( sFuncInfo->mGetFuncDetailInfo( sArgCount,
                                            sIsConstantData,
                                            sInputArgDataTypes,
                                            sInputArgDataTypeDefInfo,
                                            sArgCount,
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
     * Expression Function 정보 설정
     */

    STL_TRY( dtlGetDataValueBufferSize( sResultType,
                                        sResultTypeDefInfo.mArgNum1,
                                        sResultTypeDefInfo.mStringLengthUnit,
                                        & sLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        QLL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_DASSERT( sLength > 0 );
                 
    STL_TRY( qloSetExprFunc( aInitFuncCode,
                             aExprInfo,
                             sFuncEntry,
                             sInitFunc->mFuncId,
                             sFuncPtrIdx,
                             sLength,
                             sFuncInfo->mReturnTypeClass,
                             aEnv )
             == STL_SUCCESS );

    
    /**
     * Set DB Type : function의 DB Type 설정
     */

    STL_TRY( qloSetExprDBType( aInitFuncCode,
                               aExprInfo,
                               sResultType,
                               sResultTypeDefInfo.mArgNum1,
                               sResultTypeDefInfo.mArgNum2,
                               sResultTypeDefInfo.mStringLengthUnit,
                               sResultTypeDefInfo.mIntervalIndicator,
                               aEnv )
             == STL_SUCCESS );
    

    /**
     * Set Cast : arguments의 DB Type 설정
     */
    
    for( sArgIdx = 0 ; sArgIdx < sArgCount ; sArgIdx++ )
    {
        sArgType  = sResultArgDataTypes[ sArgIdx ];
        sArgTypeDefInfo.mArgNum1 = sResultArgDataTypeDefInfo[ sArgIdx ].mArgNum1;
        sArgTypeDefInfo.mArgNum2 = sResultArgDataTypeDefInfo[ sArgIdx ].mArgNum2;
        sArgTypeDefInfo.mStringLengthUnit = sResultArgDataTypeDefInfo[ sArgIdx ].mStringLengthUnit;
        sArgTypeDefInfo.mIntervalIndicator = sResultArgDataTypeDefInfo[ sArgIdx ].mIntervalIndicator;
        
        sCodeExpr = sInitFunc->mArgs[ sArgIdx ];
        
        STL_TRY_THROW( qloCastExprDBType( sCodeExpr,
                                          aExprInfo,
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
    }

    /**
     * TO_CHAR( str, fmt )
     * TO_CHAR_FORMAT 함수에서 TO_CHAR의 첫번째 인자 str에 대한 타입정보가 필요하므로,
     * 이에 대한 조정이 필요.
     *
     * TO_CHAR( str, fmt )를 parsing 단계에서 아래와 같이 변형해서 처리함.
     * TO_CHAR( str, TO_CHAR_FORMAT(fmt) )
     *         ^^^^^
     *
     * str의 type 정보를 보고 TO_CHAR_FORMAT 함수 포인터 연결정보를 재조정한다.
     * TO_CHAR_FORMAT( fmt )
     */
    
    if( sInitFunc->mFuncId == KNL_BUILTIN_FUNC_TO_CHAR )
    {
        sCodeExpr = sInitFunc->mArgs[ 1 ];

        if( sCodeExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
        {
            sCodeExpr = sCodeExpr->mExpr.mConstExpr->mOrgExpr;
        }
        else
        {
            /* Do Nothing */
        }

        STL_DASSERT( sCodeExpr->mType == QLV_EXPR_TYPE_FUNCTION );
        STL_DASSERT( sCodeExpr->mExpr.mFunction->mFuncId
                     == KNL_BUILTIN_FUNC_TO_CHAR_FORMAT );
        
        STL_TRY( dtlGetFuncPtrIdxToCharFormat(
                     sResultArgDataTypes[0],
                     & aExprInfo->mExprFunc[ sCodeExpr->mOffset ].mFuncIdx,
                     QLL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * Expression Code Stack에 function entry 재설정 : 
     * 1. unit entry count
     */

    sFuncEntry->mExpr.mFuncInfo.mUnitEntryCount   = sUnitEntryCnt;
    STL_THROW( RAMP_EXIT );

    
    /******************************************************
     * AND / OR Function에 대한 처리는 별도로 한다.
     ******************************************************/
    
    /**
     * Expression Code Stack에 argument entries 추가
     */

    STL_RAMP( RAMP_AND_OR_EXPR );

    for( sArgIdx = sArgCount ; sArgIdx > 0 ; )
    {
        sArgIdx--;
        sCodeExpr = sInitFunc->mArgs[ sArgIdx ];
        
        STL_TRY( qloAddExprStackEntry( aSQLString,
                                       sCodeExpr,
                                       aBindContext,
                                       aExeExprCodeStack,
                                       aConstExprCodeStack,
                                       sGroupEntryNo,
                                       sInitFunc->mFuncId,
                                       aExprInfo,
                                       & sExprEntry[ 0 ],
                                       aRegionMem,
                                       aEnv )
                 == STL_SUCCESS );

        if( sExprEntry[ 0 ]->mExprType == KNL_EXPR_TYPE_VALUE )
        {
            sUnitEntryCnt++;
        }
        else
        {
            /* KNL_EXPR_TYPE_FUNCTION */
            sUnitEntryCnt += sExprEntry[ 0 ]->mExpr.mFuncInfo.mUnitEntryCount;
        }
    }

    
    /**
     * Expression Function 정보 설정
     */

    sFuncInfo = & gBuiltInFuncInfoArr[ sInitFunc->mFuncId ];

    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_BOOLEAN,
                                        gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                                        gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                                        & sLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        QLL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_DASSERT( sLength > 0 );
                 
    STL_TRY( qloSetExprFunc( aInitFuncCode,
                             aExprInfo,
                             sFuncEntry,
                             sInitFunc->mFuncId,
                             sFuncPtrIdx,
                             sLength,
                             sFuncInfo->mReturnTypeClass,
                             aEnv )
             == STL_SUCCESS );

    
    /**
     * Set DB Type : function의 DB Type 설정
     */

    STL_TRY( qloSetExprDBType( aInitFuncCode,
                               aExprInfo,
                               DTL_TYPE_BOOLEAN,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                               aEnv )
             == STL_SUCCESS );
    

    /**
     * Set Cast : arguments의 DB Type 설정
     */
    
    for( sArgIdx = 0 ; sArgIdx < sArgCount ; sArgIdx++ )
    {
        sCodeExpr = sInitFunc->mArgs[ sArgIdx ];
        
        STL_TRY_THROW( qloCastExprDBType( sCodeExpr,
                                          aExprInfo,
                                          DTL_TYPE_BOOLEAN,
                                          gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                                          gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                                          gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                                          gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                                          STL_TRUE,
                                          STL_FALSE,
                                          aEnv )
                       == STL_SUCCESS,
                       RAMP_ERR_CAST_DB_TYPE );
    }

    
    /**
     * Expression Code Stack에 function entry 재설정 : 
     * 1. unit entry count
     */

    sFuncEntry->mExpr.mFuncInfo.mUnitEntryCount   = sUnitEntryCnt;
    

    /**
     * OUTPUT 설정
     */

    STL_RAMP( RAMP_EXIT );
    
    *aExprEntry = sFuncEntry;
    
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_CAST_DB_TYPE )
    {
        if( aSQLString != NULL )
        {
            sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
            
            if( sLastError[0] == '\0' )
            {
                stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                              qlgMakeErrPosString( aSQLString,
                                                                   sCodeExpr->mPosition,
                                                                   aEnv ) );
            }
        }
        sIsSetErrorPos = STL_TRUE;
    }

    STL_FINISH;

    if( ( aSQLString != NULL ) && ( sIsSetErrorPos == STL_FALSE ) )
    {
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
        if( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                          qlgMakeErrPosString( aSQLString,
                                                               aInitFuncCode->mPosition,
                                                               aEnv ) );
        }
    }
    
    return STL_FAILURE;
}


/**
 * @brief CAST 의 Code Area를 구성한다.
 * @param[in]     aSQLString          SQL String
 * @param[in]     aInitCastCode       Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext        Bind Context
 * @param[in,out] aExeExprCodeStack   Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo       Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId      현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[out]    aExprEntry          추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloCodeOptimizeCast( stlChar            * aSQLString,
                               qlvInitExpression  * aInitCastCode,
                               knlBindContext     * aBindContext,
                               knlExprStack       * aExeExprCodeStack,
                               knlExprStack       * aConstExprCodeStack,
                               stlUInt32            aGroupEntryNo,
                               knlBuiltInFunc       aRelatedFuncId,
                               qloExprInfo        * aExprInfo,
                               knlExprEntry      ** aExprEntry,
                               knlRegionMem       * aRegionMem,
                               qllEnv             * aEnv )
{
    knlExprStack        * sCodeStack    = aExeExprCodeStack;
    qlvInitTypeCast     * sInitCast     = NULL;
    qlvInitTypeDef      * sCastType     = NULL;
    dtlBuiltInFuncInfo  * sFuncInfo     = NULL;
    stlInt32              sArgIdx       = 0;

    qlvInitExpression   * sCodeExpr     = NULL;
    knlExprEntry        * sCastEntry    = NULL;
    knlExprEntry        * sExprEntry    = NULL;
    stlUInt32             sJumpEntryNo  = 0;
    stlUInt32             sGroupEntryNo = 0;
    stlUInt32             sUnitEntryCnt = 0;
    stlUInt32             sFuncPtrIdx   = 0;
    dtlDataType           sResultType   = DTL_TYPE_NA;
    
    stlBool               sIsConstantData[ 2 ];
    dtlDataType           sInputArgDataTypes[ 2 ];
    dtlDataType           sResultArgDataTypes[ 2 ];

    dtlDataTypeDefInfo    sResultTypeDefInfo;
    dtlDataTypeDefInfo    sInputArgDataTypeDefInfo[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
    dtlDataTypeDefInfo    sResultArgDataTypeDefInfo[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];

    stlInt64              sLength = 0;

    stlChar             * sLastError = NULL;
    stlBool               sIsSetErrorPos = STL_FALSE;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitCastCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitCastCode->mType == QLV_EXPR_TYPE_CAST,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Expression Code Stack에 function entry 추가 ( 기본 정보 )
     */

    sCastEntry   = & sCodeStack->mEntries[ sCodeStack->mEntryCount + 1 ];
    sInitCast    = aInitCastCode->mExpr.mTypeCast;
    sFuncInfo    = & gBuiltInFuncInfoArr[ aRelatedFuncId ];
    sJumpEntryNo = aGroupEntryNo - 1;
    sCastType    = & sInitCast->mTypeDef;
    
    STL_TRY( knlExprAddFunction( (knlExprStack *) sCodeStack,
                                 KNL_BUILTIN_FUNC_CAST,
                                 gBuiltInFuncInfoArr[ KNL_BUILTIN_FUNC_CAST ].mIsExprCompositionComplex,
                                 aInitCastCode->mOffset,
                                 DTL_CAST_INPUT_ARG_CNT,
                                 0,    /* unit entry count : entry 재설정 필요 */
                                 sFuncInfo->mExceptionType,
                                 sFuncInfo->mExceptionResult,
                                 sJumpEntryNo,
                                 & sGroupEntryNo,
                                 aRegionMem,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    sUnitEntryCnt++;

    /* type cast 정보 설정 */
    sIsConstantData[ 1 ] = STL_FALSE;

    sInputArgDataTypes[ 1 ] = sCastType->mDBType;

    STL_DASSERT( ( sInputArgDataTypes[ 1 ] >= 0 ) &&
                 ( sInputArgDataTypes[ 1 ] < DTL_TYPE_MAX )  );

    sInputArgDataTypeDefInfo[ 1 ].mArgNum1 = sCastType->mArgNum1;
    sInputArgDataTypeDefInfo[ 1 ].mArgNum2 = sCastType->mArgNum2;
    sInputArgDataTypeDefInfo[ 1 ].mStringLengthUnit =
        sCastType->mStringLengthUnit;
    sInputArgDataTypeDefInfo[ 1 ].mIntervalIndicator =
        sCastType->mIntervalIndicator;

    
    /**
     * Expression Code Stack에 argument entries 추가
     */

    /* StringLengthUnit, IntervalIndicator */
    for( sArgIdx = DTL_CAST_INPUT_ARG_CNT - 1 ; sArgIdx >= 3 ; sArgIdx-- )
    {
        sCodeExpr = sInitCast->mArgs[ sArgIdx ];
        
        STL_TRY( qloAddExprStackEntry( aSQLString,
                                       sCodeExpr,
                                       aBindContext,
                                       aExeExprCodeStack,
                                       aConstExprCodeStack,
                                       sGroupEntryNo,
                                       sCodeExpr->mRelatedFuncId,
                                       aExprInfo,
                                       & sExprEntry,
                                       aRegionMem,
                                       aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( qloCastExprDBType( sCodeExpr,
                                          aExprInfo,
                                          DTL_TYPE_NATIVE_INTEGER,
                                          DTL_PRECISION_NA,
                                          DTL_SCALE_NA,
                                          DTL_STRING_LENGTH_UNIT_NA,
                                          DTL_INTERVAL_INDICATOR_NA,
                                          STL_TRUE,
                                          STL_FALSE,
                                          aEnv )
                       == STL_SUCCESS,
                       RAMP_ERR_CAST_DB_TYPE );
        
        sUnitEntryCnt++;
    }

    /* ArgNum1, ArgNum2 */
    for( ; sArgIdx >= 1 ; sArgIdx-- )
    {
        sCodeExpr = sInitCast->mArgs[ sArgIdx ];
        
        STL_TRY( qloAddExprStackEntry( aSQLString,
                                       sCodeExpr,
                                       aBindContext,
                                       aExeExprCodeStack,
                                       aConstExprCodeStack,
                                       sGroupEntryNo,
                                       sCodeExpr->mRelatedFuncId,
                                       aExprInfo,
                                       & sExprEntry,
                                       aRegionMem,
                                       aEnv )
                 == STL_SUCCESS );

        STL_TRY_THROW( qloCastExprDBType( sCodeExpr,
                                          aExprInfo,
                                          DTL_TYPE_NATIVE_BIGINT,
                                          DTL_PRECISION_NA,
                                          DTL_SCALE_NA,
                                          DTL_STRING_LENGTH_UNIT_NA,
                                          DTL_INTERVAL_INDICATOR_NA,
                                          STL_TRUE,
                                          STL_FALSE,
                                          aEnv )
                       == STL_SUCCESS,
                       RAMP_ERR_CAST_DB_TYPE );
        
        sUnitEntryCnt++;
    }
   
    /* target value */
    sCodeExpr = sInitCast->mArgs[ 0 ];

    STL_TRY( qloAddExprStackEntry( aSQLString,
                                   sInitCast->mArgs[0],
                                   aBindContext,
                                   aExeExprCodeStack,
                                   aConstExprCodeStack,
                                   sGroupEntryNo,
                                   sInitCast->mArgs[0]->mRelatedFuncId,
                                   aExprInfo,
                                   & sExprEntry,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    if( sExprEntry->mExprType == KNL_EXPR_TYPE_VALUE )
    {
        sUnitEntryCnt++;
    }
    else
    {
        /* KNL_EXPR_TYPE_FUNCTION */
        sUnitEntryCnt += sExprEntry->mExpr.mFuncInfo.mUnitEntryCount;
    }

    sIsConstantData[ 0 ] = STL_FALSE;

    sInputArgDataTypes[ 0 ] =
        aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mDBType;

    STL_DASSERT( ( sInputArgDataTypes[ 0 ] >= 0 ) &&
                 ( sInputArgDataTypes[ 0 ] < DTL_TYPE_MAX )  );

    sInputArgDataTypeDefInfo[ 0 ].mArgNum1 =
        aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mArgNum1;
    sInputArgDataTypeDefInfo[ 0 ].mArgNum2 =
        aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mArgNum2;
    sInputArgDataTypeDefInfo[ 0 ].mStringLengthUnit =
        aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mStringLengthUnit;
    sInputArgDataTypeDefInfo[ 0 ].mIntervalIndicator =
        aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mIntervalIndicator;

    
    /**
     * Function의 Return Data Type 결정하기
     */

    sFuncInfo = & gBuiltInFuncInfoArr[ KNL_BUILTIN_FUNC_CAST ];
    
    STL_TRY( sFuncInfo->mGetFuncDetailInfo( 2,
                                            sIsConstantData,
                                            sInputArgDataTypes,
                                            sInputArgDataTypeDefInfo,
                                            1,
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
     * Expression Function 정보 설정
     */

    STL_TRY( dtlGetDataValueBufferSize( sCastType->mDBType,
                                        sCastType->mArgNum1,
                                        sCastType->mStringLengthUnit,
                                        & sLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        QLL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_DASSERT( sLength > 0 );
                 
    STL_TRY( qloSetExprFunc( aInitCastCode,
                             aExprInfo,
                             sCastEntry,
                             KNL_BUILTIN_FUNC_CAST,
                             sFuncPtrIdx,
                             sLength,
                             sFuncInfo->mReturnTypeClass,
                             aEnv )
             == STL_SUCCESS );


    /**
     * Set DB Type : function의 DB Type 설정
     */

    STL_TRY( qloSetExprDBType( aInitCastCode,
                               aExprInfo,
                               sCastType->mDBType,
                               sCastType->mArgNum1,
                               sCastType->mArgNum2,
                               sCastType->mStringLengthUnit,
                               sCastType->mIntervalIndicator,
                               aEnv )
             == STL_SUCCESS );

    
    /**
     * Expression Code Stack에 function entry 재설정 : 
     * 1. unit entry count
     */

    sCastEntry->mExpr.mFuncInfo.mUnitEntryCount = sUnitEntryCnt;
    

    /**
     * OUTPUT 설정
     */
    
    *aExprEntry = sCastEntry;

    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_CAST_DB_TYPE )
    {
        if( aSQLString != NULL )
        {
            sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
            
            if( sLastError[0] == '\0' )
            {
                stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                              qlgMakeErrPosString( aSQLString,
                                                                   sCodeExpr->mPosition,
                                                                   aEnv ) );
            }
        }
        sIsSetErrorPos = STL_TRUE;
    }

    STL_FINISH;

    if( ( aSQLString != NULL ) && ( sIsSetErrorPos == STL_FALSE ) )
    {
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
        if( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                          qlgMakeErrPosString( aSQLString,
                                                               aInitCastCode->mPosition,
                                                               aEnv ) );
        }
    }
    
    return STL_FAILURE;
}


/**
 * @brief CASE expression 의 Code Area를 구성한다.
 * @param[in]     aSQLString          SQL String
 * @param[in]     aInitCaseExprCode   Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext        Bind Context
 * @param[in,out] aExeExprCodeStack   Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo       Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId      현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[out]    aExprEntry          추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloCodeOptimizeCaseExpr( stlChar            * aSQLString,
                                   qlvInitExpression  * aInitCaseExprCode,
                                   knlBindContext     * aBindContext,
                                   knlExprStack       * aExeExprCodeStack,
                                   knlExprStack       * aConstExprCodeStack,
                                   stlUInt32            aGroupEntryNo,
                                   knlBuiltInFunc       aRelatedFuncId,
                                   qloExprInfo        * aExprInfo,
                                   knlExprEntry      ** aExprEntry,
                                   knlRegionMem       * aRegionMem,
                                   qllEnv             * aEnv )
{
    knlExprStack        * sCodeStack        = aExeExprCodeStack;
    qlvInitCaseExpr     * sInitCaseExpr     = NULL;
    dtlBuiltInFuncInfo  * sFuncInfo         = NULL;    
    stlInt32              sIdx              = 0;
    
    qlvInitExpression   * sCodeExpr         = NULL;
    knlExprEntry        * sCaseExprEntry    = NULL;
    knlExprEntry        * sExprEntry        = NULL;
    stlUInt32             sJumpEntryNo      = 0;
    stlUInt32             sGroupEntryNo     = 0;
    stlUInt32             sWhenGroupEntryNo = 0;
    stlUInt32             sUnitEntryCnt     = 0;
    
    qlvInitDataTypeInfo   sDataTypeInfo;
    
    stlInt64              sLength           = 0;
    
    stlChar             * sLastError        = NULL;
    stlBool               sIsSetErrorPos    = STL_FALSE;
    
    /**
     * Paramter Validation
     */
    
    STL_PARAM_VALIDATE( aInitCaseExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitCaseExprCode->mType == QLV_EXPR_TYPE_CASE_EXPR,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Expression Code Stack에 function entry 추가 ( 기본 정보 )
     */

    sCaseExprEntry = & sCodeStack->mEntries[ sCodeStack->mEntryCount + 1 ];
    sInitCaseExpr  = aInitCaseExprCode->mExpr.mCaseExpr;
    sFuncInfo      = & gBuiltInFuncInfoArr[ aRelatedFuncId ];
    sJumpEntryNo   = aGroupEntryNo - 1;
    
    STL_TRY( knlExprAddFunction( (knlExprStack *) sCodeStack,
                                 sInitCaseExpr->mFuncId,
                                 gBuiltInFuncInfoArr[ sInitCaseExpr->mFuncId ].mIsExprCompositionComplex,
                                 aInitCaseExprCode->mOffset,
                                 sInitCaseExpr->mCount,    
                                 0,    /* unit entry count : entry 재설정 필요  */
                                 sFuncInfo->mExceptionType,
                                 sFuncInfo->mExceptionResult,
                                 sJumpEntryNo,
                                 & sGroupEntryNo,
                                 aRegionMem,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    sUnitEntryCnt++;

    /**
     * Expression Code Stack에 argument entries 추가
     */
    
    /*
     * Default Result 추가
     */

    sCodeExpr = sInitCaseExpr->mDefResult;
    
    STL_TRY( qloAddExprStackEntry( aSQLString,
                                   sCodeExpr,
                                   aBindContext,
                                   aExeExprCodeStack,
                                   aConstExprCodeStack,
                                   sGroupEntryNo,
                                   KNL_BUILTIN_FUNC_RETURN_PASS,
                                   aExprInfo,
                                   & sExprEntry,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    if( sExprEntry->mExprType == KNL_EXPR_TYPE_VALUE )
    {
        sUnitEntryCnt++;
    }
    else
    {
        /* KNL_EXPR_TYPE_FUNCTION */
        sUnitEntryCnt += sExprEntry->mExpr.mFuncInfo.mUnitEntryCount;
    }

    /*
     * When ... Then 추가 
     */

    sWhenGroupEntryNo = ((knlExprStack *)sCodeStack)->mEntryCount + 1;
    
    for( sIdx = sInitCaseExpr->mCount; sIdx > 0;  )
    {
        sIdx--;
        /*
         * Then
         */
        
        sCodeExpr = sInitCaseExpr->mThenArr[ sIdx ];
        
        STL_TRY( qloAddExprStackEntry( aSQLString,
                                       sCodeExpr,
                                       aBindContext,
                                       aExeExprCodeStack,
                                       aConstExprCodeStack,
                                       sGroupEntryNo,
                                       KNL_BUILTIN_FUNC_RETURN_PASS,
                                       aExprInfo,
                                       & sExprEntry,
                                       aRegionMem,
                                       aEnv )
                 == STL_SUCCESS );

        if( sExprEntry->mExprType == KNL_EXPR_TYPE_VALUE )
        {
            sUnitEntryCnt++;
        }
        else
        {
            /* KNL_EXPR_TYPE_FUNCTION */
            sUnitEntryCnt += sExprEntry->mExpr.mFuncInfo.mUnitEntryCount;
        }

        /*
         * When
         */

        sCodeExpr = sInitCaseExpr->mWhenArr[ sIdx ];
        
        STL_TRY( qloAddExprStackEntry( aSQLString,
                                       sCodeExpr,
                                       aBindContext,
                                       aExeExprCodeStack,
                                       aConstExprCodeStack,
                                       sWhenGroupEntryNo,
                                       KNL_BUILTIN_FUNC_CASE,
                                       aExprInfo,
                                       & sExprEntry,
                                       aRegionMem,
                                       aEnv )
                 == STL_SUCCESS );

        if( sExprEntry->mExprType == KNL_EXPR_TYPE_VALUE )
        {
            sUnitEntryCnt++;
        }
        else
        {
            /* KNL_EXPR_TYPE_FUNCTION */
            sUnitEntryCnt += sExprEntry->mExpr.mFuncInfo.mUnitEntryCount;
        }

        sWhenGroupEntryNo = ((knlExprStack *)sCodeStack)->mEntryCount + 1;
    }

    /**
     * when절의 type validation
     */
    
    for( sIdx = 0; sIdx < sInitCaseExpr->mCount; sIdx++ )
    {
        sCodeExpr = sInitCaseExpr->mWhenArr[sIdx];
        
        if( sCodeExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
        {
            sCodeExpr = sCodeExpr->mExpr.mConstExpr->mOrgExpr;
        }

        if( sCodeExpr->mType == QLV_EXPR_TYPE_VALUE )
        {
            if( sCodeExpr->mExpr.mValue->mValueType == QLV_VALUE_TYPE_NULL )
            {
                /* CASE WHEN NULL THEN 'AAA' ELSE 'DEFAULT VALUE' END : success */
                
                /* Do Nothing */
            }
            else
            {
                /* CASE WHEN 1 THEN 'AAA' ELSE 'DEFAULT VALUE' END    : error
                 * CASE WHEN TRUE THEN 'AAA' ELSE 'DEFAULT VALUE' END : success
                 */
                STL_TRY_THROW( aExprInfo->mExprDBType[sCodeExpr->mOffset].mDBType == DTL_TYPE_BOOLEAN,
                               RAMP_ERR_INVALID_ARGUMENT_TYPE );
            }
        }
        else
        {
            STL_TRY_THROW( aExprInfo->mExprDBType[sCodeExpr->mOffset].mDBType == DTL_TYPE_BOOLEAN,
                           RAMP_ERR_INVALID_ARGUMENT_TYPE );
        }
    }        
    
    /**
     * Case expression의 result DB Type 얻기.
     */

    STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                     aRegionMem,
                                     aInitCaseExprCode,
                                     aBindContext,
                                     & sDataTypeInfo,
                                     aEnv )
             == STL_SUCCESS );
    
    
    /**
     * Expression Function 정보 설정
     */

    sFuncInfo = & gBuiltInFuncInfoArr[ sInitCaseExpr->mFuncId ];
    
    STL_TRY( dtlGetDataValueBufferSize( sDataTypeInfo.mDataType,
                                        sDataTypeInfo.mArgNum1,
                                        sDataTypeInfo.mStringLengthUnit,
                                        & sLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        QLL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    STL_DASSERT( sLength > 0 );
    
    STL_TRY( qloSetExprFunc( aInitCaseExprCode,
                             aExprInfo,
                             sCaseExprEntry,
                             sInitCaseExpr->mFuncId,
                             0,
                             sLength,
                             sFuncInfo->mReturnTypeClass,
                             aEnv )
             == STL_SUCCESS );
    
    /**
     * Set DB Type : function의 DB Type 설정
     */
    
    STL_TRY( qloSetExprDBType( aInitCaseExprCode,
                               aExprInfo,
                               sDataTypeInfo.mDataType,
                               sDataTypeInfo.mArgNum1,
                               sDataTypeInfo.mArgNum2,
                               sDataTypeInfo.mStringLengthUnit,
                               sDataTypeInfo.mIntervalIndicator,
                               aEnv )
             == STL_SUCCESS );
    
    /**
     * Set Cast : Default 의 DB Type 설정
     */
    
    sCodeExpr = sInitCaseExpr->mDefResult;

    STL_TRY_THROW( qloCastExprDBType( sCodeExpr,
                                      aExprInfo,
                                      sDataTypeInfo.mDataType,
                                      sDataTypeInfo.mArgNum1,
                                      sDataTypeInfo.mArgNum2,
                                      sDataTypeInfo.mStringLengthUnit,
                                      sDataTypeInfo.mIntervalIndicator,
                                      STL_FALSE,
                                      STL_FALSE,
                                      aEnv )
                   == STL_SUCCESS,
                   RAMP_ERR_CAST_DB_TYPE );
    
    /**
     * Set Cast : Then 의 DB Type 설정
     */
    
    for( sIdx = 0; sIdx < sInitCaseExpr->mCount; sIdx++ )
    {
        sCodeExpr = sInitCaseExpr->mThenArr[ sIdx ];
        
        STL_TRY_THROW( qloCastExprDBType( sCodeExpr,
                                          aExprInfo,
                                          sDataTypeInfo.mDataType,
                                          sDataTypeInfo.mArgNum1,
                                          sDataTypeInfo.mArgNum2,
                                          sDataTypeInfo.mStringLengthUnit,
                                          sDataTypeInfo.mIntervalIndicator,
                                          STL_FALSE,
                                          STL_FALSE,
                                          aEnv )
                       == STL_SUCCESS,
                       RAMP_ERR_CAST_DB_TYPE );
    }
    
    /**
     * Expression Code Stack에 function entry 재설정 : 
     * 1. unit entry count
     */

    sCaseExprEntry->mExpr.mFuncInfo.mUnitEntryCount   = sUnitEntryCnt;
    

    /**
     * OUTPUT 설정
     */
    
    *aExprEntry = sCaseExprEntry;
    
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_CAST_DB_TYPE )
    {
        if( aSQLString != NULL )
        {
            sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
            
            if( sLastError[0] == '\0' )
            {
                stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                              qlgMakeErrPosString( aSQLString,
                                                                   sCodeExpr->mPosition,
                                                                   aEnv ) );
            }
        }
        sIsSetErrorPos = STL_TRUE;
    }
    
    STL_CATCH( RAMP_ERR_INVALID_ARGUMENT_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_ARGUMENT_OF_CASE_WHEN_MUST_BE_TYPE_BOOLEAN,
                      qlgMakeErrPosString( aSQLString,
                                           sCodeExpr->mPosition,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
        
        sIsSetErrorPos = STL_TRUE;        
    }    
    
    STL_FINISH;
    
    if( ( aSQLString != NULL ) && ( sIsSetErrorPos == STL_FALSE ) )
    {
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
        if( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                          qlgMakeErrPosString( aSQLString,
                                                               aInitCaseExprCode->mPosition,
                                                               aEnv ) );
        }
    }
    
    return STL_FAILURE;
}


/**
 * @brief LIST FUNCTION 의 Code Area를 구성한다.
 * @param[in]     aSQLString          SQL String 
 * @param[in]     aInitColumnCode     Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext        Bind Context
 * @param[in,out] aExeExprCodeStack   Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo       Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId      현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[out]    aExprEntry          추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloCodeOptimizeListFunction( stlChar            * aSQLString,
                                       qlvInitExpression  * aInitColumnCode,
                                       knlBindContext     * aBindContext,
                                       knlExprStack       * aExeExprCodeStack,
                                       knlExprStack       * aConstExprCodeStack,
                                       stlUInt32            aGroupEntryNo,
                                       knlBuiltInFunc       aRelatedFuncId,
                                       qloExprInfo        * aExprInfo,
                                       knlExprEntry      ** aExprEntry,
                                       knlRegionMem       * aRegionMem,
                                       qllEnv             * aEnv )
{
    knlExprStack          * sCodeStack     = aExeExprCodeStack;
    qlvInitListFunc       * sInitFunc      = NULL;
    qlvInitExpression     * sLeftExpr      = NULL;
    qlvInitExpression     * sRightExpr     = NULL;
    qlvInitRowExpr        * sRowExpr       = NULL;

    knlExprEntry          * sFuncEntry     = NULL;
    stlUInt32               sFuncPtrIdx    = 0;
    stlUInt32               sJumpEntryNo   = 0;
    stlUInt32               sListJumpNo    = 0;
    stlUInt32               sPrevEntryCnt  = 0;
    knlExprEntry          * sExprEntry;
    stlBool                 sAddListEntry  = STL_TRUE;
    stlBool                 sOnlyValue     = STL_TRUE;
        
    dtlBuiltInFuncInfo    * sFuncInfo      = NULL;
    knlListStack          * sListStack;
    stlUInt16               sArgCount;
    stlUInt32               sEntryCount;
    stlInt32                sArgIdx;
    stlUInt16               sValueCount;

    stlChar               * sLastError      = NULL;
    stlBool                 sIsSetErrorPos  = STL_FALSE;

    stlInt32                sIdx            = 1;
    dtlDataType             sDBType[2];
    dtlDataType             sArgType[2];
    knlExprType             sExprType       = KNL_EXPR_TYPE_INVALID;
    
    dtlDataTypeDefInfo      sDBTypeInfo[2];
    qlvInitDataTypeInfo     sDataTypeInfo;
    
    dtlGroup                sReturnGroup;
    dtlIntervalIndicator    sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitColumnCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitColumnCode->mType == QLV_EXPR_TYPE_LIST_FUNCTION,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Expression Code Stack에 function entry 추가 ( 기본 정보 )
     */

    sFuncEntry    = & sCodeStack->mEntries[ sCodeStack->mEntryCount + 1 ];
    sInitFunc     = aInitColumnCode->mExpr.mListFunc;
    sFuncInfo     = & gBuiltInFuncInfoArr[ aRelatedFuncId ];
    sJumpEntryNo  = aGroupEntryNo - 1;
    sArgCount     = sInitFunc->mArgCount;
    sListJumpNo   = aExeExprCodeStack->mEntryCount;

    
    /**
     * @todo 현재 List Function에는 List Column이 Left와 Right만 올 수 있다. (확장시변경)
     * List Function에 List Column 은 Left, Right 두개만 올 수 있다 가정.
     */
    
    sArgCount = sInitFunc->mArgs[ 0 ]->mExpr.mListCol->mArgCount;
    
    STL_PARAM_VALIDATE( sArgCount > 0 , QLL_ERROR_STACK(aEnv) );

    if( sInitFunc->mArgCount == 2 )
    {
        sLeftExpr   = sInitFunc->mArgs[ 1 ];
        sRightExpr  = sInitFunc->mArgs[ 0 ];
        sRowExpr    = sLeftExpr->mExpr.mListCol->mArgs[0]->mExpr.mRowExpr;
        sEntryCount = ( ( sArgCount * sRowExpr->mArgCount ) * 2 );

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlListStack ),
                                    (void **) & sListStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sListStack->mMaxEntryCount = sEntryCount + 2;
        sListStack->mEntryCount    = sEntryCount;

    
        /**
         * entry 공간 할당
         */
    
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlListEntry ) * ( sListStack->mMaxEntryCount ),
                                    (void**) & sListStack->mEntries,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sListStack->mEntries,
                   0x00,
                   STL_SIZEOF( knlListEntry ) * ( sListStack->mMaxEntryCount ) );

    
        /**
         * Expression Code Stack에 argument entries 추가
         */

        sEntryCount = 1;
        for( sArgIdx = sArgCount ; sArgIdx > 0 ;  )
        {
            sArgIdx--;
            
            if( sAddListEntry == STL_TRUE )
            {
                /**
                 * List Column 의 Stack 생성
                 */
            
                STL_TRY( knlExprAddListExpr( (knlExprStack *) sCodeStack,
                                             aRelatedFuncId,
                                             gBuiltInFuncInfoArr[ aRelatedFuncId ].mIsExprCompositionComplex,
                                             aInitColumnCode->mOffset,
                                             DTL_LIST_PREDICATE_FUNCTION,
                                             sListStack->mEntryCount,
                                             sEntryCount,
                                             sFuncInfo->mExceptionType,
                                             sFuncInfo->mExceptionResult,
                                             sJumpEntryNo,
                                             ( aExeExprCodeStack->mEntryCount - sListJumpNo ),
                                             sOnlyValue,
                                             aRegionMem,
                                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );

                sIdx = 1;
                sAddListEntry = STL_FALSE;
            }
            else
            {
                /* Do Nothing */
            }

            sPrevEntryCnt = aExeExprCodeStack->mEntryCount;

        
            /**
             * Set List Stack Entry
             */
        
            sValueCount = sRowExpr->mArgCount;
            while( sValueCount > 0 )
            {
                sValueCount --;
            
            
                /**
                 * Function의 Return data type 결정하기
                 * 
                 * Set Cast : arguments의 DB Type 설정
                 */

                STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                                 aRegionMem,
                                                 sLeftExpr->mExpr.mListCol->mArgs[0]->mExpr.mRowExpr->mArgs[sValueCount],
                                                 aBindContext,
                                                 & sDataTypeInfo,
                                                 aEnv )
                         == STL_SUCCESS );

                sDBType[0]                        = sDataTypeInfo.mDataType;
                sDBTypeInfo[0].mArgNum1           = sDataTypeInfo.mArgNum1;
                sDBTypeInfo[0].mArgNum2           = sDataTypeInfo.mArgNum2;
                sDBTypeInfo[0].mStringLengthUnit  = sDataTypeInfo.mStringLengthUnit;
                sDBTypeInfo[0].mIntervalIndicator = sDataTypeInfo.mIntervalIndicator;
                
                STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                                 aRegionMem,
                                                 sRightExpr->mExpr.mListCol->mArgs[sArgIdx]->mExpr.mRowExpr->mArgs[sValueCount],
                                                 aBindContext,
                                                 & sDataTypeInfo,
                                                 aEnv )
                         == STL_SUCCESS );

                sDBType[1]                        = sDataTypeInfo.mDataType;
                sDBTypeInfo[1].mArgNum1           = sDataTypeInfo.mArgNum1;
                sDBTypeInfo[1].mArgNum2           = sDataTypeInfo.mArgNum2;
                sDBTypeInfo[1].mStringLengthUnit  = sDataTypeInfo.mStringLengthUnit;
                sDBTypeInfo[1].mIntervalIndicator = sDataTypeInfo.mIntervalIndicator;

                STL_TRY( dtlGetDataTypesConversionInfo( sDBType,
                                                        2,
                                                        & sReturnGroup,
                                                        QLL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );

                /**
                 * return group을 통하여
                 * Func Argument data type 얻기.
                 */
            
                STL_TRY( dtlSetFuncArgDataTypeInfo( sDBType,
                                                    2,
                                                    sReturnGroup,
                                                    sArgType,
                                                    QLL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );

                /**
                 * INTERVAL TYPE 인 경우, indicator를 설정
                 */
            
                if( ( sReturnGroup == DTL_GROUP_INTERVAL_YEAR_TO_MONTH ) ||
                    ( sReturnGroup == DTL_GROUP_INTERVAL_DAY_TO_SECOND ) )
                {
                    STL_TRY( dtlGetIntervalTypeIndicator( sDBTypeInfo,
                                                          2,
                                                          & sIntervalIndicator,
                                                          QLL_ERROR_STACK( aEnv ) )
                             == STL_SUCCESS );
                
                }
                else
                {
                    sIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;
                }

                /**
                 * Left Column
                 */

                sPrevEntryCnt = aExeExprCodeStack->mEntryCount;
                sRowExpr = sLeftExpr->mExpr.mListCol->mArgs[sArgIdx]->mExpr.mRowExpr;
                
                STL_TRY( qloAddExprStackEntry( aSQLString,
                                               sRowExpr->mArgs[ sValueCount ],
                                               aBindContext,
                                               aExeExprCodeStack,
                                               aConstExprCodeStack,
                                               aGroupEntryNo,
                                               sInitFunc->mFuncId,
                                               aExprInfo,
                                               & sExprEntry,
                                               aRegionMem,
                                               aEnv )
                         == STL_SUCCESS );

                switch( sRowExpr->mArgs[ sValueCount ]->mType )
                {
                    case QLV_EXPR_TYPE_VALUE :
                    case QLV_EXPR_TYPE_BIND_PARAM :
                    case QLV_EXPR_TYPE_PSEUDO_COLUMN :
                    case QLV_EXPR_TYPE_COLUMN :
                    case QLV_EXPR_TYPE_ROWID_COLUMN :
                    case QLV_EXPR_TYPE_INNER_COLUMN :
                    case QLV_EXPR_TYPE_OUTER_COLUMN :
                    case QLV_EXPR_TYPE_AGGREGATION :
                    case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
                    case QLV_EXPR_TYPE_REFERENCE :
                        {
                            /**
                             * Pop-up Entries
                             * @remark List Function에 대한 Row Expr Value Stack 구성은 Offset으로 접근하므로
                             * Stack의 Argument 구성은 불필요하다.
                             */

                            sExprType = KNL_EXPR_TYPE_VALUE;
                            break;
                        }
                    case QLV_EXPR_TYPE_FUNCTION :
                    case QLV_EXPR_TYPE_CAST :
                    case QLV_EXPR_TYPE_CASE_EXPR :
                        {
                            /* Do Nothing */
                    
                            sExprType = KNL_EXPR_TYPE_FUNCTION;
                            break;
                        }
                    case QLV_EXPR_TYPE_LIST_FUNCTION :
                        {
                            /* Do Nothing */
                            sExprType = KNL_EXPR_TYPE_LIST;
                            break;
                        }
                    case QLV_EXPR_TYPE_ROW_EXPR :
                    case QLV_EXPR_TYPE_LIST_COLUMN :
                    case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
                    case QLV_EXPR_TYPE_SUB_QUERY :
                    default :
                        {
                            STL_DASSERT( 0 );
                            break;
                        }
                }
                sRowExpr->mEntry[ sValueCount ].mOffset = sRowExpr->mArgs[ sValueCount ]->mOffset;
                sRowExpr->mEntry[ sValueCount ].mType   = sExprType;

                sLeftExpr->mExpr.mListCol->mEntry[ sArgIdx ] = sRowExpr->mEntry;
                
                STL_TRY( qloCastExprDBType( sRowExpr->mArgs[ sValueCount ],
                                            aExprInfo,
                                            sArgType[0],
                                            gResultDataTypeDef[ sArgType[0] ].mArgNum1,
                                            gResultDataTypeDef[ sArgType[0] ].mArgNum2,
                                            gResultDataTypeDef[ sArgType[0] ].mStringLengthUnit,
                                            sIntervalIndicator,
                                            STL_TRUE,
                                            STL_FALSE,
                                            aEnv )
                         == STL_SUCCESS );

                if( ( aExeExprCodeStack->mEntryCount - sPrevEntryCnt ) == 1 )
                {
                    if( aExeExprCodeStack->mEntries[ aExeExprCodeStack->mEntryCount ].mExprType
                        == KNL_EXPR_TYPE_VALUE )
                    {
                        if( aExprInfo->mExprDBType[ aExeExprCodeStack->mEntries
                                                    [ aExeExprCodeStack->mEntryCount ].mExpr.mValueInfo.mOffset ].mIsNeedCast
                            == STL_FALSE )
                        {
                            aExeExprCodeStack->mEntryCount = sPrevEntryCnt;
                        }                        
                    }
                    else
                    {
                        sOnlyValue = STL_FALSE;
                        sAddListEntry = STL_TRUE;
                    }
                }
                else
                {
                    sOnlyValue = STL_FALSE;
                    sAddListEntry = STL_TRUE;
                }
                

                /**
                 * Right Column
                 */

                sPrevEntryCnt = aExeExprCodeStack->mEntryCount;
                sRowExpr = sRightExpr->mExpr.mListCol->mArgs[sArgIdx]->mExpr.mRowExpr;
                
                STL_TRY( qloAddExprStackEntry( aSQLString,
                                               sRowExpr->mArgs[ sValueCount ],
                                               aBindContext,
                                               aExeExprCodeStack,
                                               aConstExprCodeStack,
                                               aGroupEntryNo,
                                               sInitFunc->mFuncId,
                                               aExprInfo,
                                               & sExprEntry,
                                               aRegionMem,
                                               aEnv )
                         == STL_SUCCESS );

                switch( sRowExpr->mArgs[ sValueCount ]->mType )
                {
                    case QLV_EXPR_TYPE_VALUE :
                    case QLV_EXPR_TYPE_BIND_PARAM :
                    case QLV_EXPR_TYPE_PSEUDO_COLUMN :
                    case QLV_EXPR_TYPE_COLUMN :
                    case QLV_EXPR_TYPE_ROWID_COLUMN :
                    case QLV_EXPR_TYPE_INNER_COLUMN :
                    case QLV_EXPR_TYPE_OUTER_COLUMN :
                    case QLV_EXPR_TYPE_AGGREGATION :
                    case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
                    case QLV_EXPR_TYPE_REFERENCE :
                        {
                            /**
                             * Pop-up Entries
                             * @remark List Function에 대한 Row Expr Value Stack 구성은 Offset으로 접근하므로
                             * Stack의 Argument 구성은 불필요하다.
                             */

                            sExprType = KNL_EXPR_TYPE_VALUE;
                            break;
                        }
                    case QLV_EXPR_TYPE_FUNCTION :
                    case QLV_EXPR_TYPE_CAST :
                    case QLV_EXPR_TYPE_CASE_EXPR :
                        {
                            /* Do Nothing */
                    
                            sExprType = KNL_EXPR_TYPE_FUNCTION;
                            break;
                        }
                    case QLV_EXPR_TYPE_LIST_FUNCTION :
                        {
                            /* Do Nothing */
                            sExprType = KNL_EXPR_TYPE_LIST;
                            break;
                        }
                    case QLV_EXPR_TYPE_ROW_EXPR :
                    case QLV_EXPR_TYPE_LIST_COLUMN :
                    case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
                    case QLV_EXPR_TYPE_SUB_QUERY :
                    default :
                        {
                            STL_DASSERT( 0 );
                            break;
                        }
                }
                sRowExpr->mEntry[ sValueCount ].mOffset = sRowExpr->mArgs[ sValueCount ]->mOffset;
                sRowExpr->mEntry[ sValueCount ].mType   = sExprType;

                sRightExpr->mExpr.mListCol->mEntry[ sArgIdx ] = sRowExpr->mEntry;
                
                STL_TRY( qloCastExprDBType( sRowExpr->mArgs[ sValueCount ],
                                            aExprInfo,
                                            sArgType[1],
                                            gResultDataTypeDef[ sArgType[1] ].mArgNum1,
                                            gResultDataTypeDef[ sArgType[1] ].mArgNum2,
                                            gResultDataTypeDef[ sArgType[1] ].mStringLengthUnit,
                                            sIntervalIndicator,
                                            STL_TRUE,
                                            STL_FALSE,
                                            aEnv )
                         == STL_SUCCESS );

                if( ( aExeExprCodeStack->mEntryCount - sPrevEntryCnt ) == 1 )
                {
                    if( aExeExprCodeStack->mEntries[ aExeExprCodeStack->mEntryCount ].mExprType
                        == KNL_EXPR_TYPE_VALUE )
                    {
                        if( aExprInfo->mExprDBType[ aExeExprCodeStack->mEntries
                                                    [ aExeExprCodeStack->mEntryCount ].mExpr.mValueInfo.mOffset ].mIsNeedCast
                            == STL_FALSE )
                        {
                            aExeExprCodeStack->mEntryCount = sPrevEntryCnt;
                        }                        
                    }
                    else
                    {
                        sOnlyValue = STL_FALSE;
                        sAddListEntry = STL_TRUE;
                    }
                }
                else
                {
                    sOnlyValue = STL_FALSE;
                    sAddListEntry = STL_TRUE;
                }

                
                /**
                 * Set List Stack Entry
                 */

                sListStack->mEntries[ sEntryCount ] =
                    sLeftExpr->mExpr.mListCol->mEntry[sArgIdx][sValueCount];
                sListStack->mEntries[ sEntryCount ].mIdx = sIdx;
            
                sListStack->mEntries[ sEntryCount + 1 ] =
                    sRightExpr->mExpr.mListCol->mEntry[sArgIdx][sValueCount];
                sListStack->mEntries[ sEntryCount + 1 ].mIdx = sIdx + 1;

                sEntryCount += 2;
                sIdx        += 2;
            }
        }

        sLeftExpr->mExpr.mListCol->mArgCount  = sArgCount;
        sRightExpr->mExpr.mListCol->mArgCount = sArgCount;

        
        /**
         * Set List Func Info
         */
    
        STL_TRY( qloSetListFuncExpr( aInitColumnCode,
                                     aExprInfo,
                                     sListStack,
                                     sRowExpr->mArgCount,
                                     sOnlyValue,
                                     aRegionMem,
                                     aEnv )
                 == STL_SUCCESS );

    
        /**
         * Expression Function 정보 설정
         */
    
        STL_TRY( qloSetExprFunc( aInitColumnCode,
                                 aExprInfo,
                                 sFuncEntry,
                                 sInitFunc->mFuncId,
                                 sFuncPtrIdx,
                                 DTL_BOOLEAN_SIZE,
                                 sFuncInfo->mReturnTypeClass,
                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* EXISTS function */
        STL_DASSERT( sInitFunc->mArgCount == 1 );

        sRightExpr  = sInitFunc->mArgs[ 0 ];
        sRowExpr    = sRightExpr->mExpr.mListCol->mArgs[0]->mExpr.mRowExpr;
        sEntryCount = 1;
        sListStack  = NULL;

        sPrevEntryCnt = aExeExprCodeStack->mEntryCount;
    
        /**
         * Expression Code Stack에 argument entries 추가
         */

        STL_TRY( qloAddExprStackEntry( aSQLString,
                                       sRightExpr,
                                       aBindContext,
                                       aExeExprCodeStack,
                                       aConstExprCodeStack,
                                       aGroupEntryNo,
                                       sInitFunc->mFuncId,
                                       aExprInfo,
                                       & sExprEntry,
                                       aRegionMem,
                                       aEnv )
                 == STL_SUCCESS );

        if( ( aExeExprCodeStack->mEntryCount - sPrevEntryCnt ) == 1 )
        {
            if( ( aExeExprCodeStack->mEntries[ aExeExprCodeStack->mEntryCount ].mExprType
                  == KNL_EXPR_TYPE_VALUE ) &&
                ( aExprInfo->mExprDBType[ aExeExprCodeStack->mEntries
                                          [ aExeExprCodeStack->mEntryCount ].mExpr.mValueInfo.mOffset ].mIsNeedCast
                  == STL_FALSE ) )
            {
                aExeExprCodeStack->mEntryCount = sPrevEntryCnt;
            }
        }
        
        sRightExpr->mExpr.mListCol->mArgCount = 1;
    }
    

    /**
     * Set DB Type : function의 DB Type 설정
     */

    STL_TRY( qloSetExprDBType( aInitColumnCode,
                               aExprInfo,
                               DTL_TYPE_BOOLEAN,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                               aEnv )
             == STL_SUCCESS );
    

    /**
     * OUTPUT 설정
     */
    
    *aExprEntry = sFuncEntry;
    

    return STL_SUCCESS;
    
    STL_FINISH;

    if( ( aSQLString != NULL ) && ( sIsSetErrorPos == STL_FALSE ) )
    {
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
        if( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                          qlgMakeErrPosString( aSQLString,
                                                               aInitColumnCode->mPosition,
                                                               aEnv ) );
        }
    }
    
    return STL_FAILURE;
}

    
/**
 * @brief LIST COLUMN 의 Code Area를 구성한다.
 * @param[in]     aSQLString          SQL String 
 * @param[in]     aInitColumnCode     Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext        Bind Context
 * @param[in,out] aExeExprCodeStack   Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo       Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId      현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[out]    aExprEntry          추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloCodeOptimizeListColumn( stlChar            * aSQLString,
                                     qlvInitExpression  * aInitColumnCode,
                                     knlBindContext     * aBindContext,
                                     knlExprStack       * aExeExprCodeStack,
                                     knlExprStack       * aConstExprCodeStack,
                                     stlUInt32            aGroupEntryNo,
                                     knlBuiltInFunc       aRelatedFuncId,
                                     qloExprInfo        * aExprInfo,
                                     knlExprEntry      ** aExprEntry,
                                     knlRegionMem       * aRegionMem,
                                     qllEnv             * aEnv )
{
    knlExprStack        * sCodeStack    = aExeExprCodeStack;
    qlvInitExpression   * sCodeExpr     = NULL;
    qlvInitListCol      * sInitColumn   = NULL;
    knlExprEntry        * sExprEntry    = NULL;
    stlUInt16             sArgCount;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitColumnCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitColumnCode->mType == QLV_EXPR_TYPE_LIST_COLUMN,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Expression code stack에 row expr entry 추가
     */
    
    *aExprEntry    = & sCodeStack->mEntries[ sCodeStack->mEntryCount + 1];
    sInitColumn    = aInitColumnCode->mExpr.mListCol;
    sInitColumn->mArgCount -= 1;
    sArgCount      = sInitColumn->mArgCount;

    /**
     *  List Column의 하나의 Row안의 value들을 stack에 추가.
     */

    
    /**
     * Expression Code Stack에 argument entries 추가
     */
    sCodeExpr = sInitColumn->mArgs[ sArgCount ];

    STL_TRY( qloAddExprStackEntry( aSQLString,
                                   sCodeExpr,
                                   aBindContext,
                                   aExeExprCodeStack,
                                   aConstExprCodeStack,
                                   aGroupEntryNo,
                                   aRelatedFuncId,
                                   aExprInfo,
                                   & sExprEntry,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /**
     * List column 에는 Row Expr만 올 수 있다. 추후 확장
     */
        
    sInitColumn->mEntry[ sArgCount ]  = sCodeExpr->mExpr.mRowExpr->mEntry;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

    
/**
 * @brief ROW EXPR 의 Code Area를 구성한다.
 * @param[in]     aSQLString          SQL String 
 * @param[in]     aInitColumnCode     Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext        Bind Context
 * @param[in,out] aExeExprCodeStack   Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo       Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId      현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[out]    aExprEntry          추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloCodeOptimizeRowExpr( stlChar            * aSQLString,
                                  qlvInitExpression  * aInitColumnCode,
                                  knlBindContext     * aBindContext,
                                  knlExprStack       * aExeExprCodeStack,
                                  knlExprStack       * aConstExprCodeStack,
                                  stlUInt32            aGroupEntryNo,
                                  knlBuiltInFunc       aRelatedFuncId,
                                  qloExprInfo        * aExprInfo,
                                  knlExprEntry      ** aExprEntry,
                                  knlRegionMem       * aRegionMem,
                                  qllEnv             * aEnv )
{
    knlExprStack        * sCodeStack    = aExeExprCodeStack;
    qlvInitExpression   * sCodeExpr     = NULL;
    qlvInitRowExpr      * sInitColumn   = NULL;
    stlInt32              sArgIdx;
    
    knlExprEntry        * sExprEntry     = NULL;
    knlExprType           sExprType      = KNL_EXPR_TYPE_INVALID;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitColumnCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitColumnCode->mType == QLV_EXPR_TYPE_ROW_EXPR,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Expression code stack에 row expr entry 추가
     */

    *aExprEntry    = & sCodeStack->mEntries[ sCodeStack->mEntryCount + 1];
    sInitColumn    = aInitColumnCode->mExpr.mRowExpr;
    
    /**
     * Expression Code Stack에 argument entries 추가
     */
    for( sArgIdx = sInitColumn->mArgCount; sArgIdx > 0; )
    {
        sArgIdx--;
        
        sCodeExpr    = sInitColumn->mArgs[ sArgIdx ];
        STL_TRY( qloAddExprStackEntry( aSQLString,
                                       sCodeExpr,
                                       aBindContext,
                                       aExeExprCodeStack,
                                       aConstExprCodeStack,
                                       aGroupEntryNo,
                                       aRelatedFuncId,
                                       aExprInfo,
                                       & sExprEntry,
                                       aRegionMem,
                                       aEnv )
                 == STL_SUCCESS );

        switch( sCodeExpr->mType )
        {
            case QLV_EXPR_TYPE_VALUE :
            case QLV_EXPR_TYPE_BIND_PARAM :
            case QLV_EXPR_TYPE_PSEUDO_COLUMN :
            case QLV_EXPR_TYPE_COLUMN :
            case QLV_EXPR_TYPE_ROWID_COLUMN :
            case QLV_EXPR_TYPE_INNER_COLUMN :
            case QLV_EXPR_TYPE_OUTER_COLUMN :
            case QLV_EXPR_TYPE_AGGREGATION :
            case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
            case QLV_EXPR_TYPE_REFERENCE :
                {
                    /**
                     * Pop-up Entries
                     * @remark List Function에 대한 Row Expr Value Stack 구성은 Offset으로 접근하므로
                     * Stack의 Argument 구성은 불필요하다.
                     */

                    sExprType = KNL_EXPR_TYPE_VALUE;
                    break;
                }
            case QLV_EXPR_TYPE_FUNCTION :
            case QLV_EXPR_TYPE_CAST :
            case QLV_EXPR_TYPE_CASE_EXPR :
                {
                    sExprType = KNL_EXPR_TYPE_FUNCTION;
                    break;
                }
            case QLV_EXPR_TYPE_LIST_FUNCTION :
                {
                    /* Do Nothing - dead code */
                    sExprType = KNL_EXPR_TYPE_LIST;
                    break;
                }
            case QLV_EXPR_TYPE_ROW_EXPR :
            case QLV_EXPR_TYPE_LIST_COLUMN :
            case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
            case QLV_EXPR_TYPE_SUB_QUERY :
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
        sInitColumn->mEntry[ sArgIdx ].mOffset = sCodeExpr->mOffset;
        sInitColumn->mEntry[ sArgIdx ].mType   = sExprType;
    }
    
    /**
     * OUTPUT 설정
     */

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}
    
/**
 * @brief PSEUDO COLUMN 의 Code Area를 구성한다.
 * @param[in]     aSQLString          SQL String
 * @param[in]     aInitValueCode      Parse/Validation 단계의 Code Expression
 * @param[in,out] aExeExprCodeStack   Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo       Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId      현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[out]    aExprEntry          추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloCodeOptimizePseudoCol( stlChar            * aSQLString,
                                    qlvInitExpression  * aInitValueCode,
                                    knlExprStack       * aExeExprCodeStack,
                                    knlExprStack       * aConstExprCodeStack,
                                    stlUInt32            aGroupEntryNo,
                                    knlBuiltInFunc       aRelatedFuncId,
                                    qloExprInfo        * aExprInfo,
                                    knlExprEntry      ** aExprEntry,
                                    knlRegionMem       * aRegionMem,
                                    qllEnv             * aEnv )
{
    knlExprStack        * sCodeStack = aExeExprCodeStack;
    qlvInitPseudoCol    * sInitValue = NULL;
    dtlBuiltInFuncInfo  * sFuncInfo  = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitValueCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitValueCode->mType == QLV_EXPR_TYPE_PSEUDO_COLUMN,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
            

    /**
     * Expression Code Stack에 value entry 추가
     */

    *aExprEntry = & sCodeStack->mEntries[ sCodeStack->mEntryCount ];
    sInitValue  = aInitValueCode->mExpr.mPseudoCol;
    sFuncInfo   = & gBuiltInFuncInfoArr[ aRelatedFuncId ];
        
    STL_TRY( knlExprAddValue( (knlExprStack *) sCodeStack,
                              aGroupEntryNo,
                              aInitValueCode->mOffset,
                              sFuncInfo->mExceptionType,
                              sFuncInfo->mExceptionResult,
                              aGroupEntryNo - 1,
                              aRegionMem,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    
    /**
     * gPseudoColInfoArr 로 부터 DB Type 정보 설정
     */
    
    STL_TRY( qloSetExprDBType( aInitValueCode,
                               aExprInfo,
                               gPseudoColInfoArr[ sInitValue->mPseudoId ].mDataType,
                               gPseudoColInfoArr[ sInitValue->mPseudoId ].mArgNum1,
                               gPseudoColInfoArr[ sInitValue->mPseudoId ].mArgNum2,
                               gPseudoColInfoArr[ sInitValue->mPseudoId ].mStringLengthUnit,
                               gPseudoColInfoArr[ sInitValue->mPseudoId ].mIntervalIndicator,
                               aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;
    
    STL_FINISH;

    if ( aSQLString != NULL )
    {
        stlChar * sLastError = NULL;
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
        if ( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                          qlgMakeErrPosString( aSQLString,
                                                               aInitValueCode->mPosition,
                                                               aEnv ) );
        }
    }
    
    return STL_FAILURE;
}


/**
 * @brief CONSTANT EXPRESSION 의 Code Area를 구성한다.
 * @param[in]     aSQLString          SQL String
 * @param[in]     aInitExprCode       Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext        Bind Context
 * @param[in,out] aExeExprCodeStack   Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo       Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId      현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[out]    aExprEntry          추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloCodeOptimizeConstExpr( stlChar            * aSQLString,
                                    qlvInitExpression  * aInitExprCode,
                                    knlBindContext     * aBindContext,
                                    knlExprStack       * aExeExprCodeStack,
                                    knlExprStack       * aConstExprCodeStack,
                                    stlUInt32            aGroupEntryNo,
                                    knlBuiltInFunc       aRelatedFuncId,
                                    qloExprInfo        * aExprInfo,
                                    knlExprEntry      ** aExprEntry,
                                    knlRegionMem       * aRegionMem,
                                    qllEnv             * aEnv )
{
    knlExprStack        * sCodeStack = aExeExprCodeStack;
    qlvInitExpression   * sCodeConst = NULL;
    dtlBuiltInFuncInfo  * sFuncInfo  = NULL;

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprCode->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Expression Code Stack에 value entry 추가
     */

    *aExprEntry = & sCodeStack->mEntries[ sCodeStack->mEntryCount ];
    sFuncInfo   = & gBuiltInFuncInfoArr[ aRelatedFuncId ];
        
    STL_TRY( knlExprAddValue( (knlExprStack *) sCodeStack,
                              aGroupEntryNo,
                              aInitExprCode->mOffset,
                              sFuncInfo->mExceptionType,
                              sFuncInfo->mExceptionResult,
                              aGroupEntryNo - 1,
                              aRegionMem,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * Constant Expression Stack에 원본 expression에 대한 entries 구성
     */

    STL_PARAM_VALIDATE( aInitExprCode->mExpr.mConstExpr != NULL,
                        QLL_ERROR_STACK(aEnv) );

    sCodeConst = aInitExprCode->mExpr.mConstExpr->mOrgExpr;
    
 
    /**
     * DB Type 정보 설정
     */

    STL_TRY( qloSetExprDBType( aInitExprCode,
                               aExprInfo,
                               aExprInfo->mExprDBType[ sCodeConst->mOffset ].mDBType,
                               aExprInfo->mExprDBType[ sCodeConst->mOffset ].mArgNum1,
                               aExprInfo->mExprDBType[ sCodeConst->mOffset ].mArgNum2,
                               aExprInfo->mExprDBType[ sCodeConst->mOffset ].mStringLengthUnit,
                               aExprInfo->mExprDBType[ sCodeConst->mOffset ].mIntervalIndicator,
                               aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;
    
    STL_FINISH;

    if ( aSQLString != NULL )
    {
        stlChar * sLastError = NULL;
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
        if ( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                          qlgMakeErrPosString( aSQLString,
                                                               aInitExprCode->mPosition,
                                                               aEnv ) );
        }
    }
    
    return STL_FAILURE;   
}


/**
 * @brief REFERENCE EXPRESSION 의 Code Area를 구성한다.
 * @param[in]     aSQLString          SQL String
 * @param[in]     aInitExprCode       Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext        Bind Context
 * @param[in,out] aExeExprCodeStack   Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo       Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId      현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[out]    aExprEntry          추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloCodeOptimizeReference( stlChar            * aSQLString,
                                    qlvInitExpression  * aInitExprCode,
                                    knlBindContext     * aBindContext,
                                    knlExprStack       * aExeExprCodeStack,
                                    knlExprStack       * aConstExprCodeStack,
                                    stlUInt32            aGroupEntryNo,
                                    knlBuiltInFunc       aRelatedFuncId,
                                    qloExprInfo        * aExprInfo,
                                    knlExprEntry      ** aExprEntry,
                                    knlRegionMem       * aRegionMem,
                                    qllEnv             * aEnv )
{
    knlExprStack        * sCodeStack = aExeExprCodeStack;
    qlvInitExpression   * sCodeExpr  = NULL;
    dtlBuiltInFuncInfo  * sFuncInfo  = NULL;

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprCode->mType == QLV_EXPR_TYPE_REFERENCE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Expression Code Stack에 value entry 추가
     */

    *aExprEntry = & sCodeStack->mEntries[ sCodeStack->mEntryCount ];
    sFuncInfo   = & gBuiltInFuncInfoArr[ aRelatedFuncId ];
        
    STL_TRY( knlExprAddValue( (knlExprStack *) sCodeStack,
                              aGroupEntryNo,
                              aInitExprCode->mOffset,
                              sFuncInfo->mExceptionType,
                              sFuncInfo->mExceptionResult,
                              aGroupEntryNo - 1,
                              aRegionMem,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * DB Type 정보 설정
     * ---------------------------
     *  Reference Expression의 경우 하위 Sub Query의 Target 또는 List Function을
     *  최종적으로 참조하게 된다.
     */

    STL_PARAM_VALIDATE( aInitExprCode->mExpr.mReference != NULL,
                        QLL_ERROR_STACK(aEnv) );

    sCodeExpr = aInitExprCode->mExpr.mReference->mOrgExpr;
    
 
    /**
     * DB Type 정보 설정
     */

    STL_TRY( qloSetExprDBType( aInitExprCode,
                               aExprInfo,
                               aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mDBType,
                               aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mArgNum1,
                               aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mArgNum2,
                               aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mStringLengthUnit,
                               aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mIntervalIndicator,
                               aEnv )
             == STL_SUCCESS );

    
    return STL_SUCCESS;
    
    STL_FINISH;

    if ( aSQLString != NULL )
    {
        stlChar * sLastError = NULL;
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
        if ( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                          qlgMakeErrPosString( aSQLString,
                                                               aInitExprCode->mPosition,
                                                               aEnv ) );
        }
    }
    
    return STL_FAILURE;
}


/**
 * @brief SUB-QUERY 의 Code Area를 구성한다.
 * @param[in]     aSQLString          SQL String
 * @param[in]     aInitSubQueryCode   Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext        Bind Context
 * @param[in,out] aExeExprCodeStack   Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo       Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId      현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[out]    aExprEntry          추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloCodeOptimizeSubQueryExpr( stlChar            * aSQLString,
                                       qlvInitExpression  * aInitSubQueryCode,
                                       knlBindContext     * aBindContext,
                                       knlExprStack       * aExeExprCodeStack,
                                       knlExprStack       * aConstExprCodeStack,
                                       stlUInt32            aGroupEntryNo,
                                       knlBuiltInFunc       aRelatedFuncId,
                                       qloExprInfo        * aExprInfo,
                                       knlExprEntry      ** aExprEntry,
                                       knlRegionMem       * aRegionMem,
                                       qllEnv             * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitSubQueryCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitSubQueryCode->mType == QLV_EXPR_TYPE_SUB_QUERY,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExeExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExeExprCodeStack->mMaxEntryCount > aExeExprCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Sub Query를 외부 참조시 Sub Query의 Target만을 참조하기 때문에
     * Sub Query 자체에 대한 DB Type 설정이나 Stack의 Entry 구성은 허용하지 않는다.
     */
    
    /* Do Nothing */

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;   
}


/**
 * @brief INNER COLUMN EXPRESSION 의 Code Area를 구성한다.
 * @param[in]     aSQLString          SQL String
 * @param[in]     aInitExprCode       Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext         Bind Context
 * @param[in,out] aExeExprCodeStack   Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo       Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId      현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[out]    aExprEntry          추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloCodeOptimizeInnerColumn( stlChar            * aSQLString,
                                      qlvInitExpression  * aInitExprCode,
                                      knlBindContext     * aBindContext,
                                      knlExprStack       * aExeExprCodeStack,
                                      knlExprStack       * aConstExprCodeStack,
                                      stlUInt32            aGroupEntryNo,
                                      knlBuiltInFunc       aRelatedFuncId,
                                      qloExprInfo        * aExprInfo,
                                      knlExprEntry      ** aExprEntry,
                                      knlRegionMem       * aRegionMem,
                                      qllEnv             * aEnv )
{
    knlExprStack        * sCodeStack    = aExeExprCodeStack;
    dtlBuiltInFuncInfo  * sFuncInfo     = NULL;

    qlvInitDataTypeInfo   sDataTypeInfo;

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprCode->mType == QLV_EXPR_TYPE_INNER_COLUMN,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Expression Code Stack에 value entry 추가
     */

    *aExprEntry = & sCodeStack->mEntries[ sCodeStack->mEntryCount ];
    sFuncInfo   = & gBuiltInFuncInfoArr[ aRelatedFuncId ];
        
    STL_TRY( knlExprAddValue( (knlExprStack *) sCodeStack,
                              aGroupEntryNo,
                              aInitExprCode->mOffset,
                              sFuncInfo->mExceptionType,
                              sFuncInfo->mExceptionResult,
                              aGroupEntryNo - 1,
                              aRegionMem,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * DB Type 정보 설정
     * ---------------------------
     *  Inner Column의 경우 하위 Base Table 또는 Sub Table을
     *  최종적으로 참조하게 되는데, 이 Column에 대한 DB Type 정보가
     *  아직 생성되지 않았을 수 있으므로 실제 DB Type 정보를
     *  읽어서 설정해준다.
     */

    STL_PARAM_VALIDATE( aInitExprCode->mExpr.mInnerColumn != NULL,
                        QLL_ERROR_STACK(aEnv) );

    STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                     aRegionMem,
                                     aInitExprCode,
                                     aBindContext,
                                     & sDataTypeInfo,
                                     aEnv )
             == STL_SUCCESS );

    STL_TRY( qloSetExprDBType( aInitExprCode,
                               aExprInfo,
                               sDataTypeInfo.mDataType,
                               sDataTypeInfo.mArgNum1,
                               sDataTypeInfo.mArgNum2,
                               sDataTypeInfo.mStringLengthUnit,
                               sDataTypeInfo.mIntervalIndicator,
                               aEnv )
             == STL_SUCCESS );
    
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if ( aSQLString != NULL )
    {
        stlChar * sLastError = NULL;
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
        if ( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                          qlgMakeErrPosString( aSQLString,
                                                               aInitExprCode->mPosition,
                                                               aEnv ) );
        }
    }
    
    return STL_FAILURE;
}


/**
 * @brief OUTER COLUMN EXPRESSION 의 Code Area를 구성한다.
 * @param[in]     aSQLString          SQL String
 * @param[in]     aInitExprCode       Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext        Bind Context
 * @param[in,out] aExeExprCodeStack   Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo       Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId      현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[out]    aExprEntry          추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloCodeOptimizeOuterColumn( stlChar            * aSQLString,
                                      qlvInitExpression  * aInitExprCode,
                                      knlBindContext     * aBindContext,
                                      knlExprStack       * aExeExprCodeStack,
                                      knlExprStack       * aConstExprCodeStack,
                                      stlUInt32            aGroupEntryNo,
                                      knlBuiltInFunc       aRelatedFuncId,
                                      qloExprInfo        * aExprInfo,
                                      knlExprEntry      ** aExprEntry,
                                      knlRegionMem       * aRegionMem,
                                      qllEnv             * aEnv )
{
    knlExprStack        * sCodeStack   = aExeExprCodeStack;
    qlvInitExpression   * sCodeExpr    = NULL;
    dtlBuiltInFuncInfo  * sFuncInfo    = NULL;
    qlvInitColumn       * sInitColumn  = NULL;
        
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprCode->mType == QLV_EXPR_TYPE_OUTER_COLUMN,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Expression Code Stack에 value entry 추가
     */

    *aExprEntry = & sCodeStack->mEntries[ sCodeStack->mEntryCount ];
    sFuncInfo   = & gBuiltInFuncInfoArr[ aRelatedFuncId ];
        
    STL_TRY( knlExprAddValue( (knlExprStack *) sCodeStack,
                              aGroupEntryNo,
                              aInitExprCode->mOffset,
                              sFuncInfo->mExceptionType,
                              sFuncInfo->mExceptionResult,
                              aGroupEntryNo - 1,
                              aRegionMem,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * DB Type 정보 설정
     * ---------------------------
     *  Outer Column의 경우 하위 Base Table 또는 Sub Table을
     *  최종적으로 참조하게 되는데, 이 Column에 대한 DB Type 정보가
     *  아직 생성되지 않았을 수 있으므로 실제 DB Type 정보를
     *  읽어서 설정해준다.
     */

    STL_PARAM_VALIDATE( aInitExprCode->mExpr.mOuterColumn != NULL,
                        QLL_ERROR_STACK(aEnv) );

    sCodeExpr = aInitExprCode->mExpr.mOuterColumn->mOrgExpr;

    
    if( aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mIsNeedCast == STL_TRUE )
    {
        STL_TRY( qloSetExprDBType( aInitExprCode,
                                   aExprInfo,
                                   aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mDBType,
                                   aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mArgNum1,
                                   aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mArgNum2,
                                   aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mStringLengthUnit,
                                   aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );
    }
    else if( sCodeExpr->mType == QLV_EXPR_TYPE_COLUMN )
    {
        sInitColumn = sCodeExpr->mExpr.mColumn;
        
        STL_TRY( qloSetExprDBType( aInitExprCode,
                                   aExprInfo,
                                   ellGetColumnDBType( sInitColumn->mColumnHandle ),
                                   ellGetColumnPrecision( sInitColumn->mColumnHandle ),
                                   ellGetColumnScale( sInitColumn->mColumnHandle ),
                                   ellGetColumnStringLengthUnit( sInitColumn->mColumnHandle ),
                                   ellGetColumnIntervalIndicator( sInitColumn->mColumnHandle ),
                                   aEnv )
                 == STL_SUCCESS );
    }
    else if( sCodeExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN )
    {
        STL_TRY( qloSetExprDBType( aInitExprCode,
                                   aExprInfo,
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
        STL_TRY( qloSetExprDBType( aInitExprCode,
                                   aExprInfo,
                                   aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mDBType,
                                   aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mArgNum1,
                                   aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mArgNum2,
                                   aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mStringLengthUnit,
                                   aExprInfo->mExprDBType[ sCodeExpr->mOffset ].mIntervalIndicator,
                                   aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;
    
    STL_FINISH;

    if ( aSQLString != NULL )
    {
        stlChar * sLastError = NULL;
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
        if ( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                          qlgMakeErrPosString( aSQLString,
                                                               aInitExprCode->mPosition,
                                                               aEnv ) );
        }
    }
    
    return STL_FAILURE;
}

/**
 * @brief ROWID COLUMN 의 Code Area를 구성한다.
 * @param[in]     aSQLString           SQL String
 * @param[in]     aInitRowIdColumnCode Parse/Validation 단계의 Code Expression
 * @param[in,out] aExeExprCodeStack    Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack  Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo        Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId       현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo            All Expression's Information
 * @param[out]    aExprEntry           추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem           Region Memory
 * @param[in]     aEnv                 Environment
 */
stlStatus qloCodeOptimizeRowIdColumn( stlChar            * aSQLString,
                                      qlvInitExpression  * aInitRowIdColumnCode,
                                      knlExprStack       * aExeExprCodeStack,
                                      knlExprStack       * aConstExprCodeStack,
                                      stlUInt32            aGroupEntryNo,
                                      knlBuiltInFunc       aRelatedFuncId,
                                      qloExprInfo        * aExprInfo,
                                      knlExprEntry      ** aExprEntry,
                                      knlRegionMem       * aRegionMem,
                                      qllEnv             * aEnv )
{
    knlExprStack        * sCodeStack = aExeExprCodeStack;
    dtlBuiltInFuncInfo  * sFuncInfo  = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitRowIdColumnCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitRowIdColumnCode->mType == QLV_EXPR_TYPE_ROWID_COLUMN,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
            

    /**
     * Expression Code Stack에 value entry 추가
     */

    *aExprEntry = & sCodeStack->mEntries[ sCodeStack->mEntryCount ];
    sFuncInfo   = & gBuiltInFuncInfoArr[ aRelatedFuncId ];
        
    STL_TRY( knlExprAddValue( (knlExprStack *) sCodeStack,
                              aGroupEntryNo,
                              aInitRowIdColumnCode->mOffset,
                              sFuncInfo->mExceptionType,
                              sFuncInfo->mExceptionResult,
                              aGroupEntryNo - 1,
                              aRegionMem,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    
    /**
     * DB Type 정보 설정
     */

    STL_TRY( qloSetExprDBType( aInitRowIdColumnCode,
                               aExprInfo,
                               DTL_TYPE_ROWID,
                               gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mArgNum1,
                               gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mArgNum2,
                               gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mStringLengthUnit,
                               gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mIntervalIndicator,
                               aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    if ( aSQLString != NULL )
    {
        stlChar * sLastError = NULL;
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
        if ( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                          qlgMakeErrPosString( aSQLString,
                                                               aInitRowIdColumnCode->mPosition,
                                                               aEnv ) );
        }
    }

    return STL_FAILURE;
}

/**
 * @brief AGGREGATION 의 Code Area를 구성한다.
 * @param[in]     aSQLString          SQL String 
 * @param[in]     aInitAggrCode       Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext        Bind Context
 * @param[in,out] aExeExprCodeStack   Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo       Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId      현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[out]    aExprEntry          추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloCodeOptimizeAggregation( stlChar            * aSQLString,
                                      qlvInitExpression  * aInitAggrCode,
                                      knlBindContext     * aBindContext,
                                      knlExprStack       * aExeExprCodeStack,
                                      knlExprStack       * aConstExprCodeStack,
                                      stlUInt32            aGroupEntryNo,
                                      knlBuiltInFunc       aRelatedFuncId,
                                      qloExprInfo        * aExprInfo,
                                      knlExprEntry      ** aExprEntry,
                                      knlRegionMem       * aRegionMem,
                                      qllEnv             * aEnv )
{
    knlExprStack        * sCodeStack     = aExeExprCodeStack;
    dtlBuiltInFuncInfo  * sFuncInfo      = NULL;
    qlvInitAggregation  * sAggrExpr      = NULL;

    dtlDataType           sDataType      = DTL_TYPE_NA;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitAggrCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitAggrCode->mType == QLV_EXPR_TYPE_AGGREGATION,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( sCodeStack->mMaxEntryCount > sCodeStack->mEntryCount,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
            

    /**
     * Expression Code Stack에 value entry 추가
     */

    *aExprEntry = & sCodeStack->mEntries[ sCodeStack->mEntryCount ];
    sFuncInfo   = & gBuiltInFuncInfoArr[ aRelatedFuncId ];
        
    STL_TRY( knlExprAddValue( (knlExprStack *) sCodeStack,
                              aGroupEntryNo,
                              aInitAggrCode->mOffset,
                              sFuncInfo->mExceptionType,
                              sFuncInfo->mExceptionResult,
                              aGroupEntryNo - 1,
                              aRegionMem,
                              KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * Set DB Type
     */

    /**
     * @remark Aggregation에 대한 Expression Stack 구성은 Complete 단계에서 별도로 수행한다.
     */
    
    STL_TRY( qloGetDataType( aSQLString,
                             aInitAggrCode,
                             aBindContext,
                             & sDataType,
                             aRegionMem,
                             aEnv )
             == STL_SUCCESS );
    
    /**
     * DB Type 설정
     */

    sAggrExpr = aInitAggrCode->mExpr.mAggregation;
    
    STL_TRY( gQlnfAggrFuncInfoArr[ sAggrExpr->mAggrId ].mSetDBTypeFuncPtr(
                 aInitAggrCode,
                 sDataType,
                 aExprInfo,
                 aEnv )
             == STL_SUCCESS );

    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    if ( aSQLString != NULL )
    {
        stlChar * sLastError = NULL;
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
        if ( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                          qlgMakeErrPosString( aSQLString,
                                                               aInitAggrCode->mPosition,
                                                               aEnv ) );
        }
    }
    
    return STL_FAILURE;
}


/*******************************************************************************
 * Expression : Common
 ******************************************************************************/

/**
 * @brief Exression Stack Entry를 구성한다.
 * @param[in]     aSQLString           SQL String
 * @param[in]     aInitExprCode        Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext         Bind Context
 * @param[in,out] aExeCodeExpr         Optimization 적용한 Code Expression
 * @param[in,out] aConstExprCodeStack  Optimization 적용한 Constant Expression
 * @param[in]     aGroupEntryNo        Code Stack의 Group Entry 번호
 * @param[in]     aRelatedFuncId       현재 Expr의 상위 Expr에 대한 Function ID
 * @param[in,out] aExprInfo            All Expression's Information
 * @param[out]    aExprEntry           추가된 Expresson Stack의 Entry
 * @param[in]     aRegionMem           Region Memory
 * @param[in]     aEnv                 Environment
 *
 * @remark Expression에 대한 Optimize 초기 수행시 (본 함수 처음 호출시)
 *    <BR> aGroupEntryNo의 값을 "QLL_INIT_GROUP_ENTRY_NO" 로 주어야 한다.
 */
stlStatus qloAddExprStackEntry( stlChar             * aSQLString,
                                qlvInitExpression   * aInitExprCode,
                                knlBindContext      * aBindContext,
                                knlExprStack        * aExeCodeExpr,
                                knlExprStack        * aConstExprCodeStack,
                                stlUInt32             aGroupEntryNo,
                                knlBuiltInFunc        aRelatedFuncId,
                                qloExprInfo         * aExprInfo,
                                knlExprEntry       ** aExprEntry,
                                knlRegionMem        * aRegionMem,
                                qllEnv              * aEnv )
{
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExeCodeExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConstExprCodeStack != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGroupEntryNo >= QLL_INIT_GROUP_ENTRY_NO,
                        QLL_ERROR_STACK(aEnv) );    
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Stack의 Entry 구성 정보 초기화
     */

    switch( aInitExprCode->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
            {
                STL_TRY( qloCodeOptimizeValue( aSQLString,
                                               aInitExprCode,
                                               aExeCodeExpr,
                                               aConstExprCodeStack,
                                               aGroupEntryNo,
                                               aRelatedFuncId,
                                               aExprInfo,
                                               aExprEntry,
                                               aRegionMem,
                                               aEnv )
                         == STL_SUCCESS );      
                
                break;
            }
            
        case QLV_EXPR_TYPE_BIND_PARAM :
            {
                STL_TRY( qloCodeOptimizeBindParam( aSQLString,
                                                   aInitExprCode,
                                                   aBindContext,
                                                   aExeCodeExpr,
                                                   aConstExprCodeStack,
                                                   aGroupEntryNo,
                                                   aRelatedFuncId,
                                                   aExprInfo,
                                                   aExprEntry,
                                                   aRegionMem,
                                                   aEnv )
                         == STL_SUCCESS );
                
                break;
            }
            
        case QLV_EXPR_TYPE_COLUMN :
            {
                STL_TRY( qloCodeOptimizeColumn( aSQLString,
                                                aInitExprCode,
                                                aExeCodeExpr,
                                                aConstExprCodeStack,
                                                aGroupEntryNo,
                                                aRelatedFuncId,
                                                aExprInfo,
                                                aExprEntry,
                                                aRegionMem,
                                                aEnv )
                         == STL_SUCCESS );      
                
                break;
            }
            
        case QLV_EXPR_TYPE_FUNCTION :
            {
                STL_TRY( qloCodeOptimizeFunction( aSQLString,
                                                  aInitExprCode,
                                                  aBindContext,
                                                  aExeCodeExpr,
                                                  aConstExprCodeStack,
                                                  aGroupEntryNo,
                                                  aRelatedFuncId,
                                                  aExprInfo,
                                                  aExprEntry,
                                                  aRegionMem,
                                                  aEnv )
                         == STL_SUCCESS );      

                break;
            }
            
        case QLV_EXPR_TYPE_CAST :
            {
                STL_TRY( qloCodeOptimizeCast( aSQLString,
                                              aInitExprCode,
                                              aBindContext,
                                              aExeCodeExpr,
                                              aConstExprCodeStack,
                                              aGroupEntryNo,
                                              aRelatedFuncId,
                                              aExprInfo,
                                              aExprEntry,
                                              aRegionMem,
                                              aEnv )
                         == STL_SUCCESS );      
                
                break;
            }

        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
            {
                STL_TRY( qloCodeOptimizePseudoCol( aSQLString,
                                                   aInitExprCode,
                                                   aExeCodeExpr,
                                                   aConstExprCodeStack,
                                                   aGroupEntryNo,
                                                   aRelatedFuncId,
                                                   aExprInfo,
                                                   aExprEntry,
                                                   aRegionMem,
                                                   aEnv )
                         == STL_SUCCESS );      
                
                break;
            }

        case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
            {
                STL_TRY( qloCodeOptimizeConstExpr( aSQLString,
                                                   aInitExprCode,
                                                   aBindContext,
                                                   aExeCodeExpr,
                                                   aConstExprCodeStack,
                                                   aGroupEntryNo,
                                                   aRelatedFuncId,
                                                   aExprInfo,
                                                   aExprEntry,
                                                   aRegionMem,
                                                   aEnv )
                         == STL_SUCCESS );      
                
                break;
            }

        case QLV_EXPR_TYPE_REFERENCE :
            {
                STL_TRY( qloCodeOptimizeReference( aSQLString,
                                                   aInitExprCode,
                                                   aBindContext,
                                                   aExeCodeExpr,
                                                   aConstExprCodeStack,
                                                   aGroupEntryNo,
                                                   aRelatedFuncId,
                                                   aExprInfo,
                                                   aExprEntry,
                                                   aRegionMem,
                                                   aEnv )
                         == STL_SUCCESS );      
                
                break;
            }

        case QLV_EXPR_TYPE_SUB_QUERY :
            {
                STL_TRY( qloCodeOptimizeSubQueryExpr( aSQLString,
                                                      aInitExprCode,
                                                      aBindContext,
                                                      aExeCodeExpr,
                                                      aConstExprCodeStack,
                                                      aGroupEntryNo,
                                                      aRelatedFuncId,
                                                      aExprInfo,
                                                      aExprEntry,
                                                      aRegionMem,
                                                      aEnv )
                         == STL_SUCCESS );      
                
                break;
            }

        case QLV_EXPR_TYPE_INNER_COLUMN :
            {
                STL_TRY( qloCodeOptimizeInnerColumn( aSQLString,
                                                     aInitExprCode,
                                                     aBindContext,
                                                     aExeCodeExpr,
                                                     aConstExprCodeStack,
                                                     aGroupEntryNo,
                                                     aRelatedFuncId,
                                                     aExprInfo,
                                                     aExprEntry,
                                                     aRegionMem,
                                                     aEnv )
                         == STL_SUCCESS );      
                
                break;
            }

        case QLV_EXPR_TYPE_OUTER_COLUMN :
            {
                STL_TRY( qloCodeOptimizeOuterColumn( aSQLString,
                                                     aInitExprCode,
                                                     aBindContext,
                                                     aExeCodeExpr,
                                                     aConstExprCodeStack,
                                                     aGroupEntryNo,
                                                     aRelatedFuncId,
                                                     aExprInfo,
                                                     aExprEntry,
                                                     aRegionMem,
                                                     aEnv )
                         == STL_SUCCESS );      
                
                break;
            }
        case QLV_EXPR_TYPE_ROWID_COLUMN :
            {
                STL_TRY( qloCodeOptimizeRowIdColumn( aSQLString,
                                                     aInitExprCode,
                                                     aExeCodeExpr,
                                                     aConstExprCodeStack,
                                                     aGroupEntryNo,
                                                     aRelatedFuncId,
                                                     aExprInfo,
                                                     aExprEntry,
                                                     aRegionMem,
                                                     aEnv )
                         == STL_SUCCESS );
                
                break;
            }
        case QLV_EXPR_TYPE_AGGREGATION :
            {
                STL_TRY( qloCodeOptimizeAggregation( aSQLString,
                                                     aInitExprCode,
                                                     aBindContext,
                                                     aExeCodeExpr,
                                                     aConstExprCodeStack,
                                                     aGroupEntryNo,
                                                     aRelatedFuncId,
                                                     aExprInfo,
                                                     aExprEntry,
                                                     aRegionMem,
                                                     aEnv )
                         == STL_SUCCESS );      

                break;
            }
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                STL_TRY( qloCodeOptimizeCaseExpr( aSQLString,
                                                  aInitExprCode,
                                                  aBindContext,
                                                  aExeCodeExpr,
                                                  aConstExprCodeStack,
                                                  aGroupEntryNo,
                                                  aRelatedFuncId,
                                                  aExprInfo,
                                                  aExprEntry,
                                                  aRegionMem,
                                                  aEnv )
                         == STL_SUCCESS );      
                
                break;
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                STL_TRY( qloCodeOptimizeListFunction( aSQLString,
                                                      aInitExprCode,
                                                      aBindContext,
                                                      aExeCodeExpr,
                                                      aConstExprCodeStack,
                                                      aGroupEntryNo,
                                                      aRelatedFuncId,
                                                      aExprInfo,
                                                      aExprEntry,
                                                      aRegionMem,
                                                      aEnv )
                         == STL_SUCCESS );

                break;
            }
        case QLV_EXPR_TYPE_LIST_COLUMN :
            {
                STL_TRY( qloCodeOptimizeListColumn( aSQLString,
                                                    aInitExprCode,
                                                    aBindContext,
                                                    aExeCodeExpr,
                                                    aConstExprCodeStack,
                                                    aGroupEntryNo,
                                                    aRelatedFuncId,
                                                    aExprInfo,
                                                    aExprEntry,
                                                    aRegionMem,
                                                    aEnv )
                         == STL_SUCCESS );

                 break;
            }
        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                STL_TRY( qloCodeOptimizeRowExpr( aSQLString,
                                                 aInitExprCode,
                                                 aBindContext,
                                                 aExeCodeExpr,
                                                 aConstExprCodeStack,
                                                 aGroupEntryNo,
                                                 aRelatedFuncId,
                                                 aExprInfo,
                                                 aExprEntry,
                                                 aRegionMem,
                                                 aEnv )
                         == STL_SUCCESS );
                
                break;
            }
            
        case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
        default :
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
 * @brief String으로 부터 dtlDataValue를 생성한다.
 * @param[in]     aSQLStmt            SQL statement
 * @param[out]    aDataValue          dtlDataValue
 * @param[in]     aDataType           DB data type
 * @param[in]     aPrecision          precision
 * @param[in]     aScale              scale
 * @param[in]     aStringLengthUnit   string length unit
 * @param[in]     aIntervalIndicator  interval indicator
 * @param[in]     aString             입력 문자열
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloSetDataValueFromString( qllStatement         * aSQLStmt,
                                     dtlDataValue        ** aDataValue,
                                     dtlDataType            aDataType,
                                     stlInt64               aPrecision,
                                     stlInt64               aScale,
                                     dtlStringLengthUnit    aStringLengthUnit,
                                     dtlIntervalIndicator   aIntervalIndicator,
                                     stlChar              * aString,
                                     knlRegionMem         * aRegionMem,
                                     qllEnv               * aEnv )
{
    dtlDataValue  * sDataValue       = NULL;
    stlInt64        sLength          = 0;
    stlBool         sSuccessWithInfo = STL_FALSE;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aDataType >= 0 ) && ( aDataType < DTL_TYPE_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Data Value 공간 할당
     */
                    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( dtlDataValue ),
                                (void **) & sDataValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sDataValue, 0x00, STL_SIZEOF( dtlDataValue ) );


    /**
     * Data Value 값 설정
     */
    
    if( aString[0] == '\0' )
    {
        STL_TRY( qlndInitSingleDataValue( & aSQLStmt->mLongTypeValueList,
                                          aDataType,
                                          sDataValue,
                                          0,
                                          aRegionMem,
                                          aEnv )
                 == STL_SUCCESS );
    }
    else 
    {
        /**
         * Data Value의 mValue 공간 할당
         */

        STL_TRY( qlndAllocDataValueArrayFromString( & aSQLStmt->mLongTypeValueList,
                                                    sDataValue,
                                                    aDataType,
                                                    aPrecision,
                                                    aScale,
                                                    aStringLengthUnit,
                                                    aString,
                                                    stlStrlen(aString),
                                                    1,
                                                    aRegionMem,
                                                    & sLength,
                                                    aEnv )
                 == STL_SUCCESS );

        STL_DASSERT( sLength > 0 );

        
        /**
         * String으로부터 Data Value 설정
         */

        STL_TRY( dtlSetValueFromString( aDataType,
                                        aString,
                                        stlStrlen(aString),
                                        aPrecision,
                                        aScale,
                                        aStringLengthUnit,
                                        aIntervalIndicator,
                                        sLength,
                                        sDataValue,
                                        & sSuccessWithInfo,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        QLL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
    }


    /**
     * OUTPUT 설정
     */
    
    *aDataValue = sDataValue;
    

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief DateTime Literal 로 부터 dtlDataValue를 생성한다.
 *   <BR> ex) DATE'1999-01-01' 
 * @param[in]     aSQLStmt            SQL statement
 * @param[out]    aDataValue          dtlDataValue
 * @param[in]     aDataType           DB data type
 * @param[in]     aPrecision          precision
 * @param[in]     aScale              scale
 * @param[in]     aStringLengthUnit   string length unit
 * @param[in]     aIntervalIndicator  interval indicator
 * @param[in]     aString             입력 문자열
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloSetDataValueFromDateTimeLiteral( qllStatement         * aSQLStmt,
                                              dtlDataValue        ** aDataValue,
                                              dtlDataType            aDataType,
                                              stlInt64               aPrecision,
                                              stlInt64               aScale,
                                              dtlStringLengthUnit    aStringLengthUnit,
                                              dtlIntervalIndicator   aIntervalIndicator,
                                              stlChar              * aString,
                                              knlRegionMem         * aRegionMem,
                                              qllEnv               * aEnv )
{
    dtlDataValue  * sDataValue       = NULL;
    stlInt64        sLength          = 0;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aDataType == DTL_TYPE_DATE ) ||
                        ( aDataType == DTL_TYPE_TIME ) ||
                        ( aDataType == DTL_TYPE_TIME_WITH_TIME_ZONE ) ||
                        ( aDataType == DTL_TYPE_TIMESTAMP ) ||
                        ( aDataType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ),    
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Data Value 공간 할당
     */
                    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( dtlDataValue ),
                                (void **) & sDataValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Data Value 값 설정
     */

    STL_DASSERT( aString[0] != '\0' );

    STL_TRY( qlndAllocDataValueArrayFromString( & aSQLStmt->mLongTypeValueList,
                                                sDataValue,
                                                aDataType,
                                                aPrecision,
                                                aScale,
                                                aStringLengthUnit,
                                                aString,
                                                stlStrlen(aString),
                                                1,
                                                aRegionMem,
                                                & sLength,
                                                aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sLength > 0 );

        
    /**
     * Datetime literal 로부터 Data Value 설정
     */
    
    STL_TRY( dtlSetDateTimeValueFromTypedLiteral( aDataType,
                                                  aString,
                                                  stlStrlen(aString),
                                                  sLength,
                                                  sDataValue,
                                                  KNL_DT_VECTOR(aEnv),
                                                  aEnv,
                                                  QLL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    /**
     * OUTPUT 설정
     */
    
    *aDataValue = sDataValue;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Integer로 부터 dtlDataValue를 생성한다.
 * @param[in]     aSQLStmt            SQL statement
 * @param[out]    aDataValue          dtlDataValue
 * @param[in]     aDataType           DB data type
 * @param[in]     aPrecision          precision
 * @param[in]     aScale              scale
 * @param[in]     aInteger            입력 숫자
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloSetDataValueFromInteger( qllStatement   * aSQLStmt,
                                      dtlDataValue  ** aDataValue,
                                      dtlDataType      aDataType,
                                      stlInt64         aPrecision,
                                      stlInt64         aScale,
                                      stlInt64         aInteger,
                                      knlRegionMem   * aRegionMem,
                                      qllEnv         * aEnv )
{
    dtlDataValue  * sDataValue       = NULL;
    stlInt64        sLength          = 0;
    stlBool         sSuccessWithInfo = STL_FALSE;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aDataType >= 0 ) && ( aDataType < DTL_TYPE_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    
    
    /**
     * Data Value 공간 할당
     */
                    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( dtlDataValue ),
                                (void **) & sDataValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Data Value의 mValue 공간 할당
     */

    STL_TRY( qlndAllocDataValueArray( & aSQLStmt->mLongTypeValueList,
                                      sDataValue,
                                      aDataType,
                                      aPrecision,
                                      DTL_STRING_LENGTH_UNIT_NA,
                                      1,
                                      aRegionMem,
                                      & sLength,
                                      aEnv )
             == STL_SUCCESS );

        
    /**
     * String으로부터 Data Value 설정
     */

    STL_TRY( dtlSetValueFromInteger( aDataType,
                                     aInteger,
                                     aPrecision,
                                     aScale,
                                     DTL_STRING_LENGTH_UNIT_NA,
                                     DTL_INTERVAL_INDICATOR_NA,
                                     sLength,
                                     sDataValue,
                                     & sSuccessWithInfo,
                                     KNL_DT_VECTOR(aEnv),
                                     aEnv,
                                     QLL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );  


    /**
     * OUTPUT 설정
     */
    
    *aDataValue = sDataValue;
    

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief String으로 부터 dtlDataValue를 생성한다.
 * @param[out]    aDataValue          dtlDataValue
 * @param[in]     aDataType           DB data type
 * @param[in]     aPrecision          Precision
 * @param[in]     aStringLengthUnit   String Length Unit
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 */
stlStatus qloSetNullDataValue( dtlDataValue         ** aDataValue,
                               dtlDataType             aDataType,
                               stlInt64                aPrecision,
                               dtlStringLengthUnit     aStringLengthUnit,
                               knlRegionMem          * aRegionMem,
                               qllEnv                * aEnv )
{
    dtlDataValue  * sDataValue = NULL;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( ( aDataType >= 0 ) && ( aDataType < DTL_TYPE_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    
    
    /**
     * Data Value 공간 할당
     */
                    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( dtlDataValue ),
                                (void **) & sDataValue,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Data Value 초기화
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                1,
                                & sDataValue->mValue,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY( dtlInitDataValue( aDataType,
                               1,
                               sDataValue,
                               QLL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    
    /**
     * OUTPUT 설정
     */
    
    *aDataValue = sDataValue;
    

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*******************************************************************************
 * Expression : DB type
 ******************************************************************************/

/**
 * @brief Expression DB Type 구조체를 생성한다.
 * @param[in]     aListCount        Code Expression List 개수 
 * @param[out]    aExprInfo         All Expression's Information
 * @param[in]     aRegionMem        Region Memory
 * @param[in]     aEnv              Environment
 */
stlStatus qloCreateExprInfo( stlInt32            aListCount,
                             qloExprInfo      ** aExprInfo,
                             knlRegionMem      * aRegionMem,
                             qllEnv            * aEnv )
{
    qloExprInfo      * sExprInfo  = NULL;
    qloDBType        * sDBTypes   = NULL;
    qloFunctionInfo  * sExprFunc  = NULL;
    qloListFuncInfo  * sListFunc = NULL;
    
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aListCount >= 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Expression DB type 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qloExprInfo ),
                                (void **) & sExprInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sExprInfo,
               0x00,
               STL_SIZEOF( qloExprInfo ) );

    STL_TRY_THROW( aListCount > 0, RAMP_FINISH );


    /**
     * DB type Array 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qloDBType ) * aListCount,
                                (void **) & sDBTypes,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sDBTypes,
               0x00,
               STL_SIZEOF( qloDBType ) * aListCount );


    /**
     * Function Array 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qloFunctionInfo ) * aListCount,
                                (void **) & sExprFunc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sExprFunc,
               0x00,
               STL_SIZEOF( qloFunctionInfo ) * aListCount );

    /**
     * Function Array 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qloListFuncInfo ) * aListCount,
                                (void **) & sListFunc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sListFunc,
               0x00,
               STL_SIZEOF( qloListFuncInfo ) * aListCount );

    
    /**
     * Expression DB type 설정
     */
    
    sExprInfo->mExprCnt    = aListCount;
    sExprInfo->mExprDBType = sDBTypes;
    sExprInfo->mExprFunc   = sExprFunc;
    sExprInfo->mListFunc   = sListFunc;

    STL_RAMP( RAMP_FINISH );

    /**
     * Output 설정
     */

    *aExprInfo = sExprInfo;
    

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief expression의 DB Type을 설정한다.
 * @param[in]     aInitExprCode       Parse/Validation 단계의 Code Expression List
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[in]     aDBType             DB Type
 * @param[in]     aPrecision          Precision (TYPE 마다 용도가 다름)
 * @param[in]     aScale              Scale (TYPE 마다 용도가 다름)
 * @param[in]     aStringLengthUnit   String Length Unit
 * @param[in]     aIntervalIndicator  Interval Indicator
 * @param[in]     aEnv                Environment
 *
 * @remark 해당 Entry에 현재 expression이 cast 대상이 아님을 설정한다.
 */
stlStatus qloSetExprDBType( qlvInitExpression     * aInitExprCode,
                            qloExprInfo           * aExprInfo,
                            dtlDataType             aDBType,
                            stlInt64                aPrecision,
                            stlInt64                aScale,
                            dtlStringLengthUnit     aStringLengthUnit,
                            dtlIntervalIndicator    aIntervalIndicator,
                            qllEnv                * aEnv )
{
    qloDBType        * sDBType = NULL;

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprCode->mOffset < aExprInfo->mExprCnt,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * DB Type 설정
     */
    
    sDBType = & aExprInfo->mExprDBType[ aInitExprCode->mOffset ];

    sDBType->mDBType                  = aDBType;
    sDBType->mIsNeedCast              = STL_FALSE;
    sDBType->mCastFuncIdx             = 0;
    
    sDBType->mSourceDBType            = aDBType;
    sDBType->mSourceArgNum1           = aPrecision;
    sDBType->mSourceArgNum2           = aScale;
    sDBType->mSourceStringLengthUnit  = aStringLengthUnit;
    sDBType->mSourceIntervalIndicator = aIntervalIndicator;
    
    sDBType->mArgNum1                 = aPrecision;
    sDBType->mArgNum2                 = aScale;
    sDBType->mStringLengthUnit        = aStringLengthUnit;
    sDBType->mIntervalIndicator       = aIntervalIndicator;

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}
    
/**
 * @brief 기존의 DB Type을 새로운 DB Type으로 변경한다.
 * @param[in]     aInitExprCode       Parse/Validation 단계의 Code Expression List
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[in]     aDBType             DB Type
 * @param[in]     aPrecision          Precision (TYPE 마다 용도가 다름)
 * @param[in]     aScale              Scale (TYPE 마다 용도가 다름)
 * @param[in]     aStringLengthUnit   String Length Unit
 * @param[in]     aIntervalIndicator  Interval Indicator
 * @param[in]     aIsInFunction       Expression Function에서의 호출 여부
 * @param[in]     aIsFixedLengthCast  Fixed Length Type에 대한 Cast 여부
 * @param[in]     aEnv                Environment
 *
 * @remark 기존의 DB Type을 새로운 DB Type으로 변경하는 Cast 정보를 생성한다.
 *    <BR> 단, constant value는 기존의 DB Type이 없으므로 Type변경을 요청해도
 *    <BR> cast가 일어나지 않으며, 해당 Type으로의 DataValue를 생성한다.
 *    <BR>
 *    <BR> cf. internal cast를 위한 함수
 */
stlStatus qloCastExprDBType( qlvInitExpression     * aInitExprCode,
                             qloExprInfo           * aExprInfo,
                             dtlDataType             aDBType,
                             stlInt64                aPrecision,
                             stlInt64                aScale,
                             dtlStringLengthUnit     aStringLengthUnit,
                             dtlIntervalIndicator    aIntervalIndicator,
                             stlBool                 aIsInFunction,
                             stlBool                 aIsFixedLengthCast,
                             qllEnv                * aEnv )
{   
    qloDBType        * sDBType = NULL;
    qloDBType        * sConstDBType = NULL;
    stlUInt32          sCastFuncIdx = 0;

    stlBool            sIsConstantData[ 2 ];

    dtlDataType        sResultType = DTL_TYPE_NA;
    dtlDataType        sInputArgDataTypes[ 2 ];
    dtlDataType        sResultArgDataTypes[ 2 ];

    dtlDataTypeDefInfo    sResultTypeDefInfo;
    dtlDataTypeDefInfo    sInputArgDataTypeDefInfo[ 2 ];
    dtlDataTypeDefInfo    sResultArgDataTypeDefInfo[ 2 ];

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprCode->mOffset < aExprInfo->mExprCnt,
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * Constant Expression은 원본 Expression에 Cast를 달아준다.
     */
    
    if( aInitExprCode->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
    {
        STL_PARAM_VALIDATE( aInitExprCode->mExpr.mConstExpr != NULL,
                            QLL_ERROR_STACK(aEnv) );

        sConstDBType  = & aExprInfo->mExprDBType[ aInitExprCode->mOffset ];
        aInitExprCode = aInitExprCode->mExpr.mConstExpr->mOrgExpr;
    }
    else
    {
        sConstDBType = NULL;
    }


    /**
     * DB Type 설정
     */
    
    sDBType = & aExprInfo->mExprDBType[ aInitExprCode->mOffset ];

    sIsConstantData[ 0 ] = STL_FALSE;
    sIsConstantData[ 1 ] = STL_FALSE;
        
    sInputArgDataTypes[ 0 ] = sDBType->mDBType;
    sInputArgDataTypes[ 1 ] = aDBType;

    sInputArgDataTypeDefInfo[0].mArgNum1 = sDBType->mArgNum1;
    sInputArgDataTypeDefInfo[0].mArgNum2 = sDBType->mArgNum2;
    sInputArgDataTypeDefInfo[0].mStringLengthUnit = sDBType->mStringLengthUnit;
    sInputArgDataTypeDefInfo[0].mIntervalIndicator = sDBType->mIntervalIndicator;

    sInputArgDataTypeDefInfo[1].mArgNum1 = aPrecision;
    sInputArgDataTypeDefInfo[1].mArgNum2 = aScale;
    sInputArgDataTypeDefInfo[1].mStringLengthUnit = aStringLengthUnit;
    sInputArgDataTypeDefInfo[1].mIntervalIndicator = aIntervalIndicator;

    switch( aInitExprCode->mType )
    {
        case QLV_EXPR_TYPE_VALUE :
            {
                if( aInitExprCode->mExpr.mValue->mValueType == QLV_VALUE_TYPE_NULL )
                {
                    sDBType->mDBType            = aDBType;
                    sDBType->mIsNeedCast        = STL_FALSE;
                    sDBType->mArgNum1           = aPrecision;
                    sDBType->mArgNum2           = aScale;
                    sDBType->mStringLengthUnit  = aStringLengthUnit;
                    sDBType->mIntervalIndicator = aIntervalIndicator;
                    
                    break;
                }
                else
                {
                    /* 아래 코드 수행 */
                }
            }
        case QLV_EXPR_TYPE_BIND_PARAM :
        case QLV_EXPR_TYPE_COLUMN :
        case QLV_EXPR_TYPE_FUNCTION :
        case QLV_EXPR_TYPE_PSEUDO_COLUMN :
        case QLV_EXPR_TYPE_CAST :
        case QLV_EXPR_TYPE_SUB_QUERY :
        case QLV_EXPR_TYPE_REFERENCE :
        case QLV_EXPR_TYPE_INNER_COLUMN :
        case QLV_EXPR_TYPE_OUTER_COLUMN :
        case QLV_EXPR_TYPE_ROWID_COLUMN :
        case QLV_EXPR_TYPE_AGGREGATION :
        case QLV_EXPR_TYPE_CASE_EXPR :
        case QLV_EXPR_TYPE_LIST_FUNCTION:
        case QLV_EXPR_TYPE_LIST_COLUMN:
        case QLV_EXPR_TYPE_ROW_EXPR:
            {
                STL_PARAM_VALIDATE( sDBType->mDBType < DTL_TYPE_MAX,
                                    QLL_ERROR_STACK(aEnv) );
                
                if( sDBType->mIsNeedCast == STL_FALSE )
                {
                    sDBType->mSourceDBType            = sDBType->mDBType;
                    sDBType->mSourceArgNum1           = sDBType->mArgNum1;
                    sDBType->mSourceArgNum2           = sDBType->mArgNum2;
                    sDBType->mSourceStringLengthUnit  = sDBType->mStringLengthUnit;
                    sDBType->mSourceIntervalIndicator = sDBType->mIntervalIndicator;
                }
                else
                {
                    /* Do Nothing */
                    /* 두 번 이상의 cast시 원본의 type이 변경되지 않도록 함 */
                }

                if( ( sDBType->mDBType == aDBType ) &&
                    ( aIsInFunction == STL_FALSE ) )
                {
                    switch( aDBType )
                    {
                        case DTL_TYPE_FLOAT :
                            {
                                /**
                                 * Cast가 필요한 경우
                                 * 1. precision이 줄어드는 경우
                                 */

                                if( sDBType->mArgNum1 <= aPrecision )
                                {
                                    sDBType->mArgNum1    = aPrecision;
                                    sDBType->mArgNum2    = aScale;
                                }
                                else
                                {
                                    sDBType->mIsNeedCast = STL_TRUE;
                                    sDBType->mArgNum1    = aPrecision;
                                    sDBType->mArgNum2    = aScale;

                                    if( dtlGetFuncInfoCast( 2,
                                                            sIsConstantData,
                                                            sInputArgDataTypes,
                                                            sInputArgDataTypeDefInfo,
                                                            1,
                                                            sResultArgDataTypes,
                                                            sResultArgDataTypeDefInfo,
                                                            & sResultType,
                                                            & sResultTypeDefInfo,
                                                            & sCastFuncIdx,
                                                            KNL_DT_VECTOR(aEnv),
                                                            aEnv,
                                                            QLL_ERROR_STACK( aEnv ) )
                                        != STL_SUCCESS )
                                    {
                                        (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                                        STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                                    }
                                    else
                                    {
                                        sDBType->mCastFuncIdx = sCastFuncIdx;
                                    }

                                    STL_PARAM_VALIDATE( sResultType == aDBType,
                                                        QLL_ERROR_STACK( aEnv ) );
                                }
                                break;
                            }
                            
                        case DTL_TYPE_NUMBER :
                            {
                                /**
                                 * Cast가 필요한 경우
                                 * 1. domain 변경 :
                                 *  - 최대값의 범위가 줄어드는 경우
                                 *    => 최대값의 범위 = Precision - Scale
                                 * 2. round 수행 :
                                 *  - 반올림 대상 범위가 커지는 경우
                                 *    => 반올림 대상 범위 = -aScale
                                 */

                                if( ( ( (stlInt64)sDBType->mArgNum1 - (stlInt64)sDBType->mArgNum2 )
                                      <= ( (stlInt64)aPrecision - (stlInt64)aScale ) ) &&
                                    ( sDBType->mArgNum2 <= aScale ) )
                                {
                                    sDBType->mArgNum1    = aPrecision;
                                    sDBType->mArgNum2    = aScale;
                                }
                                else
                                {
                                    sDBType->mIsNeedCast = STL_TRUE;
                                    sDBType->mArgNum1    = aPrecision;
                                    sDBType->mArgNum2    = aScale;

                                    if( dtlGetFuncInfoCast( 2,
                                                            sIsConstantData,
                                                            sInputArgDataTypes,
                                                            sInputArgDataTypeDefInfo,
                                                            1,
                                                            sResultArgDataTypes,
                                                            sResultArgDataTypeDefInfo,
                                                            & sResultType,
                                                            & sResultTypeDefInfo,
                                                            & sCastFuncIdx,
                                                            KNL_DT_VECTOR(aEnv),
                                                            aEnv,
                                                            QLL_ERROR_STACK( aEnv ) )
                                        != STL_SUCCESS )
                                    {
                                        (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                                        STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                                    }
                                    else
                                    {
                                        sDBType->mCastFuncIdx = sCastFuncIdx;
                                    }

                                    STL_PARAM_VALIDATE( sResultType == aDBType,
                                                        QLL_ERROR_STACK( aEnv ) );
                                }
                                break;
                            }

                        case DTL_TYPE_TIME :
                        case DTL_TYPE_TIMESTAMP :
                        case DTL_TYPE_TIME_WITH_TIME_ZONE : 
                        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE : 
                            {
                                if( sDBType->mArgNum1 == aPrecision )
                                {
                                    sDBType->mArgNum1    = aPrecision;
                                    sDBType->mArgNum2    = DTL_SCALE_NA;
                                }
                                else
                                {
                                    sDBType->mIsNeedCast = STL_TRUE;
                                    sDBType->mArgNum1    = aPrecision;
                                    sDBType->mArgNum2    = DTL_SCALE_NA;

                                    if( dtlGetFuncInfoCast( 2,
                                                            sIsConstantData,
                                                            sInputArgDataTypes,
                                                            sInputArgDataTypeDefInfo,
                                                            1,
                                                            sResultArgDataTypes,
                                                            sResultArgDataTypeDefInfo,
                                                            & sResultType,
                                                            & sResultTypeDefInfo,
                                                            & sCastFuncIdx,
                                                            KNL_DT_VECTOR(aEnv),
                                                            aEnv,
                                                            QLL_ERROR_STACK( aEnv ) )
                                        != STL_SUCCESS )
                                    {
                                        (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                                        STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                                    }
                                    else
                                    {
                                        sDBType->mCastFuncIdx = sCastFuncIdx;
                                    }

                                    STL_PARAM_VALIDATE( sResultType == aDBType,
                                                        QLL_ERROR_STACK( aEnv ) );
                                }
                                break;
                            }

                        case DTL_TYPE_CHAR :
                            {
                                if( (sDBType->mArgNum1 == aPrecision) &&
                                    (sDBType->mStringLengthUnit == aStringLengthUnit ) &&
                                    (aIsFixedLengthCast == STL_FALSE) )
                                {
                                    /* Do Nothing */
                                }
                                else
                                {
                                    sDBType->mIsNeedCast       = STL_TRUE;
                                    sDBType->mArgNum1          = aPrecision;
                                    sDBType->mStringLengthUnit = aStringLengthUnit;

                                    if( dtlGetFuncInfoCast( 2,
                                                            sIsConstantData,
                                                            sInputArgDataTypes,
                                                            sInputArgDataTypeDefInfo,
                                                            1,
                                                            sResultArgDataTypes,
                                                            sResultArgDataTypeDefInfo,
                                                            & sResultType,
                                                            & sResultTypeDefInfo,
                                                            & sCastFuncIdx,
                                                            KNL_DT_VECTOR(aEnv),
                                                            aEnv,
                                                            QLL_ERROR_STACK( aEnv ) )
                                        != STL_SUCCESS )
                                    {
                                        (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                                        STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                                    }
                                    else
                                    {
                                        sDBType->mCastFuncIdx = sCastFuncIdx;
                                    }

                                    STL_PARAM_VALIDATE( sResultType == aDBType,
                                                        QLL_ERROR_STACK( aEnv ) );
                                }

                                break;
                            }
                            
                        case DTL_TYPE_VARCHAR :
                        case DTL_TYPE_LONGVARCHAR :
                            {
                                if( (sDBType->mArgNum1 <= aPrecision) &&
                                    (sDBType->mStringLengthUnit == aStringLengthUnit ) )
                                {
                                    sDBType->mArgNum1          = aPrecision;
                                }
                                else
                                {
                                    sDBType->mIsNeedCast       = STL_TRUE;
                                    sDBType->mArgNum1          = aPrecision;
                                    sDBType->mStringLengthUnit = aStringLengthUnit;

                                    if( dtlGetFuncInfoCast( 2,
                                                            sIsConstantData,
                                                            sInputArgDataTypes,
                                                            sInputArgDataTypeDefInfo,
                                                            1,
                                                            sResultArgDataTypes,
                                                            sResultArgDataTypeDefInfo,
                                                            & sResultType,
                                                            & sResultTypeDefInfo,
                                                            & sCastFuncIdx,
                                                            KNL_DT_VECTOR(aEnv),
                                                            aEnv,
                                                            QLL_ERROR_STACK( aEnv ) )
                                        != STL_SUCCESS )
                                    {
                                        (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                                        STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                                    }
                                    else
                                    {
                                        sDBType->mCastFuncIdx = sCastFuncIdx;
                                    }

                                    STL_PARAM_VALIDATE( sResultType == aDBType,
                                                        QLL_ERROR_STACK( aEnv ) );
                                }

                                break;
                            }

                        case DTL_TYPE_BINARY :
                            {
                                if( ( sDBType->mArgNum1 == aPrecision ) &&
                                    ( aIsFixedLengthCast == STL_FALSE ) )
                                {
                                    /* Do Nothing */
                                }
                                else
                                {
                                    sDBType->mIsNeedCast = STL_TRUE;
                                    sDBType->mArgNum1    = aPrecision;

                                    if( dtlGetFuncInfoCast( 2,
                                                            sIsConstantData,
                                                            sInputArgDataTypes,
                                                            sInputArgDataTypeDefInfo,
                                                            1,
                                                            sResultArgDataTypes,
                                                            sResultArgDataTypeDefInfo,
                                                            & sResultType,
                                                            & sResultTypeDefInfo,
                                                            & sCastFuncIdx,
                                                            KNL_DT_VECTOR(aEnv),
                                                            aEnv,
                                                            QLL_ERROR_STACK( aEnv ) )
                                        != STL_SUCCESS )
                                    {
                                        (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                                        STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                                    }
                                    else
                                    {
                                        sDBType->mCastFuncIdx = sCastFuncIdx;
                                    }

                                    STL_PARAM_VALIDATE( sResultType == aDBType,
                                                        QLL_ERROR_STACK( aEnv ) );
                                }
                                break;
                            }
                            
                        case DTL_TYPE_VARBINARY :
                        case DTL_TYPE_LONGVARBINARY :
                            {
                                if( sDBType->mArgNum1 == aPrecision )
                                {
                                    /* Do Nothing */
                                }
                                else
                                {
                                    sDBType->mIsNeedCast = STL_TRUE;
                                    sDBType->mArgNum1    = aPrecision;

                                    if( dtlGetFuncInfoCast( 2,
                                                            sIsConstantData,
                                                            sInputArgDataTypes,
                                                            sInputArgDataTypeDefInfo,
                                                            1,
                                                            sResultArgDataTypes,
                                                            sResultArgDataTypeDefInfo,
                                                            & sResultType,
                                                            & sResultTypeDefInfo,
                                                            & sCastFuncIdx,
                                                            KNL_DT_VECTOR(aEnv),
                                                            aEnv,
                                                            QLL_ERROR_STACK( aEnv ) )
                                        != STL_SUCCESS )
                                    {
                                        (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                                        STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                                    }
                                    else
                                    {
                                        sDBType->mCastFuncIdx = sCastFuncIdx;
                                    }

                                    STL_PARAM_VALIDATE( sResultType == aDBType,
                                                        QLL_ERROR_STACK( aEnv ) );
                                }
                                break;
                            }

                        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
                        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
                            {
                                if( ( sDBType->mArgNum1 == aPrecision ) &&
                                    ( sDBType->mArgNum2 == aScale ) && 
                                    ( sDBType->mIntervalIndicator == aIntervalIndicator ) )
                                {
                                    /* Do Nothing */
                                }
                                else
                                {
                                    sDBType->mIsNeedCast        = STL_TRUE;
                                    sDBType->mArgNum1           = aPrecision;
                                    sDBType->mArgNum2           = aScale;
                                    sDBType->mIntervalIndicator = aIntervalIndicator;

                                    if( dtlGetFuncInfoCast( 2,
                                                            sIsConstantData,
                                                            sInputArgDataTypes,
                                                            sInputArgDataTypeDefInfo,
                                                            1,
                                                            sResultArgDataTypes,
                                                            sResultArgDataTypeDefInfo,
                                                            & sResultType,
                                                            & sResultTypeDefInfo,
                                                            & sCastFuncIdx,
                                                            KNL_DT_VECTOR(aEnv),
                                                            aEnv,
                                                            QLL_ERROR_STACK( aEnv ) )
                                        != STL_SUCCESS )
                                    {
                                        (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                                        STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                                    }
                                    else
                                    {
                                        sDBType->mCastFuncIdx = sCastFuncIdx; 
                                    }

                                    STL_PARAM_VALIDATE( sResultType == aDBType,
                                                        QLL_ERROR_STACK( aEnv ) );
                                }
                                break;
                            }

                        case DTL_TYPE_BOOLEAN :
                        case DTL_TYPE_NATIVE_SMALLINT :
                        case DTL_TYPE_NATIVE_INTEGER :
                        case DTL_TYPE_NATIVE_BIGINT :
                        case DTL_TYPE_NATIVE_REAL :
                        case DTL_TYPE_NATIVE_DOUBLE :
                        case DTL_TYPE_DATE :
                        case DTL_TYPE_ROWID :
                            {
                                /* ignore */
                                /* Do Nothing */
                                break;
                            }
                        default :
                            {
                                STL_DASSERT( 0 );
                                break;
                            }
                            
                    }
                }
                else if( ( ( dtlDataTypeGroup[sDBType->mDBType] == DTL_GROUP_NUMBER ) &&
                           ( dtlDataTypeGroup[aDBType] == DTL_GROUP_NUMBER ) ) &&
                         ( sDBType->mDBType != aDBType ) )
                {
                    /* FLOAT  NUMBER 간 conversion이 발생하지 않도록. */

                    if( sDBType->mDBType == DTL_TYPE_FLOAT )
                    {
                        /**
                         * Cast가 필요한 경우
                         * 1. precision이 줄어드는 경우
                         * 2. target scale 이 N/A가 아닌 경우
                         */

                        STL_DASSERT( aDBType == DTL_TYPE_NUMBER );

                        if( ( aScale == DTL_SCALE_NA ) &&
                            ( dtlBinaryToDecimalPrecision[ sDBType->mArgNum1 ] <= aPrecision ) )
                        {
                            sDBType->mDBType     = aDBType;
                            sDBType->mArgNum1    = aPrecision;
                            sDBType->mArgNum2    = aScale;
                        }
                        else
                        {
                            sDBType->mDBType     = aDBType;
                            sDBType->mIsNeedCast = STL_TRUE;
                            sDBType->mArgNum1    = aPrecision;
                            sDBType->mArgNum2    = aScale;
                        
                            if( dtlGetFuncInfoCast( 2,
                                                    sIsConstantData,
                                                    sInputArgDataTypes,
                                                    sInputArgDataTypeDefInfo,
                                                    1,
                                                    sResultArgDataTypes,
                                                    sResultArgDataTypeDefInfo,
                                                    & sResultType,
                                                    & sResultTypeDefInfo,
                                                    & sCastFuncIdx,
                                                    KNL_DT_VECTOR(aEnv),
                                                    aEnv,
                                                    QLL_ERROR_STACK( aEnv ) )
                                != STL_SUCCESS )
                            {
                                (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                                STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                            }
                            else
                            {
                                sDBType->mCastFuncIdx = sCastFuncIdx;
                            }

                            STL_PARAM_VALIDATE( sResultType == aDBType,
                                                QLL_ERROR_STACK( aEnv ) );
                        }
                    }
                    else
                    {
                        /**
                         * Cast가 필요한 경우
                         * 1. Precision 줄어드는 경우
                         */

                        STL_DASSERT( sDBType->mDBType == DTL_TYPE_NUMBER );
                        STL_DASSERT( aDBType == DTL_TYPE_FLOAT );

                        if( sDBType->mArgNum1 <= dtlBinaryToDecimalPrecision[ (stlUInt16) aPrecision ] )
                        {
                            sDBType->mDBType     = aDBType;
                            sDBType->mArgNum1    = aPrecision;
                            sDBType->mArgNum2    = aScale;
                        }
                        else
                        {
                            sDBType->mDBType     = aDBType;
                            sDBType->mIsNeedCast = STL_TRUE;
                            sDBType->mArgNum1    = aPrecision;
                            sDBType->mArgNum2    = aScale;
                        
                            if( dtlGetFuncInfoCast( 2,
                                                    sIsConstantData,
                                                    sInputArgDataTypes,
                                                    sInputArgDataTypeDefInfo,
                                                    1,
                                                    sResultArgDataTypes,
                                                    sResultArgDataTypeDefInfo,
                                                    & sResultType,
                                                    & sResultTypeDefInfo,
                                                    & sCastFuncIdx,
                                                    KNL_DT_VECTOR(aEnv),
                                                    aEnv,
                                                    QLL_ERROR_STACK( aEnv ) )
                                != STL_SUCCESS )
                            {
                                (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                                STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                            }
                            else
                            {
                                sDBType->mCastFuncIdx = sCastFuncIdx;
                            }

                            STL_PARAM_VALIDATE( sResultType == aDBType,
                                                QLL_ERROR_STACK( aEnv ) );
                        }
                    }
                }
                else if( sDBType->mDBType != aDBType )
                {
                    sDBType->mDBType            = aDBType;
                    sDBType->mIsNeedCast        = STL_TRUE;
                    sDBType->mArgNum1           = aPrecision;
                    sDBType->mArgNum2           = aScale;
                    sDBType->mStringLengthUnit  = aStringLengthUnit;
                    sDBType->mIntervalIndicator = aIntervalIndicator;

                    if( dtlGetFuncInfoCast( 2,
                                            sIsConstantData,
                                            sInputArgDataTypes,
                                            sInputArgDataTypeDefInfo,
                                            1,
                                            sResultArgDataTypes,
                                            sResultArgDataTypeDefInfo,
                                            & sResultType,
                                            & sResultTypeDefInfo,
                                            & sCastFuncIdx,
                                            KNL_DT_VECTOR(aEnv),
                                            aEnv,
                                            QLL_ERROR_STACK( aEnv ) )
                        != STL_SUCCESS )
                    {
                        (void)stlPopError( QLL_ERROR_STACK(aEnv) );
                        STL_THROW( RAMP_ERR_NOT_APPLICABLE_CAST );
                    }
                    else
                    {
                        sDBType->mCastFuncIdx = sCastFuncIdx;
                    }
                    
                    STL_PARAM_VALIDATE( sResultType == aDBType,
                                        QLL_ERROR_STACK( aEnv ) );
                }
                else
                {
                    /* Do Nothing */
                }
                break;
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
        
    }
    

    if( sConstDBType != NULL )
    {
        sConstDBType->mDBType = sDBType->mDBType;

        sConstDBType->mIsNeedCast              = STL_FALSE;
        sConstDBType->mCastFuncIdx             = 0;

        sConstDBType->mSourceDBType            = sDBType->mDBType;
        sConstDBType->mSourceArgNum1           = sDBType->mArgNum1;
        sConstDBType->mSourceArgNum2           = sDBType->mArgNum2;
        sConstDBType->mSourceStringLengthUnit  = sDBType->mStringLengthUnit;
        sConstDBType->mSourceIntervalIndicator = sDBType->mIntervalIndicator;

        sConstDBType->mArgNum1                 = sDBType->mArgNum1;
        sConstDBType->mArgNum2                 = sDBType->mArgNum2;
        sConstDBType->mStringLengthUnit        = sDBType->mStringLengthUnit;
        sConstDBType->mIntervalIndicator       = sDBType->mIntervalIndicator;
    }
    else
    {
        /* Do Nothing */
    }
    

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_APPLICABLE_CAST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INCONSISTENT_DATATYPES,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      gDataTypeDefinition[aDBType].mSqlNormalTypeName );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Function Expression의 Code 영역 정보를 구성한다.
 * @param[in]     aInitExprCode       Parse/Validation 단계의 Code Expression List
 * @param[in,out] aExprInfo           All Expression's Information
 * @param[in,out] aExprEntry          Expresson Stack의 Type 변경 대상 Entry
 * @param[in]     aFuncID             Function ID
 * @param[in]     aFuncIdx            Function Idx
 * @param[in]     aResultBufferSize   Result Buffer Size
 * @param[in]     aReturnType         Return Type Class
 * @param[in]     aEnv                Environment
 *
 * @remark Function Expression의 Data Area 구성에 필요한 정보를 구성한다.
 */
stlStatus qloSetExprFunc( qlvInitExpression       * aInitExprCode,
                          qloExprInfo             * aExprInfo,
                          knlExprEntry            * aExprEntry,
                          knlBuiltInFunc            aFuncID,
                          stlUInt32                 aFuncIdx,
                          stlUInt32                 aResultBufferSize,
                          dtlFuncReturnTypeClass    aReturnType,
                          qllEnv                  * aEnv )
{
    qloFunctionInfo   * sFuncInfo = NULL;

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprCode->mOffset < aExprInfo->mExprCnt,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprEntry != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aExprEntry->mExprType == KNL_EXPR_TYPE_FUNCTION ) ||
                        ( aExprEntry->mExprType == KNL_EXPR_TYPE_LIST ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultBufferSize > 0, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Function 정보 설정
     */
    
    sFuncInfo = & aExprInfo->mExprFunc[ aInitExprCode->mOffset ];

    sFuncInfo->mFuncID           = aFuncID;
    sFuncInfo->mFuncIdx          = aFuncIdx;
    sFuncInfo->mResultBufferSize = aResultBufferSize;
    sFuncInfo->mReturnType       = aReturnType;
            
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief List Function Stack Expression의 Code 영역 정보를 구성한다.
 * @param[in]     aInitExprCode       Parse/Validation 단계의 Code Expression List
 * @param[in,out] aExprInfo           All Expression's Information
  *@param[in]     aInitStack          List Stack
 * @param[in]     aRowCount           List Stack Row Value Count
 * @param[in]     aOnlyValue          List Stack Only Value Entries 
 * @param[in]     aRegionMem          Region Memory
 * @param[in]     aEnv                Environment
 *
 * @remark Function Expression의 Data Area 구성에 필요한 정보를 구성한다.
 */
stlStatus qloSetListFuncExpr( qlvInitExpression       * aInitExprCode,
                              qloExprInfo             * aExprInfo,
                              knlListStack            * aInitStack,
                              stlUInt32                 aRowCount,
                              stlBool                   aOnlyValue,
                              knlRegionMem            * aRegionMem,
                              qllEnv                  * aEnv )
{
    qloListFuncInfo   * sListFuncInfo = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitExprCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExprCode->mOffset < aExprInfo->mExprCnt,
                        QLL_ERROR_STACK(aEnv) );
    

    /* qloListFuncInfo 공간 할당 */

    sListFuncInfo = & aExprInfo->mListFunc[ aInitExprCode->mOffset ];

    sListFuncInfo->mValueCount    = aRowCount;
    sListFuncInfo->mListStack     = aInitStack;

    if( aRowCount > 1 )
    {
        /* Multiple Columns In a Row*/
        if( aOnlyValue == STL_TRUE )
        {
            sListFuncInfo->mListFuncType = KNL_LIST_FUNCTION_MULTI_ROW_ONLY_VALUES;
        }
        else
        {
            sListFuncInfo->mListFuncType = KNL_LIST_FUNCTION_MULTI_ROW_ALL_EXPR;
        }
    }
    else
    {
        /* Single Column In a Row*/
        if( aOnlyValue == STL_TRUE )
        {
            sListFuncInfo->mListFuncType = KNL_LIST_FUNCTION_SINGLE_ROW_ONLY_VALUES;
        }
        else
        {
            sListFuncInfo->mListFuncType = KNL_LIST_FUNCTION_SINGLE_ROW_ALL_EXPR;
        }
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*******************************************************************************
 * Data Type Validation
 ******************************************************************************/


/**
 * @brief VALUE expression에 대한 숫자형 DataType을 결정한다.
 * @param[in]     aInitValueCode Parse/Validation 단계의 Code Expression
 * @param[out]    aDataType      DB Data Type
 * @param[in]     aEnv           Environment
 */
stlStatus qloGetValueDataType( qlvInitValue  * aInitValueCode,
                               dtlDataType   * aDataType,
                               qllEnv        * aEnv )
{
    dtlDataType     sDataType  = DTL_TYPE_NA;
    qlvInitValue  * sInitValue = NULL;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aInitValueCode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataType != NULL, QLL_ERROR_STACK(aEnv) );


    sInitValue = aInitValueCode;

    switch( sInitValue->mValueType )
    {
        case QLV_VALUE_TYPE_NULL :
            {
                sDataType = DTL_TYPE_VARCHAR;
                break;
            }
        case QLV_VALUE_TYPE_BOOLEAN :
            {
                /* validation 과정에서 확인됨 */
                sDataType = DTL_TYPE_BOOLEAN;
                break;
            }
        case QLV_VALUE_TYPE_NUMERIC :
            {
                /**
                 * DTL_TYPE_NUMBER (max precision/scale)
                 */
                sDataType = DTL_TYPE_NUMBER;
                break;
            }
        case QLV_VALUE_TYPE_BINARY_DOUBLE :
            {
                sDataType = DTL_TYPE_NATIVE_DOUBLE;
                break;
            }
        case QLV_VALUE_TYPE_BINARY_REAL :
            {
                sDataType = DTL_TYPE_NATIVE_REAL;
                break;
            }
        case QLV_VALUE_TYPE_BINARY_INTEGER :
            {
                sDataType = DTL_TYPE_NATIVE_BIGINT;
                break;
            }
        case QLV_VALUE_TYPE_STRING_FIXED :
            {
                sDataType = DTL_TYPE_CHAR;
                break;
            }
        case QLV_VALUE_TYPE_STRING_VARYING :
            {
                sDataType = DTL_TYPE_VARCHAR;
                break;
            }
        case QLV_VALUE_TYPE_BITSTRING_FIXED :
            {
                sDataType = DTL_TYPE_BINARY;
                break;
            }
        case QLV_VALUE_TYPE_BITSTRING_VARYING :
            {
                sDataType = DTL_TYPE_VARBINARY;
                break;
            }
        case QLV_VALUE_TYPE_DATE_LITERAL_STRING :
            {
                sDataType = DTL_TYPE_DATE;
                break;
            }
        case QLV_VALUE_TYPE_TIME_LITERAL_STRING :
            {
                sDataType = DTL_TYPE_TIME;
                break;
            }
        case QLV_VALUE_TYPE_TIME_TZ_LITERAL_STRING :
            {
                sDataType = DTL_TYPE_TIME_WITH_TIME_ZONE;
                break;
            }
        case QLV_VALUE_TYPE_TIMESTAMP_LITERAL_STRING :
            {
                sDataType = DTL_TYPE_TIMESTAMP;
                break;
            }
        case QLV_VALUE_TYPE_TIMESTAMP_TZ_LITERAL_STRING :
            {
                sDataType = DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE;
                break;
            }            
        case QLV_VALUE_TYPE_INTERVAL_YEAR_TO_MONTH_STRING :
            {
                sDataType = DTL_TYPE_INTERVAL_YEAR_TO_MONTH;
                break;
            }
        case QLV_VALUE_TYPE_INTERVAL_DAY_TO_SECOND_STRING :
            {
                sDataType = DTL_TYPE_INTERVAL_DAY_TO_SECOND;
                break;
            }
        case QLV_VALUE_TYPE_ROWID_STRING :
            {
                sDataType = DTL_TYPE_ROWID;
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
    
    *aDataType = sDataType;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Constant Exression Stack을 평가한다.
 * @param[in]     aExprStack       Constant Expression Stack
 * @param[in]     aDataArea        Data Area
 * @param[in]     aEnv             Environment
 */
stlStatus qlxEvaluateConstExpr( knlExprStack        * aExprStack,
                                qllDataArea         * aDataArea,
                                knlEnv              * aEnv )
{
    knlExprEvalInfo   sExprEvalInfo;


    /**
     * SYS DATETIME, STATEMENT DATETIME, TRANSACTION DATETIME 값을 만들기 위한 값을 얻어서 이를 이용한다.
     *
     * 참고로,
     * SYS DATETIME 과 STATEMENT DATETIME 값을 얻는 순서는 오라클과 동일함.
     */

    /*
     * 1. SYS DATETIME 값을 만들기 위한 timestamp with time zone 값을 얻는다.
     */

    if( aDataArea->mSysTimestampTzBlock != NULL )
    {
        STL_TRY( dtlSysTimestampTzSetValueFromStlTime(
                     stlNow(),
                     DTL_TIMESTAMPTZ_MAX_PRECISION,
                     DTL_SCALE_NA,
                     DTL_TIMESTAMPTZ_SIZE,
                     KNL_GET_BLOCK_FIRST_DATA_VALUE( aDataArea->mSysTimestampTzBlock ),
                     KNL_DT_VECTOR( aEnv ),
                     (void*)aEnv,
                     QLL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * 2. STATEMENT DATETIME 값을 만들기 위한 timestamp with time zone 값을 얻는다.
     */

    if( aDataArea->mStmtTimestampTzBlock != NULL )
    {
        STL_TRY( dtlTimestampTzSetValueFromStlTime(
                     stlNow(),
                     DTL_TIMESTAMPTZ_MAX_PRECISION,
                     DTL_SCALE_NA,
                     DTL_TIMESTAMPTZ_SIZE,
                     KNL_GET_BLOCK_FIRST_DATA_VALUE( aDataArea->mStmtTimestampTzBlock ),
                     KNL_DT_VECTOR( aEnv ),
                     (void*)aEnv,
                     QLL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * 3. TRANSACTION DATETIME 값을 만들기 위한 timestamp with time zone 값을 얻는다.
     */

    if( aDataArea->mTransactionTimestampTzBlock != NULL )
    {
        STL_TRY( dtlTimestampTzSetValueFromStlTime(
                     smlBeginTransTime( aDataArea->mTransID, SML_ENV( aEnv ) ),
                     DTL_TIMESTAMPTZ_MAX_PRECISION,
                     DTL_SCALE_NA,
                     DTL_TIMESTAMPTZ_SIZE,
                     KNL_GET_BLOCK_FIRST_DATA_VALUE( aDataArea->mTransactionTimestampTzBlock ),
                     KNL_DT_VECTOR( aEnv ),
                     (void*)aEnv,
                     QLL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }
    
    if( aExprStack == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        if( aExprStack->mEntryCount > 0 )
        {
            STL_PARAM_VALIDATE( aDataArea->mExprDataContext != NULL,
                                QLL_ERROR_STACK( aEnv ) );
        
            sExprEvalInfo.mExprStack   = (knlExprStack *) aExprStack;
            sExprEvalInfo.mContext     = aDataArea->mExprDataContext;
            sExprEvalInfo.mResultBlock = aDataArea->mConstResultBlock;

            sExprEvalInfo.mBlockIdx    = 0;
            sExprEvalInfo.mBlockCount  = 1;

            STL_TRY( knlEvaluateOneExpression( & sExprEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Parse/Validation 단계의 Code Expression의 DataType을 반환한다.
 * @param[in]     aSQLString       SQL String
 * @param[in]     aInitExprCode    Parse/Validation 단계의 Code Expression
 * @param[in]     aBindContext     Bind Context 
 * @param[out]    aDataType        DataType
 * @param[in]     aRegionMem       Region Memory
 * @param[in]     aEnv             Environment
 */
stlStatus qloGetDataType( stlChar            * aSQLString,
                          qlvInitExpression  * aInitExprCode,
                          knlBindContext     * aBindContext,
                          dtlDataType        * aDataType,
                          knlRegionMem       * aRegionMem,
                          qllEnv             * aEnv )
{
    qlvInitDataTypeInfo  sDataTypeInfo;
    
    STL_TRY( qlvGetExprDataTypeInfo( aSQLString,
                                     aRegionMem,
                                     aInitExprCode,
                                     aBindContext,
                                     & sDataTypeInfo,
                                     aEnv )
             == STL_SUCCESS );

    *aDataType = sDataTypeInfo.mDataType;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;   
}


/**
 * @brief Filter 에서 Logical Filter 와 Physical Filter 를 구분
 * @param[in]     aSQLString       SQL String
 * @param[in]     aFilterExpr      Filter Expression
 * @param[in]     aRelationNode    Relation Node
 * @param[in]     aRegionMem       Region Memory
 * @param[in]     aBindContext     Bind Context
 * @param[out]    aLogicalFilter   Logical Filter Expression
 * @param[out]    aPhysicalFilter  Physical Filter Expression
 * @param[in]     aEnv             Environment
 */
stlStatus qloClassifyFilterExpr( stlChar             * aSQLString,
                                 qlvInitExpression   * aFilterExpr,
                                 qlvInitNode         * aRelationNode,
                                 knlRegionMem        * aRegionMem,
                                 knlBindContext      * aBindContext,
                                 qlvInitExpression  ** aLogicalFilter,
                                 qlvInitExpression  ** aPhysicalFilter,
                                 qllEnv              * aEnv )
{
    qlvInitExpression  * sFilterExpr            = NULL;
    qlvInitExpression  * sLogicalFilter         = NULL;
    qlvInitExpression  * sPhysicalFilter        = NULL;

    qlvInitExpression ** sLogicalFilterArr      = NULL;
    qlvInitExpression ** sPhysicalFilterArr     = NULL;
    stlInt32             sLogicalFilterCnt      = 0;
    stlInt32             sPhysicalFilterCnt     = 0;

    dtlDataType          sDataType1             = DTL_TYPE_NA;
    dtlDataType          sDataType2             = DTL_TYPE_NA;
    qlvInitFunction    * sFuncCode              = NULL;
    qlvInitListFunc    * sListFunc              = NULL;
    qlvInitListCol     * sListColLeft           = NULL;
    qlvInitListCol     * sListColRight          = NULL;
    qlvInitRowExpr     * sRowExpr               = NULL;

    const dtlCompareType  * sCompareType        = NULL;
    
    dtlPhysicalFunc      sPhysicalFuncPtr;
    stlUInt16            sArgCnt;
    stlInt32             i;

    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLString != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFilterExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aLogicalFilter != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPhysicalFilter != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRelationNode != NULL, QLL_ERROR_STACK(aEnv) );
    
    sFilterExpr     = aFilterExpr;

#define SET_FILTER_EXPR( filter, expr )         \
    {                                           \
        STL_TRY( qlvDuplicateExpr( (expr),      \
                                   &(filter),   \
                                   aRegionMem,  \
                                   aEnv )       \
                 == STL_SUCCESS );              \
    }


    /**
     * Filter 구분
     */

    if( sFilterExpr->mType == QLV_EXPR_TYPE_FUNCTION )
    {
        sFuncCode = sFilterExpr->mExpr.mFunction;

        switch( sFuncCode->mFuncId )
        {
            case KNL_BUILTIN_FUNC_AND :
                {
                    /**
                     * Expression 배열 공간 할당
                     */

                    sArgCnt = sFuncCode->mArgCount;
                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                (STL_SIZEOF( qlvInitExpression * )) * sArgCnt,
                                                (void **) & sLogicalFilterArr,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    STL_TRY( knlAllocRegionMem( aRegionMem,
                                                (STL_SIZEOF( qlvInitExpression * )) * sArgCnt,
                                                (void **) & sPhysicalFilterArr,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    /**
                     * 모든 Argument들에 대해 logical/physical classify
                     */
                    for( i = 0; i < sFuncCode->mArgCount; i++ )
                    {
                        if( ( sFuncCode->mArgs[i]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                            ( sFuncCode->mArgs[i]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) ||
                            ( sFuncCode->mArgs[i]->mType == QLV_EXPR_TYPE_FUNCTION )  ||
                            ( sFuncCode->mArgs[i]->mType == QLV_EXPR_TYPE_LIST_FUNCTION ) )
                        {
                            /* Arg[i]을 다시 평가 */
                            STL_TRY( qloClassifyFilterExpr( aSQLString,
                                                            sFuncCode->mArgs[i],
                                                            aRelationNode,
                                                            aRegionMem,
                                                            aBindContext,
                                                            & sLogicalFilter,
                                                            & sPhysicalFilter,
                                                            aEnv )
                                     == STL_SUCCESS );
                        }
                        else
                        {
                            sPhysicalFilter = NULL;
                            SET_FILTER_EXPR( sLogicalFilter, sFuncCode->mArgs[i] );
                        }

                        if( sLogicalFilter != NULL )
                        {
                            sLogicalFilterArr[sLogicalFilterCnt] = sLogicalFilter;
                            sLogicalFilterCnt++;
                        }
                        if( sPhysicalFilter != NULL )
                        {
                            sPhysicalFilterArr[sPhysicalFilterCnt] = sPhysicalFilter;
                            sPhysicalFilterCnt++;
                        }
                    }

                    /**
                     * logical/physical filter 결과 생성
                     */
                    if( sLogicalFilterCnt == 0 )
                    {
                        sLogicalFilter = NULL;
                    }
                    else if( sLogicalFilterCnt == 1 )
                    {
                        sLogicalFilter = sLogicalFilterArr[0];
                    }
                    else
                    {
                        STL_TRY( qlvMakeFuncAndNArg( aRegionMem,
                                                     sFilterExpr->mPosition,
                                                     sLogicalFilterCnt,
                                                     sLogicalFilterArr,
                                                     &sLogicalFilter,
                                                     aEnv )
                                 == STL_SUCCESS );
                        sLogicalFilter->mOffset = sFilterExpr->mOffset;
                    }

                    if( sPhysicalFilterCnt == 0 )
                    {
                        sPhysicalFilter = NULL;
                    }
                    else if( sPhysicalFilterCnt == 1 )
                    {
                        sPhysicalFilter = sPhysicalFilterArr[0];
                    }
                    else
                    {
                        STL_TRY( qlvMakeFuncAndNArg( aRegionMem,
                                                     sFilterExpr->mPosition,
                                                     sPhysicalFilterCnt,
                                                     sPhysicalFilterArr,
                                                     &sPhysicalFilter,
                                                     aEnv )
                                 == STL_SUCCESS );
                        sPhysicalFilter->mOffset = sFilterExpr->mOffset;
                    }

                    break;
                }
                    
            case KNL_BUILTIN_FUNC_IS_EQUAL :
            case KNL_BUILTIN_FUNC_IS_NOT_EQUAL :
            case KNL_BUILTIN_FUNC_IS_LESS_THAN :
            case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN :
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :
                {
                    if( ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                        ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                    {
                        if( ( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                            ( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                        {
                            /* physical function이 없으면 Logical Filter로 분류 */

                            STL_TRY( qloGetDataType( aSQLString,
                                                     sFuncCode->mArgs[0],
                                                     aBindContext,
                                                     & sDataType1,
                                                     aRegionMem,
                                                     aEnv )
                                     == STL_SUCCESS );

                            STL_TRY( qloGetDataType( aSQLString,
                                                     sFuncCode->mArgs[1],
                                                     aBindContext,
                                                     & sDataType2,
                                                     aRegionMem,
                                                     aEnv )
                                     == STL_SUCCESS );
                                
                            qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                                   sDataType1,
                                                   sDataType2,
                                                   & sPhysicalFuncPtr,
                                                   STL_FALSE );
                                
                            if( sPhysicalFuncPtr == NULL )
                            {
                                /* Logical Filter에 삽입 */
                                SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                                sPhysicalFilter = NULL;
                            }
                            else
                            {
                                /* Physical Filter에 삽입 */
                                SET_FILTER_EXPR( sPhysicalFilter, sFilterExpr );
                                sLogicalFilter = NULL;
                            }
                        }
                        else if( (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                                 (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_VALUE) ||
                                 (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                                 (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_OUTER_COLUMN) )
                        {
                            /* physical function이 없으면 Logical Filter로 분류 */
                            STL_TRY( qloGetDataType( aSQLString,
                                                     sFuncCode->mArgs[0],
                                                     aBindContext,
                                                     & sDataType1,
                                                     aRegionMem,
                                                     aEnv )
                                     == STL_SUCCESS );

                            STL_TRY( qloGetDataType( aSQLString,
                                                     sFuncCode->mArgs[1],
                                                     aBindContext,
                                                     & sDataType2,
                                                     aRegionMem,
                                                     aEnv )
                                     == STL_SUCCESS );

                            STL_TRY( qlnoGetCompareType( aSQLString,
                                                         sDataType1,
                                                         sDataType2,
                                                         STL_FALSE,
                                                         STL_TRUE,
                                                         sFilterExpr->mPosition,
                                                         &sCompareType,
                                                         aEnv )
                                     == STL_SUCCESS );

                            if( sCompareType->mLeftType == sDataType1 )
                            {
                                /* Physical Filter에 삽입 */
                                SET_FILTER_EXPR( sPhysicalFilter, sFilterExpr );
                                sLogicalFilter = NULL;
                            }
                            else
                            {
                                /* Logical Filter에 삽입 */
                                SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                                sPhysicalFilter = NULL;
                            }
                        }
                        else
                        {
                            if( qloIsExistColumnInExpr( sFuncCode->mArgs[1],
                                                        aRelationNode )
                                == STL_TRUE )
                            {
                                /* Logical Filter에 삽입 */
                                SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                                sPhysicalFilter = NULL;
                            }
                            else
                            {
                                /* physical function이 없으면 Logical Filter로 분류 */
                                STL_TRY( qloGetDataType( aSQLString,
                                                         sFuncCode->mArgs[0],
                                                         aBindContext,
                                                         & sDataType1,
                                                         aRegionMem,
                                                         aEnv )
                                         == STL_SUCCESS );

                                STL_TRY( qloGetDataType( aSQLString,
                                                         sFuncCode->mArgs[1],
                                                         aBindContext,
                                                         & sDataType2,
                                                         aRegionMem,
                                                         aEnv )
                                         == STL_SUCCESS );

                                STL_TRY( qlnoGetCompareType( aSQLString,
                                                             sDataType1,
                                                             sDataType2,
                                                             STL_FALSE,
                                                             STL_TRUE,
                                                             sFilterExpr->mPosition,
                                                             &sCompareType,
                                                             aEnv )
                                         == STL_SUCCESS );

                                if( sCompareType->mLeftType == sDataType1 )
                                {
                                    /* Physical Filter에 삽입 */
                                    SET_FILTER_EXPR( sPhysicalFilter, sFilterExpr );
                                    sLogicalFilter = NULL;
                                }
                                else
                                {
                                    /* Logical Filter에 삽입 */
                                    SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                                    sPhysicalFilter = NULL;
                                }
                            }
                        }
                    }
                    else if( (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                             (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_VALUE) ||
                             (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                             (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_OUTER_COLUMN))
                    {
                        if( ( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                            ( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                        {
                            /* physical function이 없으면 Logical Filter로 분류 */
                            STL_TRY( qloGetDataType( aSQLString,
                                                     sFuncCode->mArgs[0],
                                                     aBindContext,
                                                     & sDataType1,
                                                     aRegionMem,
                                                     aEnv )
                                     == STL_SUCCESS );

                            STL_TRY( qloGetDataType( aSQLString,
                                                     sFuncCode->mArgs[1],
                                                     aBindContext,
                                                     & sDataType2,
                                                     aRegionMem,
                                                     aEnv )
                                     == STL_SUCCESS );

                            STL_TRY( qlnoGetCompareType( aSQLString,
                                                         sDataType1,
                                                         sDataType2,
                                                         STL_TRUE,
                                                         STL_FALSE,
                                                         sFilterExpr->mPosition,
                                                         &sCompareType,
                                                         aEnv )
                                     == STL_SUCCESS );

                            if( sCompareType->mRightType == sDataType2 )
                            {
                                /* Physical Filter에 삽입 */
                                SET_FILTER_EXPR( sPhysicalFilter, sFilterExpr );
                                sLogicalFilter = NULL;
                            }
                            else
                            {
                                /* Logical Filter에 삽입 */
                                SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                                sPhysicalFilter = NULL;
                            }
                        }
                        else
                        {
                            /* Logical Filter에 삽입 */
                            SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                            sPhysicalFilter = NULL;
                        }
                    }
                    else
                    {
                        if( ( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                            ( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                        {
                            if( qloIsExistColumnInExpr( sFuncCode->mArgs[0],
                                                        aRelationNode )
                                == STL_TRUE )
                            {
                                /* Logical Filter에 삽입 */
                                SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                                sPhysicalFilter = NULL;
                            }
                            else
                            {
                                /* physical function이 없으면 Logical Filter로 분류 */
                                STL_TRY( qloGetDataType( aSQLString,
                                                         sFuncCode->mArgs[0],
                                                         aBindContext,
                                                         & sDataType1,
                                                         aRegionMem,
                                                         aEnv )
                                         == STL_SUCCESS );

                                STL_TRY( qloGetDataType( aSQLString,
                                                         sFuncCode->mArgs[1],
                                                         aBindContext,
                                                         & sDataType2,
                                                         aRegionMem,
                                                         aEnv )
                                         == STL_SUCCESS );

                                STL_TRY( qlnoGetCompareType( aSQLString,
                                                             sDataType1,
                                                             sDataType2,
                                                             STL_TRUE,
                                                             STL_FALSE,
                                                             sFilterExpr->mPosition,
                                                             &sCompareType,
                                                             aEnv )
                                         == STL_SUCCESS );

                                if( sCompareType->mRightType == sDataType2 )
                                {
                                    /* Physical Filter에 삽입 */
                                    SET_FILTER_EXPR( sPhysicalFilter, sFilterExpr );
                                    sLogicalFilter = NULL;
                                }
                                else
                                {
                                    /* Logical Filter에 삽입 */
                                    SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                                    sPhysicalFilter = NULL;
                                }
                            }
                        }
                        else
                        {
                            /* Logical Filter에 삽입 */
                            SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                            sPhysicalFilter = NULL;
                        }
                    }
                       
                    break;
                }

            case KNL_BUILTIN_FUNC_IS_NULL :
            case KNL_BUILTIN_FUNC_IS_NOT_NULL :
                {
                    if( ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                        ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                    {
                        /* Physical Filter에 삽입 */
                        SET_FILTER_EXPR( sPhysicalFilter, sFilterExpr );
                        sLogicalFilter = NULL;
                    }
                    else
                    {
                        /* Logical Filter에 삽입 */
                        SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                        sPhysicalFilter = NULL;
                    }
                       
                    break;
                }
                    
            case KNL_BUILTIN_FUNC_NOT :
            case KNL_BUILTIN_FUNC_IS :
            case KNL_BUILTIN_FUNC_IS_NOT :
            case KNL_BUILTIN_FUNC_IS_UNKNOWN :
            case KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN :
                {
                    if( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN )
                    {
                        if( ellGetColumnDBType( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle )
                            == DTL_TYPE_BOOLEAN )
                        {
                            /* Physical Filter에 삽입 */
                            SET_FILTER_EXPR( sPhysicalFilter, sFilterExpr );
                            sLogicalFilter = NULL;
                        }
                        else
                        {
                            /* Logical Filter에 삽입 */
                            SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                            sPhysicalFilter = NULL;
                        }
                    }
                    else if( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN )
                    {
                        STL_TRY( qloGetDataType( aSQLString,
                                                 sFuncCode->mArgs[0],
                                                 aBindContext,
                                                 & sDataType1,
                                                 aRegionMem,
                                                 aEnv )
                                 == STL_SUCCESS );

                        if( sDataType1 == DTL_TYPE_BOOLEAN )
                        {
                            /* Physical Filter에 삽입 */
                            SET_FILTER_EXPR( sPhysicalFilter, sFilterExpr );
                            sLogicalFilter = NULL;
                        }
                        else
                        {
                            /* Logical Filter에 삽입 */
                            SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                            sPhysicalFilter = NULL;
                        }
                    }
                    else
                    {
                        /* Logical Filter에 삽입 */
                        SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                        sPhysicalFilter = NULL;
                    }

                    break;
                }

            default :
                {
                    /* Logical Filter에 삽입 */
                    SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                    sPhysicalFilter = NULL;
                    break;
                }
        }
    }
    else if( ( sFilterExpr->mType == QLV_EXPR_TYPE_COLUMN ) ||
             ( sFilterExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
    {
        STL_TRY( qloGetDataType( aSQLString,
                                 sFilterExpr,
                                 aBindContext,
                                 & sDataType1,
                                 aRegionMem,
                                 aEnv )
                 == STL_SUCCESS );

        if( sDataType1 == DTL_TYPE_BOOLEAN )
        {
            /* Physical Filter에 삽입 */
            SET_FILTER_EXPR( sPhysicalFilter, sFilterExpr );
            sLogicalFilter = NULL;
        }
        else
        {
            /* Logical Filter에 삽입 */
            SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
            sPhysicalFilter = NULL;
        }
    }
    else if( sFilterExpr->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
    {
        sListFunc = sFilterExpr->mExpr.mListFunc;
        STL_DASSERT( ( sListFunc->mArgs[0]->mType == QLV_EXPR_TYPE_LIST_COLUMN ) &&
                     ( sListFunc->mArgCount <= 2 ) );

        /* EXISTS FUNCTION은 해당 코드에 오지 않는다. */
        STL_DASSERT( sListFunc->mArgCount > 1 );
            
        sListColLeft  = sListFunc->mArgs[1]->mExpr.mListCol;
        sListColRight  = sListFunc->mArgs[0]->mExpr.mListCol;
        STL_DASSERT( ( sListColLeft->mArgs[0]->mType == QLV_EXPR_TYPE_ROW_EXPR ) &&
                     ( sListColRight->mArgs[0]->mType == QLV_EXPR_TYPE_ROW_EXPR ) );

        sRowExpr  = sListColLeft->mArgs[0]->mExpr.mRowExpr;
            
        if( ( sRowExpr->mArgCount == 1 ) &&
            ( ( sListFunc->mFuncId >= KNL_LIST_FUNC_START_ID ) &&
              ( sListFunc->mFuncId <= KNL_LIST_FUNC_END_ID ) ) )
        {
            if( sListFunc->mArgs[0]->mIterationTime != DTL_ITERATION_TIME_FOR_EACH_EXPR )
            {
                if( ( sRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                    ( sRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                {
                    /* physical function이 없으면 Logical Filter로 분류 */
                    for( i = 0; i < sListColLeft->mArgCount; i ++ )
                    {
                        STL_TRY( qloGetDataType( aSQLString,
                                                 sListColLeft->mArgs[i]->mExpr.mRowExpr->mArgs[0],
                                                 aBindContext,
                                                 & sDataType1,
                                                 aRegionMem,
                                                 aEnv )
                                 == STL_SUCCESS );

                        STL_TRY( qloGetDataType( aSQLString,
                                                 sListColRight->mArgs[i]->mExpr.mRowExpr->mArgs[0],
                                                 aBindContext,
                                                 & sDataType2,
                                                 aRegionMem,
                                                 aEnv )
                                 == STL_SUCCESS );

                        STL_TRY( qlnoGetCompareType( aSQLString,
                                                     sDataType1,
                                                     sDataType2,
                                                     STL_FALSE,
                                                     STL_TRUE,
                                                     sFilterExpr->mPosition,
                                                     &sCompareType,
                                                     aEnv )
                                 == STL_SUCCESS );

                        if( sCompareType->mLeftType != sDataType1 )
                        {
                            SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                            sPhysicalFilter = NULL;
                            break;
                        }
                    }
                        
                    if( i == sListColLeft->mArgCount )
                    {
                        /* Physical Filter에 삽입 */
                        SET_FILTER_EXPR( sPhysicalFilter, sFilterExpr );
                        sLogicalFilter = NULL;
                    }
                }
                else
                {
                    /* Logical Filter에 삽입 */
                    SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                    sPhysicalFilter = NULL;
                }
            }
            else
            {
                /* Logical Filter에 삽입 */
                SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
                sPhysicalFilter = NULL;
            }
        }
        else
        {
            /* Logical Filter에 삽입 */
            SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
            sPhysicalFilter = NULL;
        }
    }
    else
    {
        /* Logical Filter에 삽입 */
        SET_FILTER_EXPR( sLogicalFilter, sFilterExpr );
        sPhysicalFilter = NULL;
    }

    /**
     * OUTPUT 설정
     */
    
    *aLogicalFilter   = sLogicalFilter;
    *aPhysicalFilter  = sPhysicalFilter;

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}
            

void qloGetPhysicalFuncPtr( knlBuiltInFunc     aExprFuncId,
                            dtlDataType        aType1,
                            dtlDataType        aType2,
                            dtlPhysicalFunc  * aPhysicalFuncPtr,
                            stlBool            aReverse )
{
    dtlPhysicalFunc  sFuncPtr = NULL;
    
    if( aReverse == STL_FALSE )
    {
        switch( aExprFuncId )
        {
            case KNL_BUILTIN_FUNC_IS_EQUAL :                   /**< IS EQUAL */
                {
                    sFuncPtr = dtlPhysicalFuncArg2FuncList[ aType1 ][ aType2 ][ DTL_PHYSICAL_FUNC_IS_EQUAL ];
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_NOT_EQUAL :               /**< IS NOT EQUAL */
                {
                    sFuncPtr = dtlPhysicalFuncArg2FuncList[ aType1 ][ aType2 ][ DTL_PHYSICAL_FUNC_IS_NOT_EQUAL ];
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_LESS_THAN :               /**< IS LESS THAN */
                {
                    sFuncPtr = dtlPhysicalFuncArg2FuncList[ aType1 ][ aType2 ][ DTL_PHYSICAL_FUNC_IS_LESS_THAN ];
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :         /**< IS LESS_THAN EQUAL */
                {
                    sFuncPtr = dtlPhysicalFuncArg2FuncList[ aType1 ][ aType2 ][ DTL_PHYSICAL_FUNC_IS_LESS_THAN_EQUAL ];
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN :            /**< IS GREATER THAN */
                {
                    sFuncPtr = dtlPhysicalFuncArg2FuncList[ aType1 ][ aType2 ][ DTL_PHYSICAL_FUNC_IS_GREATER_THAN ];
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :      /**< IS GREATER THAN EQUAL */
                {
                    sFuncPtr = dtlPhysicalFuncArg2FuncList[ aType1 ][ aType2 ][ DTL_PHYSICAL_FUNC_IS_GREATER_THAN_EQUAL ];
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_NULL :                    /**< IS NULL */
                {
                    sFuncPtr = dtlPhysicalFuncArg1FuncList[ DTL_PHYSICAL_FUNC_IS_NULL - DTL_PHYSICAL_FUNC_IS ];
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_NOT_NULL :                /**< IS NOT NULL */
                {
                    sFuncPtr = dtlPhysicalFuncArg1FuncList[ DTL_PHYSICAL_FUNC_IS_NOT_NULL - DTL_PHYSICAL_FUNC_IS ];
                    break;
                }
            case KNL_BUILTIN_FUNC_IS :                         /**< IS */
                {
                    if( aType1 == DTL_TYPE_BOOLEAN )
                    {
                        sFuncPtr = dtlPhysicalFuncArg1FuncList[ DTL_PHYSICAL_FUNC_IS - DTL_PHYSICAL_FUNC_IS ];
                    }
                    else
                    {
                        sFuncPtr = NULL;
                    }
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_NOT :                     /**< IS NOT */
                {
                    if( aType1 == DTL_TYPE_BOOLEAN )
                    {
                        sFuncPtr = dtlPhysicalFuncArg1FuncList[ DTL_PHYSICAL_FUNC_IS_NOT - DTL_PHYSICAL_FUNC_IS ];
                    }
                    else
                    {
                        sFuncPtr = NULL;
                    }
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_UNKNOWN :                 /**< IS UNKNOWN */
                {
                    if( aType1 == DTL_TYPE_BOOLEAN )
                    {
                        sFuncPtr = dtlPhysicalFuncArg1FuncList[ DTL_PHYSICAL_FUNC_IS_NULL - DTL_PHYSICAL_FUNC_IS ];
                    }
                    else
                    {
                        sFuncPtr = NULL;
                    }
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_NOT_UNKNOWN :             /**< IS NOT UNKNOWN */
                {
                    if( aType1 == DTL_TYPE_BOOLEAN )
                    {
                        sFuncPtr = dtlPhysicalFuncArg1FuncList[ DTL_PHYSICAL_FUNC_IS_NOT_NULL - DTL_PHYSICAL_FUNC_IS ];
                    }
                    else
                    {
                        sFuncPtr = NULL;
                    }
                    break;
                }
            case KNL_BUILTIN_FUNC_NOT :                        /**< NOT */
                {
                    if( aType1 == DTL_TYPE_BOOLEAN )
                    {
                        sFuncPtr = dtlPhysicalFuncArg1FuncList[ DTL_PHYSICAL_FUNC_NOT - DTL_PHYSICAL_FUNC_IS ];
                    }
                    else
                    {
                        sFuncPtr = NULL;
                    }
                    break;
                }
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
    }
    else
    {
        switch( aExprFuncId )
        {
            case KNL_BUILTIN_FUNC_IS_EQUAL :                   /**< IS EQUAL */
                {
                    sFuncPtr = dtlPhysicalFuncArg2FuncList[ aType2 ][ aType1 ][ DTL_PHYSICAL_FUNC_IS_EQUAL ];
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_NOT_EQUAL :               /**< IS NOT EQUAL */
                {
                    sFuncPtr = dtlPhysicalFuncArg2FuncList[ aType2 ][ aType1 ][ DTL_PHYSICAL_FUNC_IS_NOT_EQUAL ];
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_LESS_THAN :               /**< IS LESS THAN */
                {
                    sFuncPtr = dtlPhysicalFuncArg2FuncList[ aType2 ][ aType1 ][ DTL_PHYSICAL_FUNC_IS_GREATER_THAN ];
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :         /**< IS LESS_THAN EQUAL */
                {
                    sFuncPtr = dtlPhysicalFuncArg2FuncList[ aType2 ][ aType1 ][ DTL_PHYSICAL_FUNC_IS_GREATER_THAN_EQUAL ];
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN :            /**< IS GREATER THAN */
                {
                    sFuncPtr = dtlPhysicalFuncArg2FuncList[ aType2 ][ aType1 ][ DTL_PHYSICAL_FUNC_IS_LESS_THAN ];
                    break;
                }
            case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :      /**< IS GREATER THAN EQUAL */
                {
                    sFuncPtr = dtlPhysicalFuncArg2FuncList[ aType2 ][ aType1 ][ DTL_PHYSICAL_FUNC_IS_LESS_THAN_EQUAL ];
                    break;
                }
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
    }

    *aPhysicalFuncPtr = sFuncPtr;
}


void qloRemoveExprAND( qlvInitExpression   * aExpr,
                       qlvInitExpression  ** aExprList,
                       stlInt32            * aExprCount )
{
    qlvInitExpression  * sExpr = aExpr;
    qlvInitFunction    * sFuncCode;
    stlInt32             i;

    if( sExpr->mType == QLV_EXPR_TYPE_FUNCTION )
    {
        sFuncCode = sExpr->mExpr.mFunction;

        if( sFuncCode->mFuncId == KNL_BUILTIN_FUNC_AND )
        {
            for( i = 0; i < sFuncCode->mArgCount; i++ )
            {
                qloRemoveExprAND( sFuncCode->mArgs[i],
                                  aExprList,
                                  aExprCount );
            }
        }
        else
        {
            aExprList[ *aExprCount ] = sExpr;
            (*aExprCount)++;
            return;
        }
    }
    else
    {
        aExprList[ *aExprCount ] = sExpr;
        (*aExprCount)++;
        return;
    }
}


/**
 * @brief RowIdScan을 위한 expression과 Filter expression을 얻는다.
 * @param[in]  aWhereExpr        Where Expression
 * @param[in]  aRegionMem        Region Memory
 * @param[out] aRowIdScanExpr    RowId Access를 위한 RowId Expr
 * @param[out] aFilterExpr       Filter Expr
 * @param[in]  aEnv              Environment
 */
stlStatus qloGetRowIdScanExprAndFilter( qlvInitExpression   * aWhereExpr,
                                        knlRegionMem        * aRegionMem,
                                        qlvInitExpression  ** aRowIdScanExpr,
                                        qlvInitExpression  ** aFilterExpr,
                                        qllEnv              * aEnv )
{
    qlvInitExpression * sRowIdScanExpr      = NULL;
    qlvInitExpression * sFilterExpr         = NULL;

    qlvRefExprList    * sRowIdScanExprList  = NULL;
    qlvRefExprList    * sFilterExprList     = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aWhereExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowIdScanExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFilterExpr != NULL, QLL_ERROR_STACK(aEnv) );

    /* RowId Scan Expression List */
    STL_TRY( qlvCreateRefExprList( & sRowIdScanExprList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /* Filter Expression List */
    STL_TRY( qlvCreateRefExprList( & sFilterExprList,
                                   aRegionMem,
                                   aEnv )
             == STL_SUCCESS );

    /**
     * RowId Scan Expression 과  Filter Expression  분류
     */

    STL_TRY( qloClassifyRowIdScanExprAndFilter( aWhereExpr,
                                                aRegionMem,
                                                sRowIdScanExprList,
                                                sFilterExprList,
                                                aEnv )
             == STL_SUCCESS );

    /* RowId Scan Expression 구성 */

    STL_DASSERT( sRowIdScanExprList->mCount == 1 );
    sRowIdScanExpr = sRowIdScanExprList->mHead->mExprPtr;

    /* Filter Expression 구성 */

    STL_TRY( qloLinkRangeExpr( & sFilterExpr,
                               sFilterExprList,
                               aRegionMem,
                               aEnv )
             == STL_SUCCESS );
    
    *aRowIdScanExpr = sRowIdScanExpr;
    *aFilterExpr = sFilterExpr;
    
    return STL_SUCCESS;    

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief RowIdScan을 위한 expression과 Filter expression을 분류한다.
 * @param[in]  aWhereExpr          Where Expression
 * @param[in]  aRegionMem          Region Memory
 * @param[out] aRowIdScanExprList  RowId Access를 위한 RowId Expr
 * @param[out] aFilterExprList     Filter Expr
 * @param[in]  aEnv                Environment
 */
stlStatus qloClassifyRowIdScanExprAndFilter( qlvInitExpression  * aWhereExpr,
                                             knlRegionMem       * aRegionMem,
                                             qlvRefExprList     * aRowIdScanExprList,
                                             qlvRefExprList     * aFilterExprList,
                                             qllEnv             * aEnv )
{
    qlvInitExpression  * sWhereExpr      = NULL;
    qlvInitFunction    * sInitFunction   = NULL;
    stlInt32             i;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aWhereExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowIdScanExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFilterExprList != NULL, QLL_ERROR_STACK(aEnv) );

    sWhereExpr    = aWhereExpr;

    if( sWhereExpr->mType == QLV_EXPR_TYPE_FUNCTION )
    {    
        sInitFunction = sWhereExpr->mExpr.mFunction;
        
        if( sInitFunction->mFuncId == KNL_BUILTIN_FUNC_AND )
        {
            /**
             * @todo AND Expression의 Argument Count를 가변 변경
             */
            
            STL_PARAM_VALIDATE( sInitFunction->mArgCount >= 2, QLL_ERROR_STACK(aEnv) );

            for( i = 0; i < sInitFunction->mArgCount; i++ )
            {
                STL_TRY( qloClassifyRowIdScanExprAndFilter( sInitFunction->mArgs[i],
                                                            aRegionMem,
                                                            aRowIdScanExprList,
                                                            aFilterExprList,
                                                            aEnv )
                         == STL_SUCCESS );
            }
        }
        else if( sInitFunction->mFuncId == KNL_BUILTIN_FUNC_IS_EQUAL )
        {
            if( sInitFunction->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_ROWID_COLUMN )
            {
                /**
                 * 예) ROWID = CONSTANT_VALUE  또는  CONSTANT_VALUE = ROWID  인 경우
                 *     ----------------------
                 *
                 *  CONSTANT_VALUE 에는 아래와 같은 TYPE 이 올 수 있다.
                 *  QLV_EXPR_TYPE_OUTER_COLUMN
                 *  QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT
                 *  QLV_EXPR_TYPE_VALUE
                 *  QLV_EXPR_TYPE_BIND_PARAM
                 *  QLV_EXPR_TYPE_FUNCTION
                 *  QLV_EXPR_TYPE_CAST
                 *  QLV_EXPR_TYPE_PSEUDO_COLUMN
                 *  QLV_EXPR_TYPE_CASE_EXPR
                 *
                 *  그러나,
                 *  QLV_EXPR_TYPE_OUTER_COLUMN 와 QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT 을 
                 *  제외한 TYPE 들은 모두 Rewrite 시 QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT 로 변경되어 있음.
                 *
                 *  따라서, 
                 *  QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT 과 QLV_EXPR_TYPE_OUTER_COLUMN 만 확인하면 됨.
                 */
                
                if( ( sInitFunction->mArgs[ 1 ]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT ) ||
                    ( sInitFunction->mArgs[ 1 ]->mType == QLV_EXPR_TYPE_VALUE ) ||
                    ( sInitFunction->mArgs[ 1 ]->mType == QLV_EXPR_TYPE_REFERENCE ) ||
                    ( sInitFunction->mArgs[ 1 ]->mType == QLV_EXPR_TYPE_OUTER_COLUMN ) )
                {
                    if( aRowIdScanExprList->mCount == 0 )
                    {
                        STL_TRY( qlvAddExprToRefExprList( aRowIdScanExprList,
                                                          sWhereExpr,
                                                          STL_FALSE,
                                                          aRegionMem,
                                                          aEnv )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        STL_TRY( qlvAddExprToRefExprList( aFilterExprList,
                                                          sWhereExpr,
                                                          STL_FALSE,
                                                          aRegionMem,
                                                          aEnv )
                                 == STL_SUCCESS );                
                    }
                }
                else
                {
                    STL_TRY( qlvAddExprToRefExprList( aFilterExprList,
                                                      sWhereExpr,
                                                      STL_FALSE,
                                                      aRegionMem,
                                                      aEnv )
                             == STL_SUCCESS );                
                }                
            }
            else
            {
                if( sInitFunction->mArgs[ 1 ]->mType == QLV_EXPR_TYPE_ROWID_COLUMN )
                {
                    if( ( sInitFunction->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT ) ||
                        ( sInitFunction->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_VALUE ) ||
                        ( sInitFunction->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_REFERENCE ) ||
                        ( sInitFunction->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_OUTER_COLUMN ) )
                    {
                        /**
                         * 예) ROWID = CONSTANT_VALUE  또는  CONSTANT_VALUE = ROWID  인 경우
                         *                                   ----------------------
                         */

                        if( aRowIdScanExprList->mCount == 0 )
                        {
                            STL_TRY( qlvAddExprToRefExprList( aRowIdScanExprList,
                                                              sWhereExpr,
                                                              STL_FALSE,
                                                              aRegionMem,
                                                              aEnv )
                                     == STL_SUCCESS );
                        }
                        else
                        {
                            STL_TRY( qlvAddExprToRefExprList( aFilterExprList,
                                                              sWhereExpr,
                                                              STL_FALSE,
                                                              aRegionMem,
                                                              aEnv )
                                     == STL_SUCCESS );                
                        }
                        
                    }
                    else
                    {
                        STL_TRY( qlvAddExprToRefExprList( aFilterExprList,
                                                          sWhereExpr,
                                                          STL_FALSE,
                                                          aRegionMem,
                                                          aEnv )
                                 == STL_SUCCESS );                                        
                    }                                    
                }
                else
                {
                    STL_TRY( qlvAddExprToRefExprList( aFilterExprList,
                                                      sWhereExpr,
                                                      STL_FALSE,
                                                      aRegionMem,
                                                      aEnv )
                             == STL_SUCCESS );                                    
                }
            }
        }
        else
        {
            STL_TRY( qlvAddExprToRefExprList( aFilterExprList,
                                              sWhereExpr,
                                              STL_FALSE,
                                              aRegionMem,
                                              aEnv )
                     == STL_SUCCESS );                
        }
    }
    else
    {
        STL_TRY( qlvAddExprToRefExprList( aFilterExprList,
                                          sWhereExpr,
                                          STL_FALSE,
                                          aRegionMem,
                                          aEnv )
                 == STL_SUCCESS );                
    }
    
    return STL_SUCCESS;    

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Statement Function Block List에서 동일한 Func ID를 가지는 Value Block 찾기
 * @param[in]  aStmtValueBlock     Statement Function Value Block List
 * @param[in]  aFuncID             Built-In Function ID
 * @param[out] aFuncValueBlock     Function Value Block
 * @param[in]  aEnv                Environment
 */
stlStatus qloGetStmtFunctionBlock( knlValueBlockList   * aStmtValueBlock,
                                   knlBuiltInFunc        aFuncID,
                                   knlValueBlockList  ** aFuncValueBlock,
                                   qllEnv              * aEnv )
{
    knlValueBlockList  * sFuncValueBlock = aStmtValueBlock;
    

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( ( aFuncID >= 0 ) && ( aFuncID < KNL_BUILTIN_FUNC_MAX ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( gBuiltInFuncInfoArr[ aFuncID ].mIterationTime ==
                        DTL_ITERATION_TIME_FOR_EACH_STATEMENT,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFuncValueBlock != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * 동일한 Func ID를 가지는 Value Block 찾기
     */

    while( sFuncValueBlock != NULL )
    {
        /* Value Block의 mColumnOrder 필드에 Func ID 정보를 저장한다. */
        if( sFuncValueBlock->mColumnOrder == aFuncID )
        {
            break;
        }
        else
        {
            sFuncValueBlock = sFuncValueBlock->mNext;
        }
    }

    
    /**
     * OUTPUT 설정
     */

    *aFuncValueBlock = sFuncValueBlock;
    
    
    return STL_SUCCESS;    

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Expression 내에 주어진 Node에 대한 Column이 존재하는지 여부 판단
 * @param[in]   aExpr     Expression
 * @param[out]  aNode     Init Node
 */
stlBool qloIsExistColumnInExpr( qlvInitExpression  * aExpr,
                                qlvInitNode        * aNode )
{
    stlInt32  sLoop;
    
    switch( aExpr->mType )
    {
        case QLV_EXPR_TYPE_COLUMN :
            {
                STL_DASSERT( aExpr->mExpr.mColumn->mRelationNode == aNode );
                return STL_TRUE;
            }
        case QLV_EXPR_TYPE_INNER_COLUMN :
            {
                STL_DASSERT( aExpr->mExpr.mInnerColumn->mRelationNode == aNode );
                return STL_TRUE;
            }
        case QLV_EXPR_TYPE_ROWID_COLUMN :
            {
                STL_DASSERT( aExpr->mExpr.mRowIdColumn->mRelationNode == aNode );
                return STL_TRUE;
            }
        case QLV_EXPR_TYPE_FUNCTION :
            {
                qlvInitFunction  * sFunction = aExpr->mExpr.mFunction;

                switch( sFunction->mFuncId )
                {
                    case KNL_BUILTIN_FUNC_RANDOM :
                    case KNL_BUILTIN_FUNC_CLOCK_DATE :
                    case KNL_BUILTIN_FUNC_CLOCK_TIME :
                    case KNL_BUILTIN_FUNC_CLOCK_TIMESTAMP :
                    case KNL_BUILTIN_FUNC_CLOCK_LOCALTIME :
                    case KNL_BUILTIN_FUNC_CLOCK_LOCALTIMESTAMP :
                        return STL_TRUE;
                    default :
                        break;
                }
                
                for( sLoop = 0 ; sLoop < sFunction->mArgCount ; sLoop++ )
                {
                    if( qloIsExistColumnInExpr( sFunction->mArgs[ sLoop ],
                                                aNode )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                return STL_FALSE;
            }
        case QLV_EXPR_TYPE_CAST :
            {
                qlvInitTypeCast  * sCast = aExpr->mExpr.mTypeCast;
                for( sLoop = 0 ; sLoop < DTL_CAST_INPUT_ARG_CNT ; sLoop++ )
                {
                    if( qloIsExistColumnInExpr( sCast->mArgs[ sLoop ],
                                                aNode )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                return STL_FALSE;
            }
        case QLV_EXPR_TYPE_CASE_EXPR :
            {
                qlvInitCaseExpr  * sCase = aExpr->mExpr.mCaseExpr;
                for( sLoop = 0 ; sLoop < sCase->mCount ; sLoop++ )
                {
                    if( qloIsExistColumnInExpr( sCase->mWhenArr[ sLoop ],
                                                aNode )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }

                    if( qloIsExistColumnInExpr( sCase->mThenArr[ sLoop ],
                                                aNode )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                return qloIsExistColumnInExpr( sCase->mDefResult,
                                               aNode );
            }
        case QLV_EXPR_TYPE_LIST_FUNCTION :
            {
                qlvInitListFunc  * sListFunc = aExpr->mExpr.mListFunc;
                for( sLoop = 0 ; sLoop < sListFunc->mArgCount ; sLoop++ )
                {
                    if( qloIsExistColumnInExpr( sListFunc->mArgs[ sLoop ],
                                                aNode )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                return STL_FALSE;
            }
        case QLV_EXPR_TYPE_LIST_COLUMN :
            {
                qlvInitListCol  * sListCol = aExpr->mExpr.mListCol;
                for( sLoop = 0 ; sLoop < sListCol->mArgCount ; sLoop++ )
                {
                    if( qloIsExistColumnInExpr( sListCol->mArgs[ sLoop ],
                                                aNode )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                return STL_FALSE;
            }
        case QLV_EXPR_TYPE_ROW_EXPR :
            {
                qlvInitRowExpr  * sRowExpr = aExpr->mExpr.mRowExpr;
                for( sLoop = 0 ; sLoop < sRowExpr->mArgCount ; sLoop++ )
                {
                    if( qloIsExistColumnInExpr( sRowExpr->mArgs[ sLoop ],
                                                aNode )
                        == STL_TRUE )
                    {
                        return STL_TRUE;
                    }
                }
                return STL_FALSE;
            }
        default :
            {
                return STL_FALSE;
            }
    }
}


/**
 * @brief Physical Filter 수행을 위한 선처리 Expression 들에 대한 Expr Stack 구성
 * @param[in]  aDBCStmt              DBC statement
 * @param[in]  aSQLStmt              SQL statement
 * @param[in]  aQueryExprList        Query 단위 Expression 정보
 * @param[in]  aPhysicalFilterExpr   Physical Filter Expression
 * @param[out] aPreFilterExprStack   Physical Filter Expression Stack
 * @param[in]  aEnv                  Environment
 */
stlStatus qloMakePhysicalFilterStack( qllDBCStmt          * aDBCStmt,
                                      qllStatement        * aSQLStmt,
                                      qloInitExprList     * aQueryExprList,
                                      qlvInitExpression   * aPhysicalFilterExpr,
                                      knlExprStack       ** aPreFilterExprStack,
                                      qllEnv              * aEnv )
{
    knlExprStack       * sFilterStack    = NULL;
    knlExprStack       * sPreFilterStack = NULL;
    knlExprStack       * sOutputStack    = NULL;

    qlvInitFunction    * sFunction       = NULL;
    qlvInitFunction    * sAndFuncExpr    = NULL;
    knlExprEntry       * sExprEntry      = NULL;
    qloDBType          * sExprDBType     = NULL;
    qlvInitFunction      sTmpFunc;
    dtlDataType          sDBType         = DTL_TYPE_NA;

    const dtlCompareType    * sCompareType;
    
    qlvInitExpression  * sCodeExpr       = NULL;
    qlvInitListFunc    * sListFunc       = NULL;
    qlvInitListCol     * sListColLeft    = NULL;
    qlvInitListCol     * sListColRight   = NULL;
    qlvInitRowExpr     * sRowExpr        = NULL;
    
    stlInt32             sFunctionCnt    = 0;
    stlInt32             sLoop           = 0;
    stlUInt16            sIdx            = 0;
    stlUInt32            sPrevEntryCnt   = 0;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPhysicalFilterExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPreFilterExprStack != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Filter Expression Code Stack 공간 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( knlExprStack ),
                                (void **) & sFilterStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Filter Expression Stack 생성
     */

    STL_TRY( knlExprInit( sFilterStack,
                          aPhysicalFilterExpr->mIncludeExprCnt,
                          & QLL_CANDIDATE_MEM( aEnv ),
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    
    /**
     * Prepare Filter Expression Stack 공간 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( knlExprStack ),
                                (void **) & sPreFilterStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Prepare Filter Expression Stack 초기화
     */
        
    STL_TRY( knlExprInit( sPreFilterStack,
                          aPhysicalFilterExpr->mIncludeExprCnt + 2,
                          & QLL_CANDIDATE_MEM( aEnv ),
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * Physical Filter를 구성하는 Expression에 대한 DB Type 정보 설정
     *  <BR> Physical Filter를 위한 임시 Predicate 생성
     */

    sExprDBType = aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType;

    if( aPhysicalFilterExpr->mType == QLV_EXPR_TYPE_FUNCTION )
    {
        sAndFuncExpr = aPhysicalFilterExpr->mExpr.mFunction;
                
        if( sAndFuncExpr->mFuncId == KNL_BUILTIN_FUNC_AND )
        {
            sFunctionCnt = sAndFuncExpr->mArgCount;
        }
        else
        {
            sFunctionCnt = 1;

            sTmpFunc.mFuncId = KNL_BUILTIN_FUNC_AND;
            sTmpFunc.mArgCount = 1;
            sTmpFunc.mArgs = & aPhysicalFilterExpr;

            sAndFuncExpr = & sTmpFunc;
        }
            
        for( sLoop = 0 ; sLoop < sAndFuncExpr->mArgCount ; sLoop++ )
        {
            /**
             * Filter Stack 구성
             */
            
            if( sAndFuncExpr->mArgs[ sLoop ]->mType == QLV_EXPR_TYPE_FUNCTION )
            {
                sFunction = sAndFuncExpr->mArgs[ sLoop ]->mExpr.mFunction;

                if( sFunction->mArgCount > 1 )
                {
                    STL_DASSERT( sFunction->mArgCount == 2 );
                    
                    if( ( sFunction->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                        ( sFunction->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                    {
                        switch( sFunction->mArgs[ 1 ]->mType )
                        {
                            case QLV_EXPR_TYPE_COLUMN :
                            case QLV_EXPR_TYPE_INNER_COLUMN :
                                break;
                            default :
                                sCodeExpr = sFunction->mArgs[ 0 ];
                                STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                                    sFunction->mArgs[ 0 ],
                                                                    aSQLStmt->mBindContext,
                                                                    sFilterStack,
                                                                    aQueryExprList->mStmtExprList->mConstExprStack,
                                                                    aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                                    & sExprEntry,
                                                                    & QLL_CANDIDATE_MEM( aEnv ),
                                                                    aEnv )
                                         == STL_SUCCESS );                            

                                sCodeExpr = sFunction->mArgs[ 1 ];
                                STL_TRY( qloAddExprStackEntry( aSQLStmt->mRetrySQL,
                                                               sFunction->mArgs[ 1 ],
                                                               aSQLStmt->mBindContext,
                                                               sPreFilterStack,
                                                               aQueryExprList->mStmtExprList->mConstExprStack,
                                                               sPreFilterStack->mEntryCount + 1,
                                                               KNL_BUILTIN_FUNC_INVALID,
                                                               aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                               & sExprEntry,
                                                               QLL_CODE_PLAN( aDBCStmt ),
                                                               aEnv )
                                         == STL_SUCCESS );

                                if( sFunction->mFuncId != KNL_BUILTIN_FUNC_LIKE )
                                {
                                    STL_TRY( qlnoGetCompareType( aSQLStmt->mRetrySQL,
                                                                 sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mDBType,
                                                                 sExprDBType[ sFunction->mArgs[ 1 ]->mOffset ].mDBType,
                                                                 STL_FALSE,
                                                                 STL_TRUE,
                                                                 sAndFuncExpr->mArgs[ sLoop ]->mPosition,
                                                                 &sCompareType,
                                                                 aEnv )
                                             == STL_SUCCESS );

                                    if( sCompareType->mRightType
                                        == sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mDBType )
                                    {
                                        sDBType = sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mDBType;

                                        sCodeExpr = sFunction->mArgs[ 1 ];
                                        STL_TRY( qloCastExprDBType(
                                                     sFunction->mArgs[ 1 ],
                                                     aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                     sDBType,
                                                     gResultDataTypeDef[ sDBType ].mArgNum1,
                                                     gResultDataTypeDef[ sDBType ].mArgNum2,
                                                     sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mStringLengthUnit,
                                                     sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mIntervalIndicator,
                                                     STL_TRUE,
                                                     STL_FALSE,
                                                     aEnv )
                                                 == STL_SUCCESS );                                    
                                    }
                                    else
                                    {
                                        sDBType = sCompareType->mRightType;
                                
                                        sCodeExpr = sFunction->mArgs[ 1 ];
                                        STL_TRY( qloCastExprDBType( sFunction->mArgs[ 1 ],
                                                                    aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                                    sDBType,
                                                                    gResultDataTypeDef[ sDBType ].mArgNum1,
                                                                    gResultDataTypeDef[ sDBType ].mArgNum2,
                                                                    gResultDataTypeDef[ sDBType ].mStringLengthUnit,
                                                                    gResultDataTypeDef[ sDBType ].mIntervalIndicator,
                                                                    STL_TRUE,
                                                                    STL_FALSE,
                                                                    aEnv )
                                                 == STL_SUCCESS );                                    
                                    }
                                }
                                    
                                sCodeExpr = sAndFuncExpr->mArgs[ sLoop ];
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
                                 * Prepare Filter Stack에서 Internal Cast가 없는 Value Entry 제거
                                 */
        
                                if( ( sPreFilterStack->mEntryCount - sPrevEntryCnt ) == 1 )
                                {
                                    if( ( sPreFilterStack->mEntries[ sPreFilterStack->mEntryCount ].mExprType
                                          == KNL_EXPR_TYPE_VALUE ) &&
                                        ( sExprDBType[ sPreFilterStack->mEntries
                                                       [ sPreFilterStack->mEntryCount ].mExpr.mValueInfo.mOffset ].mIsNeedCast
                                          == STL_FALSE ) )
                                    {
                                        sPreFilterStack->mEntryCount = sPrevEntryCnt;
                                    }
                                    else
                                    {
                                        sPrevEntryCnt = sPreFilterStack->mEntryCount;
                                    }
                                }
                                else
                                {
                                    sPrevEntryCnt = sPreFilterStack->mEntryCount;
                                }

                                
                                continue;
                        }
                    }
                    else
                    {
                        STL_DASSERT( ( sFunction->mArgs[ 1 ]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                                     ( sFunction->mArgs[ 1 ]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );

                        sCodeExpr = sFunction->mArgs[ 1 ];
                        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                            sFunction->mArgs[ 1 ],
                                                            aSQLStmt->mBindContext,
                                                            sFilterStack,
                                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                            & sExprEntry,
                                                            & QLL_CANDIDATE_MEM( aEnv ),
                                                            aEnv )
                                 == STL_SUCCESS );                            

                        sCodeExpr = sFunction->mArgs[ 0 ];
                        STL_TRY( qloAddExprStackEntry( aSQLStmt->mRetrySQL,
                                                       sFunction->mArgs[ 0 ],
                                                       aSQLStmt->mBindContext,
                                                       sPreFilterStack,
                                                       aQueryExprList->mStmtExprList->mConstExprStack,
                                                       sPreFilterStack->mEntryCount + 1,
                                                       KNL_BUILTIN_FUNC_INVALID,
                                                       aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                       & sExprEntry,
                                                       QLL_CODE_PLAN( aDBCStmt ),
                                                       aEnv )
                                 == STL_SUCCESS );

                        if( sFunction->mFuncId != KNL_BUILTIN_FUNC_LIKE )
                        {
                            STL_TRY( qlnoGetCompareType( aSQLStmt->mRetrySQL,
                                                         sExprDBType[ sFunction->mArgs[ 1 ]->mOffset ].mDBType,
                                                         sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mDBType,
                                                         STL_FALSE,
                                                         STL_TRUE,
                                                         sAndFuncExpr->mArgs[ sLoop ]->mPosition,
                                                         &sCompareType,
                                                         aEnv )
                                     == STL_SUCCESS );

                            if( sCompareType->mRightType
                                == sExprDBType[ sFunction->mArgs[ 1 ]->mOffset ].mDBType )
                            {
                                sDBType = sExprDBType[ sFunction->mArgs[ 1 ]->mOffset ].mDBType;

                                sCodeExpr = sFunction->mArgs[ 0 ];
                                STL_TRY( qloCastExprDBType( sFunction->mArgs[ 0 ],
                                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                            sDBType,
                                                            gResultDataTypeDef[ sDBType ].mArgNum1,
                                                            gResultDataTypeDef[ sDBType ].mArgNum2,
                                                            sExprDBType[ sFunction->mArgs[ 1 ]->mOffset ].mStringLengthUnit,
                                                            sExprDBType[ sFunction->mArgs[ 1 ]->mOffset ].mIntervalIndicator,
                                                            STL_TRUE,
                                                            STL_FALSE,
                                                            aEnv )
                                         == STL_SUCCESS );                                    
                            }
                            else
                            {
                                sDBType = sCompareType->mRightType;

                                sCodeExpr = sFunction->mArgs[ 0 ];
                                STL_TRY( qloCastExprDBType( sFunction->mArgs[ 0 ],
                                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                            sDBType,
                                                            gResultDataTypeDef[ sDBType ].mArgNum1,
                                                            gResultDataTypeDef[ sDBType ].mArgNum2,
                                                            gResultDataTypeDef[ sDBType ].mStringLengthUnit,
                                                            gResultDataTypeDef[ sDBType ].mIntervalIndicator,
                                                            STL_TRUE,
                                                            STL_FALSE,
                                                            aEnv )
                                         == STL_SUCCESS );                                    
                            }
                        }
                            
                        sCodeExpr = sAndFuncExpr->mArgs[ sLoop ];
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
                         * Prepare Filter Stack에서 Internal Cast가 없는 Value Entry 제거
                         */
        
                        if( ( sPreFilterStack->mEntryCount - sPrevEntryCnt ) == 1 )
                        {
                            if( ( sPreFilterStack->mEntries[ sPreFilterStack->mEntryCount ].mExprType
                                  == KNL_EXPR_TYPE_VALUE ) &&
                                ( sExprDBType[ sPreFilterStack->mEntries
                                               [ sPreFilterStack->mEntryCount ].mExpr.mValueInfo.mOffset ].mIsNeedCast
                                  == STL_FALSE ) )
                            {
                                sPreFilterStack->mEntryCount = sPrevEntryCnt;
                            }
                            else
                            {
                                sPrevEntryCnt = sPreFilterStack->mEntryCount;
                            }
                        }
                        else
                        {
                            sPrevEntryCnt = sPreFilterStack->mEntryCount;
                        }
                        
                        continue;
                    }
                }
                else
                {
                    /* Do Nothing */
                }
            }
            else if( sAndFuncExpr->mArgs[ sLoop ]->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
            {
                /* List Function으로 구성된 filter */
                sListFunc = sAndFuncExpr->mArgs[ sLoop ]->mExpr.mListFunc;
                STL_DASSERT( sListFunc->mArgs[1]->mType == QLV_EXPR_TYPE_LIST_COLUMN );

                sListColLeft = sListFunc->mArgs[1]->mExpr.mListCol;
                STL_DASSERT( sListColLeft->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_ROW_EXPR );

                sListColRight = sListFunc->mArgs[ 0 ]->mExpr.mListCol;
                STL_DASSERT( sListColRight->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_ROW_EXPR );

                sRowExpr = sListColLeft->mArgs[ 0 ]->mExpr.mRowExpr;
                STL_DASSERT( ( sRowExpr->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                             ( sRowExpr->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );

                sCodeExpr = sRowExpr->mArgs[ 0 ];
                STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                    sRowExpr->mArgs[ 0 ],
                                                    aSQLStmt->mBindContext,
                                                    sFilterStack,
                                                    aQueryExprList->mStmtExprList->mConstExprStack,
                                                    aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                    & sExprEntry,
                                                    & QLL_CANDIDATE_MEM( aEnv ),
                                                    aEnv )
                         == STL_SUCCESS );                            

                for( sIdx = 0 ; sIdx < sListColRight->mArgCount ; sIdx++ )
                {
                    STL_DASSERT( sListColRight->mArgs[ sIdx ]->mType == QLV_EXPR_TYPE_ROW_EXPR );

                    sCodeExpr = sListColRight->mArgs[ sIdx ];
                    STL_TRY( qloAddExprStackEntry( aSQLStmt->mRetrySQL,
                                                   sListColRight->mArgs[ sIdx ],
                                                   aSQLStmt->mBindContext,
                                                   sPreFilterStack,
                                                   aQueryExprList->mStmtExprList->mConstExprStack,
                                                   sPreFilterStack->mEntryCount + 1,
                                                   KNL_BUILTIN_FUNC_INVALID,
                                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                   & sExprEntry,
                                                   QLL_CODE_PLAN( aDBCStmt ),
                                                   aEnv )
                             == STL_SUCCESS );

                    STL_TRY( qlnoGetCompareType( aSQLStmt->mRetrySQL,
                                                 sExprDBType[ sListColLeft->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ]->mOffset ].mDBType,
                                                 sExprDBType[ sListColRight->mArgs[ sIdx ]->mExpr.mRowExpr->mArgs[ 0 ]->mOffset ].mDBType,
                                                 STL_FALSE,
                                                 STL_TRUE,
                                                 sAndFuncExpr->mArgs[ sLoop ]->mPosition,
                                                 &sCompareType,
                                                 aEnv )
                             == STL_SUCCESS );

                    if( sCompareType->mRightType
                        == sExprDBType[ sListColLeft->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ]->mOffset ].mDBType )
                    {
                        sDBType = sExprDBType[ sListColLeft->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ]->mOffset ].mDBType;

                        sCodeExpr = sListColRight->mArgs[ sIdx ]->mExpr.mRowExpr->mArgs[ 0 ];
                        STL_TRY( qloCastExprDBType(
                                     sListColRight->mArgs[ sIdx ]->mExpr.mRowExpr->mArgs[ 0 ],
                                     aQueryExprList->mStmtExprList->mTotalExprInfo,
                                     sDBType,
                                     gResultDataTypeDef[ sDBType ].mArgNum1,
                                     gResultDataTypeDef[ sDBType ].mArgNum2,
                                     sExprDBType[ sListColLeft->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ]->mOffset ].mStringLengthUnit,
                                     sExprDBType[ sListColLeft->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ]->mOffset ].mIntervalIndicator,
                                     STL_TRUE,
                                     STL_FALSE,
                                     aEnv )
                                 == STL_SUCCESS );                                    
                    }
                    else
                    {
                        sDBType = sCompareType->mRightType;
                                
                        sCodeExpr = sListColRight->mArgs[ sIdx ]->mExpr.mRowExpr->mArgs[ 0 ];
                        STL_TRY( qloCastExprDBType( sListColRight->mArgs[ sIdx ]->mExpr.mRowExpr->mArgs[ 0 ],
                                                    aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                    sDBType,
                                                    gResultDataTypeDef[ sDBType ].mArgNum1,
                                                    gResultDataTypeDef[ sDBType ].mArgNum2,
                                                    gResultDataTypeDef[ sDBType ].mStringLengthUnit,
                                                    gResultDataTypeDef[ sDBType ].mIntervalIndicator,
                                                    STL_TRUE,
                                                    STL_FALSE,
                                                    aEnv )
                                 == STL_SUCCESS );                                    
                    }

                    
                    /**
                     * Prepare Filter Stack에서 Internal Cast가 없는 Value Entry 제거
                     */

                    if( ( sPreFilterStack->mEntryCount - sPrevEntryCnt ) == 1 )
                    {
                        if( ( sPreFilterStack->mEntries[ sPreFilterStack->mEntryCount ].mExprType
                              == KNL_EXPR_TYPE_VALUE ) &&
                            ( sExprDBType[ sPreFilterStack->mEntries
                                           [ sPreFilterStack->mEntryCount ].mExpr.mValueInfo.mOffset ].mIsNeedCast
                              == STL_FALSE ) )
                        {
                            sPreFilterStack->mEntryCount = sPrevEntryCnt;
                        }
                        else
                        {
                            sPrevEntryCnt = sPreFilterStack->mEntryCount;
                        }
                    }
                    else
                    {
                        sPrevEntryCnt = sPreFilterStack->mEntryCount;
                    }
                }
                    
                sCodeExpr = sAndFuncExpr->mArgs[ sLoop ];
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
                continue;
            }
            else
            {
                /**
                 * Column / Inner Column 만 들어온다.
                 */

                STL_DASSERT( ( sAndFuncExpr->mArgs[ sLoop ]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                             ( sAndFuncExpr->mArgs[ sLoop ]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );
            }

            sCodeExpr = sAndFuncExpr->mArgs[ sLoop ];
            STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                sAndFuncExpr->mArgs[ sLoop ],
                                                aSQLStmt->mBindContext,
                                                sFilterStack,
                                                aQueryExprList->mStmtExprList->mConstExprStack,
                                                aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                & sExprEntry,
                                                & QLL_CANDIDATE_MEM( aEnv ),
                                                aEnv )
                     == STL_SUCCESS );                            

            sCodeExpr = sAndFuncExpr->mArgs[ sLoop ];
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
        }
    }
    else if( aPhysicalFilterExpr->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
    {
        /* List Function으로 구성된 filter */
        sListFunc = aPhysicalFilterExpr->mExpr.mListFunc;
        STL_DASSERT( sListFunc->mArgs[ 1 ]->mType == QLV_EXPR_TYPE_LIST_COLUMN );

        sListColLeft = sListFunc->mArgs[ 1 ]->mExpr.mListCol;
        STL_DASSERT( sListColLeft->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_ROW_EXPR );

        sListColRight = sListFunc->mArgs[ 0 ]->mExpr.mListCol;
        STL_DASSERT( sListColRight->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_ROW_EXPR );

        sRowExpr = sListColLeft->mArgs[ 0 ]->mExpr.mRowExpr;
        STL_DASSERT( ( sRowExpr->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                     ( sRowExpr->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );

        sCodeExpr = sRowExpr->mArgs[ 0 ];
        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                            sRowExpr->mArgs[ 0 ],
                                            aSQLStmt->mBindContext,
                                            sFilterStack,
                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            & sExprEntry,
                                            & QLL_CANDIDATE_MEM( aEnv ),
                                            aEnv )
                 == STL_SUCCESS );                            

        for( sIdx = 0 ; sIdx < sListColRight->mArgCount ; sIdx++ )
        {
            STL_DASSERT( sListColRight->mArgs[ sIdx ]->mType == QLV_EXPR_TYPE_ROW_EXPR );

            sCodeExpr = sListColRight->mArgs[ sIdx ];
            STL_TRY( qloAddExprStackEntry( aSQLStmt->mRetrySQL,
                                           sListColRight->mArgs[ sIdx ],
                                           aSQLStmt->mBindContext,
                                           sPreFilterStack,
                                           aQueryExprList->mStmtExprList->mConstExprStack,
                                           sPreFilterStack->mEntryCount + 1,
                                           KNL_BUILTIN_FUNC_INVALID,
                                           aQueryExprList->mStmtExprList->mTotalExprInfo,
                                           & sExprEntry,
                                           QLL_CODE_PLAN( aDBCStmt ),
                                           aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlnoGetCompareType( aSQLStmt->mRetrySQL,
                                         sExprDBType[ sListColLeft->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ]->mOffset ].mDBType,
                                         sExprDBType[ sListColRight->mArgs[ sIdx ]->mExpr.mRowExpr->mArgs[ 0 ]->mOffset ].mDBType,
                                         STL_FALSE,
                                         STL_TRUE,
                                         aPhysicalFilterExpr->mPosition,
                                         &sCompareType,
                                         aEnv )
                     == STL_SUCCESS );

            if( sCompareType->mRightType
                == sExprDBType[ sListColLeft->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ]->mOffset ].mDBType )
            {
                sDBType = sExprDBType[ sListColLeft->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ]->mOffset ].mDBType;

                sCodeExpr = sListColRight->mArgs[ sIdx ]->mExpr.mRowExpr->mArgs[ 0 ];
                STL_TRY( qloCastExprDBType(
                             sListColRight->mArgs[ sIdx ]->mExpr.mRowExpr->mArgs[ 0 ],
                             aQueryExprList->mStmtExprList->mTotalExprInfo,
                             sDBType,
                             gResultDataTypeDef[ sDBType ].mArgNum1,
                             gResultDataTypeDef[ sDBType ].mArgNum2,
                             sExprDBType[ sListColLeft->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ]->mOffset ].mStringLengthUnit,
                             sExprDBType[ sListColLeft->mArgs[ 0 ]->mExpr.mRowExpr->mArgs[ 0 ]->mOffset ].mIntervalIndicator,
                             STL_TRUE,
                             STL_FALSE,
                             aEnv )
                         == STL_SUCCESS );                                    
            }
            else
            {
                sDBType = sCompareType->mRightType;

                sCodeExpr = sListColRight->mArgs[ sIdx ]->mExpr.mRowExpr->mArgs[ 0 ];
                STL_TRY( qloCastExprDBType( sListColRight->mArgs[ sIdx ]->mExpr.mRowExpr->mArgs[ 0 ],
                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                            sDBType,
                                            gResultDataTypeDef[ sDBType ].mArgNum1,
                                            gResultDataTypeDef[ sDBType ].mArgNum2,
                                            gResultDataTypeDef[ sDBType ].mStringLengthUnit,
                                            gResultDataTypeDef[ sDBType ].mIntervalIndicator,
                                            STL_TRUE,
                                            STL_FALSE,
                                            aEnv )
                         == STL_SUCCESS );                                    
            }

                    
            /**
             * Prepare Filter Stack에서 Internal Cast가 없는 Value Entry 제거
             */
        
            if( ( sPreFilterStack->mEntryCount - sPrevEntryCnt ) == 1 )
            {
                if( ( sPreFilterStack->mEntries[ sPreFilterStack->mEntryCount ].mExprType
                      == KNL_EXPR_TYPE_VALUE ) &&
                    ( sExprDBType[ sPreFilterStack->mEntries
                                   [ sPreFilterStack->mEntryCount ].mExpr.mValueInfo.mOffset ].mIsNeedCast
                      == STL_FALSE ) )
                {
                    sPreFilterStack->mEntryCount = sPrevEntryCnt;
                }
                else
                {
                    sPrevEntryCnt = sPreFilterStack->mEntryCount;
                }
            }
            else
            {
                sPrevEntryCnt = sPreFilterStack->mEntryCount;
            }
        }
                    
        sCodeExpr = aPhysicalFilterExpr;
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
    }
    else
    {
        sFunctionCnt = 0;

        sCodeExpr = aPhysicalFilterExpr;
        STL_TRY( qloAddExprStackEntry( aSQLStmt->mRetrySQL,
                                       aPhysicalFilterExpr,
                                       aSQLStmt->mBindContext,
                                       sFilterStack,
                                       aQueryExprList->mStmtExprList->mConstExprStack,
                                       sFilterStack->mEntryCount + 1,
                                       KNL_BUILTIN_FUNC_INVALID,
                                       aQueryExprList->mStmtExprList->mTotalExprInfo,
                                       & sExprEntry,
                                       QLL_CODE_PLAN( aDBCStmt ),
                                       aEnv )
                 == STL_SUCCESS );

        sCodeExpr = aPhysicalFilterExpr;
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
    }


    /**
     * Code Plan을 위한 Prepare Filter Expression 구성
     */

    sCodeExpr = NULL;
    
    if( sPreFilterStack->mEntryCount > 0 )
    {
        /**
         * Prepare Filter Expression Stack 공간 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprStack ),
                                    (void **) & sOutputStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

    
        /**
         * Prepare Filter Expression Stack 초기화
         */
        
        STL_TRY( knlExprInit( sOutputStack,
                              sPreFilterStack->mEntryCount + 1,
                              QLL_CODE_PLAN( aDBCStmt ),
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );


        /**
         * Prepare Filter Expression Stack 복사 
         */

        sOutputStack->mEntryCount = sPreFilterStack->mEntryCount;
        
        stlMemcpy( sOutputStack->mEntries,
                   sPreFilterStack->mEntries,
                   STL_SIZEOF( knlExprEntry ) * sOutputStack->mMaxEntryCount );

        if( sFunctionCnt > 1 )
        {
            sOutputStack->mExprComposition = KNL_EXPR_COMPOSITION_COMPLEX;
        }
        else
        {
            STL_TRY( knlAnalyzeExprStack( sOutputStack, KNL_ENV(aEnv) ) == STL_SUCCESS );
        }
    }
    else
    {
        sOutputStack = NULL;
    }
    

    *aPreFilterExprStack = sOutputStack;


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

    STL_FINISH;
    
    if ( ( aSQLStmt->mRetrySQL != NULL ) &&
         ( sCodeExpr != NULL ) )
    {
        stlChar * sLastError = NULL;
        sLastError = stlGetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv) );
        
        if ( sLastError[0] == '\0' )
        {
            stlSetLastDetailErrorMessage( QLL_ERROR_STACK(aEnv),
                                          qlgMakeErrPosString( aSQLStmt->mRetrySQL,
                                                               sCodeExpr->mPosition,
                                                               aEnv ) );
        }
    }

    return STL_FAILURE;
}


/**
 * @brief Range 수행을 위한 선처리 Expression 들에 대한 Expr Stack 구성
 * @param[in]  aDBCStmt              DBC statement
 * @param[in]  aSQLStmt              SQL statement
 * @param[in]  aQueryExprList        Query 단위 Expression 정보
 * @param[in]  aColCount             Column Count
 * @param[in]  aIsHashKey            Hash Key를 위한 Prepare Expr Stack인지 여부
 * @param[in]  aRangeExpr1           Range Expression 1
 * @param[in]  aRangeExpr2           Range Expression 2
 * @param[out] aPreRangeExprStack    Range Expression Stack
 * @param[in]  aEnv                  Environment
 */
stlStatus qloMakeRangeStack( qllDBCStmt          * aDBCStmt,
                             qllStatement        * aSQLStmt,
                             qloInitExprList     * aQueryExprList,
                             stlInt32              aColCount,
                             stlBool               aIsHashKey,
                             qlvRefExprList     ** aRangeExpr1,
                             qlvRefExprList     ** aRangeExpr2,
                             knlExprStack       ** aPreRangeExprStack,
                             qllEnv              * aEnv )
{
    knlExprStack     * sRangeStack      = NULL;
    knlExprStack     * sPreRangeStack   = NULL;
    knlExprStack     * sOutputStack     = NULL;

    qlvInitFunction  * sFunction        = NULL;
    knlExprEntry     * sExprEntry       = NULL;
    qlvRefExprItem   * sListItem        = NULL;
    qloDBType        * sExprDBType      = NULL;
    dtlDataType        sDBType          = DTL_TYPE_NA;

    const dtlCompareType    * sCompareType;
    
    stlInt16           sPreRangeExprCnt = 0;
    stlInt32           sPreRangeCnt     = 0;
    stlInt32           sLoop            = 0;
    stlUInt32          sPrevEntryCnt    = 0;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPreRangeExprStack != NULL, QLL_ERROR_STACK(aEnv) );
    
    
    /**
     * Range Expression Code Stack 공간 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( knlExprStack ),
                                (void **) & sRangeStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Range Expression Stack 생성
     */

    STL_TRY( knlExprInit( sRangeStack,
                          4,
                          & QLL_CANDIDATE_MEM( aEnv ),
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * Prepare Range Expression 내부의 Expression 개수 세기
     */

    sPreRangeExprCnt = 0;
    sExprDBType = aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType;
    
    for( sLoop = 0 ; sLoop < aColCount ; sLoop++ )
    {
        /* Range 1 */
        if( aRangeExpr1 == NULL )
        {
            sListItem = NULL;
        }
        else
        {
            sListItem = aRangeExpr1[ sLoop ]->mHead;
        }
        
        while( sListItem != NULL )
        {
            STL_DASSERT( sListItem->mExprPtr->mType == QLV_EXPR_TYPE_FUNCTION );

            sFunction = sListItem->mExprPtr->mExpr.mFunction;

            if( sFunction->mArgCount > 1 )
            {
                if( ( sFunction->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                    ( sFunction->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                {
                    STL_DASSERT( (sFunction->mArgs[ 1 ]->mType != QLV_EXPR_TYPE_COLUMN) &&
                                 (sFunction->mArgs[ 1 ]->mType != QLV_EXPR_TYPE_INNER_COLUMN) );
                    
                    sPreRangeExprCnt += sFunction->mArgs[ 1 ]->mIncludeExprCnt;
                    sPreRangeCnt++;
                }
                else
                {
                    sPreRangeExprCnt += sFunction->mArgs[ 0 ]->mIncludeExprCnt;
                    sPreRangeCnt++;
                }
            }
            else
            {
                /* Do Nothing */
            }

            sListItem = sListItem->mNext;
        }

        /* Range 2 */
        if( aRangeExpr2 == NULL )
        {
            sListItem = NULL;
        }
        else
        {
            sListItem = aRangeExpr2[ sLoop ]->mHead;
        }
        
        while( sListItem != NULL )
        {
            STL_DASSERT( sListItem->mExprPtr->mType == QLV_EXPR_TYPE_FUNCTION );

            sFunction = sListItem->mExprPtr->mExpr.mFunction;

            if( sFunction->mArgCount > 1 )
            {
                STL_DASSERT( ( sFunction->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                             ( sFunction->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );
                
                sPreRangeExprCnt += sFunction->mArgs[ 1 ]->mIncludeExprCnt;
                sPreRangeCnt++;
            }
            else
            {
                /* Do Nothing */
            }

            sListItem = sListItem->mNext;
        }
    }


    /**
     * Prepare Range Expression Stack 공간 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( knlExprStack ),
                                (void **) & sPreRangeStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Prepare Range Expression Stack 초기화
     */
        
    STL_TRY( knlExprInit( sPreRangeStack,
                          sPreRangeExprCnt + 2,
                          & QLL_CANDIDATE_MEM( aEnv ),
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    

    /**
     * Range1 Expression 정보 설정
     */

    STL_TRY_THROW( aRangeExpr1 != NULL, RAMP_SKIP_RANGE1 );

    for( sLoop = 0 ; sLoop < aColCount ; sLoop++ )
    {
        sListItem = aRangeExpr1[ sLoop ]->mHead;
        
        while( sListItem != NULL )
        {
            sRangeStack->mEntryCount = 0;

            sFunction = sListItem->mExprPtr->mExpr.mFunction;
            
            if( sFunction->mArgCount > 1 )
            {
                if( ( sFunction->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                    ( sFunction->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) )
                {
                    switch( sFunction->mArgs[ 1 ]->mType )
                    {
                        case QLV_EXPR_TYPE_COLUMN :
                        case QLV_EXPR_TYPE_INNER_COLUMN :
                            break;
                        default :
                            STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                                sFunction->mArgs[ 0 ],
                                                                aSQLStmt->mBindContext,
                                                                sRangeStack,
                                                                aQueryExprList->mStmtExprList->mConstExprStack,
                                                                aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                                & sExprEntry,
                                                                & QLL_CANDIDATE_MEM( aEnv ),
                                                                aEnv )
                                     == STL_SUCCESS );                            

                            STL_TRY( qloAddExprStackEntry( aSQLStmt->mRetrySQL,
                                                           sFunction->mArgs[ 1 ],
                                                           aSQLStmt->mBindContext,
                                                           sPreRangeStack,
                                                           aQueryExprList->mStmtExprList->mConstExprStack,
                                                           sPreRangeStack->mEntryCount + 1,
                                                           KNL_BUILTIN_FUNC_INVALID,
                                                           aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                           & sExprEntry,
                                                           QLL_CODE_PLAN( aDBCStmt ),
                                                           aEnv )
                                     == STL_SUCCESS );

                            if( sFunction->mFuncId != KNL_BUILTIN_FUNC_LIKE )
                            {
                                STL_TRY( qlnoGetCompareType( aSQLStmt->mRetrySQL,
                                                             sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mDBType,
                                                             sExprDBType[ sFunction->mArgs[ 1 ]->mOffset ].mDBType,
                                                             STL_FALSE,
                                                             STL_TRUE,
                                                             sListItem->mExprPtr->mPosition,
                                                             &sCompareType,
                                                             aEnv )
                                         == STL_SUCCESS );

                                if( ( aIsHashKey == STL_TRUE ) ||
                                    ( sCompareType->mRightType
                                      == sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mDBType ) )
                                {
                                    sDBType = sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mDBType;
                                
                                    STL_TRY( qloCastExprDBType( sFunction->mArgs[ 1 ],
                                                                aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                                sDBType,
                                                                gResultDataTypeDef[ sDBType ].mArgNum1,
                                                                gResultDataTypeDef[ sDBType ].mArgNum2,
                                                                sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mStringLengthUnit,
                                                                sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mIntervalIndicator,
                                                                STL_TRUE,
                                                                STL_FALSE,
                                                                aEnv )
                                             == STL_SUCCESS );                                    
                                }
                                else
                                {
                                    sDBType = sCompareType->mRightType;

                                    STL_TRY( qloCastExprDBType( sFunction->mArgs[ 1 ],
                                                                aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                                sDBType,
                                                                gResultDataTypeDef[ sDBType ].mArgNum1,
                                                                gResultDataTypeDef[ sDBType ].mArgNum2,
                                                                gResultDataTypeDef[ sDBType ].mStringLengthUnit,
                                                                gResultDataTypeDef[ sDBType ].mIntervalIndicator,
                                                                STL_TRUE,
                                                                STL_FALSE,
                                                                aEnv )
                                             == STL_SUCCESS );                                    
                                }
                            }
                            STL_TRY( qloSetExprDBType( sListItem->mExprPtr,
                                                       aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                       DTL_TYPE_BOOLEAN,
                                                       gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                                                       gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                                                       gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                                                       gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                                                       aEnv )
                                     == STL_SUCCESS );

                            
                            /**
                             * Prepare Filter Stack에서 Internal Cast가 없는 Value Entry 제거
                             */

                            if( ( sPreRangeStack->mEntryCount - sPrevEntryCnt ) == 1 )
                            {
                                if( ( sPreRangeStack->mEntries[ sPreRangeStack->mEntryCount ].mExprType
                                      == KNL_EXPR_TYPE_VALUE ) &&
                                    ( sExprDBType[ sPreRangeStack->mEntries
                                                   [ sPreRangeStack->mEntryCount ].mExpr.mValueInfo.mOffset ].mIsNeedCast
                                      == STL_FALSE ) )
                                {
                                    sPreRangeStack->mEntryCount = sPrevEntryCnt;
                                }
                                else
                                {
                                    sPrevEntryCnt = sPreRangeStack->mEntryCount;
                                }
                            }
                            else
                            {
                                sPrevEntryCnt = sPreRangeStack->mEntryCount;
                            }


                            sListItem = sListItem->mNext;
                            continue;
                    }
                }
                else
                {
                    STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                        sFunction->mArgs[ 1 ],
                                                        aSQLStmt->mBindContext,
                                                        sRangeStack,
                                                        aQueryExprList->mStmtExprList->mConstExprStack,
                                                        aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                        & sExprEntry,
                                                        & QLL_CANDIDATE_MEM( aEnv ),
                                                        aEnv )
                             == STL_SUCCESS );                            

                    STL_TRY( qloAddExprStackEntry( aSQLStmt->mRetrySQL,
                                                   sFunction->mArgs[ 0 ],
                                                   aSQLStmt->mBindContext,
                                                   sPreRangeStack,
                                                   aQueryExprList->mStmtExprList->mConstExprStack,
                                                   sPreRangeStack->mEntryCount + 1,
                                                   KNL_BUILTIN_FUNC_INVALID,
                                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                   & sExprEntry,
                                                   QLL_CODE_PLAN( aDBCStmt ),
                                                   aEnv )
                             == STL_SUCCESS );

                    if( sFunction->mFuncId != KNL_BUILTIN_FUNC_LIKE )
                    {
                        STL_DASSERT( aIsHashKey == STL_TRUE );
                        
                        sDBType = sExprDBType[ sFunction->mArgs[ 1 ]->mOffset ].mDBType;
                                
                        STL_TRY( qloCastExprDBType( sFunction->mArgs[ 0 ],
                                                    aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                    sDBType,
                                                    gResultDataTypeDef[ sDBType ].mArgNum1,
                                                    gResultDataTypeDef[ sDBType ].mArgNum2,
                                                    sExprDBType[ sFunction->mArgs[ 1 ]->mOffset ].mStringLengthUnit,
                                                    sExprDBType[ sFunction->mArgs[ 1 ]->mOffset ].mIntervalIndicator,
                                                    STL_TRUE,
                                                    STL_FALSE,
                                                    aEnv )
                                 == STL_SUCCESS );                                    
                    }

                    STL_TRY( qloSetExprDBType( sListItem->mExprPtr,
                                               aQueryExprList->mStmtExprList->mTotalExprInfo,
                                               DTL_TYPE_BOOLEAN,
                                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                                               gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                                               aEnv )
                             == STL_SUCCESS );
                    
                            
                    /**
                     * Prepare Filter Stack에서 Internal Cast가 없는 Value Entry 제거
                     */

                    if( ( sPreRangeStack->mEntryCount - sPrevEntryCnt ) == 1 )
                    {
                        if( ( sPreRangeStack->mEntries[ sPreRangeStack->mEntryCount ].mExprType
                              == KNL_EXPR_TYPE_VALUE ) &&
                            ( sExprDBType[ sPreRangeStack->mEntries
                                           [ sPreRangeStack->mEntryCount ].mExpr.mValueInfo.mOffset ].mIsNeedCast
                              == STL_FALSE ) )
                        {
                            sPreRangeStack->mEntryCount = sPrevEntryCnt;
                        }
                        else
                        {
                            sPrevEntryCnt = sPreRangeStack->mEntryCount;
                        }
                    }
                    else
                    {
                        sPrevEntryCnt = sPreRangeStack->mEntryCount;
                    }


                    sListItem = sListItem->mNext;
                    continue;
                }
            }
            else
            {
                /* Do Nothing */
            }
            
            STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                sListItem->mExprPtr,
                                                aSQLStmt->mBindContext,
                                                sRangeStack,
                                                aQueryExprList->mStmtExprList->mConstExprStack,
                                                aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                & sExprEntry,
                                                & QLL_CANDIDATE_MEM( aEnv ),
                                                aEnv )
                     == STL_SUCCESS );

            sListItem = sListItem->mNext;
        }
    }

    STL_RAMP( RAMP_SKIP_RANGE1 );
    

    /**
     * Range2 Expression 정보 설정
     */

    STL_TRY_THROW( aRangeExpr2 != NULL, RAMP_SKIP_RANGE2 );

    for( sLoop = 0 ; sLoop < aColCount ; sLoop++ )
    {
        sListItem = aRangeExpr2[ sLoop ]->mHead;
        
        while( sListItem != NULL )
        {
            sRangeStack->mEntryCount = 0;

            sFunction = sListItem->mExprPtr->mExpr.mFunction;
            
            if( sFunction->mArgCount > 1 )
            {
                STL_DASSERT( ( sFunction->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_COLUMN ) ||
                             ( sFunction->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) );
                
                switch( sFunction->mArgs[ 1 ]->mType )
                {
                    case QLV_EXPR_TYPE_COLUMN :
                    case QLV_EXPR_TYPE_INNER_COLUMN :
                        break;
                    default :
                        STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                            sFunction->mArgs[ 0 ],
                                                            aSQLStmt->mBindContext,
                                                            sRangeStack,
                                                            aQueryExprList->mStmtExprList->mConstExprStack,
                                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                            & sExprEntry,
                                                            & QLL_CANDIDATE_MEM( aEnv ),
                                                            aEnv )
                                 == STL_SUCCESS );                            

                        STL_TRY( qloAddExprStackEntry( aSQLStmt->mRetrySQL,
                                                       sFunction->mArgs[ 1 ],
                                                       aSQLStmt->mBindContext,
                                                       sPreRangeStack,
                                                       aQueryExprList->mStmtExprList->mConstExprStack,
                                                       sPreRangeStack->mEntryCount + 1,
                                                       KNL_BUILTIN_FUNC_INVALID,
                                                       aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                       & sExprEntry,
                                                       QLL_CODE_PLAN( aDBCStmt ),
                                                       aEnv )
                                 == STL_SUCCESS );

                        if( sFunction->mFuncId != KNL_BUILTIN_FUNC_LIKE )
                        {
                            STL_TRY( qlnoGetCompareType( aSQLStmt->mRetrySQL,
                                                         sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mDBType,
                                                         sExprDBType[ sFunction->mArgs[ 1 ]->mOffset ].mDBType,
                                                         STL_FALSE,
                                                         STL_TRUE,
                                                         sListItem->mExprPtr->mPosition,
                                                         &sCompareType,
                                                         aEnv )
                                     == STL_SUCCESS );

                            if( ( aIsHashKey == STL_TRUE ) ||
                                ( sCompareType->mRightType
                                  == sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mDBType ) )
                            {
                                sDBType = sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mDBType;
                                
                                STL_TRY( qloCastExprDBType( sFunction->mArgs[ 1 ],
                                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                            sDBType,
                                                            gResultDataTypeDef[ sDBType ].mArgNum1,
                                                            gResultDataTypeDef[ sDBType ].mArgNum2,
                                                            sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mStringLengthUnit,
                                                            sExprDBType[ sFunction->mArgs[ 0 ]->mOffset ].mIntervalIndicator,
                                                            STL_TRUE,
                                                            STL_FALSE,
                                                            aEnv )
                                         == STL_SUCCESS );                                    
                            }
                            else
                            {
                                sDBType = sCompareType->mRightType;

                                STL_TRY( qloCastExprDBType( sFunction->mArgs[ 1 ],
                                                            aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                            sDBType,
                                                            gResultDataTypeDef[ sDBType ].mArgNum1,
                                                            gResultDataTypeDef[ sDBType ].mArgNum2,
                                                            gResultDataTypeDef[ sDBType ].mStringLengthUnit,
                                                            gResultDataTypeDef[ sDBType ].mIntervalIndicator,
                                                            STL_TRUE,
                                                            STL_FALSE,
                                                            aEnv )
                                         == STL_SUCCESS );                                    
                            }
                        }
                            
                        STL_TRY( qloSetExprDBType( sListItem->mExprPtr,
                                                   aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                   DTL_TYPE_BOOLEAN,
                                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum1,
                                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mArgNum2,
                                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mStringLengthUnit,
                                                   gResultDataTypeDef[ DTL_TYPE_BOOLEAN ].mIntervalIndicator,
                                                   aEnv )
                                 == STL_SUCCESS );

                            
                        /**
                         * Prepare Filter Stack에서 Internal Cast가 없는 Value Entry 제거
                         */

                        if( ( sPreRangeStack->mEntryCount - sPrevEntryCnt ) == 1 )
                        {
                            if( ( sPreRangeStack->mEntries[ sPreRangeStack->mEntryCount ].mExprType
                                  == KNL_EXPR_TYPE_VALUE ) &&
                                ( sExprDBType[ sPreRangeStack->mEntries
                                               [ sPreRangeStack->mEntryCount ].mExpr.mValueInfo.mOffset ].mIsNeedCast
                                  == STL_FALSE ) )
                            {
                                sPreRangeStack->mEntryCount = sPrevEntryCnt;
                            }
                            else
                            {
                                sPrevEntryCnt = sPreRangeStack->mEntryCount;
                            }
                        }
                        else
                        {
                            sPrevEntryCnt = sPreRangeStack->mEntryCount;
                        }

                            
                        sListItem = sListItem->mNext;
                        continue;
                }
            }
            else
            {
                /* Do Nothing */
            }
            
            STL_TRY( qloCodeOptimizeExpression( aSQLStmt->mRetrySQL,
                                                sListItem->mExprPtr,
                                                aSQLStmt->mBindContext,
                                                sRangeStack,
                                                aQueryExprList->mStmtExprList->mConstExprStack,
                                                aQueryExprList->mStmtExprList->mTotalExprInfo,
                                                & sExprEntry,
                                                & QLL_CANDIDATE_MEM( aEnv ),
                                                aEnv )
                     == STL_SUCCESS );

            sListItem = sListItem->mNext;
        }
    }

    STL_RAMP( RAMP_SKIP_RANGE2 );

    
    /**
     * Code Plan을 위한 Prepare Range Expression 구성
     */

    if( sPreRangeStack->mEntryCount > 0 )
    {
        /**
         * Prepare Range Expression Stack 공간 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprStack ),
                                    (void **) & sOutputStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

    
        /**
         * Prepare Range Expression Stack 초기화
         */
        
        STL_TRY( knlExprInit( sOutputStack,
                              sPreRangeStack->mEntryCount + 1,
                              QLL_CODE_PLAN( aDBCStmt ),
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );


        /**
         * Prepare Range Expression Stack 복사 
         */

        sOutputStack->mEntryCount = sPreRangeStack->mEntryCount;
        
        stlMemcpy( sOutputStack->mEntries,
                   sPreRangeStack->mEntries,
                   STL_SIZEOF( knlExprEntry ) * sOutputStack->mMaxEntryCount );

        if( sPreRangeCnt > 1 )
        {
            sOutputStack->mExprComposition = KNL_EXPR_COMPOSITION_COMPLEX;
        }
        else
        {
            STL_TRY( knlAnalyzeExprStack( sOutputStack, KNL_ENV(aEnv) ) == STL_SUCCESS );
        }
    }
    else
    {
        sOutputStack = NULL;
    }


    *aPreRangeExprStack = sOutputStack;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Range 수행을 위한 선처리 Expression 들에 대한 Expr Stack 구성
 * @param[in]  aDBCStmt              DBC statement
 * @param[in]  aSQLStmt              SQL statement
 * @param[in]  aQueryExprList        Query 단위 Expression 정보
 * @param[in]  aListColCount         List Column Count
 * @param[in]  aValueExpr            Value Expression Array
 * @param[in]  aListColExpr          List Column Expression Array
 * @param[out] aPreExprStack         IN Key Range Expression Stack
 * @param[in]  aEnv                  Environment
 * 
 * @remark 하나의 Prepare Expression Stack에
 *    <BR> 모든 IN Key Range Expression에 대한 Expression 처리 정보를 담는다.
 */
stlStatus qloMakeExprStackForInKeyRange( qllDBCStmt          * aDBCStmt,
                                         qllStatement        * aSQLStmt,
                                         qloInitExprList     * aQueryExprList,
                                         stlInt32              aListColCount,
                                         qlvInitExpression  ** aValueExpr,
                                         qlvInitExpression  ** aListColExpr,
                                         knlExprStack       ** aPreExprStack,
                                         qllEnv              * aEnv )
{
    knlExprStack     * sPreRangeStack   = NULL;
    knlExprStack     * sOutputStack     = NULL;

    knlExprEntry     * sExprEntry       = NULL;
    qloDBType        * sExprDBType      = NULL;

    qlvInitListCol   * sListCol         = NULL;
    qlvInitRowExpr   * sRowExpr         = NULL;

    stlInt16           sPreRangeExprCnt = 0;
    stlInt32           sPreRangeCnt     = 0;
    stlInt32           sLoop            = 0;
    stlInt32           sRowIdx          = 0;
    stlUInt32          sPrevEntryCnt    = 0;


    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPreExprStack != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * List Column Expression 평가를 위한 Expression Stack 구성
     */

    STL_TRY_THROW( aListColCount > 0, RAMP_FINISH );
    
    STL_DASSERT( aValueExpr != NULL );
    STL_DASSERT( aListColExpr != NULL );
    
    
    /**
     * Prepare Range Expression 내부의 Expression 개수 세기
     */

    sPreRangeExprCnt = 0;
        
    for( sLoop = 0 ; sLoop < aListColCount ; sLoop++ )
    {
        STL_DASSERT( aListColExpr[ sLoop ] != NULL );
        sPreRangeExprCnt += aListColExpr[ sLoop ]->mIncludeExprCnt;
    }
    

    /**
     * Prepare Range Expression Stack 공간 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( knlExprStack ),
                                (void **) & sPreRangeStack,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Prepare Range Expression Stack 초기화
     */
        
    STL_TRY( knlExprInit( sPreRangeStack,
                          sPreRangeExprCnt + 2,
                          & QLL_CANDIDATE_MEM( aEnv ),
                          KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    

    /**
     * Range Expression 정보 설정
     */

    for( sLoop = 0 ; sLoop < aListColCount ; sLoop++ )
    {
        STL_DASSERT( aValueExpr[ sLoop ] != NULL );

        sExprDBType = & aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType
            [ aValueExpr[ sLoop ]->mOffset ];

        sListCol = aListColExpr[ sLoop ]->mExpr.mListCol;

        for( sRowIdx = 0 ; sRowIdx < sListCol->mArgCount ; sRowIdx++ )
        {
            /**
             * Expression Stack 구성
             */

            sRowExpr = sListCol->mArgs[ sRowIdx ]->mExpr.mRowExpr;

            STL_DASSERT( sRowExpr->mArgCount == 1 );
        
            STL_TRY( qloAddExprStackEntry( aSQLStmt->mRetrySQL,
                                           sRowExpr->mArgs[ 0 ],
                                           aSQLStmt->mBindContext,
                                           sPreRangeStack,
                                           aQueryExprList->mStmtExprList->mConstExprStack,
                                           sPreRangeStack->mEntryCount + 1,
                                           KNL_BUILTIN_FUNC_INVALID,
                                           aQueryExprList->mStmtExprList->mTotalExprInfo,
                                           & sExprEntry,
                                           QLL_CODE_PLAN( aDBCStmt ),
                                           aEnv )
                     == STL_SUCCESS );
            

            /**
             * DB Type Cast
             */
            
            STL_TRY( qloCastExprDBType( sRowExpr->mArgs[ 0 ],
                                        aQueryExprList->mStmtExprList->mTotalExprInfo,
                                        sExprDBType->mDBType,
                                        gResultDataTypeDef[ sExprDBType->mDBType ].mArgNum1,
                                        gResultDataTypeDef[ sExprDBType->mDBType ].mArgNum2,
                                        gResultDataTypeDef[ sExprDBType->mDBType ].mStringLengthUnit,
                                        gResultDataTypeDef[ sExprDBType->mDBType ].mIntervalIndicator,
                                        STL_FALSE,
                                        STL_FALSE,
                                        aEnv )
                     == STL_SUCCESS );


            /**
             * Prepare Filter Stack에서 Internal Cast가 없는 Value Entry 제거
             */

            if( ( sPreRangeStack->mEntryCount - sPrevEntryCnt ) == 1 )
            {
                sExprDBType = & aQueryExprList->mStmtExprList->mTotalExprInfo->mExprDBType
                    [ sRowExpr->mArgs[ 0 ]->mOffset ];

                if( ( sPreRangeStack->mEntries[ sPreRangeStack->mEntryCount ].mExprType
                      == KNL_EXPR_TYPE_VALUE ) &&
                    ( sExprDBType->mIsNeedCast == STL_FALSE ) )
                {
                    sPreRangeStack->mEntryCount = sPrevEntryCnt;
                }
                else
                {
                    sPrevEntryCnt = sPreRangeStack->mEntryCount;
                }
            }
            else
            {
                sPrevEntryCnt = sPreRangeStack->mEntryCount;
            }
        }
    }


    /**
     * Code Plan을 위한 Prepare Range Expression 구성
     */

    if( sPreRangeStack->mEntryCount > 0 )
    {
        /**
         * Prepare Range Expression Stack 공간 할당
         */

        STL_TRY( knlAllocRegionMem( QLL_CODE_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprStack ),
                                    (void **) & sOutputStack,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

    
        /**
         * Prepare Range Expression Stack 초기화
         */
        
        STL_TRY( knlExprInit( sOutputStack,
                              sPreRangeStack->mEntryCount + 1,
                              QLL_CODE_PLAN( aDBCStmt ),
                              KNL_ENV( aEnv ) )
                 == STL_SUCCESS );


        /**
         * Prepare Range Expression Stack 복사 
         */

        sOutputStack->mEntryCount = sPreRangeStack->mEntryCount;
        
        stlMemcpy( sOutputStack->mEntries,
                   sPreRangeStack->mEntries,
                   STL_SIZEOF( knlExprEntry ) * sOutputStack->mMaxEntryCount );

        if( sPreRangeCnt > 1 )
        {
            sOutputStack->mExprComposition = KNL_EXPR_COMPOSITION_COMPLEX;
        }
        else
        {
            STL_TRY( knlAnalyzeExprStack( sOutputStack, KNL_ENV(aEnv) ) == STL_SUCCESS );
        }
    }
    else
    {
        sOutputStack = NULL;
    }


    STL_RAMP( RAMP_FINISH );

    
    /**
     * OUTPUT 설정
     */

    *aPreExprStack = sOutputStack;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} qlo */
