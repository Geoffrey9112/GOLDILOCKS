/*******************************************************************************
 * qlncTrace.c
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
 * @file qlncTrace.c
 * @brief SQL Processor Layer Optimizer Trace Common Functions
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */

/**
 * @brief Tree 형태의 Trace에서 Prefix 문자
 */
const stlChar * qlncTraceTreePrefix[QLNC_TRACE_TREE_MAX_PREFIX_COUNT] =
{
    /* 00 */    "",
    /* 01 */    "  ",
    /* 02 */    "    ",
    /* 03 */    "      ",
    /* 04 */    "        ",
    /* 05 */    "          ",
    /* 06 */    "            ",
    /* 07 */    "              ",
    /* 08 */    "                ",
    /* 09 */    "                  ",
    /* 10 */    "                    ",
    /* 11 */    "                      ",
    /* 12 */    "                        ",
    /* 13 */    "                          ",
    /* 14 */    "                            ",
    /* 15 */    "                              ",
    /* 16 */    "                                ",
    /* 17 */    "                                  ",
    /* 18 */    "                                    ",
    /* 19 */    "                                      ",
    /* 20 */    "                                        ",
    /* 21 */    "                                          ",
    /* 22 */    "                                            ",
    /* 23 */    "                                              ",
    /* 24 */    "                                                ",
    /* 25 */    "                                                  ",
    /* 26 */    "                                                    ",
    /* 27 */    "                                                      ",
    /* 28 */    "                                                        ",
    /* 29 */    "                                                          ",
    /* 30 */    "                                                            ",
    /* 31 */    "                                                              "
};


const stlChar * qlncExprTypeName[QLV_EXPR_TYPE_MAX] =
{
    "VALUE",
    "BIND_PARAM",
    "COLUMN",
    "FUNCTION",
    "CAST",
    "PSEUDO_COLUMN",
    "PSEUDO_ARGUMENT",
    "CONSTANT_EXPR_RESULT",
    "REFERENCE",
    "SUB_QUERY",
    "INNER_COLUMN",
    "OUTER_COLUMN",
    "ROWID_COLUMN",
    "AGGREGATION",
    "CASE_EXPR",
    "LIST_FUNCTION",
    "LIST_COLUMN",
    "ROW_EXPR"
};

const stlChar * qlncIterationTimeName[DTL_ITERATION_TIME_MAX] =
{
    "AGGR",
    "EXPR",
    "QUERY",
    "STMT",
    "NONE"
};

const stlChar * qlncInitNodeTypeName[QLV_NODE_TYPE_MAX] =
{
    "QUERY_SET",
    "QUERY_SPEC",
    "BASE_TABLE",
    "SUB_TABLE",
    "JOIN_TABLE",
    "FLAT_INSTANT",
    "SORT_INSTANT",
    "HASH_INSTANT",
    "GROUP_HASH_INSTANT",
    "GROUP_SORT_INSTANT",
    "SORT_JOIN_INSTANT",
    "HASH_JOIN_INSTANT"
};

const stlChar * qlncJoinTypeName[QLV_JOIN_TYPE_MAX] =
{
    "CROSS",
    "CROSS",
    "INNER",
    "LEFT_OUTER",
    "RIGHT_OUTER",
    "FULL_OUTER",
    "LEFT_SEMI",
    "RIGHT_SEMI",
    "LEFT_ANTI_SEMI",
    "RIGHT_ANTI_SEMI",
    "LEFT_ANTI_SEMI_NA",
    "RIGHT_ANTI_SEMI_NA",
    "INVERTED_LEFT_SEMI",
    "INVERTED_RIGHT_SEMI"
};


/**
 * @brief Candidate Node로부터 Query 구문을 만들어 Trace Log에 출력한다.
 * @param[in]   aRegionMem  Region Memory
 * @param[in]   aCandNode   Candidate Node
 * @param[in]   aEnv        Environment
 */
stlStatus qlncTraceQueryFromCandNode( knlRegionMem      * aRegionMem,
                                      qlncNodeCommon    * aCandNode,
                                      qllEnv            * aEnv )
{
    knlRegionMark         sRegionMemMark;
    qlncStringBlock     * sStringBlock  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Region Memory Mark */
    KNL_INIT_REGION_MARK( &sRegionMemMark );
    STL_TRY( knlMarkRegionMem( aRegionMem,
                               &sRegionMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* String Block 생성 */
    STL_TRY( qlncTraceCreateStringBlock( aRegionMem,
                                         &sStringBlock,
                                         aEnv )
             == STL_SUCCESS );

    /* Cand Node로부터 Query String 생성 */
    STL_TRY( qlncTraceQueryFromCandNodeFuncList[ aCandNode->mNodeType ]( sStringBlock,
                                                                         aCandNode,
                                                                         aEnv )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock( sStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    /* Query String을 Trace Log에 기록 */
    (void)qllTraceString( sStringBlock->mString, aEnv );

    /* Region Memory UnMark */
    STL_TRY( knlFreeUnmarkedRegionMem( aRegionMem,
                                       &sRegionMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Candidate Node로부터 Tree구조의 구문을 만들어 Trace Log에 출력한다.
 * @param[in]   aRegionMem  Region Memory
 * @param[in]   aCandNode   Candidate Node
 * @param[in]   aEnv        Environment
 */
stlStatus qlncTraceTreeFromCandNode( knlRegionMem   * aRegionMem,
                                     qlncNodeCommon * aCandNode,
                                     qllEnv         * aEnv )
{
    knlRegionMark                 sRegionMemMark;
    qlncStringBlock             * sStringBlock  = NULL;
    qlncTraceSubQueryExprList     sSubQueryExprList;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Region Memory Mark */
    KNL_INIT_REGION_MARK( &sRegionMemMark );
    STL_TRY( knlMarkRegionMem( aRegionMem,
                               &sRegionMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* String Block 생성 */
    STL_TRY( qlncTraceCreateStringBlock( aRegionMem,
                                         &sStringBlock,
                                         aEnv )
             == STL_SUCCESS );

    /* SubQuery Expression List 초기화 */
    sSubQueryExprList.mRegionMem = aRegionMem;
    sSubQueryExprList.mSubQueryID = 0;
    sSubQueryExprList.mCount = 0;
    sSubQueryExprList.mHead = NULL;
    sSubQueryExprList.mTail = NULL;

    /* Cand Node로부터 Tree String 생성 */
    STL_TRY( qlncTraceTreeFromCandNodeFuncList[ aCandNode->mNodeType ]( sStringBlock,
                                                                        aCandNode,
                                                                        &sSubQueryExprList,
                                                                        0,
                                                                        aEnv )
             == STL_SUCCESS );

    /* Query String을 Trace Log에 기록 */
    (void)qllTraceString( "***** Main Query *****\n", aEnv );
    (void)qllTraceString( sStringBlock->mString, aEnv );

    /* SubQuery Expression List 처리 */
    if( sSubQueryExprList.mCount > 0 )
    {
        STL_TRY( qlncTraceTreeFromSubQueryExpr( aRegionMem,
                                                &sSubQueryExprList,
                                                1,
                                                aEnv )
                 == STL_SUCCESS );
    }

    /* Region Memory UnMark */
    STL_TRY( knlFreeUnmarkedRegionMem( aRegionMem,
                                       &sRegionMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SubQuery Expression List로부터 Tree구조의 구문을 만들어 Trace Log에 출력한다.
 * @param[in]   aRegionMem          Region Memory
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromSubQueryExpr( knlRegionMem               * aRegionMem,
                                         qlncTraceSubQueryExprList  * aSubQueryExprList,
                                         stlInt32                     aLevel,
                                         qllEnv                     * aEnv )
{
    stlInt32                      sSubQueryID;
    knlRegionMark                 sRegionMemMark;
    knlRegionMark                 sRegionMemMark2;
    qlncStringBlock             * sStringBlock      = NULL;
    qlncTraceSubQueryExprList     sSubQueryExprList;
    qlncTraceSubQueryExprItem   * sSubQueryExprItem = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubQueryExprList != NULL, QLL_ERROR_STACK(aEnv) );


    /* Region Memory Mark */
    KNL_INIT_REGION_MARK( &sRegionMemMark );
    STL_TRY( knlMarkRegionMem( aRegionMem,
                               &sRegionMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* String Block 생성 */
    STL_TRY( qlncTraceCreateStringBlock( aRegionMem,
                                         &sStringBlock,
                                         aEnv )
             == STL_SUCCESS );

    /* SubQueryID 설정 */
    sSubQueryID = aSubQueryExprList->mSubQueryID;

    /* SubQuery 처리 */
    sSubQueryExprItem = aSubQueryExprList->mHead;
    while( sSubQueryExprItem != NULL )
    {
        /* SubQuery Expression List 초기화 */
        sSubQueryExprList.mRegionMem = aRegionMem;
        sSubQueryExprList.mSubQueryID = sSubQueryID;
        sSubQueryExprList.mCount = 0;
        sSubQueryExprList.mHead = NULL;
        sSubQueryExprList.mTail = NULL;

        /* String Block 초기화 */
        QLNC_TRACE_RESET_STRING_BLOCK( sStringBlock );

        /* Region Memory Mark */
        KNL_INIT_REGION_MARK( &sRegionMemMark2 );
        STL_TRY( knlMarkRegionMem( aRegionMem,
                                   &sRegionMemMark2,
                                   KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        /* Cand Node로부터 Query String 생성 */
        STL_TRY( qlncTraceTreeFromCandNodeFuncList[ sSubQueryExprItem->mSubQueryExpr->mNode->mNodeType ](
                     sStringBlock,
                     sSubQueryExprItem->mSubQueryExpr->mNode,
                     &sSubQueryExprList,
                     aLevel,
                     aEnv )
                 == STL_SUCCESS );

        /* Query String을 Trace Log에 기록 */
        (void)qllTraceBody( aRegionMem,
                            aEnv,
                            "%s***** Sub Query (%s) *****\n",
                            qlncTraceTreePrefix[aLevel],
                            sSubQueryExprItem->mSubQueryName );
        (void)qllTraceString( sStringBlock->mString, aEnv );

        /* SubQueryID 갱신 */
        sSubQueryID = sSubQueryExprList.mSubQueryID;

        /* SubQuery Expression List 처리 */
        if( sSubQueryExprList.mCount > 0 )
        {
            STL_TRY( qlncTraceTreeFromSubQueryExpr( aRegionMem,
                                                    &sSubQueryExprList,
                                                    aLevel + 1,
                                                    aEnv )
                     == STL_SUCCESS );
        }

        /* Region Memory UnMark */
        STL_TRY( knlFreeUnmarkedRegionMem( aRegionMem,
                                           &sRegionMemMark2,
                                           STL_FALSE, /* aFreeChunk */
                                           KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sSubQueryExprItem = sSubQueryExprItem->mNext;
    }

    /* Region Memory UnMark */
    STL_TRY( knlFreeUnmarkedRegionMem( aRegionMem,
                                       &sRegionMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief String Block을 생성한다.
 * @param[in]   aRegionMem      Region Memory
 * @param[out]  aStringBlock    String Block
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceCreateStringBlock( knlRegionMem      * aRegionMem,
                                      qlncStringBlock  ** aStringBlock,
                                      qllEnv            * aEnv )
{
    stlChar             * sString       = NULL;
    qlncStringBlock     * sStringBlock  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /* String Block 할당 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlncStringBlock ),
                                (void**) &sStringBlock,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* String 할당 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( stlChar ) * QLNC_TRACE_DEFAULT_STRING_BLOCK_SIZE,
                                (void**) &sString,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    sString[0] = '\0';

    /* String Block 정보 설정 */
    sStringBlock->mRegionMem = aRegionMem;
    sStringBlock->mString = sString;
    sStringBlock->mMaxSize = QLNC_TRACE_DEFAULT_STRING_BLOCK_SIZE;
    sStringBlock->mCurSize = 0;


    /**
     * Output 설정
     */

    *aStringBlock = sStringBlock;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief String Block에 String을 추가한다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aEnv            Environment
 * @param[in]   aFormat         Format
 * @param[in]   ...             Output Parameter
 */
stlStatus qlncTraceAppendStringToStringBlock( qlncStringBlock   * aStringBlock,
                                              qllEnv            * aEnv,
                                              const stlChar     * aFormat,
                                              ... )
{
    stlInt32      sStrLen;
    stlInt32      sBlkCnt;
    stlVarList    sVarArgList;
    stlChar     * sNewString    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFormat != NULL, QLL_ERROR_STACK(aEnv) );


    /* String Size 설정 */
    stlVarStart( sVarArgList, aFormat );
    sStrLen = stlGetVprintfBufferSize( aFormat, sVarArgList );
    stlVarEnd( sVarArgList );

    /* Size 확인 */
    if( (aStringBlock->mCurSize + sStrLen + 1/* 종료문자 */) > aStringBlock->mMaxSize )
    {
        /* String의 공간을 늘린다. */
        sBlkCnt = (sStrLen / QLNC_TRACE_INCREASE_STRING_BLOCK_SIZE) + 1;
        STL_TRY( knlAllocRegionMem( aStringBlock->mRegionMem,
                                    STL_SIZEOF( stlChar ) * (aStringBlock->mMaxSize + (QLNC_TRACE_INCREASE_STRING_BLOCK_SIZE * sBlkCnt)),
                                    (void**) &sNewString,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sNewString, aStringBlock->mString, STL_SIZEOF( stlChar ) * aStringBlock->mCurSize );

        aStringBlock->mString = sNewString;
        aStringBlock->mMaxSize += (QLNC_TRACE_INCREASE_STRING_BLOCK_SIZE * sBlkCnt);
    }

    /* Append String */
    stlVarStart( sVarArgList, aFormat );
    stlVsnprintf( aStringBlock->mString + aStringBlock->mCurSize,
                  sStrLen + 1,
                  aFormat,
                  sVarArgList );
    stlVarEnd( sVarArgList );

    aStringBlock->mCurSize += sStrLen;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


////////////////////////////////////////////////////////////////////////


/**
 * @brief All Expression List를 출력한다.
 * @param[in]   aRegionMem      Region Memory
 * @param[in]   aAllExprList    All Expression List
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceAllExprList( knlRegionMem    * aRegionMem,
                                qlvRefExprList  * aAllExprList,
                                qllEnv          * aEnv )
{
    knlRegionMark         sRegionMemMark;
    qlncStringBlock     * sStringBlock  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAllExprList != NULL, QLL_ERROR_STACK(aEnv) );


    /* Region Memory Mark */
    KNL_INIT_REGION_MARK( &sRegionMemMark );
    STL_TRY( knlMarkRegionMem( aRegionMem,
                               &sRegionMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* String Block 생성 */
    STL_TRY( qlncTraceCreateStringBlock( aRegionMem,
                                         &sStringBlock,
                                         aEnv )
             == STL_SUCCESS );

    /* All Expression List 수행 */
    STL_TRY( qlncTraceAllExprList_Internal( sStringBlock,
                                            aAllExprList,
                                            aEnv )
             == STL_SUCCESS );

    /* Query String을 Trace Log에 기록 */
    (void)qllTraceString( "***** All Expression List *****\n", aEnv );
    (void)qllTraceString( sStringBlock->mString, aEnv );
    (void)qllTraceString( "\n", aEnv );

    /* Region Memory UnMark */
    STL_TRY( knlFreeUnmarkedRegionMem( aRegionMem,
                                       &sRegionMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Code Plan을 Tree 형태로 출력한다.
 * @param[in]   aRegionMem  Region Memory
 * @param[in]   aCodePlan   Code Plan
 * @param[in]   aEnv        Environment
 */
stlStatus qlncTraceTreeFromCodePlan( knlRegionMem           * aRegionMem,
                                     qllOptimizationNode    * aCodePlan,
                                     qllEnv                 * aEnv )
{
    knlRegionMark         sRegionMemMark;
    qlncStringBlock     * sStringBlock  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCodePlan != NULL, QLL_ERROR_STACK(aEnv) );


    /* Region Memory Mark */
    KNL_INIT_REGION_MARK( &sRegionMemMark );
    STL_TRY( knlMarkRegionMem( aRegionMem,
                               &sRegionMemMark,
                               KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* String Block 생성 */
    STL_TRY( qlncTraceCreateStringBlock( aRegionMem,
                                         &sStringBlock,
                                         aEnv )
             == STL_SUCCESS );

    /* Code Plan 수행 */
    STL_TRY( qlncTraceTreeFromCodePlanFuncList[ aCodePlan->mType ](
                 sStringBlock,
                 aCodePlan,
                 0,
                 aEnv )
             == STL_SUCCESS );

    /* Query String을 Trace Log에 기록 */
    (void)qllTraceString( "***** Code Plan *****\n", aEnv );
    (void)qllTraceString( sStringBlock->mString, aEnv );
    (void)qllTraceString( "\n", aEnv );

    /* Region Memory UnMark */
    STL_TRY( knlFreeUnmarkedRegionMem( aRegionMem,
                                       &sRegionMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV(aEnv) )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Explain Plan형태의 Trace Log를 출력한다.
 * @param[in]   aPlanText   Explain Plan Text
 * @param[in]   aEnv        Environment
 */
stlStatus qlncTraceExplainPlan( qllExplainNodeText  * aPlanText,
                                qllEnv              * aEnv )
{
    qllExplainNodeText  * sNodeText = NULL;
    qllExplainPredText  * sPredText = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aPlanText != NULL, QLL_ERROR_STACK(aEnv) );


    /* Node Text 출력 */
    sNodeText = aPlanText;
    while( sNodeText != NULL )
    {
        (void)qllTraceString( sNodeText->mLineText, aEnv );

        sNodeText = sNodeText->mNext;
    }

    /* Predicate Text 출력 */
    sNodeText = aPlanText;
    while( sNodeText != NULL )
    {
        sPredText = sNodeText->mPredText;
        while( sPredText != NULL )
        {
            (void)qllTraceString( sPredText->mLineText, aEnv );

            sPredText = sPredText->mNext;
        }

        sNodeText = sNodeText->mNext;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */
