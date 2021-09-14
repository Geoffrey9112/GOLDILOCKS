/*******************************************************************************
 * qlneQuerySpec.c
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
 * @file qlneQuerySpec.c
 * @brief SQL Processor Layer Explain Node - QUERY SPEC
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlne
 * @{
 */


/**
 * @brief node에 대한 수행 정보를 출력한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area
 * @param[in]      aRegionMem    Region Memory Manager
 * @param[in]      aDepth        Node Depth
 * @param[in,out]  aPlanText     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 */
stlStatus qlneExplainQuerySpec( qllOptimizationNode   * aOptNode,
                                qllDataArea           * aDataArea,
                                knlRegionMem          * aRegionMem,
                                stlUInt32               aDepth,
                                qllExplainNodeText   ** aPlanText,
                                qllEnv                * aEnv )
{
    qlnoQuerySpec       * sOptQuerySpec     = NULL;

    
    /*
     * Parameter Validation
     */ 

    STL_PARAM_VALIDATE( aOptNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aOptNode->mType == QLL_PLAN_NODE_QUERY_SPEC_TYPE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDataArea != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDepth >= 0, QLL_ERROR_STACK(aEnv) );
    
    
    /**********************************************************
     * QUERY SPEC Node 정보 출력
     *********************************************************/

    sOptQuerySpec = (qlnoQuerySpec *) aOptNode->mOptNode;

    
    /**********************************************************
     * Child Node 정보 출력
     *********************************************************/

    /**
     * Child Node 정보 출력
     */

    STL_TRY( qlneExplainNodeList[ sOptQuerySpec->mChildNode->mType ] (
                 sOptQuerySpec->mChildNode,
                 aDataArea,
                 aRegionMem,
                 aDepth,
                 aPlanText,
                 aEnv )
             == STL_SUCCESS );
    

    /**********************************************************
     * Cost 정보 출력
     *********************************************************/

    /**
     * @todo Cost 정보 출력
     */
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} qlne */
