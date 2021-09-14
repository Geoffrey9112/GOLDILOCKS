/*******************************************************************************
 * eldcTablespaceDesc.c
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
 * @file eldcTablespaceDesc.c
 * @brief Cache for Tablespace descriptor
 */

#include <ell.h>
#include <eldt.h>                               
#include <eldc.h>

#include <eldDictionary.h>



/**
 * @addtogroup eldcTablespaceDesc
 * @{
 */


/**
 * @brief Hash Key 로부터 Hash Value 생성을 위한 함수 포인터
 * @param[in] aHashKey     Hash Key(Tablespace ID)
 * @remarks
 */
stlUInt32  eldcHashFuncTablespaceID( void * aHashKey )
{
    stlInt64  sTablespaceID = 0;
    
    stlUInt32 sHashValue = 0;

    /**
     * Column ID 로부터 Hash Value 생성 
     */
    
    sTablespaceID = *(stlInt64 *) aHashKey;
    
    sHashValue = ellGetHashValueOneID( sTablespaceID );

    return sHashValue;
}



/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key(Tablespace ID)
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncTablespaceID( void * aHashKey, eldcHashElement * aHashElement )
{
    stlInt64               sTablespaceID = 0;
    
    eldcHashDataTablespaceID * sHashData    = NULL;

    /**
     * 입력된 Tablespace ID 와 Hash Element 의 Tablespace ID 를 비교 
     */
    
    sTablespaceID  = *(stlInt64 *) aHashKey;
    
    sHashData    = (eldcHashDataTablespaceID *) aHashElement->mHashData;

    if ( sHashData->mKeyTablespaceID == sTablespaceID )
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
 * @param[in] aHashKey     Hash Key (Tablespace Name)
 * @remarks
 */
stlUInt32  eldcHashFuncTablespaceName( void * aHashKey )
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
 * @param[in] aHashKey        Hash Key (Tablespace Name )
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncTablespaceName( void * aHashKey, eldcHashElement * aHashElement )
{
    stlChar * sHashKey = NULL;
    
    eldcHashDataTablespaceName * sHashData = NULL;

    /**
     * 입력된 Hash Key 와 Hash Element 의 Key 를 비교 
     */
    
    sHashKey  = (stlChar *) aHashKey;
    
    sHashData    = (eldcHashDataTablespaceName *) aHashElement->mHashData;

    if ( stlStrcmp( sHashData->mKeyTablespaceName, sHashKey ) == 0 )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}













/**
 * @brief Tablespace Descriptor 를 위한 공간을 계산한다.
 * @param[in] aTablespaceNameLen    Tablespace Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeTablespaceDesc( stlInt32 aTablespaceNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(ellTablespaceDesc) );
    
    if ( aTablespaceNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aTablespaceNameLen + 1);
    }

    return sSize;
}




/**
 * @brief Tablespace Descriptor 로부터 Tablespace Name 의 Pointer 위치를 계산
 * @param[in] aTablespaceDesc   Tablespace Descriptor
 * @remarks
 */
stlChar * eldcGetTablespaceNamePtr( ellTablespaceDesc * aTablespaceDesc )
{
    return (stlChar *) aTablespaceDesc + STL_ALIGN_DEFAULT( STL_SIZEOF(ellTablespaceDesc) );
}



/**
 * @brief Tablespace ID Hash Data 를 위한 공간을 계산한다.
 * @remarks
 */
stlInt32  eldcCalSizeHashDataTablespaceID()
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF( eldcHashDataTablespaceID ) );

    return sSize;
}


/**
 * @brief Tablespace Name Hash Data 를 위한 공간을 계산한다.
 * @param[in]  aTablespaceNameLen   Tablespace Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeHashDataTablespaceName( stlInt32 aTablespaceNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataTablespaceName) );

    if ( aTablespaceNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aTablespaceNameLen + 1);
    }
    
    return sSize;
}


/**
 * @brief Tablespace Name Hash Data 로부터 Tablespace Name 의 Pointer 위치를 계산
 * @param[in] aHashDataTablespaceName   Tablespace Name Hash Data
 * @remarks
 */
stlChar * eldcGetHashDataTablespaceNamePtr( eldcHashDataTablespaceName * aHashDataTablespaceName )
{
    return (stlChar *) aHashDataTablespaceName
        + STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataTablespaceName) );
}










/**
 * @brief Tablespace Descriptor 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aTablespaceDesc   Tablespace Descriptor
 * @param[out] aStringBuffer     출력 정보를 위한 String Buffer 공간
 * @remarks
 */
void eldcPrintTablespaceDesc( void * aTablespaceDesc, stlChar * aStringBuffer )
{
    stlInt32        sSize = 0;
    ellTablespaceDesc * sTablespaceDesc = (ellTablespaceDesc *) aTablespaceDesc;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Tablespace Descriptor( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sCREATOR_ID: %ld, ",
                 aStringBuffer,
                 sTablespaceDesc->mCreatorID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sTABLESPACE_ID: %ld, ",
                 aStringBuffer,
                 sTablespaceDesc->mTablespaceID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sTABLESPACE_NAME: %s, ",
                 aStringBuffer,
                 sTablespaceDesc->mTablespaceName );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sMEDIA_TYPE: %d, ",
                 aStringBuffer,
                 sTablespaceDesc->mMediaType );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sUSAGE_TYPE: %d ) ",
                         aStringBuffer,
                         sTablespaceDesc->mUsageType );
    
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
 * @brief Tablespace ID Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataTablespaceID    Tablespace ID 의 Hash Data
 * @param[out] aStringBuffer            출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataTablespaceID( void * aHashDataTablespaceID, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataTablespaceID * sHashData
        = (eldcHashDataTablespaceID *) aHashDataTablespaceID;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Tablespace ID Hash Data( " );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sKEY_TABLESPACE_ID: %ld )",
                         aStringBuffer,
                         sHashData->mKeyTablespaceID );

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
 * @brief Tablespace Name Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataTablespaceName  Tablespace Name 의 Hash Data
 * @param[out] aStringBuffer            출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataTablespaceName( void    * aHashDataTablespaceName,
                                      stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataTablespaceName * sHashData
        = (eldcHashDataTablespaceName *) aHashDataTablespaceName;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Tablespace Name Hash Data( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_TABLESPACE_NAME: %s, ",
                 aStringBuffer,
                 sHashData->mKeyTablespaceName );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sDATA_TABLESPACE_ID: %ld )",
                         aStringBuffer,
                         sHashData->mDataTablespaceID );

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
 * @brief SQL-Tablespace Cache 를 위한 Dump Handle을 설정
 * @remarks
 */

void eldcSetSQLTablespaceDumpHandle()
{
    /*
     * ELDC_OBJECTCACHE_SPACE_ID
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_SPACE_ID].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SPACE_ID );
    gEldcObjectDump[ELDC_OBJECTCACHE_SPACE_ID].mPrintHashData
        = eldcPrintHashDataTablespaceID;
    gEldcObjectDump[ELDC_OBJECTCACHE_SPACE_ID].mPrintObjectDesc
        = eldcPrintTablespaceDesc;
    
    /*
     * ELDC_OBJECTCACHE_SPACE_NAME
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_SPACE_NAME].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SPACE_NAME );
    gEldcObjectDump[ELDC_OBJECTCACHE_SPACE_NAME].mPrintHashData
        = eldcPrintHashDataTablespaceName;
    gEldcObjectDump[ELDC_OBJECTCACHE_SPACE_NAME].mPrintObjectDesc
        = eldcPrintINVALID;
}






/**
 * @brief SQL-Tablespace 객체를 위한 Cache 를 구축한다.
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildTablespaceCache( smlTransId     aTransID,
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
                                      ELDT_TABLE_ID_TABLESPACES,
                                      & sBucketCnt,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * 소수에 해당하는 Hash Bucket 개수의 조정
     */

    sBucketCnt = ellGetPrimeBucketCnt( sBucketCnt );
    
    /**
     * Tablespace ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SPACE_ID ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Tablespace Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SPACE_NAME ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLTablespaceDumpHandle();
    
    /**
     * SQL-Tablespace Cache 구축 
     */

    STL_TRY( eldcBuildCacheSQLTablespace( aTransID,
                                          aStmt,
                                          aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief SQL-Tablespace 을 위한 Cache 를 구축한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildCacheSQLTablespace( smlTransId       aTransID,
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
     * DEFINITION_SCHEMA.TABLESPACES 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_TABLESPACES];

    /**
     * DEFINITION_SCHEMA.TABLESPACES 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_TABLESPACES,
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
     * Tablespace Descriptor 를 Cache 에 추가한다.
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
        STL_TRY( eldcInsertCacheTablespace( aTransID,
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
 * @brief Tablespace ID 를 이용해 Tablespace Handle 을 얻는다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aViewSCN         View SCN
 * @param[in]  aTablespaceID    Tablespace ID
 * @param[out] aSpaceDictHandle Tablespace Dictionary Handle
 * @param[out] aExist           존재 여부
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus eldcGetTablespaceHandleByID( smlTransId           aTransID,
                                       smlScn               aViewSCN,
                                       stlInt64             aTablespaceID,
                                       ellDictHandle      * aSpaceDictHandle,
                                       stlBool            * aExist,
                                       ellEnv             * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlBool sIsValid = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTablespaceID > ELL_DICT_TABLESPACE_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_SPACE_ID Hash 로부터 검색
     */

    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SPACE_ID ),
                                  & aTablespaceID,
                                  eldcHashFuncTablespaceID,
                                  eldcCompareFuncTablespaceID,
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
             * Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aSpaceDictHandle,
                              ELL_OBJECT_TABLESPACE,
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
 * @brief Tablespace Name 를 이용해 Tablespace Handle 을 얻는다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aViewSCN         View SCN
 * @param[in]  aTablespaceName  Tablespace Name
 * @param[out] aSpaceDictHandle Tablespace Dictionary Handle
 * @param[out] aExist           존재 여부
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus eldcGetTablespaceHandleByName( smlTransId           aTransID,
                                         smlScn               aViewSCN,
                                         stlChar            * aTablespaceName,
                                         ellDictHandle      * aSpaceDictHandle,
                                         stlBool            * aExist,
                                         ellEnv             * aEnv )
{
    eldcHashElement        * sHashElement = NULL;

    eldcHashDataTablespaceName * sHashData = NULL;

    stlBool sIsValid = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTablespaceName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_SPACE_NAME Hash 로부터 검색
     */

    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SPACE_NAME ),
                                  aTablespaceName,
                                  eldcHashFuncTablespaceName,
                                  eldcCompareFuncTablespaceName,
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
        
        sHashData = (eldcHashDataTablespaceName *) sHashElement->mHashData;

        STL_TRY( eldcIsVisibleCache( aTransID,
                                     aViewSCN,
                                     & sHashData->mDataHashElement->mVersionInfo,
                                     & sIsValid,
                                     aEnv )
                 == STL_SUCCESS );
        
        if ( sIsValid == STL_TRUE )
        {
            /**
             * Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aSpaceDictHandle,
                              ELL_OBJECT_TABLESPACE,
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
 * @brief SQL Tablespace 을 위한 Cache 정보를 추가한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aValueList        Data Value List
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcInsertCacheTablespace( smlTransId          aTransID,
                                     smlStatement      * aStmt,
                                     knlValueBlockList * aValueList,
                                     ellEnv            * aEnv )
{
    ellTablespaceDesc   * sTablespaceDesc = NULL;
    
    eldcHashElement * sHashElementTablespaceID = NULL;
    eldcHashElement * sHashElementTablespaceName = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Tablespace ID를 위한 Tablespace Descriptor 구성
     */
    
    STL_TRY( eldcMakeTablespaceDesc( aTransID,
                                     aStmt,
                                     aValueList,
                                     & sTablespaceDesc,
                                     aEnv )
             == STL_SUCCESS );
    
    /**
     * Tablespace ID 를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementTablespaceID( & sHashElementTablespaceID,
                                              aTransID,
                                              sTablespaceDesc,
                                              aEnv )
             == STL_SUCCESS );
                                                  
    /**
     * ELDC_OBJECTCACHE_SPACE_ID Hash 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SPACE_ID ),
                                    sHashElementTablespaceID,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Tablespace Name 을 위한 Hash Element 구성
     */

    STL_TRY( eldcMakeHashElementTablespaceName( & sHashElementTablespaceName,
                                                aTransID,
                                                sTablespaceDesc,
                                                sHashElementTablespaceID,
                                                aEnv )
             == STL_SUCCESS );
    
    /**
     * ELDC_OBJECTCACHE_SPACE_NAME 에 추가 
     */

    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SPACE_NAME ),
                                    sHashElementTablespaceName,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    

/**
 * @brief Tablespace ID를 기준으로 SQL-Column Cache 를 추가한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aTablespaceID Tablespace ID
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcInsertCacheTablespaceByTablespaceID( smlTransId          aTransID,
                                                   smlStatement      * aStmt,
                                                   stlInt64            aTablespaceID,
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
     * DEFINITION_SCHEMA.TABLESPACES 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_TABLESPACES];

    /**
     * DEFINITION_SCHEMA.TABLESPACES 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_TABLESPACES,
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
     * WHERE TABLESPACE_ID = aTablespaceID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_TABLESPACES,
                                            ELDT_Space_ColumnOrder_TABLESPACE_ID );

    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_TABLESPACES,
                                  sFilterColumn,
                                  & aTablespaceID,
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
     * SQL-Table Cache 정보를 추가한다.
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
            STL_TRY( eldcInsertCacheTablespace( aTransID,
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
 * @brief SQL Tablespace 을 위한 Cache 정보를 삭제한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aSpaceHandle      Space Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcDeleteCacheTablespace( smlTransId          aTransID,
                                     smlStatement      * aStmt,
                                     ellDictHandle     * aSpaceHandle,
                                     ellEnv            * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlInt64   sSpaceID = ELL_DICT_TABLESPACE_ID_NA;
    stlChar  * sSpaceName = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSpaceHandle != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * ELDC_OBJECTCACHE_SPACE_ID Hash 에서 삭제 
     */

    sSpaceID = ellGetTablespaceID( aSpaceHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SPACE_ID ),
                                  & sSpaceID,
                                  eldcHashFuncTablespaceID,
                                  eldcCompareFuncTablespaceID,
                                  & sHashElement,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SPACE_ID ),
                                    sHashElement,
                                    aEnv )
             == STL_SUCCESS );
                                    
    /**
     * ELDC_OBJECTCACHE_SPACE_NAME Hash 로부터 검색
     */

    sSpaceName = ellGetTablespaceName( aSpaceHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SPACE_NAME ),
                                  sSpaceName,
                                  eldcHashFuncTablespaceName,
                                  eldcCompareFuncTablespaceName,
                                  & sHashElement,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SPACE_NAME ),
                                    sHashElement,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}










/**
 * @brief 읽은 Value List 로부터 SQL Tablespace 의 Tablespace Descriptor 를 생성한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[out] aTablespaceDesc  Tablespace Descriptor
 * @param[in]  aValueList       Value List
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldcMakeTablespaceDesc( smlTransId           aTransID,
                                  smlStatement       * aStmt,
                                  knlValueBlockList  * aValueList,
                                  ellTablespaceDesc ** aTablespaceDesc,
                                  ellEnv             * aEnv )
{
    stlInt32            sTablespaceDescSize = 0;
    ellTablespaceDesc * sTablespaceDesc = NULL;

    dtlDataValue     * sDataValue = NULL;

    stlInt32           sTablespaceNameLen = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTablespaceDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Tablespace Descriptor 를 위한 공간의 크기 계산
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLESPACES,
                                   ELDT_Space_ColumnOrder_TABLESPACE_NAME );
    sTablespaceNameLen = sDataValue->mLength;
    
    sTablespaceDescSize = eldcCalSizeTablespaceDesc( sTablespaceNameLen );
    
    /**
     * Tablespace Descriptor 를 위한 공간 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sTablespaceDesc,
                               sTablespaceDescSize,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Tablespace Descriptor 의 각 정보를 설정한다.
     */

    /*
     * mCreatorID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLESPACES,
                                   ELDT_Space_ColumnOrder_CREATOR_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTablespaceDesc->mCreatorID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTablespaceDesc->mCreatorID) == sDataValue->mLength );
    }

    /*
     * mTablespaceID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLESPACES,
                                   ELDT_Space_ColumnOrder_TABLESPACE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTablespaceDesc->mTablespaceID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTablespaceDesc->mTablespaceID) == sDataValue->mLength );
    }

    /*
     * mTablespaceName
     * - 메모리 공간 내에서의 위치 계산
     */

    sTablespaceDesc->mTablespaceName = eldcGetTablespaceNamePtr( sTablespaceDesc );

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLESPACES,
                                   ELDT_Space_ColumnOrder_TABLESPACE_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sTablespaceDesc->mTablespaceName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sTablespaceDesc->mTablespaceName[sDataValue->mLength] = '\0';
    }

    /*
     * mMediaType
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLESPACES,
                                   ELDT_Space_ColumnOrder_MEDIA_TYPE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTablespaceDesc->mMediaType,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTablespaceDesc->mMediaType) == sDataValue->mLength );
    }

    /*
     * mUsageType
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_TABLESPACES,
                                   ELDT_Space_ColumnOrder_USAGE_TYPE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sTablespaceDesc->mUsageType,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sTablespaceDesc->mUsageType) == sDataValue->mLength );
    }
    
    /**
     * return 값 설정
     */
    
    *aTablespaceDesc = sTablespaceDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    

/**
 * @brief Tablespace ID 를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aTablespaceDesc Tablespace Descriptor
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementTablespaceID( eldcHashElement   ** aHashElement,
                                           smlTransId           aTransID,
                                           ellTablespaceDesc  * aTablespaceDesc,
                                           ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32                  sHashValue = 0;
    eldcHashDataTablespaceID * sHashDataTablespaceID = NULL;
    eldcHashElement          * sHashElement      = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTablespaceDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataTablespaceID();
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataTablespaceID,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataTablespaceID, 0x00, sHashDataSize );
    
    sHashDataTablespaceID->mKeyTablespaceID = aTablespaceDesc->mTablespaceID;

    /**
     * Hash Value 생성
     */
    
    sHashValue = eldcHashFuncTablespaceID( (void *) & aTablespaceDesc->mTablespaceID );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataTablespaceID,
                                  sHashValue,
                                  aTablespaceDesc,
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
 * @brief Tablespace Name 을 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aTablespaceDesc Tablespace Descriptor
 * @param[in]  aHashElementID  Tablespace ID로부터 구축된 Hash Element
 * @param[in]  aEnv            Environment 
 * @remarks
 */ 
stlStatus eldcMakeHashElementTablespaceName( eldcHashElement   ** aHashElement,
                                             smlTransId           aTransID,
                                             ellTablespaceDesc  * aTablespaceDesc,
                                             eldcHashElement    * aHashElementID,
                                             ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32                    sHashValue = 0;
    eldcHashDataTablespaceName * sHashDataTablespaceName = NULL;
    eldcHashElement            * sHashElement        = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTablespaceDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataTablespaceName( stlStrlen(aTablespaceDesc->mTablespaceName) );
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataTablespaceName,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataTablespaceName, 0x00, sHashDataSize );
    
    /* Tablespace Name 을 복사할 위치 지정 */
    sHashDataTablespaceName->mKeyTablespaceName
        = eldcGetHashDataTablespaceNamePtr( sHashDataTablespaceName );
    
    stlStrcpy( sHashDataTablespaceName->mKeyTablespaceName,
               aTablespaceDesc->mTablespaceName );

    sHashDataTablespaceName->mDataTablespaceID = aTablespaceDesc->mTablespaceID;
    sHashDataTablespaceName->mDataHashElement = aHashElementID;

    /**
     * Hash Value 생성
     */

    sHashValue = eldcHashFuncTablespaceName( (void *) sHashDataTablespaceName->mKeyTablespaceName );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataTablespaceName,
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
    




/** @} eldcTablespaceDesc */
