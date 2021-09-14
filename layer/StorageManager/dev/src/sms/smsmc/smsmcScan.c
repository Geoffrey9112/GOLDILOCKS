/*******************************************************************************
 * smsmcScan.c
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
#include <smt.h>
#include <smxm.h>
#include <smxl.h>
#include <smp.h>
#include <sma.h>
#include <smg.h>
#include <smf.h>
#include <smsDef.h>
#include <smsmcDef.h>
#include <smsmcScan.h>

/**
 * @file smsmcScan.c
 * @brief Storage Manager Layer Memory Circular Full Scan Internal Routines
 */

/**
 * @addtogroup smsmcScan
 * @{
 */

stlStatus smsmcAllocSegIterator( smlStatement   * aStatement,
                                 void          ** aSegIterator,
                                 smlEnv         * aEnv )
{
    STL_TRY( smaAllocRegionMem( aStatement,
                                STL_SIZEOF( smsmcSegIterator ),
                                aSegIterator,
                                aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmcFreeSegIterator( void   * aSegIterator,
                                smlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smsmcGetFirstPage( smlTbsId      aTbsId,
                             void        * aSegHandle,
                             void        * aSegIterator,
                             smlPid      * aPageId,
                             smlEnv      * aEnv )
{
    smsmcCache       * sCache;
    smsmcSegIterator * sSegIterator;

    sSegIterator = (smsmcSegIterator*)aSegIterator;
    sCache = (smsmcCache*)aSegHandle;

    sSegIterator->mFirstPid = sCache->mCurDataPid;
    sSegIterator->mCurDataPid = sCache->mCurDataPid;
    sSegIterator->mHdrPid = sCache->mHdrPid;

    *aPageId = sSegIterator->mCurDataPid;
    
    return STL_SUCCESS;
}
    
stlStatus smsmcGetNextPage( smlTbsId      aTbsId,
                            void        * aSegIterator,
                            smlPid      * aPageId,
                            smlEnv      * aEnv )
{
    smsmcSegIterator * sSegIterator;
    smsmcExtHdr      * sExtHdr;
    smpHandle          sPageHandle;
    stlUInt16          sPageCountInExt;

    sPageCountInExt = smfGetPageCountInExt( aTbsId );
    sSegIterator = (smsmcSegIterator*)aSegIterator;

    sSegIterator->mCurDataPid--;

    if( sSegIterator->mHdrPid == sSegIterator->mCurDataPid )
    {
        STL_TRY( smpFix( aTbsId,
                         sSegIterator->mHdrPid,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

        sExtHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

        sSegIterator->mHdrPid = sExtHdr->mPrevHdrPid;
        sSegIterator->mCurDataPid = sExtHdr->mPrevHdrPid + sPageCountInExt - 1;

        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    if( sSegIterator->mFirstPid == sSegIterator->mCurDataPid )
    {
        *aPageId = SMP_NULL_PID;
    }
    else
    {
        *aPageId = sSegIterator->mCurDataPid;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
