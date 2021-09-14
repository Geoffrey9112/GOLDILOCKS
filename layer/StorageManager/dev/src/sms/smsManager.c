
/*******************************************************************************
 * smsManager.c
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
#include <sma.h>
#include <smxl.h>
#include <smf.h>
#include <smt.h>
#include <smsDef.h>
#include <smsManager.h>


extern smsWarmupEntry * gSmsWarmupEntry;
extern smsSegmentFuncs gMemoryCircularSegment;
extern smsSegmentFuncs gMemoryBitmapSegment;
extern smsSegmentFuncs gMemoryFlatSegment;
extern smsSegmentFuncs gMemoryPendingSegment;

smsSegmentFuncs * gSegmentFuncs[SML_SEG_TYPE_MAX] =
{
    &gMemoryBitmapSegment,
    &gMemoryCircularSegment,
    &gMemoryFlatSegment,
    &gMemoryPendingSegment,
    NULL,
    NULL
};

/**
 * @file smsManager.c
 * @brief Storage Manager Layer Segment Manager Internal Routines
 */

/**
 * @addtogroup sms
 * @{
 */

/**
 * @brief 새로운 세그먼트 헤더를 할당한다.
 * @param[in]     aMiniTrans   Mini-Transaction
 * @param[in]     aTbsId       Tablespace Identifier
 * @param[in]     aSegType     Segment Type
 * @param[in]     aAttr        Segment 속성
 * @param[in]     aHasRelHdr   relation header를 가지는지 여부
 * @param[out]    aSegRid      생성된 Segment Rid
 * @param[out]    aSegHandle   생성된 Segment Handle
 * @param[in,out] aEnv         Environment 정보
 */
stlStatus smsAllocSegHdr( smxmTrans       * aMiniTrans,
                          smlTbsId          aTbsId,
                          smlSegType        aSegType,
                          smlSegmentAttr  * aAttr,
                          stlBool           aHasRelHdr,
                          smlRid          * aSegRid,
                          void           ** aSegHandle,
                          smlEnv          * aEnv )
{
    smsCommonCache  * sSegHandle = NULL;
    smpHandle         sPageHandle;
    smlRid            sSegRid;
    smlRid            sSegMapRid;
    smsSegHdr       * sSegHdr = NULL;

    STL_PARAM_VALIDATE( gSegmentFuncs[aSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[aSegType]->mAllocHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegMapRid = SMS_SEGMENT_MAP_RID;

    if( SMS_IS_NEED_SEGMENT_HDR(aSegType) )
    {
        STL_TRY( gSegmentFuncs[SMS_SEGMENT_MAP_SEGMENT_TYPE]->mAllocPage(
                     aMiniTrans,
                     sSegMapRid.mTbsId,
                     gSmsWarmupEntry->mSegmentMapHandle,
                     SMP_PAGE_TYPE_BITMAP_HEADER,
                     NULL, /* aIsAgableFunc */
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );

        SML_MAKE_RID( &sSegRid,
                      sSegMapRid.mTbsId,
                      smpGetPageId( &sPageHandle ),
                      0 );

        sSegHdr = (smsSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

        STL_TRY( smsInitSegHdr( aMiniTrans,
                                &sPageHandle,
                                aSegType,
                                aTbsId,
                                aHasRelHdr,
                                sSegHdr,
                                aEnv )
                 == STL_SUCCESS );
    }
    else
    {
        sSegRid = SML_INVALID_RID;
        sPageHandle.mPch = NULL;
    }
    
    STL_TRY( gSegmentFuncs[aSegType]->mAllocHandle( smxmGetTransId(aMiniTrans),
                                                    sSegRid,
                                                    aTbsId,
                                                    aAttr,
                                                    aHasRelHdr,
                                                    sSegHdr,
                                                    (void**)&sSegHandle,
                                                    aEnv )
             == STL_SUCCESS );
 
    if( sPageHandle.mPch != NULL )
    {
        smpSetVolatileArea( &sPageHandle,
                            (void*)&sSegHandle,
                            STL_SIZEOF(void*) );
    }

    *aSegHandle = sSegHandle;
    *aSegRid = sSegRid;

    return STL_SUCCESS;
    
    STL_FINISH;

    if( sSegHandle != NULL )
    {
        (void)smgFreeShmMem4Open( sSegHandle, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Truncate 연산을 위해서 새로운 세그먼트 헤더를 할당한다.
 * @param[in]     aMiniTrans    Mini-Transaction
 * @param[in]     aOrgSegHandle Tablespace Identifier
 * @param[out]    aNewSegRid    생성된 Segment Rid
 * @param[out]    aNewSegHandle 생성된 Segment Handle
 * @param[in,out] aEnv          Environment 정보
 */
stlStatus smsAllocSegHdrForTruncate( smxmTrans   * aMiniTrans,
                                     void        * aOrgSegHandle,
                                     smlRid      * aNewSegRid,
                                     void       ** aNewSegHandle,
                                     smlEnv      * aEnv )
{
    smsCommonCache  * sNewSegHandle = NULL;
    smpHandle         sPageHandle;
    smlRid            sSegRid;
    smlRid            sSegMapRid;
    smlSegType        sSegType;
    smsSegHdr       * sSegHdr;

    sSegType = smsGetSegType( aOrgSegHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mCloneHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegMapRid = SMS_SEGMENT_MAP_RID;
    
    STL_TRY( gSegmentFuncs[SMS_SEGMENT_MAP_SEGMENT_TYPE]->mAllocPage(
                 aMiniTrans,
                 sSegMapRid.mTbsId,
                 gSmsWarmupEntry->mSegmentMapHandle,
                 SMP_PAGE_TYPE_BITMAP_HEADER,
                 NULL, /* aIsAgableFunc */
                 &sPageHandle,
                 aEnv ) == STL_SUCCESS );

    SML_MAKE_RID( &sSegRid,
                  sSegMapRid.mTbsId,
                  smpGetPageId( &sPageHandle ),
                  0 );

    sSegHdr = (smsSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

    STL_TRY( smsInitSegHdr( aMiniTrans,
                            &sPageHandle,
                            sSegType,
                            smsGetTbsId(aOrgSegHandle),
                            smsHasRelHdr(aOrgSegHandle),
                            sSegHdr,
                            aEnv )
             == STL_SUCCESS );
    
    STL_TRY( gSegmentFuncs[sSegType]->mCloneHandle( smxmGetTransId(aMiniTrans),
                                                    sSegRid,
                                                    aOrgSegHandle,
                                                    sSegHdr,
                                                    (void**)&sNewSegHandle,
                                                    aEnv )
             == STL_SUCCESS );
    
    smpSetVolatileArea( &sPageHandle,
                        (void*)&sNewSegHandle,
                        STL_SIZEOF(void*) );

    *aNewSegHandle = sNewSegHandle;
    *aNewSegRid = sSegRid;

    return STL_SUCCESS;
    
    STL_FINISH;

    if( sNewSegHandle != NULL )
    {
        (void)smgFreeShmMem4Open( sNewSegHandle, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 세그먼트 헤더를 삭제한다.
 * @param[in]     aTransId    Transaction Identifier
 * @param[in]     aSegHandle  삭제할 Segment Handle
 * @param[in,out] aEnv        Environment 정보
 */
stlStatus smsFreeSegHdr( smxlTransId   aTransId,
                         void        * aSegHandle,
                         smlEnv      * aEnv )
{
    stlInt64    sData[2];
    smxmTrans   sMiniTrans;
    stlUInt32   sAttr = SMXM_ATTR_REDO;
    smlRid      sSegmentRid;
    smpHandle   sPageHandle;
    smsSegHdr * sSegHdr;
    stlInt16    sOffset;
    stlUInt32   sState = 0;
    void      * sNullHandle = NULL;
    
    if( SMS_IS_NEED_SEGMENT_HDR( smsGetSegType( aSegHandle ) ) )
    {
        sSegmentRid = smsGetSegRid( aSegHandle );
        
        STL_TRY( smxmBegin( &sMiniTrans,
                            aTransId,
                            sSegmentRid,
                            sAttr,
                            aEnv ) == STL_SUCCESS );
        sState = 1;
    
        STL_TRY( smpAcquire( &sMiniTrans,
                             sSegmentRid.mTbsId,
                             sSegmentRid.mPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        
        sSegHdr = (smsSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

        sData[0] = sSegHdr->mValidSeq;
        sSegHdr->mValidSeq += 1;
        sData[1] = sSegHdr->mValidSeq;
        
        sOffset = STL_DIFF_POINTER( &(sSegHdr->mValidSeq), SMP_FRAME(&sPageHandle) );

        STL_TRY( smxmWriteLog( &sMiniTrans,
                               SMG_COMPONENT_RECOVERY,
                               SMR_LOG_BYTES,
                               SMR_REDO_TARGET_PAGE,
                               (void*)sData,
                               STL_SIZEOF( stlInt64 ) * 2,
                               sSegmentRid.mTbsId,
                               sSegmentRid.mPageId,
                               sOffset,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    
        smpSetVolatileArea( &sPageHandle,
                            (void*)&sNullHandle,
                            STL_SIZEOF(void*) );

        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
        
        STL_TRY( smsFreeSegMapPage( aTransId,
                                    smsGetSegRid( aSegHandle ),
                                    aEnv ) == STL_SUCCESS );
    }
    
    (void)smgFreeShmMem4Open( aSegHandle, aEnv );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void) smxmRollback( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 세그먼트를 Reset한다.
 * @param[in]     aTransId    Transaction Identifier
 * @param[in]     aSegHandle  삭제할 Segment Handle
 * @param[in]     aFreePages  기존 페이지 삭제 여부
 * @param[in,out] aEnv        Environment 정보
 * @remarks segment cache를 초기화하고, segment에 할달되어 있는 모든 페이지를 삭제한다.
 */
stlStatus smsReset( smxlTransId   aTransId,
                    void        * aSegHandle,
                    stlBool       aFreePages,
                    smlEnv      * aEnv )
{
    smlSegType  sSegType;
    smlTbsId    sTbsId;
    
    sSegType = smsGetSegType( aSegHandle );
    sTbsId = smsGetTbsId( aSegHandle );

    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mReset != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    STL_TRY( gSegmentFuncs[sSegType]->mReset( aTransId,
                                              sTbsId,
                                              aSegHandle,
                                              aFreePages,
                                              aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세그먼트를 생성한다.
 * @param[in] aTransId          Transaction Identifier
 * @param[in] aTbsId            Tablespace Identifier
 * @param[in] aSegType          Segment Type
 * @param[in] aSegHandle        Segment Handle
 * @param[in] aInitialExtent    초기에 할당된 extent의 개수를 지정한다.
 * @param[in,out] aEnv          Environment 정보
 * @remarks
 *     aIntialExtent가 0 인 경우는 optimal 값이 적용된다.
 */
stlStatus smsCreate( smxlTransId   aTransId,
                     smlTbsId      aTbsId,
                     smlSegType    aSegType,
                     void        * aSegHandle,
                     stlInt64      aInitialExtent,
                     smlEnv      * aEnv )
{
    STL_PARAM_VALIDATE( gSegmentFuncs[aSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[aSegType]->mCreate != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    STL_TRY( gSegmentFuncs[aSegType]->mCreate( aTransId,
                                               aTbsId,
                                               (void*)aSegHandle,
                                               aInitialExtent,
                                               aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    /**
     * nothing to do 
     * - Create 도중 실패하는 경우는 relation component undo logging으로
     *   Undo 된다.
     */

    return STL_FAILURE;
}

/**
 * @brief 세그먼트에 삭제 표시를 한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in] aSegmentHandle 삭제할 Segment의 Segment Handle
 * @param[in] aOnStartup Startup 중인지 여부
 * @param[in,out] aEnv Environment 정보
 * @note 세그먼트의 물리적 삭제는 Ager에 의해서 처리된다.
 */
stlStatus smsDrop( smxlTransId   aTransId,
                   void        * aSegmentHandle,
                   stlBool       aOnStartup,
                   smlEnv      * aEnv )
{
    smlSegType sSegType;
    smlTbsId   sTbsId;

    STL_PARAM_VALIDATE( aSegmentHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegType = smsGetSegType( aSegmentHandle );
    sTbsId = smsGetTbsId( aSegmentHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mDrop != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    if( smfIsValidTbs( sTbsId ) == STL_TRUE )
    {
        /**
         * Restart 단계에서 No Logging Segment는 Extent를 해제할 필요없다.
         */

        if( (smtIsLogging( sTbsId ) == STL_TRUE) || (aOnStartup == STL_FALSE) )
        {
            if( (smfGetTbsState( sTbsId ) != SMF_TBS_STATE_AGING) &&
                (smfGetTbsState( sTbsId ) != SMF_TBS_STATE_DROPPED) )
            {
                if( SMF_IS_ONLINE_TBS( sTbsId ) == STL_TRUE )
                {
                    STL_TRY( gSegmentFuncs[sSegType]->mDrop( aTransId,
                                                             sTbsId,
                                                             aSegmentHandle,
                                                             aEnv )
                             == STL_SUCCESS );
                }
                else
                {
                    /**
                     * Offline Tablespace는 pending operation으로 처리한다.
                     * - Segment Header Page만 삭제한다.
                     */
                }
            }
        }
    }

    STL_TRY( smsFreeSegHdr( aTransId,
                            aSegmentHandle,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세그먼트 BODY를 삭제한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in] aSegmentHandle 삭제할 Segment의 Segment Handle
 * @param[in,out] aEnv Environment 정보
 * @note 세그먼트의 물리적 삭제는 Ager에 의해서 처리된다.
 * @todo 현재는 물리적 삭제를 하지만 Ager 개발 이후에 Ager에 의해서 처리되어야함.
 */
stlStatus smsDropBody( smxlTransId   aTransId,
                       void        * aSegmentHandle,
                       smlEnv      * aEnv )
{
    smlSegType sSegType;
    smlTbsId   sTbsId;

    STL_PARAM_VALIDATE( aSegmentHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegType = smsGetSegType( aSegmentHandle );
    sTbsId = smsGetTbsId( aSegmentHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mDrop != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    STL_TRY( gSegmentFuncs[sSegType]->mDrop( aTransId,
                                             sTbsId,
                                             aSegmentHandle,
                                             aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Offline Tablespace에서 Online을 변경시 반영하지 못했던 Drop연산을 수행한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in] aSegmentHandle 삭제할 Segment의 Segment Handle
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsPendingDrop( smxlTransId   aTransId,
                          void        * aSegmentHandle,
                          smlEnv      * aEnv )
{
    return smsDropBody( aTransId,
                        aSegmentHandle,
                        aEnv );
}

/**
 * @brief 세그먼트로부터 페이지 한개를 할당받는다.
 * @param[in] aMiniTrans Mini-transaction Pointer
 * @param[in] aSegmentHandle Target Segment Handle
 * @param[in] aPageType Page Type
 * @param[in] aIsAgableFunc Agable 가능 확인 Function Pointer
 * @param[out] aPageHandle 할당한 페이지의 Page Handle
 * @param[in,out] aEnv Environment 정보
 * @note 할당받은 페이지는 내부적으로 Exclusive Mode 래치가 잡혀있는 상태이다.
 */
stlStatus smsAllocPage( smxmTrans       * aMiniTrans,
                        void            * aSegmentHandle,
                        smpPageType       aPageType,
                        smpIsAgableFunc   aIsAgableFunc,
                        smpHandle       * aPageHandle,
                        smlEnv          * aEnv )
{
    smlSegType sSegType;
    smlTbsId   sTbsId;

    STL_PARAM_VALIDATE( aSegmentHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegType = smsGetSegType( aSegmentHandle );
    sTbsId = smsGetTbsId( aSegmentHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mAllocPage != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    
    STL_TRY( gSegmentFuncs[sSegType]->mAllocPage( aMiniTrans,
                                                  sTbsId,
                                                  aSegmentHandle,
                                                  aPageType,
                                                  aIsAgableFunc,
                                                  aPageHandle,
                                                  aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세그먼트로부터 가용한 공간을 확보한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in] aSegmentHandle Target Segment Handle
 * @param[in] aPageCount 요청한 페이지 개수
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsPreparePage( smxlTransId   aTransId,
                          void        * aSegmentHandle,
                          stlUInt32     aPageCount,
                          smlEnv      * aEnv )
{
    smlSegType sSegType;

    STL_PARAM_VALIDATE( aSegmentHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegType = smsGetSegType( aSegmentHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mPreparePage != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    
    STL_TRY( gSegmentFuncs[sSegType]->mPreparePage( aTransId,
                                                    aSegmentHandle,
                                                    aPageCount,
                                                    aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세그먼트로부터 페이지 N개를 할당받는다.
 * @param[in] aMiniTrans Mini-transaction Pointer
 * @param[in] aSegmentHandle Target Segment Handle
 * @param[in] aIsAgableFunc Agable 가능 확인 Function Pointer
 * @param[in] aPageType Page Type
 * @param[in] aPageCount 할당할 페이지 개수
 * @param[out] aPageHandle 할당한 페이지들의 Page Handle Array
 * @param[in,out] aEnv Environment 정보
 * @note 할당받은 페이지는 내부적으로 Exclusive Mode 래치가 잡혀있는 상태이다.
 */
stlStatus smsAllocMultiPage( smxmTrans       * aMiniTrans,
                             void            * aSegmentHandle,
                             smpPageType       aPageType,
                             stlInt32          aPageCount,
                             smpIsAgableFunc   aIsAgableFunc,
                             smpHandle       * aPageHandle,
                             smlEnv          * aEnv )
{
    smlSegType sSegType;
    smlTbsId   sTbsId;

    STL_PARAM_VALIDATE( aSegmentHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegType = smsGetSegType( aSegmentHandle );
    sTbsId = smsGetTbsId( aSegmentHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mAllocMultiPage != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    
    STL_TRY( gSegmentFuncs[sSegType]->mAllocMultiPage( aMiniTrans,
                                                       sTbsId,
                                                       aSegmentHandle,
                                                       aPageType,
                                                       aPageCount,
                                                       aIsAgableFunc,
                                                       aPageHandle,
                                                       aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세그먼트로부터 Extent 한개를 할당받는다.
 * @param[in] aTransId Transaction Identifier
 * @param[in] aSegmentHandle Target Segment Handle
 * @param[in] aPageType Page Type
 * @param[out] aFirstDataPid 할당한 Extent의 첫번째 페이지 아이디
 * @param[in,out] aEnv Environment 정보
 * @note smsAllocPage()와 smsGetInsertablePage()와 concurrent하게 사용될수 없다.
 * <BR>  extent의 할당과 페이지들의 freeness설정을 atomic하게 할수 없기 때문이다.
 * <BR>  즉, 인덱스 build와 같은 batch 작업시에만 사용될수 있다.
 */
stlStatus smsAllocExtent( smxlTransId   aTransId,
                          void        * aSegmentHandle,
                          smpPageType   aPageType,
                          smlPid      * aFirstDataPid,
                          smlEnv      * aEnv )
{
    smlSegType sSegType;
    smlTbsId   sTbsId;

    STL_PARAM_VALIDATE( aSegmentHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegType = smsGetSegType( aSegmentHandle );
    sTbsId = smsGetTbsId( aSegmentHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    
    STL_TRY( gSegmentFuncs[sSegType]->mAllocExtent( aTransId,
                                                    sTbsId,
                                                    aSegmentHandle,
                                                    aPageType,
                                                    aFirstDataPid,
                                                    aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 다른 세그먼트로부터 공간을 확보한다.
 * @param[in] aMiniTrans Mini-transaction Pointer
 * @param[in] aDstSegHandle Target Segment Handle
 * @param[in] aSrcSegHandle Source Segment Handle
 * @param[out] aStolenPageCount steal한 페이지 개수
 * @param[in,out] aEnv Environment 정보
 * @note 할당받은 페이지는 내부적으로 Exclusive Mode 래치가 잡혀있는 상태이다.
 */
stlStatus smsStealPages( smxmTrans   * aMiniTrans,
                         void        * aDstSegHandle,
                         void        * aSrcSegHandle,
                         stlInt64    * aStolenPageCount,
                         smlEnv      * aEnv )
{
    smlSegType sSegType;
    smlTbsId   sTbsId;

    STL_PARAM_VALIDATE( aDstSegHandle != NULL, KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( aSrcSegHandle != NULL, KNL_ERROR_STACK( aEnv ) );

    sSegType = smsGetSegType( aDstSegHandle );
    sTbsId = smsGetTbsId( aDstSegHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mStealPages != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    
    STL_TRY( gSegmentFuncs[sSegType]->mStealPages( aMiniTrans,
                                                   sTbsId,
                                                   aDstSegHandle,
                                                   aSrcSegHandle,
                                                   0,  /* aShrinkToPageCount */
                                                   aStolenPageCount,
                                                   aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 삽입 가능한 페이지를 얻는다.
 * @param[in] aMiniTrans Mini-transaction Pointer
 * @param[in] aSegmentHandle Target Segment Handle
 * @param[in] aPageType Page Type
 * @param[in] aIsAgableFunc Agable 가능 확인 Function Pointer
 * @param[in] aSearchHint Search를 위한 힌트정보
 * @param[out] aPageHandle 대상 페이지의 Page Handle
 * @param[out] aIsNewPage 할당된 페이지가 New Page인지 여부
 * @param[in,out] aEnv Environment 정보
 * @note 대상 페이지는 내부적으로 Exclusive Mode 래치가 잡혀있는 상태이다.
 */
stlStatus smsGetInsertablePage( smxmTrans       * aMiniTrans,
                                void            * aSegmentHandle,
                                smpPageType       aPageType,
                                smpIsAgableFunc   aIsAgableFunc,
                                smsSearchHint   * aSearchHint,
                                smpHandle       * aPageHandle,
                                stlBool         * aIsNewPage,
                                smlEnv          * aEnv )
{
    smlSegType  sSegType;
    smlTbsId    sTbsId;

    STL_PARAM_VALIDATE( aSegmentHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegType = smsGetSegType( aSegmentHandle );
    sTbsId = smsGetTbsId( aSegmentHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mGetInsertablePage != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    
    STL_TRY( gSegmentFuncs[sSegType]->mGetInsertablePage( aMiniTrans,
                                                          sTbsId,
                                                          aSegmentHandle,
                                                          aPageType,
                                                          aIsAgableFunc,
                                                          aSearchHint,
                                                          aPageHandle,
                                                          aIsNewPage,
                                                          aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 페이지의 Freeness(자유도) 상태를 변경한다.
 * @param[in] aMiniTrans Mini-transaction Pointer
 * @param[in] aSegmentHandle Target Segment Handle
 * @param[in] aPageHandle 대상 페이지의 Page Handle
 * @param[in] aFreeness 변경할 Freeness
 * @param[in] aScn Freeness의 유효성 기간
 * @param[in,out] aEnv Environment 정보
 * @remark aScn은 aFreeness가 SMP_FREENESS_FREE일때만 유효하다.
 * @note 대상 페이지는 Fix(or Acquire)가 되어 있는 상태여야 한다.
 */
stlStatus smsUpdatePageStatus( smxmTrans   * aMiniTrans,
                               void        * aSegmentHandle,
                               smpHandle   * aPageHandle,
                               smpFreeness   aFreeness,
                               smlScn        aScn,
                               smlEnv      * aEnv )
{
    smlSegType sSegType;
    smlTbsId   sTbsId;

    STL_PARAM_VALIDATE( aSegmentHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegType = smsGetSegType( aSegmentHandle );
    sTbsId = smsGetTbsId( aSegmentHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mUpdatePageStatus != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    
    STL_TRY( gSegmentFuncs[sSegType]->mUpdatePageStatus( aMiniTrans,
                                                         sTbsId,
                                                         aPageHandle,
                                                         aFreeness,
                                                         aScn,
                                                         aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세그먼트의 Attribute를 변경한다.
 * @param[in] aTransId Transaction Identifier
 * @param[in] aSegmentHandle Segment Handle
 * @param[in] aSegmentAttr 변경될 Segment Attribute
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsAlterSegmentAttr( smxlTransId      aTransId,
                               void           * aSegmentHandle,
                               smlSegmentAttr * aSegmentAttr,
                               smlEnv         * aEnv )
{
    smxmTrans             sMiniTrans;
    stlUInt32             sAttr = SMXM_ATTR_REDO;
    stlUInt32             sState = 0;
    smlRid                sSegmentRid;
    smpHandle             sPageHandle;
    smsSegHdr           * sSegmentHeader;
    smlRid                sUndoRid;
    smlSegmentAttr        sSegmentAttr;
    stlUInt16             sPageCountInExt;
    smsCommonCache      * sCache;
    stlInt64              sBytes;
    stlInt32              sExtents;

    sCache = (smsCommonCache*)aSegmentHandle;
    sSegmentRid = smsGetSegRid( aSegmentHandle );
    sPageCountInExt = smfGetPageCountInExt( smsGetTbsId(aSegmentHandle) );

    sSegmentAttr.mValidFlags = 0;
    smsMergeSegmentAttr( aSegmentHandle, &sSegmentAttr, &sSegmentAttr );
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        sSegmentRid,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( smpAcquire( &sMiniTrans,
                         sSegmentRid.mTbsId,
                         sSegmentRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sSegmentHeader = (smsSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

    /**
     * logging undo log
     */
    
    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_SEGMENT,
                                   SMS_UNDO_LOG_ALTER_SEGMENT_HDR,
                                   (stlChar*)&sSegmentAttr,
                                   STL_SIZEOF(smlSegmentAttr),
                                   sSegmentRid,
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    /**
     * applys segment attribute to segment header
     */
    
    if( (aSegmentAttr->mValidFlags & SML_SEGMENT_INITIAL_MASK) ==
        SML_SEGMENT_INITIAL_YES )
    {
        sBytes = aSegmentAttr->mInitial;
        sSegmentHeader->mInitialExtents = STL_ALIGN( STL_ALIGN( sBytes,
                                                                SMP_PAGE_SIZE ) / SMP_PAGE_SIZE,
                                                     sPageCountInExt ) / sPageCountInExt;
    }

    if( (aSegmentAttr->mValidFlags & SML_SEGMENT_NEXT_MASK) ==
        SML_SEGMENT_NEXT_YES )
    {
        sBytes = aSegmentAttr->mNext;
        sSegmentHeader->mNextExtents = STL_ALIGN( STL_ALIGN( sBytes,
                                                             SMP_PAGE_SIZE ) / SMP_PAGE_SIZE,
                                                  sPageCountInExt ) / sPageCountInExt;
    }
    
    if( (aSegmentAttr->mValidFlags & SML_SEGMENT_MINSIZE_MASK) ==
        SML_SEGMENT_MINSIZE_YES )
    {
        sBytes = aSegmentAttr->mMinSize;
        sSegmentHeader->mMinExtents = STL_ALIGN( STL_ALIGN( sBytes,
                                                            SMP_PAGE_SIZE ) / SMP_PAGE_SIZE,
                                                 sPageCountInExt ) / sPageCountInExt;
    }
    
    if( (aSegmentAttr->mValidFlags & SML_SEGMENT_MAXSIZE_MASK) ==
        SML_SEGMENT_MAXSIZE_YES )
    {
        sBytes = aSegmentAttr->mMaxSize;
        sExtents = STL_ALIGN( STL_ALIGN( sBytes,
                                         SMP_PAGE_SIZE ) / SMP_PAGE_SIZE,
                              sPageCountInExt ) / sPageCountInExt;

        /**
         * 변경될 MAXSIE 가 할당되어 있는 공간보다 작다면 할당된 사이즈로
         * MAXSIZE를 결정한다.
         */
        
        if( sExtents > sCache->mAllocExtCount )
        {
            sSegmentHeader->mMaxExtents = sExtents;
        }
        else
        {
            sSegmentHeader->mMaxExtents = sCache->mAllocExtCount;
        }
    }
    
    /**
     * applys segment attribute to segment cache
     */

    sCache->mInitialExtents = sSegmentHeader->mInitialExtents;
    sCache->mNextExtents = sSegmentHeader->mNextExtents;
    sCache->mMinExtents = sSegmentHeader->mMinExtents;
    sCache->mMaxExtents = sSegmentHeader->mMaxExtents;
    
    /**
     * logging redo log
     */
    
    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_INIT_SEGMENT_HDR,
                           SMR_REDO_TARGET_PAGE,
                           (stlChar*)sSegmentHeader,
                           STL_SIZEOF(smsSegHdr),
                           sSegmentRid.mTbsId,
                           sSegmentRid.mPageId,
                           (stlChar*)sSegmentHeader - (stlChar*)SMP_FRAME(&sPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv )
             == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smxmRollback( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 세그먼트 캐시를 구성한다.
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aSegHdrPid Segment Header Page Identifier
 * @param[in] aSegmentHandle Target Segment Handle
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsBuildCache( smlTbsId     aTbsId,
                         smlPid       aSegHdrPid,
                         void      ** aSegmentHandle,
                         smlEnv     * aEnv )
{
    smpHandle    sPageHandle;
    smlSegType   sSegType;
    smsSegHdr  * sSegHdr;
    stlInt32     sState = 0;
    stlInt8      sSegState;

    STL_PARAM_VALIDATE( aSegmentHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    *aSegmentHandle = NULL;

    STL_TRY( smpFix( aTbsId,
                     aSegHdrPid,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;

    sSegHdr = (smsSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

    /* column type이면 skip한다. */
    if( sSegHdr->mPageType != SMS_SEG_HDR_PAGE_TYPE_SEGHDR )
    {
        sState = 0;
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

        STL_THROW( RAMP_END_BUILD_CACHE );
    }
    
    sSegType = sSegHdr->mSegType;
    sSegState = sSegHdr->mState;
    
    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    /**
     * Segment가 Body까지 할당된 경우에만 Cache를 만든다
     */
    if( sSegState != SMS_STATE_ALLOC_HDR )
    {
        if( gSegmentFuncs[sSegType]->mBuildCache != NULL )
        {
            STL_TRY( gSegmentFuncs[sSegType]->mBuildCache( aTbsId,
                                                           aSegHdrPid,
                                                           aSegmentHandle,
                                                           aEnv )
                     == STL_SUCCESS );
        }

        if( aSegHdrPid == (SMS_SEGMENT_MAP_RID).mPageId )
        {
            STL_ASSERT( *aSegmentHandle != NULL );

            gSmsWarmupEntry->mSegmentMapHandle = *aSegmentHandle;
        }
    }

    STL_RAMP( RAMP_END_BUILD_CACHE );

    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Full-Scan을 위한 Iterator를 할당받는다.
 * @param[in] aStatement Statement Pointer
 * @param[in] aSegmentHandle Target Segment Handle
 * @param[out] aSegIterator 할당받을 Segment Full-Scan Iterator
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsAllocSegIterator( smlStatement * aStatement,
                               void         * aSegmentHandle,
                               void        ** aSegIterator,
                               smlEnv       * aEnv )
{
    smlSegType sSegType;

    STL_PARAM_VALIDATE( aSegmentHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegType = smsGetSegType( aSegmentHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mAllocSegIterator != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    
    STL_TRY( gSegmentFuncs[sSegType]->mAllocSegIterator( aStatement,
                                                         aSegIterator,
                                                         aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Full-Scan을 위한 Iterator를 해제한다.
 * @param[in] aSegmentHandle Target Segment Handle
 * @param[in] aSegIterator 해제할 Segment Full-Scan Iterator
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsFreeSegIterator( void    * aSegmentHandle,
                              void    * aSegIterator,
                              smlEnv  * aEnv )
{
    smlSegType sSegType;

    STL_PARAM_VALIDATE( aSegmentHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegType = smsGetSegType( aSegmentHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mFreeSegIterator != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    
    STL_TRY( gSegmentFuncs[sSegType]->mFreeSegIterator( aSegIterator,
                                                        aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세그먼트에 할당된 첫번째 페이지를 얻는다.
 * @param[in] aSegmentHandle Target Segment Handle
 * @param[in] aSegIterator Full-Scan Iterator
 * @param[in] aPageId Target Page Identifier
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsGetFirstPage( void    * aSegmentHandle,
                           void    * aSegIterator,
                           smlPid  * aPageId,
                           smlEnv  * aEnv )
{
    smlSegType sSegType;
    smlTbsId   sTbsId;
    
    STL_PARAM_VALIDATE( aSegmentHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegType = smsGetSegType( aSegmentHandle );
    sTbsId = smsGetTbsId( aSegmentHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mGetFirstPage != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    
    STL_TRY( gSegmentFuncs[sSegType]->mGetFirstPage( sTbsId,
                                                     aSegmentHandle,
                                                     aSegIterator,
                                                     aPageId,
                                                     aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세그먼트에 할당된 페이지중 Iterator상의 Next 페이지를 얻는다.
 * @param[in] aSegmentHandle Target Segment Handle
 * @param[in] aSegIterator Full-Scan Iterator
 * @param[in] aPageId Target Page Identifier
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsGetNextPage( void    * aSegmentHandle,
                          void    * aSegIterator,
                          smlPid  * aPageId,
                          smlEnv  * aEnv )
{
    smlSegType sSegType;
    smlTbsId   sTbsId;

    STL_PARAM_VALIDATE( aSegmentHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegType = smsGetSegType( aSegmentHandle );
    sTbsId = smsGetTbsId( aSegmentHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mGetFirstPage != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    
    /**
     * Event 검사
     */
    STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );

    STL_TRY( gSegmentFuncs[sSegType]->mGetNextPage( sTbsId,
                                                    aSegIterator,
                                                    aPageId,
                                                    aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세그먼트 핸들(캐시)에 주어진 트랜잭션 정보를 설정한다.
 * @param[in] aSegmentHandle Target Segment Handle
 * @param[in] aTransId       Transaction Identifier
 * @param[in] aTransViewScn  Transaction View Scn
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsSetTransToHandle( void        * aSegmentHandle,
                               smxlTransId   aTransId,
                               smlScn        aTransViewScn,
                               smlEnv      * aEnv )
{
    smlSegType sSegType;

    STL_PARAM_VALIDATE( aSegmentHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegType = smsGetSegType( aSegmentHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mSetTransToHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    
    STL_TRY( gSegmentFuncs[sSegType]->mSetTransToHandle( aSegmentHandle,
                                                         aTransId,
                                                         aTransViewScn,
                                                         aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세그먼트 공간을 정해진 크기로 줄인다.
 * @param[in]     aTransId       Transaction Identifier
 * @param[in]     aSegmentHandle Target Segment Handle
 * @param[in]     aAttr          Segment attribute
 * @param[in,out] aEnv           Environment 정보
 */
stlStatus smsShrinkTo( smxlTransId      aTransId,
                       void           * aSegmentHandle,
                       smlSegmentAttr * aAttr,
                       smlEnv         * aEnv )
{
    smlSegType sSegType;

    STL_PARAM_VALIDATE( aSegmentHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    sSegType = smsGetSegType( aSegmentHandle );
    
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[sSegType]->mShrinkTo != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    
    STL_TRY( gSegmentFuncs[sSegType]->mShrinkTo( aTransId,
                                                 aSegmentHandle,
                                                 aAttr,
                                                 aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세그먼트에 할당된 페이지 개수를 반환한다.
 * @param[in] aSegmentHandle Target Segment Handle
 */
stlInt64 smsGetAllocPageCount( void * aSegmentHandle )
{
    smsCommonCache * sCache;
    
    sCache = (smsCommonCache*)aSegmentHandle;

    return ((stlInt64)sCache->mAllocExtCount * smfGetPageCountInExt(sCache->mTbsId));
}

/**
 * @brief 세그먼트에 사용되고 있는 페이지 개수를 반환한다.
 * @param[in]       aSegmentHandle     Target Segment Handle
 * @param[out]      aUsedPageCount     사용중인 페이지 개수
 * @param[in,out]   aEnv               Environment 정보
 * @note 해당 함수는 통계정보를 위해서 사용되며 정확한 값이 아닐수도 있다.
 */
stlStatus smsGetUsedPageCount4Statistics( void     * aSegmentHandle,
                                          stlInt64 * aUsedPageCount,
                                          smlEnv   * aEnv )
{
    smsCommonCache * sCache;
    stlInt64         sUsedPageCount;
    smlSegType       sSegType;
    
    sSegType = smsGetSegType( aSegmentHandle );
    sCache = (smsCommonCache*)aSegmentHandle;

    sUsedPageCount = ( (stlInt64)sCache->mAllocExtCount *
                       smfGetPageCountInExt(sCache->mTbsId) );
        
    if( sSegType == SML_SEG_TYPE_MEMORY_BITMAP )
    {
        if( sCache->mAllocExtCount <= 4 )
        {
            STL_TRY( gSegmentFuncs[sSegType]->mGetUsedPageCount( aSegmentHandle,
                                                                 &sUsedPageCount,
                                                                 aEnv )
                     == STL_SUCCESS );
        }
    }

    *aUsedPageCount = sUsedPageCount;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세그먼트에 설정된 Minimum Extents 정보를 반환한다.
 * @param[in] aSegmentHandle Target Segment Handle
 * @return Minimum Extents의 개수
 */
stlInt32 smsGetMinExtents( void * aSegmentHandle )
{
    return ((smsCommonCache*)aSegmentHandle)->mMinExtents;
}

/**
 * @brief 세그먼트 헤더의 크기를 얻는다.
 * @param[in] aSegmentHandle Target Segment Handle
 * @return Segment Header Size
 */
stlInt32 smsGetHeaderSize( void * aSegmentHandle )
{
    smlSegType sSegType;

    sSegType = smsGetSegType( aSegmentHandle );
    return gSegmentFuncs[sSegType]->mGetHeaderSize();
}

/**
 * @brief 세그먼트 Cache의 크기를 얻는다.
 * @param[in] aSegmentHandle Target Segment Handle
 * @return Segment Cache Size
 */
stlInt32 smsGetCacheSize( void * aSegmentHandle )
{
    smlSegType sSegType;

    sSegType = smsGetSegType( aSegmentHandle );
    return gSegmentFuncs[sSegType]->mGetCacheSize();
}

/**
 * @brief 세그먼트 힌트 정보를 생성한다.
 * @param[in]     aSessionEnv Session Environment 정보
 * @param[in,out] aEnv        Environment 정보
 */
stlStatus smsCreateSegHint( smlSessionEnv * aSessionEnv,
                            smlEnv        * aEnv )
{
    stlInt32      i;
    smsHintPool * sSegHintPool;
    
    STL_TRY( knlAllocRegionMem( &aSessionEnv->mSessionMem,
                                STL_SIZEOF( smsHintPool ),
                                &aSessionEnv->mSegHintPool,
                                KNL_ENV(aEnv) ) == STL_SUCCESS );

    sSegHintPool = aSessionEnv->mSegHintPool;

    sSegHintPool->mSequence = 0;
    for( i = 0; i < SMS_MAX_SEGMENT_HINT; i++ )
    {
        sSegHintPool->mSegHint[i].mSegHandle = NULL;
        sSegHintPool->mSegHint[i].mValidScn = 0;
        sSegHintPool->mSegHint[i].mHintPid = SMP_NULL_PID;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 세그먼트 핸들로부터 Segment Type을 얻는다.
 * @param[in] aSegmentHandle Segment Handle
 * @return Segment Type
 */
inline smlSegType smsGetSegType( void * aSegmentHandle )
{
    return ((smsCommonCache*)aSegmentHandle)->mSegType;
}

/**
 * @brief 세그먼트 핸들로부터 Tablespace Identifier를 얻는다.
 * @param[in] aSegmentHandle Segment Handle
 * @return Tablespace Identifier
 */
inline smlTbsId smsGetTbsId( void * aSegmentHandle )
{
    return ((smsCommonCache*)aSegmentHandle)->mTbsId;
}

/**
 * @brief 세그먼트 핸들로부터 Relation handle을 가지는 segment인지 확인한다.
 * @param[in] aSegmentHandle Segment Handle
 * @return STL_TRUE/STL_FALSE
 */
inline stlBool smsHasRelHdr( void * aSegmentHandle )
{
    if( aSegmentHandle == NULL )
    {
        return STL_FALSE;
    }
    else
    {
        return ((smsCommonCache*)aSegmentHandle)->mHasRelHdr;
    }
}

/**
 * @brief 세그먼트 핸들로부터 Segment Record Identifier를 얻는다.
 * @param[in] aSegmentHandle Segment Handle
 * @return Segment Identifier
 */
smlRid smsGetSegRid( void * aSegmentHandle )
{
    return ((smsCommonCache*)aSegmentHandle)->mSegRid;
}

/**
 * @brief 세그먼트 핸들로부터 Segment Identifier를 얻는다.
 * @param[in] aSegmentHandle Segment Handle
 * @return Segment Identifier
 */
stlInt64 smsGetSegmentId( void * aSegmentHandle )
{
    return ((smsCommonCache*)aSegmentHandle)->mSegId;
}

/**
 * @brief 세그먼트 핸들로부터 Valid Scn를 얻는다.
 * @param[in] aSegmentHandle Segment Handle
 * @return Valid Scn
 */
smlScn smsGetValidScn( void * aSegmentHandle )
{
    return ((smsCommonCache*)aSegmentHandle)->mValidScn;
}

/**
 * @brief 세그먼트 핸들로부터 Valid Sequence를 얻는다.
 * @param[in] aSegmentHandle Segment Handle
 * @return Valid Sequence
 */
stlInt64 smsGetValidSeqFromHandle( void * aSegmentHandle )
{
    return ((smsCommonCache*)aSegmentHandle)->mValidSeq;
}

/**
 * @brief 세그먼트로부터 Valid Sequence를 얻는다.
 * @param[in]     aSegRid       Target Segment Rid
 * @param[out]    aValidSeq     Valid Sequence Pointer    
 * @param[in,out] aEnv          Environment 정보
 */
stlStatus smsGetValidSeq( smlRid     aSegRid,
                          stlInt64 * aValidSeq,
                          smlEnv   * aEnv )
{
    smpHandle   sPageHandle;
    stlInt32    sState = 0;
    smsSegHdr * sSegHdr;
    
    STL_TRY( smpFix( aSegRid.mTbsId,
                     aSegRid.mPageId,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;

    sSegHdr = (smsSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

    *aValidSeq = sSegHdr->mValidSeq;
    
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
 * @brief 세그먼트 핸들로부터 Extent내에 Usable Page Count를 얻는다.
 * @param[in] aSegmentHandle Segment Handle
 * @return Usable page count
 */
stlInt64 smsGetUsablePageCountInExt( void * aSegmentHandle )
{
    return ((smsCommonCache*)aSegmentHandle)->mUsablePageCountInExt;
}

/**
 * @brief 세그먼트의 Attribute와 aMergeSegAttr를 합병해서 aMergedSegAttr로 반환한다.
 * @param[in] aSegmentHandle Segment Handle
 * @param[in] aMergeSegAttr 합병될 Segment Attribute
 * @param[out] aMergedSegAttr 합병된 Segment Attribute
 */
void smsMergeSegmentAttr( void           * aSegmentHandle,
                          smlSegmentAttr * aMergeSegAttr,
                          smlSegmentAttr * aMergedSegAttr )
{
    smsCommonCache * sCache;
    smlSegmentAttr   sSegmentAttr;
    stlInt64         sPageCountInExt;

    sCache = (smsCommonCache*)aSegmentHandle;

    sPageCountInExt = smfGetPageCountInExt( sCache->mTbsId );

    stlMemset( &sSegmentAttr, 0x00, STL_SIZEOF(sSegmentAttr) );
    
    sSegmentAttr.mValidFlags = SML_SEGMENT_DEFAULT_ATTR;
    
    sSegmentAttr.mInitial = ( ((stlInt64)sCache->mInitialExtents) *
                              SMP_PAGE_SIZE *
                              sPageCountInExt );
    sSegmentAttr.mNext = ( ((stlInt64)sCache->mNextExtents) *
                           SMP_PAGE_SIZE *
                           sPageCountInExt );
    sSegmentAttr.mMinSize = ( ((stlInt64)sCache->mMinExtents) *
                              SMP_PAGE_SIZE *
                              sPageCountInExt );
    sSegmentAttr.mMaxSize = ( ((stlInt64)sCache->mMaxExtents) *
                              SMP_PAGE_SIZE *
                              sPageCountInExt );
    
    if( (aMergeSegAttr->mValidFlags & SML_SEGMENT_INITIAL_MASK) ==
        SML_SEGMENT_INITIAL_YES )
    {
        sSegmentAttr.mInitial = aMergeSegAttr->mInitial;
    }
    
    if( (aMergeSegAttr->mValidFlags & SML_SEGMENT_NEXT_MASK) ==
        SML_SEGMENT_NEXT_YES )
    {
        sSegmentAttr.mNext = aMergeSegAttr->mNext;
    }

    if( (aMergeSegAttr->mValidFlags & SML_SEGMENT_MINSIZE_MASK) ==
        SML_SEGMENT_MINSIZE_YES )
    {
        sSegmentAttr.mMinSize = aMergeSegAttr->mMinSize;
    }

    if( (aMergeSegAttr->mValidFlags & SML_SEGMENT_MAXSIZE_MASK) ==
        SML_SEGMENT_MAXSIZE_YES )
    {
        sSegmentAttr.mMaxSize = aMergeSegAttr->mMaxSize;
    }

    *aMergedSegAttr = sSegmentAttr;
}

/**
 * @brief 세그먼트 상태를 얻는다.
 * @param[in] aSegRid Segment RID
 * @param[out] aSegState Segment 상태
 * @param[in,out] aEnv Environment 정보
 */
stlStatus smsGetSegmentState( smlRid     aSegRid,
                              stlInt8  * aSegState,
                              smlEnv   * aEnv )
{
    smpHandle    sPageHandle;
    smsSegHdr  * sSegHdr;
    stlInt32     sState = 0;
    stlInt8      sSegState;
    
    STL_TRY( smpFix( aSegRid.mTbsId,
                     aSegRid.mPageId,
                     &sPageHandle,
                     aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    sSegHdr = (smsSegHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
    sSegState = sSegHdr->mState;
    
    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

    *aSegState = sSegState;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Segment Map에서 특정 페이지를 삭제한다.
 * @param[in] aTransId 트랜잭션 아이디
 * @param[in] aSegRid 삭제할 페이지 정보를 갖는 Segment RID
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smsFreeSegMapPage( smxlTransId   aTransId,
                             smlRid        aSegRid,
                             smlEnv      * aEnv )
{
    smlRid      sSegMapRid;
    smpHandle   sPageHandle;
    smxmTrans   sMiniTrans;
    stlInt32    sState = 0;
    
    sSegMapRid = SMS_SEGMENT_MAP_RID;
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        sSegMapRid,
                        SMXM_ATTR_REDO,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpAcquire( &sMiniTrans,
                         sSegMapRid.mTbsId,
                         aSegRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    STL_TRY( smsUpdatePageStatus( &sMiniTrans,
                                  gSmsWarmupEntry->mSegmentMapHandle,
                                  &sPageHandle,
                                  SMP_FREENESS_FREE,
                                  0,  /* aScn */
                                  aEnv ) == STL_SUCCESS );
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmRollback( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Segment Map에서 column page를 삭제한다.
 * @param[in] aTransId 트랜잭션 아이디
 * @param[in] aColPid Column Page Id
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smsFreeSegMapColumnMeta( smxlTransId   aTransId,
                                   smlPid        aColPid,
                                   smlEnv      * aEnv )
{
    smlRid           sSegMapRid;
    smpHandle        sPageHandle;
    smxmTrans        sMiniTrans;
    stlInt32         sState = 0;
    
    sSegMapRid = SMS_SEGMENT_MAP_RID;

    STL_TRY( smxmBegin( &sMiniTrans,
                        aTransId,
                        sSegMapRid,
                        SMXM_ATTR_REDO,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpAcquire( &sMiniTrans,
                         sSegMapRid.mTbsId,
                         aColPid,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    STL_TRY( smsUpdatePageStatus( &sMiniTrans,
                                  gSmsWarmupEntry->mSegmentMapHandle,
                                  &sPageHandle,
                                  SMP_FREENESS_FREE,
                                  0,  /* aScn */
                                  aEnv ) == STL_SUCCESS );

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmRollback( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 해당 Segment가 Irrecoverable Segment에 해당되는지 검사한다.
 * @param[in] aSegmentId 대상 Segment Identifier
 */
stlBool smsIsIrrecoverableSegment( stlInt64 aSegmentId )
{
    smsIrrecoverableSegment * sSegment;
    stlBool                   sResult = STL_FALSE;
    
    STL_RING_FOREACH_ENTRY( &(gSmsWarmupEntry->mIrrecoverableSegmentList), sSegment )
    {
        if( sSegment->mSegmentId == aSegmentId )
        {
            sResult = STL_TRUE;
            break;
        }
    }

    return sResult;
}

/**
 * @brief 해당 세그먼트를 Irrecoverable Segment로 추가한다.
 * @param[in] aSegmentId 대상 Segment Identifier
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smsAddIrrecoverableSegment( stlInt64    aSegmentId,
                                      smlEnv    * aEnv )
{
    smsIrrecoverableSegment * sSegment;
    stlBool                   sFound = STL_FALSE;
    
    STL_RING_FOREACH_ENTRY( &(gSmsWarmupEntry->mIrrecoverableSegmentList), sSegment )
    {
        if( sSegment->mSegmentId == aSegmentId )
        {
            sFound = STL_TRUE;
            break;
        }
    }

    if( sFound == STL_FALSE )
    {
        STL_TRY( smgAllocShmMem4Mount( STL_SIZEOF( smsIrrecoverableSegment ),
                                       (void**)&sSegment,
                                       aEnv )
                 == STL_SUCCESS );

        STL_RING_INIT_DATALINK( sSegment,
                                STL_OFFSETOF(smsIrrecoverableSegment, mSegmentLink) );

        sSegment->mSegmentId = aSegmentId;
    
        STL_RING_ADD_LAST( &gSmsWarmupEntry->mIrrecoverableSegmentList, sSegment );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 해당 Segment를 Unusable 상태로 만든다.
 * @param[in] aStatement Statement Pointer
 * @param[in] aSegmentHandle Segment Handle
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smsSetUnusableSegment( smlStatement * aStatement,
                                 void         * aSegmentHandle,
                                 smlEnv       * aEnv )
{
    smxlTransId      sTransId = SMA_GET_TRANS_ID(aStatement);
    stlUInt32        sAttr = SMXM_ATTR_REDO;
    smlRid           sUndoRid;
    smxmTrans        sMiniTrans;
    smpHandle        sPageHandle;
    stlInt16         sOffset;
    stlBool          sData[2];
    smsSegHdr      * sSegmentHeader;
    smlRid           sSegmentRid;
    stlInt32         sState = 0;
    smsPendArg       sPendArg;
    
    sSegmentRid = smsGetSegRid( aSegmentHandle );
    
    STL_TRY( smxlInsertTransRecord( sTransId,
                                    aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        sTransId,
                        sSegmentRid,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpAcquire( &sMiniTrans,
                         sSegmentRid.mTbsId,
                         sSegmentRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sSegmentHeader = (smsSegHdr*)smpGetBody(SMP_FRAME(&sPageHandle));

    sOffset = ( (stlChar*)&sSegmentHeader->mIsUsable -
                (stlChar*)SMP_FRAME(&sPageHandle) );

    sData[0] = STL_TRUE;
    sData[1] = STL_FALSE;

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sData,
                           STL_SIZEOF( stlBool ) * 2,
                           sSegmentRid.mTbsId,
                           sSegmentRid.mPageId,
                           sOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMS_UNDO_LOG_MEMORY_SET_UNUSABLE_SEGMENT,
                                   NULL,
                                   0,
                                   sSegmentRid,
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    sSegmentHeader->mIsUsable = STL_FALSE;
    ((smsCommonCache*)aSegmentHandle)->mIsUsable = STL_FALSE;
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    
    /**
     * register pending operation
     */
    
    stlMemset( &sPendArg, 0x00, STL_SIZEOF(smsPendArg) );
    sPendArg.mSegmentHandle = aSegmentHandle;
    sPendArg.mTransId = sTransId;
                                   
    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_UNUSABLE_SEGMENT,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendArg,
                           STL_SIZEOF( smsPendArg ),
                           0,
                           aEnv ) == STL_SUCCESS );
    
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

/**
 * @brief MOUNT 단계에서 지정된 Irrecoverable segment들을 Unusable 상태로 설정한다.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smsIrrecoverableSegmentToUnusable( smlEnv * aEnv )
{
    smsIrrecoverableSegment * sSegment;
    smxlTransId               sTransId;
    stlUInt64                 sTransSeq;
    void                    * sSegmentHandle;
    smlRid                    sSegmentRid;
    smlStatement            * sStatement;
    stlInt32                  sState = 0;
    smpHandle                 sPageHandle;
    smpFreeness               sFreeness;
    void                    * sVolatileArea;
    stlInt64                  sSegmentId;
    stlInt32                  sPageType;
    smpPhyHdr               * sPhyHdr;
    
    STL_RING_FOREACH_ENTRY( &(gSmsWarmupEntry->mIrrecoverableSegmentList), sSegment )
    {
        stlMemcpy( &sSegmentRid, &sSegment->mSegmentId, STL_SIZEOF(smlRid) );

        if( smsIsValidSegmentId( sSegment->mSegmentId ) == STL_FALSE )
        {
            continue;
        }
        
        STL_TRY( smxlAllocTrans( SML_TIL_READ_COMMITTED,
                                 STL_FALSE, /* aIsGlobalTrans */
                                 &sTransId,
                                 &sTransSeq,
                                 aEnv ) == STL_SUCCESS );
        sState = 1;
        
        smxlSetTransAttr( sTransId, SMXL_ATTR_UPDATABLE );
        
        STL_TRY( smaAllocStatement( sTransId,
                                    SMXL_TO_SML_TRANS_ID( sTransId, sTransSeq ),
                                    NULL,   /* aUpdateRelHandle */
                                    SML_STMT_ATTR_READONLY,
                                    SML_LOCK_TIMEOUT_INFINITE,
                                    STL_FALSE, /* aNeedSnapshotIterator */
                                    &sStatement,
                                    aEnv )
                 == STL_SUCCESS );
        sState = 2;

        STL_TRY( smpFix( sSegmentRid.mTbsId,
                         sSegmentRid.mPageId,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
        sState = 3;

        sFreeness = smpGetFreeness( &sPageHandle );
        sVolatileArea = smpGetVolatileArea( &sPageHandle );
        sSegmentHandle = *(stlChar**)sVolatileArea;
        sPhyHdr = (smpPhyHdr*)SMP_FRAME( &sPageHandle );

        sPageType = sPhyHdr->mPageType;
        sSegmentId = sPhyHdr->mSegmentId;
        
        sState = 2;
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

        if( (sPageType == SMP_PAGE_TYPE_BITMAP_HEADER) &&
            (sSegmentId == smsGetSegmentId(gSmsWarmupEntry->mSegmentMapHandle)) &&
            (sFreeness != SMP_FREENESS_FREE) &&
            (sSegmentHandle != NULL) )
        {
            STL_TRY( knlLogMsg( NULL,
                                KNL_ENV(aEnv),
                                KNL_LOG_LEVEL_INFO,
                                "[SEGMENT] segment become unusable ( physical id : %ld ) \n",
                                sSegment->mSegmentId )
                     == STL_SUCCESS );
        
            STL_TRY( smsSetUnusableSegment( sStatement,
                                            sSegmentHandle,
                                            aEnv )
                     == STL_SUCCESS );
        }
        
        sState = 1;
        STL_TRY( smaFreeStatement( sStatement, aEnv ) == STL_SUCCESS );

        sState = 0;
        STL_TRY( smxlCommit( sTransId,
                             NULL, /* aComment */
                             SML_TRANSACTION_CWM_WAIT,
                             aEnv ) == STL_SUCCESS );
        STL_TRY( smxlFreeTrans( sTransId,
                                SML_PEND_ACTION_COMMIT,
                                STL_FALSE, /* aCleanup */
                                SML_TRANSACTION_CWM_WAIT,
                                aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;
    
    STL_FINISH;
    
    switch( sState )
    {
        case 3:
            (void)smpUnfix( &sPageHandle, aEnv );
        case 2:
            (void)smaFreeStatement( sStatement, aEnv );
        case 1:
            (void)smxlRollback( sTransId,
                                SML_TRANSACTION_CWM_NO_WAIT,
                                STL_FALSE, /* aCleanup */
                                aEnv );
            (void)smxlFreeTrans( sTransId,
                                 SML_PEND_ACTION_ROLLBACK,
                                 STL_FALSE, /* aCleanup */
                                 SML_TRANSACTION_CWM_NO_WAIT,
                                 aEnv );
       default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief 해당 Segment를 Usable 상태로 만든다.
 * @param[in] aStatement Statement Pointer
 * @param[in] aSegmentHandle Segment Handle
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smsSetUsableSegment( smlStatement * aStatement,
                               void         * aSegmentHandle,
                               smlEnv       * aEnv )
{
    smxlTransId      sTransId = SMA_GET_TRANS_ID(aStatement);
    stlUInt32        sAttr = SMXM_ATTR_REDO;
    smlRid           sUndoRid;
    smxmTrans        sMiniTrans;
    smpHandle        sPageHandle;
    stlInt16         sOffset;
    stlBool          sData[2];
    smsSegHdr      * sSegmentHeader;
    smlRid           sSegmentRid;
    stlInt32         sState = 0;
    smsPendArg       sPendArg;
    
    sSegmentRid = smsGetSegRid( aSegmentHandle );
    
    STL_TRY( smxlInsertTransRecord( sTransId,
                                    aEnv )
             == STL_SUCCESS );
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        sTransId,
                        sSegmentRid,
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpAcquire( &sMiniTrans,
                         sSegmentRid.mTbsId,
                         sSegmentRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sSegmentHeader = (smsSegHdr*)smpGetBody(SMP_FRAME(&sPageHandle));

    sOffset = ( (stlChar*)&sSegmentHeader->mIsUsable -
                (stlChar*)SMP_FRAME(&sPageHandle) );

    sData[0] = STL_FALSE;
    sData[1] = STL_TRUE;

    STL_TRY( smxmWriteLog( &sMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sData,
                           STL_SIZEOF( stlBool ) * 2,
                           sSegmentRid.mTbsId,
                           sSegmentRid.mPageId,
                           sOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
    
    STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMS_UNDO_LOG_MEMORY_SET_USABLE_SEGMENT,
                                   NULL,
                                   0,
                                   sSegmentRid,
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

    sSegmentHeader->mIsUsable = STL_TRUE;
    ((smsCommonCache*)aSegmentHandle)->mIsUsable = STL_TRUE;
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    
    /**
     * register pending operation
     */
    
    stlMemset( &sPendArg, 0x00, STL_SIZEOF(smsPendArg) );
    sPendArg.mSegmentHandle = aSegmentHandle;
    sPendArg.mTransId = sTransId;
                                   
    STL_TRY( smgAddPendOp( aStatement,
                           SMG_PEND_USABLE_SEGMENT,
                           SML_PEND_ACTION_COMMIT,
                           (void*)&sPendArg,
                           STL_SIZEOF( smsPendArg ),
                           0,
                           aEnv ) == STL_SUCCESS );
    
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

/**
 * @brief 해당 Segment가 Unusable Segment인지 확인한다.
 * @param[in] aSegmentHandle  대상 Segment Identifier
 */
stlBool smsIsUsableSegment( void * aSegmentHandle )
{
    return ((smsCommonCache*)aSegmentHandle)->mIsUsable;
}

stlStatus smsUnusablePending( stlUInt32   aActionFlag,
                              smgPendOp * aPendOp,
                              smlEnv    * aEnv )
{
    smsPendArg     * sPendArg = (smsPendArg*)aPendOp->mArgs;
    smsCommonCache * sCache;

    sCache = (smsCommonCache*)(sPendArg->mSegmentHandle);
    sCache->mIsUsable = STL_FALSE;

    return STL_SUCCESS;
}


stlStatus smsUsablePending( stlUInt32   aActionFlag,
                            smgPendOp * aPendOp,
                            smlEnv    * aEnv )
{
    smsPendArg     * sPendArg = (smsPendArg*)aPendOp->mArgs;
    smsCommonCache * sCache;

    sCache = (smsCommonCache*)(sPendArg->mSegmentHandle);
    sCache->mIsUsable = STL_TRUE;

    return STL_SUCCESS;
}

stlStatus smsValidateSegment( void   * aSegmentHandle,
                              smlEnv * aEnv )
{
    STL_TRY_THROW( smsIsUsableSegment( aSegmentHandle ) == STL_TRUE,
                   RAMP_ERR_ACCESS_UNUSABLE_SEGMENT );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_ACCESS_UNUSABLE_SEGMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_ACCESS_UNUSABLE_SEGMENT,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      smsGetSegmentId(aSegmentHandle) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlBool smsIsValidSegmentId( stlInt64  aSegmentId )
{
    smlRid          sSegmentRid;
    stlBool         sResult = STL_TRUE;
    smlDatafileId   sDatafileId;
    stlInt64        sPageSeqId;
    
    stlMemcpy( &sSegmentRid, &aSegmentId, STL_SIZEOF(smlRid) );
        
    if( sSegmentRid.mTbsId != SML_MEMORY_DICT_SYSTEM_TBS_ID )
    {
        sResult = STL_FALSE;
    }
    else
    {
        sDatafileId = SMP_DATAFILE_ID( sSegmentRid.mPageId );
        sPageSeqId = SMP_PAGE_SEQ_ID( sSegmentRid.mPageId );
    
        if( smfIsValidDatafile( sSegmentRid.mTbsId, sDatafileId ) == STL_FALSE )
        {
            sResult = STL_FALSE;
        }
        else
        {
            if( sPageSeqId >= smfGetMaxPageCountAtMount( sSegmentRid.mTbsId, sDatafileId ) )
            {
                sResult = STL_FALSE;
            }
            else
            {
                if( sSegmentRid.mOffset != 0 )
                {
                    sResult = STL_FALSE;
                }
            }
        }
    }
    
    return sResult;
}

/** @} */

/**
 * @addtogroup smsManager
 * @{
 */

stlStatus smsInitSegHdr( smxmTrans  * aMiniTrans,
                         smpHandle  * aPageHandle,
                         smlSegType   aSegType,
                         smlTbsId     aTbsId,
                         stlBool      aHasRelHdr,
                         smsSegHdr  * aSegHdr,
                         smlEnv     * aEnv )
{
    aSegHdr->mPageType = SMS_SEG_HDR_PAGE_TYPE_SEGHDR;
    aSegHdr->mSegType = aSegType;
    aSegHdr->mTbsId = aTbsId;
    aSegHdr->mHasRelHdr = aHasRelHdr;
    aSegHdr->mValidScn = SML_INFINITE_SCN;
    aSegHdr->mValidSeq += 1;
    aSegHdr->mState = SMS_STATE_ALLOC_HDR;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_INIT_SEGMENT_HDR,
                           SMR_REDO_TARGET_PAGE,
                           (void*)aSegHdr,
                           STL_SIZEOF( smsSegHdr ),
                           SMS_SEGMENT_MAP_RID.mTbsId,
                           smpGetPageId( aPageHandle ),
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

void smsSetSegType( void       * aSegmentHandle,
                    smlSegType   aSegType )
{
    ((smsCommonCache*)aSegmentHandle)->mSegType = aSegType;
}

void smsSetSegRid( void   * aSegmentHandle,
                   smlRid   aSegRid )
{
    ((smsCommonCache*)aSegmentHandle)->mSegRid = aSegRid;
}

void smsSetSegmentId( void   * aSegmentHandle,
                      smlRid   aSegRid )
{
    SMS_MAKE_SEGMENT_ID( &(((smsCommonCache*)aSegmentHandle)->mSegId),
                         &(((smsCommonCache*)aSegmentHandle)->mSegRid) )
}

void smsSetTbsId( void     * aSegmentHandle,
                  smlTbsId   aTbsId )
{
    ((smsCommonCache*)aSegmentHandle)->mTbsId = aTbsId;
}

void smsSetHasRelHdr( void    * aSegmentHandle,
                      stlBool   aHasRelHdr )
{
    ((smsCommonCache*)aSegmentHandle)->mHasRelHdr = aHasRelHdr;
}

void smsSetValidScn( void    * aSegmentHandle,
                     smlScn    aValidScn )
{
    ((smsCommonCache*)aSegmentHandle)->mValidScn = aValidScn;
}

void smsSetValidSeq( void      * aSegmentHandle,
                     stlInt64    aValidSeq )
{
    ((smsCommonCache*)aSegmentHandle)->mValidSeq = aValidSeq;
}

void smsSetUsablePageCountInExt( void    * aSegmentHandle,
                                 stlInt64  aUsablePageCountInExt )
{
    ((smsCommonCache*)aSegmentHandle)->mUsablePageCountInExt = aUsablePageCountInExt;
}

smsHint * smsFindSegHint( void   * aCache,
                          smlScn   aValidScn,
                          smlEnv * aEnv )
{
    stlInt32       i;
    smsHintPool  * sSegHintPool;
    smsHint      * sSegHint = NULL;

    sSegHintPool = (smsHintPool*)SML_SESS_ENV(aEnv)->mSegHintPool;
    
    for( i = 0; i < SMS_MAX_SEGMENT_HINT; i++ )
    {
        if( (aCache == sSegHintPool->mSegHint[i].mSegHandle) &&
            (aValidScn == sSegHintPool->mSegHint[i].mValidScn) )
        {
            sSegHint = (smsHint*)&(sSegHintPool->mSegHint[i]);
            break;
        }
    }

    return sSegHint;
}

void smsAddNewSegHint( smlPid     aHintPid,
                       void     * aCache,
                       smlScn     aValidScn,
                       smlEnv   * aEnv )
{
    smsHintPool  * sSegHintPool;
    smsHint      * sSegHint = NULL;

    sSegHintPool = (smsHintPool*)SML_SESS_ENV(aEnv)->mSegHintPool;

    sSegHint = (smsHint*)&(sSegHintPool->mSegHint[sSegHintPool->mSequence % SMS_MAX_SEGMENT_HINT]);
    sSegHint->mSegHandle = aCache;
    sSegHint->mValidScn = aValidScn;
    sSegHint->mHintPid = aHintPid;
    
    sSegHintPool->mSequence += 1;
}

/**
 * @brief Temp tablespace에서 extent를 할당받았을 때 data를 기록할 수 있는 공간의 크기를 구한다.
 *        다른 tablespace와는 다르게, 첫번째 page를 header로 사용하지 않는다.
 *        temp tablespace의 경우 첫번째 page도 아주 일부만 extent header로 사용되고, 나머지 공간은
 *        모두 data를 기록하는데 사용할 수 있다.
 * @param[in] aTbsId 대상 tablespace ID
 */
inline stlUInt64 smsGetPageCountInTempTbsExt( smlTbsId aTbsId )
{
    return smfGetPageCountInExt( aTbsId );
}

/**
 * @brief 새로운 column meta page를 할당한다.
 * @param[in]     aMiniTrans   Mini-Transaction
 * @param[in]     aSegType     Segment Type
 * @param[in]     aPageId      Alloc된 page Id
 * @param[in,out] aEnv         Environment 정보
 */
stlStatus smsAllocColMetaHdr( smxmTrans  * aMiniTrans,
                              smlSegType   aSegType,
                              smlPid     * aPageId,
                              smlEnv     * aEnv )
{
    smpHandle   sPageHandle;
    stlInt32    sSegHdrPageType = SMS_SEG_HDR_PAGE_TYPE_COLLEN;

    STL_PARAM_VALIDATE( gSegmentFuncs[aSegType] != NULL,
                        KNL_ERROR_STACK( aEnv ) );
    STL_PARAM_VALIDATE( gSegmentFuncs[aSegType]->mAllocHandle != NULL,
                        KNL_ERROR_STACK( aEnv ) );

    STL_TRY( gSegmentFuncs[SMS_SEGMENT_MAP_SEGMENT_TYPE]->mAllocPage(
                 aMiniTrans,
                 SMS_SEGMENT_MAP_RID.mTbsId,
                 gSmsWarmupEntry->mSegmentMapHandle,
                 SMP_PAGE_TYPE_BITMAP_HEADER,
                 NULL, /* aIsAgableFunc */
                 &sPageHandle,
                 aEnv ) == STL_SUCCESS );

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEGMENT,
                           SMR_LOG_INIT_SEGMENT_HDR,
                           SMR_REDO_TARGET_PAGE,
                           (void*)&sSegHdrPageType,
                           STL_SIZEOF( stlInt32 ),
                           SMS_SEGMENT_MAP_RID.mTbsId,
                           smpGetPageId( &sPageHandle ),
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv )
             == STL_SUCCESS );

    *aPageId = smpGetPageId( &sPageHandle );

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

inline stlBool smsIsSegHdrCollenPage( stlInt32  aPageType )
{
    return (aPageType == SMS_SEG_HDR_PAGE_TYPE_COLLEN);
}

/** @} */
