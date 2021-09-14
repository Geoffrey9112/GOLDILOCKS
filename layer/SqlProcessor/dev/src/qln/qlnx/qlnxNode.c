/*******************************************************************************
 * qlnxNode.c
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
 * @file qlnxNode.c
 * @brief SQL Processor Layer Execution Node Common Functions
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qlnx
 * @{
 */


/**
 * @brief Execution Node Function Pointers
 */
qlnxFunctionPtr qlnxNodeFunctions[ QLL_PLAN_NODE_TYPE_MAX ] =
{
    /***************************************
     * STATEMENT
     ***************************************/
    
    /* QLL_PLAN_NODE_STMT_SELECT_TYPE */
    {
        qlnxInitializeSelectStmt,
        qlnxExecuteSelectStmt,
        qlnxFetchSelectStmt,
        qlnxFinalizeSelectStmt
    },
    
    /* QLL_PLAN_NODE_STMT_INSERT_TYPE */
    {
        qlnxInitializeInsertStmt,
        qlnxExecuteInsertStmt,
        qlnxFetchNA,
        qlnxFinalizeInsertStmt
    },

    /* QLL_PLAN_NODE_STMT_UPDATE_TYPE */
    {
        qlnxInitializeUpdateStmt,
        qlnxExecuteUpdateStmt,
        qlnxFetchNA,
        qlnxFinalizeUpdateStmt
    },

    /* QLL_PLAN_NODE_STMT_DELETE_TYPE */
    {
        qlnxInitializeDeleteStmt,
        qlnxExecuteDeleteStmt,
        qlnxFetchNA,
        qlnxFinalizeDeleteStmt
    },

    /* QLL_PLAN_NODE_STMT_MERGE_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
    },

    
    /***************************************
     * QUERY
     ***************************************/

    /* QLL_PLAN_NODE_QUERY_SPEC_TYPE */
    {
        qlnxInitializeQuerySpec,
        qlnxExecuteQuerySpec,
        qlnxFetchQuerySpec,
        qlnxFinalizeQuerySpec
    },

    /* QLL_PLAN_NODE_INDEX_BUILD_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
    },

    /* QLL_PLAN_NODE_FOR_UPDATE_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
    },

    /* QLL_PLAN_NODE_MERGE_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
    },

    /* QLL_PLAN_NODE_VIEW_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
    },

    /* QLL_PLAN_NODE_VIEW_PUSHED_PRED_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
    },

    /* QLL_PLAN_NODE_FILTER_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
    },

    /* QLL_PLAN_NODE_INLIST_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
    },


    /***************************************
     * SET
     ***************************************/

    /* QLL_PLAN_NODE_QUERY_SET_TYPE */
    {
        qlnxInitializeQuerySet,
        qlnxExecuteQuerySet,
        qlnxFetchQuerySet,
        qlnxFinalizeQuerySet
    },

    /* QLL_PLAN_NODE_UNION_ALL_TYPE */
    {
        qlnxInitializeUnionAll,
        qlnxExecuteUnionAll,
        qlnxFetchUnionAll,
        qlnxFinalizeUnionAll
    },

    /* QLL_PLAN_NODE_UNION_DISTINCT_TYPE */
    {
        qlnxInitializeUnionDistinct,
        qlnxExecuteUnionDistinct,
        qlnxFetchUnionDistinct,
        qlnxFinalizeUnionDistinct
    },
    
    /* QLL_PLAN_NODE_EXCEPT_ALL_TYPE */
    {
        qlnxInitializeExcept,
        qlnxExecuteExcept,
        qlnxFetchExcept,
        qlnxFinalizeExcept
    },

    /* QLL_PLAN_NODE_EXCEPT_DISTINCT_TYPE */
    {
        qlnxInitializeExcept,
        qlnxExecuteExcept,
        qlnxFetchExcept,
        qlnxFinalizeExcept
    },

    /* QLL_PLAN_NODE_INTERSECT_ALL_TYPE */
    {
        qlnxInitializeIntersect,
        qlnxExecuteIntersect,
        qlnxFetchIntersect,
        qlnxFinalizeIntersect
    },

    /* QLL_PLAN_NODE_INTERSECT_DISTINCT_TYPE */
    {
        qlnxInitializeIntersect,
        qlnxExecuteIntersect,
        qlnxFetchIntersect,
        qlnxFinalizeIntersect
    },
    

    /***************************************
     * SUB-QUERY
     ***************************************/

    /* QLL_PLAN_NODE_SUB_QUERY_TYPE */
    {
        qlnxInitializeSubQuery,
        qlnxExecuteSubQuery,
        qlnxFetchSubQuery,
        qlnxFinalizeSubQuery
    },

    /* QLL_PLAN_NODE_SUB_QUERY_LIST_TYPE */
    {
        qlnxInitializeSubQueryList,
        qlnxExecuteSubQueryList,
        qlnxFetchSubQueryList,
        qlnxFinalizeSubQueryList
    },

    /* QLL_PLAN_NODE_SUB_QUERY_FUNCTION_TYPE */
    {
        qlnxInitializeSubQueryFunc,
        qlnxExecuteSubQueryFunc,
        qlnxFetchSubQueryFunc,
        qlnxFinalizeSubQueryFunc
    },

    /* QLL_PLAN_NODE_SUB_QUERY_FILTER_TYPE */
    {
        qlnxInitializeSubQueryFilter,
        qlnxExecuteSubQueryFilter,
        qlnxFetchSubQueryFilter,
        qlnxFinalizeSubQueryFilter
    },


    /***************************************
     * OR
     ***************************************/

    /* QLL_PLAN_NODE_CONCAT_TYPE */
    {
        qlnxInitializeConcat,
        qlnxExecuteConcat,
        qlnxFetchConcat,
        qlnxFinalizeConcat
    },

    
    /***************************************
     * JOIN
     ***************************************/

    /* QLL_PLAN_NODE_NESTED_LOOPS_TYPE */
    {
        qlnxInitializeNestedLoopsJoin,
        qlnxExecuteNestedLoopsJoin,
        qlnxFetchNestedLoopsJoin,
        qlnxFinalizeNestedLoopsJoin
    },

    /* QLL_PLAN_NODE_MERGE_JOIN_TYPE */
    {
        qlnxInitializeSortMergeJoin,
        qlnxExecuteSortMergeJoin,
        qlnxFetchSortMergeJoin,
        qlnxFinalizeSortMergeJoin
    },

    /* QLL_PLAN_NODE_HASH_JOIN_TYPE */
    {
        qlnxInitializeHashJoin,
        qlnxExecuteHashJoin,
        qlnxFetchHashJoin,
        qlnxFinalizeHashJoin
    },

    /* QLL_PLAN_NODE_INDEX_JOIN_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
    },


    /***************************************
     * ORDER BY, GROUP BY, WINDOWS AND Etc.
     ***************************************/

    /* QLL_PLAN_NODE_CONNECT_BY_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
    },

    /* QLL_PLAN_NODE_PSEUDO_COLUMN_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
    },

    /* QLL_PLAN_NODE_COUNT_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
    },

    /* QLL_PLAN_NODE_WINDOW_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
    },

    /* QLL_PLAN_NODE_HASH_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
    },

    /* QLL_PLAN_NODE_HASH_AGGREGATION_TYPE */
    {
        qlnxInitializeHashAggregation,
        qlnxExecuteHashAggregation,
        qlnxFetchHashAggregation,
        qlnxFinalizeHashAggregation
    },

    /* QLL_PLAN_NODE_GROUP_TYPE */
    {
        qlnxInitializeGroup,
        qlnxExecuteGroup,
        qlnxFetchGroup,
        qlnxFinalizeGroup
    },

    
    /***************************************
     * SCAN
     ***************************************/

    /* QLL_PLAN_NODE_TABLE_ACCESS_TYPE */
    {
        qlnxInitializeTableAccess,
        qlnxExecuteTableAccess,
        qlnxFetchTableAccess,
        qlnxFinalizeTableAccess
    },

    /* QLL_PLAN_NODE_INDEX_ACCESS_TYPE */
    {
        qlnxInitializeIndexAccess,
        qlnxExecuteIndexAccess,
        qlnxFetchIndexAccess,
        qlnxFinalizeIndexAccess
    },

    /* QLL_PLAN_NODE_ROWID_ACCESS_TYPE */
    {
        qlnxInitializeRowIdAccess,
        qlnxExecuteRowIdAccess,
        qlnxFetchRowIdAccess,
        qlnxFinalizeRowIdAccess
    },
    
    /* QLL_PLAN_NODE_FAST_DUAL_ACCESS_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
#if 0
        qlnxInitializeFastDualAccess,
        qlnxExecuteFastDualAccess,
        qlnxFetchFastDualAccess,
        qlnxFinalizeFastDualAccess
#endif
    },

    /* QLL_PLAN_NODE_FLAT_INSTANT_ACCESS_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
#if 0
        qlnxInitializeFlatInstantAccess,
        qlnxExecuteFlatInstantAccess,
        qlnxFetchFlatInstantAccess,
        qlnxFinalizeFlatInstantAccess
#endif
    },

    /* QLL_PLAN_NODE_SORT_INSTANT_ACCESS_TYPE */
    {
        qlnxInitializeSortInstantAccess,
        qlnxExecuteSortInstantAccess,
        qlnxFetchSortInstantAccess,
        qlnxFinalizeSortInstantAccess
    },

    /* QLL_PLAN_NODE_HASH_INSTANT_ACCESS_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
#if 0
        qlnxInitializeHashInstantAccess,
        qlnxExecuteHashInstantAccess,
        qlnxFetchHashInstantAccess,
        qlnxFinalizeHashInstantAccess
#endif
    },

    /* QLL_PLAN_NODE_GROUP_HASH_INSTANT_ACCESS_TYPE */
    {
        qlnxInitializeGroupHashInstantAccess,
        qlnxExecuteGroupHashInstantAccess,
        qlnxFetchGroupHashInstantAccess,
        qlnxFinalizeGroupHashInstantAccess
    },

    /* QLL_PLAN_NODE_GROUP_SORT_INSTANT_ACCESS_TYPE */
    {
        qlnxInitializeNA,
        qlnxExecuteNA,
        qlnxFetchNA,
        qlnxFinalizeNA
#if 0
        qlnxInitializeGroupSortInstantAccess,
        qlnxExecuteGroupSortInstantAccess,
        qlnxFetchGroupSortInstantAccess,
        qlnxFinalizeGroupSortInstantAccess
#endif
    },

    /* QLL_PLAN_NODE_SORT_JOIN_INSTANT_ACCESS_TYPE */
    {
        qlnxInitializeSortJoinInstantAccess,
        qlnxExecuteSortJoinInstantAccess,
        qlnxFetchSortJoinInstantAccess,
        qlnxFinalizeSortJoinInstantAccess
    },

    /* QLL_PLAN_NODE_HASH_JOIN_INSTANT_ACCESS_TYPE */
    {
        qlnxInitializeHashJoinInstantAccess,
        qlnxExecuteHashJoinInstantAccess,
        qlnxFetchHashJoinInstantAccess,
        qlnxFinalizeHashJoinInstantAccess
    }
};
    



/**
 * @brief execution node를 초기화한다.
 * @param[in]      aTransID      Transaction ID
 * @param[in]      aStmt         Statement
 * @param[in]      aDBCStmt      DBC Statement
 * @param[in,out]  aSQLStmt      SQL Statement
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Data Optimization 단계에서 수행한다.
 *    <BR> Execution Node를 생성하고, Execution을 위한 정보를 구축한다.
 *    <BR> 공간 할당시 Data Plan 메모리 관리자를 이용한다.
 */
stlStatus qlnxInitializeNA( smlTransId              aTransID,
                            smlStatement          * aStmt,
                            qllDBCStmt            * aDBCStmt,
                            qllStatement          * aSQLStmt,
                            qllOptimizationNode   * aOptNode,
                            qllDataArea           * aDataArea,
                            qllEnv                * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief Execution Node를 수행한다.
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
stlStatus qlnxExecuteNA( smlTransId              aTransID,
                         smlStatement          * aStmt,
                         qllDBCStmt            * aDBCStmt,
                         qllStatement          * aSQLStmt,
                         qllOptimizationNode   * aOptNode,
                         qllDataArea           * aDataArea,
                         qllEnv                * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief Execution Node를 Fetch한다.
 * @param[in]      aFetchNodeInfo  Fetch Node Info
 * @param[out]     aUsedBlockCnt   Read Value Block의 Data가 저장된 공간의 개수
 * @param[out]     aEOF            End Of Fetch 여부
 * @param[in]      aEnv            Environment
 *
 * @remark Fetch 수행시 호출된다.
 */
stlStatus qlnxFetchNA( qlnxFetchNodeInfo     * aFetchNodeInfo,
                       stlInt64              * aUsedBlockCnt,
                       stlBool               * aEOF,
                       qllEnv                * aEnv )
{
    return STL_SUCCESS;
}


/**
 * @brief Execution Node 수행을 종료한다.
 * @param[in]      aOptNode      Optimization Node
 * @param[in]      aDataArea     Data Area (Data Optimization 결과물)
 * @param[in]      aEnv          Environment
 *
 * @remark Execution을 종료하고자 할 때 호출한다.
 */
stlStatus qlnxFinalizeNA( qllOptimizationNode   * aOptNode,
                          qllDataArea           * aDataArea,
                          qllEnv                * aEnv )
{
    return STL_SUCCESS;
}





/**
 * @brief FLAT Instant Table 을 생성한다.
 * @param[in]     aTransID                Transaction ID
 * @param[in]     aStmt                   Statement
 * @param[in,out] aInstantTable           Instant Table
 * @param[in]     aIsScrollableInstant    Instant Table 의 Scrollability
 * @param[in]     aEnv                    Environment
 * @remarks
 */
stlStatus qlnxCreateFlatInstantTable( smlTransId               aTransID,
                                      smlStatement           * aStmt,
                                      qlnInstantTable        * aInstantTable,
                                      stlBool                  aIsScrollableInstant,
                                      qllEnv                 * aEnv )
{
    smlTableAttr            sTableAttr;
    stlInt64                sTablePhyID = 0;

    ellDictHandle           sSpaceHandle;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstantTable != NULL, QLL_ERROR_STACK(aEnv) );

    /**************************************************
     * Instant Table 정보 초기화 
     **************************************************/

    aInstantTable->mIsNeedBuild = STL_TRUE;
    aInstantTable->mRecCnt      = 0;
    aInstantTable->mCurIdx      = 0;

    aInstantTable->mStmt        = aStmt;
    aInstantTable->mIterator    = NULL;
    aInstantTable->mIndexHandle = NULL;
    
    SML_INIT_ITERATOR_PROP( aInstantTable->mIteratorProperty );

    /**************************************************
     * Instant Table 정보 설정 
     **************************************************/

    stlMemset( & sTableAttr, 0x00, STL_SIZEOF(smlTableAttr) );

    /**
     * Logging 하지 않음
     */
    
    sTableAttr.mValidFlags |= SML_TABLE_LOGGING_YES;
    sTableAttr.mLoggingFlag = STL_FALSE;
    
    /**
     * Locking 하지 않음
     */
    
    sTableAttr.mValidFlags |= SML_TABLE_LOCKING_YES;
    sTableAttr.mLockingFlag = STL_FALSE;

    /**
     * Instant Table 의 Scrollable 여부
     */

    sTableAttr.mValidFlags |= SML_TABLE_SCROLLABLE_YES;
    sTableAttr.mScrollable = aIsScrollableInstant;
    
    /**
     * Memory Instant Table 임
     */

    sTableAttr.mValidFlags |= SML_TABLE_TYPE_MEMORY_INSTANT;
    
    /**************************************************
     * Instant Table 생성
     **************************************************/

    /**
     * Current User 의 Default Temporary Space Handle 획득
     */

    STL_TRY( ellGetCurrentTempSpaceHandle( aTransID,
                                           aStmt->mScn,
                                           & sSpaceHandle,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );
             
    /**
     * Instant Table 생성
     */

    STL_TRY( smlCreateTable( aInstantTable->mStmt,
                             ellGetTablespaceID( & sSpaceHandle ),
                             SML_MEMORY_INSTANT_FLAT_TABLE,
                             & sTableAttr,
                             & sTablePhyID,
                             & aInstantTable->mTableHandle,
                             SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SORT Instant Table 을 생성한다.
 * @param[in]     aTransID          Transaction ID
 * @param[in]     aStmt             Statement
 * @param[in,out] aInstantTable     Instant Table
 * @param[in]     aBaseTableHandle  Base Table Handle
 * @param[in]     aKeyColCount      Sort Key Column Count
 * @param[in]     aKeyColList       Sort Key Column Value Block List
 * @param[in]     aKeyColFlags      Sort Key Column Flags
 * @param[in]     aIsTopDown        Top Down Build 여부 ( true : top-down, false : bottom-up)
 * @param[in]     aIsVolatile       Volatile 여부
 * @param[in]     aIsLeafOnly       index에 internal 없이 leaf만 만들것인지 여부
 * @param[in]     aIsDistinct       Distinct 여부
 * @param[in]     aNullExcluded     Null Value 포함한 Record Insert Skip 여부
 * @param[in]     aEnv              Environment
 * @remarks
 *  # volatile 모드이면 중간 merge를 하지 않음. scroll 불가. true이면 mOnlyLeaf는 무시
 *  # index에 internal 없이 leaf만 만들들면 search 불가.
 */
stlStatus qlnxCreateSortInstantTable( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      qlnInstantTable    * aInstantTable,
                                      void               * aBaseTableHandle,
                                      stlUInt16            aKeyColCount,
                                      knlValueBlockList  * aKeyColList,
                                      stlUInt8           * aKeyColFlags,
                                      stlBool              aIsTopDown,
                                      stlBool              aIsVolatile,
                                      stlBool              aIsLeafOnly,
                                      stlBool              aIsDistinct,
                                      stlBool              aNullExcluded,
                                      qllEnv             * aEnv )
{
    smlSortTableAttr   sSortTableAttr;
    stlInt64           sTablePhyID = 0;

    ellDictHandle      sSpaceHandle;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstantTable != NULL, QLL_ERROR_STACK(aEnv) );

    /**************************************************
     * Instant Table 정보 초기화 
     **************************************************/

    aInstantTable->mIsNeedBuild = STL_TRUE;
    aInstantTable->mRecCnt      = 0;
    aInstantTable->mCurIdx      = 0;

    aInstantTable->mStmt        = aStmt;
    aInstantTable->mIterator    = NULL;
    aInstantTable->mIndexHandle = NULL;
    
    SML_INIT_ITERATOR_PROP( aInstantTable->mIteratorProperty );

    /**************************************************
     * Instant Table 정보 설정 
     **************************************************/

    stlMemset( & sSortTableAttr, 0x00, STL_SIZEOF(smlSortTableAttr) );

    sSortTableAttr.mTopDown      = aIsTopDown;
    sSortTableAttr.mVolatile     = aIsVolatile;
    sSortTableAttr.mLeafOnly     = aIsLeafOnly;
    sSortTableAttr.mDistinct     = aIsDistinct;
    sSortTableAttr.mNullExcluded = aNullExcluded;

    
    /**************************************************
     * Instant Table 생성
     **************************************************/

    /**
     * Current User 의 Default Temporary Space Handle 획득
     */

    STL_TRY( ellGetCurrentTempSpaceHandle( aTransID,
                                           aStmt->mScn,
                                           & sSpaceHandle,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Instant Table 생성
     */

    STL_TRY( smlCreateSortTable( aStmt,
                                 ellGetTablespaceID( & sSpaceHandle ),
                                 & sSortTableAttr,
                                 aBaseTableHandle, /* Table Handle for Secondary Instant Table */
                                 aKeyColCount,
                                 aKeyColList,
                                 aKeyColFlags,
                                 & sTablePhyID,
                                 & aInstantTable->mTableHandle,
                                 SML_ENV( aEnv ) )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief HASH Instant Table 을 생성한다.
 * @param[in]     aTransID          Transaction ID
 * @param[in]     aStmt             Statement
 * @param[in,out] aInstantTable     Instant Table
 * @param[in]     aBaseTableHandle  Base Table Handle
 * @param[in]     aKeyColCount      Hash Key Column Count
 * @param[in]     aColList          Hash Row Value Block List
 * @param[in]     aHashTableAttr    Hash Instant Table Attribute
 * @param[in]     aEnv              Environment
 * @remarks
 */
stlStatus qlnxCreateHashInstantTable( smlTransId           aTransID,
                                      smlStatement       * aStmt,
                                      qlnInstantTable    * aInstantTable,
                                      void               * aBaseTableHandle,
                                      stlUInt16            aKeyColCount,
                                      knlValueBlockList  * aColList,
                                      smlIndexAttr       * aHashTableAttr,
                                      qllEnv             * aEnv )
{
    stlInt64           sTablePhyID = 0;

    ellDictHandle      sSpaceHandle;


    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstantTable != NULL, QLL_ERROR_STACK(aEnv) );


    /**************************************************
     * Instant Table 정보 초기화 
     **************************************************/

    aInstantTable->mIsNeedBuild = STL_TRUE;
    aInstantTable->mRecCnt      = 0;
    aInstantTable->mCurIdx      = 0;

    aInstantTable->mStmt        = aStmt;
    aInstantTable->mIterator    = NULL;
    aInstantTable->mIndexHandle = NULL;
    
    SML_INIT_ITERATOR_PROP( aInstantTable->mIteratorProperty );


    /**************************************************
     * Instant Table 생성
     **************************************************/

    /**
     * Current User 의 Default Temporary Space Handle 획득
     */

    STL_TRY( ellGetCurrentTempSpaceHandle( aTransID,
                                           aStmt->mScn,
                                           & sSpaceHandle,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Instant Table 생성
     */

    STL_TRY( smlCreateIoTable( aStmt,
                               ellGetTablespaceID( & sSpaceHandle ),
                               SML_MEMORY_INSTANT_HASH_TABLE,
                               NULL,
                               aHashTableAttr,
                               aColList,
                               aKeyColCount,
                               NULL,
                               & sTablePhyID,
                               & aInstantTable->mTableHandle,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief HASH Instant Index 을 생성한다.
 * @param[in]     aTransID          Transaction ID
 * @param[in]     aStmt             Statement
 * @param[in,out] aInstantTable     Instant Table
 * @param[in]     aKeyColCount      Hash Key Column Count
 * @param[in]     aColList          Hash Row Value Block List
 * @param[in]     aHashTableAttr    Hash Instant Table Attribute
 * @param[in]     aEnv              Environment
 * @remarks
 */
stlStatus qlnxCreateHashJoinInstantIndex( smlTransId           aTransID,
                                          smlStatement       * aStmt,
                                          qlnInstantTable    * aInstantTable,
                                          stlUInt16            aKeyColCount,
                                          knlValueBlockList  * aColList,
                                          smlIndexAttr       * aHashTableAttr,
                                          qllEnv             * aEnv )
{
    stlInt64           sTablePhyID = 0;

    ellDictHandle      sSpaceHandle;


    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstantTable != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstantTable->mTableHandle != NULL, QLL_ERROR_STACK(aEnv) );


    /**************************************************
     * Instant Table 정보 초기화 
     **************************************************/

    aInstantTable->mIsNeedBuild = STL_TRUE;
    aInstantTable->mRecCnt      = 0;
    aInstantTable->mCurIdx      = 0;

    aInstantTable->mStmt        = aStmt;
    aInstantTable->mIterator    = NULL;
    aInstantTable->mIndexHandle = NULL;
    
    SML_INIT_ITERATOR_PROP( aInstantTable->mIteratorProperty );


    /**************************************************
     * Instant Index 생성
     **************************************************/

    /**
     * Current User 의 Default Temporary Space Handle 획득
     */

    STL_TRY( ellGetCurrentTempSpaceHandle( aTransID,
                                           aStmt->mScn,
                                           & sSpaceHandle,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Instant Index 생성
     */

    STL_TRY( smlCreateIndex( aStmt,
                             ellGetTablespaceID( & sSpaceHandle ),
                             SML_INDEX_TYPE_MEMORY_INSTANT_HASH,
                             aHashTableAttr,
                             aInstantTable->mTableHandle,
                             aKeyColCount,
                             aColList,
                             NULL,
                             & sTablePhyID,
                             & aInstantTable->mIndexHandle,
                             SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief GROUP HASH Instant Table 을 생성한다.
 * @param[in]     aTransID          Transaction ID
 * @param[in]     aStmt             Statement
 * @param[in,out] aInstantTable     Instant Table
 * @param[in]     aSkipNullFlag     Skip Null 여부
 * @param[in]     aKeyColCount      Group Key Column Count
 * @param[in]     aColList          Hash Row Value Block List
 * @param[in]     aEnv              Environment
 */
stlStatus qlnxCreateGroupHashInstantTable( smlTransId           aTransID,
                                           smlStatement       * aStmt,
                                           qlnInstantTable    * aInstantTable,
                                           stlBool              aSkipNullFlag,
                                           stlUInt16            aKeyColCount,
                                           knlValueBlockList  * aColList,
                                           qllEnv             * aEnv )
{
    smlIndexAttr       sIndexAttr;
    stlInt64           sTablePhyID = 0;

    ellDictHandle      sSpaceHandle;

    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstantTable != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**************************************************
     * Instant Table 정보 초기화 
     **************************************************/

    aInstantTable->mIsNeedBuild = STL_TRUE;
    aInstantTable->mRecCnt      = 0;
    aInstantTable->mCurIdx      = 0;

    aInstantTable->mStmt        = aStmt;
    aInstantTable->mIterator    = NULL;
    aInstantTable->mIndexHandle = NULL;
    
    SML_INIT_ITERATOR_PROP( aInstantTable->mIteratorProperty );

    
    /**************************************************
     * Instant Index 정보 설정 (Index)
     **************************************************/

    stlMemset( & sIndexAttr, 0x00, STL_SIZEOF(smlIndexAttr) );

    /**
     * Logging 하지 않음
     */
    
    sIndexAttr.mValidFlags |= SML_INDEX_LOGGING_YES;
    sIndexAttr.mLoggingFlag = STL_FALSE;

    
    /**
     * Uniqueness 여부
     */

    sIndexAttr.mValidFlags |= SML_INDEX_UNIQUENESS_YES;
    sIndexAttr.mUniquenessFlag = STL_TRUE;


    /**
     * Primary Key 여부
     */

    sIndexAttr.mPrimaryFlag = STL_FALSE;

    
    /**
     * Skip Null 여부
     */


    sIndexAttr.mValidFlags |= SML_INDEX_SKIP_NULL_YES;
    sIndexAttr.mSkipNullFlag = aSkipNullFlag;
    

    /**************************************************
     * Instant Table 생성
     **************************************************/

    /**
     * Current User 의 Default Temporary Space Handle 획득
     */

    STL_TRY( ellGetCurrentTempSpaceHandle( aTransID,
                                           aStmt->mScn,
                                           & sSpaceHandle,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );
    

    /**
     * Instant Table 생성
     */

    STL_TRY( smlCreateIoTable( aStmt,
                               ellGetTablespaceID( & sSpaceHandle ),
                               SML_MEMORY_INSTANT_HASH_TABLE,
                               NULL,
                               & sIndexAttr,
                               aColList,
                               aKeyColCount,
                               NULL,
                               & sTablePhyID,
                               & aInstantTable->mTableHandle,
                               SML_ENV( aEnv ) )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


#if 0
/**
 * @brief GROUP SORT Instant Table 을 생성한다.
 * @param[in]     aTransID          Transaction ID
 * @param[in]     aStmt             Statement
 * @param[in,out] aInstantTable     Instant Table
 * @param[in]     aBaseTableHandle  Base Table Handle
 * @param[in]     aKeyColCount      Group Key Column Count
 * @param[in]     aKeyColList       Group Key Column Value Block List
 * @param[in]     aKeyColFlags      Group Key Column Flags
 * @param[in]     aIsTopDown        Top Down Build 여부 ( true : top-down, false : bottom-up)
 * @param[in]     aIsVolatile       Volatile 여부
 * @param[in]     aIsLeafOnly       index에 internal 없이 leaf만 만들것인지 여부
 * @param[in]     aIsUnique         Unique Check 여부
 * @param[in]     aNullExcluded     Null Value 포함한 Record Insert Skip 여부
 * @param[in]     aEnv              Environment
 * @remarks
 *  # volatile 모드이면 중간 merge를 하지 않음. scroll 불가. true이면 mOnlyLeaf는 무시
 *  # index에 internal 없이 leaf만 만들들면 search 불가.
 */
stlStatus qlnxCreateGroupSortInstantTable( smlTransId           aTransID,
                                           smlStatement       * aStmt,
                                           qlnInstantTable    * aInstantTable,
                                           void               * aBaseTableHandle,
                                           stlUInt16            aKeyColCount,
                                           knlValueBlockList  * aKeyColList,
                                           stlUInt8           * aKeyColFlags,
                                           stlBool              aIsTopDown,
                                           stlBool              aIsVolatile,
                                           stlBool              aIsLeafOnly,
                                           stlBool              aIsUnique,
                                           qllEnv             * aEnv )
{
    smlSortTableAttr   sSortTableAttr;
    stlInt64           sTablePhyID = 0;

    ellDictHandle      sSpaceHandle;

    
    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstantTable != NULL, QLL_ERROR_STACK(aEnv) );

    
    /**************************************************
     * Instant Table 정보 초기화 
     **************************************************/

    aInstantTable->mIsNeedBuild = STL_TRUE;
    aInstantTable->mRecCnt      = 0;
    aInstantTable->mCurIdx      = 0;

    aInstantTable->mStmt        = aStmt;
    aInstantTable->mIterator    = NULL;
    aInstantTable->mIndexHandle = NULL;
    
    SML_INIT_ITERATOR_PROP( aInstantTable->mIteratorProperty );
    

    /**************************************************
     * Instant Table 정보 설정 
     **************************************************/

    sSortTableAttr.mTopDown      = aIsTopDown;
    sSortTableAttr.mVolatile     = aIsVolatile;
    sSortTableAttr.mLeafOnly     = aIsLeafOnly;
    sSortTableAttr.mDistinct     = aIsDistinct;
    sSortTableAttr.mNullExcluded = aNullExcluded;

    
    /**************************************************
     * Instant Table 생성
     **************************************************/

    /**
     * Current User 의 Default Temporary Space Handle 획득
     */

    STL_TRY( ellGetCurrentTempSpaceHandle( aTransID,
                                           & sSpaceHandle,
                                           ELL_ENV(aEnv) )
             == STL_SUCCESS );

    
    /**
     * Instant Table 생성
     */

    STL_TRY( smlCreateSortTable( aStmt,
                                 ellGetTablespaceID( & sSpaceHandle ),
                                 & sSortTableAttr,
                                 aBaseTableHandle, /* Table Handle for Secondary Instant Table */
                                 aKeyColCount,
                                 aKeyColList,
                                 aKeyColFlags,
                                 & sTablePhyID,
                                 & aInstantTable->mTableHandle,
                                 SML_ENV( aEnv ) )
             == STL_SUCCESS );

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
#endif


/**
 * @brief Value Block 내에서 조건에 부합하는 Row 만을 Instant Table 에 추가한다.
 * @param[in]  aInstantTable      Instant Table
 * @param[in]  aRowBlock          Row Block
 * @param[in]  aStoreBlockList    Store Value Block List
 * @param[in]  aMeetArray         조건에 만족하는 지의 여부를 저장하고 있는 정보 (nullable, all true)
 * @param[in]  aSkipCnt           Skip Block Count
 * @param[in]  aUsedCnt           Used Block Count
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus qlnxAddMeetBlockIntoInstantTable( qlnInstantTable   * aInstantTable,
                                            smlRowBlock       * aRowBlock,
                                            knlValueBlockList * aStoreBlockList,
                                            stlBool           * aMeetArray,
                                            stlInt64            aSkipCnt,
                                            stlInt64            aUsedCnt,
                                            qllEnv            * aEnv )
{
    stlInt64               sBlockIdx = 0;
    
    stlInt64               sStoreIdx = 0;
    stlInt64               sSkipStoreIdx = 0;
    stlInt64               sUsedStoreIdx = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aInstantTable != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstantTable->mStmt != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInstantTable->mTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRowBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aStoreBlockList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSkipCnt >= 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aUsedCnt >= aSkipCnt, QLL_ERROR_STACK(aEnv) );
    
    for ( sBlockIdx = aSkipCnt; sBlockIdx < aUsedCnt; sBlockIdx++ )
    {
        /**
         * 저장을 시작할 구간 검색
         */

        if ( aMeetArray == NULL )
        {
            sSkipStoreIdx = aSkipCnt;
            sUsedStoreIdx = aUsedCnt;
        }
        else
        {
            sSkipStoreIdx = -1;
            
            for ( sStoreIdx = sBlockIdx; sStoreIdx < aUsedCnt; sStoreIdx++ )
            {
                if ( aMeetArray[sStoreIdx] == STL_TRUE )
                {
                    sSkipStoreIdx = sStoreIdx;
                    break;
                }
            }
            
            if ( sSkipStoreIdx == -1 )
            {
                break;
            }
            
            /**
             * 저장을 종료할 구간 검색
             */
            
            sUsedStoreIdx = aUsedCnt;
        
            for ( sStoreIdx = sSkipStoreIdx; sStoreIdx < aUsedCnt; sStoreIdx++ )
            {
                if ( aMeetArray[sStoreIdx] == STL_FALSE )
                {
                    sUsedStoreIdx = sStoreIdx;
                    break;
                }
            }
        }
        
        /**
         * 저장할 구간을 저장
         */
        
        KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aStoreBlockList,
                                             sSkipStoreIdx,
                                             sUsedStoreIdx );
        
        STL_TRY( smlInsertRecord( aInstantTable->mStmt,
                                  aInstantTable->mTableHandle,
                                  aStoreBlockList,
                                  NULL, /* unique violation */
                                  aRowBlock,
                                  SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        /**
         * 저장된 Row 의 갯수를 누적
         */
            
        aInstantTable->mRecCnt += (sUsedStoreIdx - sSkipStoreIdx);
        
        /**
         * 다음 구간 검색
         */
        
        sBlockIdx = sUsedStoreIdx;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnx */
