/*******************************************************************************
 * smsmfScan.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:
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
#include <smxm.h>
#include <smp.h>
#include <sma.h>
#include <smsDef.h>
#include <smsmfDef.h>
#include <smsmfScan.h>

/**
 * @file smsmfScan.c
 * @brief Storage Manager Layer Memory Flat Segment Scan Internal Routines
 */

/**
 * @addtogroup smsmfScan
 * @{
 */

stlStatus smsmfAllocSegIterator( smlStatement  * aStatement,
                                 void         ** aSegIterator,
                                 smlEnv        * aEnv )
{
    STL_TRY( smaAllocRegionMem( aStatement,
                                STL_SIZEOF( smsmfSegIterator ),
                                aSegIterator,
                                aEnv ) == STL_SUCCESS );

    ((smsmfSegIterator *)(*aSegIterator))->mNextPid = SMP_NULL_PID;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmfFreeSegIterator( void   *aSegIterator,
                                smlEnv *aEnv )
{
    return STL_SUCCESS;
}

static stlStatus smsmfSetNext( smlTbsId     aTbsId,
                               smlPid       aPid,
                               smlPid      *aNextPid,
                               smlEnv      *aEnv )
{
    smpHandle        sPageHandle;
    smsmfExtHdr    * sExtHdr;

    STL_TRY( smpFix( aTbsId,
                     aPid,
                     &sPageHandle,
                     aEnv )
             == STL_SUCCESS );

    sExtHdr = (smsmfExtHdr *)smpGetBody( SMP_FRAME( &sPageHandle ) );
    *aNextPid = sExtHdr->mNextExtFirstPid;

    STL_TRY( smpUnfix( &sPageHandle,
                       aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmfGetFirstPage( smlTbsId     aTbsId,
                             void        *aSegHandle,
                             void        *aSegIterator,
                             smlPid      *aPageId,
                             smlEnv      *aEnv )
{
    *aPageId = ((smsmfCache *)aSegHandle)->mFirstExtFirstPid;

    if( *aPageId != SMP_NULL_PID )
    {
        STL_TRY( smsmfSetNext( aTbsId,
                               *aPageId,
                               &((smsmfSegIterator *)aSegIterator)->mNextPid,
                               aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmfGetNextPage( smlTbsId     aTbsId,
                            void        *aSegIterator,
                            smlPid      *aPageId,
                            smlEnv      *aEnv )
{
    *aPageId = ((smsmfSegIterator *)aSegIterator)->mNextPid;

    if( *aPageId != SMP_NULL_PID )
    {
        STL_TRY( smsmfSetNext( aTbsId,
                               *aPageId,
                               &((smsmfSegIterator *)aSegIterator)->mNextPid,
                               aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */

