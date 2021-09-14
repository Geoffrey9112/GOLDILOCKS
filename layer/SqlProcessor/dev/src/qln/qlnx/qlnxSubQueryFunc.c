/*******************************************************************************
 * qlnxSubQueryFunc.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlnxSubQueryFunc.c 8502 2013-05-20 02:42:55Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlnxSubQueryFunc.c
 * @brief SQL Processor Layer Execution Node - SUB QUERY FUNCTION
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */

/**
 * @brief SUB QUERY FUNCTION node를 초기화한다.
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
stlStatus qlnxInitializeSubQueryFunc( smlTransId              aTransID,
                                      smlStatement          * aStmt,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode        = NULL;
//    qlnoSubQueryFunc  * sOptSubQueryFunc = NULL;
    qlnxSubQueryFunc  * sExeSubQueryFunc = NULL;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * SUB-QUERY FUNCTION Optimization Node 획득
     */

//    sOptSubQueryFunc = (qlnoSubQueryFunc *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SUB-QUERY FUNCTION Execution Node 할당
     */

    sExeSubQueryFunc = (qlnxSubQueryFunc *) sExecNode->mExecNode;


    /**
     * SORT INSTANT TABLE 생성
     */

    if( ( sExeSubQueryFunc->mNeedMaterialize == STL_TRUE ) &&
        ( sExeSubQueryFunc->mIsOrdering == STL_FALSE ) )
    {
        STL_DASSERT( sExeSubQueryFunc->mListFuncInstantFetchInfo != NULL );
        
        STL_TRY( qlnxCreateSortInstantTable(
                     aTransID,
                     aStmt,
                     sExeSubQueryFunc->mListFuncInstantFetchInfo->mInstantTable,
                     sExeSubQueryFunc->mListFuncInstantFetchInfo->mInstantTable->mTableHandle,
                     sExeSubQueryFunc->mValueBlockCnt,
                     sExeSubQueryFunc->mRightBlockList,
                     sExeSubQueryFunc->mListFuncInstantFetchInfo->mSortKeyFlags,
                     STL_TRUE,   /* top-down */
                     STL_FALSE,  /* volatile */
                     STL_FALSE,  /* leaf only */
                     STL_TRUE,   /* unique */
                     STL_FALSE,  /* null exlcuded */
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }

    sExeSubQueryFunc->mIsFirstExec = STL_TRUE;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SUB QUERY FUNCTION node를 수행한다.
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
stlStatus qlnxExecuteSubQueryFunc( smlTransId              aTransID,
                                   smlStatement          * aStmt,
                                   qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode        = NULL;
    qlnoSubQueryFunc  * sOptSubQueryFunc = NULL;
    qlnxSubQueryFunc  * sExeSubQueryFunc = NULL;
    qlnInstantTable   * sInstantTable    = NULL;
    

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * SUB-QUERY FUNCTION Optimization Node 획득
     */

    sOptSubQueryFunc = (qlnoSubQueryFunc *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SUB-QUERY FUNCTION Execution Node 획득
     */

    sExeSubQueryFunc = sExecNode->mExecNode;


    /**
     * Statement 단위 Sub Query Function 평가
     */

    STL_DASSERT( sOptSubQueryFunc->mFuncExpr != NULL );
    
    if( sOptSubQueryFunc->mFuncExpr->mIterationTime >=
        DTL_ITERATION_TIME_FOR_EACH_STATEMENT )
    {
        if( sExeSubQueryFunc->mIsFirstExec == STL_TRUE )
        {
            /**
             * Function 평가
             */
    
            if( sOptSubQueryFunc->mChildNode != NULL )
            {
                QLNX_EXECUTE_NODE( aTransID,
                                   aStmt,
                                   aDBCStmt,
                                   aSQLStmt,
                                   aDataArea->mExecNodeList[ sOptSubQueryFunc->mChildNode->mIdx ].mOptNode,
                                   aDataArea,
                                   aEnv );
            }

            if( sOptSubQueryFunc->mRightOperandNode != NULL )
            {
                QLNX_EXECUTE_NODE( aTransID,
                                   aStmt,
                                   aDBCStmt,
                                   aSQLStmt,
                                   aDataArea->mExecNodeList[ sOptSubQueryFunc->mRightOperandNode->mIdx ].mOptNode,
                                   aDataArea,
                                   aEnv );
            }

            STL_TRY( sExeSubQueryFunc->mSubQueryFuncPtr( aTransID,
                                                         aStmt,
                                                         aDBCStmt,
                                                         aSQLStmt,
                                                         aOptNode,
                                                         aDataArea,
                                                         0,
                                                         aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        if( sExeSubQueryFunc->mNeedMaterialize == STL_TRUE )
        {
            if( sExeSubQueryFunc->mIsOrdering == STL_FALSE )
            {
                sInstantTable = sExeSubQueryFunc->mListFuncInstantFetchInfo->mInstantTable;
    
                sInstantTable->mStmt   = aStmt;
                sInstantTable->mCurIdx = 0;

                if( sExeSubQueryFunc->mIsFirstExec == STL_TRUE )
                {
                    sInstantTable->mIsNeedBuild = STL_TRUE;
                    sExeSubQueryFunc->mHasNull  = STL_FALSE;
                }
                else
                {
                    /* Do Nothing */
                }
            }
            else
            {
                if( sExeSubQueryFunc->mIsFirstExec == STL_TRUE )
                {
                    sExeSubQueryFunc->mListFuncArrayFetchInfo->mIsNeedBuild = STL_TRUE;

                    stlMemset( sExeSubQueryFunc->mListFuncArrayFetchInfo->mIsUsedRowCol,
                               0x00,
                               STL_SIZEOF( stlBool ) * sExeSubQueryFunc->mListFuncArrayFetchInfo->mRowColRecordCnt );

                }
                else
                {
                    /* Do Nothing */
                }
            }
        }
        else
        {
            /* Do Nothing */
        }

        if( sOptSubQueryFunc->mRightOperandNode != NULL )
        {
            QLNX_EXECUTE_NODE( aTransID,
                               aStmt,
                               aDBCStmt,
                               aSQLStmt,
                               aDataArea->mExecNodeList[ sOptSubQueryFunc->mRightOperandNode->mIdx ].mOptNode,
                               aDataArea,
                               aEnv );
        }
    }
        
    sExeSubQueryFunc->mIsFirstExec = STL_FALSE;
        

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SUB QUERY FUNCTION node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 또는 반복되는 Execution 수행시 호출된다.
 */
stlStatus qlnxFetchSubQueryFunc( qlnxFetchNodeInfo     * aFetchNodeInfo,
                                 stlInt64              * aUsedBlockCnt,
                                 stlBool               * aEOF,
                                 qllEnv                * aEnv )
{
    qllExecutionNode   * sExecNode        = NULL;
    qlnoSubQueryFunc   * sOptSubQueryFunc = NULL;
    qlnxSubQueryFunc   * sExeSubQueryFunc = NULL;
    qlnInstantTable    * sInstantTable    = NULL;
    
    knlCastFuncInfo    * sRightCastInfo   = NULL;
    knlValueBlockList  * sValueBlockList  = NULL;
    knlValueBlockList  * sRightValueBlock = NULL;
    dtlDataValue       * sDataValue       = NULL;
    dtlDataValue       * sRowDataValue    = NULL;
    dtlDataValue       * sRecDataValue    = NULL;
    
    stlInt32             sLoop            = 0;
    stlInt32             sCurIdx          = 0;
    stlInt32             sStartIdx        = 0;
    stlInt32             sColIdx          = 0;
    stlInt64             sReadCnt         = 0;
    stlBool              sEOF             = STL_FALSE;
    stlBool              sIsNewRowCol     = STL_FALSE;
    
    knlExprEvalInfo      sBlockExprEvalInfo;

    qlnxListFuncArrayFetchInfo  * sArrayFetchInfo  = NULL;
    dtlCompareResult              sCompResult      = DTL_COMPARISON_EQUAL;
        
    stlInt64             sFetchedCnt      = 0;
    qlnxFetchNodeInfo    sLocalFetchInfo;
    
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

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * SUB-QUERY FUNCTION Optimization Node 획득
     */

    sOptSubQueryFunc = (qlnoSubQueryFunc *) aFetchNodeInfo->mOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * SUB-QUERY FUNCTION Execution Node 획득
     */

    sExeSubQueryFunc = sExecNode->mExecNode;


    /**
     * Node 수행 여부 확인
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);
    STL_DASSERT( aFetchNodeInfo->mSkipCnt == 0);

    sStartIdx = aFetchNodeInfo->mStartIdx;
        

    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * Node Fetch 수행
     ****************************************/

    sFetchedCnt = aFetchNodeInfo->mFetchCnt;
    
    STL_TRY_THROW( sFetchedCnt > 0, RAMP_FINISH );

    STL_TRY_THROW( ( sOptSubQueryFunc->mFuncExpr->mIterationTime <
                     DTL_ITERATION_TIME_FOR_EACH_STATEMENT ),
                   RAMP_FINISH );


    /**
     * Materialize 수행
     */

    STL_TRY_THROW( sExeSubQueryFunc->mNeedMaterialize == STL_TRUE,
                   RAMP_SKIP_MATERIALIZE );

    /**
     * Build Array
     */
    
    if( sExeSubQueryFunc->mIsOrdering == STL_TRUE )
    {
        /**
         * 전체 대표값과 현재값 비교하여 새로운 대표값 설정 (MIN or MAX)
         *
         * 예 1) 
         * 전체 대표값(MIN) : ( 1, 2, 3 )
         * 현재값            : ( 1, 1, 5 )
         * =>
         * 1. 새로운 전체 대표값(MIN) : ( 1, 1, 5 )
         * 
         * 2. 다음 값 비교로 넘어감.
         *
         * 
         * 예 2) 
         * 대표값(MIN) : ( 1, 2, 3 )
         * 현재값      : ( 2, null, 1 )
         * =>
         * 1. 전체 대표값 변동없음.
         *    전체 대표값(MIN) : ( 1, 2, 3 )
         *    
         * 2. 현재값 중에 null 값인 column 뒤에 오는 모든 column은 null로 설정.
         *    현재값 : ( 2, null, null )
         *    
         * 3. null 값이 처음 오는 column idx 를 기준으로 하는 그룹 대표값과의 비교하여 새로운 그룹 대표값 설정.
         *   만약 그룹 대표값이 없으면 현재값을 그룹 대표값으로 설정한다.
         *   
         *   그룹 대표값(MIN) : ( 3, null, null )
         *   현재값 : ( 2, null, null )
         *   
         *   새로운 그룹 대표값(MIN) : ( 2, null, null )
         */


        /**
         * Right Node Fetch하여 Sort Instant Table에 Insert
         */

        sArrayFetchInfo = sExeSubQueryFunc->mListFuncArrayFetchInfo;

        if( sArrayFetchInfo->mIsNeedBuild == STL_FALSE )
        {
            STL_THROW( RAMP_SKIP_MATERIALIZE );
        }
        
        while( sEOF == STL_FALSE )
        {
            /**
             * Block Read
             */

            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            /* fetch records*/
            sReadCnt = 0;
            QLNX_FETCH_NODE( & sLocalFetchInfo,
                             sOptSubQueryFunc->mRightOperandNode->mIdx,
                             0, /* Start Idx */
                             0, /* Skip Count */
                             QLL_FETCH_COUNT_MAX, /* Fetch Count */
                             & sReadCnt,
                             & sEOF,
                             aEnv );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sReadCnt > 0 )
            {
                for( sLoop = 0 ; sLoop < sReadCnt ; sLoop++ )
                {
                    /**
                     * Cast
                     */

                    sValueBlockList  = sExeSubQueryFunc->mBlockListForCast;
                    sRightValueBlock = sExeSubQueryFunc->mRightBlockList;
                    sColIdx          = 0;
                    
                    while( sValueBlockList != NULL )
                    {
                        sRightCastInfo = sExeSubQueryFunc->mRightCastInfo[ sColIdx ];
                        
                        if( DTL_IS_NULL( KNL_GET_BLOCK_DATA_VALUE( sValueBlockList, sLoop ) ) )
                        {
                            DTL_SET_NULL( KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sLoop ) );

                            sRightValueBlock = sRightValueBlock->mNext;
                            while( sRightValueBlock != NULL )
                            {
                                DTL_SET_NULL( KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sLoop ) );
                                sRightValueBlock = sRightValueBlock->mNext;
                            }
                            break;
                        }
                        else
                        {
                            /* Evaluate Right Operand Expresson's Internal Cast */
                            if( sRightCastInfo == NULL )
                            {
                                /* copy */
                                STL_TRY( knlCopyDataValue(
                                             KNL_GET_BLOCK_DATA_VALUE( sValueBlockList, sLoop ),
                                             KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sLoop ),
                                             KNL_ENV( aEnv ) )
                                         == STL_SUCCESS );
                            }
                            else
                            {
                                /* cast */
                                sRightCastInfo->mArgs[0].mValue.mDataValue =
                                    KNL_GET_BLOCK_DATA_VALUE( sValueBlockList, sLoop );
                        
                                STL_TRY( sRightCastInfo->mCastFuncPtr(
                                             DTL_CAST_INPUT_ARG_CNT,
                                             sRightCastInfo->mArgs,
                                             KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sLoop ),
                                             (void *)(sRightCastInfo->mSrcTypeInfo),
                                             KNL_DT_VECTOR(aEnv),
                                             aEnv,
                                             KNL_ERROR_STACK( aEnv ) )
                                         == STL_SUCCESS );
                            }
                        }

                        sColIdx++;
                        
                        sValueBlockList  = sValueBlockList->mNext;
                        sRightValueBlock = sRightValueBlock->mNext;
                    }

                    
                    /**
                     * Row Column 을 위한 Array 구성 (전체 대표값 설정)
                     */

                    sIsNewRowCol = STL_FALSE;
                    
                    if( sArrayFetchInfo->mIsUsedRowCol[ 0 ] == STL_FALSE )
                    {
                        sIsNewRowCol = STL_TRUE;
                    }
                    else
                    {
                        /**
                         * 대표값과의 비교
                         */

                        sCurIdx = 0;
                        
                        sRightValueBlock = sExeSubQueryFunc->mRightBlockList;
                        sValueBlockList  = sArrayFetchInfo->mRowColBlock;
                        
                        while( sValueBlockList != NULL )
                        {
                            sRecDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sLoop );
                            
                            if( DTL_IS_NULL( sRecDataValue ) == STL_TRUE )
                            {
                                break;
                            }

                            sRowDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sValueBlockList, 0 );

                            if( DTL_IS_NULL( sRowDataValue ) == STL_TRUE )
                            {
                                sIsNewRowCol = STL_TRUE;
                                break;
                            }
                            
                            sCompResult = sArrayFetchInfo->mCompFuncForBuild[ sCurIdx ]( sRecDataValue->mValue,
                                                                                         sRecDataValue->mLength,
                                                                                         sRowDataValue->mValue,
                                                                                         sRowDataValue->mLength );
                            sCompResult++;

                            if( sArrayFetchInfo->mIsCheckNextForBuild[ sCompResult ] == STL_FALSE )
                            {
                                sIsNewRowCol = sArrayFetchInfo->mResultForBuild[ sCompResult ];
                                break;
                            }

                            sCurIdx++;
                            sValueBlockList  = sValueBlockList->mNext;
                            sRightValueBlock = sRightValueBlock->mNext;
                        }

                        if( sValueBlockList == NULL )
                        {
                            sIsNewRowCol = sArrayFetchInfo->mResultForBuild[ sCompResult ];
                        }
                    }
                    
                    if( sIsNewRowCol == STL_TRUE )
                    {
                        /**
                         * 새로운 대표값으로 설정
                         */
                                    
                        sRightValueBlock = sExeSubQueryFunc->mRightBlockList;
                        sValueBlockList  = sArrayFetchInfo->mRowColBlock;
                        
                        while( sValueBlockList != NULL )
                        {
                            sRecDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sLoop );
                            
                            if( DTL_IS_NULL( sRecDataValue ) == STL_TRUE )
                            {
                                DTL_SET_NULL( KNL_GET_BLOCK_SEP_DATA_VALUE( sValueBlockList, 0 ) );
                            }
                            else
                            {
                                STL_TRY( knlCopyDataValue(
                                             KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sLoop ),
                                             KNL_GET_BLOCK_SEP_DATA_VALUE( sValueBlockList, 0 ),
                                             KNL_ENV( aEnv ) )
                                         == STL_SUCCESS );
                            }
                        
                            sValueBlockList  = sValueBlockList->mNext;
                            sRightValueBlock = sRightValueBlock->mNext;
                        }

                        sArrayFetchInfo->mIsUsedRowCol[ 0 ] = STL_TRUE;

                        if( sArrayFetchInfo->mRowColRecordCnt > 1 )
                        {
                            /**
                             * 새로운 대표값으로 설정
                             */
                                    
                            sColIdx++;
                            sRightValueBlock = sExeSubQueryFunc->mRightBlockList;
                            sValueBlockList  = sArrayFetchInfo->mRowColBlock;

                            while( sValueBlockList != NULL )
                            {
                                sRecDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sLoop );

                                if( DTL_IS_NULL( sRecDataValue ) == STL_TRUE )
                                {
                                    DTL_SET_NULL( KNL_GET_BLOCK_SEP_DATA_VALUE( sValueBlockList, sColIdx ) );
                                }
                                else
                                {
                                    STL_TRY( knlCopyDataValue(
                                                 KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sLoop ),
                                                 KNL_GET_BLOCK_SEP_DATA_VALUE( sValueBlockList, sColIdx ),
                                                 KNL_ENV( aEnv ) )
                                             == STL_SUCCESS );
                                }
                        
                                sValueBlockList  = sValueBlockList->mNext;
                                sRightValueBlock = sRightValueBlock->mNext;
                            }

                            sArrayFetchInfo->mIsUsedRowCol[ sColIdx ] = STL_TRUE;
                            continue;
                        }
                    }

                    
                    /**
                     * Row Column 을 위한 Array 구성 (그룹 대표값 설정)
                     */

                    if( sArrayFetchInfo->mRowColRecordCnt == 1 )
                    {
                        /* null check 가 필요없는 경우임 */
                        continue;
                    }
                    
                    sColIdx++;
                    sIsNewRowCol = STL_FALSE;
                    
                    if( sArrayFetchInfo->mIsUsedRowCol[ sColIdx ] == STL_FALSE )
                    {
                        sIsNewRowCol = STL_TRUE;
                    }
                    else
                    {
                        /**
                         * 대표값과의 비교
                         */

                        sCurIdx = 0;
                        
                        sRightValueBlock = sExeSubQueryFunc->mRightBlockList;
                        sValueBlockList  = sArrayFetchInfo->mRowColBlock;
                        
                        while( sValueBlockList != NULL )
                        {
                            sRecDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sLoop );
                            
                            if( DTL_IS_NULL( sRecDataValue ) == STL_TRUE )
                            {
                                break;
                            }

                            sRowDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sValueBlockList, sColIdx );

                            sCompResult = sArrayFetchInfo->mCompFuncForBuild[ sCurIdx ]( sRecDataValue->mValue,
                                                                                         sRecDataValue->mLength,
                                                                                         sRowDataValue->mValue,
                                                                                         sRowDataValue->mLength );

                            sCompResult++;
                            
                            if( sArrayFetchInfo->mIsCheckNextForBuild[ sCompResult ] == STL_FALSE )
                            {
                                sIsNewRowCol = sArrayFetchInfo->mResultForBuild[ sCompResult ];
                                break;
                            }

                            sCurIdx++;
                            sValueBlockList  = sValueBlockList->mNext;
                            sRightValueBlock = sRightValueBlock->mNext;
                        }

                        if( sValueBlockList == NULL )
                        {
                            sIsNewRowCol = sArrayFetchInfo->mResultForBuild[ sCompResult ];
                        }
                    }
                    
                    if( sIsNewRowCol == STL_TRUE )
                    {
                        /**
                         * 새로운 대표값으로 설정
                         */
                                    
                        sRightValueBlock = sExeSubQueryFunc->mRightBlockList;
                        sValueBlockList  = sArrayFetchInfo->mRowColBlock;
                        
                        while( sValueBlockList != NULL )
                        {
                            sRecDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sLoop );

                            if( DTL_IS_NULL( sRecDataValue ) == STL_TRUE )
                            {
                                DTL_SET_NULL( KNL_GET_BLOCK_SEP_DATA_VALUE( sValueBlockList, sColIdx ) );
                            }
                            else
                            {
                                STL_TRY( knlCopyDataValue(
                                             KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sLoop ),
                                             KNL_GET_BLOCK_SEP_DATA_VALUE( sValueBlockList, sColIdx ),
                                             KNL_ENV( aEnv ) )
                                         == STL_SUCCESS );
                            }
                        
                            sValueBlockList  = sValueBlockList->mNext;
                            sRightValueBlock = sRightValueBlock->mNext;
                        }

                        sArrayFetchInfo->mIsUsedRowCol[ sColIdx ] = STL_TRUE;
                    }
                }
            }
            else
            {
                break;
            }
        }

        sArrayFetchInfo->mIsNeedBuild = STL_FALSE;
        STL_THROW( RAMP_SKIP_MATERIALIZE );
    }


    /**
     * Build Instant
     */
    
    STL_DASSERT( sExeSubQueryFunc->mListFuncInstantFetchInfo != NULL );
    STL_DASSERT( sExeSubQueryFunc->mListFuncInstantFetchInfo->mInstantTable != NULL );
    
    sInstantTable = sExeSubQueryFunc->mListFuncInstantFetchInfo->mInstantTable;
    
    if( sInstantTable->mIsNeedBuild == STL_TRUE )
    {
        sInstantTable->mStmt   = sLocalFetchInfo.mStmt;
        sInstantTable->mCurIdx = 0;

        
        /**
         * Exists를 위한 Instant Table 정보 설정
         */

        if( ( sOptSubQueryFunc->mFuncExpr->mExpr.mListFunc->mFuncId
              == KNL_BUILTIN_FUNC_EXISTS ) ||
            ( sOptSubQueryFunc->mFuncExpr->mExpr.mListFunc->mFuncId
              == KNL_BUILTIN_FUNC_NOT_EXISTS ) )
        {
            /**
             * Block Read
             */

            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            /* fetch records*/
            sReadCnt = 0;
            QLNX_FETCH_NODE( & sLocalFetchInfo,
                             sOptSubQueryFunc->mRightOperandNode->mIdx,
                             0, /* Start Idx */
                             0, /* Skip Count */
                             1, /* Fetch Count */
                             & sReadCnt,
                             & sEOF,
                             aEnv );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            sInstantTable->mRecCnt = sReadCnt;
            
            STL_THROW( RAMP_SKIP_INSERT );
        }
        

        /**
         * Right Node Fetch하여 Sort Instant Table에 Insert
         */
        
        while( sEOF == STL_FALSE )
        {
            /**
             * Block Read
             */

            QLL_OPT_CHECK_TIME( sBeginExceptTime );

            /* fetch records*/
            sReadCnt = 0;
            QLNX_FETCH_NODE( & sLocalFetchInfo,
                             sOptSubQueryFunc->mRightOperandNode->mIdx,
                             0, /* Start Idx */
                             0, /* Skip Count */
                             QLL_FETCH_COUNT_MAX, /* Fetch Count */
                             & sReadCnt,
                             & sEOF,
                             aEnv );

            QLL_OPT_CHECK_TIME( sEndExceptTime );
            QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

            if( sReadCnt > 0 )
            {
                /**
                 * Instant Table에 Record 삽입
                 */

                sCurIdx = 0;
                for( sLoop = 0 ; sLoop < sReadCnt ; sLoop++ )
                {
                    sValueBlockList  = sExeSubQueryFunc->mBlockListForCast;
                    sRightValueBlock = sExeSubQueryFunc->mRightBlockList;
                    sColIdx          = 0;
                    
                    while( sValueBlockList != NULL )
                    {
                        sRightCastInfo = sExeSubQueryFunc->mRightCastInfo[ sColIdx ];
                        
                        if( DTL_IS_NULL( KNL_GET_BLOCK_DATA_VALUE( sValueBlockList, sLoop ) ) )
                        {
                            /* Exact */
                            DTL_SET_NULL( KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sCurIdx ) );
                        }
                        else
                        {
                            /* Evaluate Right Operand Expresson's Internal Cast */
                            if( sRightCastInfo == NULL )
                            {
                                /* copy */
                                STL_TRY( knlCopyDataValue(
                                             KNL_GET_BLOCK_DATA_VALUE( sValueBlockList, sLoop ),
                                             KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sCurIdx ),
                                             KNL_ENV( aEnv ) )
                                         == STL_SUCCESS );
                            }
                            else
                            {
                                /* cast */
                                sRightCastInfo->mArgs[0].mValue.mDataValue =
                                    KNL_GET_BLOCK_DATA_VALUE( sValueBlockList, sLoop );
                        
                                STL_TRY( sRightCastInfo->mCastFuncPtr(
                                             DTL_CAST_INPUT_ARG_CNT,
                                             sRightCastInfo->mArgs,
                                             KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sCurIdx ),
                                             (void *)(sRightCastInfo->mSrcTypeInfo),
                                             KNL_DT_VECTOR(aEnv),
                                             aEnv,
                                             KNL_ERROR_STACK( aEnv ) )
                                         == STL_SUCCESS );
                            }
                        }

                        sColIdx++;
                        
                        sValueBlockList  = sValueBlockList->mNext;
                        sRightValueBlock = sRightValueBlock->mNext;
                    }
                    
                    sCurIdx++;
                }

                if( sCurIdx > 0 )
                {
                    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeSubQueryFunc->mRightBlockList, 0, sCurIdx );
                    
                    sReadCnt = sCurIdx;
                    
                    STL_TRY( smlInsertRecord( sLocalFetchInfo.mStmt,
                                              sInstantTable->mTableHandle,
                                              sExeSubQueryFunc->mRightBlockList,
                                              sLocalFetchInfo.mDataArea->mPredResultBlock, /* unique violation */
                                              & sExeSubQueryFunc->mListFuncInstantFetchInfo->mSortRowBlock,
                                              SML_ENV( aEnv ) )
                             == STL_SUCCESS );

                    sDataValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( sLocalFetchInfo.mDataArea->mPredResultBlock );

                    for( sLoop = 0 ; sLoop < sCurIdx ; sLoop++ )
                    {
                        if( DTL_BOOLEAN_IS_TRUE( sDataValue ) == STL_TRUE )
                        {
                            sReadCnt--;
                        }
                        sDataValue++;
                    }
                    
                    /* Unique 처리 후의 record 개수 (NULL Row는 제거) */
                    sInstantTable->mRecCnt += sReadCnt;
                }
            }
            else
            {
                break;
            }
        }


        /**
         * Iterator Allocation
         */
        
        sInstantTable->mIterator = NULL;

        if( sInstantTable->mRecCnt > 0 )
        {
            SML_INIT_ITERATOR_PROP( sInstantTable->mIteratorProperty );
            
            STL_TRY( smlAllocIterator( sLocalFetchInfo.mStmt,
                                       sInstantTable->mTableHandle,
                                       SML_TRM_SNAPSHOT,
                                       SML_SRM_SNAPSHOT,
                                       & sInstantTable->mIteratorProperty,
                                       SML_SCAN_FORWARD,
                                       & sInstantTable->mIterator,
                                       SML_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
        else
        {
            sInstantTable->mIterator = NULL;
        }

        STL_RAMP( RAMP_SKIP_INSERT );

        sInstantTable->mIsNeedBuild = STL_FALSE;
    }
    else
    {
        /* Do Nothing */
    }
    
    STL_RAMP( RAMP_SKIP_MATERIALIZE );
    
    
    /**
     * Evaluate Left Operand Expression
     */

    sBlockExprEvalInfo.mContext    = sLocalFetchInfo.mDataArea->mExprDataContext;
    sBlockExprEvalInfo.mBlockIdx   = 0;
    sBlockExprEvalInfo.mBlockCount = sFetchedCnt;

    sValueBlockList = sExeSubQueryFunc->mLeftBlockList;

    if( sOptSubQueryFunc->mLeftOperCodeStack != NULL )
    {
        for( sLoop = 0 ; sLoop < sExeSubQueryFunc->mValueBlockCnt ; sLoop++ )
        {
            if( sOptSubQueryFunc->mLeftOperCodeStack[ sLoop ] != NULL )
            {
                sBlockExprEvalInfo.mExprStack = sOptSubQueryFunc->mLeftOperCodeStack[ sLoop ];
                sBlockExprEvalInfo.mResultBlock = sValueBlockList;
        
                STL_TRY( knlEvaluateBlockExpression( & sBlockExprEvalInfo,
                                                     KNL_ENV( aEnv ) ) 
                         == STL_SUCCESS );
            }

            sValueBlockList = sValueBlockList->mNext;
        }
    }

    
    /**
     * Sub Query Node Fetch 수행
     */

    for( sCurIdx = 0 ; sCurIdx < sFetchedCnt ; sCurIdx++ )
    {
        /**
         * Set Outer Column Value
         */

        if( ( sExeSubQueryFunc->mNeedMaterialize == STL_FALSE ) &&
            ( sOptSubQueryFunc->mChildNode != NULL ) )
        {
            STL_TRY( qlndSetOuterColumnValue( sLocalFetchInfo.mDataArea,
                                              sOptSubQueryFunc->mChildNode->mIdx,
                                              sStartIdx + sCurIdx,
                                              aEnv )
                     == STL_SUCCESS );
        }


        /**
         * Call Sub Query Function
         */
    
        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        STL_TRY( sExeSubQueryFunc->mSubQueryFuncPtr( sLocalFetchInfo.mTransID,
                                                     sLocalFetchInfo.mStmt,
                                                     sLocalFetchInfo.mDBCStmt,
                                                     sLocalFetchInfo.mSQLStmt,
                                                     sExecNode->mOptNode,
                                                     sLocalFetchInfo.mDataArea,
                                                     sStartIdx + sCurIdx,
                                                     aEnv )
                 == STL_SUCCESS );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );
    }
        

    STL_RAMP( RAMP_FINISH );

    
    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt = 0;

    *aEOF = STL_FALSE;
    
    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sExeSubQueryFunc->mResultBlock,
                                         0,
                                         sFetchedCnt );

    
    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSubQueryFunc->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSubQueryFunc->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeSubQueryFunc->mCommonInfo.mFetchRecordStat,
                                  sExeSubQueryFunc->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeSubQueryFunc->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;

    
    return STL_SUCCESS;

    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSubQueryFunc->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeSubQueryFunc->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief SUB QUERY FUNCTION node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeSubQueryFunc( qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode        = NULL;
    /* qlnoSubQueryFunc  * sOptSubQueryFunc = NULL; */
    qlnxSubQueryFunc  * sExeSubQueryFunc = NULL;
    qlnInstantTable   * sInstantTable    = NULL;
    
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * SUB-QUERY FUNCTION Optimization Node 획득
     */

    /* sOptSubQueryFunc = (qlnoSubQueryFunc *) aOptNode->mOptNode; */

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SUB-QUERY FUNCTION Execution Node 획득
     */

    sExeSubQueryFunc = sExecNode->mExecNode;

    if( ( sExeSubQueryFunc->mNeedMaterialize == STL_TRUE ) &&
        ( sExeSubQueryFunc->mIsOrdering == STL_FALSE ) )
    {
        sInstantTable = sExeSubQueryFunc->mListFuncInstantFetchInfo->mInstantTable;

        if( sInstantTable->mTableHandle != NULL )
        {
            STL_DASSERT( sInstantTable->mStmt != NULL );
        
            STL_TRY( smlDropTable( sInstantTable->mStmt,
                                   sInstantTable->mTableHandle,
                                   SML_ENV( aEnv ) )
                     == STL_SUCCESS );

            sInstantTable->mTableHandle = NULL;
        }
            
        if( sInstantTable->mIterator != NULL )
        {
            (void) smlFreeIterator( sInstantTable->mIterator, SML_ENV(aEnv) );
            sInstantTable->mIterator = NULL;
        }

        sInstantTable->mStmt = NULL;
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
 * @brief EXISTS function을 수행한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aIdx          Result Value Block Idx
 * @param[in]      aEnv          Environment
 */
stlStatus qlnxSubQueryFuncExists( smlTransId              aTransID,
                                  smlStatement          * aStmt,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  stlInt32                aIdx,
                                  qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode        = NULL;
    qlnoSubQueryFunc  * sOptSubQueryFunc = NULL;
    qlnxSubQueryFunc  * sExeSubQueryFunc = NULL;

    dtlDataValue      * sResultDataValue = NULL;
    stlInt64            sUsedBlockCnt    = 0;
    stlBool             sEOF             = STL_FALSE;

    qlnxFetchNodeInfo   sFetchNodeInfo;
    
    
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

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    
    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * SUB-QUERY FUNCTION Optimization Node 획득
     */

    sOptSubQueryFunc = (qlnoSubQueryFunc *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SUB-QUERY FUNCTION Execution Node 획득
     */

    sExeSubQueryFunc = sExecNode->mExecNode;


    /**
     * Result Data Value 얻기
     */
    
    sResultDataValue = KNL_GET_BLOCK_DATA_VALUE( sExeSubQueryFunc->mResultBlock, aIdx );
    

    /**
     * Fetch Node Info 설정
     */
    
    QLNX_INIT_FETCH_NODE_INFO( & sFetchNodeInfo,
                               aTransID,
                               aStmt,
                               aDBCStmt,
                               aSQLStmt,
                               aDataArea );


    /*****************************************
     * Operand Node Fetch 수행
     ****************************************/

    /**
     * Fetch Right Operand Node
     */
    
    QLL_OPT_CHECK_TIME( sBeginExceptTime );

    STL_DASSERT( sExeSubQueryFunc->mNeedMaterialize == STL_FALSE );

    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[ sOptSubQueryFunc->mRightOperandNode->mIdx ].mOptNode,
                       aDataArea,
                       aEnv );

    QLNX_FETCH_NODE( & sFetchNodeInfo,
                     sOptSubQueryFunc->mRightOperandNode->mIdx,
                     0, /* Start Idx */
                     0,
                     1,
                     & sUsedBlockCnt,
                     & sEOF,
                     aEnv );
    
    QLL_OPT_CHECK_TIME( sEndExceptTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );


    /*****************************************
     * Sub Query Function 평가
     ****************************************/

    if( sExeSubQueryFunc->mIsResReverse == STL_FALSE )
    {
        DTL_BOOLEAN( sResultDataValue ) = ( sUsedBlockCnt > 0 );
    }
    else
    {
        DTL_BOOLEAN( sResultDataValue ) = ( sUsedBlockCnt == 0 );
    }

    DTL_SET_BOOLEAN_LENGTH( sResultDataValue );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSubQueryFunc->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief EXISTS function을 수행한다. (Right Node가 Materialize되어 있음)
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aIdx          Result Value Block Idx
 * @param[in]      aEnv          Environment
 */
stlStatus qlnxSubQueryFuncExistsWithInstant( smlTransId              aTransID,
                                             smlStatement          * aStmt,
                                             qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptimizationNode   * aOptNode,
                                             qllDataArea           * aDataArea,
                                             stlInt32                aIdx,
                                             qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode        = NULL;
    qlnxSubQueryFunc  * sExeSubQueryFunc = NULL;

    dtlDataValue      * sResultDataValue = NULL;
    
    
    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    
    /*****************************************
     * 기본 정보 획득
     ****************************************/

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SUB-QUERY FUNCTION Execution Node 획득
     */

    sExeSubQueryFunc = sExecNode->mExecNode;


    /**
     * Result Data Value 얻기
     */
    
    sResultDataValue = KNL_GET_BLOCK_DATA_VALUE( sExeSubQueryFunc->mResultBlock, aIdx );
    
    STL_DASSERT( sExeSubQueryFunc->mNeedMaterialize == STL_TRUE );


    /*****************************************
     * Operand Node Fetch 수행
     ****************************************/

    /* Do Nothing */


    /*****************************************
     * Sub Query Function 평가
     ****************************************/

    if( sExeSubQueryFunc->mIsResReverse == STL_FALSE )
    {
        DTL_BOOLEAN( sResultDataValue ) =
            ( sExeSubQueryFunc->mListFuncInstantFetchInfo->mInstantTable->mRecCnt > 0 );
    }
    else
    {
        DTL_BOOLEAN( sResultDataValue ) =
            ( sExeSubQueryFunc->mListFuncInstantFetchInfo->mInstantTable->mRecCnt == 0 );
    }

    DTL_SET_BOOLEAN_LENGTH( sResultDataValue );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSubQueryFunc->mCommonInfo.mFetchTime, sFetchTime );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Quantifier Comparison function을 수행한다. (ANY)
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aIdx          Result Value Block Idx
 * @param[in]      aEnv          Environment
 */
stlStatus qlnxSubQueryFuncCompAnyOrdering( smlTransId              aTransID,
                                           smlStatement          * aStmt,
                                           qllDBCStmt            * aDBCStmt,
                                           qllStatement          * aSQLStmt,
                                           qllOptimizationNode   * aOptNode,
                                           qllDataArea           * aDataArea,
                                           stlInt32                aIdx,
                                           qllEnv                * aEnv )
{
    qllExecutionNode   * sExecNode        = NULL;
    qlnoSubQueryFunc   * sOptSubQueryFunc = NULL;
    qlnxSubQueryFunc   * sExeSubQueryFunc = NULL;

    knlCastFuncInfo    * sRightCastInfo   = NULL;
    knlValueBlockList  * sLeftValueBlock  = NULL;
    knlValueBlockList  * sRightValueBlock = NULL;
    knlValueBlockList  * sValueBlockList  = NULL;
    
    dtlDataValue       * sLeftDataValue   = NULL;
    dtlDataValue       * sRightDataValue  = NULL;
    dtlDataValue       * sResultDataValue = NULL;
    dtlCompareResult     sCompResult      = DTL_COMPARISON_EQUAL;
    stlInt64             sUsedBlockCnt    = 0;
    stlInt32             sLoop            = 0;
    stlInt32             sIdx             = 0;
    stlBool              sEOF             = STL_FALSE;
    stlBool              sHasNullValue    = STL_FALSE;

    qlnxFetchNodeInfo    sFetchNodeInfo;
        
    
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

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    
    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * SUB-QUERY FUNCTION Optimization Node 획득
     */

    sOptSubQueryFunc = (qlnoSubQueryFunc *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SUB-QUERY FUNCTION Execution Node 획득
     */

    sExeSubQueryFunc = sExecNode->mExecNode;

    
    /**
     * Result Data Value 얻기
     */
    
    sResultDataValue = KNL_GET_BLOCK_DATA_VALUE( sExeSubQueryFunc->mResultBlock, aIdx );

    STL_DASSERT( sExeSubQueryFunc->mNeedMaterialize == STL_FALSE );
    STL_DASSERT( sExeSubQueryFunc->mIsOrdering == STL_TRUE );

    
    /**
     * Fetch Node Info 설정
     */
    
    QLNX_INIT_FETCH_NODE_INFO( & sFetchNodeInfo,
                               aTransID,
                               aStmt,
                               aDBCStmt,
                               aSQLStmt,
                               aDataArea );


    /*****************************************
     * Operand Node Fetch 수행
     ****************************************/

    /**
     * Fetch Right Operand Node
     */
    
    QLL_OPT_CHECK_TIME( sBeginExceptTime );

    /* Fetch Next */
    sEOF = STL_FALSE;

    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[ sOptSubQueryFunc->mRightOperandNode->mIdx ].mOptNode,
                       aDataArea,
                       aEnv );

    QLNX_FETCH_NODE( & sFetchNodeInfo,
                     sOptSubQueryFunc->mRightOperandNode->mIdx,
                     0, /* Start Idx */
                     0,
                     QLL_FETCH_COUNT_MAX,
                     & sUsedBlockCnt,
                     & sEOF,
                     aEnv );

    QLL_OPT_CHECK_TIME( sEndExceptTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

    if( sUsedBlockCnt == 0 )
    {
        DTL_BOOLEAN( sResultDataValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( sResultDataValue );

        STL_THROW( RAMP_FINISH );
    }
    else
    {
        sLeftDataValue = KNL_GET_BLOCK_DATA_VALUE( sExeSubQueryFunc->mLeftBlockList, aIdx );
            
        if( DTL_IS_NULL( sLeftDataValue ) == STL_TRUE )
        {
            DTL_SET_NULL( sResultDataValue );
            STL_THROW( RAMP_FINISH );
        }
    }
    

    /*****************************************
     * Sub Query Function 평가
     ****************************************/

    while( sUsedBlockCnt > 0 )
    {
        for( sLoop = 0 ; sLoop < sUsedBlockCnt ; sLoop++ )
        {
            sLeftValueBlock  = sExeSubQueryFunc->mLeftBlockList;
            sRightValueBlock = sExeSubQueryFunc->mRightBlockList;
            sValueBlockList  = sExeSubQueryFunc->mBlockListForCast;

            for( sIdx = 0 ; sLeftValueBlock != NULL ; sIdx++ )
            {
                STL_DASSERT( sRightValueBlock != NULL );

                sRightCastInfo = sExeSubQueryFunc->mRightCastInfo[ sIdx ];
                sLeftDataValue = KNL_GET_BLOCK_DATA_VALUE( sLeftValueBlock, aIdx );

                if( DTL_IS_NULL( sLeftDataValue ) == STL_TRUE )
                {
                    sHasNullValue = STL_TRUE;
                    STL_THROW( RAMP_SKIP_COMP );
                }

                if( sRightCastInfo == NULL )
                {
                    /* get */
                    sRightDataValue = KNL_GET_BLOCK_DATA_VALUE( sValueBlockList, sLoop );

                    if( DTL_IS_NULL( sRightDataValue ) == STL_TRUE )
                    {
                        sHasNullValue = STL_TRUE;
                        STL_THROW( RAMP_SKIP_COMP );
                    }
                }
                else
                {
                    /* cast */
                    sRightCastInfo->mArgs[0].mValue.mDataValue =
                        KNL_GET_BLOCK_DATA_VALUE( sValueBlockList, sLoop );
                        
                    if( DTL_IS_NULL( sRightCastInfo->mArgs[0].mValue.mDataValue ) == STL_TRUE )
                    {
                        sHasNullValue = STL_TRUE;
                        STL_THROW( RAMP_SKIP_COMP );
                    }
                    else
                    {
                        sRightDataValue = KNL_GET_BLOCK_DATA_VALUE( sRightValueBlock, sLoop );
                        
                        STL_TRY( sRightCastInfo->mCastFuncPtr(
                                     DTL_CAST_INPUT_ARG_CNT,
                                     sRightCastInfo->mArgs,
                                     sRightDataValue,
                                     (void *)(sRightCastInfo->mSrcTypeInfo),
                                     KNL_DT_VECTOR(aEnv),
                                     aEnv,
                                     KNL_ERROR_STACK( aEnv ) )
                                 == STL_SUCCESS );
                    }
                }

                sCompResult = sExeSubQueryFunc->mCompFunc[ sIdx ]( sLeftDataValue->mValue,
                                                                   sLeftDataValue->mLength,
                                                                   sRightDataValue->mValue,
                                                                   sRightDataValue->mLength );
                sCompResult++;

                if( sExeSubQueryFunc->mIsCheckNext[ sCompResult ] == STL_FALSE )
                {
                    break;
                }

                sLeftValueBlock  = sLeftValueBlock->mNext;
                sRightValueBlock = sRightValueBlock->mNext;
                sValueBlockList  = sValueBlockList->mNext;
            }
            
            if( sExeSubQueryFunc->mResult[ sCompResult ] == STL_TRUE )
            {
                DTL_BOOLEAN( sResultDataValue ) = STL_TRUE;
                DTL_SET_BOOLEAN_LENGTH( sResultDataValue );
                STL_THROW( RAMP_FINISH );
            }

            STL_RAMP( RAMP_SKIP_COMP );
        }

        
        /**
         * Fetch Right Operand Node
         */
    
        if( sEOF == STL_TRUE )
        {
            break;
        }

        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        /* Fetch Next */
        QLNX_FETCH_NODE( & sFetchNodeInfo,
                         sOptSubQueryFunc->mRightOperandNode->mIdx,
                         0, /* Start Idx */
                         0,
                         QLL_FETCH_COUNT_MAX,
                         & sUsedBlockCnt,
                         & sEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );
    }


    /**
     * Sub Query Function Result 설정
     */
    
    if( sHasNullValue == STL_TRUE )
    {
        DTL_SET_NULL( sResultDataValue );
    }
    else
    {
        DTL_BOOLEAN( sResultDataValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( sResultDataValue );
    }


    /*****************************************
     * FINISH
     ****************************************/

    STL_RAMP( RAMP_FINISH );

    
    /**
     * ALL quantifier에 의해 호출된 경우 Result를 반전시킨다.
     */
    
    if( sExeSubQueryFunc->mIsResReverse == STL_TRUE )
    {
        if( DTL_IS_NULL( sResultDataValue ) != STL_TRUE )
        {
            if( DTL_BOOLEAN( sResultDataValue ) == STL_TRUE )
            {
                DTL_BOOLEAN( sResultDataValue ) = STL_FALSE;
            }
            else
            {
                DTL_BOOLEAN( sResultDataValue ) = STL_TRUE;
            }
        }
    }

    
    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSubQueryFunc->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Quantifier Comparison function을 수행한다. (ANY, Right Node가 Array로 구성되어 있음)
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aIdx          Result Value Block Idx
 * @param[in]      aEnv          Environment
 */
stlStatus qlnxSubQueryFuncCompAnyWithArrayOrdering( smlTransId              aTransID,
                                                    smlStatement          * aStmt,
                                                    qllDBCStmt            * aDBCStmt,
                                                    qllStatement          * aSQLStmt,
                                                    qllOptimizationNode   * aOptNode,
                                                    qllDataArea           * aDataArea,
                                                    stlInt32                aIdx,
                                                    qllEnv                * aEnv )
{
    qllExecutionNode            * sExecNode        = NULL;
//    qlnoSubQueryFunc            * sOptSubQueryFunc = NULL;
    qlnxSubQueryFunc            * sExeSubQueryFunc = NULL;
    qlnxListFuncArrayFetchInfo  * sArrayFetchInfo  = NULL;
        
    knlValueBlockList           * sLeftValueBlock  = NULL;
    knlValueBlockList           * sRightValueBlock = NULL;
    
    dtlDataValue                * sLeftDataValue   = NULL;
    dtlDataValue                * sRightDataValue  = NULL;
    dtlDataValue                * sResultDataValue = NULL;
    dtlCompareResult              sCompResult      = DTL_COMPARISON_EQUAL;
    stlInt32                      sLoop            = 0;
    stlInt32                      sIdx             = 0;
    
    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    
    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * SUB-QUERY FUNCTION Optimization Node 획득
     */

//    sOptSubQueryFunc = (qlnoSubQueryFunc *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SUB-QUERY FUNCTION Execution Node 획득
     */

    sExeSubQueryFunc = sExecNode->mExecNode;

    
    /**
     * Result Data Value 얻기
     */
    
    sResultDataValue = KNL_GET_BLOCK_DATA_VALUE( sExeSubQueryFunc->mResultBlock, aIdx );


    /**
     * Instant Table 얻기
     */

    STL_DASSERT( sExeSubQueryFunc->mListFuncArrayFetchInfo != NULL );

    sArrayFetchInfo = sExeSubQueryFunc->mListFuncArrayFetchInfo;

    STL_DASSERT( sExeSubQueryFunc->mNeedMaterialize == STL_TRUE );
    STL_DASSERT( sExeSubQueryFunc->mIsOrdering == STL_TRUE );
    
    
    /*****************************************
     * Node Fetch 이전 평가
     ****************************************/

    /**
     * Fetch 이전 평가
     */

    if( sArrayFetchInfo->mIsUsedRowCol[ 0 ] == STL_FALSE )
    {
        /* Inner 쪽에 record가 없는 경우 */
        DTL_BOOLEAN( sResultDataValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( sResultDataValue );
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Do Nothing */
    }
    

    /*****************************************
     * Row Column Block 의 대표값과의 비교
     ****************************************/

    /**
     * Row Column 들의 첫번째 value 와의 비교
     */
    
    sLeftValueBlock  = sExeSubQueryFunc->mLeftBlockList;
    sRightValueBlock = sArrayFetchInfo->mRowColBlock;
            
    for( sIdx = 0 ; sLeftValueBlock != NULL ; sIdx++ )
    {
        STL_DASSERT( sRightValueBlock != NULL );

        sLeftDataValue = KNL_GET_BLOCK_DATA_VALUE( sLeftValueBlock, aIdx );

        if( DTL_IS_NULL( sLeftDataValue ) == STL_TRUE )
        {
            DTL_SET_NULL( sResultDataValue );
            STL_THROW( RAMP_FINISH );
        }

        sRightDataValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( sRightValueBlock );

        if( DTL_IS_NULL( sRightDataValue ) == STL_TRUE )
        {
            DTL_SET_NULL( sResultDataValue );
            STL_THROW( RAMP_FINISH );
        }

        sCompResult = sExeSubQueryFunc->mCompFunc[ sIdx ]( sLeftDataValue->mValue,
                                                           sLeftDataValue->mLength,
                                                           sRightDataValue->mValue,
                                                           sRightDataValue->mLength );
        sCompResult++;
        
        if( sExeSubQueryFunc->mIsCheckNext[ sCompResult ] == STL_FALSE )
        {
            if( sExeSubQueryFunc->mResult[ sCompResult ] == STL_TRUE )
            {
                DTL_BOOLEAN( sResultDataValue ) = STL_TRUE;
                DTL_SET_BOOLEAN_LENGTH( sResultDataValue );
                STL_THROW( RAMP_FINISH );
            }
            break;
        }
                
        sLeftValueBlock  = sLeftValueBlock->mNext;
        sRightValueBlock = sRightValueBlock->mNext;
    }

    if( sExeSubQueryFunc->mResult[ sCompResult ] == STL_TRUE )
    {
        DTL_BOOLEAN( sResultDataValue ) = STL_TRUE;
        DTL_SET_BOOLEAN_LENGTH( sResultDataValue );
        STL_THROW( RAMP_FINISH );
    }
    

    /*****************************************
     * Unknown 여부를 평가
     ****************************************/

    /**
     * Unknown 평가가 필요없는 경우 result 반환
     */
    
    if( sArrayFetchInfo->mRowColRecordCnt == 1 )
    {
        DTL_BOOLEAN( sResultDataValue ) = sExeSubQueryFunc->mResult[ sCompResult ];
        DTL_SET_BOOLEAN_LENGTH( sResultDataValue );
        STL_THROW( RAMP_FINISH );
    }

    
    /**
     * 모든 Column의 값이 Null Value인 Row 가 있는 경우
     */
    
    if( sArrayFetchInfo->mIsUsedRowCol[ 1 ] == STL_TRUE )
    {
        DTL_SET_NULL( sResultDataValue );
        STL_THROW( RAMP_FINISH );
    }
    

    /**
     * 일부 Column의 값이 Null Value인 경우
     */

    for( sLoop = 2 ; sLoop < sArrayFetchInfo->mRowColRecordCnt ; sLoop++ )
    {
        if( sArrayFetchInfo->mIsUsedRowCol[ sLoop ] == STL_FALSE )
        {
            /* Left Value의 null 값이 평가되기 이전에 이미 대소 비교가 정해짐 */
            continue;
        }

        sLeftValueBlock  = sExeSubQueryFunc->mLeftBlockList;
        sRightValueBlock = sArrayFetchInfo->mRowColBlock;
            
        for( sIdx = 0 ; sLeftValueBlock != NULL ; sIdx++ )
        {
            STL_DASSERT( sRightValueBlock != NULL );

            sLeftDataValue = KNL_GET_BLOCK_DATA_VALUE( sLeftValueBlock, aIdx );

            /* Left Value의 null 값이 평가되기 이전에 이미 대소 비교가 정해짐 */
            STL_DASSERT( DTL_IS_NULL( sLeftDataValue ) == STL_FALSE );

            sRightDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sLoop );

            if( DTL_IS_NULL( sRightDataValue ) == STL_TRUE )
            {
                DTL_SET_NULL( sResultDataValue );
                STL_THROW( RAMP_FINISH );
            }

            sCompResult = sExeSubQueryFunc->mCompFunc[ sIdx ]( sLeftDataValue->mValue,
                                                               sLeftDataValue->mLength,
                                                               sRightDataValue->mValue,
                                                               sRightDataValue->mLength );
            sCompResult++;
                
            if( sExeSubQueryFunc->mIsCheckNext[ sCompResult ] == STL_FALSE )
            {
                STL_DASSERT( sExeSubQueryFunc->mResult[ sCompResult ] == STL_FALSE );
                break;
            }
                
            sLeftValueBlock  = sLeftValueBlock->mNext;
            sRightValueBlock = sRightValueBlock->mNext;
        }
    }
    
    
    /**
     * Sub Query Function Result 설정 (결과가 FALSE 인 경우)
     */

    DTL_BOOLEAN( sResultDataValue ) = STL_FALSE;
    DTL_SET_BOOLEAN_LENGTH( sResultDataValue );
    

    /*****************************************
     * FINISH
     ****************************************/

    STL_RAMP( RAMP_FINISH );


    /**
     * ALL quantifier에 의해 호출된 경우 Result를 반전시킨다.
     */
    
    if( sExeSubQueryFunc->mIsResReverse == STL_TRUE )
    {
        if( DTL_IS_NULL( sResultDataValue ) != STL_TRUE )
        {
            if( DTL_BOOLEAN( sResultDataValue ) == STL_TRUE )
            {
                DTL_BOOLEAN( sResultDataValue ) = STL_FALSE;
            }
            else
            {
                DTL_BOOLEAN( sResultDataValue ) = STL_TRUE;
            }
        }
    }

    
    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSubQueryFunc->mCommonInfo.mFetchTime, sFetchTime );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Quantifier Comparison function을 수행한다. (ANY)
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aIdx          Result Value Block Idx
 * @param[in]      aEnv          Environment
 */
stlStatus qlnxSubQueryFuncCompAnyExact( smlTransId              aTransID,
                                        smlStatement          * aStmt,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        stlInt32                aIdx,
                                        qllEnv                * aEnv )
{
    qllExecutionNode   * sExecNode        = NULL;
    qlnoSubQueryFunc   * sOptSubQueryFunc = NULL;
    qlnxSubQueryFunc   * sExeSubQueryFunc = NULL;

    knlCastFuncInfo    * sRightCastInfo   = NULL;
    knlValueBlockList  * sLeftValueBlock  = NULL;
    knlValueBlockList  * sRightValueBlock = NULL;
    knlValueBlockList  * sValueBlockList  = NULL;
    
    dtlDataValue       * sLeftDataValue   = NULL;
    dtlDataValue       * sRightDataValue  = NULL;
    dtlDataValue       * sResultDataValue = NULL;
    dtlCompareResult     sCompResult      = DTL_COMPARISON_EQUAL;
    stlInt64             sUsedBlockCnt    = 0;
    stlInt32             sLoop            = 0;
    stlInt32             sIdx             = 0;
    stlBool              sEOF             = STL_FALSE;
    stlBool              sHasNullValue    = STL_FALSE;
    stlBool              sTmpHasNullValue = STL_FALSE;

    qlnxFetchNodeInfo    sFetchNodeInfo;
        
    
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

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    
    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * SUB-QUERY FUNCTION Optimization Node 획득
     */

    sOptSubQueryFunc = (qlnoSubQueryFunc *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SUB-QUERY FUNCTION Execution Node 획득
     */

    sExeSubQueryFunc = sExecNode->mExecNode;

    
    /**
     * Result Data Value 얻기
     */
    
    sResultDataValue = KNL_GET_BLOCK_DATA_VALUE( sExeSubQueryFunc->mResultBlock, aIdx );

    STL_DASSERT( sExeSubQueryFunc->mNeedMaterialize == STL_FALSE );
    STL_DASSERT( sExeSubQueryFunc->mIsOrdering == STL_FALSE );

    
    /**
     * Fetch Node Info 설정
     */
    
    QLNX_INIT_FETCH_NODE_INFO( & sFetchNodeInfo,
                               aTransID,
                               aStmt,
                               aDBCStmt,
                               aSQLStmt,
                               aDataArea );


    /*****************************************
     * Operand Node Fetch 수행
     ****************************************/

    /**
     * Fetch Right Operand Node
     */
    
    QLL_OPT_CHECK_TIME( sBeginExceptTime );

    /* Fetch Next */
    sEOF = STL_FALSE;

    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[ sOptSubQueryFunc->mRightOperandNode->mIdx ].mOptNode,
                       aDataArea,
                       aEnv );

    QLNX_FETCH_NODE( & sFetchNodeInfo,
                     sOptSubQueryFunc->mRightOperandNode->mIdx,
                     0, /* Start Idx */
                     0,
                     QLL_FETCH_COUNT_MAX,
                     & sUsedBlockCnt,
                     & sEOF,
                     aEnv );

    QLL_OPT_CHECK_TIME( sEndExceptTime );
    QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

    if( sUsedBlockCnt == 0 )
    {
        DTL_BOOLEAN( sResultDataValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( sResultDataValue );

        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Do Nothing */
    }
    

    /*****************************************
     * Sub Query Function 평가
     ****************************************/

    while( sUsedBlockCnt > 0 )
    {
        for( sLoop = 0 ; sLoop < sUsedBlockCnt ; sLoop++ )
        {
            sLeftValueBlock  = sExeSubQueryFunc->mLeftBlockList;
            sRightValueBlock = sExeSubQueryFunc->mRightBlockList;
            sValueBlockList  = sExeSubQueryFunc->mBlockListForCast;

            sTmpHasNullValue = STL_FALSE;
            
            for( sIdx = 0 ; sLeftValueBlock != NULL ; sIdx++ )
            {
                STL_DASSERT( sRightValueBlock != NULL );

                sRightCastInfo = sExeSubQueryFunc->mRightCastInfo[ sIdx ];
                sLeftDataValue = KNL_GET_BLOCK_DATA_VALUE( sLeftValueBlock, aIdx );

                if( DTL_IS_NULL( sLeftDataValue ) == STL_TRUE )
                {
                    sTmpHasNullValue = STL_TRUE;
                    STL_THROW( RAMP_NEXT_COMP );
                }

                if( sRightCastInfo == NULL )
                {
                    /* get */
                    sRightDataValue = KNL_GET_BLOCK_DATA_VALUE( sValueBlockList, sLoop );

                    if( DTL_IS_NULL( sRightDataValue ) == STL_TRUE )
                    {
                        sTmpHasNullValue = STL_TRUE;
                        STL_THROW( RAMP_NEXT_COMP );
                    }
                }
                else
                {
                    /* cast */
                    sRightCastInfo->mArgs[0].mValue.mDataValue =
                        KNL_GET_BLOCK_DATA_VALUE( sValueBlockList, sLoop );
                        
                    if( DTL_IS_NULL( sRightCastInfo->mArgs[0].mValue.mDataValue ) == STL_TRUE )
                    {
                        sTmpHasNullValue = STL_TRUE;
                        STL_THROW( RAMP_NEXT_COMP );
                    }
                    else
                    {
                        sRightDataValue = KNL_GET_BLOCK_DATA_VALUE( sRightValueBlock, sLoop );
                        
                        STL_TRY( sRightCastInfo->mCastFuncPtr(
                                     DTL_CAST_INPUT_ARG_CNT,
                                     sRightCastInfo->mArgs,
                                     sRightDataValue,
                                     (void *)(sRightCastInfo->mSrcTypeInfo),
                                     KNL_DT_VECTOR(aEnv),
                                     aEnv,
                                     KNL_ERROR_STACK( aEnv ) )
                                 == STL_SUCCESS );
                    }
                }

                sCompResult = sExeSubQueryFunc->mCompFunc[ sIdx ]( sLeftDataValue->mValue,
                                                                   sLeftDataValue->mLength,
                                                                   sRightDataValue->mValue,
                                                                   sRightDataValue->mLength );
                sCompResult++;

                if( sExeSubQueryFunc->mIsCheckNext[ sCompResult ] == STL_FALSE )
                {
                    sTmpHasNullValue = STL_FALSE;
                    break;
                }

                STL_RAMP( RAMP_NEXT_COMP );

                sLeftValueBlock  = sLeftValueBlock->mNext;
                sRightValueBlock = sRightValueBlock->mNext;
                sValueBlockList  = sValueBlockList->mNext;
            }

            if( sTmpHasNullValue == STL_TRUE )
            {
                sHasNullValue = STL_TRUE;
            }
            else
            {
                if( sExeSubQueryFunc->mResult[ sCompResult ] == STL_TRUE )
                {
                    DTL_BOOLEAN( sResultDataValue ) = STL_TRUE;
                    DTL_SET_BOOLEAN_LENGTH( sResultDataValue );
                    STL_THROW( RAMP_FINISH );
                }
            }
        }

        
        /**
         * Fetch Right Operand Node
         */
    
        if( sEOF == STL_TRUE )
        {
            break;
        }

        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        /* Fetch Next */
        QLNX_FETCH_NODE( & sFetchNodeInfo,
                         sOptSubQueryFunc->mRightOperandNode->mIdx,
                         0, /* Start Idx */
                         0,
                         QLL_FETCH_COUNT_MAX,
                         & sUsedBlockCnt,
                         & sEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );
    }


    /**
     * Sub Query Function Result 설정
     */
    
    if( sHasNullValue == STL_TRUE )
    {
        DTL_SET_NULL( sResultDataValue );
    }
    else
    {
        DTL_BOOLEAN( sResultDataValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( sResultDataValue );
    }


    /*****************************************
     * FINISH
     ****************************************/

    STL_RAMP( RAMP_FINISH );

    
    /**
     * ALL quantifier에 의해 호출된 경우 Result를 반전시킨다.
     */
    
    if( sExeSubQueryFunc->mIsResReverse == STL_TRUE )
    {
        if( DTL_IS_NULL( sResultDataValue ) != STL_TRUE )
        {
            if( DTL_BOOLEAN( sResultDataValue ) == STL_TRUE )
            {
                DTL_BOOLEAN( sResultDataValue ) = STL_FALSE;
            }
            else
            {
                DTL_BOOLEAN( sResultDataValue ) = STL_TRUE;
            }
        }
    }

    
    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSubQueryFunc->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Quantifier Comparison function을 수행한다. (ANY, Right Node가 Materialize되어 있음)
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aIdx          Result Value Block Idx
 * @param[in]      aEnv          Environment
 */
stlStatus qlnxSubQueryFuncCompAnyWithInstantExact( smlTransId              aTransID,
                                                   smlStatement          * aStmt,
                                                   qllDBCStmt            * aDBCStmt,
                                                   qllStatement          * aSQLStmt,
                                                   qllOptimizationNode   * aOptNode,
                                                   qllDataArea           * aDataArea,
                                                   stlInt32                aIdx,
                                                   qllEnv                * aEnv )
{
    qllExecutionNode                * sExecNode        = NULL;
//    qlnoSubQueryFunc                * sOptSubQueryFunc = NULL;
    qlnxSubQueryFunc                * sExeSubQueryFunc = NULL;
    qlnInstantTable                 * sInstantTable    = NULL;
    qlnxListFuncInstantFetchInfo    * sFetchInfo       = NULL;  
        
    knlValueBlockList      * sLeftValueBlock  = NULL;
    knlValueBlockList      * sRightValueBlock = NULL;
    knlCompareList         * sCompList        = NULL;
    
    dtlDataValue           * sLeftDataValue   = NULL;
    dtlDataValue           * sRightDataValue  = NULL;
    dtlDataValue           * sResultDataValue = NULL;
    dtlCompareResult         sCompResult      = DTL_COMPARISON_EQUAL;
    stlInt64                 sUsedBlockCnt    = 0;
    stlInt32                 sKeyColCnt       = 0;
    stlInt32                 sLoop            = 0;
    stlInt32                 sIdx             = 0;
    stlBool                  sIsNullResult    = STL_FALSE;
    stlBool                  sTmpHasNullValue = STL_FALSE;
    
    
    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );

    
    /*****************************************
     * 기본 정보 획득
     ****************************************/

    /**
     * SUB-QUERY FUNCTION Optimization Node 획득
     */

//    sOptSubQueryFunc = (qlnoSubQueryFunc *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * SUB-QUERY FUNCTION Execution Node 획득
     */

    sExeSubQueryFunc = sExecNode->mExecNode;

    
    /**
     * Result Data Value 얻기
     */
    
    sResultDataValue = KNL_GET_BLOCK_DATA_VALUE( sExeSubQueryFunc->mResultBlock, aIdx );


    /**
     * Instant Table 얻기
     */

    STL_DASSERT( sExeSubQueryFunc->mListFuncInstantFetchInfo != NULL );

    sFetchInfo    = sExeSubQueryFunc->mListFuncInstantFetchInfo;
    sInstantTable = sFetchInfo->mInstantTable;

    STL_DASSERT( sExeSubQueryFunc->mNeedMaterialize == STL_TRUE );
    STL_DASSERT( sExeSubQueryFunc->mIsOrdering == STL_FALSE );
    
    
    /*****************************************
     * Node Fetch 이전 평가
     ****************************************/

    /**
     * Fetch 이전 평가
     */
    
    if( sInstantTable->mRecCnt == 0 )
    {
        DTL_BOOLEAN( sResultDataValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( sResultDataValue );

        STL_THROW( RAMP_FINISH );
    }
    else
    {
        if( sExeSubQueryFunc->mValueBlockCnt == 1 )
        {
            sLeftDataValue = KNL_GET_BLOCK_DATA_VALUE( sExeSubQueryFunc->mLeftBlockList, aIdx );
            
            if( DTL_IS_NULL( sLeftDataValue ) == STL_TRUE )
            {
                DTL_SET_NULL( sResultDataValue );
                STL_THROW( RAMP_FINISH );
            }
        }

        
        /**
         * Range Compare List의 constant value 정보 갱신
         */

        if( sFetchInfo->mMinRange != NULL )
        {
            sLeftValueBlock = sExeSubQueryFunc->mLeftBlockList;
        
            for( sLoop = 0 ; sLoop < sExeSubQueryFunc->mValueBlockCnt ; sLoop++ )
            {
                sCompList = & sFetchInfo->mMinRange[ sLoop ];

                sCompList->mConstVal = KNL_GET_BLOCK_DATA_VALUE( sLeftValueBlock, aIdx );

                sCompList->mRangeVal = sCompList->mConstVal->mValue;
                sCompList->mRangeLen = sCompList->mConstVal->mLength;

                sLeftValueBlock = sLeftValueBlock->mNext;
            }
        }

        if( sFetchInfo->mMaxRange != NULL )
        {
            sLeftValueBlock = sExeSubQueryFunc->mLeftBlockList;
        
            for( sLoop = 0 ; sLoop < sExeSubQueryFunc->mValueBlockCnt ; sLoop++ )
            {
                sCompList = & sFetchInfo->mMaxRange[ sLoop ];

                sCompList->mConstVal = KNL_GET_BLOCK_DATA_VALUE( sLeftValueBlock, aIdx );

                sCompList->mRangeVal = sCompList->mConstVal->mValue;
                sCompList->mRangeLen = sCompList->mConstVal->mLength;

                sLeftValueBlock = sLeftValueBlock->mNext;
            }
        }
        
        sLeftValueBlock = sExeSubQueryFunc->mLeftBlockList;

        while( sLeftValueBlock != NULL )
        {
            sLeftDataValue = KNL_GET_BLOCK_DATA_VALUE( sLeftValueBlock, aIdx );
            
            sKeyColCnt++;
            
            if( DTL_IS_NULL( sLeftDataValue ) == STL_TRUE )
            {
                STL_THROW( RAMP_SKIP_DEFAULT_CASE );
            }
            
            sLeftValueBlock = sLeftValueBlock->mNext;
        }
    }

    if( ( sFetchInfo->mMinRange == NULL ) &&
        ( sFetchInfo->mMaxRange == NULL ) )
    {
        STL_THROW( RAMP_SKIP_DEFAULT_CASE );
    }
    

    /*****************************************
     * Default Range Scan 수행하여 List Function 평가
     ****************************************/

    /**
     * range 재조정
     */

    STL_DASSERT( sKeyColCnt > 0 );

    qlnxRebuildSubQueryFuncFetchInfo( sFetchInfo,
                                      NULL,
                                      sKeyColCnt,
                                      1 );


    /**
     * Default Range Scan 수행
     */

    STL_DASSERT( sFetchInfo->mInstantTable->mIterator != NULL );

    STL_TRY( smlResetIterator( sFetchInfo->mInstantTable->mIterator,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );
        
    /* fetch records */
    STL_TRY( smlFetchNext( sFetchInfo->mInstantTable->mIterator,
                           sFetchInfo->mFetchInfo,
                           SML_ENV(aEnv) )
             == STL_SUCCESS );

    sUsedBlockCnt = sFetchInfo->mFetchInfo->mRowBlock->mUsedBlockSize;


    /**
     * Default Range Scan 결과 평가
     */

    if( sUsedBlockCnt > 0 )
    {
        DTL_BOOLEAN( sResultDataValue ) = STL_TRUE;
        DTL_SET_BOOLEAN_LENGTH( sResultDataValue );
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Do Nothing */
    }

    
    /*****************************************
     * Range 재조정하여 List Function 평가
     ****************************************/

    STL_RAMP( RAMP_SKIP_DEFAULT_CASE );

    
    /**
     * range 재조정
     */

    qlnxRebuildSubQueryFuncFetchInfo( sFetchInfo,
                                      sExeSubQueryFunc->mRightBlockList,
                                      0,
                                      QLL_FETCH_COUNT_MAX );
    

    /**
     * List Function 평가
     */

    STL_TRY( smlResetIterator( sFetchInfo->mInstantTable->mIterator,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );
        
    /* fetch records */
    STL_TRY( smlFetchNext( sFetchInfo->mInstantTable->mIterator,
                           sFetchInfo->mFetchInfo,
                           SML_ENV(aEnv) )
             == STL_SUCCESS );

    sUsedBlockCnt = sFetchInfo->mFetchInfo->mRowBlock->mUsedBlockSize;

    sIsNullResult = STL_FALSE;

    while( sUsedBlockCnt > 0 )
    {
        for( sLoop = 0 ; sLoop < sUsedBlockCnt ; sLoop++ )
        {
            sLeftValueBlock  = sExeSubQueryFunc->mLeftBlockList;
            sRightValueBlock = sExeSubQueryFunc->mRightBlockList;

            for( sIdx = 0 ; sLeftValueBlock != NULL ; sIdx++ )
            {
                STL_DASSERT( sRightValueBlock != NULL );

                sLeftDataValue = KNL_GET_BLOCK_DATA_VALUE( sLeftValueBlock, aIdx );

                if( DTL_IS_NULL( sLeftDataValue ) == STL_TRUE )
                {
                    sTmpHasNullValue = STL_TRUE;
                    STL_THROW( RAMP_NEXT_COMP );
                }

                sRightDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sRightValueBlock, sLoop );

                if( DTL_IS_NULL( sRightDataValue ) == STL_TRUE )
                {
                    sTmpHasNullValue = STL_TRUE;
                    STL_THROW( RAMP_NEXT_COMP );
                }

                sCompResult = sExeSubQueryFunc->mCompFunc[ sIdx ]( sLeftDataValue->mValue,
                                                                   sLeftDataValue->mLength,
                                                                   sRightDataValue->mValue,
                                                                   sRightDataValue->mLength );
                sCompResult++;
                
                if( sExeSubQueryFunc->mIsCheckNext[ sCompResult ] == STL_FALSE )
                {
                    sTmpHasNullValue = STL_FALSE;
                    break;
                }

                STL_RAMP( RAMP_NEXT_COMP );
                
                sLeftValueBlock  = sLeftValueBlock->mNext;
                sRightValueBlock = sRightValueBlock->mNext;
            }

            if( sTmpHasNullValue == STL_TRUE )
            {
                sIsNullResult = STL_TRUE;
            }
            else
            {
                if( sExeSubQueryFunc->mResult[ sCompResult ] == STL_TRUE )
                {
                    DTL_BOOLEAN( sResultDataValue ) = STL_TRUE;
                    DTL_SET_BOOLEAN_LENGTH( sResultDataValue );
                    STL_THROW( RAMP_FINISH );
                }
            }
        }

        if( sFetchInfo->mFetchInfo->mIsEndOfScan == STL_TRUE )
        {
            break;
        }

        /* fetch records */
        STL_TRY( smlFetchNext( sFetchInfo->mInstantTable->mIterator,
                               sFetchInfo->mFetchInfo,
                               SML_ENV(aEnv) )
                 == STL_SUCCESS );

        sUsedBlockCnt = sFetchInfo->mFetchInfo->mRowBlock->mUsedBlockSize;
    }
    
    
    /**
     * Sub Query Function Result 설정
     */

    if( sIsNullResult == STL_FALSE )
    {
        DTL_BOOLEAN( sResultDataValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( sResultDataValue );
    }
    else
    {
        DTL_SET_NULL( sResultDataValue );
    }
    

    /*****************************************
     * FINISH
     ****************************************/

    STL_RAMP( RAMP_FINISH );


    /**
     * ALL quantifier에 의해 호출된 경우 Result를 반전시킨다.
     */
    
    if( sExeSubQueryFunc->mIsResReverse == STL_TRUE )
    {
        if( DTL_IS_NULL( sResultDataValue ) != STL_TRUE )
        {
            if( DTL_BOOLEAN( sResultDataValue ) == STL_TRUE )
            {
                DTL_BOOLEAN( sResultDataValue ) = STL_FALSE;
            }
            else
            {
                DTL_BOOLEAN( sResultDataValue ) = STL_TRUE;
            }
        }
    }

    
    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeSubQueryFunc->mCommonInfo.mFetchTime, sFetchTime );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief List Function 평가 대상 탐색을 위한 Fetch 정보를 갱신한다.
 * @param[in,out]  aListFuncInstantFetchInfo    Instant를 이용한 List Function Fetch Info
 * @param[in]      aReadColList                 Read Column List
 * @param[in]      aKeyColCnt                   Key Column Count
 * @param[in]      aFetchCnt                    Fetch Record Count
 */
void qlnxRebuildSubQueryFuncFetchInfo( qlnxListFuncInstantFetchInfo * aListFuncInstantFetchInfo,
                                       knlValueBlockList            * aReadColList,
                                       stlInt32                       aKeyColCnt,
                                       stlInt64                       aFetchCnt )
{
    stlInt32  sLoop = 0;
    
    STL_DASSERT( aListFuncInstantFetchInfo != NULL );
    STL_DASSERT( aListFuncInstantFetchInfo->mFetchInfo != NULL );
    STL_DASSERT( aFetchCnt > 0 );

    if( aKeyColCnt == 0 )
    {
        aListFuncInstantFetchInfo->mFetchInfo->mRange->mMinRange = NULL;
        aListFuncInstantFetchInfo->mFetchInfo->mRange->mMaxRange = NULL;
    }
    else
    {
        /* Min range */
        STL_DASSERT( aListFuncInstantFetchInfo->mMinRange != NULL );

        for( sLoop = 1 ; sLoop < aKeyColCnt ; sLoop++ )
        {
            aListFuncInstantFetchInfo->mMinRange[ sLoop - 1 ].mNext =
                & aListFuncInstantFetchInfo->mMinRange[ sLoop ];
        }
        aListFuncInstantFetchInfo->mMinRange[ sLoop - 1 ].mNext = NULL;
        
        aListFuncInstantFetchInfo->mFetchInfo->mRange->mMinRange =
            aListFuncInstantFetchInfo->mMinRange;

        /* Max range */
        STL_DASSERT( aListFuncInstantFetchInfo->mMaxRange != NULL );

        for( sLoop = 1 ; sLoop < aKeyColCnt ; sLoop++ )
        {
            aListFuncInstantFetchInfo->mMaxRange[ sLoop - 1 ].mNext =
                & aListFuncInstantFetchInfo->mMaxRange[ sLoop ];
        }
        aListFuncInstantFetchInfo->mMaxRange[ sLoop - 1 ].mNext = NULL;
        
        aListFuncInstantFetchInfo->mFetchInfo->mRange->mMaxRange =
            aListFuncInstantFetchInfo->mMaxRange;
    }
    
    aListFuncInstantFetchInfo->mFetchInfo->mRange->mNext = NULL;
    
    /* Read Column List */
    aListFuncInstantFetchInfo->mFetchInfo->mColList = aReadColList;

    /* Fetch Count */
    aListFuncInstantFetchInfo->mFetchInfo->mFetchCnt = aFetchCnt;
}


/** @} qlnx */
