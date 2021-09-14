/*******************************************************************************
 * qlndUpdateStmt.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndUpdateStmt.c 11148 2014-02-06 07:52:37Z jhkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndUpdateStmt.c
 * @brief SQL Processor Layer Data Optimization Node - UPDATE STATEMENT Node
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief UPDATE STATEMENT Execution node에 대한 Data 정보를 구축한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in,out]  aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Data Optimization 단계에서 수행한다.
 *    <BR> Execution Node를 생성하고, Execution을 위한 정보를 구축한다.
 *    <BR> 공간 할당시 Data Plan 메모리 관리자를 이용한다.
 */
stlStatus qlndDataOptimizeUpdateStmt( smlTransId              aTransID,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv )
{
    qllExecutionNode   * sExecNode       = NULL;
    qlnoUpdateStmt     * sOptUpdateStmt  = NULL;
    qlnxUpdateStmt     * sExeUpdateStmt  = NULL;
    qlvInitExpression  * sExpr           = NULL;

    qlvRefExprList     * sReadColList    = NULL;
    qlvRefExprList     * sWriteColList   = NULL;
    qllExecutionNode   * sExecScanNode   = NULL;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_UPDATE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * UPDATE STATEMENT Optimization Node 획득
     */

    sOptUpdateStmt = (qlnoUpdateStmt *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptUpdateStmt->mScanNode,
                        aDataArea,
                        aEnv );

    if( sOptUpdateStmt->mSetChildNode != NULL )
    {
        QLND_DATA_OPTIMIZE( aTransID,
                            aDBCStmt,
                            aSQLStmt,
                            sOptUpdateStmt->mSetChildNode,
                            aDataArea,
                            aEnv );
    }

    if( sOptUpdateStmt->mReturnChildNode != NULL )
    {
        QLND_DATA_OPTIMIZE( aTransID,
                            aDBCStmt,
                            aSQLStmt,
                            sOptUpdateStmt->mReturnChildNode,
                            aDataArea,
                            aEnv );
    }


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * UPDATE STATEMENT Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxUpdateStmt ),
                                (void **) & sExeUpdateStmt,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeUpdateStmt, 0x00, STL_SIZEOF( qlnxUpdateStmt ) );


    /**
     * Pseudo Column List 구성
     */

    if( sOptUpdateStmt->mStmtExprList->mInitExprList->mPseudoColExprList->mCount > 0 )
    {
        /* Total Pseudo Column List 구성 */
        STL_TRY( qlndBuildPseudoColBlockList( aDataArea,
                                              sOptUpdateStmt->mStmtExprList->mInitExprList->mPseudoColExprList,
                                              & sExeUpdateStmt->mTotalPseudoColBlock,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                 == STL_SUCCESS );

        /* Query Pseudo Column List 구성 */
        STL_TRY( qlndBuildPseudoColBlockList( aDataArea,
                                              sOptUpdateStmt->mQueryExprList->mInitExprList->mPseudoColExprList,
                                              & sExeUpdateStmt->mQueryPseudoColBlock,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeUpdateStmt->mTotalPseudoColBlock = NULL;
        sExeUpdateStmt->mQueryPseudoColBlock = NULL;
    }


    /**
     * Constant Expression Result Column 공간 확보
     */
    
    if( sOptUpdateStmt->mStmtExprList->mInitExprList->mConstExprList->mCount > 0 )
    {
        sExpr = sOptUpdateStmt->mStmtExprList->mInitExprList->mConstExprList->mHead->mExprPtr;
        
        STL_TRY( knlInitShareBlockFromBlock(
                     & aDataArea->mConstResultBlock,
                     aDataArea->mExprDataContext->mContexts[ sExpr->mOffset ].mInfo.mValueInfo,
                     & QLL_DATA_PLAN( aDBCStmt ),
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        aDataArea->mConstResultBlock = NULL;
    }


    /**
     * RowId Column을 위한 Value Block List 구성
     */

    if( ((qlvInitBaseTableNode *)sOptUpdateStmt->mWriteTableNode)->mRefRowIdColumn != NULL )
    {
        if( (sOptUpdateStmt->mReadTableNode->mType == QLV_NODE_TYPE_BASE_TABLE) &&
            (((qlvInitBaseTableNode *)sOptUpdateStmt->mReadTableNode)->mRefRowIdColumn != NULL) )
        {
            /**
             * Read / Write RowId Column이 중복으로 처리됨을 방지하기 위해
             * ReadTable의 RowIdColumn Block을 공유한다.
             * 예) UPDAE T1 SET I1 = 1
             *     WHERE ROWID = 'AAAA24AAAAAAACAAAAAiAAK'
             *           -----
             *     RETURNING NEW ROWID INTO :V1;
             *               ---------
             */
            STL_TRY( qlndBuildTableRowIdColBlockList(
                         aDataArea,
                         ((qlvInitBaseTableNode *)sOptUpdateStmt->mReadTableNode)->mRefRowIdColumn,
                         & sExeUpdateStmt->mRowIdColBlock,
                         & QLL_DATA_PLAN( aDBCStmt ),
                         aEnv )
                     == STL_SUCCESS );
            
            STL_DASSERT( sExeUpdateStmt->mRowIdColBlock != NULL );
        }
        else
        {
            /**
             * 예) UPDAE T1 SET I1 = 1
             *     WHERE I2 = 3
             *     RETURNING NEW ROWID INTO :V1;
             *               ---------
             */
            STL_TRY( qlndBuildTableRowIdColBlockList( aDataArea,
                                                      sOptUpdateStmt->mRowIdColumn,
                                                      & sExeUpdateStmt->mRowIdColBlock,
                                                      & QLL_DATA_PLAN( aDBCStmt ),
                                                      aEnv )
                     == STL_SUCCESS );
            
            STL_DASSERT( sExeUpdateStmt->mRowIdColBlock != NULL );
        }
    }
    else
    {
        sExeUpdateStmt->mRowIdColBlock = NULL;
    }
    
    
    /**
     * Update Table에 대한 Read Column List 구성
     */

    if( sOptUpdateStmt->mReadTableNode->mType == QLV_NODE_TYPE_BASE_TABLE )
    {
        sReadColList = ((qlvInitBaseTableNode *) sOptUpdateStmt->mReadTableNode)->mRefColumnList;
    }
    else
    {
        STL_DASSERT( sOptUpdateStmt->mReadTableNode->mType == QLV_NODE_TYPE_JOIN_TABLE );
        sReadColList = ((qlvInitJoinTableNode *) sOptUpdateStmt->mReadTableNode)->mRefColumnList;
    }
    STL_DASSERT( sReadColList != NULL );
    
    if( sReadColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sReadColList,
                                             STL_TRUE,
                                             & sExeUpdateStmt->mReadColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeUpdateStmt->mReadColBlock = NULL;
    }

    
    /**
     * Update Table에 대한 Write Column List 구성
     */

    sWriteColList = sOptUpdateStmt->mSetColExprList;
    STL_DASSERT( sWriteColList != NULL );
    
    STL_DASSERT( sWriteColList->mCount > 0 );

    STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                         sWriteColList,
                                         STL_FALSE,
                                         & sExeUpdateStmt->mWriteColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );


    /**
     * UPDATE 관련 Object 들의 공간 확보 
     */

    STL_TRY( qlndAllocRelObject4Update( aDBCStmt,
                                        aSQLStmt,
                                        aDataArea,
                                        sOptUpdateStmt->mTableHandle,
                                        sReadColList,
                                        sExeUpdateStmt->mWriteColBlock,
                                        sOptUpdateStmt->mRelObject,
                                        & sExeUpdateStmt->mRelObject,
                                        aEnv )
             == STL_SUCCESS );


    /**
     * UPDATE시 conflict 처리를 위한 Fetch Record 정보 관련
     */

    sExeUpdateStmt->mVersionConflict = STL_FALSE;
    
    stlMemset( & sExeUpdateStmt->mFetchRecordInfo,
               0x00,
               STL_SIZEOF( smlFetchRecordInfo ) );

    
    /**
     * update 성공 여부를 저장하기 위한 공간 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( stlBool ) * aDataArea->mBlockAllocCnt,
                                (void **) & sExeUpdateStmt->mIsUpdated,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sExeUpdateStmt->mIsUpdated,
               0x00,
               STL_SIZEOF( stlBool ) * aDataArea->mBlockAllocCnt );

    
    /**
     * Outer Column Block 설정
     */

    if( sOptUpdateStmt->mSetOuterColumnList != NULL )
    {
        STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                             sOptUpdateStmt->mSetOuterColumnList,
                                             &sExeUpdateStmt->mSetOuterColBlock,
                                             &sExeUpdateStmt->mSetOuterOrgColBlock,
                                             &QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }

    if( sOptUpdateStmt->mReturnOuterColumnList != NULL )
    {
        STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                             sOptUpdateStmt->mReturnOuterColumnList,
                                             &sExeUpdateStmt->mReturnOuterColBlock,
                                             &sExeUpdateStmt->mReturnOuterOrgColBlock,
                                             &QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }


    /******************************************************
     * Row Block 설정
     ******************************************************/

    /**
     *  SCAN node의 Row Block으로 Update의 Row Block 설정
     */
    
    sExecScanNode = QLL_GET_EXECUTION_NODE( aDataArea, sOptUpdateStmt->mScanNode->mIdx );

    while( STL_TRUE )
    {
        if( sExecScanNode->mNodeType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE )
        {
            sExecScanNode =
                QLL_GET_EXECUTION_NODE(
                    aDataArea,
                    ( (qlnoSubQueryList *) sExecScanNode->mOptNode->mOptNode)->mChildNode->mIdx );
        }
        else if( sExecScanNode->mNodeType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE )
        {
            sExecScanNode =
                QLL_GET_EXECUTION_NODE(
                    aDataArea,
                    ( (qlnoSubQueryFilter *) sExecScanNode->mOptNode->mOptNode)->mChildNode->mIdx );

            STL_DASSERT( sExecScanNode->mNodeType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE );

            sExecScanNode =
                QLL_GET_EXECUTION_NODE(
                    aDataArea,
                    ( (qlnoSubQueryList *) sExecScanNode->mOptNode->mOptNode)->mChildNode->mIdx );
        }
        else
        {
            break;
        }
    }

    switch( sExecScanNode->mNodeType )
    {
        case QLL_PLAN_NODE_TABLE_ACCESS_TYPE :
            {
                /* TABLE ACCESS Row Block */
                sExeUpdateStmt->mRowBlock = 
                    ( (qlnxTableAccess *) sExecScanNode->mExecNode )->mFetchInfo.mRowBlock;
                break;
            }
        case QLL_PLAN_NODE_INDEX_ACCESS_TYPE :
            {
                /* INDEX ACCESS Row Block */
                sExeUpdateStmt->mRowBlock =
                    ( (qlnxIndexAccess *) sExecScanNode->mExecNode )->mFetchInfo.mRowBlock;
                break;
            }
        case QLL_PLAN_NODE_ROWID_ACCESS_TYPE :
            {
                /* ROWID ACCESS Row Block */
                sExeUpdateStmt->mRowBlock =
                    ( (qlnxRowIdAccess *) sExecScanNode->mExecNode )->mFetchInfo.mRowBlock;
                break;
            }
        case QLL_PLAN_NODE_CONCAT_TYPE :
            {
                /* CONCAT Row Block */
                sExeUpdateStmt->mRowBlock =
                    ( (qlnxConcat *) sExecScanNode->mExecNode )->mRowBlockList.mHead->mRowBlock;
                break;
            }
        case QLL_PLAN_NODE_NESTED_LOOPS_TYPE:
            {
                /* Nested Join Row Block */
                sExeUpdateStmt->mRowBlock =
                    ( (qlnxNestedLoopsJoin *) sExecScanNode->mExecNode )->mRowBlockList.mHead->mRowBlock;
                break;
            }
        case QLL_PLAN_NODE_MERGE_JOIN_TYPE:
            {
                /* Merge Join Row Block */
                sExeUpdateStmt->mRowBlock =
                    ( (qlnxSortMergeJoin *) sExecScanNode->mExecNode )->mRowBlockList.mHead->mRowBlock;
                break;
            }
        case QLL_PLAN_NODE_HASH_JOIN_TYPE:
            {
                /* Hash Join Row Block */
                sExeUpdateStmt->mRowBlock =
                    ( (qlnxHashJoin *) sExecScanNode->mExecNode )->mRowBlockList.mHead->mRowBlock;
                break;
            }
        default:
            {
                STL_DASSERT( 0 );            
                break;
            }
    }


    /**
     * Sub Query Filter의 Row Block 재조정
     */

#ifdef STL_DEBUG
    if( sOptUpdateStmt->mScanNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE )
    {
        qlnxSubQueryList  * sSubQueryList = NULL;

        sSubQueryList = (qlnxSubQueryList *)
            ( QLL_GET_EXECUTION_NODE( aDataArea, sOptUpdateStmt->mScanNode->mIdx )->mExecNode );

        STL_DASSERT( sSubQueryList->mCommonInfo.mRowBlockList != NULL );
        STL_DASSERT( sSubQueryList->mCommonInfo.mRowBlockList->mCount > 0 );
    }
    else if( sOptUpdateStmt->mScanNode->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE )
    {
        qlnxSubQueryFilter  * sSubQueryFilter = NULL;

        sSubQueryFilter = (qlnxSubQueryFilter *)
            ( QLL_GET_EXECUTION_NODE( aDataArea, sOptUpdateStmt->mScanNode->mIdx )->mExecNode );

        STL_DASSERT( sSubQueryFilter->mCommonInfo.mRowBlockList != NULL );
        STL_DASSERT( sSubQueryFilter->mCommonInfo.mRowBlockList->mCount > 0 );
    }
    else
    {
        /* Do Nothing */
    }
#endif


    /**
     * Common Execution Node 정보 설정
     */
    
    sExecNode->mNodeType  = QLL_PLAN_NODE_STMT_UPDATE_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeUpdateStmt;

    if( aDataArea->mIsEvaluateCost == STL_TRUE )
    {
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qllExecutionCost ),
                                    (void **) & sExecNode->mExecutionCost,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sExecNode->mExecutionCost, 0x00, STL_SIZEOF( qllExecutionCost ) );
    }
    else
    {
        sExecNode->mExecutionCost  = NULL;
    }
    

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief UPDATE 구문에 영향을 받는 SQL Object 들을 위한 Key Block 공간을 확보한다.
 * @param[in]       aDBCStmt            DBC Statement
 * @param[in,out]   aSQLStmt            SQL Statement
 * @param[in]       aDataArea           Data Area
 * @param[in]       aTableHandle        Table Handle
 * @param[in]       aReadColList        Join Read Column List
 * @param[in]       aTableWriteBlock    Table Write Block List
 * @param[in]       aIniRelObject       Related Object 의 Init Plan
 * @param[in,out]   aExeRelObject       Related Object 의 Data Plan
 * @param[in]       aEnv                Environment
 */
stlStatus qlndAllocRelObject4Update( qllDBCStmt             * aDBCStmt,
                                     qllStatement           * aSQLStmt,
                                     qllDataArea            * aDataArea,
                                     ellDictHandle          * aTableHandle,
                                     qlvRefExprList         * aReadColList,
                                     knlValueBlockList      * aTableWriteBlock,
                                     qlvInitUpdateRelObject * aIniRelObject,
                                     qlxExecUpdateRelObject * aExeRelObject,
                                     qllEnv                 * aEnv )
{
        
    ellDictHandle      * sIndexHandle    = NULL;
    ellDictHandle      * sColumnHandle   = NULL;
    knlValueBlockList  * sColumnBlock    = NULL;
    knlValueBlockList  * sValueBlock     = NULL;
    knlValueBlockList  * sNewValueBlock  = NULL;
    knlValueBlockList  * sPrevValueBlock = NULL;
    stlInt32             i               = 0;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReadColList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableWriteBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIniRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExeRelObject != NULL, QLL_ERROR_STACK(aEnv) );


    /************************************************
     * PRIMARY KEY constraint
     ************************************************/

    if ( aIniRelObject->mAffectPrimaryKeyCnt > 0 )
    {
        /************************************************
         * Key 개수만큼의 공간 확보
         ************************************************/

        /**
         * Deferred Constraint
         */

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlBool) * aIniRelObject->mAffectPrimaryKeyCnt,
                                    (void **) & aExeRelObject->mPrimaryKeyIsDeferred,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mPrimaryKeyIsDeferred,
                   0x00,
                   STL_SIZEOF(stlBool) * aIniRelObject->mAffectPrimaryKeyCnt );

        /**
         * Collision Count 
         */
        
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlInt64) * aIniRelObject->mAffectPrimaryKeyCnt,
                                    (void **) & aExeRelObject->mPrimaryKeyCollCnt,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mPrimaryKeyCollCnt,
                   0x00,
                   STL_SIZEOF(stlInt64) * aIniRelObject->mAffectPrimaryKeyCnt );

        /**
         * Delete Key Block
         */

        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectPrimaryKeyCnt,
                     (void **) & aExeRelObject->mPrimaryKeyDeleteKeyBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mPrimaryKeyDeleteKeyBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectPrimaryKeyCnt );
        
        /**
         * Insert Key Block
         */

        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectPrimaryKeyCnt,
                     (void **) & aExeRelObject->mPrimaryKeyInsertKeyBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mPrimaryKeyInsertKeyBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectPrimaryKeyCnt );

        /************************************************
         * 각 Key 에 대한 정보 및 Value Block 공간 확보 
         ************************************************/
        
        for ( i = 0; i < aIniRelObject->mAffectPrimaryKeyCnt; i++ )
        {
            sIndexHandle =
                ellGetConstraintIndexDictHandle( & aIniRelObject->mAffectPrimaryKeyHandle[i] );

            /**
             * Index Delete/Insert Key 공간 확보 
             */
            
            STL_TRY( qlndShareIndexValueListForDelete(
                         aDBCStmt,
                         aDataArea,
                         aReadColList,
                         sIndexHandle,
                         &aExeRelObject->mPrimaryKeyDeleteKeyBlock[i],
                         aEnv )
                     == STL_SUCCESS );
            
            STL_TRY( qlndShareIndexValueListForInsert(
                         aDBCStmt,
                         aDataArea,
                         aReadColList,
                         aTableWriteBlock,
                         sIndexHandle,
                         &aExeRelObject->mPrimaryKeyInsertKeyBlock[i],
                         aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        aExeRelObject->mPrimaryKeyIsDeferred     = NULL;
        aExeRelObject->mPrimaryKeyCollCnt        = NULL;
        aExeRelObject->mPrimaryKeyDeleteKeyBlock = NULL;
        aExeRelObject->mPrimaryKeyInsertKeyBlock = NULL;
    }


    /************************************************
     * UNIQUE constraint
     ************************************************/

    if ( aIniRelObject->mAffectUniqueKeyCnt > 0 )
    {
        /************************************************
         * Key 개수만큼의 공간 확보
         ************************************************/

        /**
         * Deferred Constraint
         */

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlBool) * aIniRelObject->mAffectUniqueKeyCnt,
                                    (void **) & aExeRelObject->mUniqueKeyIsDeferred,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mUniqueKeyIsDeferred,
                   0x00,
                   STL_SIZEOF(stlBool) * aIniRelObject->mAffectUniqueKeyCnt );

        /**
         * Collision Count 
         */
        
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlInt64) * aIniRelObject->mAffectUniqueKeyCnt,
                                    (void **) & aExeRelObject->mUniqueKeyCollCnt,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mUniqueKeyCollCnt,
                   0x00,
                   STL_SIZEOF(stlInt64) * aIniRelObject->mAffectUniqueKeyCnt );

        /**
         * Delete Key Block
         */

        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectUniqueKeyCnt,
                     (void **) & aExeRelObject->mUniqueKeyDeleteKeyBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mUniqueKeyDeleteKeyBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectUniqueKeyCnt );
        
        /**
         * Insert Key Block
         */

        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectUniqueKeyCnt,
                     (void **) & aExeRelObject->mUniqueKeyInsertKeyBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mUniqueKeyInsertKeyBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectUniqueKeyCnt );

        /************************************************
         * 각 Key 에 대한 정보 및 Value Block 공간 확보 
         ************************************************/
        
        for ( i = 0; i < aIniRelObject->mAffectUniqueKeyCnt; i++ )
        {
            sIndexHandle =
                ellGetConstraintIndexDictHandle( & aIniRelObject->mAffectUniqueKeyHandle[i] );

            /**
             * Index Delete/Insert Key 공간 확보 
             */

            STL_TRY( qlndShareIndexValueListForDelete(
                         aDBCStmt,
                         aDataArea,
                         aReadColList,
                         sIndexHandle,
                         &aExeRelObject->mUniqueKeyDeleteKeyBlock[i],
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlndShareIndexValueListForInsert(
                         aDBCStmt,
                         aDataArea,
                         aReadColList,
                         aTableWriteBlock,
                         sIndexHandle,
                         &aExeRelObject->mUniqueKeyInsertKeyBlock[i],
                         aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        aExeRelObject->mUniqueKeyIsDeferred     = NULL;
        aExeRelObject->mUniqueKeyCollCnt        = NULL;
        aExeRelObject->mUniqueKeyDeleteKeyBlock = NULL;
        aExeRelObject->mUniqueKeyInsertKeyBlock = NULL;
    }

    
    /************************************************
     * FOREIGN KEY constraint
     ************************************************/

    /**
     * @todo Foreign Key 구현해야 함.
     */
    
    STL_TRY_THROW( aIniRelObject->mAffectForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );

    /************************************************
     * CHILD FOREIGN KEY constraint
     ************************************************/

    /**
     * @todo Child Foreign Key 구현해야 함.
     */
    
    STL_TRY_THROW( aIniRelObject->mAffectChildForeignKeyCnt == 0, RAMP_ERR_NOT_IMPLEMENTED );

    
    /************************************************
     * Unique Index 
     ************************************************/

    if ( aIniRelObject->mAffectUniqueIndexCnt > 0 )
    {
        /************************************************
         * Key 개수만큼의 공간 확보
         ************************************************/

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlBool) * aIniRelObject->mAffectUniqueIndexCnt,
                                    (void **) & aExeRelObject->mUniqueIndexIsDeferred,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mUniqueIndexIsDeferred,
                   0x00,
                   STL_SIZEOF(stlBool) * aIniRelObject->mAffectUniqueIndexCnt );
        
        /**
         * Collision Count 
         */
        
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlInt64) * aIniRelObject->mAffectUniqueIndexCnt,
                                    (void **) & aExeRelObject->mUniqueIndexCollCnt,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mUniqueIndexCollCnt,
                   0x00,
                   STL_SIZEOF(stlInt64) * aIniRelObject->mAffectUniqueIndexCnt );

        /**
         * Delete Key Block
         */

        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectUniqueIndexCnt,
                     (void **) & aExeRelObject->mUniqueIndexDeleteKeyBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mUniqueIndexDeleteKeyBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectUniqueIndexCnt );
        
        /**
         * Insert Key Block
         */

        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectUniqueIndexCnt,
                     (void **) & aExeRelObject->mUniqueIndexInsertKeyBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mUniqueIndexInsertKeyBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectUniqueIndexCnt );

        /************************************************
         * 각 Key 에 대한 정보 및 Value Block 공간 확보 
         ************************************************/
        
        for ( i = 0; i < aIniRelObject->mAffectUniqueIndexCnt; i++ )
        {
            sIndexHandle = & aIniRelObject->mAffectUniqueIndexHandle[i];

            /**
             * Index Delete/Insert Key 공간 확보 
             */

            STL_TRY( qlndShareIndexValueListForDelete(
                         aDBCStmt,
                         aDataArea,
                         aReadColList,
                         sIndexHandle,
                         &aExeRelObject->mUniqueIndexDeleteKeyBlock[i],
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlndShareIndexValueListForInsert(
                         aDBCStmt,
                         aDataArea,
                         aReadColList,
                         aTableWriteBlock,
                         sIndexHandle,
                         &aExeRelObject->mUniqueIndexInsertKeyBlock[i],
                         aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        aExeRelObject->mUniqueIndexIsDeferred     = NULL;
        aExeRelObject->mUniqueIndexCollCnt        = NULL;
        aExeRelObject->mUniqueIndexDeleteKeyBlock = NULL;
        aExeRelObject->mUniqueIndexInsertKeyBlock = NULL;
    }
   

    /************************************************
     * Non-Unique Key 
     ************************************************/

    if ( aIniRelObject->mAffectNonUniqueIndexCnt > 0 )
    {
        /************************************************
         * Key 개수만큼의 공간 확보
         ************************************************/

        /**
         * Delete Key Block
         */

        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectNonUniqueIndexCnt,
                     (void **) & aExeRelObject->mNonUniqueIndexDeleteKeyBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mNonUniqueIndexDeleteKeyBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectNonUniqueIndexCnt );
        
        /**
         * Insert Key Block
         */

        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectNonUniqueIndexCnt,
                     (void **) & aExeRelObject->mNonUniqueIndexInsertKeyBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mNonUniqueIndexInsertKeyBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectNonUniqueIndexCnt );

        /************************************************
         * 각 Key 에 대한 정보 및 Value Block 공간 확보 
         ************************************************/
        
        for ( i = 0; i < aIniRelObject->mAffectNonUniqueIndexCnt; i++ )
        {
            sIndexHandle = & aIniRelObject->mAffectNonUniqueIndexHandle[i];

            /**
             * Index Delete/Insert Key 공간 확보 
             */

            STL_TRY( qlndShareIndexValueListForDelete(
                         aDBCStmt,
                         aDataArea,
                         aReadColList,
                         sIndexHandle,
                         &aExeRelObject->mNonUniqueIndexDeleteKeyBlock[i],
                         aEnv )
                     == STL_SUCCESS );

            STL_TRY( qlndShareIndexValueListForInsert(
                         aDBCStmt,
                         aDataArea,
                         aReadColList,
                         aTableWriteBlock,
                         sIndexHandle,
                         &aExeRelObject->mNonUniqueIndexInsertKeyBlock[i],
                         aEnv )
                     == STL_SUCCESS );

        }
    }
    else
    {
        aExeRelObject->mNonUniqueIndexDeleteKeyBlock = NULL;
        aExeRelObject->mNonUniqueIndexInsertKeyBlock = NULL;
    }


    /************************************************
     * NOT NULL Constraint
     ************************************************/

    if ( aIniRelObject->mAffectCheckNotNullCnt > 0 )
    {
        /************************************************
         * NOT NULL 개수만큼의 공간 확보
         ************************************************/

        /**
         * Deferred Constraint
         */

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlBool) * aIniRelObject->mAffectCheckNotNullCnt,
                                    (void **) & aExeRelObject->mNotNullIsDeferred,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mNotNullIsDeferred,
                   0x00,
                   STL_SIZEOF(stlBool) * aIniRelObject->mAffectCheckNotNullCnt );

        /**
         * Collision Count 
         */
        
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlInt64) * aIniRelObject->mAffectCheckNotNullCnt,
                                    (void **) & aExeRelObject->mNotNullCollCnt,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mNotNullCollCnt,
                   0x00,
                   STL_SIZEOF(stlInt64) * aIniRelObject->mAffectCheckNotNullCnt );
        
        /**
         * New Column Block
         */

        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectCheckNotNullCnt,
                     (void **) & aExeRelObject->mNewNotNullColumnBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mNewNotNullColumnBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectCheckNotNullCnt );

        /**
         * Old Column Block
         */

        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectCheckNotNullCnt,
                     (void **) & aExeRelObject->mOldNotNullColumnBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mOldNotNullColumnBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mAffectCheckNotNullCnt );

        /************************************************
         * 각 NOT NULL 에 대한 Value Block 공간 확보
         ************************************************/
        
        for ( i = 0; i < aIniRelObject->mAffectCheckNotNullCnt; i++ )
        {
            sColumnHandle = ellGetCheckNotNullColumnHandle( & aIniRelObject->mAffectCheckNotNullHandle[i] );

            /**
             * New Column Block 공유
             */

            sColumnBlock = ellFindColumnValueList( aTableWriteBlock,
                                                   ellGetColumnTableID( sColumnHandle ), 
                                                   ellGetColumnIdx( sColumnHandle ) );

            STL_TRY( knlInitShareBlock( & aExeRelObject->mNewNotNullColumnBlock[i],
                                        sColumnBlock,
                                        & QLL_DATA_PLAN(aDBCStmt),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            /**
             * Old Column Block 공유
             */

            if ( aIniRelObject->mAffectCheckNotNullDefer[i] == STL_TRUE )
            {
                STL_TRY( qlndShareNotNullReadColumnValueList( aDBCStmt,
                                                              aDataArea,
                                                              aReadColList,
                                                              sColumnHandle,
                                                              & aExeRelObject->mOldNotNullColumnBlock[i],
                                                              aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                aExeRelObject->mOldNotNullColumnBlock[i] = NULL;
            }
        }
    }
    else
    {
        aExeRelObject->mOldNotNullColumnBlock = NULL;
        aExeRelObject->mNewNotNullColumnBlock = NULL;
    }
    
    /************************************************
     * CHECK Constraint
     ************************************************/

    /** @todo CHECK constraint 의 Value Block 공유 */

    /************************************************
     * Identity Column
     ************************************************/
    
    if ( aIniRelObject->mHasAffectIdentity == STL_TRUE )
    {
        sColumnBlock = ellFindColumnValueList( aTableWriteBlock,
                                               ellGetColumnTableID( & aIniRelObject->mIdentityColumnHandle ), 
                                               ellGetColumnIdx( & aIniRelObject->mIdentityColumnHandle ) );
        
        STL_TRY( knlInitShareBlock( & aExeRelObject->mIdentityColumnBlock,
                                    sColumnBlock,
                                    & QLL_DATA_PLAN(aDBCStmt),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        aExeRelObject->mIdentityColumnBlock = NULL;
    }

    /************************************************
     * Supplemental Logging 적용 여부
     ************************************************/

    aExeRelObject->mIsSuppLog = ellGetTableNeedSupplePK( aTableHandle, ELL_ENV( aEnv ) );
    
    if( aExeRelObject->mIsSuppLog == STL_TRUE )
    {
        if( aIniRelObject->mTotalPrimaryKeyCnt > 0 )
        {
            STL_TRY( qlndShareSuppleValueList( aDBCStmt,
                                               aDataArea,
                                               aReadColList,
                                               aIniRelObject->mTotalPrimaryKeyHandle,
                                               &aExeRelObject->mSuppLogPrimaryKeyBlock,
                                               aEnv )
                     == STL_SUCCESS );

            aExeRelObject->mSuppLogBeforeColBlock = NULL;

            sValueBlock = aTableWriteBlock;
            while( sValueBlock != NULL )
            {
                STL_TRY( qlndCopyBlock( & aSQLStmt->mLongTypeValueList,
                                        & sNewValueBlock,
                                        STL_LAYER_SQL_PROCESSOR,
                                        KNL_GET_BLOCK_ALLOC_CNT( sValueBlock ),
                                        sValueBlock,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
                         == STL_SUCCESS );

                /* link */
                if( sPrevValueBlock == NULL )
                {
                    aExeRelObject->mSuppLogBeforeColBlock = sNewValueBlock;
                }
                else
                {
                    KNL_LINK_BLOCK_LIST( sPrevValueBlock, sNewValueBlock );
                }
                sPrevValueBlock = sNewValueBlock;

                sValueBlock = sValueBlock->mNext;
            }
        }
        else
        {
            aExeRelObject->mSuppLogPrimaryKeyBlock = NULL;
            aExeRelObject->mSuppLogBeforeColBlock  = NULL;
        }
    }
    else
    {
        aExeRelObject->mSuppLogPrimaryKeyBlock = NULL;
        aExeRelObject->mSuppLogBeforeColBlock  = NULL;
    }


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlndAllocRelObject4Update()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnd */
