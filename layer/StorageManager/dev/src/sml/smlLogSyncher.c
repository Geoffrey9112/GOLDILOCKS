/*******************************************************************************
 * smlSyncher.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smlSyncher.c  $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smf.h>
#include <smr.h>
#include <smlLogSyncher.h>
#include <smlGeneral.h>


stlStatus smlReproduceRecoveryWarmupEntry( void      *  aSrcWarmupEntry,
                                           void      ** aDesWarmupEntry,
                                           void      *  aSmfWarmupEntry,
                                           smlEnv    *  aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smrReproduceRecoveryWarmupEntry( aSrcWarmupEntry,
                                              aDesWarmupEntry,
                                              aSmfWarmupEntry,
                                              aEnv ) == STL_SUCCESS ); 

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    return STL_FAILURE;
}

stlStatus smlFlushAllLogs4Syncher( void      * aSmrWarmupEntry,
                                   stlBool   * aSwitchedLogfile,
                                   stlBool     aSilent,
                                   stlLogger * aLogger,
                                   smlEnv    * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smrFlushAllLogs4Syncher( aSmrWarmupEntry,
                                      aSwitchedLogfile,
                                      aSilent,
                                      aLogger,
                                      aEnv ) == STL_SUCCESS ); 

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    return STL_FAILURE;
}

stlStatus smlSaveCtrlFile4Syncher( void      * aSmfWarmupEntry,
                                   void      * aSmrWarmupEntry,
                                   stlBool     aSilent,
                                   stlLogger * aLogger,
                                   smlEnv    * aEnv )
{
    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY( smfSaveCtrlFile4Syncher( aSmfWarmupEntry,
                                      aSmrWarmupEntry,
                                      aSilent,
                                      aLogger,
                                      aEnv ) == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );

    return STL_FAILURE;
}


