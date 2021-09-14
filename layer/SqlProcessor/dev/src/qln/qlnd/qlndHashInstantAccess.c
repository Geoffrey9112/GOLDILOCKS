/*******************************************************************************
 * qlndHashInstantAccess.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndHashInstantAccess.c 10973 2014-01-16 07:17:19Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndHashInstantAccess.c
 * @brief SQL Processor Layer Data Optimization Node - HASH INSTANT ACCESS
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


#if 0
/**
 * @brief HASH INSTANT ACCESS Execution node에 대한 Data 정보를 구축한다.
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
 *    <BR> 상위 Query Node를 가지지 않는다.
 */
stlStatus qlndDataOptimizeHashInstantAccess( smlTransId              aTransID,
                                             qllDBCStmt            * aDBCStmt,
                                             qllStatement          * aSQLStmt,
                                             qllOptimizationNode   * aOptNode,
                                             qllDataArea           * aDataArea,
                                             qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode       = NULL;
    qlnoInstant       * sOptHashInstant = NULL;
    qlnxInstant       * sExeHashInstant = NULL;
    smlFetchInfo      * sFetchInfo      = NULL;
    qlnxCommonInfo    * sCommonInfo     = NULL;

    knlValueBlockList   sTempColBlock;
    knlValueBlockList * sLastColBlock   = NULL;
    knlValueBlockList * sNewBlock       = NULL;
    knlValueBlockList * sCurBlock       = NULL;
    knlValueBlockList * sLastTableBlock = NULL;
    qlnxRowBlockItem  * sRowBlockItem   = NULL;
    smlRowBlock       * sRowBlock       = NULL;
    dtlDataValue      * sScnValue       = NULL;
    dtlDataValue      * sRidValue       = NULL;
    stlInt32            sColOrder       = 0;
    stlInt32            sLoop           = 0;
    stlInt32            sRowIdx         = 0;
    stlInt32            sKeyColCnt      = 0;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * HASH INSTANT ACCESS Optimization Node 획득
     */

    sOptHashInstant = (qlnoInstant *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    QLND_DATA_OPTIMIZE( aTransID,
                        aDBCStmt,
                        aSQLStmt,
                        sOptHashInstant->mChildNode,
                        aDataArea,
                        aEnv );


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );

    
    /**
     * HASH INSTANT ACCESS Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxInstant ),
                                (void **) & sExeHashInstant,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeHashInstant, 0x00, STL_SIZEOF( qlnxInstant ) );


    /**
     * HASH INSTANT TABLE Iterator 에 대한 Fetch Info 설정
     */

    sFetchInfo = & sExeHashInstant->mFetchInfo;


    /**
     * Read Row Block 구성
     */

    sTempColBlock.mNext = NULL;
    sLastColBlock = & sTempColBlock;
    sCommonInfo = (qlnxCommonInfo*) aDataArea->mExecNodeList[ sOptHashInstant->mChildNode->mIdx ].mExecNode;
        
    if( sCommonInfo->mRowBlockList == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        if( sCommonInfo->mRowBlockList->mCount > 0 )
        {
            sRowBlockItem = sCommonInfo->mRowBlockList->mHead;
            sColOrder     = ( sOptHashInstant->mInstantNode->mKeyColCnt +
                              sOptHashInstant->mInstantNode->mRecColCnt );
            
            for( sLoop = sCommonInfo->mRowBlockList->mCount ; sLoop > 0 ; sLoop-- )
            {
                /* SCN value block 생성 */
                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & sNewBlock,
                                            aDataArea->mBlockAllocCnt,
                                            STL_TRUE, /* IsSepBuff */
                                            STL_LAYER_SQL_PROCESSOR,
                                            0, /* TableID */
                                            sColOrder,
                                            DTL_TYPE_BINARY,
                                            STL_SIZEOF( smlScn ),
                                            DTL_SCALE_NA,
                                            DTL_STRING_LENGTH_UNIT_NA,
                                            DTL_INTERVAL_INDICATOR_NA,
                                            & QLL_DATA_PLAN( aDBCStmt ),
                                            s )
                         == STL_SUCCESS );

                sScnValue = sNewBlock->mValueBlock->mDataValue;

                /* link */
                KNL_LINK_BLOCK_LIST( sLastColBlock, sNewBlock );
                sLastColBlock = sNewBlock;
                sColOrder++;

                /* RID value block 생성 */
                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & sNewBlock,
                                            aDataArea->mBlockAllocCnt,
                                            STL_TRUE, /* IsSepBuff */
                                            STL_LAYER_SQL_PROCESSOR,
                                            0, /* TableID */
                                            sColOrder,
                                            DTL_TYPE_BINARY,
                                            STL_SIZEOF( smlRid ),
                                            DTL_SCALE_NA,
                                            DTL_STRING_LENGTH_UNIT_NA,
                                            DTL_INTERVAL_INDICATOR_NA,
                                            & QLL_DATA_PLAN( aDBCStmt ),
                                            aEnv )
                         == STL_SUCCESS );

                sRidValue = sNewBlock->mValueBlock->mDataValue;

                /* link */
                KNL_LINK_BLOCK_LIST( sLastColBlock, sNewBlock );
                sLastColBlock = sNewBlock;
                sColOrder++;

                /* Row Block(smlRowBlock)과 Value Block's Data Value 연결 */
                sRowBlock = sRowBlockItem->mRowBlock;
                for( sRowIdx = 0 ; sRowIdx < aDataArea->mBlockAllocCnt ; sRowIdx++ )
                {
                    sScnValue->mValue = & sRowBlock->mScnBlock[ sRowIdx ];
                    sScnValue->mLength = STL_SIZEOF( smlScn );
                    sScnValue++;
            
                    sRidValue->mValue = & sRowBlock->mRidBlock[ sRowIdx ];
                    sRidValue->mLength = STL_SIZEOF( smlRid );
                    sRidValue++;
                }

                sRowBlockItem = sRowBlockItem->mNext;
            }
        }
        else
        {
            /* Do Nothing */
        }
    }


    /**
     * Iterator를 위한 Hash Key Value Block List 구성
     */

    STL_DASSERT( sOptHashInstant->mInstantNode->mKeyColList != NULL );
    
    if( sOptHashInstant->mInstantNode->mKeyColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptHashInstant->mInstantNode->mKeyColList,
                                             STL_TRUE,
                                             & sExeHashInstant->mKeyColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sExeHashInstant->mKeyColBlock = NULL;
    }


    /**
     * Iterator를 위한 Record Value Block List 구성
     */

    STL_DASSERT( sOptHashInstant->mInstantNode->mRecColList != NULL );
    
    if( sOptHashInstant->mInstantNode->mRecColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptHashInstant->mInstantNode->mRecColList,
                                             STL_TRUE,
                                             & sExeHashInstant->mRecColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );

        /* Record Column List의 마지막 Value Block 얻기 */
        sLastColBlock = sExeHashInstant->mRecColBlock;
        while( sLastColBlock->mNext != NULL )
        {
            sLastColBlock = sLastColBlock->mNext;
        }

        /* Read Row Block 연결 */
        sLastColBlock->mNext = sTempColBlock.mNext;
    }
    else
    {
        sExeHashInstant->mRecColBlock = sTempColBlock.mNext;
    }


    /**
     * Iterator를 위한 Read Value Block List 구성
     */

    STL_DASSERT( sOptHashInstant->mInstantNode->mReadColList != NULL );
    
    if( sOptHashInstant->mInstantNode->mReadColList->mCount > 0 )
    {
        STL_TRY( qlndBuildTableColBlockList( aDataArea,
                                             sOptHashInstant->mInstantNode->mReadColList,
                                             STL_TRUE,
                                             & sExeHashInstant->mReadColBlock,
                                             & QLL_DATA_PLAN( aDBCStmt ),
                                             aEnv )
                 == STL_SUCCESS );


        /**
         * Read Column Block을 INDEX와 TABLE로 구분
         */

        sLastTableBlock = NULL;
        sLastColBlock = NULL;
        sKeyColCnt = sOptHashInstant->mInstantNode->mKeyColList->mCount;
        sCurBlock  = sExeHashInstant->mReadColBlock;
        sExeHashInstant->mIndexReadColBlock = NULL;
        sExeHashInstant->mTableReadColBlock = NULL;

        while( sCurBlock != NULL )
        {
            /* Share Block 할당 */
            STL_TRY( knlInitShareBlock( & sNewBlock,
                                        sCurBlock,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        KNL_ENV( aEnv ) )
                     == STL_SUCCESS );

            if( KNL_GET_BLOCK_COLUMN_ORDER( sCurBlock ) < sKeyColCnt )
            {
                /**
                 * Index Read Column Block 설정
                 */

                if( sLastColBlock == NULL )
                {
                    sExeHashInstant->mIndexReadColBlock = sNewBlock;
                }
                else
                {
                    KNL_LINK_BLOCK_LIST( sLastColBlock, sNewBlock );
                }
                sLastColBlock = sNewBlock;
            }
            else
            {
                /**
                 * Table Read Column Block 설정
                 */

                if( sLastTableBlock == NULL )
                {
                    sExeHashInstant->mTableReadColBlock = sNewBlock;
                }
                else
                {
                    KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock );
                }
                sLastTableBlock = sNewBlock;
            }

            sCurBlock = sCurBlock->mNext;
        }

        /* Read Column List의 마지막 Value Block 얻기 */
        sLastColBlock = sExeHashInstant->mReadColBlock;
        while( sLastColBlock->mNext != NULL )
        {
            sLastColBlock = sLastColBlock->mNext;
        }

        /* Read Row Block 연결 */
        sLastColBlock->mNext = sTempColBlock.mNext;
    }
    else
    {
        sExeHashInstant->mReadColBlock = sTempColBlock.mNext;
    }
    sFetchInfo->mColList = sExeHashInstant->mReadColBlock;


    /**
     * Insert Value Block List 설정
     */

    sLastTableBlock = NULL;

    sCurBlock = sExeHashInstant->mKeyColBlock;
    while( sCurBlock != NULL )
    {
        STL_TRY( knlInitShareBlock( & sNewBlock,
                                    sCurBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        if( sLastTableBlock == NULL )
        {
            sExeHashInstant->mInsertColBlock = sNewBlock;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock );
        }
        sLastTableBlock = sNewBlock;

        sCurBlock = sCurBlock->mNext;
    }

    STL_DASSERT( sLastTableBlock != NULL );

    sCurBlock = sExeHashInstant->mRecColBlock;
    while( sCurBlock != NULL )
    {
        STL_TRY( knlInitShareBlock( & sNewBlock,
                                    sCurBlock,
                                    & QLL_DATA_PLAN( aDBCStmt ),
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        KNL_LINK_BLOCK_LIST( sLastTableBlock, sNewBlock );
        sLastTableBlock = sNewBlock;

        sCurBlock = sCurBlock->mNext;
    }

    
    /**
     * Row Block 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( smlRowBlock ),
                                (void **) & sFetchInfo->mRowBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( smlInitRowBlock( sFetchInfo->mRowBlock,
                              aDataArea->mBlockAllocCnt,
                              & QLL_DATA_PLAN( aDBCStmt ),
                              SML_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Hash 관련 정보 & Fetch Record 설정
     */

    STL_TRY( qlndGetHashInfoForHashInstant( aDBCStmt,
                                            aSQLStmt,
                                            aDataArea,
                                            sOptHashInstant,
                                            sExeHashInstant,
                                            aEnv )
             == STL_SUCCESS );


    /**
     * Fetch 정보 초기화
     */

    sFetchInfo->mSkipCnt         = 0;
    sFetchInfo->mFetchCnt        = QLL_FETCH_COUNT_MAX;
    sFetchInfo->mIsEndOfScan     = STL_FALSE;


    /**
     * Materialize 정보 초기화
     */

    sExeHashInstant->mNeedMaterialize = STL_TRUE;


    /**
     * Rebuild 정보 설정
     */

    sExeHashInstant->mNeedRebuild = sOptHashInstant->mNeedRebuild;


    /**
     * Prepare Hash Key 정보 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptHashInstant->mPrepareRangeStack,
                                           & sExeHashInstant->mPreRangeEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Prepare Key Filter 정보 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptHashInstant->mPrepareKeyFilterStack,
                                           & sExeHashInstant->mPreKeyFilterEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );


    /**
     * Prepare Filter 정보 설정
     */

    STL_TRY( qlndMakePrepareStackEvalInfo( aDataArea->mExprDataContext,
                                           sOptHashInstant->mPrepareFilterStack,
                                           & sExeHashInstant->mPreFilterEvalInfo,
                                           & QLL_DATA_PLAN( aDBCStmt ),
                                           aEnv )
             == STL_SUCCESS );

    
    /**
     * Common Info 설정
     */

    sExeHashInstant->mCommonInfo.mResultColBlock   = sExeHashInstant->mReadColBlock;
    sExeHashInstant->mCommonInfo.mRowBlockList     = sCommonInfo->mRowBlockList;
    sExeHashInstant->mCommonInfo.mOuterColBlock    = NULL;
    sExeHashInstant->mCommonInfo.mOuterOrgColBlock = NULL;


    /***************************************************************************
     * Execution Node 설정
     **************************************************************************/

    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE;
    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeHashInstant;

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
#endif


/**
 * @brief HASH INSTANT ACCESS Optimization Node로 부터 Index Scan에 필요한 hash filter 정보를 구성한다.
 * @param[in]      aDBCStmt           DBC Statement
 * @param[in,out]  aSQLStmt           SQL Statement
 * @param[in]      aDataArea          Data Area (Data Optimization 결과물)
 * @param[in]      aOptHashInstant    Hash Instant Access Optimization Node
 * @param[in,out]  aExeHashInstant    Hash Instant Access Execution Node
 * @param[in]      aEnv               Environment
 */
stlStatus qlndGetHashInfoForHashInstant( qllDBCStmt     * aDBCStmt,
                                         qllStatement   * aSQLStmt,
                                         qllDataArea    * aDataArea,
                                         qlnoInstant    * aOptHashInstant,
                                         qlnxInstant    * aExeHashInstant,
                                         qllEnv         * aEnv )
{
    qlnoInstant         * sOptHashInstant       = NULL;
    qlnxInstant         * sExeHashInstant       = NULL;

    qloHashScanInfo     * sHashScanInfo         = NULL;

    smlFetchInfo        * sFetchInfo            = NULL;

    qlvInitExpression   * sFilterExpr           = NULL;
    qlvInitFunction     * sFuncCode             = NULL;
    dtlDataValue        * sDataValue            = NULL;
    
    stlInt32              sLoop                 = 0;
    stlInt32              sExprListCnt          = 0;
    knlDataValueList    * sConstValList         = NULL;

    knlValueBlockList     * sLeftConstBlockList = NULL;
    knlValueBlockList     * sLeftConstBlockLast = NULL;
    knlValueBlockList     * sLeftConstBlockItem = NULL;
    

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptHashInstant != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExeHashInstant != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Node 획득
     */

    sOptHashInstant = aOptHashInstant;
    sExeHashInstant = aExeHashInstant;
    sHashScanInfo  = sOptHashInstant->mHashScanInfo;


    /**********************************************************
     * Rewrite Filter 
     **********************************************************/

    /**
     * @todo Predicate 생성 여부 판단
     */


    /**
     * @todo Range & Filter 재구성
     * @remark Min/Max Range 및 Filter 의 Constant Value 를 고려하여,
     *    <BR> 불필요한 조건들을 제거한다.
     *    <BR> 남은 조건들만을 가지고 Physical / Logical Filter 구성
     *    <BR> 
     *    <BR> Bind Parameter에 대한 value가 설정되어 있기 때문에
     *    <BR> 정확한 Range 및 Filter를 구성할 수 있다.
     *    <BR> Scan 여부를 판가름 할 수도 있는 튜닝 요소임.
     */

    
    

    /**
     * @todo MIN Range를 분석하여 불필요한 조건들 제거
     */

    
    /**
     * @todo MAX Range를 분석하여 불필요한 조건들 제거
     */


    /**
     * @todo Physical Key Filter를 분석하여 불필요한 조건들 제거
     */

    
    /**
     * @todo Logical Key Filter를 분석하여 불필요한 조건들 제거
     */


    /**
     * @todo Physical Table Filter를 분석하여 불필요한 조건들 제거
     */


    /**
     * @todo Logical Table Filter를 분석하여 불필요한 조건들 제거
     */
    

    /**
     * 항상 같은 값을 반환하는 경우 Ragne Predicate 생성하지 않음 (Optimization 정보)
     */

    sFetchInfo = & sExeHashInstant->mFetchInfo;


    /**********************************************************
     * Key Compare List 설정
     **********************************************************/

    sFetchInfo->mKeyCompList = NULL;

    
    /**********************************************************
     * Fetch 정보 설정
     **********************************************************/

    sFetchInfo->mRange           = NULL;
    sFetchInfo->mPhysicalFilter  = NULL;
    sFetchInfo->mLogicalFilter   = NULL;

    sFetchInfo->mFilterEvalInfo  = NULL;

    sFetchInfo->mFetchRecordInfo = NULL;

    /**********************************************************
     * Hash를 위한 Fetch Info 구성
     **********************************************************/
    
    /**
     * Hash 관련 Compare List 구성
     */

    STL_DASSERT( sHashScanInfo->mHashFilterExpr != NULL );


    /**
     * Filter 공간 할당
     */

    sExprListCnt = sHashScanInfo->mHashColCount;
    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( knlDataValueList ) * sExprListCnt,
                                (void **) & sConstValList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Filter 구성
     */

    for( sLoop = 0; sLoop < sExprListCnt; sLoop++ )
    {
        sFilterExpr = sHashScanInfo->mHashFilterExpr[sLoop]->mHead->mExprPtr;

        /* Column Order 기준으로 Filter entry 정렬 */
        STL_DASSERT( sFilterExpr->mType == QLV_EXPR_TYPE_FUNCTION );

        /* function 으로 구성된 filter */
        sFuncCode = sFilterExpr->mExpr.mFunction;
        STL_DASSERT( sFuncCode->mArgCount == 2 );

        if( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN )
        {
            /**
             * column vs constant value
             */

            STL_DASSERT( sFuncCode->mArgs[1]->mType != QLV_EXPR_TYPE_INNER_COLUMN );

            /* constant value 설정 */
            if( aDataArea->mExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].mCast != NULL )
            {
                sDataValue = & aDataArea->mExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].mCast->mCastResultBuf;

                STL_TRY( knlInitBlockWithDataValue(
                             & sLeftConstBlockItem,
                             sDataValue,
                             sDataValue->mType,
                             STL_LAYER_SQL_PROCESSOR,
                             gResultDataTypeDef[ sDataValue->mType ].mArgNum1,
                             gResultDataTypeDef[ sDataValue->mType ].mArgNum2,
                             gResultDataTypeDef[ sDataValue->mType ].mStringLengthUnit,
                             gResultDataTypeDef[ sDataValue->mType ].mIntervalIndicator,
                             &QLL_DATA_PLAN( aDBCStmt ),
                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
            else
            {
                if( (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                    (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_VALUE) ||
                    (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                    (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_OUTER_COLUMN) )
                {
                    sDataValue = aDataArea->mExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].
                        mInfo.mValueInfo->mDataValue;

                    STL_TRY( knlInitShareBlockFromBlock(
                                 & sLeftConstBlockItem,
                                 aDataArea->mExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].mInfo.mValueInfo,
                                 &QLL_DATA_PLAN( aDBCStmt ),
                                 KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else if( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
                {
                    sDataValue = & aDataArea->mExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].
                        mInfo.mListFunc->mResultValue;

                    STL_TRY( knlInitBlockWithDataValue(
                                 & sLeftConstBlockItem,
                                 sDataValue,
                                 sDataValue->mType,
                                 STL_LAYER_SQL_PROCESSOR,
                                 gResultDataTypeDef[ sDataValue->mType ].mArgNum1,
                                 gResultDataTypeDef[ sDataValue->mType ].mArgNum2,
                                 gResultDataTypeDef[ sDataValue->mType ].mStringLengthUnit,
                                 gResultDataTypeDef[ sDataValue->mType ].mIntervalIndicator,
                                 &QLL_DATA_PLAN( aDBCStmt ),
                                 KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_DASSERT( ( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_FUNCTION ) ||
                                 ( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_CAST ) ||
                                 ( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_CASE_EXPR ) );
                    
                    sDataValue = & aDataArea->mExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].
                        mInfo.mFuncData->mResultValue;

                    STL_TRY( knlInitBlockWithDataValue(
                                 & sLeftConstBlockItem,
                                 sDataValue,
                                 sDataValue->mType,
                                 STL_LAYER_SQL_PROCESSOR,
                                 gResultDataTypeDef[ sDataValue->mType ].mArgNum1,
                                 gResultDataTypeDef[ sDataValue->mType ].mArgNum2,
                                 gResultDataTypeDef[ sDataValue->mType ].mStringLengthUnit,
                                 gResultDataTypeDef[ sDataValue->mType ].mIntervalIndicator,
                                 &QLL_DATA_PLAN( aDBCStmt ),
                                 KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                }
            }

            sConstValList[sLoop].mConstVal = sDataValue;
        }
        else
        {
            /**
             * column vs constant value : reverse function
             */

            STL_DASSERT( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_INNER_COLUMN );
            STL_DASSERT( sFuncCode->mArgs[0]->mType != QLV_EXPR_TYPE_INNER_COLUMN );

            /* constant value 설정 */
            if( aDataArea->mExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].mCast != NULL )
            {
                sDataValue = & aDataArea->mExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].mCast->mCastResultBuf;

                STL_TRY( knlInitBlockWithDataValue(
                             & sLeftConstBlockItem,
                             sDataValue,
                             sDataValue->mType,
                             STL_LAYER_SQL_PROCESSOR,
                             gResultDataTypeDef[ sDataValue->mType ].mArgNum1,
                             gResultDataTypeDef[ sDataValue->mType ].mArgNum2,
                             gResultDataTypeDef[ sDataValue->mType ].mStringLengthUnit,
                             gResultDataTypeDef[ sDataValue->mType ].mIntervalIndicator,
                             &QLL_DATA_PLAN( aDBCStmt ),
                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
            else
            {
                if( (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                    (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_VALUE) ||
                    (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                    (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_OUTER_COLUMN) )
                {
                    sDataValue = aDataArea->mExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].
                        mInfo.mValueInfo->mDataValue;

                    STL_TRY( knlInitShareBlockFromBlock(
                                 & sLeftConstBlockItem,
                                 aDataArea->mExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].mInfo.mValueInfo,
                                 &QLL_DATA_PLAN( aDBCStmt ),
                                 KNL_ENV(aEnv) )
                             == STL_SUCCESS );
                }
                else if( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
                {
                    sDataValue = & aDataArea->mExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].
                        mInfo.mListFunc->mResultValue;

                    STL_TRY( knlInitBlockWithDataValue(
                                 & sLeftConstBlockItem,
                                 sDataValue,
                                 sDataValue->mType,
                                 STL_LAYER_SQL_PROCESSOR,
                                 gResultDataTypeDef[ sDataValue->mType ].mArgNum1,
                                 gResultDataTypeDef[ sDataValue->mType ].mArgNum2,
                                 gResultDataTypeDef[ sDataValue->mType ].mStringLengthUnit,
                                 gResultDataTypeDef[ sDataValue->mType ].mIntervalIndicator,
                                 &QLL_DATA_PLAN( aDBCStmt ),
                                 KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_DASSERT( ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_FUNCTION ) ||
                                 ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_CAST ) ||
                                 ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_CASE_EXPR ) );
                    
                    sDataValue = & aDataArea->mExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].
                        mInfo.mFuncData->mResultValue;

                    STL_TRY( knlInitBlockWithDataValue(
                                 & sLeftConstBlockItem,
                                 sDataValue,
                                 sDataValue->mType,
                                 STL_LAYER_SQL_PROCESSOR,
                                 gResultDataTypeDef[ sDataValue->mType ].mArgNum1,
                                 gResultDataTypeDef[ sDataValue->mType ].mArgNum2,
                                 gResultDataTypeDef[ sDataValue->mType ].mStringLengthUnit,
                                 gResultDataTypeDef[ sDataValue->mType ].mIntervalIndicator,
                                 &QLL_DATA_PLAN( aDBCStmt ),
                                 KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                }
            }

            sConstValList[sLoop].mConstVal = sDataValue;
        }

        /******************************************************************
         * Blocked Join 을 위한 Left Key Block List 구성 
         ******************************************************************/

        if ( sLeftConstBlockLast == NULL )
        {
            sLeftConstBlockList = sLeftConstBlockItem;
        }
        else
        {
            sLeftConstBlockLast->mNext = sLeftConstBlockItem;
        }
        sLeftConstBlockLast = sLeftConstBlockItem;
        
        
        if( sLoop + 1 < sExprListCnt )
        {
            sConstValList[sLoop].mNext = &sConstValList[sLoop+1];
        }
        else
        {
            sConstValList[sLoop].mNext = NULL;
        }
    }

    /**
     * Left 정보를 연결 
     */
    
    sExeHashInstant->mLeftConstBlockList = sLeftConstBlockList;
    sExeHashInstant->mLeftConstValueList = sConstValList;


    /**
     * Physical Table Filter 구성
     */

    STL_DASSERT( sHashScanInfo->mPhysicalTableFilterExprList != NULL );

    if( sHashScanInfo->mPhysicalTableFilterExprList->mCount > 0 )
    {
        STL_TRY( qlndMakePhysicalFilterFromExprListForSortInstant(
                     sHashScanInfo->mPhysicalTableFilterExprList,
                     sFetchInfo->mColList,
                     aDataArea->mExprDataContext,
                     STL_FALSE,
                     aOptHashInstant,
                     & QLL_DATA_PLAN( aDBCStmt ),
                     & sFetchInfo->mPhysicalFilter,
                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sFetchInfo->mPhysicalFilter = NULL;
    }


    /**
     * Logical Table Filter 구성
     */

    if( sOptHashInstant->mTableLogicalStack == NULL )
    {
        sFetchInfo->mLogicalFilter = NULL;
        sFetchInfo->mFilterEvalInfo = NULL;
    }
    else
    {
        sFetchInfo->mLogicalFilter = sOptHashInstant->mTableLogicalStack;
        
        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlExprEvalInfo ),
                                    (void **) & sFetchInfo->mFilterEvalInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sFetchInfo->mFilterEvalInfo->mExprStack =
            sOptHashInstant->mTableLogicalStack;
        sFetchInfo->mFilterEvalInfo->mContext =
            aDataArea->mExprDataContext;
        sFetchInfo->mFilterEvalInfo->mResultBlock =
            aDataArea->mPredResultBlock;
        sFetchInfo->mFilterEvalInfo->mBlockIdx = 0;
        sFetchInfo->mFilterEvalInfo->mBlockCount = 0;
    }

    sFetchInfo->mFetchRecordInfo = NULL;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnd */
