/*******************************************************************************
 * eldcSequenceDesc.c
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
 * @file eldcSequenceDesc.c
 * @brief Cache for Sequence descriptor
 */

#include <ell.h>
#include <eldt.h>                               
#include <eldc.h>

#include <eldDictionary.h>



/**
 * @addtogroup eldcSequenceDesc
 * @{
 */


/**
 * @brief Hash Key 로부터 Hash Value 생성을 위한 함수 포인터
 * @param[in] aHashKey     Hash Key(Sequence ID)
 * @remarks
 */
stlUInt32  eldcHashFuncSequenceID( void * aHashKey )
{
    stlInt64  sSequenceID = 0;
    
    stlUInt32 sHashValue = 0;

    /**
     * Sequence ID 로부터 Hash Value 생성 
     */
    
    sSequenceID = *(stlInt64 *) aHashKey;
    
    sHashValue = ellGetHashValueOneID( sSequenceID );

    return sHashValue;
}


/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key(Sequence ID)
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncSequenceID( void * aHashKey, eldcHashElement * aHashElement )
{
    stlInt64               sSequenceID = 0;
    
    eldcHashDataSequenceID * sHashData    = NULL;

    /**
     * 입력된 Sequence ID 와 Hash Element 의 Sequence ID 를 비교 
     */
    
    sSequenceID  = *(stlInt64 *) aHashKey;
    
    sHashData    = (eldcHashDataSequenceID *) aHashElement->mHashData;

    if ( sHashData->mKeySequenceID == sSequenceID )
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
 * @param[in] aHashKey     Hash Key (Schema ID, Sequence Name)
 * @remarks
 */
stlUInt32  eldcHashFuncSequenceName( void * aHashKey )
{
    eldcHashKeySequenceName * sKeyData = NULL;
    
    stlUInt32 sHashValue = 0;

    /**
     * Hash Key 로부터 Hash Value 생성 
     */
    
    sKeyData = (eldcHashKeySequenceName *) aHashKey;
    
    sHashValue = ellGetHashValueNameAndID( sKeyData->mSchemaID, sKeyData->mSequenceName );

    return sHashValue;
}


/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key (Schema ID, Sequence Name )
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncSequenceName( void * aHashKey, eldcHashElement * aHashElement )
{
    eldcHashKeySequenceName  * sHashKey = NULL;
    
    eldcHashDataSequenceName * sHashData    = NULL;

    /**
     * 입력된 Hash Key 와 Hash Element 의 Key 를 비교 
     */
    
    sHashKey  = (eldcHashKeySequenceName *) aHashKey;
    
    sHashData    = (eldcHashDataSequenceName *) aHashElement->mHashData;

    if ( ( sHashData->mKeySchemaID == sHashKey->mSchemaID ) &&
         ( stlStrcmp( sHashData->mKeySequenceName, sHashKey->mSequenceName ) == 0 ) )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}









/**
 * @brief Sequence Descriptor 를 위한 공간을 계산한다.
 * @param[in] aSeqNameLen    Sequence Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeSequenceDesc( stlInt32 aSeqNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(ellSequenceDesc) );
    
    if ( aSeqNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aSeqNameLen + 1);
    }

    return sSize;
}

/**
 * @brief Sequence Descriptor 로부터 Sequence Name 의 Pointer 위치를 계산
 * @param[in] aSeqDesc   Sequence Descriptor
 * @remarks
 */
stlChar * eldcGetSequenceNamePtr( ellSequenceDesc * aSeqDesc )
{
    return (stlChar *) aSeqDesc + STL_ALIGN_DEFAULT( STL_SIZEOF(ellSequenceDesc) );
}



/**
 * @brief Sequence ID Hash Data 를 위한 공간을 계산한다.
 * @remarks
 */
stlInt32  eldcCalSizeHashDataSequenceID()
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF( eldcHashDataSequenceID ) );

    return sSize;
}


/**
 * @brief Sequence Name Hash Data 를 위한 공간을 계산한다.
 * @param[in]  aSeqNameLen   Sequence Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeHashDataSequenceName( stlInt32 aSeqNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataSequenceName) );

    if ( aSeqNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aSeqNameLen + 1);
    }
    
    return sSize;
}


/**
 * @brief Sequence Name Hash Data 로부터 Sequence Name 의 Pointer 위치를 계산
 * @param[in] aHashDataSeqName   Sequence Name Hash Data
 * @remarks
 */
stlChar * eldcGetHashDataSequenceNamePtr( eldcHashDataSequenceName * aHashDataSeqName )
{
    return (stlChar *) aHashDataSeqName
        + STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataSequenceName) );
}


/**
 * @brief Sequence Descriptor 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aSeqDesc   Sequence Descriptor
 * @param[out] aStringBuffer 출력 정보를 위한 String Buffer 공간
 * @remarks
 */
void eldcPrintSequenceDesc( void * aSeqDesc, stlChar * aStringBuffer )
{
    stlInt32        sSize = 0;
    ellSequenceDesc * sSeqDesc = (ellSequenceDesc *) aSeqDesc;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Sequence Descriptor( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sOWNER_ID: %ld, ",
                 aStringBuffer,
                 sSeqDesc->mOwnerID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sSCHEMA_ID: %ld, ",
                 aStringBuffer,
                 sSeqDesc->mSchemaID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sSEQUENCE_ID: %ld, ",
                 aStringBuffer,
                 sSeqDesc->mSequenceID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sTABLESPACE_ID: %ld, ",
                 aStringBuffer,
                 sSeqDesc->mTablespaceID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sPHYSICAL_ID: %ld, ",
                 aStringBuffer,
                 sSeqDesc->mPhysicalID );
    
    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sSEQUENCE_NAME: %s, ",
                         aStringBuffer,
                         sSeqDesc->mSequenceName );

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
 * @brief Sequence ID Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataSeqID       Sequence ID 의 Hash Data
 * @param[out] aStringBuffer        출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataSequenceID( void * aHashDataSeqID, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataSequenceID * sHashData = (eldcHashDataSequenceID *) aHashDataSeqID;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Sequence ID Hash Data( " );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sKEY_SEQUENCE_ID: %ld )",
                         aStringBuffer,
                         sHashData->mKeySequenceID );

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
 * @brief Sequence Name Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataSeqName     Sequence Name 의 Hash Data
 * @param[out] aStringBuffer        출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataSequenceName( void * aHashDataSeqName, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataSequenceName * sHashData = (eldcHashDataSequenceName *) aHashDataSeqName;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Sequence Name Hash Data( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_SCHEMA_ID: %ld, ",
                 aStringBuffer,
                 sHashData->mKeySchemaID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_SEQUENCE_NAME: %s, ",
                 aStringBuffer,
                 sHashData->mKeySequenceName );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sDATA_SEQUENCE_ID: %ld )",
                         aStringBuffer,
                         sHashData->mDataSequenceID );

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
 * @brief SQL-Sequence Cache 를 위한 Dump Handle을 설정
 * @remarks
 */

void eldcSetSQLSequenceDumpHandle()
{
    /*
     * ELDC_OBJECTCACHE_SEQUENCE_ID
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_SEQUENCE_ID].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SEQUENCE_ID );
    gEldcObjectDump[ELDC_OBJECTCACHE_SEQUENCE_ID].mPrintHashData
        = eldcPrintHashDataSequenceID;
    gEldcObjectDump[ELDC_OBJECTCACHE_SEQUENCE_ID].mPrintObjectDesc
        = eldcPrintSequenceDesc;
    
    /*
     * ELDC_OBJECTCACHE_SEQUENCE_NAME
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_SEQUENCE_NAME].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SEQUENCE_NAME );
    gEldcObjectDump[ELDC_OBJECTCACHE_SEQUENCE_NAME].mPrintHashData
        = eldcPrintHashDataSequenceName;
    gEldcObjectDump[ELDC_OBJECTCACHE_SEQUENCE_NAME].mPrintObjectDesc
        = eldcPrintINVALID;
}




/**
 * @brief SQL-Sequence 객체를 위한 Cache 를 구축한다.
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildSequenceCache( smlTransId     aTransID,
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
                                      ELDT_TABLE_ID_SEQUENCES,
                                      & sBucketCnt,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * 소수에 해당하는 Hash Bucket 개수의 조정
     */

    sBucketCnt = ellGetPrimeBucketCnt( sBucketCnt );
    
    /**
     * Sequence ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SEQUENCE_ID ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Sequence Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SEQUENCE_NAME ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLSequenceDumpHandle();
    
    /**
     * SQL-Sequence Cache 구축 
     */

    STL_TRY( eldcBuildCacheSQLSequence( aTransID,
                                        aStmt,
                                        aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief SQL-Sequence 을 위한 Cache 를 구축한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildCacheSQLSequence( smlTransId       aTransID,
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

    smlFetchInfo        sFetchInfo;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.SEQUENCES 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_SEQUENCES];

    /**
     * DEFINITION_SCHEMA.SEQUENCES 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_SEQUENCES,
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
        STL_TRY( eldcInsertCacheSequence( aTransID,
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
 * @brief Sequence ID 를 이용해 Sequence Dictionary Handle 을 획득 
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             View SCN
 * @param[in]  aSequenceID          Sequence ID
 * @param[out] aSequenceDictHandle  Sequence Dictionary Handle
 * @param[out] aExist               존재 여부 
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus eldcGetSequenceHandleByID( smlTransId           aTransID,
                                     smlScn               aViewSCN,
                                     stlInt64             aSequenceID,
                                     ellDictHandle      * aSequenceDictHandle,
                                     stlBool            * aExist,
                                     ellEnv             * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlBool sIsValid = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSequenceID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSequenceDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_SEQUENCE_ID Hash 로부터 검색
     */

    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SEQUENCE_ID ),
                                  & aSequenceID,
                                  eldcHashFuncSequenceID,
                                  eldcCompareFuncSequenceID,
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
             * Sequence Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aSequenceDictHandle,
                              ELL_OBJECT_SEQUENCE,
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
 * @brief Schema ID 와 Sequence Name 을 이용해 Sequence Dictionary Handle 을 획득 
 * @param[in]  aTransID             Transaction ID
 * @param[in]  aViewSCN             View SCN
 * @param[in]  aSchemaID            Schema ID
 * @param[in]  aSequenceName        Sequence Name
 * @param[out] aSequenceDictHandle  Sequence Dictionary Handle
 * @param[out] aExist               존재 여부 
 * @param[in]  aEnv                 Envirionment 
 * @remarks
 */
stlStatus eldcGetSequenceHandleByName( smlTransId           aTransID,
                                       smlScn               aViewSCN,
                                       stlInt64             aSchemaID,
                                       stlChar            * aSequenceName,
                                       ellDictHandle      * aSequenceDictHandle,
                                       stlBool            * aExist,
                                       ellEnv             * aEnv )
{
    eldcHashElement        * sHashElement = NULL;

    eldcHashDataSequenceName * sHashData = NULL;

    stlBool sIsValid = STL_FALSE;
    
    eldcHashKeySequenceName  sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeySequenceName) );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSequenceName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSequenceDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_SEQUENCE_NAME Hash 로부터 검색
     */

    sHashKey.mSchemaID     = aSchemaID;
    sHashKey.mSequenceName = aSequenceName;
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SEQUENCE_NAME ),
                                  & sHashKey,
                                  eldcHashFuncSequenceName,
                                  eldcCompareFuncSequenceName,
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
        sHashData = (eldcHashDataSequenceName *) sHashElement->mHashData;

        STL_TRY( eldcIsVisibleCache( aTransID,
                                     aViewSCN,
                                     & sHashData->mDataHashElement->mVersionInfo,
                                     & sIsValid,
                                     aEnv )
                 == STL_SUCCESS );
        
        if ( sIsValid == STL_TRUE )
        {
            /**
             * Sequence Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aSequenceDictHandle,
                              ELL_OBJECT_SEQUENCE,
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
 * @brief SQL Sequence 을 위한 Cache 정보를 추가한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aValueList        Data Value List
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcInsertCacheSequence( smlTransId          aTransID,
                                   smlStatement      * aStmt,
                                   knlValueBlockList * aValueList,
                                   ellEnv            * aEnv )
{
    ellSessObjectMgr  * sObjMgr = ELL_SESS_OBJ(aEnv);
    
    ellSequenceDesc   * sSeqDesc = NULL;
    
    eldcHashElement * sHashElementSeqID = NULL;
    eldcHashElement * sHashElementSeqName = NULL;
    eldcLocalCache  * sNewLocalCache = NULL;

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
     * Sequence ID를 위한 Sequence Descriptor 구성
     */
    
    STL_TRY( eldcMakeSequenceDesc( aTransID,
                                   aStmt,
                                   aValueList,
                                   & sSeqDesc,
                                   aEnv )
             == STL_SUCCESS );
    
    /**
     * Sequence ID 를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementSequenceID( & sHashElementSeqID,
                                            aTransID,
                                            sSeqDesc,
                                            aEnv )
             == STL_SUCCESS );
                                                  
    /**
     * ELDC_OBJECTCACHE_SEQUENCE_ID Hash 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SEQUENCE_ID ),
                                    sHashElementSeqID,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Sequence Name 을 위한 Hash Element 구성
     */

    STL_TRY( eldcMakeHashElementSequenceName( & sHashElementSeqName,
                                              aTransID,
                                              sSeqDesc,
                                              sHashElementSeqID,
                                              aEnv )
             == STL_SUCCESS );
    
    /**
     * ELDC_OBJECTCACHE_SEQUENCE_NAME 에 추가 
     */

    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SEQUENCE_NAME ),
                                    sHashElementSeqName,
                                    aEnv )
             == STL_SUCCESS );
    
    /**
     * 객체가 변경되고 있음을 세션에 기록한다.
     */
    
    ELL_SESS_ENV(aEnv)->mLocalModifySeq += 1;
    
    eldcInsertLocalCache( sHashElementSeqID, sNewLocalCache, aEnv );

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
 * @brief Sequence ID를 기준으로 SQL-Sequnce Cache 를 추가한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aSeqID        Sequence ID
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcInsertCacheSequenceBySequenceID( smlTransId          aTransID,
                                               smlStatement      * aStmt,
                                               stlInt64            aSeqID,
                                               ellEnv            * aEnv )
{
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    
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

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.SEQUENCES 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_SEQUENCES];

    /**
     * DEFINITION_SCHEMA.SEQUENCES 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_SEQUENCES,
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
     * WHERE SEQUENCE_ID = aSeqID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_SEQUENCES,
                                            ELDT_Sequences_ColumnOrder_SEQUENCE_ID );

    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_SEQUENCES,
                                  sFilterColumn,
                                  & aSeqID,
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
     * SQL-Sequnce Cache 정보를 추가한다.
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
            STL_TRY( eldcInsertCacheSequence( aTransID,
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
 * @brief SQL Sequence 를 위한 Cache 정보를 삭제한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aSeqHandle        Sequence Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcDeleteCacheSequence( smlTransId          aTransID,
                                   smlStatement      * aStmt,
                                   ellDictHandle     * aSeqHandle,
                                   ellEnv            * aEnv )
{
    ellSessObjectMgr  * sObjMgr = ELL_SESS_OBJ(aEnv);
    
    eldcHashElement * sHashElementID = NULL;
    eldcHashElement * sHashElementName = NULL;

    eldcLocalCache  * sLocalCache;
    eldcLocalCache  * sNewLocalCache = NULL;
    
    stlInt64   sSeqID = ELL_DICT_OBJECT_ID_NA;
    
    eldcHashKeySequenceName  sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeySequenceName) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * New Local Cache 공간 할당
     */
    
    STL_TRY( knlAllocDynamicMem( & sObjMgr->mMemSessObj,
                                 STL_SIZEOF(eldcLocalCache),
                                 (void **) & sNewLocalCache,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    
    /**
     * ELDC_OBJECTCACHE_SEQUENCE_ID Hash 에서 삭제 
     */

    sSeqID = ellGetSequenceID( aSeqHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SEQUENCE_ID ),
                                  & sSeqID,
                                  eldcHashFuncSequenceID,
                                  eldcCompareFuncSequenceID,
                                  & sHashElementID,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SEQUENCE_ID ),
                                    sHashElementID,
                                    aEnv )
             == STL_SUCCESS );
                                    
    /**
     * ELDC_OBJECTCACHE_SEQUENCE_NAME Hash 에서 삭제 
     */

    sHashKey.mSchemaID     = ellGetSequenceSchemaID( aSeqHandle );
    sHashKey.mSequenceName = ellGetSequenceName( aSeqHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SEQUENCE_NAME ),
                                  & sHashKey,
                                  eldcHashFuncSequenceName,
                                  eldcCompareFuncSequenceName,
                                  & sHashElementName,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SEQUENCE_NAME ),
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




















/**
 * @brief 읽은 Value List 로부터 SQL Sequence 의 Sequence Descriptor 를 생성한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[out] aSeqDesc         Sequence Descriptor
 * @param[in]  aValueList       Value List
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldcMakeSequenceDesc( smlTransId          aTransID,
                                smlStatement      * aStmt,
                                knlValueBlockList * aValueList,
                                ellSequenceDesc  ** aSeqDesc,
                                ellEnv            * aEnv )
{
    stlInt32           sSeqDescSize = 0;
    ellSequenceDesc  * sSeqDesc = NULL;

    dtlDataValue     * sDataValue = NULL;

    stlInt32           sSeqNameLen = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Sequence Descriptor 를 위한 공간의 크기 계산
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SEQUENCES,
                                   ELDT_Sequences_ColumnOrder_SEQUENCE_NAME );
    sSeqNameLen = sDataValue->mLength;
    
    sSeqDescSize = eldcCalSizeSequenceDesc( sSeqNameLen );
    
    /**
     * Sequence Descriptor 를 위한 공간 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sSeqDesc,
                               sSeqDescSize,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Sequence Descriptor 의 각 정보를 설정한다.
     */

    /*
     * mOwnerID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SEQUENCES,
                                   ELDT_Sequences_ColumnOrder_OWNER_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sSeqDesc->mOwnerID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sSeqDesc->mOwnerID) == sDataValue->mLength );
    }

    /*
     * mSchemaID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SEQUENCES,
                                   ELDT_Sequences_ColumnOrder_SCHEMA_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sSeqDesc->mSchemaID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sSeqDesc->mSchemaID) == sDataValue->mLength );
    }

    /*
     * mTablespaceID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SEQUENCES,
                                   ELDT_Sequences_ColumnOrder_TABLESPACE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sSeqDesc->mTablespaceID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sSeqDesc->mTablespaceID) == sDataValue->mLength );
    }
    
    /*
     * mSequenceID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SEQUENCES,
                                   ELDT_Sequences_ColumnOrder_SEQUENCE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sSeqDesc->mSequenceID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sSeqDesc->mSequenceID) == sDataValue->mLength );
    }

    /*
     * mSeqTableID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SEQUENCES,
                                   ELDT_Sequences_ColumnOrder_SEQUENCE_TABLE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sSeqDesc->mSeqTableID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sSeqDesc->mSeqTableID) == sDataValue->mLength );
    }
    
    /*
     * mPhysicalID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SEQUENCES,
                                   ELDT_Sequences_ColumnOrder_PHYSICAL_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sSeqDesc->mPhysicalID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sSeqDesc->mPhysicalID) == sDataValue->mLength );
    }

    /*
     * mSequenceName
     * - 메모리 공간 내에서의 위치 계산
     */

    sSeqDesc->mSequenceName = eldcGetSequenceNamePtr( sSeqDesc );

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_SEQUENCES,
                                   ELDT_Sequences_ColumnOrder_SEQUENCE_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sSeqDesc->mSequenceName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sSeqDesc->mSequenceName[sDataValue->mLength] = '\0';
    }

    /*
     * mSequenceHandle
     */

    if ( sSeqDesc->mPhysicalID == ELL_DICT_OBJECT_ID_NA )
    {
        STL_ASSERT(0);
    }
    else
    {
        STL_TRY( smlGetSequenceHandle( sSeqDesc->mPhysicalID,
                                       & sSeqDesc->mSequenceHandle,
                                       SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        STL_ASSERT( sSeqDesc->mSequenceHandle != NULL );
    }
    
    /**
     * return 값 설정
     */
    
    *aSeqDesc = sSeqDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sequence ID 를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aSeqDesc        Sequence Descriptor
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementSequenceID( eldcHashElement   ** aHashElement,
                                         smlTransId           aTransID,
                                         ellSequenceDesc    * aSeqDesc,
                                         ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32                sHashValue = 0;
    eldcHashDataSequenceID * sHashDataSeqID = NULL;
    eldcHashElement        * sHashElement   = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataSequenceID();
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataSeqID,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataSeqID, 0x00, sHashDataSize );
    
    sHashDataSeqID->mKeySequenceID = aSeqDesc->mSequenceID;

    /**
     * Hash Value 생성
     */
    
    sHashValue = eldcHashFuncSequenceID( (void *) & aSeqDesc->mSequenceID );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataSeqID,
                                  sHashValue,
                                  aSeqDesc,
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
 * @brief Sequence Name 을 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aSeqDesc        Sequence Descriptor
 * @param[in]  aHashElementID  Sequence ID로부터 구축된 Hash Element
 * @param[in]  aEnv            Environment 
 * @remarks
 */ 
stlStatus eldcMakeHashElementSequenceName( eldcHashElement   ** aHashElement,
                                           smlTransId           aTransID,
                                           ellSequenceDesc    * aSeqDesc,
                                           eldcHashElement    * aHashElementID,
                                           ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32                  sHashValue = 0;
    eldcHashDataSequenceName * sHashDataSeqName = NULL;
    eldcHashElement          * sHashElement     = NULL;

    eldcHashKeySequenceName    sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeySequenceName) );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSeqDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataSequenceName( stlStrlen(aSeqDesc->mSequenceName) );
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataSeqName,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataSeqName, 0x00, sHashDataSize );
    
    sHashDataSeqName->mKeySchemaID = aSeqDesc->mSchemaID;
    
    /* Sequence Name 을 복사할 위치 지정 */
    sHashDataSeqName->mKeySequenceName
        = eldcGetHashDataSequenceNamePtr( sHashDataSeqName );
    
    stlStrcpy( sHashDataSeqName->mKeySequenceName,
               aSeqDesc->mSequenceName );

    sHashDataSeqName->mDataSequenceID = aSeqDesc->mSequenceID;
    sHashDataSeqName->mDataHashElement = aHashElementID;

    /**
     * Hash Value 생성
     */

    sHashKey.mSchemaID     = sHashDataSeqName->mKeySchemaID;
    sHashKey.mSequenceName = sHashDataSeqName->mKeySequenceName;
    
    sHashValue = eldcHashFuncSequenceName( (void *) & sHashKey );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataSeqName,
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

/** @} eldcSequenceDesc */
