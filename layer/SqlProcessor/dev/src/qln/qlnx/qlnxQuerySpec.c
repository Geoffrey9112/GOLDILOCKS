/*******************************************************************************
 * qlnxQuerySpec.c
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
 * @file qlnxQuerySpec.c
 * @brief SQL Processor Layer Execution Node - QUERY SPECIFICATION
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

/**
 * @brief QUERY SPEC node를 초기화한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark 첫번째 Execution에서 수행한다.
 *    <BR> Execution을 위한 정보를 보강한다.
 */
stlStatus qlnxInitializeQuerySpec( smlTransId              aTransID,
                                   smlStatement          * aStmt,
                                   qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv )
{
    /* qllExecutionNode  * sExecNode       = NULL; */
    /* qlnoQuerySpec     * sOptQuerySpec   = NULL; */
    /* qlnxQuerySpec     * sExeQuerySpec   = NULL; */

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_QUERY_SPEC_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * QUERY SPEC Optimization Node 획득
     */

    /* sOptQuerySpec = (qlnoQuerySpec *) aOptNode->mOptNode; */


    /**
     * Common Execution Node 획득
     */
        
    /* sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, */
    /*                                     QLL_GET_OPT_NODE_IDX( aOptNode ) ); */


    /**
     * QUERY SPEC Execution Node 할당
     */

    /* sExeQuerySpec = (qlnxQuerySpec *) sExecNode->mExecNode; */

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief QUERY SPEC node를 수행한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution 첫번째 수행시 호출한다.
 */
stlStatus qlnxExecuteQuerySpec( smlTransId              aTransID,
                                smlStatement          * aStmt,
                                qllDBCStmt            * aDBCStmt,
                                qllStatement          * aSQLStmt,
                                qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                qllEnv                * aEnv )
{
    qllExecutionNode   * sExecNode     = NULL;
    qlnoQuerySpec      * sOptQuerySpec = NULL;
    qlnxQuerySpec      * sExeQuerySpec = NULL;

    stlInt64             sSkipCnt      = 0;
    stlInt64             sFetchCnt     = 0;


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_QUERY_SPEC_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * QUERY SPEC Optimization Node 획득
     */

    sOptQuerySpec = (qlnoQuerySpec *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * QUERY SPEC Execution Node 획득
     */

    sExeQuerySpec = sExecNode->mExecNode;

    /*****************************************
     * OFFSET .. LIMIT 절 관련
     ****************************************/

    /**
     * Skip Count 설정
     */

    if( sOptQuerySpec->mResultSkip == NULL )
    {
        sSkipCnt = sOptQuerySpec->mSkipCnt;
    }
    else if( sOptQuerySpec->mResultSkip->mType == QLV_EXPR_TYPE_VALUE )
    {
        sSkipCnt = sOptQuerySpec->mSkipCnt;
    }
    else
    {
        STL_TRY( qlnxGetBindedValue4FETCH( aDBCStmt,
                                           aSQLStmt,
                                           STL_FALSE, /* aIsFetch */
                                           sOptQuerySpec->mResultSkip,
                                           & sSkipCnt,
                                           aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Fetch Count 설정
     */
    
    if( sOptQuerySpec->mResultLimit == NULL )
    {
        sFetchCnt = sOptQuerySpec->mFetchCnt;
    }
    else if( sOptQuerySpec->mResultLimit->mType == QLV_EXPR_TYPE_VALUE )
    {
        sFetchCnt = sOptQuerySpec->mFetchCnt;
    }
    else
    {
        STL_TRY( qlnxGetBindedValue4FETCH( aDBCStmt,
                                           aSQLStmt,
                                           STL_TRUE, /* aIsFetch */
                                           sOptQuerySpec->mResultLimit,
                                           & sFetchCnt,
                                           aEnv )
                 == STL_SUCCESS );
    }

    sExeQuerySpec->mRemainSkipCnt  = sSkipCnt;
    sExeQuerySpec->mRemainFetchCnt = sFetchCnt;


    /******************************************************
     * 하위 Node들 Execute 수행
     ******************************************************/

    /**
     * 하위 노드 Execute
     */

    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[sOptQuerySpec->mChildNode->mIdx].mOptNode,
                       aDataArea,
                       aEnv );

    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief QUERY SPEC node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 또는 반복되는 Execution 수행시 호출된다.
 */
stlStatus qlnxFetchQuerySpec( qlnxFetchNodeInfo     * aFetchNodeInfo,
                              stlInt64              * aUsedBlockCnt,
                              stlBool               * aEOF,
                              qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode     = NULL;
    qlnoQuerySpec         * sOptQuerySpec = NULL;
    qlnxQuerySpec         * sExeQuerySpec = NULL;

    stlInt64                sSkipCnt      = 0;
    stlInt64                sFetchCnt     = 0;

    knlExprEvalInfo         sEvalInfo;
    knlValueBlockList     * sTargetBlock  = NULL;
    knlExprStack          * sTargetStack  = NULL;
    stlInt32                sTargetIdx    = 0;

    stlInt64                sFetchedCnt   = 0;
    stlInt64                sSkippedCnt   = 0;
    qlnxFetchNodeInfo       sLocalFetchInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_QUERY_SPEC_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * QUERY SPEC Optimization Node 획득
     */

    sOptQuerySpec = (qlnoQuerySpec *) aFetchNodeInfo->mOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * QUERY SPEC Execution Node 획득
     */

    sExeQuerySpec = sExecNode->mExecNode;


    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );

    
    /*****************************************
     * Child Node Fetch 수행
     ****************************************/

    sSkipCnt  = aFetchNodeInfo->mSkipCnt + sExeQuerySpec->mRemainSkipCnt;
    if ( sExeQuerySpec->mRemainFetchCnt > aFetchNodeInfo->mFetchCnt )
    {
        sFetchCnt = aFetchNodeInfo->mFetchCnt;
    }
    else
    {
        sFetchCnt = sExeQuerySpec->mRemainFetchCnt;
    }

    if( sFetchCnt == 0 )
    {
        *aEOF = ( sExeQuerySpec->mRemainFetchCnt == 0 );
        STL_THROW( RAMP_EXIT );
    }


    /*****************************************
     * FETCH
     ****************************************/

    QLL_OPT_CHECK_TIME( sBeginExceptTime );

    /* Fetch Next */
    QLNX_FETCH_NODE( & sLocalFetchInfo,
                     sOptQuerySpec->mChildNode->mIdx,
                     0, /* Start Idx */
                     sSkipCnt,
                     sFetchCnt,
                     & sFetchedCnt,
                     aEOF,
                     aEnv );

    if( sSkipCnt > 0 )
    {
        sSkippedCnt = sSkipCnt - sLocalFetchInfo.mSkipCnt;
        if( sSkippedCnt >= sExeQuerySpec->mRemainSkipCnt )
        {
            sExeQuerySpec->mRemainSkipCnt = 0;
            sSkippedCnt -= sExeQuerySpec->mRemainSkipCnt;
        }
        else
        {
            sExeQuerySpec->mRemainSkipCnt -= sSkippedCnt;
            sSkippedCnt = 0;
        }
    }
    else
    {
        /* Do Nothing */
    }

    QLL_OPT_CHECK_TIME( sEndExceptTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

    STL_TRY_THROW( sFetchedCnt > 0, RAMP_EXIT );

    /* Remain Fetch Count 수정 */
    sExeQuerySpec->mRemainFetchCnt -= sFetchedCnt;
    

    /*****************************************
     * Pseudo Column 수행 ( RESULT )
     ****************************************/

    /**
     * RESULT Pseudo Column 수행
     */

    if( ( sOptQuerySpec->mQueryExprList->mInitExprList->mPseudoColExprList->mCount > 0 ) &&
        ( sFetchedCnt > 0 ) )
    {
        STL_TRY( qlxExecutePseudoColumnBlock(
                     sLocalFetchInfo.mTransID,
                     sLocalFetchInfo.mStmt,
                     DTL_ITERATION_TIME_FOR_EACH_EXPR,
                     NULL,
                     NULL,
                     sOptQuerySpec->mQueryExprList->mInitExprList->mPseudoColExprList,
                     sExeQuerySpec->mPseudoColBlock,
                     0,
                     sFetchedCnt,
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* nothing to do */
    }
    
    
    /*****************************************
     * Target Expression 수행
     ****************************************/

    sEvalInfo.mContext    = sLocalFetchInfo.mDataArea->mExprDataContext;
    sEvalInfo.mBlockIdx   = 0;
    sEvalInfo.mBlockCount = sFetchedCnt;
        
    for( sTargetIdx = 0, sTargetBlock = sExeQuerySpec->mTargetColBlock;
         sTargetIdx < sOptQuerySpec->mTargetCount;
         sTargetIdx++, sTargetBlock = sTargetBlock->mNext )
    {
        sTargetStack = & sOptQuerySpec->mTargetsCodeStack[ sTargetIdx ];
             
        if( ( sTargetStack->mEntries[1].mExprType == KNL_EXPR_TYPE_VALUE ) &&
            ( sTargetStack->mEntryCount == 1 ) &&
            ( sLocalFetchInfo.mDataArea->mExprDataContext->mContexts
              [ sTargetStack->mEntries[1].mExpr.mValueInfo.mOffset ].mCast == NULL ) )
        {
            /* Do Nothing */
        }
        else
        {
            sEvalInfo.mExprStack   = sTargetStack;
            sEvalInfo.mResultBlock = sTargetBlock;

            STL_TRY( knlEvaluateBlockExpression( & sEvalInfo,
                                                 KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }

    
    STL_RAMP( RAMP_EXIT );


    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;

    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeQuerySpec->mTargetColBlock,
                                         0,
                                         sFetchedCnt );                                         

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeQuerySpec->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeQuerySpec->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeQuerySpec->mCommonInfo.mFetchRecordStat,
                                  sExeQuerySpec->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeQuerySpec->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;
    
    return STL_SUCCESS;

    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeQuerySpec->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeQuerySpec->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief QUERY SPEC node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeQuerySpec( qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 qllEnv                * aEnv )
{
    /* qllExecutionNode  * sExecNode     = NULL; */
    /* qlnoQuerySpec     * sOptQuerySpec = NULL; */
    /* qlnxQuerySpec     * sExeQuerySpec = NULL; */

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_QUERY_SPEC_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * QUERY SPEC Optimization Node 획득
     */

    /* sOptQuerySpec = (qlnoQuerySpec *) aOptNode->mOptNode; */

    
    /**
     * Common Execution Node 획득
     */
        
    /* sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) ); */


    /**
     * QUERY SPEC Execution Node 획득
     */

    /* sExeQuerySpec = sExecNode->mExecNode; */


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief OFFSET .. FETCH .. 의 Binded Value 값을 얻는다
 * @param[in]  aDBCStmt    DBC statement
 * @param[in]  aSQLStmt    SQL statement
 * @param[in]  aIsFetch    FETCH or OFFSET
 * @param[in]  aFetchExpr  Init Expression
 * @param[out] aBindValue  Binded Value
 * @param[in]  aEnv        Environment
 */
stlStatus qlnxGetBindedValue4FETCH( qllDBCStmt        * aDBCStmt,
                                    qllStatement      * aSQLStmt,
                                    stlBool             aIsFetch,   
                                    qlvInitExpression * aFetchExpr,
                                    stlInt64          * aBindValue,
                                    qllEnv            * aEnv )
{
    qlvInitBindParam   * sBindParam    = NULL;
    knlValueBlockList  * sValueBlock   = NULL;
    dtlValueEntry        sInputArgument[ DTL_CAST_INPUT_ARG_CNT ];
    dtlDataValue         sArgDataValue[ DTL_CAST_INPUT_ARG_CNT - 1 ];
    dtlDataValue         sResDataValue;

    dtlBigIntType        sArg1;
    dtlBigIntType        sArg2;
    dtlIntegerType       sArg3;
    dtlIntegerType       sArg4;

    dtlBigIntType        sResult = 0;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFetchExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBindValue != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFetchExpr->mType == QLV_EXPR_TYPE_BIND_PARAM, QLL_ERROR_STACK(aEnv) );

    /**
     * FETCH (host var) 의 Value Block 획득
     */
    
    sBindParam  = aFetchExpr->mExpr.mBindParam;
    sValueBlock = aSQLStmt->mBindContext->mMap[sBindParam->mBindParamIdx]->mINValueBlock;

    /**
     * Casting 가능 여부 검사 
     */
    
    STL_TRY_THROW( dtlIsApplicableCast( KNL_GET_BLOCK_DB_TYPE(sValueBlock),
                                        KNL_GET_BLOCK_INTERVAL_INDICATOR(sValueBlock),
                                        DTL_TYPE_NATIVE_BIGINT,
                                        DTL_INTERVAL_INDICATOR_NA )
                   == STL_TRUE,
                   RAMP_ERR_INVALID_OFFSET_FETCH );
    
    /**
     * Casting 수행
     */
    
    /* data value */
    sInputArgument[ 0 ].mValue.mDataValue = KNL_GET_BLOCK_DATA_VALUE( sValueBlock, 0 );
    STL_TRY_THROW( DTL_IS_NULL( sInputArgument[ 0 ].mValue.mDataValue ) == STL_FALSE,
                   RAMP_ERR_INVALID_OFFSET_FETCH );
    
    /* precision */
    sArg1 = DTL_PRECISION_NA;

    sArgDataValue[ 0 ].mType       = DTL_TYPE_NATIVE_BIGINT;
    sArgDataValue[ 0 ].mBufferSize = DTL_NATIVE_BIGINT_SIZE;
    sArgDataValue[ 0 ].mLength     = DTL_NATIVE_BIGINT_SIZE;
    sArgDataValue[ 0 ].mValue      = & sArg1;

    sInputArgument[ 1 ].mValue.mDataValue = & sArgDataValue[ 0 ];

    /* scale */
    sArg2 = DTL_SCALE_NA;

    sArgDataValue[ 1 ].mType       = DTL_TYPE_NATIVE_BIGINT;
    sArgDataValue[ 1 ].mBufferSize = DTL_NATIVE_BIGINT_SIZE;
    sArgDataValue[ 1 ].mLength     = DTL_NATIVE_BIGINT_SIZE;
    sArgDataValue[ 1 ].mValue      = & sArg2;

    sInputArgument[ 2 ].mValue.mDataValue = & sArgDataValue[ 1 ];
    
    /* string length unit */
    sArg3 = DTL_STRING_LENGTH_UNIT_NA;

    sArgDataValue[ 2 ].mType       = DTL_TYPE_NATIVE_INTEGER;
    sArgDataValue[ 2 ].mBufferSize = DTL_NATIVE_INTEGER_SIZE;
    sArgDataValue[ 2 ].mLength     = DTL_NATIVE_INTEGER_SIZE;
    sArgDataValue[ 2 ].mValue      = & sArg3;

    sInputArgument[ 3 ].mValue.mDataValue = & sArgDataValue[ 2 ];
        
    /* interval indicator */
    sArg4 = DTL_INTERVAL_INDICATOR_NA;

    sArgDataValue[ 3 ].mType       = DTL_TYPE_NATIVE_INTEGER;
    sArgDataValue[ 3 ].mBufferSize = DTL_NATIVE_INTEGER_SIZE;
    sArgDataValue[ 3 ].mLength     = DTL_NATIVE_INTEGER_SIZE;
    sArgDataValue[ 3 ].mValue      = & sArg4;

    sInputArgument[ 4 ].mValue.mDataValue = & sArgDataValue[ 3 ];

    /* CAST 수행 */
    sResDataValue.mType   = DTL_TYPE_NATIVE_BIGINT;
    sResDataValue.mValue  = (void *) & sResult;
    sResDataValue.mLength = 0;
        
    STL_TRY_THROW(
        dtlCastFunctionList[ KNL_GET_BLOCK_DB_TYPE(sValueBlock) ][ DTL_TYPE_NATIVE_BIGINT ](
            DTL_CAST_INPUT_ARG_CNT,
            sInputArgument,
            & sResDataValue,
            NULL,
            NULL,
            NULL,
            QLL_ERROR_STACK( aEnv ) )
        == STL_SUCCESS,
        RAMP_ERR_INVALID_OFFSET_FETCH );
    
    if ( aIsFetch == STL_TRUE )
    {
        /**
         * error : FETCH 0
         */
        STL_TRY_THROW( sResult > 0, RAMP_ERR_INVALID_OFFSET_FETCH );
    }
    else
    {
        /**
         * error : OFFSET -1
         */
        STL_TRY_THROW( sResult >= 0, RAMP_ERR_INVALID_OFFSET_FETCH );
    }

    /**
     * Output 설정
     */

    *aBindValue = sResult;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_OFFSET_FETCH )
    {
        if ( aIsFetch == STL_TRUE )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_RESULT_LIMIT_MUST_BE_POSITIVE,
                          qlgMakeErrPosString( aSQLStmt->mRetrySQL,
                                               aFetchExpr->mPosition,
                                               aEnv ),
                          QLL_ERROR_STACK(aEnv) );
            
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_RESULT_OFFSET_MUST_BE_ZERO_OR_POSITIVE,
                          qlgMakeErrPosString( aSQLStmt->mRetrySQL,
                                               aFetchExpr->mPosition,
                                               aEnv ),
                          QLL_ERROR_STACK(aEnv) );
            
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnx */
