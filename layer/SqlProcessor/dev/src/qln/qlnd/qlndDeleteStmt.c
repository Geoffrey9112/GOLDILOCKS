/*******************************************************************************
 * qlndDeleteStmt.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndDeleteStmt.c 11148 2014-02-06 07:52:37Z jhkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndDeleteStmt.c
 * @brief SQL Processor Layer Data Optimization Node - DELETE STATEMENT Node
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief DELETE STATEMENT Execution node에 대한 Data 정보를 구축한다.
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
stlStatus qlndDataOptimizeDeleteStmt( smlTransId              aTransID,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv )
{
    qllExecutionNode    * sExecNode         = NULL;
    qlnoDeleteStmt      * sOptDeleteStmt    = NULL;
    qlnxDeleteStmt      * sExeDeleteStmt    = NULL;
    qlvInitExpression   * sExpr             = NULL;
    qllExecutionNode    * sExecScanNode     = NULL;
    qlvRefExprList      * sRefColumnList    = NULL;
    qlvInitExpression   * sRefRowIdColumn   = NULL;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_STMT_DELETE_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * DELETE STATEMENT Optimization Node 획득
     */

    sOptDeleteStmt = (qlnoDeleteStmt *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptDeleteStmt->mScanNode,
                        aDataArea,
                        aEnv );

    if( sOptDeleteStmt->mReturnChildNode != NULL )
    {
        QLND_DATA_OPTIMIZE( aTransID,
                            aDBCStmt,
                            aSQLStmt,
                            sOptDeleteStmt->mReturnChildNode,
                            aDataArea,
                            aEnv );
    }


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * DELETE STATEMENT Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxDeleteStmt ),
                                (void **) & sExeDeleteStmt,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeDeleteStmt, 0x00, STL_SIZEOF( qlnxDeleteStmt ) );


    /**
     * Pseudo Column List 구성
     */

    if( sOptDeleteStmt->mStmtExprList->mInitExprList->mPseudoColExprList->mCount > 0 )
    {
        /* Coverage : 현재 stmt 단위로 지원되는 Pseudo Column은 없다. */
        /* Total Pseudo Column List 구성 */
        STL_TRY( qlndBuildPseudoColBlockList( aDataArea,
                                              sOptDeleteStmt->mStmtExprList->mInitExprList->mPseudoColExprList,
                                              & sExeDeleteStmt->mTotalPseudoColBlock,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                 == STL_SUCCESS );

        /* Query Pseudo Column List 구성 */
        STL_TRY( qlndBuildPseudoColBlockList( aDataArea,
                                              sOptDeleteStmt->mQueryExprList->mInitExprList->mPseudoColExprList,
                                              & sExeDeleteStmt->mQueryPseudoColBlock,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeDeleteStmt->mTotalPseudoColBlock = NULL;
        sExeDeleteStmt->mQueryPseudoColBlock = NULL;
    }


    /**
     * Constant Expression Result Column 공간 확보
     */
    
    if( sOptDeleteStmt->mStmtExprList->mInitExprList->mConstExprList->mCount > 0 )
    {
        sExpr = sOptDeleteStmt->mStmtExprList->mInitExprList->mConstExprList->mHead->mExprPtr;
        
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


    if( sOptDeleteStmt->mReadTableNode->mType == QLV_NODE_TYPE_JOIN_TABLE )
    {
        STL_DASSERT( (((qlvInitJoinTableNode *) sOptDeleteStmt->mReadTableNode)->mJoinType == QLV_JOIN_TYPE_LEFT_SEMI) ||
                     (((qlvInitJoinTableNode *) sOptDeleteStmt->mReadTableNode)->mJoinType == QLV_JOIN_TYPE_RIGHT_SEMI) ||
                     (((qlvInitJoinTableNode *) sOptDeleteStmt->mReadTableNode)->mJoinType == QLV_JOIN_TYPE_LEFT_ANTI_SEMI) ||
                     (((qlvInitJoinTableNode *) sOptDeleteStmt->mReadTableNode)->mJoinType == QLV_JOIN_TYPE_RIGHT_ANTI_SEMI) ||
                     (((qlvInitJoinTableNode *) sOptDeleteStmt->mReadTableNode)->mJoinType == QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA) ||
                     (((qlvInitJoinTableNode *) sOptDeleteStmt->mReadTableNode)->mJoinType == QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA) ||
                     (((qlvInitJoinTableNode *) sOptDeleteStmt->mReadTableNode)->mJoinType == QLV_JOIN_TYPE_INVERTED_LEFT_SEMI) ||
                     (((qlvInitJoinTableNode *) sOptDeleteStmt->mReadTableNode)->mJoinType == QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI) );

        sRefColumnList = 
            ((qlvInitJoinTableNode *) sOptDeleteStmt->mReadTableNode)->mRefColumnList;

        sRefRowIdColumn = NULL;
    }
    else
    {
        STL_DASSERT( sOptDeleteStmt->mReadTableNode->mType == QLV_NODE_TYPE_BASE_TABLE );

        sRefColumnList =
            ((qlvInitBaseTableNode *) sOptDeleteStmt->mReadTableNode)->mRefColumnList;

        sRefRowIdColumn =
            ((qlvInitBaseTableNode *) sOptDeleteStmt->mReadTableNode)->mRefRowIdColumn;
    }

    /**
     * Delete Table에 대한 Column List 구성
     */

    STL_DASSERT( sRefColumnList != NULL );

    if( sRefColumnList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sRefColumnList,
                                             STL_TRUE,
                                             & sExeDeleteStmt->mReadColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * Delete Table에 대한 RowId Column List 구성
     */

    if( sRefRowIdColumn != NULL )
    {
        STL_TRY( qlndBuildTableRowIdColBlockList( aDataArea,
                                                 sRefRowIdColumn,
                                                 & sExeDeleteStmt->mRowIdColBlock,
                                                 & QLL_DATA_PLAN( aDBCStmt ),
                                                 aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeDeleteStmt->mRowIdColBlock = NULL;
    }


    /**
     * DELETE 관련 Object 들의 공간 확보 
     */

    STL_TRY( qlndAllocRelObject4Delete( aDBCStmt,
                                        aDataArea,
                                        sOptDeleteStmt->mTableHandle,
                                        sRefColumnList,
                                        sOptDeleteStmt->mRelObject,
                                        & sExeDeleteStmt->mRelObject,
                                        aEnv )
             == STL_SUCCESS );


    /**
     * DELETE시 conflict 처리를 위한 Fetch Record 정보 관련
     */

    sExeDeleteStmt->mVersionConflict = STL_FALSE;
    
    stlMemset( & sExeDeleteStmt->mFetchRecordInfo,
               0x00,
               STL_SIZEOF( smlFetchRecordInfo ) );

    
    /**
     * delete 성공 여부를 저장하기 위한 공간 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( stlBool ) * aDataArea->mBlockAllocCnt,
                                (void **) & sExeDeleteStmt->mIsDeleted,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sExeDeleteStmt->mIsDeleted,
               0x00,
               STL_SIZEOF( stlBool ) * aDataArea->mBlockAllocCnt );
    

    /**
     * Outer Column Block 설정
     */

    if( sOptDeleteStmt->mReturnOuterColumnList != NULL )
    {
        STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                             sOptDeleteStmt->mReturnOuterColumnList,
                                             &sExeDeleteStmt->mReturnOuterColBlock,
                                             &sExeDeleteStmt->mReturnOuterOrgColBlock,
                                             &QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }


    /******************************************************
     * Row Block 설정
     ******************************************************/
    
    /**
     *  SCAN node의 Row Block으로 Delete의 Row Block 설정
     */
    
    sExecScanNode = QLL_GET_EXECUTION_NODE( aDataArea, sOptDeleteStmt->mScanNode->mIdx );
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
                sExeDeleteStmt->mRowBlock = 
                    ( (qlnxTableAccess *) sExecScanNode->mExecNode )->mFetchInfo.mRowBlock;
                break;
            }
        case QLL_PLAN_NODE_INDEX_ACCESS_TYPE :
            {
                /* INDEX ACCESS Row Block */
                sExeDeleteStmt->mRowBlock =
                    ( (qlnxIndexAccess *) sExecScanNode->mExecNode )->mFetchInfo.mRowBlock;
                break;
            }
        case QLL_PLAN_NODE_ROWID_ACCESS_TYPE :
            {
                /* ROWID ACCESS Row Block */
                sExeDeleteStmt->mRowBlock =
                    ( (qlnxRowIdAccess *) sExecScanNode->mExecNode )->mFetchInfo.mRowBlock;
                break;
            }
        case QLL_PLAN_NODE_CONCAT_TYPE :
            {
                /* CONCAT Row Block */
                sExeDeleteStmt->mRowBlock =
                    ( (qlnxConcat *) sExecScanNode->mExecNode )->mRowBlockList.mHead->mRowBlock;
                break;
            }
        case QLL_PLAN_NODE_NESTED_LOOPS_TYPE:
            {
                /* Nested Join Row Block */
                sExeDeleteStmt->mRowBlock =
                    ( (qlnxNestedLoopsJoin *) sExecScanNode->mExecNode )->mRowBlockList.mHead->mRowBlock;
                break;
            }
        case QLL_PLAN_NODE_MERGE_JOIN_TYPE:
            {
                /* Merge Join Row Block */
                sExeDeleteStmt->mRowBlock =
                    ( (qlnxSortMergeJoin *) sExecScanNode->mExecNode )->mRowBlockList.mHead->mRowBlock;
                break;
            }
        case QLL_PLAN_NODE_HASH_JOIN_TYPE:
            {
                /* Hash Join Row Block */
                sExeDeleteStmt->mRowBlock =
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
    if( sOptDeleteStmt->mScanNode->mType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE )
    {
        qlnxSubQueryList  * sSubQueryList = NULL;

        sSubQueryList = (qlnxSubQueryList *)
            ( QLL_GET_EXECUTION_NODE( aDataArea, sOptDeleteStmt->mScanNode->mIdx )->mExecNode );

        STL_DASSERT( sSubQueryList->mCommonInfo.mRowBlockList != NULL );
        STL_DASSERT( sSubQueryList->mCommonInfo.mRowBlockList->mCount > 0 );
    }
    else if( sOptDeleteStmt->mScanNode->mType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE )
    {
        qlnxSubQueryFilter  * sSubQueryFilter = NULL;

        sSubQueryFilter = (qlnxSubQueryFilter *)
            ( QLL_GET_EXECUTION_NODE( aDataArea, sOptDeleteStmt->mScanNode->mIdx )->mExecNode );

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
    
    sExecNode->mNodeType  = QLL_PLAN_NODE_STMT_DELETE_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeDeleteStmt;

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
 * @brief DELETE 구문에 영향을 받는 SQL Object 들을 위한 Key Block 공간을 Read Column List를 이용하여 확보한다.
 * @param[in]       aDBCStmt        DBC Statement
 * @param[in]       aDataArea       Data Area
 * @param[in]       aTableHandle    Table Handle
 * @param[in]       aReadColList    Join Read Column List
 * @param[in]       aIniRelObject   Related Object 의 Init Plan
 * @param[in,out]   aExeRelObject   Related Object 의 Data Plan
 * @param[in]       aEnv            Environment
 */
stlStatus qlndAllocRelObject4Delete( qllDBCStmt             * aDBCStmt,
                                     qllDataArea            * aDataArea,
                                     ellDictHandle          * aTableHandle,
                                     qlvRefExprList         * aReadColList,
                                     qlvInitDeleteRelObject * aIniRelObject,
                                     qlxExecDeleteRelObject * aExeRelObject,
                                     qllEnv                 * aEnv )
{
    stlInt32  i = 0;
    
    ellDictHandle       * sIndexHandle  = NULL;
    ellDictHandle       * sColumnHandle  = NULL;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReadColList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIniRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExeRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    

    /************************************************
     * Primary Key 
     ************************************************/

    if ( aIniRelObject->mPrimaryKeyCnt > 0 )
    {
        /**
         * Key 개수만큼의 공간 확보
         */
        
        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mPrimaryKeyCnt,
                     (void **) & aExeRelObject->mPrimaryKeyIndexBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mPrimaryKeyIndexBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mPrimaryKeyCnt );

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlBool) * aIniRelObject->mPrimaryKeyCnt,
                                    (void **) & aExeRelObject->mPrimaryKeyIsDeferred,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( aExeRelObject->mPrimaryKeyIsDeferred,
                   0x00,
                   STL_SIZEOF(stlBool) * aIniRelObject->mPrimaryKeyCnt );

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlInt64) * aIniRelObject->mPrimaryKeyCnt,
                                    (void **) & aExeRelObject->mPrimaryKeyCollCnt,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mPrimaryKeyCollCnt,
                   0x00,
                   STL_SIZEOF(stlInt64) * aIniRelObject->mPrimaryKeyCnt );

        
        /**
         * 각 Key Index 에 대한 Key Block 공간 확보
         */
        
        for ( i = 0; i < aIniRelObject->mPrimaryKeyCnt; i++ )
        {
            sIndexHandle =
                ellGetConstraintIndexDictHandle( & aIniRelObject->mPrimaryKeyHandle[ i ] );

            STL_TRY( qlndShareIndexValueListForDelete(
                         aDBCStmt,
                         aDataArea,
                         aReadColList,
                         sIndexHandle,
                         &aExeRelObject->mPrimaryKeyIndexBlock[i],
                         aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        aExeRelObject->mPrimaryKeyIndexBlock = NULL;
        aExeRelObject->mPrimaryKeyIsDeferred = NULL;
    }


    /************************************************
     * Unique Key 
     ************************************************/

    if ( aIniRelObject->mUniqueKeyCnt > 0 )
    {
        /**
         * Key 개수만큼의 공간 확보
         */
        
        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mUniqueKeyCnt,
                     (void **) & aExeRelObject->mUniqueKeyIndexBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mUniqueKeyIndexBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mUniqueKeyCnt );

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlBool) * aIniRelObject->mUniqueKeyCnt,
                                    (void **) & aExeRelObject->mUniqueKeyIsDeferred,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mUniqueKeyIsDeferred,
                   0x00,
                   STL_SIZEOF(stlBool) * aIniRelObject->mUniqueKeyCnt );

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlInt64) * aIniRelObject->mUniqueKeyCnt,
                                    (void **) & aExeRelObject->mUniqueKeyCollCnt,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mUniqueKeyCollCnt,
                   0x00,
                   STL_SIZEOF(stlInt64) * aIniRelObject->mUniqueKeyCnt );

        
        /**
         * 각 Key Index 에 대한 Key Block 공간 확보
         */
        
        for ( i = 0; i < aIniRelObject->mUniqueKeyCnt; i++ )
        {
            sIndexHandle =
                ellGetConstraintIndexDictHandle( & aIniRelObject->mUniqueKeyHandle[i] );

            STL_TRY( qlndShareIndexValueListForDelete(
                         aDBCStmt,
                         aDataArea,
                         aReadColList,
                         sIndexHandle,
                         &aExeRelObject->mUniqueKeyIndexBlock[i],
                         aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        aExeRelObject->mUniqueKeyIndexBlock = NULL;
        aExeRelObject->mUniqueKeyIsDeferred = NULL;
    }

    /************************************************
     * Foreign Key 
     ************************************************/

    if ( aIniRelObject->mForeignKeyCnt > 0 )
    {
        /**
         * Key 개수만큼의 Foreign Index 공간 확보
         */
        
        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mForeignKeyCnt,
                     (void **) & aExeRelObject->mForeignKeyIndexBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mForeignKeyIndexBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mForeignKeyCnt );

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlBool) * aIniRelObject->mForeignKeyCnt,
                                    (void **) & aExeRelObject->mForeignKeyIsDeferred,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mForeignKeyIsDeferred,
                   0x00,
                   STL_SIZEOF(stlBool) * aIniRelObject->mForeignKeyCnt );

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlInt64) * aIniRelObject->mForeignKeyCnt,
                                    (void **) & aExeRelObject->mForeignKeyCollCnt,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mForeignKeyCollCnt,
                   0x00,
                   STL_SIZEOF(stlInt64) * aIniRelObject->mForeignKeyCnt );
        
        /**
         * 각 Key Index 에 대한 Key Block 공간 확보
         */
        
        for ( i = 0; i < aIniRelObject->mForeignKeyCnt; i++ )
        {
            sIndexHandle =
                ellGetConstraintIndexDictHandle( & aIniRelObject->mForeignKeyHandle[i] );

            STL_TRY( qlndShareIndexValueListForDelete(
                         aDBCStmt,
                         aDataArea,
                         aReadColList,
                         sIndexHandle,
                         &aExeRelObject->mForeignKeyIndexBlock[i],
                         aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        aExeRelObject->mForeignKeyIndexBlock = NULL;
        aExeRelObject->mForeignKeyIsDeferred = NULL;
    }

    /************************************************
     * Child Foreign Key 
     ************************************************/

    if ( aIniRelObject->mChildForeignKeyCnt > 0 )
    {
        /**
         * @todo Key 개수만큼의 Index 공간 확보
         */

        STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
    }
    else
    {
        aExeRelObject->mChildForeignKeyIndexBlock = NULL;
    }

    /************************************************
     * Unique Index 
     ************************************************/

    if ( aIniRelObject->mUniqueIndexCnt > 0 )
    {
        /**
         * Index 개수만큼의 공간 확보
         */
        
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlBool) * aIniRelObject->mUniqueIndexCnt,
                                    (void **) & aExeRelObject->mUniqueIndexIsDeferred,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mUniqueIndexIsDeferred,
                   0x00,
                   STL_SIZEOF(stlBool) * aIniRelObject->mUniqueIndexCnt );

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlInt64) * aIniRelObject->mUniqueIndexCnt,
                                    (void **) & aExeRelObject->mUniqueIndexCollCnt,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mUniqueIndexCollCnt,
                   0x00,
                   STL_SIZEOF(stlInt64) * aIniRelObject->mUniqueIndexCnt );

        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mUniqueIndexCnt,
                     (void **) & aExeRelObject->mUniqueIndexBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mUniqueIndexBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mUniqueIndexCnt );

        /**
         * 각 Index 에 대한 Key Block 공간 확보
         */
        
        for ( i = 0; i < aIniRelObject->mUniqueIndexCnt; i++ )
        {
            STL_TRY( qlndShareIndexValueListForDelete(
                         aDBCStmt,
                         aDataArea,
                         aReadColList,
                         &aIniRelObject->mUniqueIndexHandle[i],
                         &aExeRelObject->mUniqueIndexBlock[i],
                         aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        aExeRelObject->mUniqueIndexIsDeferred = NULL;
        aExeRelObject->mUniqueIndexCollCnt = NULL;
        aExeRelObject->mUniqueIndexBlock = NULL;
    }
    
    /************************************************
     * Non-Unique Index 
     ************************************************/

    if ( aIniRelObject->mNonUniqueIndexCnt > 0 )
    {
        /**
         * Index 개수만큼의 공간 확보
         */
        
        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mNonUniqueIndexCnt,
                     (void **) & aExeRelObject->mNonUniqueIndexBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mNonUniqueIndexBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mNonUniqueIndexCnt );

        /**
         * 각 Index 에 대한 Key Block 공간 확보
         */
        
        for ( i = 0; i < aIniRelObject->mNonUniqueIndexCnt; i++ )
        {
            STL_TRY( qlndShareIndexValueListForDelete(
                         aDBCStmt,
                         aDataArea,
                         aReadColList,
                         &aIniRelObject->mNonUniqueIndexHandle[i],
                         &aExeRelObject->mNonUniqueIndexBlock[i],
                         aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        aExeRelObject->mNonUniqueIndexBlock = NULL;
    }

    /************************************************
     * deferrable NOT NULL constraint
     ************************************************/

    if ( aIniRelObject->mDeferNotNullCnt > 0 )
    {
        /**
         * Deferrable NOT NULL 개수만큼의 공간 확보
         */
        
        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mDeferNotNullCnt,
                     (void **) & aExeRelObject->mNotNullColumnBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( aExeRelObject->mNotNullColumnBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mDeferNotNullCnt );
        
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlBool) * aIniRelObject->mDeferNotNullCnt,
                                    (void **) & aExeRelObject->mNotNullIsDeferred,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mNotNullIsDeferred,
                   0x00,
                   STL_SIZEOF(stlBool) * aIniRelObject->mDeferNotNullCnt );

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlInt64) * aIniRelObject->mDeferNotNullCnt,
                                    (void **) & aExeRelObject->mNotNullCollCnt,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mNotNullCollCnt,
                   0x00,
                   STL_SIZEOF(stlInt64) * aIniRelObject->mDeferNotNullCnt );

        /**
         * NOT NULL 의 Column Block 공간 확보
         */

        for ( i = 0; i < aIniRelObject->mDeferNotNullCnt; i++ )
        {
            sColumnHandle = ellGetCheckNotNullColumnHandle( & aIniRelObject->mDeferNotNullHandle[i] );
            
            STL_TRY( qlndShareNotNullReadColumnValueList( aDBCStmt,
                                                          aDataArea,
                                                          aReadColList,
                                                          sColumnHandle,
                                                          & aExeRelObject->mNotNullColumnBlock[i],
                                                          aEnv )
                     == STL_SUCCESS );
        }
    }
    else
    {
        aExeRelObject->mNotNullColumnBlock = NULL;
        aExeRelObject->mNotNullIsDeferred = NULL;
        aExeRelObject->mNotNullCollCnt = NULL;
    }

    /************************************************
     * Supplemental Logging 적용 여부
     ************************************************/

    aExeRelObject->mIsSuppLog = ellGetTableNeedSupplePK( aTableHandle, ELL_ENV( aEnv ) );
    
    if( aExeRelObject->mIsSuppLog == STL_TRUE )
    {
        if( aIniRelObject->mPrimaryKeyCnt > 0 )
        {
            STL_TRY( qlndShareSuppleValueList( aDBCStmt,
                                               aDataArea,
                                               aReadColList,
                                               aIniRelObject->mPrimaryKeyHandle,
                                               &aExeRelObject->mSuppLogPrimaryKeyBlock,
                                               aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            aExeRelObject->mSuppLogPrimaryKeyBlock   = NULL;
        }
    }
    else
    {
        aExeRelObject->mSuppLogPrimaryKeyBlock   = NULL;
    }


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      "qlndAllocRelObject4Delete()" );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnd */
