/*******************************************************************************
 * smsMap.c
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
#include <smxm.h>
#include <sms.h>
#include <smp.h>
#include <smg.h>
#include <smsDef.h>
#include <smsMap.h>
#include <smsManager.h>


extern smsWarmupEntry * gSmsWarmupEntry;
extern smsSegmentFuncs * gSegmentFuncs[];
/**
 * @file smsMap.c
 * @brief Storage Manager Layer Segment Map Internal Routines
 */

/**
 * @addtogroup sms
 * @{
 */

/**
 * @brief Segment Map 세그먼트를 생성한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in,out] aEnv Environment 정보
 * @note Database 생성시에만 호출되는 함수.
 */
stlStatus smsMapCreate( smxlTransId   aTransId,
                        smlEnv      * aEnv )
{
    smlSegType       sSegType;
    void           * sSegHandle = NULL;
    smxmTrans        sMiniTrans;
    smlRid           sSegMapRid;
    stlInt64         sSegId;
    smpHandle        sPageHandle;
    stlInt32         sState = 0;
    smlSegmentAttr   sAttr;

    sSegType = SMS_SEGMENT_MAP_SEGMENT_TYPE;
    sSegMapRid = SMS_SEGMENT_MAP_RID;
    SMS_MAKE_SEGMENT_ID( &sSegId, &sSegMapRid );

    sAttr.mValidFlags = 0;
    
    /**
     * @todo 아래 확인 필요
     * sAttr.mValidFlags |= SML_TABLE_LOCKING_YES;
     * sAttr.mLockingFlag = STL_FALSE;
     */ 
    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        sSegMapRid,
                        SMXM_ATTR_REDO,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpCreate( &sMiniTrans,
                        sSegMapRid.mTbsId,
                        sSegMapRid.mPageId,
                        SMP_PAGE_TYPE_BITMAP_HEADER,
                        sSegId, /* aSegmentId */
                        0, /* aScn */
                        &sPageHandle,
                        aEnv ) == STL_SUCCESS );
    
    STL_TRY( gSegmentFuncs[sSegType]->mAllocHandle( aTransId,
                                                    sSegMapRid,
                                                    sSegMapRid.mTbsId,
                                                    &sAttr,
                                                    STL_FALSE,
                                                    NULL,
                                                    &sSegHandle,
                                                    aEnv )
             == STL_SUCCESS );
    
    smpSetVolatileArea( &sPageHandle,
                        (void*)&sSegHandle,
                        STL_SIZEOF(void*) );
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    STL_TRY( gSegmentFuncs[sSegType]->mCreate( aTransId,
                                               sSegMapRid.mTbsId,
                                               sSegHandle,
                                               0, /* aInitialExtents(optimal) */
                                               aEnv )
             == STL_SUCCESS );
    
    gSmsWarmupEntry->mSegmentMapHandle = sSegHandle;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmRollback( &sMiniTrans, aEnv );
    }

    if( sSegHandle != NULL )
    {
        (void)smgFreeShmMem4Open( sSegHandle, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Segment Rid로부터 Segment Handle을 얻는다.
 * @param[in] aSegRid Target Segment Rid
 * @param[out] aSegHandle 얻은 Segment Handle
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsGetSegmentHandle( smlRid    aSegRid,
                               void   ** aSegHandle,
                               smlEnv  * aEnv )
{
    smpHandle   sPageHandle;
    void      * sVolatileArea;
    stlInt32    sState = 0;
    
    STL_TRY( smpFix( aSegRid.mTbsId,
                     aSegRid.mPageId,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;

    sVolatileArea = smpGetVolatileArea( &sPageHandle );

    *aSegHandle = *(stlChar**)sVolatileArea;
    
    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Segment Map의 Handle을 얻는다.
 * @note smsBuildSegmentCache 이후에 호출되어야 함.
 */
void * smsGetSegmentMapHandle()
{
    return gSmsWarmupEntry->mSegmentMapHandle;
}

/**
 * @brief Segment Map Scan을 위한 Iterator를 할당받는다.
 * @param[in]     aStatement   Statement Pointer
 * @param[out]    aIterator    Iterator
 * @param[in,out] aEnv         Environment 정보
 */
stlStatus smsAllocSegMapIterator( smlStatement  * aStatement,
                                  void         ** aIterator,
                                  smlEnv        * aEnv )
{
    STL_TRY( smsAllocSegIterator( aStatement,
                                  gSmsWarmupEntry->mSegmentMapHandle,
                                  aIterator,
                                  aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Segment Map Scan을 위한 Iterator를 해제한다.
 * @param[in]     aIterator    Iterator
 * @param[in,out] aEnv         Environment 정보
 */
stlStatus smsFreeSegMapIterator( void   * aIterator,
                                 smlEnv * aEnv )
{
    STL_TRY( smsFreeSegIterator( gSmsWarmupEntry->mSegmentMapHandle,
                                 aIterator,
                                 aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세그먼트 MAP에 할당된 첫번째 Segment를 얻는다.
 * @param[in]     aIterator       Iterator
 * @param[out]    aSegmentHandle  Segment Handle
 * @param[in]     aSegmentRid     Setment RID
 * @param[in,out] aEnv            Environment 정보
 */
stlStatus smsGetFirstSegment( void    * aIterator,
                              void   ** aSegmentHandle,
                              smlRid  * aSegmentRid,
                              smlEnv  * aEnv )
{
    smlPid   sPageId = SMP_NULL_PID;
    void   * sSegmentHandle = NULL;
    smlRid   sSegmentRid = {0,0,sPageId};
    
    STL_TRY( smsGetFirstPage( gSmsWarmupEntry->mSegmentMapHandle,
                              aIterator,
                              &sPageId,
                              SML_ENV( aEnv ) ) == STL_SUCCESS);

    while( sPageId != SMP_NULL_PID )
    {
        sSegmentRid = (smlRid){0,0,sPageId};
        
        STL_TRY( smsGetSegmentHandle( sSegmentRid,
                                      &sSegmentHandle,
                                      SML_ENV( aEnv ) ) == STL_SUCCESS );

        if( sSegmentHandle == NULL )
        {
            STL_TRY( smsGetNextPage( gSmsWarmupEntry->mSegmentMapHandle,
                                     aIterator,
                                     &sPageId,
                                     SML_ENV( aEnv ) ) == STL_SUCCESS);
        }
        else
        {
            break;
        }
    }

    *aSegmentRid = sSegmentRid;
    *aSegmentHandle = sSegmentHandle;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세그먼트 MAP에 할당된 Next Segment를 얻는다.
 * @param[in]     aIterator       Iterator
 * @param[out]    aSegmentHandle  Segment Handle
 * @param[in]     aSegmentRid     Setment RID
 * @param[in,out] aEnv            Environment 정보
 */
stlStatus smsGetNextSegment( void    * aIterator,
                             void   ** aSegmentHandle,
                             smlRid  * aSegmentRid,
                             smlEnv  * aEnv )
{
    smlPid   sPageId = SMP_NULL_PID;
    void   * sSegmentHandle = NULL;
    smlRid   sSegmentRid = {0,0,sPageId};
    
    STL_TRY( smsGetNextPage( gSmsWarmupEntry->mSegmentMapHandle,
                             aIterator,
                             &sPageId,
                             SML_ENV( aEnv ) ) == STL_SUCCESS);
    
    while( sPageId != SMP_NULL_PID )
    {
        sSegmentRid = (smlRid){0,0,sPageId};
        
        STL_TRY( smsGetSegmentHandle( sSegmentRid,
                                      &sSegmentHandle,
                                      SML_ENV( aEnv ) ) == STL_SUCCESS );

        if( sSegmentHandle == NULL )
        {
            STL_TRY( smsGetNextPage( gSmsWarmupEntry->mSegmentMapHandle,
                                     aIterator,
                                     &sPageId,
                                     SML_ENV( aEnv ) ) == STL_SUCCESS);
        }
        else
        {
            break;
        }
    }

    *aSegmentRid = sSegmentRid;
    *aSegmentHandle = sSegmentHandle;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
