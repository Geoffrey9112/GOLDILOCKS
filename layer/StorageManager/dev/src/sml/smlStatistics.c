/*******************************************************************************
 * smlStatistics.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smlStatistics.c 6947 2013-01-14 09:16:33Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <dtl.h>
#include <sml.h>
#include <sme.h>
#include <smn.h>
#include <sma.h>
#include <smd.h>
#include <smxl.h>
#include <smf.h>
#include <sms.h>

/**
 * @file smlStatistics.c
 * @brief Storage Manager Layer Statistics Routines
 */

/**
 * @addtogroup smlStatistics
 * @{
 */

/**
 * @brief 주어진 relation의 사용중인 페이지 개수를 반환한다.
 * @param[in]       aRelationHandle     검사할 Relation Handle
 * @param[out]      aUsedPageCount      사용중인 페이지 개수
 * @param[in,out]   aEnv                Environment 구조체
 */
stlStatus smlGetUsedPageCount( void     * aRelationHandle,
                               stlInt64 * aUsedPageCount,
                               smlEnv   * aEnv )
{
    void  * sSegmentHandle;
    
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( aRelationHandle );

    STL_DASSERT( sSegmentHandle );

    return smsGetUsedPageCount4Statistics( sSegmentHandle,
                                           aUsedPageCount,
                                           aEnv );
}

/** @} */
