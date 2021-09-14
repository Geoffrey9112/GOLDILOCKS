/*******************************************************************************
 * knlProcess.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: knlProcess.c 13901 2014-11-13 01:31:33Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file knlProcess.c
 * @brief Kernel Layer Process Internal Routines
 */

#include <knl.h>
#include <knlDef.h>
#include <knDef.h>

/**
 * @brief 공유 메모리에 붙어있는 모든 Process를 종료시킨다.
 * @param[in] aEntryPoint  kernel entry point
 * @param[out] aIsActive  gmaster active flag
 * @param[in] aSilent  silent flag
 * @param[in] aLogger  logger manager
 * @param[in,out] aEnv  environment
 * @remark gmaster가 살아있다면 에러
 */ 
stlStatus knlKillProcesses4Syncher( void      * aEntryPoint,
                                    stlBool   * aIsActive,
                                    stlBool     aSilent,
                                    stlLogger * aLogger,
                                    knlEnv    * aEnv )
{
    kniProcessManager * sProcessManager;
    kniProcessInfo    * sProcessArray;
    kniProcessInfo    * sProcessInfo;
    stlInt32            i;
    knsEntryPoint     * sKnEntryPoint; 
    stlChar             sProcName[STL_PATH_MAX];
    
    *aIsActive = STL_FALSE;
    
    sKnEntryPoint = (knsEntryPoint *)aEntryPoint;
    
    sProcessManager = sKnEntryPoint->mProcessManager;
    sProcessArray   = (void*)sProcessManager + STL_SIZEOF( kniProcessManager );

    sProcessInfo = &sProcessArray[0];
    
    if( stlIsActiveProc( &sProcessInfo->mProc ) == STL_TRUE )
    {
        *aIsActive = STL_TRUE;
        STL_THROW( RAMP_SKIP );
    }
    
    for( i = 1; i < sProcessManager->mMaxProcessCount; i++ )
    {
        sProcessInfo = &sProcessArray[i];

        if( sProcessInfo->mProc.mProcID == KNL_INVALID_PROCESS_ID )
        {
            continue;
        }

        if( stlIsActiveProc( &sProcessInfo->mProc ) == STL_FALSE )
        {
            continue;
        }

        STL_TRY( stlGetProcName( &sProcessInfo->mProc,
                                 sProcName,
                                 STL_PATH_MAX,
                                 KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        STL_TRY( stlKillProc( &sProcessInfo->mProc,
                              STL_SIGNAL_KILL,
                              KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );

        
        if( aLogger != NULL )
        {
            STL_TRY( stlLogMsg( aLogger,
                                KNL_ERROR_STACK(aEnv),
                                "[CLEAR PROCESS] Process '%s' is cleared.\n",
                                sProcName  ) == STL_SUCCESS );
        }

        if( aSilent == STL_FALSE )
        {
            stlPrintf( "[CLEAR PROCESS] Process '%s' is cleared.\n",
                       sProcName );
        }
    }

    STL_RAMP( RAMP_SKIP );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( aLogger != NULL )
    {
        (void) stlLogMsg( aLogger,
                          KNL_ERROR_STACK( aEnv ),
                          "[CLEAR PROCESS] Clear process '%d' is failed.\n",
                          sProcessInfo->mProc.mProcID );
    }
    
    if( aSilent == STL_FALSE )
    {
        stlPrintf( "[CLEAR PROCESS] Clear process '%d' is failed.\n",
                   sProcessInfo->mProc.mProcID );
    }

    return STL_FAILURE;
}

