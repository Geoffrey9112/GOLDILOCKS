/*******************************************************************************
 * eldcNonServiceCache.h
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


#ifndef _ELDC_NON_SERVICE_CACHE_H_
#define _ELDC_NON_SERVICE_CACHE_H_ 1

/**
 * @file eldcNonServiceCache.h
 * @brief Non-Service 단계에서의 Dictionary Cache 를 위한 전용 Hash 
 */

#define ELDC_NON_SERVICE_HASH_BUCKET_COUNT  (7)

/*****************************************************
 * NO MOUNT 단계 함수 
 *****************************************************/


stlStatus eldcNoMountBuildObjectCache( ellEnv * aEnv );

stlStatus eldcNoMountBuildColumnCache( ellEnv * aEnv );
stlStatus eldcNoMountBuildTableCache( ellEnv * aEnv );
stlStatus eldcNoMountBuildIndexCache( ellEnv * aEnv );
stlStatus eldcNoMountBuildConstCache( ellEnv * aEnv );
stlStatus eldcNoMountBuildSequenceCache( ellEnv * aEnv );
stlStatus eldcNoMountBuildSchemaCache( ellEnv * aEnv );
stlStatus eldcNoMountBuildTablespaceCache( ellEnv * aEnv );
stlStatus eldcNoMountBuildAuthCache( ellEnv * aEnv );
stlStatus eldcNoMountBuildCatalogCache( ellEnv * aEnv );
stlStatus eldcNoMountBuildSynonymCache( ellEnv * aEnv );
stlStatus eldcNoMountBuildPublicSynonymCache( ellEnv * aEnv );
stlStatus eldcNoMountBuildProfileCache( ellEnv * aEnv );

/*****************************************************
 * MOUNT 단계 함수 
 *****************************************************/

stlStatus eldcMountBuildObjectCache( ellEnv * aEnv );

stlStatus eldcMountBuildColumnCache( ellEnv * aEnv );
stlStatus eldcMountBuildTableCache( ellEnv * aEnv );
stlStatus eldcMountBuildIndexCache( ellEnv * aEnv );
stlStatus eldcMountBuildConstCache( ellEnv * aEnv );
stlStatus eldcMountBuildSequenceCache( ellEnv * aEnv );
stlStatus eldcMountBuildSchemaCache( ellEnv * aEnv );
stlStatus eldcMountBuildTablespaceCache( ellEnv * aEnv );
stlStatus eldcMountBuildAuthCache( ellEnv * aEnv );
stlStatus eldcMountBuildCatalogCache( ellEnv * aEnv );
stlStatus eldcMountBuildSynonymCache( ellEnv * aEnv );
stlStatus eldcMountBuildPublicSynonymCache( ellEnv * aEnv );
stlStatus eldcMountBuildProfileCache( ellEnv * aEnv );

/*****************************************************
 * NON-SERVICE 단계에서의 Object Cache 구축 
 *****************************************************/

stlStatus eldcAddNonServiceSchemaCache( ellEnv * aEnv );
stlStatus eldcAddNonServiceAuthCache( ellEnv * aEnv );
stlStatus eldcAddNonServiceCatalogCache( ellEnv * aEnv );
stlStatus eldcAddNonServiceSpaceCache( knlStartupPhase   aPhase,
                                       ellEnv          * aEnv );

/*****************************************************
 * NON-SERVICE 단계에서의 Privilege Cache 구축 
 *****************************************************/

stlStatus eldcNonServiceBuildPrivCache( ellEnv * aEnv );

#endif /* _ELDC_NON_SERVICE_CACHE_H_ */
