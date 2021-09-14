/*******************************************************************************
 * qlnxInsertStmt.c
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
 * @file qlnxInsertStmt.c
 * @brief SQL Processor Layer Execution - INSERT STATEMENT Node
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnx
 * @{
 */


/**
 * @brief INSERT STATEMENT node를 초기화한다.
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
stlStatus qlnxInitializeInsertStmt( smlTransId              aTransID,
                                    smlStatement          * aStmt,
                                    qllDBCStmt            * aDBCStmt,
                                    qllStatement          * aSQLStmt,
                                    qllOptimizationNode   * aOptNode,
                                    qllDataArea           * aDataArea,
                                    qllEnv                * aEnv )
{
    /* qllExecutionNode  * sExecNode = NULL; */
    qlnoInsertStmt    * sOptInsertStmt = NULL;
    /* qlnxInsertStmt    * sExeInsertStmt = NULL; */
    
    qllOptNodeItem    * sOptNodeItem = NULL;
    stlInt16            sNodeCnt = 0;
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_INSERT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * INSERT STATEMENT Optimization Node 획득
     */

    sOptInsertStmt = (qlnoInsertStmt *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    /* sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, */
    /*                                     QLL_GET_OPT_NODE_IDX( aOptNode ) ); */


    /**
     * INSERT STATEMENT Execution Node 획득
     */

    /* sExeInsertStmt = (qlnxInsertStmt *) sExecNode->mExecNode; */


    /******************************************************
     * 하위 Node들 Initialize 수행
     ******************************************************/

    sOptNodeItem = sOptInsertStmt->mOptNodeList->mTail;
    
    for( sNodeCnt = 1 ; /* Statement 제외한 노드들에 대해 Execute 수행 */
         sNodeCnt < aDataArea->mExecNodeCnt ;
         sNodeCnt++ )
    {
        QLNX_INITIALIZE_NODE( aTransID,
                              aStmt,
                              aDBCStmt,
                              aSQLStmt,
                              sOptNodeItem->mNode,
                              aDataArea,
                              aEnv );
        sOptNodeItem = sOptNodeItem->mPrev;
    }
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief INSERT STATEMENT node를 수행한다.
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
stlStatus qlnxExecuteInsertStmt( smlTransId              aTransID,
                                 smlStatement          * aStmt,
                                 qllDBCStmt            * aDBCStmt,
                                 qllStatement          * aSQLStmt,
                                 qllOptimizationNode   * aOptNode,
                                 qllDataArea           * aDataArea,
                                 qllEnv                * aEnv )
{
    STL_DASSERT( aOptNode->mOptNode != NULL );
    
    if( ((qlnoInsertStmt *) aOptNode->mOptNode)->mHasQuery == STL_FALSE )
    {
        return qlnxExecuteInsertValues( aTransID,
                                        aStmt,
                                        aDBCStmt,
                                        aSQLStmt,
                                        aOptNode,
                                        aDataArea,
                                        aEnv );
    }
    else
    {
        return qlnxExecuteInsertSelect( aTransID,
                                        aStmt,
                                        aDBCStmt,
                                        aSQLStmt,
                                        aOptNode,
                                        aDataArea,
                                        aEnv );
    }
}


/**
 * @brief INSERT VALUES STATEMENT 를 수행한다.
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
stlStatus qlnxExecuteInsertValues( smlTransId              aTransID,
                                   smlStatement          * aStmt,
                                   qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode = NULL;
    qlnoInsertStmt        * sOptInsertStmt = NULL;
    qlnxInsertStmt        * sExeInsertStmt = NULL;
    
    knlValueBlockList     * sInsertBlock = NULL;
//    qlvInitExpression     * sInsertValueExpr = NULL;
    stlInt32                sRowIdx = 0;
    stlInt32                sColumnIdx = 0;
    knlExprEvalInfo         sExprEvalInfo;

    stlInt32                sParamArraySize = 0;
    stlInt32                sArrayIdx       = 0;
    stlInt32                sBlockCount     = 0;
    stlInt32                sBlockIdx       = 0;

    stlBool                 sEOF           = STL_FALSE;
    stlBool                 sSuqbQueryEOF  = STL_FALSE;
    stlInt64                sUsedBlockCnt = 0;
    stlInt64                sTmpUsedBlockCnt;

    qlnxFetchNodeInfo       sFetchNodeInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_INSERT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );


    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * INSERT STATEMENT Optimization Node 획득
     */

    sOptInsertStmt = (qlnoInsertStmt *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * INSERT STATEMENT Execution Node 획득
     */

    sExeInsertStmt = sExecNode->mExecNode;

    
    /**
     * 기본 정보 설정
     */

    aDataArea->mWriteRowCnt = 0;
    sParamArraySize = knlGetExecuteValueBlockSize( aSQLStmt->mBindContext );

    sBlockCount = sOptInsertStmt->mInsertRowCount * sParamArraySize;

    /**
     * Constraint Check Time 획득,
     */
    
    STL_TRY( qlxInsertGetConstraintCheckTime( sOptInsertStmt->mRelObject,
                                              aOptNode,
                                              aDataArea,
                                              aEnv )
             == STL_SUCCESS );
    
    /**
     * Fetch Node Info 설정
     */
    
    QLNX_INIT_FETCH_NODE_INFO( & sFetchNodeInfo,
                               aTransID,
                               aStmt,
                               aDBCStmt,
                               aSQLStmt,
                               aDataArea );


    /******************************************************
     * Statement 단위 Pseudo Column 평가
     ******************************************************/
    
    STL_DASSERT( sOptInsertStmt->mInsertRowCount == aDataArea->mBlockAllocCnt );

    /* /\** */
    /*  * Pseudo Column 수행 */
    /*  *\/ */

    /* if( sOptInsertStmt->mStmtExprList->mInitExprList->mPseudoColExprList->mCount > 0 ) */
    /* { */
    /*     /\* STATEMENT 단위 Pseudo Column 수행 *\/ */
    /*     STL_TRY( qlxExecutePseudoColumnBlock( */
    /*                  aTransID, */
    /*                  aStmt, */
    /*                  DTL_ITERATION_TIME_FOR_EACH_STATEMENT, */
    /*                  sOptInsertStmt->mTableHandle, */
    /*                  &sExeInsertStmt->mRowBlock, */
    /*                  sOptInsertStmt->mStmtExprList->mInitExprList->mPseudoColExprList, */
    /*                  sExeInsertStmt->mTotalPseudoColBlock, */
    /*                  0, */
    /*                  sBlockCount, */
    /*                  aEnv ) */
    /*              == STL_SUCCESS ); */
    /* } */
    /* else */
    /* { */
    /*     /\* nothing to do *\/ */
    /* } */


    /******************************************************
     * CONSTANT EXPRESSION STACK 평가 
     ******************************************************/

    /**
     * FOR_EACH_STMT인 iteration time을 가지는 Pseudo Column 수행 후 &
     * statement 내의 어떠한 expression stack을 평가하기 전에 수행
     */

    STL_DASSERT( aDataArea->mTransID == aTransID );
    
    STL_TRY( qlxEvaluateConstExpr( sOptInsertStmt->mStmtExprList->mConstExprStack,
                                   aDataArea,
                                   KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    sEOF = STL_FALSE;


    /******************************************************
     * 하위 Node Execute 수행
     ******************************************************/

    if( sOptInsertStmt->mInsertValueChildNode != NULL )
    {
        QLNX_EXECUTE_NODE( aTransID,
                           aStmt,
                           aDBCStmt,
                           aSQLStmt,
                           aDataArea->mExecNodeList[ sOptInsertStmt->mInsertValueChildNode->mIdx ].mOptNode,
                           aDataArea,
                           aEnv );

        sEOF = STL_FALSE;
        QLNX_FETCH_NODE( & sFetchNodeInfo,
                         sOptInsertStmt->mInsertValueChildNode->mIdx,
                         0, /* Start Idx */
                         0,
                         aDataArea->mBlockAllocCnt,
                         & sUsedBlockCnt,
                         & sEOF,
                         aEnv );
    }


    /******************************************************
     * INSERT VALUES 평가 
     ******************************************************/

    /**
     * Insert Values에 대한 Value Expression 을 수행
     */
    
    sInsertBlock = aDataArea->mWriteColumnBlock->mHead->mColumnValueBlock;

    sExprEvalInfo.mContext    = (knlExprContextInfo *) aDataArea->mExprDataContext;
    sExprEvalInfo.mBlockCount = 1;

    if( sOptInsertStmt->mQueryExprList->mInitExprList->mPseudoColExprList->mCount > 0 )
    {
        /* RESULT Pseudo Column 수행 */
        STL_TRY( qlxExecutePseudoColumnBlock(
                     aTransID,
                     aStmt,
                     DTL_ITERATION_TIME_FOR_EACH_EXPR,
                     sOptInsertStmt->mTableHandle,
                     &sExeInsertStmt->mRowBlock,
                     sOptInsertStmt->mQueryExprList->mInitExprList->mPseudoColExprList,
                     sExeInsertStmt->mQueryPseudoColBlock,
                     0,
                     sBlockCount,
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* nothing to do */
    }

    for( sColumnIdx = 0 ; sColumnIdx < sOptInsertStmt->mInsertColumnCnt ; sColumnIdx++ )
    {
        if ( ellGetColumnUnused( & sOptInsertStmt->mColumnHandle[sColumnIdx] ) == STL_TRUE )
        {
            /**
             * UNUSED Column 임
             */
        }
        else
        {
//            sInsertValueExpr = sOptInsertStmt->mInsertValueExpr[ sColumnIdx ];

            for( sRowIdx = 0 ; sRowIdx < sOptInsertStmt->mInsertRowCount ; sRowIdx++ )
            {
                sExprEvalInfo.mExprStack   =
                    (knlExprStack *) & sOptInsertStmt->mInsertValueStackList[ sRowIdx ][ sColumnIdx ];
                sExprEvalInfo.mResultBlock = sInsertBlock;
                
                for( sArrayIdx = (sRowIdx * sParamArraySize);
                     sArrayIdx < (sParamArraySize + (sRowIdx * sParamArraySize));
                     sArrayIdx++ )
                {
                    sExprEvalInfo.mBlockIdx = sArrayIdx;

                    STL_TRY( knlEvaluateOneExpression( & sExprEvalInfo, 
                                                       KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                }
            }
        }

        sInsertBlock = sInsertBlock->mNext;
    }
    
    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aDataArea->mWriteColumnBlock->mHead->mColumnValueBlock,
                                         0, 
                                         sBlockCount );

    /******************************************************
     * 테이블에 Row 들을 추가 
     ******************************************************/
    
    /**
     * Row 추가
     */

    STL_TRY( smlInsertRecord( aStmt,
                              sOptInsertStmt->mPhysicalHandle,
                              aDataArea->mWriteColumnBlock->mHead->mColumnValueBlock,
                              NULL, /* unique violation */
                              & sExeInsertStmt->mRowBlock,
                              SML_ENV(aEnv) )
             == STL_SUCCESS );

    aDataArea->mWriteRowCnt += sBlockCount;

    
    /******************************************************
     * Check 무결성 검사 
     ******************************************************/
    
    /**
     * Check Constraint 검사
     */

    if( sOptInsertStmt->mHasCheck == STL_TRUE )
    {
        STL_TRY( qlxInsertIntegrityCheckConst( aTransID,
                                               aSQLStmt,
                                               sOptInsertStmt->mTableHandle,
                                               sOptInsertStmt->mRelObject,
                                               & sExeInsertStmt->mRelObject,
                                               aEnv )
             == STL_SUCCESS );
    }
    else
    {
        /* check constraint 가 없음 */
    }

    /******************************************************
     * Key 무결성 검사 및 Index Key 추가 
     ******************************************************/

    if( sOptInsertStmt->mHasIndex == STL_TRUE )
    {
        STL_TRY( qlxInsertIntegrityKeyConstIndexKey( aTransID,
                                                     aStmt,
                                                     sOptInsertStmt->mTableHandle,
                                                     sOptInsertStmt->mRelObject,
                                                     & sExeInsertStmt->mRelObject,
                                                     & sExeInsertStmt->mRowBlock,
                                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* index 가 없음 */
    }

    
    /******************************************************
     * RETURN 절 수행
     ******************************************************/

    /** 
     * 수행후, Returning절에 올 수 있는 RowId Pseudo Column 수행
     */

    if( sOptInsertStmt->mRowIdColumn != NULL )
    {
        STL_TRY( qlxExecuteRowIdColumnBlock(
                     aTransID,
                     aStmt,
                     sOptInsertStmt->mTableHandle,
                     & sExeInsertStmt->mRowBlock,
                     sOptInsertStmt->mRowIdColumn,
                     sExeInsertStmt->mRowIdColBlock,
                     0,
                     aDataArea->mWriteRowCnt,
                     aEnv )
                 == STL_SUCCESS );

        KNL_SET_ONE_BLOCK_USED_CNT( sExeInsertStmt->mRowIdColBlock,
                                    sBlockCount );
    }
    else
    {
        /* Do Nothing */
    }


    /** 
     * Return Expression 수행 
     */

    if ( sBlockCount > 0 )
    {
        switch ( aSQLStmt->mStmtType )
        {
            case QLL_STMT_INSERT_TYPE:
                {
                    /**
                     * Nothing To Do
                     */
                    break;
                }
            case QLL_STMT_INSERT_RETURNING_QUERY_TYPE:
                {
                    if( sOptInsertStmt->mReturnChildNode != NULL )
                    {
                        knlValueBlockList  * sOuterColBlock;
                        knlValueBlockList  * sOuterOrgColBlock;
                        knlValueBlockList  * sRefColBlock;
                        qlnxSubQueryList   * sExeSubQueryList;
                        stlInt32             sLoop;
    
                        for ( sBlockIdx = 0; sBlockIdx < sBlockCount; sBlockIdx++ )
                        {
                            /* Outer Column Block Value 설정 */
                            if( sExeInsertStmt->mReturnOuterColBlock != NULL )
                            {
                                STL_DASSERT( sExeInsertStmt->mReturnOuterOrgColBlock != NULL );

                                sOuterColBlock    = sExeInsertStmt->mReturnOuterColBlock;
                                sOuterOrgColBlock = sExeInsertStmt->mReturnOuterOrgColBlock;

                                while( sOuterColBlock != NULL )
                                {
                                    STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_DATA_VALUE( sOuterOrgColBlock, sBlockIdx ),
                                                               KNL_GET_BLOCK_FIRST_DATA_VALUE( sOuterColBlock ),
                                                               KNL_ENV( aEnv ) )
                                             == STL_SUCCESS );

                                    sOuterColBlock    = sOuterColBlock->mNext;
                                    sOuterOrgColBlock = sOuterOrgColBlock->mNext;
                                }
                            }

                            /* Return SubQuery Fetch 수행 */
                            QLNX_EXECUTE_NODE( aTransID,
                                               aStmt,
                                               aDBCStmt,
                                               aSQLStmt,
                                               aDataArea->mExecNodeList[ sOptInsertStmt->mReturnChildNode->mIdx ].mOptNode,
                                               aDataArea,
                                               aEnv );

                            sSuqbQueryEOF = STL_FALSE;
                            QLNX_FETCH_NODE( & sFetchNodeInfo,
                                             sOptInsertStmt->mReturnChildNode->mIdx,
                                             0, /* Start Idx */
                                             0,
                                             aDataArea->mBlockAllocCnt,
                                             & sTmpUsedBlockCnt,
                                             & sSuqbQueryEOF,
                                             aEnv );

                            sExeSubQueryList = QLL_GET_EXECUTION_NODE( aDataArea,
                                                                       sOptInsertStmt->mReturnChildNode->mIdx )->mExecNode;

                            if( sBlockIdx > 0 )
                            {
                                for( sLoop = 0 ; sLoop < sExeSubQueryList->mSubQueryCnt ; sLoop++ )
                                {
                                    sRefColBlock = sExeSubQueryList->mRefColBlockList[ sLoop ];

                                    while( sRefColBlock != NULL )
                                    {
                                        if( DTL_GET_BLOCK_IS_SEP_BUFF( sRefColBlock ) == STL_TRUE )
                                        {
                                            STL_TRY( knlCopyDataValue(
                                                         KNL_GET_BLOCK_FIRST_DATA_VALUE( sRefColBlock ),
                                                         KNL_GET_BLOCK_DATA_VALUE( sRefColBlock, sBlockIdx ),
                                                         KNL_ENV( aEnv ) )
                                                     == STL_SUCCESS );
                                        }
                                        sRefColBlock = sRefColBlock->mNext;
                                    }
                                }
                            }
                            
                            /* Return Expression 수행 */
                            STL_TRY( qlnxExecuteReturnExprBlock( aDataArea,
                                                                 STL_TRUE, /* aIsReturnQuery */
                                                                 aDataArea->mTargetBlock,
                                                                 sBlockIdx,
                                                                 sOptInsertStmt->mReturnExprCnt,
                                                                 sOptInsertStmt->mReturnExprStack,
                                                                 aEnv )
                                     == STL_SUCCESS );

                            /* Inserted Row 를 Instant Table 에 저장 */
                            STL_TRY( qlnxAddMeetBlockIntoInstantTable( aSQLStmt->mResultSetDesc->mInstantTable,
                                                                       aSQLStmt->mResultSetDesc->mInstantFetchInfo.mRowBlock,
                                                                       aDataArea->mTargetBlock,
                                                                       NULL,  /* aMeetArray */
                                                                       sBlockIdx,
                                                                       sBlockIdx+1,
                                                                       aEnv )
                                     == STL_SUCCESS );
                        }
                    }
                    else
                    {
                        for ( sBlockIdx = 0; sBlockIdx < sBlockCount; sBlockIdx++ )
                        {
                            STL_TRY( qlnxExecuteReturnExprBlock( aDataArea,
                                                                 STL_TRUE, /* aIsReturnQuery */
                                                                 aDataArea->mTargetBlock,
                                                                 sBlockIdx,
                                                                 sOptInsertStmt->mReturnExprCnt,
                                                                 sOptInsertStmt->mReturnExprStack,
                                                                 aEnv )
                                     == STL_SUCCESS );
                        }

                        /**
                         * Inserted Row 를 Instant Table 에 저장 
                         */

                        STL_TRY( qlnxAddMeetBlockIntoInstantTable( aSQLStmt->mResultSetDesc->mInstantTable,
                                                                   aSQLStmt->mResultSetDesc->mInstantFetchInfo.mRowBlock,
                                                                   aDataArea->mTargetBlock,
                                                                   NULL,  /* aMeetArray */
                                                                   0,
                                                                   sBlockCount,
                                                                   aEnv )
                                 == STL_SUCCESS );
                    }
                break;
            }
        case QLL_STMT_INSERT_RETURNING_INTO_TYPE:
            {
                STL_TRY_THROW( sBlockCount == 1, RAMP_ERR_SINGLE_ROW_EXPRESSION );
                
                /* Outer Column Block Value 설정 */
                if( sExeInsertStmt->mReturnOuterColBlock != NULL )
                {
                    knlValueBlockList   * sOuterColBlock    = NULL;
                    knlValueBlockList   * sOuterOrgColBlock = NULL;

                    STL_DASSERT( sExeInsertStmt->mReturnOuterOrgColBlock != NULL );

                    sOuterColBlock    = sExeInsertStmt->mReturnOuterColBlock;
                    sOuterOrgColBlock = sExeInsertStmt->mReturnOuterOrgColBlock;

                    while( sOuterColBlock != NULL )
                    {
                        STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_DATA_VALUE( sOuterOrgColBlock, 0 ),
                                                   KNL_GET_BLOCK_FIRST_DATA_VALUE( sOuterColBlock ),
                                                   KNL_ENV( aEnv ) )
                                 == STL_SUCCESS );

                        sOuterColBlock    = sOuterColBlock->mNext;
                        sOuterOrgColBlock = sOuterOrgColBlock->mNext;
                    }
                }

                /* SubQuery를 위한 Child Node 수행 */
                if( sOptInsertStmt->mReturnChildNode != NULL )
                {
                    QLNX_EXECUTE_NODE( aTransID,
                                       aStmt,
                                       aDBCStmt,
                                       aSQLStmt,
                                       aDataArea->mExecNodeList[ sOptInsertStmt->mReturnChildNode->mIdx ].mOptNode,
                                       aDataArea,
                                       aEnv );

                    sSuqbQueryEOF = STL_FALSE;
                    QLNX_FETCH_NODE( & sFetchNodeInfo,
                                     sOptInsertStmt->mReturnChildNode->mIdx,
                                     0, /* Start Idx */
                                     0,
                                     aDataArea->mBlockAllocCnt,
                                     & sTmpUsedBlockCnt,
                                     & sSuqbQueryEOF,
                                     aEnv );
                }

                STL_TRY( qlnxExecuteReturnExprBlock( aDataArea,
                                                     STL_FALSE, /* aIsReturnQuery */
                                                     aDataArea->mIntoBlock,
                                                     0,
                                                     sOptInsertStmt->mReturnExprCnt,
                                                     sOptInsertStmt->mReturnExprStack,
                                                     aEnv )
                         == STL_SUCCESS );
                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aDataArea->mIntoBlock, 0, 1 );
                
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
        }
    }
    else
    {
        /**
         * nothing to do
         */
    }

    /**********************************************
     * Deferred Constraint 에 대한 정보 등록
     **********************************************/

    STL_TRY( qlxInsertCheckStmtCollision( aTransID,
                                          aStmt,
                                          aDataArea,
                                          sExecNode,
                                          aEnv )
             == STL_SUCCESS );
    
    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_SET_VALUE( sExecNode,
                       QLL_GET_EXECUTION_NODE( aDataArea,
                                               QLL_GET_OPT_NODE_IDX( aOptNode ) ) );
    QLL_OPT_SET_VALUE( sExeInsertStmt, sExecNode->mExecNode );

    QLL_OPT_ADD_ELAPSED_TIME( sExeInsertStmt->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeInsertStmt->mCommonInfo.mFetchCallCount, 1 );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SINGLE_ROW_EXPRESSION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INTO_CLAUSE_MULTIPLE_ROWS,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_SET_VALUE( sExecNode,
                       QLL_GET_EXECUTION_NODE( aDataArea,
                                               QLL_GET_OPT_NODE_IDX( aOptNode ) ) );
    QLL_OPT_SET_VALUE( sExeInsertStmt, sExecNode->mExecNode );

    QLL_OPT_ADD_ELAPSED_TIME( sExeInsertStmt->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeInsertStmt->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief INSERT ... SELECT STATEMENT 를 수행한다.
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
stlStatus qlnxExecuteInsertSelect( smlTransId              aTransID,
                                   smlStatement          * aStmt,
                                   qllDBCStmt            * aDBCStmt,
                                   qllStatement          * aSQLStmt,
                                   qllOptimizationNode   * aOptNode,
                                   qllDataArea           * aDataArea,
                                   qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode = NULL;
    qlnoInsertStmt        * sOptInsertStmt = NULL;
    qlnxInsertStmt        * sExeInsertStmt = NULL;
    

    knlValueBlockList     * sInsertBlock = NULL;
    qlvInitExpression     * sInsertValueExpr = NULL;
    stlInt32                sColumnIdx = 0;
    knlExprEvalInfo         sExprEvalInfo;

    stlInt64                sUsedBlockCnt = 0;
    stlInt64                sTmpUsedBlockCnt;
    stlBool                 sEOF = STL_FALSE;
    stlBool                 sSubQueryEOF = STL_FALSE;

    stlInt32                sBlockIdx = 0;

    qlnxFetchNodeInfo       sFetchNodeInfo;

    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );


    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_INSERT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    QLL_OPT_CHECK_TIME( sBeginFetchTime );


    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * INSERT STATEMENT Optimization Node 획득
     */

    sOptInsertStmt = (qlnoInsertStmt *) aOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * INSERT STATEMENT Execution Node 획득
     */

    sExeInsertStmt = sExecNode->mExecNode;

    
    /**
     * 기본 정보 설정
     */

    aDataArea->mWriteRowCnt = 0;

    /**
     * Constraint Check Time 획득,
     */
    
    STL_TRY( qlxInsertGetConstraintCheckTime( sOptInsertStmt->mRelObject,
                                              aOptNode,
                                              aDataArea,
                                              aEnv )
             == STL_SUCCESS );
    
    /**
     * Fetch Node Info 설정
     */
    
    QLNX_INIT_FETCH_NODE_INFO( & sFetchNodeInfo,
                               aTransID,
                               aStmt,
                               aDBCStmt,
                               aSQLStmt,
                               aDataArea );


    /******************************************************
     * Statement 단위 Pseudo Column 평가
     ******************************************************/
    
    /* /\** */
    /*  * Pseudo Column 수행 */
    /*  *\/ */

    /* if( sOptInsertStmt->mStmtExprList->mInitExprList->mPseudoColExprList->mCount > 0 ) */
    /* { */
    /*     /\* STATEMENT 단위 Pseudo Column 수행 *\/ */
    /*     STL_TRY( qlxExecutePseudoColumnBlock( */
    /*                  aTransID, */
    /*                  aStmt, */
    /*                  DTL_ITERATION_TIME_FOR_EACH_STATEMENT, */
    /*                  sOptInsertStmt->mTableHandle, */
    /*                  &sExeInsertStmt->mRowBlock, */
    /*                  sOptInsertStmt->mStmtExprList->mInitExprList->mPseudoColExprList, */
    /*                  sExeInsertStmt->mTotalPseudoColBlock, */
    /*                  0, */
    /*                  aDataArea->mBlockAllocCnt, */
    /*                  aEnv ) */
    /*              == STL_SUCCESS ); */
    /* } */
    /* else */
    /* { */
    /*     /\* nothing to do *\/ */
    /* } */

    /******************************************************
     * CONSTANT EXPRESSION STACK 평가 
     ******************************************************/

    /**
     * FOR_EACH_STMT인 iteration time을 가지는 Pseudo Column 수행 후 &
     * statement 내의 어떠한 expression stack을 평가하기 전에 수행
     */

    STL_DASSERT( aDataArea->mTransID == aTransID );
    
    STL_TRY( qlxEvaluateConstExpr( sOptInsertStmt->mStmtExprList->mConstExprStack,
                                   aDataArea,
                                   KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /******************************************************
     * 하위 Node들 Execute 수행
     ******************************************************/

    /* Select Query Node */
    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[ sOptInsertStmt->mQueryNode->mIdx ].mOptNode,
                       aDataArea,
                       aEnv );

    /* Insert Values의 SubQuery Node */
    STL_DASSERT( sOptInsertStmt->mInsertValueChildNode == NULL );


    /******************************************************
     * INSERT SELECT 평가 
     ******************************************************/

    /**
     * Fetch Query
     */

    sExprEvalInfo.mContext = (knlExprContextInfo *) aDataArea->mExprDataContext;

    sEOF = STL_FALSE;
    while( sEOF == STL_FALSE )
    {
        QLNX_FETCH_NODE( & sFetchNodeInfo,
                         sOptInsertStmt->mQueryNode->mIdx,
                         0, /* Start Idx */
                         0,
                         QLL_FETCH_COUNT_MAX,
                         & sUsedBlockCnt,
                         & sEOF,
                         aEnv );

        if( sUsedBlockCnt == 0 )
        {
            STL_THROW( RAMP_EXIT );
        }
        else
        {
            /* Do Nothing */
        }


        /**
         * Pseudo Column 수행
         */

        if( sOptInsertStmt->mQueryExprList->mInitExprList->mPseudoColExprList->mCount > 0 )
        {
            /* RESULT Pseudo Column 수행 */
            STL_TRY( qlxExecutePseudoColumnBlock(
                         aTransID,
                         aStmt,
                         DTL_ITERATION_TIME_FOR_EACH_EXPR,
                         sOptInsertStmt->mTableHandle,
                         &sExeInsertStmt->mRowBlock,
                         sOptInsertStmt->mQueryExprList->mInitExprList->mPseudoColExprList,
                         sExeInsertStmt->mQueryPseudoColBlock,
                         0,
                         sUsedBlockCnt,
                         aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* nothing to do */
        }

        
        /**
         * Insert Select에 대한 Value Expression 을 수행
         */

        sInsertBlock = aDataArea->mWriteColumnBlock->mHead->mColumnValueBlock;
        
        for( sColumnIdx = 0 ; sColumnIdx < sOptInsertStmt->mInsertColumnCnt ; sColumnIdx++ )
        {
            if ( ellGetColumnUnused( & sOptInsertStmt->mColumnHandle[sColumnIdx] ) == STL_TRUE )
            {
                /**
                 * UNUSED Column 임
                 */
            }
            else
            {
                /**
                 * @todo Sub-Query Expression 이 오는 경우,
                 *  <BR> Sub-Query Node를 수행하고 Expression Stack을 수행한다.
                 */

                sInsertValueExpr = sOptInsertStmt->mInsertValueExpr[ sColumnIdx ]; 

                if( sInsertValueExpr->mType == QLV_EXPR_TYPE_SUB_QUERY )
                {
                    /* EOF가 나올 때까지 Fetch */
                    /* 1개 이하의 Row 가 반환되는 경우만 성공 (Scalar Value) */
                    /* 0개의 Row 가 반환되는 경우 NULL 값으로 대체 */

                    /* Sub-Query의 Target에 Insert Values의 DataType으로의 변경 정보가 반영되어 있음을 가정 */

                    /* 하위 노드 Fetch */
                    /* STL_TRY( qlnxFetchNode( aTransID, */
                    /*                           aStmt, */
                    /*                           aDBCStmt, */
                    /*                           aSQLStmt, */
                    /*                           sOptNode, */
                    /*                           sDataArea, */
                    /*                           aEnv ) */
                    /*          == STL_SUCCESS ); */
                }
                else
                {
                    /* Do Nothing */
                }
        
                    
                /**
                 * Insert Value Expression에 대한 Block 단위 평가
                 * @todo Block 단위 Cast 연산만 수행하도록 변경
                 */
                    
                sExprEvalInfo.mExprStack       =
                    (knlExprStack *) & sOptInsertStmt->mInsertValueStackList[ 0 ][ sColumnIdx ];
                sExprEvalInfo.mResultBlock     = sInsertBlock;
                sExprEvalInfo.mBlockIdx        = 0;
                sExprEvalInfo.mBlockCount      = sUsedBlockCnt;

                STL_TRY( knlEvaluateBlockExpression( & sExprEvalInfo, 
                                                     KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
            }

            sInsertBlock = sInsertBlock->mNext;
        }

        KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aDataArea->mWriteColumnBlock->mHead->mColumnValueBlock,
                                             0, 
                                             sUsedBlockCnt );

        
        /******************************************************
         * 수행 전 Check 무결성 검사 
         ******************************************************/
    
        /* /\** */
        /*  * PSEUDO COLUMN Expression 을 수행 */
        /*  *\/ */

        /* if( sOptInsertStmt->mQueryExprList->mInitExprList->mPseudoColExprList->mCount > 0 ) */
        /* { */
        /*     /\* RESULT Pseudo Column 수행 *\/ */
        /*     STL_TRY( qlxExecutePseudoColumnBlock( */
        /*                  aTransID, */
        /*                  aStmt, */
        /*                  DTL_ITERATION_TIME_FOR_EACH_EXPR, */
        /*                  sOptInsertStmt->mTableHandle, */
        /*                  &sExeInsertStmt->mRowBlock, */
        /*                  sOptInsertStmt->mQueryExprList->mInitExprList->mPseudoColExprList, */
        /*                  sExeInsertStmt->mQueryPseudoColBlock, */
        /*                  0, */
        /*                  sUsedBlockCnt, */
        /*                  aEnv ) */
        /*              == STL_SUCCESS ); */
        /* } */
        /* else */
        /* { */
        /*     /\* nothing to do *\/ */
        /* } */


        /******************************************************
         * 테이블에 Row 들을 추가 
         ******************************************************/

        /**
         * RETURNING INTO 점검
         */

        if( aSQLStmt->mStmtType == QLL_STMT_INSERT_RETURNING_INTO_TYPE )
        {
            STL_TRY_THROW( ( aDataArea->mWriteRowCnt + sUsedBlockCnt ) == 1,
                           RAMP_ERR_SINGLE_ROW_EXPRESSION );
        }


        /**
         * Row 추가
         */

        STL_TRY( smlInsertRecord( aStmt,
                                  sOptInsertStmt->mPhysicalHandle,
                                  aDataArea->mWriteColumnBlock->mHead->mColumnValueBlock,
                                  NULL, /* unique violation */
                                  & sExeInsertStmt->mRowBlock,
                                  SML_ENV(aEnv) )
                 == STL_SUCCESS );

        aDataArea->mWriteRowCnt += sUsedBlockCnt;

        /******************************************************
         * 수행 후 Check Constraint 검사
         ******************************************************/
        
        if( sOptInsertStmt->mHasCheck == STL_TRUE )
        {
            STL_TRY( qlxInsertIntegrityCheckConst( aTransID,
                                                   aSQLStmt,
                                                   sOptInsertStmt->mTableHandle,
                                                   sOptInsertStmt->mRelObject,
                                                   & sExeInsertStmt->mRelObject,
                                                   aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* check constraint 가 없음 */
        }
    
        /******************************************************
         * 수행 후 Key 무결성 검사 및 Index Key 추가 
         ******************************************************/

        if( sOptInsertStmt->mHasIndex == STL_TRUE )
        {
            STL_TRY( qlxInsertIntegrityKeyConstIndexKey( aTransID,
                                                         aStmt,
                                                         sOptInsertStmt->mTableHandle,
                                                         sOptInsertStmt->mRelObject,
                                                         & sExeInsertStmt->mRelObject,
                                                         & sExeInsertStmt->mRowBlock,
                                                         aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /* index 가 없음 */
        }

        
        /******************************************************
         * RETURN ... INTO 절 평가
         ******************************************************/

        if( sOptInsertStmt->mRowIdColumn != NULL )
        {
            STL_TRY( qlxExecuteRowIdColumnBlock(
                         aTransID,
                         aStmt,
                         sOptInsertStmt->mTableHandle,
                         & sExeInsertStmt->mRowBlock,
                         sOptInsertStmt->mRowIdColumn,
                         sExeInsertStmt->mRowIdColBlock,
                         0,
                         sUsedBlockCnt,
                         aEnv )
                     == STL_SUCCESS );

            KNL_SET_ONE_BLOCK_USED_CNT( sExeInsertStmt->mRowIdColBlock,
                                        sUsedBlockCnt );
        }
        else
        {
            /* Do Nothing */
        }
        
        /** 
         * Return Expression 수행 
         */

        if ( sUsedBlockCnt > 0 )
        {
            switch ( aSQLStmt->mStmtType )
            {
                case QLL_STMT_INSERT_SELECT_TYPE:
                    {
                        /**
                         * Nothing To Do
                         */
                        break;
                    }
                case QLL_STMT_INSERT_RETURNING_QUERY_TYPE:
                    {
                        if( sOptInsertStmt->mReturnChildNode != NULL )
                        {
                            knlValueBlockList  * sOuterColBlock;
                            knlValueBlockList  * sOuterOrgColBlock;
                            knlValueBlockList  * sRefColBlock;
                            qlnxSubQueryList   * sExeSubQueryList;
                            stlInt32             sLoop;

                            for ( sBlockIdx = 0; sBlockIdx < sUsedBlockCnt; sBlockIdx++ )
                            {
                                /* Outer Column Block Value 설정 */
                                if( sExeInsertStmt->mReturnOuterColBlock != NULL )
                                {
                                    STL_DASSERT( sExeInsertStmt->mReturnOuterOrgColBlock != NULL );

                                    sOuterColBlock    = sExeInsertStmt->mReturnOuterColBlock;
                                    sOuterOrgColBlock = sExeInsertStmt->mReturnOuterOrgColBlock;

                                    while( sOuterColBlock != NULL )
                                    {
                                        STL_TRY( knlCopyDataValue(
                                                     KNL_GET_BLOCK_DATA_VALUE(
                                                         sOuterOrgColBlock,
                                                         sBlockIdx ),
                                                     KNL_GET_BLOCK_FIRST_DATA_VALUE( sOuterColBlock ),
                                                     KNL_ENV( aEnv ) )
                                                 == STL_SUCCESS );

                                        sOuterColBlock    = sOuterColBlock->mNext;
                                        sOuterOrgColBlock = sOuterOrgColBlock->mNext;
                                    }
                                }

                                /* Return SubQuery Fetch 수행 */
                                QLNX_EXECUTE_NODE( aTransID,
                                                   aStmt,
                                                   aDBCStmt,
                                                   aSQLStmt,
                                                   aDataArea->mExecNodeList[ sOptInsertStmt->mReturnChildNode->mIdx ].mOptNode,
                                                   aDataArea,
                                                   aEnv );

                                sSubQueryEOF = STL_FALSE;
                                QLNX_FETCH_NODE( & sFetchNodeInfo,
                                                 sOptInsertStmt->mReturnChildNode->mIdx,
                                                 0, /* Start Idx */
                                                 0,
                                                 1,
                                                 & sTmpUsedBlockCnt,
                                                 & sSubQueryEOF,
                                                 aEnv );

                                sExeSubQueryList = QLL_GET_EXECUTION_NODE( aDataArea,
                                                                           sOptInsertStmt->mReturnChildNode->mIdx )->mExecNode;

                                if( sBlockIdx > 0 )
                                {
                                    for( sLoop = 0 ; sLoop < sExeSubQueryList->mSubQueryCnt ; sLoop++ )
                                    {
                                        sRefColBlock = sExeSubQueryList->mRefColBlockList[ sLoop ];

                                        while( sRefColBlock != NULL )
                                        {
                                            if( DTL_GET_BLOCK_IS_SEP_BUFF( sRefColBlock ) == STL_TRUE )
                                            {
                                                STL_TRY( knlCopyDataValue(
                                                             KNL_GET_BLOCK_FIRST_DATA_VALUE( sRefColBlock ),
                                                             KNL_GET_BLOCK_DATA_VALUE( sRefColBlock, sBlockIdx ),
                                                             KNL_ENV( aEnv ) )
                                                         == STL_SUCCESS );
                                            }
                                            sRefColBlock = sRefColBlock->mNext;
                                        }
                                    }
                                }

                                /* Return Expression 수행 */
                                STL_TRY( qlnxExecuteReturnExprBlock( aDataArea,
                                                                     STL_TRUE, /* aIsReturnQuery */
                                                                     aDataArea->mTargetBlock,
                                                                     sBlockIdx,
                                                                     sOptInsertStmt->mReturnExprCnt,
                                                                     sOptInsertStmt->mReturnExprStack,
                                                                     aEnv )
                                         == STL_SUCCESS );

                                /* Inserted Row 를 Instant Table 에 저장 */
                                STL_TRY( qlnxAddMeetBlockIntoInstantTable( aSQLStmt->mResultSetDesc->mInstantTable,
                                                                           aSQLStmt->mResultSetDesc->mInstantFetchInfo.mRowBlock,
                                                                           aDataArea->mTargetBlock,
                                                                           NULL,  /* aMeetArray */
                                                                           sBlockIdx,
                                                                           sBlockIdx+1,
                                                                           aEnv )
                                         == STL_SUCCESS );
                            }
                        }
                        else
                        {
                            for ( sBlockIdx = 0; sBlockIdx < sUsedBlockCnt; sBlockIdx++ )
                            {
                                STL_TRY( qlnxExecuteReturnExprBlock( aDataArea,
                                                                     STL_TRUE, /* aIsReturnQuery */
                                                                     aDataArea->mTargetBlock,
                                                                     sBlockIdx,
                                                                     sOptInsertStmt->mReturnExprCnt,
                                                                     sOptInsertStmt->mReturnExprStack,
                                                                     aEnv )
                                         == STL_SUCCESS );
                            }

                            /**
                             * Inserted Row 를 Instant Table 에 저장 
                             */

                            STL_TRY( qlnxAddMeetBlockIntoInstantTable( aSQLStmt->mResultSetDesc->mInstantTable,
                                                                       aSQLStmt->mResultSetDesc->mInstantFetchInfo.mRowBlock,
                                                                       aDataArea->mTargetBlock,
                                                                       NULL,  /* aMeetArray */
                                                                       0,
                                                                       sUsedBlockCnt,
                                                                       aEnv )
                                     == STL_SUCCESS );
                        }
                    break;
                }
            case QLL_STMT_INSERT_RETURNING_INTO_TYPE:
                {
                    STL_TRY_THROW( sUsedBlockCnt == 1, RAMP_ERR_SINGLE_ROW_EXPRESSION );

                    /* Outer Column Block Value 설정 */
                    if( sExeInsertStmt->mReturnOuterColBlock != NULL )
                    {
                        knlValueBlockList   * sOuterColBlock    = NULL;
                        knlValueBlockList   * sOuterOrgColBlock = NULL;

                        STL_DASSERT( sExeInsertStmt->mReturnOuterOrgColBlock != NULL );

                        sOuterColBlock    = sExeInsertStmt->mReturnOuterColBlock;
                        sOuterOrgColBlock = sExeInsertStmt->mReturnOuterOrgColBlock;

                        while( sOuterColBlock != NULL )
                        {
                            STL_TRY( knlCopyDataValue(
                                         KNL_GET_BLOCK_DATA_VALUE( sOuterOrgColBlock, 0 ),
                                         KNL_GET_BLOCK_FIRST_DATA_VALUE( sOuterColBlock ),
                                         KNL_ENV( aEnv ) )
                                     == STL_SUCCESS );

                            sOuterColBlock    = sOuterColBlock->mNext;
                            sOuterOrgColBlock = sOuterOrgColBlock->mNext;
                        }
                    }

                    /* SubQuery를 위한 Child Node 수행 */
                    if( sOptInsertStmt->mReturnChildNode != NULL )
                    {
                        QLNX_EXECUTE_NODE( aTransID,
                                           aStmt,
                                           aDBCStmt,
                                           aSQLStmt,
                                           aDataArea->mExecNodeList[ sOptInsertStmt->mReturnChildNode->mIdx ].mOptNode,
                                           aDataArea,
                                           aEnv );

                        sSubQueryEOF = STL_FALSE;
                        QLNX_FETCH_NODE( & sFetchNodeInfo,
                                         sOptInsertStmt->mReturnChildNode->mIdx,
                                         0, /* Start Idx */
                                         0,
                                         1,
                                         & sTmpUsedBlockCnt,
                                         & sSubQueryEOF,
                                         aEnv );
                    }

                    STL_TRY( qlnxExecuteReturnExprBlock( aDataArea,
                                                         STL_FALSE, /* aIsReturnQuery */
                                                         aDataArea->mIntoBlock,
                                                         0,
                                                         sOptInsertStmt->mReturnExprCnt,
                                                         sOptInsertStmt->mReturnExprStack,
                                                         aEnv )
                             == STL_SUCCESS );
                    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aDataArea->mIntoBlock, 0, 1 );
                    break;
                }
            default:
                {
                    STL_ASSERT(0);
                    break;
                }
            }
        }
        else
        {
            /**
             * nothing to do
             */
        }
    }

    
    STL_RAMP( RAMP_EXIT );

    /**********************************************
     * Deferred Constraint 에 대한 정보 등록
     **********************************************/

    STL_TRY( qlxInsertCheckStmtCollision( aTransID,
                                          aStmt,
                                          aDataArea,
                                          sExecNode,
                                          aEnv )
             == STL_SUCCESS );
    
    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_SET_VALUE( sExecNode,
                       QLL_GET_EXECUTION_NODE( aDataArea,
                                               QLL_GET_OPT_NODE_IDX( aOptNode ) ) );
    QLL_OPT_SET_VALUE( sExeInsertStmt, sExecNode->mExecNode );

    QLL_OPT_ADD_ELAPSED_TIME( sExeInsertStmt->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeInsertStmt->mCommonInfo.mFetchCallCount, 1 );

    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SINGLE_ROW_EXPRESSION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INTO_CLAUSE_MULTIPLE_ROWS,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_SET_VALUE( sExecNode,
                       QLL_GET_EXECUTION_NODE( aDataArea,
                                               QLL_GET_OPT_NODE_IDX( aOptNode ) ) );
    QLL_OPT_SET_VALUE( sExeInsertStmt, sExecNode->mExecNode );

    QLL_OPT_ADD_ELAPSED_TIME( sExeInsertStmt->mCommonInfo.mFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_COUNT( sExeInsertStmt->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


#if 0
/**
 * @brief INSERT STATEMENT node를 Fetch한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aStartIdx     Start Index
 * @param[in]      aSkipCnt      Skip Record Count
 * @param[in]      aFetchCnt     Fetch Record Count
 * @param[out]     aUsedBlockCnt Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF          End Of Fetch 여부
 * @param[in]      aEnv          Environment
 *
 * @remark INTO 절이 없는 RETURN 절에 대한 Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchInsertStmt( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               stlInt32                aStartIdx,
                               stlInt64                aSkipCnt,
                               stlInt64                aFetchCnt,
                               stlInt64              * aUsedBlockCnt,
                               stlBool               * aEOF,
                               qllEnv                * aEnv )
{
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_INSERT_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /* Do Nothing */
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
#endif


/**
 * @brief INSERT STATEMENT node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeInsertStmt( qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv )
{
//    qllExecutionNode      * sExecNode = NULL;
//    qlnxInsertStmt        * sExeInsertStmt = NULL;

    stlInt16                sNodeCnt = 0;
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_INSERT_TYPE,
                        QLL_ERROR_STACK(aEnv) );


    /**
     * Common Execution Node 획득
     */
        
//    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * INSERT STATEMENT Execution Node 획득
     */

//    sExeInsertStmt = sExecNode->mExecNode;


    /******************************************************
     * 하위 Node들에 대한 Finalize 수행
     ******************************************************/

    for( sNodeCnt = 1 ; /* Statement 제외한 노드들에 대해 Finalize 수행 */
         sNodeCnt < aDataArea->mExecNodeCnt ;
         sNodeCnt++ )
    {
        QLNX_FINALIZE_NODE( aDataArea->mExecNodeList[ sNodeCnt ].mOptNode,
                            aDataArea,
                            aEnv );
    }

    aDataArea->mIsFirstExecution = STL_TRUE;
    

    return STL_SUCCESS;

    STL_FINISH;

    for( sNodeCnt = 1 ; /* Statement 제외한 노드들에 대해 Finalize 수행 */
         sNodeCnt < aDataArea->mExecNodeCnt ;
         sNodeCnt++ )
    {
        QLNX_FINALIZE_NODE_IGNORE_FAILURE( aDataArea->mExecNodeList[ sNodeCnt ].mOptNode,
                                           aDataArea,
                                           aEnv );
    }

    return STL_FAILURE;
}


/** @} qlnx */
