/*******************************************************************************
 * qlncNode.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
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
 * @file qlncNode.c
 * @brief SQL Processor Layer SELECT statement candidate optimization
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/*******************************************************
 * NORMAL FUNCTIONS
 *******************************************************/

/*******************************************************
 * NODE ARRAY FUNCTIONS
 *******************************************************/

/**
 * @brief Node Array를 초기화한다.
 * @param[in]   aRegionMem      Region Memory
 * @param[in]   aNodeArray      Node Array
 * @param[in]   aEnv            Environment
 */
stlStatus qlncInitializeNodeArray( knlRegionMem     * aRegionMem,
                                   qlncNodeArray    * aNodeArray,
                                   qllEnv           * aEnv )
{
    stlInt32      sSize;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNodeArray != NULL, QLL_ERROR_STACK(aEnv) );


    sSize = STL_SIZEOF( qlncNodeCommon* ) * QLNC_NODE_ARRAY_DEFAULT_COUNT;
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                sSize,
                                (void**) &(aNodeArray->mNodeArr),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    aNodeArray->mRegionMem = aRegionMem;
    aNodeArray->mCurCnt = 0;
    aNodeArray->mMaxCnt = QLNC_NODE_ARRAY_DEFAULT_COUNT;
    stlMemset( aNodeArray->mNodeArr, 0x00, sSize );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Node Array를 초기화한다.
 * @param[in]   aNodeArray      Node Array
 * @param[in]   aNode           Common Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncAddNodeToNodeArray( qlncNodeArray     * aNodeArray,
                                  qlncNodeCommon    * aNode,
                                  qllEnv            * aEnv )
{
    stlInt32              sSize;
    qlncNodeCommon     ** sNodeArr  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aNodeArray != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    if( aNodeArray->mCurCnt == aNodeArray->mMaxCnt )
    {
        /* Node Array를 확장 */
        sSize = STL_SIZEOF( qlncNodeCommon* ) *
            (aNodeArray->mCurCnt + QLNC_NODE_ARRAY_DEFAULT_COUNT);

        STL_TRY( knlAllocRegionMem( aNodeArray->mRegionMem,
                                    sSize,
                                    (void**) &sNodeArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sNodeArr,
                   aNodeArray->mNodeArr,
                   STL_SIZEOF( qlncNodeCommon* ) * aNodeArray->mCurCnt );
        stlMemset( &sNodeArr[aNodeArray->mCurCnt],
                   0x00,
                   STL_SIZEOF( qlncNodeCommon*) * QLNC_NODE_ARRAY_DEFAULT_COUNT );

        aNodeArray->mMaxCnt += QLNC_NODE_ARRAY_DEFAULT_COUNT;
        aNodeArray->mNodeArr = sNodeArr;
    }

    aNodeArray->mNodeArr[aNodeArray->mCurCnt] = aNode;
    aNodeArray->mCurCnt++;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************
 * TABLE MAP ARRAY FUNCTIONS
 *******************************************************/

/**
 * @brief Table Map Array를 초기화한다.
 * @param[in]   aParamInfo      Common Parameter Info
 * @param[out]  aTableMapArray  Table Map Array
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCreateTableMapArray( qlncParamInfo        * aParamInfo,
                                   qlncTableMapArray   ** aTableMapArray,
                                   qllEnv               * aEnv )
{
    qlncTableMapArray   * sTableMapArray    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncTableMapArray ),
                                (void**) &sTableMapArray,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncTableMap ) * QLNC_TABLE_MAP_ARRAY_DEFAULT_SIZE,
                                (void**) &(sTableMapArray->mArray),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTableMapArray->mRegionMem = &QLL_CANDIDATE_MEM( aEnv );
    sTableMapArray->mCurCount = 0;
    sTableMapArray->mMaxCount = QLNC_TABLE_MAP_ARRAY_DEFAULT_SIZE;

    /* Output 설정 */
    *aTableMapArray = sTableMapArray;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Table Map Array에 Table Map을 추가한다.
 * @param[in]   aTableMapArray  Table Map Array
 * @param[in]   aInitNode       Validation Node
 * @param[in]   aCandNode       Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncAddTableMapToTableMapArray( qlncTableMapArray    * aTableMapArray,
                                          qlvInitNode          * aInitNode,
                                          qlncNodeCommon       * aCandNode,
                                          qllEnv               * aEnv )
{
    qlncTableMap  * sArray = NULL;

    
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aTableMapArray != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );

    if( aTableMapArray->mCurCount < aTableMapArray->mMaxCount )
    {
        aTableMapArray->mArray[aTableMapArray->mCurCount].mInitNode = aInitNode;
        aTableMapArray->mArray[aTableMapArray->mCurCount].mCandNode = aCandNode;
        aTableMapArray->mCurCount++;
    }
    else
    {
        aTableMapArray->mMaxCount += QLNC_TABLE_MAP_ARRAY_INCREASE_SIZE;
        
        STL_TRY( knlAllocRegionMem( aTableMapArray->mRegionMem,
                                    STL_SIZEOF( qlncTableMap ) * aTableMapArray->mMaxCount,
                                    (void**) &sArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sArray,
                   aTableMapArray->mArray,
                   STL_SIZEOF( qlncTableMap ) * aTableMapArray->mCurCount );

        aTableMapArray->mArray = sArray;
        
        aTableMapArray->mArray[aTableMapArray->mCurCount].mInitNode = aInitNode;
        aTableMapArray->mArray[aTableMapArray->mCurCount].mCandNode = aCandNode;
        aTableMapArray->mCurCount++;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 2개의 Table Map Array를 Merge 한다.
 * @param[in]   aRegionMem          Region Memory
 * @param[in]   aSrcTableMapArr1    Source Table Map Array 1
 * @param[in]   aSrcTableMapArr2    Source Table Map Array 2
 * @param[out]  aMergedTableMapArr  Merged Table Map Array
 * @param[in]   aEnv                Environment
 */
stlStatus qlncMergeTableMapArray( knlRegionMem          * aRegionMem,
                                  qlncTableMapArray     * aSrcTableMapArr1,
                                  qlncTableMapArray     * aSrcTableMapArr2,
                                  qlncTableMapArray    ** aMergedTableMapArr,
                                  qllEnv                * aEnv )
{
    stlInt32              sArrCount;
    qlncTableMapArray   * sNewTableMapArray = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    STL_DASSERT( aSrcTableMapArr1 != NULL );
    if( aSrcTableMapArr2 == NULL )
    {
        /* Table Map Array 1만 복사 */
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlncTableMapArray ),
                                    (void**) &sNewTableMapArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlncTableMap ) * aSrcTableMapArr1->mCurCount,
                                    (void**) &(sNewTableMapArray->mArray),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sNewTableMapArray->mArray,
                   aSrcTableMapArr1->mArray,
                   STL_SIZEOF( qlncTableMap ) * aSrcTableMapArr1->mCurCount );

        sNewTableMapArray->mCurCount = aSrcTableMapArr1->mCurCount;
        sNewTableMapArray->mMaxCount = aSrcTableMapArr1->mCurCount;
    }
    else
    {
        /* Table Map Array 1과 Table Map Array 2 모두 복사 */
        sArrCount = aSrcTableMapArr1->mCurCount + aSrcTableMapArr2->mCurCount;

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlncTableMapArray ),
                                    (void**) &sNewTableMapArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlncTableMap ) * sArrCount,
                                    (void**) &(sNewTableMapArray->mArray),
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sNewTableMapArray->mArray,
                   aSrcTableMapArr1->mArray,
                   STL_SIZEOF( qlncTableMap ) * aSrcTableMapArr1->mCurCount );

        stlMemcpy( sNewTableMapArray->mArray + aSrcTableMapArr1->mCurCount,
                   aSrcTableMapArr2->mArray,
                   STL_SIZEOF( qlncTableMap ) * aSrcTableMapArr2->mCurCount );

        sNewTableMapArray->mCurCount = sArrCount;
        sNewTableMapArray->mMaxCount = sArrCount;
    }


    /**
     * Output 설정
     */

    *aMergedTableMapArr = sNewTableMapArray;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Table Map Array에서 Init Node에 대응하는 Cand Node를 찾는다.
 * @param[in]   aTableMapArray      Table Map Array
 * @param[in]   aInitNode           Validation Node
 */
qlncNodeCommon * qlncGetCandNodePtrFromInitNodePtr( qlncTableMapArray   * aTableMapArray,
                                                    qlvInitNode         * aInitNode )
{
    stlInt32          i;
    qlncNodeCommon  * sNode     = NULL;

    for( i = 0; i < aTableMapArray->mCurCount; i++ )
    {
        if( aTableMapArray->mArray[i].mInitNode == aInitNode )
        {
            sNode = aTableMapArray->mArray[i].mCandNode;
            break;
        }
    }


    return sNode;
}


/**
 * @brief Table Map Array에서 Cand Node가 존재하는지 판단한다.
 */
stlBool qlncIsExistCandNodeOnTableMap( qlncTableMapArray    * aTableMapArray,
                                       qlncNodeCommon       * aCandNode )
{
    stlInt32      i;


    STL_DASSERT( aTableMapArray != NULL );
    STL_DASSERT( aCandNode != NULL );


    for( i = 0; i < aTableMapArray->mCurCount; i++ )
    {
        if( aTableMapArray->mArray[i].mCandNode->mNodeID == aCandNode->mNodeID )
        {
            return STL_TRUE;
        }
    }

    return STL_FALSE;
}


/*******************************************************
 * QUERY BLOCK MAP ARRAY FUNCTIONS
 *******************************************************/

/**
 * @brief Query Block Map Array를 초기화한다.
 * @param[in]   aParamInfo      Common Parameter Info
 * @param[out]  aQBMapArray     Query Block Map Array
 * @param[in]   aEnv            Environment
 */
stlStatus qlncCreateQBMapArray( qlncParamInfo   * aParamInfo,
                                qlncQBMapArray ** aQBMapArray,
                                qllEnv          * aEnv )
{
    qlncQBMapArray  * sQBMapArray   = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aParamInfo != NULL, QLL_ERROR_STACK(aEnv) );


    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncQBMapArray ),
                                (void**) &sQBMapArray,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncQBMap ) * QLNC_QUERY_BLOCK_MAP_ARRAY_DEFAULT_SIZE,
                                (void**) &(sQBMapArray->mArray),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sQBMapArray->mRegionMem = &QLL_CANDIDATE_MEM( aEnv );
    sQBMapArray->mCurCount = 0;
    sQBMapArray->mMaxCount = QLNC_TABLE_MAP_ARRAY_DEFAULT_SIZE;

    /* Output 설정 */
    *aQBMapArray = sQBMapArray;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Block Map Array에 Table Map을 추가한다.
 * @param[in]   aQBMapArray     Query Block Map Array
 * @param[in]   aQBID           Query Block ID
 * @param[in]   aInitNode       Validation Node
 * @param[in]   aCandNode       Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncAddQBMapToQBMapArray( qlncQBMapArray  * aQBMapArray,
                                    stlInt32          aQBID,
                                    qlvInitNode     * aInitNode,
                                    qlncNodeCommon  * aCandNode,
                                    qllEnv          * aEnv )
{
    qlncQBMap   * sArray    = NULL;

    
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aQBMapArray != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aQBID >= 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInitNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );

    if( aQBMapArray->mCurCount < aQBMapArray->mMaxCount )
    {
        aQBMapArray->mArray[aQBMapArray->mCurCount].mQBID = aQBID;
        aQBMapArray->mArray[aQBMapArray->mCurCount].mInitNode = aInitNode;
        aQBMapArray->mArray[aQBMapArray->mCurCount].mCandNode = aCandNode;
        aQBMapArray->mCurCount++;
    }
    else
    {
        aQBMapArray->mMaxCount += QLNC_QUERY_BLOCK_MAP_ARRAY_INCREASE_SIZE;
        
        STL_TRY( knlAllocRegionMem( aQBMapArray->mRegionMem,
                                    STL_SIZEOF( qlncQBMap ) * aQBMapArray->mMaxCount,
                                    (void**) &sArray,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sArray,
                   aQBMapArray->mArray,
                   STL_SIZEOF( qlncQBMap ) * aQBMapArray->mCurCount );

        aQBMapArray->mArray = sArray;
        
        aQBMapArray->mArray[aQBMapArray->mCurCount].mQBID = aQBID;
        aQBMapArray->mArray[aQBMapArray->mCurCount].mInitNode = aInitNode;
        aQBMapArray->mArray[aQBMapArray->mCurCount].mCandNode = aCandNode;
        aQBMapArray->mCurCount++;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Block Map Array에서 Init Node와 동일한 것이 존재하는지 찾는다.
 * @param[in]   aQBMapArray     Query Block Map Array
 * @param[in]   aInitNode       Validation Node
 * @param[out]  aQBID           Query Block ID
 * @param[out]  aCandNode       Candidate Node
 */
stlBool qlncFindQueryBlockInQBMapArray( qlncQBMapArray  * aQBMapArray,
                                        qlvInitNode     * aInitNode,
                                        stlInt32        * aQBID,
                                        qlncNodeCommon ** aCandNode )
{
    stlInt32          i;
    stlInt32          sQBID;
    stlInt32          sResultQBID;
    qlncNodeCommon  * sResultCandNode   = NULL;


    STL_DASSERT( (aQBMapArray != NULL) &&
                 (aInitNode != NULL) );

    /* Result Value 초기화 */
    sResultQBID = -1;
    sResultCandNode = NULL;

    /* Query Block ID 설정 */
    if( aInitNode->mType == QLV_NODE_TYPE_QUERY_SPEC )
    {
        sQBID = ((qlvInitQuerySpecNode*)aInitNode)->mQBID;
    }
    else
    {
        STL_DASSERT( aInitNode->mType == QLV_NODE_TYPE_QUERY_SET );

        sQBID = ((qlvInitQuerySetNode*)aInitNode)->mQBID;
    }

    /* QBID와 Init Node Pointer를 이용하여 먼저 검사 */
    for( i = 0; i < aQBMapArray->mCurCount; i++ )
    {
        /* 동일한 QBID를 가진 Query Block이 존재하는지 판단 */
        if( aQBMapArray->mArray[i].mQBID == sQBID )
        {
            sResultQBID = sQBID;
            sResultCandNode = aQBMapArray->mArray[i].mCandNode;

            STL_THROW( RAMP_FINISH );
        }

        /* 동일한 Init Node Pointer를 가진다면
         * Query Block ID가 동일하여 위에서 체크가 됐다. */
        STL_DASSERT( aQBMapArray->mArray[i].mInitNode != aInitNode );
    }


    /**
     * @todo qlvInitNode에 대하여 동일한지 비교하는 과정 필요
     */


    STL_RAMP( RAMP_FINISH );


    *aQBID = sResultQBID;
    *aCandNode = sResultCandNode;

    if( sResultQBID >= 0 )
    {
        return STL_TRUE;
    }

    return STL_FALSE;
}


/** @} qlnc */
