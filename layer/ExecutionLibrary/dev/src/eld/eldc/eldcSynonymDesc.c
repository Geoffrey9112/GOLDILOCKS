/*******************************************************************************
 * eldcSynonymDesc.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: 
 *
 * NOTES
 *    
 *
 ******************************************************************************/


/**
 * @file eldcSynonymDesc.c
 * @brief Cache for Synonym descriptor
 */

#include <ell.h>
#include <eldt.h>                               
#include <eldc.h>

#include <eldDictionary.h>


/**
 * @addtogroup eldcSynonymDesc
 * @{
 */

/*******************************************************************************
 * Hash 함수 
 ******************************************************************************/

/**
 * @brief Hash Key 로부터 Hash Value 생성을 위한 함수 포인터
 * @param[in] aHashKey     Hash Key(Synonym ID)
 * @remarks
 */
stlUInt32  eldcHashFuncSynonymID( void * aHashKey )
{
    stlInt64  sSynonymID = 0;
    stlUInt32 sHashValue = 0;

    /**
     * Synonym ID 로부터 Hash Value 생성 
     */
    
    sSynonymID = *(stlInt64 *) aHashKey;
    
    sHashValue = ellGetHashValueOneID( sSynonymID );

    return sHashValue;
}

/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key(Synonym ID)
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool eldcCompareFuncSynonymID( void * aHashKey, eldcHashElement * aHashElement )
{
    stlInt64                sSynonymID = 0;
    eldcHashDataSynonymID * sHashData    = NULL;

    /**
     * 입력된 Synonym ID 와 Hash Element 의 Synonym ID 를 비교 
     */
    
    sSynonymID  = *(stlInt64 *) aHashKey;
    sHashData    = (eldcHashDataSynonymID *) aHashElement->mHashData;

    if ( sHashData->mKeySynonymID == sSynonymID )
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
 * @param[in] aHashKey     Hash Key (Schema ID, Synonym Name)
 * @remarks
 */
stlUInt32  eldcHashFuncSynonymName( void * aHashKey )
{
    eldcHashKeySynonymName * sKeyData = NULL;
    stlUInt32                sHashValue = 0;

    /**
     * Hash Key 로부터 Hash Value 생성 
     */
    
    sKeyData = (eldcHashKeySynonymName *) aHashKey;
    
    sHashValue = ellGetHashValueNameAndID( sKeyData->mSchemaID, sKeyData->mSynonymName );

    return sHashValue;
}

/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key (Schema ID, Synonym Name )
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool eldcCompareFuncSynonymName( void * aHashKey, eldcHashElement * aHashElement )
{
    eldcHashKeySynonymName  * sHashKey = NULL;
    
    eldcHashDataSynonymName * sHashData    = NULL;

    /**
     * 입력된 Hash Key 와 Hash Element 의 Key 를 비교 
     */
    
    sHashKey  = (eldcHashKeySynonymName *) aHashKey;
    
    sHashData = (eldcHashDataSynonymName *) aHashElement->mHashData;

    if ( ( sHashData->mKeySchemaID == sHashKey->mSchemaID ) &&
         ( stlStrcmp( sHashData->mKeySynonymName, sHashKey->mSynonymName ) == 0 ) )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}

/*******************************************************************************
 * Buffer Size 계산 함수 
 ******************************************************************************/

/**
 * @brief Synonym Descriptor 를 위한 공간을 계산한다.
 * @param[in] aSynonymNameLen       Synonym Name 의 길이
 * @param[in] aObjectSchemaNameLen  Object Schema Name 의 길이
 * @param[in] aObjectNameLen        Object Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeSynonymDesc( stlInt32 aSynonymNameLen,
                                  stlInt32 aObjectSchemaNameLen,
                                  stlInt32 aObjectNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(ellSynonymDesc) );
    
    if ( aSynonymNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aSynonymNameLen + 1);
    }

    if ( aObjectSchemaNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aObjectSchemaNameLen + 1);
    }

    if ( aObjectNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aObjectNameLen + 1);
    }

    return sSize;
}


/**
 * @brief Synonym Descriptor 로부터 Synonym Name 의 Pointer 위치를 계산
 * @param[in] aSynonymDesc          Synonym Descriptor
 * @remarks
 */
stlChar * eldcGetSynonymNamePtr( ellSynonymDesc * aSynonymDesc )
{
    return (stlChar *) aSynonymDesc + STL_ALIGN_DEFAULT( STL_SIZEOF(ellSynonymDesc) );
}


/**
 * @brief Synonym Descriptor 로부터 Object Schema Name 의 Pointer 위치를 계산
 * @param[in] aSynonymDesc          Synonym Descriptor
 * @param[in] aSynonymNameLen       Synonym Name Length
 * @remarks
 */
stlChar * eldcGetObjectSchemaNamePtr( ellSynonymDesc * aSynonymDesc,
                                      stlInt32         aSynonymNameLen )
{
    return (stlChar *) aSynonymDesc
        + STL_ALIGN_DEFAULT( STL_SIZEOF(ellSynonymDesc) )
        + STL_ALIGN_DEFAULT(aSynonymNameLen + 1);
}


/**
 * @brief Synonym Descriptor 로부터 Object Name 의 Pointer 위치를 계산
 * @param[in] aSynonymDesc          Synonym Descriptor
 * @param[in] aSynonymNameLen       Synonym Name Length
 * @param[in] aObjectSchemaNameLen  Object Schema Name Length 
 * @remarks
 */
stlChar * eldcGetObjectNamePtr( ellSynonymDesc * aSynonymDesc,
                                stlInt32         aSynonymNameLen,
                                stlInt32         aObjectSchemaNameLen )
{
    return (stlChar *) aSynonymDesc
        + STL_ALIGN_DEFAULT( STL_SIZEOF(ellSynonymDesc) )
        + STL_ALIGN_DEFAULT(aSynonymNameLen + 1)
        + STL_ALIGN_DEFAULT(aObjectSchemaNameLen + 1);
}


/**
 * @brief Synonym ID Hash Data 를 위한 공간을 계산한다.
 * @remarks
 */
stlInt32  eldcCalSizeHashDataSynonymID()
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF( eldcHashDataSynonymID ) );

    return sSize;
}

/**
 * @brief Synonym Name Hash Data 를 위한 공간을 계산한다.
 * @param[in]  aSynonymNameLen   Synonym Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeHashDataSynonymName( stlInt32 aSynonymNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataSynonymName) );

    if ( aSynonymNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aSynonymNameLen + 1);
    }
    
    return sSize;
}

/**
 * @brief Synonym Name Hash Data 로부터 Synonym Name 의 Pointer 위치를 계산
 * @param[in] aHashDataSynonymName   Synonym Name Hash Data
 * @remarks
 */
stlChar * eldcGetHashDataSynonymNamePtr( eldcHashDataSynonymName * aHashDataSynonymName )
{
    return (stlChar *) aHashDataSynonymName
        + STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataSynonymName) );
}

/*******************************************************************************
 * Dump Information
 ******************************************************************************/

/**
 * @brief Synonym Descriptor 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aSynonymDesc   Synonym Descriptor
 * @param[out] aStringBuffer  출력 정보를 위한 String Buffer 공간
 * @remarks
 */
void eldcPrintSynonymDesc( void * aSynonymDesc, stlChar * aStringBuffer )
{
    stlInt32         sSize = 0;
    ellSynonymDesc * sSynonymDesc = (ellSynonymDesc *) aSynonymDesc;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Synonym Descriptor( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sOWNER_ID: %ld, ",
                 aStringBuffer,
                 sSynonymDesc->mOwnerID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sSCHEMA_ID: %ld, ",
                 aStringBuffer,
                 sSynonymDesc->mSchemaID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sSYNONYM_ID: %ld, ",
                 aStringBuffer,
                 sSynonymDesc->mSynonymID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sSYNONYM_TABLE_ID: %ld, ",
                 aStringBuffer,
                 sSynonymDesc->mSynonymTableID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sSYNONYM_NAME: %s, ",
                 aStringBuffer,
                 sSynonymDesc->mSynonymName );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sOBJECT_SCHEMA_NAME: %s, ",
                 aStringBuffer,
                 sSynonymDesc->mObjectSchemaName );
    
    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sOBJECT_NAME: %s, ",
                         aStringBuffer,
                         sSynonymDesc->mObjectName );

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

void eldcPrintHashDataSynonymID( void * aHashDataSynonymID, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataSynonymID * sHashData = (eldcHashDataSynonymID *) aHashDataSynonymID;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Synonym ID Hash Data( " );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sKEY_SYNONYM_ID: %ld )",
                         aStringBuffer,
                         sHashData->mKeySynonymID );

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

void eldcPrintHashDataSynonymName( void * aHashDataSynonymName, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataSynonymName * sHashData = (eldcHashDataSynonymName *) aHashDataSynonymName;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Synonym Name Hash Data( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_SCHEMA_ID: %ld, ",
                 aStringBuffer,
                 sHashData->mKeySchemaID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_SYNONYM_NAME: %s, ",
                 aStringBuffer,
                 sHashData->mKeySynonymName );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sDATA_SYNONYM_ID: %ld )",
                         aStringBuffer,
                         sHashData->mDataSynonymID );

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


/*******************************************************************************
 * 초기화 마무리 함수 
 ******************************************************************************/

void eldcSetSQLSynonymDumpHandle()
{
    /*
     * ELDC_OBJECTCACHE_SYNONYM_ID
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_SYNONYM_ID].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SYNONYM_ID );
    gEldcObjectDump[ELDC_OBJECTCACHE_SYNONYM_ID].mPrintHashData
        = eldcPrintHashDataSynonymID;
    gEldcObjectDump[ELDC_OBJECTCACHE_SYNONYM_ID].mPrintObjectDesc
        = eldcPrintSynonymDesc;
    
    /*
     * ELDC_OBJECTCACHE_SYNONYM_NAME
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_SYNONYM_NAME].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SYNONYM_NAME );
    gEldcObjectDump[ELDC_OBJECTCACHE_SYNONYM_NAME].mPrintHashData
        = eldcPrintHashDataSynonymName;
    gEldcObjectDump[ELDC_OBJECTCACHE_SYNONYM_NAME].mPrintObjectDesc
        = eldcPrintINVALID;
}

/**
 * @brief SQL-Synonym 객체를 위한 Cache 를 구축한다.
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildSynonymCache( smlTransId     aTransID,
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
                                      ELDT_TABLE_ID_SYNONYMS,
                                      & sBucketCnt,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * 소수에 해당하는 Hash Bucket 개수의 조정
     */

    sBucketCnt = ellGetPrimeBucketCnt( sBucketCnt );
    
    /**
     * Synonym ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash(  ELL_OBJECT_CACHE_ADDR(  ELDC_OBJECTCACHE_SYNONYM_ID ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Synonym Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SYNONYM_NAME ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLSynonymDumpHandle();
    
    /**
     * SQL-Synonym Cache 구축 
     */

    STL_TRY( eldcBuildCacheSQLSynonym( aTransID,
                                       aStmt,
                                       aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief SQL-Synonym 을 위한 Cache 를 구축한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildCacheSQLSynonym( smlTransId       aTransID,
                                     smlStatement   * aStmt,
                                     ellEnv         * aEnv )
{
    stlBool               sBeginSelect = STL_FALSE;
    
    void                * sTableHandle = NULL;
    knlValueBlockList   * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    smlRid                sRowRid;
    smlScn                sRowScn;
    smlRowBlock           sRowBlock;
    eldMemMark            sMemMark;

    smlFetchInfo          sFetchInfo;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.SYNONYMS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_SYNONYMS];

    /**
     * DEFINITION_SCHEMA.SYNONYMS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_SYNONYMS,
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
     * Synonym Descriptor 를 Cache 에 추가한다.
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
        STL_TRY( eldcInsertCacheSynonym( aTransID,
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


/*******************************************************************************
 * 조회 함수 
 ******************************************************************************/

/**
 * @brief Schema ID 와 Synonym Name 을 이용해 Synonym Dictionary Handle 을 획득 
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             View SCN
 * @param[in]  aSchemaID            Schema ID
 * @param[in]  aSynonymName         Synonym Name
 * @param[out] aSynonymDictHandle   Synonym Dictionary Handle
 * @param[out] aExist               존재 여부 
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus eldcGetSynonymHandleByName( smlTransId           aTransID,
                                      smlScn               aViewSCN,
                                      stlInt64             aSchemaID,
                                      stlChar            * aSynonymName,
                                      ellDictHandle      * aSynonymDictHandle,
                                      stlBool            * aExist,
                                      ellEnv             * aEnv )
{
    eldcHashElement         * sHashElement = NULL;

    eldcHashDataSynonymName * sHashData = NULL;

    stlBool                   sIsValid = STL_FALSE;
    
    eldcHashKeySynonymName    sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeySynonymName) );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_SYNONYM_NAME Hash 로부터 검색
     */

    sHashKey.mSchemaID    = aSchemaID;
    sHashKey.mSynonymName = aSynonymName;
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SYNONYM_NAME ),
                                  & sHashKey,
                                  eldcHashFuncSynonymName,
                                  eldcCompareFuncSynonymName,
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
        sHashData = (eldcHashDataSynonymName *) sHashElement->mHashData;

        STL_TRY( eldcIsVisibleCache( aTransID,
                                     aViewSCN,
                                     & sHashData->mDataHashElement->mVersionInfo,
                                     & sIsValid,
                                     aEnv )
                 == STL_SUCCESS );
        
        if ( sIsValid == STL_TRUE )
        {
            /**
             * Synonym Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aSynonymDictHandle,
                              ELL_OBJECT_SYNONYM,
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
 * @brief Synonym ID 를 이용해 Synonym Dictionary Handle 을 획득 
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             View SCN
 * @param[in]  aSynonymID           Synonym ID
 * @param[out] aSynonymDictHandle   Synonym Dictionary Handle
 * @param[out] aExist               존재 여부 
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus eldcGetSynonymHandleByID( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aSynonymID,
                                    ellDictHandle      * aSynonymDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlBool sIsValid = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSynonymID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_Synonym_ID Hash 로부터 검색
     */

    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SYNONYM_ID ),
                                  & aSynonymID,
                                  eldcHashFuncSynonymID,
                                  eldcCompareFuncSynonymID,
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
             * Synonym Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aSynonymDictHandle,
                              ELL_OBJECT_SYNONYM,
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

/*******************************************************************************
 * 제어 함수 
 ******************************************************************************/


/**
 * @brief SQL Synonym 을 위한 Cache 정보를 추가한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aValueList        Data Value List
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcInsertCacheSynonym( smlTransId          aTransID,
                                  smlStatement      * aStmt,
                                  knlValueBlockList * aValueList,
                                  ellEnv            * aEnv )
{
    ellSessObjectMgr  * sObjMgr = ELL_SESS_OBJ(aEnv);
    ellSynonymDesc    * sSynonymDesc = NULL;
    
    eldcHashElement   * sHashElementSynonymID = NULL;
    eldcHashElement   * sHashElementSynonymName = NULL;
    eldcLocalCache    * sNewLocalCache = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * New Local Cache 공간 할당
     */
    
    STL_TRY( knlAllocDynamicMem( & sObjMgr->mMemSessObj,
                                 STL_SIZEOF(eldcLocalCache),
                                 (void **) & sNewLocalCache,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Synonym ID를 위한 Synonym Descriptor 구성
     */
    
    STL_TRY( eldcMakeSynonymDesc( aTransID,
                                  aStmt,
                                  aValueList,
                                  & sSynonymDesc,
                                  aEnv )
             == STL_SUCCESS );
    
    /**
     * Synonym ID 를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementSynonymID( & sHashElementSynonymID,
                                            aTransID,
                                            sSynonymDesc,
                                            aEnv )
             == STL_SUCCESS );
                                                  
    /**
     * ELDC_OBJECTCACHE_SYNONYM_ID Hash 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SYNONYM_ID ),
                                    sHashElementSynonymID,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Synonym Name 을 위한 Hash Element 구성
     */

    STL_TRY( eldcMakeHashElementSynonymName( & sHashElementSynonymName,
                                              aTransID,
                                              sSynonymDesc,
                                              sHashElementSynonymID,
                                              aEnv )
             == STL_SUCCESS );
    
    /**
     * ELDC_OBJECTCACHE_SYNONYM_NAME 에 추가 
     */

    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SYNONYM_NAME ),
                                    sHashElementSynonymName,
                                    aEnv )
             == STL_SUCCESS );
    
    /**
     * 객체가 변경되고 있음을 세션에 기록한다.
     */
    
    ELL_SESS_ENV(aEnv)->mLocalModifySeq += 1;
    
    eldcInsertLocalCache( sHashElementSynonymID, sNewLocalCache, aEnv );

    return STL_SUCCESS;

    STL_FINISH;

    if ( sNewLocalCache != NULL )
    {
        (void) knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                  (void *) sNewLocalCache,
                                  KNL_ENV(aEnv) );
        sNewLocalCache = NULL;
    }
    
    return STL_FAILURE;
}


/**
 * @brief Synonym ID를 기준으로 SQL-Synonym Cache 를 추가한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aSynonymID    Synonym ID
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcInsertCacheSynonymBySynonymID( smlTransId          aTransID,
                                             smlStatement      * aStmt,
                                             stlInt64            aSynonymID,
                                             ellEnv            * aEnv )
{
    smlRid                sRowRid;
    smlScn                sRowScn;
    smlRowBlock           sRowBlock;
    
    stlBool               sBeginSelect = STL_FALSE;

    void                * sTableHandle = NULL;
    knlValueBlockList   * sTableValueList = NULL;

    knlValueBlockList   * sFilterColumn = NULL;

    void                * sIndexHandle = NULL;
    knlValueBlockList   * sIndexValueList = NULL;
    knlKeyCompareList   * sKeyCompList = NULL;
    
    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;

    smlFetchInfo          sFetchInfo;
    smlFetchRecordInfo    sFetchRecordInfo;
    eldMemMark            sMemMark;

    stlInt32   sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.SYNONYM 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_SYNONYMS];

    /**
     * DEFINITION_SCHEMA.SYNONYMS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_SYNONYMS,
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
     * WHERE SYNONYM_ID = aSynonymID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_SYNONYMS,
                                            ELDT_Synonyms_ColumnOrder_SYNONYM_ID );

    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_SYNONYMS,
                                  sFilterColumn,
                                  & aSynonymID,
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
     * SQL-Synonym Cache 정보를 추가한다.
     */

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
            STL_TRY( eldcInsertCacheSynonym( aTransID,
                                             aStmt,
                                             sTableValueList,
                                             aEnv )
                     == STL_SUCCESS );
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
 * @brief SQL Synonym 를 위한 Cache 정보를 삭제한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aSynonymHandle    Synonym Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcDeleteCacheSynonym( smlTransId          aTransID,
                                  smlStatement      * aStmt,
                                  ellDictHandle     * aSynonymHandle,
                                  ellEnv            * aEnv )
{
    ellSessObjectMgr  * sObjMgr = ELL_SESS_OBJ(aEnv);
    
    eldcHashElement   * sHashElementID = NULL;
    eldcHashElement   * sHashElementName = NULL;

    eldcLocalCache    * sLocalCache;
    eldcLocalCache    * sNewLocalCache = NULL;
    
    stlInt64            sSynonymID = ELL_DICT_OBJECT_ID_NA;
    
    eldcHashKeySynonymName  sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeySynonymName) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * New Local Cache 공간 할당
     */
    
    STL_TRY( knlAllocDynamicMem( & sObjMgr->mMemSessObj,
                                 STL_SIZEOF(eldcLocalCache),
                                 (void **) & sNewLocalCache,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    /**
     * ELDC_OBJECTCACHE_SYNONYM_ID Hash 에서 삭제 
     */

    sSynonymID = ellGetSynonymID( aSynonymHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SYNONYM_ID ),
                                  & sSynonymID,
                                  eldcHashFuncSynonymID,
                                  eldcCompareFuncSynonymID,
                                  & sHashElementID,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SYNONYM_ID ),
                                    sHashElementID,
                                    aEnv )
             == STL_SUCCESS );
                                    
    /**
     * ELDC_OBJECTCACHE_SYNONYM_NAME Hash 에서 삭제 
     */

    sHashKey.mSchemaID    = ellGetSynonymSchemaID( aSynonymHandle );
    sHashKey.mSynonymName = ellGetSynonymName( aSynonymHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SYNONYM_NAME ),
                                  & sHashKey,
                                  eldcHashFuncSynonymName,
                                  eldcCompareFuncSynonymName,
                                  & sHashElementName,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SYNONYM_NAME ),
                                    sHashElementName,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * 객체가 변경되고 있음을 세션에 기록한다.
     */
    
    ELL_SESS_ENV(aEnv)->mLocalModifySeq += 1;
    
    eldcSearchLocalCache( sHashElementID, &sLocalCache, aEnv );

    if( sLocalCache == NULL )
    {
        eldcInsertLocalCache( sHashElementID, sNewLocalCache, aEnv );
        
        STL_TRY( eldcDeleteLocalCache( sNewLocalCache, aEnv ) == STL_SUCCESS );
    }
    else
    {
        
        STL_TRY( eldcDeleteLocalCache( sLocalCache, aEnv ) == STL_SUCCESS );
        
        STL_TRY( knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                    (void *) sNewLocalCache,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sNewLocalCache = NULL;
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    if ( sNewLocalCache != NULL )
    {
        (void) knlFreeDynamicMem( & sObjMgr->mMemSessObj,
                                  (void *) sNewLocalCache,
                                  KNL_ENV(aEnv) );
        sNewLocalCache = NULL;
    }
    
    return STL_FAILURE;
}



/*******************************************************************************
 * 멤버 함수 
 ******************************************************************************/


/**
 * @brief 읽은 Value List 로부터 SQL Synonym 의 Synonym Descriptor 를 생성한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[out] aSynonymDesc     Synonym Descriptor
 * @param[in]  aValueList       Value List
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldcMakeSynonymDesc( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               knlValueBlockList  * aValueList,
                               ellSynonymDesc    ** aSynonymDesc,
                               ellEnv             * aEnv )
{
    stlInt32           sSynonymDescSize = 0;
    ellSynonymDesc   * sSynonymDesc = NULL;

    dtlDataValue     * sDataValue = NULL;

    stlInt32           sSynonymNameLen = 0;
    stlInt32           sObjectSchemaNameLen = 0;
    stlInt32           sObjectNameLen = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Synonym Descriptor 를 위한 공간의 크기 계산
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SYNONYMS,
                                   ELDT_Synonyms_ColumnOrder_SYNONYM_NAME );
    sSynonymNameLen = sDataValue->mLength;

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SYNONYMS,
                                   ELDT_Synonyms_ColumnOrder_OBJECT_SCHEMA_NAME );
    sObjectSchemaNameLen = sDataValue->mLength;

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SYNONYMS,
                                   ELDT_Synonyms_ColumnOrder_OBJECT_NAME );
    sObjectNameLen = sDataValue->mLength;
    
    
    sSynonymDescSize = eldcCalSizeSynonymDesc( sSynonymNameLen,
                                               sObjectSchemaNameLen,
                                               sObjectNameLen );
    
    /**
     * Synonym Descriptor 를 위한 공간 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sSynonymDesc,
                               sSynonymDescSize,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Synonym Descriptor 의 각 정보를 설정한다.
     */

    /*
     * mOwnerID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SYNONYMS,
                                   ELDT_Synonyms_ColumnOrder_OWNER_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sSynonymDesc->mOwnerID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sSynonymDesc->mSchemaID) == sDataValue->mLength );
    }

    /*
     * mSchemaID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SYNONYMS,
                                   ELDT_Synonyms_ColumnOrder_SCHEMA_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sSynonymDesc->mSchemaID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sSynonymDesc->mSchemaID) == sDataValue->mLength );
    }
    
    /*
     * mSynonymID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SYNONYMS,
                                   ELDT_Synonyms_ColumnOrder_SYNONYM_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sSynonymDesc->mSynonymID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sSynonymDesc->mSynonymID) == sDataValue->mLength );
    }

    /*
     * mSynonymTableID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SYNONYMS,
                                   ELDT_Synonyms_ColumnOrder_SYNONYM_TABLE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sSynonymDesc->mSynonymTableID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sSynonymDesc->mSynonymTableID) == sDataValue->mLength );
    }
    
    /*
     * mSynonymName
     * - 메모리 공간 내에서의 위치 계산
     */

    sSynonymDesc->mSynonymName = eldcGetSynonymNamePtr( sSynonymDesc );

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SYNONYMS,
                                   ELDT_Synonyms_ColumnOrder_SYNONYM_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sSynonymDesc->mSynonymName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sSynonymDesc->mSynonymName[sDataValue->mLength] = '\0';
    }

    /*
     * mObjectSchemaName
     * - 메모리 공간 내에서의 위치 계산
     */

    sSynonymDesc->mObjectSchemaName = eldcGetObjectSchemaNamePtr( sSynonymDesc,
                                                                  sSynonymNameLen );
    
    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SYNONYMS,
                                   ELDT_Synonyms_ColumnOrder_OBJECT_SCHEMA_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sSynonymDesc->mObjectSchemaName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sSynonymDesc->mObjectSchemaName[sDataValue->mLength] = '\0';
    }

    /*
     * mObjectName
     * - 메모리 공간 내에서의 위치 계산
     */

    sSynonymDesc->mObjectName = eldcGetObjectNamePtr( sSynonymDesc,
                                                      sSynonymNameLen,
                                                      sObjectSchemaNameLen );
    
    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SYNONYMS,
                                   ELDT_Synonyms_ColumnOrder_OBJECT_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sSynonymDesc->mObjectName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sSynonymDesc->mObjectName[sDataValue->mLength] = '\0';
    }
    
    /**
     * return 값 설정
     */
    
    *aSynonymDesc = sSynonymDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Synonym ID 를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aSynonymDesc    Synonym Descriptor
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementSynonymID( eldcHashElement  ** aHashElement,
                                        smlTransId          aTransID,
                                        ellSynonymDesc    * aSynonymDesc,
                                        ellEnv            * aEnv )
{
    stlInt32                sHashDataSize = 0;
    stlUInt32               sHashValue = 0;
    eldcHashDataSynonymID * sHashDataSynonymID = NULL;
    eldcHashElement       * sHashElement   = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataSynonymID();
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataSynonymID,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataSynonymID, 0x00, sHashDataSize );
    
    sHashDataSynonymID->mKeySynonymID = aSynonymDesc->mSynonymID;

    /**
     * Hash Value 생성
     */
    
    sHashValue = eldcHashFuncSynonymID( (void *) & aSynonymDesc->mSynonymID );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataSynonymID,
                                  sHashValue,
                                  aSynonymDesc,
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
 * @brief Synonym Name 을 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aSynonymDesc    Synonym Descriptor
 * @param[in]  aHashElementID  Synonym ID로부터 구축된 Hash Element
 * @param[in]  aEnv            Environment 
 * @remarks
 */ 
stlStatus eldcMakeHashElementSynonymName( eldcHashElement  ** aHashElement,
                                          smlTransId          aTransID,
                                          ellSynonymDesc    * aSynonymDesc,
                                          eldcHashElement   * aHashElementID,
                                          ellEnv            * aEnv )
{
    stlInt32                  sHashDataSize = 0;
    stlUInt32                 sHashValue = 0;
    eldcHashDataSynonymName * sHashDataSynonymName = NULL;
    eldcHashElement         * sHashElement     = NULL;
    eldcHashKeySynonymName    sHashKey;
    
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeySynonymName) );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataSynonymName( stlStrlen(aSynonymDesc->mSynonymName) );
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataSynonymName,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataSynonymName, 0x00, sHashDataSize );
    
    sHashDataSynonymName->mKeySchemaID = aSynonymDesc->mSchemaID;
    
    /* Synonym Name 을 복사할 위치 지정 */
    sHashDataSynonymName->mKeySynonymName
        = eldcGetHashDataSynonymNamePtr( sHashDataSynonymName );
    
    stlStrcpy( sHashDataSynonymName->mKeySynonymName,
               aSynonymDesc->mSynonymName );

    sHashDataSynonymName->mDataSynonymID   = aSynonymDesc->mSynonymID;
    sHashDataSynonymName->mDataHashElement = aHashElementID;

    /**
     * Hash Value 생성
     */

    sHashKey.mSchemaID    = sHashDataSynonymName->mKeySchemaID;
    sHashKey.mSynonymName = sHashDataSynonymName->mKeySynonymName;
    
    sHashValue = eldcHashFuncSynonymName( (void *) & sHashKey );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataSynonymName,
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

/** @} eldcSynonymDesc */
