/*******************************************************************************
 * eldcSchemaDesc.c
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
 * @file eldcSchemaDesc.c
 * @brief Cache for Schema descriptor
 */

#include <ell.h>
#include <eldt.h>                               
#include <eldc.h>

#include <eldDictionary.h>



/**
 * @addtogroup eldcSchemaDesc
 * @{
 */



/**
 * @brief Hash Key 로부터 Hash Value 생성을 위한 함수 포인터
 * @param[in] aHashKey     Hash Key(Schema ID)
 * @remarks
 */
stlUInt32  eldcHashFuncSchemaID( void * aHashKey )
{
    stlInt64  sSchemaID = 0;
    
    stlUInt32 sHashValue = 0;

    /**
     * Column ID 로부터 Hash Value 생성 
     */
    
    sSchemaID = *(stlInt64 *) aHashKey;
    
    sHashValue = ellGetHashValueOneID( sSchemaID );

    return sHashValue;
}



/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key(Schema ID)
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncSchemaID( void * aHashKey, eldcHashElement * aHashElement )
{
    stlInt64               sSchemaID = 0;
    
    eldcHashDataSchemaID * sHashData    = NULL;

    /**
     * 입력된 Schema ID 와 Hash Element 의 Schema ID 를 비교 
     */
    
    sSchemaID  = *(stlInt64 *) aHashKey;
    
    sHashData    = (eldcHashDataSchemaID *) aHashElement->mHashData;

    if ( sHashData->mKeySchemaID == sSchemaID )
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
 * @param[in] aHashKey     Hash Key (Schema Name)
 * @remarks
 */
stlUInt32  eldcHashFuncSchemaName( void * aHashKey )
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
 * @param[in] aHashKey        Hash Key (Schema Name )
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncSchemaName( void * aHashKey, eldcHashElement * aHashElement )
{
    stlChar * sHashKey = NULL;
    
    eldcHashDataSchemaName * sHashData = NULL;

    /**
     * 입력된 Hash Key 와 Hash Element 의 Key 를 비교 
     */
    
    sHashKey  = (stlChar *) aHashKey;
    
    sHashData    = (eldcHashDataSchemaName *) aHashElement->mHashData;

    if ( stlStrcmp( sHashData->mKeySchemaName, sHashKey ) == 0 )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}













/**
 * @brief Schema Descriptor 를 위한 공간을 계산한다.
 * @param[in] aSchemaNameLen    Schema Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeSchemaDesc( stlInt32 aSchemaNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(ellSchemaDesc) );
    
    if ( aSchemaNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aSchemaNameLen + 1);
    }

    return sSize;
}




/**
 * @brief Schema Descriptor 로부터 Schema Name 의 Pointer 위치를 계산
 * @param[in] aSchemaDesc   Schema Descriptor
 * @remarks
 */
stlChar * eldcGetSchemaNamePtr( ellSchemaDesc * aSchemaDesc )
{
    return (stlChar *) aSchemaDesc + STL_ALIGN_DEFAULT( STL_SIZEOF(ellSchemaDesc) );
}



/**
 * @brief Schema ID Hash Data 를 위한 공간을 계산한다.
 * @remarks
 */
stlInt32  eldcCalSizeHashDataSchemaID()
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF( eldcHashDataSchemaID ) );

    return sSize;
}


/**
 * @brief Schema Name Hash Data 를 위한 공간을 계산한다.
 * @param[in]  aSchemaNameLen   Schema Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeHashDataSchemaName( stlInt32 aSchemaNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataSchemaName) );

    if ( aSchemaNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aSchemaNameLen + 1);
    }
    
    return sSize;
}


/**
 * @brief Schema Name Hash Data 로부터 Schema Name 의 Pointer 위치를 계산
 * @param[in] aHashDataSchemaName   Schema Name Hash Data
 * @remarks
 */
stlChar * eldcGetHashDataSchemaNamePtr( eldcHashDataSchemaName * aHashDataSchemaName )
{
    return (stlChar *) aHashDataSchemaName
        + STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataSchemaName) );
}










/**
 * @brief Schema Descriptor 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aSchemaDesc   Schema Descriptor
 * @param[out] aStringBuffer 출력 정보를 위한 String Buffer 공간
 * @remarks
 */
void eldcPrintSchemaDesc( void * aSchemaDesc, stlChar * aStringBuffer )
{
    stlInt32        sSize = 0;
    ellSchemaDesc * sSchemaDesc = (ellSchemaDesc *) aSchemaDesc;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Schema Descriptor( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sOWNER_ID: %ld, ",
                 aStringBuffer,
                 sSchemaDesc->mOwnerID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sSCHEMA_ID: %ld, ",
                 aStringBuffer,
                 sSchemaDesc->mSchemaID );
    
    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sSCHEMA_NAME: %s )",
                         aStringBuffer,
                         sSchemaDesc->mSchemaName );


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
 * @brief Schema ID Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataSchemaID    Schema ID 의 Hash Data
 * @param[out] aStringBuffer        출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataSchemaID( void * aHashDataSchemaID, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataSchemaID * sHashData = (eldcHashDataSchemaID *) aHashDataSchemaID;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Schema ID Hash Data( " );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sKEY_SCHEMA_ID: %ld )",
                         aStringBuffer,
                         sHashData->mKeySchemaID );

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
 * @brief Schema Name Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataSchemaName  Schema Name 의 Hash Data
 * @param[out] aStringBuffer        출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataSchemaName( void * aHashDataSchemaName, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataSchemaName * sHashData = (eldcHashDataSchemaName *) aHashDataSchemaName;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Schema Name Hash Data( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_SCHEMA_NAME: %s, ",
                 aStringBuffer,
                 sHashData->mKeySchemaName );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sDATA_SCHEMA_ID: %ld )",
                         aStringBuffer,
                         sHashData->mDataSchemaID );

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
 * @brief SQL-Schema Cache 를 위한 Dump Handle을 설정
 * @remarks
 */

void eldcSetSQLSchemaDumpHandle()
{
    /*
     * ELDC_OBJECTCACHE_SCHEMA_ID
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_SCHEMA_ID].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SCHEMA_ID );
    gEldcObjectDump[ELDC_OBJECTCACHE_SCHEMA_ID].mPrintHashData
        = eldcPrintHashDataSchemaID;
    gEldcObjectDump[ELDC_OBJECTCACHE_SCHEMA_ID].mPrintObjectDesc
        = eldcPrintSchemaDesc;
    
    /*
     * ELDC_OBJECTCACHE_SCHEMA_NAME
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_SCHEMA_NAME].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SCHEMA_NAME );
    gEldcObjectDump[ELDC_OBJECTCACHE_SCHEMA_NAME].mPrintHashData
        = eldcPrintHashDataSchemaName;
    gEldcObjectDump[ELDC_OBJECTCACHE_SCHEMA_NAME].mPrintObjectDesc
        = eldcPrintINVALID;
}




/**
 * @brief SQL-Schema 객체를 위한 Cache 를 구축한다.
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildSchemaCache( smlTransId     aTransID,
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
                                      ELDT_TABLE_ID_SCHEMATA,
                                      & sBucketCnt,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * 소수에 해당하는 Hash Bucket 개수의 조정
     */

    sBucketCnt = ellGetPrimeBucketCnt( sBucketCnt );
    
    /**
     * Schema ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SCHEMA_ID ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Schema Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SCHEMA_NAME ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLSchemaDumpHandle();
    
    /**
     * SQL-Schema Cache 구축 
     */

    STL_TRY( eldcBuildCacheSQLSchema( aTransID,
                                      aStmt,
                                      aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief SQL-Schema 을 위한 Cache 를 구축한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildCacheSQLSchema( smlTransId       aTransID,
                                   smlStatement   * aStmt,
                                   ellEnv         * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;
    
    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    eldMemMark          sMemMark;

    smlFetchInfo          sFetchInfo;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.SCHEMATA 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_SCHEMATA];

    /**
     * DEFINITION_SCHEMA.SCHEMATA 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_SCHEMATA,
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
     * Schema Descriptor 를 Cache 에 추가한다.
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
        STL_TRY( eldcInsertCacheSchema( aTransID,
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
 * @brief Schema ID 를 이용해 Schema Handle 을 얻는다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aViewSCN          View SCN
 * @param[in]  aSchemaID         Schema ID
 * @param[out] aSchemaDictHandle Schema Dictionary Handle
 * @param[out] aExist            존재 여부
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus eldcGetSchemaHandleByID( smlTransId           aTransID,
                                   smlScn               aViewSCN,
                                   stlInt64             aSchemaID,
                                   ellDictHandle      * aSchemaDictHandle,
                                   stlBool            * aExist,
                                   ellEnv             * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlBool  sIsValid = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_SCHEMA_ID Hash 로부터 검색
     */

    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SCHEMA_ID ),
                                  & aSchemaID,
                                  eldcHashFuncSchemaID,
                                  eldcCompareFuncSchemaID,
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
         * 유효한 Version 획득
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
             * Schema Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aSchemaDictHandle,
                              ELL_OBJECT_SCHEMA,
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
 * @brief Schema Name 을 이용해 Schema Handle 을 얻는다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aViewSCN          View SCN
 * @param[in]  aSchemaName       Schema Name
 * @param[out] aSchemaDictHandle Schema Dictionary Handle
 * @param[out] aExist            존재 여부
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus eldcGetSchemaHandleByName( smlTransId           aTransID,
                                     smlScn               aViewSCN,
                                     stlChar            * aSchemaName,
                                     ellDictHandle      * aSchemaDictHandle,
                                     stlBool            * aExist,
                                     ellEnv             * aEnv )
{
    eldcHashElement        * sHashElement = NULL;

    eldcHashDataSchemaName * sHashData = NULL;

    stlBool sIsValid = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_SCHEMA_NAME Hash 로부터 검색
     */

    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SCHEMA_NAME ),
                                  aSchemaName,
                                  eldcHashFuncSchemaName,
                                  eldcCompareFuncSchemaName,
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
        sHashData = (eldcHashDataSchemaName *) sHashElement->mHashData;
        
        STL_TRY( eldcIsVisibleCache( aTransID,
                                     aViewSCN,
                                     & sHashData->mDataHashElement->mVersionInfo,
                                     & sIsValid,
                                     aEnv )
                 == STL_SUCCESS );
        
        if ( sIsValid == STL_TRUE )
        {
            /**
             * Schema Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aSchemaDictHandle,
                              ELL_OBJECT_SCHEMA,
                              (void*)sHashData->mDataHashElement,
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
 * @brief SQL Schema 을 위한 Cache 정보를 추가한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aValueList        Data Value List
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcInsertCacheSchema( smlTransId          aTransID,
                                 smlStatement      * aStmt,
                                 knlValueBlockList * aValueList,
                                 ellEnv            * aEnv )
{
    ellSchemaDesc   * sSchemaDesc = NULL;
    
    eldcHashElement * sHashElementSchemaID = NULL;
    eldcHashElement * sHashElementSchemaName = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Schema ID를 위한 Schema Descriptor 구성
     */
    
    STL_TRY( eldcMakeSchemaDesc( aTransID,
                                 aStmt,
                                 aValueList,
                                 & sSchemaDesc,
                                 aEnv )
             == STL_SUCCESS );
    
    /**
     * Schema ID 를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementSchemaID( & sHashElementSchemaID,
                                          aTransID,
                                          sSchemaDesc,
                                          aEnv )
             == STL_SUCCESS );
                                                  
    /**
     * ELDC_OBJECTCACHE_SCHEMA_ID Hash 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SCHEMA_ID ),
                                    sHashElementSchemaID,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Schema Name 을 위한 Hash Element 구성
     */

    STL_TRY( eldcMakeHashElementSchemaName( & sHashElementSchemaName,
                                            aTransID,
                                            sSchemaDesc,
                                            sHashElementSchemaID,
                                            aEnv )
             == STL_SUCCESS );
    
    /**
     * ELDC_OBJECTCACHE_SCHEMA_NAME 에 추가 
     */

    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SCHEMA_NAME ),
                                    sHashElementSchemaName,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    

/**
 * @brief Schema ID를 기준으로 SQL-Schema Cache 를 추가한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aSchemaID     Schema ID
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcInsertCacheSchemaBySchemaID( smlTransId          aTransID,
                                           smlStatement      * aStmt,
                                           stlInt64            aSchemaID,
                                           ellEnv            * aEnv )
{
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    stlBool             sFetchOne = STL_FALSE;
    
    stlBool             sBeginSelect = STL_FALSE;

    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    knlValueBlockList * sFilterColumn = NULL;

    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    void              * sIterator  = NULL;
    smlIteratorProperty sIteratorProperty;

    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.SCHEMATA 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_SCHEMATA];

    /**
     * DEFINITION_SCHEMA.SCHEMATA 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_SCHEMATA,
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
     * 조건 생성
     * WHERE SCHEMA_ID = aSchemaID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_SCHEMATA,
                                            ELDT_Schemata_ColumnOrder_SCHEMA_ID );

    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_SCHEMATA,
                                  sFilterColumn,
                                  & aSchemaID,
                                  STL_SIZEOF(stlInt64),
                                  & sIndexHandle,
                                  & sKeyCompList,
                                  & sIndexValueList,
                                  & sRangePred,
                                  & sRangeContext,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * SELECT 준비
     */

    STL_TRY( eldBeginForIndexAccessQuery( aStmt,
                                          sTableHandle,
                                          sIndexHandle,
                                          sKeyCompList,
                                          & sIteratorProperty,
                                          & sRowBlock,
                                          sTableValueList,
                                          sIndexValueList,
                                          sRangePred,
                                          sRangeContext,
                                          & sFetchInfo,
                                          & sFetchRecordInfo,
                                          & sIterator,
                                          aEnv )                    
             == STL_SUCCESS );
    
    sBeginSelect = STL_TRUE;

    /**
     * SQL-Schema Cache 정보를 추가한다.
     */

    sFetchOne = STL_FALSE;
    while (1)
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
            STL_TRY( eldcInsertCacheSchema( aTransID,
                                            aStmt,
                                            sTableValueList,
                                            aEnv )
                     == STL_SUCCESS );

            /* 한 건만 존재해야 한다. */
            STL_ASSERT( sFetchOne == STL_FALSE );
            sFetchOne = STL_TRUE;
            
            continue;
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
    



/**
 * @brief SQL Schema 를 위한 Cache 정보를 삭제한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aSchemaHandle     Schema Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcDeleteCacheSchema( smlTransId          aTransID,
                                 ellDictHandle     * aSchemaHandle,
                                 ellEnv            * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlInt64   sSchemaID = ELL_DICT_OBJECT_ID_NA;
    stlChar  * sSchemaName = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaHandle != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * ELDC_OBJECTCACHE_SCHEMA_ID Hash 에서 삭제 
     */

    sSchemaID = ellGetSchemaID( aSchemaHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SCHEMA_ID ),
                                  & sSchemaID,
                                  eldcHashFuncSchemaID,
                                  eldcCompareFuncSchemaID,
                                  & sHashElement,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SCHEMA_ID ),
                                    sHashElement,
                                    aEnv )
             == STL_SUCCESS );
                                    
    /**
     * ELDC_OBJECTCACHE_SCHEMA_NAME Hash 로부터 검색
     */

    sSchemaName = ellGetSchemaName( aSchemaHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SCHEMA_NAME ),
                                  sSchemaName,
                                  eldcHashFuncSchemaName,
                                  eldcCompareFuncSchemaName,
                                  & sHashElement,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SCHEMA_NAME ),
                                    sHashElement,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}









/**
 * @brief 읽은 Value List 로부터 SQL Schema 의 Schema Descriptor 를 생성한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[out] aSchemaDesc      Schema Descriptor
 * @param[in]  aValueList       Value List
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldcMakeSchemaDesc( smlTransId          aTransID,
                              smlStatement      * aStmt,
                              knlValueBlockList * aValueList,
                              ellSchemaDesc    ** aSchemaDesc,
                              ellEnv            * aEnv )
{
    stlInt32           sSchemaDescSize = 0;
    ellSchemaDesc    * sSchemaDesc = NULL;

    dtlDataValue     * sDataValue = NULL;

    stlInt32           sSchemaNameLen = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Schema Descriptor 를 위한 공간의 크기 계산
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SCHEMATA,
                                   ELDT_Schemata_ColumnOrder_SCHEMA_NAME );
    sSchemaNameLen = sDataValue->mLength;
    
    sSchemaDescSize = eldcCalSizeSchemaDesc( sSchemaNameLen );
    
    /**
     * Schema Descriptor 를 위한 공간 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sSchemaDesc,
                               sSchemaDescSize,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Schema Descriptor 의 각 정보를 설정한다.
     */

    /*
     * mOwnerID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SCHEMATA,
                                   ELDT_Schemata_ColumnOrder_OWNER_ID );

    STL_ASSERT( DTL_IS_NULL( sDataValue ) != STL_TRUE );
    
    stlMemcpy( & sSchemaDesc->mOwnerID,
               sDataValue->mValue,
               sDataValue->mLength );
    STL_ASSERT( STL_SIZEOF(sSchemaDesc->mOwnerID) == sDataValue->mLength );

    /*
     * mSchemaID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SCHEMATA,
                                   ELDT_Schemata_ColumnOrder_SCHEMA_ID );

    STL_ASSERT( DTL_IS_NULL( sDataValue ) != STL_TRUE );

    stlMemcpy( & sSchemaDesc->mSchemaID,
               sDataValue->mValue,
               sDataValue->mLength );
    STL_ASSERT( STL_SIZEOF(sSchemaDesc->mSchemaID) == sDataValue->mLength );

    /*
     * mSchemaName
     * - 메모리 공간 내에서의 위치 계산
     */

    sSchemaDesc->mSchemaName = eldcGetSchemaNamePtr( sSchemaDesc );

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SCHEMATA,
                                   ELDT_Schemata_ColumnOrder_SCHEMA_NAME );

    STL_ASSERT( DTL_IS_NULL( sDataValue ) != STL_TRUE );

    stlMemcpy( sSchemaDesc->mSchemaName,
               sDataValue->mValue,
               sDataValue->mLength );
    sSchemaDesc->mSchemaName[sDataValue->mLength] = '\0';

    /**
     * return 값 설정
     */
    
    *aSchemaDesc = sSchemaDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    

/**
 * @brief Schema ID 를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aSchemaDesc     Schema Descriptor
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementSchemaID( eldcHashElement   ** aHashElement,
                                       smlTransId           aTransID,
                                       ellSchemaDesc      * aSchemaDesc,
                                       ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32              sHashValue = 0;
    eldcHashDataSchemaID * sHashDataSchemaID = NULL;
    eldcHashElement      * sHashElement      = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataSchemaID();
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataSchemaID,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataSchemaID, 0x00, sHashDataSize );
    
    sHashDataSchemaID->mKeySchemaID = aSchemaDesc->mSchemaID;

    /**
     * Hash Value 생성
     */
    
    sHashValue = eldcHashFuncSchemaID( (void *) & aSchemaDesc->mSchemaID );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataSchemaID,
                                  sHashValue,
                                  aSchemaDesc,
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
 * @brief Schema Name 을 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aSchemaDesc     Schema Descriptor
 * @param[in]  aHashElementID  Schema ID로부터 구축된 Hash Element
 * @param[in]  aEnv            Environment 
 * @remarks
 */ 
stlStatus eldcMakeHashElementSchemaName( eldcHashElement   ** aHashElement,
                                         smlTransId           aTransID,
                                         ellSchemaDesc      * aSchemaDesc,
                                         eldcHashElement    * aHashElementID,
                                         ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32                sHashValue = 0;
    eldcHashDataSchemaName * sHashDataSchemaName = NULL;
    eldcHashElement        * sHashElement        = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataSchemaName( stlStrlen(aSchemaDesc->mSchemaName) );
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataSchemaName,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataSchemaName, 0x00, sHashDataSize );
    
    /* Schema Name 을 복사할 위치 지정 */
    sHashDataSchemaName->mKeySchemaName
        = eldcGetHashDataSchemaNamePtr( sHashDataSchemaName );
    
    stlStrcpy( sHashDataSchemaName->mKeySchemaName,
               aSchemaDesc->mSchemaName );

    sHashDataSchemaName->mDataSchemaID = aSchemaDesc->mSchemaID;
    sHashDataSchemaName->mDataHashElement = aHashElementID;

    /**
     * Hash Value 생성
     */

    sHashValue = eldcHashFuncSchemaName( (void *) sHashDataSchemaName->mKeySchemaName );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataSchemaName,
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
    




/** @} eldcSchemaDesc */
