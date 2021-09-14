/*******************************************************************************
 * qlndInsertStmt.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndInsertStmt.c 9998 2013-10-23 06:48:36Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndInsertStmt.c
 * @brief SQL Processor Layer Data Optimization Node - INSERT STATEMENT Node
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief INSERT STATEMENT Execution node에 대한 Data 정보를 구축한다.
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
stlStatus qlndDataOptimizeInsertStmt( smlTransId              aTransID,
                                      qllDBCStmt            * aDBCStmt,
                                      qllStatement          * aSQLStmt,
                                      qllOptimizationNode   * aOptNode,
                                      qllDataArea           * aDataArea,
                                      qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode      = NULL;
    qlnoInsertStmt    * sOptInsertStmt = NULL;
    qlnxInsertStmt    * sExeInsertStmt = NULL;
    qlvInitExpression * sExpr          = NULL;

    
    /*
     * Paramter Validation
     */
    
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
     * 하위 node에 대한 Data Optimize 수행
     */

    if( sOptInsertStmt->mQueryNode != NULL )
    {
        QLND_DATA_OPTIMIZE( aTransID,
                            aDBCStmt,
                            aSQLStmt,
                            sOptInsertStmt->mQueryNode,
                            aDataArea,
                            aEnv );
    }

    if( sOptInsertStmt->mInsertValueChildNode != NULL )
    {
        QLND_DATA_OPTIMIZE( aTransID,
                            aDBCStmt,
                            aSQLStmt,
                            sOptInsertStmt->mInsertValueChildNode,
                            aDataArea,
                            aEnv );
    }

    if( sOptInsertStmt->mReturnChildNode != NULL )
    {
        QLND_DATA_OPTIMIZE( aTransID,
                            aDBCStmt,
                            aSQLStmt,
                            sOptInsertStmt->mReturnChildNode,
                            aDataArea,
                            aEnv );
    }


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * INSERT STATEMENT Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxInsertStmt ),
                                (void **) & sExeInsertStmt,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeInsertStmt, 0x00, STL_SIZEOF( qlnxInsertStmt ) );


    /**
     * Pseudo Column List 구성
     */

    if( sOptInsertStmt->mStmtExprList->mInitExprList->mPseudoColExprList->mCount > 0 )
    {
        /* Total Pseudo Column List 구성 */
        STL_TRY( qlndBuildPseudoColBlockList( aDataArea,
                                              sOptInsertStmt->mStmtExprList->mInitExprList->mPseudoColExprList,
                                              & sExeInsertStmt->mTotalPseudoColBlock,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                 == STL_SUCCESS );

        /* Query Pseudo Column List 구성 */
        STL_TRY( qlndBuildPseudoColBlockList( aDataArea,
                                              sOptInsertStmt->mQueryExprList->mInitExprList->mPseudoColExprList,
                                              & sExeInsertStmt->mQueryPseudoColBlock,
                                              & QLL_DATA_PLAN( aDBCStmt ),
                                              aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeInsertStmt->mTotalPseudoColBlock = NULL;
        sExeInsertStmt->mQueryPseudoColBlock = NULL;
    }


    /**
     * Constant Expression Result Column 공간 확보
     */
    
    if( sOptInsertStmt->mStmtExprList->mInitExprList->mConstExprList->mCount > 0 )
    {
        sExpr = sOptInsertStmt->mStmtExprList->mInitExprList->mConstExprList->mHead->mExprPtr;
        
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
     * RowId 를 위한 Value Block List 구성
     */

    if( sOptInsertStmt->mRowIdColumn != NULL )
    {
        STL_TRY( qlndBuildTableRowIdColBlockList( aDataArea,
                                                  sOptInsertStmt->mRowIdColumn,
                                                  & sExeInsertStmt->mRowIdColBlock,
                                                  & QLL_DATA_PLAN( aDBCStmt ),
                                                  aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeInsertStmt->mRowIdColBlock = NULL;
    }

    /**
     * INSERT 관련 Object 들의 공간 확보 
     */

    STL_TRY( qlndAllocRelObject4Insert( aTransID,
                                        aDBCStmt,
                                        sOptInsertStmt->mTableHandle,
                                        aDataArea->mWriteColumnBlock->mHead->mColumnValueBlock,
                                        sOptInsertStmt->mRelObject,
                                        & sExeInsertStmt->mRelObject,
                                        aEnv )
             == STL_SUCCESS );


    /**
     * Row Block 공간 확보
     */

    if( aSQLStmt->mIsAtomic == STL_FALSE )
    {
        STL_TRY( smlInitRowBlock( & sExeInsertStmt->mRowBlock,
                                  aDataArea->mBlockAllocCnt,
                                  & QLL_DATA_PLAN( aDBCStmt ),
                                  SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( aDataArea->mBlockAllocCnt > 0 );
        
        STL_TRY( smlInitRowBlock( & sExeInsertStmt->mRowBlock,
                                  knlGetPropertyBigIntValueByID( KNL_PROPERTY_BLOCK_READ_COUNT,
                                                                 KNL_ENV(aEnv) ) * aDataArea->mBlockAllocCnt,
                                  & QLL_DATA_PLAN( aDBCStmt ),
                                  SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }


    /**
     * Outer Column Block 설정
     */

    if( sOptInsertStmt->mReturnOuterColumnList != NULL )
    {
        STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                             sOptInsertStmt->mReturnOuterColumnList,
                                             &sExeInsertStmt->mReturnOuterColBlock,
                                             &sExeInsertStmt->mReturnOuterOrgColBlock,
                                             &QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }


    /**
     * Common Execution Node 정보 설정
     */
    
    sExecNode->mNodeType  = QLL_PLAN_NODE_STMT_INSERT_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeInsertStmt;

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
 * @brief INSERT 구문에 영향을 받는 SQL Object 들을 위한 Key Block 공간을 확보한다.
 * @param[in]      aTransID          Transaction ID
 * @param[in]      aDBCStmt          DBC Statement
 * @param[in]      aTableHandle      Table Handle
 * @param[in]      aTableWriteBlock  Table Write Block List
 * @param[in]      aIniRelObject     Related Object 의 Init Plan
 * @param[in,out]  aExeRelObject     Related Object 의 Data Plan
 * @param[in]      aEnv              Environment
 */
stlStatus qlndAllocRelObject4Insert( smlTransId               aTransID,
                                     qllDBCStmt             * aDBCStmt,
                                     ellDictHandle          * aTableHandle,
                                     knlValueBlockList      * aTableWriteBlock,
                                     qlvInitInsertRelObject * aIniRelObject,
                                     qlxExecInsertRelObject * aExeRelObject,
                                     qllEnv                 * aEnv )
{
    stlInt32  i = 0;
    
    ellDictHandle * sIndexHandle = NULL;
    ellDictHandle * sParentKeyHandle = NULL;

    ellDictHandle     * sColumnHandle = NULL;
    knlValueBlockList * sColumnBlock = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableWriteBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIniRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExeRelObject != NULL, QLL_ERROR_STACK(aEnv) );
    
    /************************************************
     * PRIMARY KEY constraint
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
                ellGetConstraintIndexDictHandle( & aIniRelObject->mPrimaryKeyHandle[i] );
            
            STL_TRY( ellShareIndexValueListForINSERT(
                         aTableHandle,
                         aTableWriteBlock,
                         sIndexHandle,
                         & QLL_DATA_PLAN(aDBCStmt),
                         & aExeRelObject->mPrimaryKeyIndexBlock[i],
                         ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        aExeRelObject->mPrimaryKeyIndexBlock = NULL;
        aExeRelObject->mPrimaryKeyIsDeferred = NULL;
    }


    /************************************************
     * UNIQUE constraint
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
            
            STL_TRY( ellShareIndexValueListForINSERT(
                         aTableHandle,
                         aTableWriteBlock,
                         sIndexHandle,
                         & QLL_DATA_PLAN(aDBCStmt),
                         & aExeRelObject->mUniqueKeyIndexBlock[i],
                         ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        aExeRelObject->mUniqueKeyIndexBlock = NULL;
        aExeRelObject->mUniqueKeyIsDeferred = NULL;
    }

    /************************************************
     * FOREIGN KEY constraint
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
         * Key 개수만큼의 Parent Index 공간 확보
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
        
        /**
         * 각 Key Index 에 대한 Key Block 공간 확보
         */
        
        for ( i = 0; i < aIniRelObject->mForeignKeyCnt; i++ )
        {
            sIndexHandle =
                ellGetConstraintIndexDictHandle( & aIniRelObject->mForeignKeyHandle[i] );
            
            STL_TRY( ellShareIndexValueListForINSERT(
                         aTableHandle,
                         aTableWriteBlock,
                         sIndexHandle,
                         & QLL_DATA_PLAN(aDBCStmt),
                         & aExeRelObject->mForeignKeyIndexBlock[i],
                         ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }

        /**
         * 각 Parent Index 에 대한 Key Block 공간 확보
         */
        
        for ( i = 0; i < aIniRelObject->mForeignKeyCnt; i++ )
        {
            sParentKeyHandle =
                ellGetForeignKeyParentUniqueKeyHandle( & aIniRelObject->mForeignKeyHandle[i] );
            sIndexHandle =
                ellGetConstraintIndexDictHandle( sParentKeyHandle );
            
            STL_TRY( ellShareIndexValueListForINSERT(
                         aTableHandle,
                         aTableWriteBlock,
                         sIndexHandle,
                         & QLL_DATA_PLAN(aDBCStmt),
                         & aExeRelObject->mParentIndexBlock[i],
                         ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        aExeRelObject->mForeignKeyIndexBlock = NULL;
        aExeRelObject->mForeignKeyIsDeferred = NULL;
        aExeRelObject->mParentIndexBlock = NULL;
    }

    /************************************************
     * @todo CHILD FOREIGN KEY constraint
     ************************************************/

    
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
            STL_TRY( ellShareIndexValueListForINSERT(
                         aTableHandle,
                         aTableWriteBlock,
                         & aIniRelObject->mUniqueIndexHandle[i],
                         & QLL_DATA_PLAN(aDBCStmt),
                         & aExeRelObject->mUniqueIndexBlock[i],
                         ELL_ENV(aEnv) )
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
            STL_TRY( ellShareIndexValueListForINSERT(
                         aTableHandle,
                         aTableWriteBlock,
                         & aIniRelObject->mNonUniqueIndexHandle[i],
                         & QLL_DATA_PLAN(aDBCStmt),
                         & aExeRelObject->mNonUniqueIndexBlock[i],
                         ELL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        aExeRelObject->mNonUniqueIndexBlock = NULL;
    }

    /************************************************
     * NOT NULL constraint
     ************************************************/

    if ( aIniRelObject->mCheckNotNullCnt > 0 )
    {
        /**
         * NOT NULL 개수만큼의 공간 확보
         */
        
        STL_TRY( knlAllocRegionMem(
                     & QLL_DATA_PLAN(aDBCStmt),
                     STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mCheckNotNullCnt,
                     (void **) & aExeRelObject->mNotNullColumnBlock,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( aExeRelObject->mNotNullColumnBlock,
                   0x00,
                   STL_SIZEOF(knlValueBlockList *) * aIniRelObject->mCheckNotNullCnt );
        
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlBool) * aIniRelObject->mCheckNotNullCnt,
                                    (void **) & aExeRelObject->mNotNullIsDeferred,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mNotNullIsDeferred,
                   0x00,
                   STL_SIZEOF(stlBool) * aIniRelObject->mCheckNotNullCnt );

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                    STL_SIZEOF(stlInt64) * aIniRelObject->mCheckNotNullCnt,
                                    (void **) & aExeRelObject->mNotNullCollCnt,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        stlMemset( aExeRelObject->mNotNullCollCnt,
                   0x00,
                   STL_SIZEOF(stlInt64) * aIniRelObject->mCheckNotNullCnt );

        /**
         * NOT NULL 의 Column Block 공간 확보
         */

        for ( i = 0; i < aIniRelObject->mCheckNotNullCnt; i++ )
        {
            sColumnHandle = ellGetCheckNotNullColumnHandle( & aIniRelObject->mCheckNotNullHandle[i] );

            sColumnBlock = ellFindColumnValueList( aTableWriteBlock,
                                                   ellGetColumnTableID( sColumnHandle ), 
                                                   ellGetColumnIdx( sColumnHandle ) );

            STL_TRY( knlInitShareBlock( & aExeRelObject->mNotNullColumnBlock[i],
                                        sColumnBlock,
                                        & QLL_DATA_PLAN(aDBCStmt),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
    }
    
    /************************************************
     * @todo CHECK constraint
     ************************************************/

    /************************************************
     * Identity Column 
     ************************************************/
    
    if ( aIniRelObject->mHasIdentity == STL_TRUE )
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
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnd */
