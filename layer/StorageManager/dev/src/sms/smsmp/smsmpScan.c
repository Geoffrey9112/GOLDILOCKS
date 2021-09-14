/*******************************************************************************
 * smsmpScan.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smsmpScan.c 2584 2011-12-05 07:43:39Z xcom73 $
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
#include <smsmpDef.h>
#include <smsmpScan.h>

/**
 * @file smsmpScan.c
 * @brief Storage Manager Layer Memory Pending Full Scan Internal Routines
 */

/**
 * @addtogroup smsmpScan
 * @{
 */

stlStatus smsmpAllocSegIterator( smlStatement  * aStatement,
                                 void         ** aSegIterator,
                                 smlEnv        * aEnv )
{
    STL_TRY( smaAllocRegionMem( aStatement,
                                STL_SIZEOF( smsmpSegIterator ),
                                aSegIterator,
                                aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmpFreeSegIterator( void   * aSegIterator,
                                smlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smsmpGetFirstPage( smlTbsId      aTbsId,
                             void        * aSegHandle,
                             void        * aSegIterator,
                             smlPid      * aPageId,
                             smlEnv      * aEnv )
{
    smsmpCache       * sCache;
    smsmpSegIterator * sSegIterator;

    sSegIterator = (smsmpSegIterator*)aSegIterator;
    sCache = (smsmpCache*)aSegHandle;

    sSegIterator->mCurDataPid = sCache->mFirstHdrPid + 1;
    sSegIterator->mHdrPid = sCache->mFirstHdrPid;
    sSegIterator->mEndDataPid = sCache->mCurDataPid;
    sSegIterator->mEndOfScan = STL_FALSE;

    if( sCache->mCurDataPid == SMP_NULL_PID )
    {
        /**
         * create 또는 reset된 이후에 사용된적이 없는 경우
         */
        *aPageId = SMP_NULL_PID;
    }
    else
    {
        *aPageId = sSegIterator->mCurDataPid;
        
        if( sSegIterator->mCurDataPid == sSegIterator->mEndDataPid )
        {
            /**
             * 해당 페이지까지 검색되어야 한다.
             * - 다음 fetchnext시 종료되어야 함.
             */
            sSegIterator->mEndOfScan = STL_TRUE;
        }
    }
    
    return STL_SUCCESS;
}
    
stlStatus smsmpGetNextPage( smlTbsId      aTbsId,
                            void        * aSegIterator,
                            smlPid      * aPageId,
                            smlEnv      * aEnv )
{
    smsmpSegIterator * sSegIterator;
    smsmpExtHdr      * sExtHdr;
    smpHandle          sPageHandle;
    stlUInt16          sPageCountInExt;

    sSegIterator = (smsmpSegIterator*)aSegIterator;
    sPageCountInExt = smfGetPageCountInExt( aTbsId );

    if( sSegIterator->mEndOfScan == STL_TRUE )
    {
        *aPageId = SMP_NULL_PID;
        STL_THROW( RAMP_EXIT_FUNCTION );
    }

    if( (sSegIterator->mCurDataPid - sSegIterator->mHdrPid + 1) >= sPageCountInExt )
    {
        /*
         * 현재 Extent에서 마지막 Page까지 도달했으므로, 다음 Extent를 검사
         */
        STL_TRY( smpFix( aTbsId,
                         sSegIterator->mHdrPid,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

        sExtHdr = (smsmpExtHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

        sSegIterator->mHdrPid = sExtHdr->mNextHdrPid;
        sSegIterator->mCurDataPid = sExtHdr->mNextHdrPid + 1;

        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

        if( sSegIterator->mHdrPid == SMP_NULL_PID )
        {
            /*
             * 더 이상 연결된 Extent가 없음
             */
            *aPageId = SMP_NULL_PID;
            STL_THROW( RAMP_EXIT_FUNCTION );
        }
    }
    else
    {
        sSegIterator->mCurDataPid++;
    }

    *aPageId = sSegIterator->mCurDataPid;

    /**
     * CurDataPid가 extent의 last page라면 종료조건을 EndDataPid로
     * 검사해야 한다.
     */
    if( sSegIterator->mCurDataPid == sSegIterator->mEndDataPid )
    {
        /**
         * 해당 페이지까지 검색되어야 한다.
         * - 다음 fetchnext시 종료되어야 함.
         */
        sSegIterator->mEndOfScan = STL_TRUE;
    }

    STL_RAMP( RAMP_EXIT_FUNCTION );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
