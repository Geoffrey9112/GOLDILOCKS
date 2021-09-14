/*******************************************************************************
 * knlExpression.c
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
 * @file knlExpression.c
 * @brief Kernel Layer Expression Routines
 */

#include <knl.h>

/**
 * @addtogroup knlExpression
 * @{
 */

/**
 * @brief expression stack 생성 (초기화도 같이 수행)
 * @param[out]    aExpr              expression stack 
 * @param[in]     aMaxEntryCount     stack의 최대 entry 개수 
 * @param[in]     aMemMgr            영역 기반 메모리 할당자 ( knlRegionMem )
 * @param[in,out] aEnv               커널 Environment
 */
stlStatus knlExprCreate( knlExprStack   ** aExpr,
                         stlUInt32         aMaxEntryCount,
                         knlRegionMem    * aMemMgr,
                         knlEnv          * aEnv )
{
    knlExprStack        * sExprStack;

    STL_PARAM_VALIDATE( aMaxEntryCount <= KNL_EXPR_MAX_ENTRY_CNT, KNL_ERROR_STACK(aEnv) );


    /**
     * Expression Stack 할당
     */
    
    STL_TRY( knlAllocRegionMem( aMemMgr,
                                STL_SIZEOF( knlExprStack ),
                                (void**) & sExprStack,
                                aEnv )
             == STL_SUCCESS );
    
    
    /**
     * entry 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( aMemMgr,
                                KNL_EXPR_STACK_SIZE( aMaxEntryCount + 1 ),
                                (void**) & sExprStack->mEntries,
                                aEnv )
             == STL_SUCCESS );

    stlMemset( sExprStack->mEntries,
               0x00,
               KNL_EXPR_STACK_SIZE( aMaxEntryCount + 1 ) );


    /**
     * stack의 초기화
     */

    sExprStack->mMaxEntryCount   = aMaxEntryCount;
    sExprStack->mEntryCount      = 0;
    sExprStack->mExprComposition = KNL_EXPR_COMPOSITION_NONE;


    /**
     * Output 설정
     */

    *aExpr = sExprStack;
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief expression stack 초기화
 * @param[in,out] aExpr              expression stack 
 * @param[in]     aMaxEntryCount     stack의 최대 entry 개수 
 * @param[in]     aMemMgr            영역 기반 메모리 할당자 ( knlRegionMem )
 * @param[in,out] aEnv               커널 Environment
 */
stlStatus knlExprInit( knlExprStack    * aExpr,
                       stlUInt32         aMaxEntryCount,
                       knlRegionMem    * aMemMgr,
                       knlEnv          * aEnv )
{
    knlExprStack        * sExprStack = aExpr;

    STL_PARAM_VALIDATE( aExpr != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aMaxEntryCount <= KNL_EXPR_MAX_ENTRY_CNT, KNL_ERROR_STACK(aEnv) );
    
    
    /**
     * entry 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( aMemMgr,
                                KNL_EXPR_STACK_SIZE( aMaxEntryCount + 1),
                                (void**) & sExprStack->mEntries,
                                aEnv )
             == STL_SUCCESS );

    stlMemset( sExprStack->mEntries,
               0x00,
               KNL_EXPR_STACK_SIZE( aMaxEntryCount + 1 ) );


    /**
     * stack의 초기화
     */

    sExprStack->mMaxEntryCount   = aMaxEntryCount;
    sExprStack->mEntryCount      = 0;
    sExprStack->mExprComposition = KNL_EXPR_COMPOSITION_NONE;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief expression stack 복사
 * @param[in]     aSrcExpr           Source expression stack 
 * @param[out]    aDstExpr           Destination expression stack 
 * @param[in]     aMemMgr            영역 기반 메모리 할당자 ( knlRegionMem )
 * @param[in,out] aEnv               커널 Environment
 */
stlStatus knlExprCopy( knlExprStack    * aSrcExpr,
                       knlExprStack   ** aDstExpr,
                       knlRegionMem    * aMemMgr,
                       knlEnv          * aEnv )
{
    knlExprStack  * sExprStack;

    STL_PARAM_VALIDATE( aSrcExpr != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDstExpr != NULL, KNL_ERROR_STACK(aEnv) );


    /**
     * Expression Stack 할당
     */
    
    STL_TRY( knlAllocRegionMem( aMemMgr,
                                STL_SIZEOF( knlExprStack ),
                                (void**) & sExprStack,
                                aEnv )
             == STL_SUCCESS );
    
    
    /**
     * entry 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( aMemMgr,
                                KNL_EXPR_STACK_SIZE( aSrcExpr->mMaxEntryCount + 1 ),
                                (void**) & sExprStack->mEntries,
                                aEnv )
             == STL_SUCCESS );
    
    stlMemcpy( sExprStack->mEntries,
               aSrcExpr->mEntries,
               KNL_EXPR_STACK_SIZE( aSrcExpr->mMaxEntryCount + 1 ) );


    /**
     * stack의 초기화
     */

    sExprStack->mMaxEntryCount   = aSrcExpr->mMaxEntryCount;
    sExprStack->mEntryCount      = aSrcExpr->mEntryCount;
    sExprStack->mExprComposition = aSrcExpr->mExprComposition;


    /**
     * Output 설정
     */

    *aDstExpr = sExprStack;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief expression stack을 분석해서 composition 정보를 설정한다.
 * @param[in]     aExprStack   expression stack 
 * @param[in,out] aEnv         커널 Environment
 */
stlStatus knlAnalyzeExprStack( knlExprStack * aExprStack,
                               knlEnv       * aEnv )
{
    stlInt32       sLoop;
    knlExprEntry * sExprEntry;

    STL_PARAM_VALIDATE( aExprStack != NULL, KNL_ERROR_STACK(aEnv) );

    if( aExprStack->mExprComposition != KNL_EXPR_COMPOSITION_COMPLEX )
    {
        sExprEntry = &(aExprStack->mEntries[1]);
        aExprStack->mExprComposition = KNL_EXPR_COMPOSITION_VALUE_ONLY;

        for( sLoop = 1; sLoop <= aExprStack->mEntryCount; sLoop++ )
        {
            if( ( sExprEntry->mExprType == KNL_EXPR_TYPE_FUNCTION ) ||
                ( sExprEntry->mExprType == KNL_EXPR_TYPE_LIST ) )
            {
                aExprStack->mExprComposition = KNL_EXPR_COMPOSITION_SIMPLE;
            }

            if( (sExprEntry->mActionType == DTL_ACTION_TYPE_NORMAL) &&
                (sExprEntry->mActionReturnValue == DTL_ACTION_RETURN_PASS) )
            {
                /* nothing to do */
            }
            else
            {
                if( (sExprEntry->mActionType != DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL) ||
                    (sExprEntry->mActionReturnValue != DTL_ACTION_RETURN_NULL) )
                {
                    aExprStack->mExprComposition = KNL_EXPR_COMPOSITION_COMPLEX;
                    break;
                }
            }

            sExprEntry++;
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief expression stack에 function entry 추가
 * @param[in,out] aExprStack         expression stack 의 stack 
 * @param[in]     aFuncID            function ID
 * @param[in]     aIsExprCompositionComplex  ExprCompositionComplex 여부
 * @param[in]     aFuncOffset        function Offset
 * @param[in]     aArgCnt            function argument 개수
 * @param[in]     aUnitEntryCnt      function과 연관된 entry의 개수
 * @param[in]     aActionType        action type
 * @param[in]     aActionReturnValue action return value 
 * @param[in]     aActionJumpNo      action jump entry 번호
 * @param[out]    aGroupNo           function의 Group No.
 * @param[in]     aMemMgr            영역 기반 메모리 할당자 ( knlRegionMem )
 * @param[in,out] aEnv               커널 Environment
 */
stlStatus knlExprAddFunction( knlExprStack            * aExprStack,
                              knlBuiltInFunc            aFuncID,
                              stlBool                   aIsExprCompositionComplex,
                              stlUInt32                 aFuncOffset,
                              stlUInt16                 aArgCnt,
                              stlUInt32                 aUnitEntryCnt,
                              dtlActionType             aActionType,
                              dtlActionReturnValue      aActionReturnValue,
                              stlUInt32                 aActionJumpNo,
                              stlUInt32               * aGroupNo,
                              knlRegionMem            * aMemMgr,
                              knlEnv                  * aEnv )
{
    stlUInt32             sEntryCount  = 0;
    knlExprStack        * sExprStack   = aExprStack; 
    knlFunctionInfo     * sFuncInfo    = NULL;
    knlExprEntry        * sExprEntry   = NULL;
    
    STL_PARAM_VALIDATE( sExprStack != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( ( aFuncID > KNL_BUILTIN_FUNC_INVALID ) &&
                        ( aFuncID < KNL_BUILTIN_FUNC_MAX ),
                        KNL_ERROR_STACK( aEnv ));
    
    STL_DASSERT( sExprStack->mEntryCount <= sExprStack->mMaxEntryCount );
    

    /**
     * expression entry 설정
     */

    sEntryCount = sExprStack->mEntryCount + 1;    
    sExprEntry  = & sExprStack->mEntries[ sEntryCount ];
    
    sExprEntry->mExprType          = KNL_EXPR_TYPE_FUNCTION;
    sExprEntry->mOperGroupEntryNo  = sEntryCount; 
    sExprEntry->mActionType        = aActionType;
    sExprEntry->mActionReturnValue = aActionReturnValue;
    sExprEntry->mActionJumpEntryNo = aActionJumpNo;

    if( aIsExprCompositionComplex == STL_TRUE )
    {
        aExprStack->mExprComposition = KNL_EXPR_COMPOSITION_COMPLEX;
    }
    else
    {
        /* Do Nothing */
    }
    
    sFuncInfo = & sExprEntry->mExpr.mFuncInfo;
    
    /**
     * function info 설정
     */

    sFuncInfo->mOffset         = aFuncOffset;
    sFuncInfo->mArgumentCount  = aArgCnt;
    sFuncInfo->mUnitEntryCount = aUnitEntryCnt + 1;


    /**
     * stack entry count 증가
     */
    
    sExprStack->mEntryCount++;


    /**
     * Output 설정
     */

    *aGroupNo = sEntryCount;
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief expression stack에 data value entry 추가
 * @param[in,out] aExprStack           expression stack 
 * @param[in]     aGroupEntryNo        관련 entry 번호 
 * @param[in]     aValueOffset         value offset
 * @param[in]     aActionType          action type
 * @param[in]     aActionReturnValue   action return value 
 * @param[in]     aActionJumpNo        action jump entry 번호
 * @param[in]     aMemMgr              영역 기반 메모리 할당자 ( knlRegionMem )
 * @param[in,out] aEnv                 커널 Environment
 */
stlStatus knlExprAddValue( knlExprStack          * aExprStack,
                           stlUInt32               aGroupEntryNo,
                           stlUInt32               aValueOffset,
                           dtlActionType           aActionType,
                           dtlActionReturnValue    aActionReturnValue,
                           stlUInt32               aActionJumpNo,
                           knlRegionMem          * aMemMgr,
                           knlEnv                * aEnv )
{
    stlUInt32       sEntryCount = 0;
    knlExprStack  * sExprStack  = aExprStack;
    knlExprEntry  * sExprEntry  = NULL;
    knlValueInfo  * sValueInfo  = NULL;
    
    STL_PARAM_VALIDATE( sExprStack != NULL, KNL_ERROR_STACK(aEnv) );
    STL_DASSERT( sExprStack->mEntryCount <= sExprStack->mMaxEntryCount );

    
    /**
     * expression entry 설정
     */

    sEntryCount = sExprStack->mEntryCount + 1;    
    sExprEntry  = & sExprStack->mEntries[ sEntryCount ];
    
    sExprEntry->mExprType          = KNL_EXPR_TYPE_VALUE;
    sExprEntry->mOperGroupEntryNo  = aGroupEntryNo; 
    sExprEntry->mActionType        = aActionType;
    sExprEntry->mActionReturnValue = aActionReturnValue;
    sExprEntry->mActionJumpEntryNo = aActionJumpNo;
    
    sValueInfo = & sExprEntry->mExpr.mValueInfo;

    
    /**
     * value info 설정
     */

    sValueInfo->mOffset = aValueOffset;

        
    /**
     * stack entry count 증가
     */
    
    sExprStack->mEntryCount++;


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief expression stack에 list entry 추가
 * @param[in,out] aExprStack         expression stack 의 stack 
 * @param[in]     aFuncID            function ID
 * @param[in]     aIsExprCompositionComplex  ExprCompositionComplex 여부
 * @param[in]     aListOffset        list offset
 * @param[in]     aPredicate         list predicate ( function, left, right )
 * @param[in]     aArgCnt            list argument 개수
 * @param[in]     aUnitEntryCnt      list과 연관된 entry의 개수
 * @param[in]     aActionType        action type
 * @param[in]     aActionReturnValue action return value 
 * @param[in]     aActionJumpNo      action jump entry 번호
 * @param[in]     aListJumpNo        List function jump entry
 * @param[in]     aAddFirstExpr      First Add List Expr 
 * @param[in]     aMemMgr            영역 기반 메모리 할당자 ( knlRegionMem )
 * @param[in,out] aEnv               커널 Environment
 */
stlStatus knlExprAddListExpr( knlExprStack            * aExprStack,
                              knlBuiltInFunc            aFuncID,
                              stlBool                   aIsExprCompositionComplex,
                              stlUInt32                 aListOffset,
                              dtlListPredicate          aPredicate,
                              stlUInt16                 aArgCnt,
                              stlUInt32                 aUnitEntryCnt,
                              dtlActionType             aActionType,
                              dtlActionReturnValue      aActionReturnValue,
                              stlUInt32                 aActionJumpNo,
                              stlUInt32                 aListJumpNo,
                              stlBool                   aAddFirstExpr,
                              knlRegionMem            * aMemMgr,
                              knlEnv                  * aEnv )
{
    stlUInt32             sEntryCount  = 0;
    knlExprStack        * sExprStack   = aExprStack; 
    knlListExprInfo     * sListInfo    = NULL;
    knlExprEntry        * sExprEntry   = NULL;
    
    STL_PARAM_VALIDATE( sExprStack != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( ( aFuncID >= KNL_BUILTIN_FUNC_INVALID ) &&
                        ( aFuncID <= KNL_BUILTIN_FUNC_MAX ),
                        KNL_ERROR_STACK( aEnv ));
    
    STL_DASSERT( sExprStack->mEntryCount <= sExprStack->mMaxEntryCount );
    

    /**
     * expression entry 설정
     */

    sEntryCount = sExprStack->mEntryCount + 1;    
    sExprEntry  = & sExprStack->mEntries[ sEntryCount ];
    
    sExprEntry->mExprType          = KNL_EXPR_TYPE_LIST;
    sExprEntry->mOperGroupEntryNo  = sEntryCount;
    sExprEntry->mActionJumpEntryNo = aActionJumpNo;
    sListInfo = & sExprEntry->mExpr.mListInfo;
    
    if( aAddFirstExpr == STL_TRUE )
    {
        sExprEntry->mActionType        = aActionType;
        sExprEntry->mActionReturnValue = aActionReturnValue;
        
        if( aIsExprCompositionComplex == STL_TRUE )
        {
            aExprStack->mExprComposition = KNL_EXPR_COMPOSITION_COMPLEX;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sExprEntry->mActionType        = DTL_ACTION_TYPE_NORMAL;
        sExprEntry->mActionReturnValue = DTL_ACTION_RETURN_EMPTY;
    }
    
    /**
     * list info 설정
     */
    
    sListInfo->mOffset         = aListOffset;
    sListInfo->mPredicate      = aPredicate;
    sListInfo->mArgumentCount  = aArgCnt;
    sListInfo->mListJumpNo     = aListJumpNo;
    
    /**
     * stack entry count 증가
     */
    
    sExprStack->mEntryCount++;
        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief expression 평가
 * @brief block(multiple row) expression 평가
 * @param[in]     aBlockExprEvalInfo   Expression Stack Block Evaluation Info
 * @param[in,out] aEnv                 커널 Environment
 */
stlStatus knlEvaluateBlockExpression( knlExprEvalInfo  * aBlockExprEvalInfo,
                                      knlEnv           * aEnv )
{
    if( aBlockExprEvalInfo->mExprStack->mExprComposition ==
        KNL_EXPR_COMPOSITION_SIMPLE )
    {
        STL_TRY( knlEvaluateBlockSimpleExpression( aBlockExprEvalInfo,
                                                   aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        if( aBlockExprEvalInfo->mExprStack->mExprComposition ==
            KNL_EXPR_COMPOSITION_VALUE_ONLY )
        {
            STL_TRY( knlEvaluateBlockValueOnlyExpression( aBlockExprEvalInfo,
                                                          aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( knlEvaluateBlockComplexExpression( aBlockExprEvalInfo,
                                                        aEnv )
                     == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief block(multiple row) value only expression 평가
 * @param[in]     aBlockExprEvalInfo   Expression Stack Block Evaluation Info
 * @param[in,out] aEnv                 커널 Environment
 */
stlStatus knlEvaluateBlockValueOnlyExpression( knlExprEvalInfo  * aBlockExprEvalInfo,
                                               knlEnv           * aEnv )
{
    stlInt32          sBlockSize;
    knlExprStack    * sExprStack;
    knlExprContext  * sContext;
    knlValueInfo    * sValueInfo;
    stlInt32          sBlockIdx;
    dtlDataValue    * sResultValue;
    dtlDataValue    * sSrcValue;
    stlUInt32         sEntryCount;
    

    sBlockSize   = aBlockExprEvalInfo->mBlockIdx + aBlockExprEvalInfo->mBlockCount;
    sExprStack   = aBlockExprEvalInfo->mExprStack;
    sEntryCount  = sExprStack->mEntryCount;
            
    while( sEntryCount > 0 )
    {
        sValueInfo = &(sExprStack->mEntries[sEntryCount].mExpr.mValueInfo);
        sContext   = &(aBlockExprEvalInfo->mContext->mContexts[sValueInfo->mOffset]);

        if( sContext->mCast == NULL )
        {
            if( sEntryCount == 1 )
            {
                for( sBlockIdx = aBlockExprEvalInfo->mBlockIdx;
                     sBlockIdx < sBlockSize;
                     sBlockIdx++ )
                {
                    sSrcValue = KNL_GET_VALUE_BLOCK_DATA_VALUE( sContext->mInfo.mValueInfo,
                                                                sBlockIdx );
                    sResultValue = KNL_GET_BLOCK_DATA_VALUE( aBlockExprEvalInfo->mResultBlock, sBlockIdx );

                    STL_TRY( knlCopyDataValue( sSrcValue,
                                               sResultValue,
                                               aEnv )
                             == STL_SUCCESS );
                }
            }
        }
        else
        {
            for( sBlockIdx = aBlockExprEvalInfo->mBlockIdx;
                 sBlockIdx < sBlockSize;
                 sBlockIdx++ )
            {
                sSrcValue = KNL_GET_VALUE_BLOCK_DATA_VALUE( sContext->mInfo.mValueInfo, sBlockIdx );
                
                if( sEntryCount == 1 )
                {
                    sResultValue = KNL_GET_BLOCK_DATA_VALUE( aBlockExprEvalInfo->mResultBlock, sBlockIdx );

                    if( DTL_IS_NULL( sSrcValue ) )
                    {
                        DTL_SET_NULL( sResultValue );
                    }
                    else
                    {
                        sContext->mCast->mArgs[0].mValue.mDataValue = sSrcValue;
                        
                        STL_TRY( sContext->mCast->mCastFuncPtr(
                                     DTL_CAST_INPUT_ARG_CNT,
                                     sContext->mCast->mArgs,
                                     sResultValue,
                                     (void *)(sContext->mCast->mSrcTypeInfo),
                                     KNL_DT_VECTOR(aEnv),
                                     aEnv,
                                     KNL_ERROR_STACK( aEnv ) )
                                 == STL_SUCCESS );

                        KNL_VALIDATE_DATA_VALUE( aBlockExprEvalInfo->mResultBlock,
                                                 sBlockIdx,
                                                 aEnv );
                    }
                }
                else
                {
                    if( DTL_IS_NULL( sSrcValue ) )
                    {
                        DTL_SET_NULL( & sContext->mCast->mCastResultBuf );
                    }
                    else
                    {
                        sContext->mCast->mArgs[0].mValue.mDataValue = sSrcValue;
                        
                        STL_TRY( sContext->mCast->mCastFuncPtr(
                                     DTL_CAST_INPUT_ARG_CNT,
                                     sContext->mCast->mArgs,
                                     (void*)&sContext->mCast->mCastResultBuf,
                                     (void *)(sContext->mCast->mSrcTypeInfo),
                                     KNL_DT_VECTOR(aEnv),
                                     aEnv,
                                     KNL_ERROR_STACK( aEnv ) )
                                 == STL_SUCCESS );
                        
                        KNL_VALIDATE_DATA_VALUE( aBlockExprEvalInfo->mResultBlock,
                                                 sBlockIdx,
                                                 aEnv );
                    }
                }
            }
        }
                
        sEntryCount--;
    }

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief block(multiple row) complex expression 평가
 * @param[in]     aBlockExprEvalInfo   Expression Stack Block Evaluation Info
 * @param[in,out] aEnv                 커널 Environment
 */
stlStatus knlEvaluateBlockComplexExpression( knlExprEvalInfo  * aBlockExprEvalInfo,
                                             knlEnv           * aEnv )
{
    stlInt32           sBlockCnt;
    knlExprEvalInfo    sEvalInfo;


    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aBlockExprEvalInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockExprEvalInfo->mContext != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockExprEvalInfo->mBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockExprEvalInfo->mBlockCount > 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockExprEvalInfo->mResultBlock != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockExprEvalInfo->mResultBlock->mValueBlock != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    
    /**
     * Block 평가 
     */

    /* Eval Info 설정 */
    sEvalInfo.mExprStack   = aBlockExprEvalInfo->mExprStack;
    sEvalInfo.mContext     = aBlockExprEvalInfo->mContext;
    sEvalInfo.mResultBlock = aBlockExprEvalInfo->mResultBlock;
    sEvalInfo.mBlockIdx    = aBlockExprEvalInfo->mBlockIdx;
    sEvalInfo.mBlockCount  = 1;

    for( sBlockCnt = aBlockExprEvalInfo->mBlockCount ;
         sBlockCnt > 0 ;
         sBlockCnt-- )
    {
        STL_TRY( knlEvaluateComplexExpression( & sEvalInfo, aEnv ) == STL_SUCCESS );
        sEvalInfo.mBlockIdx++;
    }

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief block(multiple row) simple expression 평가
 * @param[in]     aBlockExprEvalInfo   Expression Stack Block Evaluation Info
 * @param[in,out] aEnv                 커널 Environment
 */
stlStatus knlEvaluateBlockSimpleExpression( knlExprEvalInfo  * aBlockExprEvalInfo,
                                            knlEnv           * aEnv )
{
    stlInt32            sBlockSize;
    stlInt32            sBlockIdx;
    dtlDataValue      * sResultValue;
    knlValueBlockList * sResultBlock;
    
    knlExprStack       * sExprStack;
    stlUInt32            sExprStackEntryCount;
    knlExprEntry       * sExprTopEntry;
    knlExprEntry       * sExprEntries;

    knlValueStack      * sArgStack;
    stlUInt32            sArgStackEntryCount;
    dtlValueEntry      * sArgTopEntry;
    dtlValueEntry      * sArgEntries;

    knlExprContextInfo * sContextInfo;
    knlExprContext     * sCurrContext = NULL;
    knlFunctionInfo    * sFuncInfo;
    knlFunctionData    * sFuncData;

    knlListExprInfo    * sListInfo;
    knlListStack       * sListStack;
    stlUInt32            sListStackEntryCount;
    knlListEntry       * sListTopEntry;
    knlListEntry       * sListEntries;
    knlListFunction    * sListFunc;
    stlBool              sIsResult;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aBlockExprEvalInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockExprEvalInfo->mContext != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockExprEvalInfo->mBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockExprEvalInfo->mBlockCount > 0, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockExprEvalInfo->mResultBlock != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockExprEvalInfo->mResultBlock->mValueBlock != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    
    /**
     * Block 평가 
     */

    /* Eval Info 설정 */
    
    sExprStack           = aBlockExprEvalInfo->mExprStack;
    sContextInfo         = aBlockExprEvalInfo->mContext;
    sExprEntries         = sExprStack->mEntries;
    sResultBlock         = aBlockExprEvalInfo->mResultBlock;
    sBlockSize           = aBlockExprEvalInfo->mBlockIdx + aBlockExprEvalInfo->mBlockCount;

    sIsResult            = STL_FALSE;
    
    for( sBlockIdx = aBlockExprEvalInfo->mBlockIdx;
         sBlockIdx < sBlockSize;
         sBlockIdx++ )
    {
        sExprStackEntryCount = sExprStack->mEntryCount;
        sExprTopEntry        = & sExprEntries[ sExprStackEntryCount ];

    
        /**
         * expression stack 관련 지역 변수 설정
         */

        sArgStack           = sContextInfo->mArgStack;
        sArgEntries         = sArgStack->mEntries;
        sArgStackEntryCount = 0;
        sArgTopEntry        = & sArgEntries[ 0 ];
    

        /**
         * expression 평가
         */

        while( sExprStackEntryCount > 0 )
        {
            if( sExprTopEntry->mExprType == KNL_EXPR_TYPE_VALUE )
            {
                sArgStackEntryCount++;
                sArgTopEntry++;

                sArgTopEntry->mValueType        = sExprTopEntry->mExprType;

                sCurrContext = & sContextInfo->mContexts[ sExprTopEntry->mExpr.mValueInfo.mOffset ];

                sArgTopEntry->mValue.mDataValue = KNL_GET_VALUE_BLOCK_DATA_VALUE( sCurrContext->mInfo.mValueInfo,
                                                                                  sBlockIdx );

                if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
                {
                    if( sExprTopEntry->mActionType == DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL )
                    {
                        DTL_SET_NULL( KNL_GET_BLOCK_DATA_VALUE( sResultBlock, sBlockIdx ) );
                        break;
                    }
                }
        
                /**
                 * Pop Expression Stack
                 */
        
                sExprTopEntry--;
                sExprStackEntryCount--;
        
                if( sCurrContext->mCast != NULL )
                {
                    if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
                    {
                        DTL_SET_NULL( & sCurrContext->mCast->mCastResultBuf );
                    }
                    else
                    {
                        sCurrContext->mCast->mArgs[0].mValue.mDataValue = 
                            sArgTopEntry->mValue.mDataValue;

                        STL_TRY( sCurrContext->mCast->mCastFuncPtr(
                                     DTL_CAST_INPUT_ARG_CNT,
                                     sCurrContext->mCast->mArgs,
                                     (void *) & sCurrContext->mCast->mCastResultBuf,
                                     (void *)(sCurrContext->mCast->mSrcTypeInfo),
                                     KNL_DT_VECTOR(aEnv),
                                     aEnv,
                                     KNL_ERROR_STACK( aEnv ) )
                                 == STL_SUCCESS );
                    }

                
                    /**
                     * 수행 결과를 Argument Stack에 반영
                     */

                    sArgTopEntry->mValue.mDataValue =
                        & sCurrContext->mCast->mCastResultBuf;
                }
            }

            if( sExprTopEntry->mExprType == KNL_EXPR_TYPE_VALUE )
            {
                sArgStackEntryCount++;
                sArgTopEntry++;

                sArgTopEntry->mValueType        = sExprTopEntry->mExprType;

                sCurrContext = & sContextInfo->mContexts[ sExprTopEntry->mExpr.mValueInfo.mOffset ];

                sArgTopEntry->mValue.mDataValue = KNL_GET_VALUE_BLOCK_DATA_VALUE( sCurrContext->mInfo.mValueInfo,
                                                                                  sBlockIdx );

                if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
                {
                    if( sExprTopEntry->mActionType == DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL )
                    {
                        DTL_SET_NULL( KNL_GET_BLOCK_DATA_VALUE( sResultBlock, sBlockIdx ) );
                        break;
                    }
                }
        
                /**
                 * Pop Expression Stack
                 */
        
                sExprTopEntry--;
                sExprStackEntryCount--;
        
                if( sCurrContext->mCast != NULL )
                {
                    if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
                    {
                        DTL_SET_NULL( & sCurrContext->mCast->mCastResultBuf );
                    }
                    else
                    {
                        sCurrContext->mCast->mArgs[0].mValue.mDataValue = 
                            sArgTopEntry->mValue.mDataValue;

                        STL_TRY( sCurrContext->mCast->mCastFuncPtr(
                                     DTL_CAST_INPUT_ARG_CNT,
                                     sCurrContext->mCast->mArgs,
                                     (void *) & sCurrContext->mCast->mCastResultBuf,
                                     (void *)(sCurrContext->mCast->mSrcTypeInfo),
                                     KNL_DT_VECTOR(aEnv),
                                     aEnv,
                                     KNL_ERROR_STACK( aEnv ) )
                                 == STL_SUCCESS );
                    }

                
                    /**
                     * 수행 결과를 Argument Stack에 반영
                     */

                    sArgTopEntry->mValue.mDataValue =
                        & sCurrContext->mCast->mCastResultBuf;
                }
            }

            if( sExprTopEntry->mExprType == KNL_EXPR_TYPE_VALUE )
            {
                sArgStackEntryCount++;
                sArgTopEntry++;

                sArgTopEntry->mValueType        = sExprTopEntry->mExprType;

                sCurrContext = & sContextInfo->mContexts[ sExprTopEntry->mExpr.mValueInfo.mOffset ];

                sArgTopEntry->mValue.mDataValue = KNL_GET_VALUE_BLOCK_DATA_VALUE( sCurrContext->mInfo.mValueInfo,
                                                                                  sBlockIdx );

                if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
                {
                    if( sExprTopEntry->mActionType == DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL )
                    {
                        DTL_SET_NULL( KNL_GET_BLOCK_DATA_VALUE( sResultBlock, sBlockIdx ) );
                        break;
                    }
                }
        
                /**
                 * Pop Expression Stack
                 */
        
                sExprTopEntry--;
                sExprStackEntryCount--;
        
                if( sCurrContext->mCast != NULL )
                {
                    if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
                    {
                        DTL_SET_NULL( & sCurrContext->mCast->mCastResultBuf );
                    }
                    else
                    {
                        sCurrContext->mCast->mArgs[0].mValue.mDataValue = 
                            sArgTopEntry->mValue.mDataValue;

                        STL_TRY( sCurrContext->mCast->mCastFuncPtr(
                                     DTL_CAST_INPUT_ARG_CNT,
                                     sCurrContext->mCast->mArgs,
                                     (void *) & sCurrContext->mCast->mCastResultBuf,
                                     (void *)(sCurrContext->mCast->mSrcTypeInfo),
                                     KNL_DT_VECTOR(aEnv),
                                     aEnv,
                                     KNL_ERROR_STACK( aEnv ) )
                                 == STL_SUCCESS );
                    }

                
                    /**
                     * 수행 결과를 Argument Stack에 반영
                     */

                    sArgTopEntry->mValue.mDataValue =
                        & sCurrContext->mCast->mCastResultBuf;
                }
            }
            
            if( sExprTopEntry->mExprType == KNL_EXPR_TYPE_FUNCTION )
            {
                /**
                 * Function 수행 정보  
                 *  : function 호출시 argument count만큼 읽을 수 있도록
                 *    argument stack의 시작 주소를 넘겨준다.
                 *  => function을 수행하는 곳에서 arguments 분석해서 사용
                 */

                sFuncInfo    = & sExprTopEntry->mExpr.mFuncInfo;

                sArgEntries  = sArgTopEntry - sFuncInfo->mArgumentCount + 1;
                    
                sCurrContext = & sContextInfo->mContexts[ sFuncInfo->mOffset ];
                sFuncData    = sCurrContext->mInfo.mFuncData;

                if( sExprStackEntryCount == 1 )
                {
                    if( sCurrContext->mCast == NULL )
                    {
                        sResultValue = KNL_GET_BLOCK_DATA_VALUE( sResultBlock, sBlockIdx );
                    }
                    else
                    {
                        sResultValue = & sFuncData->mResultValue;
                    }
                }
                else
                {
                    sResultValue = & sFuncData->mResultValue;
                }

                /**
                 * Function 수행
                 */

                STL_TRY( sFuncData->mFuncPtr( sFuncInfo->mArgumentCount,
                                              sArgEntries,
                                              sResultValue,
                                              sFuncData->mInfo,
                                              KNL_DT_VECTOR(aEnv),
                                              aEnv,
                                              KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
                
                if( DTL_IS_NULL( sResultValue ) )
                {
                    if( sExprTopEntry->mActionType == DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL )
                    {
                        DTL_SET_NULL( KNL_GET_BLOCK_DATA_VALUE( sResultBlock, sBlockIdx ) );
                        break;
                    }
                }

                /**
                 * 수행 결과를 Argument Stack에 push
                 */
                    
                sArgTopEntry = sArgEntries;
                sArgStackEntryCount -= sFuncInfo->mArgumentCount;
                sArgStackEntryCount++;


                /**
                 * 결과값을 argument stack에 저장
                 */

                sArgTopEntry->mValueType = DTL_VALUE_TYPE_VALUE;
                sArgTopEntry->mValue.mDataValue = sResultValue;
            
                if( sCurrContext->mCast != NULL )
                {
                    if( sExprStackEntryCount == 1 )
                    {
                        sResultValue = KNL_GET_BLOCK_DATA_VALUE( sResultBlock, sBlockIdx );
                    }
                    else
                    {
                        sResultValue = & sCurrContext->mCast->mCastResultBuf;
                    }
                    
                    if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
                    {
                        DTL_SET_NULL( sResultValue );
                    }
                    else
                    {
                        sCurrContext->mCast->mArgs[0].mValue.mDataValue = 
                            sArgTopEntry->mValue.mDataValue;
                        
                        STL_TRY( sCurrContext->mCast->mCastFuncPtr(
                                     DTL_CAST_INPUT_ARG_CNT,
                                     sCurrContext->mCast->mArgs,
                                     sResultValue,
                                     (void *)(sCurrContext->mCast->mSrcTypeInfo),
                                     KNL_DT_VECTOR(aEnv),
                                     aEnv,
                                     KNL_ERROR_STACK( aEnv ) )
                                 == STL_SUCCESS );
                    }
                    
                
                    /**
                     * 수행 결과를 Argument Stack에 반영
                     */

                    sArgTopEntry->mValue.mDataValue = sResultValue;
                }
                
                /**
                 * Pop Expression Stack
                 */
        
                sExprTopEntry--;
                sExprStackEntryCount--;
            }

            if( sExprTopEntry->mExprType == KNL_EXPR_TYPE_LIST )
            {
                sArgTopEntry ++;
                sArgStackEntryCount++;

            
                sListInfo     = & sExprTopEntry->mExpr.mListInfo;
            
                STL_PARAM_VALIDATE( ( sListInfo->mPredicate == DTL_LIST_PREDICATE_FUNCTION ),
                                    KNL_ERROR_STACK( aEnv ) );
            
                sCurrContext  = & sContextInfo->mContexts[ sListInfo->mOffset ];
                sListFunc     = sCurrContext->mInfo.mListFunc;
                sListStack    = sListFunc->mListStack;
            
                /**
                 * List Stack Entries 구성
                 */
            
                sListStackEntryCount            = sListStack->mEntryCount;
                sListEntries                    = sListStack->mEntries;
            
                if( sExprStackEntryCount == 1 )
                {
                    if( sCurrContext->mCast == NULL )
                    {
                        sResultValue = KNL_GET_BLOCK_DATA_VALUE( sResultBlock, sBlockIdx );
                    }
                    else
                    {
                        sResultValue = & sListFunc->mResultValue;
                    }
                }
                else
                {
                    sResultValue = & sListFunc->mResultValue;
                }     

                /**
                 * ResultValue ( 초기화 )
                 */

                if( sListInfo->mArgumentCount == sListStackEntryCount )
                {
                    DTL_SET_BOOLEAN_LENGTH( sResultValue );
                }

            
                /**
                 * List Function Type에 따른 Entries구성
                 */
                    
                sListTopEntry  = & sListEntries[ 0 ];
                
                /**
                 * List Function 수행
                 */
                    
                sListEntries  = sListTopEntry + 1;

                STL_TRY( sListFunc->mListFuncPtr( sListStackEntryCount,
                                                  sListFunc->mValueCount,
                                                  sListEntries,
                                                  sContextInfo,
                                                  sBlockIdx,
                                                  & sIsResult,
                                                  sResultValue,
                                                  KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
                    
                /**
                 * 결과값을 argument stack에 저장
                 */
                if( ( DTL_IS_NULL( sResultValue ) ) &&
                    ( sExprTopEntry->mActionType == DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL ) )
                {
                    DTL_SET_NULL( KNL_GET_BLOCK_DATA_VALUE( sResultBlock,
                                                            sBlockIdx ) );
                    break;
                }
                else
                {
                    if( sIsResult == STL_TRUE )
                    {
                        sListStackEntryCount = 0;                
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                                    
                sArgTopEntry->mValueType        = DTL_VALUE_TYPE_VALUE;
                sArgTopEntry->mValue.mDataValue = sResultValue;
                
                if( sCurrContext->mCast != NULL )
                {
                    if( sExprStackEntryCount == 1 )
                    {
                        sResultValue = KNL_GET_BLOCK_DATA_VALUE( sResultBlock, sBlockIdx );
                    }
                    else
                    {
                        sResultValue = & sCurrContext->mCast->mCastResultBuf;
                    }
                    
                    if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
                    {
                        DTL_SET_NULL( sResultValue );
                    }
                    else
                    {
                        sCurrContext->mCast->mArgs[0].mValue.mDataValue = 
                            sArgTopEntry->mValue.mDataValue;

                        STL_TRY( sCurrContext->mCast->mCastFuncPtr(
                                     DTL_CAST_INPUT_ARG_CNT,
                                     sCurrContext->mCast->mArgs,
                                     sResultValue,
                                     (void *)(sCurrContext->mCast->mSrcTypeInfo),
                                     KNL_DT_VECTOR(aEnv),
                                     aEnv,
                                     KNL_ERROR_STACK( aEnv ) )
                                 == STL_SUCCESS );
                    }
                    
                
                    /**
                     * 수행 결과를 Argument Stack에 반영
                     */

                    sArgTopEntry->mValue.mDataValue = sResultValue;
                }

                sListStack->mEntryCount = ( sListStackEntryCount > 0 ) ?
                    sListStackEntryCount : sListInfo->mArgumentCount;

                sExprTopEntry--;
                sExprStackEntryCount--;
                 
            }
        }
    }

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief join filter 평가
 * @param[in]  aJoinEvalInfo     Join Filter Evaluation Info
 * @param[in]  aRightBlockIdx    Right Block Idx
 * @param[out] aResult           Result
 * @param[in]  aEnv              커널 Environment
 */
stlStatus knlEvaluateJoinFilter( knlExprEvalInfo     * aJoinEvalInfo,
                                 stlInt32              aRightBlockIdx,
                                 stlBool             * aResult,
                                 knlEnv              * aEnv )
{
    dtlDataValue * sResultValue = NULL;
    
    aJoinEvalInfo->mBlockIdx = aRightBlockIdx;
    STL_TRY( knlEvaluateOneExpression( aJoinEvalInfo, aEnv ) == STL_SUCCESS );

    sResultValue = KNL_GET_BLOCK_DATA_VALUE( aJoinEvalInfo->mResultBlock, aRightBlockIdx );
    
    if ( DTL_BOOLEAN_IS_TRUE( sResultValue ) == STL_TRUE )
    {
        *aResult = STL_TRUE;
    }
    else
    {
        *aResult = STL_FALSE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief single row expression 평가
 * @param[in]     aExprEvalInfo     Expression Stack Evaluation Info
 * @param[in,out] aEnv              커널 Environment
 */
stlStatus knlEvaluateOneExpression( knlExprEvalInfo     * aExprEvalInfo,
                                    knlEnv              * aEnv )
{
    if( aExprEvalInfo->mExprStack->mExprComposition == KNL_EXPR_COMPOSITION_SIMPLE )
    {
        STL_TRY( knlEvaluateSimpleExpression( aExprEvalInfo,
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        if( aExprEvalInfo->mExprStack->mExprComposition == KNL_EXPR_COMPOSITION_VALUE_ONLY )
        {
            STL_TRY( knlEvaluateValueOnlyExpression( aExprEvalInfo,
                                                     aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( knlEvaluateComplexExpression( aExprEvalInfo,
                                                   aEnv )
                     == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief single row value only expression 평가
 * @param[in]     aBlockExprEvalInfo   Expression Stack Block Evaluation Info
 * @param[in,out] aEnv                 커널 Environment
 */
stlStatus knlEvaluateValueOnlyExpression( knlExprEvalInfo  * aBlockExprEvalInfo,
                                          knlEnv           * aEnv )
{
    knlExprStack    * sExprStack;
    knlExprContext  * sContext;
    knlValueInfo    * sValueInfo;
    dtlDataValue    * sResultValue;
    dtlDataValue    * sSrcValue;
    stlUInt32         sEntryCount;

    sExprStack   = aBlockExprEvalInfo->mExprStack;
    sEntryCount  = sExprStack->mEntryCount;
            
    while( sEntryCount > 0 )
    {
        sValueInfo = &(sExprStack->mEntries[sEntryCount].mExpr.mValueInfo);
        sContext   = &(aBlockExprEvalInfo->mContext->mContexts[sValueInfo->mOffset]);
        
        if( sContext->mCast == NULL )
        {
            if( sEntryCount == 1 )
            {
                sSrcValue = KNL_GET_VALUE_BLOCK_DATA_VALUE( sContext->mInfo.mValueInfo,
                                                            aBlockExprEvalInfo->mBlockIdx );
                sResultValue = KNL_GET_BLOCK_DATA_VALUE( aBlockExprEvalInfo->mResultBlock,
                                                         aBlockExprEvalInfo->mBlockIdx );
            
                STL_TRY( knlCopyDataValue( sSrcValue,
                                           sResultValue,
                                           aEnv )
                         == STL_SUCCESS );
            }
        }
        else
        {
            sSrcValue = KNL_GET_VALUE_BLOCK_DATA_VALUE( sContext->mInfo.mValueInfo,
                                                        aBlockExprEvalInfo->mBlockIdx );
            
            if( sEntryCount == 1 )
            {
                sResultValue = KNL_GET_BLOCK_DATA_VALUE( aBlockExprEvalInfo->mResultBlock,
                                                         aBlockExprEvalInfo->mBlockIdx );

                if( DTL_IS_NULL( sSrcValue ) )
                {
                    DTL_SET_NULL( sResultValue );
                }
                else
                {
                    sContext->mCast->mArgs[0].mValue.mDataValue = sSrcValue;
                    STL_TRY( sContext->mCast->mCastFuncPtr(
                                 DTL_CAST_INPUT_ARG_CNT,
                                 sContext->mCast->mArgs,
                                 sResultValue,
                                 (void *)(sContext->mCast->mSrcTypeInfo),
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 KNL_ERROR_STACK( aEnv ) )
                             == STL_SUCCESS );
                }
            }
            else
            {
                if( DTL_IS_NULL( sSrcValue ) )
                {
                    DTL_SET_NULL( & sContext->mCast->mCastResultBuf );
                }
                else
                {
                    sContext->mCast->mArgs[0].mValue.mDataValue = sSrcValue;
                        
                    STL_TRY( sContext->mCast->mCastFuncPtr(
                                 DTL_CAST_INPUT_ARG_CNT,
                                 sContext->mCast->mArgs,
                                 (void*)&sContext->mCast->mCastResultBuf,
                                 (void *)(sContext->mCast->mSrcTypeInfo),
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 KNL_ERROR_STACK( aEnv ) )
                             == STL_SUCCESS );
                }
            }
        }

        sEntryCount--;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief single row complex expression 평가
 * @param[in]     aExprEvalInfo     Expression Stack Evaluation Info
 * @param[in,out] aEnv              커널 Environment
 */
stlStatus knlEvaluateComplexExpression( knlExprEvalInfo     * aExprEvalInfo,
                                        knlEnv              * aEnv )
{
    stlBool              sIsCheckedAction;
    stlUInt8             sActionType;
    stlUInt8             sActionReturnValue;
    stlUInt32            sActionJumpEntryNo;

    knlExprStack       * sExprStack;
    stlUInt32            sExprStackEntryCount;
    knlExprEntry       * sExprTopEntry;
    knlExprEntry       * sExprEntries;

    knlValueStack      * sArgStack;
    stlUInt32            sArgStackEntryCount;
    dtlValueEntry      * sArgTopEntry;
    dtlValueEntry      * sArgEntries;

    dtlValueEntry      * sArgJumpEntry;

    knlExprContextInfo * sContextInfo;
    knlExprContext     * sCurrContext = NULL;
    knlFunctionInfo    * sFuncInfo;
    dtlDataValue       * sDataValue;

    knlFunctionData    * sFuncData;
    dtlDataValue       * sResultValue;
    dtlDataValue       * sDataValue1;
    dtlDataValue       * sDataValue2;

    knlListExprInfo    * sListInfo;
    knlListStack       * sListStack;
    stlUInt32            sListStackEntryCount;
    knlListEntry       * sListTopEntry;
    knlListEntry       * sListEntries;

    stlUInt32            sListEntryArgCount;
    knlListFunction    * sListFunc;
    stlBool              sIsResult;
    
    

    STL_PARAM_VALIDATE( aExprEvalInfo->mExprStack != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aExprEvalInfo->mContext != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aExprEvalInfo->mResultBlock != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aExprEvalInfo->mBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );


    /**
     * expression stack 관련 지역 변수 설정
     */

    sExprStack           = aExprEvalInfo->mExprStack;
    sExprEntries         = sExprStack->mEntries;
    sExprStackEntryCount = sExprStack->mEntryCount;
    sExprTopEntry        = & sExprEntries[ sExprStackEntryCount ];

    
    /**
     * expression stack 관련 지역 변수 설정
     */

    sContextInfo         = aExprEvalInfo->mContext;

    sArgStack            = sContextInfo->mArgStack;
    sArgEntries          = sArgStack->mEntries;
    sArgStackEntryCount  = 0;
    sArgTopEntry         = & sArgEntries[ 0 ];

    sListEntryArgCount   = 0;
    sIsResult            = STL_FALSE;
    
    /**
     * expression 평가
     */

    while( sExprStackEntryCount > 0 )
    {
        /************************************************************
         * pop from expression stack & push into argument stack 
         ***********************************************************/

        if( sExprTopEntry->mExprType == KNL_EXPR_TYPE_VALUE )
        {
            /**
             * Value : expression stack에서 pop하여 argument stack으로 push
             */

            sArgStackEntryCount++;
            sArgTopEntry++;

            sArgTopEntry->mValueType        = sExprTopEntry->mExprType;
            sArgTopEntry->mOperGroupEntryNo = sExprTopEntry->mOperGroupEntryNo;

            sCurrContext = & sContextInfo->mContexts[ sExprTopEntry->mExpr.mValueInfo.mOffset ];

            sArgTopEntry->mValue.mDataValue = KNL_GET_VALUE_BLOCK_DATA_VALUE( sCurrContext->mInfo.mValueInfo,
                                                                              aExprEvalInfo->mBlockIdx );
        }
        else if( sExprTopEntry->mExprType == KNL_EXPR_TYPE_FUNCTION )
        {
            /**
             * Function 수행 정보  
             *  : function 호출시 argument count만큼 읽을 수 있도록
             *    argument stack의 시작 주소를 넘겨준다.
             *  => function을 수행하는 곳에서 arguments 분석해서 사용
             */

            sFuncInfo    = & sExprTopEntry->mExpr.mFuncInfo;

            sArgEntries  = sArgTopEntry - sFuncInfo->mArgumentCount + 1;
                    
            sCurrContext = & sContextInfo->mContexts[ sFuncInfo->mOffset ];
            sFuncData    = sCurrContext->mInfo.mFuncData;

            sResultValue = & sFuncData->mResultValue;


            /**
             * Function 수행
             */

            STL_TRY( sFuncData->mFuncPtr( sFuncInfo->mArgumentCount,
                                          sArgEntries,
                                          sResultValue,
                                          sFuncData->mInfo,
                                          KNL_DT_VECTOR(aEnv),
                                          aEnv,
                                          KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );


            /**
             * 수행 결과를 Argument Stack에 push
             */
                    
            sArgTopEntry = sArgEntries;
            sArgStackEntryCount -= sFuncInfo->mArgumentCount;
            sArgStackEntryCount++;


            /**
             * 결과값을 argument stack에 저장
             */

            sArgTopEntry->mValueType = DTL_VALUE_TYPE_VALUE;
            sArgTopEntry->mValue.mDataValue = sResultValue;
            sArgTopEntry->mOperGroupEntryNo = sExprTopEntry->mActionJumpEntryNo + 1;

            /* switch( sCurrContext->mReturnType ) */
            /* { */
            /*     case DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_VALUE : */
            /*         { */
            /*             sArgTopEntry->mValueType = DTL_VALUE_TYPE_VALUE; */
            /*             sArgTopEntry->mValue.mDataValue = sResultValue; */
            /*             break; */
            /*         } */
            /*     default : */
            /*         { */
            /*             /\* DTL_FUNC_RETURN_TYPE_CLASS_SINGLE_ARRAY *\/ */
            /*             /\* DTL_FUNC_RETURN_TYPE_CLASS_MULTISET_VALUE *\/ */
            /*             /\* DTL_FUNC_RETURN_TYPE_CLASS_MULTISET_ARRAY *\/ */
            /*             /\* DTL_FUNC_RETURN_TYPE_CLASS_RELATION *\/ */
                                
            /*             STL_DASSERT( 0 ); */
            /*             break; */
            /*         } */
            /* } */
        }
        else
        {
            STL_DASSERT( sExprTopEntry->mExprType == KNL_EXPR_TYPE_LIST );

            sArgStackEntryCount++;
            sArgTopEntry++;
            sListInfo     = & sExprTopEntry->mExpr.mListInfo;
            
            STL_PARAM_VALIDATE( ( sListInfo->mPredicate == DTL_LIST_PREDICATE_FUNCTION ),
                                KNL_ERROR_STACK( aEnv ) );
            
            sCurrContext  = & sContextInfo->mContexts[ sListInfo->mOffset ];
            sListFunc     = sCurrContext->mInfo.mListFunc;
            sListStack    = sListFunc->mListStack;
            
            /**
             * List Stack Entries 구성
             */
            
            sListStackEntryCount            = sListStack->mEntryCount;
            sListEntries                    = sListStack->mEntries;
            
            sResultValue                    = & sListFunc->mResultValue;
            
            sArgTopEntry->mValueType        = DTL_VALUE_TYPE_VALUE;
            sArgTopEntry->mValue.mDataValue = sResultValue;
            sArgTopEntry->mOperGroupEntryNo = sExprTopEntry->mActionJumpEntryNo + 1;

            /**
             * ResultValue ( 초기화 )
             */

            if( sListInfo->mArgumentCount == sListStackEntryCount )
            {
                DTL_SET_BOOLEAN_LENGTH( sResultValue );
            }

            
            /**
             * List Function Type에 따른 Entries구성
             */
                    
            sListTopEntry  = & sListEntries[ sListStackEntryCount ];


            
            /**
             * 하나의 ListEntries를 묶음
             * Idx는 해당 KNL_EXPR_TYPE_LIST에 포함되는 ListEntries 개수.
             */
            
            sListEntryArgCount    = sListTopEntry->mIdx;
            sListStackEntryCount -= sListEntryArgCount;
            sListTopEntry        -= sListEntryArgCount;

            /**
             * List Function 수행
             */
                    
            sListEntries  = sListTopEntry + 1;

            STL_TRY( sListFunc->mListFuncPtr( sListEntryArgCount,
                                              sListFunc->mValueCount,
                                              sListEntries,
                                              sContextInfo,
                                              aExprEvalInfo->mBlockIdx,
                                              & sIsResult,
                                              sResultValue,
                                              KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );
                    
            /**
             * 결과값을 argument stack에 저장
             * ListJumpNo는 같은 Offset이 갖는 List Function에 해당하는 모든 EXPR 개수
             */
            if( sIsResult == STL_TRUE )
            {
                sListStackEntryCount  = 0;
                
                sExprStackEntryCount -= sListInfo->mListJumpNo;
                sExprTopEntry        -= sListInfo->mListJumpNo;
                
            }
                    
            sListStack->mEntryCount = ( sListStackEntryCount > 0 ) ?
                sListStackEntryCount : sListInfo->mArgumentCount;
        }

        STL_RAMP( RAMP_RECHECK_ACTION );        
        
        /**
         * Internal Cast 수행
         */
       
        
        if( sCurrContext->mCast != NULL )
        {
            if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
            {
                DTL_SET_NULL( & sCurrContext->mCast->mCastResultBuf );
                sArgTopEntry->mValue.mDataValue =
                    & sCurrContext->mCast->mCastResultBuf;
            }
            else
            {
                /**
                 * Function 수행
                 */

                sCurrContext->mCast->mArgs[0].mValue.mDataValue = 
                    sArgTopEntry->mValue.mDataValue;

                STL_TRY( sCurrContext->mCast->mCastFuncPtr(
                             DTL_CAST_INPUT_ARG_CNT,
                             sCurrContext->mCast->mArgs,
                             (void *) & sCurrContext->mCast->mCastResultBuf,
                             (void *)(sCurrContext->mCast->mSrcTypeInfo),
                             KNL_DT_VECTOR(aEnv),
                             aEnv,
                             KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );

                
                /**
                 * 수행 결과를 Argument Stack에 반영
                 */

                sArgTopEntry->mValue.mDataValue =
                    & sCurrContext->mCast->mCastResultBuf;
            }
        }
        

        /**
         * Pop Expression Stack
         */
        
        sActionType        = sExprTopEntry->mActionType;
        sActionJumpEntryNo = sExprTopEntry->mActionJumpEntryNo;
        sActionReturnValue = sExprTopEntry->mActionReturnValue;
        
        sExprTopEntry--;
        sExprStackEntryCount--;

        sIsCheckedAction = STL_FALSE;

        
        /************************************************************
         * Action for argument stack entry
         ***********************************************************/

        /**
         * argument 값으로 single value만 지원 가능
         * @todo array, row 등으로 확장
         */

        sDataValue = sArgTopEntry->mValue.mDataValue;

        
        /**
         * Top Argument에 대한 Action 수행
         */

        if( sActionType == DTL_ACTION_TYPE_NORMAL )
        {
            if( sActionReturnValue == DTL_ACTION_RETURN_PASS )
            {
                /* Do Nothing */
            }
            else if( sActionReturnValue == DTL_ACTION_RETURN_EMPTY )
            {
                sArgTopEntry--;
                sArgStackEntryCount--;
            }
            else
            {
                STL_DASSERT( sActionReturnValue == DTL_ACTION_RETURN_NULL );
                
                sArgTopEntry->mOperGroupEntryNo =
                    sExprTopEntry->mOperGroupEntryNo;
                sArgTopEntry->mValueType = DTL_VALUE_TYPE_VALUE;
                sArgTopEntry->mValue.mDataValue = dtlBooleanNull;
            }
            
            continue;
        }
        else if( sActionType == DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL )
        {
            STL_PARAM_VALIDATE(
                sArgTopEntry->mValueType == DTL_VALUE_TYPE_VALUE,
                KNL_ERROR_STACK( aEnv ) );

            if( DTL_IS_NULL( sDataValue ) == STL_FALSE )
            {
                continue;
            }
        }
        else
        {
            if( sActionType == DTL_ACTION_TYPE_CONDITION_JUMP_IS_TRUE )
            {
                STL_PARAM_VALIDATE(
                    sArgTopEntry->mValueType == DTL_VALUE_TYPE_VALUE,
                    KNL_ERROR_STACK( aEnv ) );

                if( DTL_BOOLEAN_IS_TRUE( sDataValue ) == STL_FALSE )
                {
                    continue;
                }
            }
            else if( sActionType == DTL_ACTION_TYPE_CONDITION_JUMP_IS_FALSE )
            {
                STL_PARAM_VALIDATE(
                    sArgTopEntry->mValueType == DTL_VALUE_TYPE_VALUE,
                    KNL_ERROR_STACK( aEnv ) );

                if( DTL_BOOLEAN_IS_FALSE( sDataValue ) == STL_FALSE )
                {
                    continue;
                }
            }
            else if( sActionType == DTL_ACTION_TYPE_CONDITION_JUMP_IS_NOT_TRUE )
            {
                STL_PARAM_VALIDATE(
                    sArgTopEntry->mValueType == DTL_VALUE_TYPE_VALUE,
                    KNL_ERROR_STACK( aEnv ) );

                if( DTL_BOOLEAN_IS_TRUE( sDataValue ) )
                {
                    continue;
                }
            }
            else
            {
                STL_DASSERT( sActionType == DTL_ACTION_TYPE_UNCONDITION_JUMP );
                
                /* Do Nothing */
            }
        }
        
        
        /**
         * Jump 수행 : pop argument entries
         */

        STL_PARAM_VALIDATE( ( sActionJumpEntryNo < sExprStackEntryCount ) ||
                            ( sActionJumpEntryNo == 0 ),
                            KNL_ERROR_STACK( aEnv ) );

        sArgJumpEntry = sArgTopEntry;
        
        while( sArgJumpEntry->mOperGroupEntryNo > sActionJumpEntryNo )
        {
            sArgStackEntryCount--;
            sArgJumpEntry--;
        }
            

        /**
         * @todo Jump한 Entry의 Action 정보를 설정
         */

        if( sExprTopEntry > & sExprEntries[ sActionJumpEntryNo ] )
        {
            if( ( sExprEntries[ sActionJumpEntryNo + 1 ].mExprType == KNL_EXPR_TYPE_FUNCTION ) &&
                ( sActionReturnValue != DTL_ACTION_RETURN_EMPTY ) )
            {
                /* top - 1은 function 이고, 이에 대한 action은 평가되지 않음.
                 * top - 1에 대한 action 수행
                 */
                
                sExprStackEntryCount = sActionJumpEntryNo + 1;
                sExprTopEntry = & sExprEntries[ sActionJumpEntryNo + 1 ];               
                sIsCheckedAction = STL_FALSE;
            }
            else
            {
                sExprStackEntryCount = sActionJumpEntryNo;
                sExprTopEntry = & sExprEntries[ sActionJumpEntryNo ];
                sIsCheckedAction = STL_TRUE;
            }
        }
        else
        {
            sExprStackEntryCount = sActionJumpEntryNo;
            sExprTopEntry = & sExprEntries[ sActionJumpEntryNo ];
            sIsCheckedAction = STL_TRUE;
        }
        
        
        /**
         * Jump 수행 : pop expression entries
         *
         * => Jump Entry의 Return Value에 Action Return Value 설정
         */
        if( sActionReturnValue == DTL_ACTION_RETURN_PASS )
        {
            sArgStackEntryCount++;

            sArgJumpEntry++;

            sArgJumpEntry->mOperGroupEntryNo =
                sExprTopEntry->mActionJumpEntryNo + 1;
            sArgJumpEntry->mValueType = sArgTopEntry->mValueType;
            sArgJumpEntry->mValue.mDataValue =
                sArgTopEntry->mValue.mDataValue;
                        
            sArgTopEntry = sArgJumpEntry;

        }
        else if( sActionReturnValue == DTL_ACTION_RETURN_NULL )
        {
            sArgStackEntryCount++;
                        
            sArgJumpEntry++;

            /**
             * Boolean의 NULL값으로 초기화
             */
                    
            sArgJumpEntry->mOperGroupEntryNo =
                sExprTopEntry->mActionJumpEntryNo + 1;
            sArgJumpEntry->mValueType = DTL_VALUE_TYPE_VALUE;
            sArgJumpEntry->mValue.mDataValue = dtlBooleanNull;
                    
            sArgTopEntry = sArgJumpEntry;
        }
        else
        {
            if( sActionReturnValue == DTL_ACTION_RETURN_TRUE )
            {
                sArgStackEntryCount++;
                        
                sArgJumpEntry++;

                /**
                 * Boolean의 TRUE값으로 설정
                 */
                    
                sArgJumpEntry->mOperGroupEntryNo =
                    sExprTopEntry->mActionJumpEntryNo + 1;
                sArgJumpEntry->mValueType = DTL_VALUE_TYPE_VALUE;
                sArgJumpEntry->mValue.mDataValue = dtlBooleanTrue;
                    
                sArgTopEntry = sArgJumpEntry;

            }
            else if( sActionReturnValue == DTL_ACTION_RETURN_FALSE )
            {
                sArgStackEntryCount++;
                        
                sArgJumpEntry++;
                    
                /**
                 * Boolean의 FALSE값으로 설정
                 */
                    
                sArgJumpEntry->mOperGroupEntryNo =
                    sExprTopEntry->mActionJumpEntryNo + 1;
                sArgJumpEntry->mValueType = DTL_VALUE_TYPE_VALUE;
                sArgJumpEntry->mValue.mDataValue = dtlBooleanFalse;


                sArgTopEntry = sArgJumpEntry;

            }
            else
            {
                STL_DASSERT( sActionReturnValue == DTL_ACTION_RETURN_EMPTY );
                sArgTopEntry = sArgJumpEntry;
            }
        }

        /* Function의 연산 결과값을 Function Buffer에 저장한다. */
        if( ( sArgStackEntryCount > 0 ) &&
            ( sExprTopEntry->mExprType == KNL_EXPR_TYPE_FUNCTION ) )
        {
            sFuncInfo    = & sExprTopEntry->mExpr.mFuncInfo;
            sCurrContext = & sContextInfo->mContexts[ sFuncInfo->mOffset ];
            sResultValue = & sCurrContext->mInfo.mFuncData->mResultValue;

            if( sArgTopEntry->mValue.mDataValue->mLength == 0 )
            {
                sResultValue->mLength = 0;
            }
            else
            {
                STL_PARAM_VALIDATE(
                    ( sResultValue->mType ==
                      sArgTopEntry->mValue.mDataValue->mType ) ||
                    ( ( dtlDataTypeGroup[sResultValue->mType]
                        == DTL_GROUP_NUMBER ) &&
                      ( dtlDataTypeGroup[sArgTopEntry->mValue.mDataValue->mType]
                        == DTL_GROUP_NUMBER ) ),
                    KNL_ERROR_STACK(aEnv) );
            
                sResultValue->mLength = sArgTopEntry->mValue.mDataValue->mLength;
                STL_TRY( knlCopyDataValue( sArgTopEntry->mValue.mDataValue,
                                           sResultValue,
                                           aEnv )
                         == STL_SUCCESS );
            }
        }
        else
        {
            /* Do Nothing */
        }
                
        STL_TRY_THROW( sIsCheckedAction == STL_TRUE, RAMP_RECHECK_ACTION );
        
    } /* while */
    
    /* argument stack에는 결과가 하나만 남아야 한다. */

    sArgEntries = sArgStack->mEntries;
    
    STL_DASSERT( sArgEntries[sArgStackEntryCount].mValueType == DTL_VALUE_TYPE_VALUE );


    /**
     * output 설정
     */

    STL_PARAM_VALIDATE( sArgEntries[sArgStackEntryCount].mValue.mDataValue->mBufferSize >=
                        sArgEntries[sArgStackEntryCount].mValue.mDataValue->mLength,
                        KNL_ERROR_STACK(aEnv) );

    if( DTL_IS_NULL( sArgEntries[sArgStackEntryCount].mValue.mDataValue ) )
    {
        DTL_SET_NULL( KNL_GET_BLOCK_DATA_VALUE( aExprEvalInfo->mResultBlock,
                                                aExprEvalInfo->mBlockIdx ) );
    }
    else
    {
        STL_DASSERT( ( sArgEntries[sArgStackEntryCount].mValue.mDataValue->mType ==
                       KNL_GET_BLOCK_DB_TYPE( aExprEvalInfo->mResultBlock ) ) ||
                     ( ( dtlDataTypeGroup[sArgEntries[sArgStackEntryCount].mValue.mDataValue->mType]
                         == DTL_GROUP_NUMBER ) &&
                       ( dtlDataTypeGroup[KNL_GET_BLOCK_DB_TYPE( aExprEvalInfo->mResultBlock )]
                         == DTL_GROUP_NUMBER ) ) );

        sDataValue1 = sArgEntries[sArgStackEntryCount].mValue.mDataValue;
        sDataValue2 = KNL_GET_BLOCK_DATA_VALUE( aExprEvalInfo->mResultBlock, aExprEvalInfo->mBlockIdx );

        STL_TRY( knlCopyDataValue( sDataValue1,
                                   sDataValue2,
                                   aEnv )
                 == STL_SUCCESS );
    }
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief single row simple expression 평가
 * @param[in]     aExprEvalInfo     Expression Stack Evaluation Info
 * @param[in,out] aEnv              커널 Environment
 */
stlStatus knlEvaluateSimpleExpression( knlExprEvalInfo     * aExprEvalInfo,
                                       knlEnv              * aEnv )
{
    knlExprStack       * sExprStack;
    stlUInt32            sExprStackEntryCount;
    knlExprEntry       * sExprTopEntry;
    knlExprEntry       * sExprEntries;

    knlValueStack      * sArgStack;
    stlUInt32            sArgStackEntryCount;
    dtlValueEntry      * sArgTopEntry;
    dtlValueEntry      * sArgEntries;

    knlExprContextInfo * sContextInfo;
    knlExprContext     * sCurrContext = NULL;
    knlFunctionInfo    * sFuncInfo;

    knlFunctionData    * sFuncData;
    dtlDataValue       * sResultValue;

    
    knlListExprInfo    * sListInfo;
    knlListStack       * sListStack;
    stlUInt32            sListStackEntryCount;
    knlListEntry       * sListTopEntry;
    knlListEntry       * sListEntries;
    knlListFunction    * sListFunc;
    stlBool              sIsResult;
    
    
    STL_PARAM_VALIDATE( aExprEvalInfo->mExprStack != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aExprEvalInfo->mContext != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aExprEvalInfo->mResultBlock != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aExprEvalInfo->mBlockIdx >= 0, KNL_ERROR_STACK( aEnv ) );


    /**
     * expression stack 관련 지역 변수 설정
     */

    sExprStack           = aExprEvalInfo->mExprStack;
    sExprEntries         = sExprStack->mEntries;
    sExprStackEntryCount = sExprStack->mEntryCount;
    sExprTopEntry        = & sExprEntries[ sExprStackEntryCount ];

    
    /**
     * expression stack 관련 지역 변수 설정
     */

    sContextInfo        = aExprEvalInfo->mContext;

    sArgStack           = sContextInfo->mArgStack;
    sArgEntries         = sArgStack->mEntries;
    sArgStackEntryCount = 0;
    sArgTopEntry        = & sArgEntries[ 0 ];
    
    sListStackEntryCount = 0;
    sIsResult            = STL_FALSE;
    
    /**
     * expression 평가
     */

    while( sExprStackEntryCount > 0 )
    {
        if( sExprTopEntry->mExprType == KNL_EXPR_TYPE_VALUE )
        {
            sArgStackEntryCount++;
            sArgTopEntry++;

            sArgTopEntry->mValueType        = sExprTopEntry->mExprType;

            sCurrContext = & sContextInfo->mContexts[ sExprTopEntry->mExpr.mValueInfo.mOffset ];

            sArgTopEntry->mValue.mDataValue = KNL_GET_VALUE_BLOCK_DATA_VALUE( sCurrContext->mInfo.mValueInfo,
                                                                              aExprEvalInfo->mBlockIdx );

            if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
            {
                if( sExprTopEntry->mActionType == DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL )
                {
                    DTL_SET_NULL( KNL_GET_BLOCK_DATA_VALUE( aExprEvalInfo->mResultBlock,
                                                            aExprEvalInfo->mBlockIdx ) );
                    return STL_SUCCESS;
                }
            }
        
            /**
             * Pop Expression Stack
             */
        
            sExprTopEntry--;
            sExprStackEntryCount--;

        
            if( sCurrContext->mCast != NULL )
            {
                /**
                 * Function 수행
                 */

                if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
                {
                    DTL_SET_NULL( & sCurrContext->mCast->mCastResultBuf );
                }
                else
                {
                    sCurrContext->mCast->mArgs[0].mValue.mDataValue = 
                        sArgTopEntry->mValue.mDataValue;

                    STL_TRY( sCurrContext->mCast->mCastFuncPtr(
                                 DTL_CAST_INPUT_ARG_CNT,
                                 sCurrContext->mCast->mArgs,
                                 (void *) & sCurrContext->mCast->mCastResultBuf,
                                 (void *)(sCurrContext->mCast->mSrcTypeInfo),
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 KNL_ERROR_STACK( aEnv ) )
                             == STL_SUCCESS );
                }

                
                /**
                 * 수행 결과를 Argument Stack에 반영
                 */

                sArgTopEntry->mValue.mDataValue = &sCurrContext->mCast->mCastResultBuf;
            }
        }
    
        if( sExprTopEntry->mExprType == KNL_EXPR_TYPE_VALUE )
        {
            sArgStackEntryCount++;
            sArgTopEntry++;

            sArgTopEntry->mValueType        = sExprTopEntry->mExprType;

            sCurrContext = & sContextInfo->mContexts[ sExprTopEntry->mExpr.mValueInfo.mOffset ];

            sArgTopEntry->mValue.mDataValue = KNL_GET_VALUE_BLOCK_DATA_VALUE( sCurrContext->mInfo.mValueInfo,
                                                                              aExprEvalInfo->mBlockIdx );

            if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
            {
                if( sExprTopEntry->mActionType == DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL )
                {
                    DTL_SET_NULL( KNL_GET_BLOCK_DATA_VALUE( aExprEvalInfo->mResultBlock,
                                                            aExprEvalInfo->mBlockIdx ) );
                    return STL_SUCCESS;
                }
            }
        
            /**
             * Pop Expression Stack
             */
        
            sExprTopEntry--;
            sExprStackEntryCount--;
        
            if( sCurrContext->mCast != NULL )
            {
                /**
                 * Function 수행
                 */

                if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
                {
                    DTL_SET_NULL( & sCurrContext->mCast->mCastResultBuf );
                }
                else
                {
                    sCurrContext->mCast->mArgs[0].mValue.mDataValue = 
                        sArgTopEntry->mValue.mDataValue;

                    STL_TRY( sCurrContext->mCast->mCastFuncPtr(
                                 DTL_CAST_INPUT_ARG_CNT,
                                 sCurrContext->mCast->mArgs,
                                 (void *) & sCurrContext->mCast->mCastResultBuf,
                                 (void *)(sCurrContext->mCast->mSrcTypeInfo),
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 KNL_ERROR_STACK( aEnv ) )
                             == STL_SUCCESS );
                }

                
                /**
                 * 수행 결과를 Argument Stack에 반영
                 */

                sArgTopEntry->mValue.mDataValue =
                    & sCurrContext->mCast->mCastResultBuf;
            }
        }

        
        if( sExprTopEntry->mExprType == KNL_EXPR_TYPE_VALUE )
        {
            sArgStackEntryCount++;
            sArgTopEntry++;

            sArgTopEntry->mValueType        = sExprTopEntry->mExprType;

            sCurrContext = & sContextInfo->mContexts[ sExprTopEntry->mExpr.mValueInfo.mOffset ];

            sArgTopEntry->mValue.mDataValue = KNL_GET_VALUE_BLOCK_DATA_VALUE( sCurrContext->mInfo.mValueInfo,
                                                                              aExprEvalInfo->mBlockIdx );

            if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
            {
                if( sExprTopEntry->mActionType == DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL )
                {
                    DTL_SET_NULL( KNL_GET_BLOCK_DATA_VALUE( aExprEvalInfo->mResultBlock,
                                                            aExprEvalInfo->mBlockIdx ) );
                    return STL_SUCCESS;
                }
            }
        
            /**
             * Pop Expression Stack
             */
        
            sExprTopEntry--;
            sExprStackEntryCount--;
        
            if( sCurrContext->mCast != NULL )
            {
                /**
                 * Function 수행
                 */

                if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
                {
                    DTL_SET_NULL( & sCurrContext->mCast->mCastResultBuf );
                }
                else
                {
                    sCurrContext->mCast->mArgs[0].mValue.mDataValue = 
                        sArgTopEntry->mValue.mDataValue;

                    STL_TRY( sCurrContext->mCast->mCastFuncPtr(
                                 DTL_CAST_INPUT_ARG_CNT,
                                 sCurrContext->mCast->mArgs,
                                 (void *) & sCurrContext->mCast->mCastResultBuf,
                                 (void *)(sCurrContext->mCast->mSrcTypeInfo),
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 KNL_ERROR_STACK( aEnv ) )
                             == STL_SUCCESS );
                }

                
                /**
                 * 수행 결과를 Argument Stack에 반영
                 */

                sArgTopEntry->mValue.mDataValue =
                    & sCurrContext->mCast->mCastResultBuf;
            }
        }

        if( sExprTopEntry->mExprType == KNL_EXPR_TYPE_FUNCTION )
        {
            /**
             * Function 수행 정보  
             *  : function 호출시 argument count만큼 읽을 수 있도록
             *    argument stack의 시작 주소를 넘겨준다.
             *  => function을 수행하는 곳에서 arguments 분석해서 사용
             */

            sFuncInfo    = & sExprTopEntry->mExpr.mFuncInfo;

            sArgEntries  = sArgTopEntry - sFuncInfo->mArgumentCount + 1;
                    
            sCurrContext = & sContextInfo->mContexts[ sFuncInfo->mOffset ];
            sFuncData    = sCurrContext->mInfo.mFuncData;
            sResultValue = & sFuncData->mResultValue;

            if( (sExprStackEntryCount == 1) && (sCurrContext->mCast == NULL) )
            {
                sResultValue = KNL_GET_BLOCK_DATA_VALUE( aExprEvalInfo->mResultBlock,
                                                         aExprEvalInfo->mBlockIdx );
            }


            /**
             * Function 수행
             */

            STL_TRY( sFuncData->mFuncPtr( sFuncInfo->mArgumentCount,
                                          sArgEntries,
                                          sResultValue,
                                          sFuncData->mInfo,
                                          KNL_DT_VECTOR(aEnv),
                                          aEnv,
                                          KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );
            
            if( DTL_IS_NULL( sResultValue ) )
            {
                if( sExprTopEntry->mActionType == DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL )
                {
                    DTL_SET_NULL( KNL_GET_BLOCK_DATA_VALUE( aExprEvalInfo->mResultBlock,
                                                            aExprEvalInfo->mBlockIdx ) );
                    break;
                }
            }
            
            /**
             * 수행 결과를 Argument Stack에 push
             */
                    
            sArgTopEntry = sArgEntries;
            sArgStackEntryCount -= sFuncInfo->mArgumentCount;
            sArgStackEntryCount++;


            /**
             * 결과값을 argument stack에 저장
             */

            sArgTopEntry->mValueType = DTL_VALUE_TYPE_VALUE;
            sArgTopEntry->mValue.mDataValue = sResultValue;
            
            if( sCurrContext->mCast != NULL )
            {
                if( sExprStackEntryCount == 1 )
                {
                    sResultValue = KNL_GET_BLOCK_DATA_VALUE( aExprEvalInfo->mResultBlock,
                                                             aExprEvalInfo->mBlockIdx );
                }
                else
                {
                    sResultValue = & sCurrContext->mCast->mCastResultBuf;
                }

                if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
                {
                    DTL_SET_NULL( sResultValue );
                }
                else
                {
                    sCurrContext->mCast->mArgs[0].mValue.mDataValue = 
                        sArgTopEntry->mValue.mDataValue;

                    STL_TRY( sCurrContext->mCast->mCastFuncPtr(
                                 DTL_CAST_INPUT_ARG_CNT,
                                 sCurrContext->mCast->mArgs,
                                 sResultValue,
                                 (void *)(sCurrContext->mCast->mSrcTypeInfo),
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 KNL_ERROR_STACK( aEnv ) )
                             == STL_SUCCESS );
                }
                
                
                /**
                 * 수행 결과를 Argument Stack에 반영
                 */

                sArgTopEntry->mValue.mDataValue = sResultValue;
            }
            
            /**
             * Pop Expression Stack
             */
        
            sExprTopEntry--;
            sExprStackEntryCount--;
        }
        
        if( sExprTopEntry->mExprType ==  KNL_EXPR_TYPE_LIST )
        {
            sArgTopEntry ++;
            sArgStackEntryCount++;
            
            sListInfo     = & sExprTopEntry->mExpr.mListInfo;
            
            STL_PARAM_VALIDATE( ( sListInfo->mPredicate == DTL_LIST_PREDICATE_FUNCTION ),
                                KNL_ERROR_STACK( aEnv ) );
            
            sCurrContext  = & sContextInfo->mContexts[ sListInfo->mOffset ];
            sListFunc     = sCurrContext->mInfo.mListFunc;
            sListStack    = sListFunc->mListStack;
            
            /**
             * List Stack Entries 구성
             */
            
            sListStackEntryCount            = sListStack->mEntryCount;
            sListEntries                    = sListStack->mEntries;
            
            sResultValue                    = & sListFunc->mResultValue;

            if( (sExprStackEntryCount == 1) && (sCurrContext->mCast == NULL) )
            {
                sResultValue = KNL_GET_BLOCK_DATA_VALUE( aExprEvalInfo->mResultBlock,
                                                         aExprEvalInfo->mBlockIdx );
            }
                
            /**
             * ResultValue ( 초기화 )
             */

            if( sListInfo->mArgumentCount == sListStackEntryCount )
            {
                DTL_SET_BOOLEAN_LENGTH( sResultValue );
            }

            
            /**
             * List Function Type에 따른 Entries구성
             */
            sListTopEntry = & sListEntries[ 0 ];

            /**
             * List Function 수행
             */
                    
            sListEntries  = sListTopEntry + 1;

            STL_TRY( sListFunc->mListFuncPtr( sListStackEntryCount,
                                              sListFunc->mValueCount,
                                              sListEntries,
                                              sContextInfo,
                                              aExprEvalInfo->mBlockIdx,
                                              & sIsResult,
                                              sResultValue,
                                              KNL_ERROR_STACK( aEnv ) )
                     == STL_SUCCESS );
                    
            /**
             * 결과값을 argument stack에 저장
             */
            if( ( DTL_IS_NULL( sResultValue ) ) &&
                ( sExprTopEntry->mActionType == DTL_ACTION_TYPE_CONDITION_JUMP_IS_NULL ) )
            {
                DTL_SET_NULL( KNL_GET_BLOCK_DATA_VALUE( aExprEvalInfo->mResultBlock,
                                                        aExprEvalInfo->mBlockIdx ) );
                break;
            }
            else
            {
                if( sIsResult == STL_TRUE )
                {
                    sListStackEntryCount = 0;
                }
                else
                {
                    /* Do Nothing */
                }
            }

            /**
             * 수행 결과를 Argument Stack에 push
             */            
            
            sArgTopEntry->mValueType        = DTL_VALUE_TYPE_VALUE;
            sArgTopEntry->mValue.mDataValue = sResultValue;

            sListStack->mEntryCount = ( sListStackEntryCount > 0 ) ?
                sListStackEntryCount : sListInfo->mArgumentCount;

            if( sCurrContext->mCast != NULL )
            {
                if( sExprStackEntryCount == 1 )
                {
                    sResultValue = KNL_GET_BLOCK_DATA_VALUE( aExprEvalInfo->mResultBlock,
                                                             aExprEvalInfo->mBlockIdx );
                }
                else
                {
                    sResultValue = & sCurrContext->mCast->mCastResultBuf;
                }
                    
                if( DTL_IS_NULL( sArgTopEntry->mValue.mDataValue ) )
                {
                    DTL_SET_NULL( sResultValue );
                }
                else
                {
                    sCurrContext->mCast->mArgs[0].mValue.mDataValue = 
                        sArgTopEntry->mValue.mDataValue;

                    STL_TRY( sCurrContext->mCast->mCastFuncPtr(
                                 DTL_CAST_INPUT_ARG_CNT,
                                 sCurrContext->mCast->mArgs,
                                 sResultValue,
                                 (void *)(sCurrContext->mCast->mSrcTypeInfo),
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 KNL_ERROR_STACK( aEnv ) )
                             == STL_SUCCESS );
                }
                
                
                /**
                 * 수행 결과를 Argument Stack에 반영
                 */

                sArgTopEntry->mValue.mDataValue = sResultValue;
            }
             
            sExprTopEntry--;
            sExprStackEntryCount--;
                 
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief knlExprContextInfo 생성
 * @param[in]     aContextCnt        context count 
 * @param[out]    aContextInfo       knlExprContextInfo
 * @param[in]     aMemMgr            영역 기반 메모리 할당자 ( knlRegionMem )
 * @param[in,out] aEnv               커널 Environment
 */
stlStatus knlCreateContextInfo( stlInt32               aContextCnt,
                                knlExprContextInfo  ** aContextInfo,
                                knlRegionMem         * aMemMgr,
                                knlEnv               * aEnv )
{
    knlExprContextInfo  * sContextInfo = NULL;
    knlExprContext      * sContexts    = NULL;
    knlValueStack       * sArgStack    = NULL;


    /**
     * Expression Context Data 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aMemMgr,
                                STL_SIZEOF( knlExprContextInfo ),
                                (void **) & sContextInfo,
                                aEnv )
             == STL_SUCCESS );

    stlMemset( sContextInfo, 0x00, STL_SIZEOF( knlExprContextInfo ) );

    if( aContextCnt == 0 )
    {
        /**
         * Expression Context Info 설정
         */

        sContextInfo->mCount    = 0;
        sContextInfo->mContexts = NULL;
        sContextInfo->mArgStack = NULL;
    }
    else
    {
        /**
         * Expression Context의 context 리스트 공간 할당
         */

        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( knlExprContext ) * aContextCnt,
                                    (void **) & sContexts,
                                    aEnv )
                 == STL_SUCCESS );
    
        stlMemset( sContexts,
                   0x00,
                   STL_SIZEOF( knlExprContext ) * aContextCnt );

    
        /**
         * Argument Stack 설정
         */

        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( knlValueStack ),
                                    (void **) & sArgStack,
                                    aEnv )
                 == STL_SUCCESS );

        sArgStack->mMaxEntryCount   = ( aContextCnt >= KNL_EXPR_MAX_ENTRY_CNT ? KNL_EXPR_MAX_ENTRY_CNT : aContextCnt );
        sArgStack->mValueEntryCount = 0;

        STL_TRY( knlAllocRegionMem( aMemMgr,
                                    STL_SIZEOF( dtlValueEntry ) * ( sArgStack->mMaxEntryCount + 1 ),
                                    (void **) & sArgStack->mEntries,
                                    aEnv )
                 == STL_SUCCESS );

        stlMemset( sArgStack->mEntries,
                   0x00,
                   STL_SIZEOF( dtlValueEntry )  * ( sArgStack->mMaxEntryCount + 1 ) );

    
        /**
         * Expression Context Info 설정
         */

        sContextInfo->mCount    = aContextCnt;
        sContextInfo->mContexts = sContexts;
        sContextInfo->mArgStack = sArgStack;
    }

    
    /**
     * OUTPUT 설정
     */
    
    *aContextInfo = sContextInfo;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Stack을 위한 Value Block을 생성한다.
 * @param[out]  aValueBlock        Value Block
 * @param[in]   aDataValue         Data Value
 * @param[in]   aBufferSize        Data Value Buffer Size
 * @param[in]   aDBType            DB Data Type
 * @param[in]   aArgNum1           Precision (타입마다 용도가 다름)
 *                            <BR> - time,timetz,timestamp,timestamptz타입은
 *                            <BR>   fractional second precision 정보
                              <BR> - interval 타입은 leading precision 정보
                              <BR> - 그외 타입은 precision 정보
 * @param[in]   aArgNum2           scale (타입마다 용도가 다름)
 *                            <BR> - interval 타입은 fractional second precision 정보
                              <BR> - 그외 타입은 scale 정보 
 * @param[in]   aStringLengthUnit  string length unit (dtlStringLengthUnit 참조)
 * @param[in]   aIntervalIndicator interval indicator (dtlIntervalIndicator 참조) 
 * @param[in]   aRegionMem         Region Memory
 * @param[in]   aEnv               Environment
 * @remarks
 */
stlStatus knlCreateStackBlock( knlValueBlock        ** aValueBlock,
                               dtlDataValue          * aDataValue,
                               stlInt64                aBufferSize,
                               dtlDataType             aDBType,
                               stlInt64                aArgNum1,
                               stlInt64                aArgNum2,
                               dtlStringLengthUnit     aStringLengthUnit,
                               dtlIntervalIndicator    aIntervalIndicator,
                               knlRegionMem          * aRegionMem,
                               knlEnv                * aEnv )
{
    knlValueBlock      * sValueBlock = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aValueBlock != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataValue != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( aDataValue->mType == aDBType ) ||
                        ( ( dtlDataTypeGroup[ aDataValue->mType ] == DTL_GROUP_NUMBER ) &&
                          ( dtlDataTypeGroup[ aDBType ] == DTL_GROUP_NUMBER ) ),
                        KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBufferSize > 0, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, KNL_ERROR_STACK(aEnv) );

    
    /**
     * Value Block 정보 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF(knlValueBlock),
                                (void **) & sValueBlock,
                                aEnv )
             == STL_SUCCESS );
    stlMemset( sValueBlock, 0x00, STL_SIZEOF(knlValueBlock) );

    sValueBlock->mIsSepBuff         = STL_FALSE;
    sValueBlock->mAllocLayer        = STL_LAYER_SQL_PROCESSOR;
    sValueBlock->mAllocBlockCnt     = 1;
    sValueBlock->mUsedBlockCnt      = 0;
    sValueBlock->mSkipBlockCnt      = 0;
    sValueBlock->mArgNum1           = aArgNum1;
    sValueBlock->mArgNum2           = aArgNum2;
    sValueBlock->mStringLengthUnit  = aStringLengthUnit;
    sValueBlock->mIntervalIndicator = aIntervalIndicator;
    sValueBlock->mAllocBufferSize   = aBufferSize;
    sValueBlock->mDataValue         = aDataValue;

    
    /**
     * Output 설정
     */

    *aValueBlock = sValueBlock;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */


