/*******************************************************************************
 * smpSlot.c
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
#include <smxmDef.h>
#include <smxm.h>
#include <smxl.h>
#include <smpDef.h>
#include <smp.h>

/**
 * @file smpSlot.c
 * @brief Storage Manager Layer Slot Manipulation Routines
 */

/**
 * @addtogroup smp
 * @{
 */

/**
 * @brief 주어진 페이지 body를 slot 사용을 위해 초기화 한다 
 * @param[in,out] aPageHandle 초기화 할 페이지 Handle
 * @param[in] aPageMgmtType 페이지내의 slot 사용 방식
 * @param[in] aLogicalHeaderSize logical header의 크기
 * @param[in] aLogicalHeader 쓰여질 logical header의 내용
 * @param[in] aMinRtsCount Tx를 위해 사용될 최소 RTS의 개수
 * @param[in] aMaxRtsCount Tx를 위해 사용될 최대 RTS의 개수
 * @param[in] aUseFreeSlotList 페이지내의 free offset slot list를 관리하는지 여부
 * @param[in,out] aMiniTrans logging시 사용될 mini-transaction
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpInitBody( smpHandle * aPageHandle,
                       stlUInt16   aPageMgmtType,
                       stlUInt16   aLogicalHeaderSize,
                       stlChar   * aLogicalHeader,
                       stlUInt8    aMinRtsCount,
                       stlUInt8    aMaxRtsCount,
                       stlBool     aUseFreeSlotList,
                       smxmTrans * aMiniTrans,
                       smlEnv    * aEnv )
{
    stlUInt32                i;
    stlChar                * sPage = (stlChar*)aPageHandle->mFrame;
    stlChar                * sSignpost = SMP_GET_SIGNPOST(sPage);
    smpRawChunkSignpost    * sRCSignpost;
    smpOrderedSlotSignpost * sOSSignpost;
    smpRts                 * sRts;
    smpOffsetTableHeader   * sOffsetTable;
    stlUInt16                sLogSize;
    stlChar                  sLogBuf[SMP_PAGE_SIZE];
    stlChar                * sLogPtr = sLogBuf;
    stlInt16                 sUseFreeList = (stlInt16)aUseFreeSlotList;

    STL_PARAM_VALIDATE( (aMinRtsCount <= SMP_MAX_RTS_COUNT) && (aMaxRtsCount <= SMP_MAX_RTS_COUNT),
                        KNL_ERROR_STACK((knlEnv*)aEnv) );

    if( aPageMgmtType == SMP_RAW_CHUNK_TYPE )
    {
        sRCSignpost = (smpRawChunkSignpost*)sSignpost;
        sRCSignpost->mCommon.mPageMgmtType = SMP_RAW_CHUNK_TYPE;
        sRCSignpost->mCommon.mHighWaterMark = 
            STL_SIZEOF(smpPhyHdr) + STL_SIZEOF(smpRawChunkSignpost);
        sLogSize = sRCSignpost->mCommon.mHighWaterMark - STL_SIZEOF(smpPhyHdr);

        stlMemcpy( sLogPtr, sSignpost, sLogSize );
        sLogPtr += sLogSize;
    }
    else
    {
        STL_ASSERT( aPageMgmtType == SMP_ORDERED_SLOT_TYPE );
        sOSSignpost = (smpOrderedSlotSignpost*)sSignpost;

        sOSSignpost->mCommon.mPageMgmtType = SMP_ORDERED_SLOT_TYPE;
        sOSSignpost->mCommon.mHighWaterMark = 
            STL_SIZEOF(smpPhyHdr) + STL_SIZEOF(smpOrderedSlotSignpost);

        sOSSignpost->mLogicalHeaderSize = STL_ALIGN(aLogicalHeaderSize, 8);
        sOSSignpost->mMinRtsCount = aMinRtsCount;
        sOSSignpost->mMaxRtsCount = aMaxRtsCount;
        sOSSignpost->mCurRtsCount = aMinRtsCount;
        sOSSignpost->mHintRts = 0;
        sOSSignpost->mLowWaterMark = SMP_PAGE_SIZE - STL_SIZEOF(smpPhyTail);
        sOSSignpost->mReclaimedSlotSpace = 0;
        sOSSignpost->mActiveSlotCount = 0;

        stlMemcpy( sPage + sOSSignpost->mCommon.mHighWaterMark,
                   aLogicalHeader,
                   aLogicalHeaderSize );

        sOSSignpost->mCommon.mHighWaterMark += STL_ALIGN(aLogicalHeaderSize, 8);
        sLogSize = sOSSignpost->mCommon.mHighWaterMark - STL_SIZEOF(smpPhyHdr);
        
        stlMemcpy( sLogPtr, sSignpost, sLogSize );
        sLogPtr += sLogSize;
        STL_WRITE_INT16( sLogPtr, &sUseFreeList );
        sLogPtr += STL_SIZEOF(stlInt16);

        sRts = (smpRts*)(sPage + sOSSignpost->mCommon.mHighWaterMark);
        for (i = 0; i < aMinRtsCount; i++)
        {
            SMP_INIT_RTS(sRts);
            sRts++;
        }
        sOSSignpost->mCommon.mHighWaterMark += STL_SIZEOF(smpRts) * aMinRtsCount;

        sOffsetTable = (smpOffsetTableHeader*)(sPage + sOSSignpost->mCommon.mHighWaterMark);
        sOffsetTable->mCurrentSlotCount = 0;
        if( aUseFreeSlotList == STL_TRUE )
        {
            sOffsetTable->mFirstFreeOffsetSlot = SMP_USE_FREE_SLOT_LIST_YES | SMP_OFFSET_SLOT_NULL;
        }
        else
        {
            sOffsetTable->mFirstFreeOffsetSlot = SMP_USE_FREE_SLOT_LIST_NO | SMP_OFFSET_SLOT_NULL;
        }
        sOSSignpost->mCommon.mHighWaterMark += STL_SIZEOF(smpOffsetTableHeader);
    }

    if( aMiniTrans != NULL )
    {
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_PAGE_ACCESS,
                               SMR_LOG_INIT_PAGE_BODY,
                               SMR_REDO_TARGET_PAGE,
                               sLogBuf,
                               sLogPtr - sLogBuf, /* signpost + logical header (+ UseFreeList ) */
                               aPageHandle->mPch->mTbsId,
                               aPageHandle->mPch->mPageId,
                               STL_SIZEOF(smpPhyHdr),
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed slot type page에 column 시작 offset을 기록한다.
 * @param[in,out] aMiniTrans logging시 사용될 mini-transaction
 * @param[in,out] aPageHandle 초기화 할 페이지 Handle
 * @param[in] aUseFreeSlotList 페이지내의 free slot list를 관리하는지 여부
 * @param[in] aPageMgmtType 페이지내의 slot 사용 방식
 * @param[in] aLogicalHeaderSize logical header의 크기
 * @param[in] aLogicalHeader 쓰여질 logical header의 내용
 * @param[in] aRowHdrLen Row Header Length
 * @param[in] aFromOrd 첫 column order
 * @param[in] aToOrd 마지막 column order
 * @param[in] aColLengthArray Column Lenth Array
 * @param[in] aContColumnThreshold Continuous column으로 전환할 threshold
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpInitFixedPageBody( smxmTrans  * aMiniTrans,
                                smpHandle  * aPageHandle,
                                stlBool      aUseFreeSlotList,
                                stlUInt16    aPageMgmtType,
                                stlUInt16    aLogicalHeaderSize,
                                stlChar    * aLogicalHeader,
                                stlUInt16    aRowHdrLen,
                                stlInt32     aFromOrd,
                                stlInt32     aToOrd,
                                stlInt16   * aColLengthArray,
                                stlInt16     aContColumnThreshold,
                                smlEnv     * aEnv )
{
    stlChar              * sPage = (stlChar *)SMP_FRAME( aPageHandle );
    smpFixedSlotSignpost * sFSSignpost = (smpFixedSlotSignpost*)SMP_GET_SIGNPOST( sPage );
    stlInt16               sOffset;
    stlUInt16              i;
    stlChar              * sPageBody;
    stlInt16               sLogSize;
    stlUInt16              sColLen;

    STL_ASSERT( aPageMgmtType == SMP_FIXED_SLOT_TYPE );

    sFSSignpost->mCommon.mPageMgmtType = SMP_FIXED_SLOT_TYPE;
    sFSSignpost->mCommon.mHighWaterMark =
        STL_SIZEOF(smpPhyHdr) + STL_SIZEOF(smpFixedSlotSignpost);

    sFSSignpost->mLogicalHeaderSize = STL_ALIGN(aLogicalHeaderSize, 8);
    sFSSignpost->mActiveSlotCount = 0;
    sFSSignpost->mAllocatedMaxSlotSeq = -1;

    stlMemcpy( sPage + sFSSignpost->mCommon.mHighWaterMark,
               aLogicalHeader,
               aLogicalHeaderSize );

    sFSSignpost->mCommon.mHighWaterMark += sFSSignpost->mLogicalHeaderSize;

    sPageBody = (stlChar*)(sPage + sFSSignpost->mCommon.mHighWaterMark);

    /**
     * Continuous column을 저장하는 page는 column length를 저장하지 않는다.
     */
    if( aColLengthArray != NULL )
    {
        sOffset = 0;
        for( i = aFromOrd; i <= aToOrd; i++ )
        {
            sColLen = aColLengthArray[i];

            if( sColLen > aContColumnThreshold )
            {
                sColLen = aContColumnThreshold | SMP_FIXED_SLOT_CONTINUOUS_COLUMN_MASK;
            }

            STL_WRITE_MOVE_INT16( sPageBody, &sColLen, sOffset );
        }

        sFSSignpost->mCommon.mHighWaterMark += sOffset;
    }

    if( aMiniTrans != NULL )
    {
        sLogSize = sFSSignpost->mCommon.mHighWaterMark - STL_SIZEOF( smpPhyHdr );

        /**
         * column start position을 로깅한다.
         */
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_PAGE_ACCESS,
                               SMR_LOG_INIT_FIXED_PAGE_BODY,
                               SMR_REDO_TARGET_PAGE,
                               (stlChar *)sFSSignpost,
                               sLogSize,
                               aPageHandle->mPch->mTbsId,
                               aPageHandle->mPch->mPageId,
                               (stlChar*)sFSSignpost - sPage,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                                
/**
 * @brief 주어진 페이지 body를 compact를 위해 재초기화 한다 
 * @param[in,out] aPageHandle 초기화 할 페이지 Handle
 */
stlStatus smpResetBody( smpHandle * aPageHandle )
{
    stlInt32                 i;
    stlInt16                 sRtsCount = 0;
    void                   * sFrame = SMP_FRAME(aPageHandle);
    smpRts                 * sRts = SMP_GET_FIRST_RTS(sFrame);
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(sFrame);
    smpOffsetTableHeader   * sOffsetTableHeader = SMP_GET_OFFSET_TABLE_HEADER(sFrame);
    stlBool                  sUseFreeSlotList = SMP_CHECK_USE_FREE_SLOT_LIST(sOffsetTableHeader);

    /**
     * @todo Rts Extention 부하를 줄여야 한다.
     */
#if 0
    for( i = 0; i < SMP_GET_CURRENT_RTS_COUNT(sFrame); i++ )
    {
        /* Stable 상태이거나 Free 상태인것은 RTS 개수에서 제외하여 RTS 개수를 감소시킨다 */
        if( (SMP_RTS_IS_FREE(sRts) != STL_TRUE) &&
            (SMP_RTS_IS_STABLE(sRts) != STL_TRUE) )
        {
            sRtsCount++;
        }
        sRts++;
    }

    if( sRtsCount <= sSignpost->mMinRtsCount )
    {
        sRtsCount = sSignpost->mMinRtsCount;
    }
    else
    {
        for( i = 2; i < sSignpost->mMaxRtsCount; i = i * 2 )
        {
            if( i >= sRtsCount )
            {
                break;
            }
        }
        sRtsCount = i;
    }
#endif

    sRtsCount = sSignpost->mMinRtsCount;
    sSignpost->mCurRtsCount = sSignpost->mMinRtsCount;
    sSignpost->mHintRts = 0;
    sSignpost->mLowWaterMark = SMP_PAGE_SIZE - STL_SIZEOF(smpPhyTail);
    sSignpost->mReclaimedSlotSpace = 0;
    sSignpost->mActiveSlotCount = 0;

    sRts = SMP_GET_FIRST_RTS(sFrame);
    for( i = 0; i < sRtsCount; i++ )
    {
        SMP_INIT_RTS(sRts);
        sRts++;
    }

    sOffsetTableHeader = (smpOffsetTableHeader*)sRts;
    sOffsetTableHeader->mCurrentSlotCount = 0;
    if( sUseFreeSlotList == STL_TRUE )
    {
        sOffsetTableHeader->mFirstFreeOffsetSlot = SMP_USE_FREE_SLOT_LIST_YES | SMP_OFFSET_SLOT_NULL;
    }
    else
    {
        sOffsetTableHeader->mFirstFreeOffsetSlot = SMP_USE_FREE_SLOT_LIST_NO | SMP_OFFSET_SLOT_NULL;
    }

    sSignpost->mCommon.mHighWaterMark =  (stlChar*)(sOffsetTableHeader + 1) - (stlChar*)sFrame;
 
    return STL_SUCCESS;
}


/**
 * @brief 주어진 sequence의 RTS에 기록된 Transaction의 이전 버전을 구해 반환한다.
 * @param[in,out] aPageHandle 초기화 할 페이지 Handle
 * @param[in] aPrevPid RTS의 이전 버전이 담긴 undo page의 id
 * @param[in] aPrevOffset RTS의 이전 버전이 담긴 undo page의 record sequence
 * @param[in] aPrevVerCnt RTS의 이전 버전의 총 개수
 * @param[in] aTargetVerNo 찾고자 하는 Rts 버전의 번호
 * @param[in] aMyStmtViewScn 나의 Statement ViewScn
 * @param[out] aCommitScn 찾고자 하는 버전의 commit scn
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpGetRtsVerCommitScn( smpHandle  * aPageHandle,
                                 smlPid       aPrevPid,
                                 stlInt16     aPrevOffset,
                                 stlUInt8     aPrevVerCnt,
                                 stlUInt8     aTargetVerNo,
                                 smlScn       aMyStmtViewScn,
                                 smlScn     * aCommitScn,
                                 smlEnv     * aEnv )
{
    smpHandle   sPageHandle;
    smlRid      sUndoRid;
    stlInt16    sUndoRecType;
    stlInt8     sUndoRecClass;
    stlInt16    sUndoSize;
    stlChar   * sUndoRec;
    stlInt16    sState = 0;
    smlScn      sPrevCommitScn;
    stlUInt8    sCurVerNo = aPrevVerCnt - 1;

    STL_ASSERT( aPrevVerCnt > aTargetVerNo );

    sUndoRid = ((smlRid){ SML_MEMORY_UNDO_SYSTEM_TBS_ID,
                          aPrevOffset,
                          aPrevPid });

    while( STL_TRUE )
    {
        STL_TRY( smpFix( SML_MEMORY_UNDO_SYSTEM_TBS_ID,
                         sUndoRid.mPageId,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );
        sState = 1;

        STL_TRY( smxlGetUndoRecord( &sPageHandle,
                                    &sUndoRid,
                                    &sUndoRecType,
                                    &sUndoRecClass,
                                    &sUndoRec,
                                    &sUndoSize ) == STL_SUCCESS );

        STL_ASSERT( (sUndoRecType == SMP_UNDO_LOG_RTS_VERSIONING) &&
                    (sUndoRecClass == SMG_COMPONENT_PAGE_ACCESS) );

        STL_WRITE_INT64( aCommitScn, sUndoRec );
        sUndoRec += STL_SIZEOF( smlScn );
        if( sCurVerNo == aTargetVerNo )
        {
            break;
        }

        STL_WRITE_INT64( &sPrevCommitScn, sUndoRec );
        sUndoRec += STL_SIZEOF( smlScn );
        if( sCurVerNo == aTargetVerNo + 1 )
        {
            *aCommitScn = sPrevCommitScn;
            break;
        }
        if( sPrevCommitScn <= aMyStmtViewScn )
        {
            *aCommitScn = 0;
            break;
        }

        STL_WRITE_INT32( &sUndoRid.mPageId, sUndoRec  );
        sUndoRec += STL_SIZEOF( smlPid );
        STL_WRITE_INT16( &sUndoRid.mOffset, sUndoRec  );
        sUndoRec += STL_SIZEOF( stlInt16 );

        sState = 0;
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
        sCurVerNo--;
    } /* for */

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
 * @brief 주어진 sequence의 RTS에 기록된 Transaction의 Commit SCN을 구해 반환한다.
 * @param[in] aTbsId Tablespace Identifier
 * @param[in,out] aPageHandle 초기화 할 페이지 Handle
 * @param[in] aRtsSeq RTS의 sequence
 * @param[in] aRtsVerNo RTS의 version number
 * @param[in] aMyStmtViewScn 내 Statement view scn
 * @param[out] aWaitTransId commit이 안된 경우 기다려야 되는 transaction ID
 * @param[out] aCommitScn Transaction의 Commit SCN
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpGetCommitScn( smlTbsId      aTbsId,
                           smpHandle   * aPageHandle,
                           stlUInt8      aRtsSeq,
                           stlUInt8      aRtsVerNo,
                           smlScn        aMyStmtViewScn,
                           smxlTransId * aWaitTransId,
                           smlScn      * aCommitScn,
                           smlEnv      * aEnv )
{
    smpRts    * sRts;
    smlScn      sCommitScn;
    smpHandle   sPageHandle;
    stlInt32    sState = 0;
    smlScn      sAgableScn = smxlGetAgableTbsScn( aTbsId, aEnv );
    stlBool     sIsSuccess;

    STL_PARAM_VALIDATE( aRtsSeq < SMP_MAX_RTS_COUNT, KNL_ERROR_STACK((knlEnv*)aEnv) );
    STL_PARAM_VALIDATE( aRtsVerNo < SMP_MAX_RTS_VERSION_COUNT, KNL_ERROR_STACK((knlEnv*)aEnv) );

    sRts = SMP_GET_FIRST_RTS(aPageHandle->mFrame) + aRtsSeq;
    *aWaitTransId = SML_INVALID_TRANSID;

    if( aRtsVerNo == sRts->mPrevVerCnt )
    {
        if( SMP_RTS_IS_STABLE(sRts) )
        {
            sCommitScn = 0;
        }
        else if( SMP_RTS_IS_STAMPED(sRts) )
        {
            if( sRts->mScn <= sAgableScn )
            {
                /**
                 * Delayed Stamping을 시도한다.
                 */
                if( aPageHandle->mPch != NULL )
                {
                    /**
                     * Lock 상승이 성공하는 경우에만 Stamping을 한다.
                     */
                    STL_TRY( knlEscalateLatch( &aPageHandle->mPch->mPageLatch,
                                               &sIsSuccess,
                                               KNL_ENV( aEnv ) )
                             == STL_SUCCESS );

                    if( sIsSuccess == STL_TRUE )
                    {
                        /* 
                         knlLogMsg(NULL, aEnv, KNL_LOG_LEVEL_INFO, "R1: %d, %d, %ld, %ld",
                         aPageHandle->mPch->mPageId, aRtsSeq, sRts->mScn, smxlGetAgableScn(aEnv));
                         */
                        SMP_RTS_SET_STABLE( sRts );

                        STL_TRY( smpSetDirty( aPageHandle, aEnv ) == STL_SUCCESS );
                    }
                }
                else
                {
                    /**
                     * 임시 버퍼는 락 상승없이 Stamping한다.
                     */
                    SMP_RTS_SET_STABLE( sRts );
                }
            }

            sCommitScn = sRts->mScn;
        }
        else
        {
            STL_ASSERT( SMP_RTS_IS_NOT_STAMPED(sRts) );
            STL_TRY( smxlGetCommitScn( sRts->mTransId,
                                       SML_MAKE_REAL_SCN(sRts->mScn),
                                       &sCommitScn,
                                       aEnv ) == STL_SUCCESS );
           
            if( sCommitScn != SML_INFINITE_SCN )
            {
                /**
                 * Delayed Stamping을 시도한다.
                 */
                if( aPageHandle->mPch != NULL )
                {
                    /**
                     * Lock 상승이 성공하는 경우에만 Stamping을 한다.
                     */
                    STL_TRY( knlEscalateLatch( &aPageHandle->mPch->mPageLatch,
                                               &sIsSuccess,
                                               KNL_ENV( aEnv ) )
                             == STL_SUCCESS );

                    if( sIsSuccess == STL_TRUE )
                    {
                        if( sCommitScn <= sAgableScn )
                        {
                            /* 
                             knlLogMsg(NULL, aEnv, KNL_LOG_LEVEL_INFO, "R2: %d, %d, %ld, %ld, %ld, %ld",
                             aPageHandle->mPch->mPageId, aRtsSeq, sRts->mScn, SML_MAKE_REAL_SCN(sRts->mScn), smxlGetAgableScn(aEnv),smxlGetAgableViewScn(aEnv));
                             */
                            SMP_RTS_SET_STABLE( sRts );
                        }
                        else
                        {
                            SMP_RTS_SET_SCN( sRts, sCommitScn );
                        }
                        STL_TRY( smpSetDirty( aPageHandle, aEnv ) == STL_SUCCESS );
                    }
                }
                else
                {
                    /**
                     * 임시 버퍼는 락 상승없이 Stamping한다.
                     */
                    if( sCommitScn <= sAgableScn )
                    {
                        SMP_RTS_SET_STABLE( sRts );
                    }
                    else
                    {
                        SMP_RTS_SET_SCN( sRts, sCommitScn );
                    }
                }
            }
            else
            {
                *aWaitTransId = sRts->mTransId;
            }
        }
    }
    else if( aRtsVerNo == sRts->mPrevVerCnt - 1 )
    {
        /* 바로 이전 RTS version의 Tx면 prev commit SCN 을 읽는다 */
        sCommitScn = sRts->mPrevCommitScn;
    }
    else
    {
        /* 2이상의 version no를 가지는 것들은 prev rid로 가서 알아온다 */
        if( sRts->mPrevCommitScn <= aMyStmtViewScn )
        {
            /* 이전 rts commit scn 리스트에 가 볼 필요없이 agable 상태임 */
            sCommitScn = 0;
        }
        else
        {
            STL_ASSERT( sRts->mPrevVerCnt > aRtsVerNo );

            /* 이전 rts commit scn 리스트에 가 봐야 함 */
            STL_TRY( smpGetRtsVerCommitScn( aPageHandle,
                                            sRts->mPrevPid,
                                            sRts->mPrevOffset,
                                            sRts->mPrevVerCnt,
                                            aRtsVerNo,
                                            aMyStmtViewScn,
                                            &sCommitScn,
                                            aEnv ) == STL_SUCCESS );
        }
    }

    *aCommitScn = sCommitScn;

    return STL_SUCCESS;
 
    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}


/**
 * @brief 주어진 Page에서 logical header의 위치를 반환한다.
 * @param[in] aPageHandle Logical header를 구할 페이지의 Handle
 */
inline stlChar * smpGetLogicalHeader( smpHandle * aPageHandle )
{
    return (stlChar*)(((smpOrderedSlotSignpost*)(SMP_GET_SIGNPOST(SMP_FRAME(aPageHandle)))) + 1);
}

/**
 * @brief 주어진 Page에 최소 RTS의 개수를 반환한다.
 * @param[in] aPageHandle RTS 개수를 구할 페이지의 Handle
 */
inline stlUInt8 smpGetMinRtsCount( smpHandle * aPageHandle )
{
    return SMP_GET_MINIMUM_RTS_COUNT(aPageHandle->mFrame);
}

/**
 * @brief 주어진 Page에 최대 RTS의 개수를 반환한다.
 * @param[in] aPageHandle RTS 개수를 구할 페이지의 Handle
 */
inline stlUInt8 smpGetMaxRtsCount( smpHandle * aPageHandle )
{
    return SMP_GET_MAXIMUM_RTS_COUNT(aPageHandle->mFrame);
}

/**
 * @brief 주어진 Page에 최대 RTS의 개수를 반환한다.
 * @param[in] aPageHandle RTS 개수를 구할 페이지의 Handle
 */
inline stlUInt8 smpGetCurRtsCount( smpHandle * aPageHandle )
{
    return SMP_GET_CURRENT_RTS_COUNT(aPageHandle->mFrame);
}
/**
 * @brief Page 내에서 수용가능한 RTS의 개수를 배로 늘린다. 
 * @param[in] aPageHandle RTS 개수를 늘릴 페이지의 Handle
 * @param[out] aIsSuccess 성공 여부
 */
stlStatus smpExtendRts( smpHandle * aPageHandle,
                        stlBool   * aIsSuccess )
{
    stlChar                  sTempPage[SMP_PAGE_SIZE];
    stlUInt8                 sBeforeRTSCount;
    stlUInt8                 sAfterRTSCount;
    stlUInt16                sExtendSize;
    stlUInt16                sOffsetTableSize;
    smpRts                 * sRts;
    stlUInt32                i;
    smpOrderedSlotSignpost * sTmpSignpost;
    smpOrderedSlotSignpost * sOrgSignpost;
    smpOffsetTableHeader   * sTmpOffsetTable;
    smpOffsetTableHeader   * sOrgOffsetTable;

    *aIsSuccess = STL_FALSE;

    stlMemcpy(sTempPage, aPageHandle->mFrame, SMP_PAGE_SIZE);
    sTmpSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(sTempPage);
    sTmpOffsetTable = SMP_GET_OFFSET_TABLE_HEADER(sTempPage);
    STL_ASSERT( sTmpSignpost->mCommon.mPageMgmtType == SMP_ORDERED_SLOT_TYPE );

    if( sTmpSignpost->mCurRtsCount < sTmpSignpost->mMaxRtsCount )
    {
        sBeforeRTSCount =  sTmpSignpost->mCurRtsCount;
        for( i = 2; i < sTmpSignpost->mMaxRtsCount; i = (i << 1) )
        {
            if( i > sBeforeRTSCount )
            {
                break;
            }
        }

        sAfterRTSCount = (i > sTmpSignpost->mMaxRtsCount) ? sTmpSignpost->mMaxRtsCount : i;
        sExtendSize = (sAfterRTSCount - sBeforeRTSCount) * STL_SIZEOF(smpRts);
        if( sTmpSignpost->mLowWaterMark - sTmpSignpost->mCommon.mHighWaterMark >= sExtendSize )
        {
            sOrgSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);
            sOrgSignpost->mCurRtsCount = sAfterRTSCount;

            sRts = SMP_GET_FIRST_RTS(aPageHandle->mFrame);
            sRts += sBeforeRTSCount;
            for( i = 0; i < sAfterRTSCount - sBeforeRTSCount; i++ )
            {
                SMP_INIT_RTS(sRts);
                sRts++;
            }

            sOrgOffsetTable = SMP_GET_OFFSET_TABLE_HEADER(aPageHandle->mFrame);
            sOffsetTableSize = STL_SIZEOF(smpOffsetTableHeader) + 
                (STL_SIZEOF(smpOffsetSlot) * sTmpOffsetTable->mCurrentSlotCount);
            stlMemcpy( sOrgOffsetTable, sTmpOffsetTable, sOffsetTableSize );
            sOrgSignpost->mCommon.mHighWaterMark += sExtendSize;

            *aIsSuccess = STL_TRUE;
        }
    }

    return STL_SUCCESS;
}


/**
 * @brief 주어진 모든 RTS들의 RowItem들의 Stabilize을 한다. 
 * @param[in,out] aPageHandle Stamping을 할  페이지 Handle
 * @param[in] aRtsSeq stamping이 필요한 RTS sequence
 * @param[in] aCommitScn stamping할 commit scn
 * @param[in] aStabilizeRow Row에 Stable 상태를 세팅하는 함수포인터
 * @param[in,out] aEnv Storage Manager Environment
 */
stlStatus smpStabilizeRtsRowItems( smpHandle           * aPageHandle,
                                   stlUInt8              aRtsSeq,
                                   smlScn                aCommitScn,
                                   smpStabilizeRowFunc   aStabilizeRow,
                                   smlEnv              * aEnv )
{
    stlUInt32       i;
    stlUInt32       j;
    stlInt16        sReferenceCount = 0;
    stlInt16        sTotalReferenceCount = 0;
    stlChar       * sRow;
    smpOffsetSlot * sOffset;
    smpRts        * sRts = SMP_GET_FIRST_RTS(SMP_FRAME(aPageHandle)) + aRtsSeq;
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);

    /* stamp row */
    for( i = 0; i < SMP_RTS_OFFSET_CACHE_COUNT; i++ )
    {
        if( sRts->mOffsetCache[i] != 0 )
        {
            STL_ASSERT(sRts->mOffsetCache[i] >= sSignpost->mCommon.mHighWaterMark);
            sRow = (stlChar*)SMP_FRAME(aPageHandle) + sRts->mOffsetCache[i];
            aStabilizeRow( sRow, aRtsSeq, sRts->mPrevVerCnt, aCommitScn, &sReferenceCount );
            sTotalReferenceCount += sReferenceCount;
        }
    }

    if( (SMP_RTS_GET_REF_COUNT(sRts) > sTotalReferenceCount) ||
        (sRts->mPrevVerCnt > 0) )
    {
        sOffset = SMP_GET_NTH_OFFSET_SLOT(SMP_FRAME(aPageHandle), 0);
        for( i = 0; i < SMP_PAGE_GET_ROWITEM_COUNT(SMP_FRAME(aPageHandle)); i++ )
        {
            for( j = 0; j < SMP_RTS_OFFSET_CACHE_COUNT; j++ )
            {
                if( sRts->mOffsetCache[j] == *sOffset )
                {
                    break;
                }
            }

            if( (SMP_OFFSET_SLOT_IS_FREE(sOffset) != STL_TRUE) &&
                (j == SMP_RTS_OFFSET_CACHE_COUNT) )
            {
                /* free slot이 아니고 위에서 stamping 한 적이 없으면 */
                STL_ASSERT((*sOffset) >= sSignpost->mCommon.mHighWaterMark);
                sRow = (stlChar*)SMP_FRAME(aPageHandle) + *sOffset;
                aStabilizeRow( sRow, aRtsSeq, sRts->mPrevVerCnt, aCommitScn, &sReferenceCount );
                sTotalReferenceCount += sReferenceCount;
            }
            sOffset++;
        }
    }

    KNL_ASSERT( SMP_RTS_GET_REF_COUNT(sRts) == sTotalReferenceCount,
                aEnv,
                ("RID(%d,%d,%d),RtsSeq(%d), sRtsRefCount(%d), sStampedRefCount(%d),Page\n%s",
                 smpGetTbsId(aPageHandle), 0, smpGetPageId(aPageHandle),
                 aRtsSeq, SMP_RTS_GET_REF_COUNT(sRts), sTotalReferenceCount,
                 knlDumpBinaryForAssert( SMP_FRAME(aPageHandle),
                                         SMP_PAGE_SIZE,
                                         &aEnv->mLogHeapMem,
                                         KNL_ENV(aEnv))) );

    sRts->mRefCount = 0;

    for( i = 0; i < SMP_RTS_OFFSET_CACHE_COUNT; i++ )
    {
        sRts->mOffsetCache[i] = 0;
    }

    return STL_SUCCESS;
}


/**
 * @brief 주어진 RTS의 stamping을 시도한다. 
 * @param[in] aMiniTrans logging이 필요한 경우에 사용할 mini transaction
 * @param[in] aTbsId Tablespace Identifier
 * @param[in,out] aPageHandle stamping할 RTS를 가진 page Handle
 * @param[in] aRtsSeq Stamping할 RTS의 순번
 * @param[in] aStabilizeRow RTS가 Stable 할 경우에 row들에 stable 상태로 세팅하는 함수포인터
 * @param[in,out] aEnv Storage Manager Environment
 */
stlStatus smpTryStampRts( smxmTrans           * aMiniTrans,
                          smlTbsId              aTbsId,
                          smpHandle           * aPageHandle,
                          stlUInt8              aRtsSeq,
                          smpStabilizeRowFunc   aStabilizeRow,
                          smlEnv              * aEnv )
{
    smxlTransId  sWaitTransId;
    smlScn       sCommitScn;
    smpRts     * sRts = SMP_GET_FIRST_RTS(aPageHandle->mFrame) + aRtsSeq;
    smlRid       sUndoRid;
    stlChar      sLogBuf[(STL_SIZEOF(smlScn) * 2) + STL_SIZEOF(smlPid) + STL_SIZEOF(stlInt16)];
    stlChar    * sLogPtr = sLogBuf;
    smlScn       sAgableScn;
    stlUInt8     sPrevVerCnt;
 
    if( SMP_RTS_IS_NOT_STAMPED(sRts) )
    {
        /* commit scn 구하면서 RTS stamping도 수행함 */
        STL_TRY( smpGetCommitScn( aTbsId,
                                  aPageHandle,
                                  aRtsSeq,
                                  sRts->mPrevVerCnt, /* RtsVerNo */
                                  0, /* MyStmtViewScn : 안씀 */
                                  &sWaitTransId,
                                  &sCommitScn,
                                  aEnv ) == STL_SUCCESS );
    }

    if( aStabilizeRow != NULL )
    {
        if( SMP_RTS_IS_STABLE(sRts) == STL_TRUE )
        {
            STL_TRY( smpStabilizeRtsRowItems( aPageHandle,
                                              aRtsSeq,
                                              0,
                                              aStabilizeRow,
                                              aEnv ) == STL_SUCCESS );
            SMP_INIT_RTS(sRts);
            STL_TRY( smpSetDirty( aPageHandle, aEnv ) == STL_SUCCESS );
        }
        else if(SMP_RTS_IS_STAMPED(sRts) == STL_TRUE )
        {
            /* 해당 RTS를 reference하는 모든 rowitem들을 Stamping한다 */
            sAgableScn = smxlGetAgableTbsScn( smpGetTbsId(aPageHandle), aEnv );
            sCommitScn = sRts->mScn;
            sPrevVerCnt = sRts->mPrevVerCnt;
            /* aMiniTrans == NULL 인 경우 :
             * 1. redo 함수 중 호출
             * 2. prev version을 만들지 않는 table 같은 경우
             */
            if( (sAgableScn >= sCommitScn) || (aMiniTrans == NULL) )
            {
                STL_TRY( smpStabilizeRtsRowItems( aPageHandle,
                                                  aRtsSeq,
                                                  sCommitScn,
                                                  aStabilizeRow,
                                                  aEnv ) == STL_SUCCESS );
                SMP_INIT_RTS(sRts);
            }
            else if( sRts->mPrevVerCnt < SMP_MAX_RTS_VERSION_COUNT - 1 )
            {
                /**
                 * sAgableScn < sCommitScn && aMiniTrans != NULL
                 * UNDO LOG : commit scn + prev commit scn + prev pid + prev offset */
                STL_WRITE_INT64(sLogPtr, &sRts->mScn);
                sLogPtr += STL_SIZEOF(smlScn);
                STL_WRITE_INT64(sLogPtr, &sRts->mPrevCommitScn);
                sLogPtr += STL_SIZEOF(smlScn);
                STL_WRITE_INT32(sLogPtr, &sRts->mPrevPid);
                sLogPtr += STL_SIZEOF(smlPid);
                STL_WRITE_INT16(sLogPtr, &sRts->mPrevOffset);
                sLogPtr += STL_SIZEOF(stlInt16);
 
                STL_TRY( smxlInsertUndoRecord( aMiniTrans,
                                   SMG_COMPONENT_PAGE_ACCESS,
                                   SMP_UNDO_LOG_RTS_VERSIONING,
                                   sLogBuf,
                                   sLogPtr - sLogBuf,
                                   ((smlRid){ aPageHandle->mPch->mTbsId,
                                              aRtsSeq,
                                              aPageHandle->mPch->mPageId }),
                                   &sUndoRid,
                                   aEnv ) == STL_SUCCESS );

                SMP_INIT_RTS(sRts);
                sRts->mPrevCommitScn = sCommitScn;
                sRts->mPrevPid = sUndoRid.mPageId;
                sRts->mPrevOffset = sUndoRid.mOffset;
                sRts->mPrevVerCnt = sPrevVerCnt + 1;

                STL_TRY( smpSetDirty( aPageHandle, aEnv ) == STL_SUCCESS );
            }
            else
            {
                /* Max version count에 도달했다. stable 해 질때 까지 이 rts 재사용 못함 */
            }
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Tx가 사용중인 RTS나 Free 상태의 RTS를 찾아 반환한다. 
 * 없으면 RTS 개수의 확장을 시도한다. 실패하면 aRtsSeq에 SMP_RTS_SEQ_NULL을 리턴한다
 * @param[in] aMiniTrans rts stamping이 필요한 경우에 사용할 mini transaction
 * @param[in,out] aPageHandle RTS를 찾을 페이지 Handle
 * @param[in] aTransId Transaction의 ID
 * @param[in] aScn Transaction의 View SCN(SML_VIEW_SCN_MASK 세팅된) 또는 Commit SCN. 
 * @param[in] aStabilizeRow RTS들중 Commit된 것들을 재사용할때 row에 stable 상태를 세팅하는 함수포인터
 * @param[out] aRtsSeq 찾은 RTS의 Sequence 번호
 * @param[in,out] aEnv Storage Manager Environment
 */
stlStatus smpAllocRts( smxmTrans           * aMiniTrans,
                       smpHandle           * aPageHandle,
                       smxlTransId           aTransId,
                       smlScn                aScn,
                       smpStabilizeRowFunc   aStabilizeRow,
                       stlUInt8            * aRtsSeq,
                       smlEnv              * aEnv )
{
    stlUInt32                i;
    stlUInt16                sRtsCount;
    smpRts                 * sFirstRts;
    smpRts                 * sRts;
    stlBool                  sIsSuccess;
    stlUInt8                 sCurrentRtsSeq = SMP_RTS_SEQ_NULL;
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);

    sFirstRts = SMP_GET_FIRST_RTS(aPageHandle->mFrame);

    STL_ASSERT( SMXL_IS_UNDO_BOUND_TRANS(aTransId) == STL_TRUE );
    STL_ASSERT( sSignpost->mCommon.mPageMgmtType == SMP_ORDERED_SLOT_TYPE );
    sRtsCount = sSignpost->mCurRtsCount;

    /* 1. Check Tx is Already exist */
    sRts = sFirstRts;
    for( i = 0; i < sRtsCount; i++ )
    {
        if( (sRts->mTransId == aTransId) && (sRts->mScn == aScn) )
        {
            sCurrentRtsSeq = i;
            /* found */
            STL_THROW( RAMP_FINISH_FINDING );
        }
        sRts++;
    }
    
    /* 2. Check Free RTS */
    sRts = sFirstRts + sSignpost->mHintRts;
    sCurrentRtsSeq = sSignpost->mHintRts;
    for( i = 0; i < sRtsCount; i++ )
    {
        if( SMP_RTS_IS_FREE(sRts) )
        {
            SMP_RESET_RTS(sRts);
            sSignpost->mHintRts = (sCurrentRtsSeq + 1) % sRtsCount;
            /* found */
            STL_THROW( RAMP_FINISH_FINDING );
        }
        else if( SMP_RTS_IS_STABLE(sRts) )
        {
            STL_TRY( smpTryStampRts( aMiniTrans,
                                     smpGetTbsId(aPageHandle),
                                     aPageHandle,
                                     sCurrentRtsSeq,
                                     aStabilizeRow,
                                     aEnv ) == STL_SUCCESS );
            STL_ASSERT( SMP_RTS_IS_FREE(sRts) );
            sSignpost->mHintRts = (sCurrentRtsSeq + 1) % sRtsCount;
            /* found */
            STL_THROW( RAMP_FINISH_FINDING );
        }

        sCurrentRtsSeq = (sCurrentRtsSeq + 1) % sRtsCount;
        sRts = sFirstRts + sCurrentRtsSeq;
    }

    /* 3. 이미 stamping 되어있는 RTS의 재사용 시도 */
    sRts = sFirstRts + sSignpost->mHintRts;
    sCurrentRtsSeq = sSignpost->mHintRts;
    for( i = 0; i < sRtsCount; i++ )
    {
        STL_TRY( smpTryStampRts( aMiniTrans,
                                 smpGetTbsId(aPageHandle),
                                 aPageHandle,
                                 sCurrentRtsSeq,
                                 aStabilizeRow,
                                 aEnv ) == STL_SUCCESS );
        if( SMP_RTS_IS_FREE(sRts) == STL_TRUE )
        {
            /* stamping 과정에서 rts가 reset 되어 나옴 */
            sSignpost->mHintRts = (sCurrentRtsSeq + 1) % sRtsCount;
            /* found */
            STL_THROW( RAMP_FINISH_FINDING );
        }
        sCurrentRtsSeq = (sCurrentRtsSeq + 1) % sRtsCount;
        sRts = sFirstRts + sCurrentRtsSeq;
    }

    /* 4. alloc 실패.  extend 시도 */

    if( sRtsCount < sSignpost->mMaxRtsCount )
    {
        /* 5. try extend rts */
        smpExtendRts( aPageHandle, &sIsSuccess );
        if( sIsSuccess != STL_TRUE )
        {
            /* not found */
            sCurrentRtsSeq = SMP_RTS_SEQ_NULL;
        }
        else
        {
            /* found */
            sCurrentRtsSeq = i;
        }
    }
    else
    {
        /* RTS가 최대치에 도달했다 */
        sCurrentRtsSeq = SMP_RTS_SEQ_NULL;
    }

    STL_RAMP( RAMP_FINISH_FINDING );

    *aRtsSeq = sCurrentRtsSeq;

    STL_TRY( smpValidateOffsetTableHeader( SMP_FRAME(aPageHandle),
                                           aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Tx가 사용중인 RTS나 Free 상태의 RTS를 찾아 반환한다. 페이지간 RTS 복사이에 사용한다
 * 없으면 RTS 개수의 확장을 시도한다. 실패하면 aRtsSeq에 SMP_RTS_SEQ_NULL을 리턴한다
 * @param[in,out] aPageHandle RTS를 찾을 페이지 Handle
 * @param[in] aTransId Transaction의 ID
 * @param[in] aScn Transaction의 View SCN(SML_VIEW_SCN_MASK 세팅된) 또는 Commit SCN. 
 * @param[out] aRtsSeq 찾은 RTS의 Sequence 번호
 * @param[in,out] aEnv Storage Manager Environment
 */
stlStatus smpAllocRtsForCopy( smpHandle          * aPageHandle,
                              smxlTransId          aTransId,
                              smlScn               aScn,
                              stlUInt8             aRtsSeq,
                              smlEnv             * aEnv )
{
    stlUInt16                sRtsCount;
    stlBool                  sIsSuccess;
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);

    /* RTS Alloc 후 insert 하다 실패하여 compact 하면 aTransId가 SML_INVALID_TRANS 일 수 있다 */
    STL_ASSERT( (SMXL_IS_UNDO_BOUND_TRANS(aTransId) == STL_TRUE) ||
                (aTransId == SML_INVALID_TRANSID) );

    STL_ASSERT( sSignpost->mCommon.mPageMgmtType == SMP_ORDERED_SLOT_TYPE );
    sRtsCount = sSignpost->mCurRtsCount;

    STL_ASSERT( sRtsCount <= sSignpost->mMaxRtsCount );

    if( sRtsCount <= aRtsSeq )
    {
        /* extend 시도 */
        smpExtendRts( aPageHandle, &sIsSuccess );
        STL_ASSERT( (sIsSuccess == STL_TRUE) && (aRtsSeq < sSignpost->mCurRtsCount) );
    }
    else
    {
        /* 주어진 rts를 할당한다 */
    }

    STL_TRY( smpValidateOffsetTableHeader( SMP_FRAME(aPageHandle),
                                           aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Compact와 page cop시에 RowItem과 Rts를 bind 시킨다
 * @param[in,out] aPageHandle 페이지 Handle
 * @param[in] aSlot Rts를 사용할 RowItem
 * @param[in] aTransId Transaction의ID
 * @param[in] aScn Transaction의 View SCN(SML_VIEW_SCN_MASK 세팅된) 또는 Commit SCN. 
 * @param[in] aRtsSeq 사용할 Rts의 sequence 번호
 * @param[in] aRtsVerNo 사용할 Rts에서의 version no.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpBeginUseRtsForCopy( smpHandle          * aPageHandle,
                                 stlChar            * aSlot,
                                 smxlTransId          aTransId,
                                 smlScn               aScn,
                                 stlUInt8             aRtsSeq,
                                 stlUInt8             aRtsVerNo,
                                 smlEnv             * aEnv )
{
    stlInt16    i;
    smpRts    * sRts = SMP_GET_FIRST_RTS(aPageHandle->mFrame) + aRtsSeq;
    stlInt16    sSlotOffset = aSlot - (stlChar*)aPageHandle->mFrame;
    smlScn      sScn = sRts->mScn;
    stlBool     sIsMatch = STL_FALSE;

    /* allocRts 성공 후 rowitem을 alloc 하려다 실패해서 compact하는 경우 aTransId가 invalid 할 수 있다 */
    STL_ASSERT( (SMXL_IS_UNDO_BOUND_TRANS(aTransId) == STL_TRUE) ||
                (aTransId == SML_INVALID_TRANSID) );
    KNL_ASSERT( SMP_GET_CURRENT_RTS_COUNT( SMP_FRAME(aPageHandle) ) > aRtsSeq,
                (knlEnv*)aEnv,
                ("RTS SEQ(%d), RTS COUNT(%d)",
                 aRtsSeq, SMP_GET_CURRENT_RTS_COUNT( SMP_FRAME(aPageHandle) )) );

    if( (sScn == aScn) && (sRts->mTransId == aTransId) )
    {
        sIsMatch = STL_TRUE;
    }

    if( sIsMatch == STL_TRUE )
    {
        if( sRts->mPrevVerCnt == aRtsVerNo )
        {
            for( i = 0; i < SMP_RTS_OFFSET_CACHE_COUNT; i++ )
            {
                if( sRts->mOffsetCache[i] == 0 )
                {
                    sRts->mOffsetCache[i] = sSlotOffset;
                    break;
                }
            }
        }
    }
    else
    {
        KNL_ASSERT( SMP_RTS_IS_FREE(sRts) == STL_TRUE,
                    (knlEnv*)aEnv,
                    ("RTS RefCount(%d), TxId(%ld), Scn(%ld)",
                     sRts->mRefCount,
                     sRts->mTransId,
                     sRts->mScn) );

        sRts->mTransId = aTransId;
        sRts->mScn     = aScn;
        
        if( sRts->mPrevVerCnt == aRtsVerNo )
        {
            sRts->mOffsetCache[0] = sSlotOffset;
        }
    }

    if( sRts->mPrevVerCnt == aRtsVerNo )
    {
        SMP_RTS_INCREASE_REF_COUNT(sRts);
    }

    STL_TRY( smpValidateOffsetTableHeader( SMP_FRAME(aPageHandle),
                                           aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 RowItem과 Rts를 bind 시킨다
 * @param[in,out] aPageHandle 페이지 Handle
 * @param[in] aSlot Rts를 사용할 RowItem
 * @param[in] aTransId Transaction의ID
 * @param[in] aScn Transaction의 View SCN(SML_VIEW_SCN_MASK 세팅된) 또는 Commit SCN. 
 * @param[in] aRtsSeq 사용할 Rts의 sequence 번호
 * @param[out] aRtsVerNo 사용할 Rts에서의 version no.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpBeginUseRts( smpHandle          * aPageHandle,
                          stlChar            * aSlot,
                          smxlTransId          aTransId,
                          smlScn               aScn,
                          stlUInt8             aRtsSeq,
                          stlUInt8           * aRtsVerNo,
                          smlEnv             * aEnv )
{
    stlInt16    i;
    smpRts    * sRts = SMP_GET_FIRST_RTS(aPageHandle->mFrame) + aRtsSeq;
    stlInt16    sSlotOffset = aSlot - (stlChar*)aPageHandle->mFrame;
    smlScn      sScn = sRts->mScn;
    stlBool     sIsMatch = STL_FALSE;

    /* allocRts 성공 후 rowitem을 alloc 하려다 실패해서 compact하는 경우 aTransId가 invalid 할 수 있다 */
    STL_ASSERT( (SMXL_IS_UNDO_BOUND_TRANS(aTransId) == STL_TRUE) ||
                (aTransId == SML_INVALID_TRANSID) );
    KNL_ASSERT( SMP_GET_CURRENT_RTS_COUNT( SMP_FRAME(aPageHandle) ) > aRtsSeq,
                (knlEnv*)aEnv,
                ("RTS SEQ(%d), RTS COUNT(%d)",
                 aRtsSeq, SMP_GET_CURRENT_RTS_COUNT( SMP_FRAME(aPageHandle) )) );

    if( (sScn == aScn) && (sRts->mTransId == aTransId) )
    {
        sIsMatch = STL_TRUE;
    }

    if( sIsMatch == STL_TRUE )
    {
        for( i = 0; i < SMP_RTS_OFFSET_CACHE_COUNT; i++ )
        {
            if( sRts->mOffsetCache[i] == 0 )
            {
                sRts->mOffsetCache[i] = sSlotOffset;
                break;
            }
        }
    }
    else
    {
        KNL_ASSERT( SMP_RTS_IS_FREE(sRts) == STL_TRUE,
                    (knlEnv*)aEnv,
                    ("RTS RefCount(%d), TxId(%ld), Scn(%ld)",
                     sRts->mRefCount,
                     sRts->mTransId,
                     sRts->mScn) );

        sRts->mTransId        = aTransId;
        sRts->mScn            = aScn;
        sRts->mOffsetCache[0] = sSlotOffset;
    }
    SMP_RTS_INCREASE_REF_COUNT(sRts);
    *aRtsVerNo = sRts->mPrevVerCnt;

    STL_TRY( smpValidateOffsetTableHeader( SMP_FRAME(aPageHandle),
                                           aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 RowItem과 Rts를 unbind 시킨다
 * @param[in,out] aPageHandle 페이지 Handle
 * @param[in] aSlot Rts를 사용을 중지할 RowItem
 * @param[in] aRtsSeq 사용 중지 할 Rts의 sequence 번호
 * @param[in] aRtsVerNo 사용 중지 할 Rts의 Version 번호
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smpEndUseRts( smpHandle * aPageHandle,
                        stlChar   * aSlot,
                        stlUInt8    aRtsSeq,
                        stlUInt8    aRtsVerNo,
                        smlEnv    * aEnv )
{
    stlInt16          i;
    smpRts          * sRts = SMP_GET_FIRST_RTS(aPageHandle->mFrame) + aRtsSeq;
    stlInt16          sSlotOffset = aSlot - (stlChar*)aPageHandle->mFrame;

    if( aRtsVerNo == sRts->mPrevVerCnt )
    {
        STL_ASSERT( SMP_RTS_GET_REF_COUNT(sRts) > 0 );
        SMP_RTS_DECREASE_REF_COUNT(sRts);

        for( i = 0; i < SMP_RTS_OFFSET_CACHE_COUNT; i++ )
        {
            if(sRts->mOffsetCache[i] == sSlotOffset)
            {
                sRts->mOffsetCache[i] = 0;
                break;
            }
        }

        STL_TRY( smpValidateOffsetTableHeader( SMP_FRAME(aPageHandle),
                                               aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smpSetRtsPrevLink( smpHandle * aPageHandle,
                             stlUInt8    aRtsSeq,
                             smlPid      aPrevPid,
                             stlInt16    aPrevOffset,
                             stlInt16    aPrevVerCnt,
                             smlScn      aPrevCommitScn )
{
    smpRts * sRts = SMP_GET_FIRST_RTS(SMP_FRAME(aPageHandle)) + aRtsSeq;

    sRts->mPrevPid    = aPrevPid;
    sRts->mPrevOffset = aPrevOffset;
    sRts->mPrevVerCnt = aPrevVerCnt;
    sRts->mPrevCommitScn = aPrevCommitScn;

    return STL_SUCCESS;
}


/**
 * @brief 일반 table을 위해 slot을 할당 받는다. 실패할 경우 aSlot에 NULL을 리턴한다
 * @param[in,out] aPageHandle 페이지 Handle
 * @param[in] aSlotSize slot을 할당받을 RowItem의 크기
 * @param[in] aIsForTest 실제로 할당받지 않고 단지 테스트 용도인지 여부
 * @param[out] aSlot 할당받은 slot
 * @param[out] aSlotSeq 할당받은 slot의 순서
 */
stlStatus smpAllocSlot( smpHandle  * aPageHandle,
                        stlUInt16    aSlotSize,
                        stlBool      aIsForTest,
                        stlChar   ** aSlot,
                        stlInt16   * aSlotSeq )
{
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);
    smpOffsetTableHeader   * sOffsetTableHeader = SMP_GET_OFFSET_TABLE_HEADER( aPageHandle->mFrame );
    smpOffsetSlot          * sOffsetSlot;
    stlUInt16                sFreeOffsetSlot = sOffsetTableHeader->mFirstFreeOffsetSlot;

    STL_ASSERT( sSignpost->mCommon.mPageMgmtType == SMP_ORDERED_SLOT_TYPE );

    if( ((sFreeOffsetSlot & SMP_USE_FREE_SLOT_LIST_MASK) == SMP_USE_FREE_SLOT_LIST_YES) &&
       ((sFreeOffsetSlot & SMP_FREE_OFFSET_SLOT_SEQ_MASK) != SMP_OFFSET_SLOT_NULL) )
    {
        if( sSignpost->mCommon.mHighWaterMark + aSlotSize <= sSignpost->mLowWaterMark )
        {
            *aSlot = (stlChar*)aPageHandle->mFrame + (sSignpost->mLowWaterMark - aSlotSize);
            *aSlotSeq = sFreeOffsetSlot & SMP_FREE_OFFSET_SLOT_SEQ_MASK;
            if( aIsForTest != STL_TRUE )
            {
                sOffsetSlot = (smpOffsetSlot*)(sOffsetTableHeader + 1) + *aSlotSeq;
                sOffsetTableHeader->mFirstFreeOffsetSlot = *sOffsetSlot;
                *sOffsetSlot = *aSlot - (stlChar*)aPageHandle->mFrame;
                sSignpost->mLowWaterMark -= aSlotSize;
            }
        }
        else
        {
            *aSlot = NULL;
            *aSlotSeq = SMP_OFFSET_SLOT_NULL;
        }
    }
    else
    {
        if( sSignpost->mCommon.mHighWaterMark + STL_SIZEOF(smpOffsetSlot) + aSlotSize
            <= sSignpost->mLowWaterMark )
        {
            *aSlot = (stlChar*)aPageHandle->mFrame + (sSignpost->mLowWaterMark - aSlotSize);
            *aSlotSeq = sOffsetTableHeader->mCurrentSlotCount;
            if( aIsForTest != STL_TRUE )
            {
                sOffsetSlot =(smpOffsetSlot*)(sOffsetTableHeader + 1) + sOffsetTableHeader->mCurrentSlotCount;
                sOffsetTableHeader->mCurrentSlotCount++;
                sSignpost->mCommon.mHighWaterMark += STL_SIZEOF(smpOffsetSlot);
                sSignpost->mLowWaterMark -= aSlotSize;
                *sOffsetSlot = sSignpost->mLowWaterMark;
            }
        }
        else
        {
            *aSlot = NULL;
            *aSlotSeq = SMP_OFFSET_SLOT_NULL;
        }
    }

    return STL_SUCCESS;
}

stlStatus smpAllocFixedSlotArray( smpHandle              * aPageHandle,
                                  stlInt16                 aRowCount,
                                  stlBool                  aIsForTest,
                                  stlInt16                 aRowHdrLen,
                                  stlInt16                 aColCount,
                                  smpIsUsableFixedSlot     aIsUsableFixedSlot,
                                  stlChar               ** aSlotArray,
                                  stlInt16               * aSlotSeq )
{
    smpFixedSlotSignpost  * sFSSignpost = (smpFixedSlotSignpost*)SMP_GET_SIGNPOST( SMP_FRAME(aPageHandle) );
    stlInt16                i;
    stlUInt8                sColLenSize;
    stlUInt16             * sColLengthArray = (stlUInt16 *)SMP_GET_COLUMN_LENGTH( aPageHandle );
    stlInt64                sColLen;
    stlInt16                sSlotSeq;
    stlInt16                sColStartOffset;
    stlChar               * sPageBody = SMP_GET_FIRST_VALUE_POS( aPageHandle, aColCount );
    stlChar               * sSlot;
    stlBool                 sIsUsable;

    STL_ASSERT( sFSSignpost->mCommon.mPageMgmtType == SMP_FIXED_SLOT_TYPE );

    /**
     * Row header slot을 포함해서 slot array를 할당한다.
     */
    for( i = 0; i <= aColCount; i++ )
    {
        aSlotArray[i] = NULL;
    }

    /**
     * 재사용 가능한 slot이 존재하면 재사용
     */

    for( sSlotSeq = 0; sSlotSeq <= sFSSignpost->mAllocatedMaxSlotSeq; sSlotSeq++ )
    {
        sSlot = sPageBody + (sSlotSeq * aRowHdrLen);
        aIsUsableFixedSlot( sSlot, &sIsUsable );

        if( sIsUsable == STL_TRUE )
        {
            break;
        }
    }

    if( sSlotSeq == aRowCount )
    {
        STL_TRY_THROW( (sFSSignpost->mAllocatedMaxSlotSeq + 1) < aRowCount,
                       RAMP_NO_SPACE );

        sSlotSeq = sFSSignpost->mAllocatedMaxSlotSeq + 1;
    }

    sColStartOffset = aRowCount * aRowHdrLen;

    /* row header slot alloc */
    aSlotArray[0] = sPageBody + (sSlotSeq * aRowHdrLen);

    for( i = 1; i <= aColCount; i++ )
    {
        sColLen = sColLengthArray[i - 1];

        if( SMP_IS_FIXED_SLOT_CONTINUOUS_COLUMN( sColLen ) == STL_TRUE )
        {
            sColLen = SMP_GET_FIXED_SLOT_CONTINUOUS_COLUMN_LENGTH( sColLen );
            SMP_GET_CONT_COLUMN_LEN_SIZE( &sColLenSize );
        }
        else
        {
            SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );
        }

        aSlotArray[i] = sPageBody + sColStartOffset + (sSlotSeq * (sColLenSize + sColLen));
        sColStartOffset += aRowCount * (sColLen + sColLenSize);
    }

    if( aIsForTest == STL_FALSE )
    {
        sFSSignpost->mActiveSlotCount++;

        if( sSlotSeq == (sFSSignpost->mAllocatedMaxSlotSeq + 1) )
        {
            sFSSignpost->mAllocatedMaxSlotSeq++;
        }
    }

    *aSlotSeq = sSlotSeq;
    
    STL_RAMP( RAMP_NO_SPACE );

    return STL_SUCCESS;
}

stlStatus smpAllocFixedNthSlotArray( smpHandle  * aPageHandle,
                                     stlInt16     aSlotSeq,
                                     stlInt16     aRowCount,
                                     stlBool      aIsForTest,
                                     stlInt16     aRowHdrLen,
                                     stlInt16     aColCount,
                                     stlChar   ** aSlotArray )
{
    smpFixedSlotSignpost  * sFSSignpost = (smpFixedSlotSignpost*)SMP_GET_SIGNPOST( SMP_FRAME(aPageHandle) );
    stlInt16                i;
    stlUInt8                sColLenSize;
    stlUInt16             * sColLengthArray = (stlUInt16 *)SMP_GET_COLUMN_LENGTH( aPageHandle );
    stlInt64                sColLen;
    stlInt16                sColStartOffset;
    stlChar               * sPageBody = SMP_GET_FIRST_VALUE_POS( aPageHandle, aColCount );

    STL_ASSERT( sFSSignpost->mCommon.mPageMgmtType == SMP_FIXED_SLOT_TYPE );

    /**
     * Row header slot을 포함해서 slot array를 할당한다.
     */
    for( i = 0; i <= aColCount; i++ )
    {
        aSlotArray[i] = NULL;
    }

    /* row header slot alloc */
    aSlotArray[0] = sPageBody + (aSlotSeq * aRowHdrLen);
    sColStartOffset = aRowHdrLen * aRowCount;

    for( i = 1; i <= aColCount; i++ )
    {
        sColLen = sColLengthArray[i - 1];

        if( SMP_IS_FIXED_SLOT_CONTINUOUS_COLUMN( sColLen ) == STL_TRUE )
        {
            sColLen = SMP_GET_FIXED_SLOT_CONTINUOUS_COLUMN_LENGTH( sColLen );
            SMP_GET_CONT_COLUMN_LEN_SIZE( &sColLenSize );
        }
        else
        {
            SMP_GET_COLUMN_LEN_SIZE( sColLen, &sColLenSize );
        }

        aSlotArray[i] = sPageBody + sColStartOffset + (aSlotSeq * (sColLenSize + sColLen));
        sColStartOffset += aRowCount * (sColLen + sColLenSize);
    }

    if( sFSSignpost->mAllocatedMaxSlotSeq < aSlotSeq )
    {
        sFSSignpost->mAllocatedMaxSlotSeq = aSlotSeq;
    }

    if( aIsForTest == STL_FALSE )
    {
        sFSSignpost->mActiveSlotCount++;
    }

    return STL_SUCCESS;
}

/**
 * @brief Undo Page를 위해 slot을 할당 받는다. 실패할 경우 aSlot에 NULL을 리턴한다
 * @param[in,out] aPageHandle 페이지 Handle
 * @param[in] aSlotSize slot을 할당받을 RowItem의 크기
 * @param[in] aIsForTest 실제로 할당받지 않고 단지 테스트 용도인지 여부
 * @param[out] aSlot 할당받은 slot
 * @param[out] aSlotSeq 할당받은 slot의 순서
 */
stlStatus smpAllocSlot4Undo( smpHandle  * aPageHandle,
                             stlUInt16    aSlotSize,
                             stlBool      aIsForTest,
                             stlChar   ** aSlot,
                             stlInt16   * aSlotSeq )
{
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);
    smpOffsetTableHeader   * sOffsetTableHeader = SMP_GET_OFFSET_TABLE_HEADER( aPageHandle->mFrame );
    smpOffsetSlot          * sOffsetSlot;

    STL_ASSERT( sSignpost->mCommon.mPageMgmtType == SMP_ORDERED_SLOT_TYPE );

    if( sSignpost->mCommon.mHighWaterMark + STL_SIZEOF(smpOffsetSlot) + aSlotSize
        <= sSignpost->mLowWaterMark )
    {
        *aSlot = (stlChar*)aPageHandle->mFrame + (sSignpost->mLowWaterMark - aSlotSize);
        *aSlotSeq = sOffsetTableHeader->mCurrentSlotCount;
        if( aIsForTest != STL_TRUE )
        {
            sOffsetSlot =(smpOffsetSlot*)(sOffsetTableHeader + 1) + sOffsetTableHeader->mCurrentSlotCount;
            sOffsetTableHeader->mCurrentSlotCount++;
            sSignpost->mCommon.mHighWaterMark += STL_SIZEOF(smpOffsetSlot);
            sSignpost->mLowWaterMark -= aSlotSize;
            *sOffsetSlot = sSignpost->mLowWaterMark;
        }
    }
    else
    {
        *aSlot = NULL;
        *aSlotSeq = SMP_OFFSET_SLOT_NULL;
    }

    return STL_SUCCESS;
}


/**
 * @brief index, table redo, compaction을 위해 slot을 할당 받는다. 실패할 경우 aSlot에 NULL을 리턴한다.
 * @param[in,out] aPageHandle 페이지 Handle
 * @param[in] aSlotSeq 페이지 내에서 key의 논리적 순서
 * @param[in] aSlotSize slot을 할당받을 Key의 크기
 * @param[in] aIsForTest 실제로 할당받지 않고 단지 테스트 용도인지 여부
 * @param[out] aSlot 할당받은 slot
 */
stlStatus smpAllocNthSlot( smpHandle  * aPageHandle,
                           stlInt16     aSlotSeq,
                           stlUInt16    aSlotSize,
                           stlBool      aIsForTest,
                           stlChar   ** aSlot )
{
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);
    smpOffsetTableHeader   * sOffsetTableHeader = SMP_GET_OFFSET_TABLE_HEADER( aPageHandle->mFrame );
    smpOffsetSlot          * sOffsetSlot;
    smpOffsetSlot          * sPrevOffsetSlot;
    stlInt16                 sSlotDiff;
    stlInt16                i;

    STL_ASSERT( sSignpost->mCommon.mPageMgmtType == SMP_ORDERED_SLOT_TYPE );

    if( sOffsetTableHeader->mCurrentSlotCount > aSlotSeq )
    {
        if( SMP_CHECK_USE_FREE_SLOT_LIST(sOffsetTableHeader) != STL_TRUE )
        {
            if( sSignpost->mCommon.mHighWaterMark + STL_SIZEOF(smpOffsetSlot) + aSlotSize
                <= sSignpost->mLowWaterMark )
            {
                if( aSlotSize == 0 )
                {
                    /* index compact시 deadkey 처리. offset slot만 할당 받음. */
                    *aSlot = NULL;
                }
                else
                {
                    *aSlot = (stlChar*)aPageHandle->mFrame + (sSignpost->mLowWaterMark - aSlotSize);
                }
                if( aIsForTest != STL_TRUE )
                {
                    sSlotDiff = sOffsetTableHeader->mCurrentSlotCount - aSlotSeq;
                    sOffsetSlot = (smpOffsetSlot*)(sOffsetTableHeader + 1) + 
                        sOffsetTableHeader->mCurrentSlotCount;
                    for(i = 0; i < sSlotDiff; i++)
                    {
                        *sOffsetSlot = *(sOffsetSlot -1);
                        sOffsetSlot--;
                    }
                    sOffsetSlot = (smpOffsetSlot*)(sOffsetTableHeader + 1) + aSlotSeq;
                    sOffsetTableHeader->mCurrentSlotCount++;
                    sSignpost->mCommon.mHighWaterMark += STL_SIZEOF(smpOffsetSlot);
                    if( aSlotSize == 0 )
                    {
                        /* index compact시 deadkey 처리. offset slot만 할당 받음. */
                        *sOffsetSlot = SMP_OFFSET_SLOT_NULL;
                    }
                    else
                    {
                        *sOffsetSlot = sSignpost->mLowWaterMark - aSlotSize;
                        sSignpost->mLowWaterMark -= aSlotSize;
                    }
                }
            }
            else
            {
                *aSlot = NULL;
            }
        }
        else
        {
            /* free slot list를 사용 */
            if( sSignpost->mCommon.mHighWaterMark + aSlotSize <= sSignpost->mLowWaterMark )
            {
                *aSlot = (stlChar*)aPageHandle->mFrame + (sSignpost->mLowWaterMark - aSlotSize);
                if( aIsForTest != STL_TRUE )
                {
                    sOffsetSlot = (smpOffsetSlot*)(sOffsetTableHeader + 1) + aSlotSeq;
                    STL_ASSERT( SMP_OFFSET_SLOT_IS_FREE(sOffsetSlot) == STL_TRUE );
                    sPrevOffsetSlot = (smpOffsetSlot*)&sOffsetTableHeader->mFirstFreeOffsetSlot;
                    if( ((*sPrevOffsetSlot) & SMP_FREE_OFFSET_SLOT_SEQ_MASK) != aSlotSeq )
                    {
                        /* free slot list의 중간 slot이 재사용되므로 리스트를 재조정함 */
                        while( ((*sPrevOffsetSlot) & SMP_FREE_OFFSET_SLOT_SEQ_MASK) != SMP_OFFSET_SLOT_NULL )
                        {
                            if( ((*sPrevOffsetSlot) & SMP_FREE_OFFSET_SLOT_SEQ_MASK)  == aSlotSeq )
                            {
                                break;
                            }
                            sPrevOffsetSlot = (smpOffsetSlot*)(sOffsetTableHeader + 1) + 
                                SMP_GET_NEXT_FREE_SLOT(sPrevOffsetSlot);
                        }
                        STL_ASSERT( ((*sPrevOffsetSlot) & SMP_FREE_OFFSET_SLOT_SEQ_MASK) != SMP_OFFSET_SLOT_NULL );
                    }
                    *sPrevOffsetSlot = *sOffsetSlot;

                    *sOffsetSlot = sSignpost->mLowWaterMark - aSlotSize;
                    sSignpost->mLowWaterMark -= aSlotSize;
                }
            }
            else
            {
                *aSlot = NULL;
            }
        }
    }
    else /* 현재 offet slot count보다 큰 slot seq 요구 */
    {
        sSlotDiff = aSlotSeq - sOffsetTableHeader->mCurrentSlotCount + 1;
        if( sSignpost->mCommon.mHighWaterMark + (STL_SIZEOF(smpOffsetSlot)*sSlotDiff) + aSlotSize
                <= sSignpost->mLowWaterMark )
        {
            if( SMP_CHECK_USE_FREE_SLOT_LIST(sOffsetTableHeader) != STL_TRUE )
            {
                /* Index */
                STL_ASSERT( sSlotDiff == 1 );
                *aSlot = (stlChar*)aPageHandle->mFrame + (sSignpost->mLowWaterMark - aSlotSize);
                if( aIsForTest != STL_TRUE )
                {
                    sOffsetSlot = (smpOffsetSlot*)(sOffsetTableHeader + 1) + 
                        sOffsetTableHeader->mCurrentSlotCount;
                    sOffsetTableHeader->mCurrentSlotCount++;
                    sSignpost->mCommon.mHighWaterMark += STL_SIZEOF(smpOffsetSlot);
                    *sOffsetSlot = sSignpost->mLowWaterMark - aSlotSize;
                    sSignpost->mLowWaterMark -= aSlotSize;
                }
            }
            else
            {
                /* Table redo, compaction */
                *aSlot = (stlChar*)aPageHandle->mFrame + (sSignpost->mLowWaterMark - aSlotSize);
                if( aIsForTest != STL_TRUE )
                {
                    for( i = sOffsetTableHeader->mCurrentSlotCount; i < aSlotSeq; i++ )
                    {
                        sOffsetSlot = (smpOffsetSlot*)(sOffsetTableHeader + 1) + i;
                        *sOffsetSlot = sOffsetTableHeader->mFirstFreeOffsetSlot;
                        sOffsetTableHeader->mFirstFreeOffsetSlot = ((stlInt16)i) | SMP_USE_FREE_SLOT_LIST_MASK;
                    }
                    sOffsetTableHeader->mCurrentSlotCount = aSlotSeq + 1;
                    sSignpost->mCommon.mHighWaterMark += STL_SIZEOF(smpOffsetSlot) * sSlotDiff;
                    sOffsetSlot = (smpOffsetSlot*)(sOffsetTableHeader + 1) + aSlotSeq;
                    *sOffsetSlot = sSignpost->mLowWaterMark - aSlotSize;
                    sSignpost->mLowWaterMark -= aSlotSize;
                }
            }
       }
        else
        {
            *aSlot = NULL;
        }
    }

    return STL_SUCCESS;
}


/**
 * @brief table의 outplace update와 index dupkey를 위해 slot을 할당 받는다. 실패할 경우 aSlot에 NULL을 리턴한다.
 * @param[in,out] aPageHandle 페이지 Handle
 * @param[in] aSlotSeq 페이지 내에서 key의 논리적 순서
 * @param[in] aSlotSize slot을 할당받을 Key의 크기
 * @param[in] aIsForTest 실제로 할당받지 않고 단지 테스트 용도인지 여부
 * @param[out] aSlot 할당받은 slot
 */
stlStatus smpReallocNthSlot( smpHandle  * aPageHandle,
                             stlInt16     aSlotSeq,
                             stlUInt16    aSlotSize,
                             stlBool      aIsForTest,
                             stlChar   ** aSlot )
{
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);
    smpOffsetTableHeader   * sOffsetTableHeader = SMP_GET_OFFSET_TABLE_HEADER( aPageHandle->mFrame );
    smpOffsetSlot          * sOffsetSlot;

    STL_ASSERT( sSignpost->mCommon.mPageMgmtType == SMP_ORDERED_SLOT_TYPE );
    STL_ASSERT( sOffsetTableHeader->mCurrentSlotCount > aSlotSeq );

    if( sSignpost->mCommon.mHighWaterMark + aSlotSize  <= sSignpost->mLowWaterMark )
    {
        *aSlot = (stlChar*)aPageHandle->mFrame + (sSignpost->mLowWaterMark - aSlotSize);
        if( aIsForTest != STL_TRUE )
        {
            sOffsetSlot = (smpOffsetSlot*)(sOffsetTableHeader + 1) + aSlotSeq;
            *sOffsetSlot = sSignpost->mLowWaterMark - aSlotSize;
            sSignpost->mLowWaterMark -= aSlotSize;
        }
    }
    else
    {
        *aSlot = NULL;
    }

    return STL_SUCCESS;
}


/**
 * @brief smpReallocSlot을 했던 변경사항을 복원한다
 * @param[in,out] aPageHandle 페이지 Handle
 * @param[in] aSlotSeq 페이지 내에서 key의 논리적 순서
 * @param[in] aOrgSlot 할당받기 전의 slot
 * @param[in] aSlotSize slot을 할당받을 Key의 크기
 */
stlStatus smpCancelReallocNthSlot( smpHandle * aPageHandle,
                                   stlInt16    aSlotSeq,
                                   stlChar   * aOrgSlot,
                                   stlUInt16   aSlotSize )
{
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);
    smpOffsetTableHeader   * sOffsetTableHeader = SMP_GET_OFFSET_TABLE_HEADER( aPageHandle->mFrame );
    smpOffsetSlot          * sOffsetSlot;

    STL_ASSERT( sSignpost->mCommon.mPageMgmtType == SMP_ORDERED_SLOT_TYPE );
    STL_ASSERT( sOffsetTableHeader->mCurrentSlotCount > aSlotSeq );
    STL_ASSERT( SMP_CHECK_USE_FREE_SLOT_LIST(sOffsetTableHeader) == STL_TRUE );

    sOffsetSlot = (smpOffsetSlot*)(sOffsetTableHeader + 1) + aSlotSeq;
    STL_ASSERT( *sOffsetSlot == sSignpost->mLowWaterMark );
    *sOffsetSlot = aOrgSlot - (stlChar*)SMP_FRAME(aPageHandle);
    sSignpost->mLowWaterMark += aSlotSize;

    return STL_SUCCESS;
}


/**
 * @brief 할당 받은 slot을 해제한다
 * @param[in,out] aPageHandle 페이지 Handle
 * @param[in] aSlotSeq Free할 RowItem의 순서
 * @param[in] aSlotSize RowItem이 할당받은 Slot의 크기
 */
stlStatus smpFreeNthSlot( smpHandle * aPageHandle,
                          stlInt16    aSlotSeq,
                          stlUInt16   aSlotSize )
{
    stlUInt32                i;
    smpOffsetSlot          * sOffsetSlot;
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);
    smpOffsetTableHeader   * sOffsetTableHeader = SMP_GET_OFFSET_TABLE_HEADER( aPageHandle->mFrame );

    STL_ASSERT( sSignpost->mCommon.mPageMgmtType == SMP_ORDERED_SLOT_TYPE );
    sOffsetSlot = (smpOffsetSlot*)(sOffsetTableHeader + 1) + aSlotSeq;

    if( SMP_CHECK_USE_FREE_SLOT_LIST(sOffsetTableHeader) != STL_TRUE )
    {
        /* Index */
        for( i = aSlotSeq; i < sOffsetTableHeader->mCurrentSlotCount - 1; i++ )
        {
            *sOffsetSlot = *(sOffsetSlot + 1);
            sOffsetSlot++;
        }
        sOffsetTableHeader->mCurrentSlotCount--;
        sSignpost->mCommon.mHighWaterMark -= STL_SIZEOF(smpOffsetSlot);
    }
    else
    {
        /* Table */
        if( aSlotSize == 0 )
        {
            /* compact 도중에 이미 RowId 유지를 위해 빈 slot을 만드는 경우 */
            /* free되는 slot이 현재 slot count보다 클 수 있으므로 중간에 있는 slot들을 free list에 단다 */
            STL_ASSERT( aSlotSeq >= sOffsetTableHeader->mCurrentSlotCount );
            sOffsetSlot = (smpOffsetSlot*)(sOffsetTableHeader + 1) + sOffsetTableHeader->mCurrentSlotCount;
            for( i = sOffsetTableHeader->mCurrentSlotCount; i <= aSlotSeq; i++ )
            {
                *sOffsetSlot = sOffsetTableHeader->mFirstFreeOffsetSlot;
                sOffsetTableHeader->mFirstFreeOffsetSlot = ((smpOffsetSlot)i) | SMP_USE_FREE_SLOT_LIST_YES;
                sSignpost->mCommon.mHighWaterMark += STL_SIZEOF(smpOffsetSlot);
                sOffsetSlot++;
            }
            sOffsetTableHeader->mCurrentSlotCount = aSlotSeq + 1;
        }
        else
        {
            *sOffsetSlot = sOffsetTableHeader->mFirstFreeOffsetSlot;
            sOffsetTableHeader->mFirstFreeOffsetSlot = aSlotSeq | SMP_USE_FREE_SLOT_LIST_YES;
        }
        /* table일때만 더해준다. index는 밖에서 상황봐서 한다 */
        sSignpost->mReclaimedSlotSpace += aSlotSize;
        STL_DASSERT( sSignpost->mReclaimedSlotSpace < SMP_PAGE_SIZE );
    }

    return STL_SUCCESS;
}

/**
 * @brief signpost에 있는 mReclaimedSlotSpace에 공간 새로 free된 공간을 더한다
 * @param aPageHandle free공간을 추가할 페이지 핸들
 * @param aFreeSize 새로 추가될 free 공간의 크기
 */
void smpAddReclaimedSpace( smpHandle * aPageHandle,
                           stlInt16    aFreeSize )
{
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);

    sSignpost->mReclaimedSlotSpace += aFreeSize;
    STL_DASSERT( sSignpost->mReclaimedSlotSpace < SMP_PAGE_SIZE );
}


/**
 * @brief signpost에 있는 mReclaimedSlotSpace에 공간 새로 free된 공간을 더한다
 * @param aPageHandle free공간을 추가할 페이지 핸들
 * @param aFreeSize 새로 추가될 free 공간의 크기
 */
void smpRemoveReclaimedSpace( smpHandle * aPageHandle,
                              stlInt16    aFreeSize )
{
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);

    STL_DASSERT( sSignpost->mReclaimedSlotSpace >= aFreeSize );

    sSignpost->mReclaimedSlotSpace -= aFreeSize;
    STL_DASSERT( sSignpost->mReclaimedSlotSpace < SMP_PAGE_SIZE );
}


/**
 * @brief index의 compaction/split 후에 dead key로 인해 생긴 빈 offset slot을 정리한다
 * @param[in,out] aPageHandle 페이지 Handle
 */
void smpShrinkOffsetTable( smpHandle  * aPageHandle )
{
    smpOffsetTableHeader   * sOffsetTableHeader = SMP_GET_OFFSET_TABLE_HEADER( aPageHandle->mFrame );
    smpOffsetSlot          * sOffsetSlot = (smpOffsetSlot*)(sOffsetTableHeader + 1);
    stlInt16                i;
    stlInt16                j;

    for( i = 0; i < sOffsetTableHeader->mCurrentSlotCount; i++ )
    {
        if( sOffsetSlot[i] == SMP_OFFSET_SLOT_NULL )
        {
            for( j = i; j < sOffsetTableHeader->mCurrentSlotCount - 1; j++ )
            {
                sOffsetSlot[j] = sOffsetSlot[j + 1];
            }
            sOffsetTableHeader->mCurrentSlotCount--;
        }
    }
}


/**
 * @brief page의 page 내의 데이터를 주어진 정보로 변경한다.(Index REDO용 함수임)
 * @param[in,out] aPageHandle 페이지 Handle
 * @param[in] aReclaimedSpace 변경 후의 reclaimed 공간의 크기
 * @param[in] aActiveSlotCount 변경 후의 delete 되지 않은 slot의 개수
 * @param[in] aRtsCount 변경 후의 Rts의 개수
 * @param[in] aRtsArray 복사할 Rts 배열
 * @param[in] aRowItemCount Data saction에 있는 rowitem의 개수
 * @param[in] aOffsetArray Data saction에 있는 rowitem의 순서를 고려한 offset 배열
 * @param[in] aDataSactionSize Data saction의 크기
 * @param[in] aDataSaction Data saction의 내용
 */
inline void smpReformPage( smpHandle * aPageHandle,
                           stlUInt16   aReclaimedSpace,
                           stlUInt16   aActiveSlotCount,
                           stlUInt16   aRtsCount,
                           void      * aRtsArray,
                           stlUInt16   aRowItemCount,
                           stlUInt16 * aOffsetArray,
                           stlUInt16   aDataSactionSize,
                           stlChar   * aDataSaction )
{
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);
    smpOffsetTableHeader   * sOffsetTableHeader;
    stlChar                * sDstPtr;
    stlInt32                 i;

    /* Reclaimed Space 설정 */
    smpAddReclaimedSpace( aPageHandle, aReclaimedSpace );
    sSignpost->mActiveSlotCount = aActiveSlotCount;
    
    /* Rts copy */
    sSignpost->mCurRtsCount = aRtsCount;

    sDstPtr = (stlChar*)(sSignpost + 1);
    sDstPtr += sSignpost->mLogicalHeaderSize;
    
    if( aRtsCount > 0 )
    {
        stlMemcpy( sDstPtr, aRtsArray, STL_SIZEOF(smpRts) * aRtsCount );
    }

    /* offset table 생성 */
    sDstPtr = sDstPtr + (STL_SIZEOF(smpRts) * aRtsCount);
    sOffsetTableHeader = (smpOffsetTableHeader*)sDstPtr;
    sOffsetTableHeader->mCurrentSlotCount = aRowItemCount;
    sOffsetTableHeader->mFirstFreeOffsetSlot = SMP_USE_FREE_SLOT_LIST_NO | SMP_OFFSET_SLOT_NULL;

    /* DataSaction Copy */
    sSignpost->mLowWaterMark = SMP_PAGE_SIZE - STL_SIZEOF(smpPhyTail) - aDataSactionSize;
    stlMemcpy( aPageHandle->mFrame + sSignpost->mLowWaterMark, aDataSaction, aDataSactionSize );

    /* Offset Slot 정상화 */
    sDstPtr = (stlChar*)(sOffsetTableHeader + 1);
    for( i = 0; i < aRowItemCount; i ++ )
    {
        /* aOffsetArray는 aDataSaction의 시작 위치를 기준으로 한 offset 값임 */
        *(smpOffsetSlot*)sDstPtr = aOffsetArray[i] + sSignpost->mLowWaterMark;
        sDstPtr += STL_SIZEOF(smpOffsetSlot);
    }
    sSignpost->mCommon.mHighWaterMark = sDstPtr - (stlChar*)aPageHandle->mFrame;
}


/**
 * @brief test 용도로 페이지 body의 내용을 출력한다
 * @param[in] aPageHandle 페이지 Handle
 */
void smpTestPrintPageBody( smpHandle * aPageHandle )
{
    smpSignpost            * sSignpost = (smpSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);
    smpOrderedSlotSignpost * sOSSignpost = (smpOrderedSlotSignpost*)sSignpost;
    smpRts                 * sRts;
    smpOffsetTableHeader   * sOffsetTableHeader;
    smpOffsetSlot          * sOffsetSlot;
    stlInt32                 i;

    stlPrintf("==================================================\n"
              "Dumping Page Body Tablespace[%d], PageId[%d]\n"
              "--------------------------------------------------\n",
              aPageHandle->mPch->mTbsId,
              aPageHandle->mPch->mPageId);

    stlPrintf("smpSignpost.mPageMgmtType  : %d\n"
              "smpSignpost.mHighWaterMark : %d\n",
              sSignpost->mPageMgmtType,
              sSignpost->mHighWaterMark);

    if( sSignpost->mPageMgmtType == SMP_ORDERED_SLOT_TYPE )
    {
        stlPrintf("smpOrderedSlotSignpost.mLogicalHeaderSize  : %d\n"
                  "smpOrderedSlotSignpost.mCurRtsCount      : %d\n"
                  "smpOrderedSlotSignpost.mHintRts            : %d\n"
                  "smpOrderedSlotSignpost.mLowWaterMark       : %d\n"
                  "smpOrderedSlotSignpost.mReclaimedSlotSpace : %d\n",
                  sOSSignpost->mLogicalHeaderSize,
                  sOSSignpost->mCurRtsCount,
                  sOSSignpost->mHintRts,
                  sOSSignpost->mLowWaterMark,
                  sOSSignpost->mReclaimedSlotSpace);

        if( sOSSignpost->mCurRtsCount > 0 )
        {
            sRts = SMP_GET_FIRST_RTS(aPageHandle->mFrame);
            for( i = 0; i < sOSSignpost->mCurRtsCount; i++ )
            {
                stlPrintf("smpRts[%d].mRefCount       : %d\n"
                          "smpRts[%d].mTransId        : %d\n"
                          "smpRts[%d].mScn            : %d\n"
                          "smpRts[%d].mOffsetCache[0] : %d\n"
                          "smpRts[%d].mOffsetCache[1] : %d\n"
                          "smpRts[%d].mOffsetCache[2] : %d\n",
                          i, sRts->mRefCount,
                          i, sRts->mTransId,
                          i, sRts->mScn,
                          i, sRts->mOffsetCache[0],
                          i, sRts->mOffsetCache[1],
                          i, sRts->mOffsetCache[2]);
                sRts++;
            }
        }

        sOffsetTableHeader = SMP_GET_OFFSET_TABLE_HEADER(aPageHandle->mFrame);
        stlPrintf("smpOffsetTableHeader.mCurrentSlotCount    : %d\n"
                  "smpOffsetTableHeader.mFirstFreeOffsetSlot : %d\n",
                  sOffsetTableHeader->mCurrentSlotCount,
                  sOffsetTableHeader->mFirstFreeOffsetSlot);
        if( sOffsetTableHeader->mCurrentSlotCount > 0 )
        {
            sOffsetSlot = (smpOffsetSlot*)(sOffsetTableHeader + 1);
            for( i = 0; i < sOffsetTableHeader->mCurrentSlotCount; i++ )
            {
                stlPrintf("smpOffsetSlot[%d] : %d\n", i, *sOffsetSlot );
                sOffsetSlot++;
            }
        }
        if( SMP_CHECK_USE_FREE_SLOT_LIST(sOffsetTableHeader) == STL_TRUE )
        {
            sOffsetSlot = (smpOffsetSlot*)&sOffsetTableHeader->mFirstFreeOffsetSlot;
            while( SMP_GET_NEXT_FREE_SLOT(sOffsetSlot) != SMP_OFFSET_SLOT_NULL )
            {
                stlPrintf("FreeSlotSeq[%d] \n", (*sOffsetSlot) & SMP_FREE_OFFSET_SLOT_SEQ_MASK);
                sOffsetSlot = (smpOffsetSlot*)(sOffsetTableHeader + 1) + 
                    SMP_GET_NEXT_FREE_SLOT(sOffsetSlot);
            }
        }
    }
}

/**
 * @brief Offset table haeder를 검증한다.
 * @param[in] aFrame Page가 저장되어 있는 물리적 주소
 * @param[in,out] aEnv Environment 구조체
 */
inline stlStatus smpValidateOffsetTableHeader( stlChar * aFrame,
                                               smlEnv  * aEnv )
{
#ifdef STL_VALIDATE_DATABASE
    smpOffsetTableHeader * sOtHeader;

    sOtHeader = SMP_GET_OFFSET_TABLE_HEADER( aFrame );

    KNL_ASSERT( sOtHeader->mCurrentSlotCount < 1000,
                aEnv,
                ("HEADER_SLOT_COUNT(%d)",
                 sOtHeader->mCurrentSlotCount ) );
    
    return STL_SUCCESS;
#else
    return STL_SUCCESS;
#endif
}

/**
 * @brief page handle로 부터 첫 offset slot의 위치를 구한다
 * @param[in] aPageHandle 페이지 Handle
 */
inline smpOffsetSlot * smpGetFirstOffsetSlot( smpHandle * aPageHandle )
{
    return (smpOffsetSlot*)(SMP_GET_OFFSET_TABLE_HEADER(SMP_FRAME(aPageHandle)) + 1);
}


/**
 * @brief page 내의 주어진 순번의 offset slot이 free 상태인지 체크한다
 * @param[in] aPageHandle 페이지 Handle
 * @param[in] aSlotSeq 체크할 offset slot의 순번
 */
inline stlBool smpIsNthOffsetSlotFree( smpHandle * aPageHandle,
                                       stlInt16    aSlotSeq )
{
    smpOffsetTableHeader * sOtHeader = SMP_GET_OFFSET_TABLE_HEADER(SMP_FRAME(aPageHandle));
    smpOffsetSlot * sOffsetSlot = (smpOffsetSlot*)(sOtHeader + 1) + aSlotSeq;
    return (sOtHeader->mCurrentSlotCount <= aSlotSeq) ||
           (SMP_OFFSET_SLOT_IS_FREE(sOffsetSlot) == STL_TRUE) ? STL_TRUE : STL_FALSE;
}

/**
 * @brief page 내의 주어진 순번의 rowitem을 반환한다
 * @param[in] aPageHandle 페이지 Handle
 * @param[in] aRowItemSeq 반환할 Row Item의 순번
 */
inline stlChar * smpGetNthRowItem( smpHandle * aPageHandle, 
                                   stlInt16    aRowItemSeq )
{
    smpOffsetTableHeader * sOtHeader = SMP_GET_OFFSET_TABLE_HEADER(SMP_FRAME(aPageHandle));
    smpOffsetSlot        * sFirstOffsetSlot;

    STL_DASSERT( sOtHeader->mCurrentSlotCount > aRowItemSeq );

    sFirstOffsetSlot = (smpOffsetSlot*)(sOtHeader + 1);

    STL_DASSERT( (sOtHeader->mCurrentSlotCount > aRowItemSeq) &&
                 (*(sFirstOffsetSlot + aRowItemSeq) > 0) &&
                 (*(sFirstOffsetSlot + aRowItemSeq) < SMP_PAGE_SIZE) );
    
    return (stlChar*)SMP_FRAME(aPageHandle) + *(sFirstOffsetSlot + aRowItemSeq);
}

/**
 * @brief page 내의 주어진 순번의 rowitem을 유효성 검사없이 반환한다
 * @param[in] aPageHandle 페이지 Handle
 * @param[in] aRowItemSeq 반환할 Row Item의 순번
 */
inline stlChar * smpGetUnsafeNthRowItem( smpHandle * aPageHandle, 
                                         stlInt16    aRowItemSeq )
{
    smpOffsetSlot * sFirstOffsetSlot = smpGetFirstOffsetSlot( aPageHandle );

    return (stlChar*)SMP_FRAME(aPageHandle) + *(sFirstOffsetSlot + aRowItemSeq);
}


/**
 * @brief page 내의 N번째 rowitem 바로 앞으로 포인터를 이동한다
 * @param[in] aPageFrame 페이지 시작 주소
 * @param[out] aIterator 현재의 위치를 기록할 iterator
 * @param[in] aRowItemSeq 현재의 위치를 기록할 iterator
 */
inline void smpBeforeNthRowItem( void * aPageFrame, smpRowItemIterator * aIterator, stlInt16 aRowItemSeq )
{
    smpOffsetTableHeader * sHeader = SMP_GET_OFFSET_TABLE_HEADER(aPageFrame);

    aIterator->mHeader = sHeader;
    aIterator->mPage = (smpPhyHdr*)(aPageFrame);
    aIterator->mSlotFence = (stlUInt8*)((smpOffsetSlot*)(sHeader + 1) + sHeader->mCurrentSlotCount);
    aIterator->mCurSlot = (stlUInt8*)((smpOffsetSlot*)(sHeader + 1) + aRowItemSeq - 1);
    aIterator->mCurSeq = aRowItemSeq - 1;
}

/**
 * @brief page 내의 N번째 rowitem 바로 뒤로 포인터를 이동한다
 * @param[in] aPageFrame 페이지 시작 주소
 * @param[out] aIterator 현재의 위치를 기록할 iterator
 * @param[in] aRowItemSeq 현재의 위치를 기록할 iterator
 */
inline void smpAfterNthRowItem( void * aPageFrame, smpRowItemIterator * aIterator, stlInt16 aRowItemSeq )
{
    smpOffsetTableHeader * sHeader = SMP_GET_OFFSET_TABLE_HEADER(aPageFrame);

    aIterator->mHeader = sHeader;
    aIterator->mPage = (smpPhyHdr*)(aPageFrame);
    aIterator->mSlotFence = (stlUInt8*)((smpOffsetSlot*)(sHeader + 1) - 1);
    aIterator->mCurSlot = (stlUInt8*)((smpOffsetSlot*)(sHeader + 1) + aRowItemSeq + 1);
}


/**
 * @brief page 내의 모든 row item의 개수을 변경한다(사용시 주의 요망!!)
 * @param[in] aPageHandle 변경할 page의 handle
 * @param[in] aRowItemCount 세팅할 rowitem count
 */
inline void smpSetPageRowItemCount( smpHandle * aPageHandle,
                                    stlUInt16   aRowItemCount)
{
    (SMP_GET_OFFSET_TABLE_HEADER(SMP_FRAME(aPageHandle)))->mCurrentSlotCount = aRowItemCount;
}

/**
 * @brief page 내에 rowitem을 담고있는 data section의 정보를 구한다
 * @param[in] aPageHandle 조사할 page의 handle
 * @param[out] aDataPtr Data section의 시작 주소
 * @param[out] aDataSize Data section의 크기
 */
inline void smpGetPageDataSection( smpHandle  * aPageHandle,
                                   stlChar   ** aDataPtr,
                                   stlUInt16  * aDataSize )
{
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);
    *aDataPtr = (stlChar*)SMP_FRAME(aPageHandle) + sSignpost->mLowWaterMark;
    *aDataSize = SMP_PAGE_SIZE - sSignpost->mLowWaterMark - STL_SIZEOF(smpPhyTail);
}

/**
 * @brief RTS의 크기를 반환한다
 */
inline stlUInt16 smpGetRtsSize( )
{
    return STL_SIZEOF(smpRts);
}

/**
 * @brief 주어진 페이지내의 사용되지 않은 공간의 크기를 반환한다
 * @param[in] aPageFrame 조사할 페이지의 시작 주소
 */
inline stlUInt16 smpGetUnusedSpace( void * aPageFrame )
{
   return  ((smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageFrame))->mLowWaterMark
       - ((smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageFrame))->mCommon.mHighWaterMark;
}


/**
 * @brief 주어진 페이지내의 회수된 공간의 크기를 반환한다
 * @param[in] aPageFrame 조사할 페이지의 시작 주소
 */
inline stlUInt16 smpGetReclaimedSpace( void * aPageFrame )
{
    return ((smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageFrame))->mReclaimedSlotSpace;
}


/**
 * @brief 주어진 페이지내의 회수된 공간의 크기를 반환한다
 * @param[in] aPageFrame 조사할 페이지의 시작 주소
 */
inline stlUInt16 smpGetActiveSlotCount( void * aPageFrame )
{
    return ((smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageFrame))->mActiveSlotCount;
}




/**
 * @brief 주어진 페이지내의 여유공간의 크기를 반환한다
 * @param[in] aPageFrame 조사할 페이지의 시작 주소
 */
inline stlUInt16 smpGetTotalFreeSpace( void * aPageFrame )
{
    return SMP_PAGE_GET_FREE_SPACE( aPageFrame );
}

/**
 * @brief 빈 페이지에서 할당할 수 있는 row의 최대 크기를 반환한다
 * @param[in] aLogicalHdrSize 페이지의 logical header의 크기
 * @param[in] aRtsCount 페이지가 가진 RTS 슬롯의 개수
 */
stlInt16 smpGetPageMaxFreeSize( stlInt16 aLogicalHdrSize, stlUInt8 aRtsCount )
{
    return SMP_PAGE_SIZE -
        (STL_SIZEOF(smpPhyHdr) + STL_SIZEOF(smpOrderedSlotSignpost) + aLogicalHdrSize
        + (STL_SIZEOF(smpRts) * aRtsCount) + STL_SIZEOF(smpOffsetTableHeader) + STL_SIZEOF(smpOffsetSlot)
        + STL_SIZEOF(smpPhyTail));
}

/**
 * @brief 주어진 페이지내의 사용중인 공간이 aPctFree%를 넘었는지 체크한다
 * @param[in] aPageFrame 조사할 페이지의 시작 주소
 * @param[in] aPctFree 페이지에 지정된 최대 사용량
 */
inline stlBool smpIsPageOverInsertLimit( void     * aPageFrame, 
                                         stlInt16   aPctFree )
{
    return SMP_PAGE_IS_OVER_INSERT_LIMIT( aPageFrame, aPctFree );
}

/**
 * @brief 주어진 페이지내의 사용중인 공간이 aPctUsed%에 못미치는지 체크한다
 * @param[in] aPageFrame 조사할 페이지의 시작 주소
 * @param[in] aPctUsed 페이지에 지정된 최소 사용량
 */
inline stlBool smpIsPageOverFreeLimit( void     * aPageFrame, 
                                       stlInt16   aPctUsed )
{
    return SMP_PAGE_IS_OVER_FREE_LIMIT( aPageFrame, aPctUsed );
}

stlUInt16 smpGetPageMgmtType( smpHandle * aPageHandle )
{
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);
    return sSignpost->mCommon.mPageMgmtType;
}

stlUInt16 smpGetHighWaterMark( smpHandle * aPageHandle )
{
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);
    return sSignpost->mCommon.mHighWaterMark;
}

stlUInt16 smpGetLogicalHeaderSize( smpHandle * aPageHandle )
{
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);
    return sSignpost->mLogicalHeaderSize;
}

stlUInt16 smpGetHintRts( smpHandle * aPageHandle )
{
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);
    return sSignpost->mHintRts;
}

stlUInt16 smpGetLowWaterMark( smpHandle * aPageHandle )
{
    smpOrderedSlotSignpost * sSignpost = (smpOrderedSlotSignpost*)SMP_GET_SIGNPOST(aPageHandle->mFrame);
    return sSignpost->mLowWaterMark;
}


void smpCheckRtsRefCount( void            * aPageFrame,
                          smpCountRefFunc   aCountRef,
                          smlEnv          * aEnv )
{
    stlUInt32                i;
    stlUInt32                j;
    smpRts                 * sRts;
    stlInt16                 sTotalRefCount;
    stlInt16                 sRefCount;
    stlChar                * sRowItem;
    smpOffsetSlot          * sOffsetSlot;


    sRts = SMP_GET_FIRST_RTS(aPageFrame);
    for( i = 0; i < SMP_GET_CURRENT_RTS_COUNT(aPageFrame); i++ )
    {
        sOffsetSlot = (smpOffsetSlot*)((SMP_GET_OFFSET_TABLE_HEADER(aPageFrame)) + 1);
        sTotalRefCount = 0;
        for( j = 0; j < SMP_PAGE_GET_ROWITEM_COUNT(aPageFrame); j++ )
        {
            if( SMP_OFFSET_SLOT_IS_FREE(sOffsetSlot) == STL_TRUE )
            {
                sOffsetSlot++;
                continue;
            }
            sRowItem = (stlChar*)aPageFrame + *sOffsetSlot;
            sRefCount = aCountRef( aPageFrame, sRowItem, i, aEnv );
            sTotalRefCount += sRefCount;
            sOffsetSlot++;
        }

        STL_ASSERT( sRts->mRefCount == sTotalRefCount );
        sRts++;
    }
}



/** @} */
