/*******************************************************************************
 * eldcNonServiceCache.c
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
 * @file eldcNonServiceCache.c
 * @brief NO_MOUNT, MOUNT 단계를 위한 Dictionary Cache
 */

#include <ell.h>
#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>
#include <elgPendOp.h>
#include <eldcDictHash.h>
#include <eldcNonServiceCache.h>

/**
 * @addtogroup eldcNonService
 * @{
 */

stlChar * gEldcNonServiceDBName = "DB_NOT_OPEN";
    
/*****************************************************
 * NO MOUNT 단계 함수 
 *****************************************************/

/**
 * @brief NO-MOUNT 단계의 SQL-Object Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcNoMountBuildObjectCache( ellEnv * aEnv )
{
    /**
     * SQL-Column Cache 를 구축
     */

    STL_TRY( eldcNoMountBuildColumnCache( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Table Cache 를 구축
     */

    STL_TRY( eldcNoMountBuildTableCache( aEnv ) == STL_SUCCESS );

    /**
     * SQL-Index Cache 를 구축
     */

    STL_TRY( eldcNoMountBuildIndexCache( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Constraint Cache 를 구축
     */

    STL_TRY( eldcNoMountBuildConstCache( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Sequence Cache 를 구축
     */

    STL_TRY( eldcNoMountBuildSequenceCache( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Schema Cache 를 구축
     */

    STL_TRY( eldcNoMountBuildSchemaCache( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Tablespace Cache 를 구축
     */

    STL_TRY( eldcNoMountBuildTablespaceCache( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Authorization Cache 를 구축
     */

    STL_TRY( eldcNoMountBuildAuthCache( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Catalog Cache 를 구축
     */

    STL_TRY( eldcNoMountBuildCatalogCache( aEnv ) == STL_SUCCESS );

    /**
     * SQL-Synonym Cache 를 구축
     */

    STL_TRY( eldcNoMountBuildSynonymCache( aEnv ) == STL_SUCCESS );

    /**
     * SQL-PublicSynonym Cache 를 구축
     */

    STL_TRY( eldcNoMountBuildPublicSynonymCache( aEnv ) == STL_SUCCESS );

    /**
     * SQL-Profile Cache 를 구축
     */

    STL_TRY( eldcNoMountBuildProfileCache( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief NO-MOUNT 단계의 SQL-Column Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcNoMountBuildColumnCache( ellEnv * aEnv )
{
    /**
     * Column ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_COLUMN_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Column Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_COLUMN_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLColumnDumpHandle();
    
    /**
     * SQL-Column Cache 구축할 필요가 없음 
     */

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief NO-MOUNT 단계의 SQL-Table Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcNoMountBuildTableCache( ellEnv * aEnv )
{
    /**
     * Table ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_TABLE_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Table Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_TABLE_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLTableDumpHandle();
    
    /**
     * SQL-Table Cache 구축할 필요가 없음 
     */
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief NO-MOUNT 단계의 SQL-Index Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcNoMountBuildIndexCache( ellEnv * aEnv )
{
    /**
     * Index ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_INDEX_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Index Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_INDEX_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLIndexDumpHandle();
    
    /**
     * SQL-Index Cache 구축할 필요가 없음 
     */

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief NO-MOUNT 단계의 SQL-Constraint Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcNoMountBuildConstCache( ellEnv * aEnv )
{
    /**
     * Constraint ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_CONSTRAINT_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Constraint Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_CONSTRAINT_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLConstDumpHandle();
    
    /**
     * SQL-Constraint Cache를 구축할 필요가 없음 
     */

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief NO-MOUNT 단계의 SQL-Sequence Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcNoMountBuildSequenceCache( ellEnv * aEnv )
{
    /**
     * Sequence ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SEQUENCE_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Sequence Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SEQUENCE_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLSequenceDumpHandle();
    
    /**
     * SQL-Sequence Cache 구축할 필요가 없음 
     */

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief NO-MOUNT 단계의 SQL-Schema Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcNoMountBuildSchemaCache( ellEnv * aEnv )
{
    /**
     * Schema ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SCHEMA_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Schema Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SCHEMA_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLSchemaDumpHandle();
    
    /**
     * SQL-Schema Cache 구축
     * - FIXED_TABLE_SCHEMA 를 접근할 수 있어야 함.
     */

    STL_TRY( eldcAddNonServiceSchemaCache( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief NO-MOUNT 단계의 SQL-Tablespace Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcNoMountBuildTablespaceCache( ellEnv * aEnv )
{
    /**
     * Tablespace ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SPACE_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Tablespace Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SPACE_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLTablespaceDumpHandle();
    
    /**
     * SQL-Tablespace Cache 구축
     * - SM 으로부터 구축
     */

    STL_TRY( eldcAddNonServiceSpaceCache( KNL_STARTUP_PHASE_NO_MOUNT, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief NO-MOUNT 단계의 SQL-Authorization Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcNoMountBuildAuthCache( ellEnv * aEnv )
{
    /**
     * Auth ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_AUTH_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Auth Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_AUTH_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLAuthDumpHandle();
    
    /**
     * SQL-Auth Cache 구축
     * - SYS 계정에 대한 정보를 구축해야 함.
     */

    STL_TRY( eldcAddNonServiceAuthCache( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief NO-MOUNT 단계의 SQL-Catalog Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcNoMountBuildCatalogCache( ellEnv * aEnv )
{
    /**
     * Catalog ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_CATALOG_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Catalog Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_CATALOG_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLCatalogDumpHandle();
    
    /**
     * SQL-Catalog Cache 구축
     * - 기본값을 이용해 구축 
     */

    STL_TRY( eldcAddNonServiceCatalogCache( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief NO-MOUNT 단계의 SQL-Synonym Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcNoMountBuildSynonymCache( ellEnv * aEnv )
{
    /**
     * Synonym ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SYNONYM_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Synonym Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SYNONYM_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLSynonymDumpHandle();
    
    /**
     * SQL-Synonym Cache 구축할 필요가 없음 
     */

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief NO-MOUNT 단계의 SQL-PublicSynonym Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcNoMountBuildPublicSynonymCache( ellEnv * aEnv )
{
    /**
     * Public Synonym ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_PUBLIC_SYNONYM_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Public Synonym Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLPublicSynonymDumpHandle();
    
    /**
     * SQL-PublicSynonym Cache 구축할 필요가 없음 
     */

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief NO-MOUNT 단계의 SQL-Profile Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcNoMountBuildProfileCache( ellEnv * aEnv )
{
    /**
     * Profile ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_PROFILE_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Profile Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_PROFILE_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLProfileDumpHandle();
    
    /**
     * SQL-Profile Cache 구축할 필요가 없음 
     */

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/*****************************************************
 * MOUNT 단계 함수 
 *****************************************************/

/**
 * @brief MOUNT 단계의 SQL-Object Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcMountBuildObjectCache( ellEnv * aEnv )
{
    /**
     * SQL-Column Cache 를 구축
     */

    STL_TRY( eldcMountBuildColumnCache( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Table Cache 를 구축
     */

    STL_TRY( eldcMountBuildTableCache( aEnv ) == STL_SUCCESS );

    /**
     * SQL-Index Cache 를 구축
     */

    STL_TRY( eldcMountBuildIndexCache( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Constraint Cache 를 구축
     */

    STL_TRY( eldcMountBuildConstCache( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Sequence Cache 를 구축
     */

    STL_TRY( eldcMountBuildSequenceCache( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Schema Cache 를 구축
     */

    STL_TRY( eldcMountBuildSchemaCache( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Tablespace Cache 를 구축
     */

    STL_TRY( eldcMountBuildTablespaceCache( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Authorization Cache 를 구축
     */

    STL_TRY( eldcMountBuildAuthCache( aEnv ) == STL_SUCCESS );
    
    /**
     * SQL-Catalog Cache 를 구축
     */

    STL_TRY( eldcMountBuildCatalogCache( aEnv ) == STL_SUCCESS );

    /**
     * SQL-Synonym Cache 를 구축
     */

    STL_TRY( eldcMountBuildSynonymCache( aEnv ) == STL_SUCCESS );

    /**
     * SQL-PublicSynonym Cache 를 구축
     */

    STL_TRY( eldcMountBuildPublicSynonymCache( aEnv ) == STL_SUCCESS );

    /**
     * SQL-Profile Cache 를 구축
     */

    STL_TRY( eldcMountBuildProfileCache( aEnv ) == STL_SUCCESS );
    
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}



/**
 * @brief MOUNT 단계의 SQL-Column Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcMountBuildColumnCache( ellEnv * aEnv )
{
    /**
     * Column ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_COLUMN_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Column Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_COLUMN_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLColumnDumpHandle();
    
    /**
     * SQL-Column Cache 구축할 필요가 없음 
     */

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief MOUNT 단계의 SQL-Table Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcMountBuildTableCache( ellEnv * aEnv )
{
    /**
     * Table ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_TABLE_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Table Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_TABLE_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLTableDumpHandle();
    
    /**
     * SQL-Table Cache 구축할 필요가 없음 
     */
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief MOUNT 단계의 SQL-Index Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcMountBuildIndexCache( ellEnv * aEnv )
{
    /**
     * Index ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_INDEX_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Index Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_INDEX_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLIndexDumpHandle();
    
    /**
     * SQL-Index Cache 구축할 필요가 없음 
     */

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief MOUNT 단계의 SQL-Constraint Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcMountBuildConstCache( ellEnv * aEnv )
{
    /**
     * Constraint ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_CONSTRAINT_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Constraint Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_CONSTRAINT_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLConstDumpHandle();
    
    /**
     * SQL-Constraint Cache를 구축할 필요가 없음 
     */

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief MOUNT 단계의 SQL-Sequence Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcMountBuildSequenceCache( ellEnv * aEnv )
{
    /**
     * Sequence ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SEQUENCE_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Sequence Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SEQUENCE_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLSequenceDumpHandle();
    
    /**
     * SQL-Sequence Cache 구축할 필요가 없음 
     */

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief MOUNT 단계의 SQL-Schema Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcMountBuildSchemaCache( ellEnv * aEnv )
{
    /**
     * Schema ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SCHEMA_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Schema Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SCHEMA_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLSchemaDumpHandle();
    
    /**
     * SQL-Schema Cache 구축
     * - FIXED_TABLE_SCHEMA 를 접근할 수 있어야 함.
     */

    STL_TRY( eldcAddNonServiceSchemaCache( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief MOUNT 단계의 SQL-Tablespace Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcMountBuildTablespaceCache( ellEnv * aEnv )
{
    /**
     * Tablespace ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SPACE_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Tablespace Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SPACE_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLTablespaceDumpHandle();
    
    /**
     * SQL-Tablespace Cache 구축
     * - SM 으로부터 구축
     */

    STL_TRY( eldcAddNonServiceSpaceCache( KNL_STARTUP_PHASE_MOUNT, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief MOUNT 단계의 SQL-Authorization Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcMountBuildAuthCache( ellEnv * aEnv )
{
    /**
     * Auth ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_AUTH_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Auth Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_AUTH_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLAuthDumpHandle();
    
    /**
     * SQL-Auth Cache 구축
     * - SYS 계정에 대한 정보를 구축해야 함.
     */

    STL_TRY( eldcAddNonServiceAuthCache( aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief MOUNT 단계의 SQL-Catalog Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcMountBuildCatalogCache( ellEnv * aEnv )
{
    /**
     * Catalog ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_CATALOG_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Catalog Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_CATALOG_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLCatalogDumpHandle();
    
    /**
     * SQL-Catalog Cache 구축
     * - 기본값을 이용해 구축 
     */

    STL_TRY( eldcAddNonServiceCatalogCache( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief MOUNT 단계의 SQL-Synonym Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcMountBuildSynonymCache( ellEnv * aEnv )
{
    /**
     * Synonym ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SYNONYM_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Synonym Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_SYNONYM_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLSynonymDumpHandle();
    
    /**
     * SQL-Synonym Cache 구축할 필요가 없음 
     */

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief MOUNT 단계의 SQL-PublicSynonym Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcMountBuildPublicSynonymCache( ellEnv * aEnv )
{
    /**
     * Public Synonym ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_PUBLIC_SYNONYM_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Public Synonym Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_PUBLIC_SYNONYM_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLPublicSynonymDumpHandle();
    
    /**
     * SQL-PublicSynonym Cache 구축할 필요가 없음 
     */

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief MOUNT 단계의 SQL-Profile Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcMountBuildProfileCache( ellEnv * aEnv )
{
    /**
     * Profile ID 를 위한 Cache 초기화 
     */

    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_PROFILE_ID ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Profile Name 을 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_OBJECT_CACHE_ADDR( ELDC_OBJECTCACHE_PROFILE_NAME ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetSQLProfileDumpHandle();
    
    /**
     * SQL-Profile Cache 구축할 필요가 없음 
     */

    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/*****************************************************
 * NON-SERVICE 단계에서의 Object Cache 구축 
 *****************************************************/

/**
 * @brief NON-SERVICE 단계에서 사용할 SQL-Schema Cache 구축
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcAddNonServiceSchemaCache( ellEnv * aEnv )
{
    ellSchemaDesc   * sSchemaDesc = NULL;

    eldcHashElement * sHashElementSchemaID = NULL;
    eldcHashElement * sHashElementSchemaName = NULL;

    stlInt32  sStrLen = 0;

    stlInt32 i = 0;
    stlBool  sByPass = STL_FALSE;
    
    /**
     * FIXED_TABLE_SCHEMA, PERFORMANCE_VIEW_SCHEMA 를 위한 Cache Descriptor 구성
     */

    for ( i = ELDT_SCHEMA_ID_NA + 1; i < ELDT_SCHEMA_ID_MAX; i++ )
    {
        STL_TRY( eldAllocCacheMem( (void **) & sSchemaDesc,
                                   STL_SIZEOF(ellSchemaDesc),
                                   aEnv )
                 == STL_SUCCESS );

        switch (i)
        {
            case ELDT_SCHEMA_ID_DEFINITION_SCHEMA:
                {
                    sByPass = STL_TRUE;
                    break;
                }
            case ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA:
                {
                    sSchemaDesc->mOwnerID    = ELDT_AUTH_ID_SYSTEM;    
                    sSchemaDesc->mSchemaID   = gEldtBuiltInSchemaFIXED_TABLE_SCHEMA.mSchemaID;
                    
                    sStrLen = stlStrlen( gEldtBuiltInSchemaString[ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA] );
                    STL_TRY( eldAllocCacheMem( (void **) & sSchemaDesc->mSchemaName,
                                               sStrLen + 1,
                                               aEnv )
                             == STL_SUCCESS );
                    stlMemcpy( sSchemaDesc->mSchemaName,
                               gEldtBuiltInSchemaString[ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA],
                               sStrLen );
                    sSchemaDesc->mSchemaName[sStrLen] = '\0';
    
                    sByPass = STL_FALSE;
                    break;
                }
            case ELDT_SCHEMA_ID_DICTIONARY_SCHEMA:
                {
                    sByPass = STL_TRUE;
                    break;
                }
            case ELDT_SCHEMA_ID_INFORMATION_SCHEMA:
                {
                    sByPass = STL_TRUE;
                    break;
                }
            case ELDT_SCHEMA_ID_PERFORMANCE_VIEW_SCHEMA:
                {
                    sSchemaDesc->mOwnerID    = ELDT_AUTH_ID_SYSTEM;    
                    sSchemaDesc->mSchemaID   = gEldtBuiltInSchemaPERFORMANCE_VIEW_SCHEMA.mSchemaID;
                    
                    sStrLen = stlStrlen( gEldtBuiltInSchemaString[ELDT_SCHEMA_ID_PERFORMANCE_VIEW_SCHEMA] );
                    STL_TRY( eldAllocCacheMem( (void **) & sSchemaDesc->mSchemaName,
                                               sStrLen + 1,
                                               aEnv )
                             == STL_SUCCESS );
                    stlMemcpy( sSchemaDesc->mSchemaName,
                               gEldtBuiltInSchemaString[ELDT_SCHEMA_ID_PERFORMANCE_VIEW_SCHEMA],
                               sStrLen );
                    sSchemaDesc->mSchemaName[sStrLen] = '\0';
                    
                    sByPass = STL_FALSE;
                    break;
                }
            case ELDT_SCHEMA_ID_PUBLIC:
                {
                    sByPass = STL_TRUE;
                    break;
                }
            default:
                {
                    STL_DASSERT(0);
                    
                    sByPass = STL_TRUE;
                    break;
                }
        }

        if ( sByPass == STL_TRUE )
        {
            continue;
        }
        
        /**
         * Schema ID 를 위한 Hash Element 구성
         */
    
        STL_TRY( eldcMakeHashElementSchemaID( & sHashElementSchemaID,
                                              SML_INVALID_TRANSID,
                                              sSchemaDesc,
                                              aEnv )
                 == STL_SUCCESS );

        /* Hash Element 의 Version 정보 변경 */
        eldcSetStableVersionInfo( & sHashElementSchemaID->mVersionInfo );

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
                                                SML_INVALID_TRANSID,
                                                sSchemaDesc,
                                                sHashElementSchemaID,
                                                aEnv )
                 == STL_SUCCESS );

        /* Hash Element 의 Version 정보 변경 */
        eldcSetStableVersionInfo( & sHashElementSchemaName->mVersionInfo );

        /**
         * ELDC_OBJECTCACHE_SCHEMA_NAME 에 추가 
         */

        STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SCHEMA_NAME ),
                                        sHashElementSchemaName,
                                        aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NON-SERVICE 단계에서 사용할 SQL-Auth Cache 구축
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcAddNonServiceAuthCache( ellEnv * aEnv )
{
    stlInt32  i;
    
    ellAuthDesc   * sAuthDesc[4];
    
    eldcHashElement * sHashElementAuthID = NULL;
    eldcHashElement * sHashElementAuthName = NULL;

    stlInt32  sStrLen = 0;

    stlInt32  sSchemaPathCnt = 2;
    
    /**
     * SYS user 를 위한 Cache Descriptor 구성
     */

    STL_TRY( eldAllocCacheMem( (void **) & sAuthDesc[0],
                               STL_SIZEOF(ellAuthDesc),
                               aEnv )
             == STL_SUCCESS );

    STL_TRY( eldAllocCacheMem( (void **) & sAuthDesc[0]->mSchemaIDArray,
                               STL_SIZEOF(stlInt64) * sSchemaPathCnt,
                               aEnv )
             == STL_SUCCESS );
    
    sAuthDesc[0]->mAuthID = gEldtBuiltInAuthSYS.mAuthID;

    sStrLen = stlStrlen( gEldtAuthString[ELDT_AUTH_ID_SYS] );
    STL_TRY( eldAllocCacheMem( (void **) & sAuthDesc[0]->mAuthName,
                               sStrLen + 1,
                               aEnv )
             == STL_SUCCESS );
    stlMemcpy( sAuthDesc[0]->mAuthName,
               gEldtAuthString[ELDT_AUTH_ID_SYS],
               sStrLen );
    sAuthDesc[0]->mAuthName[sStrLen] = '\0';
    
    sAuthDesc[0]->mAuthType = gEldtBuiltInAuthSYS.mAuthType;
    sAuthDesc[0]->mDataSpaceID = SML_MEMORY_DATA_SYSTEM_TBS_ID;
    sAuthDesc[0]->mTempSpaceID = SML_MEMORY_TEMP_SYSTEM_TBS_ID;
    sAuthDesc[0]->mSchemaPathCnt = sSchemaPathCnt;
    sAuthDesc[0]->mSchemaIDArray[0] = ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA;
    sAuthDesc[0]->mSchemaIDArray[1] = ELDT_SCHEMA_ID_PERFORMANCE_VIEW_SCHEMA;

    sAuthDesc[0]->mGrantedRoleCnt     = 0;
    sAuthDesc[0]->mGrantedRoleIDArray = NULL;
    sAuthDesc[0]->mIsGrantableArray   = NULL;

    /**
     * PUBLIC 를 위한 Cache Descriptor 구성
     */

    STL_TRY( eldAllocCacheMem( (void **) & sAuthDesc[1],
                               STL_SIZEOF(ellAuthDesc),
                               aEnv )
             == STL_SUCCESS );

    STL_TRY( eldAllocCacheMem( (void **) & sAuthDesc[1]->mSchemaIDArray,
                               STL_SIZEOF(stlInt64) * sSchemaPathCnt,
                               aEnv )
             == STL_SUCCESS );
    
    sAuthDesc[1]->mAuthID = gEldtBuiltInAuthPUBLIC.mAuthID;

    sStrLen = stlStrlen( gEldtAuthString[ELDT_AUTH_ID_PUBLIC] );
    STL_TRY( eldAllocCacheMem( (void **) & sAuthDesc[1]->mAuthName,
                               sStrLen + 1,
                               aEnv )
             == STL_SUCCESS );
    stlMemcpy( sAuthDesc[1]->mAuthName,
               gEldtAuthString[ELDT_AUTH_ID_PUBLIC],
               sStrLen );
    sAuthDesc[1]->mAuthName[sStrLen] = '\0';
    
    sAuthDesc[1]->mAuthType = gEldtBuiltInAuthPUBLIC.mAuthType;
    sAuthDesc[1]->mDataSpaceID = SML_MEMORY_DATA_SYSTEM_TBS_ID;
    sAuthDesc[1]->mTempSpaceID = SML_MEMORY_TEMP_SYSTEM_TBS_ID;
    sAuthDesc[1]->mSchemaPathCnt = sSchemaPathCnt;
    sAuthDesc[1]->mSchemaIDArray[0] = ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA;
    sAuthDesc[1]->mSchemaIDArray[1] = ELDT_SCHEMA_ID_PERFORMANCE_VIEW_SCHEMA;

    sAuthDesc[1]->mGrantedRoleCnt     = 0;
    sAuthDesc[1]->mGrantedRoleIDArray = NULL;
    sAuthDesc[1]->mIsGrantableArray   = NULL;

    /**
     * SYSDBA role 를 위한 Cache Descriptor 구성
     */

    STL_TRY( eldAllocCacheMem( (void **) & sAuthDesc[2],
                               STL_SIZEOF(ellAuthDesc),
                               aEnv )
             == STL_SUCCESS );

    STL_TRY( eldAllocCacheMem( (void **) & sAuthDesc[2]->mSchemaIDArray,
                               STL_SIZEOF(stlInt64) * sSchemaPathCnt,
                               aEnv )
             == STL_SUCCESS );
    
    sAuthDesc[2]->mAuthID = gEldtBuiltInAuthSYSDBA.mAuthID;

    sStrLen = stlStrlen( gEldtAuthString[ELDT_AUTH_ID_SYSDBA] );
    STL_TRY( eldAllocCacheMem( (void **) & sAuthDesc[2]->mAuthName,
                               sStrLen + 1,
                               aEnv )
             == STL_SUCCESS );
    stlMemcpy( sAuthDesc[2]->mAuthName,
               gEldtAuthString[ELDT_AUTH_ID_SYSDBA],
               sStrLen );
    sAuthDesc[2]->mAuthName[sStrLen] = '\0';
    
    sAuthDesc[2]->mAuthType = gEldtBuiltInAuthSYSDBA.mAuthType;
    sAuthDesc[2]->mDataSpaceID = SML_MEMORY_DATA_SYSTEM_TBS_ID;
    sAuthDesc[2]->mTempSpaceID = SML_MEMORY_TEMP_SYSTEM_TBS_ID;
    sAuthDesc[2]->mSchemaPathCnt = sSchemaPathCnt;
    sAuthDesc[2]->mSchemaIDArray[0] = ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA;
    sAuthDesc[2]->mSchemaIDArray[1] = ELDT_SCHEMA_ID_PERFORMANCE_VIEW_SCHEMA;

    sAuthDesc[2]->mGrantedRoleCnt     = 0;
    sAuthDesc[2]->mGrantedRoleIDArray = NULL;
    sAuthDesc[2]->mIsGrantableArray   = NULL;

    /**
     * ADMIN role 를 위한 Cache Descriptor 구성
     */

    STL_TRY( eldAllocCacheMem( (void **) & sAuthDesc[3],
                               STL_SIZEOF(ellAuthDesc),
                               aEnv )
             == STL_SUCCESS );

    STL_TRY( eldAllocCacheMem( (void **) & sAuthDesc[3]->mSchemaIDArray,
                               STL_SIZEOF(stlInt64) * sSchemaPathCnt,
                               aEnv )
             == STL_SUCCESS );
    
    sAuthDesc[3]->mAuthID = gEldtBuiltInAuthADMIN.mAuthID;

    sStrLen = stlStrlen( gEldtAuthString[ELDT_AUTH_ID_ADMIN] );
    STL_TRY( eldAllocCacheMem( (void **) & sAuthDesc[3]->mAuthName,
                               sStrLen + 1,
                               aEnv )
             == STL_SUCCESS );
    stlMemcpy( sAuthDesc[3]->mAuthName,
               gEldtAuthString[ELDT_AUTH_ID_ADMIN],
               sStrLen );
    sAuthDesc[3]->mAuthName[sStrLen] = '\0';
    
    sAuthDesc[3]->mAuthType = gEldtBuiltInAuthADMIN.mAuthType;
    sAuthDesc[3]->mDataSpaceID = SML_MEMORY_DATA_SYSTEM_TBS_ID;
    sAuthDesc[3]->mTempSpaceID = SML_MEMORY_TEMP_SYSTEM_TBS_ID;
    sAuthDesc[3]->mSchemaPathCnt = sSchemaPathCnt;
    sAuthDesc[3]->mSchemaIDArray[0] = ELDT_SCHEMA_ID_FIXED_TABLE_SCHEMA;
    sAuthDesc[3]->mSchemaIDArray[1] = ELDT_SCHEMA_ID_PERFORMANCE_VIEW_SCHEMA;

    sAuthDesc[3]->mGrantedRoleCnt     = 0;
    sAuthDesc[3]->mGrantedRoleIDArray = NULL;
    sAuthDesc[3]->mIsGrantableArray   = NULL;
    
    for ( i = 0; i < 4; i++ )
    {
        /**
         * Auth ID 를 위한 Hash Element 구성
         */
        
        STL_TRY( eldcMakeHashElementAuthID( & sHashElementAuthID,
                                            SML_INVALID_TRANSID,
                                            sAuthDesc[i],
                                            aEnv )
                 == STL_SUCCESS );

        /* Hash Element 의 Version 정보 변경 */
        eldcSetStableVersionInfo( & sHashElementAuthID->mVersionInfo );
        
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
                                              SML_INVALID_TRANSID,
                                              sAuthDesc[i],
                                              sHashElementAuthID,
                                              aEnv )
                 == STL_SUCCESS );
        
        /* Hash Element 의 Version 정보 변경 */
        eldcSetStableVersionInfo( & sHashElementAuthName->mVersionInfo );
        
        /**
         * ELDC_OBJECTCACHE_AUTH_NAME 에 추가 
         */
        
        STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_AUTH_NAME ),
                                        sHashElementAuthName,
                                        aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NON-SERVICE 단계에서 사용할 SQL-Catalog Cache 구축
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcAddNonServiceCatalogCache( ellEnv * aEnv )
{
    ellCatalogDesc   * sCatalogDesc = NULL;
    
    eldcHashElement * sHashElementCatalogID = NULL;
    eldcHashElement * sHashElementCatalogName = NULL;

    stlInt32        sStrLen = 0;

    /**
     * DEFAULT 값을 이용해 Cache Descriptor 구성
     */

    STL_TRY( eldAllocCacheMem( (void **) & sCatalogDesc,
                               STL_SIZEOF(ellCatalogDesc),
                               aEnv )
             == STL_SUCCESS );

    sCatalogDesc->mCatalogID        = ELDT_DEFAULT_CATALOG_ID;        

    sStrLen = stlStrlen( gEldcNonServiceDBName );
    STL_TRY( eldAllocCacheMem( (void **) & sCatalogDesc->mCatalogName,
                               sStrLen + 1,
                               aEnv )
             == STL_SUCCESS );
    stlMemcpy( sCatalogDesc->mCatalogName,
               gEldcNonServiceDBName,
               sStrLen );
    sCatalogDesc->mCatalogName[sStrLen] = '\0';
    
    sCatalogDesc->mCharacterSetID   = DTL_NOMOUNT_CHARSET_ID;   
    sCatalogDesc->mTimeZoneInterval.mIndicator = DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE;
    sCatalogDesc->mTimeZoneInterval.mDay       = 0;
    sCatalogDesc->mTimeZoneInterval.mTime      = DTL_NOMOUNT_GMT_OFFSET;     

    /**
     * Catalog ID 를 위한 Hash Element 구성
     */
    
    STL_TRY( eldcMakeHashElementCatalogID( & sHashElementCatalogID,
                                           SML_INVALID_TRANSID,
                                           sCatalogDesc,
                                           aEnv )
             == STL_SUCCESS );

    /* Hash Element 의 Version 정보 변경 */
    eldcSetStableVersionInfo( & sHashElementCatalogID->mVersionInfo );

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
                                             SML_INVALID_TRANSID,
                                             sCatalogDesc,
                                             sHashElementCatalogID,
                                             aEnv )
             == STL_SUCCESS );

    /* Hash Element 의 Version 정보 변경 */
    eldcSetStableVersionInfo( & sHashElementCatalogName->mVersionInfo );

    /**
     * ELDC_OBJECTCACHE_CATALOG_NAME 에 추가 
     */

    STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_CATALOG_NAME ),
                                    sHashElementCatalogName,
                                    aEnv )
             == STL_SUCCESS );

    /**
     * Catalog Handle 설정
     */
    
    STL_TRY( ellGetCatalogDictHandle( SML_INVALID_TRANSID,
                                      ELL_CATALOG_HANDLE_ADDR(),
                                      ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NON-SERVICE 단계에서 사용할 SQL-Tablespace Cache 구축
 * @param[in]  aPhase  Start-Up Phase
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcAddNonServiceSpaceCache( knlStartupPhase   aPhase,
                                       ellEnv          * aEnv )
{
    ellTablespaceDesc  * sSpaceDesc = NULL;
    
    eldcHashElement * sHashElementSpaceID = NULL;
    eldcHashElement * sHashElementSpaceName = NULL;

    void      * sSpaceIter = NULL;
    stlChar     sSpaceName[STL_MAX_SQL_IDENTIFIER_LENGTH + 1];
    stlInt32    sSpaceNameLen = 0;
    stlUInt32   sSpaceAttr = 0;
    smlTbsId    sSpaceID   = 0;

    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( (aPhase == KNL_STARTUP_PHASE_NO_MOUNT) || (aPhase == KNL_STARTUP_PHASE_MOUNT),
                        ELL_ERROR_STACK(aEnv) );

    /**
     * TABLESPACE 정보가 존재할때까지 반복
     */

    STL_TRY( smlFetchFirstTbsInfo( & sSpaceIter,
                                   & sSpaceID,
                                   sSpaceName,
                                   & sSpaceAttr,
                                   SML_ENV(aEnv) )
             == STL_SUCCESS );

    while ( sSpaceIter != NULL )
    {
        /**
         * TABLESPACE Cache Descriptor 구성
         */
        
        STL_TRY( eldAllocCacheMem( (void **) & sSpaceDesc,
                                   STL_SIZEOF(ellTablespaceDesc),
                               aEnv )
                 == STL_SUCCESS );
        
        sSpaceDesc->mCreatorID = ELDT_AUTH_ID_SYS;
        sSpaceDesc->mTablespaceID = sSpaceID;

        sSpaceNameLen = stlStrlen( sSpaceName );
        
        STL_TRY( eldAllocCacheMem( (void **) & sSpaceDesc->mTablespaceName,
                                   sSpaceNameLen + 1,
                                   aEnv )
                 == STL_SUCCESS );
        stlMemcpy( sSpaceDesc->mTablespaceName, sSpaceName, sSpaceNameLen );
        sSpaceDesc->mTablespaceName[sSpaceNameLen] = '\0';

        switch ( sSpaceAttr & SML_TBS_DEVICE_MASK )
        {
            case SML_TBS_DEVICE_MEMORY:
                sSpaceDesc->mMediaType = ELL_SPACE_MEDIA_TYPE_MEMORY;
                break;
            case SML_TBS_DEVICE_DISK:
                sSpaceDesc->mMediaType = ELL_SPACE_MEDIA_TYPE_DISK;
                break;
            default:
                STL_ASSERT(0);
                break;
        }
        
        switch ( sSpaceAttr & SML_TBS_USAGE_MASK )
        {
            case SML_TBS_USAGE_DICT:
                sSpaceDesc->mUsageType = ELL_SPACE_USAGE_TYPE_DICT;
                break;
            case SML_TBS_USAGE_UNDO:
                sSpaceDesc->mUsageType = ELL_SPACE_USAGE_TYPE_UNDO;
                break;
            case SML_TBS_USAGE_DATA:
                sSpaceDesc->mUsageType = ELL_SPACE_USAGE_TYPE_DATA;
                break;
            case SML_TBS_USAGE_TEMPORARY:
                sSpaceDesc->mUsageType = ELL_SPACE_USAGE_TYPE_TEMPORARY;
                break;
            default:
                STL_ASSERT(0);
                break;
        }
        
        /**
         * Space ID 를 위한 Hash Element 구성
         */
    
        STL_TRY( eldcMakeHashElementTablespaceID( & sHashElementSpaceID,
                                                  SML_INVALID_TRANSID,
                                                  sSpaceDesc,
                                                  aEnv )
                 == STL_SUCCESS );

        /* Hash Element 의 Version 정보 변경 */
        eldcSetStableVersionInfo( & sHashElementSpaceID->mVersionInfo );

        /**
         * ELDC_OBJECTCACHE_SPACE_ID Hash 에 추가 
         */
    
        STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SPACE_ID ),
                                        sHashElementSpaceID,
                                        aEnv )
                 == STL_SUCCESS );

        /**
         * Space Name 을 위한 Hash Element 구성
         */

        STL_TRY( eldcMakeHashElementTablespaceName( & sHashElementSpaceName,
                                               SML_INVALID_TRANSID,
                                               sSpaceDesc,
                                               sHashElementSpaceID,
                                               aEnv )
                 == STL_SUCCESS );

        /* Hash Element 의 Version 정보 변경 */
        eldcSetStableVersionInfo( & sHashElementSpaceName->mVersionInfo );

        /**
         * ELDC_OBJECTCACHE_SPACE_NAME 에 추가 
         */

        STL_TRY( eldcInsertHashElement( ELL_OBJECT_CACHE( ELDC_OBJECTCACHE_SPACE_NAME ),
                                        sHashElementSpaceName,
                                        aEnv )
                 == STL_SUCCESS );

        /**
         * 다음 TABLESPACE 정보 획득
         */

        STL_TRY( smlFetchNextTbsInfo( & sSpaceIter,
                                      & sSpaceID,
                                      sSpaceName,
                                      & sSpaceAttr,
                                      SML_ENV(aEnv) )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*****************************************************
 * NON-SERVICE 단계에서의 Privilege Cache 구축 
 *****************************************************/

/**
 * @brief Non-Service 단계의 Privilege Cache 를 구축한다.
 * @param[in]  aEnv    Environment
 * @remarks
 */
stlStatus eldcNonServiceBuildPrivCache( ellEnv * aEnv )
{
    ellPrivDesc      * sPrivDesc = NULL;
    eldcHashElement  * sHashElement = NULL;
    
    stlInt32 i = 0;
    
    /**
     * Database Privilege 를 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_PRIV_CACHE_ADDR( ELDC_PRIVCACHE_DATABASE ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );
    eldcSetPrivDumpHandle( ELDC_PRIVCACHE_DATABASE );
    

    /**
     * Tablespace Privilege 를 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_PRIV_CACHE_ADDR( ELDC_PRIVCACHE_SPACE ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );
    eldcSetPrivDumpHandle( ELDC_PRIVCACHE_SPACE );

    /**
     * Schema Privilege 를 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_PRIV_CACHE_ADDR( ELDC_PRIVCACHE_SCHEMA ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );
    eldcSetPrivDumpHandle( ELDC_PRIVCACHE_SCHEMA );

    /**
     * Table Privilege 를 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_PRIV_CACHE_ADDR( ELDC_PRIVCACHE_TABLE ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );
    eldcSetPrivDumpHandle( ELDC_PRIVCACHE_TABLE );
    

    /**
     * Usage Privilege 를 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_PRIV_CACHE_ADDR( ELDC_PRIVCACHE_USAGE ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );
    eldcSetPrivDumpHandle( ELDC_PRIVCACHE_USAGE );

    /**
     * Column Privilege 를 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_PRIV_CACHE_ADDR( ELDC_PRIVCACHE_COLUMN ),
                                 ELDC_NON_SERVICE_HASH_BUCKET_COUNT,
                                 aEnv )
             == STL_SUCCESS );
    eldcSetPrivDumpHandle( ELDC_PRIVCACHE_COLUMN );
    
    /**
     * Database Privilege Cache 구축
     * - 모든 Database Privilege 를 추가 
     */

    for ( i = ELL_DB_PRIV_ACTION_NA + 1; i < ELL_DB_PRIV_ACTION_MAX; i++ )
    {
        /**
         * Privilege Descriptor 를 위한 공간 할당
         */
        
        STL_TRY( eldAllocCacheMem( (void **) & sPrivDesc,
                                   STL_SIZEOF(ellPrivDesc),
                                   aEnv )
                 == STL_SUCCESS );

        /**
         * Privilege Descriptor 구성
         */

        sPrivDesc->mGranteeID = ELDT_AUTH_ID_SYS;
        sPrivDesc->mObjectID  = ELDT_DEFAULT_CATALOG_ID;
        sPrivDesc->mPrivObject = ELL_PRIV_DATABASE;
        sPrivDesc->mPrivAction.mAction = i;
        sPrivDesc->mWithGrant = STL_FALSE;
        
        
        /**
         * Privilege 를 위한 Hash Element 구성
         */
        
        STL_TRY( eldcMakeHashElementPriv( & sHashElement,
                                          SML_INVALID_TRANSID,
                                          sPrivDesc,
                                          aEnv )
                 == STL_SUCCESS );

        /* Hash Element 의 Version 정보 변경 */
        eldcSetStableVersionInfo( & sHashElement->mVersionInfo );
        
        /**
         * ELDC_PRIVCACHE_DATABASE 에 추가 
         */
        
        STL_TRY( eldcInsertHashElement( ELL_PRIV_CACHE( ELDC_PRIVCACHE_DATABASE ),
                                        sHashElement,
                                        aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} eldcNonService */

