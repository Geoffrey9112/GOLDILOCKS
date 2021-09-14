/*******************************************************************************
 * eldcColumnDesc.c
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
 * @file eldcColumnDesc.c
 * @brief Cache for Column descriptor
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>

/**
 * @addtogroup eldcColumnDesc
 * @{
 */



/**
 * @brief Hash Key 로부터 Hash Value 생성을 위한 함수 포인터
 * @param[in] aHashKey     Hash Key(Column ID)
 * @remarks
 */
stlUInt32  eldcHashFuncColumnID( void * aHashKey )
{
    stlInt64  sColumnID = 0;
    
    stlUInt32 sHashValue = 0;

    /**
     * Column ID 로부터 Hash Value 생성 
     */
    
    sColumnID = *(stlInt64 *) aHashKey;
    
    sHashValue = ellGetHashValueOneID( sColumnID );

    return sHashValue;
}

/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key(Column ID)
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncColumnID( void * aHashKey, eldcHashElement * aHashElement )
{
    stlInt64               sColumnID = 0;
    
    eldcHashDataColumnID * sHashData    = NULL;

    /**
     * 입력된 Column ID 와 Hash Element 의 Column ID 를 비교 
     */
    
    sColumnID  = *(stlInt64 *) aHashKey;
    
    sHashData    = (eldcHashDataColumnID *) aHashElement->mHashData;

    if ( sHashData->mKeyColumnID == sColumnID )
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
 * @param[in] aHashKey     Hash Key (Table ID, Column Name)
 * @remarks
 */
stlUInt32  eldcHashFuncColumnName( void * aHashKey )
{
    eldcHashKeyColumnName * sKeyData = NULL;
    
    stlUInt32 sHashValue = 0;

    /**
     * Hash Key 로부터 Hash Value 생성 
     */
    
    sKeyData = (eldcHashKeyColumnName *) aHashKey;
    
    sHashValue = ellGetHashValueNameAndID( sKeyData->mTableID, sKeyData->mColumnName );

    return sHashValue;
}

/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key (Table ID, Column Name )
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncColumnName( void * aHashKey, eldcHashElement * aHashElement )
{
    eldcHashKeyColumnName  * sHashKey = NULL;
    
    eldcHashDataColumnName * sHashData    = NULL;

    /**
     * 입력된 Hash Key 와 Hash Element 의 Key 를 비교 
     */
    
    sHashKey  = (eldcHashKeyColumnName *) aHashKey;
    
    sHashData    = (eldcHashDataColumnName *) aHashElement->mHashData;

    if ( ( sHashData->mKeyTableID == sHashKey->mTableID ) &&
         ( stlStrcmp( sHashData->mKeyColumnName, sHashKey->mColumnName ) == 0 ) )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}






/**
 * @brief Column Descriptor 를 위한 공간을 계산한다.
 * @param[in] aColNameLen    Column Name 의 길이 
 * @param[in] aDefStringLen  Column 의 Default String 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeColumnDesc( stlInt32 aColNameLen, stlInt32 aDefStringLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(ellColumnDesc) );
    
    if ( aColNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aColNameLen + 1);
    }

    if ( aDefStringLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aDefStringLen + 1);
    }

    return sSize;
}

/**
 * @brief Column Descriptor 로부터 Column Name 의 Pointer 위치를 계산
 * @param[in] aColumnDesc   Column Descriptor
 * @remarks
 */
stlChar * eldcGetColumnNamePtr( ellColumnDesc * aColumnDesc )
{
    return (stlChar *) aColumnDesc + STL_ALIGN_DEFAULT( STL_SIZEOF(ellColumnDesc) );
}

/**
 * @brief Column Descriptor 로부터 Column Default String 의 Pointer 위치를 계산
 * @param[in] aColumnDesc   Column Descriptor
 * @param[in] aColNameLen   Column Name 의 길이 
 * @remarks
 */
stlChar * eldcGetColumnDefStringPtr( ellColumnDesc * aColumnDesc, stlInt32 aColNameLen )
{
    return (stlChar *) aColumnDesc
        + STL_ALIGN_DEFAULT( STL_SIZEOF(ellColumnDesc) )
        + STL_ALIGN_DEFAULT(aColNameLen + 1);
}


/**
 * @brief Column ID Hash Data 를 위한 공간을 계산한다.
 * @remarks
 */
stlInt32  eldcCalSizeHashDataColumnID()
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF( eldcHashDataColumnID ) );

    return sSize;
}

/**
 * @brief Column Name Hash Data 를 위한 공간을 계산한다.
 * @param[in]  aColNameLen   Column Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeHashDataColumnName( stlInt32 aColNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataColumnName) );

    if ( aColNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aColNameLen + 1);
    }
    
    return sSize;
}

/**
 * @brief Column Name Hash Data 로부터 Column Name 의 Pointer 위치를 계산
 * @param[in] aHashDataColumnName   Column Name Hash Data
 * @remarks
 */
stlChar * eldcGetHashDataColumnNamePtr( eldcHashDataColumnName * aHashDataColumnName )
{
    return (stlChar *) aHashDataColumnName
        + STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataColumnName) );
}

/**
 * @brief Column Descriptor 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aColumnDesc   Column Descriptor
 * @param[out] aStringBuffer 출력 정보를 위한 String Buffer 공간
 * @remarks
 */
void eldcPrintColumnDesc( void * aColumnDesc, stlChar * aStringBuffer )
{
    stlInt32        sSize = 0;
    ellColumnDesc * sColumnDesc = (ellColumnDesc *)aColumnDesc;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Column Descriptor( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sOWNER_ID: %ld, ",
                 aStringBuffer,
                 sColumnDesc->mOwnerID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sSCHEMA_ID: %ld, ",
                 aStringBuffer,
                 sColumnDesc->mSchemaID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sTABLE_ID: %ld, ",
                 aStringBuffer,
                 sColumnDesc->mTableID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sCOLUMN_ID: %ld, ",
                 aStringBuffer,
                 sColumnDesc->mColumnID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sCOLUMN_NAME: %s, ",
                 aStringBuffer,
                 sColumnDesc->mColumnName );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sORDINAL_POSITION: %d, ",
                 aStringBuffer,
                 sColumnDesc->mOrdinalPosition );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sDEFAULT_STRING: %s, ",
                 aStringBuffer,
                 sColumnDesc->mDefaultString );

    /**
     * Nullable 여부 
     */
    
    if ( sColumnDesc->mNullable == 1 )
    {
        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%sNULLABLE: TRUE, ",
                     aStringBuffer );
    }
    else
    {
        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%sNULLABLE: FALSE, ",
                     aStringBuffer );
    }

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sDATA_TYPE: %d, ",
                 aStringBuffer,
                 sColumnDesc->mDataType );
    
    
    /**
     * Identity Column 여부 
     */
    
    if ( sColumnDesc->mIdentityHandle != NULL )
    {
        sSize = stlSnprintf( aStringBuffer,
                             ELDC_DUMP_PRINT_BUFFER_SIZE,
                             "%sIS_IDENTITY: TRUE )",
                             aStringBuffer );
    }
    else
    {
        sSize = stlSnprintf( aStringBuffer,
                             ELDC_DUMP_PRINT_BUFFER_SIZE,
                             "%sIS_IDENTITY: FALSE )",
                             aStringBuffer );
    }

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
 * @brief Column ID Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataColumnID    Column ID 의 Hash Data
 * @param[out] aStringBuffer        출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataColumnID( void * aHashDataColumnID, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataColumnID * sHashData = (eldcHashDataColumnID *) aHashDataColumnID;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Column ID Hash Data( " );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sKEY_COLUMN_ID: %ld )",
                         aStringBuffer,
                         sHashData->mKeyColumnID );

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
 * @brief Column Name Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataColumnName  Column Name 의 Hash Data
 * @param[out] aStringBuffer        출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataColumnName( void * aHashDataColumnName, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataColumnName * sHashData = (eldcHashDataColumnName *) aHashDataColumnName;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Column Name Hash Data( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_TABLE_ID: %ld, ",
                 aStringBuffer,
                 sHashData->mKeyTableID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_COLUMN_NAME: %s, ",
                 aStringBuffer,
                 sHashData->mKeyColumnName );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sDATA_COLUMN_ID: %ld )",
                         aStringBuffer,
                         sHashData->mDataColumnID );

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
 * @brief SQL-Column Cache 를 위한 Dump Handle을 설정
 * @remarks
 */

void eldcSetSQLColumnDumpHandle()
{
    /*
     * ELDC_OBJECTCACHE_COLUMN_ID
     */
    
    gEldcObjectDump[ELDC_OBJECTCACHE_COLUMN_ID].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_COLUMN_ID );
    gEldcObjectDump[ELDC_OBJECTCACHE_COLUMN_ID].mPrintHashData
        = eldcPrintHashDataColumnID;
    gEldcObjectDump[ELDC_OBJECTCACHE_COLUMN_ID].mPrintObjectDesc
        = eldcPrintColumnDesc;

    /*
     * ELDC_OBJECTCACHE_COLUMN_NAME
     */
    
    gEldcObjectDump[ELDC_OBJECTCACHE_COLUMN_NAME].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_COLUMN_NAME );
    gEldcObjectDump[ELDC_OBJECTCACHE_COLUMN_NAME].mPrintHashData
        = eldcPrintHashDataColumnName;
    gEldcObjectDump[ELDC_OBJECTCACHE_COLUMN_NAME].mPrintObjectDesc
        = eldcPrintINVALID;
}


/**
 * @brief SQL-Column 객체를 위한 Cache 를 구축한다.
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildColumnCache( smlTransId     aTransID,
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
                                      ELDT_TABLE_ID_COLUMNS,
                                      & sBucketCnt,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * 소수에 해당하는 Hash Bucket 개수의 조정
     */

    sBucketCnt = ellGetPrimeBucketCnt( sBucketCnt );
    
    /**
     * Column ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_COLUMN_ID ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Column Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_COLUMN_NAME ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLColumnDumpHandle();
    
    /**
     * SQL-Column Cache 구축 
     */

    STL_TRY( eldcBuildCacheSQLColumn( aTransID,
                                      aStmt,
                                      aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SQL-Column 을 위한 Cache 를 구축한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildCacheSQLColumn( smlTransId       aTransID,
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

    smlFetchInfo          sFetchInfo;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.COLUMNS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_COLUMNS];

    /**
     * DEFINITION_SCHEMA.COLUMNS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_COLUMNS,
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
     * Column Descriptor 를 Cache 에 추가한다.
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
        STL_TRY( eldcInsertCacheColumn( aTransID,
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
 * @brief Column ID 를 이용해 Column Handle 을 획득
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aColumnID          Column ID
 * @param[out] aColumnDictHandle  Column Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Environment
 * @remarks
 */
stlStatus eldcGetColumnHandleByID( smlTransId           aTransID,
                                   smlScn               aViewSCN,
                                   stlInt64             aColumnID,
                                   ellDictHandle      * aColumnDictHandle,
                                   stlBool            * aExist,
                                   ellEnv             * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlBool sIsValid = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aColumnID != ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_COLUMN_ID Hash 로부터 검색
     */

    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_COLUMN_ID ),
                                  & aColumnID,
                                  eldcHashFuncColumnID,
                                  eldcCompareFuncColumnID,
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
             * Column Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aColumnDictHandle,
                              ELL_OBJECT_COLUMN,
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
 * @brief Column Name 을 이용해 Column Handle 을 획득
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aTableID           Table ID
 * @param[in]  aColumnName        Column Name
 * @param[out] aColumnDictHandle  Column Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment
 * @remarks
 */
stlStatus eldcGetColumnHandleByName( smlTransId           aTransID,
                                     smlScn               aViewSCN,
                                     stlInt64             aTableID,
                                     stlChar            * aColumnName,
                                     ellDictHandle      * aColumnDictHandle,
                                     stlBool            * aExist,
                                     ellEnv             * aEnv )
{
    eldcHashElement        * sHashElement = NULL;
    
    eldcHashDataColumnName * sHashData = NULL;

    stlBool sIsValid = STL_FALSE;
    
    eldcHashKeyColumnName  sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeyColumnName) );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableID != ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_COLUMN_NAME Hash 로부터 검색
     */

    sHashKey.mTableID    = aTableID;
    sHashKey.mColumnName = aColumnName;
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_COLUMN_NAME ),
                                  & sHashKey,
                                  eldcHashFuncColumnName,
                                  eldcCompareFuncColumnName,
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
        
        sHashData = (eldcHashDataColumnName *) sHashElement->mHashData;
        
        STL_TRY( eldcIsVisibleCache( aTransID,
                                     aViewSCN,
                                     & sHashData->mDataHashElement->mVersionInfo,
                                     & sIsValid,
                                     aEnv )
                 == STL_SUCCESS );
        
        if ( sIsValid == STL_TRUE )
        {
            /**
             * Column Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aColumnDictHandle,
                              ELL_OBJECT_COLUMN,
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
 * @brief SQL Column 을 위한 Cache 정보를 추가한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aValueList        Data Value List
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcInsertCacheColumn( smlTransId          aTransID,
                                 smlStatement      * aStmt,
                                 knlValueBlockList * aValueList,
                                 ellEnv            * aEnv )
{
    ellColumnDesc   * sColumnDesc = NULL;
    
    eldcHashElement * sHashElementColumnID = NULL;
    eldcHashElement * sHashElementColumnName = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Column ID를 위한 Column Descriptor 구성
     */
    
    STL_TRY( eldcMakeColumnDesc( aTransID,
                                 aStmt,
                                 aValueList,
                                 & sColumnDesc,
                                 aEnv )
             == STL_SUCCESS );
    
    /**
     * Column ID 를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementColumnID( & sHashElementColumnID,
                                          aTransID,
                                          sColumnDesc,
                                          aEnv )
             == STL_SUCCESS );
                                                  
    /**
     * ELDC_OBJECTCACHE_COLUMN_ID Hash 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_COLUMN_ID ),
                                    sHashElementColumnID,
                                    aEnv )
             == STL_SUCCESS );

    if ( sColumnDesc->mUnused == STL_TRUE )
    {
        STL_ASSERT( sColumnDesc->mColumnName == NULL );

        /**
         * UNUSED 컬럼은 named cache 를 구축하지 않음.
         */
    }
    else
    {
        /**
         * Column Name 을 위한 Hash Element 구성
         */
        
        STL_TRY( eldcMakeHashElementColumnName( & sHashElementColumnName,
                                                aTransID,
                                                sColumnDesc,
                                                sHashElementColumnID,
                                                aEnv )
                 == STL_SUCCESS );
        
        /**
         * ELDC_OBJECTCACHE_COLUMN_NAME 에 추가 
         */
        
        STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_COLUMN_NAME ),
                                        sHashElementColumnName,
                                        aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Table ID를 기준으로 SQL-Column Cache 를 추가한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aTableID      Table ID
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcInsertCacheColumnByTableID( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aTableID,
                                          ellEnv            * aEnv )
{
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
    
    smlRid       sRowRid;
    smlScn       sRowScn;
    smlRowBlock  sRowBlock;
    eldMemMark   sMemMark;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    
    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID != ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.COLUMNS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_COLUMNS];

    /**
     * DEFINITION_SCHEMA.COLUMNS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_COLUMNS,
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
     * WHERE TABLE_ID = aTableID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_COLUMNS,
                                            ELDT_Columns_ColumnOrder_TABLE_ID );

    /**
     * Key Range 생성
     */

    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_COLUMNS,
                                  sFilterColumn,
                                  & aTableID,
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
     * SQL-Column Cache 정보를 추가한다.
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
            STL_TRY( eldcInsertCacheColumn( aTransID,
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
 * @brief SQL Column 을 위한 Cache 정보를 삭제한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aColumnHandle     Column Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcDeleteCacheColumn( smlTransId          aTransID,
                                 ellDictHandle     * aColumnHandle,
                                 ellEnv            * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlInt64   sColumnID = ELL_DICT_OBJECT_ID_NA;
    
    eldcHashKeyColumnName  sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeyColumnName) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aColumnHandle != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * ELDC_OBJECTCACHE_COLUMN_ID Hash 에서 삭제 
     */

    sColumnID = ellGetColumnID( aColumnHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_COLUMN_ID ),
                                  & sColumnID,
                                  eldcHashFuncColumnID,
                                  eldcCompareFuncColumnID,
                                  & sHashElement,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_COLUMN_ID ),
                                    sHashElement,
                                    aEnv )
             == STL_SUCCESS );

    if ( ellGetColumnUnused( aColumnHandle ) == STL_TRUE )
    {
        STL_ASSERT( ellGetColumnName( aColumnHandle ) == NULL );

        /**
         * UNUSED Column 은 Named Cache 가 구축되어 있지 않음
         */
    }
    else
    {
        /**
         * ELDC_OBJECTCACHE_COLUMN_NAME Hash 로부터 검색
         */

        sHashKey.mTableID    = ellGetColumnTableID( aColumnHandle );
        sHashKey.mColumnName = ellGetColumnName( aColumnHandle );
    
        STL_TRY( eldcFindHashElement( aTransID,
                                      SML_MAXIMUM_STABLE_SCN,
                                      ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_COLUMN_NAME ),
                                      & sHashKey,
                                      eldcHashFuncColumnName,
                                      eldcCompareFuncColumnName,
                                      & sHashElement,
                                      aEnv )
                 == STL_SUCCESS );

        STL_TRY( eldcDeleteHashElement( aTransID,
                                        ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_COLUMN_NAME ),
                                        sHashElement,
                                        aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief 읽은 Value List 로부터 SQL Column 의 Column Descriptor 를 생성한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[out] aColumnDesc      Column Descriptor
 * @param[in]  aValueList       Value List
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldcMakeColumnDesc( smlTransId          aTransID,
                              smlStatement      * aStmt,
                              knlValueBlockList * aValueList,
                              ellColumnDesc    ** aColumnDesc,
                              ellEnv            * aEnv )
{
    stlInt64           sDTD = 0;
    stlInt64           sPhysicalID = ELL_DICT_OBJECT_ID_NA;
    
    stlInt32           sColumnDescSize = 0;
    ellColumnDesc    * sColumnDesc = NULL;

    dtlDataValue     * sDataValue = NULL;

    stlInt32           sColNameLen = 0;
    stlInt32           sDefStrLen = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Column Descriptor 를 위한 공간의 크기 계산
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_COLUMNS,
                                   ELDT_Columns_ColumnOrder_COLUMN_NAME );
    sColNameLen = sDataValue->mLength;

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_COLUMNS,
                                   ELDT_Columns_ColumnOrder_COLUMN_DEFAULT );
    sDefStrLen = sDataValue->mLength;
    
    sColumnDescSize = eldcCalSizeColumnDesc( sColNameLen, sDefStrLen );
    
    /**
     * Column Descriptor 를 위한 공간 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sColumnDesc,
                               sColumnDescSize,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Column Descriptor 의 각 정보를 설정한다.
     */

    /*
     * mOwnerID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_COLUMNS,
                                   ELDT_Columns_ColumnOrder_OWNER_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sColumnDesc->mOwnerID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sColumnDesc->mOwnerID) == sDataValue->mLength );
    }

    /*
     * mSchemaID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_COLUMNS,
                                   ELDT_Columns_ColumnOrder_SCHEMA_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sColumnDesc->mSchemaID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sColumnDesc->mSchemaID) == sDataValue->mLength );
    }

    /*
     * mTableID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_COLUMNS,
                                   ELDT_Columns_ColumnOrder_TABLE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sColumnDesc->mTableID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sColumnDesc->mTableID) == sDataValue->mLength );
    }

    /*
     * mColumnID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_COLUMNS,
                                   ELDT_Columns_ColumnOrder_COLUMN_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sColumnDesc->mColumnID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sColumnDesc->mColumnID) == sDataValue->mLength );
    }

    /*
     * mColumnName
     * - 메모리 공간 내에서의 위치 계산
     */

    sColumnDesc->mColumnName = eldcGetColumnNamePtr( sColumnDesc );

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_COLUMNS,
                                   ELDT_Columns_ColumnOrder_COLUMN_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        /**
         * UNUSED Column 은 Column Name 이 없다.
         */
        sColumnDesc->mColumnName = NULL;
    }
    else
    {
        stlMemcpy( sColumnDesc->mColumnName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sColumnDesc->mColumnName[sDataValue->mLength] = '\0';
    }

    /*
     * mOrdinalPosition
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_COLUMNS,
                                   ELDT_Columns_ColumnOrder_PHYSICAL_ORDINAL_POSITION );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sColumnDesc->mOrdinalPosition,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sColumnDesc->mOrdinalPosition) == sDataValue->mLength );

        /* Process 상의 ordinal position 으로 변경 */
        sColumnDesc->mOrdinalPosition
            = ELD_ORDINAL_POSITION_TO_PROCESS(sColumnDesc->mOrdinalPosition);
    }

    /*
     * mDefaultString
     * - 메모리 위치를 계산 
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_COLUMNS,
                                   ELDT_Columns_ColumnOrder_COLUMN_DEFAULT );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        sColumnDesc->mDefaultString = NULL;
    }
    else
    {
        sColumnDesc->mDefaultString = eldcGetColumnDefStringPtr( sColumnDesc, sColNameLen );
        
        stlMemcpy( sColumnDesc->mDefaultString,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sColumnDesc->mDefaultString[sDataValue->mLength] = '\0';
    }
    
    /*
     * mNullable
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_COLUMNS,
                                   ELDT_Columns_ColumnOrder_IS_NULLABLE );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sColumnDesc->mNullable,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sColumnDesc->mNullable) == sDataValue->mLength );
    }

    /*
     * Updatable
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_COLUMNS,
                                   ELDT_Columns_ColumnOrder_IS_UPDATABLE );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sColumnDesc->mUpdatable,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sColumnDesc->mUpdatable) == sDataValue->mLength );
    }

    /*
     * Unused
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_COLUMNS,
                                   ELDT_Columns_ColumnOrder_IS_UNUSED );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sColumnDesc->mUnused,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sColumnDesc->mUnused) == sDataValue->mLength );
    }
    
    /*
     * mIdentityPhyID 
     * mIdentityHandle
     * mIdentityGenOption
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_COLUMNS,
                                   ELDT_Columns_ColumnOrder_IDENTITY_PHYSICAL_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        sColumnDesc->mIdentityPhyID = ELL_DICT_OBJECT_ID_NA;
        sColumnDesc->mIdentityHandle = NULL;
        sColumnDesc->mIdentityGenOption = ELL_IDENTITY_GENERATION_NA;
    }
    else
    {
        stlMemcpy( & sPhysicalID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sPhysicalID) == sDataValue->mLength );
        
        sColumnDesc->mIdentityPhyID = sPhysicalID;
        
        STL_TRY( smlGetSequenceHandle( sPhysicalID,
                                       & sColumnDesc->mIdentityHandle,
                                       SML_ENV(aEnv) )
                 == STL_SUCCESS );

        sDataValue = eldFindDataValue( aValueList,
                                       ELDT_TABLE_ID_COLUMNS,
                                       ELDT_Columns_ColumnOrder_IDENTITY_GENERATION_ID );

        stlMemcpy( & sColumnDesc->mIdentityGenOption,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(ellIdentityGenOption) == sDataValue->mLength );
    }
    
    /*
     * mDataType
     * - DEFINTION_SCHEMA.DATA_TYPE_DESCRIPTOR 로부터 획득해야 함.
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_COLUMNS,
                                   ELDT_Columns_ColumnOrder_DTD_IDENTIFIER );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sDTD,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sDTD) == sDataValue->mLength );
    }

    STL_TRY( eldcSetColumnType( aTransID,
                                aStmt,
                                sDTD,
                                sColumnDesc,
                                aEnv )
             == STL_SUCCESS );
        
    /**
     * return 값 설정
     */
    
    *aColumnDesc = sColumnDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief Column ID 를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aColumnDesc     Column Descriptor
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementColumnID( eldcHashElement   ** aHashElement,
                                       smlTransId           aTransID,
                                       ellColumnDesc      * aColumnDesc,
                                       ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32              sHashValue = 0;
    eldcHashDataColumnID * sHashDataColumnID = NULL;
    eldcHashElement      * sHashElement      = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataColumnID();
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataColumnID,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataColumnID, 0x00, sHashDataSize );
    
    sHashDataColumnID->mKeyColumnID = aColumnDesc->mColumnID;

    /**
     * Hash Value 생성
     */
    
    sHashValue = eldcHashFuncColumnID( (void *) & aColumnDesc->mColumnID );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataColumnID,
                                  sHashValue,
                                  aColumnDesc,
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
 * @brief Column Name 을 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aColumnDesc     Column Descriptor
 * @param[in]  aHashElementID  Column ID로부터 구축된 Hash Element
 * @param[in]  aEnv            Environment 
 * @remarks
 */ 
stlStatus eldcMakeHashElementColumnName( eldcHashElement   ** aHashElement,
                                         smlTransId           aTransID,
                                         ellColumnDesc      * aColumnDesc,
                                         eldcHashElement    * aHashElementID,
                                         ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32                sHashValue = 0;
    eldcHashDataColumnName * sHashDataColumnName = NULL;
    eldcHashElement        * sHashElement        = NULL;

    eldcHashKeyColumnName    sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeyColumnName) );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataColumnName( stlStrlen(aColumnDesc->mColumnName) );
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataColumnName,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataColumnName, 0x00, sHashDataSize );
    
    sHashDataColumnName->mKeyTableID = aColumnDesc->mTableID;
    
    /* Column Name 을 복사할 위치 지정 */
    sHashDataColumnName->mKeyColumnName
        = eldcGetHashDataColumnNamePtr( sHashDataColumnName );
    
    stlStrcpy( sHashDataColumnName->mKeyColumnName,
               aColumnDesc->mColumnName );

    sHashDataColumnName->mDataColumnID = aColumnDesc->mColumnID;
    sHashDataColumnName->mDataHashElement = aHashElementID;

    /**
     * Hash Value 생성
     */

    sHashKey.mTableID = sHashDataColumnName->mKeyTableID;
    sHashKey.mColumnName = sHashDataColumnName->mKeyColumnName;
    
    sHashValue = eldcHashFuncColumnName( (void *) & sHashKey );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataColumnName,
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


/**
 * @brief DTD(Data Type Descriptor) ID 로부터 컬럼 타입 정보를 설정한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aDTD          Data Type Desciptor ID
 * @param[in]  aColumnDesc   Column Descriptor (여기에 정보를 채움)
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcSetColumnType( smlTransId             aTransID,
                             smlStatement         * aStmt,
                             stlInt64               aDTD,
                             ellColumnDesc        * aColumnDesc,
                             ellEnv               * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;

    dtlDataValue      * sDataValue = NULL;
    
    dtlDataType          sDataType = DTL_TYPE_MAX;
    dtlStringLengthUnit  sLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
    stlInt64             sLength     = 0;
    stlInt64             sLength2    = 0;
    stlInt32             sPrecision  = 0;
    stlInt32             sPrecision2 = 0;
    stlInt32             sScale      = 0;
    stlInt32             sRadix      = DTL_NUMERIC_PREC_RADIX_NA;
    dtlIntervalIndicator sIntervalType = DTL_INTERVAL_INDICATOR_NA;
    
    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    knlValueBlockList   * sFilterColumn = NULL;

    void              * sIndexHandle = NULL;
    knlValueBlockList * sIndexValueList = NULL;
    knlKeyCompareList * sKeyCompList = NULL;
    
    knlPredicateList    * sRangePred = NULL;
    knlExprContextInfo  * sRangeContext = NULL;
    
    smlRid       sRowRid;
    smlScn       sRowScn;
    smlRowBlock  sRowBlock;
    eldMemMark   sMemMark;
    
    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    
    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aDTD > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnDesc != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR];

    /**
     * DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
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
     * WHERE DTD_IDENTIFIER = aDTD
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                            ELDT_DTDesc_ColumnOrder_DTD_IDENTIFIER );
    
    if ( gEldCacheBuilding == STL_TRUE )
    {
        /**
         * Start-Up 수행 시간이 Column 개수의 제곱에 비례하여 증가하게 된다.
         * Index Cache 가 존재하지 않아 General 한 Key Range 방식이 아닌
         * hard-coded key range 로 start-up 성능 문제를 해결한다.
         */

        STL_TRY( eldcMakeNonCacheRange( aTransID,
                                        aStmt,
                                        sFilterColumn,
                                        aDTD,
                                        & sKeyCompList,
                                        & sIndexValueList,
                                        & sRangePred,
                                        & sRangeContext,
                                        aEnv )
                 == STL_SUCCESS );
        
        sIndexHandle = gEldNonCacheIndexPhyHandle4DTD;
    }
    else
    {
        /**
         * Key Range 생성
         */

        STL_TRY( eldMakeOneEquiRange( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                      sFilterColumn,
                                      & aDTD,
                                      STL_SIZEOF(stlInt64),
                                      & sIndexHandle,
                                      & sKeyCompList,
                                      & sIndexValueList,
                                      & sRangePred,
                                      & sRangeContext,
                                      aEnv )
                 == STL_SUCCESS );
    }

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
     * Column Type 정보를 읽는다.
     */

    STL_TRY( eldFetchNext( sIterator,
                           & sFetchInfo, 
                           aEnv )
             == STL_SUCCESS );

    STL_ASSERT( sFetchInfo.mIsEndOfScan == STL_FALSE );
    
    /*
     * Data Type
     */
    
    sDataValue = eldFindDataValue( sTableValueList,
                                   ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                   ELDT_DTDesc_ColumnOrder_DATA_TYPE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sDataType,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sDataType) == sDataValue->mLength );
    }

    aColumnDesc->mDataType = sDataType;
    
    switch ( sDataType )
    {
        case DTL_TYPE_BOOLEAN:
            {
                // nothing todo
                break;
            }
        case DTL_TYPE_NATIVE_SMALLINT:
        case DTL_TYPE_NATIVE_INTEGER:
        case DTL_TYPE_NATIVE_BIGINT:
        case DTL_TYPE_NATIVE_REAL:
        case DTL_TYPE_NATIVE_DOUBLE:
            {
                /*
                 * Precision 
                 */
                
                sDataValue = eldFindDataValue( sTableValueList,
                                               ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                               ELDT_DTDesc_ColumnOrder_NUMERIC_PRECISION );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & sPrecision,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(sPrecision) == sDataValue->mLength );
                }
                
                aColumnDesc->mColumnType.mBinaryNum.mBinaryPrec = sPrecision;
                break;
            }
        case DTL_TYPE_FLOAT:
        case DTL_TYPE_NUMBER:
            {
                /*
                 * Radix
                 */
                sDataValue = eldFindDataValue( sTableValueList,
                                               ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                               ELDT_DTDesc_ColumnOrder_NUMERIC_PRECISION_RADIX );

                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & sRadix,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(sRadix) == sDataValue->mLength );
                }

                /*
                 * Precision 
                 */

                sDataValue = eldFindDataValue( sTableValueList,
                                               ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                               ELDT_DTDesc_ColumnOrder_NUMERIC_PRECISION );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & sPrecision,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(sPrecision) == sDataValue->mLength );
                }

                /*
                 * Scale
                 */
                
                sDataValue = eldFindDataValue( sTableValueList,
                                               ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                               ELDT_DTDesc_ColumnOrder_NUMERIC_SCALE );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & sScale,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(sScale) == sDataValue->mLength );
                }

                aColumnDesc->mColumnType.mDecimalNum.mRadix = sRadix;
                aColumnDesc->mColumnType.mDecimalNum.mPrec  = sPrecision;
                aColumnDesc->mColumnType.mDecimalNum.mScale = sScale;
                break;
            }
        case DTL_TYPE_DECIMAL:
            {
                STL_ASSERT(0);
                break;
            }
        case DTL_TYPE_CHAR:
        case DTL_TYPE_VARCHAR:
        case DTL_TYPE_LONGVARCHAR:
            {
                /*
                 * Character Length Unit
                 */

                sDataValue =
                    eldFindDataValue( sTableValueList,
                                      ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                      ELDT_DTDesc_ColumnOrder_STRING_LENGTH_UNIT_ID );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & sLengthUnit,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(dtlStringLengthUnit) == sDataValue->mLength );
                }
                
                aColumnDesc->mColumnType.mString.mLengthUnit = sLengthUnit;
                
                /*
                 * Character Length
                 */
                
                sDataValue =
                    eldFindDataValue( sTableValueList,
                                      ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                      ELDT_DTDesc_ColumnOrder_CHARACTER_MAXIMUM_LENGTH );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & sLength,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(sLength) == sDataValue->mLength );
                }

                aColumnDesc->mColumnType.mString.mCharLength  = sLength;
                
                /*
                 * Octet Length
                 */

                sDataValue =
                    eldFindDataValue( sTableValueList,
                                      ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                      ELDT_DTDesc_ColumnOrder_CHARACTER_OCTET_LENGTH );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & sLength2,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(sLength2) == sDataValue->mLength );
                }
                
                aColumnDesc->mColumnType.mString.mOctetLength = sLength2;
                break;
            }
        case DTL_TYPE_CLOB:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case DTL_TYPE_BINARY:
        case DTL_TYPE_VARBINARY:
        case DTL_TYPE_LONGVARBINARY:
            {
                /*
                 * Octet Length
                 */

                sDataValue =
                    eldFindDataValue( sTableValueList,
                                      ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                      ELDT_DTDesc_ColumnOrder_CHARACTER_OCTET_LENGTH );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & sLength,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(sLength) == sDataValue->mLength );
                }
                
                aColumnDesc->mColumnType.mBinary.mByteLength = sLength;
                break;
            }
        case DTL_TYPE_BLOB:
            {
                STL_THROW( RAMP_ERR_NOT_IMPLEMENTED );
                break;
            }
        case DTL_TYPE_DATE:
            {
                // nothing to do
                break;
            }
        case DTL_TYPE_TIME:
        case DTL_TYPE_TIMESTAMP:
        case DTL_TYPE_TIME_WITH_TIME_ZONE:
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
            {
                /*
                 * fractional second precision
                 */

                sDataValue =
                    eldFindDataValue( sTableValueList,
                                      ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                      ELDT_DTDesc_ColumnOrder_DATETIME_PRECISION );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & sPrecision,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(sScale) == sDataValue->mLength );
                }

                aColumnDesc->mColumnType.mDateTime.mFracSecPrec = sPrecision;
                break;
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
            {
                /*
                 * interval type
                 */

                sDataValue =
                    eldFindDataValue( sTableValueList,
                                      ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                      ELDT_DTDesc_ColumnOrder_INTERVAL_TYPE_ID );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & sIntervalType,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(sIntervalType) == sDataValue->mLength );
                }

                /*
                 * leading precision
                 */

                sDataValue =
                    eldFindDataValue( sTableValueList,
                                      ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                      ELDT_DTDesc_ColumnOrder_INTERVAL_PRECISION );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & sPrecision,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(sPrecision) == sDataValue->mLength );
                }
                
                /*
                 * fractional second precision
                 */

                sDataValue =
                    eldFindDataValue( sTableValueList,
                                      ELDT_TABLE_ID_DATA_TYPE_DESCRIPTOR,
                                      ELDT_DTDesc_ColumnOrder_DATETIME_PRECISION );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    sPrecision2 = DTL_PRECISION_NA;
                }
                else
                {
                    stlMemcpy( & sPrecision2,
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(sPrecision2) == sDataValue->mLength );
                }
                
                
                aColumnDesc->mColumnType.mInterval.mIntervalType = sIntervalType;
                aColumnDesc->mColumnType.mInterval.mStartPrec    = sPrecision;
                aColumnDesc->mColumnType.mInterval.mEndPrec      = sPrecision2;
                break;
            }
        case DTL_TYPE_ROWID:
            {
                // nothing to do
                break;
            }
        default:
            {
                STL_ASSERT(0);
                break;
            }
    }
    
    /**
     * 한 건만 존재해야 함
     */

    STL_TRY( eldFetchNext( sIterator,
                           & sFetchInfo, 
                           aEnv )
             == STL_SUCCESS );

    STL_ASSERT( sFetchInfo.mIsEndOfScan == STL_TRUE );

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

    STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      ELL_ERROR_STACK(aEnv),
                      "eldcSetColumnType()" );
    }
    
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


/** @} eldcColumnDesc */
