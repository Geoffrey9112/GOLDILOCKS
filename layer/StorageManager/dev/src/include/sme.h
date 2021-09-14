/*******************************************************************************
 * sme.h
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


#ifndef _SME_H_
#define _SME_H_ 1

#include <smDef.h>

/**
 * @file sme.h
 * @brief Storage Manager Layer Relation Internal Routines
 */

/**
 * @defgroup sme Relation Internal Routines
 * @ingroup smInternal
 * @{
 */

stlStatus smeStartupNoMount( void  ** aWarmupEntry,
                             smlEnv * aEnv );

stlStatus smeStartupOpen( smlEnv * aEnv );

stlStatus smeWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv );

stlStatus smeCooldown( smlEnv * aEnv );
stlStatus smeBootdown( smlEnv * aEnv );
stlStatus smeShutdownClose( smlEnv * aEnv );


inline smeIteratorModule * smeGetIteratorModule( void *aRelationHandle );
inline smeGroupIteratorModule * smeGetGroupIteratorModule( smlRelationType aRelationType );

stlStatus smeSetObjectScnPend( void * aArgs, smlEnv * aEnv );

stlStatus smeBuildCache( smlTbsId   aTbsId,
                         smlPid     aSegPid,
                         void     * aSegHandle,
                         smlEnv   * aEnv );

stlStatus smeRefineAtStartup( smlEnv * aEnv );

stlStatus smeBuildCachesAtStartup( smlEnv * aEnv );

stlStatus smeGetRelationHandle( smlRid    aSegRid,
                                void   ** aRelationHandle,
                                smlEnv  * aEnv );

stlStatus smeCreateRelHint( smlSessionEnv * aSessionEnv,
                            smlEnv        * aEnv );

smeHint * smeFindRelHint( void   * aCache,
                          smlScn   aValidScn,
                          smlEnv * aEnv );

smeHint * smeAddNewRelHint( smlPid     aHintPid,
                            void     * aCache,
                            smlScn     aValidScn,
                            smlEnv   * aEnv );

void smeSetJoinFilter4InstRelation( smlBlockJoinFetchInfo  * aJoinFetchInfo );
void smeSetPhysicalFilter4InstRelation( smlFetchInfo  *  aFetchInfo );

/** @} */
    
#endif /* _SME_H_ */
