/*******************************************************************************
 * eldcSchemaPrivDesc.c
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
 * @file eldcSchemaPrivDesc.c
 * @brief Cache for Schema Privilege descriptor
 */

#include <ell.h>
#include <eldt.h>                               
#include <eldc.h>

#include <eldDictionary.h>



/**
 * @addtogroup eldcSchemaPrivDesc
 * @{
 */


/****************************************************************
 * 초기화, 마무리 함수
 ****************************************************************/

/**
 * @brief Schema Privilege 객체를 위한 Cache 를 구축한다.
 * @param[in] aTransID  Transaction ID
 * @param[in] aStmt     Statement
 * @param[in] aEnv      Environment
 * @remarks
 */
stlStatus eldcBuildSchemaPrivCache( smlTransId     aTransID,
                                    smlStatement * aStmt,
                                    ellEnv       * aEnv )
{
    stlInt64  sBucketCnt = 0;

    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );
    
    /**
     * Hash 의 Bucket 개수를 결정하기 위한 레코드 개수 획득
     */

    STL_TRY( eldcGetTableRecordCount( aTransID,
                                      aStmt,
                                      ELDT_TABLE_ID_SCHEMA_PRIVILEGES,
                                      & sBucketCnt,
                                      aEnv )
             == STL_SUCCESS );
    
    /**
     * 소수에 해당하는 Hash Bucket 개수의 조정
     */

    sBucketCnt = ellGetPrimeBucketCnt( sBucketCnt );
    
    /**
     * Schema Privilege 를 위한 Cache 초기화 
     */
    
    STL_TRY( eldcCreateDictHash( ELL_PRIV_CACHE_ADDR( ELDC_PRIVCACHE_SCHEMA ),
                                 sBucketCnt,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Dump Handle 설정
     */
    
    eldcSetPrivDumpHandle( ELDC_PRIVCACHE_SCHEMA );
    
    /**
     * Schema Privilege Cache 구축 
     */

    STL_TRY( eldcBuildCachePriv( aTransID,
                                 aStmt,
                                 ELDC_PRIVCACHE_SCHEMA,
                                 aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}




/** @} eldcSchemaPrivDesc */
