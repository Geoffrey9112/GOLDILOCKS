/*******************************************************************************
 * smlAggr.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smlAggr.c 6947 2013-01-14 09:16:33Z mkkim $
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
 * @file smlAggr.c
 * @brief Storage Manager Layer Aggr Routines
 */

/**
 * @addtogroup smlAggr
 * @{
 */

/**
 * @brief Aggregation을 위한 스캔을 수행한다.
 * @param[in]       aIterator       Iterator 구조체
 * @param[in,out]   aAggrFetchInfo  Aggregation Fetch 정보
 * @param[in,out]   aEnv            Environment 구조체
 */
stlStatus smlFetchAggr( void          * aIterator,
                        smlFetchInfo  * aAggrFetchInfo,
                        smlEnv        * aEnv )
{
    smiIterator * sIterator = (smiIterator*)aIterator;

    STL_DASSERT( smlEnterStorageManager( aEnv ) == STL_SUCCESS );
    
    STL_TRY_THROW( sIterator->mState != SMI_STATE_CLOSED,
                   RAMP_ERR_FETCH_SEQUENCE );

    /**
     * Event 검사
     */
    STL_TRY( knlCheckEndSession( KNL_SESS_ENV(aEnv), KNL_ENV(aEnv) ) == STL_SUCCESS );
    STL_TRY( knlCheckEndStatement( KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( sIterator->mFetchAggr( aIterator,
                                    aAggrFetchInfo,
                                    aEnv )
             == STL_SUCCESS );

    STL_DASSERT( smlLeaveStorageManager( STL_FALSE, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_FETCH_SEQUENCE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_FETCH_SEQUENCE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }
    
    STL_FINISH;

    STL_DASSERT( smlLeaveStorageManager( STL_TRUE, aEnv ) == STL_SUCCESS );
    
    return STL_FAILURE;
}

/** @} */
