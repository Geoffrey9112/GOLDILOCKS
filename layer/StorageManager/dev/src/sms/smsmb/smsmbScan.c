/*******************************************************************************
 * smsmbScan.c
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
#include <sms.h>
#include <smg.h>
#include <smsDef.h>
#include <smsmbDef.h>
#include <smsmbScan.h>
#include <smsmbSegment.h>

/**
 * @file smsmbScan.c
 * @brief Storage Manager Layer Memory Bitmap Full Scan Internal Routines
 */

/**
 * @addtogroup smsmbScan
 * @{
 */

stlStatus smsmbAllocSegIterator( smlStatement  * aStatement,
                                 void         ** aSegIterator,
                                 smlEnv        * aEnv )
{
    smsmbSegIterator * sSegIterator;

    STL_TRY( smaAllocRegionMem( aStatement,
                                STL_SIZEOF( smsmbSegIterator ),
                                aSegIterator,
                                aEnv ) == STL_SUCCESS );

    sSegIterator = (smsmbSegIterator*)(*aSegIterator);
    
    sSegIterator->mNextDataPos = 0;
    sSegIterator->mNextLeafPid = SMP_NULL_PID;
    sSegIterator->mSegmentHandle = NULL;
    stlMemset( sSegIterator->mDataPid, 0x00, STL_SIZEOF(smlPid) * SMSMB_LEAF_BIT_COUNT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmbFreeSegIterator( void   * aSegIterator,
                                smlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smsmbGetFirstPage( smlTbsId      aTbsId,
                             void        * aSegHandle,
                             void        * aSegIterator,
                             smlPid      * aPageId,
                             smlEnv      * aEnv )
{
    smsmbSegIterator * sSegIterator;
    smsmbCache       * sCache;
    smlPid             sPageId;
    smpHandle          sPageHandle;
    smpPageType        sPageType;
    stlInt32           i;
    smpFreeness        sFreeness;
    stlInt32           sState = 0;
    smsmbLeafHdr     * sLeafHdr;
    smsmbLeaf        * sLeaf;
    smsmbInternalHdr * sInternalHdr;
    smsmbInternal    * sInternal;

    sSegIterator = (smsmbSegIterator*)aSegIterator;
    sSegIterator->mSegmentHandle = aSegHandle;
    sCache = (smsmbCache*)aSegHandle;
    sPageId = sCache->mRootNode;

    while( 1 )
    {
        STL_TRY( smpFix( aTbsId,
                         sPageId,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
        sState = 1;

        sPageType = smpGetPageType( &sPageHandle );

        if( sPageType == SMP_PAGE_TYPE_BITMAP_INTERNAL )
        {
            sInternalHdr = (smsmbInternalHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
            sInternal = (smsmbInternal*)((stlChar*)sInternalHdr + STL_SIZEOF(smsmbInternalHdr));

            for( i = 0; i < SMSMB_INTERNAL_BIT_COUNT; i++ )
            {
                sFreeness = smsmbGetFreeness( sInternal->mFreeness, i );
                
                if( sFreeness != SMP_FREENESS_UNALLOCATED )
                {
                    sPageId = sInternal->mChildPid[i];
                    break;
                }
            }
        }
        else
        {
            SMS_TRY_INTERNAL_ERROR( sPageType == SMP_PAGE_TYPE_BITMAP_LEAF,
                                    aEnv,
                                    ( (void*)sCache,
                                      aEnv,
                                      "PAGE ID : %d \nSEGMENT CACHE : \n%s\nPAGE : \n%s",
                                      smpGetPageId( &sPageHandle ),
                                      knlDumpBinaryForAssert( (void*)sCache,
                                                              STL_SIZEOF(smsmbCache),
                                                              &aEnv->mLogHeapMem,
                                                              KNL_ENV(aEnv) ),
                                      knlDumpBinaryForAssert( SMP_FRAME(&sPageHandle),
                                                              SMP_PAGE_SIZE,
                                                              &aEnv->mLogHeapMem,
                                                              KNL_ENV(aEnv) ) ) );
            
            sLeafHdr = (smsmbLeafHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
            sLeaf = (smsmbLeaf*)((stlChar*)sLeafHdr + STL_SIZEOF(smsmbLeafHdr));

            sSegIterator->mNextDataPos = 0;
            sSegIterator->mDataPidCount = 0;
            sSegIterator->mNextLeafPid = sLeafHdr->mNextPid;
            
            for( i = 0; i < SMSMB_LEAF_BIT_COUNT; i++ )
            {
                sFreeness = smsmbGetFreeness( sLeaf->mFreeness, i );

                if( (sFreeness == SMP_FREENESS_FULL)       ||
                    (sFreeness == SMP_FREENESS_UPDATEONLY) ||
                    (sFreeness == SMP_FREENESS_INSERTABLE) )
                {
                    sSegIterator->mDataPid[sSegIterator->mDataPidCount] = sLeaf->mDataPid[i];
                    sSegIterator->mDataPidCount++;
                }
                else if( sFreeness == SMP_FREENESS_FREE )
                {
                    if( (sLeaf->mScn[i] != 0) &&
                        (smsmbIsAgableForScan(aTbsId, sLeaf->mScn[i], aEnv) == STL_FALSE) )
                    {
                        sSegIterator->mDataPid[sSegIterator->mDataPidCount] = sLeaf->mDataPid[i];
                        sSegIterator->mDataPidCount++;
                    }
                }
            }

            sState = 0;
            STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
            
            break;
        }
        
        sState = 0;
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    STL_TRY( smsmbGetNextPage( aTbsId,
                               aSegIterator,
                               aPageId,
                               aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smsmbGetNextPage( smlTbsId      aTbsId,
                            void        * aSegIterator,
                            smlPid      * aPageId,
                            smlEnv      * aEnv )
{
    smlPid             sPageId;
    smpHandle          sPageHandle;
    smpPageType        sPageType;
    stlInt32           i;
    smpFreeness        sFreeness;
    stlInt32           sState = 0;
    smsmbLeafHdr     * sLeafHdr;
    smsmbLeaf        * sLeaf;
    smsmbSegIterator * sSegIterator;

    *aPageId = SMP_NULL_PID;
    
    sSegIterator = (smsmbSegIterator*)aSegIterator;
    
    if( sSegIterator->mNextDataPos < sSegIterator->mDataPidCount )
    {
        *aPageId = sSegIterator->mDataPid[sSegIterator->mNextDataPos];
        sSegIterator->mNextDataPos += 1;
        
        STL_THROW( RAMP_SUCCESS );
    }

    sPageId = sSegIterator->mNextLeafPid;
    
    while( sPageId != SMP_NULL_PID )
    {
        STL_TRY( smpFix( aTbsId,
                         sPageId,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
        sState = 1;

        sPageType = smpGetPageType( &sPageHandle );

        SMS_TRY_INTERNAL_ERROR( sPageType == SMP_PAGE_TYPE_BITMAP_LEAF,
                                aEnv,
                                ( sSegIterator->mSegmentHandle,
                                  aEnv,
                                  "PAGE ID : %d \nSEGMENT CACHE : \n%s\nPAGE : \n%s",
                                  smpGetPageId( &sPageHandle ),
                                  knlDumpBinaryForAssert( (void*)sSegIterator->mSegmentHandle,
                                                          STL_SIZEOF(smsmbCache),
                                                          &aEnv->mLogHeapMem,
                                                          KNL_ENV(aEnv) ),
                                  knlDumpBinaryForAssert( SMP_FRAME(&sPageHandle),
                                                          SMP_PAGE_SIZE,
                                                          &aEnv->mLogHeapMem,
                                                          KNL_ENV(aEnv) ) ) );
            
        sLeafHdr = (smsmbLeafHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
        sLeaf = (smsmbLeaf*)((stlChar*)sLeafHdr + STL_SIZEOF(smsmbLeafHdr));
    
        sSegIterator->mNextDataPos = 0;
        sSegIterator->mDataPidCount = 0;
        sSegIterator->mNextLeafPid = sLeafHdr->mNextPid;
            
        for( i = 0; i < SMSMB_LEAF_BIT_COUNT; i++ )
        {
            sFreeness = smsmbGetFreeness( sLeaf->mFreeness, i );

            if( (sFreeness == SMP_FREENESS_FULL)       ||
                (sFreeness == SMP_FREENESS_UPDATEONLY) ||
                (sFreeness == SMP_FREENESS_INSERTABLE) )
            {
                sSegIterator->mDataPid[sSegIterator->mDataPidCount] = sLeaf->mDataPid[i];
                sSegIterator->mDataPidCount++;
            }
            else if( sFreeness == SMP_FREENESS_FREE )
            {
                if( (sLeaf->mScn[i] != 0) &&
                    (smsmbIsAgableForScan(aTbsId, sLeaf->mScn[i], aEnv) == STL_FALSE) )
                {
                    sSegIterator->mDataPid[sSegIterator->mDataPidCount] = sLeaf->mDataPid[i];
                    sSegIterator->mDataPidCount++;
                }
            }
        }

        sState = 0;
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
            
        if( sSegIterator->mDataPidCount > 0 )
        {
            break;
        }

        sPageId = sSegIterator->mNextLeafPid;
    }

    if( sSegIterator->mNextDataPos < sSegIterator->mDataPidCount )
    {
        *aPageId = sSegIterator->mDataPid[sSegIterator->mNextDataPos];
        sSegIterator->mNextDataPos += 1;
    }

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

/** @} */
