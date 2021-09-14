/*******************************************************************************
 * qlndConcat.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndConcat.c 7616 2013-03-18 02:57:54Z xcom73 $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndConcat.c
 * @brief SQL Processor Layer Data Optimization NODE - CONCAT
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief CONCAT Execution node에 대한 Data 정보를 구축한다.
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
stlStatus qlndDataOptimizeConcat( smlTransId              aTransID,
                                  qllDBCStmt            * aDBCStmt,
                                  qllStatement          * aSQLStmt,
                                  qllOptimizationNode   * aOptNode,
                                  qllDataArea           * aDataArea,
                                  qllEnv                * aEnv )
{
    qllExecutionNode    * sExecNode             = NULL;
    qlnoConcat          * sOptConcat            = NULL;
    qlnxConcat          * sExeConcat            = NULL;
    qlnxCommonInfo      * sFirstChild           = NULL;
    qlnxCommonInfo      * sCurChild             = NULL;
    qlnxRowBlockItem    * sRowBlockItem         = NULL;
    qlnxRowBlockItem    * sOrgRowBlockItem      = NULL;
    knlValueBlockList   * sValueBlockList       = NULL;
    knlValueBlockList   * sLastValueBlockList   = NULL;

    stlInt32              i;
    stlInt32              sFirstChildIdx;
    stlInt32              sCurChildIdx;


    /****************************************************************
     * Paramter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_CONCAT_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * 준비작업
     ****************************************************************/

    /**
     * CONCAT Optimization Node 획득
     */

    sOptConcat = (qlnoConcat *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    for( i = 0; i < sOptConcat->mChildCount; i++ )
    {
        QLND_DATA_OPTIMIZE( aTransID,
                            aDBCStmt,
                            aSQLStmt,
                            sOptConcat->mChildNodeArr[i],
                            aDataArea,
                            aEnv );

    }

    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

 
    /**
     * CONCAT Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxConcat ),
                                (void **) & sExeConcat,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeConcat, 0x00, STL_SIZEOF( qlnxConcat ) );


    /**
     * Iterator를 위한 Read Value Block List 구성
     */

    sExeConcat->mChildCount = sOptConcat->mChildCount;
    sFirstChildIdx = (sOptConcat->mChildNodeArr[0])->mIdx;
    sFirstChild = ((qlnxCommonInfo*)(aDataArea->mExecNodeList[ sFirstChildIdx ].mExecNode));

    STL_DASSERT( sOptConcat->mConcatColList != NULL );
    
    STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                         sOptConcat->mConcatColList,
                                         STL_TRUE,
                                         & sExeConcat->mOutputValueBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );

    
    /**
     * RowId Column 관련 Value Block을 찾아서 Read Value Block List 앞부분에 추가
     */

    if( sOptConcat->mRowIdColExpr != NULL )
    {
        STL_TRY( qlndFindRowIdColumnBlockList( aDataArea->mRowIdColumnBlock,
                                               sOptConcat->mRowIdColExpr,
                                               & sValueBlockList,
                                               aEnv )
                 == STL_SUCCESS );

        sValueBlockList->mNext = sExeConcat->mOutputValueBlock;
        sExeConcat->mOutputValueBlock = sValueBlockList;
    }
    else
    {
        /* Do Nothing */
    }
    

    /**
     * Row Block List 구성 
     */

    sExeConcat->mRowBlockList.mCount = 0;
    sExeConcat->mRowBlockList.mHead = NULL;
    sExeConcat->mRowBlockList.mTail = NULL;

    sOrgRowBlockItem = sFirstChild->mRowBlockList->mHead;
    while( sOrgRowBlockItem != NULL )
    {
        /* Row Block Item 공간 할당 */
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( qlnxRowBlockItem ),
                                    (void **) & sRowBlockItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Row Block 공간 할당 */
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( smlRowBlock ),
                                    (void **) & sRowBlockItem->mRowBlock,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Row Block  초기화 */
        STL_TRY( smlInitRowBlock( sRowBlockItem->mRowBlock,
                                  aDataArea->mBlockAllocCnt,
                                  & QLL_DATA_PLAN( aDBCStmt ),
                                  SML_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Optimization Node 및 Original Row Block, Left Table Row Block 설정 */
        sRowBlockItem->mLeafOptNode = sOrgRowBlockItem->mLeafOptNode;
        sRowBlockItem->mOrgRowBlock = sOrgRowBlockItem->mRowBlock;
        sRowBlockItem->mIsLeftTableRowBlock = sOrgRowBlockItem->mIsLeftTableRowBlock;
        sRowBlockItem->mNext = NULL;

        if( sExeConcat->mRowBlockList.mCount == 0 )
        {
            sExeConcat->mRowBlockList.mHead = sRowBlockItem;
            sExeConcat->mRowBlockList.mTail = sRowBlockItem;
        }
        else
        {
            sExeConcat->mRowBlockList.mTail->mNext = sRowBlockItem;
            sExeConcat->mRowBlockList.mTail = sRowBlockItem;
        }
        sExeConcat->mRowBlockList.mCount++;

        sOrgRowBlockItem = sOrgRowBlockItem->mNext;
    }


    /**
     * Iterator를 위한 Read Value Block List 구성
     */

    STL_TRY( qlndBuildOuterColBlockList( aDataArea,
                                         sOptConcat->mOuterColumnList,
                                         & sExeConcat->mCommonInfo.mOuterColBlock,
                                         & sExeConcat->mCommonInfo.mOuterOrgColBlock,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         aEnv )
             == STL_SUCCESS );

    
    /**
     * Common Info 설정
     */

    sExeConcat->mCommonInfo.mResultColBlock = sExeConcat->mOutputValueBlock;
    if( sOptConcat->mNeedRowBlock == STL_TRUE )
    {
        sExeConcat->mCommonInfo.mRowBlockList = &sExeConcat->mRowBlockList;
    }
    else
    {
        sExeConcat->mCommonInfo.mRowBlockList = NULL;
    }


    /**
     * Concat이 모든 child 노드의 RID BLOCK에 동일하게 접근하기 위해서
     * 모든 FetchInfo의 Rid block을 첫번째 Child의 것으로 통일시킨다
     */
    for( i = 1; i < sOptConcat->mChildCount; i++ )
    {
        sCurChildIdx = (sOptConcat->mChildNodeArr[i])->mIdx;
        sCurChild = ((qlnxCommonInfo*)(aDataArea->mExecNodeList[ sCurChildIdx ].mExecNode));

        sRowBlockItem = sCurChild->mRowBlockList->mHead;
        sOrgRowBlockItem = sFirstChild->mRowBlockList->mHead;
        while( sRowBlockItem != NULL )
        {
            STL_DASSERT( sOrgRowBlockItem != NULL );
            sRowBlockItem->mRowBlock->mRidBlock =
                sOrgRowBlockItem->mRowBlock->mRidBlock;
            sRowBlockItem->mRowBlock->mScnBlock =
                sOrgRowBlockItem->mRowBlock->mScnBlock;

            sRowBlockItem = sRowBlockItem->mNext;
            sOrgRowBlockItem = sOrgRowBlockItem->mNext;
        }
    }


    /*
     * Hash Key Value block 설정
     */

    sLastValueBlockList = NULL;
    sRowBlockItem = sFirstChild->mRowBlockList->mHead;
    while( sRowBlockItem != NULL )
    {
        /* Key Value Block 할당 */
        STL_TRY( knlInitBlockNoBuffer( &sValueBlockList,
                                       STL_LAYER_SQL_PROCESSOR,
                                       aDataArea->mBlockAllocCnt,
                                       DTL_TYPE_BINARY,
                                       & QLL_DATA_PLAN( aDBCStmt ),
                                       KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        /* Key Value Block의 RID 정보 설정 */
        sValueBlockList->mValueBlock->mAllocBufferSize = STL_SIZEOF(smlRid);
        for( i = 0; i < aDataArea->mBlockAllocCnt; i++ )
        {
            sValueBlockList->mValueBlock->mDataValue[i].mBufferSize = STL_SIZEOF(smlRid);
            sValueBlockList->mValueBlock->mDataValue[i].mValue = 
                &sRowBlockItem->mRowBlock->mRidBlock[i];
            sValueBlockList->mValueBlock->mDataValue[i].mLength = STL_SIZEOF(smlRid);
        }

        /* Key Value Block 연결 */
        if( sLastValueBlockList == NULL )
        {
            sExeConcat->mKeyValueBlock = sValueBlockList;
        }
        else
        {
            sLastValueBlockList->mNext = sValueBlockList;
        }
        sLastValueBlockList = sValueBlockList;

        sRowBlockItem = sRowBlockItem->mNext;
    }

    sExeConcat->mKeyColCount = sFirstChild->mRowBlockList->mCount;

    /* Hash Instant Table에 Insert시 사용될 Row Block 생성 */
    sExeConcat->mHashRowBlock.mBlockSize = aDataArea->mBlockAllocCnt;
    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( smlRid ) * aDataArea->mBlockAllocCnt,
                                (void **) & sExeConcat->mHashRowBlock.mRidBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( smlScn ) * aDataArea->mBlockAllocCnt,
                                (void **) & sExeConcat->mHashRowBlock.mScnBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Duplicate Check용 Value Block List 생성 */
    STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                & sExeConcat->mDupFlagBlockList,
                                aDataArea->mBlockAllocCnt,
                                STL_TRUE, /* IsSepBuff */
                                STL_LAYER_SQL_PROCESSOR,
                                0, /* TableID */
                                0, /* Column Order */
                                DTL_TYPE_BOOLEAN,
                                STL_SIZEOF( stlBool ),
                                DTL_SCALE_NA,
                                DTL_STRING_LENGTH_UNIT_NA,
                                DTL_INTERVAL_INDICATOR_NA,
                                & QLL_DATA_PLAN( aDBCStmt ),
                                aEnv )
             == STL_SUCCESS );

    /* Hash Table Attribute 설정 */
    sExeConcat->mHashTableAttr = sOptConcat->mHashTableAttr;
    

    /****************************************************************
     * 기타 정보 설정
     ****************************************************************/

    /* First Fetch 여부 */
    sExeConcat->mIsFirstFetch = STL_TRUE;

    /* EOF (더이상 Fetch할 것이 없는지) 여부 */
    sExeConcat->mIsEOF = STL_FALSE;


    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_CONCAT_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeConcat;

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



/** @} qlnd */

