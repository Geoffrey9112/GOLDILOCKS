/*******************************************************************************
 * smn.h
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


#ifndef _SMN_H_
#define _SMN_H_ 1

#include <smnDef.h>

/**
 * @file smn.h
 * @brief Storage Manager Layer Index Internal Routines
 */

/**
 * @defgroup smn Index
 * @ingroup smInternal
 * @{
 */

inline smnIndexModule * smnGetIndexModule( smlIndexType aIndexType );

inline stlStatus smnInitVolatileInfo( void * aRelationHandle, smlEnv * aEnv );

inline stlStatus smnSetRootPageId( smxmTrans * aMiniTrans,
                                   void      * aRelationHandle,
                                   smlPid      aNewRootPid,
                                   smlEnv    * aEnv );
inline stlStatus smnSetMirrorRootPageId( smxmTrans * aMiniTrans,
                                         void      * aRelationHandle,
                                         smlPid      aNewRootPid,
                                         smlEnv    * aEnv );

stlStatus smnDropAging( void      * aData,
                        stlUInt32   aDataSize,
                        stlBool   * aDone,
                        void      * aEnv );

stlStatus smnDropAgingInternal( void        * aRelHandle,
                                stlBool       aOnStartup,
                                stlBool     * aDone,
                                smlEnv      * aEnv );

stlStatus smnTruncateAging( void      * aData,
                            stlUInt32   aDataSize,
                            stlBool   * aDone,
                            void      * aEnv );

stlStatus smnStartupNoMount( void  ** aWarmupEntry,
                             smlEnv * aEnv );
stlStatus smnStartupOpen( smlEnv * aEnv );

stlStatus smnWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv );

stlStatus smnCooldown( smlEnv * aEnv );
stlStatus smnBootdown( smlEnv * aEnv );
stlStatus smnShutdownClose( smlEnv * aEnv );
stlStatus smnRegisterFxTables( smlEnv * aEnv );

stlStatus smnRebuildAtStartup( smlEnv * aEnv );
stlStatus smnRebuildIndex( void       * aIndexHandle,
                           smlEnv     * aEnv );

stlStatus smnBuildUnusableIndexes( smlTbsId   aTbsId,
                                   smlEnv   * aEnv );

stlStatus smnInternalError( smpHandle      * aPageHandle,
                            smlEnv         * aEnv,
                            const stlChar  * aFormat,
                            ... );

/** @} */
    
#endif /* _SMN_H_ */
