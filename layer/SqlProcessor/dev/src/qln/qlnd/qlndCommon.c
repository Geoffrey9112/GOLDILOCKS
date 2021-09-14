/*******************************************************************************
 * qlndCommon.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlndCommon.c 11037 2014-01-23 02:11:07Z bsyou $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file qlndCommon.c
 * @brief SQL Processor Layer Optimization Node Common Functions
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnd
 * @{
 */

/*********************************************************
 * COMMON
 ********************************************************/

/**
 * @brief 참조하는 Value Block들을 모아 Value Block List를 구성한다.
 * @param[in]      aSQLStmt           SQL Statement 객체
 * @param[in]      aDataArea          Data Area
 * @param[in]      aStmtExprList      Statement Expression List
 * @param[in]      aValueExprList     Value Expression List
 * @param[out]     aValueBlockList    Value Block List
 * @param[in]      aRegionMem         Region Memory
 * @param[in]      aEnv               Environment
 *
 * @remark Value Block List의 Value Block은 Data Area에 이미 만들어진 Block을 공유한다.
 */
stlStatus qlndBuildRefBlockList( qllStatement          * aSQLStmt,
                                 qllDataArea           * aDataArea,
                                 qloInitStmtExprList   * aStmtExprList,
                                 qlvRefExprList        * aValueExprList,
                                 knlValueBlockList    ** aValueBlockList,
                                 knlRegionMem          * aRegionMem,
                                 qllEnv                * aEnv )
{
    knlValueBlockList  * sValueBlockList = NULL;
    knlValueBlockList  * sCurBlockList = NULL;
    knlValueBlockList  * sPrevBlockList = NULL;
    knlExprContext     * sContexts = NULL;
    qlvRefExprItem     * sRefExprItem = NULL;
    qloDBType          * sDBTypes = NULL;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Block Array 설정
     */
    
    sValueBlockList = NULL;
    sContexts       = aDataArea->mExprDataContext->mContexts;
    sRefExprItem    = aValueExprList->mHead;
    
    while( sRefExprItem != NULL )
    {
        /**
         * Block List 생성
         */

        STL_DASSERT( aDataArea->mExprDataContext->mCount > sRefExprItem->mExprPtr->mOffset );

        switch( sRefExprItem->mExprPtr->mType )
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
            case QLV_EXPR_TYPE_ROW_EXPR :
                {
                    /* value block 간의 연결 */
                    STL_TRY( knlInitShareBlockFromBlock(
                                 & sCurBlockList,
                                 sContexts[ sRefExprItem->mExprPtr->mOffset ].mInfo.mValueInfo,
                                 aRegionMem,
                                 KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                    break;
                }
            case QLV_EXPR_TYPE_FUNCTION :
            case QLV_EXPR_TYPE_CAST :
            case QLV_EXPR_TYPE_CASE_EXPR :
            case QLV_EXPR_TYPE_LIST_FUNCTION :
            case QLV_EXPR_TYPE_LIST_COLUMN :
                {
                    /* value block을 할당 받아 연결 */
                    STL_DASSERT( aStmtExprList != NULL );
                    sDBTypes = & aStmtExprList->mTotalExprInfo->mExprDBType[ sRefExprItem->mExprPtr->mOffset ];
                    
                    STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                                & sCurBlockList,
                                                aDataArea->mBlockAllocCnt,
                                                STL_TRUE,
                                                STL_LAYER_SQL_PROCESSOR,
                                                0,   /* table ID */
                                                0,   /* column ID */
                                                sDBTypes->mDBType,
                                                sDBTypes->mArgNum1,
                                                sDBTypes->mArgNum2,
                                                sDBTypes->mStringLengthUnit,
                                                sDBTypes->mIntervalIndicator,
                                                aRegionMem,
                                                aEnv )
                             == STL_SUCCESS );
                    break;
                }

            case QLV_EXPR_TYPE_SUB_QUERY :
                {
                    /**
                     * @todo Sub-Query 구현시 작성하기
                     */

                    STL_DASSERT( 0 );
                    break;
                }

            case QLV_EXPR_TYPE_PSEUDO_ARGUMENT :
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }

        
        /**
         * Block List 를 연결
         */

        if( sValueBlockList == NULL )
        {
            sValueBlockList = sCurBlockList;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevBlockList, sCurBlockList );
        }
        sPrevBlockList = sCurBlockList;
        
        sRefExprItem = sRefExprItem->mNext;
    }


    /**
     * OUTPUT 설정
     */
    
    *aValueBlockList = sValueBlockList;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Table Column Block들을 Table Column Order 순으로 Value Block List를 구성한다.
 * @param[in]      aDataArea          Data Area
 * @param[in]      aValueExprList     Value Expression List
 * @param[in]      aIsReadBlock       찾을 Value Block이 Read Block 인지 여부
 * @param[out]     aValueBlockList    Value Block List
 * @param[in]      aRegionMem         Region Memory
 * @param[in]      aEnv               Environment
 *
 * @remark Value Block List의 Value Block은 Data Area에 이미 만들어진 Block을 공유한다.
 */
stlStatus qlndBuildTableColBlockList( qllDataArea         * aDataArea,
                                      qlvRefExprList      * aValueExprList,
                                      stlBool               aIsReadBlock,
                                      knlValueBlockList  ** aValueBlockList,
                                      knlRegionMem        * aRegionMem,
                                      qllEnv              * aEnv )
{
    knlValueBlockList  * sValueBlockList = NULL;
    knlValueBlockList  * sCurBlockList = NULL;
    knlValueBlockList  * sPrevBlockList = NULL;
    knlValueBlockList  * sBlockList = NULL;
    
    qlvRefExprItem     * sRefExprItem = NULL;
    qlvInitExpression  * sInitExpr = NULL;
    
    stlInt64             sTableID = KNL_ANONYMOUS_TABLE_ID;
    stlInt32             sTabColOrder = 0;
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Block Array 설정
     */

    if( aValueExprList->mCount > 0 )
    {
        sRefExprItem = aValueExprList->mHead;

        while( sRefExprItem != NULL )
        {
            /**
             * Block List 생성
             */

            sInitExpr = sRefExprItem->mExprPtr;


            /**
             * Read Column List에서 해당 Value Block 찾기
             */

            sBlockList = NULL;
            if( aIsReadBlock == STL_TRUE )
            {
                STL_TRY( qlndFindColumnBlockList( aDataArea,
                                                  aDataArea->mReadColumnBlock,
                                                  sInitExpr,
                                                  aRegionMem,
                                                  & sBlockList,
                                                  aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                STL_TRY( qlndFindColumnBlockList( aDataArea,
                                                  aDataArea->mWriteColumnBlock,
                                                  sInitExpr,
                                                  aRegionMem,
                                                  & sBlockList,
                                                  aEnv )
                         == STL_SUCCESS );
            }

            STL_DASSERT( sBlockList != NULL );

            STL_TRY( knlInitShareBlock( & sCurBlockList,
                                        sBlockList,
                                        aRegionMem,
                                        KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
            
        
            /**
             * TableID 및 Column Order 설정
             */

            if( sInitExpr->mType == QLV_EXPR_TYPE_COLUMN )
            {
                /* expression list로부터 table column order 얻기 */
                sTableID = ellGetColumnTableID( sInitExpr->mExpr.mColumn->mColumnHandle );
                sTabColOrder = ellGetColumnIdx( sInitExpr->mExpr.mColumn->mColumnHandle );

                /* table id 및 column order 설정 */
                KNL_SET_BLOCK_TABLE_ID( sCurBlockList, sTableID );        
                KNL_SET_BLOCK_COLUMN_ORDER( sCurBlockList, sTabColOrder );
            }
            else
            {
                /* expression list로부터 table column order 얻기 */
                STL_DASSERT( sInitExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
                sTableID = KNL_ANONYMOUS_TABLE_ID;
                sTabColOrder = *sInitExpr->mExpr.mInnerColumn->mIdx;

                /* table id 및 column order 설정 */
                KNL_SET_BLOCK_TABLE_ID( sCurBlockList, sTableID );        
                KNL_SET_BLOCK_COLUMN_ORDER( sCurBlockList, sTabColOrder );
            }
        
        
            /**
             * Block List 를 연결 (ColumnOrder 순으로 Insertion Sort)
             */

            if( sValueBlockList == NULL )
            {
                sValueBlockList = sCurBlockList;
            }
            else
            {
                /* insertion sort */
                sBlockList     = sValueBlockList;
                sPrevBlockList = NULL;
            
                while( sBlockList != NULL )
                {
                    STL_DASSERT( KNL_GET_BLOCK_COLUMN_ORDER( sBlockList ) != 
                                 KNL_GET_BLOCK_COLUMN_ORDER( sCurBlockList ) );
                
                    if( KNL_GET_BLOCK_COLUMN_ORDER( sBlockList ) >
                        KNL_GET_BLOCK_COLUMN_ORDER( sCurBlockList ) )
                    {
                        /* insertion */
                        if( sPrevBlockList != NULL )
                        {
                            KNL_LINK_BLOCK_LIST( sCurBlockList, sBlockList );
                            KNL_LINK_BLOCK_LIST( sPrevBlockList, sCurBlockList );
                        }
                        else
                        {
                            KNL_LINK_BLOCK_LIST( sCurBlockList, sBlockList );
                            sValueBlockList = sCurBlockList;
                        }
                        break;
                    }
                    else
                    {
                        sPrevBlockList = sBlockList;
                        sBlockList = sBlockList->mNext;
                    }
                }

                if( sBlockList == NULL )
                {
                    KNL_LINK_BLOCK_LIST( sPrevBlockList, sCurBlockList );                
                }
                else
                {
                    /* Do Nothing */
                }
            }
        
            sRefExprItem = sRefExprItem->mNext;
        }
    }
    else
    {
        /* Do Nothing */
    }
    

    /**
     * OUTPUT 설정
     */
    
    *aValueBlockList = sValueBlockList;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Table RowId Column의 Value Block List를 구성한다.
 * @param[in]      aDataArea          Data Area
 * @param[in]      aRowIdExpr         RowId Expression
 * @param[out]     aRowIdBlockList    Value Block List
 * @param[in]      aRegionMem         Region Memory
 * @param[in]      aEnv               Environment
 *
 * @remark RowId Block List의 Value Block은 Data Area에 이미 만들어진 Block을 공유한다.
 */
stlStatus qlndBuildTableRowIdColBlockList( qllDataArea         * aDataArea,
                                           qlvInitExpression   * aRowIdExpr,
                                           knlValueBlockList  ** aRowIdBlockList,
                                           knlRegionMem        * aRegionMem,
                                           qllEnv              * aEnv )
{
    knlValueBlockList  * sBlockList    = NULL;
    knlValueBlockList  * sCurBlockList = NULL;    

    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowIdExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowIdBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    /*
     * value block list 구성
     */
    
    STL_TRY( qlndFindRowIdColumnBlockList( aDataArea->mRowIdColumnBlock,
                                           aRowIdExpr,
                                           & sBlockList,
                                           aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sBlockList != NULL );

    STL_TRY( knlInitShareBlock( & sCurBlockList,
                                sBlockList,
                                aRegionMem,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * Output 설정
     */

    *aRowIdBlockList = sCurBlockList;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief Index Column Expression들을 Index Key Column Order 순으로 Value Block List를 구성한다.
 * @param[in]      aDataArea          Data Area
 * @param[in]      aIndexColExprList  Index Column Expression List
 * @param[in]      aIndexHandle       Index Handle
 * @param[out]     aIndexColBlockList Index Based Value Block List
 * @param[in]      aRegionMem         Region Memory
 * @param[in]      aEnv               Environment
 *
 * @remark Value Block List의 Value Block은 Data Area에 이미 만들어진 Block을 공유한다.
 */
stlStatus qlndBuildIndexColBlockList( qllDataArea         * aDataArea,
                                      qlvRefExprList      * aIndexColExprList,
                                      ellDictHandle       * aIndexHandle,
                                      knlValueBlockList  ** aIndexColBlockList,
                                      knlRegionMem        * aRegionMem,
                                      qllEnv              * aEnv )
{
    knlValueBlockList  * sValueBlockList = NULL;
    knlValueBlockList  * sCurBlockList = NULL;
    knlValueBlockList  * sPrevBlockList = NULL;
    knlValueBlockList  * sBlockList = NULL;
    knlExprContext     * sContexts = NULL;
    qlvRefExprItem     * sRefExprItem = NULL;
    qlvInitExpression  * sInitExpr = NULL;

    ellIndexKeyDesc    * sIndexKeyDesc = NULL;
    stlInt32             sIndexKeyCnt = 0;
    stlInt32             sLoop = 0;
    stlInt64             sTableID = KNL_ANONYMOUS_TABLE_ID;
    stlInt32             sTabColOrder = 0;
    stlInt32             sIdxColOrder = 0;
    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexColExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexColBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    
    /**
     * Block Array 설정
     */
    
    sValueBlockList = NULL;
    sContexts       = aDataArea->mExprDataContext->mContexts;
    sRefExprItem    = aIndexColExprList->mHead;

    sIndexKeyCnt  = ellGetIndexKeyCount( aIndexHandle );
    sIndexKeyDesc = ellGetIndexKeyDesc( aIndexHandle );
    sTableID      = ellGetIndexTableID( aIndexHandle );

    while( sRefExprItem != NULL )
    {
        /**
         * Block List 생성
         */

        sInitExpr = sRefExprItem->mExprPtr;
        
        STL_DASSERT( aDataArea->mExprDataContext->mCount > sInitExpr->mOffset );

        STL_TRY( knlInitShareBlockFromBlock(
                     & sCurBlockList,
                     sContexts[ sInitExpr->mOffset ].mInfo.mValueInfo,
                     aRegionMem,
                     KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        
        /**
         * TableID 및 Column Order 설정
         */

        /* expression list로부터 table column order 얻기 */
        STL_DASSERT( sInitExpr->mType == QLV_EXPR_TYPE_COLUMN );
        sTabColOrder = ellGetColumnIdx( sInitExpr->mExpr.mColumn->mColumnHandle );

        /* table order로 부터 index column order 얻기 */
        for( sLoop = 0; sLoop < sIndexKeyCnt ; sLoop++ )
        {
            if( ellGetColumnIdx( & sIndexKeyDesc[ sLoop ].mKeyColumnHandle ) ==
                sTabColOrder )
            {
                sIdxColOrder = sLoop;
                break;
            }
        }

        STL_DASSERT( sLoop < sIndexKeyCnt );
        
        /* table id 및 column order 설정 */
        KNL_SET_BLOCK_TABLE_ID( sCurBlockList, sTableID );        
        KNL_SET_BLOCK_COLUMN_ORDER( sCurBlockList, sIdxColOrder );
        
        
        /**
         * Block List 를 연결 (ColumnOrder 순으로 Insertion Sort)
         */

        if( sValueBlockList == NULL )
        {
            sValueBlockList = sCurBlockList;
        }
        else
        {
            /* insertion sort */
            sBlockList     = sValueBlockList;
            sPrevBlockList = NULL;
            
            while( sBlockList != NULL )
            {
                STL_DASSERT( KNL_GET_BLOCK_COLUMN_ORDER( sBlockList ) != 
                             KNL_GET_BLOCK_COLUMN_ORDER( sCurBlockList ) );
                
                if( KNL_GET_BLOCK_COLUMN_ORDER( sBlockList ) >
                    KNL_GET_BLOCK_COLUMN_ORDER( sCurBlockList ) )
                {
                    /* insertion */
                    if( sPrevBlockList != NULL )
                    {
                        KNL_LINK_BLOCK_LIST( sCurBlockList, sBlockList );
                        KNL_LINK_BLOCK_LIST( sPrevBlockList, sCurBlockList );
                    }
                    else
                    {
                        KNL_LINK_BLOCK_LIST( sCurBlockList, sBlockList );
                        sValueBlockList = sCurBlockList;
                    }
                    break;
                }
                else
                {
                    sPrevBlockList = sBlockList;
                    sBlockList = sBlockList->mNext;
                }
            }

            if( sBlockList == NULL )
            {
                KNL_LINK_BLOCK_LIST( sPrevBlockList, sCurBlockList );                
            }
            else
            {
                /* Do Nothing */
            }
        }
        
        sRefExprItem = sRefExprItem->mNext;
    }


    /**
     * OUTPUT 설정
     */
    
    *aIndexColBlockList = sValueBlockList;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 참조하는 Value Block들을 모아 Value Block List를 구성한다.
 * @param[in]      aDataArea          Data Area
 * @param[in]      aPseudoColExprList Value Expression List
 * @param[out]     aValueBlockList    Value Block List
 * @param[in]      aRegionMem         Region Memory
 * @param[in]      aEnv               Environment
 *
 * @remark Value Block List의 Value Block은 Data Area에 이미 만들어진 Block을 공유한다.
 */
stlStatus qlndBuildPseudoColBlockList( qllDataArea           * aDataArea,
                                       qlvRefExprList        * aPseudoColExprList,
                                       knlValueBlockList    ** aValueBlockList,
                                       knlRegionMem          * aRegionMem,
                                       qllEnv                * aEnv )
{
    knlValueBlockList  * sValueBlockList = NULL;
    knlValueBlockList  * sCurBlockList   = NULL;
    knlValueBlockList  * sPrevBlockList  = NULL;
    qlvRefExprItem     * sRefExprItem    = NULL;
    qlvInitPseudoCol   * sPseudoCol      = NULL;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPseudoColExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Block Array 설정
     */

    sValueBlockList = NULL;
    sRefExprItem    = aPseudoColExprList->mHead;
    
    while( sRefExprItem != NULL )
    {
        /**
         * Block List 생성
         */

        STL_DASSERT( sRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_PSEUDO_COLUMN );
        
        sPseudoCol = sRefExprItem->mExprPtr->mExpr.mPseudoCol;
        
        STL_TRY( knlGetBlockList( aDataArea->mPseudoColBlock,
                                  sPseudoCol->mPseudoOffset,
                                  & sCurBlockList,
                                  KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        STL_TRY( knlInitShareBlock( & sCurBlockList,
                                    sCurBlockList,
                                    aRegionMem,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        
        /**
         * Block List 를 연결
         */

        if( sValueBlockList == NULL )
        {
            sValueBlockList = sCurBlockList;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevBlockList, sCurBlockList );
        }
        sPrevBlockList = sCurBlockList;
        
        sRefExprItem = sRefExprItem->mNext;
    }


    /**
     * OUTPUT 설정
     */
    
    *aValueBlockList = sValueBlockList;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief RETURN 구문의 Expression 을 수행한다.
 * @param[in]  aDataArea        Data Area
 * @param[in]  aIsReturnQuery   RETURN Query or RETURN INTO
 * @param[in]  aReturnBlockList RETURN Value Block List
 * @param[in]  aBlockIdx        Expression 을 수행할 Block Idx
 * @param[in]  aReturnExprCnt   RETURN Expression 개수
 * @param[in]  aReturnExprStack RETURN Expression Stack
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus qlnxExecuteReturnExprBlock( qllDataArea       * aDataArea,
                                      stlBool             aIsReturnQuery,
                                      knlValueBlockList * aReturnBlockList,
                                      stlInt64            aBlockIdx,
                                      stlInt32            aReturnExprCnt,
                                      knlExprStack      * aReturnExprStack,
                                      qllEnv            * aEnv )
{
    stlInt32              sReturnExprIdx   = 0;
    stlInt32              sReturnBlockIdx  = 0;
    knlExprEvalInfo       sReturnEvalInfo;
    knlValueBlockList   * sReturnBlock = NULL;
                                          
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReturnBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBlockIdx >= 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReturnExprCnt > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReturnExprStack > 0, QLL_ERROR_STACK(aEnv) );
    
    if ( aIsReturnQuery == STL_TRUE )
    {
        /**
         * RETURNING Query
         */
        
        sReturnBlockIdx = aBlockIdx;
    }
    else
    {
        /**
         * RETURNING INTO 는 RETURNING row 가 한 건이어야 함.
         */
        
        STL_TRY_THROW( aDataArea->mWriteRowCnt == 1,
                       RAMP_ERR_SINGLE_ROW_EXPRESSION );
        
        sReturnBlockIdx = 0;
    }
    
    /**
     * RETURNING Target Expression 을 수행
     */
    
    sReturnEvalInfo.mBlockCount = 1;
    sReturnEvalInfo.mContext    = aDataArea->mExprDataContext;
    
    for( sReturnExprIdx = 0, sReturnBlock = aReturnBlockList;
         sReturnExprIdx < aReturnExprCnt;
         sReturnExprIdx++, sReturnBlock = sReturnBlock->mNext )
    {
        /**
         * Evaluation Information 구성
         */
        
        sReturnEvalInfo.mExprStack      = & aReturnExprStack[ sReturnExprIdx ];
        sReturnEvalInfo.mResultBlock    = sReturnBlock;
        sReturnEvalInfo.mBlockIdx       = sReturnBlockIdx;

        /**
         * RETURN Expression 수행
         */

        STL_TRY( knlEvaluateOneExpression( &sReturnEvalInfo,
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SINGLE_ROW_EXPRESSION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INTO_CLAUSE_MULTIPLE_ROWS,
                      NULL,
                      QLL_ERROR_STACK(aEnv) );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Instant Table의 Aggregation 수행을 위한 Merge Record 정보를 구축한다.
 * @param[in,out] aSQLStmt        SQL Statement
 * @param[in]     aAggrOptInfo    Aggregation Optimization Info
 * @param[in]     aAggrFuncList   Aggregation Function List
 * @param[in]     aIniInstant     Instant Validation Node
 * @param[in,out] aMergeRecord    Merge Record
 * @param[in]     aDataArea       Data Area
 * @param[in]     aRegionMem      Region Memory
 * @param[in]     aEnv            Environment
 *
 * @remark Merge Record는 Non-Distinct Aggregation만 구축 가능하다.
 */
stlStatus qlndSetMergeRecordInfo( qllStatement         * aSQLStmt,
                                  qlnfAggrOptInfo      * aAggrOptInfo,
                                  qlvRefExprList       * aAggrFuncList,
                                  qlvInitInstantNode   * aIniInstant,
                                  smlMergeRecordInfo  ** aMergeRecord,
                                  qllDataArea          * aDataArea,
                                  knlRegionMem         * aRegionMem,
                                  qllEnv               * aEnv )
{
    smlAggrFuncInfo     * sAggrFuncInfo  = NULL;
    smlMergeRecordInfo  * sMergeRecord   = NULL;
    qlvInitAggregation  * sAggrExpr      = NULL;
    qlvInitExpression   * sRecColExpr    = NULL;
    qlvRefExprItem      * sRefExprItem   = NULL;
    qlvRefExprItem      * sRefRecColItem = NULL;

    knlValueBlockList   * sLastUpdateCol = NULL;
    knlValueBlockList   * sLastColBlock  = NULL;
    knlValueBlockList   * sNewBlock      = NULL;
    knlExprEvalInfo     * sEvalInfo      = NULL;
    knlExprContext      * sContexts      = NULL;
    knlBuiltInAggrFunc    sAggrFuncId    = KNL_BUILTIN_AGGREGATION_INVALID;
    dtlDataType           sDataType      = DTL_TYPE_NA;
    
    stlInt32              sLoop          = 0;
    stlInt32              sIdx           = 0;
    stlInt32              sColOrder      = 0;
    stlInt32              sOffset        = 0;
    stlBool               sIsExist       = STL_FALSE;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrFuncList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIniInstant != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aMergeRecord != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Merge Record Info 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( smlAggrFuncInfo ),
                                (void **) & sMergeRecord,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    *aMergeRecord = sMergeRecord;


    /**
     * Aggregation Function Info 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( smlAggrFuncInfo ) * aAggrFuncList->mCount,
                                (void **) & sAggrFuncInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sAggrFuncInfo,
               0x00,
               STL_SIZEOF( smlAggrFuncInfo ) * aAggrFuncList->mCount );

    sMergeRecord->mAggrFuncInfoList = sAggrFuncInfo;
    

    /**
     * Aggregation Function Info Link
     */
    
    for( sLoop = 0 ; sLoop < aAggrFuncList->mCount ; sLoop++ )
    {
        sAggrFuncInfo[ sLoop ].mNext = & sAggrFuncInfo[ sLoop + 1 ];
    }
    sAggrFuncInfo[ sLoop - 1 ].mNext = NULL;


    /**
     * Aggregation Fetch Info 설정
     */

    sContexts    = aDataArea->mExprDataContext->mContexts;
    sRefExprItem = aAggrFuncList->mHead;
    
    for( sLoop = 0 ; sLoop < aAggrFuncList->mCount ; sLoop++ )
    {
        STL_DASSERT( sRefExprItem->mExprPtr->mType == QLV_EXPR_TYPE_AGGREGATION );

        sRecColExpr    = NULL;
        sRefRecColItem = aIniInstant->mRecColList->mHead;
        sColOrder      = aIniInstant->mKeyColCnt;

        while( sRefRecColItem != NULL )
        {
            STL_DASSERT( sRefRecColItem->mExprPtr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

            if( sRefRecColItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr ==
                sRefExprItem->mExprPtr )
            {
                sRecColExpr = sRefRecColItem->mExprPtr;
                break;
            }
            else
            {
                sColOrder++;
                sRefRecColItem = sRefRecColItem->mNext;
            }
        }

        STL_DASSERT( sRecColExpr != NULL );

        sAggrExpr   = sRefExprItem->mExprPtr->mExpr.mAggregation;
        sAggrFuncId = sAggrExpr->mAggrId;
        

        /**
         * Input Argument Value Block List
         */

        if( sAggrExpr->mArgCount > 0 )
        {
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( knlExprEvalInfo * ) * sAggrExpr->mArgCount,
                                        (void **) & sAggrFuncInfo->mEvalInfo,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( knlExprEvalInfo )  * sAggrExpr->mArgCount,
                                        (void **) & sEvalInfo,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( knlExprEvalInfo * ),
                                        (void **) & sAggrFuncInfo->mEvalInfo,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sAggrFuncInfo->mEvalInfo[ 0 ] = NULL;
        }

        sLastColBlock = NULL;
        for( sIdx = 0 ; sIdx < sAggrExpr->mArgCount ; sIdx++ )
        {
            switch( sAggrExpr->mArgs[ sIdx ]->mType )
            {
                case QLV_EXPR_TYPE_FUNCTION :
                case QLV_EXPR_TYPE_CAST :
                case QLV_EXPR_TYPE_CASE_EXPR :
                case QLV_EXPR_TYPE_LIST_FUNCTION:
                case QLV_EXPR_TYPE_LIST_COLUMN:
                    {
                        sIsExist = STL_FALSE;
                        break;
                    }
                default :
                    {
                        sIsExist = STL_TRUE;
                        break;
                    }
            }

            sOffset = sAggrExpr->mArgs[ sIdx ]->mOffset;

            if( ( sIsExist == STL_TRUE ) &&
                ( sContexts[ sOffset ].mCast == NULL ) )
            {
                /**
                 * 연관된 Value Block이 있고, Internal Cast가 없으면 Expression에 대한 Value Block을 연결
                 */
        
                STL_TRY( knlInitShareBlockFromBlock(
                             & sNewBlock,
                             sContexts[ sOffset ].mInfo.mValueInfo,
                             aRegionMem,
                             KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
            else
            {
                /**
                 * 새로운 Value Block 할당
                 */

                if( sContexts[ sOffset ].mCast == NULL )
                {
                    sDataType = sContexts[ sOffset ].mInfo.mFuncData->mResultValue.mType;
                }
                else
                {
                    sDataType = sContexts[ sOffset ].mCast->mCastResultBuf.mType;
                }

                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & sNewBlock,
                                            aDataArea->mBlockAllocCnt,
                                            STL_TRUE,
                                            STL_LAYER_SQL_PROCESSOR,
                                            0,   /* table ID */
                                            0,   /* column ID */
                                            sDataType,
                                            gResultDataTypeDef[ sDataType ].mArgNum1,
                                            gResultDataTypeDef[ sDataType ].mArgNum2,
                                            gResultDataTypeDef[ sDataType ].mStringLengthUnit,
                                            gResultDataTypeDef[ sDataType ].mIntervalIndicator,
                                            aRegionMem,
                                            aEnv )
                         == STL_SUCCESS );
            }

            if( sLastColBlock == NULL )
            {
                sAggrFuncInfo->mInputArgList = sNewBlock;
            }
            else
            {
                KNL_LINK_BLOCK_LIST( sLastColBlock, sNewBlock );
            }
            sLastColBlock = sNewBlock;

            KNL_SET_BLOCK_COLUMN_ORDER( sNewBlock, sColOrder );
                                    
            
            /**
             * Fetch Info 설정
             */
            
            /* expression stack */
            sAggrFuncInfo->mInputArgStack = aAggrOptInfo[ sLoop ].mArgCodeStack[ sIdx ];

            if( sAggrFuncInfo->mInputArgStack == NULL )
            {
                sAggrFuncInfo->mEvalInfo[ sIdx ] = NULL;
            }
            else
            {
                /* evaluate info */
                sEvalInfo->mExprStack   = sAggrFuncInfo->mInputArgStack;
                sEvalInfo->mContext     = aDataArea->mExprDataContext;
                sEvalInfo->mResultBlock = sAggrFuncInfo->mInputArgList;
                sEvalInfo->mBlockIdx    = 0;
                sEvalInfo->mBlockCount  = 0;

                sAggrFuncInfo->mEvalInfo[ sIdx ] = sEvalInfo;
            }
            sEvalInfo++;
        }


        /**
         * Aggregation Value Block List 생성
         */

        STL_TRY( knlInitShareBlockFromBlock(
                     & sAggrFuncInfo->mAggrValueList,
                     sContexts[ sRecColExpr->mExpr.mInnerColumn->mOrgExpr->mOffset ].mInfo.mValueInfo,
                     aRegionMem,
                     KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        
        /**
         * Fetch Info 설정
         */

        sAggrFuncInfo->mAggrFuncModule = &gSmlAggrFuncInfoArr[sAggrFuncId];
        sAggrFuncInfo->mAggrMergeFunc = gSmlAggrMergeFuncInfoArr[sAggrFuncId];
            
        
        /**
         * Update Column List 설정
         */

        STL_TRY( knlInitShareBlock( & sNewBlock,
                                    sAggrFuncInfo->mAggrValueList,
                                    aRegionMem,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        KNL_SET_BLOCK_COLUMN_ORDER( sNewBlock, sColOrder );
        
        if( sLastUpdateCol == NULL )
        {
            sMergeRecord->mUpdateColList = sNewBlock;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sLastUpdateCol, sNewBlock );
        }
        sLastUpdateCol = sNewBlock;


        /**
         * 다음 Aggregation으로 이동
         */

        sAggrFuncInfo++;
        sRefExprItem = sRefExprItem->mNext;
    }


    /**
     * Merge Record Info 설정
     */

    sMergeRecord->mReadColList = sMergeRecord->mUpdateColList;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Grouping Aggregation Distinct Function에 대한 Conflict Check 정보를 구축한다.
 * @param[in,out]  aSQLStmt            SQL Statement
 * @param[in]      aAggrDistFuncCnt    Aggregation Distinct Function 개수
 * @param[in]      aAggrFuncCnt        Aggregation Optimization Info 개수
 * @param[in]      aAggrOptInfo        Aggregation Optimization Info
 * @param[in]      aKeyColCnt          Key Column 개수
 * @param[in]      aKeyColList         Key Column Value Block List
 * @param[in]      aMergeRecord        Merge Record 정보
 * @param[in]      aDataArea           Data Area (Data Optimization 결과물)
 * @param[out]     aConflictCheckInfo  Conflict Check Info
 * @param[in]      aRegionMem          Region Memory
 * @param[in]      aEnv                Environment
 */
stlStatus qlndSetConflictCheckInfo( qllStatement            * aSQLStmt,
                                    stlInt32                  aAggrDistFuncCnt,
                                    stlInt32                  aAggrFuncCnt,
                                    qlnfAggrOptInfo         * aAggrOptInfo,
                                    stlInt32                  aKeyColCnt,
                                    knlValueBlockList       * aKeyColList,
                                    smlMergeRecordInfo      * aMergeRecord,
                                    qllDataArea             * aDataArea,
                                    qlnxConflictCheckInfo  ** aConflictCheckInfo,
                                    knlRegionMem            * aRegionMem,
                                    qllEnv                  * aEnv )
{
    qlnxConflictCheckInfo  * sConflictCheckInfo = NULL;
    qlnfAggrOptInfo        * sAggrOptInfo       = NULL;
    qlvInitAggregation     * sAggrExpr          = NULL;
    smlAggrFuncInfo        * sMergeRecordFunc   = NULL;
    knlValueBlockList      * sKeyColList        = NULL;
    knlValueBlockList      * sConflictColList   = NULL;
    knlValueBlockList      * sLastColBlock      = NULL;
    knlValueBlockList      * sColumnBlock       = NULL;
    knlValueBlockList      * sNewBlock          = NULL;
    
    stlInt32                 sColOrder          = 0;
    stlInt32                 sArgIdx            = 0;
    stlInt32                 sLoop              = 0;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrDistFuncCnt > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrFuncCnt > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAggrOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyColCnt > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyColList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aMergeRecord != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aConflictCheckInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Conflict Info 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlnxConflictCheckInfo ) * aAggrDistFuncCnt,
                                (void **) & sConflictCheckInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sConflictCheckInfo,
               0x00,
               STL_SIZEOF( qlnxConflictCheckInfo ) * aAggrDistFuncCnt );

    *aConflictCheckInfo = sConflictCheckInfo;


    /**
     * Confilict Info 설정
     */

    sAggrOptInfo     = aAggrOptInfo;
    sMergeRecordFunc = aMergeRecord->mAggrFuncInfoList;
    

    /**
     * Conflict Column List 구성
     */

    for( sLoop = 0 ; sLoop < aAggrFuncCnt ; sLoop++ )
    {
        /**
         * Distinct 옵션을 포함한 Aggregation만 평가
         */

        if( sAggrOptInfo->mAggrExpr->mExpr.mAggregation->mIsDistinct == STL_FALSE )
        {
            sAggrOptInfo++;
            sMergeRecordFunc = sMergeRecordFunc->mNext;
            continue;
        }
        else
        {
            /* Do Nothing */
        }

        
        /**
         * Key Column List 구성
         */
        
        sColumnBlock  = aKeyColList;
        sLastColBlock = NULL;
        sColOrder     = 0;
        
        while( sColumnBlock != NULL )
        {
            STL_TRY( knlInitShareBlock( & sNewBlock,
                                        sColumnBlock,
                                        aRegionMem,
                                        KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
            KNL_SET_BLOCK_COLUMN_ORDER( sNewBlock, sColOrder );

            if( sLastColBlock == NULL )
            {
                sKeyColList = sNewBlock;
            }
            else
            {
                KNL_LINK_BLOCK_LIST( sLastColBlock, sNewBlock );
            }
            sLastColBlock = sNewBlock;

            sColOrder++;
            sColumnBlock = sColumnBlock->mNext;
        }

        sConflictCheckInfo->mConflictColCnt = sColOrder;
        STL_DASSERT( sColOrder == aKeyColCnt );

        sAggrExpr = sAggrOptInfo->mAggrExpr->mExpr.mAggregation;
        STL_DASSERT( sAggrExpr->mArgCount > 0 );

        sMergeRecordFunc->mInputArgStack = NULL;
        sColumnBlock = sMergeRecordFunc->mInputArgList;
        
        for( sArgIdx = 0 ; sArgIdx < sAggrExpr->mArgCount ; sArgIdx++ )
        {
            /* aggregation을 위한 merge record info의 value block 설정 */
            sMergeRecordFunc->mEvalInfo[ sArgIdx ] = NULL;

            STL_DASSERT( aDataArea->mExprDataContext->mContexts[ sAggrExpr->mArgs[ sArgIdx ]->mOffset ].mCast == NULL );

            STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                        & sNewBlock,
                                        aDataArea->mBlockAllocCnt,
                                        STL_TRUE,
                                        STL_LAYER_SQL_PROCESSOR,
                                        KNL_GET_BLOCK_TABLE_ID( sColumnBlock ),
                                        KNL_GET_BLOCK_COLUMN_ORDER( sColumnBlock ),
                                        KNL_GET_BLOCK_DB_TYPE( sColumnBlock ),
                                        KNL_GET_BLOCK_ARG_NUM1( sColumnBlock ),
                                        KNL_GET_BLOCK_ARG_NUM2( sColumnBlock ),
                                        KNL_GET_BLOCK_STRING_LENGTH_UNIT( sColumnBlock ),
                                        KNL_GET_BLOCK_INTERVAL_INDICATOR( sColumnBlock ),
                                        aRegionMem,
                                        aEnv )
                     == STL_SUCCESS );
                
            KNL_SET_BLOCK_COLUMN_ORDER( sNewBlock, sColOrder );
            
            if( sArgIdx == 0 )
            {
                sConflictColList = sNewBlock;
            }
            else
            {
                /* Do Nothing */
            }
            KNL_LINK_BLOCK_LIST( sLastColBlock, sNewBlock );
            sLastColBlock = sNewBlock;

            sColumnBlock = sColumnBlock->mNext;
            sColOrder++;
        }
        
        /* 새로 할당 받은 Value Block을 Merge Record의 Input Value로 설정 */
        sMergeRecordFunc->mInputArgList = sConflictColList;

        sConflictCheckInfo->mInsertColCnt    = sColOrder;

        sConflictCheckInfo->mInsertColList   = sKeyColList;
        sConflictCheckInfo->mConflictColList = sConflictColList;
        sConflictCheckInfo->mConflictResult  = aDataArea->mPredResultBlock;


        /**
         * Instant Table 설정
         */

        stlMemset( & sConflictCheckInfo->mInstantTable, 0x00, STL_SIZEOF( qlnInstantTable ) );
        sConflictCheckInfo->mInstantTable.mIsNeedBuild = STL_TRUE;

        sAggrOptInfo++;
        sMergeRecordFunc = sMergeRecordFunc->mNext;
        sConflictCheckInfo++;
    }
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Block Idx에 해당하는 값으로 해당 노드의 Outer Column Value를 설정한다.
 * @param[in]     aDataArea       Data Area
 * @param[in]     aNodeIdx        Node Idx
 * @param[in]     aBlockIdx       Block Idx
 * @param[in]     aEnv            Environment
 */
stlStatus qlndSetOuterColumnValue( qllDataArea  * aDataArea,
                                   stlInt16       aNodeIdx,
                                   stlInt32       aBlockIdx,
                                   qllEnv       * aEnv )
{
    qlnxCommonInfo    * sCommonInfo       = NULL;
    knlValueBlockList * sOuterColBlock    = NULL;
    knlValueBlockList * sOuterOrgColBlock = NULL;


    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNodeIdx >= 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBlockIdx >= 0, QLL_ERROR_STACK(aEnv) );
    

    STL_RAMP( RAMP_RETRY );


    /**
     * Common Info 얻기
     */

    sCommonInfo = (qlnxCommonInfo*) aDataArea->mExecNodeList[ aNodeIdx ].mExecNode;
    

    /**
     * Outer Value 설정
     */

    if( sCommonInfo->mOuterColBlock != NULL )
    {
        STL_DASSERT( sCommonInfo->mOuterOrgColBlock != NULL );
        
        sOuterColBlock    = sCommonInfo->mOuterColBlock;
        sOuterOrgColBlock = sCommonInfo->mOuterOrgColBlock;
    
        while( sOuterColBlock != NULL )
        {
            STL_TRY( knlCopyDataValue( KNL_GET_BLOCK_DATA_VALUE( sOuterOrgColBlock, aBlockIdx ),
                                       KNL_GET_BLOCK_FIRST_DATA_VALUE( sOuterColBlock ),
                                       KNL_ENV( aEnv ) )
                     == STL_SUCCESS );

            sOuterColBlock    = sOuterColBlock->mNext;
            sOuterOrgColBlock = sOuterOrgColBlock->mNext;
        }
    }
    else
    {
        if( aDataArea->mExecNodeList[ aNodeIdx ].mNodeType == QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE )
        {
            qlnoSubQueryList    * sSubQueryList     = NULL;

            sSubQueryList = (qlnoSubQueryList*)(aDataArea->mExecNodeList[ aNodeIdx ].mOptNode->mOptNode);

            aNodeIdx = QLL_GET_OPT_NODE_IDX( sSubQueryList->mChildNode );

            STL_THROW( RAMP_RETRY );
        }
        else if( aDataArea->mExecNodeList[ aNodeIdx ].mNodeType == QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE )
        {
            qlnoSubQueryFilter  * sSubQueryFilter   = NULL;
            qlnoSubQueryList    * sSubQueryList     = NULL;

            sSubQueryFilter = (qlnoSubQueryFilter*)(aDataArea->mExecNodeList[ aNodeIdx ].mOptNode->mOptNode);
            sSubQueryList = (qlnoSubQueryList*)(sSubQueryFilter->mChildNode->mOptNode);

            aNodeIdx = QLL_GET_OPT_NODE_IDX( sSubQueryList->mChildNode );

            STL_THROW( RAMP_RETRY );
        }
        else
        {
            /* Do Nothing */
        }
    }
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Outer Column Block들에 대한 Value Block List를 구성한다.
 * @param[in]      aDataArea                Data Area
 * @param[in]      aValueExprList           Outer Value Expression List
 * @param[out]     aOuterColBlockList       Outer Value Block List
 * @param[out]     aOuterOrgColBlockList    Original Value Block List for Outer Columns
 * @param[in]      aRegionMem               Region Memory
 * @param[in]      aEnv                     Environment
 *
 * @remark Value Block List의 Value Block은 Data Area에 이미 만들어진 Block을 공유한다.
 */
stlStatus qlndBuildOuterColBlockList( qllDataArea         * aDataArea,
                                      qlvRefExprList      * aValueExprList,
                                      knlValueBlockList  ** aOuterColBlockList,
                                      knlValueBlockList  ** aOuterOrgColBlockList,
                                      knlRegionMem        * aRegionMem,
                                      qllEnv              * aEnv )
{

    knlValueBlockList   * sOuterColBlock      = NULL;
    knlValueBlockList   * sOuterOrgColBlock   = NULL;
    qlvRefExprItem      * sExprItem           = NULL;

    knlExprContextInfo  * sExprContext        = NULL;
    knlExprContext      * sOuterColContext    = NULL;
    knlExprContext      * sOuterOrgColContext = NULL;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOuterColBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOuterOrgColBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**
     * Block Array 설정
     */

    sExprContext = aDataArea->mExprDataContext;
    
    sExprItem = aValueExprList->mHead;
        
    while( sExprItem != NULL  )
    {
        STL_DASSERT( sExprItem->mExprPtr->mType == QLV_EXPR_TYPE_OUTER_COLUMN );
        STL_DASSERT( ( sExprItem->mExprPtr->mExpr.mOuterColumn->mOrgExpr->mType ==
                       QLV_EXPR_TYPE_COLUMN ) ||
                     ( sExprItem->mExprPtr->mExpr.mOuterColumn->mOrgExpr->mType ==
                       QLV_EXPR_TYPE_INNER_COLUMN ) ||
                     ( sExprItem->mExprPtr->mExpr.mOuterColumn->mOrgExpr->mType ==
                       QLV_EXPR_TYPE_OUTER_COLUMN ) ||
                     ( sExprItem->mExprPtr->mExpr.mOuterColumn->mOrgExpr->mType ==
                       QLV_EXPR_TYPE_ROWID_COLUMN ) );
                    
        sOuterColContext    = & sExprContext->mContexts[ sExprItem->mExprPtr->mOffset ];
        sOuterOrgColContext = & sExprContext->mContexts
            [ sExprItem->mExprPtr->mExpr.mOuterColumn->mOrgExpr->mOffset ];
            
        STL_TRY( knlInitShareBlockFromBlock( & sOuterColBlock,
                                             sOuterColContext->mInfo.mValueInfo,
                                             aRegionMem,
                                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlInitShareBlockFromBlock( & sOuterOrgColBlock,
                                             sOuterOrgColContext->mInfo.mValueInfo,
                                             aRegionMem,
                                             KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( *aOuterColBlockList == NULL )
        {
            /* Do Nothing */
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sOuterColBlock, *aOuterColBlockList );
            KNL_LINK_BLOCK_LIST( sOuterOrgColBlock, *aOuterOrgColBlockList );
        }
            
        *aOuterColBlockList    = sOuterColBlock;
        *aOuterOrgColBlockList = sOuterOrgColBlock;

        sExprItem = sExprItem->mNext;
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Node의 Row Block을 각 Row Block의 Opt Node Idx순으로 저장한다.
 * @param[in]       aRegionMem          Region Memory
 * @param[in]       aBlockAllocCount    Block의 할당 개수
 * @param[in]       aLeftRowBlockList   Left Row Block List
 * @param[in]       aRightRowBlockList  Left Row Block List
 * @param[in,out]   aResultRowBlockList Left Row Block List
 * @param[in]       aEnv                Environment
 */
stlStatus qlndSetRowBlockOnJoinNode( knlRegionMem       * aRegionMem,
                                     stlInt32             aBlockAllocCount,
                                     qlnxRowBlockList   * aLeftRowBlockList,
                                     qlnxRowBlockList   * aRightRowBlockList,
                                     qlnxRowBlockList   * aResultRowBlockList,
                                     qllEnv             * aEnv )
{
    qlnxRowBlockItem    * sNewRowBlockItem  = NULL;
    qlnxRowBlockItem    * sOrgRowBlockItem  = NULL;
    qlnxRowBlockItem    * sPrevRowBlockItem = NULL;
    qlnxRowBlockItem    * sCurrRowBlockItem = NULL;


#define ALLOC_AND_INIT_ROW_BLOCK_ITEM( _aRowBlockItem,                              \
                                       _aLeafOptNode,                               \
                                       _aOrgRowBlock,                               \
                                       _aIsLeftTableRowBlock )                      \
    {                                                                               \
        /* Row Block Item 공간 할당 */                                              \
        STL_TRY( knlAllocRegionMem( aRegionMem,                                     \
                                    STL_SIZEOF( qlnxRowBlockItem ),                 \
                                    (void **) & (_aRowBlockItem),                   \
                                    KNL_ENV(aEnv) )                                 \
                 == STL_SUCCESS );                                                  \
                                                                                    \
        /* Row Block 공간 할당 */                                                   \
        STL_TRY( knlAllocRegionMem( aRegionMem,                                     \
                                    STL_SIZEOF( smlRowBlock ),                      \
                                    (void **) & (_aRowBlockItem)->mRowBlock,        \
                                    KNL_ENV(aEnv) )                                 \
                 == STL_SUCCESS );                                                  \
                                                                                    \
        /* Row Block  초기화 */                                                     \
        STL_TRY( smlInitRowBlock( (_aRowBlockItem)->mRowBlock,                      \
                                  aBlockAllocCount,                                 \
                                  aRegionMem,                                       \
                                  SML_ENV(aEnv) )                                   \
                 == STL_SUCCESS );                                                  \
                                                                                    \
        /* Optimization Node 및 Original Row Block, Left Table Row Block 설정 */    \
        (_aRowBlockItem)->mLeafOptNode = (_aLeafOptNode);                           \
        (_aRowBlockItem)->mOrgRowBlock = (_aOrgRowBlock);                           \
        (_aRowBlockItem)->mIsLeftTableRowBlock = (_aIsLeftTableRowBlock);           \
        (_aRowBlockItem)->mNext = NULL;                                             \
    }

#define GET_TABLE_ID_AND_NAME( _aOptNode, _aTableID, _aRelationName )                           \
    {                                                                                           \
        if( (_aOptNode)->mType == QLL_PLAN_NODE_TABLE_ACCESS_TYPE )                             \
        {                                                                                       \
            qlnoTableAccess * _sTableAccess = NULL;                                             \
                                                                                                \
            _sTableAccess = (qlnoTableAccess*)((_aOptNode)->mOptNode);                          \
                                                                                                \
            /* Table ID */                                                                      \
            (_aTableID) =                                                                       \
                ellGetTableID( _sTableAccess->mTableHandle );                                   \
                                                                                                \
            /* Relation Name */                                                                 \
            STL_DASSERT( (_sTableAccess->mRelationNode != NULL) &&                              \
                         (_sTableAccess->mRelationNode->mType == QLV_NODE_TYPE_BASE_TABLE) );   \
            (_aRelationName) =                                                                  \
                ((qlvInitBaseTableNode*)(_sTableAccess->mRelationNode))->mRelationName;         \
        }                                                                                       \
        else if( (_aOptNode)->mType == QLL_PLAN_NODE_INDEX_ACCESS_TYPE )                        \
        {                                                                                       \
            qlnoIndexAccess * _sIndexAccess = NULL;                                             \
                                                                                                \
            _sIndexAccess = (qlnoIndexAccess*)((_aOptNode)->mOptNode);                          \
                                                                                                \
            /* Table ID */                                                                      \
            (_aTableID) =                                                                       \
                ellGetTableID( _sIndexAccess->mTableHandle );                                   \
                                                                                                \
            /* Relation Name */                                                                 \
            STL_DASSERT( (_sIndexAccess->mRelationNode != NULL) &&                              \
                         (_sIndexAccess->mRelationNode->mType == QLV_NODE_TYPE_BASE_TABLE) );   \
            (_aRelationName) =                                                                  \
                ((qlvInitBaseTableNode*)(_sIndexAccess->mRelationNode))->mRelationName;         \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            qlnoRowIdAccess * _sRowIdAccess = NULL;                                             \
                                                                                                \
            STL_DASSERT( (_aOptNode)->mType == QLL_PLAN_NODE_ROWID_ACCESS_TYPE );               \
            _sRowIdAccess = (qlnoRowIdAccess*)((_aOptNode)->mOptNode);                          \
                                                                                                \
            /* Table ID */                                                                      \
            (_aTableID) =                                                                       \
                ellGetTableID( _sRowIdAccess->mTableHandle );                                   \
                                                                                                \
            /* Relation Name */                                                                 \
            STL_DASSERT( (_sRowIdAccess->mRelationNode != NULL) &&                              \
                         (_sRowIdAccess->mRelationNode->mType == QLV_NODE_TYPE_BASE_TABLE) );   \
            (_aRelationName) =                                                                  \
                ((qlvInitBaseTableNode*)(_sRowIdAccess->mRelationNode))->mRelationName;         \
        }                                                                                       \
    }

#define COMPARE_STRING( _aStr1, _aStr2, _aCmpResult )               \
    {                                                               \
        if( (_aStr1) == NULL )                                      \
        {                                                           \
            if( (_aStr2) == NULL )                                  \
            {                                                       \
                (_aCmpResult) = 0;                                  \
            }                                                       \
            else                                                    \
            {                                                       \
                (_aCmpResult) = -1;                                 \
            }                                                       \
        }                                                           \
        else                                                        \
        {                                                           \
            if( (_aStr2) == NULL )                                  \
            {                                                       \
                (_aCmpResult) = 1;                                  \
            }                                                       \
            else                                                    \
            {                                                       \
                (_aCmpResult) = stlStrcmp(_aStr1, _aStr2);          \
            }                                                       \
        }                                                           \
    }

#define ADD_ROW_BLOCK_ITEM_TO_ROW_BLOCK_LIST( _aRowBlockList, _aNewRowBlockItem )   \
    {                                                                               \
        stlInt32          _sCompareResult;                                          \
        stlInt64          _sCurTableID;                                             \
        stlInt64          _sNewTableID;                                             \
        qlvRelationName * _sCurRelationName = NULL;                                 \
        qlvRelationName * _sNewRelationName = NULL;                                 \
                                                                                    \
        sPrevRowBlockItem = NULL;                                                   \
        sCurrRowBlockItem = (_aRowBlockList)->mHead;                                \
        /* @todo 현재 Row Block은 Table/Index/Rowid Access 노드들만 가질 수 있음 */ \
        GET_TABLE_ID_AND_NAME( (_aNewRowBlockItem)->mLeafOptNode,                   \
                               _sNewTableID,                                        \
                               _sNewRelationName );                                 \
        while( sCurrRowBlockItem != NULL )                                          \
        {                                                                           \
            GET_TABLE_ID_AND_NAME( sCurrRowBlockItem->mLeafOptNode,                 \
                                   _sCurTableID,                                    \
                                   _sCurRelationName );                             \
                                                                                    \
            if( _sNewTableID < _sCurTableID )                                       \
            {                                                                       \
                break;                                                              \
            }                                                                       \
            else if( _sNewTableID == _sCurTableID )                                 \
            {                                                                       \
                /* Catalog 비교 */                                                  \
                COMPARE_STRING( _sNewRelationName->mCatalog,                        \
                                _sCurRelationName->mCatalog,                        \
                                _sCompareResult );                                  \
                if( _sCompareResult < 0 )                                           \
                {                                                                   \
                    break;                                                          \
                }                                                                   \
                else if( _sCompareResult == 0 )                                     \
                {                                                                   \
                    /* Schema 비교 */                                               \
                    COMPARE_STRING( _sNewRelationName->mSchema,                     \
                                    _sCurRelationName->mSchema,                     \
                                    _sCompareResult );                              \
                    if( _sCompareResult < 0 )                                       \
                    {                                                               \
                        break;                                                      \
                    }                                                               \
                    else if( _sCompareResult == 0 )                                 \
                    {                                                               \
                        /* Table 비교 */                                            \
                        COMPARE_STRING( _sNewRelationName->mTable,                  \
                                        _sCurRelationName->mTable,                  \
                                        _sCompareResult );                          \
                        if( _sCompareResult <= 0 )                                  \
                        {                                                           \
                            break;                                                  \
                        }                                                           \
                    }                                                               \
                }                                                                   \
            }                                                                       \
                                                                                    \
            sPrevRowBlockItem = sCurrRowBlockItem;                                  \
            sCurrRowBlockItem = sCurrRowBlockItem->mNext;                           \
        }                                                                           \
                                                                                    \
        if( sPrevRowBlockItem == NULL )                                             \
        {                                                                           \
            (_aNewRowBlockItem)->mNext = (_aRowBlockList)->mHead;                   \
            (_aRowBlockList)->mHead = (_aNewRowBlockItem);                          \
            if( (_aRowBlockList)->mCount == 0 )                                     \
            {                                                                       \
                (_aRowBlockList)->mTail = (_aNewRowBlockItem);                      \
            }                                                                       \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            (_aNewRowBlockItem)->mNext = sPrevRowBlockItem->mNext;                  \
            sPrevRowBlockItem->mNext = (_aNewRowBlockItem);                         \
            if( (_aNewRowBlockItem)->mNext == NULL )                                \
            {                                                                       \
                (_aRowBlockList)->mTail = (_aNewRowBlockItem);                      \
            }                                                                       \
        }                                                                           \
        (_aRowBlockList)->mCount++;                                                 \
    }


    /****************************************************************
     * Paramter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aLeftRowBlockList != NULL) || (aRightRowBlockList != NULL),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aResultRowBlockList != NULL, QLL_ERROR_STACK(aEnv) );


    /****************************************************************
     * Row Block 정보 설정
     ****************************************************************/

    /**
     * Left(Outer) Row Block 정보 추가
     */

    if( aLeftRowBlockList != NULL )
    {
        sOrgRowBlockItem = aLeftRowBlockList->mHead;
        while( sOrgRowBlockItem != NULL )
        {
            ALLOC_AND_INIT_ROW_BLOCK_ITEM( sNewRowBlockItem,
                                           sOrgRowBlockItem->mLeafOptNode,
                                           sOrgRowBlockItem->mRowBlock,
                                           STL_TRUE );

            ADD_ROW_BLOCK_ITEM_TO_ROW_BLOCK_LIST( aResultRowBlockList,
                                                  sNewRowBlockItem );

            sOrgRowBlockItem = sOrgRowBlockItem->mNext;
        }
    }


    /**
     * Right(Inner) Row Block 정보 추가
     */

    if( aRightRowBlockList != NULL )
    {
        sOrgRowBlockItem = aRightRowBlockList->mHead;
        while( sOrgRowBlockItem != NULL )
        {
            ALLOC_AND_INIT_ROW_BLOCK_ITEM( sNewRowBlockItem,
                                           sOrgRowBlockItem->mLeafOptNode,
                                           sOrgRowBlockItem->mRowBlock,
                                           STL_FALSE );

            ADD_ROW_BLOCK_ITEM_TO_ROW_BLOCK_LIST( aResultRowBlockList,
                                                  sNewRowBlockItem );

            sOrgRowBlockItem = sOrgRowBlockItem->mNext;
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Filter 수행을 위한 Evaluation Info 정보 구성
 * @param[in]  aExprDataContext      Expression Context Info
 * @param[in]  aFilterStack          Filter Expression Stack
 * @param[out] aFilterEvalInfo       Filter Evaluation Info
 * @param[in]  aRegionMem            Region Memory
 * @param[in]  aEnv                  Environment
 */
stlStatus qlndMakePrepareStackEvalInfo( knlExprContextInfo  * aExprDataContext,
                                        knlExprStack        * aFilterStack,
                                        knlExprEvalInfo    ** aFilterEvalInfo,
                                        knlRegionMem        * aRegionMem,
                                        qllEnv              * aEnv )
{
    knlExprEvalInfo  * sFilterEvalInfo = NULL;
    dtlDataValue     * sDataValue      = NULL;


    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aExprDataContext != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFilterEvalInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * Filter Stack 수행을 위한 Eval 정보 구성
     */
    
    if( aFilterStack != NULL )
    {
        STL_DASSERT( aFilterStack->mEntryCount > 0 );

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( knlExprEvalInfo ),
                                    (void **) & sFilterEvalInfo,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sFilterEvalInfo->mExprStack = aFilterStack;
        sFilterEvalInfo->mContext   = aExprDataContext;

        if( aExprDataContext->mContexts
            [ aFilterStack->mEntries[1].mExpr.mValueInfo.mOffset ].mCast == NULL )
        {
            switch( aFilterStack->mEntries[1].mExprType )
            {
                case KNL_EXPR_TYPE_VALUE :
                    STL_DASSERT( 0 );
                    break;
                        
                case KNL_EXPR_TYPE_FUNCTION :
                    sDataValue = & (aExprDataContext->mContexts
                                    [ aFilterStack->mEntries[1].mExpr.mFuncInfo.mOffset ]
                                    .mInfo.mFuncData->mResultValue );

                    STL_TRY( knlInitBlockWithDataValue(
                                 & sFilterEvalInfo->mResultBlock,
                                 sDataValue,
                                 sDataValue->mType,
                                 STL_LAYER_SQL_PROCESSOR,
                                 gResultDataTypeDef[ sDataValue->mType ].mArgNum1,
                                 gResultDataTypeDef[ sDataValue->mType ].mArgNum2,
                                 gResultDataTypeDef[ sDataValue->mType ].mStringLengthUnit,
                                 gResultDataTypeDef[ sDataValue->mType ].mIntervalIndicator,
                                 aRegionMem,
                                 KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                    break;
                    
                case KNL_EXPR_TYPE_LIST :
                    sDataValue = & (aExprDataContext->mContexts
                                    [ aFilterStack->mEntries[1].mExpr.mValueInfo.mOffset ]
                                    .mInfo.mListFunc->mResultValue );

                    STL_TRY( knlInitBlockWithDataValue(
                                 & sFilterEvalInfo->mResultBlock,
                                 sDataValue,
                                 sDataValue->mType,
                                 STL_LAYER_SQL_PROCESSOR,
                                 gResultDataTypeDef[ sDataValue->mType ].mArgNum1,
                                 gResultDataTypeDef[ sDataValue->mType ].mArgNum2,
                                 gResultDataTypeDef[ sDataValue->mType ].mStringLengthUnit,
                                 gResultDataTypeDef[ sDataValue->mType ].mIntervalIndicator,
                                 aRegionMem,
                                 KNL_ENV( aEnv ) )
                             == STL_SUCCESS );
                    break;

                default :
                    STL_DASSERT( 0 );
            }
        }
        else
        {
            sDataValue = & ( aExprDataContext->mContexts
                             [ aFilterStack->mEntries[1].mExpr.mValueInfo.mOffset ]
                             .mCast->mCastResultBuf );

            STL_TRY( knlInitBlockWithDataValue(
                         & sFilterEvalInfo->mResultBlock,
                         sDataValue,
                         sDataValue->mType,
                         STL_LAYER_SQL_PROCESSOR,
                         gResultDataTypeDef[ sDataValue->mType ].mArgNum1,
                         gResultDataTypeDef[ sDataValue->mType ].mArgNum2,
                         gResultDataTypeDef[ sDataValue->mType ].mStringLengthUnit,
                         gResultDataTypeDef[ sDataValue->mType ].mIntervalIndicator,
                         aRegionMem,
                         KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

        sFilterEvalInfo->mBlockIdx   = 0;
        sFilterEvalInfo->mBlockCount = 1;
    }
    else
    {
        sFilterEvalInfo = NULL;
    }


    /**
     * OUTPUT 설정
     */
    
    *aFilterEvalInfo = sFilterEvalInfo;
    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}
    

/**
 * @brief Pseudo Column Function List 를 위한 Pseudo Value Block List 를 할당 받는다.
 * @param[in]  aSQLStmt          SQL Statement 객체
 * @param[in]  aPseudoColList    Pseduo Column Function List
 * @param[in]  aBlockCnt         Block Count
 * @param[out] aPseudoBlockList  Pseudo Column Value Block List
 * @param[in]  aRegionMem        Region Memory
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus qlndAllocPseudoBlockList( qllStatement       * aSQLStmt,
                                    qlvRefExprList     * aPseudoColList,
                                    stlInt32             aBlockCnt,
                                    knlValueBlockList ** aPseudoBlockList,
                                    knlRegionMem       * aRegionMem,
                                    qllEnv             * aEnv )
{
    stlInt32  i = 0;
    
    qlvRefExprItem    * sPsColItem = NULL;
    qlvInitPseudoCol  * sPsColCode = NULL;
    knlPseudoCol        sPsColID   = KNL_PSEUDO_COL_INVALID;
    
    knlValueBlockList * sBlockList = NULL;
    knlValueBlockList * sCurrBlock = NULL;
    knlValueBlockList * sPrevBlock = NULL;


    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPseudoColList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPseudoBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Pseudo Column List 가 존재할 경우 Value Block List 를 할당
     */

    sPsColItem = aPseudoColList->mHead;
        
    for ( i = 0; i < aPseudoColList->mCount; i++ )
    {
        sPsColCode = sPsColItem->mExprPtr->mExpr.mPseudoCol;
        sPsColID   = sPsColCode->mPseudoId;


        /**
         * Pseudo Column 의 수행 시점의 유형에 따라 공간을 할당받는다.
         */
            
        switch ( gPseudoColInfoArr[ sPsColID ].mIterationTime )
        {
            case DTL_ITERATION_TIME_NONE:
            case DTL_ITERATION_TIME_FOR_EACH_STATEMENT:
                {
                    /**
                     * 모든 Value 가 공간을 공유한다.
                     */

                    /* Coverage : Pseudo Column은 현재 sequence 밖에 없다. */
                    STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                                & sCurrBlock,
                                                aBlockCnt,
                                                STL_FALSE, /* 공유 */
                                                STL_LAYER_SQL_PROCESSOR,
                                                KNL_ANONYMOUS_TABLE_ID, /* Table ID 지정이 의미가 없음 */
                                                0,
                                                gPseudoColInfoArr[ sPsColID ].mDataType,
                                                gPseudoColInfoArr[ sPsColID ].mArgNum1,
                                                gPseudoColInfoArr[ sPsColID ].mArgNum2,
                                                gPseudoColInfoArr[ sPsColID ].mStringLengthUnit,
                                                gPseudoColInfoArr[ sPsColID ].mIntervalIndicator,
                                                aRegionMem,
                                                aEnv )
                             == STL_SUCCESS );
                    break;
                }
            case DTL_ITERATION_TIME_FOR_EACH_EXPR:
                {
                    /**
                     * 모든 Value 가 각자의 공간을 갖는다.
                     */
                        
                    STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                                & sCurrBlock,
                                                aBlockCnt,
                                                STL_TRUE, /* 각자의 공간 */
                                                STL_LAYER_SQL_PROCESSOR,
                                                KNL_ANONYMOUS_TABLE_ID, /* Table ID 지정이 의미가 없음 */
                                                0,
                                                gPseudoColInfoArr[ sPsColID ].mDataType,
                                                gPseudoColInfoArr[ sPsColID ].mArgNum1,
                                                gPseudoColInfoArr[ sPsColID ].mArgNum2,
                                                gPseudoColInfoArr[ sPsColID ].mStringLengthUnit,
                                                gPseudoColInfoArr[ sPsColID ].mIntervalIndicator,
                                                aRegionMem,
                                                aEnv )
                             == STL_SUCCESS );
                    break;
                }
            default:
                {
                    STL_DASSERT(0);
                    break;
                }
        }

        /**
         * Block List 연결
         */

        if( sPrevBlock == NULL )
        {
            sBlockList = sCurrBlock;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevBlock, sCurrBlock );
        }

        sPrevBlock = sCurrBlock;
        sPsColItem = sPsColItem->mNext;
    }


    /**
     * Output 설정
     */

    *aPseudoBlockList = sBlockList;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief RowId Column List 를 위한 Value Block List 를 할당 받는다.
 * @param[in]      aSQLStmt               SQL Statement 객체
 * @param[in,out]  aRowIdColumnBlockList  Column Expression List
 * @param[in]      aRowIdColumnExprList   RowId Column Function List
 * @param[in]      aBlockCnt              Block Count
 * @param[in]      aRegionMem             Region Memory
 * @param[in]      aEnv                   Environment
 * @remarks
 */
stlStatus qlndAllocRowIdColumnBlockList( qllStatement       * aSQLStmt,
                                         qloColumnBlockList * aRowIdColumnBlockList,
                                         qlvRefExprList     * aRowIdColumnExprList,
                                         stlInt32             aBlockCnt,
                                         knlRegionMem       * aRegionMem,
                                         qllEnv             * aEnv )
{
    qlvRefExprItem        * sRowIdColExprItem = NULL;
    qlvInitBaseTableNode  * sCurBaseTableNode = NULL;

    qloColumnBlockItem    * sNewBlockItem   = NULL;
    qloColumnBlockItem    * sPrevBlockItem  = NULL;
        
    knlValueBlockList     * sNewValueBlock = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );     
    STL_PARAM_VALIDATE( aRowIdColumnBlockList != NULL, QLL_ERROR_STACK(aEnv) );    
    STL_PARAM_VALIDATE( aBlockCnt > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * RowId Column List 가 존재할 경우 Value Block List 를 할당
     * aRowIdColumnList에는
     * 각 테이블의 ROWID column list가 연결되어 있다.
     * t1.rowid -> t2.rowid -> t3.rowid ...
     */

    if( aRowIdColumnExprList == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sRowIdColExprItem  = aRowIdColumnExprList->mHead;

        while( sRowIdColExprItem != NULL )
        {
            sCurBaseTableNode =
                (qlvInitBaseTableNode *)(sRowIdColExprItem->mExprPtr->mExpr.mRowIdColumn->mRelationNode);

            STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                        & sNewValueBlock,
                                        aBlockCnt,
                                        STL_TRUE,  /* 각 레코드마다 공간이 필요함. */
                                        STL_LAYER_SQL_PROCESSOR,
                                        ellGetTableID( & sCurBaseTableNode->mTableHandle ),
                                        QLL_ROWID_COLUMN_IDX, 
                                        DTL_TYPE_ROWID,
                                        gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mArgNum1,
                                        gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mArgNum2,
                                        gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mStringLengthUnit,
                                        gDefaultDataTypeDef[ DTL_TYPE_ROWID ].mIntervalIndicator,
                                        aRegionMem,
                                        aEnv )
                     == STL_SUCCESS );

            /**
             * 새로운 Column Block Item 생성
             */

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( qloColumnBlockItem ),
                                        (void **) & sNewBlockItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
                
            sNewBlockItem->mRelationNode     = sRowIdColExprItem->mExprPtr->mExpr.mRowIdColumn->mRelationNode;
            sNewBlockItem->mColumnValueBlock = sNewValueBlock;
            sNewBlockItem->mNextRelation     = NULL;


            /**
             * Link 설정
             */
                
            if( sPrevBlockItem == NULL )
            {
                /* 새로운 Relation에 대한 Column Expression List 생성하여 list의 head로 설정 */
                aRowIdColumnBlockList->mHead = sNewBlockItem;
            }
            else
            {
                /* sPrevBlockItem 뒤에 새로운 Column Expression List 추가 */
                sPrevBlockItem->mNextRelation = sNewBlockItem;
            }

            sPrevBlockItem = sNewBlockItem;
            
            sRowIdColExprItem  = sRowIdColExprItem->mNext;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Read Table과 Write Table의 RowId Column Block을 공유한다.
 * @param[in,out]  aRowIdColumnBlockList     Column Expression List
 * @param[in]      aReadTableNode            Read Table Node
 * @param[out]     aWriteTableNode           Write Table Node
 * @param[in]      aEnv                      Environment
 * 
 * @remark Data Optimization 과정에서 해당 함수를 호출한다.
 */
stlStatus qlndShareRowIdColumnBlockListForUpdate( qloColumnBlockList * aRowIdColumnBlockList,
                                                  qlvInitNode        * aReadTableNode,
                                                  qlvInitNode        * aWriteTableNode,
                                                  qllEnv             * aEnv )
{
    qloColumnBlockItem     * sBlockItem         = NULL;
    knlValueBlockList      * sReadValueBlock    = NULL;
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aRowIdColumnBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReadTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aWriteTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    

    for( sBlockItem = aRowIdColumnBlockList->mHead;
         sBlockItem != NULL;
         sBlockItem = sBlockItem->mNextRelation )
    {
        if( sBlockItem->mRelationNode == aReadTableNode )
        {
            sReadValueBlock = sBlockItem->mColumnValueBlock;
            break;
        }
        else
        {
            /* Do Nothing */
        }        
    }

    if( sReadValueBlock != NULL )
    {
        for( sBlockItem = aRowIdColumnBlockList->mHead;
             sBlockItem != NULL;
             sBlockItem = sBlockItem->mNextRelation )
        {
            if( sBlockItem->mRelationNode == aWriteTableNode )
            {
                STL_DASSERT( sBlockItem->mColumnValueBlock != NULL );
                sBlockItem->mColumnValueBlock = sReadValueBlock;
                break;
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
    

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;   
}


/**
 * @brief Column Block List 를 생성한다.
 * @param[out]     aColumnBlockList  Column Block List
 * @param[in]      aRegionMem        Region Memory
 * @param[in]      aEnv              Environment
 */
stlStatus qlndCreateColumnBlockList( qloColumnBlockList  ** aColumnBlockList,
                                     knlRegionMem         * aRegionMem,
                                     qllEnv               * aEnv )
{
    qloColumnBlockList  * sColumnBlockList = NULL;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aColumnBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Column Expression List에 대한 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qloColumnBlockList ),
                                (void **) & sColumnBlockList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sColumnBlockList,
               0x00,
               STL_SIZEOF( qloColumnBlockList ) );
    

    /**
     * OUTPUT 설정
     */
    
    *aColumnBlockList = sColumnBlockList;

    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Column Block List에 Column Value Block을 추가한다.
 * @param[in]      aSQLStmt          SQL Statement 객체
 * @param[in]      aDataOptInfo      Data Optimization Info
 * @param[in,out]  aColumnBlockList  Column Expression List
 * @param[in]      aInitColumnExpr   Init Expression
 * @param[in]      aBlockAllocCnt    Block Allocation Count
 * @param[in]      aEnv              Environment
 *
 * @remark Data Optimization 과정에서 해당 함수를 호출한다.
 */
stlStatus qlndAddColumnToColumnBlockList( qllStatement        * aSQLStmt,
                                          qloDataOptExprInfo  * aDataOptInfo,
                                          qloColumnBlockList  * aColumnBlockList,
                                          qlvInitExpression   * aInitColumnExpr,
                                          stlInt32              aBlockAllocCnt,
                                          qllEnv              * aEnv )
{
    qlvInitNode         * sCurRelNode     = NULL;

    qloColumnBlockItem  * sNewBlockItem   = NULL;
    qloColumnBlockItem  * sCurBlockItem   = NULL;
    qloColumnBlockItem  * sPrevBlockItem  = NULL;

    knlValueBlockList   * sCurValueBlock  = NULL;
    knlValueBlockList   * sPrevValueBlock = NULL;
    knlValueBlockList   * sNewValueBlock  = NULL;

    dtlDataValue        * sUnusedValue = NULL;

    ellDictHandle       * sColumnHandle   = NULL;
    stlBool               sIsExistSame    = STL_FALSE;
    stlBool               sIsAddNewItem   = STL_FALSE;

    qloDBType           * sExprDBType     = NULL;
    
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitColumnExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBlockAllocCnt > 0, QLL_ERROR_STACK(aEnv) );


    /**
     * Column Expression List 구성
     */

    if( aInitColumnExpr->mType == QLV_EXPR_TYPE_COLUMN )
    {
        sCurRelNode = aInitColumnExpr->mExpr.mColumn->mRelationNode;
    }
    else
    {
        STL_DASSERT( aInitColumnExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

        sCurRelNode = aInitColumnExpr->mExpr.mInnerColumn->mRelationNode;
    }
    
    sCurBlockItem  = aColumnBlockList->mHead;
    sPrevBlockItem = NULL;
    sIsExistSame   = STL_FALSE;

            
    /**
     * Column Expression을 추가할 Relation 찾기
     */

    STL_DASSERT( sCurRelNode != NULL );

    if( sCurBlockItem == NULL )
    {
        sIsAddNewItem = STL_TRUE;
    }
    else
    {
        sIsAddNewItem = STL_TRUE;
        while( sCurBlockItem != NULL )
        {
            if( sCurRelNode <= sCurBlockItem->mRelationNode )
            {
                sIsAddNewItem = ( sCurRelNode != sCurBlockItem->mRelationNode );
                break;
            }
            else
            {
                sPrevBlockItem = sCurBlockItem;
                sCurBlockItem  = sCurBlockItem->mNextRelation;
            }
        }
    }
    
    
    /**
     * Column Expression List 구성
     */
            
    if( sIsAddNewItem == STL_FALSE )
    {
        /* Column Expression과 동일한 Relation을 갖는 Expression List가 존재 */

        /**
         * Insertion 위치 찾기
         */
                
        sCurValueBlock  = sCurBlockItem->mColumnValueBlock;
        sPrevValueBlock = NULL;

        STL_DASSERT( sCurValueBlock != NULL );

        if( sCurRelNode->mType == QLV_NODE_TYPE_BASE_TABLE )
        {
            /**
             * Base-Table은 Column handle을 이용하여 Column Order를 비교
             */
            
            sColumnHandle = aInitColumnExpr->mExpr.mColumn->mColumnHandle;
            
            while( sCurValueBlock != NULL )
            {
                if( ellGetColumnIdx( sColumnHandle ) <=
                    KNL_GET_BLOCK_COLUMN_ORDER( sCurValueBlock ) )
                {
                    sIsExistSame = ( ellGetColumnIdx( sColumnHandle ) ==
                                     KNL_GET_BLOCK_COLUMN_ORDER( sCurValueBlock ) );
                    break;
                }
                else
                {
                    sPrevValueBlock = sCurValueBlock;
                    sCurValueBlock  = sCurValueBlock->mNext;
                }
            }
            
            
            /**
             * 중복된 Expression은 List에 추가하지 않음
             */
            
            if( sIsExistSame == STL_TRUE )
            {
                /* Do Nothing */
            }
            else
            {
                /**
                 * Value Block 추가
                 */

                if ( ellGetColumnUnused( sColumnHandle ) == STL_TRUE )
                {
                    /**
                     * UNUSED Column 인 경우
                     * - INSERT 구문의 경우 NULL 값을 채워줘야 함.
                     */
                    
                    STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                                & sNewValueBlock,
                                                aBlockAllocCnt,
                                                STL_FALSE, /* aIsSepBuff */
                                                STL_LAYER_SQL_PROCESSOR,
                                                ellGetColumnTableID( sColumnHandle ),
                                                ellGetColumnIdx( sColumnHandle ),
                                                ellGetColumnDBType( sColumnHandle ),
                                                ellGetColumnPrecision( sColumnHandle ),
                                                ellGetColumnScale( sColumnHandle ),
                                                ellGetColumnStringLengthUnit( sColumnHandle ),
                                                ellGetColumnIntervalIndicator( sColumnHandle ),
                                                aDataOptInfo->mRegionMem,
                                                aEnv )
                             == STL_SUCCESS );

                    sUnusedValue = KNL_GET_BLOCK_DATA_VALUE( sNewValueBlock, 0 );
                    DTL_SET_NULL( sUnusedValue );
                }
                else
                {
                    STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                                & sNewValueBlock,
                                                aBlockAllocCnt,
                                                STL_TRUE, /* aIsSepBuff */
                                                STL_LAYER_SQL_PROCESSOR,
                                                ellGetColumnTableID( sColumnHandle ),
                                                ellGetColumnIdx( sColumnHandle ),
                                                ellGetColumnDBType( sColumnHandle ),
                                                ellGetColumnPrecision( sColumnHandle ),
                                                ellGetColumnScale( sColumnHandle ),
                                                ellGetColumnStringLengthUnit( sColumnHandle ),
                                                ellGetColumnIntervalIndicator( sColumnHandle ),
                                                aDataOptInfo->mRegionMem,
                                                aEnv )
                             == STL_SUCCESS );
                }
            
                
                /**
                 * Block List 연결
                 */

                if( sPrevValueBlock == NULL )
                {
                    /* list의 head에 value block 추가 */
                    KNL_LINK_BLOCK_LIST( sNewValueBlock, sCurValueBlock );
                    sCurBlockItem->mColumnValueBlock = sNewValueBlock;
                }
                else
                {
                    /* sPrevValueBlock의 mNext에 value block 추가 */
                    KNL_LINK_BLOCK_LIST( sNewValueBlock, sPrevValueBlock->mNext );
                    KNL_LINK_BLOCK_LIST( sPrevValueBlock, sNewValueBlock );
                }
            }
        }
        else
        {
            /**
             * Inner Column의 Idx를 이용하여 Column Order를 비교
             */
            
            while( sCurValueBlock != NULL )
            {
                if( *(aInitColumnExpr->mExpr.mInnerColumn->mIdx) <=
                    KNL_GET_BLOCK_COLUMN_ORDER( sCurValueBlock ) )
                {
                    sIsExistSame = ( *(aInitColumnExpr->mExpr.mInnerColumn->mIdx) == 
                                     KNL_GET_BLOCK_COLUMN_ORDER( sCurValueBlock ) );
                    break;
                }
                else
                {
                    sPrevValueBlock = sCurValueBlock;
                    sCurValueBlock  = sCurValueBlock->mNext;
                }
            }
            

            /**
             * 중복된 Expression은 List에 추가하지 않음
             */
            
            if( sIsExistSame == STL_TRUE )
            {
                /* Do Nothing */
            }
            else
            {
                if( ( sCurRelNode->mType == QLV_NODE_TYPE_JOIN_TABLE ) ||
                    ( sCurRelNode->mType == QLV_NODE_TYPE_QUERY_SET ) )
                {
                    sExprDBType = QLO_GET_DB_TYPE( aDataOptInfo, aInitColumnExpr->mOffset );
            
                    STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                                & sNewValueBlock,
                                                aBlockAllocCnt,
                                                STL_TRUE, /* 컬럼임 */
                                                STL_LAYER_SQL_PROCESSOR,
                                                KNL_ANONYMOUS_TABLE_ID,
                                                *aInitColumnExpr->mExpr.mInnerColumn->mIdx,
                                                sExprDBType->mDBType,
                                                sExprDBType->mArgNum1,
                                                sExprDBType->mArgNum2,
                                                sExprDBType->mStringLengthUnit,
                                                sExprDBType->mIntervalIndicator,
                                                aDataOptInfo->mRegionMem,
                                                aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    STL_TRY( qloDataOptimizeExpression( aDataOptInfo,
                                                        aInitColumnExpr,
                                                        aEnv )
                             == STL_SUCCESS );

                    STL_TRY( knlInitShareBlockFromBlock(
                                 & sNewValueBlock,
                                 QLO_GET_CONTEXT( aDataOptInfo, aInitColumnExpr->mOffset )->mInfo.mValueInfo,
                                 aDataOptInfo->mRegionMem,
                                 KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    sNewValueBlock->mTableID     = KNL_ANONYMOUS_TABLE_ID;
                    sNewValueBlock->mColumnOrder = *aInitColumnExpr->mExpr.mInnerColumn->mIdx;
                }


                /**
                 * Block List 연결
                 */

                if( sPrevValueBlock == NULL )
                {
                    /* list의 head에 value block 추가 */
                    KNL_LINK_BLOCK_LIST( sNewValueBlock, sCurValueBlock );
                    sCurBlockItem->mColumnValueBlock = sNewValueBlock;
                }
                else
                {
                    /* sPrevValueBlock의 mNext에 value block 추가 */
                    KNL_LINK_BLOCK_LIST( sNewValueBlock, sPrevValueBlock->mNext );
                    KNL_LINK_BLOCK_LIST( sPrevValueBlock, sNewValueBlock );
                }
            }
        }
    }
    else
    {
        /* Column Expression과 동일한 Relation을 갖는 Expression List가 존재하지 않음 */

        /**
         * 새로운 Column Value Block List에 생성
         */
        
        if( sCurRelNode->mType == QLV_NODE_TYPE_BASE_TABLE )
        {
            sColumnHandle = aInitColumnExpr->mExpr.mColumn->mColumnHandle;
            
            if ( ellGetColumnUnused( sColumnHandle ) == STL_TRUE )
            {
                /**
                 * UNUSED Column 인 경우
                 * - INSERT 구문의 경우 NULL 값을 채워줘야 함.
                 */
                
                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & sNewValueBlock,
                                            aBlockAllocCnt,
                                            STL_FALSE, /* aIsSepBuff */
                                            STL_LAYER_SQL_PROCESSOR,
                                            ellGetColumnTableID( sColumnHandle ),
                                            ellGetColumnIdx( sColumnHandle ),
                                            ellGetColumnDBType( sColumnHandle ),
                                            ellGetColumnPrecision( sColumnHandle ),
                                            ellGetColumnScale( sColumnHandle ),
                                            ellGetColumnStringLengthUnit( sColumnHandle ),
                                            ellGetColumnIntervalIndicator( sColumnHandle ),
                                            aDataOptInfo->mRegionMem,
                                            aEnv )
                         == STL_SUCCESS );
                
                sUnusedValue = KNL_GET_BLOCK_DATA_VALUE( sNewValueBlock, 0 );
                DTL_SET_NULL( sUnusedValue );
            }
            else
            {
                STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                            & sNewValueBlock,
                                            aBlockAllocCnt,
                                            STL_TRUE, /* 컬럼임 */
                                            STL_LAYER_SQL_PROCESSOR,
                                            ellGetColumnTableID( sColumnHandle ),
                                            ellGetColumnIdx( sColumnHandle ),
                                            ellGetColumnDBType( sColumnHandle ),
                                            ellGetColumnPrecision( sColumnHandle ),
                                            ellGetColumnScale( sColumnHandle ),
                                            ellGetColumnStringLengthUnit( sColumnHandle ),
                                            ellGetColumnIntervalIndicator( sColumnHandle ),
                                            aDataOptInfo->mRegionMem,
                                            aEnv )
                         == STL_SUCCESS );
            }
        }
        else if( ( sCurRelNode->mType == QLV_NODE_TYPE_JOIN_TABLE ) ||
                 ( sCurRelNode->mType == QLV_NODE_TYPE_QUERY_SET ) )
        {
            sExprDBType = QLO_GET_DB_TYPE( aDataOptInfo, aInitColumnExpr->mOffset );
            
            STL_TRY( qlndInitBlockList( & aSQLStmt->mLongTypeValueList,
                                        & sNewValueBlock,
                                        aBlockAllocCnt,
                                        STL_TRUE, /* 컬럼임 */
                                        STL_LAYER_SQL_PROCESSOR,
                                        KNL_ANONYMOUS_TABLE_ID,
                                        *aInitColumnExpr->mExpr.mInnerColumn->mIdx,
                                        sExprDBType->mDBType,
                                        sExprDBType->mArgNum1,
                                        sExprDBType->mArgNum2,
                                        sExprDBType->mStringLengthUnit,
                                        sExprDBType->mIntervalIndicator,
                                        aDataOptInfo->mRegionMem,
                                        aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            STL_TRY( qloDataOptimizeExpression( aDataOptInfo,
                                                aInitColumnExpr,
                                                aEnv )
                     == STL_SUCCESS );
            
            STL_TRY( knlInitShareBlockFromBlock(
                         & sNewValueBlock,
                         QLO_GET_CONTEXT( aDataOptInfo, aInitColumnExpr->mOffset )->mInfo.mValueInfo,
                         aDataOptInfo->mRegionMem,
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sNewValueBlock->mTableID     = KNL_ANONYMOUS_TABLE_ID;
            sNewValueBlock->mColumnOrder = *aInitColumnExpr->mExpr.mInnerColumn->mIdx;
        }


        /**
         * 새로운 Column Block Item 생성
         */

        STL_TRY( knlAllocRegionMem( aDataOptInfo->mRegionMem,
                                    STL_SIZEOF( qloColumnBlockItem ),
                                    (void **) & sNewBlockItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
                
        sNewBlockItem->mRelationNode     = sCurRelNode;
        sNewBlockItem->mColumnValueBlock = sNewValueBlock;
        sNewBlockItem->mNextRelation     = NULL;

        
        /**
         * Link 설정
         */
                
        if( sPrevBlockItem == NULL )
        {
            /* 새로운 Relation에 대한 Column Expression List 생성하여 list의 head로 설정 */
            sNewBlockItem->mNextRelation = sCurBlockItem;
            aColumnBlockList->mHead = sNewBlockItem;
        }
        else
        {
            /* sPrevBlockItem 뒤에 새로운 Column Expression List 추가 */
            sNewBlockItem->mNextRelation = sPrevBlockItem->mNextRelation;
            sPrevBlockItem->mNextRelation = sNewBlockItem;
        }
    }
    

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Column Block List에 Column Expression List에 대한 Value Block들을 추가한다.
 * @param[in]      aSQLStmt             SQL Statement 객체
 * @param[in]      aDataOptInfo         Data Optimization Info
 * @param[in,out]  aColumnBlockList     Column Expression List
 * @param[in]      aInitColumnExprList  Init Expression List
 * @param[in]      aBlockAllocCnt       Block Allocation Count
 * @param[in]      aEnv                 Environment
 *
 * @remark Data Optimization 과정에서 해당 함수를 호출한다.
 */
stlStatus qlndAddColumnListToColumnBlockList( qllStatement        * aSQLStmt,
                                              qloDataOptExprInfo  * aDataOptInfo,
                                              qloColumnBlockList  * aColumnBlockList,
                                              qlvRefExprList      * aInitColumnExprList,
                                              stlInt32              aBlockAllocCnt,
                                              qllEnv              * aEnv )
{
    qlvRefExprItem  * sRefExprItem = NULL;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataOptInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBlockAllocCnt > 0, QLL_ERROR_STACK(aEnv) );


    /**
     * Column Block 구성
     */

    if( aInitColumnExprList == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        sRefExprItem = aInitColumnExprList->mHead;

        while( sRefExprItem != NULL )
        {
            STL_TRY( qlndAddColumnToColumnBlockList( aSQLStmt,
                                                     aDataOptInfo,
                                                     aColumnBlockList,
                                                     sRefExprItem->mExprPtr,
                                                     aBlockAllocCnt,
                                                     aEnv )
                     == STL_SUCCESS );

            sRefExprItem = sRefExprItem->mNext;
        }
    }

    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Column Block List에서 Column Expression 에 대한 Value Block들을 찾는다.
 * @param[in]      aDataArea            Data Area
 * @param[in]      aColumnBlockList     Column Expression List
 * @param[in]      aInitExpr            Init Expression
 * @param[in]      aRegionMem           Region Memory
 * @param[out]     aValueBlock          Column Value Block
 * @param[in]      aEnv                 Environment
 * 
 * @remark Data Optimization 과정에서 해당 함수를 호출한다.
 */
stlStatus qlndFindColumnBlockList( qllDataArea          * aDataArea,
                                   qloColumnBlockList   * aColumnBlockList,
                                   qlvInitExpression    * aInitExpr,
                                   knlRegionMem         * aRegionMem,
                                   knlValueBlockList   ** aValueBlock,
                                   qllEnv               * aEnv )
{
    qlvInitColumn       * sColumn       = NULL;
    qlvInitInnerColumn  * sInnerColumn  = NULL;
    knlValueBlockList   * sValueBlock   = NULL;
    qloColumnBlockItem  * sBlockItem    = NULL;
    qlvInitNode         * sCurRelNode   = NULL;
    stlInt32              sColIdx       = 0;

    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( ( (aInitExpr->mType == QLV_EXPR_TYPE_COLUMN) ||
                          (aInitExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN) ),
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueBlock != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * 찾을 Column Order 설정
     */

    if( aInitExpr->mType == QLV_EXPR_TYPE_COLUMN )
    {
        sColumn     = aInitExpr->mExpr.mColumn;
        sCurRelNode = sColumn->mRelationNode;

        STL_DASSERT( sCurRelNode != NULL );
        STL_DASSERT( sCurRelNode->mType == QLV_NODE_TYPE_BASE_TABLE );

        sColIdx = ellGetColumnIdx( sColumn->mColumnHandle );
    }
    else
    {
        STL_DASSERT( aInitExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );

        sInnerColumn = aInitExpr->mExpr.mInnerColumn;
        sCurRelNode  = sInnerColumn->mRelationNode;

        STL_DASSERT( sCurRelNode != NULL );

        sColIdx = *sInnerColumn->mIdx;
    }


    /**
     * Column Block 찾기
     */
    for ( sBlockItem = aColumnBlockList->mHead;
          sBlockItem != NULL;
          sBlockItem = sBlockItem->mNextRelation )
    {
        if( sBlockItem->mRelationNode == sCurRelNode )
        {
            sValueBlock = sBlockItem->mColumnValueBlock;

            while( sValueBlock != NULL )
            {
                if( KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock ) == sColIdx )
                {
                    break;
                }
                else
                {
                    sValueBlock = sValueBlock->mNext;
                }
            }
            break;
        }
        else
        {
            /* Do Nothing */
        }
    }


    /**
     * OUTPUT 설정
     */

    *aValueBlock = sValueBlock;


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief RowId Column Block List에서 RowId Expression 에 대한 Value Block들을 찾는다.
 * @param[in]      aRowIdColumnBlockList     Column Expression List
 * @param[in]      aInitExpr                 Init Expression
 * @param[out]     aValueBlock               RowId Column Value Block
 * @param[in]      aEnv                      Environment
 * 
 * @remark Data Optimization 과정에서 해당 함수를 호출한다.
 */
stlStatus qlndFindRowIdColumnBlockList( qloColumnBlockList   * aRowIdColumnBlockList,
                                        qlvInitExpression    * aInitExpr,
                                        knlValueBlockList   ** aValueBlock,
                                        qllEnv               * aEnv )
{
    qlvInitNode            * sCurRelNode        = NULL;
    qloColumnBlockItem     * sBlockItem         = NULL;
    knlValueBlockList      * sValueBlock        = NULL;
    
    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aRowIdColumnBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueBlock != NULL, QLL_ERROR_STACK(aEnv) );

    sCurRelNode = aInitExpr->mExpr.mRowIdColumn->mRelationNode;

    STL_DASSERT( sCurRelNode != NULL );    

    for( sBlockItem = aRowIdColumnBlockList->mHead;
         sBlockItem != NULL;
         sBlockItem = sBlockItem->mNextRelation )
    {
        if( sBlockItem->mRelationNode == sCurRelNode )
        {
            sValueBlock = sBlockItem->mColumnValueBlock;
            break;
        }
        else
        {
            /* Do Nothing */
        }        
    }

    /**
     * Output 설정
     */

    *aValueBlock = sValueBlock;

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;   
}


/**
 * @brief Execution을 위한 Physical Filter 정보 구축 
 * @param[in]     aRelationNode    Source Relation Node
 * @param[in]     aFilterExpr      Filter Expression 
 * @param[in]     aReadValueBlocks Read Value Block List
 * @param[in]     aExprDataContext Context Info
 * @param[in]     aRegionMem       Region Memory
 * @param[out]    aPhysicalFilter  Physical Filter
 * @param[in]     aEnv             Environment
 */
stlStatus qlndMakePhysicalFilter( qlvInitNode          * aRelationNode,
                                  qlvInitExpression    * aFilterExpr,
                                  knlValueBlockList    * aReadValueBlocks,
                                  knlExprContextInfo   * aExprDataContext,
                                  knlRegionMem         * aRegionMem,
                                  knlPhysicalFilter   ** aPhysicalFilter,
                                  qllEnv               * aEnv )
{
    qlvInitExpression     * sFilterExpr;
    knlPhysicalFilter     * sPhysicalFilter = NULL;
    knlPhysicalFilter     * sCurFilter      = NULL;
    knlValueBlockList     * sValueBlock;
    qlvInitFunction       * sFuncCode;
    qlvInitListFunc       * sListFunc       = NULL;
    qlvInitListCol        * sListColLeft    = NULL;
    qlvInitRowExpr        * sRowExpr1       = NULL;
    qlvInitListCol        * sListColRight   = NULL;
//    qlvInitRowExpr        * sRowExpr2       = NULL;
    
    knlColumnInReadRow    * sColumnInReadRow;
    
    dtlDataValue          * sDataValue;
    
    stlInt32                sCount = 0;
    stlInt32                sLoop1 = 0;
    stlInt32                sLoop2 = 0;

    knlPhysicalFilterPtr  * sSortList = NULL;
    knlPhysicalFilter     * sSortTemp;

    stlInt32                sExprListCnt;
    qlvInitExpression    ** sExprListWithoutAND;    

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aRelationNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFilterExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReadValueBlocks != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExprDataContext != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPhysicalFilter != NULL, QLL_ERROR_STACK(aEnv) );
    
#define GET_VALUE_BLOCK_WITH_COLUMN_ORDER( block, order )       \
    (block) = aReadValueBlocks;                                 \
    while( (block) != NULL )                                    \
    {                                                           \
        if( KNL_GET_BLOCK_COLUMN_ORDER( (block) ) == (order) )  \
        {                                                       \
            break;                                              \
        }                                                       \
        (block) = (block)->mNext;                               \
    }

    
    /**
     * Filter 개수 세기
     */
    
    sCount = aFilterExpr->mIncludeExprCnt;

    
    /**
     * Filter 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlPhysicalFilter ) * sCount,
                                (void **) & sPhysicalFilter,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * 재귀호출로 AND function 제거한 Function 들만 모으기
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression * ) * sCount,
                                (void **) & sExprListWithoutAND,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    sExprListCnt = 0;
    
    qloRemoveExprAND( aFilterExpr,
                      sExprListWithoutAND,
                      & sExprListCnt );      
    

    /**
     * Physical Filter List 생성
     */
    
    sCurFilter = sPhysicalFilter;

    switch( aRelationNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE :
            {
                break;
            }
        case QLV_NODE_TYPE_JOIN_TABLE :
        case QLV_NODE_TYPE_FLAT_INSTANT :
        case QLV_NODE_TYPE_SORT_INSTANT :
        case QLV_NODE_TYPE_HASH_INSTANT :
        case QLV_NODE_TYPE_GROUP_HASH_INSTANT :
        case QLV_NODE_TYPE_GROUP_SORT_INSTANT :
        case QLV_NODE_TYPE_SORT_JOIN_INSTANT :
        case QLV_NODE_TYPE_HASH_JOIN_INSTANT :
            {
                STL_THROW( RAMP_CHECK_INNER_COLUMN );
            }
        /* case QLV_NODE_TYPE_SUB_TABLE :  */
        /* case QLV_NODE_TYPE_QUERY_SET : */
        /* case QLV_NODE_TYPE_QUERY_SPEC : */
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    
    /**
     * Column 관련 Physical Filter 분류
     */
        
    for( sLoop1 = 0 ; sLoop1 < sExprListCnt ; sLoop1++ )
    {
        /**
         * Filter 구성
         */
        sFilterExpr = sExprListWithoutAND[ sLoop1 ];
        
        /* Column Order 기준으로 Filter entry 정렬 */
        if( sFilterExpr->mType == QLV_EXPR_TYPE_FUNCTION )
        {
            /* function 으로 구성된 filter */ 
            sFuncCode = sFilterExpr->mExpr.mFunction;
            
            if( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN )
            {
                STL_DASSERT( sFuncCode->mArgs[0]->mExpr.mColumn->mRelationNode == aRelationNode );

                if( ( sFuncCode->mArgCount == 2 ) &&
                    ( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_COLUMN ) )
                {
                    STL_DASSERT( sFuncCode->mArgs[1]->mExpr.mColumn->mRelationNode == aRelationNode );
                    
                    /**
                     * 두 개의 column으로 구성된 filter
                     */
                    if( ellGetColumnIdx( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ) >=
                        ellGetColumnIdx( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle ) )
                    {
                        /**
                         * column vs column ( idx1 >= idx2 )
                         */
                        
                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            ellGetColumnIdx( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ) );

                        sCurFilter->mColIdx1 = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            ellGetColumnIdx( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle ) );

                        sCurFilter->mColIdx2  = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
                        sCurFilter->mColVal2  = & sValueBlock->mColumnInRow;
                        sCurFilter->mConstVal = NULL;

                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               ellGetColumnDBType( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ),
                                               ellGetColumnDBType( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle ),
                                               & sCurFilter->mFunc,
                                               STL_FALSE );
                    }
                    else
                    {
                        /**
                         * column vs column ( idx1 < idx2 ) : Operand의 위치 바꾸기 => reverse function 적용
                         */

                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            ellGetColumnIdx( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle ) );
                        
                        sCurFilter->mColIdx1 = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            ellGetColumnIdx( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ) );

                        sCurFilter->mColIdx2  = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
                        sCurFilter->mColVal2  = & sValueBlock->mColumnInRow;
                        sCurFilter->mConstVal = NULL;

                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               ellGetColumnDBType( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ),
                                               ellGetColumnDBType( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle ),
                                               & sCurFilter->mFunc,
                                               STL_TRUE );
                    }
                }
                else
                {
                    if( sFuncCode->mArgCount == 1 )
                    {
                        /**
                         * column 하나만으로 구성된 filter
                         */ 
            
                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            ellGetColumnIdx( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ) );
            
                        sCurFilter->mColIdx1 = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* emtpy value 설정 */
                        sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT;
                        sCurFilter->mColVal2  = knlEmtpyColumnValue;
                        sCurFilter->mConstVal = NULL;
                        
                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               ellGetColumnDBType( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ),
                                               0,
                                               & sCurFilter->mFunc,
                                               STL_FALSE );
                    }
                    else
                    {
                        /**
                         * column vs constant value
                         */
                        
                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            ellGetColumnIdx( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ) );

                        sCurFilter->mColIdx1 = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* constant value 설정 */
                        if( aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].mCast != NULL )
                        {
                            sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].mCast->mCastResultBuf;
                        }
                        else
                        {
                            if( (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                                (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_VALUE) ||
                                (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                                (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_OUTER_COLUMN) )
                            {
                                sDataValue = aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].
                                    mInfo.mValueInfo->mDataValue;
                            }
                            else
                            {
                                if( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
                                {
                                    sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].
                                        mInfo.mListFunc->mResultValue;
                                }
                                else
                                {
                                    sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].
                                        mInfo.mFuncData->mResultValue;
                                }
                            }
                        }

                        STL_TRY( knlAllocRegionMem( aRegionMem,
                                                    STL_SIZEOF( knlColumnInReadRow ),
                                                    (void **) & sColumnInReadRow,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        sColumnInReadRow->mValue  = (void *) sDataValue->mValue;
                        sColumnInReadRow->mLength = sDataValue->mLength;
                                
                        sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT;
                        sCurFilter->mColVal2  = sColumnInReadRow;
                        sCurFilter->mConstVal = sDataValue;
                        
                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               ellGetColumnDBType( sFuncCode->mArgs[0]->mExpr.mColumn->mColumnHandle ),
                                               sDataValue->mType,
                                               & sCurFilter->mFunc,
                                               STL_FALSE );
                    }
                }
            }
            else
            {
                /**
                 * column vs constant value : reverse function
                 */
                
                STL_DASSERT( sFuncCode->mArgCount == 2 );         /* constant value만으로 구성된 physical filter는 없다. */
                STL_DASSERT( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_COLUMN );
                STL_DASSERT( sFuncCode->mArgs[1]->mExpr.mColumn->mRelationNode == aRelationNode );
                
                /* column value 설정 */
                GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                    sValueBlock,
                    ellGetColumnIdx( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle ) );

                sCurFilter->mColIdx1 = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
                sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                /* constant value 설정 */
                if( aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].mCast != NULL )
                {
                    sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].mCast->mCastResultBuf;
                }
                else
                {
                    if( (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                        (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_VALUE) ||
                        (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                        (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_OUTER_COLUMN) )
                    {
                        sDataValue = aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].
                            mInfo.mValueInfo->mDataValue;
                    }
                    else
                    {
                        if( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
                        {
                            sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].
                                mInfo.mListFunc->mResultValue;
                        }
                        else
                        {
                            sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].
                                mInfo.mFuncData->mResultValue;
                        }
                    }
                }

                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            STL_SIZEOF( knlColumnInReadRow ),
                                            (void **) & sColumnInReadRow,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sColumnInReadRow->mValue  = (void *) sDataValue->mValue;
                sColumnInReadRow->mLength = sDataValue->mLength;
                                
                sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT;
                sCurFilter->mColVal2  = sColumnInReadRow;
                sCurFilter->mConstVal = sDataValue;

                /* function pointer 설정 */
                qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                       sDataValue->mType,
                                       ellGetColumnDBType( sFuncCode->mArgs[1]->mExpr.mColumn->mColumnHandle ),
                                       & sCurFilter->mFunc,
                                       STL_TRUE );
            }
        }
        else if( sFilterExpr->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
        {
            /* List Function으로 구성된 filter */
            sListFunc = sFilterExpr->mExpr.mListFunc;
            STL_DASSERT( sListFunc->mArgs[1]->mType == QLV_EXPR_TYPE_LIST_COLUMN );

            sListColLeft = sListFunc->mArgs[1]->mExpr.mListCol;
            STL_DASSERT( sListColLeft->mArgs[0]->mType == QLV_EXPR_TYPE_ROW_EXPR );

            sRowExpr1 = sListColLeft->mArgs[0]->mExpr.mRowExpr;
           
            STL_DASSERT( sRowExpr1->mArgs[0]->mType == QLV_EXPR_TYPE_COLUMN );
            STL_DASSERT( sRowExpr1->mArgs[0]->mExpr.mColumn->mRelationNode == aRelationNode );

            /* column value 설정 */
            GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                sValueBlock,
                ellGetColumnIdx( sRowExpr1->mArgs[0]->mExpr.mColumn->mColumnHandle ) );

            sCurFilter->mColIdx1 = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
            sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

            STL_DASSERT( sListFunc->mArgs[0]->mType == QLV_EXPR_TYPE_LIST_COLUMN );

            sListColRight = sListFunc->mArgs[0]->mExpr.mListCol;
            STL_DASSERT( sListColRight->mArgs[0]->mType == QLV_EXPR_TYPE_ROW_EXPR );

//            sRowExpr2 = sListColRight->mArgs[0]->mExpr.mRowExpr;

            /* Constant value 설정 */

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( knlColumnInReadRow ),
                                        (void **) & sColumnInReadRow,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( dtlDataValue ) * ( sListColRight->mArgCount + 1 ),
                                        (void **) & sDataValue,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            
            /**
             * @todo IN Physical Function Pointer
             * aIsInPhysical = STL_TRUE인 경우, 기존의 Physical Function 과 달리
             * Left와 Right의 void형 포인트로 dtlDataValue 가 list 로 오게된다.
             * Function pointer를 통해 dtlDataValue를 physical function안에서 하나씩 처리하도록 함.
             */

            /* RightVal의 마지막 DataValue는 Left의 Type만 갖고 있다 */
            sDataValue[sListColRight->mArgCount].mType
                = ellGetColumnDBType( sRowExpr1->mArgs[0]->mExpr.mColumn->mColumnHandle );
            
            for( sLoop2 = 0 ; sLoop2 < sListColRight->mArgCount ; sLoop2 ++ )
            {
                if( aExprDataContext->mContexts
                    [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mCast != NULL )
                {
                    sDataValue[sLoop2].mValue = & aExprDataContext->mContexts
                        [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mCast->mCastResultBuf;
                }
                else
                {
                    if( (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                        (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_VALUE) ||
                        (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                        (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_OUTER_COLUMN) )
                    {
                        sDataValue[sLoop2].mValue = aExprDataContext->mContexts
                            [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mInfo.mValueInfo->mDataValue;
                    }
                    else
                    {
                        if( sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType ==
                            QLV_EXPR_TYPE_LIST_FUNCTION )
                        {
                            sDataValue[sLoop2].mValue = & aExprDataContext->mContexts
                                [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mInfo.mListFunc->mResultValue;
                        }
                        else
                        {
                            sDataValue[sLoop2].mValue = & aExprDataContext->mContexts
                                [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mInfo.mFuncData->mResultValue;
                        }
                    }
                }
            }
            sColumnInReadRow->mValue  = sDataValue;
            sColumnInReadRow->mLength = sListColRight->mArgCount;
                    
            sCurFilter->mColIdx2  = KNL_PHYSICAL_LIST_VALUE_CONSTANT;
            sCurFilter->mColVal2  = sColumnInReadRow;
            sCurFilter->mConstVal = sDataValue;
            
            sCurFilter->mFunc = dtlInPhysicalFuncPointer[KNL_IN_PHYSICAL_FUNC_ID_PTR( sListFunc->mFuncId )];
        }
        else
        {
            /* column 하나만으로 구성된 filter */
            STL_DASSERT( sFilterExpr->mType == QLV_EXPR_TYPE_COLUMN );
            STL_DASSERT( sFilterExpr->mExpr.mColumn->mRelationNode == aRelationNode );
                
            /* function pointer 설정 */
            sCurFilter->mFunc = dtlPhysicalFuncIsTrue;
            
            /* column value 설정 */
            GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                sValueBlock,
                ellGetColumnIdx( sFilterExpr->mExpr.mColumn->mColumnHandle ) );
            
            sCurFilter->mColIdx1 = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
            sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

            /* emtpy value 설정 */
            sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT;
            sCurFilter->mColVal2  = knlEmtpyColumnValue;
            sCurFilter->mConstVal = NULL;
        }

        STL_DASSERT( sCurFilter->mFunc != NULL );

        sCurFilter++;
    }

    STL_THROW( RAMP_SKIP_INNER_COLUMN );
    

    /**
     * Inner Column 관련 Physical Filter 분류
     */

    STL_RAMP( RAMP_CHECK_INNER_COLUMN );

    for( sLoop1 = 0 ; sLoop1 < sExprListCnt ; sLoop1++ )
    {
        /**
         * Filter 구성
         */
        sFilterExpr = sExprListWithoutAND[ sLoop1 ];
        
        /* Column Order 기준으로 Filter entry 정렬 */
        if( sFilterExpr->mType == QLV_EXPR_TYPE_FUNCTION )
        {
            /* function 으로 구성된 filter */ 
            sFuncCode = sFilterExpr->mExpr.mFunction;
            
            if( ( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) &&
                ( sFuncCode->mArgs[0]->mExpr.mInnerColumn->mRelationNode == aRelationNode ) )
            {
                if( ( sFuncCode->mArgCount == 2 ) &&
                    ( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_INNER_COLUMN ) &&
                    ( sFuncCode->mArgs[1]->mExpr.mInnerColumn->mRelationNode == aRelationNode ) )
                {
                    /**
                     * 두 개의 column으로 구성된 filter
                     */
                    if( *sFuncCode->mArgs[0]->mExpr.mInnerColumn->mIdx >=
                        *sFuncCode->mArgs[1]->mExpr.mInnerColumn->mIdx )
                    {
                        /**
                         * column vs column ( idx1 >= idx2 )
                         */

                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            *sFuncCode->mArgs[0]->mExpr.mInnerColumn->mIdx );

                        sCurFilter->mColIdx1 = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            *sFuncCode->mArgs[1]->mExpr.mInnerColumn->mIdx );

                        sCurFilter->mColIdx2  = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
                        sCurFilter->mColVal2  = & sValueBlock->mColumnInRow;
                        sCurFilter->mConstVal = NULL;

                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].mInfo.mValueInfo->mDataValue->mType,
                                               aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].mInfo.mValueInfo->mDataValue->mType,
                                               & sCurFilter->mFunc,
                                               STL_FALSE );
                    }
                    else
                    {
                        /**
                         * column vs column ( idx1 < idx2 ) : Operand의 위치 바꾸기 => reverse function 적용
                         */

                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            *sFuncCode->mArgs[1]->mExpr.mInnerColumn->mIdx );
                        
                        sCurFilter->mColIdx1 = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            *sFuncCode->mArgs[0]->mExpr.mInnerColumn->mIdx );

                        sCurFilter->mColIdx2  = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
                        sCurFilter->mColVal2  = & sValueBlock->mColumnInRow;
                        sCurFilter->mConstVal = NULL;
                        
                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].mInfo.mValueInfo->mDataValue->mType,
                                               aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].mInfo.mValueInfo->mDataValue->mType,
                                               & sCurFilter->mFunc,
                                               STL_TRUE );
                    }
                }
                else
                {
                    if( sFuncCode->mArgCount == 1 )
                    {
                        /**
                         * column 하나만으로 구성된 filter
                         */ 

                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            *sFuncCode->mArgs[0]->mExpr.mInnerColumn->mIdx );
            
                        sCurFilter->mColIdx1 = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* emtpy value 설정 */
                        sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT;
                        sCurFilter->mColVal2  = knlEmtpyColumnValue;
                        sCurFilter->mConstVal = NULL;
                        
                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].mInfo.mValueInfo->mDataValue->mType,
                                               0,
                                               & sCurFilter->mFunc,
                                               STL_FALSE );
                    }
                    else
                    {
                        /**
                         * column vs constant value
                         */
                        
                        STL_DASSERT( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN );
                        STL_DASSERT( sFuncCode->mArgs[0]->mExpr.mInnerColumn->mRelationNode == aRelationNode );
            
                        /* column value 설정 */
                        GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                            sValueBlock,
                            *sFuncCode->mArgs[0]->mExpr.mInnerColumn->mIdx );

                        sCurFilter->mColIdx1 = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
                        sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                        /* constant value 설정 */
                        if( aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].mCast != NULL )
                        {
                            sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].mCast->mCastResultBuf;
                        }
                        else
                        {
                            if( (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                                (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_VALUE) ||
                                (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                                (sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_OUTER_COLUMN) )
                            {
                                sDataValue = aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].
                                    mInfo.mValueInfo->mDataValue;
                            }
                            else
                            {
                                if( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
                                {
                                    sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].
                                        mInfo.mListFunc->mResultValue;
                                }
                                else
                                {
                                    sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].
                                        mInfo.mFuncData->mResultValue;
                                }
                            }
                        }

                        STL_TRY( knlAllocRegionMem( aRegionMem,
                                                    STL_SIZEOF( knlColumnInReadRow ),
                                                    (void **) & sColumnInReadRow,
                                                    KNL_ENV(aEnv) )
                                 == STL_SUCCESS );

                        sColumnInReadRow->mValue  = (void *) sDataValue->mValue;
                        sColumnInReadRow->mLength = sDataValue->mLength;
                                
                        sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT;
                        sCurFilter->mColVal2  = sColumnInReadRow;
                        sCurFilter->mConstVal = sDataValue;
                        
                        /* function pointer 설정 */
                        qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                               aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].mInfo.mValueInfo->mDataValue->mType,
                                               sDataValue->mType,
                                               & sCurFilter->mFunc,
                                               STL_FALSE );
                    }
                }
            }
            else
            {
                /**
                 * column vs constant value : reverse function
                 */
                
                STL_DASSERT( sFuncCode->mArgCount == 2 );         /* constant value만으로 구성된 physical filter는 없다. */
                STL_DASSERT( sFuncCode->mArgs[1]->mType == QLV_EXPR_TYPE_INNER_COLUMN );
                STL_DASSERT( sFuncCode->mArgs[1]->mExpr.mInnerColumn->mRelationNode == aRelationNode );
                
                /* column value 설정 */
                GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                    sValueBlock,
                    *sFuncCode->mArgs[1]->mExpr.mInnerColumn->mIdx );

                sCurFilter->mColIdx1 = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
                sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

                /* constant value 설정 */
                if( aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].mCast != NULL )
                {
                    sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].mCast->mCastResultBuf;
                }
                else
                {
                    if( (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                        (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_VALUE) ||
                        (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                        (sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_OUTER_COLUMN) )
                    {
                        sDataValue = aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].
                            mInfo.mValueInfo->mDataValue;
                    }
                    else
                    {
                        if( sFuncCode->mArgs[0]->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
                        {
                            sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].
                                mInfo.mListFunc->mResultValue;
                        }
                        else
                        {
                            sDataValue = & aExprDataContext->mContexts[ sFuncCode->mArgs[0]->mOffset ].
                                mInfo.mFuncData->mResultValue;
                        }
                    }
                }

                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            STL_SIZEOF( knlColumnInReadRow ),
                                            (void **) & sColumnInReadRow,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sColumnInReadRow->mValue  = (void *) sDataValue->mValue;
                sColumnInReadRow->mLength = sDataValue->mLength;
                                
                sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT;
                sCurFilter->mColVal2  = sColumnInReadRow;
                sCurFilter->mConstVal = sDataValue;

                /* function pointer 설정 */
                qloGetPhysicalFuncPtr( sFuncCode->mFuncId,
                                       sDataValue->mType,
                                       aExprDataContext->mContexts[ sFuncCode->mArgs[1]->mOffset ].mInfo.mValueInfo->mDataValue->mType,
                                       & sCurFilter->mFunc,
                                       STL_TRUE );
            }
        }
        else if( sFilterExpr->mType == QLV_EXPR_TYPE_LIST_FUNCTION )
        {
            /* List Function으로 구성된 filter */
            sListFunc = sFilterExpr->mExpr.mListFunc;
            STL_DASSERT( sListFunc->mArgs[1]->mType == QLV_EXPR_TYPE_LIST_COLUMN );

            sListColLeft = sListFunc->mArgs[1]->mExpr.mListCol;
            STL_DASSERT( sListColLeft->mArgs[0]->mType == QLV_EXPR_TYPE_ROW_EXPR );

            sRowExpr1 = sListColLeft->mArgs[0]->mExpr.mRowExpr;
           
            STL_DASSERT( sRowExpr1->mArgs[0]->mType == QLV_EXPR_TYPE_INNER_COLUMN );
            STL_DASSERT( sRowExpr1->mArgs[0]->mExpr.mInnerColumn->mRelationNode == aRelationNode );

            /* column value 설정 */
            GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                sValueBlock,
                *sRowExpr1->mArgs[0]->mExpr.mInnerColumn->mIdx );

            sCurFilter->mColIdx1 = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
            sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;
                      
            STL_DASSERT( sListFunc->mArgs[0]->mType == QLV_EXPR_TYPE_LIST_COLUMN );

            sListColRight = sListFunc->mArgs[0]->mExpr.mListCol;
            STL_DASSERT( sListColRight->mArgs[0]->mType == QLV_EXPR_TYPE_ROW_EXPR );

//            sRowExpr2 = sListColRight->mArgs[0]->mExpr.mRowExpr;

            /* Constant value 설정 */

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( knlColumnInReadRow ),
                                        (void **) & sColumnInReadRow,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( dtlDataValue ) * ( sListColRight->mArgCount + 1 ),
                                        (void **) & sDataValue,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            
            /**
             * @todo IN Physical Function Pointer
             * aIsInPhysical = STL_TRUE인 경우, 기존의 Physical Function 과 달리
             * Left와 Right의 void형 포인트로 dtlDataValue 가 list 로 오게된다.
             * Function pointer를 통해 dtlDataValue를 physical function안에서 하나씩 처리하도록 함.
             */

            /* RightVal의 첫번째 DataValue는 Left의 Type만 갖고 있다 */
            sDataValue[sListColRight->mArgCount].mType
                = aExprDataContext->mContexts[ sRowExpr1->mArgs[0]->mOffset ].mInfo.mValueInfo->mDataValue->mType;
            
            for( sLoop2 = 0 ; sLoop2 < sListColRight->mArgCount ; sLoop2 ++ )
            {
                if( aExprDataContext->mContexts
                    [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mCast != NULL )
                {
                    sDataValue[sLoop2].mValue = & aExprDataContext->mContexts
                        [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mCast->mCastResultBuf;
                }
                else
                {
                    if( (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT) ||
                        (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_VALUE) ||
                        (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_REFERENCE) ||
                        (sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType == QLV_EXPR_TYPE_OUTER_COLUMN) )
                    {
                        sDataValue[sLoop2].mValue = aExprDataContext->mContexts
                            [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mInfo.mValueInfo->mDataValue;
                    }
                    else
                    {
                        if( sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mType ==
                            QLV_EXPR_TYPE_LIST_FUNCTION )
                        {
                            sDataValue[sLoop2].mValue = & aExprDataContext->mContexts
                                [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mInfo.mListFunc->mResultValue;
                        }
                        else
                        {
                            sDataValue[sLoop2].mValue = & aExprDataContext->mContexts
                                [ sListColRight->mArgs[sLoop2]->mExpr.mRowExpr->mArgs[0]->mOffset ].mInfo.mFuncData->mResultValue;
                        }
                    }
                }
            }
            sColumnInReadRow->mValue  = sDataValue;
            sColumnInReadRow->mLength = sListColRight->mArgCount;
                    
            sCurFilter->mColIdx2  = KNL_PHYSICAL_LIST_VALUE_CONSTANT;
            sCurFilter->mColVal2  = sColumnInReadRow;
            sCurFilter->mConstVal = sDataValue;
            
            sCurFilter->mFunc = dtlInPhysicalFuncPointer[KNL_IN_PHYSICAL_FUNC_ID_PTR( sListFunc->mFuncId )];
        }
        else
        {
            /* column 하나만으로 구성된 filter */
            STL_DASSERT( sFilterExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN );
            STL_DASSERT( sFilterExpr->mExpr.mInnerColumn->mRelationNode == aRelationNode );
                
            /* function pointer 설정 */
            sCurFilter->mFunc = dtlPhysicalFuncIsTrue;
            
            /* column value 설정 */
            GET_VALUE_BLOCK_WITH_COLUMN_ORDER(
                sValueBlock,
                *sFilterExpr->mExpr.mInnerColumn->mIdx );
            
            sCurFilter->mColIdx1 = KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock );
            sCurFilter->mColVal1 = & sValueBlock->mColumnInRow;

            /* emtpy value 설정 */
            sCurFilter->mColIdx2  = KNL_PHYSICAL_VALUE_CONSTANT;
            sCurFilter->mColVal2  = knlEmtpyColumnValue;
            sCurFilter->mConstVal = NULL;
        }

        STL_DASSERT( sCurFilter->mFunc != NULL );
        
        sCurFilter++;
    }
    
    STL_RAMP( RAMP_SKIP_INNER_COLUMN );

    
    /**
     * Column1 Order 기준으로 Filter 정렬
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlPhysicalFilterPtr ) * sCount,
                                (void **) & sSortList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* insertion sort */
    sCount = sExprListCnt;
    for( sLoop1 = 0 ; sLoop1 < sCount ; sLoop1++ )
    {
        sSortList[ sLoop1 ] = & sPhysicalFilter[ sLoop1 ];
    }

    for( sLoop1 = 1 ; sLoop1 < sCount ; sLoop1++ )
    {
        if( sSortList[ sLoop1 - 1 ]->mColIdx1 <= sSortList[ sLoop1 ]->mColIdx1 )
        {
            continue;
        }

        sSortTemp = sSortList[ sLoop1 ];
        sSortList[ sLoop1 ] = sSortList[ sLoop1 - 1 ];
            
        for( sLoop2 = sLoop1 - 1 ; sLoop2 > 0 ; sLoop2-- )
        {
            if( sSortList[ sLoop2 - 1 ]->mColIdx1 <= sSortTemp->mColIdx1 )
            {
                break;
            }
            sSortList[ sLoop2 ] = sSortList[ sLoop2 - 1 ];
        }
        sSortList[ sLoop2 ] = sSortTemp;
    }

    
    /**
     * Physical Filter List의 Link 구성
     */
    
    for( sLoop1 = 0 ; sLoop1 < sCount - 1 ; sLoop1++ )
    {
        sSortList[ sLoop1 ]->mNext = sSortList[ sLoop1 + 1 ];
    }
    sSortList[ sLoop1 ]->mNext = NULL;


    /**
     * OUTPUT 설정
     */
    
    *aPhysicalFilter = sSortList[0];

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief INTO 절의 CAST 정보를 구축한다.
 * @param[in]     aSQLStmt         SQL Statement 객체
 * @param[in]     aQueryExprInfo   Expression Information of a QUERY
 * @param[in]     aBindContext     Bind Context
 * @param[in]     aIntoCount       INTO Count
 * @param[in]     aTargetExprList  Target 절의 Init Expression List
 * @param[in]     aInitIntoArray   INTO 절의 Init Plan Array
 * @param[in,out] aDataArea        Data Area
 * @param[in]     aRegionMem       Region Memory
 * @param[in]     aEnv             Environment
 * @remarks
 * Complete Optimization Node를 수행한 후에 INTO절 최적화를 수행한다.
 * 별도의 정보 구축 없이 Bind Type 후 호환 여부를 Validation 한다.
 */ 
stlStatus qlndDataOptIntoPhrase( qllStatement        * aSQLStmt,
                                 qloExprInfo         * aQueryExprInfo,
                                 knlBindContext      * aBindContext,
                                 stlInt32              aIntoCount,
                                 qlvRefExprList      * aTargetExprList,
                                 qlvInitExpression   * aInitIntoArray,
                                 qllDataArea         * aDataArea,
                                 knlRegionMem        * aRegionMem,
                                 qllEnv              * aEnv )
{
    stlInt32             sLoop           = 0;

    qlvRefExprItem     * sExprItem       = NULL;
    knlValueBlockList  * sOutParamBlock  = NULL;
    knlValueBlockList  * sIntoBlock      = NULL;
    knlValueBlockList  * sPrevBlock      = NULL;
    
    knlBindType          sUserBindType   = KNL_BIND_TYPE_INVALID;
    dtlDataType          sTargetDBType   = DTL_TYPE_NA;
    dtlDataType          sIntoDBType     = DTL_TYPE_NA;

    knlCastFuncInfo    * sIntoAssignInfo = NULL;

    qlvInitExpression  * sExpr           = NULL;

    qloDBType            sIntoqloDBType;
    knlCastFuncInfo    * sIntoCastFuncInfo = NULL;
    

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSQLStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQueryExprInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIntoCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetExprList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitIntoArray != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea->mExprDataContext != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    

    /**
     * INTO 절 정보 구축을 위한 공간 할당
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( knlCastFuncInfo ) * aIntoCount,
                                (void **) & sIntoAssignInfo,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
        
    stlMemset( sIntoAssignInfo, 0x00, STL_SIZEOF( knlCastFuncInfo ) * aIntoCount );

    aDataArea->mIntoAssignInfo = sIntoAssignInfo;
    
    
    /**
     * INTO 정보 구성
     */
    
    STL_DASSERT( aBindContext != NULL );

    aDataArea->mIntoBlock = NULL;

    sExprItem = aTargetExprList->mHead;
    
    for( sLoop = 0 ; sLoop < aIntoCount ; sLoop++ )
    {
        /**
         * Bind Context 로부터 Output Parameter 정보 획득
         */

        STL_DASSERT( sExprItem != NULL );
        STL_DASSERT( sExprItem->mExprPtr != NULL );
        
        /**
         * OUT bind type 에 부합하는 Bind Type 이어야 함.
         */

        STL_TRY( knlGetBindParamType( aBindContext,
                                      aInitIntoArray[ sLoop ].mExpr.mBindParam->mBindParamIdx + 1,
                                      & sUserBindType,
                                      KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
        
        STL_DASSERT( (sUserBindType == KNL_BIND_TYPE_OUT) ||
                     (sUserBindType == KNL_BIND_TYPE_INOUT) );

        STL_TRY( knlGetParamValueBlock( aBindContext,
                                        aInitIntoArray[ sLoop ].mExpr.mBindParam->mBindParamIdx + 1,
                                        NULL,
                                        & sOutParamBlock,
                                        KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
        
        STL_DASSERT( sOutParamBlock != NULL );


        /**
         * Assign Information 설정 : Cast Function Pointer
         */

        sIntoDBType   = KNL_GET_BLOCK_DB_TYPE( sOutParamBlock );
        sTargetDBType = aQueryExprInfo->mExprDBType[ sExprItem->mExprPtr->mOffset ].mDBType;

        sExpr = sExprItem->mExprPtr;
        if( sExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
        {
            sExpr = sExpr->mExpr.mInnerColumn->mOrgExpr;
        }
        else
        {
            /* Do Nothing */
        }

        if( sExpr->mType == QLV_EXPR_TYPE_CONSTANT_EXPR_RESULT )
        {
            sExpr = sExpr->mExpr.mConstExpr->mOrgExpr;
            if( sExpr->mType == QLV_EXPR_TYPE_VALUE )
            {
                if( sExpr->mExpr.mValue->mValueType == 
                    QLV_VALUE_TYPE_NULL )
                {
                    sIntoAssignInfo[ sLoop ].mCastFuncPtr = NULL;
                }
                else
                {
                    /* Target TYPE 과 Output TYPE 이 호환되는지 검사 */
                    STL_TRY_THROW(
                        dtlIsApplicableCast(
                            sTargetDBType,
                            aQueryExprInfo->mExprDBType[ sExprItem->mExprPtr->mOffset ].mIntervalIndicator,
                            sIntoDBType,
                            KNL_GET_BLOCK_INTERVAL_INDICATOR( sOutParamBlock ) ) == STL_TRUE,
                        RAMP_ERR_INCOMPATIBLE_TYPE );
                }
            }
            else
            {
                /* Target TYPE 과 Output TYPE 이 호환되는지 검사 */                
                STL_TRY_THROW(
                    dtlIsApplicableCast(
                        sTargetDBType,
                        aQueryExprInfo->mExprDBType[ sExprItem->mExprPtr->mOffset ].mIntervalIndicator,
                        sIntoDBType,
                        KNL_GET_BLOCK_INTERVAL_INDICATOR( sOutParamBlock ) ) == STL_TRUE,
                    RAMP_ERR_INCOMPATIBLE_TYPE );
            }
        }
        else
        {
            /* Target TYPE 과 Output TYPE 이 호환되는지 검사 */                
            STL_TRY_THROW(
                dtlIsApplicableCast(
                    sTargetDBType,
                    aQueryExprInfo->mExprDBType[ sExprItem->mExprPtr->mOffset ].mIntervalIndicator,
                    sIntoDBType,
                    KNL_GET_BLOCK_INTERVAL_INDICATOR( sOutParamBlock ) ) == STL_TRUE,
                RAMP_ERR_INCOMPATIBLE_TYPE );
        }
        
        /**
         * INTO value block list 구성
         */
        
        /* share out-param value block */
        STL_TRY( knlInitShareBlock( & sIntoBlock,
                                    sOutParamBlock,
                                    aRegionMem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /* link */
        if( aDataArea->mIntoBlock == NULL )
        {
            aDataArea->mIntoBlock = sIntoBlock;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevBlock, sIntoBlock );
        }
        sPrevBlock = sIntoBlock;
        
        
        /**
         * Assign Information 설정 : Cast Result Buffer
         * => 별도의 공간을 할당 받지 않음 ( executin 시 Out-Bind Parameter의 공간 활용)
         */
        
        /* Do Nothing */
        
        
        /**
         * Assign Information 설정 : Cast Function Arguments
         */
        
        /* INTO 의 data value */

        sIntoqloDBType.mDBType = KNL_GET_BLOCK_DB_TYPE( sOutParamBlock );
        sIntoqloDBType.mSourceDBType = sTargetDBType;
        sIntoqloDBType.mIsNeedCast = STL_TRUE;
        sIntoqloDBType.mCastFuncIdx = DTL_GET_CAST_FUNC_PTR_IDX( sIntoqloDBType.mSourceDBType, sIntoqloDBType.mDBType);

        sIntoqloDBType.mArgNum1 = KNL_GET_BLOCK_ARG_NUM1( sOutParamBlock );
        sIntoqloDBType.mArgNum2 = KNL_GET_BLOCK_ARG_NUM2( sOutParamBlock );
        sIntoqloDBType.mStringLengthUnit = KNL_GET_BLOCK_STRING_LENGTH_UNIT( sOutParamBlock );
        sIntoqloDBType.mIntervalIndicator = KNL_GET_BLOCK_INTERVAL_INDICATOR( sOutParamBlock );

        sIntoqloDBType.mSourceArgNum1 = aQueryExprInfo->mExprDBType[ sExprItem->mExprPtr->mOffset ].mArgNum1;
        sIntoqloDBType.mSourceArgNum2 = aQueryExprInfo->mExprDBType[ sExprItem->mExprPtr->mOffset ].mArgNum2;
        sIntoqloDBType.mSourceStringLengthUnit = aQueryExprInfo->mExprDBType[ sExprItem->mExprPtr->mOffset ].mStringLengthUnit;
        sIntoqloDBType.mSourceIntervalIndicator = aQueryExprInfo->mExprDBType[ sExprItem->mExprPtr->mOffset ].mIntervalIndicator;

        sIntoCastFuncInfo = & sIntoAssignInfo[ sLoop ];
        STL_TRY( qloDataSetInternalCastInfo( aSQLStmt,
                                             aRegionMem,
                                             & sIntoqloDBType,
                                             & sIntoCastFuncInfo,
                                             STL_FALSE, /* aIsAllocCastFuncInfo */
                                             STL_FALSE, /* aIsAllocCastResultBuf */
                                             aEnv )
                 == STL_SUCCESS );

        sExprItem = sExprItem->mNext;
    }


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INCOMPATIBLE_TYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_CONVERSION_NOT_APPLICABLE,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      gDataTypeDefinition[ sTargetDBType ].mSqlNormalTypeName,
                      gDataTypeDefinition[ sIntoDBType ].mSqlNormalTypeName );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DML 에 대한 Index Insert/Delete Key Value List 를 할당받는다.
 * @param[in]   aDBCStmt                DBC Statement
 * @param[in]   aDataArea               Data Area
 * @param[in]   aReadColList            Read Column List
 * @param[in]   aIndexDictHandle        Index Dictionary Handle
 * @param[out]  aIndexDeleteValueList   Index Delete Key 를 위한 Value List
 * @param[in]   aEnv                    Environment
 */
stlStatus qlndShareIndexValueListForDelete( qllDBCStmt          * aDBCStmt,
                                            qllDataArea         * aDataArea,
                                            qlvRefExprList      * aReadColList,
                                            ellDictHandle       * aIndexDictHandle,
                                            knlValueBlockList  ** aIndexDeleteValueList,
                                            qllEnv              * aEnv )
{
    stlInt32              i;
    stlInt32              sIndexKeyCnt;
    ellIndexKeyDesc     * sIndexKeyDesc     = NULL;

    qlvInitExpression   * sExpr             = NULL;
    qlvRefExprItem      * sRefExprItem      = NULL;

    knlValueBlockList   * sDeleteValueList  = NULL;
    knlValueBlockList   * sValueList        = NULL;
    knlValueBlockList   * sPrevValueList    = NULL;


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReadColList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexDictHandle != NULL, QLL_ERROR_STACK(aEnv) );


    sIndexKeyCnt  = ellGetIndexKeyCount( aIndexDictHandle );
    sIndexKeyDesc = ellGetIndexKeyDesc( aIndexDictHandle );

    sDeleteValueList = NULL;

    for( i = 0; i < sIndexKeyCnt; i++ )
    {
        sRefExprItem = aReadColList->mHead;
        while( sRefExprItem != NULL )
        {
            sExpr = sRefExprItem->mExprPtr;
            while( sExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
            {
                sExpr = sExpr->mExpr.mInnerColumn->mOrgExpr;
            }

            STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_COLUMN );

            if( ellGetColumnID( sExpr->mExpr.mColumn->mColumnHandle )
                == ellGetColumnID( &sIndexKeyDesc[i].mKeyColumnHandle ) )
            {
                STL_TRY( knlInitShareBlockFromBlock( & sValueList,
                                                     aDataArea->mExprDataContext->mContexts[ sRefExprItem->mExprPtr->mOffset ].mInfo.mValueInfo,
                                                     & QLL_DATA_PLAN(aDBCStmt),
                                                     KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                break;
            }

            sRefExprItem = sRefExprItem->mNext;
        }

        STL_DASSERT( sRefExprItem != NULL );

        /**
         * Link 연결
         */

        if ( sDeleteValueList == NULL )
        {
            sDeleteValueList = sValueList;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevValueList, sValueList );
        }

        sPrevValueList = sValueList;
    }


    /**
     * Output 설정
     */

    *aIndexDeleteValueList = sDeleteValueList;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DML 에 대한 Index Insert/Delete Key Value List 를 할당받는다.
 * @param[in]   aDBCStmt                DBC Statement
 * @param[in]   aDataArea               Data Area
 * @param[in]   aReadColList            Read Column List
 * @param[in]   aTableWriteBlock        Table Write Block List
 * @param[in]   aIndexDictHandle        Index Dictionary Handle
 * @param[out]  aIndexInsertValueList   Index Insert Key 를 위한 Value List
 * @param[in]   aEnv                    Environment
 */
stlStatus qlndShareIndexValueListForInsert( qllDBCStmt          * aDBCStmt,
                                            qllDataArea         * aDataArea,
                                            qlvRefExprList      * aReadColList,
                                            knlValueBlockList   * aTableWriteBlock,
                                            ellDictHandle       * aIndexDictHandle,
                                            knlValueBlockList  ** aIndexInsertValueList,
                                            qllEnv              * aEnv )
{
    stlInt32              i;
    stlInt32              sIndexKeyCnt;
    ellIndexKeyDesc     * sIndexKeyDesc     = NULL;

    ellDictHandle       * sColumnHandle     = NULL;

    qlvInitExpression   * sExpr             = NULL;
    qlvRefExprItem      * sRefExprItem      = NULL;

    knlValueBlockList   * sColumnValueList  = NULL;
    knlValueBlockList   * sInsertValueList  = NULL;
    knlValueBlockList   * sValueList        = NULL;
    knlValueBlockList   * sPrevValueList    = NULL;


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReadColList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableWriteBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexDictHandle != NULL, QLL_ERROR_STACK(aEnv) );


    sIndexKeyCnt  = ellGetIndexKeyCount( aIndexDictHandle );
    sIndexKeyDesc = ellGetIndexKeyDesc( aIndexDictHandle );

    sInsertValueList = NULL;

    for( i = 0; i < sIndexKeyCnt; i++ )
    {
        /**
         * Write Value List 에서 검색하고, 존재하지 않으면 Read Column List를 이용하여 검색 
         */

        sColumnHandle = &sIndexKeyDesc[i].mKeyColumnHandle;
        sColumnValueList = ellFindColumnValueList( aTableWriteBlock,
                                                   ellGetColumnTableID(sColumnHandle),
                                                   ellGetColumnIdx(sColumnHandle) );

        /* Write Column List에서 못찾으면 Read Column List에서 찾는다. */
        if( sColumnValueList == NULL )
        {
            sRefExprItem = aReadColList->mHead;
            while( sRefExprItem != NULL )
            {
                sExpr = sRefExprItem->mExprPtr;
                while( sExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
                {
                    sExpr = sExpr->mExpr.mInnerColumn->mOrgExpr;
                }

                STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_COLUMN );

                if( ellGetColumnID( sExpr->mExpr.mColumn->mColumnHandle )
                    == ellGetColumnID( &sIndexKeyDesc[i].mKeyColumnHandle ) )
                {
                    STL_TRY( knlInitShareBlockFromBlock( & sValueList,
                                                         aDataArea->mExprDataContext->mContexts[ sRefExprItem->mExprPtr->mOffset ].mInfo.mValueInfo,
                                                         & QLL_DATA_PLAN(aDBCStmt),
                                                         KNL_ENV(aEnv) )
                             == STL_SUCCESS );

                    break;
                }

                sRefExprItem = sRefExprItem->mNext;
            }

            STL_DASSERT( sRefExprItem != NULL );
        }
        else
        {
            STL_TRY( knlInitShareBlock( & sValueList,
                                        sColumnValueList,
                                        & QLL_DATA_PLAN(aDBCStmt),
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
        }


        /**
         * Link 연결
         */

        if ( sInsertValueList == NULL )
        {
            sInsertValueList = sValueList;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevValueList, sValueList );
        }

        sPrevValueList = sValueList;
    }


    /**
     * Output 설정
     */

    *aIndexInsertValueList = sInsertValueList;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Supplemental Logging 을 위한 Primary Key Value List 를 Table Read Value List 와 공유한다.
 * @param[in]   aDBCStmt            DBC Statement
 * @param[in]   aDataArea           Data Area
 * @param[in]   aReadColList        Read Column List
 * @param[in]   aPrimaryKeyHandle   Primary Key Constraint Handle
 * @param[out]  aSuppleValueList    Index Delete Key 를 위한 Value List
 * @param[in]   aEnv                Environment
 */
stlStatus qlndShareSuppleValueList( qllDBCStmt          * aDBCStmt,
                                    qllDataArea         * aDataArea,
                                    qlvRefExprList      * aReadColList,
                                    ellDictHandle       * aPrimaryKeyHandle,
                                    knlValueBlockList  ** aSuppleValueList,
                                    qllEnv              * aEnv )
{
    stlInt32              i;
    stlInt32              j;
    stlInt32              sIndexKeyCnt;
    ellIndexKeyDesc     * sIndexKeyDesc         = NULL;
    ellDictHandle       * sIndexHandle          = NULL;
    ellDictHandle      ** sIndexKeyHandleArr    = NULL;

    qlvInitExpression   * sExpr                 = NULL;
    qlvRefExprItem      * sRefExprItem          = NULL;

    knlValueBlockList   * sPrimaryValueList     = NULL;
    knlValueBlockList   * sValueList            = NULL;
    knlValueBlockList   * sPrevValueList        = NULL;


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReadColList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPrimaryKeyHandle != NULL, QLL_ERROR_STACK(aEnv) );


    sIndexHandle  = ellGetConstraintIndexDictHandle( aPrimaryKeyHandle );
    sIndexKeyCnt  = ellGetIndexKeyCount( sIndexHandle );
    sIndexKeyDesc = ellGetIndexKeyDesc( sIndexHandle );

    /* Index에 존재하는 Key Column들을 table order 순으로 정렬한다. */
    STL_TRY( knlAllocRegionMem( & QLL_DATA_PLAN(aDBCStmt),
                                STL_SIZEOF( ellDictHandle* ) * sIndexKeyCnt,
                                (void**) &sIndexKeyHandleArr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sIndexKeyHandleArr[0] = &sIndexKeyDesc[0].mKeyColumnHandle;
    for( i = 1; i < sIndexKeyCnt; i++ )
    {
        for( j = i - 1; j >= 0; j-- )
        {
            if( ellGetColumnIdx( &sIndexKeyDesc[i].mKeyColumnHandle )
                < ellGetColumnIdx( sIndexKeyHandleArr[j] ) )
            {
                sIndexKeyHandleArr[j+1] = sIndexKeyHandleArr[j];
            }
            else
            {
                break;
            }
        }

        sIndexKeyHandleArr[j+1] = &sIndexKeyDesc[i].mKeyColumnHandle;
    }

    /* Value Block들을 찾아 연결 */
    sPrimaryValueList = NULL;

    for( i = 0; i < sIndexKeyCnt; i++ )
    {
        sRefExprItem = aReadColList->mHead;
        while( sRefExprItem != NULL )
        {
            sExpr = sRefExprItem->mExprPtr;
            while( sExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
            {
                sExpr = sExpr->mExpr.mInnerColumn->mOrgExpr;
            }

            STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_COLUMN );

            if( ellGetColumnID( sExpr->mExpr.mColumn->mColumnHandle )
                == ellGetColumnID( sIndexKeyHandleArr[i] ) )
            {
                STL_TRY( knlInitShareBlockFromBlock( & sValueList,
                                                     aDataArea->mExprDataContext->mContexts[ sRefExprItem->mExprPtr->mOffset ].mInfo.mValueInfo,
                                                     & QLL_DATA_PLAN(aDBCStmt),
                                                     KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sValueList->mTableID = ellGetColumnTableID( sExpr->mExpr.mColumn->mColumnHandle );
                sValueList->mColumnOrder = ellGetColumnIdx( sExpr->mExpr.mColumn->mColumnHandle );
                break;
            }

            sRefExprItem = sRefExprItem->mNext;
        }

        STL_DASSERT( sRefExprItem != NULL );

        /**
         * Link 연결
         */

        if ( sPrimaryValueList == NULL )
        {
            sPrimaryValueList = sValueList;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevValueList, sValueList );
        }

        sPrevValueList = sValueList;
    }


    /**
     * Output 설정
     */

    *aSuppleValueList = sPrimaryValueList;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief NOT NULL, CHECK constraint column 의 value list 를 read column list 와 공유한다.
 * @param[in]   aDBCStmt            DBC Statement
 * @param[in]   aDataArea           Data Area
 * @param[in]   aReadColList        Read Column List
 * @param[in]   aColumnHandle       Constraint Column Handle
 * @param[out]  aNotNullValueList   NOT NULL column 을 위한 Value List
 * @param[in]   aEnv                Environment
 */
stlStatus qlndShareNotNullReadColumnValueList( qllDBCStmt          * aDBCStmt,
                                               qllDataArea         * aDataArea,
                                               qlvRefExprList      * aReadColList,
                                               ellDictHandle       * aColumnHandle,
                                               knlValueBlockList  ** aNotNullValueList,
                                               qllEnv              * aEnv )
{
    qlvInitExpression   * sExpr                 = NULL;
    qlvRefExprItem      * sRefExprItem          = NULL;

    knlValueBlockList   * sValueList            = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aDBCStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aReadColList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNotNullValueList != NULL, QLL_ERROR_STACK(aEnv) );

    sRefExprItem = aReadColList->mHead;
    while( sRefExprItem != NULL )
    {
        sExpr = sRefExprItem->mExprPtr;
        while( sExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
        {
            sExpr = sExpr->mExpr.mInnerColumn->mOrgExpr;
        }

        STL_DASSERT( sExpr->mType == QLV_EXPR_TYPE_COLUMN );

        if( ellGetColumnID( sExpr->mExpr.mColumn->mColumnHandle ) == ellGetColumnID( aColumnHandle ) )
        {
            STL_TRY( knlInitShareBlockFromBlock(
                         & sValueList,
                         aDataArea->mExprDataContext->mContexts[ sRefExprItem->mExprPtr->mOffset ].mInfo.mValueInfo,
                         & QLL_DATA_PLAN(aDBCStmt),
                         KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            break;
        }

        sRefExprItem = sRefExprItem->mNext;
    }

    STL_DASSERT( sRefExprItem != NULL );

    /**
     * Output 설정
     */

    *aNotNullValueList = sValueList;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief long varchar 와 long varbinary 를 참조하는 Data Value List를 구성한다.
 * @param[in,out] aLongTypeValueList   long varchar 또는 long varbinary를 가지는 Data Value List
 * @param[in]     aDataValueCnt        Data Value Array Count
 * @param[in]     aDataValueArr        Data Value Array
 * @param[in]     aRegionMem           Region Memory
 * @param[in]     aEnv                 Environment
 * @remarks Data Optimization 과정에서만 호출
 */
stlStatus qlndAddLongTypeValueList( qllLongTypeValueList  * aLongTypeValueList,
                                    stlInt32                aDataValueCnt,
                                    dtlDataValue          * aDataValueArr,
                                    knlRegionMem          * aRegionMem,
                                    qllEnv                * aEnv )
{
    qllLongTypeValueItem  * sValueItem = NULL;
    stlInt32                sIdx       = 0;
    
    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aLongTypeValueList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDataValueCnt > 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDataValueArr != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK( aEnv ) );

    
    /**
     * Long Type Data Value Item 생성
     */
    
    STL_TRY_THROW( knlAllocRegionMem( aRegionMem,
                                      STL_SIZEOF( qllLongTypeValueItem ),
                                      (void **) & sValueItem,
                                      KNL_ENV( aEnv ) )
                   == STL_SUCCESS,
                   RAMP_ERR_CANNOT_MAKE_VALUE_ITEM );

    sValueItem->mArrSize    = aDataValueCnt;
    sValueItem->mDataValues = aDataValueArr;
    sValueItem->mNext       = NULL;

    if( aLongTypeValueList->mCount == 0 )
    {
        aLongTypeValueList->mCount = 1;
        aLongTypeValueList->mHead  = sValueItem;
        aLongTypeValueList->mTail  = sValueItem;
    }
    else
    {
        aLongTypeValueList->mCount++;
        aLongTypeValueList->mTail->mNext = sValueItem;
        aLongTypeValueList->mTail        = sValueItem;
    }


#ifdef STL_DEBUG
    for( ; aDataValueCnt > 0 ; )
    {
        aDataValueCnt--;
        STL_ASSERT( ( aDataValueArr[ aDataValueCnt ].mType == DTL_TYPE_LONGVARCHAR ) ||
                    ( aDataValueArr[ aDataValueCnt ].mType == DTL_TYPE_LONGVARBINARY ) );
        STL_ASSERT( aDataValueArr[ aDataValueCnt ].mValue != NULL );
        STL_ASSERT( aDataValueArr[ aDataValueCnt ].mBufferSize > 0 );
    }
#endif


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CANNOT_MAKE_VALUE_ITEM )
    {
        /* 현재 Item으로 구성할 Long Value Data 만 해제한다. */
        for( sIdx = 0 ; sIdx < aDataValueCnt ; sIdx++ )
        {
            STL_DASSERT( ( aDataValueArr[ sIdx ].mType == DTL_TYPE_LONGVARCHAR ) ||
                         ( aDataValueArr[ sIdx ].mType == DTL_TYPE_LONGVARBINARY ) );

            if( aDataValueArr[ sIdx ].mValue != NULL )
            {
                (void) knlFreeLongVaryingMem( aDataValueArr[ sIdx ].mValue,
                                              KNL_ENV( aEnv ) );
            }
            else
            {
                /* Do Nothing */
            }

            aDataValueArr[ sIdx ].mValue      = NULL;
            aDataValueArr[ sIdx ].mLength     = 0;
            aDataValueArr[ sIdx ].mBufferSize = 0;
        }
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Long Varchar/Varbinary Data Value List들의 mValue를 해제한다.
 * @param[in,out] aLongTypeValueList   long varchar 또는 long varbinary를 가지는 Data Value List
 * @param[in]     aEnv                 Environment
 */
stlStatus qlndFreeLongTypeValues( qllLongTypeValueList  * aLongTypeValueList,
                                  qllEnv                * aEnv )
{
    qllLongTypeValueItem  * sValueItem  = NULL;
    dtlDataValue          * sDataValue  = NULL;
    stlInt32                sLoop       = 0;
    stlInt32                sIdx        = 0;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aLongTypeValueList != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Data Value List들의 mValue를 초기화
     */

    sValueItem = aLongTypeValueList->mHead;
    for( sLoop = aLongTypeValueList->mCount ; sLoop > 0 ; sLoop-- )
    {
        STL_DASSERT( sValueItem != NULL );
        STL_DASSERT( sValueItem->mArrSize > 0 );
        STL_DASSERT( sValueItem->mDataValues != NULL );

        sDataValue = sValueItem->mDataValues;

        for( sIdx = 0 ; sIdx < sValueItem->mArrSize ; sIdx++ )
        {
            STL_DASSERT( sValueItem->mDataValues != NULL );
            STL_DASSERT( ( sDataValue[ sIdx ].mType == DTL_TYPE_LONGVARCHAR ) ||
                         ( sDataValue[ sIdx ].mType == DTL_TYPE_LONGVARBINARY ) );

            if( sDataValue[ sIdx ].mValue != NULL )
            {
                STL_TRY( knlFreeLongVaryingMem( sDataValue[ sIdx ].mValue,
                                                KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
            }
            else
            {
                /* Do Nothing */
            }

            sDataValue[ sIdx ].mValue      = NULL;
            sDataValue[ sIdx ].mLength     = 0;
            sDataValue[ sIdx ].mBufferSize = 0;
        }
        
        aLongTypeValueList->mCount--;
        aLongTypeValueList->mHead = sValueItem->mNext;
        
        sValueItem = sValueItem->mNext;
    }
    
    aLongTypeValueList->mHead = NULL;
    aLongTypeValueList->mTail = NULL;
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Long Varchar/Varbinary Data Value List들의 mValue를 초기화한다.
 * @param[in,out] aLongTypeValueList   long varchar 또는 long varbinary를 가지는 Data Value List
 * @param[in]     aEnv                 Environment
 */
stlStatus qlndResetLongTypeValues( qllLongTypeValueList  * aLongTypeValueList,
                                   qllEnv                * aEnv )
{
    qllLongTypeValueItem  * sValueItem  = NULL;
    dtlDataValue          * sDataValue  = NULL;
    void                  * sDataBuffer = NULL;
    stlInt32                sLoop       = 0;
    stlInt32                sIdx        = 0;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aLongTypeValueList != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Data Value List들의 mValue를 초기화
     */

    sValueItem = aLongTypeValueList->mHead;
    for( sLoop = 0 ; sLoop < aLongTypeValueList->mCount ; sLoop++ )
    {
        STL_DASSERT( sValueItem != NULL );
        STL_DASSERT( sValueItem->mArrSize > 0 );
        STL_DASSERT( sValueItem->mDataValues != NULL );

        sDataValue = sValueItem->mDataValues;

        for( sIdx = 0 ; sIdx < sValueItem->mArrSize ; sIdx++ )
        {
            STL_DASSERT( sValueItem->mDataValues != NULL );
            STL_DASSERT( ( sDataValue[ sIdx ].mType == DTL_TYPE_LONGVARCHAR ) ||
                         ( sDataValue[ sIdx ].mType == DTL_TYPE_LONGVARBINARY ) );

            if( sDataValue[ sIdx ].mBufferSize > DTL_LONGVARYING_INIT_STRING_SIZE )
            {
                STL_TRY( knlFreeLongVaryingMem( sDataValue[ sIdx ].mValue,
                                                KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
                
                STL_TRY( knlAllocLongVaryingMem( DTL_LONGVARYING_INIT_STRING_SIZE,
                                                 & sDataBuffer,
                                                 KNL_ENV( aEnv ) )
                         == STL_SUCCESS );
            
                sDataValue[ sIdx ].mValue      = sDataBuffer;
                sDataValue[ sIdx ].mBufferSize = DTL_LONGVARYING_INIT_STRING_SIZE;
            }
            else
            {
                /* Do Nothing */
            }
        }
        
        sValueItem = sValueItem->mNext;
    }
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 하나의 Data Value에 대한 mValue를 할당하고 초기화한다.
 * @param[in,out] aLongTypeValueList   long varchar 또는 long varbinary를 가지는 Data Value List
 * @param[in]     aDBType              Data Type
 * @param[in]     aDataValue           Data Value
 * @param[in]     aAllocSize           Data Value's mValue Allocation Size
 * @param[in]     aRegionMem           Region Memory
 * @param[in]     aEnv                 Environment
 */
stlStatus qlndInitSingleDataValue( qllLongTypeValueList   * aLongTypeValueList,
                                   dtlDataType              aDBType,
                                   dtlDataValue           * aDataValue,
                                   stlInt32                 aAllocSize,
                                   knlRegionMem           * aRegionMem,
                                   qllEnv                 * aEnv )
{
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aLongTypeValueList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( ( aDBType >= 0 ) && ( aDBType < DTL_TYPE_MAX ),
                        QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDataValue != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Data Value의 mValue 공간 할당
     */

    STL_DASSERT( aDataValue->mValue == NULL );

    if( aAllocSize > 0 )
    {
        /**
         * Data Value 초기화
         */
    
        STL_TRY( dtlInitDataValue( aDBType,
                                   aAllocSize,
                                   aDataValue,
                                   QLL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
        
        if( ( aDBType == DTL_TYPE_LONGVARCHAR ) ||
            ( aDBType == DTL_TYPE_LONGVARBINARY ) )
        {
            STL_TRY( knlAllocLongVaryingMem( aAllocSize,
                                             (void**) & aDataValue->mValue,
                                             KNL_ENV( aEnv ) )
                     == STL_SUCCESS );

            STL_TRY( qlndAddLongTypeValueList( aLongTypeValueList,
                                               1,
                                               aDataValue,
                                               aRegionMem,
                                               aEnv )
                     == STL_SUCCESS );         
        }
        else
        {
            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        aAllocSize,
                                        & aDataValue->mValue,
                                        KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

        stlMemset( aDataValue->mValue,
                   0x00,
                   aAllocSize );
    }
    else
    {
        /* LONGVARCHAR 나 LONGVARBINARY 타입의 NULL Constant value도 region memory를 사용한다. */
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    1,
                                    & aDataValue->mValue,
                                    KNL_ENV( aEnv ) )
                 == STL_SUCCESS );

        stlMemset( aDataValue->mValue,
                   0x00,
                   aAllocSize );

        STL_TRY( dtlInitDataValue( aDBType,
                                   1,
                                   aDataValue,
                                   QLL_ERROR_STACK( aEnv ) )
                 == STL_SUCCESS );
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Data Value Array를 할당하고 초기화한다.
 * @param[in,out] aLongTypeValueList   long varchar 또는 long varbinary를 가지는 Data Value List
 * @param[in,out] aDataValue           Data Value
 * @param[in]     aDBType              DB Data Type
 * @param[in]     aPrecision           Precision
 * @param[in]     aStringLengthUnit    string length unit (dtlStringLengthUnit 참조)
 * @param[in]     aArraySize           Data Value Array 개수
 * @param[in]     aRegionMem           Region Memory
 * @param[out]    aBufferSize          Allocated Buffer Size For Each Value (not aligned)
 * @param[in]     aEnv                 Environment
 */
stlStatus qlndAllocDataValueArray( qllLongTypeValueList   * aLongTypeValueList,
                                   dtlDataValue           * aDataValue,
                                   dtlDataType              aDBType,
                                   stlInt64                 aPrecision,
                                   dtlStringLengthUnit      aStringLengthUnit,
                                   stlInt32                 aArraySize,
                                   knlRegionMem           * aRegionMem,
                                   stlInt64               * aBufferSize,
                                   qllEnv                 * aEnv )
{
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aLongTypeValueList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDataValue != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( ( aDBType >= 0 ) && ( aDBType < DTL_TYPE_MAX ),
                        QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aArraySize > 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Data Value의 mValue 공간 할당
     */

    STL_TRY( knlAllocDataValueArray( aDataValue,
                                     aDBType,
                                     aPrecision,
                                     aStringLengthUnit,
                                     aArraySize,
                                     aRegionMem,
                                     aBufferSize,
                                     KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    if( ( aDBType == DTL_TYPE_LONGVARCHAR ) ||
        ( aDBType == DTL_TYPE_LONGVARBINARY ) )
    {
        STL_TRY( qlndAddLongTypeValueList( aLongTypeValueList,
                                           aArraySize,
                                           aDataValue,
                                           aRegionMem,
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
 * @brief String으로부터 Data Value를 설정한다.
 * @param[in,out]  aLongTypeValueList  long varchar 또는 long varbinary를 가지는 Data Value List
 * @param[in,out]  aDataValue          Data Value
 * @param[in]      aDBType             DB Data Type
 * @param[in]      aPrecision          Precision
 * @param[in]      aScale              Scale
 * @param[in]      aStringLengthUnit   string length unit (dtlStringLengthUnit 참조)
 * @param[in]      aString             Input String 
 * @param[in]      aStringLength       Input String Length
 * @param[in]      aArraySize          Data Value Array 개수
 * @param[in]      aRegionMem          Region Memory
 * @param[out]     aBufferSize         Allocated Buffer Size For Each Value (not aligned)
 * @param[in]      aEnv                Environment
 */
stlStatus qlndAllocDataValueArrayFromString( qllLongTypeValueList   * aLongTypeValueList,
                                             dtlDataValue           * aDataValue,
                                             dtlDataType              aDBType,
                                             stlInt64                 aPrecision,
                                             stlInt64                 aScale,
                                             dtlStringLengthUnit      aStringLengthUnit,
                                             stlChar                * aString,
                                             stlInt64                 aStringLength,
                                             stlInt32                 aArraySize,
                                             knlRegionMem           * aRegionMem,
                                             stlInt64               * aBufferSize,
                                             qllEnv                 * aEnv )
{
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aLongTypeValueList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aDataValue != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( ( aDBType >= 0 ) && ( aDBType < DTL_TYPE_MAX ),
                        QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aString != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aArraySize > 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Data Value의 mValue 공간 할당
     */

    STL_TRY( knlAllocDataValueArrayFromString( aDataValue,
                                               aDBType,
                                               aPrecision,
                                               aScale,
                                               aStringLengthUnit,
                                               aString,
                                               aStringLength,
                                               aArraySize,
                                               aRegionMem,
                                               aBufferSize,
                                               KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    if( ( aDBType == DTL_TYPE_LONGVARCHAR ) ||
        ( aDBType == DTL_TYPE_LONGVARBINARY ) )
    {
        STL_TRY( qlndAddLongTypeValueList( aLongTypeValueList,
                                           aArraySize,
                                           aDataValue,
                                           aRegionMem,
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
 * @brief Value Block을 복사한다.
 * @param[in,out]  aLongTypeValueList  long varchar 또는 long varbinary를 가지는 Data Value List
 * @param[out]     aValueBlockList     Target Value Block List
 * @param[in]      aAllocLayer         Block이 할당되는 Layer
 * @param[in]      aBlockCnt           Block Read Count
 * @param[in]      aSourceBlockList    참조할 Source Value Block List
 * @param[in]      aRegionMem          Region Memory
 * @param[in]      aEnv                Environment
 */
stlStatus qlndCopyBlock( qllLongTypeValueList   * aLongTypeValueList,
                         knlValueBlockList     ** aValueBlockList,
                         stlLayerClass            aAllocLayer,
                         stlInt32                 aBlockCnt,
                         knlValueBlockList      * aSourceBlockList,
                         knlRegionMem           * aRegionMem,
                         qllEnv                 * aEnv )
{
    knlValueBlockList  * sValueBlock = NULL;

    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aLongTypeValueList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aValueBlockList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aSourceBlockList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK( aEnv ) );


    /**
     * Copy Value Block
     */

    STL_TRY( knlInitCopyBlock( aValueBlockList,
                               aAllocLayer,
                               aBlockCnt,
                               aSourceBlockList,
                               aRegionMem,
                               KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    sValueBlock = *aValueBlockList;
    
    if( ( KNL_GET_BLOCK_DB_TYPE( sValueBlock ) == DTL_TYPE_LONGVARCHAR ) ||
        ( KNL_GET_BLOCK_DB_TYPE( sValueBlock ) == DTL_TYPE_LONGVARBINARY ) )
    {
        if( KNL_GET_BLOCK_IS_SEP_BUFF( sValueBlock ) == STL_TRUE )
        {
            STL_TRY( qlndAddLongTypeValueList( aLongTypeValueList,
                                               KNL_GET_BLOCK_ALLOC_CNT( sValueBlock ),
                                               KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                               aRegionMem,
                                               aEnv )
                     == STL_SUCCESS );         
        }
        else
        {
            STL_TRY( qlndAddLongTypeValueList( aLongTypeValueList,
                                               1,
                                               KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                               aRegionMem,
                                               aEnv )
                     == STL_SUCCESS );         
        }
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
 * @brief Value Block List 를 생성한다.
 * @param[in,out]  aLongTypeValueList  long varchar 또는 long varbinary를 가지는 Data Value List
 * @param[out]     aValueBlockList     Value Block List
 * @param[in]      aBlockCnt           Block Read Count
 * @param[in]      aIsSepBuff          각 Value 마다 공간을 확보할 지의 여부          
 *                            <BR> - 컬럼인 경우, aBlockCnt 만큼 Data Buffer 공간을 확보
 *                            <BR> - 상수인 경우, aBlockCnt 와 관계없이 Data Buffer를 공유
 * @param[in]      aAllocLayer         Block이 할당되는 Layer
 * @param[in]      aTableID            Table ID (컬럼이 아닌경우, 무시됨)
 * @param[in]      aColumnOrder        컬럼의 Table 내 위치 (컬럼이 아닌경우, 무시됨)
 * @param[in]      aDBType             DB Data Type
 * @param[in]      aArgNum1            Precision (타입마다 용도가 다름)
 *                            <BR> - time,timetz,timestamp,timestamptz타입은
 *                            <BR>   fractional second precision 정보
                              <BR> - interval 타입은 leading precision 정보
                              <BR> - 그외 타입은 precision 정보
 * @param[in]      aArgNum2            scale (타입마다 용도가 다름)
 *                            <BR> - interval 타입은 fractional second precision 정보
                              <BR> - 그외 타입은 scale 정보 
 * @param[in]      aStringLengthUnit   string length unit (dtlStringLengthUnit 참조)
 * @param[in]      aIntervalIndicator  interval indicator (dtlIntervalIndicator 참조) 
 * @param[in]      aRegionMem          Region Memory
 * @param[in]      aEnv                Environment
 * @remarks
 */
stlStatus qlndInitBlockList( qllLongTypeValueList   * aLongTypeValueList,
                             knlValueBlockList     ** aValueBlockList,
                             stlInt32                 aBlockCnt,
                             stlBool                  aIsSepBuff,
                             stlLayerClass            aAllocLayer,
                             stlInt64                 aTableID,
                             stlInt32                 aColumnOrder,
                             dtlDataType              aDBType,
                             stlInt64                 aArgNum1,
                             stlInt64                 aArgNum2,
                             dtlStringLengthUnit      aStringLengthUnit,
                             dtlIntervalIndicator     aIntervalIndicator,
                             knlRegionMem           * aRegionMem,
                             qllEnv                 * aEnv )
{
    knlValueBlockList  * sValueBlock = NULL;


    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aLongTypeValueList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aValueBlockList != NULL, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aBlockCnt > 0, QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( ( aDBType >= 0 ) && ( aDBType < DTL_TYPE_MAX ),
                        QLL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK( aEnv ) );

    
    /**
     * Value Block List 생성
     */

    STL_TRY( knlInitBlock( aValueBlockList,
                           aBlockCnt,
                           aIsSepBuff,
                           aAllocLayer,
                           aTableID,
                           aColumnOrder,
                           aDBType,
                           aArgNum1,
                           aArgNum2,
                           aStringLengthUnit,
                           aIntervalIndicator,
                           aRegionMem,
                           KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    sValueBlock = *aValueBlockList;
    
    if( ( KNL_GET_BLOCK_DB_TYPE( sValueBlock ) == DTL_TYPE_LONGVARCHAR ) ||
        ( KNL_GET_BLOCK_DB_TYPE( sValueBlock ) == DTL_TYPE_LONGVARBINARY ) )
    {
        if( KNL_GET_BLOCK_IS_SEP_BUFF( sValueBlock ) == STL_TRUE )
        {
            STL_TRY( qlndAddLongTypeValueList( aLongTypeValueList,
                                               KNL_GET_BLOCK_ALLOC_CNT( sValueBlock ),
                                               KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                               aRegionMem,
                                               aEnv )
                     == STL_SUCCESS );         
        }
        else
        {
            STL_TRY( qlndAddLongTypeValueList( aLongTypeValueList,
                                               1,
                                               KNL_GET_BLOCK_FIRST_DATA_VALUE( sValueBlock ),
                                               aRegionMem,
                                               aEnv )
                     == STL_SUCCESS );         
        }
    }
    else
    {
        /* Do Nothing */
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnd */
