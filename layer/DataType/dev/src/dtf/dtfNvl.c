/*******************************************************************************
 * dtfNvl.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:$
 *        
 * NOTES
 * 
 ******************************************************************************/

/**
 * @file dtfNvl.c
 * @brief Nvl Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>

/*******************************************************************************
 *   Nvl
 *   Nvl ==> [ O ]
 * ex) nvl(    1, 999 )  ==>  1
 *     nvl( null, 999 )  ==>  999
 *******************************************************************************/

/* /\** */
/*  * @brief Nvl */
/*  *        <BR> Nvl( expr1, expr2 ) */
/*  *        <BR>   Nvl => [ O ] */
/*  *        <BR> ex) nvl(    1, 999 )  ==>  1 */
/*  *        <BR>     nvl( null, 999 )  ==>  999 */
/*  *         */
/*  * @param[in]  aInputArgumentCnt input argument count */
/*  * @param[in]  aInputArgument    input argument */
/*  * @param[out] aResultValue      연산 결과 */
/*  * @param[in]  aVectorFunc       Function Vector */
/*  * @param[in]  aVectorArg        Vector Argument */
/*  * @param[out] aEnv              environment (stlErrorStack) */
/*  *\/ */
/* stlStatus dtfNvl( stlUInt16        aInputArgumentCnt, */
/*                   dtlValueEntry  * aInputArgument, */
/*                   void           * aResultValue, */
/*                   dtlFuncVector  * aVectorFunc, */
/*                   void           * aVectorArg, */
/*                   void           * aEnv ) */
/* { */
/*     dtlDataValue  * sValue1 = aInputArgument[0].mValue.mDataValue; */
/*     dtlDataValue  * sValue2 = aInputArgument[1].mValue.mDataValue; */
/*     dtlDataValue  * sResultValue = (dtlDataValue *)aResultValue; */
    
/*     DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv ); */
    
/*     if( DTL_IS_NULL( sValue1 ) == STL_TRUE ) */
/*     { */
/*         stlMemcpy( sResultValue->mValue, */
/*                    sValue2->mValue, */
/*                    sValue2->mLength ); */
        
/*         sResultValue->mLength = sValue2->mLength; */
/*     } */
/*     else */
/*     { */
/*         stlMemcpy( sResultValue->mValue, */
/*                    sValue1->mValue, */
/*                    sValue1->mLength ); */
        
/*         sResultValue->mLength = sValue1->mLength; */
/*     } */
    
/*     return STL_SUCCESS; */
    
/*     STL_FINISH; */
    
/*     return STL_FAILURE; */
/* } */


/*******************************************************************************
 *   Nvl2
 *   Nvl2 ==> [ O ]
 * ex) nvl2(    1, 111, 999 )  ==>  111
 *     nvl2( null, 111, 999 )  ==>  999
 *******************************************************************************/

/* /\** */
/*  * @brief Nvl2 */
/*  *        <BR> Nvl2( expr1, expr2, expr3 ) */
/*  *        <BR>   Nvl2 => [ O ] */
/*  *        <BR> ex) nvl2(    1, 111, 999 )  ==>  111 */
/*  *        <BR>     nvl2( null, 111, 999 )  ==>  999 */
/*  *         */
/*  * @param[in]  aInputArgumentCnt input argument count */
/*  * @param[in]  aInputArgument    input argument */
/*  * @param[out] aResultValue      연산 결과 */
/*  * @param[in]  aVectorFunc       Function Vector */
/*  * @param[in]  aVectorArg        Vector Argument */
/*  * @param[out] aEnv              environment (stlErrorStack) */
/*  *\/ */
/* stlStatus dtfNvl2( stlUInt16        aInputArgumentCnt, */
/*                    dtlValueEntry  * aInputArgument, */
/*                    void           * aResultValue, */
/*                    dtlFuncVector  * aVectorFunc, */
/*                    void           * aVectorArg, */
/*                    void           * aEnv ) */
/* { */
/*     dtlDataValue  * sValue1 = aInputArgument[0].mValue.mDataValue; */
/*     dtlDataValue  * sValue2 = aInputArgument[1].mValue.mDataValue; */
/*     dtlDataValue  * sValue3 = aInputArgument[2].mValue.mDataValue; */
/*     dtlDataValue  * sResultValue = (dtlDataValue *)aResultValue; */
    
/*     DTL_PARAM_VALIDATE( aInputArgumentCnt == 3, (stlErrorStack *)aEnv ); */
    
/*     if( DTL_IS_NULL( sValue1 ) == STL_TRUE ) */
/*     { */
/*         stlMemcpy( sResultValue->mValue, */
/*                    sValue3->mValue, */
/*                    sValue3->mLength ); */
        
/*         sResultValue->mLength = sValue3->mLength; */
/*     } */
/*     else */
/*     { */
/*         stlMemcpy( sResultValue->mValue, */
/*                    sValue2->mValue, */
/*                    sValue2->mLength ); */
        
/*         sResultValue->mLength = sValue2->mLength; */
/*     } */
    
/*     return STL_SUCCESS; */
    
/*     STL_FINISH; */
    
/*     return STL_FAILURE; */
/* } */


/** @} */
