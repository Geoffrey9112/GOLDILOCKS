/*******************************************************************************
 * smoAggregation.c
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
 * @file smoAggregation.c
 * @brief Storage Manager Layer Aggregation Operator Routines
 */

#include <sml.h>
#include <smDef.h>
#include <smo.h>
#include <smoDef.h>


/**
 * @addtogroup smlTable
 * @{
 */

smlAggrFuncModule gSmlAggrFuncInfoArr[ KNL_BUILTIN_AGGREGATION_UNARY_MAX ] =
{
    {
        KNL_BUILTIN_AGGREGATION_INVALID,
        smoInvalidAssign,
        smoInvalidBuild,
        smoInvalidBlockBuild
    },
    
    {
        KNL_BUILTIN_AGGREGATION_SUM,
        smoSumAssign,
        smoSumBuild,
        smoSumBlockBuild
    },

    {
        KNL_BUILTIN_AGGREGATION_COUNT,
        smoCountAssign,
        smoCountBuild,
        smoCountBlockBuild
    },

    {
        KNL_BUILTIN_AGGREGATION_COUNT_ASTERISK,
        smoCountAsteriskAssign,
        smoCountAsteriskBuild,
        smoCountAsteriskBlockBuild
    },

    {
        KNL_BUILTIN_AGGREGATION_MIN,
        smoMinAssign,
        smoMinBuild,
        smoMinBlockBuild
    },

    {
        KNL_BUILTIN_AGGREGATION_MAX,
        smoMaxAssign,
        smoMaxBuild,
        smoMaxBlockBuild
    }
};


/************************************************
 * Aggregation Functions
 ***********************************************/

/**
 * @brief Aggregation Function List에 속한 모든 Aggregation에 값을 assign한다.
 * @param[in,out]  aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Value Block에서 사용할 block idx
 * @param[in]      aEnv            Environment
 */
stlStatus smoAssignAggrFunc( smlAggrFuncInfo   * aAggrExecInfo,
                             stlInt32            aBlockIdx,
                             smlEnv            * aEnv )
{
    smlAggrFuncInfo   * sAggrExecInfo = aAggrExecInfo;

    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );


    /**
     * Aggregation Function들에 대한 Initialize
     */
    
    while( sAggrExecInfo != NULL )
    {
        STL_TRY( sAggrExecInfo->mAggrFuncModule->mAssignFuncPtr( sAggrExecInfo,
                                                                 aBlockIdx,
                                                                 aEnv )
                 == STL_SUCCESS );

        sAggrExecInfo = sAggrExecInfo->mNext;
    }

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation Function List에 속한 모든 Aggregation 연산을 수행한다.
 * @param[in,out]  aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Value Block에서 사용할 block idx
 * @param[in]      aEnv            Environment
 */
stlStatus smoBuildAggrFunc( smlAggrFuncInfo   * aAggrExecInfo,
                            stlInt32            aBlockIdx,
                            smlEnv            * aEnv )
{
    smlAggrFuncInfo   * sAggrExecInfo = aAggrExecInfo;

    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );


    /**
     * Aggregation Function들에 대한 Initialize
     */
    
    while( sAggrExecInfo != NULL )
    {
        STL_TRY( sAggrExecInfo->mAggrFuncModule->mBuildFuncPtr( sAggrExecInfo,
                                                                aBlockIdx,
                                                                aEnv )
                 == STL_SUCCESS );

        sAggrExecInfo = sAggrExecInfo->mNext;
    }

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation Function List에 속한 모든 Aggregation 연산을 수행한다.
 * @param[in,out]  aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Value Block 개수
 * @param[in]      aEnv            Environment
 */
stlStatus smoBlockBuildAggrFunc( smlAggrFuncInfo   * aAggrExecInfo,
                                 stlInt32            aStartBlockIdx,
                                 stlInt32            aBlockCnt,
                                 smlEnv            * aEnv )
{
    smlAggrFuncInfo   * sAggrExecInfo = aAggrExecInfo;

    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );


    /**
     * Aggregation Function들에 대한 Initialize
     */
    
    while( sAggrExecInfo != NULL )
    {
        STL_TRY( sAggrExecInfo->mAggrFuncModule->mBlockBuildFuncPtr( sAggrExecInfo,
                                                                     aStartBlockIdx,
                                                                     aBlockCnt,
                                                                     aEnv )
                 == STL_SUCCESS );

        sAggrExecInfo = sAggrExecInfo->mNext;
    }

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/************************************************
 * Invalid Function
 ***********************************************/

/**
 * @brief Aggregation에 값을 설정한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Block에서 사용할 block idx
 * @param[in]      aEnv            Environment
 */
stlStatus smoInvalidAssign( smlAggrFuncInfo   * aAggrExecInfo,
                            stlInt32            aBlockIdx,
                            smlEnv            * aEnv )
{
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );


    /**
     * Output 설정
     */

    STL_DASSERT( 0 );
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Value Block에서 사용할 block idx
 * @param[in]      aEnv            Environment
 */
stlStatus smoInvalidBuild( smlAggrFuncInfo   * aAggrExecInfo,
                           stlInt32            aBlockIdx,
                           smlEnv            * aEnv )
{
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );


    /**
     * Output 설정
     */

    STL_DASSERT( 0 );
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Value Block 개수
 * @param[in]      aEnv            Environment
 */
stlStatus smoInvalidBlockBuild( smlAggrFuncInfo   * aAggrExecInfo,
                                stlInt32            aStartBlockIdx,
                                stlInt32            aBlockCnt,
                                smlEnv            * aEnv )
{
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );


    /**
     * Output 설정
     */

    STL_DASSERT( 0 );
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/************************************************
 * SUM Function
 ***********************************************/

/**
 * @brief Aggregation에 값을 설정한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Block에서 사용할 block idx
 * @param[in]      aEnv            Environment
 */
stlStatus smoSumAssign( smlAggrFuncInfo   * aAggrExecInfo,
                        stlInt32            aBlockIdx,
                        smlEnv            * aEnv )
{
    dtlDataValue     * sDataValue;
    knlExprEvalInfo  * sEvalInfo;
    

    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, KNL_ERROR_STACK( aEnv ) );


    /**
     * Aggregation Value Block List 초기화
     */

    STL_DASSERT( aAggrExecInfo->mAggrValueList->mNext == NULL );
    DTL_SET_NULL( KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aBlockIdx ) );
    
    
    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[0];
    
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockIdx = aBlockIdx;
        
        STL_TRY( knlEvaluateOneExpression( sEvalInfo,
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
        
        
    /**
     * SUM 연산 수행
     */

    sDataValue = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mInputArgList, aBlockIdx );
    
    if( DTL_IS_NULL( sDataValue ) )
    {
        /* Do Nothing */
    }
    else
    {
        STL_TRY( gKnlAddValueToNumFuncPtrArr[ sDataValue->mType ] (
                     sDataValue,
                     KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aBlockIdx ),
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Value Block에서 사용할 block idx
 * @param[in]      aEnv            Environment
 */
stlStatus smoSumBuild( smlAggrFuncInfo   * aAggrExecInfo,
                       stlInt32            aBlockIdx,
                       smlEnv            * aEnv )
{
    dtlDataValue     * sDataValue;
    knlExprEvalInfo  * sEvalInfo;

    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, KNL_ERROR_STACK( aEnv ) );
    
    
    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[0];
    
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockIdx = aBlockIdx;
        
        STL_TRY( knlEvaluateOneExpression( sEvalInfo,
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
        
        
    /**
     * SUM 연산 수행
     */

    sDataValue = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mInputArgList, aBlockIdx );
    
    if( DTL_IS_NULL( sDataValue ) )
    {
        /* Do Nothing */
    }
    else
    {
        STL_TRY( gKnlAddValueToNumFuncPtrArr[ sDataValue->mType ] (
                     sDataValue,
                     KNL_GET_BLOCK_FIRST_DATA_VALUE( aAggrExecInfo->mAggrValueList ),
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Value Block 개수
 * @param[in]      aEnv            Environment
 */
stlStatus smoSumBlockBuild( smlAggrFuncInfo   * aAggrExecInfo,
                            stlInt32            aStartBlockIdx,
                            stlInt32            aBlockCnt,
                            smlEnv            * aEnv )
{
    knlExprEvalInfo  * sEvalInfo;

    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, KNL_ERROR_STACK( aEnv ) );
    
    
    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[0];
    
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockCount = aBlockCnt;
        sEvalInfo->mBlockIdx = aStartBlockIdx;
        
        STL_TRY( knlEvaluateBlockExpression( sEvalInfo,
                                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
        
        
    /**
     * SUM 연산 수행
     */

    STL_TRY( gKnlAddToNumFuncPtrArr[ KNL_GET_BLOCK_DB_TYPE( aAggrExecInfo->mInputArgList ) ] (
                 aAggrExecInfo->mInputArgList,
                 aAggrExecInfo->mAggrValueList,
                 aStartBlockIdx,
                 aBlockCnt,
                 0, /* result block idx */
                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/************************************************
 * COUNT Function
 ***********************************************/

/**
 * @brief Aggregation에 값을 설정한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Value Block에서 사용할 block idx
 * @param[in]      aEnv            Environment
 */
stlStatus smoCountAssign( smlAggrFuncInfo   * aAggrExecInfo,
                          stlInt32            aBlockIdx,
                          smlEnv            * aEnv )
{
    dtlDataValue     * sDataValue;
    knlExprEvalInfo  * sEvalInfo;
    dtlBigIntType    * sBigInt;
    
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, KNL_ERROR_STACK( aEnv ) );

    
    /**
     * Aggregation Value Block List 초기화
     */

    STL_DASSERT( aAggrExecInfo->mAggrValueList->mNext == NULL );

    /* Set Zero */
    sDataValue = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aBlockIdx );
    sDataValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    sBigInt    = (dtlBigIntType *) sDataValue->mValue;
    *sBigInt   = 0;
    
    
    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[0];
    
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockIdx = aBlockIdx;
        
        STL_TRY( knlEvaluateOneExpression( sEvalInfo,
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }


    /**
     * COUNT 연산 수행
     */

    if( DTL_IS_NULL( KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mInputArgList, aBlockIdx ) ) )
    {
        /* Do Nothing */
    }
    else
    {
        sBigInt = (dtlBigIntType *) KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aBlockIdx )->mValue;
        (*sBigInt)++;
    }
        

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Value Block에서 사용할 block idx
 * @param[in]      aEnv            Environment
 */
stlStatus smoCountBuild( smlAggrFuncInfo   * aAggrExecInfo,
                         stlInt32            aBlockIdx,
                         smlEnv            * aEnv )
{
    dtlBigIntType         * sBigInt;
    knlExprEvalInfo       * sEvalInfo;

    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, KNL_ERROR_STACK( aEnv ) );


    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[0];
    
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockIdx = aBlockIdx;
        
        STL_TRY( knlEvaluateOneExpression( sEvalInfo,
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }


    /**
     * COUNT 연산 수행
     */

    if( DTL_IS_NULL( KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mInputArgList, aBlockIdx ) ) )
    {
        /* Do Nothing */
    }
    else
    {
        sBigInt = (dtlBigIntType *) KNL_GET_BLOCK_FIRST_DATA_VALUE( aAggrExecInfo->mAggrValueList )->mValue;
        (*sBigInt)++;
    }

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Value Block 개수
 * @param[in]      aEnv            Environment
 */
stlStatus smoCountBlockBuild( smlAggrFuncInfo   * aAggrExecInfo,
                              stlInt32            aStartBlockIdx,
                              stlInt32            aBlockCnt,
                              smlEnv            * aEnv )
{
    dtlBigIntType         * sBigInt;
    knlExprEvalInfo       * sEvalInfo;
    stlInt32                sLoop      = 0;
    stlInt32                sAddCnt    = 0;
    dtlDataValue          * sDataValue = NULL;

    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, KNL_ERROR_STACK( aEnv ) );


    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[0];
    
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockCount = aBlockCnt;
        sEvalInfo->mBlockIdx = aStartBlockIdx;
        
        STL_TRY( knlEvaluateBlockExpression( sEvalInfo,
                                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }


    /**
     * COUNT 연산 수행
     */

    sDataValue = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mInputArgList, aStartBlockIdx );
    sBigInt    = (dtlBigIntType *) KNL_GET_BLOCK_FIRST_DATA_VALUE( aAggrExecInfo->mAggrValueList )->mValue;
        
    if( KNL_GET_BLOCK_IS_SEP_BUFF( aAggrExecInfo->mInputArgList ) == STL_TRUE )
    {
        for( sLoop = 0 ; sLoop < aBlockCnt ; sLoop++ )
        {
            sAddCnt += ( DTL_IS_NULL( sDataValue ) == STL_FALSE );
            sDataValue++;
        }

        *sBigInt += sAddCnt;
    }
    else
    {
        if( DTL_IS_NULL( sDataValue ) )
        {
            /* Do Nothing */
        }
        else
        {
            *sBigInt += aBlockCnt;
        }
    }

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/************************************************
 * COUNT ASTERISK Function
 ***********************************************/

/**
 * @brief Aggregation에 값을 설정한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Value Block에서 사용할 block idx
 * @param[in]      aEnv            Environment
 */
stlStatus smoCountAsteriskAssign( smlAggrFuncInfo   * aAggrExecInfo,
                                  stlInt32            aBlockIdx,
                                  smlEnv            * aEnv )
{
    dtlDataValue     * sDataValue;
    dtlBigIntType    * sBigInt;
    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList == NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, KNL_ERROR_STACK( aEnv ) );

    
    /**
     * Aggregation Value Block List 초기화
     */

    STL_DASSERT( aAggrExecInfo->mAggrValueList->mNext == NULL );

    /* Set One */
    sDataValue = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aBlockIdx );
    sDataValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    sBigInt    = (dtlBigIntType *) sDataValue->mValue;
    *sBigInt   = 1;
    

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Value Block에서 사용할 block idx
 * @param[in]      aEnv            Environment
 */
stlStatus smoCountAsteriskBuild( smlAggrFuncInfo   * aAggrExecInfo,
                                 stlInt32            aBlockIdx,
                                 smlEnv            * aEnv )
{
    dtlBigIntType         * sBigInt;


    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList == NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, KNL_ERROR_STACK( aEnv ) );


    /**
     * COUNT 연산 수행
     */

    sBigInt = (dtlBigIntType *) KNL_GET_BLOCK_FIRST_DATA_VALUE( aAggrExecInfo->mAggrValueList )->mValue;

    (*sBigInt)++;
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Value Block 개수
 * @param[in]      aEnv            Environment
 */
stlStatus smoCountAsteriskBlockBuild( smlAggrFuncInfo   * aAggrExecInfo,
                                      stlInt32            aStartBlockIdx,
                                      stlInt32            aBlockCnt,
                                      smlEnv            * aEnv )
{
    dtlBigIntType         * sBigInt;


    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList == NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, KNL_ERROR_STACK( aEnv ) );


    /**
     * COUNT 연산 수행
     */

    sBigInt = (dtlBigIntType *) KNL_GET_BLOCK_FIRST_DATA_VALUE( aAggrExecInfo->mAggrValueList )->mValue;

    *sBigInt += aBlockCnt;
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/************************************************
 * MIN Function
 ***********************************************/

/**
 * @brief Aggregation을 초기화한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Value Block에서 사용할 block idx
 * @param[in]      aEnv            Environment
 */
stlStatus smoMinAssign( smlAggrFuncInfo   * aAggrExecInfo,
                        stlInt32            aBlockIdx,
                        smlEnv            * aEnv )
{
    knlExprEvalInfo  * sEvalInfo;
    dtlDataValue     * sDataValue1;
    dtlDataValue     * sDataValue2;

    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, KNL_ERROR_STACK( aEnv ) );

    
    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[0];
    
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockIdx = aBlockIdx;
        
        STL_TRY( knlEvaluateOneExpression( sEvalInfo,
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    

    /**
     * MIN 연산 수행
     */

    sDataValue1 = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mInputArgList, aBlockIdx );
    sDataValue2 = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aBlockIdx );
    
    STL_TRY( knlCopyDataValue( sDataValue1,
                               sDataValue2,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Value Block에서 사용할 block idx
 * @param[in]      aEnv            Environment
 */
stlStatus smoMinBuild( smlAggrFuncInfo   * aAggrExecInfo,
                       stlInt32            aBlockIdx,
                       smlEnv            * aEnv )
{
    knlExprEvalInfo       * sEvalInfo;
    dtlDataValue          * sDataValue;
    dtlDataValue          * sResultValue;
    

    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, KNL_ERROR_STACK( aEnv ) );
    

    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[0];
    
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockIdx = aBlockIdx;
        
        STL_TRY( knlEvaluateOneExpression( sEvalInfo,
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
        
        
    /**
     * MIN 연산 수행
     */
    
    sDataValue = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mInputArgList, aBlockIdx );
    sResultValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( aAggrExecInfo->mAggrValueList );
            
    if( DTL_IS_NULL( sDataValue ) )
    {
        /* Do Nothing */
    }
    else
    {
        if( DTL_IS_NULL( sResultValue ) )
        {
            STL_TRY( knlCopyDataValue( sDataValue,
                                       sResultValue,
                                       KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        else
        {
            dtlBooleanType          sBoolean    = STL_FALSE;
            dtlDataValue            sTempValue  = { DTL_TYPE_BOOLEAN, STL_SIZEOF(dtlBooleanType), 0, & sBoolean };
            dtlDataValue          * sCompResult = & sTempValue;
            dtlBuiltInFuncPtr       sCompFunc;
            dtlDataType             sDataType;
            dtlValueEntry           sValueEntry[ 2 ];
            
            sDataType    = sDataValue->mType;
            sCompFunc    = dtlCompOperArg2FuncList[ sDataType ][ sDataType ][ DTL_COMP_OPER_IS_LESS_THAN ];
        
            STL_DASSERT( sCompFunc != NULL );

            sValueEntry[0].mValue.mDataValue = sDataValue;
            sValueEntry[1].mValue.mDataValue = sResultValue;
                
            STL_TRY( sCompFunc( 2,
                                sValueEntry,
                                sCompResult,
                                NULL,
                                NULL,
                                NULL,
                                aEnv )
                     == STL_SUCCESS );

            if( DTL_BOOLEAN_IS_TRUE( sCompResult ) )
            {
                STL_TRY( knlCopyDataValue( sDataValue,
                                           sResultValue,
                                           KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }
            else
            {
                /* Do Nothing */
            }
        }
    }
    
        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Value Block 개수
 * @param[in]      aEnv            Environment
 */
stlStatus smoMinBlockBuild( smlAggrFuncInfo   * aAggrExecInfo,
                            stlInt32            aStartBlockIdx,
                            stlInt32            aBlockCnt,
                            smlEnv            * aEnv )
{
    stlInt32                sLoop        = 0;
    dtlBooleanType          sBoolean;
    dtlDataValue            sTempValue   = { DTL_TYPE_BOOLEAN, STL_SIZEOF(dtlBooleanType), 0, & sBoolean };
    dtlDataValue          * sDataValue   = NULL;
    dtlDataValue          * sResultValue = NULL;
    dtlDataValue          * sCompResult  = NULL;
    knlExprEvalInfo       * sEvalInfo    = NULL;
    dtlBuiltInFuncPtr       sCompFunc    = NULL;
    dtlDataType             sDataType    = DTL_TYPE_NA;
    dtlValueEntry           sValueEntry[ 2 ];
    

    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, KNL_ERROR_STACK( aEnv ) );
    

    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[0];
    
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockCount = aBlockCnt;
        sEvalInfo->mBlockIdx = aStartBlockIdx;
        
        STL_TRY( knlEvaluateBlockExpression( sEvalInfo,
                                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
        
        
    /**
     * MIN 연산 수행
     */
    
    sDataValue   = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mInputArgList, aStartBlockIdx );
    sResultValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( aAggrExecInfo->mAggrValueList );
    sCompResult  = & sTempValue;
    sDataType    = sDataValue->mType;
    sCompFunc    = dtlCompOperArg2FuncList[ sDataType ][ sDataType ][ DTL_COMP_OPER_IS_LESS_THAN ];
        
    STL_DASSERT( sCompFunc != NULL );

    if( KNL_GET_BLOCK_IS_SEP_BUFF( aAggrExecInfo->mInputArgList ) == STL_TRUE )
    {
        sValueEntry[1].mValue.mDataValue = sResultValue;
            
        for( sLoop = 0 ; sLoop < aBlockCnt ; sLoop++ )
        {
            if( DTL_IS_NULL( sDataValue ) )
            {
                /* Do Nothing */
            }
            else
            {
                if( DTL_IS_NULL( sResultValue ) )
                {
                    STL_TRY( knlCopyDataValue( sDataValue,
                                               sResultValue,
                                               KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    sValueEntry[0].mValue.mDataValue = sDataValue;
                
                    STL_TRY( sCompFunc( 2,
                                        sValueEntry,
                                        sCompResult,
                                        NULL,
                                        NULL,
                                        NULL,
                                        aEnv )
                             == STL_SUCCESS );

                    if( DTL_BOOLEAN_IS_TRUE( sCompResult ) )
                    {
                        STL_TRY( knlCopyDataValue( sDataValue,
                                                   sResultValue,
                                                   KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
            }
                
            sDataValue++;
        }
    }
    else
    {
        if( DTL_IS_NULL( sDataValue ) )
        {
            /* Do Nothing */
        }
        else
        {
            if( DTL_IS_NULL( sResultValue ) )
            {
                STL_TRY( knlCopyDataValue( sDataValue,
                                           sResultValue,
                                           KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }
            else
            {
                sValueEntry[0].mValue.mDataValue = sDataValue;
                sValueEntry[1].mValue.mDataValue = sResultValue;

                STL_TRY( sCompFunc( 2,
                                    sValueEntry,
                                    sCompResult,
                                    NULL,
                                    NULL,
                                    NULL,
                                    aEnv )
                         == STL_SUCCESS );

                if( DTL_BOOLEAN_IS_TRUE( sCompResult ) )
                {
                    STL_TRY( knlCopyDataValue( sDataValue,
                                               sResultValue,
                                               KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }
    }
    
        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/************************************************
 * MAX Function
 ***********************************************/

/**
 * @brief Aggregation에 값을 설정한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Value Block에서 사용할 block idx
 * @param[in]      aEnv            Environment
 */
stlStatus smoMaxAssign( smlAggrFuncInfo   * aAggrExecInfo,
                        stlInt32            aBlockIdx,
                        smlEnv            * aEnv )
{
    knlExprEvalInfo  * sEvalInfo;
    dtlDataValue     * sDataValue1;
    dtlDataValue     * sDataValue2;

    
    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, KNL_ERROR_STACK( aEnv ) );

    
    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[0];
    
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockIdx = aBlockIdx;
        
        STL_TRY( knlEvaluateOneExpression( sEvalInfo,
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    

    /**
     * MIN 연산 수행
     */

    STL_DASSERT( aAggrExecInfo->mAggrValueList->mNext == NULL );

    sDataValue1 = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mInputArgList, aBlockIdx );
    sDataValue2 = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mAggrValueList, aBlockIdx );
    
    STL_TRY( knlCopyDataValue( sDataValue1,
                               sDataValue2,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aBlockIdx       Value Block에서 사용할 block idx
 * @param[in]      aEnv            Environment
 */
stlStatus smoMaxBuild( smlAggrFuncInfo   * aAggrExecInfo,
                       stlInt32            aBlockIdx,
                       smlEnv            * aEnv )
{
    knlExprEvalInfo       * sEvalInfo;
    dtlDataValue          * sDataValue;
    dtlDataValue          * sResultValue;


    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, KNL_ERROR_STACK( aEnv ) );
    

    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[0];
    
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockIdx = aBlockIdx;
        
        STL_TRY( knlEvaluateOneExpression( sEvalInfo,
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
        
        
    /**
     * MAX 연산 수행
     */

    sDataValue   = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mInputArgList, aBlockIdx );
    sResultValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( aAggrExecInfo->mAggrValueList );
        
            
    if( DTL_IS_NULL( sDataValue ) )
    {
        /* Do Nothing */
    }
    else
    {
        if( DTL_IS_NULL( sResultValue ) )
        {
            STL_TRY( knlCopyDataValue( sDataValue,
                                       sResultValue,
                                       KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        else
        {
            dtlBooleanType          sBoolean    = STL_FALSE;
            dtlDataValue            sTempValue  = { DTL_TYPE_BOOLEAN, STL_SIZEOF(dtlBooleanType), 0, & sBoolean };
            dtlDataValue          * sCompResult = & sTempValue;
            dtlBuiltInFuncPtr       sCompFunc;
            dtlDataType             sDataType;
            dtlValueEntry           sValueEntry[ 2 ];
            
            sDataType    = sDataValue->mType;
            sCompFunc    = dtlCompOperArg2FuncList[ sDataType ][ sDataType ][ DTL_COMP_OPER_IS_GREATER_THAN ];
        
            STL_DASSERT( sCompFunc != NULL );

            sValueEntry[0].mValue.mDataValue = sDataValue;
            sValueEntry[1].mValue.mDataValue = sResultValue;
                
            STL_TRY( sCompFunc( 2,
                                sValueEntry,
                                sCompResult,
                                NULL,
                                NULL,
                                NULL,
                                aEnv )
                     == STL_SUCCESS );

            if( DTL_BOOLEAN_IS_TRUE( sCompResult ) )
            {
                STL_TRY( knlCopyDataValue( sDataValue,
                                           sResultValue,
                                           KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }
            else
            {
                /* Do Nothing */
            }
        }
    }
    
        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrExecInfo   Aggregation Execution 정보
 * @param[in]      aStartBlockIdx  유효한 Value 의 시작 위치
 * @param[in]      aBlockCnt       Value Block 개수
 * @param[in]      aEnv            Environment
 */
stlStatus smoMaxBlockBuild( smlAggrFuncInfo   * aAggrExecInfo,
                            stlInt32            aStartBlockIdx,
                            stlInt32            aBlockCnt,
                            smlEnv            * aEnv )
{
    stlInt32                sLoop        = 0;
    dtlBooleanType          sBoolean;
    dtlDataValue            sTempValue   = { DTL_TYPE_BOOLEAN, STL_SIZEOF(dtlBooleanType), 0, & sBoolean };
    dtlDataValue          * sDataValue   = NULL;
    dtlDataValue          * sResultValue = NULL;
    dtlDataValue          * sCompResult  = NULL;
    knlExprEvalInfo       * sEvalInfo    = NULL;
    dtlBuiltInFuncPtr       sCompFunc    = NULL;
    dtlDataType             sDataType    = DTL_TYPE_NA;
    dtlValueEntry           sValueEntry[ 2 ];


    /*
     * Parameter Validation
     */ 
    
    STL_PARAM_VALIDATE( aAggrExecInfo != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mInputArgList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aAggrExecInfo->mAggrValueList->mNext == NULL, KNL_ERROR_STACK( aEnv ) );
    

    /**
     * Aggregation내 Expression Stack 수행
     */

    sEvalInfo = aAggrExecInfo->mEvalInfo[0];
    
    if( sEvalInfo == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sEvalInfo->mBlockCount = aBlockCnt;
        sEvalInfo->mBlockIdx = aStartBlockIdx;
        
        STL_TRY( knlEvaluateBlockExpression( sEvalInfo,
                                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
        
        
    /**
     * MAX 연산 수행
     */

    sDataValue   = KNL_GET_BLOCK_DATA_VALUE( aAggrExecInfo->mInputArgList, aStartBlockIdx );
    sResultValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( aAggrExecInfo->mAggrValueList );
    sCompResult  = & sTempValue;
    sDataType    = sDataValue->mType;
    sCompFunc    = dtlCompOperArg2FuncList[ sDataType ][ sDataType ][ DTL_COMP_OPER_IS_GREATER_THAN ];
        
    STL_DASSERT( sCompFunc != NULL );

    if( KNL_GET_BLOCK_IS_SEP_BUFF( aAggrExecInfo->mInputArgList ) == STL_TRUE )
    {
        sValueEntry[1].mValue.mDataValue = sResultValue;
            
        for( sLoop = 0 ; sLoop < aBlockCnt ; sLoop++ )
        {
            if( DTL_IS_NULL( sDataValue ) )
            {
                /* Do Nothing */
            }
            else
            {
                if( DTL_IS_NULL( sResultValue ) )
                {
                    STL_TRY( knlCopyDataValue( sDataValue,
                                               sResultValue,
                                               KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    sValueEntry[0].mValue.mDataValue = sDataValue;
                
                    STL_TRY( sCompFunc( 2,
                                        sValueEntry,
                                        sCompResult,
                                        NULL,
                                        NULL,
                                        NULL,
                                        aEnv )
                             == STL_SUCCESS );

                    if( DTL_BOOLEAN_IS_TRUE( sCompResult ) )
                    {
                        STL_TRY( knlCopyDataValue( sDataValue,
                                                   sResultValue,
                                                   KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
            }
                
            sDataValue++;
        }
    }
    else
    {
        if( DTL_IS_NULL( sDataValue ) )
        {
            /* Do Nothing */
        }
        else
        {
            if( DTL_IS_NULL( sResultValue ) )
            {
                STL_TRY( knlCopyDataValue( sDataValue,
                                           sResultValue,
                                           KNL_ENV(aEnv) )
                         == STL_SUCCESS );
            }
            else
            {
                sValueEntry[0].mValue.mDataValue = sDataValue;
                sValueEntry[1].mValue.mDataValue = sResultValue;

                STL_TRY( sCompFunc( 2,
                                    sValueEntry,
                                    sCompResult,
                                    NULL,
                                    NULL,
                                    NULL,
                                    aEnv )
                         == STL_SUCCESS );

                if( DTL_BOOLEAN_IS_TRUE( sCompResult ) )
                {
                    STL_TRY( knlCopyDataValue( sDataValue,
                                               sResultValue,
                                               KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }
    }
    
        
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} */
