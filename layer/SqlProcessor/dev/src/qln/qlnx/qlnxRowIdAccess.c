/*******************************************************************************
 * qlnxRowIdAccess.c
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
 * @file qlnxRowIdAccess.c
 * @brief SQL Processor Layer Execution Node - ROWID ACCESS
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlnx
 * @{
 */


/**
 * @brief ROWID ACCESS node를 초기화한다.
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
stlStatus qlnxInitializeRowIdAccess( smlTransId              aTransID,
                                     smlStatement          * aStmt,
                                     qllDBCStmt            * aDBCStmt,
                                     qllStatement          * aSQLStmt,
                                     qllOptimizationNode   * aOptNode,
                                     qllDataArea           * aDataArea,
                                     qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode       = NULL;
    /* qlnoRowIdAccess   * sOptRowIdAccess = NULL; */
    qlnxRowIdAccess   * sExeRowIdAccess = NULL;
    
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_ROWID_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * ROWID ACCESS Optimization Node 획득
     */

    /* sOptRowIdAccess = (qlnoRowIdAccess *) aOptNode->mOptNode; */


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

    
    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* Query Node는 없을 수도 있다. */


    /**
     * ROWID ACCESS Execution Node 획득
     */

    sExeRowIdAccess = (qlnxRowIdAccess *) sExecNode->mExecNode;


    /**
     * ROWID ACCESS Execution Node 정보 초기화
     */
    
    sExeRowIdAccess->mIsFirstExecute = STL_TRUE;
    sExeRowIdAccess->mHasFalseFilter = STL_FALSE;
    

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ROWID ACCESS node를 수행한다.
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
stlStatus qlnxExecuteRowIdAccess( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode       = NULL;
    qlnoRowIdAccess       * sOptRowIdAccess = NULL;
    qlnxRowIdAccess       * sExeRowIdAccess = NULL;

    knlPhysicalFilter     * sPhysicalFilter = NULL;

    knlExprContext        * sExprContext    = NULL;
    smlFetchInfo          * sFetchInfo      = NULL;


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_ROWID_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * ROWID ACCESS Optimization Node 획득
     */

    sOptRowIdAccess = (qlnoRowIdAccess *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * ROWID ACCESS Execution Node 획득
     */

    sExeRowIdAccess = sExecNode->mExecNode;
    
    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */
    
    /* Query Node는 없을 수도 있다. */

    
    /*****************************************
     * Node 수행 준비
     ****************************************/
    
    /**
     * 초기화
     */

    sFetchInfo = & sExeRowIdAccess->mFetchInfo;

    sFetchInfo->mIsEndOfScan = STL_FALSE;
    sFetchInfo->mSkipCnt     = 0;
    sFetchInfo->mFetchCnt    = QLL_FETCH_COUNT_MAX;

    
    /**
     * Rowid Column Value 검사
     */

    sExprContext =
        & aDataArea->mExprDataContext->mContexts[ sExeRowIdAccess->mRowIdScanValueExpr->mOffset ];   
   
    if( DTL_IS_NULL( sExprContext->mInfo.mValueInfo->mDataValue ) )
    {
        sExeRowIdAccess->mHasFalseFilter = STL_TRUE;
        STL_THROW( RAMP_FINISH );
    }


    /**
     * Prepare Filter 수행
     */
    
    if( sExeRowIdAccess->mIsFirstExecute == STL_FALSE )
    {
        if( sExeRowIdAccess->mPreFilterEvalInfo != NULL )
        {
            STL_TRY( knlEvaluateOneExpression( sExeRowIdAccess->mPreFilterEvalInfo,
                                               KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * RowIdScanValueExpr 정보로 SM 에 맞는 RowId Access 정보 구성
     * 예) ROWID = 'AAAFKAAAAAAAACAAAEAkAAC'
     *             -------------------------
     *             sExeRowIdAccess->mRowIdScanValueExpr
     *
     *     . smlRid 정보              
     *          smlRid.mTbsId  = 2
     *          smlRid.mPageId = 16420
     *          smlRid.mOffset = 2
     *
     *     . TableID = 223
     */
    
    /* smlRid 정보 구성  */
    DTL_GET_RID_INFO( sExprContext->mInfo.mValueInfo->mDataValue->mValue,
                      sExeRowIdAccess->mRidForRowIdScan->mTbsId,
                      sExeRowIdAccess->mRidForRowIdScan->mPageId,
                      sExeRowIdAccess->mRidForRowIdScan->mOffset );
        
    /* Table ID 정보 Validate */
    DTL_GET_TABLE_ID( sExprContext->mInfo.mValueInfo->mDataValue->mValue,
                      sExeRowIdAccess->mTableLogicalIDForRowIdScan );
    
    
    /**
     * Physical Filter의 constant value 정보 갱신
     */

    sPhysicalFilter = sFetchInfo->mFetchRecordInfo->mPhysicalFilter;
    while( sPhysicalFilter != NULL )
    {
        if( sPhysicalFilter->mConstVal != NULL )
        {
            if( sPhysicalFilter->mColIdx2 == KNL_PHYSICAL_LIST_VALUE_CONSTANT )
            {
                /* Do Nothing */
            }
            else
            {
                sPhysicalFilter->mColVal2->mValue  = sPhysicalFilter->mConstVal->mValue;
                sPhysicalFilter->mColVal2->mLength = sPhysicalFilter->mConstVal->mLength;
            }
        }

        sPhysicalFilter = sPhysicalFilter->mNext;
    }
        

    sExeRowIdAccess->mHasFalseFilter =
        qlfHasFalseFilter( sOptRowIdAccess->mLogicalFilterExpr,
                           aDataArea );


    STL_RAMP( RAMP_FINISH );
    
    sExeRowIdAccess->mIsFirstExecute = STL_FALSE;
        
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief ROWID ACCESS node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchRowIdAccess( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                stlInt64              * aUsedBlockCnt,
                                stlBool               * aEOF,
                                qllEnv                * aEnv )
{
    qllExecutionNode    * sExecNode        = NULL;
    qlnxRowIdAccess     * sExeRowIdAccess  = NULL;
    qlnoRowIdAccess     * sOptRowIdAccess  = NULL;

    stlBool               sDeleted         = STL_FALSE;
    stlBool               sIsMatched       = STL_FALSE;
    smlFetchRecordInfo  * sFetchRecordInfo = NULL;

    qlnfAggrOptInfo     * sAggrOptInfo     = NULL;
    qlnfAggrExecInfo    * sAggrExecInfo    = NULL;
    stlInt32              sAggrFuncCnt     = 0;
    stlInt32              sLoop            = 0;
    stlInt64              sFetchedCnt      = 0;
    stlInt64              sSkippedCnt      = 0;
    qlnxFetchNodeInfo     sLocalFetchInfo;
    
    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_ROWID_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * ROWID ACCESS Optimization Node 획득
     */

    sOptRowIdAccess = (qlnoRowIdAccess *) aFetchNodeInfo->mOptNode->mOptNode;
    
    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * ROWID ACCESS Execution Node 획득
     */

    sExeRowIdAccess = sExecNode->mExecNode;

    
    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */
    
    /* Query Node는 없을 수도 있다. */


    /**
     * Used Block Count 초기화
     */
    
    *aUsedBlockCnt = 0;
    
    
    /**
     * Node 수행 여부 확인
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( sExeRowIdAccess->mFetchInfo.mIsEndOfScan == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node 수행 여부 확인
     ****************************************/

    if( aFetchNodeInfo->mSkipCnt > 0 )
    {
        sFetchedCnt = 0;
        sSkippedCnt = 1;
    }
    else
    {
        sFetchedCnt = 1;
        sSkippedCnt = 0;
    }

    if( sExeRowIdAccess->mHasFalseFilter == STL_TRUE )
    {
        if( sOptRowIdAccess->mAggrFuncCnt > 0 )
        {
            sAggrExecInfo = sExeRowIdAccess->mAggrExecInfo;
            for( sLoop = 0 ; sLoop < sOptRowIdAccess->mAggrFuncCnt ; sLoop++ )
            {
                STL_TRY( gQlnfAggrFuncInfoArr[sAggrExecInfo->mAggrOptInfo->mAggrFuncId].mInitFuncPtr(
                             sAggrExecInfo,
                             0, /* result block idx */
                             aEnv )
                         == STL_SUCCESS );
                KNL_SET_ONE_BLOCK_USED_CNT( sAggrExecInfo->mAggrValueList, sFetchedCnt );
                sAggrExecInfo++;
            }
        }
        else
        {
            sFetchedCnt = 0;
        }
        
        sExeRowIdAccess->mFetchInfo.mIsEndOfScan = STL_TRUE;
        STL_THROW( RAMP_EXIT );
    }
    else
    {
        /* Do Nothing */
    }


    /*****************************************
     * Fetch 수행
     ****************************************/

    sFetchRecordInfo = sExeRowIdAccess->mFetchInfo.mFetchRecordInfo;    
    
    /**
     * RowId에 해당하는 레코드 패치
     *
     * 예) ROWID = 'AAAFKAAAAAAAACAAAEAkAAC' 인 경우,
     *             -------------------------
     *
     *     ROWID = 'AAAFKAAAAAAAACAAAEAkAAC'를 SM 에 맞는 정보로 변환한
     *     아래의 정보로 레코드 패치.
     *
     *     . smlRid 정보
     *          smlRid.mTbsId  = 2
     *          smlRid.mPageId = 16420
     *          smlRid.mOffset = 2
     *
     *     . TableID = 223
     */

    /* Table ID 정보 Validate  */
    
    STL_TRY_THROW( sFetchRecordInfo->mTableLogicalId
                   == sExeRowIdAccess->mTableLogicalIDForRowIdScan,
                   ERR_INVALID_ROWID );
    
    STL_TRY( smlFetchRecordWithRowid(
                 aFetchNodeInfo->mStmt,
                 sFetchRecordInfo,
                 sExeRowIdAccess->mRidForRowIdScan, // aRowRid
                 0, // aBlockIdx
                 & sIsMatched, // aIsMatched
                 & sDeleted, // aDeleted
                 SML_ENV(aEnv) ) // aEnv
             == STL_SUCCESS );
        
    /**
     * Data 존재 여부 설정
     */
        
    if( ( sDeleted == STL_FALSE ) && ( sIsMatched == STL_TRUE ) )
    {
        /* 주어진 ROWID에 해당하는 레코드를 가져옴. */
        SML_SET_USED_BLOCK_SIZE( sExeRowIdAccess->mFetchInfo.mRowBlock, 1 );
        KNL_SET_ALL_BLOCK_USED_CNT( sFetchRecordInfo->mColList, 1 );
        KNL_SET_ONE_BLOCK_USED_CNT( sFetchRecordInfo->mRowIdColList, 1 );
    }
    else
    {
        /* 주어진 ROWID에 해당하는 레코드가 없음.  */
        SML_SET_USED_BLOCK_SIZE( sExeRowIdAccess->mFetchInfo.mRowBlock, 0 );
        KNL_SET_ALL_BLOCK_USED_CNT( sFetchRecordInfo->mColList, 0 );
        KNL_SET_ONE_BLOCK_USED_CNT( sFetchRecordInfo->mRowIdColList, 0 );
    }


    /*****************************************
     * Aggregation Build 수행
     ****************************************/

    sAggrFuncCnt = sOptRowIdAccess->mAggrFuncCnt;
    
    if( sAggrFuncCnt > 0 )
    {
        sAggrOptInfo  = sOptRowIdAccess->mAggrOptInfo;
        sAggrExecInfo = sExeRowIdAccess->mAggrExecInfo;

        if( SML_USED_BLOCK_SIZE( sExeRowIdAccess->mFetchInfo.mRowBlock ) == 1 )
        {
            for( sLoop = 0 ; sLoop < sAggrFuncCnt ; sLoop++ )
            {
                /**
                 * Init Aggregation
                 */

                STL_TRY( gQlnfAggrFuncInfoArr[sAggrExecInfo->mAggrOptInfo->mAggrFuncId].mInitFuncPtr(
                             sAggrExecInfo,
                             0, /* result block idx */
                             aEnv )
                         == STL_SUCCESS );

                
                /**
                 * Build Aggregation
                 */
                
                STL_TRY( gQlnfAggrFuncInfoArr[sAggrOptInfo->mAggrFuncId].mBuildFuncPtr(
                             sAggrExecInfo,
                             0, /* start block idx */
                             1,
                             0, /* result block idx */
                             aEnv )
                         == STL_SUCCESS );
                
                KNL_SET_ONE_BLOCK_USED_CNT( sAggrExecInfo->mAggrValueList, sFetchedCnt );
                
                sAggrOptInfo++;
                sAggrExecInfo++;
            }
        }
        else
        {
            STL_DASSERT( SML_USED_BLOCK_SIZE( sExeRowIdAccess->mFetchInfo.mRowBlock ) == 0 );
            
            /**
             * Init Aggregation
             */

            for( sLoop = 0 ; sLoop < sAggrFuncCnt ; sLoop++ )
            {
                STL_TRY( gQlnfAggrFuncInfoArr[sAggrExecInfo->mAggrOptInfo->mAggrFuncId].mInitFuncPtr(
                             sAggrExecInfo,
                             0, /* result block idx */
                             aEnv )
                         == STL_SUCCESS );
        
                KNL_SET_ONE_BLOCK_USED_CNT( sAggrExecInfo->mAggrValueList, sFetchedCnt );

                sAggrOptInfo++;
                sAggrExecInfo++;
            }
        }
    }
    else
    {
        if( SML_USED_BLOCK_SIZE( sExeRowIdAccess->mFetchInfo.mRowBlock ) == 0 )
        {
            sFetchedCnt = 0;
        }
        else
        {
            /* Do Nothing */
        }

        SML_SET_USED_BLOCK_SIZE( sExeRowIdAccess->mFetchInfo.mRowBlock, sFetchedCnt );
        KNL_SET_ALL_BLOCK_USED_CNT( sFetchRecordInfo->mColList, sFetchedCnt );
        KNL_SET_ONE_BLOCK_USED_CNT( sFetchRecordInfo->mRowIdColList, sFetchedCnt );
    }

    STL_RAMP( RAMP_EXIT );

    
    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;

    *aEOF = STL_TRUE;
    
 
    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExeRowIdAccess->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeRowIdAccess->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeRowIdAccess->mCommonInfo.mFetchRecordStat,
                                  sExeRowIdAccess->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeRowIdAccess->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;

    return STL_SUCCESS;

    STL_CATCH( ERR_INVALID_ROWID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_ROWID,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExeRowIdAccess->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeRowIdAccess->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief ROWID ACCESS node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeRowIdAccess( qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv )
{
    /* qllExecutionNode      * sExecNode       = NULL; */
    /* qlnoRowIdAccess       * sOptRowIdAccess = NULL; */
    /* qlnxRowIdAccess       * sExeRowIdAccess = NULL; */
    

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_ROWID_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Common Execution Node 획득
     */
        
    /* sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) ); */


    /**
     * ROWID ACCESS Optimization Node 획득
     */

    /* sOptRowIdAccess = (qlnoRowIdAccess * ) aOptNode->mOptNode; */
        

    /**
     * ROWID ACCESS Execution Node 획득
     */

    /* sExeRowIdAccess = sExecNode->mExecNode; */
    

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnx */
