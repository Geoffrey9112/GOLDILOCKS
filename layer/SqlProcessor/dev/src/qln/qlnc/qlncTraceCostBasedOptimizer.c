/*******************************************************************************
 * qlncTraceCostBasedOptimizer.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: qlncTraceCostBasedOptimizer.c 9859 2013-10-10 09:09:26Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file qlncTraceCostBasedOptimizer.c
 * @brief SQL Processor Layer Optimizer Trace Cost Based Optimizer
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Base Table에 대한 Cost 정보를 출력한다.
 * @param[in]   aRegionMem      Region Memory
 * @param[in]   aBaseTable      Base Table Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceCBOptBaseTable( knlRegionMem         * aRegionMem,
                                   qlncBaseTableNode    * aBaseTable,
                                   qllEnv               * aEnv )
{
    knlRegionMark         sRegionMemMark;
    qlncStringBlock     * sStringBlock  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBaseTable != NULL, QLL_ERROR_STACK(aEnv) );


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

    /**
     * Base Statistic 정보
     */

    STL_TRY( qlncTraceCBOptBaseStats( sStringBlock,
                                      aBaseTable,
                                      aEnv )
             == STL_SUCCESS );


    /**
     * Cost 계산 정보
     */

    STL_TRY( qlncTraceCBOptBaseCost( sStringBlock,
                                     aBaseTable,
                                     STL_FALSE,
                                     aEnv )
             == STL_SUCCESS );


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
 * @brief Base Statistic 정보를 출력한다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aBaseTable      Base Table Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceCBOptBaseStats( qlncStringBlock      * aStringBlock,
                                   qlncBaseTableNode    * aBaseTable,
                                   qllEnv               * aEnv )
{
    stlInt32          i;
//    stlInt32          j;
//    ellDictHandle   * sColumnHandle     = NULL;
    stlChar         * sIndexTypeName    = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBaseTable != NULL, QLL_ERROR_STACK(aEnv) );


    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "*****************************\n"
                 " BASE STATISTIC INFORMATIOIN\n"
                 "*****************************\n" )
             == STL_SUCCESS );

    /* Table Statistic */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "Table Statistic: %s\n",
                 (aBaseTable->mTableStat->mIsAnalyzed == STL_TRUE
                  ? "Analyzed" : "NOT Analyzed") )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "  Table: %s   Alias: %s\n",
                 ellGetTableName( aBaseTable->mTableInfo->mTableHandle ),
                 aBaseTable->mTableInfo->mRelationName.mTable )
             == STL_SUCCESS );

    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "    #Rows: %.0lf   #Pages: %.0lf   #AvgRowLen: %.4lf   #AvgRowCnt/Page: %.4lf\n",
                 aBaseTable->mTableStat->mRowCount,
                 aBaseTable->mTableStat->mPageCount,
                 aBaseTable->mTableStat->mAvgRowLen,
                 aBaseTable->mTableStat->mAvgRowCountPerPage )
             == STL_SUCCESS );

#if 0
    /* Columns */
    sColumnHandle = ellGetTableColumnDictHandle( aBaseTable->mTableInfo->mTableHandle );
    for( i = 0; i < aBaseTable->mTableStat->mColumnCount; i++ )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "  Column (#%d): %s(%s)\n",
                     i + 1,
                     ellGetColumnName( &sColumnHandle[i] ),
                     QLNC_TRACE_GET_DATA_TYPE_NAME( ellGetColumnDBType( &sColumnHandle[i] ) ) )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "    #AvgLen: %.4lf\n",
                     aBaseTable->mTableStat->mAvgColumnLen[i] )
                 == STL_SUCCESS );
    }
#endif

    /* Index Statistic */
    for( i = 0; i < aBaseTable->mIndexCount; i++ )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "Index Statistic: %s\n",
                     (aBaseTable->mIndexStatArr[i].mIsAnalyzed == STL_TRUE
                      ? "Analyzed" : "NOT Analyzed") )
                 == STL_SUCCESS );

        switch( aBaseTable->mIndexStatArr[i].mIndexType )
        {
            case QLNC_INDEX_TYPE_PRIMARY_KEY:
                sIndexTypeName = "Primary Key";
                break;
            case QLNC_INDEX_TYPE_UNIQUE_KEY:
                sIndexTypeName = "Unique Key";
                break;
            case QLNC_INDEX_TYPE_FOREIGN_KEY:
                sIndexTypeName = "Foreign Key";
                break;
            case QLNC_INDEX_TYPE_UNIQUE_INDEX:
                sIndexTypeName = "Unique Index";
                break;
            case QLNC_INDEX_TYPE_NON_UNIQUE_INDEX:
                sIndexTypeName = "Non Unique Index";
                break;
            default:
                STL_DASSERT( 0 );
                break;
        }

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "  Index: %s(%s)   KeyCnt: %d\n",
                     ellGetIndexName( aBaseTable->mIndexStatArr[i].mIndexHandle ),
                     sIndexTypeName,
                     aBaseTable->mIndexStatArr[i].mIndexKeyCount )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "    #Depth: %.0lf   #LeafPageCnt: %.0lf   #AVGKeyCnt/Page: %.4lf\n",
                     aBaseTable->mIndexStatArr[i].mDepth,
                     aBaseTable->mIndexStatArr[i].mLeafPageCount,
                     aBaseTable->mIndexStatArr[i].mAvgKeyCountPerPage )
                 == STL_SUCCESS );

#if 0
        for( j = 0; j < aBaseTable->mIndexStatArr[i].mIndexKeyCount; j++ )
        {
            sColumnHandle = &(aBaseTable->mIndexStatArr[i].mIndexKeyDesc[j].mKeyColumnHandle);
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "    Column (#%d): %s(%s)   #Ord: %s   #Nulls: %s\n",
                         j + 1,
                         ellGetColumnName( sColumnHandle ),
                         QLNC_TRACE_GET_DATA_TYPE_NAME( ellGetColumnDBType( sColumnHandle ) ),
                         (aBaseTable->mIndexStatArr[i].mIndexKeyDesc[j].mKeyOrdering
                          == ELL_INDEX_COLUMN_ASCENDING
                          ? "Asc" : "Desc"),
                         (aBaseTable->mIndexStatArr[i].mIndexKeyDesc[j].mKeyNullOrdering
                          == ELL_INDEX_COLUMN_NULLS_FIRST
                          ? "First" : "Last") )
                     == STL_SUCCESS );
        }
#endif
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Base Cost 정보를 출력한다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aBaseTable      Base Table Candidate Node
 * @param[in]   aIsCopied       Copied Base Table 여부
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceCBOptBaseCost( qlncStringBlock   * aStringBlock,
                                  qlncBaseTableNode * aBaseTable,
                                  stlBool             aIsCopied,
                                  qllEnv            * aEnv )
{
    stlInt32      i;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBaseTable != NULL, QLL_ERROR_STACK(aEnv) );


    if( aIsCopied == STL_FALSE )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "**************************\n"
                     " SINGLE TABLE ACCESS PATH\n"
                     "**************************\n" )
                 == STL_SUCCESS );

        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "Table: %s   Alias: %s\n",
                     ellGetTableName( aBaseTable->mTableInfo->mTableHandle ),
                     aBaseTable->mTableInfo->mRelationName.mTable )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "Access path analysis for %s[%s]\n",
                     ellGetTableName( aBaseTable->mTableInfo->mTableHandle ),
                     aBaseTable->mTableInfo->mRelationName.mTable )
                 == STL_SUCCESS );
    }

    /* Table Scan Cost */
    STL_TRY( qlncTraceCBOptTableScanCost( aStringBlock,
                                          aBaseTable->mTableScanCost,
                                          1,
                                          aEnv )
             == STL_SUCCESS );

    /* Index Scan Cost */
    for( i = 0; i < aBaseTable->mIndexScanCostCount; i++ )
    {
        STL_TRY( qlncTraceCBOptIndexScanCost( aStringBlock,
                                              &(aBaseTable->mIndexScanCostArr[i]),
                                              1,
                                              aEnv )
                 == STL_SUCCESS );
    }

    /* In Key Range Cost */
    for( i = 0; i < aBaseTable->mBestInKeyRangeCostCount; i++ )
    {
        if( aBaseTable->mBestInKeyRangeCostArr[i].mIndexHandle != NULL )
        {
            STL_TRY( qlncTraceCBOptIndexScanCost(
                         aStringBlock,
                         &(aBaseTable->mBestInKeyRangeCostArr[i]),
                         1,
                         aEnv )
                     == STL_SUCCESS );
        }
    }

    /* Rowid Scan Cost */
    STL_TRY( qlncTraceCBOptRowidScanCost( aStringBlock,
                                          aBaseTable->mRowidScanCost,
                                          1,
                                          aEnv )
             == STL_SUCCESS );

    /* Index Combine Cost */
    STL_TRY( qlncTraceCBOptIndexCombineCost( aStringBlock,
                                             aBaseTable->mIndexCombineCost,
                                             1,
                                             aEnv )
             == STL_SUCCESS );

    /* Best Scan Cost */
    if( aBaseTable->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_TABLE_SCAN )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%sBEST: Table Scan\n",
                     qlncTraceTreePrefix[1] )
                 == STL_SUCCESS );
    }
    else if( aBaseTable->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_SCAN )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%sBEST: Index Scan   Index: %s\n",
                     qlncTraceTreePrefix[1],
                     ellGetIndexName( ((qlncIndexScanCost*)(aBaseTable->mNodeCommon.mBestCost))->mIndexHandle ) )
                 == STL_SUCCESS );
    }
    else if( aBaseTable->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_ROWID_SCAN )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%sBEST: Rowid Scan\n",
                     qlncTraceTreePrefix[1] )
                 == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( aBaseTable->mNodeCommon.mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_COMBINE );
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%sBEST: Index Combine\n",
                     qlncTraceTreePrefix[1] )
                 == STL_SUCCESS );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Table Scan Cost 정보를 출력한다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aTableScanCost  Table Scan Cost
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceCBOptTableScanCost( qlncStringBlock      * aStringBlock,
                                       qlncTableScanCost    * aTableScanCost,
                                       stlInt32               aLevel,
                                       qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );


    if( aTableScanCost == NULL )
    {
        STL_THROW( RAMP_FINISH );
    }

    /* Access Path Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%sAccess Path: Table Scan\n",
                 qlncTraceTreePrefix[aLevel] )
             == STL_SUCCESS );

    /* Table Filter */
    if( aTableScanCost->mTableAndFilter != NULL )
    {
        QLNC_TRACE_APPEND_FILTER_INFO( aStringBlock,
                                       qlncTraceTreePrefix[aLevel+1],
                                       "Table Filter",
                                       aTableScanCost->mTableAndFilter,
                                       aEnv );
    }

    /* Cost */
    QLNC_TRACE_APPEND_COST_INFO( aStringBlock,
                                 qlncTraceTreePrefix[aLevel+1],
                                 &(aTableScanCost->mCostCommon),
                                 aEnv );


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index Scan Cost 정보를 출력한다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aIndexScanCost  Index Scan Cost
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceCBOptIndexScanCost( qlncStringBlock      * aStringBlock,
                                       qlncIndexScanCost    * aIndexScanCost,
                                       stlInt32               aLevel,
                                       qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );


    if( aIndexScanCost == NULL )
    {
        STL_THROW( RAMP_FINISH );
    }

    /* Access Path Name */
    if( aIndexScanCost->mListColMap != NULL )
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%sAccess Path: Index Scan (In Key Range)\n",
                     qlncTraceTreePrefix[aLevel] )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( qlncTraceAppendStringToStringBlock(
                     aStringBlock,
                     aEnv,
                     "%sAccess Path: Index Scan\n",
                     qlncTraceTreePrefix[aLevel] )
                 == STL_SUCCESS );
    }

    /* Index Scan Info */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%sIndex: %s   #ScanDirect: %s   #TableAccess: %s\n",
                 qlncTraceTreePrefix[aLevel+1],
                 ellGetIndexName( aIndexScanCost->mIndexHandle ),
                 (aIndexScanCost->mIsAsc == STL_TRUE
                  ? "Asc" : "Desc"),
                 (aIndexScanCost->mNeedTableScan == STL_TRUE
                  ? "Yes" : "No") )
             == STL_SUCCESS );

    /* Index Range Filter */
    if( aIndexScanCost->mIndexRangeAndFilter != NULL )
    {
        QLNC_TRACE_APPEND_FILTER_INFO( aStringBlock,
                                       qlncTraceTreePrefix[aLevel+1],
                                       "Index Range Filter",
                                       aIndexScanCost->mIndexRangeAndFilter,
                                       aEnv );
    }

    /* Index Key Filter */
    if( aIndexScanCost->mIndexKeyAndFilter != NULL )
    {
        QLNC_TRACE_APPEND_FILTER_INFO( aStringBlock,
                                       qlncTraceTreePrefix[aLevel+1],
                                       "Index Key Filter",
                                       aIndexScanCost->mIndexKeyAndFilter,
                                       aEnv );
    }

    /* Table Filter */
    if( aIndexScanCost->mTableAndFilter != NULL )
    {
        QLNC_TRACE_APPEND_FILTER_INFO( aStringBlock,
                                       qlncTraceTreePrefix[aLevel+1],
                                       "Table Filter",
                                       aIndexScanCost->mTableAndFilter,
                                       aEnv );
    }

    /* Cost */
    QLNC_TRACE_APPEND_COST_INFO( aStringBlock,
                                 qlncTraceTreePrefix[aLevel+1],
                                 &(aIndexScanCost->mCostCommon),
                                 aEnv );


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;

}


/**
 * @brief Rowid Scan Cost 정보를 출력한다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aRowidScanCost  Rowid Scan Cost
 * @param[in]   aLevel          Level
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceCBOptRowidScanCost( qlncStringBlock      * aStringBlock,
                                       qlncRowidScanCost    * aRowidScanCost,
                                       stlInt32               aLevel,
                                       qllEnv               * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );


    if( aRowidScanCost == NULL )
    {
        STL_THROW( RAMP_FINISH );
    }

    /* Access Path Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%sAccess Path: Rowid Scan\n",
                 qlncTraceTreePrefix[aLevel] )
             == STL_SUCCESS );

    /* Rowid Filter */
    if( aRowidScanCost->mRowidAndFilter != NULL )
    {
        QLNC_TRACE_APPEND_FILTER_INFO( aStringBlock,
                                       qlncTraceTreePrefix[aLevel+1],
                                       "Rowid Filter",
                                       aRowidScanCost->mRowidAndFilter,
                                       aEnv );
    }

    /* Table Filter */
    if( aRowidScanCost->mTableAndFilter != NULL )
    {
        QLNC_TRACE_APPEND_FILTER_INFO( aStringBlock,
                                       qlncTraceTreePrefix[aLevel+1],
                                       "Table Filter",
                                       aRowidScanCost->mTableAndFilter,
                                       aEnv );
    }

    /* Cost */
    QLNC_TRACE_APPEND_COST_INFO( aStringBlock,
                                 qlncTraceTreePrefix[aLevel+1],
                                 &(aRowidScanCost->mCostCommon),
                                 aEnv );


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index Combine Cost 정보를 출력한다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aIndexCombineCost   Index Combine Cost
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceCBOptIndexCombineCost( qlncStringBlock       * aStringBlock,
                                          qlncIndexCombineCost  * aIndexCombineCost,
                                          stlInt32                aLevel,
                                          qllEnv                * aEnv )
{
    stlInt32      i;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );


    if( aIndexCombineCost == NULL )
    {
        STL_THROW( RAMP_FINISH );
    }

    /* Access Path Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%sAccess Path: Index Combine\n",
                 qlncTraceTreePrefix[aLevel] )
             == STL_SUCCESS );

    /* Cost */
    QLNC_TRACE_APPEND_COST_INFO( aStringBlock,
                                 qlncTraceTreePrefix[aLevel+1],
                                 &(aIndexCombineCost->mCostCommon),
                                 aEnv );

    /* Index Scan Cost */
    for( i = 0; i < aIndexCombineCost->mDnfAndFilterCount; i++ )
    {
        STL_TRY( qlncTraceCBOptIndexScanCost( aStringBlock,
                                              &(aIndexCombineCost->mIndexScanCostArr[i]),
                                              1,
                                              aEnv )
                 == STL_SUCCESS );
    }


    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 초기 Join Order를 출력한다.
 * @param[in]   aRegionMem              Region Memory
 * @param[in]   aSeqNum                 Sequence Number
 * @param[in]   aInnerJoinNodeCount     Inner Join Candidate Node Count
 * @param[in]   aInnerJoinNodeInfoArr   Inner Join Candidate Node Array
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncTraceCBOptJoinOrder( knlRegionMem             * aRegionMem,
                                   stlInt32                   aSeqNum,
                                   stlInt32                   aInnerJoinNodeCount,
                                   qlncInnerJoinNodeInfo    * aInnerJoinNodeInfoArr,
                                   qllEnv                   * aEnv )
{
    stlInt32              i;
    knlRegionMark         sRegionMemMark;
    qlncStringBlock     * sStringBlock  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInnerJoinNodeCount > 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aInnerJoinNodeInfoArr != NULL, QLL_ERROR_STACK(aEnv) );


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

    /* Join Order */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 sStringBlock,
                 aEnv,
                 "Join Order[%d]: ",
                 aSeqNum )
             == STL_SUCCESS );

    for( i = 0; i < aInnerJoinNodeCount; i++ )
    {
        switch( aInnerJoinNodeInfoArr[i].mNode->mNodeType )
        {
            case QLNC_NODE_TYPE_BASE_TABLE:
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             sStringBlock,
                             aEnv,
                             "%s[%s]#%d  ",
                             ellGetTableName( ((qlncBaseTableNode*)(aInnerJoinNodeInfoArr[i].mNode))->mTableInfo->mTableHandle ),
                             ((qlncBaseTableNode*)(aInnerJoinNodeInfoArr[i].mNode))->mTableInfo->mRelationName.mTable,
                             i )
                         == STL_SUCCESS );
                break;

            case QLNC_NODE_TYPE_SUB_TABLE:
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             sStringBlock,
                             aEnv,
                             "[%s]#%d  ",
                             ((qlncSubTableNode*)(aInnerJoinNodeInfoArr[i].mNode))->mRelationName->mTable,
                             i )
                         == STL_SUCCESS );
                break;

            case QLNC_NODE_TYPE_JOIN_TABLE:
            case QLNC_NODE_TYPE_INNER_JOIN_TABLE:
            case QLNC_NODE_TYPE_OUTER_JOIN_TABLE:
            case QLNC_NODE_TYPE_SEMI_JOIN_TABLE:
            case QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE:
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             sStringBlock,
                             aEnv,
                             "[@Join]#%d  ",
                             i )
                         == STL_SUCCESS );
                break;

            case QLNC_NODE_TYPE_SORT_INSTANT:
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             sStringBlock,
                             aEnv,
                             "[@Sort]#%d  ",
                             i )
                         == STL_SUCCESS );
                break;

            case QLNC_NODE_TYPE_HASH_INSTANT:
                STL_TRY( qlncTraceAppendStringToStringBlock(
                             sStringBlock,
                             aEnv,
                             "[@Hash]#%d  ",
                             i )
                         == STL_SUCCESS );
                break;

            default:
                STL_DASSERT( 0 );
                break;
        }
    }


    STL_TRY( qlncTraceAppendStringToStringBlock(
                 sStringBlock,
                 aEnv,
                 "\n**********************************\n" )
             == STL_SUCCESS );

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
 * @brief Join Table Name을 출력한다.
 * @param[in]   aRegionMem              Region Memory
 * @param[in]   aNodeIdx                Node Index Number
 * @param[in]   aCurNode                Current Candidate Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncTraceCBOptJoinTableName( knlRegionMem     * aRegionMem,
                                       stlInt32           aNodeIdx,
                                       qlncNodeCommon   * aCurNode,
                                       qllEnv           * aEnv )
{
    knlRegionMark         sRegionMemMark;
    qlncStringBlock     * sStringBlock  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCurNode != NULL, QLL_ERROR_STACK(aEnv) );


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

    /* Join Node Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 sStringBlock,
                 aEnv,
                 "Joining: " )
             == STL_SUCCESS );

    switch( aCurNode->mNodeType )
    {
        case QLNC_NODE_TYPE_BASE_TABLE:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         sStringBlock,
                         aEnv,
                         "%s[%s]#%d  ",
                         ellGetTableName( ((qlncBaseTableNode*)aCurNode)->mTableInfo->mTableHandle ),
                         ((qlncBaseTableNode*)aCurNode)->mTableInfo->mRelationName.mTable,
                         aNodeIdx )
                     == STL_SUCCESS );
            break;

        case QLNC_NODE_TYPE_SUB_TABLE:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         sStringBlock,
                         aEnv,
                         "[%s]#%d  ",
                         ((qlncSubTableNode*)(aCurNode))->mRelationName->mTable,
                         aNodeIdx )
                     == STL_SUCCESS );
            break;

        case QLNC_NODE_TYPE_JOIN_TABLE:
        case QLNC_NODE_TYPE_INNER_JOIN_TABLE:
        case QLNC_NODE_TYPE_OUTER_JOIN_TABLE:
        case QLNC_NODE_TYPE_SEMI_JOIN_TABLE:
        case QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         sStringBlock,
                         aEnv,
                         "[@Join]#%d  ",
                         aNodeIdx )
                     == STL_SUCCESS );
            break;

        case QLNC_NODE_TYPE_SORT_INSTANT:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         sStringBlock,
                         aEnv,
                         "[@Sort]#%d  ",
                         aNodeIdx )
                     == STL_SUCCESS );
            break;

        case QLNC_NODE_TYPE_HASH_INSTANT:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         sStringBlock,
                         aEnv,
                         "[@Hash]#%d  ",
                         aNodeIdx )
                     == STL_SUCCESS );
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }

    STL_TRY( qlncTraceAppendStringToStringBlock(
                 sStringBlock,
                 aEnv,
                 "\n**********************************\n" )
             == STL_SUCCESS );

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
 * @brief Best Inner Join Cost를 출력한다.
 * @param[in]   aRegionMem              Region Memory
 * @param[in]   aAnalyzedNode           Analyzed Candidate Node
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncTraceCBOptBestJoinCost( knlRegionMem      * aRegionMem,
                                      qlncNodeCommon    * aAnalyzedNode,
                                      qllEnv            * aEnv )
{
    knlRegionMark         sRegionMemMark;
    qlncStringBlock     * sStringBlock  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAnalyzedNode != NULL, QLL_ERROR_STACK(aEnv) );


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

    /* Best 출력 */
    switch( aAnalyzedNode->mBestOptType )
    {
        case QLNC_BEST_OPT_TYPE_NESTED_LOOPS:
            QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                         "Best: Nested Loops Join\n      ",
                                         aAnalyzedNode->mBestCost,
                                         aEnv );
            break;

        case QLNC_BEST_OPT_TYPE_MERGE:
            QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                         "Best: Sort Merge Join\n      ",
                                         aAnalyzedNode->mBestCost,
                                         aEnv );
            break;

        case QLNC_BEST_OPT_TYPE_HASH:
            QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                         "Best: Hash Join\n      ",
                                         aAnalyzedNode->mBestCost,
                                         aEnv );
            break;

        case QLNC_BEST_OPT_TYPE_JOIN_COMBINE:
            QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                         "Best: Join Combine\n      ",
                                         aAnalyzedNode->mBestCost,
                                         aEnv );
            break;

        default:
            STL_DASSERT( 0 );
            break;
    }

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
 * @brief Nested Join Cost를 출력한다.
 * @param[in]   aRegionMem              Region Memory
 * @param[in]   aNestedJoinCost         Nested Join Cost
 * @param[in]   aSwitchNestedJoinCost   Switch Nested Join Cost
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncTraceCBOptNestedJoinCost( knlRegionMem        * aRegionMem,
                                        qlncNestedJoinCost  * aNestedJoinCost,
                                        qlncNestedJoinCost  * aSwitchNestedJoinCost,
                                        qllEnv              * aEnv )
{
    knlRegionMark         sRegionMemMark;
    qlncStringBlock     * sStringBlock  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


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

    /* Nested Join Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 sStringBlock,
                 aEnv,
                 "Nested Loops Join\n" )
             == STL_SUCCESS );

    if( aNestedJoinCost != NULL )
    {
        /* Outer Table */
        STL_TRY( qlncTraceCBOptJoinTableInfo( sStringBlock,
                                              "Outer",
                                              aNestedJoinCost->mLeftNode,
                                              aEnv )
                 == STL_SUCCESS );

        /* Inner Table */
        STL_TRY( qlncTraceCBOptJoinTableInfo( sStringBlock,
                                              "Inner",
                                              aNestedJoinCost->mRightNode,
                                              aEnv )
                 == STL_SUCCESS );

        /* Join Condition */
        if( aNestedJoinCost->mJoinCondition != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Join Filter",
                                           aNestedJoinCost->mJoinCondition,
                                           aEnv );
        }

        /* Join Filter */
        if( aNestedJoinCost->mJoinFilter != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Where Filter",
                                           aNestedJoinCost->mJoinFilter,
                                           aEnv );
        }


        /* Join Cost */
        QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                     "    NL Join: ",
                                     &(aNestedJoinCost->mCostCommon),
                                     aEnv );

        if( aNestedJoinCost->mRightNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
        {
            STL_TRY( qlncTraceCBOptBaseCost( sStringBlock,
                                             (qlncBaseTableNode*)(aNestedJoinCost->mRightNode),
                                             STL_TRUE,
                                             aEnv )
                     == STL_SUCCESS );
        }
        else if( aNestedJoinCost->mRightNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
        {
            if( (aNestedJoinCost->mCopiedBaseNode != NULL) &&
                (aNestedJoinCost->mCopiedBaseNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE) )
            {
                STL_TRY( qlncTraceCBOptBaseCost( sStringBlock,
                                                 (qlncBaseTableNode*)(aNestedJoinCost->mCopiedBaseNode),
                                                 STL_TRUE,
                                                 aEnv )
                         == STL_SUCCESS );
            }

            STL_TRY( qlncTraceCBOptSortInstantCost( sStringBlock,
                                                    (qlncSortInstantNode*)(aNestedJoinCost->mRightNode),
                                                    "For NL",
                                                    0,
                                                    aEnv )
                     == STL_SUCCESS );
        }
    }

    if( aSwitchNestedJoinCost != NULL )
    {
        /* Outer Table */
        STL_TRY( qlncTraceCBOptJoinTableInfo( sStringBlock,
                                              "Outer",
                                              aSwitchNestedJoinCost->mLeftNode,
                                              aEnv )
                 == STL_SUCCESS );

        /* Inner Table */
        STL_TRY( qlncTraceCBOptJoinTableInfo( sStringBlock,
                                              "Inner",
                                              aSwitchNestedJoinCost->mRightNode,
                                              aEnv )
                 == STL_SUCCESS );

        /* Join Condition */
        if( aSwitchNestedJoinCost->mJoinCondition != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Join Filter",
                                           aSwitchNestedJoinCost->mJoinCondition,
                                           aEnv );
        }

        /* Join Filter */
        if( aSwitchNestedJoinCost->mJoinFilter != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Where Filter",
                                           aSwitchNestedJoinCost->mJoinFilter,
                                           aEnv );
        }

        /* Join Cost */
        QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                     "    NL Join: ",
                                     &(aSwitchNestedJoinCost->mCostCommon),
                                     aEnv );

        if( aSwitchNestedJoinCost->mRightNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE )
        {
            STL_TRY( qlncTraceCBOptBaseCost( sStringBlock,
                                             (qlncBaseTableNode*)(aSwitchNestedJoinCost->mRightNode),
                                             STL_TRUE,
                                             aEnv )
                     == STL_SUCCESS );
        }
    }

    /* Best Nested Join Cost */
    if( aNestedJoinCost != NULL )
    {
        if( aSwitchNestedJoinCost != NULL )
        {
            if( QLNC_LESS_THAN_DOUBLE( aSwitchNestedJoinCost->mCostCommon.mTotalCost,
                                       aNestedJoinCost->mCostCommon.mTotalCost ) )
            {
                QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                             "  Best NL ",
                                             &(aSwitchNestedJoinCost->mCostCommon),
                                             aEnv );
            }
            else
            {
                QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                             "  Best NL ",
                                             &(aNestedJoinCost->mCostCommon),
                                             aEnv );
            }
        }
        else
        {
            QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                         "  Best NL ",
                                         &(aNestedJoinCost->mCostCommon),
                                         aEnv );
        }
    }
    else
    {
        if( aSwitchNestedJoinCost != NULL )
        {
            QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                         "  Best NL ",
                                         &(aSwitchNestedJoinCost->mCostCommon),
                                         aEnv );
        }
        else
        {
            /* Do Nothing */
        }
    }

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
 * @brief Merge Join Cost를 출력한다.
 * @param[in]   aRegionMem              Region Memory
 * @param[in]   aMergeJoinCost          Merge Join Cost
 * @param[in]   aSwitchMergeJoinCost    Switch Merge Join Cost
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncTraceCBOptMergeJoinCost( knlRegionMem         * aRegionMem,
                                       qlncMergeJoinCost    * aMergeJoinCost,
                                       qlncMergeJoinCost    * aSwitchMergeJoinCost,
                                       qllEnv               * aEnv )
{
    knlRegionMark         sRegionMemMark;
    qlncStringBlock     * sStringBlock  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


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

    /* Merge Join Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 sStringBlock,
                 aEnv,
                 "Sort Merge Join\n" )
             == STL_SUCCESS );

    if( aMergeJoinCost != NULL )
    {
        /* Outer Table */
        STL_TRY( qlncTraceCBOptJoinTableInfo( sStringBlock,
                                              "Outer",
                                              aMergeJoinCost->mLeftNode,
                                              aEnv )
                 == STL_SUCCESS );

        /* Inner Table */
        STL_TRY( qlncTraceCBOptJoinTableInfo( sStringBlock,
                                              "Inner",
                                              aMergeJoinCost->mRightNode,
                                              aEnv )
                 == STL_SUCCESS );

        /* Equi Join Condition */
        if( aMergeJoinCost->mEquiJoinCondition != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Equi-Join Filter",
                                           aMergeJoinCost->mEquiJoinCondition,
                                           aEnv );
        }

        /* Non Equi Join Condition */
        if( aMergeJoinCost->mNonEquiJoinCondition != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Non Equi-Join Filter",
                                           aMergeJoinCost->mNonEquiJoinCondition,
                                           aEnv );
        }

        /* Join Filter */
        if( aMergeJoinCost->mJoinFilter != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Where Filter",
                                           aMergeJoinCost->mJoinFilter,
                                           aEnv );
        }

        /* Join Cost */
        QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                     "    SM Join: ",
                                     &(aMergeJoinCost->mCostCommon),
                                     aEnv );

        if( aMergeJoinCost->mLeftNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
        {
            STL_TRY( qlncTraceCBOptSortInstantCost( sStringBlock,
                                                    (qlncSortInstantNode*)(aMergeJoinCost->mLeftNode),
                                                    "Left",
                                                    0,
                                                    aEnv )
                     == STL_SUCCESS );
        }

        if( aMergeJoinCost->mRightNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
        {
            STL_TRY( qlncTraceCBOptSortInstantCost( sStringBlock,
                                                    (qlncSortInstantNode*)(aMergeJoinCost->mRightNode),
                                                    "Right",
                                                    0,
                                                    aEnv )
                     == STL_SUCCESS );
        }
    }

    if( aSwitchMergeJoinCost != NULL )
    {
        /* Outer Table */
        STL_TRY( qlncTraceCBOptJoinTableInfo( sStringBlock,
                                              "Outer",
                                              aSwitchMergeJoinCost->mLeftNode,
                                              aEnv )
                 == STL_SUCCESS );

        /* Inner Table */
        STL_TRY( qlncTraceCBOptJoinTableInfo( sStringBlock,
                                              "Inner",
                                              aSwitchMergeJoinCost->mRightNode,
                                              aEnv )
                 == STL_SUCCESS );

        /* Equi Join Condition */
        if( aSwitchMergeJoinCost->mEquiJoinCondition != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Equi-Join Filter",
                                           aSwitchMergeJoinCost->mEquiJoinCondition,
                                           aEnv );
        }

        /* Non Equi Join Condition */
        if( aSwitchMergeJoinCost->mNonEquiJoinCondition != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Non Equi-Join Filter",
                                           aSwitchMergeJoinCost->mNonEquiJoinCondition,
                                           aEnv );
        }

        /* Join Filter */
        if( aSwitchMergeJoinCost->mJoinFilter != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Where Filter",
                                           aSwitchMergeJoinCost->mJoinFilter,
                                           aEnv );
        }

        /* Join Cost */
        QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                     "    SM Join: ",
                                     &(aSwitchMergeJoinCost->mCostCommon),
                                     aEnv );

        if( aSwitchMergeJoinCost->mLeftNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
        {
            STL_TRY( qlncTraceCBOptSortInstantCost( sStringBlock,
                                                    (qlncSortInstantNode*)(aSwitchMergeJoinCost->mLeftNode),
                                                    "Left",
                                                    0,
                                                    aEnv )
                     == STL_SUCCESS );
        }

        if( aSwitchMergeJoinCost->mRightNode->mNodeType == QLNC_NODE_TYPE_SORT_INSTANT )
        {
            STL_TRY( qlncTraceCBOptSortInstantCost( sStringBlock,
                                                    (qlncSortInstantNode*)(aSwitchMergeJoinCost->mRightNode),
                                                    "Right",
                                                    0,
                                                    aEnv )
                     == STL_SUCCESS );
        }
    }

    /* Best Merge Join Cost */
    if( aMergeJoinCost != NULL )
    {
        if( aSwitchMergeJoinCost != NULL )
        {
            if( QLNC_LESS_THAN_DOUBLE( aSwitchMergeJoinCost->mCostCommon.mTotalCost,
                                       aMergeJoinCost->mCostCommon.mTotalCost ) )
            {
                QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                             "  Best SM ",
                                             &(aSwitchMergeJoinCost->mCostCommon),
                                             aEnv );
            }
            else
            {
                QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                             "  Best SM ",
                                             &(aMergeJoinCost->mCostCommon),
                                             aEnv );
            }
        }
        else
        {
            QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                         "  Best SM ",
                                         &(aMergeJoinCost->mCostCommon),
                                         aEnv );
        }
    }
    else
    {
        if( aSwitchMergeJoinCost != NULL )
        {
            QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                         "  Best SM ",
                                         &(aSwitchMergeJoinCost->mCostCommon),
                                         aEnv );
        }
        else
        {
            /* Do Nothing */
        }
    }

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
 * @brief Hash Join Cost를 출력한다.
 * @param[in]   aRegionMem          Region Memory
 * @param[in]   aHashJoinCost       Hash Join Cost
 * @param[in]   aSwitchHashJoinCost Switch Hash Join Cost
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceCBOptHashJoinCost( knlRegionMem      * aRegionMem,
                                      qlncHashJoinCost  * aHashJoinCost,
                                      qlncHashJoinCost  * aSwitchHashJoinCost,
                                      qllEnv            * aEnv )
{
    knlRegionMark         sRegionMemMark;
    qlncStringBlock     * sStringBlock  = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


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

    /* Hash Join Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 sStringBlock,
                 aEnv,
                 "Hash Join\n" )
             == STL_SUCCESS );

    if( aHashJoinCost != NULL )
    {
        /* Outer Table */
        STL_TRY( qlncTraceCBOptJoinTableInfo( sStringBlock,
                                              "Outer",
                                              aHashJoinCost->mLeftNode,
                                              aEnv )
                 == STL_SUCCESS );

        /* Inner Table */
        STL_TRY( qlncTraceCBOptJoinTableInfo( sStringBlock,
                                              "Inner",
                                              aHashJoinCost->mRightNode,
                                              aEnv )
                 == STL_SUCCESS );

        /* Equi Join Condition */
        if( aHashJoinCost->mEquiJoinCondition != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Equi-Join Filter",
                                           aHashJoinCost->mEquiJoinCondition,
                                           aEnv );
        }

        /* Non Equi Join Condition */
        if( aHashJoinCost->mNonEquiJoinCondition != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Non Equi-Join Filter",
                                           aHashJoinCost->mNonEquiJoinCondition,
                                           aEnv );
        }

        /* Join Filter */
        if( aHashJoinCost->mJoinFilter != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Where Filter",
                                           aHashJoinCost->mJoinFilter,
                                           aEnv );
        }

        /* Join Cost */
        QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                     "    HASH Join: ",
                                     &(aHashJoinCost->mCostCommon),
                                     aEnv );

        STL_TRY( qlncTraceCBOptHashInstantCost( sStringBlock,
                                                (qlncHashInstantNode*)(aHashJoinCost->mRightNode),
                                                "Right",
                                                0,
                                                aEnv )
                 == STL_SUCCESS );
    }

    if( aSwitchHashJoinCost != NULL )
    {
        /* Outer Table */
        STL_TRY( qlncTraceCBOptJoinTableInfo( sStringBlock,
                                              "Outer",
                                              aSwitchHashJoinCost->mLeftNode,
                                              aEnv )
                 == STL_SUCCESS );

        /* Inner Table */
        STL_TRY( qlncTraceCBOptJoinTableInfo( sStringBlock,
                                              "Inner",
                                              aSwitchHashJoinCost->mRightNode,
                                              aEnv )
                 == STL_SUCCESS );

        /* Equi Join Condition */
        if( aSwitchHashJoinCost->mEquiJoinCondition != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Equi-Join Filter",
                                           aSwitchHashJoinCost->mEquiJoinCondition,
                                           aEnv );
        }

        /* Non Equi Join Condition */
        if( aSwitchHashJoinCost->mNonEquiJoinCondition != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Non Equi-Join Filter",
                                           aSwitchHashJoinCost->mNonEquiJoinCondition,
                                           aEnv );
        }

        /* Join Filter */
        if( aSwitchHashJoinCost->mJoinFilter != NULL )
        {
            QLNC_TRACE_APPEND_FILTER_INFO( sStringBlock,
                                           "    ",
                                           "Where Filter",
                                           aSwitchHashJoinCost->mJoinFilter,
                                           aEnv );
        }

        /* Join Cost */
        QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                     "    HASH Join: ",
                                     &(aSwitchHashJoinCost->mCostCommon),
                                     aEnv );

        STL_TRY( qlncTraceCBOptHashInstantCost( sStringBlock,
                                                (qlncHashInstantNode*)(aSwitchHashJoinCost->mRightNode),
                                                "Right",
                                                0,
                                                aEnv )
                 == STL_SUCCESS );
    }

    /* Best Hash Join Cost */
    if( aHashJoinCost != NULL )
    {
        if( aSwitchHashJoinCost != NULL )
        {
            if( QLNC_LESS_THAN_DOUBLE( aSwitchHashJoinCost->mCostCommon.mTotalCost,
                                       aHashJoinCost->mCostCommon.mTotalCost ) )
            {
                QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                             "  Best HASH ",
                                             &(aSwitchHashJoinCost->mCostCommon),
                                             aEnv );
            }
            else
            {
                QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                             "  Best HASH ",
                                             &(aHashJoinCost->mCostCommon),
                                             aEnv );
            }
        }
        else
        {
            QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                         "  Best HASH ",
                                         &(aHashJoinCost->mCostCommon),
                                         aEnv );
        }
    }
    else
    {
        if( aSwitchHashJoinCost != NULL )
        {
            QLNC_TRACE_APPEND_COST_INFO( sStringBlock,
                                         "  Best HASH ",
                                         &(aSwitchHashJoinCost->mCostCommon),
                                         aEnv );
        }
        else
        {
            /* Do Nothing */
        }
    }

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
 * @brief Sort Instant Node의 Cost 정보를 출력한다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aSortInstantNode    Sort Instant Node
 * @param[in]   aCont               Content
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceCBOptSortInstantCost( qlncStringBlock        * aStringBlock,
                                         qlncSortInstantNode    * aSortInstantNode,
                                         stlChar                * aCont,
                                         stlInt32                 aLevel,
                                         qllEnv                 * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSortInstantNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Instant Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%sSort Instant: %s\n",
                 qlncTraceTreePrefix[aLevel],
                 aCont )
             == STL_SUCCESS );

    /* Filter */
    if( aSortInstantNode->mFilter != NULL )
    {
        QLNC_TRACE_APPEND_FILTER_INFO( aStringBlock,
                                       qlncTraceTreePrefix[aLevel+1],
                                       "Filter",
                                       aSortInstantNode->mFilter,
                                       aEnv );
    }

    /* Cost */
    QLNC_TRACE_APPEND_COST_INFO( aStringBlock,
                                 qlncTraceTreePrefix[aLevel+1],
                                 &(aSortInstantNode->mSortInstantCost->mCostCommon),
                                 aEnv );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Hash Instant Node의 Cost 정보를 출력한다.
 * @param[in]   aStringBlock        String Block
 * @param[in]   aHashInstantNode    Hash Instant Node
 * @param[in]   aCont               Content
 * @param[in]   aLevel              Level
 * @param[in]   aEnv                Environment
 */
stlStatus qlncTraceCBOptHashInstantCost( qlncStringBlock        * aStringBlock,
                                         qlncHashInstantNode    * aHashInstantNode,
                                         stlChar                * aCont,
                                         stlInt32                 aLevel,
                                         qllEnv                 * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aHashInstantNode != NULL, QLL_ERROR_STACK(aEnv) );


    /* Instant Name */
    STL_TRY( qlncTraceAppendStringToStringBlock(
                 aStringBlock,
                 aEnv,
                 "%sHash Instant: %s\n",
                 qlncTraceTreePrefix[aLevel],
                 aCont )
             == STL_SUCCESS );

    /* Filter */
    if( aHashInstantNode->mFilter != NULL )
    {
        QLNC_TRACE_APPEND_FILTER_INFO( aStringBlock,
                                       qlncTraceTreePrefix[aLevel+1],
                                       "Filter",
                                       aHashInstantNode->mFilter,
                                       aEnv );
    }

    /* Cost */
    QLNC_TRACE_APPEND_COST_INFO( aStringBlock,
                                 qlncTraceTreePrefix[aLevel+1],
                                 &(aHashInstantNode->mHashInstantCost->mCostCommon),
                                 aEnv );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Table Info를 출력한다.
 * @param[in]   aStringBlock    String Block
 * @param[in]   aPosName        Position Name
 * @param[in]   aNode           Candidate Node
 * @param[in]   aEnv            Environment
 */
stlStatus qlncTraceCBOptJoinTableInfo( qlncStringBlock  * aStringBlock,
                                       stlChar          * aPosName,
                                       qlncNodeCommon   * aNode,
                                       qllEnv           * aEnv )
{
    stlChar     * sScanTypeName     = NULL;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aStringBlock != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPosName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNode != NULL, QLL_ERROR_STACK(aEnv) );


    switch( aNode->mNodeType )
    {
        case QLNC_NODE_TYPE_BASE_TABLE:
            if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_TABLE_SCAN )
            {
                sScanTypeName = "Table Scan";
            }
            else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_SCAN )
            {
                sScanTypeName = "Index Scan";
            }
            else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_ROWID_SCAN )
            {
                sScanTypeName = "Rowid Scan";
            }
            else
            {
                STL_DASSERT( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_INDEX_COMBINE );
                sScanTypeName = "Index Combine";
            }

            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "  %s Table: %s  Alias: %s  Access Path: %s  ",
                         aPosName,
                         ellGetTableName( ((qlncBaseTableNode*)aNode)->mTableInfo->mTableHandle ),
                         ((qlncBaseTableNode*)aNode)->mTableInfo->mRelationName.mTable,
                         sScanTypeName )
                     == STL_SUCCESS );
            QLNC_TRACE_APPEND_COST_INFO( aStringBlock,
                                         "",
                                         aNode->mBestCost,
                                         aEnv );
            break;

        case QLNC_NODE_TYPE_SUB_TABLE:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "  %s Table: Alias: %s  ",
                         aPosName,
                         ((qlncSubTableNode*)aNode)->mRelationName->mTable )
                     == STL_SUCCESS );
            QLNC_TRACE_APPEND_COST_INFO( aStringBlock,
                                         "",
                                         aNode->mBestCost,
                                         aEnv );
            break;

        case QLNC_NODE_TYPE_JOIN_TABLE:
            if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_INNER_JOIN )
            {
                STL_TRY( qlncTraceCBOptJoinTableInfo(
                             aStringBlock,
                             aPosName,
                             (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestInnerJoinTable),
                             aEnv )
                         == STL_SUCCESS );
            }
            else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_OUTER_JOIN )
            {
                STL_TRY( qlncTraceCBOptJoinTableInfo(
                             aStringBlock,
                             aPosName,
                             (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestOuterJoinTable),
                             aEnv )
                         == STL_SUCCESS );
            }
            else if( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_SEMI_JOIN )
            {
                STL_TRY( qlncTraceCBOptJoinTableInfo(
                             aStringBlock,
                             aPosName,
                             (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestSemiJoinTable),
                             aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( aNode->mBestOptType == QLNC_BEST_OPT_TYPE_ANTI_SEMI_JOIN );
                STL_TRY( qlncTraceCBOptJoinTableInfo(
                             aStringBlock,
                             aPosName,
                             (qlncNodeCommon*)(((qlncJoinTableNode*)aNode)->mBestAntiSemiJoinTable),
                             aEnv )
                         == STL_SUCCESS );
            }

            break;

        case QLNC_NODE_TYPE_INNER_JOIN_TABLE:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "  %s Table: Alias: @InnerJoin  ",
                         aPosName )
                     == STL_SUCCESS );
            QLNC_TRACE_APPEND_COST_INFO( aStringBlock,
                                         "",
                                         aNode->mBestCost,
                                         aEnv );
            break;

        case QLNC_NODE_TYPE_OUTER_JOIN_TABLE:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "  %s Table: Alias: @OuterJoin  ",
                         aPosName )
                     == STL_SUCCESS );
            QLNC_TRACE_APPEND_COST_INFO( aStringBlock,
                                         "",
                                         aNode->mBestCost,
                                         aEnv );
            break;

        case QLNC_NODE_TYPE_SEMI_JOIN_TABLE:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "  %s Table: Alias: @SemiJoin  ",
                         aPosName )
                     == STL_SUCCESS );
            QLNC_TRACE_APPEND_COST_INFO( aStringBlock,
                                         "",
                                         aNode->mBestCost,
                                         aEnv );
            break;

        case QLNC_NODE_TYPE_ANTI_SEMI_JOIN_TABLE:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "  %s Table: Alias: @AntiSemiJoin  ",
                         aPosName )
                     == STL_SUCCESS );
            QLNC_TRACE_APPEND_COST_INFO( aStringBlock,
                                         "",
                                         aNode->mBestCost,
                                         aEnv );
            break;

        case QLNC_NODE_TYPE_SORT_INSTANT:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "  %s Table: Alias: @SortInst  ",
                         aPosName )
                     == STL_SUCCESS );
            QLNC_TRACE_APPEND_COST_INFO( aStringBlock,
                                         "",
                                         aNode->mBestCost,
                                         aEnv );
            break;
        case QLNC_NODE_TYPE_HASH_INSTANT:
            STL_TRY( qlncTraceAppendStringToStringBlock(
                         aStringBlock,
                         aEnv,
                         "  %s Table: Alias: @HashInst  ",
                         aPosName )
                     == STL_SUCCESS );
            QLNC_TRACE_APPEND_COST_INFO( aStringBlock,
                                         "",
                                         aNode->mBestCost,
                                         aEnv );
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */
