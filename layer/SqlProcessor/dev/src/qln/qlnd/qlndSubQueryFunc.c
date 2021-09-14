/*******************************************************************************
 * qlndSubQueryFunc.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndSubQueryFunc.c 8545 2013-05-23 01:55:08Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndSubQueryFunc.c
 * @brief SQL Processor Layer Data Optimization Node - Sub Query Func
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */


/**
 * @brief Sub Query Function 을 Data Optimize 한다.
 * @param[in]  aTransID   Transcation ID
 * @param[in]  aDBCStmt   DBC Statement
 * @param[in]  aSQLStmt   SQL Statement
 * @param[in]  aOptNode   Code Optimization Node
 * @param[in]  aDataArea  Data Area
 * @param[in]  aEnv       Environment
 * @remarks
 */
stlStatus qlndDataOptimizeSubQueryFunc( smlTransId              aTransID,
                                        qllDBCStmt            * aDBCStmt,
                                        qllStatement          * aSQLStmt,
                                        qllOptimizationNode   * aOptNode,
                                        qllDataArea           * aDataArea,
                                        qllEnv                * aEnv )
{
    qllExecutionNode      * sExecNode            = NULL;
    qlnoSubQueryFunc      * sOptSubQueryFunc     = NULL;
    qlnxSubQueryFunc      * sExeSubQueryFunc     = NULL;

    knlExprContext        * sContexts            = NULL;
    qlvInitListFunc       * sListFunc            = NULL;
    qlvInitRowExpr        * sLeftRowExpr         = NULL;
    qlvInitRowExpr        * sRightRowExpr        = NULL;
    
    knlValueBlockList     * sCurLeftValueBlock   = NULL;
    knlValueBlockList     * sCurRightValueBlock  = NULL;
    knlValueBlockList     * sLastLeftValueBlock  = NULL;
    knlValueBlockList     * sLastRightValueBlock = NULL;
    knlValueBlock         * sValueBlock          = NULL;
    knlCastFuncInfo       * sCastFuncInfo        = NULL;

    stlInt32                sOffset              = 0;
    stlInt32                sLoop                = 0;
    dtlDataType             sDBType;
    

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * SUB QUERY FUNCTION Optimization Node 획득
     */

    sOptSubQueryFunc = (qlnoSubQueryFunc *) aOptNode->mOptNode;


    /**
     * 하위 node에 대한 Data Optimize 수행
     */

    if( sOptSubQueryFunc->mRightOperandNode != NULL )
    {
        QLND_DATA_OPTIMIZE( aTransID,
                            aDBCStmt,
                            aSQLStmt,
                            sOptSubQueryFunc->mRightOperandNode,
                            aDataArea,
                            aEnv );
    }


    /**
     * Common Execution Node 획득
     */
        
    sExecNode = QLL_GET_EXECUTION_NODE( aDataArea,
                                        QLL_GET_OPT_NODE_IDX( aOptNode ) );


    /**
     * QUERY EXPR Execution Node 할당
     */

    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                STL_SIZEOF( qlnxSubQueryFunc ),
                                (void **) & sExeSubQueryFunc,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    stlMemset( sExeSubQueryFunc, 0x00, STL_SIZEOF( qlnxSubQueryFunc ) );


    /**
     * Execution Node 정보 설정
     */

    STL_DASSERT( sOptSubQueryFunc->mFuncExpr != NULL );
    STL_DASSERT( sOptSubQueryFunc->mFuncExpr->mType == QLV_EXPR_TYPE_LIST_FUNCTION );

    sContexts = aDataArea->mExprDataContext->mContexts;


    /**
     * List Function에 포함된 Row Expression에 대한 Value Block 공유
     * : Cast Value Block
     */

    /* Right Operand Row Expression 얻기 */
    sListFunc = sOptSubQueryFunc->mFuncExpr->mExpr.mListFunc;

    STL_DASSERT( sListFunc->mArgs[ 0 ]->mExpr.mListCol->mArgCount == 1 );
    STL_DASSERT( sListFunc->mArgs[ 0 ]->mExpr.mListCol->mArgs[ 0 ]->mType
                 == QLV_EXPR_TYPE_ROW_EXPR );
        
    sRightRowExpr = sListFunc->mArgs[ 0 ]->mExpr.mListCol->mArgs[ 0 ]->mExpr.mRowExpr;

    /* Value Block Count 설정 */
    sExeSubQueryFunc->mValueBlockCnt = sRightRowExpr->mArgCount;

    for( sLoop = 0 ; sLoop < sRightRowExpr->mArgCount ; sLoop++ )
    {
        /**
         * Cast Value Block List 구성
         */
            
        /* Cast Value Block 설정 */
        STL_DASSERT( sRightRowExpr->mArgs[ sLoop ]->mType == QLV_EXPR_TYPE_REFERENCE );

        STL_TRY( knlInitShareBlockFromBlock(
                     & sCurRightValueBlock,
                     sContexts[ sRightRowExpr->mArgs[ sLoop ]->mExpr.mReference->mOrgExpr->mOffset ].mInfo.mValueInfo,
                     & QLL_DATA_PLAN( aDBCStmt ),
                     KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        /* link */
        if( sLastRightValueBlock == NULL )
        {
            sExeSubQueryFunc->mBlockListForCast = sCurRightValueBlock;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sLastRightValueBlock, sCurRightValueBlock );
        }
        sLastRightValueBlock = sCurRightValueBlock;
    }        
    

    /**
     * List Function에 포함된 Row Expression에 대한 Value Block 공유
     * : Left Value Block & Right Value Block
     */

    sLastRightValueBlock = NULL;
    
    if( sListFunc->mArgCount == 1 )
    {
        /* Left Operand Value Block 설정 */
        sExeSubQueryFunc->mLeftBlockList = NULL;

        /* Right Operand Row Expression 얻기 */
        STL_DASSERT( sListFunc->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_LIST_COLUMN );
        STL_DASSERT( sListFunc->mArgs[ 0 ]->mExpr.mListCol->mArgCount == 1 );
        STL_DASSERT( sListFunc->mArgs[ 0 ]->mExpr.mListCol->mArgs[ 0 ]->mType
                     == QLV_EXPR_TYPE_ROW_EXPR );
        
        sRightRowExpr = sListFunc->mArgs[ 0 ]->mExpr.mListCol->mArgs[ 0 ]->mExpr.mRowExpr;

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlCastFuncInfo * ) * sRightRowExpr->mArgCount,
                                    (void **) & sExeSubQueryFunc->mRightCastInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        for( sLoop = 0 ; sLoop < sRightRowExpr->mArgCount ; sLoop++ )
        {
            /**
             * Right Value Block List 구성
             */
            
            /* Right Operand Value Block 설정 */
            STL_DASSERT( sRightRowExpr->mArgs[ sLoop ]->mType == QLV_EXPR_TYPE_REFERENCE );

            /* Cast Info 설정 */
            sValueBlock = sContexts[ sRightRowExpr->mArgs[ sLoop ]->mOffset ].mInfo.mValueInfo;
            
            STL_DASSERT( sContexts[ sRightRowExpr->mArgs[ sLoop ]->mOffset ].mCast == NULL );

            sExeSubQueryFunc->mRightCastInfo[ sLoop ] = NULL;

            sDBType = sValueBlock->mDataValue->mType;
            STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                        & sCurRightValueBlock,
                                        aDataArea->mBlockAllocCnt,
                                        STL_TRUE,
                                        STL_LAYER_SQL_PROCESSOR,
                                        0,
                                        sLoop,
                                        sDBType,
                                        gResultDataTypeDef[ sDBType ].mArgNum1,
                                        gResultDataTypeDef[ sDBType ].mArgNum2,
                                        gResultDataTypeDef[ sDBType ].mStringLengthUnit,
                                        gResultDataTypeDef[ sDBType ].mIntervalIndicator,
                                        & QLL_DATA_PLAN( aDBCStmt ),
                                        aEnv )
                     == STL_SUCCESS );

            /* link */
            if( sLastRightValueBlock == NULL )
            {
                sExeSubQueryFunc->mRightBlockList = sCurRightValueBlock;
            }
            else
            {
                KNL_LINK_BLOCK_LIST( sLastRightValueBlock, sCurRightValueBlock );
            }
            sLastRightValueBlock = sCurRightValueBlock;
        }        
        
        sExeSubQueryFunc->mCompFunc = NULL;
    }
    else
    {
        STL_DASSERT( sListFunc->mArgCount == 2 );
        
        /* Left Operand Row Expression 얻기 */
        STL_DASSERT( sListFunc->mArgs[ 1 ]->mType == QLV_EXPR_TYPE_LIST_COLUMN );
        STL_DASSERT( sListFunc->mArgs[ 1 ]->mExpr.mListCol->mArgCount == 1 );
        STL_DASSERT( sListFunc->mArgs[ 1 ]->mExpr.mListCol->mArgs[ 0 ]->mType
                     == QLV_EXPR_TYPE_ROW_EXPR );

        sLeftRowExpr = sListFunc->mArgs[ 1 ]->mExpr.mListCol->mArgs[ 0 ]->mExpr.mRowExpr;

        /* Right Operand Row Expression 얻기 */
        STL_DASSERT( sListFunc->mArgs[ 0 ]->mType == QLV_EXPR_TYPE_LIST_COLUMN );
        STL_DASSERT( sListFunc->mArgs[ 0 ]->mExpr.mListCol->mArgCount == 1 );
        STL_DASSERT( sListFunc->mArgs[ 0 ]->mExpr.mListCol->mArgs[ 0 ]->mType
                     == QLV_EXPR_TYPE_ROW_EXPR );

        sRightRowExpr = sListFunc->mArgs[ 0 ]->mExpr.mListCol->mArgs[ 0 ]->mExpr.mRowExpr;

        STL_DASSERT( sLeftRowExpr->mArgCount == sRightRowExpr->mArgCount );

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( knlCastFuncInfo * ) * sRightRowExpr->mArgCount,
                                    (void **) & sExeSubQueryFunc->mRightCastInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        
        /**
         * Comapre Function 공간 할당
         */

        STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                    STL_SIZEOF( dtlPhysicalFunc ) * sLeftRowExpr->mArgCount,
                                    (void **) & sExeSubQueryFunc->mCompFunc,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        

        /**
         * Value Block List 구성
         */

        for( sLoop = 0 ; sLoop < sLeftRowExpr->mArgCount ; sLoop++ )
        {
            /**
             * Right Value Block List 구성
             */
            
            /* Right Operand Value Block 설정 */
            STL_DASSERT( sRightRowExpr->mArgs[ sLoop ]->mType == QLV_EXPR_TYPE_REFERENCE );
            STL_DASSERT( sRightRowExpr->mArgs[ sLoop ]->mExpr.mReference->mOrgExpr != NULL );
 
            /* Cast Info 설정 */
            sOffset = sRightRowExpr->mArgs[ sLoop ]->mExpr.mReference->mOrgExpr->mOffset;
            
            if( sContexts[ sOffset ].mCast == NULL )
            {
                if( sContexts[ sRightRowExpr->mArgs[ sLoop ]->mOffset ].mCast != NULL )
                {
                    sExeSubQueryFunc->mRightCastInfo[ sLoop ] =
                        sContexts[ sRightRowExpr->mArgs[ sLoop ]->mOffset ].mCast;

                    sDBType = sContexts[ sRightRowExpr->mArgs[ sLoop ]->mOffset ].mCast->mCastResultBuf.mType;
                    STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                                & sCurRightValueBlock,
                                                aDataArea->mBlockAllocCnt,
                                                STL_TRUE,
                                                STL_LAYER_SQL_PROCESSOR,
                                                0,
                                                sLoop,
                                                sDBType,
                                                gResultDataTypeDef[ sDBType ].mArgNum1,
                                                gResultDataTypeDef[ sDBType ].mArgNum2,
                                                gResultDataTypeDef[ sDBType ].mStringLengthUnit,
                                                gResultDataTypeDef[ sDBType ].mIntervalIndicator,
                                                & QLL_DATA_PLAN( aDBCStmt ),
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    sExeSubQueryFunc->mRightCastInfo[ sLoop ] = NULL;
                
                    sDBType = sContexts[ sOffset ].mInfo.mValueInfo->mDataValue->mType;
                    STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                                & sCurRightValueBlock,
                                                aDataArea->mBlockAllocCnt,
                                                STL_TRUE,
                                                STL_LAYER_SQL_PROCESSOR,
                                                0,
                                                sLoop,
                                                sDBType,
                                                gResultDataTypeDef[ sDBType ].mArgNum1,
                                                gResultDataTypeDef[ sDBType ].mArgNum2,
                                                gResultDataTypeDef[ sDBType ].mStringLengthUnit,
                                                gResultDataTypeDef[ sDBType ].mIntervalIndicator,
                                                & QLL_DATA_PLAN( aDBCStmt ),
                                                aEnv )
                             == STL_SUCCESS );
                }
            }
            else
            {
                if( sContexts[ sRightRowExpr->mArgs[ sLoop ]->mOffset ].mCast != NULL )
                {
                    sDBType = sContexts[ sRightRowExpr->mArgs[ sLoop ]->mOffset ].mCast->mCastResultBuf.mType;

                    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN( aDBCStmt ),
                                                STL_SIZEOF( knlCastFuncInfo ),
                                                (void **) & sCastFuncInfo,
                                                KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    STL_TRY( gBuiltInFuncInfoArr[ KNL_BUILTIN_FUNC_CAST ].mGetFuncPointer(
                                 DTL_GET_CAST_FUNC_PTR_IDX( sContexts[ sOffset ].mInfo.mValueInfo->mDataValue->mType, sDBType),
                                 & sCastFuncInfo->mCastFuncPtr,
                                 QLL_ERROR_STACK( aEnv ) )
                             == STL_SUCCESS );

                    sCastFuncInfo->mCastResultBuf =
                        sContexts[ sRightRowExpr->mArgs[ sLoop ]->mOffset ].mCast->mCastResultBuf;
                        
                    stlMemcpy( sCastFuncInfo->mArgs,
                               sContexts[ sRightRowExpr->mArgs[ sLoop ]->mOffset ].mCast->mArgs,
                               STL_SIZEOF( dtlValueEntry ) * KNL_CAST_FUNC_ARG_COUNT );
                    
                    sCastFuncInfo->mSrcTypeInfo = sContexts[ sOffset ].mCast->mSrcTypeInfo;

                    sExeSubQueryFunc->mRightCastInfo[ sLoop ] = sCastFuncInfo;

                    STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                                & sCurRightValueBlock,
                                                aDataArea->mBlockAllocCnt,
                                                STL_TRUE,
                                                STL_LAYER_SQL_PROCESSOR,
                                                0,
                                                sLoop,
                                                sDBType,
                                                gResultDataTypeDef[ sDBType ].mArgNum1,
                                                gResultDataTypeDef[ sDBType ].mArgNum2,
                                                gResultDataTypeDef[ sDBType ].mStringLengthUnit,
                                                gResultDataTypeDef[ sDBType ].mIntervalIndicator,
                                                & QLL_DATA_PLAN( aDBCStmt ),
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    sExeSubQueryFunc->mRightCastInfo[ sLoop ] =
                        sContexts[ sOffset ].mCast;
                
                    sDBType = sContexts[ sOffset ].mCast->mCastResultBuf.mType;
                    STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                                & sCurRightValueBlock,
                                                aDataArea->mBlockAllocCnt,
                                                STL_TRUE,
                                                STL_LAYER_SQL_PROCESSOR,
                                                0,
                                                sLoop,
                                                sDBType,
                                                gResultDataTypeDef[ sDBType ].mArgNum1,
                                                gResultDataTypeDef[ sDBType ].mArgNum2,
                                                gResultDataTypeDef[ sDBType ].mStringLengthUnit,
                                                gResultDataTypeDef[ sDBType ].mIntervalIndicator,
                                                & QLL_DATA_PLAN( aDBCStmt ),
                                                aEnv )
                             == STL_SUCCESS );
                }
            }

            /* link */
            if( sLastRightValueBlock == NULL )
            {
                sExeSubQueryFunc->mRightBlockList = sCurRightValueBlock;
            }
            else
            {
                KNL_LINK_BLOCK_LIST( sLastRightValueBlock, sCurRightValueBlock );
            }
            sLastRightValueBlock = sCurRightValueBlock;
            
            /**
             * Left Value Block List 구성
             */

            if( sContexts[ sLeftRowExpr->mArgs[ sLoop ]->mOffset ].mCast == NULL )
            {
                /* Left Operand Value Block 설정 */
                switch( sLeftRowExpr->mArgs[ sLoop ]->mType )
                {
                    case QLV_EXPR_TYPE_VALUE :
                    case QLV_EXPR_TYPE_BIND_PARAM :
                    case QLV_EXPR_TYPE_PSEUDO_COLUMN :
                    case QLV_EXPR_TYPE_COLUMN :
                    case QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT :
                    case QLV_EXPR_TYPE_REFERENCE :
                    case QLV_EXPR_TYPE_INNER_COLUMN :
                    case QLV_EXPR_TYPE_OUTER_COLUMN :
                    case QLV_EXPR_TYPE_ROWID_COLUMN :
                    case QLV_EXPR_TYPE_AGGREGATION :
                        {
                            /* value block 간의 연결 */
                            STL_TRY( knlInitShareBlockFromBlock(
                                         & sCurLeftValueBlock,
                                         sContexts[ sLeftRowExpr->mArgs[ sLoop ]->mOffset ].mInfo.mValueInfo,
                                         & QLL_DATA_PLAN( aDBCStmt ),
                                         KNL_ENV( aEnv ) )
                                     == STL_SUCCESS );
                            break;
                        }
                    case QLV_EXPR_TYPE_FUNCTION :
                    case QLV_EXPR_TYPE_CAST :
                    case QLV_EXPR_TYPE_CASE_EXPR :
                        {
                            /* value block을 할당 받아 연결 */
                            sDBType = sContexts[ sLeftRowExpr->mArgs[ sLoop ]->mOffset ].mInfo.mFuncData->mResultValue.mType;
                        
                            STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                                        & sCurLeftValueBlock,
                                                        aDataArea->mBlockAllocCnt,
                                                        STL_TRUE,
                                                        STL_LAYER_SQL_PROCESSOR,
                                                        0,
                                                        0,
                                                        sDBType,
                                                        gResultDataTypeDef[ sDBType ].mArgNum1,
                                                        gResultDataTypeDef[ sDBType ].mArgNum2,
                                                        gResultDataTypeDef[ sDBType ].mStringLengthUnit,
                                                        gResultDataTypeDef[ sDBType ].mIntervalIndicator,
                                                        & QLL_DATA_PLAN( aDBCStmt ),
                                                        aEnv )
                                     == STL_SUCCESS );
                            break;
                        }

                    case QLV_EXPR_TYPE_LIST_FUNCTION :
                        {
                            /* value block을 할당 받아 연결 */
                            sDBType = sContexts[ sLeftRowExpr->mArgs[ sLoop ]->mOffset ].mInfo.mListFunc->mResultValue.mType;
                        
                            STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                                        & sCurLeftValueBlock,
                                                        aDataArea->mBlockAllocCnt,
                                                        STL_TRUE,
                                                        STL_LAYER_SQL_PROCESSOR,
                                                        0,
                                                        0,
                                                        sDBType,
                                                        gResultDataTypeDef[ sDBType ].mArgNum1,
                                                        gResultDataTypeDef[ sDBType ].mArgNum2,
                                                        gResultDataTypeDef[ sDBType ].mStringLengthUnit,
                                                        gResultDataTypeDef[ sDBType ].mIntervalIndicator,
                                                        & QLL_DATA_PLAN( aDBCStmt ),
                                                        aEnv )
                                     == STL_SUCCESS );
                            break;
                        }

                        /* case QLV_EXPR_TYPE_LIST_COLUMN : */
                        /* case QLV_EXPR_TYPE_ROW_EXPR : */
                        /* case QLV_EXPR_TYPE_SUB_QUERY : */
                        /* case QLV_EXPR_TYPE_PSEUDO_ARGUMENT : */
                    default :
                        {
                            STL_DASSERT( 0 );
                            break;
                        }
                }
            }
            else
            {
                sDBType = sContexts[ sLeftRowExpr->mArgs[ sLoop ]->mOffset ].mCast->mCastResultBuf.mType;
                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & sCurLeftValueBlock,
                                            aDataArea->mBlockAllocCnt,
                                            STL_TRUE,
                                            STL_LAYER_SQL_PROCESSOR,
                                            0,
                                            0,
                                            sDBType,
                                            gResultDataTypeDef[ sDBType ].mArgNum1,
                                            gResultDataTypeDef[ sDBType ].mArgNum2,
                                            gResultDataTypeDef[ sDBType ].mStringLengthUnit,
                                            gResultDataTypeDef[ sDBType ].mIntervalIndicator,
                                            & QLL_DATA_PLAN( aDBCStmt ),
                                            aEnv )
                         == STL_SUCCESS );
            }

            /* link */
            if( sLastLeftValueBlock == NULL )
            {
                sExeSubQueryFunc->mLeftBlockList = sCurLeftValueBlock;
            }
            else
            {
                KNL_LINK_BLOCK_LIST( sLastLeftValueBlock, sCurLeftValueBlock );
            }
            sLastLeftValueBlock = sCurLeftValueBlock;


            /**
             * Comapre Function 설정
             */

            sExeSubQueryFunc->mCompFunc[ sLoop ] =
                dtlPhysicalCompareFuncList
                [ DTL_GET_BLOCK_DB_TYPE( sCurLeftValueBlock ) ]
                [ DTL_GET_BLOCK_DB_TYPE( sCurRightValueBlock ) ];
        }
    }

    /* Result Value Block 설정 */
    STL_DASSERT( sOptSubQueryFunc->mResultExpr != NULL );
    STL_DASSERT( sOptSubQueryFunc->mResultExpr->mType == QLV_EXPR_TYPE_REFERENCE );

    STL_TRY( knlInitShareBlockFromBlock(
                 & sExeSubQueryFunc->mResultBlock,
                 sContexts[ sOptSubQueryFunc->mResultExpr->mOffset ].mInfo.mValueInfo,
                 & QLL_DATA_PLAN( aDBCStmt ),
                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    /* Is First Execute Flag 초기화 */ 
    sExeSubQueryFunc->mIsFirstExec = STL_TRUE;
    sExeSubQueryFunc->mHasNull = STL_FALSE;


    /**
     * Result Expr의 Related Function 이 N/A 인 경우
     * List Function 결과가 NULL인지 체크할 필요가 없다.
     */

    if( ( sOptSubQueryFunc->mFuncExpr->mPhraseType == QLV_EXPR_PHRASE_CONDITION ) ||
        ( sOptSubQueryFunc->mFuncExpr->mPhraseType == QLV_EXPR_PHRASE_HAVING ) )
    {
        sExeSubQueryFunc->mNeedResNullCheck =
            ( sOptSubQueryFunc->mResultExpr->mRelatedFuncId != KNL_BUILTIN_FUNC_INVALID );
    }
    else
    {
        sExeSubQueryFunc->mNeedResNullCheck = STL_TRUE;
    }
    
    /* Sub Query Function의 Func Pointer 설정 */
    switch( sListFunc->mFuncId )
    {
        case KNL_BUILTIN_FUNC_EXISTS :
            sExeSubQueryFunc->mIsResReverse = STL_FALSE;
            sExeSubQueryFunc->mIsOrdering   = STL_FALSE;
            
            sExeSubQueryFunc->mSubQueryFuncPtr = qlnxSubQueryFuncExists;

            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_EQUAL + 1 ]   = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mResult[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_EQUAL + 1 ]   = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;
            
            break;
            
        case KNL_BUILTIN_FUNC_NOT_EXISTS :
            sExeSubQueryFunc->mIsResReverse = STL_TRUE;
            sExeSubQueryFunc->mIsOrdering   = STL_FALSE;
            
            sExeSubQueryFunc->mSubQueryFuncPtr = qlnxSubQueryFuncExists;

            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_EQUAL + 1 ]   = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mResult[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_EQUAL + 1 ]   = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;
            
            break;

        case KNL_BUILTIN_FUNC_IN :
        case KNL_BUILTIN_FUNC_EQUAL_ANY :
            sExeSubQueryFunc->mIsResReverse = STL_FALSE;
            sExeSubQueryFunc->mIsOrdering   = STL_FALSE;
            
            sExeSubQueryFunc->mSubQueryFuncPtr = qlnxSubQueryFuncCompAnyExact;
            
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mResult[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            break;
            
        case KNL_BUILTIN_FUNC_NOT_EQUAL_ANY :
            sExeSubQueryFunc->mIsResReverse = STL_FALSE;
            sExeSubQueryFunc->mIsOrdering   = STL_FALSE;

            sExeSubQueryFunc->mSubQueryFuncPtr = qlnxSubQueryFuncCompAnyExact;
            
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mResult[ DTL_COMPARISON_LESS + 1 ]    = STL_TRUE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_EQUAL + 1 ]   = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_GREATER + 1 ] = STL_TRUE;

            break;
        case KNL_BUILTIN_FUNC_LESS_THAN_ANY :
            sExeSubQueryFunc->mIsResReverse = STL_FALSE;
            sExeSubQueryFunc->mIsOrdering   = STL_TRUE;

            sExeSubQueryFunc->mSubQueryFuncPtr = qlnxSubQueryFuncCompAnyOrdering;
            
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mResult[ DTL_COMPARISON_LESS + 1 ]    = STL_TRUE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_EQUAL + 1 ]   = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            break;
        case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY :
            sExeSubQueryFunc->mIsResReverse = STL_FALSE;
            sExeSubQueryFunc->mIsOrdering   = STL_TRUE;

            sExeSubQueryFunc->mSubQueryFuncPtr = qlnxSubQueryFuncCompAnyOrdering;
            
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mResult[ DTL_COMPARISON_LESS + 1 ]    = STL_TRUE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            break;
        case KNL_BUILTIN_FUNC_GREATER_THAN_ANY :
            sExeSubQueryFunc->mIsResReverse = STL_FALSE;
            sExeSubQueryFunc->mIsOrdering   = STL_TRUE;

            sExeSubQueryFunc->mSubQueryFuncPtr = qlnxSubQueryFuncCompAnyOrdering;
            
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mResult[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_EQUAL + 1 ]   = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_GREATER + 1 ] = STL_TRUE;

            break;
        case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY :
            sExeSubQueryFunc->mIsResReverse = STL_FALSE;
            sExeSubQueryFunc->mIsOrdering   = STL_TRUE;

            sExeSubQueryFunc->mSubQueryFuncPtr = qlnxSubQueryFuncCompAnyOrdering;
            
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mResult[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_GREATER + 1 ] = STL_TRUE;

            break;

        case KNL_BUILTIN_FUNC_EQUAL_ALL :
            sExeSubQueryFunc->mIsResReverse = STL_TRUE;
            sExeSubQueryFunc->mIsOrdering   = STL_FALSE;

            sExeSubQueryFunc->mSubQueryFuncPtr = qlnxSubQueryFuncCompAnyExact;
            
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mResult[ DTL_COMPARISON_LESS + 1 ]    = STL_TRUE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_EQUAL + 1 ]   = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_GREATER + 1 ] = STL_TRUE;

            sExeSubQueryFunc->mNeedResNullCheck = STL_TRUE;
            
            break;
        case KNL_BUILTIN_FUNC_NOT_IN :
        case KNL_BUILTIN_FUNC_NOT_EQUAL_ALL :
            sExeSubQueryFunc->mIsResReverse = STL_TRUE;
            sExeSubQueryFunc->mIsOrdering   = STL_FALSE;

            sExeSubQueryFunc->mSubQueryFuncPtr = qlnxSubQueryFuncCompAnyExact;
            
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mResult[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mNeedResNullCheck = STL_TRUE;
            
            break;
        case KNL_BUILTIN_FUNC_LESS_THAN_ALL :
            sExeSubQueryFunc->mIsResReverse = STL_TRUE;
            sExeSubQueryFunc->mIsOrdering   = STL_TRUE;

            sExeSubQueryFunc->mSubQueryFuncPtr = qlnxSubQueryFuncCompAnyOrdering;

            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mResult[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_GREATER + 1 ] = STL_TRUE;

            sExeSubQueryFunc->mNeedResNullCheck = STL_TRUE;
            
            break;
        case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL :
            sExeSubQueryFunc->mIsResReverse = STL_TRUE;
            sExeSubQueryFunc->mIsOrdering   = STL_TRUE;

            sExeSubQueryFunc->mSubQueryFuncPtr = qlnxSubQueryFuncCompAnyOrdering;

            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mResult[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_EQUAL + 1 ]   = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_GREATER + 1 ] = STL_TRUE;

            sExeSubQueryFunc->mNeedResNullCheck = STL_TRUE;
            
            break;
        case KNL_BUILTIN_FUNC_GREATER_THAN_ALL :
            sExeSubQueryFunc->mIsResReverse = STL_TRUE;
            sExeSubQueryFunc->mIsOrdering   = STL_TRUE;

            sExeSubQueryFunc->mSubQueryFuncPtr = qlnxSubQueryFuncCompAnyOrdering;

            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mResult[ DTL_COMPARISON_LESS + 1 ]    = STL_TRUE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mNeedResNullCheck = STL_TRUE;
            
            break;
        case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL :
            sExeSubQueryFunc->mIsResReverse = STL_TRUE;
            sExeSubQueryFunc->mIsOrdering   = STL_TRUE;

            sExeSubQueryFunc->mSubQueryFuncPtr = qlnxSubQueryFuncCompAnyOrdering;

            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
            sExeSubQueryFunc->mIsCheckNext[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mResult[ DTL_COMPARISON_LESS + 1 ]    = STL_TRUE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_EQUAL + 1 ]   = STL_FALSE;
            sExeSubQueryFunc->mResult[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

            sExeSubQueryFunc->mNeedResNullCheck = STL_TRUE;
            
            break;
            
        default :
            STL_DASSERT( 0 );
            break;
    }


    /**
     * Materialize Node 정보 생성
     */

    STL_DASSERT( sRightRowExpr->mSubQueryExpr != NULL );

    if( ( sOptSubQueryFunc->mFuncExpr->mIterationTime < DTL_ITERATION_TIME_FOR_EACH_STATEMENT ) &&
        ( ( sRightRowExpr->mSubQueryExpr->mExpr.mSubQuery->mSubQueryType == QLV_SUB_QUERY_N_TYPE ) ||
          ( sRightRowExpr->mSubQueryExpr->mExpr.mSubQuery->mSubQueryType == QLV_SUB_QUERY_A_TYPE ) ) )
    {
        /**
         * Fetch Info 설정
         */
        
        sCurRightValueBlock = sExeSubQueryFunc->mRightBlockList;

        while( sCurRightValueBlock != NULL )
        {
            if( ( KNL_GET_BLOCK_DB_TYPE( sCurRightValueBlock ) == DTL_TYPE_LONGVARCHAR ) ||
                ( KNL_GET_BLOCK_DB_TYPE( sCurRightValueBlock ) == DTL_TYPE_LONGVARBINARY ) )
            {
                break;
            }

            sCurRightValueBlock = sCurRightValueBlock->mNext;
        }
        
        if( sCurRightValueBlock == NULL )
        {
            sExeSubQueryFunc->mNeedMaterialize = STL_TRUE;
            
            if( sExeSubQueryFunc->mIsOrdering == STL_FALSE )
            {
                STL_TRY( qlndGetListFuncInstantFetchInfo( aSQLStmt,
                                                          aDataArea,
                                                          sOptSubQueryFunc,
                                                          sExeSubQueryFunc->mNeedResNullCheck,
                                                          sExeSubQueryFunc->mLeftBlockList,
                                                          sExeSubQueryFunc->mRightBlockList,
                                                          & sExeSubQueryFunc->mListFuncInstantFetchInfo,
                                                          & sExeSubQueryFunc->mSubQueryFuncPtr,
                                                          & QLL_DATA_PLAN( aDBCStmt ),
                                                          aEnv )
                         == STL_SUCCESS );
                sExeSubQueryFunc->mListFuncArrayFetchInfo = NULL;
            }
            else
            {
                sExeSubQueryFunc->mListFuncInstantFetchInfo = NULL;
                STL_TRY( qlndGetListFuncArrayFetchInfo( aSQLStmt,
                                                        aDataArea,
                                                        sOptSubQueryFunc,
                                                        sExeSubQueryFunc->mNeedResNullCheck,
                                                        sExeSubQueryFunc->mRightBlockList,
                                                        & sExeSubQueryFunc->mListFuncArrayFetchInfo,
                                                        & sExeSubQueryFunc->mSubQueryFuncPtr,
                                                        & QLL_DATA_PLAN( aDBCStmt ),
                                                        aEnv )
                         == STL_SUCCESS );
            }
        }
        else
        {
            sExeSubQueryFunc->mNeedMaterialize = STL_FALSE;

            sExeSubQueryFunc->mListFuncInstantFetchInfo = NULL;
            sExeSubQueryFunc->mListFuncArrayFetchInfo = NULL;
        }
    }
    else
    {
        sExeSubQueryFunc->mNeedMaterialize = STL_FALSE;

        sExeSubQueryFunc->mListFuncInstantFetchInfo = NULL;
        sExeSubQueryFunc->mListFuncArrayFetchInfo = NULL;
    }
    

    /**
     * Common Info 설정
     */

    sExeSubQueryFunc->mCommonInfo.mResultColBlock = NULL;
    sExeSubQueryFunc->mCommonInfo.mRowBlockList = NULL;
    sExeSubQueryFunc->mCommonInfo.mOuterColBlock = NULL;
    sExeSubQueryFunc->mCommonInfo.mOuterOrgColBlock = NULL;
    

    /**
     * Common Execution Node 정보 설정
     */

    sExecNode->mNodeType  = QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE;

    sExecNode->mOptNode   = aOptNode;
    sExecNode->mExecNode  = sExeSubQueryFunc;

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
        sExecNode->mExecutionCost = NULL;
    }


    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Instant를 이용한 SUB QUERY FUNCTION 을 Materialize하여 Fetch 하기 위한 정보를 설정한다.
 * @param[in]      aSQLStmt                     SQL Statement
 * @param[in]      aDataArea                    Data Area (Data Optimization 결과물)
 * @param[in]      aOptSubQueryFunc             Opt Sub Query Function 
 * @param[in]      aNeedResNullCheck            List Function 결과가 Null인지 체크할지 여부
 * @param[in]      aLeftValueBlockList          Left Operand Value Block List
 * @param[in]      aRightValueBlockList         Right Operand Value Block List
 * @param[out]     aListFuncInstantFetchInfo    List Function Fetch Info
 * @param[out]     aSubQueryFuncPtr             Sub Query Fuction Pointer
 * @param[in]      aRegionMem                   영역 기반 메모리 할당자 (knlRegionMem)
 * @param[in]      aEnv                         Environment
 */
stlStatus qlndGetListFuncInstantFetchInfo( qllStatement                     * aSQLStmt,
                                           qllDataArea                      * aDataArea,
                                           qlnoSubQueryFunc                 * aOptSubQueryFunc,
                                           stlBool                            aNeedResNullCheck,
                                           knlValueBlockList                * aLeftValueBlockList,
                                           knlValueBlockList                * aRightValueBlockList,
                                           qlnxListFuncInstantFetchInfo    ** aListFuncInstantFetchInfo,
                                           qlnxSubQueryFuncPtr              * aSubQueryFuncPtr,
                                           knlRegionMem                     * aRegionMem,
                                           qllEnv                           * aEnv )
{
    qlnoSubQueryFunc                * sOptSubQueryFunc = NULL;
    qlnxListFuncInstantFetchInfo    * sFuncFetchInfo   = NULL;
    qlvInitListFunc                 * sListFunc        = NULL;
    qlvInitRowExpr                  * sRightRowExpr    = NULL;

    knlValueBlockList      * sLeftValueBlock  = NULL;
    knlValueBlockList      * sRightValueBlock = NULL;
    knlValueBlockList      * sNewValueBlock   = NULL;
    knlValueBlockList      * sPrevValueBlock  = NULL;
    
    dtlDataValue           * sLeftValue       = NULL;
    dtlDataValue           * sRightValue      = NULL;

    knlCompareList         * sMinRange        = NULL;
    knlCompareList         * sMaxRange        = NULL;
    knlKeyCompareList      * sKeyCompList     = NULL;
    knlRange               * sRange           = NULL;

    stlInt32                 sLoop            = 0;
    stlInt32                 sExprCnt         = 0;
    stlUInt8                 sSortKeyFlag     = DTL_KEYCOLUMN_EMPTY;
    dtlDataType              sDBType;
    knlBuiltInFunc           sFuncId;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptSubQueryFunc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRightValueBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aListFuncInstantFetchInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryFuncPtr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * List Function 정보 획득
     */
    
    sOptSubQueryFunc = aOptSubQueryFunc;

    sListFunc = sOptSubQueryFunc->mFuncExpr->mExpr.mListFunc;

    sFuncId = sListFunc->mFuncId;

    sRightRowExpr = sListFunc->mArgs[ 0 ]->mExpr.mListCol->mArgs[ 0 ]->mExpr.mRowExpr;
    
    sExprCnt = sRightRowExpr->mArgCount;


    /**
     * List Function Fetch Info 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlnxListFuncInstantFetchInfo ),
                                (void **) & sFuncFetchInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sFuncFetchInfo, 0x00, STL_SIZEOF( qlnxListFuncInstantFetchInfo ) );


    /**
     * Instant Table 공간 할당
     * @remark Execution 시에 정보 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlnInstantTable ),
                                (void **) & sFuncFetchInfo->mInstantTable,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sFuncFetchInfo->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );


    /**
     * Sort Instant Table에 Insert시 사용될 Row Block 생성
     */

    STL_TRY( smlInitRowBlock( & sFuncFetchInfo->mSortRowBlock,
                              aDataArea->mBlockAllocCnt,
                              aRegionMem,
                              SML_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**************************************************
     * quantifier가 ANY 인지 여부 설정
     **************************************************/

    switch( sFuncId )
    {
        case KNL_BUILTIN_FUNC_EXISTS :
            {
                sFuncId = KNL_BUILTIN_FUNC_EXISTS;
                break;
            }
        case KNL_BUILTIN_FUNC_NOT_EXISTS :
            {
                sFuncId = KNL_BUILTIN_FUNC_NOT_EXISTS;
                break;
            }
        case KNL_BUILTIN_FUNC_IN :
            {
                sFuncId = KNL_BUILTIN_FUNC_EQUAL_ANY;
                break;
            }
        case KNL_BUILTIN_FUNC_EQUAL_ALL :
            {
                sFuncId = KNL_BUILTIN_FUNC_NOT_EQUAL_ANY;
                break;
            }
        case KNL_BUILTIN_FUNC_NOT_IN :
        case KNL_BUILTIN_FUNC_NOT_EQUAL_ALL :
            {
                sFuncId = KNL_BUILTIN_FUNC_EQUAL_ANY;
                break;
            }
        case KNL_BUILTIN_FUNC_EQUAL_ANY :
        case KNL_BUILTIN_FUNC_NOT_EQUAL_ANY :
            {
                /* Do Nothing */
                break;
            }
        default :
        /* case KNL_BUILTIN_FUNC_LESS_THAN_ANY : */
        /* case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY : */
        /* case KNL_BUILTIN_FUNC_GREATER_THAN_ANY : */
        /* case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY : */
        /* case KNL_BUILTIN_FUNC_LESS_THAN_ALL : */
        /* case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL : */
        /* case KNL_BUILTIN_FUNC_GREATER_THAN_ALL : */
        /* case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL : */
            {
                STL_DASSERT( 0 );
                break;
            }
    }


    /**************************************************
     * Key Compare List 구성
     **************************************************/

    /**
     * Key Compare List 공간 할당
     */

    STL_TRY( knlCreateKeyCompareList( sExprCnt,
                                      & sKeyCompList,
                                      aRegionMem,
                                      KNL_ENV(aEnv) )
             == STL_SUCCESS );    


    /**
     * Table Column Order 정보 구성 - SORT
     */

    sRightValueBlock = aRightValueBlockList;

    for( sLoop = 0; sLoop < sExprCnt ; sLoop++ )
    {
        /* table column order */
        sKeyCompList->mTableColOrder[ sLoop ]  = sLoop;
        sKeyCompList->mIndexColOffset[ sLoop ] = sLoop;

        /* function pointer */
        sDBType = DTL_GET_BLOCK_DB_TYPE( sRightValueBlock );
        sKeyCompList->mCompFunc[ sLoop ] =
            dtlPhysicalFuncArg2FuncList[ sDBType ][ sDBType ][ DTL_PHYSICAL_FUNC_IS_EQUAL ];

        /* value block list */
        STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                    & sNewValueBlock,
                                    1,
                                    STL_TRUE,
                                    STL_LAYER_SQL_PROCESSOR,
                                    sLoop,
                                    sLoop,
                                    sDBType,
                                    gDefaultDataTypeDef[ sDBType ].mArgNum1,
                                    gDefaultDataTypeDef[ sDBType ].mArgNum2,
                                    gDefaultDataTypeDef[ sDBType ].mStringLengthUnit,
                                    gDefaultDataTypeDef[ sDBType ].mIntervalIndicator,
                                    aRegionMem,
                                    aEnv )
                 == STL_SUCCESS );

        if( sKeyCompList->mValueList == NULL )
        {
            sKeyCompList->mValueList = sNewValueBlock;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevValueBlock, sNewValueBlock );
        }
        sPrevValueBlock = sNewValueBlock;
    }

    
    /**************************************************
     * Sort Instant Key 정보 설정
     **************************************************/

    /**
     * Sort Instant Key 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( stlUInt8 ) * sRightRowExpr->mArgCount,
                                (void **) & sFuncFetchInfo->mSortKeyFlags,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Range 구성에 따라 다르게 설정
     */

    DTL_SET_INDEX_COLUMN_FLAG( sSortKeyFlag,
                               DTL_KEYCOLUMN_INDEX_ORDER_ASC,
                               DTL_KEYCOLUMN_INDEX_NULLABLE_TRUE,
                               DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST );

    stlMemset( sFuncFetchInfo->mSortKeyFlags,
               sSortKeyFlag,
               STL_SIZEOF( stlUInt8 ) * sRightRowExpr->mArgCount );


    /**************************************************
     * Range 구성
     **************************************************/

    /**
     * Range 구성
     */

    if( ( sFuncId == KNL_BUILTIN_FUNC_EXISTS ) ||
        ( sFuncId == KNL_BUILTIN_FUNC_NOT_EXISTS ) ||
        ( sFuncId == KNL_BUILTIN_FUNC_NOT_EQUAL_ANY ) )
    {
        sMinRange = NULL;
        sMaxRange = NULL;
    }
    else
    {
        /**
         * Min Range 공간 할당
         */
        
        STL_DASSERT( sFuncId == KNL_BUILTIN_FUNC_EQUAL_ANY );

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlCompareList ) * sExprCnt,
                                    (void **) & sMinRange,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    
        stlMemset( sMinRange,
                   0x00,
                   STL_SIZEOF( knlCompareList ) * sExprCnt );


        /**
         * Min Range 속성 설정
         */

        sLeftValueBlock  = aLeftValueBlockList;
        sRightValueBlock = aRightValueBlockList;
        
        for( sLoop = 0 ; sLoop < sExprCnt ; sLoop++ )
        {
            STL_DASSERT( sLeftValueBlock != NULL );
            
            sLeftValue  = KNL_GET_BLOCK_FIRST_DATA_VALUE( sLeftValueBlock );
            sRightValue = KNL_GET_BLOCK_FIRST_DATA_VALUE( sRightValueBlock );
            
            sMinRange[ sLoop ].mRangeVal = sLeftValue->mValue;
            sMinRange[ sLoop ].mRangeLen = sLeftValue->mLength;
            
            sMinRange[ sLoop ].mColOrder = sLoop;

            sMinRange[ sLoop ].mIsIncludeEqual = STL_TRUE;
            sMinRange[ sLoop ].mIsAsc = STL_TRUE;
            sMinRange[ sLoop ].mIsNullFirst = STL_TRUE;
            sMinRange[ sLoop ].mIsDiffTypeCmp = ( sLeftValue->mType != sRightValue->mType );
            sMinRange[ sLoop ].mIsLikeFunc = STL_FALSE;

            sMinRange[ sLoop ].mCompNullStop = STL_FALSE;
            sMinRange[ sLoop ].mConstVal = sLeftValue;
            sMinRange[ sLoop ].mCompFunc =
                dtlPhysicalCompareFuncList[ sRightValue->mType ][ sLeftValue->mType ];
            sMinRange[ sLoop ].mNext = NULL;
        
            sLeftValueBlock  = sLeftValueBlock->mNext;
            sRightValueBlock = sRightValueBlock->mNext;
        }

        
        /**
         * Max Range 구성
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlCompareList ) * sExprCnt,
                                    (void **) & sMaxRange,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    
        stlMemcpy( sMaxRange,
                   sMinRange,
                   STL_SIZEOF( knlCompareList ) * sExprCnt );
    }


    /**
     * MIN & MAX range 설정
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlRange ),
                                (void **) & sRange,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    sRange->mMinRange = sMinRange;
    sRange->mMaxRange = sMaxRange;
    sRange->mNext     = NULL;
    
    sFuncFetchInfo->mMinRange = sMinRange;
    sFuncFetchInfo->mMaxRange = sMaxRange;

    
    /**************************************************
     * Fetch Info
     **************************************************/
    
    /**
     * Fetch Info 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( smlFetchInfo ),
                                (void **) & sFuncFetchInfo->mFetchInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sFuncFetchInfo->mFetchInfo, 0x00, STL_SIZEOF( smlFetchInfo ) );

    
    /**
     * Range & Read Column List 없이 설정
     * @remark fetch시에 재설정한다.
     */
        
    SML_SET_FETCH_INFO_FOR_INDEX( sFuncFetchInfo->mFetchInfo,
                                  sRange,  /* Range */
                                  NULL,  /* Physical Filter */
                                  sKeyCompList,  /* Key Compare List */
                                  NULL,  /* Logical Filter */
                                  NULL,  /* Read Column List */
                                  NULL,  /* Row ID Column List */
                                  & sFuncFetchInfo->mSortRowBlock,  /* Row Block */
                                  0,  /* Skip Count */
                                  0,  /* Fetch Count */
                                  STL_FALSE,  /* Group Key Fetch */
                                  NULL,  /* Filter Eval Info */
                                  NULL   /* Fetch Record Info */ );

    
    /**
     * OUTPUT 설정
     */
    
    *aListFuncInstantFetchInfo = sFuncFetchInfo;

    if( ( sFuncId == KNL_BUILTIN_FUNC_EXISTS ) ||
        ( sFuncId == KNL_BUILTIN_FUNC_NOT_EXISTS ) )
    {
        *aSubQueryFuncPtr = qlnxSubQueryFuncExistsWithInstant;
    }
    else
    {
        STL_DASSERT( ( sFuncId == KNL_BUILTIN_FUNC_EQUAL_ANY ) ||
                     ( sFuncId == KNL_BUILTIN_FUNC_NOT_EQUAL_ANY ) );

        *aSubQueryFuncPtr = qlnxSubQueryFuncCompAnyWithInstantExact;
    }
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Array로 처리가능한 SUB QUERY FUNCTION 을 Materialize하여 Fetch 하기 위한 정보를 설정한다.
 * @param[in,out]  aSQLStmt                 SQL Statement
 * @param[in]      aDataArea                Data Area (Data Optimization 결과물)
 * @param[in]      aOptSubQueryFunc         Opt Sub Query Function 
 * @param[in]      aNeedResNullCheck        List Function 결과가 Null인지 체크할지 여부
 * @param[in]      aRightValueBlockList     Right Operand Value Block List
 * @param[out]     aListFuncArrayFetchInfo  List Function Fetch Info
 * @param[out]     aSubQueryFuncPtr         Sub Query Fuction Pointer
 * @param[in]      aRegionMem               영역 기반 메모리 할당자 (knlRegionMem)
 * @param[in]      aEnv                     Environment
 */
stlStatus qlndGetListFuncArrayFetchInfo( qllStatement                   * aSQLStmt,
                                         qllDataArea                    * aDataArea,
                                         qlnoSubQueryFunc               * aOptSubQueryFunc,
                                         stlBool                          aNeedResNullCheck,
                                         knlValueBlockList              * aRightValueBlockList,
                                         qlnxListFuncArrayFetchInfo    ** aListFuncArrayFetchInfo,
                                         qlnxSubQueryFuncPtr            * aSubQueryFuncPtr,
                                         knlRegionMem                   * aRegionMem,
                                         qllEnv                         * aEnv )
{
    qlnoSubQueryFunc            * sOptSubQueryFunc        = NULL;
    qlnxListFuncArrayFetchInfo  * sListFuncArrayFetchInfo = NULL;
    qlvInitListFunc             * sListFunc               = NULL;
    qlvInitRowExpr              * sRightRowExpr           = NULL;

    knlValueBlockList           * sRightValueBlock        = NULL;
    knlValueBlockList           * sNewValueBlock          = NULL;
    knlValueBlockList           * sPrevValueBlock         = NULL;
    
    stlInt32                      sLoop                   = 0;
    stlInt32                      sExprCnt                = 0;
    stlInt32                      sRowColRecordCnt        = 0;
    stlBool                       sIsSetMinValue          = STL_FALSE;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptSubQueryFunc != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRightValueBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aListFuncArrayFetchInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryFuncPtr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * List Function 정보 획득
     */
    
    sOptSubQueryFunc = aOptSubQueryFunc;

    sListFunc = sOptSubQueryFunc->mFuncExpr->mExpr.mListFunc;

    sRightRowExpr = sListFunc->mArgs[ 0 ]->mExpr.mListCol->mArgs[ 0 ]->mExpr.mRowExpr;
    
    sExprCnt = sRightRowExpr->mArgCount;


    /**
     * List Function Array Fetch Info 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlnxListFuncArrayFetchInfo ),
                                (void **) & sListFuncArrayFetchInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sListFuncArrayFetchInfo, 0x00, STL_SIZEOF( qlnxListFuncArrayFetchInfo ) );


    /**************************************************
     * quantifier가 ANY 인지 여부 설정
     **************************************************/

    switch( sListFunc->mFuncId )
    {
        case KNL_BUILTIN_FUNC_LESS_THAN_ANY :
        case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY :
        case KNL_BUILTIN_FUNC_GREATER_THAN_ALL :
        case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL :
            {
                sIsSetMinValue = STL_FALSE;
                break;
            }
        case KNL_BUILTIN_FUNC_GREATER_THAN_ANY :
        case KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY :
        case KNL_BUILTIN_FUNC_LESS_THAN_ALL :
        case KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL :
            {
                sIsSetMinValue = STL_TRUE;
                break;
            }
        default :
        /* case KNL_BUILTIN_FUNC_EXISTS : */
        /* case KNL_BUILTIN_FUNC_NOT_EXISTS : */
        /* case KNL_BUILTIN_FUNC_IN : */
        /* case KNL_BUILTIN_FUNC_EQUAL_ALL : */
        /* case KNL_BUILTIN_FUNC_NOT_IN : */
        /* case KNL_BUILTIN_FUNC_NOT_EQUAL_ALL : */
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    
    /**************************************************
     * Row Col Block Record Count 설정
     **************************************************/

    if( aNeedResNullCheck == STL_TRUE )
    {
        sRowColRecordCnt = sExprCnt + 2;
    }
    else
    {
        sRowColRecordCnt = 1;
    }

    sListFuncArrayFetchInfo->mRowColRecordCnt = sRowColRecordCnt;
    

    /**************************************************
     * Row Col Block 설정
     **************************************************/

    /**
     * Row Col Block 사용 여부 flag 설정
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( stlBool ) * sRowColRecordCnt,
                                (void **) & sListFuncArrayFetchInfo->mIsUsedRowCol,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sListFuncArrayFetchInfo->mIsUsedRowCol,
               0x00,
               STL_SIZEOF( stlBool ) * sRowColRecordCnt );


    /**
     * Row Col Block 설정
     */

    sRightValueBlock = aRightValueBlockList;
    for( sLoop = 0 ; sLoop < sExprCnt ; sLoop++ )
    {
        /**
         * Value Block List 생성
         */
        
        STL_DASSERT( sRightValueBlock != NULL );
        STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF( sRightValueBlock ) == STL_TRUE );
    
        STL_TRY( qlndCopyBlock( & aSQLStmt->mLongTypeValueList,
                                & sNewValueBlock,
                                STL_LAYER_SQL_PROCESSOR,
                                sRowColRecordCnt,
                                sRightValueBlock,
                                aRegionMem,
                                aEnv )
                 == STL_SUCCESS );

        /* link */
        if( sPrevValueBlock == NULL )
        {
            sListFuncArrayFetchInfo->mRowColBlock = sNewValueBlock;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevValueBlock, sNewValueBlock );
        }
        sPrevValueBlock = sNewValueBlock;

        sRightValueBlock = sRightValueBlock->mNext;
    }


    /**
     * Build 여부 설정
     */
    
    sListFuncArrayFetchInfo->mIsNeedBuild = STL_TRUE;
    
        
    /**************************************************
     * Build시 사용할 Compare Function 설정
     **************************************************/
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( dtlPhysicalFunc ) * sExprCnt,
                                (void **) & sListFuncArrayFetchInfo->mCompFuncForBuild,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sRightValueBlock = aRightValueBlockList;
    for( sLoop = 0 ; sLoop < sExprCnt ; sLoop++ )
    {
        sListFuncArrayFetchInfo->mCompFuncForBuild[ sLoop ] =
            dtlPhysicalCompareFuncList
            [ DTL_GET_BLOCK_DB_TYPE( sRightValueBlock ) ]
            [ DTL_GET_BLOCK_DB_TYPE( sRightValueBlock ) ];

        sRightValueBlock = sRightValueBlock->mNext;
    }


    /**************************************************
     * Check Next 여부 및 Result에 대한 설정
     **************************************************/

    if( sIsSetMinValue == STL_TRUE )
    {
        sListFuncArrayFetchInfo->mIsCheckNextForBuild[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
        sListFuncArrayFetchInfo->mIsCheckNextForBuild[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
        sListFuncArrayFetchInfo->mIsCheckNextForBuild[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

        sListFuncArrayFetchInfo->mResultForBuild[ DTL_COMPARISON_LESS + 1 ]    = STL_TRUE;
        sListFuncArrayFetchInfo->mResultForBuild[ DTL_COMPARISON_EQUAL + 1 ]   = STL_FALSE;
        sListFuncArrayFetchInfo->mResultForBuild[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;
    }
    else
    {
        sListFuncArrayFetchInfo->mIsCheckNextForBuild[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
        sListFuncArrayFetchInfo->mIsCheckNextForBuild[ DTL_COMPARISON_EQUAL + 1 ]   = STL_TRUE;
        sListFuncArrayFetchInfo->mIsCheckNextForBuild[ DTL_COMPARISON_GREATER + 1 ] = STL_FALSE;

        sListFuncArrayFetchInfo->mResultForBuild[ DTL_COMPARISON_LESS + 1 ]    = STL_FALSE;
        sListFuncArrayFetchInfo->mResultForBuild[ DTL_COMPARISON_EQUAL + 1 ]   = STL_FALSE;
        sListFuncArrayFetchInfo->mResultForBuild[ DTL_COMPARISON_GREATER + 1 ] = STL_TRUE;
    }


    /**
     * OUTPUT 설정
     */
    
    *aListFuncArrayFetchInfo = sListFuncArrayFetchInfo;
    *aSubQueryFuncPtr = qlnxSubQueryFuncCompAnyWithArrayOrdering;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnd */
