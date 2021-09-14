/*******************************************************************************
 * qlnxGroup.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlnxGroup.c 13531 2014-09-04 02:10:50Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlnxGroup.c
 * @brief SQL Processor Layer Execution Node - GROUP
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlnx
 * @{
 */


/**
 * @brief GROUP node를 초기화한다.
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
stlStatus qlnxInitializeGroup( smlTransId              aTransID,
                               smlStatement          * aStmt,
                               qllDBCStmt            * aDBCStmt,
                               qllStatement          * aSQLStmt,
                               qllOptimizationNode   * aOptNode,
                               qllDataArea           * aDataArea,
                               qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode         = NULL;
    qlnoGroup             * sOptGroup         = NULL;
    qlnxGroup             * sExeGroup         = NULL;
    qlnfAggrDistExecInfo  * sAggrDistExecInfo = NULL;
    stlInt32                sLoop             = 0;
    smlIndexAttr            sHashTableAttr;
    

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * GROUP Optimization Node 획득
     */

    sOptGroup = (qlnoGroup *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );



    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * GROUP Execution Node 획득
     */

    sExeGroup = (qlnxGroup *) sExecNode->mExecNode;

    
    /**
     * Hash Attr 셋팅
     */
    
    stlMemset( &sHashTableAttr, 0x00, STL_SIZEOF( smlIndexAttr ) );

    /* Logging 하지 않음*/
    sHashTableAttr.mValidFlags |= SML_INDEX_LOGGING_YES;
    sHashTableAttr.mLoggingFlag = STL_FALSE;

    /* Uniqueness 여부 */
    sHashTableAttr.mValidFlags |= SML_INDEX_UNIQUENESS_YES;
    sHashTableAttr.mUniquenessFlag = STL_TRUE;
    
    /* Primary Key 여부 */
    sHashTableAttr.mPrimaryFlag = STL_FALSE;


    /**
     * Aggregation Distinct를 위한 Instant Table 생성
     */

    sAggrDistExecInfo = sExeGroup->mAggrDistExecInfo;

    for( sLoop = 0 ; sLoop < sOptGroup->mAggrDistFuncCnt ; sLoop++ )
    {
        stlMemset( & sAggrDistExecInfo->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );

        STL_TRY( qlnxCreateHashInstantTable( aTransID,
                                             aStmt,
                                             & sAggrDistExecInfo->mInstantTable,
                                             NULL,
                                             sAggrDistExecInfo->mInputArgCnt + sOptGroup->mInstantNode->mKeyColCnt,
                                             sAggrDistExecInfo->mInsertColList,
                                             & sHashTableAttr,
                                             aEnv )
                 == STL_SUCCESS );

        sAggrDistExecInfo++;
    }


    /**
     * Scan 정보
     */
    
    sExeGroup->mHasFalseFilter = STL_FALSE;
    sExeGroup->mIsFirstExecute = STL_TRUE;
    sExeGroup->mIsSwitched = STL_FALSE;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief GROUP node를 수행한다.
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
stlStatus qlnxExecuteGroup( smlTransId              aTransID,
                            smlStatement          * aStmt,
                            qllDBCStmt            * aDBCStmt,
                            qllStatement          * aSQLStmt,
                            qllOptimizationNode   * aOptNode,
                            qllDataArea           * aDataArea,
                            qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode         = NULL;
    qlnoGroup             * sOptGroup         = NULL;
    qlnxGroup             * sExeGroup         = NULL;
    stlInt32                sLoop             = 0;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * GROUP Optimization Node 획득
     */

    sOptGroup = (qlnoGroup *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * GROUP Execution Node 획득
     */

    sExeGroup = (qlnxGroup *) sExecNode->mExecNode;


    /*****************************************
     * Filter
     ****************************************/

    /**
     * @todo 불필요한 요소를 제거하여 Filter 재구성 (확장 필요)
     */
    
    sExeGroup->mHasFalseFilter =
        qlfHasFalseFilter( sOptGroup->mTotalFilterExpr,
                           aDataArea );

    sExeGroup->mIsFirstExecute = STL_FALSE;
    sExeGroup->mIsExistGroup = STL_FALSE;
    sExeGroup->mIsNeedFetch = STL_TRUE;
    sExeGroup->mEOF = STL_FALSE;
    sExeGroup->mCurrBlockIdx = 0;
    sExeGroup->mReadBlockCnt = 0;


    /*****************************************
     * Group Value 초기화
     ****************************************/

    for( sLoop = 0 ; sLoop < sOptGroup->mInstantNode->mKeyColCnt ; sLoop++ )
    {
        DTL_SET_NULL( & sExeGroup->mGroupColValue[ sLoop ] );
    }
    

    /*****************************************
     * Child Node Execution
     ****************************************/

    /**
     * 하위 노드 Execute
     */

    QLNX_EXECUTE_NODE( aTransID,
                       aStmt,
                       aDBCStmt,
                       aSQLStmt,
                       aDataArea->mExecNodeList[ sOptGroup->mChildNode->mIdx ].mOptNode,
                       aDataArea,
                       aEnv );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief GROUP node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchGroup( qlnxFetchNodeInfo     * aFetchNodeInfo,
                          stlInt64              * aUsedBlockCnt,
                          stlBool               * aEOF,
                          qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode         = NULL;
    qlnoGroup             * sOptGroup         = NULL;
    qlnxGroup             * sExeGroup         = NULL;
    qlnfAggrExecInfo      * sAggrExecInfo     = NULL;
    qlnfAggrDistExecInfo  * sAggrDistExecInfo = NULL;
    
    knlValueBlockList     * sCurColBlock      = NULL;
    knlValueBlockList     * sKeyColBlock      = NULL;
    knlValueBlockList     * sRecColBlock      = NULL;
    knlValueBlockList     * sSwitchBlock      = NULL;
    knlValueBlockList     * sGroupKeyBlock    = NULL;
    knlValueBlockList     * sBlockList        = NULL;
    knlValueBlockList     * sSwitchBlockList  = NULL;
    knlExprStack         ** sExprStack        = NULL;
    knlExprEvalInfo         sEvalInfo;

    dtlDataValue          * sCurDataValue     = NULL;
    dtlDataValue          * sDataValue        = NULL;

    stlBool                 sIsSameGroup      = STL_FALSE;
    stlInt64                sRemainFetchCnt   = 0;
    stlInt64                sRemainSkipCnt    = 0;
    stlInt64                sUsedBlockCnt     = 0;
    stlInt32                sLoop             = 0;
    stlInt32                sKeyColIdx        = 0;
    stlInt32                sRecColIdx        = 0;
    stlInt32                sGroupKeyIdx      = 0;
    stlInt32                sStartIdx         = 0;
    stlInt32                sGroupRecCnt      = 0;
    stlInt32                sLength           = 0;
    
    stlInt64                sFetchedCnt       = 0;
    stlInt64                sSkippedCnt       = 0;
    qlnxFetchNodeInfo       sLocalFetchInfo;
    
    QLL_OPT_DECLARE( stlTime sBeginFetchTime );
    QLL_OPT_DECLARE( stlTime sEndFetchTime );
    QLL_OPT_DECLARE( stlTime sFetchTime );

    QLL_OPT_DECLARE( stlTime sBeginExceptTime );
    QLL_OPT_DECLARE( stlTime sEndExceptTime );
    QLL_OPT_DECLARE( stlTime sExceptTime );

    QLL_OPT_SET_VALUE( sFetchTime, 0 );
    QLL_OPT_SET_VALUE( sExceptTime, 0 );

    
#define _QLNX_SWITCH_TOTAL_COLUMN_BLOCKS( )                                             \
    {                                                                                   \
        if( sExeGroup->mIsSwitched == STL_FALSE )                                       \
        {                                                                               \
            /* Switch */                                                                \
            sBlockList       = sExeGroup->mTotalColBlock;                               \
            sSwitchBlockList = sExeGroup->mSwitchBlock;                                 \
                                                                                        \
            while( sBlockList )                                                         \
            {                                                                           \
                QLNX_SWAP_DATA_VALUE_ARRAY( sBlockList, sSwitchBlockList, sDataValue ); \
                                                                                        \
                sBlockList    = sBlockList->mNext;                                      \
                sSwitchBlockList = sSwitchBlockList->mNext;                             \
            }                                                                           \
                                                                                        \
            sExeGroup->mIsSwitched = STL_TRUE;                                          \
        }                                                                               \
    }

#define _QLNX_RESET_SWITCH_TOTAL_COLUMN_BLOCKS( )                                       \
    {                                                                                   \
        if( sExeGroup->mIsSwitched == STL_TRUE )                                        \
        {                                                                               \
            /* Switch */                                                                \
            sBlockList       = sExeGroup->mTotalColBlock;                               \
            sSwitchBlockList = sExeGroup->mSwitchBlock;                                 \
                                                                                        \
            while( sBlockList )                                                         \
            {                                                                           \
                QLNX_SWAP_DATA_VALUE_ARRAY( sBlockList, sSwitchBlockList, sDataValue ); \
                sBlockList    = sBlockList->mNext;                                      \
                sSwitchBlockList = sSwitchBlockList->mNext;                             \
            }                                                                           \
                                                                                        \
            sExeGroup->mIsSwitched = STL_FALSE;                                         \
        }                                                                               \
    }
    
#define _QLNX_SET_KEY_COLUMN_VALUES( )                                                  \
    {                                                                                   \
        sKeyColBlock = sExeGroup->mSwitchBlock;                                         \
        for( sKeyColIdx = 0 ;                                                           \
             sKeyColIdx < sOptGroup->mInstantNode->mKeyColCnt ;                         \
             sKeyColIdx++ )                                                             \
        {                                                                               \
            if( KNL_GET_BLOCK_IS_SEP_BUFF( sKeyColBlock ) == STL_TRUE )                 \
            {                                                                           \
                DTL_COPY_DATA_VALUE(                                                    \
                    KNL_GET_BLOCK_DATA_BUFFER_SIZE( sKeyColBlock ),                     \
                    & sExeGroup->mGroupColValue[ sKeyColIdx ],                          \
                    KNL_GET_BLOCK_SEP_DATA_VALUE( sKeyColBlock, sUsedBlockCnt ) );      \
            }                                                                           \
            sKeyColBlock = sKeyColBlock->mNext;                                         \
        }                                                                               \
    }

#define _QLNX_SET_RECORD_COLUMN_VALUES( )                                               \
    {                                                                                   \
        sSwitchBlock = sExeGroup->mSwitchRecBlock;                                      \
        sRecColBlock = sExeGroup->mRecColBlock;                                         \
        for( sRecColIdx = 0 ;                                                           \
             sRecColIdx < sOptGroup->mInstantNode->mRecColCnt ;                         \
             sRecColIdx++ )                                                             \
        {                                                                               \
            if( KNL_GET_BLOCK_IS_SEP_BUFF( sSwitchBlock ) == STL_TRUE )                 \
            {                                                                           \
                DTL_COPY_DATA_VALUE(                                                    \
                    KNL_GET_BLOCK_DATA_BUFFER_SIZE( sSwitchBlock ),                     \
                    KNL_GET_BLOCK_SEP_DATA_VALUE( sRecColBlock, 0 ),                    \
                    KNL_GET_BLOCK_SEP_DATA_VALUE( sSwitchBlock, sUsedBlockCnt ) );      \
            }                                                                           \
            sSwitchBlock = sSwitchBlock->mNext;                                         \
            sRecColBlock = sRecColBlock->mNext;                                         \
        }                                                                               \
    }

#define _QLNX_SET_GROUPING_VALUES( )                                                            \
    {                                                                                           \
        sGroupKeyBlock = sExeGroup->mTotalColBlock;                                             \
        for( sGroupKeyIdx = 0 ;                                                                 \
             sGroupKeyIdx < sOptGroup->mInstantNode->mKeyColCnt ;                               \
             sGroupKeyIdx++ )                                                                   \
        {                                                                                       \
            if( KNL_GET_BLOCK_IS_SEP_BUFF( sGroupKeyBlock ) == STL_TRUE )                       \
            {                                                                                   \
                DTL_COPY_DATA_VALUE(                                                            \
                    KNL_GET_BLOCK_DATA_BUFFER_SIZE( sGroupKeyBlock ),                           \
                    KNL_GET_BLOCK_SEP_DATA_VALUE( sGroupKeyBlock, sExeGroup->mCurrBlockIdx ),   \
                    & sExeGroup->mGroupColValue[ sGroupKeyIdx ] );                              \
            }                                                                                   \
            else                                                                                \
            {                                                                                   \
                DTL_COPY_DATA_VALUE(                                                            \
                    KNL_GET_BLOCK_DATA_BUFFER_SIZE( sGroupKeyBlock ),                           \
                    KNL_GET_BLOCK_FIRST_DATA_VALUE( sGroupKeyBlock ),                           \
                    & sExeGroup->mGroupColValue[ sGroupKeyIdx ] );                              \
            }                                                                                   \
            sGroupKeyBlock = sGroupKeyBlock->mNext;                                             \
        }                                                                                       \
    }
    
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aFetchNodeInfo->mOptNode->mType == QLL_PLAN_NODE_GROUP_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    QLL_OPT_CHECK_TIME( sBeginFetchTime );
    

    /*****************************************
     * 기본 정보 획득
     ****************************************/
    
    /**
     * GROUP INSTANT TABLE Optimization Node 획득
     */

    sOptGroup = (qlnoGroup *) aFetchNodeInfo->mOptNode->mOptNode;

    
    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aFetchNodeInfo->mDataArea,
                                        QLL_GET_OPT_NODE_IDX( aFetchNodeInfo->mOptNode ) );


    /**
     * GROUP INSTANT TABLE Execution Node 획득
     */

    sExeGroup = sExecNode->mExecNode;


    /**
     * Used Block Count 초기화
     */
    
    *aUsedBlockCnt = 0;

    
    /**
     * Node 수행 여부 확인 
     */

    STL_DASSERT( *aEOF == STL_FALSE );
    STL_DASSERT( aFetchNodeInfo->mFetchCnt > 0);

    sRemainSkipCnt  = aFetchNodeInfo->mSkipCnt;
    sRemainFetchCnt = aFetchNodeInfo->mFetchCnt;
    

    /**
     * total column block switch
     */
    
    _QLNX_SWITCH_TOTAL_COLUMN_BLOCKS( );


    /**
     * Local Fetch Info 초기화
     */
    
    QLNX_SET_LOCAL_FETCH_INFO( & sLocalFetchInfo, aFetchNodeInfo );


    /*****************************************
     * False Filter 처리
     ****************************************/

    /**
     * False Filter인 경우 Aggregation 초기화하여 결과값 설정
     */

    if( ( sOptGroup->mAggrFuncCnt > 0 ) ||
        ( sOptGroup->mAggrDistFuncCnt > 0 ) )
    {
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
    }
    else
    {
        /* Do Nothing */
    }

    if( sExeGroup->mHasFalseFilter == STL_TRUE )
    {
        if( sOptGroup->mAggrFuncCnt > 0 )
        {
            if( sRemainSkipCnt > 0 )
            {
                /* Do Nothing */
            }
            else
            {
                /* Aggregation 초기화 */
                sAggrExecInfo = sExeGroup->mAggrExecInfo;
                for( sLoop = 0 ; sLoop < sOptGroup->mAggrFuncCnt ; sLoop++ )
                {
                    STL_DASSERT( sAggrExecInfo != NULL );
        
                    STL_TRY( gQlnfAggrFuncInfoArr[ sAggrExecInfo->mAggrOptInfo->mAggrFuncId ].mInitFuncPtr(
                                 sAggrExecInfo,
                                 0, /* result block idx */
                                 aEnv )
                             == STL_SUCCESS );

                    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrExecInfo->mAggrValueList, 0, sFetchedCnt );
            
                    sAggrExecInfo++;
                }
            }
        }
        else if( sOptGroup->mAggrDistFuncCnt > 0 )
        {
            if( sRemainSkipCnt > 0 )
            {
                /* Do Nothing */
            }
            else
            {
                /* Aggregation Distinct를 위한 Instant Table 초기화 */
                sAggrDistExecInfo = sExeGroup->mAggrDistExecInfo;
                for( sLoop = 0 ; sLoop < sOptGroup->mAggrDistFuncCnt ; sLoop++ )
                {
                    STL_DASSERT( sAggrDistExecInfo != NULL );

                    STL_TRY( gQlnfAggrFuncInfoArr[ sAggrDistExecInfo->mAggrOptInfo->mAggrFuncId ].mInitFuncPtr(
                                 (qlnfAggrExecInfo *) sAggrDistExecInfo,
                                 0, /* result block idx */
                                 aEnv )
                             == STL_SUCCESS );

                    KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrDistExecInfo->mAggrValueList, 0, sFetchedCnt );
            
                    sAggrDistExecInfo++;
                }
            }
        }
        else
        {
            sFetchedCnt = 0;
            sSkippedCnt = 0;
        }

        *aEOF = STL_TRUE;
        STL_THROW( RAMP_EXIT );
    }
    else
    {
        /* Do Nothing */
    }
    
    
    /*****************************************
     * 하위 노드 fetch 하여 grouping 수행
     ****************************************/

    sEvalInfo.mContext  = aFetchNodeInfo->mDataArea->mExprDataContext;
    sEvalInfo.mBlockIdx = 0;

    sExprStack = sOptGroup->mColExprStack;
    sStartIdx  = sExeGroup->mCurrBlockIdx;

    STL_RAMP( RAMP_FETCH_CHILD );

    if( sExeGroup->mIsNeedFetch == STL_FALSE )
    {
        sIsSameGroup = STL_TRUE;
        
        for( ; sExeGroup->mCurrBlockIdx < sExeGroup->mReadBlockCnt ; sExeGroup->mCurrBlockIdx++ )
        {
            sCurColBlock = sExeGroup->mTotalColBlock;
                
            for( sLoop = 0 ; sLoop < sOptGroup->mInstantNode->mKeyColCnt ; sLoop++ )
            {
                sCurDataValue = KNL_GET_BLOCK_DATA_VALUE( sCurColBlock, sExeGroup->mCurrBlockIdx );
                sLength = sCurDataValue->mLength;
                
                if( sExeGroup->mGroupColValue[ sLoop ].mLength == sLength )
                {
                    while( sLength > 0 )
                    {
                        sLength--;

                        if( ((stlChar*)sExeGroup->mGroupColValue[ sLoop ].mValue)[ sLength ] !=
                            ((stlChar*)sCurDataValue->mValue)[ sLength ] )
                        {
                            sIsSameGroup = STL_FALSE;
                            break;
                        }                            
                    }

                    if( sIsSameGroup == STL_FALSE )
                    {
                        break;
                    }
                }
                else
                {
                    sIsSameGroup = STL_FALSE;
                    break;
                }
                
                sCurColBlock = sCurColBlock->mNext;
            }


            if( sIsSameGroup == STL_TRUE )
            {
                sGroupRecCnt++;
            }
            else
            {
                /**
                 * 새로운 Group이 나타난 경우
                 */

                /* Offset과 Having Filter 처리 */
                if( ( sRemainSkipCnt > 0 ) && ( sExeGroup->mFilterExprEvalInfo == NULL ) )
                {
                    /* group 정보 재설정 */
                    _QLNX_SET_GROUPING_VALUES( );

                    sRemainSkipCnt--;
                    sStartIdx    = sExeGroup->mCurrBlockIdx;
                    sGroupRecCnt = 1;
                    sIsSameGroup = STL_TRUE;
                    
                    continue;
                }
                else
                {
                    /* Do Nothing */
                }

                /* Key Column 값 설정 */
                _QLNX_SET_KEY_COLUMN_VALUES( );

                    
                /**
                 * Aggregation & Aggregation Distinct
                 */

                if( sGroupRecCnt > 0 )
                {
                    /**
                     * Aggregation Build
                     */

                    if( sOptGroup->mAggrFuncCnt > 0 )
                    {
                        STL_TRY( qlnxGroupBuildAggrExecInfo( sOptGroup->mAggrFuncCnt,
                                                             sExeGroup->mAggrExecInfo,
                                                             sStartIdx,
                                                             sGroupRecCnt,
                                                             aEnv )
                                 == STL_SUCCESS );
                    }


                    /**
                     * Aggregation Distinct Build
                     */
        
                    if( sOptGroup->mAggrDistFuncCnt > 0 )
                    {
                        STL_TRY( qlnxGroupBuildAggrDistExecInfo( sLocalFetchInfo.mStmt,
                                                                 sOptGroup->mAggrDistFuncCnt,
                                                                 sExeGroup->mAggrDistExecInfo,
                                                                 sStartIdx,
                                                                 sGroupRecCnt,
                                                                 sLocalFetchInfo.mDataArea->mPredResultBlock,
                                                                 & sExeGroup->mRowBlock,
                                                                 aEnv )
                                 == STL_SUCCESS );
                    }
                }

                /* Record Column 값 설정 */
                _QLNX_SET_RECORD_COLUMN_VALUES( );

                /* Aggregation 초기화 */
                sAggrExecInfo = sExeGroup->mAggrExecInfo;
                for( sLoop = 0 ; sLoop < sOptGroup->mAggrFuncCnt ; sLoop++ )
                {
                    STL_DASSERT( sAggrExecInfo != NULL );
        
                    STL_TRY( gQlnfAggrFuncInfoArr[ sAggrExecInfo->mAggrOptInfo->mAggrFuncId ].mInitFuncPtr(
                                 sAggrExecInfo,
                                 0, /* result block idx */
                                 aEnv )
                             == STL_SUCCESS );
            
                    sAggrExecInfo++;
                }

                /* Aggregation Distinct를 위한 Instant Table 초기화 */
                sAggrDistExecInfo = sExeGroup->mAggrDistExecInfo;
                for( sLoop = 0 ; sLoop < sOptGroup->mAggrDistFuncCnt ; sLoop++ )
                {
                    STL_DASSERT( sAggrDistExecInfo != NULL );

                    STL_TRY( gQlnfAggrFuncInfoArr[ sAggrDistExecInfo->mAggrOptInfo->mAggrFuncId ].mInitFuncPtr(
                                 (qlnfAggrExecInfo *) sAggrDistExecInfo,
                                 0, /* result block idx */
                                 aEnv )
                             == STL_SUCCESS );

                    sAggrDistExecInfo++;
                }
                
                /* group 정보 재설정 */
                _QLNX_SET_GROUPING_VALUES( );

                
                /**
                 * Having 처리
                 */

                if( sExeGroup->mFilterExprEvalInfo != NULL )
                {
                    _QLNX_RESET_SWITCH_TOTAL_COLUMN_BLOCKS( );
                    
                    sExeGroup->mFilterExprEvalInfo->mBlockIdx = sUsedBlockCnt;
                    sExeGroup->mFilterExprEvalInfo->mBlockCount = 1;
                    STL_TRY( knlEvaluateOneExpression( sExeGroup->mFilterExprEvalInfo,
                                                       KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    _QLNX_SWITCH_TOTAL_COLUMN_BLOCKS( );

                    if( DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_SEP_DATA_VALUE(
                                                 sExeGroup->mFilterExprEvalInfo->mResultBlock,
                                                 sUsedBlockCnt ) ) )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        sStartIdx = sExeGroup->mCurrBlockIdx;
                        sGroupRecCnt = 1;
                        
                        sIsSameGroup = STL_TRUE;
                        continue;
                    }
                }
                
                /* offset 처리 */
                if( sRemainSkipCnt > 0 )
                {
                    sRemainSkipCnt--;

                    sStartIdx = sExeGroup->mCurrBlockIdx;
                    sGroupRecCnt = 1;
                }
                else
                {
                    /* Result Block이 꽉 찼으면 fetch 종료 */
                    sUsedBlockCnt++;
                    sRemainFetchCnt--;
                
                    sStartIdx = sExeGroup->mCurrBlockIdx;
                    sGroupRecCnt = 1;

                    if( ( sRemainFetchCnt == 0 ) ||
                        ( sUsedBlockCnt == sLocalFetchInfo.mDataArea->mBlockAllocCnt ) )
                    {
                        sFetchedCnt = sUsedBlockCnt;
                        sSkippedCnt = aFetchNodeInfo->mSkipCnt - sRemainSkipCnt;
                        
                        *aEOF = STL_FALSE;
                        STL_THROW( RAMP_EXIT );
                    }
                }
                
                sIsSameGroup = STL_TRUE;
            }
        }
        
        sExeGroup->mIsNeedFetch = STL_TRUE;
    }

    
    /**
     * Aggregation & Aggregation Distinct
     */

    if( ( sExeGroup->mIsNeedFetch == STL_TRUE ) && ( sGroupRecCnt > 0 ) )
    {
        /* Key Column 값 설정 */
        _QLNX_SET_KEY_COLUMN_VALUES( );
        
        /**
         * Aggregation Build
         */

        if( sOptGroup->mAggrFuncCnt > 0 )
        {
            STL_TRY( qlnxGroupBuildAggrExecInfo( sOptGroup->mAggrFuncCnt,
                                                 sExeGroup->mAggrExecInfo,
                                                 sStartIdx,
                                                 sGroupRecCnt,
                                                 aEnv )
                     == STL_SUCCESS );
        }
        

        /**
         * Aggregation Distinct Build
         */
        
        if( sOptGroup->mAggrDistFuncCnt > 0 )
        {
            STL_TRY( qlnxGroupBuildAggrDistExecInfo( sLocalFetchInfo.mStmt,
                                                     sOptGroup->mAggrDistFuncCnt,
                                                     sExeGroup->mAggrDistExecInfo,
                                                     sStartIdx,
                                                     sGroupRecCnt,
                                                     sLocalFetchInfo.mDataArea->mPredResultBlock,
                                                     & sExeGroup->mRowBlock,
                                                     aEnv )
                     == STL_SUCCESS );
        }
    }
    

    /**
     * Fetch Child Node
     */
    
    if( sExeGroup->mEOF == STL_FALSE )
    {
        /**
         * Block Read
         */

        QLL_OPT_CHECK_TIME( sBeginExceptTime );

        /* fetch records*/
        sExeGroup->mReadBlockCnt = 0;
        QLNX_FETCH_NODE( & sLocalFetchInfo,
                         sOptGroup->mChildNode->mIdx,
                         0, /* Start Idx */
                         0, /* Skip Count */
                         QLL_FETCH_COUNT_MAX, /* Fetch Count */
                         & sExeGroup->mReadBlockCnt,
                         & sExeGroup->mEOF,
                         aEnv );

        QLL_OPT_CHECK_TIME( sEndExceptTime );
        QLL_OPT_ADD_ELAPSED_TIME( sExceptTime, sBeginExceptTime, sEndExceptTime );

        if( sExeGroup->mReadBlockCnt > 0 )
        {
            sCurColBlock = sExeGroup->mTotalColBlock;

            if( sOptGroup->mNeedEvalKeyCol == STL_TRUE )
            {
                for( sLoop = 0 ; sLoop < sOptGroup->mInstantNode->mKeyColCnt ; sLoop++ )
                {
                    if( sExprStack[ sLoop ] != NULL )
                    {
                        sEvalInfo.mBlockIdx    = 0;
                        sEvalInfo.mBlockCount  = sExeGroup->mReadBlockCnt;
                        sEvalInfo.mExprStack   = sExprStack[ sLoop ];
                        sEvalInfo.mResultBlock = sCurColBlock;

                        STL_TRY( knlEvaluateBlockExpression( & sEvalInfo,
                                                             KNL_ENV( aEnv ) )
                                 == STL_SUCCESS );
                    }
                    sCurColBlock = sCurColBlock->mNext;
                }
            }
            
            sExeGroup->mCurrBlockIdx = 0;
            sExeGroup->mIsNeedFetch  = STL_FALSE;

            sStartIdx    = 0;
            sGroupRecCnt = 0;
            
            if( sExeGroup->mIsExistGroup == STL_FALSE )
            {
                /* Key Column 값 설정 */
                _QLNX_SET_GROUPING_VALUES( );

                /* Aggregation 초기화 */
                sAggrExecInfo = sExeGroup->mAggrExecInfo;
                for( sLoop = 0 ; sLoop < sOptGroup->mAggrFuncCnt ; sLoop++ )
                {
                    STL_DASSERT( sAggrExecInfo != NULL );
        
                    STL_TRY( gQlnfAggrFuncInfoArr[ sAggrExecInfo->mAggrOptInfo->mAggrFuncId ].mInitFuncPtr(
                                 sAggrExecInfo,
                                 0, /* result block idx */
                                 aEnv )
                             == STL_SUCCESS );
            
                    sAggrExecInfo++;
                }

                /* Aggregation Distinct를 위한 Instant Table 초기화 */
                sAggrDistExecInfo = sExeGroup->mAggrDistExecInfo;
                for( sLoop = 0 ; sLoop < sOptGroup->mAggrDistFuncCnt ; sLoop++ )
                {
                    STL_DASSERT( sAggrDistExecInfo != NULL );

                    STL_TRY( gQlnfAggrFuncInfoArr[ sAggrDistExecInfo->mAggrOptInfo->mAggrFuncId ].mInitFuncPtr(
                                 (qlnfAggrExecInfo *) sAggrDistExecInfo,
                                 0, /* result block idx */
                                 aEnv )
                             == STL_SUCCESS );
                    
                    /* Instant Table 초기화*/
                    if( sAggrDistExecInfo->mInstantTable.mRecCnt > 1 )
                    {
                        STL_TRY( qlnxTruncateInstantTable( sLocalFetchInfo.mStmt,
                                                           & sAggrDistExecInfo->mInstantTable,
                                                           aEnv )
                                 == STL_SUCCESS );

                        sAggrDistExecInfo->mInstantTable.mIsNeedBuild = STL_TRUE;
                        sAggrDistExecInfo->mInstantTable.mRecCnt      = 0;
                        sAggrDistExecInfo->mInstantTable.mCurIdx      = 0;
                    }
                    sAggrDistExecInfo++;
                }

                sExeGroup->mIsExistGroup = STL_TRUE;
                sExeGroup->mCurrBlockIdx = 1;

                sGroupRecCnt = 1;
            }
            else
            {
                /* Do Nothing */
            }
            
            STL_THROW( RAMP_FETCH_CHILD );
        }
        else
        {
            STL_DASSERT( sExeGroup->mEOF == STL_TRUE );

            sFetchedCnt = sUsedBlockCnt;
            sSkippedCnt = aFetchNodeInfo->mSkipCnt - sRemainSkipCnt;

            *aEOF = STL_TRUE;
        }
    }
    

    /**
     * 마지막 group에 대한 결과값 생성
     */

    if( ( sExeGroup->mEOF == STL_TRUE ) && ( sExeGroup->mIsExistGroup == STL_TRUE ) )
    {
        /* Offset과 Having Filter 처리 */
        if( ( sRemainSkipCnt > 0 ) && ( sExeGroup->mFilterExprEvalInfo == NULL ) )
        {
            /* group 정보 재설정 */
            _QLNX_SET_GROUPING_VALUES( );

            sFetchedCnt = sUsedBlockCnt;
            sSkippedCnt = aFetchNodeInfo->mSkipCnt - sRemainSkipCnt;
            
            *aEOF = STL_TRUE;
            STL_THROW( RAMP_EXIT );
        }
        else
        {
            /* Do Nothing */
        }
        
        /* Record Column 값 설정 */
        _QLNX_SET_RECORD_COLUMN_VALUES( );

        
        /**
         * Having 처리
         */
        
        if( sExeGroup->mFilterExprEvalInfo != NULL )
        {
            _QLNX_RESET_SWITCH_TOTAL_COLUMN_BLOCKS( );
                    
            sExeGroup->mFilterExprEvalInfo->mBlockIdx = sUsedBlockCnt;
            sExeGroup->mFilterExprEvalInfo->mBlockCount = 1;
            STL_TRY( knlEvaluateOneExpression( sExeGroup->mFilterExprEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            _QLNX_SWITCH_TOTAL_COLUMN_BLOCKS( );

            if( DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_SEP_DATA_VALUE(
                                         sExeGroup->mFilterExprEvalInfo->mResultBlock,
                                         sUsedBlockCnt ) ) )
            {
                if( sRemainSkipCnt == 0 )
                {
                    /* Result Block이 꽉 찼으면 fetch 종료 */
                    sUsedBlockCnt++;
                }
                else
                {
                    /* Do Nothing */
                }
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            if( sRemainSkipCnt == 0 )
            {
                /* Result Block이 꽉 찼으면 fetch 종료 */
                sUsedBlockCnt++;
            }
            else
            {
                /* Do Nothing */
            }
        }

        sFetchedCnt = sUsedBlockCnt;
        sSkippedCnt = aFetchNodeInfo->mSkipCnt - sRemainSkipCnt;

        *aEOF = STL_TRUE;
        STL_THROW( RAMP_EXIT );
    }

    *aEOF = ( sFetchedCnt == 0 );

    STL_RAMP( RAMP_EXIT );

    
    /**
     * OUTPUT 설정
     */

    *aUsedBlockCnt = sFetchedCnt;
    
    /* Parent의 Fetch Node Info 설정 */
    aFetchNodeInfo->mFetchCnt -= sFetchedCnt;
    aFetchNodeInfo->mSkipCnt  -= sSkippedCnt;


    /* total column block 원복 */
    _QLNX_RESET_SWITCH_TOTAL_COLUMN_BLOCKS( );

    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeGroup->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeGroup->mCommonInfo.mFetchCallCount, 1 );
    QLL_OPT_ADD_RECORD_STAT_INFO( sExeGroup->mCommonInfo.mFetchRecordStat,
                                  sExeGroup->mCommonInfo.mResultColBlock,
                                  *aUsedBlockCnt,
                                  aEnv );

    sExeGroup->mCommonInfo.mFetchRowCnt += *aUsedBlockCnt;


    return STL_SUCCESS;
    
    STL_FINISH;

    /* total column block 원복 */
    _QLNX_RESET_SWITCH_TOTAL_COLUMN_BLOCKS( );
 
    QLL_OPT_CHECK_TIME( sEndFetchTime );
    QLL_OPT_ADD_ELAPSED_TIME( sFetchTime, sBeginFetchTime, sEndFetchTime );
    QLL_OPT_ADD_TIME( sExeGroup->mCommonInfo.mFetchTime, sFetchTime - sExceptTime );
    QLL_OPT_ADD_COUNT( sExeGroup->mCommonInfo.mFetchCallCount, 1 );

    return STL_FAILURE;
}


/**
 * @brief GROUP node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeGroup( qllOptimizationNode   * aOptNode,
                             qllDataArea           * aDataArea,
                             qllEnv                * aEnv )
{
    qllExecutionNode  * sExecNode = NULL;
    qlnoGroup         * sOptGroup = NULL;
    qlnxGroup         * sExeGroup = NULL;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_GROUP_TYPE,
                        QLL_ERROR_STACK(aEnv) );

    
    /**
     * GROUP INSTANT TABLE Optimization Node 획득
     */

    sOptGroup = (qlnoGroup *) aOptNode->mOptNode;


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea, QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * 해당 노드를 포함하는 Query Expression Optimization Node 획득
     */

    /* sOptQueryNode = aOptNode->mOptCurQueryNode; */


    /**
     * GROUP INSTANT ACCESS Execution Node 획득
     */

    sExeGroup = (qlnxGroup *) sExecNode->mExecNode;


    /**
     * Aggregation Distinct를 위한 Instant Table 해제
     */

    if( sOptGroup->mAggrDistFuncCnt > 0 )
    {
        STL_TRY( qlnxFinalizeAggrDist( sOptGroup->mAggrDistFuncCnt,
                                       sExeGroup->mAggrDistExecInfo,
                                       aEnv )
                 == STL_SUCCESS );
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
 * @brief Aggregation을 수행한다.
 * @param[in]      aAggrCnt       Aggregation Count
 * @param[in,out]  aAggrExecInfo  Aggregation Execution Info (No Distinct)
 * @param[in]      aStartIdx      Aggregation 적용 대상 Value Block의 시작 Idx
 * @param[in]      aGroupRecCnt   Aggregation 적용할 record 개수
 * @param[in]      aEnv           Environment
 */
stlStatus qlnxGroupBuildAggrExecInfo( stlInt32            aAggrCnt,
                                      qlnfAggrExecInfo  * aAggrExecInfo,
                                      stlInt32            aStartIdx,
                                      stlInt32            aGroupRecCnt,
                                      qllEnv            * aEnv )
{
    qlnfAggrExecInfo  * sAggrExecInfo = aAggrExecInfo;
    stlInt32            sLoop         = 0;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStartIdx >= 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGroupRecCnt > 0, QLL_ERROR_STACK(aEnv) );


    /**
     * Aggregation Execution Info 초기화
     */

    for( sLoop = 0 ; sLoop < aAggrCnt ; sLoop++ )
    {
        STL_DASSERT( sAggrExecInfo != NULL );
        
        STL_TRY( gQlnfAggrFuncInfoArr[ sAggrExecInfo->mAggrOptInfo->mAggrFuncId ].mBuildFuncPtr(
                     sAggrExecInfo,
                     aStartIdx, /* start block idx */
                     aGroupRecCnt,
                     0, /* result block idx */
                     aEnv )
                 == STL_SUCCESS );
        
        sAggrExecInfo++;
    }

    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Aggregation Distinct를 수행한다.
 * @param[in]      aStmt              Statement
 * @param[in]      aAggrDistCnt       Aggregation Distinct Count
 * @param[in,out]  aAggrDistExecInfo  Aggregation Distinct Execution Info (No Distinct)
 * @param[in]      aStartIdx          Aggregation Distinct 적용 대상 Value Block의 시작 Idx
 * @param[in]      aGroupRecCnt       Aggregation Distinct 적용할 record 개수
 * @param[in]      aDupCheckBlock     Duplication Check 정보를 설정할 Value Block
 * @param[in]      aRowBlock          Row Block for Insert
 * @param[in]      aEnv               Environment
 */
stlStatus qlnxGroupBuildAggrDistExecInfo( smlStatement          * aStmt,
                                          stlInt32                aAggrDistCnt,
                                          qlnfAggrDistExecInfo  * aAggrDistExecInfo,
                                          stlInt32                aStartIdx,
                                          stlInt32                aGroupRecCnt,
                                          knlValueBlockList     * aDupCheckBlock,
                                          smlRowBlock           * aRowBlock,
                                          qllEnv                * aEnv )
{
    qlnfAggrDistExecInfo  * sAggrDistExecInfo = aAggrDistExecInfo;
    knlExprEvalInfo       * sEvalInfo         = NULL;
    dtlDataValue          * sResultValue      = NULL;
    stlInt32                sLoop             = 0;
    stlInt32                sArgIdx           = 0;
    stlInt32                sBlockIdx         = 0;
    stlInt32                sStartBlockIdx    = 0;
    stlInt32                sRecCnt           = 0;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aStartIdx >= 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGroupRecCnt > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDupCheckBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowBlock != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Aggregation Execution Info 초기화
     */

    for( sLoop = 0 ; sLoop < aAggrDistCnt ; sLoop++ )
    {
        STL_DASSERT( sAggrDistExecInfo != NULL );

        /**
         * check duplication
         */

        /* Evaluate Column Expression Stack */
        for( sArgIdx = 0 ; sArgIdx < sAggrDistExecInfo->mInputArgCnt ; sArgIdx++ )
        {
            if( sAggrDistExecInfo->mEvalInfo[ sArgIdx ] != NULL )
            {
                sAggrDistExecInfo->mEvalInfo[ sArgIdx ]->mBlockIdx   = aStartIdx;
                sAggrDistExecInfo->mEvalInfo[ sArgIdx ]->mBlockCount = aGroupRecCnt;
                STL_TRY( knlEvaluateBlockExpression( sAggrDistExecInfo->mEvalInfo[ sArgIdx ],
                                                     KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
            else
            {
                /* Do Nothing */
            }
                
            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( sAggrDistExecInfo->mInsertColList, aStartIdx, aStartIdx + aGroupRecCnt );
        }

        
        /**
         * Instant Table에 Record 삽입
         */
        
        sAggrDistExecInfo->mInstantTable.mRecCnt += aGroupRecCnt;

        STL_TRY( smlInsertRecord( aStmt,
                                  sAggrDistExecInfo->mInstantTable.mTableHandle,
                                  sAggrDistExecInfo->mInsertColList,
                                  aDupCheckBlock, /* unique violation */
                                  aRowBlock,
                                  SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        
        
        /**
         * Duplication 체크하여 Aggregation 수행
         */

        sResultValue = KNL_GET_BLOCK_SEP_DATA_VALUE( aDupCheckBlock, aStartIdx );
        sStartBlockIdx = aStartIdx;
        sRecCnt = 0;
        
        for( sBlockIdx = 0 ; sBlockIdx < aGroupRecCnt ; sBlockIdx++ )
        {
            if( DTL_BOOLEAN_IS_TRUE( sResultValue ) )
            {
                /**
                 * duplication 발생
                 */
                
                if( sRecCnt > 0 )
                {
                    /**
                     * duplicate하지 않은 레코드만 aggregation 수행
                     */

                    sEvalInfo = sAggrDistExecInfo->mEvalInfo[ 0 ];
                    sAggrDistExecInfo->mEvalInfo[ 0 ] = NULL;
                    
                    STL_TRY( gQlnfAggrFuncInfoArr[ sAggrDistExecInfo->mAggrOptInfo->mAggrFuncId ].mBuildFuncPtr(
                                 (qlnfAggrExecInfo *) sAggrDistExecInfo,
                                 sStartBlockIdx, /* start block idx */
                                 sRecCnt,
                                 0, /* result block idx */
                                 aEnv )
                             == STL_SUCCESS );

                    sAggrDistExecInfo->mEvalInfo[ 0 ] = sEvalInfo;
                }
                else
                {
                    /* Do Nothing */
                }

                sStartBlockIdx = aStartIdx + sBlockIdx + 1;
                sRecCnt = 0;
            }
            else
            {
                /**
                 * duplication 발생하지 않음
                 */
                
                sRecCnt++;
            }
            sResultValue++;
        }

        if( sRecCnt > 0 )
        {
            /**
             * duplicate하지 않은 레코드만 aggregation 수행
             */
                    
            sEvalInfo = sAggrDistExecInfo->mEvalInfo[ 0 ];
            sAggrDistExecInfo->mEvalInfo[ 0 ] = NULL;
            
            STL_TRY( gQlnfAggrFuncInfoArr[ sAggrDistExecInfo->mAggrOptInfo->mAggrFuncId ].mBuildFuncPtr(
                         (qlnfAggrExecInfo *) sAggrDistExecInfo,
                         sStartBlockIdx, /* start block idx */
                         sRecCnt,
                         0, /* result block idx */
                         aEnv )
                     == STL_SUCCESS );

            sAggrDistExecInfo->mEvalInfo[ 0 ] = sEvalInfo;
        }
        else
        {
            /* Do Nothing */
        }
        
        sAggrDistExecInfo++;
    }

    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnx */
