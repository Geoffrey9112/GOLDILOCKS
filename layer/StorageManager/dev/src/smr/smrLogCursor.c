/*******************************************************************************
 * smrScan.c
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

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smDef.h>
#include <smrDef.h>
#include <smr.h>
#include <smf.h>
#include <smrLogFile.h>
#include <smrLogBuffer.h>
#include <smrLogCursor.h>

/**
 * @file smrLogCursor.c
 * @brief Storage Manager Layer Recovery Log Cursor Internal Routines
 */

extern smrWarmupEntry * gSmrWarmupEntry;

/**
 * @addtogroup smrLogCursor
 * @{
 */

stlStatus smrOpenLogCursor( smrLsn         aOldestLsn,
                            smrLogCursor * aLogCursor,
                            stlBool        aIsForRecover,
                            smlEnv       * aEnv )
{
    /**
     * Log Cursor 초기화
     */

    aLogCursor->mIsForRecover = aIsForRecover;
    aLogCursor->mCurLsn = aOldestLsn;
    KNL_INIT_REGION_MARK( &aLogCursor->mMemMark );
    
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &aLogCursor->mMemMark,
                               (knlEnv*)aEnv )
             == STL_SUCCESS );
             
    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                SMR_MAX_LOGBODY_SIZE,
                                (void**)&aLogCursor->mLogBody,
                                KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrCloseLogCursor( smrLogCursor * aLogCursor,
                             smlEnv       * aEnv )
{
    if( aLogCursor->mIsForRecover == STL_TRUE )
    {
        gSmrWarmupEntry->mLsn = aLogCursor->mCurLsn + 1;
    }

    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &aLogCursor->mMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

inline smrLogHdr * smrGetCurLogHdr( smrLogCursor * aLogCursor )
{
    return &(aLogCursor->mLogHdr);
}

inline stlChar * smrGetCurLogBody( smrLogCursor * aLogCursor )
{
    return (aLogCursor->mLogBody);
}

stlStatus smrReadLog( smrLogCursor * aLogCursor,
                      stlBool      * aEndOfLog,
                      smlEnv       * aEnv )
{
    STL_TRY( smrReadStreamLog( &gSmrWarmupEntry->mLogStream,
                               &aLogCursor->mLogHdr,
                               aLogCursor->mLogBody,
                               aEndOfLog,
                               aEnv ) == STL_SUCCESS );

    aLogCursor->mCurLsn = aLogCursor->mLogHdr.mLsn;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smrReadNextLog( smrLogCursor * aCursor,
                          stlBool      * aEndOfLog,
                          smlEnv       * aEnv )
{
    *aEndOfLog = STL_FALSE;
    
    STL_TRY( smrReadLog( aCursor,
                         aEndOfLog,
                         aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
