/*******************************************************************************
 * smsmfSegment.c
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
#include <smg.h>
#include <sms.h>
#include <smf.h>
#include <smsDef.h>
#include <smsmfDef.h>
#include <smsmfSegment.h>
#include <smsmfScan.h>
#include <smsManager.h>

/**
 * @file smsmfSegment.c
 * @brief Storage Manager Layer Flat Segment Internal Routines
 */

/**
 * @addtogroup smsmfSegment
 * @{
 */

smsSegmentFuncs gMemoryFlatSegment =
{
    smsmfAllocHandle,  /* alloc handle */
    NULL,              /* clone handle */
    smsmfCreate,
    smsmfDrop,
    NULL,              /* insert pending drop */
    NULL,              /* reset */
    NULL,              /* alloc page */
    NULL,              /* prepare Page */
    NULL,              /* alloc multiple page */
    NULL,              /* steal pages */
    NULL,              /* get insertable page */
    NULL,              /* update page status */
    smsmfAllocSegIterator,
    smsmfFreeSegIterator,
    smsmfGetFirstPage,
    smsmfGetNextPage,
    NULL,              /* build cache */
    smsmfAllocExtent,
    NULL,              /* get header size */
    NULL,              /* get cache size */
    NULL,              /* get used page count for statistics */
    NULL,              /* shrink */
    NULL               /* SetTransToHandle */
};

stlStatus smsmfAllocHandle( smxlTransId       aTransId,
                            smlRid            aSegRid,
                            smlTbsId          aTbsId,
                            smlSegmentAttr  * aAttr,
                            stlBool           aHasRelHdr,
                            smsSegHdr       * aSegHdr,
                            void           ** aSegmentHandle,
                            smlEnv          * aEnv )
{
    stlInt32      sState = 0;
    smsmfCache  * sCache;

    STL_TRY( smgAllocShmMem4Open( STL_SIZEOF( smsmfCache ),
                                  aSegmentHandle,
                                  aEnv ) == STL_SUCCESS );
    sState = 1;

    sCache = (smsmfCache*)(*aSegmentHandle);
    
    sCache->mFirstExtFirstPid = SMP_NULL_PID;
    sCache->mCurExtFirstPid = SMP_NULL_PID;
    sCache->mCommonCache.mAllocExtCount = 0;
    sCache->mCommonCache.mIsUsable = STL_TRUE;

    smsSetSegRid( (void*)sCache, aSegRid );
    smsSetSegmentId( (void*)sCache, aSegRid );
    smsSetSegType( (void*)sCache, SML_SEG_TYPE_MEMORY_FLAT );
    smsSetTbsId( (void*)sCache, aTbsId );
    smsSetHasRelHdr( (void*)sCache, aHasRelHdr );
    smsSetUsablePageCountInExt( (void*)sCache, smsGetPageCountInTempTbsExt( aTbsId ) );

    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smgFreeShmMem4Open( *aSegmentHandle, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smsmfCreate( smxlTransId    aTransId,
                       smlTbsId       aTbsId,
                       void         * aSegmentHandle,
                       stlInt32       aInitialExtents,
                       smlEnv       * aEnv )
{
    smsmfCache  * sCache;

    sCache = (smsmfCache *)(aSegmentHandle);
    sCache->mFirstExtFirstPid = SMP_NULL_PID;
    sCache->mCurExtFirstPid = SMP_NULL_PID;
    sCache->mCommonCache.mAllocExtCount = 0;

    return STL_SUCCESS;
}

stlStatus smsmfDrop( smxlTransId   aTransId,
                     smlTbsId      aTbsId,
                     void        * aSegmentHandle,
                     smlEnv      * aEnv )
{
    smsmfCache     * sCache;
    smxmTrans        sMiniTrans;
    stlInt32         sState = 0;
    smpHandle        sPageHandle;
    smlPid           sExtFirstPid;
    smlPid           sNextExtFirstPid;
    smsmfExtHdr    * sExtHdr;
    stlBool          sIsSuccess;
    smlExtId         sExtId;
#ifdef STL_DEBUG
    smlPid           sLastFreedExtFirstPid;
#endif

    sCache = (smsmfCache*)aSegmentHandle;
    sExtFirstPid = sCache->mFirstExtFirstPid;

#ifdef STL_DEBUG
    sLastFreedExtFirstPid = sExtFirstPid;
#endif

    while( sExtFirstPid != SMP_NULL_PID )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            SML_INVALID_TRANSID,
                            SML_INVALID_RID,
                            SMXM_ATTR_NOLOG_DIRTY | SMXM_ATTR_NO_VALIDATE_PAGE,
                            aEnv ) == STL_SUCCESS );
        sState = 1;

#ifdef STL_DEBUG
        sLastFreedExtFirstPid = sExtFirstPid;
#endif

        STL_TRY( smpFix( aTbsId,
                         sExtFirstPid,
                         &sPageHandle,
                         aEnv )
                 == STL_SUCCESS );

        sExtHdr = (smsmfExtHdr *)smpGetBody( SMP_FRAME( &sPageHandle ) );
        sNextExtFirstPid = sExtHdr->mNextExtFirstPid;
        sExtId = sExtHdr->mExtId;

        STL_TRY( smpUnfix( &sPageHandle,
                           aEnv ) == STL_SUCCESS );

        sIsSuccess = STL_TRUE;
        STL_TRY( smtFreeExtent( &sMiniTrans,
                                aTbsId,
                                sExtId,
                                &sIsSuccess,
                                aEnv ) == STL_SUCCESS );

        if( sIsSuccess == STL_TRUE )
        {
            sExtFirstPid = sNextExtFirstPid;
        }
        else
        {
            /**
             * 잠시 쉬었다가 다시 시도한다.
             */
            stlYieldThread();
        }

        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    }

    STL_DASSERT( sCache->mCurExtFirstPid == sLastFreedExtFirstPid );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smxmRollback( &sMiniTrans, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smsmfAllocExtent( smxlTransId   aTransId,
                            smlTbsId      aTbsId,
                            void        * aCache,
                            smpPageType   aPageType,
                            smlPid      * aFirstDataPid,
                            smlEnv      * aEnv )
{
    smsmfCache  * sCache;
    smlExtId      sExtId;
    smlPid        sFirstPid;
    smxmTrans     sMiniTrans;
    stlInt32      sState = 0;
    smpHandle     sPageHandle;
    smsmfExtHdr  *sNewExtHdr;
    smsmfExtHdr  *sCurExtHdr;

    sCache = (smsmfCache*)aCache;

    STL_TRY( smxmBegin( &sMiniTrans,
                        SML_INVALID_TRANSID,
                        SML_INVALID_RID,
                        SMXM_ATTR_NOLOG_DIRTY | SMXM_ATTR_NO_VALIDATE_PAGE,
                        aEnv ) == STL_SUCCESS );
    sState = 1;
   
    STL_TRY( smtAllocExtent( &sMiniTrans,
                             aTbsId,
                             &sExtId,
                             &sFirstPid,
                             aEnv ) == STL_SUCCESS );

    sCache->mCommonCache.mAllocExtCount++;

    STL_TRY( smpFix( aTbsId,
                     sFirstPid,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );

    sNewExtHdr = (smsmfExtHdr *)smpGetBody( SMP_FRAME( &sPageHandle ) );
    sNewExtHdr->mNextExtFirstPid = SMP_NULL_PID;
    sNewExtHdr->mExtId = sExtId;
   
    STL_TRY( smpUnfix( &sPageHandle,
                       aEnv ) == STL_SUCCESS );

    if( sCache->mCurExtFirstPid == SMP_NULL_PID )
    {
        sCache->mFirstExtFirstPid = sFirstPid;
    }
    else
    {
        STL_TRY( smpFix( aTbsId,
                         sCache->mCurExtFirstPid,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

        sCurExtHdr = (smsmfExtHdr *)smpGetBody( SMP_FRAME( &sPageHandle ) );
        sCurExtHdr->mNextExtFirstPid = sFirstPid;

        STL_TRY( smpUnfix( &sPageHandle,
                           aEnv ) == STL_SUCCESS );
    }
    sCache->mCurExtFirstPid = sFirstPid;

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    *aFirstDataPid = sFirstPid;
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smxmRollback( &sMiniTrans, aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/** @} */
