/*******************************************************************************
 * eldcFxTableDesc.c
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
 * @file eldcFxTableDesc.c
 * @brief Cache for Fixed Table descriptor
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>

/**
 * @addtogroup eldcFxTableDesc
 * @{
 */




/**
 * @brief Fixed Table 을 위한 Fixed Cache 를 구축한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildCacheFxTable( smlTransId       aTransID,
                                 smlStatement   * aStmt,
                                 ellEnv         * aEnv )
{
    knlValueBlockList * sValueList = NULL;
    
    stlBool             sBeginSelect = STL_FALSE;
    
    void              * sFxTableHandle   = NULL;
    void              * sFxTableHeapDesc = NULL;
    knlFxColumnDesc   * sFixedColumnDesc = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;

    smlFetchInfo          sFetchInfo;

    eldMemMark  sMemMark;
    stlInt32    sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * X$TABLES 의 Heap Descriptor 을 획득
     */

    sFxTableHandle   = knlGetFxRelationHandle( ELDC_XTABLES_FXTABLE_NO );
    sFxTableHeapDesc = knlGetFxTableHeapDesc( sFxTableHandle );
    
    /**
     * X$TABLES 의 fixed column descriptor 를 획득
     */

    STL_TRY( knlGetFixedColumnDesc( sFxTableHeapDesc,
                                    & sFixedColumnDesc,
                                    KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * X$TABLES의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocFixedValueList( sFxTableHeapDesc,
                                     & sMemMark,
                                     & sValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForTableAccessQuery( aStmt,
                                          sFxTableHandle,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sValueList,
                                          NULL,
                                          NULL,
                                          & sFetchInfo,
                                          NULL,
                                          & sIterator,         
                                          aEnv )
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * Table Descriptor 를 Cache 에 추가한다.
     */

    while ( 1 )
    {
        /**
         * 레코드를 읽는다.
         */
        
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo,
                               aEnv )
                 == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }

        /**
         * Cache 정보를 추가
         */
        STL_TRY( eldcInsertCacheFxTable( aTransID,
                                         aStmt,
                                         sFixedColumnDesc,
                                         sValueList,
                                         aEnv )
                 == STL_SUCCESS );
    }
    
    /**
     * SELECT 종료
     */

    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}    


/**
 * @brief Fixed Table 을 위한 Cache 정보를 추가한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aFixedColumnDesc  Fixed Column Descriptor
 * @param[in] aValueList        Data Value List
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcInsertCacheFxTable( smlTransId          aTransID,
                                  smlStatement      * aStmt,
                                  knlFxColumnDesc   * aFixedColumnDesc,
                                  knlValueBlockList * aValueList,
                                  ellEnv            * aEnv )
{
    ellTableDesc      * sTableDesc              = NULL;
    eldcHashElement   * sHashElementTableID   = NULL;
    eldcHashElement   * sHashElementTableName = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFixedColumnDesc != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Table ID를 위한 Table Descriptor 구성
     */
    
    STL_TRY( eldcMakeFxTableDesc( aTransID,
                                  aStmt,
                                  aFixedColumnDesc,
                                  aValueList,
                                  & sTableDesc,
                                  aEnv )
             == STL_SUCCESS );
    
    /**
     * Table ID를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementFxTableID( & sHashElementTableID,
                                           aTransID,
                                           aStmt,
                                           sTableDesc,
                                           aEnv )
             == STL_SUCCESS );
                                                  
    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                    sHashElementTableID,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Table Name을 위한 Hash Element 구성
     */

    STL_TRY( eldcMakeHashElementFxTableName( & sHashElementTableName,
                                             aTransID,
                                             sTableDesc,
                                             sHashElementTableID,
                                             aEnv )
             == STL_SUCCESS );
    
    /**
     * ELDC_OBJECTCACHE_TABLE_NAME 에 추가 
     */

    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_NAME ),
                                    sHashElementTableName,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}






/**
 * @brief 읽은 Value List 로부터 Fixed Table 의 Table Descriptor 를 생성한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aFixedColumnDesc Fixed Column 의 정보 
 * @param[in]  aValueList       Value List
 * @param[out] aTableDesc       Table Descriptor
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldcMakeFxTableDesc( smlTransId          aTransID,
                               smlStatement      * aStmt,
                               knlFxColumnDesc   * aFixedColumnDesc,
                               knlValueBlockList * aValueList,
                               ellTableDesc     ** aTableDesc,
                               ellEnv            * aEnv )
{
    stlInt64           sTableID = ELL_DICT_OBJECT_ID_NA;
    stlInt32           sTableDescSize = 0;
    ellTableDesc     * sTableDesc = NULL;

    dtlDataValue     * sDataValue = NULL;

    stlInt32           sColumnCnt = 0;

    knlFxTableUsageType    sUsageType = KNL_FXTABLE_USAGE_MAX;
    ellFixedTableDepDesc * sFixedTableDesc = NULL;

    stlInt32           sTableNameLen = 0;
        
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableDesc != NULL,        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFixedColumnDesc != NULL , ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL      , ELL_ERROR_STACK(aEnv) );

    /**
     * Table 에 소속된 컬럼의 개수를 얻는다.
     * Fixed Table 의 경우 컬럼 개수 정보가 있다.
     * 그러나, 일반 테이블과의 일관성을 위해 Fixed Table Iterator 를 통해 개수를 직접 계산한다.
     */

    sDataValue = eldFindDataValue( aValueList,
                                   KNL_ANONYMOUS_TABLE_ID,
                                   KNL_FXTABLE_ORDER_TABLE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTableID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTableID) == sDataValue->mLength );
    }

    STL_TRY( eldcGetFxColumnCount( aTransID,
                                   aStmt,
                                   sTableID,
                                   & sColumnCnt,
                                   aEnv )
             == STL_SUCCESS );
    
    /**
     * Table Descriptor 를 위한 공간의 크기 계산
     */

    sDataValue = eldFindDataValue( aValueList,
                                   KNL_ANONYMOUS_TABLE_ID,
                                   KNL_FXTABLE_ORDER_TABLE_NAME );
    sTableNameLen = sDataValue->mLength;
    sTableDescSize = eldcCalSizeTableDesc( sTableNameLen,
                                           sColumnCnt,
                                           0,    /* aViewColumnLen */
                                           0 );  /* aViewQueryLen */

    /**
     * Table Descriptor 를 위한 공간 할당
     */

    STL_TRY( eldAllocCacheMem( (void **) & sTableDesc,
                               sTableDescSize,
                               aEnv )
             == STL_SUCCESS );
    
    /**
     * Table Descriptor 의 각 정보를 설정한다.
     */

    /*
     * mOwnerID
     */

    sTableDesc->mOwnerID = ELDT_AUTH_ID_SYSTEM;

    /*
     * mSchemaID
     */

    sTableDesc->mSchemaID = ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA;

    /*
     * mTableID
     */

    sTableDesc->mTableID = sTableID;

    /*
     * mTablespaceID
     */

    sTableDesc->mTablespaceID = ELL_DICT_TABLESPACE_ID_NA;

    /*
     * mPhysicalID
     */

    sTableDesc->mPhysicalID = ELL_DICT_OBJECT_ID_NA;
    
    /*
     * mTableName
     * - 메모리 공간 내에서의 위치 계산
     */

    sTableDesc->mTableName = eldcGetTableNamePtr( sTableDesc );

    sDataValue = eldFindDataValue( aValueList,
                                   KNL_ANONYMOUS_TABLE_ID,
                                   KNL_FXTABLE_ORDER_TABLE_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sTableDesc->mTableName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sTableDesc->mTableName[sDataValue->mLength] = '\0';
    }

    /*
     * mTableType
     */

    sDataValue = eldFindDataValue( aValueList,
                                   KNL_ANONYMOUS_TABLE_ID,
                                   KNL_FXTABLE_ORDER_USAGE_TYPE_VALUE );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sUsageType,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sUsageType) == sDataValue->mLength );
        
        switch ( sUsageType )
        {
            case KNL_FXTABLE_USAGE_FIXED_TABLE:
                {
                    sTableDesc->mTableType = ELL_TABLE_TYPE_FIXED_TABLE;
                    break;
                }
            case KNL_FXTABLE_USAGE_DUMP_TABLE:
                {
                    sTableDesc->mTableType = ELL_TABLE_TYPE_DUMP_TABLE;
                    break;
                }
            case KNL_FXTABLE_USAGE_MAX:
            default:
                {
                    STL_ASSERT(0);
                    break;
                }
        }
    }

    /*
     * mIsSuppPK
     */

    sTableDesc->mIsSuppPK = STL_FALSE;
    
    /*
     * mTableHandle
     * Fixed Table 의 Relation Handle 은 공유 메모리 상에서 관리되며
     * Fixed Table 의 ID 가 음수(-1) 부터 시작되어 이에 해당하는 Array Idx 로 Relation Handle 을 찾는다.
     */

    sTableDesc->mTableHandle = knlGetFxRelationHandle( (sTableID * -1) -1 );
    
    /*
     * mColumnCnt
     */

    sTableDesc->mColumnCnt = sColumnCnt;

    /*
     * mColumnDictHandleArray
     * Table 에 속한 Column Hash Element 를 설정함.
     */

    sTableDesc->mColumnDictHandle =
        eldcGetTableColumnDictHandlePtr( sTableDesc, sTableNameLen );

    STL_TRY( eldcSetFxColumnDictHandle( aTransID,
                                        aStmt,
                                        sTableDesc,
                                        aEnv )
             == STL_SUCCESS );
                                         
    /**
     * mTableType
     * Fixed Table 에 종속적인 descriptor 정보를 설정한다.
     */

    sFixedTableDesc = (ellFixedTableDepDesc *) & sTableDesc->mTableTypeDesc.mFixedTable;
    
    sDataValue = eldFindDataValue( aValueList,
                                   KNL_ANONYMOUS_TABLE_ID,
                                   KNL_FXTABLE_ORDER_STARTUP_PHASE_VALUE );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sFixedTableDesc->mStartupPhase,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sFixedTableDesc->mStartupPhase) == sDataValue->mLength );
    }
    
    /**
     * return 값 설정
     */
    
    *aTableDesc = sTableDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Fixed Table ID 를 위한  Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aStmt           Statement
 * @param[in]  aTableDesc      Table Descriptor
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementFxTableID( eldcHashElement   ** aHashElement,
                                        smlTransId           aTransID,
                                        smlStatement       * aStmt,
                                        ellTableDesc       * aTableDesc,
                                        ellEnv             * aEnv )
{
    eldcHashElement   * sHashElement   = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table ID 를 위한 Hash Element 생성
     */
    
    STL_TRY( eldcMakeHashElementTableID( & sHashElement,
                                         aTransID,
                                         aStmt,
                                         aTableDesc,
                                         aEnv )
             == STL_SUCCESS );

    /**
     * 참조) ticket #222
     * Fixed Cache 의 SCN 및 Transaction ID 정보는 Version 관리 대상이 아니다.
     * 
     * Object Cache 가 Transaction Commit 시 변경하는 내용을
     * Fixed Cache 는 Transaction, Statement 와 관계 없이 즉시 변경한다.
     */

    /* Hash Element 의 Version 정보 변경 */
    eldcSetStableVersionInfo( & sHashElement->mVersionInfo );

    /**
     * Output 설정
     */

    *aHashElement = sHashElement;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Table Name 를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aTableDesc      Table Descriptor
 * @param[in]  aHashElementID  Table ID 의 Hash Element
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementFxTableName( eldcHashElement   ** aHashElement,
                                          smlTransId           aTransID,
                                          ellTableDesc       * aTableDesc,
                                          eldcHashElement    * aHashElementID,
                                          ellEnv             * aEnv )
{
    eldcHashElement   * sHashElement   = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Table Name을 위한 Hash Element 생성
     */
    
    STL_TRY( eldcMakeHashElementTableName( & sHashElement,
                                           aTransID,
                                           aTableDesc,
                                           aHashElementID,
                                           aEnv )
             == STL_SUCCESS );

    /**
     * 참조) ticket #222
     * Fixed Cache 의 SCN 및 Transaction ID 정보는 Version 관리 대상이 아니다.
     * 
     * Object Cache 가 Transaction Commit 시 변경하는 내용을
     * Fixed Cache 는 Transaction, Statement 와 관계 없이 즉시 변경한다.
     */

    /* Hash Element 의 Version 정보 변경 */
    eldcSetStableVersionInfo( & sHashElement->mVersionInfo );

    /**
     * Output 설정
     */

    *aHashElement = sHashElement;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Fixed Table ID 를 이용해 Column 개수를 구한다.
 * @param[in]   aTransID     Transaction ID
 * @param[in]   aStmt        Statement
 * @param[in]   aTableID     Table ID
 * @param[out]  aColumnCnt   Column 개수
 * @param[in]   aEnv         Environment
 */
stlStatus eldcGetFxColumnCount( smlTransId           aTransID,
                                smlStatement       * aStmt,
                                stlInt64             aTableID,
                                stlInt32           * aColumnCnt,
                                ellEnv             * aEnv )
{
    stlInt32            sColumnCnt = 0;

    stlBool             sBeginSelect = STL_FALSE;
    
    void              * sFxTableHandle   = NULL;
    void              * sFxTableHeapDesc = NULL;
    knlFxColumnDesc   * sFixedColumnDesc = NULL;

    knlValueBlockList * sValueList = NULL;

    knlPredicateList    * sFilterPred = NULL;
    knlExprContextInfo  * sFilterContext = NULL;
    knlValueBlockList   * sFilterColumn = NULL;
    
    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;

    smlFetchInfo          sFetchInfo;
    knlExprEvalInfo       sFilterEvalInfo;

    eldMemMark  sMemMark;
    stlInt32    sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID < ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnCnt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * X$COLUMNS 의 Heap Descriptor 을 획득
     */

    sFxTableHandle   = knlGetFxRelationHandle( ELDC_XCOLUMNS_FXTABLE_NO );
    sFxTableHeapDesc = knlGetFxTableHeapDesc( sFxTableHandle );
    
    /**
     * X$COLUMNS 의 fixed column descriptor 를 획득
     */

    STL_TRY( knlGetFixedColumnDesc( sFxTableHeapDesc,
                                    & sFixedColumnDesc,
                                    KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * X$COLUMNS의 Target Value List와 Row Block 할당
     */

    STL_TRY( eldAllocFixedValueList( sFxTableHeapDesc,
                                     & sMemMark,
                                     & sValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Filter 생성
     * FROM X$COLUMNS WHERE TABLE_ID = aTableID;
     * KNL_FXCOLUMN_ORDER_TABLE_ID
     */

    sFilterColumn = ellFindColumnValueList( sValueList,
                                            KNL_ANONYMOUS_TABLE_ID,
                                            KNL_FXCOLUMN_ORDER_TABLE_ID );
    STL_TRY( eldMakeOneFilter( & sFilterPred,
                               & sFilterContext,
                               sFilterColumn,
                               KNL_BUILTIN_FUNC_IS_EQUAL,
                               & aTableID,
                               STL_SIZEOF(stlInt64),
                               aEnv )
             == STL_SUCCESS );
    
    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForTableAccessQuery( aStmt,
                                          sFxTableHandle,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sValueList,
                                          sFilterPred,
                                          sFilterContext,
                                          & sFetchInfo,
                                          & sFilterEvalInfo,
                                          & sIterator,         
                                          aEnv )
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * 컬럼 개수를 얻는다.
     */

    while ( 1 )
    {
        /**
         * 레코드를 읽는다.
         */
        
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo,
                               aEnv )
                 == STL_SUCCESS );
        
        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }
        else
        {
            /**
             * 컬럼 개수 증가
             */
            sColumnCnt++;
        }
    }
    
    /**
     * SELECT 종료
     */

    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    /**
     * Output 설정
     */
    
    *aColumnCnt = sColumnCnt;

    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}



/**
 * @brief Table Descriptor 에 column dictionary 정보를 채운다.
 * @param[in] aTransID         Transaction ID
 * @param[in] aStmt            Statement
 * @param[in] aTableDesc       Table Descriptor
 * @param[in] aEnv             Environment
 * @remarks
 */

stlStatus eldcSetFxColumnDictHandle( smlTransId           aTransID,
                                     smlStatement       * aStmt,
                                     ellTableDesc       * aTableDesc,
                                     ellEnv             * aEnv )
{
    stlBool             sExist = STL_FALSE;
    
    stlInt64            sColumnID = 0;
    stlInt32            sColumnIdx = 0;
    
    stlBool             sBeginSelect = STL_FALSE;
    
    void              * sFxTableHandle   = NULL;
    void              * sFxTableHeapDesc = NULL;
    knlFxColumnDesc   * sFixedColumnDesc = NULL;

    knlValueBlockList * sValueList = NULL;

    knlPredicateList    * sFilterPred = NULL;
    knlExprContextInfo  * sFilterContext = NULL;
    knlValueBlockList   * sFilterColumn = NULL;
    
    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    dtlDataValue      * sDataValue = NULL;
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;

    smlFetchInfo          sFetchInfo;
    knlExprEvalInfo       sFilterEvalInfo;

    eldMemMark  sMemMark;
    stlInt32    sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableDesc != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * X$COLUMNS 의 Heap Descriptor 을 획득
     */

    sFxTableHandle   = knlGetFxRelationHandle( ELDC_XCOLUMNS_FXTABLE_NO );
    sFxTableHeapDesc = knlGetFxTableHeapDesc( sFxTableHandle );
    
    /**
     * X$COLUMNS 의 fixed column descriptor 를 획득
     */

    STL_TRY( knlGetFixedColumnDesc( sFxTableHeapDesc,
                                    & sFixedColumnDesc,
                                    KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * X$COLUMNS의 Target Value List와 Row Block 할당
     */

    STL_TRY( eldAllocFixedValueList( sFxTableHeapDesc,
                                     & sMemMark,
                                     & sValueList,
                                     aEnv )
             == STL_SUCCESS );
    sState = 1;

    STL_TRY( smlInitSingleRowBlock( &sRowBlock,
                                    &sRowRid,
                                    &sRowScn,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );

    /**
     * Filter 생성
     * FROM X$COLUMNS WHERE TABLE_ID = aTableID;
     * KNL_FXCOLUMN_ORDER_TABLE_ID
     */

    sFilterColumn = ellFindColumnValueList( sValueList,
                                            KNL_ANONYMOUS_TABLE_ID,
                                            KNL_FXCOLUMN_ORDER_TABLE_ID );
    STL_TRY( eldMakeOneFilter( & sFilterPred,
                               & sFilterContext,
                               sFilterColumn,
                               KNL_BUILTIN_FUNC_IS_EQUAL,
                               & aTableDesc->mTableID,
                               STL_SIZEOF(stlInt64),
                               aEnv )
             == STL_SUCCESS );
    
    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForTableAccessQuery( aStmt,
                                          sFxTableHandle,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sValueList,
                                          sFilterPred,
                                          sFilterContext,
                                          & sFetchInfo,
                                          & sFilterEvalInfo,
                                          & sIterator,         
                                          aEnv )
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * Column Dictionary Handle 을 table descriptor 에 추가한다.
     */

    sColumnIdx = 0;
    while ( 1 )
    {
        /**
         * 레코드를 읽는다.
         */
        
        STL_TRY( eldFetchNext( sIterator,
                               & sFetchInfo,
                               aEnv )
                 == STL_SUCCESS );

        if( sFetchInfo.mIsEndOfScan == STL_TRUE )
        {
            break;
        }
        else
        {
            sDataValue = eldFindDataValue( sValueList,
                                           KNL_ANONYMOUS_TABLE_ID,
                                           KNL_FXCOLUMN_ORDER_COLUMN_ID );
            sColumnID = *(stlInt64 *) sDataValue->mValue;

            STL_TRY( eldcGetColumnHandleByID( aTransID,
                                              SML_MAXIMUM_STABLE_SCN,
                                              sColumnID,
                                              & aTableDesc->mColumnDictHandle[sColumnIdx],
                                              & sExist,
                                              aEnv )
                     == STL_SUCCESS );

            STL_ASSERT( sExist == STL_TRUE );
            STL_ASSERT( sColumnIdx < aTableDesc->mColumnCnt );

            sColumnIdx++;
        }
    }
    
    /**
     * SELECT 종료
     */

    sBeginSelect = STL_FALSE;
    STL_TRY( eldEndForQuery( sIterator,
                             aEnv )
             == STL_SUCCESS );

    /**
     * 사용한 메모리를 해제한다.
     */

    sState = 0;
    STL_TRY( eldFreeTableValueList( & sMemMark, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sBeginSelect == STL_TRUE )
    {
        (void) eldEndForQuery( sIterator,
                               aEnv );
    }

    switch (sState)
    {
        case 1:
            (void) eldFreeTableValueList( & sMemMark, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/** @} eldcFxTableDesc */
