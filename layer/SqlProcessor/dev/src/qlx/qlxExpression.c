/*******************************************************************************
 * qlxExpression.c
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
 * @file qlxExpression.c
 * @brief SQL Processor Layer EXPRESSION statement execution
 */

#include <qll.h>

#include <qlDef.h>


/**
 * @addtogroup qlx
 * @{
 */

/** @} qlx */


/**
 * @addtogroup qlxPseudo
 * @{
 */


/**
 * @brief Iteration Time 에 부합하는 Pseudo Column Block 을 수행한다.
 * @param[in] aTransID         Transaction ID
 * @param[in] aStmt            Statement
 * @param[in] aIterationTime   Iteration Time
 * @param[in] aTableHandle     Table Dictionary Handle
 * @param[in] aSmlRowBlock     smlRowBlock
 * @param[in] aPseudoColList   Pseudo Column List
 * @param[in] aPseudoBlockList Pseduo Column 의 실행결과를 담을 공간
 * @param[in] aStartBlockIdx   Start Block Index
 * @param[in] aBlockCnt        수행할 Value Block Data 개수
 * @param[in] aEnv             Environment
 * @remarks
 */
stlStatus qlxExecutePseudoColumnBlock( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       dtlIterationTime    aIterationTime,
                                       ellDictHandle     * aTableHandle,
                                       smlRowBlock       * aSmlRowBlock,
                                       qlvRefExprList    * aPseudoColList,
                                       knlValueBlockList * aPseudoBlockList,
                                       stlInt32            aStartBlockIdx,
                                       stlInt32            aBlockCnt,
                                       qllEnv            * aEnv )
{
    stlInt32   sLoop = 0;
    stlInt32   sPseudoBlockIdx = 0;
    stlInt32   sLastBlockIdx = aBlockCnt + aStartBlockIdx;

    qlvRefExprItem     * sPseudoInfo = NULL;
    qlvInitPseudoCol   * sPseudoCode = NULL;
    knlValueBlockList  * sValueBlock = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );


    /**
     * Iteration Time 에 부합하는 Pseudo Column 수행
     */

    if( ( aPseudoColList != NULL ) && ( aBlockCnt > 0 ) )
    {
        sPseudoInfo = aPseudoColList->mHead;
        sValueBlock = aPseudoBlockList;
        
        for( sLoop = 0 ; sLoop < aPseudoColList->mCount ; sLoop++ )
        {
            sPseudoCode = sPseudoInfo->mExprPtr->mExpr.mPseudoCol;

            if ( gPseudoColInfoArr[ sPseudoCode->mPseudoId ].mIterationTime
                 == aIterationTime )
            {
                /**
                 * Iteration Time 에 부합하는 경우 Pseudo Column 수행
                 */

                switch( aIterationTime )
                {
                    case DTL_ITERATION_TIME_FOR_EACH_EXPR :
                        {
                            STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF( sValueBlock ) == STL_TRUE );
                            
                            for( sPseudoBlockIdx = aStartBlockIdx ;
                                 sPseudoBlockIdx < sLastBlockIdx ;
                                 sPseudoBlockIdx++ )
                            {
                                STL_TRY( gPseudoColInfoArr[ sPseudoCode->mPseudoId ].mFuncPtr(
                                             aTransID,
                                             aStmt,
                                             ( aSmlRowBlock == NULL ) ? NULL : &(SML_GET_RID_VALUE(aSmlRowBlock, sPseudoBlockIdx)),
                                             ( aTableHandle == NULL ) ? KNL_ANONYMOUS_TABLE_ID : ellGetTableID( aTableHandle ),
                                             sPseudoCode->mArgCount,
                                             sPseudoCode->mArgs,
                                             KNL_GET_BLOCK_DATA_VALUE( sValueBlock, sPseudoBlockIdx ),
                                             ELL_ENV( aEnv ) )
                                         == STL_SUCCESS );
                            }

                            break;
                        }
 
                    case DTL_ITERATION_TIME_FOR_EACH_STATEMENT :
                        {
                            STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF( sValueBlock ) == STL_FALSE );

                            STL_TRY( gPseudoColInfoArr[ sPseudoCode->mPseudoId ].mFuncPtr(
                                         aTransID,
                                         aStmt,
                                         ( aSmlRowBlock == NULL ) ? NULL : &(SML_GET_RID_VALUE(aSmlRowBlock, sPseudoBlockIdx)),
                                         ( aTableHandle == NULL ) ? KNL_ANONYMOUS_TABLE_ID : ellGetTableID( aTableHandle ),
                                         sPseudoCode->mArgCount,
                                         sPseudoCode->mArgs,
                                         KNL_GET_BLOCK_DATA_VALUE( sValueBlock, 0 ),
                                         ELL_ENV( aEnv ) )
                                     == STL_SUCCESS );
                            
                            break;
                        }
                        
                    /* case DTL_ITERATION_TIME_NONE : */
                        /* DTL_ITERATION_TIME_NONE을 Iteration Time으로 갖는 Pseudo Column 은 없다. */
                    default :
                        {
                            STL_DASSERT( 0 );
                            break;
                        }
                }
            }
            else
            {
                /* 부합하지 않는 Iteration Time 임 */
            }

            sPseudoInfo = sPseudoInfo->mNext;
            sValueBlock = sValueBlock->mNext;
        }
    }
    else
    {
        /* nothing to do */
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief RowId Pseudo Column Block 을 수행한다.
 * @param[in] aTransID           Transaction ID
 * @param[in] aStmt              Statement
 * @param[in] aTableHandle       Table Dictionary Handle
 * @param[in] aSmlRowBlock       smlRowBlock
 * @param[in] aRowIdColumnExpr   RowId Pseudo Column Expr
 * @param[in] aRowIdColBlockList Pseduo Column 의 실행결과를 담을 공간
 * @param[in] aStartBlockIdx     Start Block Index
 * @param[in] aBlockCnt          수행할 Value Block Data 개수
 * @param[in] aEnv               Environment
 * @remarks
 */
stlStatus qlxExecuteRowIdColumnBlock( smlTransId          aTransID,
                                      smlStatement      * aStmt,
                                      ellDictHandle     * aTableHandle,
                                      smlRowBlock       * aSmlRowBlock,
                                      qlvInitExpression * aRowIdColumnExpr,
                                      knlValueBlockList * aRowIdColBlockList,
                                      stlInt32            aStartBlockIdx,
                                      stlInt32            aBlockCnt,
                                      qllEnv            * aEnv )
{
    stlInt32   sBlockIdx     = 0;
    stlInt32   sLastBlockIdx = aBlockCnt + aStartBlockIdx;
    stlInt64   sTableID;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * 실행
     */

    sTableID = ellGetTableID( aTableHandle );
    
    for( sBlockIdx = aStartBlockIdx ;
         sBlockIdx < sLastBlockIdx ;
         sBlockIdx++ )
    {
        STL_TRY( gPseudoColInfoArr[ KNL_PSEUDO_COL_ROWID ].mFuncPtr(
                     aTransID,
                     aStmt,
                     &(SML_GET_RID_VALUE(aSmlRowBlock, sBlockIdx)),
                     sTableID,
                     0,
                     NULL,
                     KNL_GET_BLOCK_DATA_VALUE( aRowIdColBlockList, sBlockIdx ),
                     ELL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlxPseudo */


