/*******************************************************************************
 * qlncTraceAllExprList.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlncTraceAllExprList.c 9543 2013-09-05 23:37:24Z bsyou $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file qlncTraceAllExprList.c
 * @brief SQL Processor Layer Optimizer Trace All Expression List
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief All Expression List를 출력한다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aAllExprList    All Expression List
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceAllExprList_Internal( qlncStringBlock    * aStringBlock,
                                         qlvRefExprList     * aAllExprList,
                                         qllEnv             * aEnv )
{
    stlInt32              sSeq;
    qlvInitExpression   * sExpr         = NULL;
    qlvRefExprItem      * sRefExprItem  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAllExprList != NULL, QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%s%s",
                 "  SEQ          TYPE           ITER_TIME   IS SET  OFFSET    POINTER\n",
                 "-----------------------------------------------------------------------\n" )
             == STL_SUCCESS );

    sSeq = 1;
    sRefExprItem = aAllExprList->mHead;
    while( sRefExprItem != NULL )
    {
        sExpr = sRefExprItem->mExprPtr;

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "  %3d  %21s    %6s       %c      %3d    0x%p\n",
                     sSeq, /* Seq */
                     qlncExprTypeName[sExpr->mType], /* Type */
                     qlncIterationTimeName[sExpr->mIterationTime], /* Iteration Time */
                     (sExpr->mIsSetOffset == STL_TRUE ? 'Y' : 'N' ), /* Is Set */
                     sExpr->mOffset, /* Offset */
                     sExpr) /* Pointer */
                 == STL_SUCCESS );

        sSeq++;
        sRefExprItem = sRefExprItem->mNext;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */
