/*******************************************************************************
 * eldcPublicSynonymDesc.c
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
 * @file eldcPublicSynonymDesc.c
 * @brief Cache for Public Synonym descriptor
 */

#include <ell.h>
#include <eldt.h>                               
#include <eldc.h>

#include <eldDictionary.h>


/**
 * @addtogroup eldcPublicSynonymDesc
 * @{
 */

/*******************************************************************************
 * Hash 함수 
 ******************************************************************************/

/**
 * @brief Hash Key 로부터 Hash Value 생성을 위한 함수 포인터
 * @param[in] aHashKey     Hash Key(Public Synonym ID)
 * @remarks
 */
stlUInt32 eldcHashFuncPublicSynonymID( void * aHashKey )
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
stlBool eldcCompareFuncPublicSynonymID( void * aHashKey, eldcHashElement * aHashElement )
{
    stlInt64                      sSynonymID = 0;
    eldcHashDataPublicSynonymID * sHashData    = NULL;

    /**
     * 입력된 Synonym ID 와 Hash Element 의 Synonym ID 를 비교 
     */
    
    sSynonymID  = *(stlInt64 *) aHashKey;
    sHashData   = (eldcHashDataPublicSynonymID *) aHashElement->mHashData;

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
 * @param[in] aHashKey     Hash Key (Synonym Name)
 * @remarks
 */
stlUInt32  eldcHashFuncPublicSynonymName( void * aHashKey )
{
    stlChar   * sKeyData = NULL;
    stlUInt32   sHashValue = 0;

    /**
     * Hash Key 로부터 Hash Value 생성 
     */
    
    sKeyData = (stlChar *) aHashKey;
    
    sHashValue = ellGetHashValueOneName( sKeyData );

    return sHashValue;
}

/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key (Public Synonym Name )
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool eldcCompareFuncPublicSynonymName( void * aHashKey, eldcHashElement * aHashElement )
{
    stlChar                       * sHashKey = NULL;
    eldcHashDataPublicSynonymName * sHashData = NULL;

    /**
     * 입력된 Hash Key 와 Hash Element 의 Key 를 비교 
     */
    
    sHashKey  = (stlChar *) aHashKey;
    
    sHashData = (eldcHashDataPublicSynonymName *) aHashElement->mHashData;

    if ( stlStrcmp( sHashData->mKeySynonymName, sHashKey ) == 0 ) 
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
 * @brief Public Synonym Descriptor 를 위한 공간을 계산한다.
 * @param[in] aSynonymNameLen       Synonym Name 의 길이
 * @param[in] aObjectSchemaNameLen  Object Schema Name 의 길이
 * @param[in] aObjectNameLen        Object Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizePublicSynonymDesc( stlInt32 aSynonymNameLen,
                                        stlInt32 aObjectSchemaNameLen,
                                        stlInt32 aObjectNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(ellPublicSynonymDesc) );
    
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
 * @brief Public Synonym Descriptor 로부터 Synonym Name 의 Pointer 위치를 계산
 * @param[in] aPublicSynonymDesc       Public Synonym Descriptor
 * @remarks
 */
stlChar * eldcGetSynonymNamePtrOfPublic( ellPublicSynonymDesc * aPublicSynonymDesc )
{
    return (stlChar *) aPublicSynonymDesc + STL_ALIGN_DEFAULT( STL_SIZEOF(ellPublicSynonymDesc) );
}


/**
 * @brief Public Synonym Descriptor 로부터 Object Schema Name 의 Pointer 위치를 계산
 * @param[in] aPublicSynonymDesc    Public Synonym Descriptor
 * @param[in] aSynonymNameLen       Synonym Name Length
 * @remarks
 */
stlChar * eldcGetObjectSchemaNamePtrOfPublic( ellPublicSynonymDesc * aPublicSynonymDesc,
                                              stlInt32               aSynonymNameLen )
{
    return (stlChar *) aPublicSynonymDesc
        + STL_ALIGN_DEFAULT( STL_SIZEOF(ellPublicSynonymDesc) )
        + STL_ALIGN_DEFAULT(aSynonymNameLen + 1);
}


/**
 * @brief Public Synonym Descriptor 로부터 Object Name 의 Pointer 위치를 계산
 * @param[in] aPublicSynonymDesc    Public Synonym Descriptor
 * @param[in] aSynonymNameLen       Synonym Name Length
 * @param[in] aObjectSchemaNameLen  Object Schema Name Length 
 * @remarks
 */
stlChar * eldcGetObjectNamePtrOfPublic( ellPublicSynonymDesc * aPublicSynonymDesc,
                                        stlInt32               aSynonymNameLen,
                                        stlInt32               aObjectSchemaNameLen )
{
    return (stlChar *) aPublicSynonymDesc
        + STL_ALIGN_DEFAULT( STL_SIZEOF(ellPublicSynonymDesc) )
        + STL_ALIGN_DEFAULT(aSynonymNameLen + 1)
        + STL_ALIGN_DEFAULT(aObjectSchemaNameLen + 1);
}


/**
 * @brief Public Synonym ID Hash Data 를 위한 공간을 계산한다.
 * @remarks
 */
stlInt32  eldcCalSizeHashDataPublicSynonymID()
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF( eldcHashDataPublicSynonymID ) );

    return sSize;
}

/**
    * @brief Public Synonym Name Hash Data 를 위한 공간을 계산한다.
 * @param[in]  aSynonymNameLen   Synonym Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeHashDataPublicSynonymName( stlInt32 aSynonymNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataPublicSynonymName) );

    if ( aSynonymNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aSynonymNameLen + 1);
    }
    
    return sSize;
}

  /**
 * @brief Public Synonym Name Hash Data 로부터 Public Synonym Name 의 Pointer 위치를 계산
 * @param[in] aHashDataPublicSynonymName   Synonym Name Hash Data
 * @remarks
 */
stlChar * eldcGetHashDataPublicSynonymNamePtr( eldcHashDataPublicSynonymName * aHashDataPublicSynonymName )
{
    return (stlChar *) aHashDataPublicSynonymName
        + STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataPublicSynonymName) );
}

/*******************************************************************************
 * Dump Information
 ******************************************************************************/

/**
 * @brief Public Synonym Descriptor 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aSynonymDesc   Synonym Descriptor
 * @param[out] aStringBuffer  출력 정보를 위한 String Buffer 공간
 * @remarks
 */
void eldcPrintPublicSynonymDesc( void * aSynonymDesc, stlChar * aStringBuffer )
{
    stlInt32               sSize = 0;
    ellPublicSynonymDesc * sSynonymDesc = (ellPublicSynonymDesc *) aSynonymDesc;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Public Synonym Descriptor( " );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sSYNONYM_ID: %ld, ",
                 aStringBuffer,
                 sSynonymDesc->mSynonymID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sSYNONYM_NAME: %s, ",
                 aStringBuffer,
                 sSynonymDesc->mSynonymName );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sCREATOR_ID: %ld, ",
                 aStringBuffer,
                 sSynonymDesc->mCreatorID );

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

void eldcPrintHashDataPublicSynonymID( void * aHashDataSynonymID, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataPublicSynonymID * sHashData = (eldcHashDataPublicSynonymID *) aHashDataSynonymID;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Public Synonym ID Hash Data( " );

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

void eldcPrintHashDataPublicSynonymName( void * aHashDataSynonymName, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataPublicSynonymName * sHashData = (eldcHashDataPublicSynonymName *) aHashDataSynonymName;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Public Synonym Name Hash Data( " );
    
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

void eldcSetSQLPublicSynonymDumpHandle()
{
    /*
     * ELDC_OBJECTCACHE_PUBLIC_SYNONYM_ID
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_PUBLIC_SYNONYM_ID].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PUBLIC_SYNONYM_ID );
    gEldcObjectDump[ELDC_OBJECTCACHE_PUBLIC_SYNONYM_ID].mPrintHashData
        = eldcPrintHashDataPublicSynonymID;
    gEldcObjectDump[ELDC_OBJECTCACHE_PUBLIC_SYNONYM_ID].mPrintObjectDesc
        = eldcPrintPublicSynonymDesc;
    
    /*
     * ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME );
    gEldcObjectDump[ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME].mPrintHashData
        = eldcPrintHashDataPublicSynonymName;
    gEldcObjectDump[ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME].mPrintObjectDesc
        = eldcPrintINVALID;
}

/**
 * @brief SQL-PublicSynonym 객체를 위한 Cache 를 구축한다.
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildPublicSynonymCache( smlTransId     aTransID,
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
                                      ELDT_TABLE_ID_PUBLIC_SYNONYMS,
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

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_PUBLIC_SYNONYM_ID ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Synonym Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME ),
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

    STL_TRY( eldcBuildCacheSQLPublicSynonym( aTransID,
                                             aStmt,
                                             aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief SQL-PublicSynonym 을 위한 Cache 를 구축한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildCacheSQLPublicSynonym( smlTransId       aTransID,
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
     * DEFINITION_SCHEMA.PUBLIC_SYNONYMS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_PUBLIC_SYNONYMS];

    /**
     * DEFINITION_SCHEMA.PUBLIC_SYNONYMS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_PUBLIC_SYNONYMS,
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
     * Sequence Descriptor 를 Cache 에 추가한다.
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
        STL_TRY( eldcInsertCachePublicSynonym( aTransID,
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
 * @brief Public Synonym Name 을 이용해 Public Synonym Dictionary Handle 을 획득 
 * @param[in]  aTransID                  Transaction ID
 * @param[in]  aViewSCN                  View SCN
 * @param[in]  aPublicSynonymName        Public Synonym Name
 * @param[out] aPublicSynonymDictHandle  Public Synonym Dictionary Handle
 * @param[out] aExist                    존재 여부 
 * @param[in]  aEnv                      Envirionment 
 * @remarks
 */
stlStatus eldcGetPublicSynonymHandleByName( smlTransId           aTransID,
                                            smlScn               aViewSCN,
                                            stlChar            * aPublicSynonymName,
                                            ellDictHandle      * aPublicSynonymDictHandle,
                                            stlBool            * aExist,
                                            ellEnv             * aEnv )
{
    eldcHashElement               * sHashElement = NULL;

    eldcHashDataPublicSynonymName * sHashData = NULL;
    stlBool                         sIsValid = STL_FALSE;

    /*
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aPublicSynonymName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPublicSynonymDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

     /**
     * ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME Hash 로부터 검색
     */
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME ),
                                  aPublicSynonymName,
                                  eldcHashFuncPublicSynonymName,
                                  eldcCompareFuncPublicSynonymName,
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
        sHashData = (eldcHashDataPublicSynonymName *) sHashElement->mHashData;

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
                              aPublicSynonymDictHandle,
                              ELL_OBJECT_PUBLIC_SYNONYM,
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


/*******************************************************************************
 * 제어 함수 
 ******************************************************************************/


/**
 * @brief SQL Public Synonym 을 위한 Cache 정보를 추가한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aValueList        Data Value List
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcInsertCachePublicSynonym( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        knlValueBlockList * aValueList,
                                        ellEnv            * aEnv )
{
    ellSessObjectMgr     * sObjMgr = ELL_SESS_OBJ(aEnv);
    ellPublicSynonymDesc * sPublicSynonymDesc = NULL;
    
    eldcHashElement      * sHashElementPublicSynonymID = NULL;
    eldcHashElement      * sHashElementPublicSynonymName = NULL;
    eldcLocalCache       * sNewLocalCache = NULL;

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
     * Public Synonym ID를 위한 Synonym Descriptor 구성
     */
    
    STL_TRY( eldcMakePublicSynonymDesc( aTransID,
                                        aStmt,
                                        aValueList,
                                        & sPublicSynonymDesc,
                                        aEnv )
             == STL_SUCCESS );
    
    /**
     * Public Synonym ID 를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementPublicSynonymID( & sHashElementPublicSynonymID,
                                                 aTransID,
                                                 sPublicSynonymDesc,
                                                 aEnv )
             == STL_SUCCESS );
                                                  
    /**
     * ELDC_OBJECTCACHE_PUBLIC_SYNONYM_ID Hash 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PUBLIC_SYNONYM_ID ),
                                    sHashElementPublicSynonymID,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Synonym Name 을 위한 Hash Element 구성
     */

    STL_TRY( eldcMakeHashElementPublicSynonymName( & sHashElementPublicSynonymName,
                                                   aTransID,
                                                   sPublicSynonymDesc,
                                                   sHashElementPublicSynonymID,
                                                   aEnv )
             == STL_SUCCESS );
    
    /**
     * ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME 에 추가 
     */

    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME ),
                                    sHashElementPublicSynonymName,
                                    aEnv )
             == STL_SUCCESS );
    
    /**
     * 객체가 변경되고 있음을 세션에 기록한다.
     */
    
    ELL_SESS_ENV(aEnv)->mLocalModifySeq += 1;
    
    eldcInsertLocalCache( sHashElementPublicSynonymID, sNewLocalCache, aEnv );

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
 * @brief Public Synonym ID를 기준으로 SQL-PulicSynonym Cache 를 추가한다.
 * @param[in]  aTransID            Transaction ID
 * @param[in]  aStmt               Statement
 * @param[in]  aSynonymID          Synonym ID
 * @param[in]  aEnv                Environment
 * @remarks
 */
stlStatus eldcInsertCachePublicSynonymBySynonymID( smlTransId          aTransID,
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

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSynonymID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.PUBLIC_SYNONYM 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_PUBLIC_SYNONYMS];

    /**
     * DEFINITION_SCHEMA.PUBLIC_SYNONYMS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_PUBLIC_SYNONYMS,
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
                                            ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                            ELDT_Public_Synonyms_ColumnOrder_SYNONYM_ID );

    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_PUBLIC_SYNONYMS,
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
     * SQL-PublicSynonym Cache 정보를 추가한다.
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
            STL_TRY( eldcInsertCachePublicSynonym( aTransID,
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
 * @brief SQL public Synonym 를 위한 Cache 정보를 삭제한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aSynonymHandle    Synonym Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcDeleteCachePublicSynonym( smlTransId          aTransID,
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
    
    stlChar           * sSynonymName;
    
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
     * ELDC_OBJECTCACHE_PUBLIC_SYNONYM_ID Hash 에서 삭제 
     */

    sSynonymID = ellGetPublicSynonymID( aSynonymHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PUBLIC_SYNONYM_ID ),
                                  & sSynonymID,
                                  eldcHashFuncPublicSynonymID,
                                  eldcCompareFuncPublicSynonymID,
                                  & sHashElementID,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PUBLIC_SYNONYM_ID ),
                                    sHashElementID,
                                    aEnv )
             == STL_SUCCESS );
                                    
    /**
     * ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME Hash 에서 삭제 
     */

    sSynonymName = ellGetPublicSynonymName( aSynonymHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME ),
                                  sSynonymName,
                                  eldcHashFuncPublicSynonymName,
                                  eldcCompareFuncPublicSynonymName,
                                  & sHashElementName,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME ),
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
 * @brief 읽은 Value List 로부터 SQL Public Synonym 의 Public Synonym Descriptor 를 생성한다.
 * @param[in]  aTransID               Transaction ID
 * @param[in]  aStmt                  Statement
 * @param[out] aPublicSynonymDesc     Public Synonym Descriptor
 * @param[in]  aValueList             Value List
 * @param[in]  aEnv                   Environment 포인터
 * @remarks
 */
stlStatus eldcMakePublicSynonymDesc( smlTransId              aTransID,
                                     smlStatement          * aStmt,
                                     knlValueBlockList     * aValueList,
                                     ellPublicSynonymDesc ** aPublicSynonymDesc,
                                     ellEnv                * aEnv )
{
    stlInt32                sPublicSynonymDescSize = 0;
    ellPublicSynonymDesc  * sPublicSynonymDesc = NULL;

    dtlDataValue          * sDataValue = NULL;

    stlInt32                sSynonymNameLen = 0;
    stlInt32                sObjectSchemaNameLen = 0;
    stlInt32                sObjectNameLen = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPublicSynonymDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Public Synonym Descriptor 를 위한 공간의 크기 계산
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                   ELDT_Public_Synonyms_ColumnOrder_SYNONYM_NAME );
    sSynonymNameLen = sDataValue->mLength;

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                   ELDT_Public_Synonyms_ColumnOrder_OBJECT_SCHEMA_NAME );
    sObjectSchemaNameLen = sDataValue->mLength;

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                   ELDT_Public_Synonyms_ColumnOrder_OBJECT_NAME );
    sObjectNameLen = sDataValue->mLength;
    
    
    sPublicSynonymDescSize = eldcCalSizePublicSynonymDesc( sSynonymNameLen,
                                                           sObjectSchemaNameLen,
                                                           sObjectNameLen );
    
    /**
     * Synonym Descriptor 를 위한 공간 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sPublicSynonymDesc,
                               sPublicSynonymDescSize,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Public Synonym Descriptor 의 각 정보를 설정한다.
     */
    
    /*
     * mSynonymID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                   ELDT_Public_Synonyms_ColumnOrder_SYNONYM_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sPublicSynonymDesc->mSynonymID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sPublicSynonymDesc->mSynonymID) == sDataValue->mLength );
    }
    
    /*
     * mSynonymName
     * - 메모리 공간 내에서의 위치 계산
     */

    sPublicSynonymDesc->mSynonymName = eldcGetSynonymNamePtrOfPublic( sPublicSynonymDesc );

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                   ELDT_Public_Synonyms_ColumnOrder_SYNONYM_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sPublicSynonymDesc->mSynonymName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sPublicSynonymDesc->mSynonymName[sDataValue->mLength] = '\0';
    }

    /*
     * mCreatorID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                   ELDT_Public_Synonyms_ColumnOrder_CREATOR_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sPublicSynonymDesc->mCreatorID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sPublicSynonymDesc->mCreatorID) == sDataValue->mLength );
    }

    /*
     * mObjectSchemaName
     * - 메모리 공간 내에서의 위치 계산
     */

    sPublicSynonymDesc->mObjectSchemaName = eldcGetObjectSchemaNamePtrOfPublic( sPublicSynonymDesc,
                                                                                sSynonymNameLen );
    
    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                   ELDT_Public_Synonyms_ColumnOrder_OBJECT_SCHEMA_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sPublicSynonymDesc->mObjectSchemaName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sPublicSynonymDesc->mObjectSchemaName[sDataValue->mLength] = '\0';
    }

    /*
     * mObjectName
     * - 메모리 공간 내에서의 위치 계산
     */

    sPublicSynonymDesc->mObjectName = eldcGetObjectNamePtrOfPublic( sPublicSynonymDesc,
                                                                    sSynonymNameLen,
                                                                    sObjectSchemaNameLen );
    
    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_PUBLIC_SYNONYMS,
                                   ELDT_Public_Synonyms_ColumnOrder_OBJECT_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sPublicSynonymDesc->mObjectName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sPublicSynonymDesc->mObjectName[sDataValue->mLength] = '\0';
    }
    
    /**
     * return 값 설정
     */
    
    *aPublicSynonymDesc = sPublicSynonymDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Public Synonym ID 를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement          Hash Element
 * @param[in]  aTransID              Transaction ID
 * @param[in]  aPublicSynonymDesc    Synonym Descriptor
 * @param[in]  aEnv                  Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementPublicSynonymID( eldcHashElement      ** aHashElement,
                                              smlTransId              aTransID,
                                              ellPublicSynonymDesc  * aPublicSynonymDesc,
                                              ellEnv                * aEnv )
{
    stlInt32                sHashDataSize = 0;
    stlUInt32               sHashValue = 0;
    eldcHashDataSynonymID * sHashDataPublicSynonymID = NULL;
    eldcHashElement       * sHashElement   = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPublicSynonymDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataPublicSynonymID();
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataPublicSynonymID,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataPublicSynonymID, 0x00, sHashDataSize );
    
    sHashDataPublicSynonymID->mKeySynonymID = aPublicSynonymDesc->mSynonymID;

    /**
     * Hash Value 생성
     */
    
    sHashValue = eldcHashFuncPublicSynonymID( (void *) & aPublicSynonymDesc->mSynonymID );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataPublicSynonymID,
                                  sHashValue,
                                  aPublicSynonymDesc,
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
 * @brief Public Synonym Name 을 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement         Hash Element
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aPublicSynonymDesc   Synonym Descriptor
 * @param[in]  aHashElementID       Public Synonym ID로부터 구축된 Hash Element
 * @param[in]  aEnv                 Environment 
 * @remarks
 */ 
stlStatus eldcMakeHashElementPublicSynonymName( eldcHashElement      ** aHashElement,
                                                smlTransId              aTransID,
                                                ellPublicSynonymDesc  * aPublicSynonymDesc,
                                                eldcHashElement       * aHashElementID,
                                                ellEnv                * aEnv )
{
    stlInt32                        sHashDataSize = 0;
    stlUInt32                       sHashValue = 0;
    eldcHashDataPublicSynonymName * sHashDataPublicSynonymName = NULL;
    eldcHashElement               * sHashElement = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aPublicSynonymDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataPublicSynonymName( stlStrlen(aPublicSynonymDesc->mSynonymName) );
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataPublicSynonymName,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataPublicSynonymName, 0x00, sHashDataSize );
    
    /* Synonym Name 을 복사할 위치 지정 */
    sHashDataPublicSynonymName->mKeySynonymName
        = eldcGetHashDataPublicSynonymNamePtr( sHashDataPublicSynonymName );
    
    stlStrcpy( sHashDataPublicSynonymName->mKeySynonymName,
               aPublicSynonymDesc->mSynonymName );

    sHashDataPublicSynonymName->mDataSynonymID   = aPublicSynonymDesc->mSynonymID;
    sHashDataPublicSynonymName->mDataHashElement = aHashElementID;

    /**
     * Hash Value 생성
     */
    
    sHashValue = eldcHashFuncPublicSynonymName( (void *) sHashDataPublicSynonymName->mKeySynonymName );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataPublicSynonymName,
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

/** @} eldcPublicSynonymDesc */
