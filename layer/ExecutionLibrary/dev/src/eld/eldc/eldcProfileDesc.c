/*******************************************************************************
 * eldcProfileDesc.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldcProfileDesc.c 15745 2015-08-20 07:37:17Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


/**
 * @file eldcProfileDesc.c
 * @brief Cache for Profile descriptor
 */

#include <ell.h>
#include <eldt.h>                               
#include <eldc.h>

#include <eldDictionary.h>



/**
 * @addtogroup eldcProfileDesc
 * @{
 */


/**
 * @brief Hash Key 로부터 Hash Value 생성을 위한 함수 포인터
 * @param[in] aHashKey     Hash Key(Profile ID)
 * @remarks
 */
stlUInt32  eldcHashFuncProfileID( void * aHashKey )
{
    stlInt64  sProfileID = 0;
    
    stlUInt32 sHashValue = 0;

    /**
     * Column ID 로부터 Hash Value 생성 
     */
    
    sProfileID = *(stlInt64 *) aHashKey;
    
    sHashValue = ellGetHashValueOneID( sProfileID );

    return sHashValue;
}


/**
 * @brief Hash Key 와 Hash Element의 비교를 위한 함수 포인터
 * @param[in] aHashKey        Hash Key(Profile ID)
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncProfileID( void * aHashKey, eldcHashElement * aHashElement )
{
    stlInt64               sProfileID = 0;
    
    eldcHashDataProfileID * sHashData    = NULL;

    /**
     * 입력된 Profile ID 와 Hash Element 의 Profile ID 를 비교 
     */
    
    sProfileID  = *(stlInt64 *) aHashKey;
    
    sHashData    = (eldcHashDataProfileID *) aHashElement->mHashData;

    if ( sHashData->mKeyProfileID == sProfileID )
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
 * @param[in] aHashKey     Hash Key (Profile Name)
 * @remarks
 */
stlUInt32  eldcHashFuncProfileName( void * aHashKey )
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
 * @param[in] aHashKey        Hash Key (Profile Name )
 * @param[in] aHashElement    Hash Element
 * @remarks
 * Hash Value 가 동일할 경우 Key 를 검사 
 */
stlBool    eldcCompareFuncProfileName( void * aHashKey, eldcHashElement * aHashElement )
{
    stlChar * sHashKey = NULL;
    
    eldcHashDataProfileName * sHashData = NULL;

    /**
     * 입력된 Hash Key 와 Hash Element 의 Key 를 비교 
     */
    
    sHashKey  = (stlChar *) aHashKey;
    
    sHashData    = (eldcHashDataProfileName *) aHashElement->mHashData;

    if ( stlStrcmp( sHashData->mKeyProfileName, sHashKey ) == 0 )
    {
        return STL_TRUE;
    }
    else
    {
        return STL_FALSE;
    }
}



/**
 * @brief Profile Descriptor 를 위한 공간을 계산한다.
 * @param[in] aProfileNameLen    Profile Name 의 길이
 * @remarks
 */
stlInt32  eldcCalSizeProfileDesc( stlInt32 aProfileNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(ellProfileDesc) );
    
    if ( aProfileNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aProfileNameLen + 1);
    }

    return sSize;
}


/**
 * @brief Profile Descriptor 로부터 Profile Name 의 Pointer 위치를 계산
 * @param[in] aProfileDesc   Profile Descriptor
 * @remarks
 */
stlChar * eldcGetProfileNamePtr( ellProfileDesc * aProfileDesc )
{
    return (stlChar *) aProfileDesc + STL_ALIGN_DEFAULT( STL_SIZEOF(ellProfileDesc) );
}






/**
 * @brief Profile ID Hash Data 를 위한 공간을 계산한다.
 * @remarks
 */
stlInt32  eldcCalSizeHashDataProfileID()
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF( eldcHashDataProfileID ) );

    return sSize;
}


/**
 * @brief Profile Name Hash Data 를 위한 공간을 계산한다.
 * @param[in]  aProfileNameLen   Profile Name 의 길이 
 * @remarks
 */
stlInt32  eldcCalSizeHashDataProfileName( stlInt32 aProfileNameLen )
{
    stlInt32   sSize = 0;

    sSize = STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataProfileName) );

    if ( aProfileNameLen > 0 )
    {
        sSize = sSize + STL_ALIGN_DEFAULT(aProfileNameLen + 1);
    }
    
    return sSize;
}


/**
 * @brief Profile Name Hash Data 로부터 Profile Name 의 Pointer 위치를 계산
 * @param[in] aHashDataProfileName   Profile Name Hash Data
 * @remarks
 */
stlChar * eldcGetHashDataProfileNamePtr( eldcHashDataProfileName * aHashDataProfileName )
{
    return (stlChar *) aHashDataProfileName
        + STL_ALIGN_DEFAULT( STL_SIZEOF(eldcHashDataProfileName) );
}








/**
 * @brief Profile Descriptor 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aProfileDesc       Profile Descriptor
 * @param[out] aStringBuffer   출력 정보를 위한 String Buffer 공간
 * @remarks
 */
void eldcPrintProfileDesc( void * aProfileDesc, stlChar * aStringBuffer )
{
    stlInt32      sSize = 0;
    ellProfileDesc * sProfileDesc = (ellProfileDesc *) aProfileDesc;

    stlChar  sLimitString[STL_MAX_SQL_IDENTIFIER_LENGTH] = {0,};
        
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Profile Descriptor( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sPROFILE_ID: %ld, ",
                 aStringBuffer,
                 sProfileDesc->mProfileID );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sPROFILE_NAME: %s, ",
                 aStringBuffer,
                 sProfileDesc->mProfileName );

    /**
     * Password Parameter 출력
     */

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sPARAMETER: ( ",
                 aStringBuffer );

    /**
     * FAILED_LOGIN_ATTEMPTS 출력
     */

    if ( sProfileDesc->mFailedLoginIsDefault == STL_TRUE )
    {
        stlStrncpy( sLimitString, ELL_PROFILE_LIMIT_STRING_DEFAULT, STL_MAX_SQL_IDENTIFIER_LENGTH );
    }
    else
    {
        if ( sProfileDesc->mFailedLoginAttempt == ELL_PROFILE_LIMIT_UNLIMITED )
        {
            stlStrncpy( sLimitString, ELL_PROFILE_LIMIT_STRING_UNLIMITED, STL_MAX_SQL_IDENTIFIER_LENGTH );
        }
        else
        {
            stlSnprintf( sLimitString, STL_MAX_SQL_IDENTIFIER_LENGTH, "%ld", sProfileDesc->mFailedLoginAttempt );
        }
    }
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sFAILED_LOGIN_ATTEMPTS:%s ",
                 aStringBuffer,
                 sLimitString );

    /**
     * PASSWORD_LOCK_TIME 출력
     */

    if ( sProfileDesc->mLockTimeIsDefault == STL_TRUE )
    {
        stlStrncpy( sLimitString, ELL_PROFILE_LIMIT_STRING_DEFAULT, STL_MAX_SQL_IDENTIFIER_LENGTH );
    }
    else
    {
        if ( DTL_IS_NULL( & sProfileDesc->mLockTimeNumber ) == STL_TRUE )
        {
            stlStrncpy( sLimitString, ELL_PROFILE_LIMIT_STRING_UNLIMITED, STL_MAX_SQL_IDENTIFIER_LENGTH );
        }
        else
        {
            stlStrncpy( sLimitString, "SPECIFIED", STL_MAX_SQL_IDENTIFIER_LENGTH );
        }
    }
        
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sPASSWORD_LOCK_TIME:%s ",
                 aStringBuffer,
                 sLimitString );

    /**
     * PASSWORD_LIFE_TIME 출력
     */

    if ( sProfileDesc->mLifeTimeIsDefault == STL_TRUE )
    {
        stlStrncpy( sLimitString, ELL_PROFILE_LIMIT_STRING_DEFAULT, STL_MAX_SQL_IDENTIFIER_LENGTH );
    }
    else
    {
        if ( DTL_IS_NULL( & sProfileDesc->mLifeTimeNumber ) == STL_TRUE )
        {
            stlStrncpy( sLimitString, ELL_PROFILE_LIMIT_STRING_UNLIMITED, STL_MAX_SQL_IDENTIFIER_LENGTH );
        }
        else
        {
            stlStrncpy( sLimitString, "SPECIFIED", STL_MAX_SQL_IDENTIFIER_LENGTH );
        }
    }
        
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sPASSWORD_LIFE_TIME:%s ",
                 aStringBuffer,
                 sLimitString );

    /**
     * PASSWORD_GRACE_TIME 출력
     */

    if ( sProfileDesc->mGraceTimeIsDefault == STL_TRUE )
    {
        stlStrncpy( sLimitString, ELL_PROFILE_LIMIT_STRING_DEFAULT, STL_MAX_SQL_IDENTIFIER_LENGTH );
    }
    else
    {
        if ( DTL_IS_NULL( & sProfileDesc->mGraceTimeNumber ) == STL_TRUE )
        {
            stlStrncpy( sLimitString, ELL_PROFILE_LIMIT_STRING_UNLIMITED, STL_MAX_SQL_IDENTIFIER_LENGTH );
        }
        else
        {
            stlStrncpy( sLimitString, "SPECIFIED", STL_MAX_SQL_IDENTIFIER_LENGTH );
        }
    }
        
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sPASSWORD_GRACE_TIME:%s ",
                 aStringBuffer,
                 sLimitString );
    
    /**
     * PASSWORD_REUSE_MAX 출력
     */

    if ( sProfileDesc->mReuseMaxIsDefault == STL_TRUE )
    {
        stlStrncpy( sLimitString, ELL_PROFILE_LIMIT_STRING_DEFAULT, STL_MAX_SQL_IDENTIFIER_LENGTH );
    }
    else
    {
        if ( sProfileDesc->mReuseMax == ELL_PROFILE_LIMIT_UNLIMITED )
        {
            stlStrncpy( sLimitString, ELL_PROFILE_LIMIT_STRING_UNLIMITED, STL_MAX_SQL_IDENTIFIER_LENGTH );
        }
        else
        {
            stlSnprintf( sLimitString, STL_MAX_SQL_IDENTIFIER_LENGTH, "%ld", sProfileDesc->mReuseMax );
        }
    }
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sPASSWORD_REUSE_MAX:%s ",
                 aStringBuffer,
                 sLimitString );

    /**
     * PASSWORD_REUSE_TIME 출력
     */

    if ( sProfileDesc->mReuseTimeIsDefault == STL_TRUE )
    {
        stlStrncpy( sLimitString, ELL_PROFILE_LIMIT_STRING_DEFAULT, STL_MAX_SQL_IDENTIFIER_LENGTH );
    }
    else
    {
        if ( DTL_IS_NULL( & sProfileDesc->mReuseTimeNumber ) == STL_TRUE )
        {
            stlStrncpy( sLimitString, ELL_PROFILE_LIMIT_STRING_UNLIMITED, STL_MAX_SQL_IDENTIFIER_LENGTH );
        }
        else
        {
            stlStrncpy( sLimitString, "SPECIFIED", STL_MAX_SQL_IDENTIFIER_LENGTH );
        }
    }
        
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sPASSWORD_REUSE_TIME:%s ",
                 aStringBuffer,
                 sLimitString );
    
    /**
     * PASSWORD_VERIFY_FUNCTION 출력
     */

    if ( sProfileDesc->mVerifyFunctionIsDefault == STL_TRUE )
    {
        stlStrncpy( sLimitString, ELL_PROFILE_LIMIT_STRING_DEFAULT, STL_MAX_SQL_IDENTIFIER_LENGTH );
    }
    else
    {
        if ( sProfileDesc->mVerifyFunctionID == ELL_DICT_OBJECT_ID_NA )
        {
            stlStrncpy( sLimitString, ELL_PROFILE_LIMIT_STRING_NULL, STL_MAX_SQL_IDENTIFIER_LENGTH );
        }
        else
        {
            stlSnprintf( sLimitString, STL_MAX_SQL_IDENTIFIER_LENGTH, "%ld", sProfileDesc->mVerifyFunctionID );
        }
    }
    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sPASSWORD_VERIFY_FUNCTION:%s ",
                 aStringBuffer,
                 sLimitString );

    
    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%s) ",
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
 * @brief Profile ID Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataProfileID    Profile ID 의 Hash Data
 * @param[out] aStringBuffer      출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataProfileID( void * aHashDataProfileID, stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataProfileID * sHashData = (eldcHashDataProfileID *) aHashDataProfileID;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Profile ID Hash Data( " );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sKEY_PROFILE_ID: %ld )",
                         aStringBuffer,
                         sHashData->mKeyProfileID );

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
 * @brief Profile Name Cache 의 Hash Data 의 String 출력을 생성하는 함수 포인터 
 * @param[in]  aHashDataProfileName  Profile Name 의 Hash Data
 * @param[out] aStringBuffer      출력 정보를 위한 String Buffer 공간 
 * @remarks
 */
void eldcPrintHashDataProfileName( void    * aHashDataProfileName,
                                stlChar * aStringBuffer )
{
    stlInt32   sSize = 0;
    eldcHashDataProfileName * sHashData = (eldcHashDataProfileName *) aHashDataProfileName;

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "Profile Name Hash Data( " );

    stlSnprintf( aStringBuffer,
                 ELDC_DUMP_PRINT_BUFFER_SIZE,
                 "%sKEY_PROFILE_NAME: %s, ",
                 aStringBuffer,
                 sHashData->mKeyProfileName );

    sSize = stlSnprintf( aStringBuffer,
                         ELDC_DUMP_PRINT_BUFFER_SIZE,
                         "%sDATA_PROFILE_ID: %ld )",
                         aStringBuffer,
                         sHashData->mDataProfileID );

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
 * @brief SQL-Profile Cache 를 위한 Dump Handle을 설정
 * @remarks
 */

void eldcSetSQLProfileDumpHandle()
{
    /*
     * ELDC_OBJECTCACHE_PROFILE_ID
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_PROFILE_ID].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PROFILE_ID );
    gEldcObjectDump[ELDC_OBJECTCACHE_PROFILE_ID].mPrintHashData
        = eldcPrintHashDataProfileID;
    gEldcObjectDump[ELDC_OBJECTCACHE_PROFILE_ID].mPrintObjectDesc
        = eldcPrintProfileDesc;
    
    /*
     * ELDC_OBJECTCACHE_PROFILE_NAME
     */

    gEldcObjectDump[ELDC_OBJECTCACHE_PROFILE_NAME].mDictHash
        = ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PROFILE_NAME );
    gEldcObjectDump[ELDC_OBJECTCACHE_PROFILE_NAME].mPrintHashData
        = eldcPrintHashDataProfileName;
    gEldcObjectDump[ELDC_OBJECTCACHE_PROFILE_NAME].mPrintObjectDesc
        = eldcPrintINVALID;
}




/**
 * @brief SQL-Profile 객체를 위한 Cache 를 구축한다.
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildProfileCache( smlTransId     aTransID,
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
                                      ELDT_TABLE_ID_PROFILES,
                                      & sBucketCnt,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * 소수에 해당하는 Hash Bucket 개수의 조정
     */

    sBucketCnt = ellGetPrimeBucketCnt( sBucketCnt );
    
    /**
     * Profile ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_PROFILE_ID ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Profile Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_PROFILE_NAME ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLProfileDumpHandle();
    
    /**
     * SQL-Profile Cache 구축 
     */

    STL_TRY( eldcBuildCacheSQLProfile( aTransID,
                                       aStmt,
                                       aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief SQL-Profile 을 위한 Cache 를 구축한다.
 * @param[in]  aTransID  Transaction ID
 * @param[in]  aStmt     Statement
 * @param[in]  aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildCacheSQLProfile( smlTransId       aTransID,
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

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.PROFILES 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_PROFILES];

    /**
     * DEFINITION_SCHEMA.PROFILES 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_PROFILES,
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
     * Profile Descriptor 를 Cache 에 추가한다.
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
        STL_TRY( eldcInsertCacheProfile( aTransID,
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
 * @brief Profile ID 를 이용해 Profile Dictionary Handle 을 획득 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aViewSCN         View SCN
 * @param[in]  aProfileID       Profile ID
 * @param[out] aProfileHandle   Profile Dictionary Handle
 * @param[out] aExist           존재 여부
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus eldcGetProfileHandleByID( smlTransId           aTransID,
                                    smlScn               aViewSCN,
                                    stlInt64             aProfileID,
                                    ellDictHandle      * aProfileHandle,
                                    stlBool            * aExist,
                                    ellEnv             * aEnv )
{
    eldcHashElement * sHashElement = NULL;

    stlBool sIsValid = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aProfileID > ELL_DICT_OBJECT_ID_NA,
                        ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_PROFILE_ID Hash 로부터 검색
     */

    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PROFILE_ID ),
                                  & aProfileID,
                                  eldcHashFuncProfileID,
                                  eldcCompareFuncProfileID,
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
             * Profile Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aProfileHandle,
                              ELL_OBJECT_PROFILE,
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
 * @brief Profile Name 를 이용해 Profile Dictionary Handle 을 획득 
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aViewSCN         View SCN
 * @param[in]  aProfileName     Profile Name
 * @param[out] aProfileHandle   Profile Dictionary Handle
 * @param[out] aExist           존재 여부
 * @param[in]  aEnv             Environment
 * @remarks
 */
stlStatus eldcGetProfileHandleByName( smlTransId      aTransID,
                                      smlScn          aViewSCN,
                                      stlChar       * aProfileName,
                                      ellDictHandle * aProfileHandle,
                                      stlBool       * aExist,
                                      ellEnv        * aEnv )
{
    eldcHashElement        * sHashElement = NULL;

    eldcHashDataProfileName * sHashData = NULL;

    stlBool sIsValid = STL_FALSE;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aProfileName != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExist != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_PROFILE_NAME Hash 로부터 검색
     */

    STL_TRY( eldcFindHashElement( aTransID,
                                  aViewSCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PROFILE_NAME ),
                                  aProfileName,
                                  eldcHashFuncProfileName,
                                  eldcCompareFuncProfileName,
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
        
        sHashData = (eldcHashDataProfileName *) sHashElement->mHashData;
        
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
             * Profile Handle 설정
             */

            ellSetDictHandle( aTransID,
                              aViewSCN,
                              aProfileHandle,
                              ELL_OBJECT_PROFILE,
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
 * @brief SQL Profile 을 위한 Cache 정보를 추가한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aStmt             Statement
 * @param[in] aValueList        Data Value List
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcInsertCacheProfile( smlTransId          aTransID,
                                  smlStatement      * aStmt,
                                  knlValueBlockList * aValueList,
                                  ellEnv            * aEnv )
{
    ellProfileDesc   * sProfileDesc = NULL;
    
    eldcHashElement * sHashElementProfileID = NULL;
    eldcHashElement * sHashElementProfileName = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Profile ID를 위한 Profile Descriptor 구성
     */
    
    STL_TRY( eldcMakeProfileDesc( aTransID,
                                  aStmt,
                                  aValueList,
                                  & sProfileDesc,
                                  aEnv )
             == STL_SUCCESS );
    
    /**
     * Profile ID 를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementProfileID( & sHashElementProfileID,
                                           aTransID,
                                           sProfileDesc,
                                           aEnv )
             == STL_SUCCESS );
                                                  
    /**
     * ELDC_OBJECTCACHE_PROFILE_ID Hash 에 추가 
     */
    
    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PROFILE_ID ),
                                    sHashElementProfileID,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Profile Name 을 위한 Hash Element 구성
     */

    STL_TRY( eldcMakeHashElementProfileName( & sHashElementProfileName,
                                             aTransID,
                                             sProfileDesc,
                                             sHashElementProfileID,
                                             aEnv )
             == STL_SUCCESS );
    
    /**
     * ELDC_OBJECTCACHE_PROFILE_NAME 에 추가 
     */

    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PROFILE_NAME ),
                                    sHashElementProfileName,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Profile ID를 기준으로 SQL-Column Cache 를 추가한다.
 * @param[in]  aTransID      Transaction ID
 * @param[in]  aStmt         Statement
 * @param[in]  aProfileID    Profile ID
 * @param[in]  aEnv          Environment
 * @remarks
 */
stlStatus eldcInsertCacheProfileByProfileID( smlTransId          aTransID,
                                             smlStatement      * aStmt,
                                             stlInt64            aProfileID,
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

    stlInt32 sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.PROFILES 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_PROFILES];

    /**
     * DEFINITION_SCHEMA.PROFILES 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_PROFILES,
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
     * WHERE PROFILE_ID = aProfileID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_PROFILES,
                                            ELDT_Profile_ColumnOrder_PROFILE_ID );

    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_PROFILES,
                                  sFilterColumn,
                                  & aProfileID,
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
     * SQL-Profile Cache 정보를 추가한다.
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
            STL_TRY( eldcInsertCacheProfile( aTransID,
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
 * @brief SQL Profile 를 위한 Cache 정보를 삭제한다.
 * @param[in] aTransID          Transaction ID
 * @param[in] aProfileHandle    Profile Dictionary Handle
 * @param[in] aEnv              Environment
 * @remarks
 */
stlStatus eldcDeleteCacheProfile( smlTransId          aTransID,
                                  ellDictHandle     * aProfileHandle,
                                  ellEnv            * aEnv )
{
    eldcHashElement * sHashElementID = NULL;
    eldcHashElement * sHashElementName = NULL;
    

    stlInt64   sProfileID = ELL_DICT_OBJECT_ID_NA;
    stlChar  * sProfileName = NULL;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aProfileHandle != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * ELDC_OBJECTCACHE_PROFILE_ID Hash 에서 삭제 
     */

    sProfileID = ellGetProfileID( aProfileHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PROFILE_ID ),
                                  & sProfileID,
                                  eldcHashFuncProfileID,
                                  eldcCompareFuncProfileID,
                                  & sHashElementID,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PROFILE_ID ),
                                    sHashElementID,
                                    aEnv )
             == STL_SUCCESS );
                                    
    /**
     * ELDC_OBJECTCACHE_PROFILE_NAME Hash 로부터 검색
     */

    sProfileName = ellGetProfileName( aProfileHandle );
    
    STL_TRY( eldcFindHashElement( aTransID,
                                  SML_MAXIMUM_STABLE_SCN,
                                  ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PROFILE_NAME ),
                                  sProfileName,
                                  eldcHashFuncProfileName,
                                  eldcCompareFuncProfileName,
                                  & sHashElementName,
                                  aEnv )
             == STL_SUCCESS );

    STL_TRY( eldcDeleteHashElement( aTransID,
                                    ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_PROFILE_NAME ),
                                    sHashElementName,
                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief 읽은 Value List 로부터 SQL Profile 의 Profile Descriptor 를 생성한다.
 * @param[in]  aTransID         Transaction ID
 * @param[in]  aStmt            Statement
 * @param[out] aProfileDesc     Profile Descriptor
 * @param[in]  aValueList       Value List
 * @param[in]  aEnv             Environment 포인터
 * @remarks
 */
stlStatus eldcMakeProfileDesc( smlTransId           aTransID,
                               smlStatement       * aStmt,
                               knlValueBlockList  * aValueList,
                               ellProfileDesc    ** aProfileDesc,
                               ellEnv             * aEnv )
{
    stlInt32      sProfileDescSize = 0;
    ellProfileDesc * sProfileDesc = NULL;

    dtlDataValue     * sDataValue = NULL;

    stlInt64  sProfileID = ELL_DICT_OBJECT_ID_NA;
    stlInt32  sProfileNameLen = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValueList != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /*
     * mProfileID
     */

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_PROFILES,
                                   ELDT_Profile_ColumnOrder_PROFILE_ID );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( & sProfileID,
                   sDataValue->mValue,
                   sDataValue->mLength );
        STL_DASSERT( STL_SIZEOF(sProfileID) == sDataValue->mLength );
    }

    /*
     * Profile Name Length
     */
    
    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_PROFILES,
                                   ELDT_Profile_ColumnOrder_PROFILE_NAME );
    sProfileNameLen = sDataValue->mLength;

    /**
     * Profile Descriptor 를 위한 공간의 크기 계산
     */

    
    sProfileDescSize = eldcCalSizeProfileDesc( sProfileNameLen );
    
    /**
     * Profile Descriptor 를 위한 공간 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sProfileDesc,
                               sProfileDescSize,
                               aEnv )
             == STL_SUCCESS );

    /**
     * Profile Descriptor 의 각 정보를 설정한다.
     */

    /*
     * mProfileID
     */

    sProfileDesc->mProfileID = sProfileID;

    /*
     * mProfileName
     * - 메모리 공간 내에서의 위치 계산
     */

    sProfileDesc->mProfileName = eldcGetProfileNamePtr( sProfileDesc );

    sDataValue = eldFindDataValue( aValueList,
                                   ELDT_TABLE_ID_PROFILES,
                                   ELDT_Profile_ColumnOrder_PROFILE_NAME );
    if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
    {
        STL_ASSERT(0);
    }
    else
    {
        stlMemcpy( sProfileDesc->mProfileName,
                   sDataValue->mValue,
                   sDataValue->mLength );
        sProfileDesc->mProfileName[sDataValue->mLength] = '\0';
    }

    /*
     * Password Parameter Information
     */

    STL_TRY( eldcSetProfilePassParam( aTransID,
                                      aStmt,
                                      sProfileDesc,
                                      aEnv )
             == STL_SUCCESS );

    /**
     * return 값 설정
     */
    
    *aProfileDesc = sProfileDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Profile ID 를 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aProfileDesc       Profile Descriptor
 * @param[in]  aEnv            Environment
 * @remarks
 */ 
stlStatus eldcMakeHashElementProfileID( eldcHashElement   ** aHashElement,
                                        smlTransId           aTransID,
                                        ellProfileDesc     * aProfileDesc,
                                        ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32                  sHashValue = 0;
    eldcHashDataProfileID    * sHashDataProfileID = NULL;
    eldcHashElement          * sHashElement      = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataProfileID();
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataProfileID,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataProfileID, 0x00, sHashDataSize );
    
    sHashDataProfileID->mKeyProfileID = aProfileDesc->mProfileID;

    /**
     * Hash Value 생성
     */
    
    sHashValue = eldcHashFuncProfileID( (void *) & aProfileDesc->mProfileID );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataProfileID,
                                  sHashValue,
                                  aProfileDesc,
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
 * @brief Profile Name 을 위한 Hash Element 를 구성한다.
 * @param[out] aHashElement    Hash Element
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aProfileDesc    Profile Descriptor
 * @param[in]  aHashElementID  Profile ID로부터 구축된 Hash Element
 * @param[in]  aEnv            Environment 
 * @remarks
 */ 
stlStatus eldcMakeHashElementProfileName( eldcHashElement   ** aHashElement,
                                          smlTransId           aTransID,
                                          ellProfileDesc     * aProfileDesc,
                                          eldcHashElement    * aHashElementID,
                                          ellEnv             * aEnv )
{
    stlInt32 sHashDataSize = 0;

    stlUInt32                    sHashValue = 0;
    eldcHashDataProfileName       * sHashDataProfileName = NULL;
    eldcHashElement            * sHashElement      = NULL;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aHashElement != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileDesc != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Hash Data 생성
     */

    sHashDataSize = eldcCalSizeHashDataProfileName( stlStrlen(aProfileDesc->mProfileName) );
    
    /**
     * Shared Cache 메모리 관리자로부터 할당
     */
    
    STL_TRY( eldAllocCacheMem( (void **) & sHashDataProfileName,
                               sHashDataSize,
                               aEnv )
             == STL_SUCCESS );

    stlMemset( sHashDataProfileName, 0x00, sHashDataSize );
    
    /* Profile Name 을 복사할 위치 지정 */
    sHashDataProfileName->mKeyProfileName
        = eldcGetHashDataProfileNamePtr( sHashDataProfileName );
    
    stlStrcpy( sHashDataProfileName->mKeyProfileName,
               aProfileDesc->mProfileName );

    sHashDataProfileName->mDataProfileID = aProfileDesc->mProfileID;
    sHashDataProfileName->mDataHashElement = aHashElementID;

    /**
     * Hash Value 생성
     */

    sHashValue = eldcHashFuncProfileName( (void *) sHashDataProfileName->mKeyProfileName );
    
    /**
     * Hash Element 생성
     */

    STL_TRY( eldcMakeHashElement( & sHashElement,
                                  aTransID,
                                  sHashDataProfileName,
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
 * @brief Profile Password Parameter 정보를 설정한다.
 * @param[in]  aTransID        Transaction ID
 * @param[in]  aStmt           Statement
 * @param[in]  aProfileDesc    Profile Descriptor
 * @param[in]  aEnv            Environment 
 * @remarks
 */ 
stlStatus eldcSetProfilePassParam( smlTransId       aTransID,
                                   smlStatement   * aStmt,
                                   ellProfileDesc * aProfileDesc,
                                   ellEnv         * aEnv )
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
    
    smlRid       sRowRid;
    smlScn       sRowScn;
    smlRowBlock  sRowBlock;

    smlFetchInfo        sFetchInfo;
    smlFetchRecordInfo  sFetchRecordInfo;
    eldMemMark          sMemMark;

    dtlDataValue     * sDataValue = NULL;
    stlBool            sIsDefault = STL_FALSE;
    ellProfileParam    sPassParam = ELL_PROFILE_PARAM_NA;
    dtlDataValue     * sParamValue = NULL;
    stlBool            sTruncated = STL_FALSE;

    stlInt32  sState = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aProfileDesc != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * DEFINITION_SCHEMA.PROFILE_PASSWORD_PARAMETER 테이블의 table handle 을 획득
     */
    
    sTableHandle = gEldTablePhysicalHandle[ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER];

    /**
     * DEFINITION_SCHEMA.PROFILE_PASSWORD_PARAMETER 테이블의 Value List와 Row Block 할당
     */

    STL_TRY( eldAllocTableValueList( ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
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
     * WHERE PROFILE_ID = aProfileID
     */

    sFilterColumn = ellFindColumnValueList( sTableValueList,
                                            ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                            ELDT_PassParam_ColumnOrder_PROFILE_ID );

    /**
     * Key Range 생성
     */
    
    STL_TRY( eldMakeOneEquiRange( aTransID,
                                  aStmt,
                                  ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                  sFilterColumn,
                                  & aProfileDesc->mProfileID,
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
     * Profile Password Parameter 정보를 설정한다.
     */

    /**
     * 초기화
     */
    
    aProfileDesc->mFailedLoginIsDefault = STL_FALSE;
    aProfileDesc->mFailedLoginAttempt   = ELL_PROFILE_LIMIT_UNLIMITED;
    
    aProfileDesc->mLockTimeIsDefault          = STL_FALSE;
    aProfileDesc->mLockTimeNumber.mType       = DTL_TYPE_NUMBER;
    aProfileDesc->mLockTimeNumber.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    aProfileDesc->mLockTimeNumber.mValue      = aProfileDesc->mLockTimeBuffer;
    DTL_SET_NULL( & aProfileDesc->mLockTimeNumber );

    aProfileDesc->mLifeTimeIsDefault          = STL_FALSE;
    aProfileDesc->mLifeTimeNumber.mType       = DTL_TYPE_NUMBER;
    aProfileDesc->mLifeTimeNumber.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    aProfileDesc->mLifeTimeNumber.mValue      = aProfileDesc->mLifeTimeBuffer;
    DTL_SET_NULL( & aProfileDesc->mLifeTimeNumber );

    aProfileDesc->mGraceTimeIsDefault          = STL_FALSE;
    aProfileDesc->mGraceTimeNumber.mType       = DTL_TYPE_NUMBER;
    aProfileDesc->mGraceTimeNumber.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    aProfileDesc->mGraceTimeNumber.mValue      = aProfileDesc->mGraceTimeBuffer;
    DTL_SET_NULL( & aProfileDesc->mGraceTimeNumber );

    aProfileDesc->mReuseMaxIsDefault = STL_FALSE;
    aProfileDesc->mReuseMax          = ELL_PROFILE_LIMIT_UNLIMITED;
    
    aProfileDesc->mReuseTimeIsDefault          = STL_FALSE;
    aProfileDesc->mReuseTimeNumber.mType       = DTL_TYPE_NUMBER;
    aProfileDesc->mReuseTimeNumber.mBufferSize = DTL_NUMERIC_MAX_SIZE;
    aProfileDesc->mReuseTimeNumber.mValue      = aProfileDesc->mReuseTimeBuffer;
    DTL_SET_NULL( & aProfileDesc->mReuseTimeNumber );

    aProfileDesc->mVerifyFunctionIsDefault = STL_FALSE;
    aProfileDesc->mVerifyFunctionID        = ELL_DICT_OBJECT_ID_NA;
    
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
         * IS_DEFAULT
         */
        
        sDataValue = eldFindDataValue( sTableValueList,
                                       ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                       ELDT_PassParam_ColumnOrder_IS_DEFAULT );
        if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
        {
            STL_DASSERT(0);
        }
        else
        {
            stlMemcpy( & sIsDefault,
                       sDataValue->mValue,
                       sDataValue->mLength );
            STL_DASSERT( STL_SIZEOF(stlBool) == sDataValue->mLength );
        }

        /**
         * PARAMETER_ID
         */
        
        sDataValue = eldFindDataValue( sTableValueList,
                                       ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                       ELDT_PassParam_ColumnOrder_PARAMETER_ID );
        if ( DTL_IS_NULL( sDataValue ) == STL_TRUE )
        {
            STL_DASSERT(0);
        }
        else
        {
            stlMemcpy( & sPassParam,
                       sDataValue->mValue,
                       sDataValue->mLength );
            STL_DASSERT( STL_SIZEOF(stlInt32) == sDataValue->mLength );
        }

        /**
         * PARAMETER_VALUE
         */

        sParamValue = eldFindDataValue( sTableValueList,
                                        ELDT_TABLE_ID_PROFILE_PASSWORD_PARAMETER,
                                        ELDT_PassParam_ColumnOrder_PARAMETER_VALUE );

        /**
         * Password Parameter 에 따른 정보 설정
         */
        switch( sPassParam )
        {
            case ELL_PROFILE_PARAM_FAILED_LOGIN_ATTEMPTS:
                {
                    if ( sIsDefault == STL_TRUE )
                    {
                        aProfileDesc->mFailedLoginIsDefault = STL_TRUE;
                    }
                    else
                    {
                        aProfileDesc->mFailedLoginIsDefault = STL_FALSE;
                        
                        if ( DTL_IS_NULL( sParamValue ) == STL_TRUE )
                        {
                            aProfileDesc->mFailedLoginAttempt = ELL_PROFILE_LIMIT_UNLIMITED;
                        }
                        else
                        {
                            STL_TRY( dtlNumericToInt64( sParamValue,
                                                        & aProfileDesc->mFailedLoginAttempt,
                                                        & sTruncated,
                                                        ELL_ERROR_STACK(aEnv) )
                                     == STL_SUCCESS );
                            STL_DASSERT( sTruncated == STL_FALSE );
                        }
                    }
                    
                    break;
                }
            case ELL_PROFILE_PARAM_PASSWORD_LOCK_TIME:
                {
                    if ( sIsDefault == STL_TRUE )
                    {
                        aProfileDesc->mLockTimeIsDefault = STL_TRUE;
                    }
                    else
                    {
                        aProfileDesc->mLockTimeIsDefault = STL_FALSE;
                        
                        if ( DTL_IS_NULL( sParamValue ) == STL_TRUE )
                        {
                            DTL_SET_NULL( & aProfileDesc->mLockTimeNumber );
                        }
                        else
                        {
                            DTL_COPY_DATA_VALUE( DTL_NUMERIC_MAX_SIZE,
                                                 sParamValue,
                                                 & aProfileDesc->mLockTimeNumber );
                        }
                    }
                    
                    break;
                }
            case ELL_PROFILE_PARAM_PASSWORD_LIFE_TIME:
                {
                    if ( sIsDefault == STL_TRUE )
                    {
                        aProfileDesc->mLifeTimeIsDefault = STL_TRUE;
                    }
                    else
                    {
                        aProfileDesc->mLifeTimeIsDefault = STL_FALSE;
                        
                        if ( DTL_IS_NULL( sParamValue ) == STL_TRUE )
                        {
                            DTL_SET_NULL( & aProfileDesc->mLifeTimeNumber );
                        }
                        else
                        {
                            DTL_COPY_DATA_VALUE( DTL_NUMERIC_MAX_SIZE,
                                                 sParamValue,
                                                 & aProfileDesc->mLifeTimeNumber );
                        }
                    }
                    
                    break;
                }
            case ELL_PROFILE_PARAM_PASSWORD_GRACE_TIME:
                {
                    if ( sIsDefault == STL_TRUE )
                    {
                        aProfileDesc->mGraceTimeIsDefault = STL_TRUE;
                    }
                    else
                    {
                        aProfileDesc->mGraceTimeIsDefault = STL_FALSE;
                        
                        if ( DTL_IS_NULL( sParamValue ) == STL_TRUE )
                        {
                            DTL_SET_NULL( & aProfileDesc->mGraceTimeNumber );
                        }
                        else
                        {
                            DTL_COPY_DATA_VALUE( DTL_NUMERIC_MAX_SIZE,
                                                 sParamValue,
                                                 & aProfileDesc->mGraceTimeNumber );
                        }
                    }
                    
                    break;
                }
            case ELL_PROFILE_PARAM_PASSWORD_REUSE_MAX:
                {
                    if ( sIsDefault == STL_TRUE )
                    {
                        aProfileDesc->mReuseMaxIsDefault = STL_TRUE;
                    }
                    else
                    {
                        aProfileDesc->mReuseMaxIsDefault = STL_FALSE;
                        
                        if ( DTL_IS_NULL( sParamValue ) == STL_TRUE )
                        {
                            aProfileDesc->mReuseMax = ELL_PROFILE_LIMIT_UNLIMITED;
                        }
                        else
                        {
                            STL_TRY( dtlNumericToInt64( sParamValue,
                                                        & aProfileDesc->mReuseMax,
                                                        & sTruncated,
                                                        ELL_ERROR_STACK(aEnv) )
                                     == STL_SUCCESS );
                            STL_DASSERT( sTruncated == STL_FALSE );
                        }
                    }
                    break;
                }
            case ELL_PROFILE_PARAM_PASSWORD_REUSE_TIME:
                {
                    if ( sIsDefault == STL_TRUE )
                    {
                        aProfileDesc->mReuseTimeIsDefault = STL_TRUE;
                    }
                    else
                    {
                        aProfileDesc->mReuseTimeIsDefault = STL_FALSE;
                        
                        if ( DTL_IS_NULL( sParamValue ) == STL_TRUE )
                        {
                            DTL_SET_NULL( & aProfileDesc->mReuseTimeNumber );
                        }
                        else
                        {
                            DTL_COPY_DATA_VALUE( DTL_NUMERIC_MAX_SIZE,
                                                 sParamValue,
                                                 & aProfileDesc->mReuseTimeNumber );
                        }
                    }
                    
                    break;
                }
            case ELL_PROFILE_PARAM_PASSWORD_VERIFY_FUNCTION:
                {
                    if ( sIsDefault == STL_TRUE )
                    {
                        aProfileDesc->mVerifyFunctionIsDefault = STL_TRUE;
                    }
                    else
                    {
                        aProfileDesc->mVerifyFunctionIsDefault = STL_FALSE;
                        
                        if ( DTL_IS_NULL( sParamValue ) == STL_TRUE )
                        {
                            aProfileDesc->mVerifyFunctionID = ELL_DICT_OBJECT_ID_NA;
                        }
                        else
                        {
                            STL_TRY( dtlNumericToInt64( sParamValue,
                                                        & aProfileDesc->mVerifyFunctionID,
                                                        & sTruncated,
                                                        ELL_ERROR_STACK(aEnv) )
                                     == STL_SUCCESS );
                            STL_DASSERT( sTruncated == STL_FALSE );
                        }
                    }
                    
                    break;
                }
            default:
                {
                    STL_DASSERT(0);
                    break;
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



/** @} eldcProfileDesc */
