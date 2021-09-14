/*******************************************************************************
 * eldcAuthorizationDesc.c
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
 * @file eldcAuthorizationDesc.c
 * @brief Cache for Authorization descriptor
 */

#include <ell.h>
#include <eldt.h>                               
#include <eldc.h>

#include <eldDictionary.h>



/**
 * @addtogroup eldcAuthorizationDesc
 * @{
 */


/**
 * @brief Hash Key 로부터 Hash Value 생성을 위한 함수 포인터
 * @param[in] aHashKey     Hash Key(Auth ID)
 * @remarks
 */
stlUInt32  eldcHashFuncAuthID( void * aHashKey )
{
    stlInt64  sAuthID = 0;
    
    stlUInt32 sHashValue = 0;

    /**
     * Column ID 로부터 Hash Value 생성 
     */
    
    sAuthID = *(stlInt64 *) aHashKey;
    
    sHashValue = ellGetHashValueOneID( sAuthID );

    return sHashValue;
}


/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key(Auth ID)
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncAuthID( void * aHashKey, eldcHashElement * aHashElement )
{
    stlInt64               sAuthID = 0;
    
    eldcHashDataAuthID * sHashData    = NULL;

    /**
     * 입력된 Auth ID 와 Hash Element 의 Auth ID 를 비교 
     */
    
    sAuthID  = *(stlInt64 *) aHashKey;
    
    sHashData    = (eldcHashDataAuthID *) aHashElement->mHashData;

    if ( sHashData->mKeyAuthID == sAuthID )
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
 * @param[in] aHashKey     Hash Key (Auth Name)
 * @remarks
 */
stlUInt32  eldcHashFuncAuthName( void * aHashKey )
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
 * @param[in] aHashKey        Hash Key (Auth Name )
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncAuthName( void * aHashKey, eldcHashElement * aHashElement )
{
    stlChar * sHashKey = NULL;
    
    eldcHashDataAuthName * sHashData = NULL;

    /**
     * 입력된 Hash Key 와 Hash Element 의 Key 를 비교 
     */
    
    sHashKey  = (stlChar *) aHashKey;
    
    sHashData    = (eldcHashDataAuthName *) aHashElement->mHashData;

    if ( stlStrcmp( sHashData->mKeyAuthName, sHashKey ) == 0 )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}



/**
 * @brief Auth Descriptor 를 위한 공간을 계산한다.
 * @param[in] aAuthNameLen    Auth Name 의 길이
 * @param[in] aSchemaCnt      Schema Count in Schema Path
 * @param[in] aGrantedRoleCnt Granted Role Count
 * @remarks
 */
stlInt32  eldcCalSizeAuthDesc( stlInt32 aAuthNameLen,
                               stlInt32 aSchemaCnt,
                               stlInt32 aGrantedRoleCnt )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(ellAuthDesc) );
    
    if ( aAuthNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aAuthNameLen + 1);
    }

    if ( aSchemaCnt > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT( STL_SIZEOF(stlInt64) * aSchemaCnt );
    }

    if ( aGrantedRoleCnt > 0 )
    {
        /* for Granted Role ID Array */
        sSize = sSize + STL_ALIGN_DEFAULT( STL_SIZEOF(stlInt64) * aGrantedRoleCnt );
        /* for Is Grantable Array */
        sSize = sSize + STL_ALIGN_DEFAULT( STL_SIZEOF(stlBool) * aGrantedRoleCnt );
    }
    
    return sSize;
}


/**
 * @brief Auth Descriptor 로부터 Auth Name 의 Pointer 위치를 계산
 * @param[in] aAuthDesc   Auth Descriptor
 * @remarks
 */
stlChar * eldcGetAuthNamePtr( ellAuthDesc * aAuthDesc )
{
    return (stlChar *) aAuthDesc + STL_ALIGN_DEFAULT( STL_SIZEOF(ellAuthDesc) );
}

/**
 * @brief Auth Descriptor 로부터 Schema Path 의 Pointer 위치를 계산
 * @param[in] aAuthDesc       Auth Descriptor
 * @param[in] aAuthNameLen    Authorization Name Length
 * @remarks
 */
stlInt64 * eldcGetSchemaPathPtr( ellAuthDesc * aAuthDesc,
                                stlInt32      aAuthNameLen )
{
    return (stlInt64*) ( (stlChar *) aAuthDesc
                         + STL_ALIGN_DEFAULT( STL_SIZEOF(ellAuthDesc) )
                         + STL_ALIGN_DEFAULT(aAuthNameLen + 1) );
}

/**
 * @brief Auth Descriptor 로부터 Granted Role 의 Pointer 위치를 계산
 * @param[in] aAuthDesc       Auth Descriptor
 * @param[in] aAuthNameLen    Authorization Name Length
 * @param[in] aSchemaCnt      Schema Count in Schema Path
 * @remarks
 */
stlInt64 * eldcGetGrantedRolePtr( ellAuthDesc * aAuthDesc,
                                  stlInt32      aAuthNameLen,
                                  stlInt32      aSchemaCnt )
{
    return (stlInt64 *) ( (stlChar *) aAuthDesc
                          + STL_ALIGN_DEFAULT( STL_SIZEOF(ellAuthDesc) )
                          + STL_ALIGN_DEFAULT(aAuthNameLen + 1)
                          + STL_ALIGN_DEFAULT( STL_SIZEOF(stlInt64) * aSchemaCnt ) );
}

/**
 * @brief Auth Descriptor 로부터 Is Grantable 의 Pointer 위치를 계산
 * @param[in] aAuthDesc       Auth Descriptor
 * @param[in] aAuthNameLen    Authorization Name Length
 * @param[in] aSchemaCnt      Schema Count in Schema Path
 * @param[in] aGrantedRoleCnt Granted Role Count
 * @remarks
 */
stlBool * eldcGetIsGrantablePtr( ellAuthDesc * aAuthDesc,
                                  stlInt32      aAuthNameLen,
                                  stlInt32      aSchemaCnt,
                                  stlInt32      aGrantedRoleCnt )
{
    return (stlBool *) ( (stlChar *) aAuthDesc
                         + STL_ALIGN_DEFAULT( STL_SIZEOF(ellAuthDesc) )
                         + STL_ALIGN_DEFAULT(aAuthNameLen + 1)
                         + STL_ALIGN_DEFAULT( STL_SIZEOF(stlInt64) * aSchemaCnt )
                         + STL_ALIGN_DEFAULT( STL_SIZEOF(stlInt64) * aGrantedRoleCnt ) );
}





/**
 * @brief Auth ID Hash Data 를 위한 공간을 계산한다.
 * @remarks
 */
stlInt32  eldcCalSizeHashDataAuthID()
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF( eldcHashDataAuthID ) );

    return sSize;
}


/**
 * @brief Auth Name Hash Data 를 위한 공간을 계산한다.
 * @param[in]  aAuthNameLen   Authorization Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeHashDataAuthName( stlInt32 aAuthNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataAuthName) );

    if ( aAuthNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aAuthNameLen + 1);
    }
    
    return sSize;
}


/**
 * @brief Auth Name Hash Data 로부터 Auth Name 의 Pointer 위치를 계산
 * @param[in] aHashDataAuthName   Auth Name Hash Data
 * @remarks
 */
stlChar * eldcGetHashDataAuthNamePtr( eldcHashDataAuthName * aHashDataAuthName )
{
    return (stlChar *) aHashDataAuthName
        + STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataAuthName) );
}








/**
 * @brief Auth Descriptor 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aAuthDesc       Auth Descriptor
 * @param[out] aStringBuffer   출력 정보를 위한 String Buffer 공간
 * @remarks
 */
void eldcPrintAuthDesc( void * aAuthDesc, stlChar * aStringBuffer )
{
    stlInt32      i = 0;
    
    stlInt32      sSize = 0;
    ellAuthDesc * sAuthDesc = (ellAuthDesc *) aAuthDesc;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Auth Descriptor( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sAUTH_ID: %ld, ",
                 aStringBuffer,
                 sAuthDesc->mAuthID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sAUTH_NAME: %s, ",
                 aStringBuffer,
                 sAuthDesc->mAuthName );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sAUTH_TYPE: %d, ",
                 aStringBuffer,
                 sAuthDesc->mAuthType );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sDATA_TBS_ID: %ld, ",
                 aStringBuffer,
                 sAuthDesc->mDataSpaceID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sTEMP_TBS_ID: %ld, ",
                 aStringBuffer,
                 sAuthDesc->mTempSpaceID );

    /**
     * Schema Path 출력
     */

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sSCHEMA_PATH_CNT: %d[ ",
                 aStringBuffer,
                 sAuthDesc->mSchemaPathCnt );

    for ( i = 0; i < sAuthDesc->mSchemaPathCnt; i++ )
    {
        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s%ld ",
                     aStringBuffer,
                     sAuthDesc->mSchemaIDArray[i] );
    }

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%s], ",
                 aStringBuffer );
    
    /**
     * Granted Role 출력
     */

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sGRANTED_ROLE_CNT: %d[ ",
                 aStringBuffer,
                 sAuthDesc->mGrantedRoleCnt );

    for ( i = 0; i < sAuthDesc->mGrantedRoleCnt; i++ )
    {
        stlSnprintf( aStringBuffer,
                     ELDC_DUMP_PRINT_BUFFER_SIZE,
                     "%s%ld-%d ",
                     aStringBuffer,
                     sAuthDesc->mGrantedRoleIDArray[i],
                     sAuthDesc->mIsGrantableArray[i] );
    }

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%s] )",
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
 * @brief Auth ID Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataAuthID    Auth ID 의 Hash Data
 * @param[out] aStringBuffer      출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataAuthID( void * aHashDataAuthID, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataAuthID * sHashData = (eldcHashDataAuthID *) aHashDataAuthID;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Auth ID Hash Data( " );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sKEY_AUTH_ID: %ld )",
                         aStringBuffer,
                         sHashData->mKeyAuthID );

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
 * @brief Auth Name Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataAuthName  Auth Name 의 Hash Data
 * @param[out] aStringBuffer      출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataAuthName( void    * aHashDataAuthName,
                                stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataAuthName * sHashData = (eldcHashDataAuthName *) aHashDataAuthName;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Auth Name Hash Data( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_AUTH_NAME: %s, ",
                 aStringBuffer,
                 sHashData->mKeyAuthName );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sDATA_AUTH_ID: %ld )",
                         aStringBuffer,
                         sHashData->mDataAuthID );

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
 * @brief SQL-Authorization Cache 를 위한 Dump Handle을 설정
 * @remarks
 */

void eldcSetSQLAuthDumpHandle()
{
    /*
     * ELDC_OBJECTCACHE_AUTH_ID
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_AUTH_ID].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_AUTH_ID );
    gEldcObjectDump[ELDC_OBJECTCACHE_AUTH_ID].mPrintHashData
        = eldcPrintHashDataAuthID;
    gEldcObjectDump[ELDC_OBJECTCACHE_AUTH_ID].mPrintObjectDesc
        = eldcPrintAuthDesc;
    
    /*
     * ELDC_OBJECTCACHE_AUTH_NAME
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_AUTH_NAME].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_AUTH_NAME );
    gEldcObjectDump[ELDC_OBJECTCACHE_AUTH_NAME].mPrintHashData
        = eldcPrintHashDataAuthName;
    gEldcObjectDump[ELDC_OBJECTCACHE_AUTH_NAME].mPrintObjectDesc
        = eldcPrintINVALID;
}




/**
 * @brief SQL-Authorization 객체를 위한 Cache 를 구축한다.
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildAuthCache( smlTransId     aTransID,
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
                                      ELDT_TABLE_ID_AUTHORIZATIONS,
                                      & sBucketCnt,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * 소수에 해당하는 Hash Bucket 개수의 조정
     */

    sBucketCnt = ellGetPrimeBucketCnt( sBucketCnt );
    
    /**
     * Auth ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_AUTH_ID ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Auth Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_AUTH_NAME ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLAuthDumpHandle();
    
    /**
     * SQL-Auth Cache 구축 
     */

    STL_TRY( eldcBuildCacheSQLAuth( aTransID,
                                    aStmt,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SQL-Auth 을 위한 Cache 를 구축한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildCacheSQLAuth( smlTransId       aTransID,
                                 smlStatement   * aStmt,
                                 ellEnv         * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;
    
    void              * sTableHandle = NULL;
    knlValueBlockList * sTableValueList = NULL;

    void                * sIterator  = NULL;
    smlIteratorProperty   sIteratorProperty;

    smlRid         sRowRid;
    smlScn         sRowScn;
    smlRowBlock    sRowBlock;
    eldMemMark     sMemMark;

    smlFetchInfo          sFetchInfo;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.AUTHORIZATIONS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_AUTHORIZATIONS];

    /**
     * DEFINITION_SCHEMA.AUTHORIZATIONS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_AUTHORIZATIONS,
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
     * Auth Descriptor 를 Cache 에 추가한다.
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
        STL_TRY( eldcInsertCacheAuth( aTransID,
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
 * @brief Auth ID 를 이용해 Auth Dictionary Handle 을 획득 
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aViewSCN      View SCN
 * @param[in]  aAuthID       Auth ID
 * @param[out] aAuthHandle   Authorization Dictionary Handle
 * @param[out] aExist        존재 여부
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcGetAuthHandleByID( smlTransId           aTransID,
                                 smlScn               aViewSCN,
                                 stlInt64             aAuthID,
                                 ellDictHandle      * aAuthHandle,
                                 stlBool            * aExist,
                                 ellEnv             * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlBool sIsValid = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aAuthID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_AUTH_ID Hash 로부터 검색
     */

    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_AUTH_ID ),
                                  & aAuthID,
                                  eldcHashFuncAuthID,
                                  eldcCompareFuncAuthID,
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
             * Auth Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aAuthHandle,
                              ELL_OBJECT_AUTHORIZATION,
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
 * @brief Auth Name 를 이용해 Auth Dictionary Handle 을 획득 
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aViewSCN      View SCN
 * @param[in]  aAuthName     Auth Name
 * @param[out] aAuthHandle   Authorization Dictionary Handle
 * @param[out] aExist        존재 여부
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcGetAuthHandleByName( smlTransId      aTransID,
                                   smlScn          aViewSCN,
                                   stlChar       * aAuthName,
                                   ellDictHandle * aAuthHandle,
                                   stlBool       * aExist,
                                   ellEnv        * aEnv )
{
    eldcHashElement        * sHashElement = NULL;

    eldcHashDataAuthName * sHashData = NULL;

    stlBool sIsValid = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aAuthName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_AUTH_NAME Hash 로부터 검색
     */

    /* add assert for 1st callstck of ticket #264 */
    STL_ASSERT( gEllSharedEntry != NULL );
    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_AUTH_NAME ),
                                  aAuthName,
                                  eldcHashFuncAuthName,
                                  eldcCompareFuncAuthName,
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
        
        sHashData = (eldcHashDataAuthName *) sHashElement->mHashData;
        /* add assert for 1st callstck of ticket #264 */
        if ( sHashData == NULL )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                "[Ticket264] Hash Element: %x, "
                                "InfoCC [ CreateTransID[%ld], CreateCommitScn[%ld], "
                                "DropTransID[%ld], DropCommitSCN[%ld], "
                                "Dropping:%d, ModifyingCnt:%ld ], "
                                "Hash Value: %d, Hash Next: %x\n",
                                sHashElement,
                                sHashElement->mVersionInfo.mCreateTransID,
                                sHashElement->mVersionInfo.mCreateCommitSCN,
                                sHashElement->mVersionInfo.mDropTransID,
                                sHashElement->mVersionInfo.mDropCommitSCN,
                                sHashElement->mVersionInfo.mDropFlag,
                                0,
                                sHashElement->mHashValue,
                                sHashElement->mNext )
             == STL_SUCCESS );
            
            STL_ASSERT( 0 );
        }

        /**
         * Version 유효성 검증
         */

        STL_TRY( eldcIsVisibleCache( aTransID,
                                     aViewSCN,
                                     & sHashData->mDataHashElement->mVersionInfo,
                                     & sIsValid,
                                     aEnv )
                 == STL_SUCCESS );
        
        if ( sIsValid == STL_TRUE )
        {
            /**
             * Auth Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aAuthHandle,
                              ELL_OBJECT_AUTHORIZATION,
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
 * @brief SQL Auth 을 위한 Cache 정보를 추가한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aValueList        Data Value List
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcInsertCacheAuth( smlTransId          aTransID,
                               smlStatement      * aStmt,
                               knlValueBlockList * aValueList,
                               ellEnv            * aEnv )
{
    ellSessObjectMgr  * sObjMgr = ELL_SESS_OBJ(aEnv);
    
    ellAuthDesc   * sAuthDesc = NULL;
    
    eldcHashElement * sHashElementAuthID = NULL;
    eldcHashElement * sHashElementAuthName = NULL;
    eldcLocalCache  * sNewLocalCache = NULL;

    /*
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
     * Auth ID를 위한 Auth Descriptor 구성
     */
    
    STL_TRY( eldcMakeAuthDesc( aTransID,
                               aStmt,
                               aValueList,
                               & sAuthDesc,
                               aEnv )
             == STL_SUCCESS );
    
    /**
     * Auth ID 를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementAuthID( & sHashElementAuthID,
                                        aTransID,
                                        sAuthDesc,
                                        aEnv )
             == STL_SUCCESS );
                                                  
    /**
     * ELDC_OBJECTCACHE_AUTH_ID Hash 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_AUTH_ID ),
                                    sHashElementAuthID,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Auth Name 을 위한 Hash Element 구성
     */

    STL_TRY( eldcMakeHashElementAuthName( & sHashElementAuthName,
                                          aTransID,
                                          sAuthDesc,
                                          sHashElementAuthID,
                                          aEnv )
             == STL_SUCCESS );
    
    /**
     * ELDC_OBJECTCACHE_AUTH_NAME 에 추가 
     */

    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_AUTH_NAME ),
                                    sHashElementAuthName,
                                    aEnv )
             == STL_SUCCESS );
    
    /**
     * 객체가 변경되고 있음을 세션에 기록한다.
     */
    
    ELL_SESS_ENV(aEnv)->mLocalModifySeq += 1;
    
    eldcInsertLocalCache( sHashElementAuthID, sNewLocalCache, aEnv );

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
 * @brief Auth ID를 기준으로 SQL-Column Cache 를 추가한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aAuthID       Auth ID
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcInsertCacheAuthByAuthID( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aAuthID,
                                       ellEnv            * aEnv )
{
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
    
    smlRid       sRowRid;
    smlScn       sRowScn;
    smlRowBlock  sRowBlock;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;
    
    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.AUTHORIZATIONS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_AUTHORIZATIONS];

    /**
     * DEFINITION_SCHEMA.AUTHORIZATIONS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_AUTHORIZATIONS,
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
     * WHERE AUTH_ID = aAuthID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_AUTHORIZATIONS,
                                            ELDT_Auth_ColumnOrder_AUTH_ID );

    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_AUTHORIZATIONS,
                                  sFilterColumn,
                                  & aAuthID,
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
            STL_TRY( eldcInsertCacheAuth( aTransID,
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
 * @brief SQL Authorization 를 위한 Cache 정보를 삭제한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aAuthHandle       Authorization Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcDeleteCacheAuth( smlTransId          aTransID,
                               ellDictHandle     * aAuthHandle,
                               ellEnv            * aEnv )
{
    ellSessObjectMgr  * sObjMgr = ELL_SESS_OBJ(aEnv);
    
    eldcHashElement * sHashElementID = NULL;
    eldcHashElement * sHashElementName = NULL;
    eldcLocalCache  * sLocalCache;
    eldcLocalCache  * sNewLocalCache = NULL;
    

    stlInt64   sAuthID = ELL_DICT_OBJECT_ID_NA;
    stlChar  * sAuthName = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aAuthHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * New Local Cache 공간 할당
     */
    
    STL_TRY( knlAllocDynamicMem( & sObjMgr->mMemSessObj,
                                 STL_SIZEOF(eldcLocalCache),
                                 (void **) & sNewLocalCache,
                                 KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * ELDC_OBJECTCACHE_AUTH_ID Hash 에서 삭제 
     */

    sAuthID = ellGetAuthID( aAuthHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_AUTH_ID ),
                                  & sAuthID,
                                  eldcHashFuncAuthID,
                                  eldcCompareFuncAuthID,
                                  & sHashElementID,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_AUTH_ID ),
                                    sHashElementID,
                                    aEnv )
             == STL_SUCCESS );
                                    
    /**
     * ELDC_OBJECTCACHE_AUTH_NAME Hash 로부터 검색
     */

    sAuthName = ellGetAuthName( aAuthHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_AUTH_NAME ),
                                  sAuthName,
                                  eldcHashFuncAuthName,
                                  eldcCompareFuncAuthName,
                                  & sHashElementName,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_AUTH_NAME ),
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
 * @brief 읽은 Value List 로부터 SQL Auth 의 Auth Descriptor 를 생성한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[out] aAuthDesc        Auth Descriptor
 * @param[in]  aValueList       Value List
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldcMakeAuthDesc( smlTransId           aTransID,
                            smlStatement       * aStmt,
                            knlValueBlockList  * aValueList,
                            ellAuthDesc       ** aAuthDesc,
                            ellEnv             * aEnv )
{
    stlInt32      sAuthDescSize = 0;
    stlInt64      sAuthID = ELL_DICT_OBJECT_ID_NA;
    ellAuthDesc * sAuthDesc = NULL;

    dtlDataValue     * sDataValue = NULL;

    stlInt32           sAuthNameLen = 0;
    stlInt32           sUserSchemaCnt = 0;
    stlInt32           sGrantedRoleCnt = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /*
     * mAuthID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_AUTHORIZATIONS,
                                   ELDT_Auth_ColumnOrder_AUTH_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sAuthID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sAuthID) == sDataValue->mLength );
    }

    
    /**
     * Auth Descriptor 를 위한 공간의 크기 계산
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_AUTHORIZATIONS,
                                   ELDT_Auth_ColumnOrder_AUTHORIZATION_NAME );
    sAuthNameLen = sDataValue->mLength;

    STL_TRY( eldcGetSchemaCnt( aTransID,
                               aStmt,
                               sAuthID,
                               & sUserSchemaCnt,
                               aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldcGetGrantedRoleCnt( aTransID,
                                    aStmt,
                                    sAuthID,
                                    & sGrantedRoleCnt,
                                    aEnv )
             == STL_SUCCESS );
    
    sAuthDescSize = eldcCalSizeAuthDesc( sAuthNameLen, sUserSchemaCnt, sGrantedRoleCnt );
    
    /**
     * Auth Descriptor 를 위한 공간 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sAuthDesc,
                               sAuthDescSize,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Auth Descriptor 의 각 정보를 설정한다.
     */

    /*
     * mAuthID
     */

    sAuthDesc->mAuthID = sAuthID;

    /*
     * mAuthName
     * - 메모리 공간 내에서의 위치 계산
     */

    sAuthDesc->mAuthName = eldcGetAuthNamePtr( sAuthDesc );

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_AUTHORIZATIONS,
                                   ELDT_Auth_ColumnOrder_AUTHORIZATION_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sAuthDesc->mAuthName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sAuthDesc->mAuthName[sDataValue->mLength] = '\0';
    }

    /*
     * mAuthType
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_AUTHORIZATIONS,
                                   ELDT_Auth_ColumnOrder_AUTHORIZATION_TYPE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sAuthDesc->mAuthType,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sAuthDesc->mAuthType) == sDataValue->mLength );
    }

    /*
     * mDataSpaceID 와 mTempSpaceID, mProfileID
     */

    STL_TRY( eldcSetUserSpaceProfileInfo( aTransID,
                                          aStmt,
                                          sAuthDesc,
                                          aEnv )
             == STL_SUCCESS );

    /*
     * mSchemaPathCnt 와 mSchemaIDArray
     */

    sAuthDesc->mSchemaPathCnt = sUserSchemaCnt;

    if ( sUserSchemaCnt > 0 )
    {
        sAuthDesc->mSchemaIDArray = eldcGetSchemaPathPtr( sAuthDesc,
                                                          sAuthNameLen );
        
        STL_TRY( eldcSetSchemaPath( aTransID,
                                    aStmt,
                                    sAuthDesc,
                                    aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sAuthDesc->mSchemaIDArray = NULL;
    }
    
    /*
     * mGrantedRoleCnt, mGrantedRoleIDArray, mIsGrantableArray
     */

    sAuthDesc->mGrantedRoleCnt = sGrantedRoleCnt;

    if ( sGrantedRoleCnt > 0 )
    {
        sAuthDesc->mGrantedRoleIDArray = eldcGetGrantedRolePtr( sAuthDesc,
                                                                sAuthNameLen,
                                                                sUserSchemaCnt );
        sAuthDesc->mIsGrantableArray = eldcGetIsGrantablePtr( sAuthDesc,
                                                              sAuthNameLen,
                                                              sUserSchemaCnt,
                                                              sGrantedRoleCnt );

        STL_TRY( eldcSetGrantedRole( aTransID,
                                     aStmt,
                                     sAuthDesc,
                                     aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sAuthDesc->mGrantedRoleIDArray = NULL;
        sAuthDesc->mIsGrantableArray = NULL;
    }
    
    /**
     * return 값 설정
     */
    
    *aAuthDesc = sAuthDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Auth ID 를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aAuthDesc       Auth Descriptor
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementAuthID( eldcHashElement   ** aHashElement,
                                     smlTransId           aTransID,
                                     ellAuthDesc        * aAuthDesc,
                                     ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32                  sHashValue = 0;
    eldcHashDataAuthID       * sHashDataAuthID = NULL;
    eldcHashElement          * sHashElement      = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataAuthID();
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataAuthID,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataAuthID, 0x00, sHashDataSize );
    
    sHashDataAuthID->mKeyAuthID = aAuthDesc->mAuthID;

    /**
     * Hash Value 생성
     */
    
    sHashValue = eldcHashFuncAuthID( (void *) & aAuthDesc->mAuthID );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataAuthID,
                                  sHashValue,
                                  aAuthDesc,
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
 * @brief Auth Name 을 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aAuthDesc       Auth Descriptor
 * @param[in]  aHashElementID  Auth ID로부터 구축된 Hash Element
 * @param[in]  aEnv            Environment 
 * @remarks
 */ 
stlStatus eldcMakeHashElementAuthName( eldcHashElement   ** aHashElement,
                                       smlTransId           aTransID,
                                       ellAuthDesc        * aAuthDesc,
                                       eldcHashElement    * aHashElementID,
                                       ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32                    sHashValue = 0;
    eldcHashDataAuthName       * sHashDataAuthName = NULL;
    eldcHashElement            * sHashElement      = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataAuthName( stlStrlen(aAuthDesc->mAuthName) );
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataAuthName,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataAuthName, 0x00, sHashDataSize );
    
    /* Auth Name 을 복사할 위치 지정 */
    sHashDataAuthName->mKeyAuthName
        = eldcGetHashDataAuthNamePtr( sHashDataAuthName );
    
    stlStrcpy( sHashDataAuthName->mKeyAuthName,
               aAuthDesc->mAuthName );

    sHashDataAuthName->mDataAuthID = aAuthDesc->mAuthID;
    sHashDataAuthName->mDataHashElement = aHashElementID;

    /**
     * Hash Value 생성
     */

    sHashValue = eldcHashFuncAuthName( (void *) sHashDataAuthName->mKeyAuthName );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataAuthName,
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
 * @brief Auth ID 의 User Schema Path Count 를 얻는다.
 * @param[in]  aTransID     Transaction ID
 * @param[in]  aStmt        Statement
 * @param[in]  aAuthID      Auth ID
 * @param[out] aSchemaCnt   Schema Count
 * @param[in]  aEnv         Environment
 * @remarks
 */
stlStatus eldcGetSchemaCnt( smlTransId     aTransID,
                            smlStatement * aStmt,
                            stlInt64       aAuthID,
                            stlInt32     * aSchemaCnt,
                            ellEnv       * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;
    
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
    
    stlInt32            sSchemaCnt = 0;

    smlRid       sRowRid;
    smlScn       sRowScn;
    smlRowBlock  sRowBlock;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSchemaCnt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.USER_SCHEMA_PATH 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_USER_SCHEMA_PATH];

    /**
     * DEFINITION_SCHEMA.USER_SCHEMA_PATH 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_USER_SCHEMA_PATH,
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
     * WHERE AUTH_ID = aAuthID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_USER_SCHEMA_PATH,
                                            ELDT_UserPath_ColumnOrder_AUTH_ID );

    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_USER_SCHEMA_PATH,
                                  sFilterColumn,
                                  & aAuthID,
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
     * User Schema Count 를 계산한다.
     */

    sSchemaCnt = 0;
    
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
            sSchemaCnt++;
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

    *aSchemaCnt = sSchemaCnt;
    
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
 * @brief Auth Descriptor 에 User Schema Path 정보를 설정한다.
 * @param[in]  aTransID    Transaction ID
 * @param[in]  aStmt       Statement
 * @param[in]  aAuthDesc   Auth Descriptor
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus eldcSetSchemaPath( smlTransId     aTransID,
                             smlStatement * aStmt,
                             ellAuthDesc  * aAuthDesc,
                             ellEnv       * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;
    
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
    
    dtlDataValue      * sDataValue = NULL;
    stlInt32            sSearchOrder = 0;

    smlRid       sRowRid;
    smlScn       sRowScn;
    smlRowBlock  sRowBlock;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;
    
    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aAuthDesc != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.USER_SCHEMA_PATH 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_USER_SCHEMA_PATH];

    /**
     * DEFINITION_SCHEMA.USER_SCHEMA_PATH 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_USER_SCHEMA_PATH,
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
     * WHERE AUTH_ID = aAuthID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_USER_SCHEMA_PATH,
                                            ELDT_UserPath_ColumnOrder_AUTH_ID );
    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_USER_SCHEMA_PATH,
                                  sFilterColumn,
                                  & aAuthDesc->mAuthID,
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
     * User Schema Path를 설정한다.
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
             * Search Order 계산
             */

            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_USER_SCHEMA_PATH,
                                           ELDT_UserPath_ColumnOrder_SEARCH_ORDER );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sSearchOrder,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sSearchOrder) == sDataValue->mLength );
                
                sSearchOrder = ELD_ORDINAL_POSITION_TO_PROCESS(sSearchOrder);

                STL_ASSERT( sSearchOrder < aAuthDesc->mSchemaPathCnt );
            }

            /**
             * Schema ID 설정
             */

            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_USER_SCHEMA_PATH,
                                           ELDT_UserPath_ColumnOrder_SCHEMA_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & aAuthDesc->mSchemaIDArray[sSearchOrder],
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(stlInt64) == sDataValue->mLength );
            }
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
 * @brief Auth ID 의 Granted Role Count 를 얻는다.
 * @param[in]  aTransID          Transaction ID
 * @param[in]  aStmt             Statement
 * @param[in]  aAuthID           Auth ID
 * @param[out] aGrantedRoleCnt   Schema Count
 * @param[in]  aEnv              Environment
 * @remarks
 */
stlStatus eldcGetGrantedRoleCnt( smlTransId     aTransID,
                                 smlStatement * aStmt,
                                 stlInt64       aAuthID,
                                 stlInt32     * aGrantedRoleCnt,
                                 ellEnv       * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;
    
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
    
    dtlDataValue      * sDataValue = NULL;
    stlInt64            sGrantorID = ELL_DICT_OBJECT_ID_NA;
    
    stlInt32            sGrantedCnt = 0;

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
    STL_PARAM_VALIDATE( aAuthID > ELL_DICT_OBJECT_ID_NA, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aGrantedRoleCnt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.ROLE_AUTHORIZATION_DESCRIPTORS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS];

    /**
     * DEFINITION_SCHEMA.ROLE_AUTHORIZATION_DESCRIPTORS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
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
     * WHERE GRANTEE_ID = aAuthID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
                                            ELDT_RoleDesc_ColumnOrder_GRANTEE_ID );

    /**
     * Key Range 생성
     */

    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
                                  sFilterColumn,
                                  & aAuthID,
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
     * Granted Role Count 를 계산한다.
     */

    sGrantedCnt = 0;
    
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
             * GRANTOR 정보 획득
             */
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
                                           ELDT_RoleDesc_ColumnOrder_GRANTOR_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sGrantorID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sGrantorID) == sDataValue->mLength );
            }

            if ( sGrantorID == ELDT_AUTH_ID_SYSTEM )
            {
                /* CREATE ROLE 구문으로 추가된 정보임 */
                continue;
            }
            else
            {
                sGrantedCnt++;
            }
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

    *aGrantedRoleCnt = sGrantedCnt;
    
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
 * @brief Auth Descriptor 에 Granted Role 정보를 설정한다.
 * @param[in]  aTransID    Transaction ID
 * @param[in]  aStmt       Statement
 * @param[in]  aAuthDesc   Auth Descriptor
 * @param[in]  aEnv        Environment
 * @remarks
 */
stlStatus eldcSetGrantedRole( smlTransId     aTransID,
                              smlStatement * aStmt,
                              ellAuthDesc  * aAuthDesc,
                              ellEnv       * aEnv )
{
    stlBool             sBeginSelect = STL_FALSE;
    
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
    
    dtlDataValue      * sDataValue = NULL;
    
    stlInt64            sGrantorID = ELL_DICT_OBJECT_ID_NA;
    stlInt64            sGrantedCnt = 0;

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
    STL_PARAM_VALIDATE( aAuthDesc != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.ROLE_AUTHORIZATION_DESCRIPTORS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS];

    /**
     * DEFINITION_SCHEMA.ROLE_AUTHORIZATION_DESCRIPTORS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
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
     * WHERE GRANTEE_ID = aAuthID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
                                            ELDT_RoleDesc_ColumnOrder_GRANTEE_ID );

    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
                                  sFilterColumn,
                                  & aAuthDesc->mAuthID,
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
     * Granted Role 정보를 설정한다.
     */

    sGrantedCnt = 0;
    
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
             * GRANTOR 정보 획득
             */
            
            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
                                           ELDT_RoleDesc_ColumnOrder_GRANTOR_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & sGrantorID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(sGrantorID) == sDataValue->mLength );
            }

            if ( sGrantorID == ELDT_AUTH_ID_SYSTEM )
            {
                /* CREATE ROLE 구문으로 추가된 정보임 */
                continue;
            }
            else
            {
                STL_ASSERT( sGrantedCnt < aAuthDesc->mGrantedRoleCnt );
                
                /**
                 * Granted Role ID 설정
                 */

                sDataValue = eldFindDataValue( sTableValueList,
                                               ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
                                               ELDT_RoleDesc_ColumnOrder_ROLE_ID );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & aAuthDesc->mGrantedRoleIDArray[sGrantedCnt],
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(stlInt64) == sDataValue->mLength );
                }

                /**
                 * Is Grantable 설정
                 */

                sDataValue = eldFindDataValue( sTableValueList,
                                               ELDT_TABLE_ID_ROLE_AUTHORIZATION_DESCRIPTORS,
                                               ELDT_RoleDesc_ColumnOrder_IS_GRANTABLE );
                if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
                {
                    STL_ASSERT(0);
                }
                else
                {
                    stlMemcpy( & aAuthDesc->mIsGrantableArray[sGrantedCnt],
                               sDataValue->mValue,
                               sDataValue->mLength );
                    STL_DASSERT( STL_SIZEOF(stlBool) == sDataValue->mLength );
                }
                
                sGrantedCnt++;
            }
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
 * @brief Authorization Descriptor 에 User Tablespace 와 Profile 정보를 설정한다.
 * @param[in]  aTransID   Transaction ID
 * @param[in]  aStmt      Statement
 * @param[in]  aAuthDesc  Auth Descriptor
 * @param[in]  aEnv       Environment
 * @remarks
 */
stlStatus eldcSetUserSpaceProfileInfo( smlTransId     aTransID,
                                       smlStatement * aStmt,
                                       ellAuthDesc  * aAuthDesc,
                                       ellEnv       * aEnv )
{
    stlBool             sFetchOne = STL_FALSE;
    
    stlBool             sBeginSelect = STL_FALSE;

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
    
    dtlDataValue      * sDataValue = NULL;

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
    
    /**
     * DEFINITION_SCHEMA.USERS 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_USERS];

    /**
     * DEFINITION_SCHEMA.USERS 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_USERS,
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
     * WHERE AUTH_ID = aAuthID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_USERS,
                                            ELDT_User_ColumnOrder_AUTH_ID );

    /**
     * Key Range 생성
     */

    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_USERS,
                                  sFilterColumn,
                                  & aAuthDesc->mAuthID,
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
     * User 의 Tablespace 정보를 설정한다.
     */

    aAuthDesc->mDataSpaceID = ELL_DICT_TABLESPACE_ID_NA;
    aAuthDesc->mTempSpaceID = ELL_DICT_TABLESPACE_ID_NA;
    aAuthDesc->mProfileID   = ELL_DICT_OBJECT_ID_NA;
    
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
            /**
             * Data TBS 설정
             */

            sDataValue =
                eldFindDataValue( sTableValueList,
                                  ELDT_TABLE_ID_USERS,
                                  ELDT_User_ColumnOrder_DEFAULT_DATA_TABLESPACE_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & aAuthDesc->mDataSpaceID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(aAuthDesc->mDataSpaceID) == sDataValue->mLength );
            }

            /**
             * Temp TBS 설정
             */

            sDataValue =
                eldFindDataValue( sTableValueList,
                                  ELDT_TABLE_ID_USERS,
                                  ELDT_User_ColumnOrder_DEFAULT_TEMP_TABLESPACE_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                STL_ASSERT(0);
            }
            else
            {
                stlMemcpy( & aAuthDesc->mTempSpaceID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(aAuthDesc->mTempSpaceID) == sDataValue->mLength );
            }

            /**
             * Profile ID 설정
             */

            sDataValue = eldFindDataValue( sTableValueList,
                                           ELDT_TABLE_ID_USERS,
                                           ELDT_User_ColumnOrder_PROFILE_ID );
            if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
            {
                aAuthDesc->mProfileID = ELL_DICT_OBJECT_ID_NA;
            }
            else
            {
                stlMemcpy( & aAuthDesc->mProfileID,
                           sDataValue->mValue,
                           sDataValue->mLength );
                STL_DASSERT( STL_SIZEOF(aAuthDesc->mProfileID) == sDataValue->mLength );
            }
            

            /* 한 건만 존재해야 한다. */
            STL_ASSERT( sFetchOne == STL_FALSE );
            sFetchOne = STL_TRUE;
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

/** @} eldcAuthorizationDesc */
