/*******************************************************************************
 * qlncStatistic.c
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
 * @file qlncStatistic.c
 * @brief SQL Processor Layer SELECT statement Statistic
 */

#include <qll.h>
#include <qlDef.h>



/**
 * @addtogroup qlnc
 * @{
 */


/**
 * @brief Base Table의 통계정보를 수집한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncGatherBaseStatistic( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                   qllEnv                   * aEnv )
{
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );

    /* Table Statistic 수집 */
    STL_TRY( qlncGatherTableStat( aCreateNodeParamInfo,
                                  aEnv )
             == STL_SUCCESS );

    /* Index Statistic 수집 */
    if( ellGetTableType( ((qlncBaseTableNode*)(aCreateNodeParamInfo->mCandNode))->mTableInfo->mTableHandle ) == ELL_TABLE_TYPE_BASE_TABLE )
    {
        STL_TRY( qlncGatherIndexStat( aCreateNodeParamInfo,
                                      ((qlncBaseTableNode*)(aCreateNodeParamInfo->mCandNode))->mTableStat,
                                      aEnv )
                 == STL_SUCCESS );

        /**
         * Unique Index 로부터 Column Stat 을 구성
         */
        STL_TRY( qlncGatherColumnStatFromUniqueIndex( aCreateNodeParamInfo, aEnv ) == STL_SUCCESS );
    }
    else
    {
        ((qlncBaseTableNode*)(aCreateNodeParamInfo->mCandNode))->mIndexCount = 0;
        ((qlncBaseTableNode*)(aCreateNodeParamInfo->mCandNode))->mIndexStatArr = NULL;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Base Table의 Table 통계정보를 수집한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncGatherTableStat( qlncCreateNodeParamInfo   * aCreateNodeParamInfo,
                               qllEnv                   * aEnv )
{
    stlInt64              i;
    stlFloat64            sColumnLen;

    qlncBaseTableNode   * sCandBaseTable    = NULL;
    qlncTableStat       * sTableStat        = NULL;

    ellDictHandle       * sColumnHandle     = NULL;

    stlInt64              sPageCount;


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );


    sCandBaseTable = (qlncBaseTableNode*)(aCreateNodeParamInfo->mCandNode);


    /**
     * Table Statistic 할당
     */

    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncTableStat ),
                                (void**) &sTableStat,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTableStat->mIsAnalyzed = STL_FALSE;

    /* Table Page Count */
    if( ellGetTableType( sCandBaseTable->mTableInfo->mTableHandle ) == ELL_TABLE_TYPE_BASE_TABLE )
    {
        STL_TRY( smlGetUsedPageCount( sCandBaseTable->mTableInfo->mTablePhyHandle,
                                      &sPageCount,
                                      SML_ENV(aEnv) )
                 == STL_SUCCESS );
        if( sPageCount == 0 )
        {
            sTableStat->mPageCount = (stlFloat64)1.0;
        }
        else
        {
            sTableStat->mPageCount = (stlFloat64)sPageCount;
        }
    }
    else
    {
        sTableStat->mPageCount = QLNC_DEFAULT_PAGE_COUNT;
    }

    /* Column Count */
    sTableStat->mColumnCount =
        ellGetTableColumnCnt( sCandBaseTable->mTableInfo->mTableHandle );

    /* AVG Column Len & AVG Row Len */
    STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                STL_SIZEOF( qlncColumnStat ) * sTableStat->mColumnCount,
                                (void**) &(sTableStat->mColumnStat),
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sTableStat->mAvgRowLen = (stlFloat64)0.0;

    sColumnHandle = ellGetTableColumnDictHandle( sCandBaseTable->mTableInfo->mTableHandle );
    for( i = 0; i < sTableStat->mColumnCount; i++ )
    {
        sColumnLen = (stlFloat64)0.0;
        switch( ellGetColumnDBType( &sColumnHandle[i] ) )
        {
            case DTL_TYPE_BOOLEAN:
                sColumnLen = (stlFloat64)DTL_BOOLEAN_SIZE;
                break;
            case DTL_TYPE_NATIVE_SMALLINT:
                sColumnLen = (stlFloat64)DTL_NATIVE_SMALLINT_SIZE;
                break;
            case DTL_TYPE_NATIVE_INTEGER:
                sColumnLen = (stlFloat64)DTL_NATIVE_INTEGER_SIZE;
                break;
            case DTL_TYPE_NATIVE_BIGINT:
                sColumnLen = (stlFloat64)DTL_NATIVE_BIGINT_SIZE;
                break;
            case DTL_TYPE_NATIVE_REAL:
                sColumnLen = (stlFloat64)DTL_NATIVE_REAL_SIZE;
                break;
            case DTL_TYPE_NATIVE_DOUBLE:
                sColumnLen = (stlFloat64)DTL_NATIVE_DOUBLE_SIZE;
                break;
            case DTL_TYPE_FLOAT:
                sColumnLen = (stlFloat64)DTL_FLOAT_SIZE( ellGetColumnPrecision( &sColumnHandle[i] ) );
                sColumnLen *= (stlFloat64)0.6;
                break;
            case DTL_TYPE_NUMBER:
                sColumnLen = (stlFloat64)DTL_NUMERIC_SIZE( ellGetColumnPrecision( &sColumnHandle[i] ) );
                sColumnLen *= (stlFloat64)0.6;
                break;
            case DTL_TYPE_DECIMAL:
                STL_ASSERT( 0 );
                break;
            case DTL_TYPE_CHAR:
                sColumnLen = (stlFloat64)1.5 * (stlFloat64)ellGetColumnPrecision( &sColumnHandle[i] );
                break;
            case DTL_TYPE_VARCHAR:
                sColumnLen = (stlFloat64)1.5 * (stlFloat64)ellGetColumnPrecision( &sColumnHandle[i] );
                sColumnLen *= (stlFloat64)0.6;
                sColumnLen = ( sColumnLen > 128.0 ? 128.0 : sColumnLen );
                break;
            case DTL_TYPE_LONGVARCHAR:
                sColumnLen = (stlFloat64) 256.0;
                break;
            case DTL_TYPE_CLOB:
                STL_ASSERT( 0 );
                break;
            case DTL_TYPE_BINARY:
                sColumnLen = (stlFloat64)ellGetColumnPrecision( &sColumnHandle[i] );
                break;
            case DTL_TYPE_VARBINARY:
                sColumnLen = (stlFloat64)ellGetColumnPrecision( &sColumnHandle[i] );
                sColumnLen *= (stlFloat64)0.6;
                sColumnLen = ( sColumnLen > 128.0 ? 128.0 : sColumnLen );
                break;
            case DTL_TYPE_LONGVARBINARY:
                sColumnLen = (stlFloat64)ellGetColumnPrecision( &sColumnHandle[i] );
                sColumnLen *= (stlFloat64)0.6;
                sColumnLen = ( sColumnLen > 256.0 ? 256.0 : sColumnLen );
                break;
            case DTL_TYPE_BLOB:
                STL_ASSERT( 0 );
                break;
            case DTL_TYPE_DATE:
                sColumnLen = (stlFloat64)DTL_DATE_SIZE;
                break;
            case DTL_TYPE_TIME:
                sColumnLen = (stlFloat64)DTL_TIME_SIZE;
                break;
            case DTL_TYPE_TIMESTAMP:
                sColumnLen = (stlFloat64)DTL_TIMESTAMP_SIZE;
                break;
            case DTL_TYPE_TIME_WITH_TIME_ZONE:
                sColumnLen = (stlFloat64)DTL_TIMETZ_SIZE;
                break;
            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
                sColumnLen = (stlFloat64)DTL_TIMESTAMPTZ_SIZE;
                break;
            case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
                sColumnLen = (stlFloat64)DTL_INTERVAL_YEAR_TO_MONTH_SIZE;
                break;
            case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
                sColumnLen = (stlFloat64)DTL_INTERVAL_DAY_TO_SECOND_SIZE;
                break;
            case DTL_TYPE_ROWID:
                sColumnLen = (stlFloat64)DTL_ROWID_SIZE;
                break;
            default:
                STL_ASSERT( 0 );
                break;
        }

        sTableStat->mColumnStat[i].mNullable     = ellGetColumnNullable( &sColumnHandle[i] );
        sTableStat->mColumnStat[i].mIsUnique     = STL_FALSE;
        sTableStat->mColumnStat[i].mColumnCard   = QLNC_DEFAULT_COLUMN_CARDINALITY;
        sTableStat->mColumnStat[i].mAvgColumnLen = sColumnLen;

        sTableStat->mAvgRowLen += sColumnLen;
    }

    sTableStat->mRowCount = (stlFloat64)(sTableStat->mPageCount * QLNC_PAGE_SIZE) / sTableStat->mAvgRowLen;
    sTableStat->mAvgRowCountPerPage = sTableStat->mRowCount / sTableStat->mPageCount;


    /**
     * Table Statistic 연결
     */

    sCandBaseTable->mTableStat = sTableStat;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Base Table의 Unique Index 로부터 Column Cardinality 통계정보를 수집한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncGatherColumnStatFromUniqueIndex( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                                               qllEnv                   * aEnv )
{
    qlncBaseTableNode   * sCandBaseTable    = NULL;
    qlncTableStat       * sTableStat        = NULL;
    qlncIndexStat       * sIndexStat        = NULL;

    stlInt32              sIndexIdx = 0;
    stlInt32              sColumnIdx = 0;
    stlFloat64            sUniqueColumnCard = (stlFloat64)0.0;
    
    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );

    /**
     * 기본 정보 획득
     */
    
    sCandBaseTable = (qlncBaseTableNode*)(aCreateNodeParamInfo->mCandNode);
    sTableStat = sCandBaseTable->mTableStat;
    sIndexStat = sCandBaseTable->mIndexStatArr;

    /**
     * One Column Unique Index 일 경우 설정할 Column Cardinality 값을 결정
     */
    
    sUniqueColumnCard = sTableStat->mRowCount;
    if ( sUniqueColumnCard > QLNC_DEFAULT_COLUMN_CARDINALITY )
    {
        /* nothing to do */
    }
    else
    {
        /**
         * Unique Index 에 penalty 가 가지 않도록 함.
         */
        sUniqueColumnCard = QLNC_DEFAULT_COLUMN_CARDINALITY;
    }

    /**
     * - @todo Foreign Key 의 경우 참조하는 Unique Index 로부터 해당 정보를 구할 수 있음.
     */
    for ( sIndexIdx = 0; sIndexIdx < sCandBaseTable->mIndexCount; sIndexIdx++ )
    {
        if ( (sIndexStat[sIndexIdx].mIsUnique == STL_TRUE) &&
             (sIndexStat[sIndexIdx].mIndexKeyCount == 1 ) )
        {
            sColumnIdx = ellGetColumnIdx( & sIndexStat[sIndexIdx].mIndexKeyDesc[0].mKeyColumnHandle );
            sTableStat->mColumnStat[sColumnIdx].mColumnCard = sUniqueColumnCard;
            sTableStat->mColumnStat[sColumnIdx].mIsUnique = STL_TRUE;
        }
        else
        {
            /* nothing to do */
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Base Table의 Index 통계정보를 수집한다.
 * @param[in]   aCreateNodeParamInfo    Create Node Parameter Info
 * @param[in]   aTableStat              Table Statistic
 * @param[in]   aEnv                    Environment
 */
stlStatus qlncGatherIndexStat( qlncCreateNodeParamInfo  * aCreateNodeParamInfo,
                               qlncTableStat            * aTableStat,
                               qllEnv                   * aEnv )
{
    stlInt64              i;
    stlInt32              sIndexIdx;
    stlInt32              sIndexCount;

    qlncBaseTableNode   * sCandBaseTable        = NULL;
    qlncIndexStat       * sIndexStatArr         = NULL;
    ellDictHandle       * sIndexHandle          = NULL;

    stlInt32              sPrimaryKeyCount;
    stlInt32              sUniqueKeyCount;
    stlInt32              sForeignKeyCount;
    stlInt32              sUniqueIndexCount;
    stlInt32              sNonUniqueIndexCount;

    ellDictHandle       * sPrimaryKeyHandle     = NULL;
    ellDictHandle       * sUniqueKeyHandle      = NULL;
    ellDictHandle       * sForeignKeyHandle     = NULL;
    ellDictHandle       * sUniqueIndexHandle    = NULL;
    ellDictHandle       * sNonUniqueIndexHandle = NULL;

    stlInt64              sPageCount;

#define _QLNC_GET_INDEX_DEPTH( _aDepth, _aLeafPageCnt, _aAvgKeyCountPerPage )   \
    {                                                                           \
        errno = 0;                                                              \
                                                                                \
        sLogPageCnt = stlLog( ( _aLeafPageCnt ) );                              \
        STL_DASSERT( ( errno != EDOM ) && ( errno != ERANGE ) );                \
                                                                                \
        sLogKeyCnt = stlLog( ( _aAvgKeyCountPerPage ) );                        \
        STL_DASSERT( ( errno != EDOM ) && ( errno != ERANGE ) );                \
                                                                                \
        (_aDepth) = sLogPageCnt / sLogKeyCnt;                                   \
    }                                                                           \


    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aCreateNodeParamInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCreateNodeParamInfo->mCandNode->mNodeType == QLNC_NODE_TYPE_BASE_TABLE,
                        QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableStat != NULL, QLL_ERROR_STACK(aEnv) );


#define _QLNC_SET_INDEX_STAT( _aIndexStat, _aIndexType, _aIndexHandle, _aIsUnique ) \
    {                                                                               \
        stlInt32      _i;                                                           \
        stlFloat64    _sAvgKeyLen;                                                  \
                                                                                    \
        (_aIndexStat)->mIndexType = (_aIndexType);                                  \
        (_aIndexStat)->mIndexHandle = (_aIndexHandle);                              \
        (_aIndexStat)->mIndexPhyHandle = ellGetIndexHandle( (_aIndexHandle) );      \
        (_aIndexStat)->mIndexKeyCount = ellGetIndexKeyCount( (_aIndexHandle) );     \
        (_aIndexStat)->mIndexKeyDesc = ellGetIndexKeyDesc( (_aIndexHandle) );       \
        STL_TRY( smlGetUsedPageCount( (_aIndexStat)->mIndexPhyHandle,               \
                                      &sPageCount,                                  \
                                      SML_ENV(aEnv) )                               \
                 == STL_SUCCESS );                                                  \
        if( sPageCount == 0 )                                                       \
        {                                                                           \
            (_aIndexStat)->mLeafPageCount = (stlFloat64)1.0;                        \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            (_aIndexStat)->mLeafPageCount = (stlFloat64)sPageCount;                 \
        }                                                                           \
                                                                                    \
        _sAvgKeyLen = (stlFloat64)0.0;                                              \
        for( _i = 0; _i < (_aIndexStat)->mIndexKeyCount; _i++ )                     \
        {                                                                           \
            _sAvgKeyLen +=                                                          \
                aTableStat->mColumnStat[ ellGetColumnIdx( &((_aIndexStat)->mIndexKeyDesc[_i].mKeyColumnHandle )) ].mAvgColumnLen;  \
        }                                                                           \
                                                                                    \
        (_aIndexStat)->mAvgKeyCountPerPage =                                        \
            QLNC_PAGE_SIZE / (_sAvgKeyLen + (stlFloat64)16.0);                      \
        (_aIndexStat)->mDepth = stlLog( (_aIndexStat)->mLeafPageCount );            \
        (_aIndexStat)->mDepth /= stlLog( (_aIndexStat)->mAvgKeyCountPerPage );      \
        (_aIndexStat)->mIsUnique = (_aIsUnique);                                    \
        (_aIndexStat)->mIsAnalyzed = STL_FALSE;                                     \
    }


    sCandBaseTable = (qlncBaseTableNode*)(aCreateNodeParamInfo->mCandNode);


    /**
     * Index 정보 획득
     */

    /* Primary Key 정보 */
    STL_TRY( ellGetTablePrimaryKeyDictHandle( aCreateNodeParamInfo->mParamInfo.mTransID,
                                              aCreateNodeParamInfo->mParamInfo.mSQLStmt->mViewSCN,
                                              &QLL_CANDIDATE_MEM( aEnv ),
                                              sCandBaseTable->mTableInfo->mTableHandle,
                                              &sPrimaryKeyCount,
                                              &sPrimaryKeyHandle,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Unique Key 정보 */
    STL_TRY( ellGetTableUniqueKeyDictHandle( aCreateNodeParamInfo->mParamInfo.mTransID,
                                             aCreateNodeParamInfo->mParamInfo.mSQLStmt->mViewSCN,
                                             &QLL_CANDIDATE_MEM( aEnv ),
                                             sCandBaseTable->mTableInfo->mTableHandle,
                                             &sUniqueKeyCount,
                                             &sUniqueKeyHandle,
                                             ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Foreign Key 정보 */
    STL_TRY( ellGetTableForeignKeyDictHandle( aCreateNodeParamInfo->mParamInfo.mTransID,
                                              aCreateNodeParamInfo->mParamInfo.mSQLStmt->mViewSCN,
                                              &QLL_CANDIDATE_MEM( aEnv ),
                                              sCandBaseTable->mTableInfo->mTableHandle,
                                              &sForeignKeyCount,
                                              &sForeignKeyHandle,
                                              ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Unique Index 정보 */
    STL_TRY( ellGetTableUniqueIndexDictHandle( aCreateNodeParamInfo->mParamInfo.mTransID,
                                               aCreateNodeParamInfo->mParamInfo.mSQLStmt->mViewSCN,
                                               &QLL_CANDIDATE_MEM( aEnv ),
                                               sCandBaseTable->mTableInfo->mTableHandle,
                                               &sUniqueIndexCount,
                                               &sUniqueIndexHandle,
                                               ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Non Unique Index 정보 */
    STL_TRY( ellGetTableNonUniqueIndexDictHandle( aCreateNodeParamInfo->mParamInfo.mTransID,
                                                  aCreateNodeParamInfo->mParamInfo.mSQLStmt->mViewSCN,
                                                  &QLL_CANDIDATE_MEM( aEnv ),
                                                  sCandBaseTable->mTableInfo->mTableHandle,
                                                  &sNonUniqueIndexCount,
                                                  &sNonUniqueIndexHandle,
                                                  ELL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Index Count */
    sIndexCount = sPrimaryKeyCount + sUniqueKeyCount + sForeignKeyCount +
                  sUniqueIndexCount + sNonUniqueIndexCount;

    /* Index Statistic 설정 */
    if( sIndexCount > 0 )
    {
        /**
         * Index Statistic Array 할당
         */

        STL_TRY( knlAllocRegionMem( &QLL_CANDIDATE_MEM( aEnv ),
                                    STL_SIZEOF( qlncIndexStat ) * sIndexCount,
                                    (void**) &sIndexStatArr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sIndexIdx = 0;

        if( sPrimaryKeyCount > 0 )
        {
            for( i = 0; i < sPrimaryKeyCount; i++ )
            {
                sIndexHandle =
                    ellGetConstraintIndexDictHandle( &sPrimaryKeyHandle[i] );

                _QLNC_SET_INDEX_STAT( &sIndexStatArr[sIndexIdx],
                                      QLNC_INDEX_TYPE_PRIMARY_KEY,
                                      sIndexHandle,
                                      STL_TRUE );

                sIndexIdx++;
            }
        }

        if( sUniqueKeyCount > 0 )
        {
            for( i = 0; i < sUniqueKeyCount; i++ )
            {
                sIndexHandle =
                    ellGetConstraintIndexDictHandle( &sUniqueKeyHandle[i] );

                _QLNC_SET_INDEX_STAT( &sIndexStatArr[sIndexIdx],
                                      QLNC_INDEX_TYPE_UNIQUE_KEY,
                                      sIndexHandle,
                                      STL_TRUE );

                sIndexIdx++;
            }
        }

        if( sForeignKeyCount > 0 )
        {
            for( i = 0; i < sForeignKeyCount; i++ )
            {
                sIndexHandle =
                    ellGetConstraintIndexDictHandle( &sForeignKeyHandle[i] );

                _QLNC_SET_INDEX_STAT( &sIndexStatArr[sIndexIdx],
                                      QLNC_INDEX_TYPE_FOREIGN_KEY,
                                      sIndexHandle,
                                      STL_FALSE );

                sIndexIdx++;
            }
        }

        if( sUniqueIndexCount > 0 )
        {
            for( i = 0; i < sUniqueIndexCount; i++ )
            {
                sIndexHandle = &sUniqueIndexHandle[i];

                _QLNC_SET_INDEX_STAT( &sIndexStatArr[sIndexIdx],
                                      QLNC_INDEX_TYPE_UNIQUE_INDEX,
                                      sIndexHandle,
                                      STL_TRUE );

                sIndexIdx++;
            }
        }

        if( sNonUniqueIndexCount > 0 )
        {
            for( i = 0; i < sNonUniqueIndexCount; i++ )
            {
                sIndexHandle = &sNonUniqueIndexHandle[i];

                _QLNC_SET_INDEX_STAT( &sIndexStatArr[sIndexIdx],
                                      QLNC_INDEX_TYPE_NON_UNIQUE_INDEX,
                                      sIndexHandle,
                                      STL_FALSE );

                sIndexIdx++;
            }
        }
    }
    else
    {
        sIndexStatArr = NULL;
    }


    /**
     * Table Statistic 연결
     */

    sCandBaseTable->mIndexCount = sIndexCount;
    sCandBaseTable->mIndexStatArr = sIndexStatArr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} qlnc */
