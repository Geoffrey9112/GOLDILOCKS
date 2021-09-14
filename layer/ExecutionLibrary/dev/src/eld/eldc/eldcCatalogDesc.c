/*******************************************************************************
 * eldcCatalogDesc.c
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
 * @file eldcCatalogDesc.c
 * @brief Cache for Catalog descriptor
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>

/**
 * @addtogroup eldcCatalogDesc
 * @{
 */


/**
 * @brief Hash Key 로부터 Hash Value 생성을 위한 함수 포인터
 * @param[in] aHashKey     Hash Key(Catalog ID)
 * @remarks
 */
stlUInt32  eldcHashFuncCatalogID( void * aHashKey )
{
    stlInt64  sCatalogID = 0;
    
    stlUInt32 sHashValue = 0;

    /**
     * Column ID 로부터 Hash Value 생성 
     */
    
    sCatalogID = *(stlInt64 *) aHashKey;
    
    sHashValue = ellGetHashValueOneID( sCatalogID );

    return sHashValue;
}


/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key(Catalog ID)
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncCatalogID( void * aHashKey, eldcHashElement * aHashElement )
{
    stlInt64               sCatalogID = 0;
    
    eldcHashDataCatalogID * sHashData    = NULL;

    /**
     * 입력된 Catalog ID 와 Hash Element 의 Catalog ID 를 비교 
     */
    
    sCatalogID  = *(stlInt64 *) aHashKey;
    
    sHashData    = (eldcHashDataCatalogID *) aHashElement->mHashData;

    if ( sHashData->mKeyCatalogID == sCatalogID )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}



/**
 * @brief Hash Key 로부터 Hash Value 생성을 위한 함수 포인터
 * @param[in] aHashKey     Hash Key (Catalog Name)
 * @remarks
 */
stlUInt32  eldcHashFuncCatalogName( void * aHashKey )
{
    stlChar * sKeyData = NULL;
    
    stlUInt32 sHashValue = 0;

    /**
     * Hash Key 로부터 Hash Value 생성 
     */
    
    sKeyData = (stlChar *) aHashKey;
    
    sHashValue = ellGetHashValueOneName( sKeyData );

    return sHashValue;
}

/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key (Catalog Name )
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncCatalogName( void * aHashKey, eldcHashElement * aHashElement )
{
    stlChar * sHashKey = NULL;
    
    eldcHashDataCatalogName * sHashData = NULL;

    /**
     * 입력된 Hash Key 와 Hash Element 의 Key 를 비교 
     */
    
    sHashKey  = (stlChar *) aHashKey;
    
    sHashData    = (eldcHashDataCatalogName *) aHashElement->mHashData;

    if ( stlStrcmp( sHashData->mKeyCatalogName, sHashKey ) == 0 )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}








/**
 * @brief Catalog Descriptor 를 위한 공간을 계산한다.
 * @param[in] aCatalogNameLen    Catalog Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeCatalogDesc( stlInt32 aCatalogNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(ellCatalogDesc) );
    
    if ( aCatalogNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aCatalogNameLen + 1);
    }

    return sSize;
}




/**
 * @brief Catalog Descriptor 로부터 Catalog Name 의 Pointer 위치를 계산
 * @param[in] aCatalogDesc   Catalog Descriptor
 * @remarks
 */
stlChar * eldcGetCatalogNamePtr( ellCatalogDesc * aCatalogDesc )
{
    return (stlChar *) aCatalogDesc + STL_ALIGN_DEFAULT( STL_SIZEOF(ellCatalogDesc) );
}



/**
 * @brief Catalog ID Hash Data 를 위한 공간을 계산한다.
 * @remarks
 */
stlInt32  eldcCalSizeHashDataCatalogID()
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF( eldcHashDataCatalogID ) );

    return sSize;
}


/**
 * @brief Catalog Name Hash Data 를 위한 공간을 계산한다.
 * @param[in]  aCatalogNameLen   Catalog Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeHashDataCatalogName( stlInt32 aCatalogNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataCatalogName) );

    if ( aCatalogNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aCatalogNameLen + 1);
    }
    
    return sSize;
}


/**
 * @brief Catalog Name Hash Data 로부터 Catalog Name 의 Pointer 위치를 계산
 * @param[in] aHashDataCatalogName   Catalog Name Hash Data
 * @remarks
 */
stlChar * eldcGetHashDataCatalogNamePtr( eldcHashDataCatalogName * aHashDataCatalogName )
{
    return (stlChar *) aHashDataCatalogName
        + STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataCatalogName) );
}







/**
 * @brief Catalog Descriptor 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aCatalogDesc   Catalog Descriptor
 * @param[out] aStringBuffer 출력 정보를 위한 String Buffer 공간
 * @remarks
 */
void eldcPrintCatalogDesc( void * aCatalogDesc, stlChar * aStringBuffer )
{
    stlInt32        sSize = 0;
    ellCatalogDesc * sCatalogDesc = (ellCatalogDesc *) aCatalogDesc;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Catalog Descriptor( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sCATALOG_ID: %ld, ",
                 aStringBuffer,
                 sCatalogDesc->mCatalogID );
    
    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sCATALOG_NAME: %s, ",
                         aStringBuffer,
                         sCatalogDesc->mCatalogName );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sCHARACTERSET_ID: %d ) ",
                 aStringBuffer,
                 sCatalogDesc->mCharacterSetID );

    /**
     * 문자열 끝 마무리 
     */
    
    if ( (sSize + 1) == ELDC_DUMP_PRINT_BUFFER_SIZE )
    {
        stlSnprintf( & aStringBuffer[ELDC_DUMP_PRINT_NULL_TERMINATE_POSITION],
                     ELDC_DUMP_PRINT_BUFFER_SIZE - ELDC_DUMP_PRINT_NULL_TERMINATE_POSITION,
                     ELDC_DUMP_PRINT_NULL_TERMINATE_STRING );
    }
    else
    {
        // nothing to do
    }
}


/**
 * @brief Catalog ID Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataCatalogID    Catalog ID 의 Hash Data
 * @param[out] aStringBuffer        출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataCatalogID( void * aHashDataCatalogID, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataCatalogID * sHashData = (eldcHashDataCatalogID *) aHashDataCatalogID;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Catalog ID Hash Data( " );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sKEY_CATALOG_ID: %ld )",
                         aStringBuffer,
                         sHashData->mKeyCatalogID );

    /**
     * 문자열 끝 마무리 
     */
    
    if ( (sSize + 1) == ELDC_DUMP_PRINT_BUFFER_SIZE )
    {
        stlSnprintf( & aStringBuffer[ELDC_DUMP_PRINT_NULL_TERMINATE_POSITION],
                     ELDC_DUMP_PRINT_BUFFER_SIZE - ELDC_DUMP_PRINT_NULL_TERMINATE_POSITION,
                     ELDC_DUMP_PRINT_NULL_TERMINATE_STRING );
    }
    else
    {
        // nothing to do
    }
}


/**
 * @brief Catalog Name Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataCatalogName  Catalog Name 의 Hash Data
 * @param[out] aStringBuffer        출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataCatalogName( void * aHashDataCatalogName, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataCatalogName * sHashData = (eldcHashDataCatalogName *) aHashDataCatalogName;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Catalog Name Hash Data( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_CATALOG_NAME: %s, ",
                 aStringBuffer,
                 sHashData->mKeyCatalogName );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sDATA_CATALOG_ID: %ld )",
                         aStringBuffer,
                         sHashData->mDataCatalogID );

    /**
     * 문자열 끝 마무리 
     */
    
    if ( (sSize + 1) == ELDC_DUMP_PRINT_BUFFER_SIZE )
    {
        stlSnprintf( & aStringBuffer[ELDC_DUMP_PRINT_NULL_TERMINATE_POSITION],
                     ELDC_DUMP_PRINT_BUFFER_SIZE - ELDC_DUMP_PRINT_NULL_TERMINATE_POSITION,
                     ELDC_DUMP_PRINT_NULL_TERMINATE_STRING );
    }
    else
    {
        // nothing to do
    }
}



/**
 * @brief SQL-Catalog Cache 를 위한 Dump Handle을 설정
 * @remarks
 */

void eldcSetSQLCatalogDumpHandle()
{
    /*
     * ELDC_OBJECTCACHE_CATALOG_ID
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_CATALOG_ID].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CATALOG_ID );
    gEldcObjectDump[ELDC_OBJECTCACHE_CATALOG_ID].mPrintHashData
        = eldcPrintHashDataCatalogID;
    gEldcObjectDump[ELDC_OBJECTCACHE_CATALOG_ID].mPrintObjectDesc
        = eldcPrintCatalogDesc;
    
    /*
     * ELDC_OBJECTCACHE_CATALOG_NAME
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_CATALOG_NAME].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CATALOG_NAME );
    gEldcObjectDump[ELDC_OBJECTCACHE_CATALOG_NAME].mPrintHashData
        = eldcPrintHashDataCatalogName;
    gEldcObjectDump[ELDC_OBJECTCACHE_CATALOG_NAME].mPrintObjectDesc
        = eldcPrintINVALID;
}





/**
 * @brief SQL-Catalog 객체를 위한 Cache 를 구축한다.
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildCatalogCache( smlTransId     aTransID,
                                smlStatement * aStmt,
                                ellEnv       * aEnv )
{
    stlInt64  sBucketCnt = 0;

    /*
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Hash 의 Bucket 개수를 결정하기 위한 레코드 개수 획득
     */

    STL_TRY( eldcGetTableRecordCount( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_CATALOG_NAME,
                                      & sBucketCnt,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * 소수에 해당하는 Hash Bucket 개수의 조정
     */

    sBucketCnt = ellGetPrimeBucketCnt( sBucketCnt );
    
    /**
     * Catalog ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_CATALOG_ID ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Catalog Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_CATALOG_NAME ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLCatalogDumpHandle();
    
    /**
     * SQL-Catalog Cache 구축 
     */

    STL_TRY( eldcBuildCacheSQLCatalog( aTransID,
                                       aStmt,
                                       aEnv )
             == STL_SUCCESS );

    /**
     * Catalog Handle 설정
     */

    STL_TRY( ellGetCatalogDictHandle( aTransID,
                                      ELL_CATALOG_HANDLE_ADDR(),
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief SQL-Catalog 을 위한 Cache 를 구축한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildCacheSQLCatalog( smlTransId       aTransID,
                                    smlStatement   * aStmt,
                                    ellEnv         * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;
    
    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    smlRid       sRowRid;
    smlScn       sRowScn;
    smlRowBlock  sRowBlock;
    eldMemMark   sMemMark;

    smlFetchInfo        sFetchInfo;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.CATALOG_NAME 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_CATALOG_NAME];

    /**
     * DEFINITION_SCHEMA.CATALOG_NAME 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_CATALOG_NAME,
                                     & sMemMark,
                                     & sTableValueList,
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
                                          sTableHandle,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          NULL,
                                          NULL,
                                          & sFetchInfo,
                                          NULL,
                                          & sIterator,         
                                          aEnv )
             == STL_SUCCESS );

    sBeginSelect = STL_TRUE;

    /**
     * Catalog Descriptor 를 Cache 에 추가한다.
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
        
        STL_TRY( eldcInsertCacheCatalog( aTransID,
                                         aStmt,
                                         sTableValueList,
                                         aEnv )
                 == STL_SUCCESS );

        /**
         * Start-Up 단계에서 Pending Memory 가 증가하지 않도록 Pending Operation 들을 수행한다.
         */

        STL_TRY( eldcRunPendOP4BuildCache( aTransID, aStmt, aEnv ) == STL_SUCCESS );
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
 * @brief Catalog Handle 을 얻는다.
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aViewSCN            View SCN
 * @param[out] aCatalogDictHandle  Catalog Dictionary Handle
 * @param[out] aExist              존재 여부 
 * @param[in]  aEnv                Environment
 * @remarks
 * Catalog 는 하나만 반드시 존재한다.
 */
stlStatus eldcGetCatalogHandle( smlTransId           aTransID,
                                smlScn               aViewSCN,
                                ellDictHandle      * aCatalogDictHandle,
                                stlBool            * aExist,
                                ellEnv             * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlInt64          sCatalogID = ELDT_DEFAULT_CATALOG_ID;
    stlBool sIsValid = STL_FALSE;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aCatalogDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
 
    /**
     * ELDC_OBJECTCACHE_CATALOG_ID Hash 로부터 검색
     */

    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CATALOG_ID ),
                                  & sCatalogID,
                                  eldcHashFuncCatalogID,
                                  eldcCompareFuncCatalogID,
                                  & sHashElement,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * Output 설정
     */
    
    if ( sHashElement == NULL )
    {
        /**
         * 존재하지 않음
         */
        
        *aExist = STL_FALSE;
    }
    else
    {
        /**
         * Version 유효성 검증
         */

        STL_TRY( eldcIsVisibleCache( aTransID,
                                     aViewSCN,
                                     & sHashElement->mVersionInfo,
                                     & sIsValid,
                                     aEnv )
                 == STL_SUCCESS );
        
        if ( sIsValid == STL_TRUE )
        {
            /**
             * Catalog Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aCatalogDictHandle,
                              ELL_OBJECT_CATALOG,
                              (void*)sHashElement,
                              STL_TRUE,  /* aRecentCheckable */
                              aEnv );
            
            *aExist = STL_TRUE;
        }
        else
        {
            /**
             * 유효한 Version 이 없음
             */

            *aExist = STL_FALSE;
        }
    }

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}







/**
 * @brief SQL Catalog 을 위한 Cache 정보를 추가한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aValueList        Data Value List
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcInsertCacheCatalog( smlTransId          aTransID,
                                  smlStatement      * aStmt,
                                  knlValueBlockList * aValueList,
                                  ellEnv            * aEnv )
{
    ellCatalogDesc   * sCatalogDesc = NULL;
    
    eldcHashElement * sHashElementCatalogID = NULL;
    eldcHashElement * sHashElementCatalogName = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Catalog ID를 위한 Catalog Descriptor 구성
     */
    
    STL_TRY( eldcMakeCatalogDesc( aTransID,
                                  aStmt,
                                  aValueList,
                                  & sCatalogDesc,
                                  aEnv )
             == STL_SUCCESS );
    
    /**
     * Catalog ID 를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementCatalogID( & sHashElementCatalogID,
                                           aTransID,
                                           sCatalogDesc,
                                           aEnv )
             == STL_SUCCESS );
                                                  
    /**
     * ELDC_OBJECTCACHE_CATALOG_ID Hash 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CATALOG_ID ),
                                    sHashElementCatalogID,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Catalog Name 을 위한 Hash Element 구성
     */

    STL_TRY( eldcMakeHashElementCatalogName( & sHashElementCatalogName,
                                             aTransID,
                                             sCatalogDesc,
                                             sHashElementCatalogID,
                                             aEnv )
             == STL_SUCCESS );
    
    /**
     * ELDC_OBJECTCACHE_CATALOG_NAME 에 추가 
     */

    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CATALOG_NAME ),
                                    sHashElementCatalogName,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}











/**
 * @brief 읽은 Value List 로부터 SQL Catalog 의 Catalog Descriptor 를 생성한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[in]  aValueList       Value List
 * @param[out] aCatalogDesc     Catalog Descriptor
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldcMakeCatalogDesc( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               knlValueBlockList  * aValueList,
                               ellCatalogDesc    ** aCatalogDesc,
                               ellEnv             * aEnv )
{
    stlInt32           sCatalogDescSize = 0;
    ellCatalogDesc   * sCatalogDesc = NULL;
    stlInt32           sCatalogNameLen = 0;
    
    dtlDataValue     * sDataValue = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCatalogDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Catalog Descriptor 를 위한 공간의 크기 계산
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_CATALOG_NAME,
                                   ELDT_Catalog_ColumnOrder_CATALOG_NAME );
    sCatalogNameLen = sDataValue->mLength;
    
    sCatalogDescSize = eldcCalSizeCatalogDesc( sCatalogNameLen );
    
    
    /**
     * Catalog Descriptor 를 위한 공간 할당
     */

    STL_TRY( eldAllocCacheMem( (void **) & sCatalogDesc,
                               sCatalogDescSize,
                               aEnv ) == STL_SUCCESS );
    
    /**
     * Catalog Descriptor 의 각 정보를 설정한다.
     */

    /*
     * mCatalogID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_CATALOG_NAME,
                                   ELDT_Catalog_ColumnOrder_CATALOG_ID );
    STL_ASSERT( DTL_IS_NULL( sDataValue ) != STL_TRUE );
    
    stlMemcpy( & sCatalogDesc->mCatalogID, sDataValue->mValue, sDataValue->mLength );
    STL_DASSERT( STL_SIZEOF(sCatalogDesc->mCatalogID) == sDataValue->mLength );
    
    /*
     * mCATALOG_NAME
     */

    sCatalogDesc->mCatalogName = eldcGetCatalogNamePtr( sCatalogDesc );
    
    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_CATALOG_NAME,
                                   ELDT_Catalog_ColumnOrder_CATALOG_NAME );
    STL_ASSERT( DTL_IS_NULL( sDataValue ) != STL_TRUE );
    
    stlMemcpy( sCatalogDesc->mCatalogName, sDataValue->mValue, sDataValue->mLength );

    sCatalogDesc->mCatalogName[sDataValue->mLength] = '\0';

    /*
     * mCharacterSetID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_CATALOG_NAME,
                                   ELDT_Catalog_ColumnOrder_CHARACTER_SET_ID );
    STL_ASSERT( DTL_IS_NULL( sDataValue ) != STL_TRUE );
    
    stlMemcpy( & sCatalogDesc->mCharacterSetID, sDataValue->mValue, sDataValue->mLength );
    STL_DASSERT( STL_SIZEOF(sCatalogDesc->mCharacterSetID) == sDataValue->mLength );

    /*
     * mCharLegnthUnit
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_CATALOG_NAME,
                                   ELDT_Catalog_ColumnOrder_CHAR_LENGTH_UNITS_ID );
    STL_ASSERT( DTL_IS_NULL( sDataValue ) != STL_TRUE );
    
    stlMemcpy( & sCatalogDesc->mCharLengthUnit, sDataValue->mValue, sDataValue->mLength );
    STL_DASSERT( STL_SIZEOF(sCatalogDesc->mCharLengthUnit) == sDataValue->mLength );
    
    /*
     * mTimeZoneInterval
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_CATALOG_NAME,
                                   ELDT_Catalog_ColumnOrder_TIME_ZONE_INTERVAL );
    STL_ASSERT( DTL_IS_NULL( sDataValue ) != STL_TRUE );
    
    stlMemcpy( & sCatalogDesc->mTimeZoneInterval, sDataValue->mValue, sDataValue->mLength );
    STL_DASSERT( STL_SIZEOF(sCatalogDesc->mTimeZoneInterval) == sDataValue->mLength );

    /**
     * Output 설정
     */

    *aCatalogDesc = sCatalogDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Catalog ID 를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aCatalogDesc    Catalog Descriptor
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementCatalogID( eldcHashElement   ** aHashElement,
                                        smlTransId           aTransID,
                                        ellCatalogDesc     * aCatalogDesc,
                                        ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32               sHashValue = 0;
    eldcHashDataCatalogID * sHashDataCatalogID = NULL;
    eldcHashElement       * sHashElement      = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCatalogDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataCatalogID();
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataCatalogID,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataCatalogID, 0x00, sHashDataSize );
    
    sHashDataCatalogID->mKeyCatalogID = aCatalogDesc->mCatalogID;

    /**
     * Hash Value 생성
     */
    
    sHashValue = eldcHashFuncCatalogID( (void *) & aCatalogDesc->mCatalogID );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataCatalogID,
                                  sHashValue,
                                  aCatalogDesc,
                                  aEnv )
             == STL_SUCCESS );
    
    /**
     * Output Parameter 설정 
     */
    
    *aHashElement = sHashElement;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    


/**
 * @brief Catalog Name 을 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aCatalogDesc    Catalog Descriptor
 * @param[in]  aHashElementID  Catalog ID로부터 구축된 Hash Element
 * @param[in]  aEnv            Environment 
 * @remarks
 */ 
stlStatus eldcMakeHashElementCatalogName( eldcHashElement   ** aHashElement,
                                          smlTransId           aTransID,
                                          ellCatalogDesc     * aCatalogDesc,
                                          eldcHashElement    * aHashElementID,
                                          ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32                 sHashValue = 0;
    eldcHashDataCatalogName * sHashDataCatalogName = NULL;
    eldcHashElement         * sHashElement        = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aCatalogDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataCatalogName( stlStrlen(aCatalogDesc->mCatalogName) );
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataCatalogName,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataCatalogName, 0x00, sHashDataSize );
    
    /* Catalog Name 을 복사할 위치 지정 */
    sHashDataCatalogName->mKeyCatalogName
        = eldcGetHashDataCatalogNamePtr( sHashDataCatalogName );
    
    stlStrcpy( sHashDataCatalogName->mKeyCatalogName,
               aCatalogDesc->mCatalogName );

    sHashDataCatalogName->mDataCatalogID = aCatalogDesc->mCatalogID;
    sHashDataCatalogName->mDataHashElement = aHashElementID;

    /**
     * Hash Value 생성
     */

    sHashValue = eldcHashFuncCatalogName( (void *) sHashDataCatalogName->mKeyCatalogName );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataCatalogName,
                                  sHashValue,
                                  NULL,   /* Object Descriptor 가 없음 */
                                  aEnv )
             == STL_SUCCESS );

    /**
     * Output Parameter 설정 
     */
    
    *aHashElement = sHashElement;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    



/** @} eldcCatalogDesc */
