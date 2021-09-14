/*******************************************************************************
 * eldcIndexDesc.c
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
 * @file eldcIndexDesc.c
 * @brief Cache for Index descriptor
 */

#include <ell.h>
#include <eldt.h>                               
#include <eldc.h>

#include <eldDictionary.h>
#include <elgPendOp.h>



/**
 * @addtogroup eldcIndexDesc
 * @{
 */


/**
 * @brief Hash Key 로부터 Hash Value 생성을 위한 함수 포인터
 * @param[in] aHashKey     Hash Key(Index ID)
 * @remarks
 */
stlUInt32  eldcHashFuncIndexID( void * aHashKey )
{
    stlInt64  sIndexID = 0;
    
    stlUInt32 sHashValue = 0;

    /**
     * Index ID 로부터 Hash Value 생성 
     */
    
    sIndexID = *(stlInt64 *) aHashKey;
    
    sHashValue = ellGetHashValueOneID( sIndexID );

    return sHashValue;
}


/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key(Index ID)
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncIndexID( void * aHashKey, eldcHashElement * aHashElement )
{
    stlInt64              sIndexID = 0;
    
    eldcHashDataIndexID * sHashData = NULL;

    /**
     * 입력된 Index ID 와 Hash Element 의 Index ID 를 비교 
     */
    
    sIndexID  = *(stlInt64 *) aHashKey;
    
    sHashData    = (eldcHashDataIndexID *) aHashElement->mHashData;

    if ( sHashData->mKeyIndexID == sIndexID )
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
 * @param[in] aHashKey     Hash Key (Schema ID, Index Name)
 * @remarks
 */
stlUInt32  eldcHashFuncIndexName( void * aHashKey )
{
    eldcHashKeyIndexName * sKeyData = NULL;
    
    stlUInt32 sHashValue = 0;

    /**
     * Hash Key 로부터 Hash Value 생성 
     */
    
    sKeyData = (eldcHashKeyIndexName *) aHashKey;
    
    sHashValue = ellGetHashValueNameAndID( sKeyData->mSchemaID, sKeyData->mIndexName );

    return sHashValue;
}


/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key (Schema ID, Index Name )
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncIndexName( void * aHashKey, eldcHashElement * aHashElement )
{
    eldcHashKeyIndexName  * sHashKey = NULL;
    
    eldcHashDataIndexName * sHashData    = NULL;

    /**
     * 입력된 Hash Key 와 Hash Element 의 Key 를 비교 
     */
    
    sHashKey  = (eldcHashKeyIndexName *) aHashKey;
    
    sHashData    = (eldcHashDataIndexName *) aHashElement->mHashData;

    if ( ( sHashData->mKeySchemaID == sHashKey->mSchemaID ) &&
         ( stlStrcmp( sHashData->mKeyIndexName, sHashKey->mIndexName ) == 0 ) )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}



/**
 * @brief Index Descriptor 를 위한 공간을 계산한다.
 * @param[in] aIndexNameLen    Index Name 의 길이
 * @remarks
 */
stlInt32  eldcCalSizeIndexDesc( stlInt32     aIndexNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(ellIndexDesc) );
    
    if ( aIndexNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aIndexNameLen + 1);
    }

    return sSize;
}


/**
 * @brief Index Descriptor 로부터 Column Name 의 Pointer 위치를 계산
 * @param[in] aIndexDesc   Index Descriptor
 * @remarks
 */
stlChar * eldcGetIndexNamePtr( ellIndexDesc * aIndexDesc )
{
    return (stlChar *) aIndexDesc + STL_ALIGN_DEFAULT( STL_SIZEOF(ellIndexDesc) );
}


/**
 * @brief Index ID  Hash Data 를 위한 공간을 계산한다.
 * @remarks
 */
stlInt32  eldcCalSizeHashDataIndexID()
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF( eldcHashDataIndexID ) );

    return sSize;
}


/**
 * @brief Index Name Hash Data 를 위한 공간을 계산한다.
 * @param[in]    aIndexNameLen    Index Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeHashDataIndexName( stlInt32 aIndexNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataIndexName) );

    if ( aIndexNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aIndexNameLen + 1);
    }
    
    return sSize;
}


/**
 * @brief Index Name Hash Data 로부터 Index Name 의 Pointer 위치를 계산
 * @param[in] aHashDataIndexName   Index Name 의 Hash Data
 * @remarks
 */
stlChar * eldcGetHashDataIndexNamePtr( eldcHashDataIndexName * aHashDataIndexName )
{
    return (stlChar *) aHashDataIndexName
        + STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataIndexName) );
}





/**
 * @brief Index Descriptor 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aIndexDesc    Index Descriptor
 * @param[out] aStringBuffer 출력 정보를 위한 String Buffer 공간
 * @remarks
 */
void eldcPrintIndexDesc( void * aIndexDesc, stlChar * aStringBuffer )
{
    stlInt32       i = 0;

    stlInt32          sSize = 0;
    ellIndexDesc    * sIndexDesc = (ellIndexDesc *)aIndexDesc;
    
    ellIndexKeyDesc * sKeyDesc = NULL;
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Index Descriptor( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sOWNER_ID: %ld, ",
                 aStringBuffer,
                 sIndexDesc->mOwnerID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sSCHEMA_ID: %ld, ",
                 aStringBuffer,
                 sIndexDesc->mSchemaID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sINDEX_ID: %ld, ",
                 aStringBuffer,
                 sIndexDesc->mIndexID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sTABLESPACE_ID: %ld, ",
                 aStringBuffer,
                 sIndexDesc->mTablespaceID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sPHYSICAL_ID: %ld, ",
                 aStringBuffer,
                 sIndexDesc->mPhysicalID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sINDEX_NAME: %s, ",
                 aStringBuffer,
                 sIndexDesc->mIndexName );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sINDEX_TYPE: %s, ",
                 aStringBuffer,
                 gEllIndexTypeString[sIndexDesc->mIndexType] );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sIS_UNIQUE: %d, ",
                 aStringBuffer,
                 sIndexDesc->mIsUnique );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sINVALID: %d, ",
                 aStringBuffer,
                 sIndexDesc->mInvalid );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sBY_CONSTRAINT: %d, ",
                 aStringBuffer,
                 sIndexDesc->mByConstraint );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_COLUMN_CNT: %ld, ",
                 aStringBuffer,
                 sIndexDesc->mKeyColumnCnt );

    /**
     * Key Column 개수만큼 컬럼 정보를 출력해야 함
     */

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_COLUMNS[",
                 aStringBuffer );
    
    sKeyDesc = sIndexDesc->mKeyDesc;
    for ( i = 0; i < sIndexDesc->mKeyColumnCnt; i++ )
    {
        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s ID:%ld %s %s ",
                     aStringBuffer,
                     ellGetColumnID( & sKeyDesc[i].mKeyColumnHandle ),
                     gEllIndexColumnOrderString[sKeyDesc[i].mKeyOrdering],
                     gEllIndexColumnNullsOrderString[sKeyDesc[i].mKeyNullOrdering]);
    }

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s ] )",
                         aStringBuffer );
    
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
 * @brief Index ID Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataIndexID    Index ID 의 Hash Data
 * @param[out] aStringBuffer       출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataIndexID( void * aHashDataIndexID, stlChar * aStringBuffer )
{
    stlInt32                 sSize = 0;
    eldcHashDataIndexID    * sHashData = (eldcHashDataIndexID *) aHashDataIndexID;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Index ID Hash Data( " );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sKEY_INDEX_ID: %ld )",
                         aStringBuffer,
                         sHashData->mKeyIndexID );

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
 * @brief Index Name Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataIndexName  Index Name 의 Hash Data
 * @param[out] aStringBuffer        출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataIndexName( void * aHashDataIndexName, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataIndexName * sHashData = (eldcHashDataIndexName *) aHashDataIndexName;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Index Name Hash Data( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_SCHEMA_ID: %ld, ",
                 aStringBuffer,
                 sHashData->mKeySchemaID );
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_INDEX_NAME: %s, ",
                 aStringBuffer,
                 sHashData->mKeyIndexName );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sDATA_INDEX_ID: %ld )",
                         aStringBuffer,
                         sHashData->mDataIndexID );

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



/*************************************************************
 * 초기화, 마무리 함수
 *************************************************************/


/**
 * @brief SQL-Index Cache 를 위한 Dump Handle을 설정
 * @remarks
 */

void eldcSetSQLIndexDumpHandle()
{
    /*
     * ELDC_OBJECTCACHE_INDEX_ID
     */
    
    gEldcObjectDump[ELDC_OBJECTCACHE_INDEX_ID].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_INDEX_ID );
    gEldcObjectDump[ELDC_OBJECTCACHE_INDEX_ID].mPrintHashData
        = eldcPrintHashDataIndexID;
    gEldcObjectDump[ELDC_OBJECTCACHE_INDEX_ID].mPrintObjectDesc
        = eldcPrintIndexDesc;

    /*
     * ELDC_OBJECTCACHE_INDEX_NAME
     */
    
    gEldcObjectDump[ELDC_OBJECTCACHE_INDEX_NAME].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_INDEX_NAME );
    gEldcObjectDump[ELDC_OBJECTCACHE_INDEX_NAME].mPrintHashData
        = eldcPrintHashDataIndexName;
    gEldcObjectDump[ELDC_OBJECTCACHE_INDEX_NAME].mPrintObjectDesc
        = eldcPrintINVALID;
}



/**
 * @brief SQL-Index 객체를 위한 Cache 를 구축한다.
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildIndexCache( smlTransId     aTransID,
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
                                      ELDT_TABLE_ID_INDEXES,
                                      & sBucketCnt,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * 소수에 해당하는 Hash Bucket 개수의 조정
     */

    sBucketCnt = ellGetPrimeBucketCnt( sBucketCnt );
    
    /**
     * Index ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_INDEX_ID ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Index Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_INDEX_NAME ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLIndexDumpHandle();
    
    /**
     * SQL-Index Cache 구축 
     */

    STL_TRY( eldcBuildCacheSQLIndex( aTransID,
                                     aStmt,
                                     aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief SQL-Index 을 위한 Cache 를 구축한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildCacheSQLIndex( smlTransId       aTransID,
                                  smlStatement   * aStmt,
                                  ellEnv         * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;
    
    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    dtlDataValue      * sDataValue = NULL;
    
    stlInt64            sIndexID = ELL_DICT_OBJECT_ID_NA;
    stlBool             sIndexExist = STL_FALSE;
    ellIndexDesc      * sIndexDesc = NULL;
    ellDictHandle       sIndexHandle;

    smlRid               sRowRid;
    smlScn               sRowScn;
    smlRowBlock          sRowBlock;
    eldMemMark           sMemMark;

    smlFetchInfo          sFetchInfo;

    stlInt32  sState = 0;
        
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.INDEXES 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_INDEXES];

    /**
     * DEFINITION_SCHEMA.INDEXES 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_INDEXES,
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
        
        STL_TRY( eldcInsertCacheIndex( aTransID,
                                       aStmt,
                                       sTableValueList,
                                       aEnv )
                 == STL_SUCCESS );

        /**
         * INDEX_ID 를 획득
         */
        
        sDataValue = eldFindDataValue( sTableValueList,
                                       ELDT_TABLE_ID_INDEXES,
                                       ELDT_Indexes_ColumnOrder_INDEX_ID );
        
        if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
        {
            STL_ASSERT(0);
        }
        else
        {
            stlMemcpy( & sIndexID,
                       sDataValue->mValue,
                       sDataValue->mLength );
            STL_DASSERT( STL_SIZEOF(sIndexID) == sDataValue->mLength );
        }

        /**
         * INDEX_ID 에 해당하는 Index Handle 획득 
         */
        
        STL_TRY( eldcGetIndexHandleByID( aTransID,
                                         SML_MAXIMUM_STABLE_SCN,
                                         sIndexID,
                                         & sIndexHandle,
                                         & sIndexExist,
                                         aEnv )
                 == STL_SUCCESS );
        
        sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( & sIndexHandle );

        /**
         * Index 와 관련된 연결 정보 설정
         */
        
        if ( sIndexDesc->mByConstraint == STL_TRUE )
        {
            /**
             * Key Constraint 에 의해 생성된 Index 는
             * Constraint Cache 구축 시
             * Primary Key, Unique Key, Foreign Key Constraint 관련 정보에 추가한다.
             */
            continue;
        }
        else
        {
            if ( sIndexDesc->mIsUnique == STL_TRUE )
            {
                /**
                 * Table Cache 에 Unique Index 정보를 추가 
                 */
                
                STL_TRY( eldcAddUniqueIndexIntoTable( aTransID,
                                                      sIndexID,
                                                      aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                /**
                 * Table Cache 에 Non-Key Index 정보를 추가 
                 */
                
                STL_TRY( eldcAddNonUniqueIndexIntoTable( aTransID,
                                                         sIndexID,
                                                         aEnv )
                         == STL_SUCCESS );
            }
        }

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


/*************************************************************
 * 조회 함수
 *************************************************************/


/**
 * @brief Index ID 를 이용해 Index Dictionary Handle 을 획득 
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aIndexID           Index ID
 * @param[out] aIndexDictHandle   Index Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus eldcGetIndexHandleByID( smlTransId           aTransID,
                                  smlScn               aViewSCN,
                                  stlInt64             aIndexID,
                                  ellDictHandle      * aIndexDictHandle,
                                  stlBool            * aExist,
                                  ellEnv             * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlBool   sIsValid = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aIndexID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_INDEX_ID Hash 로부터 검색
     */

    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_INDEX_ID ),
                                  & aIndexID,
                                  eldcHashFuncIndexID,
                                  eldcCompareFuncIndexID,
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
             * Index Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aIndexDictHandle,
                              ELL_OBJECT_INDEX,
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
 * @brief Index Name을 이용해 Index Dictionary Handle 을 획득 
 * @param[in]  aTransID           Transaction ID
 * @param[in]  aViewSCN           View SCN
 * @param[in]  aSchemaID          Schema ID
 * @param[in]  aIndexName         Index Name
 * @param[out] aIndexDictHandle   Index Dictionary Handle
 * @param[out] aExist             존재 여부 
 * @param[in]  aEnv               Envirionment 
 * @remarks
 */
stlStatus eldcGetIndexHandleByName( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aSchemaID,
                                    stlChar            * aIndexName,
                                    ellDictHandle      * aIndexDictHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv )
{
    eldcHashElement        * sHashElement = NULL;

    eldcHashDataIndexName  * sHashData = NULL;

    stlBool   sIsValid = STL_FALSE;
    
    eldcHashKeyIndexName  sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeyIndexName) );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aSchemaID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexDictHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_INDEX_NAME Hash 로부터 검색
     */

    sHashKey.mSchemaID  = aSchemaID;
    sHashKey.mIndexName = aIndexName;
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_INDEX_NAME ),
                                  & sHashKey,
                                  eldcHashFuncIndexName,
                                  eldcCompareFuncIndexName,
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
        sHashData = (eldcHashDataIndexName *) sHashElement->mHashData;
        STL_TRY( eldcIsVisibleCache( aTransID,
                                     aViewSCN,
                                     & sHashData->mDataHashElement->mVersionInfo,
                                     & sIsValid,
                                     aEnv )
                 == STL_SUCCESS );
        
        if ( sIsValid == STL_TRUE )
        {
            /**
             * Index Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aIndexDictHandle,
                              ELL_OBJECT_INDEX,
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





/*************************************************************
 * 제어 함수
 *************************************************************/





/**
 * @brief SQL Index 을 위한 Cache 정보를 추가한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aValueList        Data Value List
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcInsertCacheIndex( smlTransId          aTransID,
                                smlStatement      * aStmt,
                                knlValueBlockList * aValueList,
                                ellEnv            * aEnv )
{
    ellIndexDesc   * sIndexDesc = NULL;
    
    eldcHashElement * sHashElementIndexID = NULL;
    eldcHashElement * sHashElementIndexName = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Index ID를 위한 Index Descriptor 구성
     */
    
    STL_TRY( eldcMakeIndexDesc( aTransID,
                                aStmt,
                                aValueList,
                                & sIndexDesc,
                                aEnv )
             == STL_SUCCESS );
    
    /**
     * Index ID 를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementIndexID( & sHashElementIndexID,
                                         aTransID,
                                         sIndexDesc,
                                         aEnv )
             == STL_SUCCESS );
                                                  
    /**
     * ELDC_OBJECTCACHE_INDEX_ID Hash 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_INDEX_ID ),
                                    sHashElementIndexID,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Index Name 을 위한 Hash Element 구성
     */

    STL_TRY( eldcMakeHashElementIndexName( & sHashElementIndexName,
                                           aTransID,
                                           sIndexDesc,
                                           sHashElementIndexID,
                                           aEnv )
             == STL_SUCCESS );
    
    /**
     * ELDC_OBJECTCACHE_INDEX_NAME 에 추가 
     */

    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_INDEX_NAME ),
                                    sHashElementIndexName,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Index ID를 기준으로 SQL-Index Cache 를 추가한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aIndexID      Index ID
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcInsertCacheIndexByIndexID( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aIndexID,
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

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.INDEXES 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_INDEXES];

    /**
     * DEFINITION_SCHEMA.INDEXES 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_INDEXES,
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
     * WHERE INDEX_ID = aIndexID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_INDEXES,
                                            ELDT_Indexes_ColumnOrder_INDEX_ID );
    
    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_INDEXES,
                                  sFilterColumn,
                                  & aIndexID,
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
     * SQL-Index Cache 정보를 추가한다.
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
            STL_TRY( eldcInsertCacheIndex( aTransID,
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
 * @brief Table ID 를 기준으로 SQL-Index Cache 를 추가한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aTableID      Table ID
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcInsertCacheIndexByTableID( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aTableID,
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

    dtlDataValue      * sTargetDataValue = NULL;
    stlInt64            sTargetIndexID = ELL_DICT_OBJECT_ID_NA;
    stlBool             sTargetIndexExist = STL_FALSE;
    ellIndexDesc      * sTargetIndexDesc = NULL;
    ellDictHandle       sTargetIndexHandle;
    
    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );

    /**
     * DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE];

    /**
     * DEFINITION_SCHEMA.INDEX_KEY_TABLE_USAGE 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,
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
                                            ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,
                                            ELDT_IndexTable_ColumnOrder_TABLE_ID );
    
    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,
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
     * INDEX_ID 를 얻는다.
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
            /**
             * INDEX_ID 를 획득
             */
            
            sTargetDataValue = eldFindDataValue( sTableValueList,
                                                 ELDT_TABLE_ID_INDEX_KEY_TABLE_USAGE,
                                                 ELDT_IndexTable_ColumnOrder_INDEX_ID );

            if ( DTL_IS_NULL( sTargetDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sTargetIndexID,
                           sTargetDataValue->mValue,
                           sTargetDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sTargetIndexID) == sTargetDataValue->mLength );
            }

            /**
             * Index Cache 를 구축
             */
            STL_TRY( eldcInsertCacheIndexByIndexID( aTransID,
                                                    aStmt,
                                                    sTargetIndexID,
                                                    aEnv )
                     == STL_SUCCESS );

            /**
             * INDEX_ID 에 해당하는 Index Handle 획득 
             */
        
            STL_TRY( eldcGetIndexHandleByID( aTransID,
                                             SML_MAXIMUM_STABLE_SCN,
                                             sTargetIndexID,
                                             & sTargetIndexHandle,
                                             & sTargetIndexExist,
                                             aEnv )
                     == STL_SUCCESS );
        
            sTargetIndexDesc = (ellIndexDesc *) ellGetObjectDesc( & sTargetIndexHandle );

            /**
             * Index 와 관련된 연결 정보 설정
             */
        
            if ( sTargetIndexDesc->mByConstraint == STL_TRUE )
            {
                /**
                 * Key Constraint 에 의해 생성된 Index 는
                 * Constraint Cache 구축 시
                 * Primary Key, Unique Key, Foreign Key Constraint 관련 정보에 추가한다.
                 */
                continue;
            }
            else
            {
                if ( sTargetIndexDesc->mIsUnique == STL_TRUE )
                {
                    /**
                     * Table Cache 에 Unique Index 정보를 추가 
                     */
                
                    STL_TRY( eldcAddUniqueIndexIntoTable( aTransID,
                                                          sTargetIndexID,
                                                          aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /**
                     * Table Cache 에 Non-Key Index 정보를 추가 
                     */
                
                    STL_TRY( eldcAddNonUniqueIndexIntoTable( aTransID,
                                                             sTargetIndexID,
                                                             aEnv )
                             == STL_SUCCESS );
                }
            }
            
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
 * @brief SQL Index 을 위한 Cache 정보를 삭제한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aIndexHandle      Index Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcDeleteCacheIndex( smlTransId          aTransID,
                                ellDictHandle     * aIndexHandle,
                                ellEnv            * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlInt64   sIndexID = ELL_DICT_OBJECT_ID_NA;
    
    eldcHashKeyIndexName  sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeyIndexName) );
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aIndexHandle != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * ELDC_OBJECTCACHE_INDEX_ID Hash 에서 삭제 
     */

    sIndexID = ellGetIndexID( aIndexHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_INDEX_ID ),
                                  & sIndexID,
                                  eldcHashFuncIndexID,
                                  eldcCompareFuncIndexID,
                                  & sHashElement,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_INDEX_ID ),
                                    sHashElement,
                                    aEnv )
             == STL_SUCCESS );
                                    
    /**
     * ELDC_OBJECTCACHE_INDEX_NAME Hash 로부터 검색
     */

    sHashKey.mSchemaID  = ellGetIndexSchemaID( aIndexHandle );
    sHashKey.mIndexName = ellGetIndexName( aIndexHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_INDEX_NAME ),
                                  & sHashKey,
                                  eldcHashFuncIndexName,
                                  eldcCompareFuncIndexName,
                                  & sHashElement,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_INDEX_NAME ),
                                    sHashElement,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief SQL-Table Element 에 Unique Index Element 를 추가한다.
 * @param[in] aTransID       Transaction ID
 * @param[in] aIndexID       Index ID
 * @param[in] aEnv           Environment
 * @remarks
 */ 
stlStatus eldcAddUniqueIndexIntoTable( smlTransId     aTransID,
                                       stlInt64       aIndexID,
                                       ellEnv       * aEnv )
{
    stlInt64              sTableID = 0;
    eldcHashElement     * sTableElement = NULL;
    eldcHashDataTableID * sTableHashData = NULL;

    stlBool               sIndexExist = STL_FALSE;
    ellIndexDesc        * sIndexDesc = NULL;
    ellDictHandle         sIndexHandle;
    stlMemset( & sIndexHandle, 0x00, STL_SIZEOF(ellDictHandle) );

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aIndexID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );

    /**
     * INDEX_ID 에 해당하는 Index Handle 획득
     */
                
    STL_TRY( eldcGetIndexHandleByID( aTransID,
                                     SML_MAXIMUM_STABLE_SCN,
                                     aIndexID,
                                     & sIndexHandle,
                                     & sIndexExist,
                                     aEnv )
             == STL_SUCCESS );
    STL_ASSERT( sIndexExist == STL_TRUE );
    
    sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( & sIndexHandle );

    /**
     * TABLE_ID 를 획득
     * 향후, 다수의 Table 과 연관된 인덱스라면 Key Column 정보를 모두 검색해야 한다.
     */

    sTableID = ellGetColumnTableID( & sIndexDesc->mKeyDesc[0].mKeyColumnHandle );

    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 로부터 검색
     */
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                  & sTableID,
                                  eldcHashFuncTableID,
                                  eldcCompareFuncTableID,
                                  & sTableElement,
                                  aEnv )
             == STL_SUCCESS );
            
    /**
     * Table ID Hash Element 에 Index Hash Element 를 등록 
     */
    
    sTableHashData = (eldcHashDataTableID *) sTableElement->mHashData;
    
    STL_TRY( eldcAddHashRelated( aTransID,
                                 sTableElement,
                                 & sTableHashData->mUniqueIndex,
                                 sIndexHandle.mObjHashElement,
                                 ELL_OBJECT_INDEX,
                                 aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief SQL-Table Element 에서 Unique Index Element 를 제거한다.
 * @param[in] aTransID       Transaction ID
 * @param[in] aTableHandle   Table Dictionary Handle
 * @param[in] aIndexHandle   Index Dictionary Handle
 * @param[in] aEnv           Environment
 * @remarks
 */ 
stlStatus eldcDelUniqueIndexFromTable( smlTransId      aTransID,
                                       ellDictHandle * aTableHandle,
                                       ellDictHandle * aIndexHandle,
                                       ellEnv        * aEnv )
{
    stlInt64              sTableID = 0;
    eldcHashElement     * sTableElement = NULL;
    eldcHashDataTableID * sTableHashData = NULL;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 로부터 검색
     */

    sTableID = ellGetTableID( aTableHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                  & sTableID,
                                  eldcHashFuncTableID,
                                  eldcCompareFuncTableID,
                                  & sTableElement,
                                  aEnv )
             == STL_SUCCESS );
            
    /**
     * Table ID Hash Element 에서 Index Hash Element 를 제거 
     */
    
    sTableHashData = (eldcHashDataTableID *) sTableElement->mHashData;
    
    STL_TRY( eldcDelHashRelated( aTransID,
                                 sTableElement,
                                 & sTableHashData->mUniqueIndex,
                                 (eldcHashElement*)aIndexHandle->mObjHashElement,
                                 aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SQL-Table Element 에 Non-Unique Index Element 를 추가한다.
 * @param[in] aTransID       Transaction ID
 * @param[in] aIndexID       Index ID
 * @param[in] aEnv           Environment
 * @remarks
 */ 
stlStatus eldcAddNonUniqueIndexIntoTable( smlTransId     aTransID,
                                          stlInt64       aIndexID,
                                          ellEnv       * aEnv )
{
    stlInt64              sTableID = 0;
    eldcHashElement     * sTableElement = NULL;
    eldcHashDataTableID * sTableHashData = NULL;

    stlBool               sIndexExist = STL_FALSE;
    ellIndexDesc        * sIndexDesc = NULL;
    ellDictHandle         sIndexHandle;
    stlMemset( & sIndexHandle, 0x00, STL_SIZEOF(ellDictHandle) );

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aIndexID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );

    /**
     * INDEX_ID 에 해당하는 Index Handle 획득
     */
                
    STL_TRY( eldcGetIndexHandleByID( aTransID,
                                     SML_MAXIMUM_STABLE_SCN,
                                     aIndexID,
                                     & sIndexHandle,
                                     & sIndexExist,
                                     aEnv )
             == STL_SUCCESS );
    STL_ASSERT( sIndexExist == STL_TRUE );
    
    sIndexDesc = (ellIndexDesc *) ellGetObjectDesc( & sIndexHandle );

    /**
     * TABLE_ID 를 획득
     * 향후, 다수의 Table 과 연관된 인덱스라면 Key Column 정보를 모두 검색해야 한다.
     */

    sTableID = ellGetColumnTableID( & sIndexDesc->mKeyDesc[0].mKeyColumnHandle );

    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 로부터 검색
     */
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                  & sTableID,
                                  eldcHashFuncTableID,
                                  eldcCompareFuncTableID,
                                  & sTableElement,
                                  aEnv )
             == STL_SUCCESS );
            
    /**
     * Table ID Hash Element 에 Index Hash Element 를 등록 
     */
    
    sTableHashData = (eldcHashDataTableID *) sTableElement->mHashData;
    
    STL_TRY( eldcAddHashRelated( aTransID,
                                 sTableElement,
                                 & sTableHashData->mNonUniqueIndex,
                                 sIndexHandle.mObjHashElement,
                                 ELL_OBJECT_INDEX,
                                 aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief SQL-Table Element 에서 Non-Unique Index Element 를 제거한다.
 * @param[in] aTransID       Transaction ID
 * @param[in] aTableHandle   Table Dictionary Handle
 * @param[in] aIndexHandle   Index Dictionary Handle
 * @param[in] aEnv           Environment
 * @remarks
 */ 
stlStatus eldcDelNonUniqueIndexFromTable( smlTransId      aTransID,
                                          ellDictHandle * aTableHandle,
                                          ellDictHandle * aIndexHandle,
                                          ellEnv        * aEnv )
{
    stlInt64              sTableID = 0;
    eldcHashElement     * sTableElement = NULL;
    eldcHashDataTableID * sTableHashData = NULL;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 로부터 검색
     */

    sTableID = ellGetTableID( aTableHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                  & sTableID,
                                  eldcHashFuncTableID,
                                  eldcCompareFuncTableID,
                                  & sTableElement,
                                  aEnv )
             == STL_SUCCESS );
            
    /**
     * Table ID Hash Element 에서 Index Hash Element 를 제거 
     */
    
    sTableHashData = (eldcHashDataTableID *) sTableElement->mHashData;
    
    STL_TRY( eldcDelHashRelated( aTransID,
                                 sTableElement,
                                 & sTableHashData->mNonUniqueIndex,
                                 (eldcHashElement*)aIndexHandle->mObjHashElement,
                                 aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Unique/Primary Key 생성시 SQL-Table Cache 에
 *        동일한 Key Column 을 갖는 SQL-Index Cache 가 존재하는 지 확인
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aTableHandle   Table Dictionary Handle
 * @param[in]  aIndexHandle   Index Dictionary Handle
 * @param[out] aExist         동일한 Key 를 갖는 인덱스 존재 여부 
 * @param[in]  aEnv           Environment
 * @note Runtime Validation 으로 다른 Tx 의 종료 여부에 관계없이 검사한다.
 * @remarks
 * 다음과 같은 범위에서 검사한다.
 * - Primary Key
 * - Unique Key 
 * 다음과 같은 사항을 검사한다.
 * - Key Column 의 동일 여부
 */
stlStatus eldcRuntimeCheckSameKey4UniqueKey( smlTransId      aTransID,
                                             ellDictHandle * aTableHandle,
                                             ellDictHandle * aIndexHandle,
                                             stlBool       * aExist,
                                             ellEnv        * aEnv )
{
    stlBool  sSameKey = STL_FALSE;

    stlInt64              sTableID = 0;
    eldcHashElement     * sTableElement = NULL;
    eldcHashDataTableID * sTableHashData = NULL;

    eldcHashRelatedObjList * sObjectList = NULL;
    
    ellDictHandle          * sKeyHandle = NULL;
    ellDictHandle          * sIndexHandle = NULL;
    
    stlBool    sStateLatch = STL_FALSE;
    stlBool    sTimeout = STL_FALSE;
    stlBool    sIsValid = STL_TRUE;

    stlInt32          sKeyCnt = 0;
    ellIndexKeyDesc * sKeyDesc = NULL;

    stlInt32          sInKeyCnt = 0;
    ellIndexKeyDesc * sInKeyDesc = NULL;

    stlInt32  i = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * 기본 정보 획득
     */
    
    sInKeyCnt  = ellGetIndexKeyCount( aIndexHandle );
    sInKeyDesc = ellGetIndexKeyDesc( aIndexHandle );
    
    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 로부터 검색
     */

    sTableID = ellGetTableID( aTableHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                  & sTableID,
                                  eldcHashFuncTableID,
                                  eldcCompareFuncTableID,
                                  & sTableElement,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * X Latch 획득
     */

    STL_TRY( knlAcquireLatch( & sTableElement->mWriteLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,   /* interval */
                              & sTimeout,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sTimeout == STL_FALSE );
    sStateLatch = STL_TRUE;
    
    /*************************************************************
     * SQL-Table Cache 정보에서 동일한 Key Column 을 갖는 인덱스를 검색 
     *************************************************************/
    
    sTableHashData = (eldcHashDataTableID *) sTableElement->mHashData;

    /**
     * Primary Key 에서 확인
     */
    
    for ( sObjectList = sTableHashData->mPrimaryKey;
          sObjectList != NULL;
          sObjectList = sObjectList->mNext )
    {
        /**
         * 다른 Trasaction 에 대해서만 검사
         */

        if ( (aTransID == sObjectList->mVersionInfo.mCreateTransID) ||
             (aTransID == sObjectList->mVersionInfo.mDropTransID) )
        {
            /* 동일한 Tx 임 */
            continue;
        }
        else
        {
            STL_TRY( eldcIsVisibleCache( sObjectList->mVersionInfo.mCreateTransID,
                                         SML_MAXIMUM_STABLE_SCN,
                                         &sObjectList->mVersionInfo,
                                         &sIsValid,
                                         aEnv )
                     == STL_SUCCESS );

            if( sIsValid == STL_FALSE )
            {
                continue;
            }
        }
        
        sKeyHandle = & sObjectList->mRelatedHandle;
        sIndexHandle = ellGetConstraintIndexDictHandle( sKeyHandle );

        sKeyCnt  = ellGetIndexKeyCount( sIndexHandle );
        sKeyDesc = ellGetIndexKeyDesc( sIndexHandle );

        if ( sKeyCnt == sInKeyCnt )
        {
            sSameKey = STL_TRUE;
            for ( i = 0; i < sKeyCnt; i++ )
            {
                if ( ellGetColumnID( & sKeyDesc[i].mKeyColumnHandle ) ==
                     ellGetColumnID( & sInKeyDesc[i].mKeyColumnHandle ) )
                {
                    /* 동일한 Key 임 */
                }
                else
                {
                    sSameKey = STL_FALSE;
                    break;
                }
            }

            STL_TRY_THROW( sSameKey != STL_TRUE, RAMP_SAME_KEY );
        }
        else
        {
            /* Key 개수가 다름 */
        }
    }

    /**
     * Unique Key 에서 확인
     */
    
    for ( sObjectList = sTableHashData->mUniqueKey;
          sObjectList != NULL;
          sObjectList = sObjectList->mNext )
    {
        /**
         * 다른 Trasaction 에 대해서만 검사
         */

        if ( (aTransID == sObjectList->mVersionInfo.mCreateTransID) ||
             (aTransID == sObjectList->mVersionInfo.mDropTransID) )
        {
            /* 동일한 Tx 임 */
            continue;
        }
        else
        {
            STL_TRY( eldcIsVisibleCache( sObjectList->mVersionInfo.mCreateTransID,
                                         SML_MAXIMUM_STABLE_SCN,
                                         &sObjectList->mVersionInfo,
                                         &sIsValid,
                                         aEnv )
                     == STL_SUCCESS );

            if( sIsValid == STL_FALSE )
            {
                continue;
            }
        }
        
        sKeyHandle = & sObjectList->mRelatedHandle;
        sIndexHandle = ellGetConstraintIndexDictHandle( sKeyHandle );

        sKeyCnt  = ellGetIndexKeyCount( sIndexHandle );
        sKeyDesc = ellGetIndexKeyDesc( sIndexHandle );

        if ( sKeyCnt == sInKeyCnt )
        {
            sSameKey = STL_TRUE;
            for ( i = 0; i < sKeyCnt; i++ )
            {
                if ( ellGetColumnID( & sKeyDesc[i].mKeyColumnHandle ) ==
                     ellGetColumnID( & sInKeyDesc[i].mKeyColumnHandle ) )
                {
                    /* 동일한 Key 임 */
                }
                else
                {
                    sSameKey = STL_FALSE;
                    break;
                }
            }

            STL_TRY_THROW( sSameKey != STL_TRUE, RAMP_SAME_KEY );
        }
        else
        {
            /* Key 개수가 다름 */
        }
    }

    
    /**
     * Same Key Index 가 존재한다면...
     */
    
    STL_RAMP( RAMP_SAME_KEY );
    
    /**
     * X Latch 해제 
     */

    sStateLatch = STL_FALSE;
    STL_TRY( knlReleaseLatch( & sTableElement->mWriteLatch,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Output 설정
     */

    *aExist = sSameKey;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if ( sStateLatch == STL_TRUE )
    {
        (void) knlReleaseLatch( & sTableElement->mWriteLatch,
                                KNL_ENV(aEnv) );

        sStateLatch = STL_FALSE;
    }
    
    return STL_FAILURE;
}



/**
 * @brief Foreign Key 생성시 SQL-Table Cache 에
 *        동일한 Key Column 을 갖는 SQL-Index Cache 가 존재하는 지 확인
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aTableHandle   Table Dictionary Handle
 * @param[in]  aIndexHandle   Index Dictionary Handle
 * @param[out] aExist         동일한 Key 를 갖는 인덱스 존재 여부 
 * @param[in]  aEnv           Environment
 * @note Runtime Validation 으로 다른 Tx 의 종료 여부에 관계없이 검사한다.
 * @remarks
 * 다음과 같은 범위에서 검사한다.
 * - Foreign Key
 * 다음과 같은 사항을 검사한다.
 * - Key Column 의 동일 여부
 * 본 함수 호출 후 추가적으로 다음 사항을 검사해야 한다.
 * - Parent Key 가 동일한지의 여부 
 */
stlStatus eldcRuntimeCheckSameKey4ForeignKey( smlTransId      aTransID,
                                              ellDictHandle * aTableHandle,
                                              ellDictHandle * aIndexHandle,
                                              stlBool       * aExist,
                                              ellEnv        * aEnv )
{
    stlBool  sSameKey = STL_FALSE;

    stlInt64              sTableID = 0;
    eldcHashElement     * sTableElement = NULL;
    eldcHashDataTableID * sTableHashData = NULL;

    eldcHashRelatedObjList * sObjectList = NULL;
    
    ellDictHandle          * sKeyHandle = NULL;
    ellDictHandle          * sIndexHandle = NULL;
    
    stlBool    sStateLatch = STL_FALSE;
    stlBool    sTimeout = STL_FALSE;
    stlBool    sIsValid = STL_TRUE;

    stlInt32          sKeyCnt = 0;
    ellIndexKeyDesc * sKeyDesc = NULL;

    stlInt32          sInKeyCnt = 0;
    ellIndexKeyDesc * sInKeyDesc = NULL;

    stlInt32  i = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * 기본 정보 획득
     */
    
    sInKeyCnt  = ellGetIndexKeyCount( aIndexHandle );
    sInKeyDesc = ellGetIndexKeyDesc( aIndexHandle );
    
    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 로부터 검색
     */

    sTableID = ellGetTableID( aTableHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                  & sTableID,
                                  eldcHashFuncTableID,
                                  eldcCompareFuncTableID,
                                  & sTableElement,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * X Latch 획득
     */

    STL_TRY( knlAcquireLatch( & sTableElement->mWriteLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,   /* interval */
                              & sTimeout,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sTimeout == STL_FALSE );
    sStateLatch = STL_TRUE;
    
    /*************************************************************
     * SQL-Table Cache 정보에서 동일한 Key Column 을 갖는 인덱스를 검색 
     *************************************************************/
    
    sTableHashData = (eldcHashDataTableID *) sTableElement->mHashData;

    /**
     * Foreign Key 에서 확인
     */

    for ( sObjectList = sTableHashData->mForeignKey;
          sObjectList != NULL;
          sObjectList = sObjectList->mNext )
    {
        /**
         * 다른 Trasaction 에 대해서만 검사
         */

        if ( (aTransID == sObjectList->mVersionInfo.mCreateTransID) ||
             (aTransID == sObjectList->mVersionInfo.mDropTransID) )
        {
            /* 동일한 Tx 임 */
            continue;
        }
        else
        {
            STL_TRY( eldcIsVisibleCache( sObjectList->mVersionInfo.mCreateTransID,
                                         SML_MAXIMUM_STABLE_SCN,
                                         &sObjectList->mVersionInfo,
                                         &sIsValid,
                                         aEnv )
                     == STL_SUCCESS );

            if( sIsValid == STL_FALSE )
            {
                continue;
            }
        }
        
        sKeyHandle = & sObjectList->mRelatedHandle;
        sIndexHandle = ellGetConstraintIndexDictHandle( sKeyHandle );

        sKeyCnt  = ellGetIndexKeyCount( sIndexHandle );
        sKeyDesc = ellGetIndexKeyDesc( sIndexHandle );

        if ( sKeyCnt == sInKeyCnt )
        {
            sSameKey = STL_TRUE;
            for ( i = 0; i < sKeyCnt; i++ )
            {
                if ( ellGetColumnID( & sKeyDesc[i].mKeyColumnHandle ) ==
                     ellGetColumnID( & sInKeyDesc[i].mKeyColumnHandle ) )
                {
                    /* 동일한 Key 임 */
                }
                else
                {
                    sSameKey = STL_FALSE;
                    break;
                }
            }

            STL_TRY_THROW( sSameKey != STL_TRUE, RAMP_SAME_KEY );
        }
        else
        {
            /* Key 개수가 다름 */
        }
    }

    /**
     * Same Key Index 가 존재한다면...
     */
    
    STL_RAMP( RAMP_SAME_KEY );
    
    /**
     * X Latch 해제 
     */

    sStateLatch = STL_FALSE;
    STL_TRY( knlReleaseLatch( & sTableElement->mWriteLatch,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Output 설정
     */

    *aExist = sSameKey;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if ( sStateLatch == STL_TRUE )
    {
        (void) knlReleaseLatch( & sTableElement->mWriteLatch,
                                KNL_ENV(aEnv) );

        sStateLatch = STL_FALSE;
    }
    
    return STL_FAILURE;
}



/**
 * @brief Index 생성시 SQL-Table Cache 에
 *        동일한 Key Column 을 갖는 SQL-Index Cache 가 존재하는 지 확인
 * @param[in]  aTransID       Transaction ID
 * @param[in]  aTableHandle   Table Dictionary Handle
 * @param[in]  aIndexHandle   Index Dictionary Handle
 * @param[out] aExist         동일한 Key 를 갖는 인덱스 존재 여부 
 * @param[in]  aEnv           Environment
 * @note Runtime Validation 으로 다른 Tx 의 종료 여부에 관계없이 검사한다.
 * @remarks
 * 다음과 같은 범위에서 검사한다.
 * - Unique Index
 * - Non-Unique Index
 * 다음과 같은 사항을 검사한다.
 * - Key Column 의 동일 여부
 * - Key Order(Asc/Desc) 의 동일 여부
 * - Null Order(First/Last) 의 동일 여부
 */
stlStatus eldcRuntimeCheckSameKey4Index( smlTransId      aTransID,
                                         ellDictHandle * aTableHandle,
                                         ellDictHandle * aIndexHandle,
                                         stlBool       * aExist,
                                         ellEnv        * aEnv )
{
    stlBool  sSameKey = STL_FALSE;

    stlInt64              sTableID = 0;
    eldcHashElement     * sTableElement = NULL;
    eldcHashDataTableID * sTableHashData = NULL;

    eldcHashRelatedObjList * sObjectList = NULL;
    
    ellDictHandle          * sIndexHandle = NULL;
    
    stlBool    sStateLatch = STL_FALSE;
    stlBool    sTimeout = STL_FALSE;
    stlBool    sIsValid = STL_TRUE;

    stlInt32          sKeyCnt = 0;
    ellIndexKeyDesc * sKeyDesc = NULL;

    stlInt32          sInKeyCnt = 0;
    ellIndexKeyDesc * sInKeyDesc = NULL;

    stlInt32  i = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aTableHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * 기본 정보 획득
     */
    
    sInKeyCnt  = ellGetIndexKeyCount( aIndexHandle );
    sInKeyDesc = ellGetIndexKeyDesc( aIndexHandle );
    
    /**
     * ELDC_OBJECTCACHE_TABLE_ID Hash 로부터 검색
     */

    sTableID = ellGetTableID( aTableHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_TABLE_ID ),
                                  & sTableID,
                                  eldcHashFuncTableID,
                                  eldcCompareFuncTableID,
                                  & sTableElement,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * X Latch 획득
     */

    STL_TRY( knlAcquireLatch( & sTableElement->mWriteLatch,
                              KNL_LATCH_MODE_EXCLUSIVE,
                              KNL_LATCH_PRIORITY_NORMAL,
                              STL_INFINITE_TIME,   /* interval */
                              & sTimeout,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sTimeout == STL_FALSE );
    sStateLatch = STL_TRUE;
    
    /*************************************************************
     * SQL-Table Cache 정보에서 동일한 Key Column 을 갖는 인덱스를 검색 
     *************************************************************/
    
    sTableHashData = (eldcHashDataTableID *) sTableElement->mHashData;

    /**
     * Unique Index 에서 확인
     */

    for ( sObjectList = sTableHashData->mUniqueIndex;
          sObjectList != NULL;
          sObjectList = sObjectList->mNext )
    {
        /**
         * 다른 Trasaction 에 대해서만 검사
         */

        if ( (aTransID == sObjectList->mVersionInfo.mCreateTransID) ||
             (aTransID == sObjectList->mVersionInfo.mDropTransID) )
        {
            /* 동일한 Tx 임 */
            continue;
        }
        else
        {
            STL_TRY( eldcIsVisibleCache( sObjectList->mVersionInfo.mCreateTransID,
                                         SML_MAXIMUM_STABLE_SCN,
                                         &sObjectList->mVersionInfo,
                                         &sIsValid,
                                         aEnv )
                     == STL_SUCCESS );

            if( sIsValid == STL_FALSE )
            {
                continue;
            }
        }
        
        sIndexHandle = & sObjectList->mRelatedHandle;

        sKeyCnt  = ellGetIndexKeyCount( sIndexHandle );
        sKeyDesc = ellGetIndexKeyDesc( sIndexHandle );

        if ( sKeyCnt == sInKeyCnt )
        {
            sSameKey = STL_TRUE;
            for ( i = 0; i < sKeyCnt; i++ )
            {
                if ( ellGetColumnID( & sKeyDesc[i].mKeyColumnHandle ) ==
                     ellGetColumnID( & sInKeyDesc[i].mKeyColumnHandle ) )
                {
                    if ( sKeyDesc[i].mKeyOrdering == sInKeyDesc[i].mKeyOrdering )
                    {
                        if ( sKeyDesc[i].mKeyNullOrdering == sInKeyDesc[i].mKeyNullOrdering )
                        {
                            /* 동일한 Key 임 */
                        }
                        else
                        {
                            sSameKey = STL_FALSE;
                            break;
                        }
                    }
                    else
                    {
                        sSameKey = STL_FALSE;
                        break;
                    }
                }
                else
                {
                    sSameKey = STL_FALSE;
                    break;
                }
            }

            STL_TRY_THROW( sSameKey != STL_TRUE, RAMP_SAME_KEY );
        }
        else
        {
            /* Key 개수가 다름 */
        }
    }
    
    /**
     * Non-Unique Index 에서 확인
     */

    for ( sObjectList = sTableHashData->mNonUniqueIndex;
          sObjectList != NULL;
          sObjectList = sObjectList->mNext )
    {
        /**
         * 다른 Trasaction 에 대해서만 검사
         */

        if ( (aTransID == sObjectList->mVersionInfo.mCreateTransID) ||
             (aTransID == sObjectList->mVersionInfo.mDropTransID) )
        {
            /* 동일한 Tx 임 */
            continue;
        }
        else
        {
            STL_TRY( eldcIsVisibleCache( sObjectList->mVersionInfo.mCreateTransID,
                                         SML_MAXIMUM_STABLE_SCN,
                                         &sObjectList->mVersionInfo,
                                         &sIsValid,
                                         aEnv )
                     == STL_SUCCESS );

            if( sIsValid == STL_FALSE )
            {
                continue;
            }
        }
        
        sIndexHandle = & sObjectList->mRelatedHandle;

        sKeyCnt  = ellGetIndexKeyCount( sIndexHandle );
        sKeyDesc = ellGetIndexKeyDesc( sIndexHandle );

        if ( sKeyCnt == sInKeyCnt )
        {
            sSameKey = STL_TRUE;
            for ( i = 0; i < sKeyCnt; i++ )
            {
                if ( ellGetColumnID( & sKeyDesc[i].mKeyColumnHandle ) ==
                     ellGetColumnID( & sInKeyDesc[i].mKeyColumnHandle ) )
                {
                    if ( sKeyDesc[i].mKeyOrdering == sInKeyDesc[i].mKeyOrdering )
                    {
                        if ( sKeyDesc[i].mKeyNullOrdering == sInKeyDesc[i].mKeyNullOrdering )
                        {
                            /* 동일한 Key 임 */
                        }
                        else
                        {
                            sSameKey = STL_FALSE;
                            break;
                        }
                    }
                    else
                    {
                        sSameKey = STL_FALSE;
                        break;
                    }
                }
                else
                {
                    sSameKey = STL_FALSE;
                    break;
                }
            }

            STL_TRY_THROW( sSameKey != STL_TRUE, RAMP_SAME_KEY );
        }
        else
        {
            /* Key 개수가 다름 */
        }
    }
    
    /**
     * Same Key Index 가 존재한다면...
     */
    
    STL_RAMP( RAMP_SAME_KEY );
    
    /**
     * X Latch 해제 
     */

    sStateLatch = STL_FALSE;
    STL_TRY( knlReleaseLatch( & sTableElement->mWriteLatch,
                              KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Output 설정
     */

    *aExist = sSameKey;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if ( sStateLatch == STL_TRUE )
    {
        (void) knlReleaseLatch( & sTableElement->mWriteLatch,
                                KNL_ENV(aEnv) );

        sStateLatch = STL_FALSE;
    }
    
    return STL_FAILURE;
}



/*************************************************************
 * 멤버 함수 
 *************************************************************/




/**
 * @brief 읽은 Value List 로부터 SQL Index 의 Index Descriptor 를 생성한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[out] aIndexDesc       Index Descriptor
 * @param[in]  aValueList       Value List
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldcMakeIndexDesc( smlTransId          aTransID,
                             smlStatement      * aStmt,
                             knlValueBlockList * aValueList,
                             ellIndexDesc     ** aIndexDesc,
                             ellEnv            * aEnv )
{
    ellIndexDesc     * sIndexDesc = NULL;

    dtlDataValue     * sDataValue = NULL;

    stlInt32           sIndexDescSize = 0;
    
    stlInt32           sIndexNameLen = 0;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aIndexDesc != NULL,        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL      , ELL_ERROR_STACK(aEnv) );

    /**
     * Index Descriptor 를 위한 공간의 크기 계산
     */

    /* Index Name 의 길이 */
    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_INDEXES,
                                   ELDT_Indexes_ColumnOrder_INDEX_NAME );
    sIndexNameLen = sDataValue->mLength;

    sIndexDescSize = eldcCalSizeIndexDesc( sIndexNameLen );

    /**
     * Index Descriptor 를 위한 공간 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sIndexDesc,
                               sIndexDescSize,
                               aEnv )
             == STL_SUCCESS );
    
    /**
     * Index Descriptor 의 각 정보를 설정한다.
     */

    /*
     * mOwnerID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_INDEXES,
                                   ELDT_Indexes_ColumnOrder_OWNER_ID );
    
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sIndexDesc->mOwnerID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sIndexDesc->mOwnerID) == sDataValue->mLength );
    }

    /*
     * mSchemaID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_INDEXES,
                                   ELDT_Indexes_ColumnOrder_SCHEMA_ID );
    
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sIndexDesc->mSchemaID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sIndexDesc->mSchemaID) == sDataValue->mLength );
    }
    
    /*
     * mIndexID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_INDEXES,
                                   ELDT_Indexes_ColumnOrder_INDEX_ID );
    
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sIndexDesc->mIndexID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sIndexDesc->mIndexID) == sDataValue->mLength );
    }
    
    /*
     * mTablespaceID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_INDEXES,
                                   ELDT_Indexes_ColumnOrder_TABLESPACE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sIndexDesc->mTablespaceID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sIndexDesc->mTablespaceID) == sDataValue->mLength );
    }

    /*
     * mPhysicalID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_INDEXES,
                                   ELDT_Indexes_ColumnOrder_PHYSICAL_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sIndexDesc->mPhysicalID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sIndexDesc->mPhysicalID) == sDataValue->mLength );
    }
    
    /*
     * mIndexName
     * - 메모리 공간 내에서의 위치 계산
     */

    sIndexDesc->mIndexName = eldcGetIndexNamePtr( sIndexDesc );

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_INDEXES,
                                   ELDT_Indexes_ColumnOrder_INDEX_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sIndexDesc->mIndexName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sIndexDesc->mIndexName[sDataValue->mLength] = '\0';
    }

    /*
     * mIndexType
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_INDEXES,
                                   ELDT_Indexes_ColumnOrder_INDEX_TYPE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sIndexDesc->mIndexType,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sIndexDesc->mIndexType) == sDataValue->mLength );
    }

    /*
     * mIsUnique
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_INDEXES,
                                   ELDT_Indexes_ColumnOrder_IS_UNIQUE );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sIndexDesc->mIsUnique,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sIndexDesc->mIsUnique) == sDataValue->mLength );
    }

    /*
     * mInvalid
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_INDEXES,
                                   ELDT_Indexes_ColumnOrder_INVALID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sIndexDesc->mInvalid,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sIndexDesc->mInvalid) == sDataValue->mLength );
    }

    /*
     * mByConstraint
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_INDEXES,
                                   ELDT_Indexes_ColumnOrder_BY_CONSTRAINT );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sIndexDesc->mByConstraint,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sIndexDesc->mByConstraint) == sDataValue->mLength );
    }
    
    /*
     * mIndexHandle
     */

    if ( sIndexDesc->mPhysicalID == ELL_DICT_OBJECT_ID_NA )
    {
        STL_ASSERT(0);
    }
    else
    {
        STL_TRY( smlGetRelationHandle( sIndexDesc->mPhysicalID,
                                       & sIndexDesc->mIndexHandle,
                                       SML_ENV( aEnv ) )
                 == STL_SUCCESS );
        STL_ASSERT( sIndexDesc->mIndexHandle != NULL );
    }
    
    /*
     * mKeyColumnCount 와 mKeyDesc
     */

    STL_TRY( eldcSetKeyColumn( aTransID,
                               aStmt,
                               sIndexDesc,
                               aEnv )
             == STL_SUCCESS );

    /*
     * mDebugTableID
     */

    sIndexDesc->mDebugTableID = ellGetColumnTableID( & sIndexDesc->mKeyDesc[0].mKeyColumnHandle );
        
    /**
     * return 값 설정
     */
    
    *aIndexDesc = sIndexDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Index ID 를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aIndexDesc      Index Descriptor
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementIndexID( eldcHashElement   ** aHashElement,
                                      smlTransId           aTransID,
                                      ellIndexDesc       * aIndexDesc,
                                      ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32              sHashValue = 0;
    eldcHashDataIndexID  * sHashDataIndexID = NULL;
    eldcHashElement      * sHashElement      = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataIndexID();
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataIndexID,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataIndexID, 0x00, sHashDataSize );
    
    sHashDataIndexID->mKeyIndexID = aIndexDesc->mIndexID;

    /**
     * Hash Value 생성
     */
    
    sHashValue = eldcHashFuncIndexID( & aIndexDesc->mIndexID );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataIndexID,
                                  sHashValue,
                                  aIndexDesc,
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
 * @brief Index Name 을 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aIndexDesc      Index Descriptor
 * @param[in]  aHashElementID  Index ID로부터 구축된 Hash Element
 * @param[in]  aEnv            Environment 
 * @remarks
 */ 
stlStatus eldcMakeHashElementIndexName( eldcHashElement   ** aHashElement,
                                        smlTransId           aTransID,
                                        ellIndexDesc       * aIndexDesc,
                                        eldcHashElement    * aHashElementID,
                                        ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32                sHashValue = 0;
    eldcHashDataIndexName  * sHashDataIndexName = NULL;
    eldcHashElement        * sHashElement        = NULL;

    eldcHashKeyIndexName     sHashKey;
    stlMemset( & sHashKey, 0x00, STL_SIZEOF(eldcHashKeyIndexName) );
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataIndexName( stlStrlen(aIndexDesc->mIndexName) );
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */

    STL_TRY( eldAllocCacheMem( (void **) & sHashDataIndexName,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataIndexName, 0x00, sHashDataSize );
    
    sHashDataIndexName->mKeySchemaID = aIndexDesc->mSchemaID;
    
    /* Index Name 을 복사할 위치 지정 */
    sHashDataIndexName->mKeyIndexName = eldcGetHashDataIndexNamePtr( sHashDataIndexName );
    
    stlStrcpy( sHashDataIndexName->mKeyIndexName,
               aIndexDesc->mIndexName );

    sHashDataIndexName->mDataIndexID = aIndexDesc->mIndexID;
    sHashDataIndexName->mDataHashElement = aHashElementID;

    /**
     * Hash Value 생성
     */

    sHashKey.mSchemaID = sHashDataIndexName->mKeySchemaID;
    sHashKey.mIndexName = sHashDataIndexName->mKeyIndexName;
    sHashValue = eldcHashFuncIndexName( & sHashKey );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataIndexName,
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
 * @brief Index Descriptor 에 Key Column 정보를 채운다.
 * @param[in] aTransID         Transaction ID
 * @param[in] aStmt            Statement
 * @param[in] aIndexDesc       Index Descriptor
 * @param[in] aEnv             Environment
 * @remarks
 */

stlStatus eldcSetKeyColumn( smlTransId           aTransID,
                            smlStatement       * aStmt,
                            ellIndexDesc       * aIndexDesc,
                            ellEnv             * aEnv )
{
    smlRid              sRowRid;
    smlScn              sRowScn;
    smlRowBlock         sRowBlock;
    stlBool             sBeginSelect = STL_FALSE;

    stlBool             sBoolValue = STL_FALSE;
    
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
    
    dtlDataValue * sDataValue = NULL;
    stlInt32       sOrdinalPosition = 0;

    stlBool        sColumnExist = STL_FALSE;
    stlInt64       sColumnID = ELL_DICT_OBJECT_ID_NA;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aIndexDesc != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE];

    /**
     * DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,
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
     * WHERE INDEX_ID = aIndexID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,
                                            ELDT_IndexKey_ColumnOrder_INDEX_ID );
    
    if ( gEldCacheBuilding == STL_TRUE )
    {
        /**
         * Start-Up 수행 시간이 View 개수의 제곱에 비례하여 증가하게 된다.
         * Index Cache 가 존재하지 않아 General 한 Key Range 방식이 아닌
         * hard-coded key range 로 start-up 성능 문제를 해결한다.
         */

        STL_TRY( eldcMakeNonCacheRange( aTransID,
                                        aStmt,
                                        sFilterColumn,
                                        aIndexDesc->mIndexID,
                                        & sKeyCompList,
                                        & sIndexValueList,
                                        & sRangePred,
                                        & sRangeContext,
                                        aEnv )
                 == STL_SUCCESS );
        
        sIndexHandle = gEldNonCacheIndexPhyHandle4IndexKeyColIndexID;
    }
    else
    {
        /**
         * Key Range 생성
         */

        STL_TRY( eldMakeOneEquiRange( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,
                                      sFilterColumn,
                                      & aIndexDesc->mIndexID,
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
     * Index Key Column Dsec 를 Index descriptor 에 추가한다.
     */

    aIndexDesc->mKeyColumnCnt = 0;
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
            aIndexDesc->mKeyColumnCnt++;
            
            /*
             * Key Column 의 Ordinal Position 획득 
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,
                                           ELDT_IndexKey_ColumnOrder_ORDINAL_POSITION );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sOrdinalPosition,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sOrdinalPosition) == sDataValue->mLength );
                
                sOrdinalPosition = ELD_ORDINAL_POSITION_TO_PROCESS(sOrdinalPosition);
            }

            /*
             * mKeyDesc.mKeyColumnHandle
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,
                                           ELDT_IndexKey_ColumnOrder_COLUMN_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sColumnID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sColumnID) == sDataValue->mLength );
            }

            STL_TRY( ellGetColumnDictHandleByID(
                         aTransID,
                         SML_MAXIMUM_STABLE_SCN,
                         sColumnID,
                         & aIndexDesc->mKeyDesc[sOrdinalPosition].mKeyColumnHandle,
                         & sColumnExist,
                         aEnv )
                     == STL_SUCCESS );
            STL_ASSERT( sColumnExist == STL_TRUE );

            /*
             * mKeyDesc.mKeyOrdering
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,
                                           ELDT_IndexKey_ColumnOrder_IS_ASCENDING_ORDER );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sBoolValue,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sBoolValue) == sDataValue->mLength );
            }

            aIndexDesc->mKeyDesc[sOrdinalPosition].mKeyOrdering = sBoolValue;
            
            /*
             * mKeyDesc.mKeyNullOrdering
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_INDEX_KEY_COLUMN_USAGE,
                                           ELDT_IndexKey_ColumnOrder_IS_NULLS_FIRST );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sBoolValue,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sBoolValue) == sDataValue->mLength );
            }

            aIndexDesc->mKeyDesc[sOrdinalPosition].mKeyNullOrdering = sBoolValue;
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

/** @} eldcIndexDesc */
