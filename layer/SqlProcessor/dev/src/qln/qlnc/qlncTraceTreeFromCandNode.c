/*******************************************************************************
 * qlncTraceTreeFromCandNode.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlncTraceTreeFromCandNode.c 9742 2013-10-01 01:45:14Z bsyou $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file qlncTraceTreeFromCandNode.c
 * @brief SQL Processor Layer Optimizer Trace Tree From Candidate Node
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Trace Tree From Candidate Node Function List
 */
const qlncTraceTreeFromCandNodeFuncPtr qlncTraceTreeFromCandNodeFuncList[ QLNC_NODE_TYPE_MAX ] =
{
    qlncTraceTreeFromCandNodeQuerySet,          /**< QLNC_NODE_TYPE_QUERY_SET */
    qlncTraceTreeFromCandNodeSetOP,             /**< QLNC_NODE_TYPE_SET_OP */
    qlncTraceTreeFromCandNodeQuerySpec,         /**< QLNC_NODE_TYPE_QUERY_SPEC */
    qlncTraceTreeFromCandNodeBaseTable,         /**< QLNC_NODE_TYPE_BASE_TABLE */
    qlncTraceTreeFromCandNodeSubTable,          /**< QLNC_NODE_TYPE_SUB_TABLE */
    qlncTraceTreeFromCandNodeJoinTable,         /**< QLNC_NODE_TYPE_JOIN_TABLE */
    qlncTraceTreeFromCandNodeFlatInstant,       /**< QLNC_NODE_TYPE_FLAT_INSTANT */
    qlncTraceTreeFromCandNodeSortInstant,       /**< QLNC_NODE_TYPE_SORT_INSTANT */
    qlncTraceTreeFromCandNodeHashInstant,       /**< QLNC_NODE_TYPE_HASH_INSTANT */
    NULL,                                       /**< QLNC_NODE_TYPE_CREATE_NODE_MAX */
    qlncTraceTreeFromCandNodeInnerJoinTable,    /**< QLNC_NODE_TYPE_INNER_JOIN_TABLE */
    qlncTraceTreeFromCandNodeOuterJoinTable,    /**< QLNC_NODE_TYPE_OUTER_JOIN_TABLE */
    qlncTraceTreeFromCandNodeSemiJoinTable,     /**< QLNC_NODE_TYPE_SEMI_JOIN_TABLE */
    qlncTraceTreeFromCandNodeAntiSemiJoinTable  /**< QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE */
};


/**
 * @brief Query Set Candidate Node로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandNode           Candidate Node
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandNodeQuerySet( qlncStringBlock            * aStringBlock,
                                             qlncNodeCommon             * aCandNode,
                                             qlncTraceSubQueryExprList  * aSubQueryExprList,
                                             stlInt32                     aLevel,
                                             qllEnv                     * aEnv )
{
    qlncQuerySet    * sCandQuerySet     = NULL;
    qlncSetOP       * sCandSetOP        = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_QUERY_SET, QLL_ERROR_STACK(aEnv) );


    /* Query Set Candidate Node 설정 */
    sCandQuerySet = (qlncQuerySet*)aCandNode;

    /* Find Child Set OP */
    if( sCandQuerySet->mChildNode->mNodeType == QLNC_NODE_TYPE_SET_OP )
    {
        sCandSetOP = (qlncSetOP*)(sCandQuerySet->mChildNode);
    }
    else
    {
        STL_DASSERT( sCandQuerySet->mChildNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT );
        STL_DASSERT( ((qlncSortInstantNode*)(sCandQuerySet->mChildNode))->mChildNode->mNodeType == QLNC_NODE_TYPE_SET_OP );
        sCandSetOP = (qlncSetOP*)(((qlncSortInstantNode*)(sCandQuerySet->mChildNode))->mChildNode);
    }

    if( sCandSetOP->mSetType == QLNC_SET_TYPE_UNION )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s[UNION ",
                                                     qlncTraceTreePrefix[aLevel] )
                 == STL_SUCCESS );
    }
    else if( sCandSetOP->mSetType == QLNC_SET_TYPE_EXCEPT )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s[EXCEPT ",
                                                     qlncTraceTreePrefix[aLevel] )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( sCandSetOP->mSetType == QLNC_SET_TYPE_INTERSECT );
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s[INTERSECT ",
                                                     qlncTraceTreePrefix[aLevel] )
                 == STL_SUCCESS );
    }

    if( sCandSetOP->mSetOption == QLNC_SET_OPTION_ALL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "ALL] " )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( sCandSetOP->mSetOption == QLNC_SET_OPTION_DISTINCT );
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "DISTINCT] " )
                 == STL_SUCCESS );
    }

    /* offset */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "(OFFSET: " )
             == STL_SUCCESS );

    if( sCandQuerySet->mOffset == NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "N/A" )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandQuerySet->mOffset->mType ](
                     aStringBlock,
                     sCandQuerySet->mOffset,
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* limit */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 ", LIMIT: " )
             == STL_SUCCESS );

    if( sCandQuerySet->mLimit == NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "N/A" )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandQuerySet->mLimit->mType ](
                     aStringBlock,
                     sCandQuerySet->mLimit,
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );
    }

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 ")\n" )
             == STL_SUCCESS );

    /* Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCandNodeFuncList[ sCandQuerySet->mChildNode->mNodeType ](
                 aStringBlock,
                 sCandQuerySet->mChildNode,
                 aSubQueryExprList,
                 aLevel + 1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Set OP Candidate Node로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandNode           Candidate Node
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandNodeSetOP( qlncStringBlock           * aStringBlock,
                                          qlncNodeCommon            * aCandNode,
                                          qlncTraceSubQueryExprList * aSubQueryExprList,
                                          stlInt32                    aLevel,
                                          qllEnv                    * aEnv )
{
    stlInt32      i;
    qlncSetOP   * sCandSetOP    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_SET_OP, QLL_ERROR_STACK(aEnv) );


     /* Set OP Candidate Node 설정 */
    sCandSetOP = (qlncSetOP*)aCandNode;

    for( i = 0; i < sCandSetOP->mChildCount; i++ )
    {
        STL_TRY( qlncTraceTreeFromCandNodeFuncList[ sCandSetOP->mChildNodeArr[i]->mNodeType ](
                     aStringBlock,
                     sCandSetOP->mChildNodeArr[i],
                     aSubQueryExprList,
                     aLevel,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Query Spec Candidate Node로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandNode           Candidate Node
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandNodeQuerySpec( qlncStringBlock           * aStringBlock,
                                              qlncNodeCommon            * aCandNode,
                                              qlncTraceSubQueryExprList * aSubQueryExprList,
                                              stlInt32                    aLevel,
                                              qllEnv                    * aEnv )
{
    stlInt32              i;
    qlncQuerySpec       * sCandQuerySpec    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_QUERY_SPEC, QLL_ERROR_STACK(aEnv) );


    /* Query Spec Candidate Node 설정 */
    sCandQuerySpec = (qlncQuerySpec*)aCandNode;

    /* Node Name */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%s[QUERY SPEC] ",
                                                 qlncTraceTreePrefix[aLevel] )
             == STL_SUCCESS );

    /* offset */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "(OFFSET: " )
             == STL_SUCCESS );

    if( sCandQuerySpec->mOffset == NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "N/A" )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandQuerySpec->mOffset->mType ](
                     aStringBlock,
                     sCandQuerySpec->mOffset,
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* limit */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 ", LIMIT: " )
             == STL_SUCCESS );

    if( sCandQuerySpec->mLimit == NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "N/A" )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandQuerySpec->mLimit->mType ](
                     aStringBlock,
                     sCandQuerySpec->mLimit,
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );
    }

    /* 줄바꿈 */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 ")\n" )
             == STL_SUCCESS );

    /* Target */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%sTARGET: ",
                                                 qlncTraceTreePrefix[aLevel+1] )
             == STL_SUCCESS );

    for( i = 0; i < sCandQuerySpec->mTargetCount; i++ )
    {
        /* Expression 출력 */
        STL_TRY( qlncTraceTreeFromCandExprFuncList[ sCandQuerySpec->mTargetArr[i].mExpr->mType ](
                     aStringBlock,
                     sCandQuerySpec->mTargetArr[i].mExpr,
                     aSubQueryExprList,
                     aEnv )
                 == STL_SUCCESS );

        /* Target의 Alias Name 출력 */
        if( sCandQuerySpec->mTargetArr[i].mDisplayName != NULL )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         " AS " )
                     == STL_SUCCESS );

            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s",
                                                         sCandQuerySpec->mTargetArr[i].mDisplayName )
                     == STL_SUCCESS );
        }

        if( (i + 1) < sCandQuerySpec->mTargetCount )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         ", " )
                     == STL_SUCCESS );
        }
    }

    /* 줄바꿈 */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    /* Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCandNodeFuncList[ sCandQuerySpec->mChildNode->mNodeType ](
                 aStringBlock,
                 sCandQuerySpec->mChildNode,
                 aSubQueryExprList,
                 aLevel + 1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Base Table Candidate Node로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandNode           Candidate Node
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandNodeBaseTable( qlncStringBlock           * aStringBlock,
                                              qlncNodeCommon            * aCandNode,
                                              qlncTraceSubQueryExprList * aSubQueryExprList,
                                              stlInt32                    aLevel,
                                              qllEnv                    * aEnv )
{
    qlncBaseTableNode   * sCandBaseTable    = NULL;
    qlncRelationName    * sRelationName     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE, QLL_ERROR_STACK(aEnv) );


    /* Candidate Base Table Node 설정 */
    sCandBaseTable = (qlncBaseTableNode*)aCandNode;

    /* Node Name */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%s[BASE TABLE] ",
                                                 qlncTraceTreePrefix[aLevel] )
             == STL_SUCCESS );

    /* Relation Name 설정 */
    sRelationName = &(sCandBaseTable->mTableInfo->mRelationName);

    if( stlStrcmp( ellGetTableName( sCandBaseTable->mTableInfo->mTableHandle ),
                   sRelationName->mTable )
        == 0 )
    {
        /* Catalog Name 출력 */
        if( sRelationName->mCatalog != NULL )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s.",
                                                         sRelationName->mCatalog )
                     == STL_SUCCESS );
        }

        /* Schema Name 출력 */
        if( sRelationName->mSchema != NULL )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s.",
                                                         sRelationName->mSchema )
                     == STL_SUCCESS );
        }

        /* Table Name 출력 */
        if( sRelationName->mTable != NULL )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "%s",
                                                         sRelationName->mTable )
                     == STL_SUCCESS );
        }
    }
    else
    {
        /* Alias Name이 있는 경우 */
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "%s %s",
                                                     ellGetTableName( sCandBaseTable->mTableInfo->mTableHandle ),
                                                     sRelationName->mTable )
                 == STL_SUCCESS );
    }

    /* 줄바꿈 */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    /* Read Column List */
    STL_TRY( qlncTraceTreeFromCandNodeReadColumnList( aStringBlock,
                                                      &(sCandBaseTable->mReadColumnList),
                                                      aSubQueryExprList,
                                                      aLevel + 1,
                                                      aEnv )
             == STL_SUCCESS );

    /* Filter */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%sFILTER: ",
                                                 qlncTraceTreePrefix[aLevel+1] )
             == STL_SUCCESS );

    if( sCandBaseTable->mFilter != NULL )
    {
        STL_TRY( qlncTraceTreeFromCandExprAndFilter( aStringBlock,
                                                     (qlncExprCommon*)(sCandBaseTable->mFilter),
                                                     aSubQueryExprList,
                                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "N/A")
                 == STL_SUCCESS );
    }

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    /* Access Path Hint */

    /* Join Hint */


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Table Candidate Node로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandNode           Candidate Node
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandNodeSubTable( qlncStringBlock            * aStringBlock,
                                             qlncNodeCommon             * aCandNode,
                                             qlncTraceSubQueryExprList  * aSubQueryExprList,
                                             stlInt32                     aLevel,
                                             qllEnv                     * aEnv )
{
    qlncSubTableNode    * sCandSubTable = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_SUB_TABLE, QLL_ERROR_STACK(aEnv) );


    /* Candidate Sub Table Node 설정 */
    sCandSubTable = (qlncSubTableNode*)aCandNode;

    /* Node Name */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%s[SUB TABLE] ",
                                                 qlncTraceTreePrefix[aLevel] )
             == STL_SUCCESS );

    /* Relation Name */
    if( sCandSubTable->mRelationName != NULL )
    {
        if( sCandSubTable->mRelationName->mTable != NULL )
        {
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                         aEnv,
                         "%s",
                         sCandSubTable->mRelationName->mTable )
                     == STL_SUCCESS );
        }
    }

    /* 줄바꿈 */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    /* Read Column List */
    STL_TRY( qlncTraceTreeFromCandNodeReadColumnList( aStringBlock,
                                                      &(sCandSubTable->mReadColumnList),
                                                      aSubQueryExprList,
                                                      aLevel + 1,
                                                      aEnv )
             == STL_SUCCESS );

    /* Filter */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%sFILTER: ",
                                                 qlncTraceTreePrefix[aLevel+1] )
             == STL_SUCCESS );

    if( sCandSubTable->mFilter != NULL )
    {
        STL_TRY( qlncTraceTreeFromCandExprAndFilter( aStringBlock,
                                                     (qlncExprCommon*)(sCandSubTable->mFilter),
                                                     aSubQueryExprList,
                                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "N/A")
                 == STL_SUCCESS );
    }

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    /* Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCandNodeFuncList[ sCandSubTable->mQueryNode->mNodeType ](
                 aStringBlock,
                 sCandSubTable->mQueryNode,
                 aSubQueryExprList,
                 aLevel + 1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Table Candidate Node로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandNode           Candidate Node
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandNodeJoinTable( qlncStringBlock           * aStringBlock,
                                              qlncNodeCommon            * aCandNode,
                                              qlncTraceSubQueryExprList * aSubQueryExprList,
                                              stlInt32                    aLevel,
                                              qllEnv                    * aEnv )
{
    stlInt32              i;
    qlncJoinTableNode   * sCandJoinTable    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_JOIN_TABLE, QLL_ERROR_STACK(aEnv) );


    /* Join Table Node 설정 */
    sCandJoinTable = (qlncJoinTableNode*)aCandNode;

    /* Node Name */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%s[JOIN TABLE] ",
                                                 qlncTraceTreePrefix[aLevel] )
             == STL_SUCCESS );

    switch( sCandJoinTable->mJoinType )
    {
        case QLNC_JOIN_TYPE_INNER:
            STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                         aEnv,
                                                         "INNER" )
                     == STL_SUCCESS );
            break;

        case QLNC_JOIN_TYPE_OUTER:
            if( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_LEFT )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "LEFT OUTER" )
                         == STL_SUCCESS );
            }
            else if( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_RIGHT )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "RIGHT OUTER" )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_FULL );
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "FULL OUTER" )
                         == STL_SUCCESS );
            }
            break;

        case QLNC_JOIN_TYPE_SEMI:
            if( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_LEFT )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "LEFT SEMI" )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_RIGHT );
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "RIGHT SEMI" )
                         == STL_SUCCESS );
            }
            break;

        case QLNC_JOIN_TYPE_ANTI_SEMI:
            if( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_LEFT )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "LEFT ANTI SEMI" )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_RIGHT );
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "RIGHT ANTI SEMI" )
                         == STL_SUCCESS );
            }
            break;

        case QLNC_JOIN_TYPE_ANTI_SEMI_NA:
            if( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_LEFT )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "LEFT ANTI SEMI NA" )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sCandJoinTable->mJoinDirect == QLNC_JOIN_DIRECT_RIGHT );
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             "RIGHT ANTI SEMI NA" )
                         == STL_SUCCESS );
            }
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }

    /* 줄바꿈 */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    /* Join Condition */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%sJOIN_COND: ",
                                                 qlncTraceTreePrefix[aLevel+1] )
             == STL_SUCCESS );

    if( sCandJoinTable->mJoinCond != NULL )
    {
        STL_TRY( qlncTraceTreeFromCandExprAndFilter( aStringBlock,
                                                     (qlncExprCommon*)(sCandJoinTable->mJoinCond),
                                                     aSubQueryExprList,
                                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "N/A")
                 == STL_SUCCESS );
    }

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    /* Where Filter */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%sFILTER: ",
                                                 qlncTraceTreePrefix[aLevel+1] )
             == STL_SUCCESS );

    if( sCandJoinTable->mFilter != NULL )
    {
        STL_TRY( qlncTraceTreeFromCandExprAndFilter( aStringBlock,
                                                     (qlncExprCommon*)(sCandJoinTable->mFilter),
                                                     aSubQueryExprList,
                                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "N/A")
                 == STL_SUCCESS );
    }

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    /* Child Node 수행 */
    for( i = 0; i < sCandJoinTable->mNodeArray.mCurCnt; i++ )
    {
        STL_TRY( qlncTraceTreeFromCandNodeFuncList[ sCandJoinTable->mNodeArray.mNodeArr[i]->mNodeType ](
                     aStringBlock,
                     sCandJoinTable->mNodeArray.mNodeArr[i],
                     aSubQueryExprList,
                     aLevel + 1,
                     aEnv )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Flat Instant Candidate Node로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandNode           Candidate Node
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandNodeFlatInstant( qlncStringBlock             * aStringBlock,
                                                qlncNodeCommon              * aCandNode,
                                                qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                stlInt32                      aLevel,
                                                qllEnv                      * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_FLAT_INSTANT, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sort Instant Candidate Node로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandNode           Candidate Node
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandNodeSortInstant( qlncStringBlock             * aStringBlock,
                                                qlncNodeCommon              * aCandNode,
                                                qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                stlInt32                      aLevel,
                                                qllEnv                      * aEnv )
{
    qlncSortInstantNode * sCandSortInstant  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT, QLL_ERROR_STACK(aEnv) );


    /* Candidate Sort Instant Node 설정 */
    sCandSortInstant = (qlncSortInstantNode*)aCandNode;

    /* Node Name */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%s[SORT INSTANT] ",
                                                 qlncTraceTreePrefix[aLevel] )
             == STL_SUCCESS );

    if( sCandSortInstant->mInstantType == QLNC_INSTANT_TYPE_NORMAL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "NORMAL" )
                 == STL_SUCCESS );
    }
    else if( sCandSortInstant->mInstantType == QLNC_INSTANT_TYPE_ORDER )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "ORDER" )
                 == STL_SUCCESS );
    }
    else if( sCandSortInstant->mInstantType == QLNC_INSTANT_TYPE_GROUP )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "GROUP" )
                 == STL_SUCCESS );
    }
    else if( sCandSortInstant->mInstantType == QLNC_INSTANT_TYPE_JOIN )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "JOIN" )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( sCandSortInstant->mInstantType == QLNC_INSTANT_TYPE_DISTINCT );
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "DISTINCT" )
                 == STL_SUCCESS );
    }

    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 " (TopDown: %c, Volatile: %c, LeafOnly: %c, Unique: %c)",
                 (sCandSortInstant->mSortTableAttr.mTopDown == STL_TRUE
                  ? 'T' : 'F'),
                 (sCandSortInstant->mSortTableAttr.mVolatile == STL_TRUE
                  ? 'T' : 'F'),
                 (sCandSortInstant->mSortTableAttr.mLeafOnly == STL_TRUE
                  ? 'T' : 'F'),
                 (sCandSortInstant->mSortTableAttr.mDistinct == STL_TRUE
                  ? 'T' : 'F'))
             == STL_SUCCESS );

    if( sCandSortInstant->mScrollable == ELL_CURSOR_SCROLLABILITY_NO_SCROLL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     " SCROLL: NO" )
                 == STL_SUCCESS );
    }
    else if( sCandSortInstant->mScrollable == ELL_CURSOR_SCROLLABILITY_SCROLL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     " SCROLL: YES" )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     " SCROLL: N/A" )
                 == STL_SUCCESS );
    }

    /* 줄바꿈 */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    /* Key Column List */
    STL_TRY( qlncTraceTreeFromCandNodeInstantColumnList( aStringBlock,
                                                         "KEY COLUMN",
                                                         sCandSortInstant->mKeyColList,
                                                         sCandSortInstant->mKeyFlags,
                                                         aSubQueryExprList,
                                                         aLevel + 1,
                                                         aEnv )
             == STL_SUCCESS );

    /* Record Column List */
    STL_TRY( qlncTraceTreeFromCandNodeInstantColumnList( aStringBlock,
                                                         "RECORD COLUMN",
                                                         sCandSortInstant->mRecColList,
                                                         NULL,
                                                         aSubQueryExprList,
                                                         aLevel + 1,
                                                         aEnv )
             == STL_SUCCESS );

    /* Read Column List */
    STL_TRY( qlncTraceTreeFromCandNodeInstantColumnList( aStringBlock,
                                                         "READ COLUMN",
                                                         sCandSortInstant->mReadColList,
                                                         NULL,
                                                         aSubQueryExprList,
                                                         aLevel + 1,
                                                         aEnv )
             == STL_SUCCESS );

    /* Filter */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%sFILTER: ",
                                                 qlncTraceTreePrefix[aLevel+1] )
             == STL_SUCCESS );

    if( sCandSortInstant->mFilter != NULL )
    {
        STL_TRY( qlncTraceTreeFromCandExprAndFilter( aStringBlock,
                                                     (qlncExprCommon*)(sCandSortInstant->mFilter),
                                                     aSubQueryExprList,
                                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "N/A")
                 == STL_SUCCESS );
    }

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    /* Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCandNodeFuncList[ sCandSortInstant->mChildNode->mNodeType ](
                 aStringBlock,
                 sCandSortInstant->mChildNode,
                 aSubQueryExprList,
                 aLevel + 1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Instant Candidate Node로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandNode           Candidate Node
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandNodeHashInstant( qlncStringBlock             * aStringBlock,
                                                qlncNodeCommon              * aCandNode,
                                                qlncTraceSubQueryExprList   * aSubQueryExprList,
                                                stlInt32                      aLevel,
                                                qllEnv                      * aEnv )
{
    qlncHashInstantNode * sCandHashInstant  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_HASH_INSTANT, QLL_ERROR_STACK(aEnv) );


    /* Candidate Hash Instant Node 설정 */
    sCandHashInstant = (qlncHashInstantNode*)aCandNode;

    /* Node Name */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%s[HASH INSTANT] ",
                                                 qlncTraceTreePrefix[aLevel] )
             == STL_SUCCESS );

    if( sCandHashInstant->mInstantType == QLNC_INSTANT_TYPE_NORMAL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "NORMAL" )
                 == STL_SUCCESS );
    }
    else if( sCandHashInstant->mInstantType == QLNC_INSTANT_TYPE_ORDER )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "ORDER" )
                 == STL_SUCCESS );
    }
    else if( sCandHashInstant->mInstantType == QLNC_INSTANT_TYPE_GROUP )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "GROUP" )
                 == STL_SUCCESS );
    }
    else if( sCandHashInstant->mInstantType == QLNC_INSTANT_TYPE_JOIN )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "JOIN" )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( sCandHashInstant->mInstantType == QLNC_INSTANT_TYPE_DISTINCT );
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "DISTINCT" )
                 == STL_SUCCESS );
    }

    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 " (Logging: %c, SkipNull: %c, Unique: %c, Primary: %c)",
                 ((sCandHashInstant->mHashTableAttr.mLoggingFlag | SML_INDEX_LOGGING_MASK)
                  == SML_INDEX_LOGGING_YES
                  ? 'T' : 'F'),
                 ((sCandHashInstant->mHashTableAttr.mSkipNullFlag | SML_INDEX_SKIP_NULL_MASK)
                   == SML_INDEX_SKIP_NULL_YES
                  ? 'T' : 'F'),
                 ((sCandHashInstant->mHashTableAttr.mUniquenessFlag | SML_INDEX_UNIQUENESS_MASK)
                  == SML_INDEX_UNIQUENESS_YES
                  ? 'T' : 'F'),
                 ((sCandHashInstant->mHashTableAttr.mPrimaryFlag | SML_INDEX_PRIMARY_MASK)
                  == SML_INDEX_PRIMARY_YES
                  ? 'T' : 'F'))
             == STL_SUCCESS );

    if( sCandHashInstant->mScrollable == ELL_CURSOR_SCROLLABILITY_NO_SCROLL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     " SCROLL: NO" )
                 == STL_SUCCESS );
    }
    else if( sCandHashInstant->mScrollable == ELL_CURSOR_SCROLLABILITY_SCROLL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     " SCROLL: YES" )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     " SCROLL: N/A" )
                 == STL_SUCCESS );
    }

    /* 줄바꿈 */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    /* Key Column List */
    STL_TRY( qlncTraceTreeFromCandNodeInstantColumnList( aStringBlock,
                                                         "KEY COLUMN",
                                                         sCandHashInstant->mKeyColList,
                                                         NULL,
                                                         aSubQueryExprList,
                                                         aLevel + 1,
                                                         aEnv )
             == STL_SUCCESS );

    /* Record Column List */
    STL_TRY( qlncTraceTreeFromCandNodeInstantColumnList( aStringBlock,
                                                         "RECORD COLUMN",
                                                         sCandHashInstant->mRecColList,
                                                         NULL,
                                                         aSubQueryExprList,
                                                         aLevel + 1,
                                                         aEnv )
             == STL_SUCCESS );

    /* Read Column List */
    STL_TRY( qlncTraceTreeFromCandNodeInstantColumnList( aStringBlock,
                                                         "READ COLUMN",
                                                         sCandHashInstant->mReadColList,
                                                         NULL,
                                                         aSubQueryExprList,
                                                         aLevel + 1,
                                                         aEnv )
             == STL_SUCCESS );

    /* Filter */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%sFILTER: ",
                                                 qlncTraceTreePrefix[aLevel+1] )
             == STL_SUCCESS );

    if( sCandHashInstant->mFilter != NULL )
    {
        STL_TRY( qlncTraceTreeFromCandExprAndFilter( aStringBlock,
                                                     (qlncExprCommon*)(sCandHashInstant->mFilter),
                                                     aSubQueryExprList,
                                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "N/A")
                 == STL_SUCCESS );
    }

    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );

    /* Child Node 수행 */
    STL_TRY( qlncTraceTreeFromCandNodeFuncList[ sCandHashInstant->mChildNode->mNodeType ](
                 aStringBlock,
                 sCandHashInstant->mChildNode,
                 aSubQueryExprList,
                 aLevel + 1,
                 aEnv )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Inner Join Table Candidate Node로부터 Tree 구조를 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandNode           Candidate Node
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandNodeInnerJoinTable( qlncStringBlock              * aStringBlock,
                                                   qlncNodeCommon               * aCandNode,
                                                   qlncTraceSubQueryExprList    * aSubQueryExprList,
                                                   stlInt32                       aLevel,
                                                   qllEnv                       * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_INNER_JOIN_TABLE, QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Outer Join Table Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandNode           Candidate Node
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandNodeOuterJoinTable( qlncStringBlock              * aStringBlock,
                                                   qlncNodeCommon               * aCandNode,
                                                   qlncTraceSubQueryExprList    * aSubQueryExprList,
                                                   stlInt32                       aLevel,
                                                   qllEnv                       * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_OUTER_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Semi Join Table Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandNode           Candidate Node
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandNodeSemiJoinTable( qlncStringBlock               * aStringBlock,
                                                  qlncNodeCommon                * aCandNode,
                                                  qlncTraceSubQueryExprList     * aSubQueryExprList,
                                                  stlInt32                        aLevel,
                                                  qllEnv                        * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_SEMI_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Anti Semi Join Table Candidate Node로부터 Query 구문을 만든다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aCandNode           Candidate Node
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandNodeAntiSemiJoinTable( qlncStringBlock               * aStringBlock,
                                                      qlncNodeCommon                * aCandNode,
                                                      qlncTraceSubQueryExprList     * aSubQueryExprList,
                                                      stlInt32                        aLevel,
                                                      qllEnv                        * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCandNode->mNodeType == QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Read Column List를 출력한다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aReadColumnList     Read Column List
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandNodeReadColumnList( qlncStringBlock              * aStringBlock,
                                                   qlncRefColumnList            * aReadColumnList,
                                                   qlncTraceSubQueryExprList    * aSubQueryExprList,
                                                   stlInt32                       aLevel,
                                                   qllEnv                       * aEnv )
{
    qlncRefColumnItem   * sRefColumnItem    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );


    /* Title */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%sCOLUMN: ",
                                                 qlncTraceTreePrefix[aLevel] )
             == STL_SUCCESS );

    if( aReadColumnList == NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "N/A" )
                 == STL_SUCCESS );
    }
    else
    {
        sRefColumnItem = aReadColumnList->mHead;
        while( sRefColumnItem != NULL )
        {
            /* Expression 출력 */
            STL_TRY( qlncTraceTreeFromCandExprFuncList[ sRefColumnItem->mExpr->mType ](
                         aStringBlock,
                         sRefColumnItem->mExpr,
                         aSubQueryExprList,
                         aEnv )
                     == STL_SUCCESS );

            if( sRefColumnItem->mNext != NULL )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             ", " )
                         == STL_SUCCESS );
            }

            sRefColumnItem = sRefColumnItem->mNext;
        }
    }

    /* 줄바꿈 */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Instant Column List를 출력한다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aTitle              Title
 * @param[in]   aColumnList         Instant Column List
 * @param[in]   aKeyFlags           Key Flags (for Sort Instant Key Column Only)
 * @param[in]   aSubQueryExprList   SubQuery Expression List
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceTreeFromCandNodeInstantColumnList( qlncStringBlock           * aStringBlock,
                                                      stlChar                   * aTitle,
                                                      qlncRefExprList           * aColumnList,
                                                      stlUInt8                  * aKeyFlags,
                                                      qlncTraceSubQueryExprList * aSubQueryExprList,
                                                      stlInt32                    aLevel,
                                                      qllEnv                    * aEnv )
{
    stlInt32          i;
    qlncRefExprItem * sRefExprItem  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTitle != NULL, QLL_ERROR_STACK(aEnv) );


    /* Title */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "%s%s ",
                                                 qlncTraceTreePrefix[aLevel],
                                                 aTitle)
             == STL_SUCCESS );

    if( aColumnList == NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                     aEnv,
                                                     "N/A" )
                 == STL_SUCCESS );
    }
    else
    {
        i = 0;
        sRefExprItem = aColumnList->mHead;
        while( sRefExprItem != NULL )
        {
            /* Expression 출력 */
            STL_TRY( qlncTraceTreeFromCandExprFuncList[ sRefExprItem->mExpr->mType ](
                         aStringBlock,
                         sRefExprItem->mExpr,
                         aSubQueryExprList,
                         aEnv )
                     == STL_SUCCESS );

            if( aKeyFlags != NULL )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             aStringBlock,
                             aEnv,
                             " %s %s",
                             (DTL_GET_KEY_FLAG_SORT_ORDER( aKeyFlags[i] )
                              == DTL_KEYCOLUMN_INDEX_ORDER_ASC
                              ? "ASC" : "DESC"),
                             (DTL_GET_KEY_FLAG_NULL_ORDER( aKeyFlags[i] )
                              == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST
                              ? "NULLS FIRST" : "NULLS LAST") )
                         == STL_SUCCESS );
            }

            if( sRefExprItem->mNext != NULL )
            {
                STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                             aEnv,
                                                             ", " )
                         == STL_SUCCESS );
            }

            i++;
            sRefExprItem = sRefExprItem->mNext;
        }
    }

    /* 줄바꿈 */
    STL_TRY( qlncTraceAppendStringToStringBlock( aStringBlock,
                                                 aEnv,
                                                 "\n" )
             == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */
