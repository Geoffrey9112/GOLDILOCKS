/*******************************************************************************
 * smnmpbScan.c
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
#include <sml.h>
#include <smlDef.h>
#include <smDef.h>
#include <sms.h>
#include <smxm.h>
#include <smxl.h>
#include <smp.h>
#include <smo.h>
#include <sme.h>
#include <sma.h>
#include <smd.h>
#include <smnDef.h>
#include <smnmpbDef.h>
#include <smnmpb.h>
#include <smdmphDef.h>
#include <smdmph.h>

/**
 * @file smnmpbScan.c
 * @brief Storage Manager Layer Memory B-Tree Scanning Routines
 */

/**
 * @addtogroup smn
 * @{
 */

/**
 * @brief 주어진 Iterator를 초기화한다
 * @param[in,out] aIterator 초기화할 iterator
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbInitIterator ( void   * aIterator,
                               smlEnv * aEnv )
{
    smnmpbIterator * sIterator = (smnmpbIterator*)aIterator;

    sIterator->mRowRid.mTbsId = 0;
    sIterator->mRowRid.mPageId = SMP_NULL_PID;
    sIterator->mRowRid.mOffset = -1;
    sIterator->mPageBufPid = SMP_NULL_PID;
    sIterator->mCurSlotSeq = -1;
    sIterator->mSlotSeqFence = STL_INT16_MAX;
    sIterator->mLastValidKeyPid = SMP_NULL_PID;
    sIterator->mAgableViewScn = smxlGetAgableViewScn( aEnv );

    sIterator->mBaseRelHandle = SMN_GET_BASE_REL_HANDLE( sIterator->mCommon.mRelationHandle );
    
    if( sIterator->mBaseRelHandle == NULL )
    {
        STL_TRY( smeGetRelationHandle( SMN_GET_BASE_TABLE_SEGMENT_RID(sIterator->mCommon.mRelationHandle),
                                       &sIterator->mBaseRelHandle,
                                       aEnv ) == STL_SUCCESS );
        SMN_SET_BASE_REL_HANDLE( sIterator->mCommon.mRelationHandle, sIterator->mBaseRelHandle );
    }

    /**
     * Table Mutating 조건의 위배 여부를 검사한다.
     */
    STL_TRY( smaCheckMutating( sIterator->mCommon.mStatement,
                               sIterator->mBaseRelHandle,
                               aEnv )
             == STL_SUCCESS );
    
    sIterator->mBaseRelTbsId = smsGetTbsId( SME_GET_SEGMENT_HANDLE(sIterator->mBaseRelHandle) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Iterator를 다시 초기화한다
 * @param[in,out] aIterator         초기화할 iterator
 * @param[in]     aEnv              Storage Manager Environment
 */
stlStatus smnmpbResetIterator( void   * aIterator,
                               smlEnv * aEnv )
{
    smnmpbIterator * sIterator = (smnmpbIterator*)aIterator;

    sIterator->mRowRid.mTbsId = 0;
    sIterator->mRowRid.mPageId = SMP_NULL_PID;
    sIterator->mRowRid.mOffset = -1;
    sIterator->mPageBufPid = SMP_NULL_PID;
    sIterator->mCurSlotSeq = -1;
    sIterator->mSlotSeqFence = STL_INT16_MAX;
    sIterator->mLastValidKeyPid = SMP_NULL_PID;
    
    return STL_SUCCESS;
}


/**
 * @brief 주어진 Iterator가 가진 현재 위치를 반환한다
 * @param[in] aIterator 조사할 iterator
 * @param[out] aRowRid 반환되는 Row의 위치
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbGetRowRid( void   * aIterator,
                           smlRid * aRowRid,
                           smlEnv * aEnv )
{
    stlMemcpy( aRowRid, &((smnmpbIterator*)aIterator)->mRowRid, STL_SIZEOF(smlRid) );

    return STL_SUCCESS;
}


/**
 * @brief 주어진 Row Rid를 가리키는 key로 Iterator의 위치를 이동시킨다
 * @param[in,out] aIterator 조사할 iterator
 * @param[in] aRowRid 이동시킬 위치
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbMoveToRowRid( void   * aIterator,
                              smlRid * aRowRid,
                              smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief 주어진 position number를 가리키는 key로 Iterator의 위치를 이동시킨다
 * @param[in,out] aIterator 조사할 iterator
 * @param[in] aPosNum 이동시킬 위치
 * @param[in] aEnv Storage Manager Environment
 */ 
stlStatus smnmpbMoveToPosNum( void   * aIterator,
                              stlInt64 aPosNum,
                              smlEnv * aEnv )
{
    return STL_SUCCESS;
}

/**
 * @brief 주어진 Tx A 와 B 가 같은 Tx인지 반환한다
 * @param[in] aTransIdA Tx A의 Transaction Id
 * @param[in] aCommitScnA Tx A의 Commit SCN
 * @param[in] aTransIdB Tx B의 Transaction Id
 * @param[in] aCommitScnB Tx B의 Commit SCN
 */
stlBool smnmpbIsSameTrans( smxlTransId aTransIdA,
                           smlScn      aCommitScnA,
                           smxlTransId aTransIdB,
                           smlScn      aCommitScnB )
{
    if( (aCommitScnA == aCommitScnB) &&
        ((aCommitScnA != SML_INFINITE_SCN) || 
         (((aCommitScnA == SML_INFINITE_SCN)  &&
           (aTransIdA == aTransIdB)))) )
    {
        return STL_TRUE;
    }
    return STL_FALSE;
}



/**
 * @brief 특정 Internal node에서 마지막 key가 현재 min keyrange에 부합하는지 체크한다
 * @param[in]     aPageHandle  조사할 internal page의 handle
 * @param[in]     aIterator    scan 조건과 현재 위치를 저장한 iterator
 * @param[in,out] aFetchInfo   Fetch 정보
 * @param[out]    aResult      체크결과 condition에 부합하는지 여부
 * @param[in]     aEnv         Storage Manager Environment
 */
inline stlStatus smnmpbCheckMinRangeWithMaxKey( smpHandle         * aPageHandle,
                                                smnmpbIterator    * aIterator,
                                                smlFetchInfo      * aFetchInfo,
                                                stlBool           * aResult,
                                                smlEnv            * aEnv )
{
    stlInt16           sLastKeySeq = SMP_GET_PAGE_ROW_ITEM_COUNT( aPageHandle ) - 1;
    stlChar          * sLastKey;
    stlBool            sIsLeaf = SMNMPB_GET_LEVEL(aPageHandle ) == 0 ? STL_TRUE : STL_FALSE;

    if( (sIsLeaf != STL_TRUE) &&
        (SMNMPB_IS_LAST_SIBLING(aPageHandle) == STL_TRUE) )
    {
        *aResult = STL_TRUE;
    }
    else if( (sIsLeaf == STL_TRUE) && (sLastKeySeq == -1) )
    {
        *aResult = STL_TRUE;
    }
    else
    {
        sLastKey = smpGetNthRowItem( aPageHandle, sLastKeySeq );

        STL_TRY( smnmpbCheckMinRange( sLastKey,
                                      aFetchInfo->mRange->mMinRange,
                                      sIsLeaf,
                                      aResult,
                                      aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 특정 Internal node에서 key range에 맞는 첫 child 노드 pid를 찾는다
 * @param[in]     aPageHandle  조사할 internal page의 handle
 * @param[in]     aIterator    scan 조건과 현재 위치를 저장한 iterator
 * @param[in,out] aFetchInfo   Fetch 정보
 * @param[out]    aChildPid    찾아낸 하위 노드의 Page Id
 * @param[in]     aEnv         Storage Manager Environment
 * @todo Key가 1개인 경우에 검증이 필요합니다.
 */
stlStatus smnmpbFindFirstChild( smpHandle         * aPageHandle,
                                smnmpbIterator    * aIterator,
                                smlFetchInfo      * aFetchInfo,
                                smlPid            * aChildPid,
                                smlEnv            * aEnv )
{
    stlUInt16              sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(aPageHandle);
    stlUInt16              sEffectiveKeyCount = sTotalKeyCount;
    stlInt16               sLeft = 0;
    stlInt16               sRight = sTotalKeyCount - 1;
    stlInt16               sMed = 0;
    stlChar              * sMedKey = NULL;
    stlChar              * sLastKey;
    dtlCompareResult       sResult;
    stlBool                sIsLastSibling = SMNMPB_IS_LAST_SIBLING(aPageHandle);
    smpOffsetTableHeader * sOtHeader = SMP_GET_OFFSET_TABLE_HEADER(SMP_FRAME(aPageHandle));
    stlInt32               sColOrder;
    stlUInt8               sColLenSize;
    stlInt64               sColLen;
    stlChar              * sKeyBody = NULL;
    stlBool                sIsInclude = STL_TRUE;
    knlCompareList       * sCompList;

    STL_DASSERT( sTotalKeyCount > 0 );

    /* 마지막 sibling일 경우 마지막 key는 max Key 이므로 compare에서 제외한다 */
    if( sIsLastSibling == STL_TRUE )
    {
        sRight--;
        sEffectiveKeyCount--;
    }

    while( sLeft <= sRight )
    {
        sMed = (sLeft + sRight) >> 1;
        sMedKey = SMP_GET_NTH_ROW_ITEM( aPageHandle, sOtHeader, sMed );
        sKeyBody = sMedKey + SMNMPB_INTL_SLOT_HDR_SIZE;
        sColOrder = -1;
        sCompList = aFetchInfo->mRange->mMinRange;
        sColLenSize = 0;
        sColLen = 0;
        sIsInclude = STL_TRUE;
            
        while( sCompList != NULL )
        {
            while( sColOrder < sCompList->mColOrder )
            {
                sKeyBody += (sColLenSize + sColLen);
                SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyBody, &sColLenSize, &sColLen );
                sColOrder++;
            }
            
            if( sColLen == 0 )
            {
                if( ( sCompList->mRangeLen == 0 ) &&
                    ( sCompList->mIsIncludeEqual ) )
                {
                    sResult = DTL_COMPARISON_EQUAL;
                }
                else
                {
                    sResult = ( !sCompList->mIsNullFirst ) - ( sCompList->mIsNullFirst );
                }
            }
            else
            {
                if( sCompList->mRangeLen == 0 )  /* range 가 NULL */
                {
                    /* IS NULL or IS NOT NULL */
                    sResult = ( sCompList->mIsIncludeEqual )
                        ? ( sCompList->mIsNullFirst ) - ( !sCompList->mIsNullFirst )
                        : DTL_COMPARISON_EQUAL;
                }
                else 
                {
                    sResult = sCompList->mCompFunc( sKeyBody + sColLenSize,
                                                    sColLen,
                                                    sCompList->mRangeVal,
                                                    sCompList->mRangeLen );

                    if( sCompList->mIsAsc )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        sResult = -sResult;
                    }
                    
                    if( (!sCompList->mIsIncludeEqual) && (sResult == DTL_COMPARISON_EQUAL) )
                    {
                        sIsInclude = STL_FALSE;
                        break;
                    }
                    else
                    {
                        /*
                         * compare next range
                         */
                    }
                }
            }
                
            if( sResult == DTL_COMPARISON_LESS )
            {
                sIsInclude = STL_FALSE;
                break;
            }
            else
            {
                if( sResult == DTL_COMPARISON_GREATER )
                {
                    sIsInclude = STL_TRUE;
                    
                    if( sCompList->mNext != NULL )
                    {
                        if( sCompList->mColOrder != sCompList->mNext->mColOrder )
                        {
                            break;
                        }
                    }
                }
                else
                {
                    sIsInclude = STL_TRUE;
                }
            }
            
            sCompList = sCompList->mNext;
        } /* while( sCompList != NULL ) */
        
        if( sIsInclude == STL_TRUE )
        {
            sRight = sMed - 1;
        }
        else
        {
            sLeft = sMed + 1;
        }
    }
    
    if( sIsInclude == STL_FALSE )
    {
        sMed++;
        sMedKey = SMP_GET_NTH_ROW_ITEM( aPageHandle, sOtHeader, sMed );
    }

    if( sMed >= sEffectiveKeyCount )
    {
        if( sIsLastSibling == STL_TRUE )
        {
            sLastKey = SMP_GET_NTH_ROW_ITEM( aPageHandle, sOtHeader, sTotalKeyCount - 1 );
            SMNMPB_INTL_SLOT_GET_CHILD_PID(sLastKey, aChildPid);
            STL_ASSERT( *aChildPid != SMP_NULL_PID );
        }
        else
        {
            /* 현재 노드에는 해당 key를 삽입할 만한 하위 노드가 없음(SMO) */
            *aChildPid = SMP_NULL_PID;
        }
    }
    else
    {
        SMNMPB_INTL_SLOT_GET_CHILD_PID(sMedKey, aChildPid);
    }

    return STL_SUCCESS;
}


/**
 * @brief 특정 Leaf node에서 min key range에 맞는 첫 Key를 찾는다
 * @param[in]     aPageHandle  조사할 internal page의 handle
 * @param[in,out] aIterator    scan 조건과 현재 위치를 저장한 iterator
 * @param[in]     aEndSeq      Binary Search의 시작 Slot Sequence
 * @param[in,out] aFetchInfo   Fetch 정보
 * @param[out]    aSlotSeq     Iterator가 위치하여야 할 sequence
 * @param[in]     aEnv         Storage Manager Environment
 */
stlStatus smnmpbBeforeFirstLeafKey( smpHandle         * aPageHandle,
                                    smnmpbIterator    * aIterator,
                                    smlFetchInfo      * aFetchInfo,
                                    stlInt16            aEndSeq,
                                    stlInt16          * aSlotSeq,
                                    smlEnv            * aEnv )
{
    stlInt16               sLeft = 0;
    stlInt16               sRight = aEndSeq;
    stlInt16               sMed = 0;
    stlChar              * sMedKey;
    dtlCompareResult       sResult;
    smpOffsetTableHeader * sOtHeader = SMP_GET_OFFSET_TABLE_HEADER(SMP_FRAME(aPageHandle));
    stlInt32               sColOrder;
    stlUInt8               sColLenSize;
    stlInt64               sColLen;
    stlChar              * sKeyBody = NULL;
    stlBool                sIsInclude = STL_TRUE;
    knlCompareList       * sCompList;
    stlUInt16              sSlotHdrSize;

    STL_TRY_THROW( aFetchInfo->mRange->mMinRange != NULL, RAMP_FINISH );
    
    while( sLeft <= sRight )
    {
        sMed = (sLeft + sRight) >> 1;
        sMedKey = SMP_GET_NTH_ROW_ITEM( aPageHandle, sOtHeader, sMed );
        SMNMPB_GET_LEAF_KEY_HDR_SIZE( sMedKey, &sSlotHdrSize );
        sKeyBody = sMedKey + sSlotHdrSize;
        sColOrder = -1;
        sCompList = aFetchInfo->mRange->mMinRange;
        sColLenSize = 0;
        sColLen = 0;
        sIsInclude = STL_TRUE;

        while( sCompList != NULL )
        {
            while( sColOrder < sCompList->mColOrder )
            {
                sKeyBody += (sColLenSize + sColLen);
                SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyBody, &sColLenSize, &sColLen );
                sColOrder++;
            }
            
            if( sColLen == 0 )
            {
                if( ( sCompList->mRangeLen == 0 ) &&
                    ( sCompList->mIsIncludeEqual ) )
                {
                    sResult = DTL_COMPARISON_EQUAL;
                }
                else
                {
                    sResult = ( !sCompList->mIsNullFirst ) - ( sCompList->mIsNullFirst );
                }
            }
            else
            {
                if( sCompList->mRangeLen == 0 )  /* range 가 NULL */
                {
                    /* IS NULL or IS NOT NULL */
                    sResult = ( sCompList->mIsIncludeEqual )
                        ? ( sCompList->mIsNullFirst ) - ( !sCompList->mIsNullFirst )
                        : DTL_COMPARISON_EQUAL;
                }
                else 
                {
                    sResult = sCompList->mCompFunc ( sKeyBody + sColLenSize,
                                                     sColLen,
                                                     sCompList->mRangeVal,
                                                     sCompList->mRangeLen );

                    if( sCompList->mIsAsc )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        sResult = -sResult;
                    }
                    
                    if( (!sCompList->mIsIncludeEqual) && (sResult == DTL_COMPARISON_EQUAL) )
                    {
                        sIsInclude = STL_FALSE;
                        break;
                    }
                    else
                    {
                        /*
                         * compare next range
                         */
                    }
                }
            }
                
            if( sResult == DTL_COMPARISON_LESS )
            {
                sIsInclude = STL_FALSE;
                break;
            }
            else
            {
                if( sResult == DTL_COMPARISON_GREATER )
                {
                    sIsInclude = STL_TRUE;

                    if( sCompList->mNext != NULL )
                    {
                        if( sCompList->mColOrder != sCompList->mNext->mColOrder )
                        {
                            break;
                        }
                    }
                }
                else
                {
                    sIsInclude = STL_TRUE;
                }
            }
            
            sCompList = sCompList->mNext;
        } /* while( sCompList != NULL ) */
        
        if( sIsInclude == STL_TRUE )
        {
            sRight = sMed - 1;
        }
        else
        {
            sLeft = sMed + 1;
        }
    }
    
    if( sIsInclude == STL_FALSE )
    {
        sMed++;
    }

    STL_RAMP( RAMP_FINISH );
    
    /* before first 이므로 현재 발견한 위치 바로 앞으로 세팅 */
    *aSlotSeq = sMed - 1;

    return STL_SUCCESS;
}


/**
 * @brief Leaf node에서 min key range에 부합하는 첫번째 key를 읽을 준비를 한다
 * @param[in]     aPageHandle  위치를 찾을 leaf page
 * @param[in]     aStartPid    최초 출발할 페이지 아이디
 * @param[in]     aIterator    위치를 이동시킬 iterator
 * @param[in,out] aFetchInfo   Fetch 정보
 * @param[in]     aEnv         Storage Manager Environment
 */
stlStatus smnmpbFindFirstLeafKey( smpHandle         * aPageHandle,
                                  smlPid              aStartPid,
                                  smnmpbIterator    * aIterator,
                                  smlFetchInfo      * aFetchInfo,
                                  smlEnv            * aEnv )
{
    void      * sRelHandle = aIterator->mCommon.mRelationHandle;
    stlBool     sResult;
    smlPid      sNextPid;
    smlPid      sCurPid = aStartPid;

    while( STL_TRUE )
    {
        if( smpGetFreeness( aPageHandle ) == SMP_FREENESS_FREE )
        {
            sResult = STL_FALSE;
        }
        else
        {
            STL_TRY( smnmpbCheckMinRangeWithMaxKey( aPageHandle,
                                                    aIterator,
                                                    aFetchInfo,
                                                    &sResult,
                                                    aEnv ) == STL_SUCCESS );
        }

        if( sResult != STL_TRUE )
        {
            /* 현재 노드에 적당한 child가 없음. 다음 노드로 */
            sNextPid = SMNMPB_GET_NEXT_PAGE(aPageHandle);
            
            STL_TRY_THROW( sNextPid != SMP_NULL_PID, RAMP_TERMINATE );
            
            sCurPid = sNextPid;

            if( SMP_IS_DUMMY_HANDLE( aPageHandle ) == STL_FALSE )
            {
                STL_TRY( smpRelease( aPageHandle, aEnv ) == STL_SUCCESS );
            }

            STL_TRY( smpAcquire( NULL,
                                 smsGetTbsId(SME_GET_SEGMENT_HANDLE(sRelHandle)),
                                 sCurPid,
                                 KNL_LATCH_MODE_SHARED,
                                 aPageHandle,
                                 aEnv )
                     == STL_SUCCESS );
            continue;
        }
        
        break;
    }

    STL_RAMP( RAMP_TERMINATE );

    STL_TRY( smnmpbBeforeFirstLeafKey( aPageHandle,
                                       aIterator,
                                       aFetchInfo,
                                       SMP_GET_PAGE_ROW_ITEM_COUNT( aPageHandle ) - 1,
                                       &aIterator->mCurSlotSeq,
                                       aEnv ) == STL_SUCCESS );

    aIterator->mPageBufPid = sCurPid;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 페이지의 Snapshot을 만든다.
 * @param[in] aTbsId     Tablespace Identifier
 * @param[in] aPageId    Page Identifier
 * @param[in] aDstHandle Snapshoted Root 페이지 핸들
 * @param[in] aPageFrame Snapshot 버퍼 공간
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbSnapshot( smlTbsId    aTbsId,
                          smlPid      aPageId,
                          smpHandle * aDstHandle,
                          stlChar   * aPageFrame,
                          smlEnv    * aEnv )
{
    smpHandle  sPageHandle;
    stlInt32   sState = 0;
    
    STL_TRY( smpFix( aTbsId,
                     aPageId,
                     &sPageHandle,
                     aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smpSnapshot( &sPageHandle,
                          (stlChar*)aPageFrame,
                          aEnv )
             == STL_SUCCESS );

    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
            
    SMP_MAKE_DUMMY_HANDLE( aDstHandle, aPageFrame );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief Iterator에서 첫번째 key를 읽을 준비를 한다
 * @param[in]     aIterator     위치를 이동시킬 iterator
 * @param[in,out] aFetchInfo    Fetch 정보
 * @param[out]    aPageHandle   찾은 첫 leaf page
 * @param[in]     aEnv Storage  Manager Environment
 */
stlStatus smnmpbFindFirst( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smpHandle     * aPageHandle,
                           smlEnv        * aEnv )
{
    smpHandle        sPageHandle;
    smnmpbIterator * sIterator = (smnmpbIterator*)aIterator;
    void           * sRelHandle = sIterator->mCommon.mRelationHandle;
    smlPid           sCurPid;
    smlPid           sChildPid;
    stlBool          sResult;
    stlInt32         sState = 0;
    smlTbsId         sTbsId = smsGetTbsId(SME_GET_SEGMENT_HANDLE(sRelHandle));
    stlInt64         sDepth;
    smlPid           sRootPid;
    stlInt32         sFixState = 0;

    SMP_MAKE_DUMMY_HANDLE( &sPageHandle, NULL );
    
    STL_RAMP( RAMP_RETRY_FROM_ROOT );
    
    sDepth = -1;
    sCurPid = SMN_GET_ROOT_PAGE_ID(sRelHandle);
    sRootPid = sCurPid;

    while( STL_TRUE )
    {
        sDepth++;
        
        /**
         * Scan Scalability를 높이기 위해서 Latch의 사용보다는
         * Mirror 페이지를 이용한다.
         */

        if( (sRootPid == sCurPid) &&
            (SMN_GET_MIRROR_ROOT_PID( sRelHandle ) != SMP_NULL_PID) )
        {
            STL_TRY( smpFix( sTbsId,
                             SMN_GET_MIRROR_ROOT_PID( sRelHandle ),
                             &sPageHandle,
                             aEnv )
                     == STL_SUCCESS );
            sFixState = 1;
        }
        else
        {
            STL_TRY( smpAcquire( NULL,
                                 sTbsId,
                                 sCurPid,
                                 KNL_LATCH_MODE_SHARED,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
            sState = 1;
        
            if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
            {
                /* 이미 Free된 Page에 도달. 처음서 부터 다시 시도 */
                if( sState == 1 )
                {
                    sState = 0;
                    STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
                }
            
                if( sFixState == 1 )
                {
                    sFixState = 0;
                    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
                }
            
                STL_THROW( RAMP_RETRY_FROM_ROOT );
            }
        }

        if( SMNMPB_IS_LEAF(&sPageHandle) == STL_TRUE )
        {
            /**
             * Leaf는 Mirror페이지가 될수 없다.
             */
            STL_DASSERT( sFixState == 0 );
            
            sChildPid = sCurPid;
            break;
        }

        while( STL_TRUE )
        {
            /* Sibling으로 이동중에는 free node를 만날 수 없다 */
            STL_DASSERT( (sRootPid == sCurPid) ||
                         (smpGetFreeness( &sPageHandle ) != SMP_FREENESS_FREE) );

            STL_TRY( smnmpbCheckMinRangeWithMaxKey( &sPageHandle,
                                                    aIterator,
                                                    aFetchInfo,
                                                    &sResult,
                                                    aEnv ) == STL_SUCCESS );
            if( sResult != STL_TRUE )
            {
                /* 현재 노드에 적당한 child가 없음. 다음 노드로 */
                sCurPid = SMNMPB_GET_NEXT_PAGE(&sPageHandle);
                if( sCurPid == SMP_NULL_PID )
                {
                    /**
                     * Max Node로 이동한다.
                     */
                    sChildPid = smnmpbGetLastChild( &sPageHandle );
                    
                    if( sState == 1 )
                    {
                        sState = 0;
                        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
                    }
                    
                    if( sFixState == 1 )
                    {
                        sFixState = 0;
                        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
                    }
            
                    sCurPid = sChildPid;
                    break;
                }

                if( sState == 1 )
                {
                    sState = 0;
                    STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
                }
                    
                if( sFixState == 1 )
                {
                    sFixState = 0;
                    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
                }
                    
                STL_TRY( smpAcquire( NULL,
                                     sTbsId,
                                     sCurPid,
                                     KNL_LATCH_MODE_SHARED,
                                     &sPageHandle,
                                     aEnv ) == STL_SUCCESS );
                sState = 1;
                
                continue;
            }
            
            STL_TRY( smnmpbFindFirstChild( &sPageHandle,
                                           sIterator,
                                           aFetchInfo,
                                           &sChildPid,   /* out */
                                           aEnv ) == STL_SUCCESS );
            STL_ASSERT( sChildPid != SMP_NULL_PID );
            
            if( sState == 1 )
            {
                sState = 0;
                STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
            }
            
            if( sFixState == 1 )
            {
                sFixState = 0;
                STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
            }
            
            sCurPid = sChildPid;
            break;
        }
    }

    STL_TRY( smnmpbFindFirstLeafKey( &sPageHandle,
                                     sCurPid,
                                     sIterator,
                                     aFetchInfo,
                                     aEnv ) == STL_SUCCESS );

    *aPageHandle = sPageHandle;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        if( SMP_IS_DUMMY_HANDLE( &sPageHandle ) == STL_FALSE )
        {
            (void)smpRelease( &sPageHandle, aEnv );
        }
    }
    
    if( sFixState == 1 )
    {
        if( SMP_IS_DUMMY_HANDLE( &sPageHandle ) == STL_FALSE )
        {
            (void)smpUnfix( &sPageHandle, aEnv );
        }
    }
    
    return STL_FAILURE;
}

/**
 * @brief 주어진 key가 읽을 수 있는 버전인지 조사하고 주어진 컬럼들을 copy한다
 * @param[in] aColPtr 조사할 Key의 첫 컬럼의 위치
 * @param[in,out] aColList 선택된 Key를 담을 컬럼 정보구조체
 * @param[in] aBlockIdx aColList에서 사용할 block idx
 */ 
inline void smnmpbFetchKeyValues( stlChar           * aColPtr,
                                  knlValueBlockList * aColList,
                                  stlInt32            aBlockIdx )
{
    stlChar           * sCurPtr = aColPtr;
    knlValueBlockList * sCurCol = aColList;
    stlInt64            sColLen;
    dtlDataValue      * sDataValue;

    while( sCurCol != NULL )
    {
        sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE(sCurCol, aBlockIdx);
        SMP_READ_COLUMN_AND_MOVE_PTR( &sCurPtr,
                                      sDataValue,
                                      &sColLen );
        sCurCol = sCurCol->mNext;
    }
}


/**
 * @brief 주어진 key가 읽을 수 있는 버전인지 조사하고 주어진 컬럼들을 copy한다
 * @param[in] aIterator 검색을 하는 iterator
 * @param[in,out] aFetchInfo Fetch 정보
 * @param[in,out] aTablePageHandle 목적 레코드가 존재하는 Page Handle
 * @param[in,out] aTablePageLatchRelease aTablePageHandle의 Latch 여부
 * @param[in] aMyTransId iterator의 tx id
 * @param[in] aMyViewScn iterator의 view scn
 * @param[in] aMyCommitScn iterator의 Commit scn
 * @param[in] aMyViewTcn iterator의 view tcn
 * @param[in] aKey 조사할 key의 위치
 * @param[out] aIsMatch 주어진 iterator의 view로 검색한 row와 key가 같은 값을 가지는 지 여부
 * @param[out] aCommitScn 검색된 row의 commit scn
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbCheckRowColWithKey( smnmpbIterator    * aIterator,
                                    smlFetchInfo      * aFetchInfo,
                                    smpHandle         * aTablePageHandle,
                                    stlBool           * aTablePageLatchRelease,
                                    smxlTransId         aMyTransId,
                                    smlScn              aMyViewScn,
                                    smlScn              aMyCommitScn,
                                    smlTcn              aMyViewTcn,
                                    stlChar           * aKey,
                                    stlBool           * aIsMatch,
                                    smlScn            * aCommitScn,
                                    smlEnv            * aEnv )
{
    smlRid              sRowRid;
    stlChar           * sCurPtr;
    stlUInt16           sSlotHdrSize;
    stlUInt64           sColLen;
    stlUInt8            sColLenSize;
    knlKeyCompareList * sKeyCompList = aFetchInfo->mKeyCompList;
    stlInt32            i;
    knlValueBlockList * sCurCol = sKeyCompList->mValueList;

    SMNMPB_GET_LEAF_KEY_HDR_SIZE( aKey, &sSlotHdrSize );
    sCurPtr = aKey + sSlotHdrSize;

    for( i = 0; i < sKeyCompList->mIndexColCount; i++ )
    {
        SMP_GET_COLUMN_LEN_ZERO_INSENS( sCurPtr, &sColLenSize, &sColLen );
        sKeyCompList->mIndexColVal[sKeyCompList->mIndexColOffset[i]] = sCurPtr + sColLenSize;
        sKeyCompList->mIndexColLen[sKeyCompList->mIndexColOffset[i]] = sColLen;
        sCurPtr += sColLenSize + sColLen;
        sCurCol = sCurCol->mNext;
    }

    sRowRid.mTbsId = aIterator->mBaseRelTbsId;
    SMNMPB_GET_ROW_PID( aKey, &sRowRid.mPageId );
    SMNMPB_GET_ROW_SEQ( aKey, &sRowRid.mOffset );

    STL_TRY( smdCompareKeyValue( aIterator->mBaseRelHandle,
                                 &sRowRid,
                                 aTablePageHandle,
                                 aTablePageLatchRelease,
                                 aMyTransId,
                                 aMyViewScn,
                                 aMyCommitScn,
                                 aMyViewTcn,
                                 sKeyCompList,
                                 aIsMatch,
                                 aCommitScn,
                                 aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** * @brief 주어진 key가 읽을 수 있는 버전인지 조사하고 주어진 컬럼들을 copy한다
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageHandle 조사할 row가 있는 page의 핸들
 * @param[in] aIterator 검색 정보를 담고 있는 iterator
 * @param[in,out] aFetchInfo   Fetch 정보
 * @param[in] aKey 조사할 key의 위치
 * @param[in,out] aColList 선택된 Key를 담을 컬럼 정보구조체
 * @param[in] aBlockIdx aColList에서 사용할 Block Idx
 * @param[out] aCommitScn 반환되는 버전의 commit SCN
 * @param[out] aWaitTransId 반환되는 버전이 commit되지 않아서 기다려야 되는 transaction id
 * @param[out] aHasValidVersion 읽을 수 있는 버전인지 여부
 * @param[out] aMatchPhysicalFilter Key-Filter 조건을 만족하는지 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbGetValidVersion( smlTbsId               aTbsId,
                                 smpHandle            * aPageHandle,
                                 smnmpbIterator       * aIterator,
                                 smlFetchInfo         * aFetchInfo,
                                 stlChar              * aKey,
                                 knlValueBlockList    * aColList,
                                 stlInt32               aBlockIdx,
                                 smlScn               * aCommitScn,
                                 smxlTransId          * aWaitTransId,
                                 stlBool              * aHasValidVersion,
                                 stlBool              * aMatchPhysicalFilter,
                                 smlEnv               * aEnv )
{
    stlChar             * sCurPtr;
    stlBool               sIsMatch = STL_FALSE;
    smxlTransId           sMyTransId = SMA_GET_TRANS_ID( aIterator->mCommon.mStatement );
    smlScn                sMyViewScn = aIterator->mCommon.mViewScn;
    smlScn                sMyCommitScn = SMA_GET_COMMIT_SCN(aIterator->mCommon.mStatement);
    smlTcn                sMyViewTcn = aIterator->mCommon.mViewTcn;
    smlScn                sInsertCommitScn = SML_INFINITE_SCN;
    smxlTransId           sInsertTransId = SML_INVALID_TRANSID;
    smlScn                sDeleteCommitScn = SML_INFINITE_SCN;
    smxlTransId           sDeleteTransId = SML_INVALID_TRANSID;
    stlBool               sIsDeleted;
    stlUInt16             sSlotHdrSize;
    /* Tx relation case
       0 : My ViewScn < Insert commit Scn
       1 : insert commit scn <= My ViewScn < delete commit Scn
       2 : delete commit scn <= My ViewScn                       */
    stlInt16              sTxRelCase = -1;

    knlPhysicalFilter   * sKeyFilter;
    stlUInt8              sColLenSize;
    stlInt64              sColLen = 0;
    knlValueBlockList   * sValueBlock;
    knlColumnInReadRow  * sCurInReadRow;
    stlInt32              sColOrder = 0;
    stlBool               sRowLevelVisibility = STL_FALSE;
    smlTcn                sInsertTcn;
    smlTcn                sDeleteTcn;
    dtlDataValue        * sDataValue;
    smlRid                sPrevRowId = SML_INVALID_RID;
    smlRid                sRowRid;
    smpHandle             sTablePageHandle;
    stlBool               sTablePageLatchRelease = STL_TRUE;

    *aMatchPhysicalFilter = STL_FALSE;
    *aHasValidVersion = STL_FALSE;

    SMNMPB_GET_LEAF_KEY_HDR_SIZE( aKey, &sSlotHdrSize );
    sCurPtr = aKey + sSlotHdrSize;

    if( SMXL_IS_AGABLE_VIEW_SCN(aIterator->mAgableViewScn, smpGetMaxViewScn(aPageHandle)) )
    {
        sIsDeleted = SMNMPB_IS_DELETED( aKey );

        if( sIsDeleted == STL_TRUE )
        {
            *aCommitScn = SML_INFINITE_SCN;
            *aWaitTransId = SML_INVALID_TRANSID;
            *aHasValidVersion = STL_FALSE;
        }
        else
        {
            *aCommitScn = 0;
            *aWaitTransId = SML_INVALID_TRANSID;
            *aHasValidVersion = STL_TRUE;
        }
        
        STL_THROW( SKIP_CHECK_VALID_VERSION );
    }
    
    STL_TRY( smnmpbGetInsertCommitScn( aTbsId,
                                       aPageHandle,
                                       aKey,
                                       sMyViewScn,
                                       &sInsertCommitScn,
                                       &sInsertTcn,
                                       &sInsertTransId,
                                       aEnv ) == STL_SUCCESS );

    sIsDeleted = SMNMPB_IS_DELETED( aKey );
    if( sIsDeleted == STL_TRUE )
    {
        STL_TRY( smnmpbGetDeleteCommitScn( aTbsId,
                                           aPageHandle,
                                           aKey,
                                           sMyViewScn,
                                           &sDeleteCommitScn,
                                           &sDeleteTcn,
                                           &sDeleteTransId,
                                           aEnv ) == STL_SUCCESS );
    }

    if( (smnmpbIsSameTrans(sMyTransId, sMyCommitScn, sInsertTransId, sInsertCommitScn) == STL_TRUE) ||
        ( (sIsDeleted == STL_TRUE) &&
          (smnmpbIsSameTrans(sMyTransId, sMyCommitScn, sDeleteTransId, sDeleteCommitScn) == STL_TRUE) ) )
    {
        if( sMyViewTcn == SML_INFINITE_TCN )
        {
            if( sIsDeleted == STL_TRUE )
            {
                sTxRelCase = 2;
            }
            else
            {
                sTxRelCase = 1;
            }
        }
        else
        {
            if( sInsertCommitScn != SML_INFINITE_SCN )
            {
                if( sInsertCommitScn > sMyViewScn )
                {
                    if( SMNMPB_IS_DUPKEY( aKey ) == STL_TRUE )
                    {
                        sRowLevelVisibility = STL_TRUE;
                    }
                    else
                    {
                        /* invalid version */
                        sTxRelCase = 0;
                    }
                    
                    STL_THROW( RAMP_ROW_LEVEL_VISIBILITY );
                }

                if( sIsDeleted == STL_TRUE )
                {
                    if( sDeleteTcn == SMNMPB_INVALID_TCN )
                    {
                        sRowLevelVisibility = STL_TRUE;
                        STL_THROW( RAMP_ROW_LEVEL_VISIBILITY );
                    }
                
                    if( sDeleteTcn < sMyViewTcn )
                    {
                        /* invalid version */
                        sTxRelCase = 2;
                    }
                    else
                    {
                        /* valid version */
                        sTxRelCase = 1;
                    }
                }
                else
                {
                    /* valid version */
                    sTxRelCase = 1;
                }
                
                STL_THROW( RAMP_ROW_LEVEL_VISIBILITY );
            }
            
            if( sInsertTcn == SMNMPB_INVALID_TCN )
            {
                sRowLevelVisibility = STL_TRUE;
                STL_THROW( RAMP_ROW_LEVEL_VISIBILITY );
            }
            else
            {
                if( sIsDeleted == STL_TRUE )
                {
                    if( sDeleteTcn == SMNMPB_INVALID_TCN )
                    {
                        sRowLevelVisibility = STL_TRUE;
                        STL_THROW( RAMP_ROW_LEVEL_VISIBILITY );
                    }
                }
            }

            if( sInsertTcn >= sMyViewTcn )
            {
                if( SMNMPB_IS_DUPKEY( aKey ) == STL_TRUE )
                {
                    sRowLevelVisibility = STL_TRUE;
                    STL_THROW( RAMP_ROW_LEVEL_VISIBILITY );
                }
                else
                {
                    /* invalid version */
                    sTxRelCase = 0;
                }
            }
            else
            {
                if( sIsDeleted == STL_TRUE)
                {
                    if( sDeleteTcn < sMyViewTcn )
                    {
                        /* invalid version */
                        sTxRelCase = 2;
                    }
                    else
                    {
                        /* valid version */
                        sTxRelCase = 1;
                    }
                }
                else
                {
                    /* valid version */
                    sTxRelCase = 1;
                }
            }
        }
    }
    else
    {
        while( STL_TRUE )
        {
            if( sInsertCommitScn != SML_INFINITE_SCN )
            {
                if( sInsertCommitScn > sMyViewScn )
                {
                    if( SMNMPB_IS_DUPKEY( aKey ) == STL_TRUE )
                    {
                        sRowLevelVisibility = STL_TRUE;
                    }
                    else
                    {
                        /* invalid version */
                        sTxRelCase = 0;
                    }
                    break;
                }
                else
                {
                    /* 내 tx가 아닌 tx 가 insert 했음 */
                    /* delete commit scn 체크해 봐야 함 */
                }
            }
            else
            {
                if( SMNMPB_IS_DUPKEY( aKey ) == STL_TRUE )
                {
                    sRowLevelVisibility = STL_TRUE;
                }
                else
                {
                    /* invalid version */
                    sTxRelCase = 0;
                }
                break;
            }

            STL_ASSERT( sTxRelCase == -1 );
            if( sIsDeleted != STL_TRUE )
            {
                sTxRelCase = 1;
                break;
            }

            if( sDeleteCommitScn != SML_INFINITE_SCN )
            {
                if( sDeleteCommitScn <= sMyViewScn )
                {
                    sTxRelCase = 2;
                }
                else
                {
                    sTxRelCase = 1;
                }
            }
            else
            {
                sTxRelCase = 1;
            }

            STL_ASSERT( sTxRelCase >= 0 );
            break;
        } /* while */
    }

    STL_RAMP( RAMP_ROW_LEVEL_VISIBILITY );
    
    if( sRowLevelVisibility == STL_TRUE )
    {
        sRowRid.mTbsId = aIterator->mBaseRelTbsId;
        SMNMPB_GET_ROW_PID( aKey, &sRowRid.mPageId );
        SMNMPB_GET_ROW_SEQ( aKey, &sRowRid.mOffset );

        if ( (sPrevRowId.mTbsId  == sRowRid.mTbsId) &&
             (sPrevRowId.mPageId == sRowRid.mPageId) &&
             (sTablePageLatchRelease == STL_FALSE) )
        {
            /**
             * 기존 Page 를 그대로 사용한다
             */
        }
        else
        {
            if( sTablePageLatchRelease == STL_FALSE )
            {
                sTablePageLatchRelease = STL_TRUE;
                STL_TRY( smpRelease( &sTablePageHandle, aEnv ) == STL_SUCCESS );
            }
                
            STL_TRY( smpAcquire( NULL,
                                 sRowRid.mTbsId,
                                 sRowRid.mPageId,
                                 KNL_LATCH_MODE_SHARED,
                                 &sTablePageHandle,
                                 aEnv ) == STL_SUCCESS );

            sPrevRowId.mTbsId  = sRowRid.mTbsId;
            sPrevRowId.mPageId = sRowRid.mPageId;
            sTablePageLatchRelease = STL_FALSE;
        }

        STL_TRY( smnmpbCheckRowColWithKey( aIterator,
                                           aFetchInfo,
                                           &sTablePageHandle,
                                           &sTablePageLatchRelease,
                                           sMyTransId,
                                           sMyViewScn,
                                           sMyCommitScn,
                                           sMyViewTcn,
                                           aKey,
                                           &sIsMatch,
                                           &sInsertCommitScn,
                                           aEnv ) == STL_SUCCESS );

        if( sTablePageLatchRelease == STL_FALSE )
        {
            sTablePageLatchRelease = STL_TRUE;
            STL_TRY( smpRelease( &sTablePageHandle, aEnv ) == STL_SUCCESS );
        }
            
        if( sIsMatch == STL_TRUE )
        {
            sTxRelCase = 1;
        }
        else
        {
            sTxRelCase = 0;
        }
    }

    STL_ASSERT( sTxRelCase >= 0 );

    switch( sTxRelCase )
    {
        case 0:
        case 2:
            *aCommitScn = SML_INFINITE_SCN;
            *aWaitTransId = SML_INVALID_TRANSID;
            *aHasValidVersion = STL_FALSE;
            break;
        case 1:
            *aCommitScn = sInsertCommitScn;
            *aWaitTransId = sInsertTransId;
            *aHasValidVersion = STL_TRUE;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }

    STL_RAMP( SKIP_CHECK_VALID_VERSION );

    if( *aHasValidVersion == STL_TRUE )
    {
        aIterator->mRowRid.mTbsId = aIterator->mBaseRelTbsId;
        SMNMPB_GET_ROW_PID( aKey, &aIterator->mRowRid.mPageId );
        SMNMPB_GET_ROW_SEQ( aKey, &aIterator->mRowRid.mOffset );

        /* set valueblock */
        sValueBlock = aColList;
        
        while( sValueBlock != NULL )
        {
            SMP_GET_COLUMN_LEN_ZERO_INSENS( sCurPtr, & sColLenSize, & sColLen );
            sCurPtr += sColLenSize;

            if( sColOrder == KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock ) )
            {
                sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( sValueBlock );
                sCurInReadRow->mValue  = sCurPtr;
                sCurInReadRow->mLength = sColLen;

                sValueBlock = sValueBlock->mNext;
            }

            sCurPtr += sColLen;
            sColOrder++;
        }
        
        /* key-filter 평가 */
        sKeyFilter  = aFetchInfo->mPhysicalFilter;
        *aMatchPhysicalFilter = STL_TRUE;
            
        while( sKeyFilter != NULL )
        {
            if( sKeyFilter->mFunc( sKeyFilter->mColVal1->mValue,
                                   sKeyFilter->mColVal1->mLength,
                                   sKeyFilter->mColVal2->mValue,
                                   sKeyFilter->mColVal2->mLength )
                == STL_FALSE )
            {
                *aMatchPhysicalFilter = STL_FALSE;
                break;
            }

            sKeyFilter = sKeyFilter->mNext;
        }

        if( *aMatchPhysicalFilter == STL_TRUE )
        {
            /* fetch key values */
            sValueBlock = aColList;
        
            while( sValueBlock != NULL )
            {
                STL_DASSERT( KNL_GET_BLOCK_USED_CNT( sValueBlock ) <= aBlockIdx );
                
                sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( sValueBlock );

                sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sValueBlock, aBlockIdx );
                
                DTL_COPY_VALUE( sDataValue,
                                sCurInReadRow->mValue,
                                sCurInReadRow->mLength );

                sDataValue->mLength = sCurInReadRow->mLength;
                sValueBlock = sValueBlock->mNext;
            }
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sTablePageLatchRelease == STL_FALSE )
    {
        sTablePageLatchRelease = STL_TRUE;
        (void) smpRelease( &sTablePageHandle, aEnv );
    }

    return STL_FAILURE;
}


/**
 * @brief 주어진 Stable 페이지에서 읽을 수 있는 버전의 Key들을 조사하고 주어진 컬럼들을 copy한다
 * @param[in] aIterator 검색 정보를 담고 있는 iterator
 * @param[in] aPageFrame 페이지 이미지
 * @param[in,out] aFetchInfo   Fetch 정보
 * @param[out] aEndOfSlot  Slot Scan 의 종료 여부
 * @param[out] aEndOfScan  Iterator Scan 의 종료 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbGetValidVersionsStablePage( smnmpbIterator  * aIterator,
                                            stlChar         * aPageFrame,
                                            smlFetchInfo    * aFetchInfo,
                                            stlBool         * aEndOfSlot,
                                            stlBool         * aEndOfScan,
                                            smlEnv          * aEnv )
{
    stlChar             * sCurPtr;
    stlUInt16             sSlotHdrSize;
    knlPhysicalFilter   * sKeyFilter;
    stlUInt8              sColLenSize;
    stlInt64              sColLen = 0;
    knlValueBlockList   * sValueBlock;
    knlColumnInReadRow  * sCurInReadRow;
    stlInt32              sColOrder = 0;
    dtlDataValue        * sDataValue;
    stlChar             * sKey;
    stlBool               sMatchPhysicalFilter;
    smpRowItemIterator  * sSlotIterator;
    stlInt32              sBlockIdx = 0;
    stlInt16              sSlotSeq;
    stlInt64              sBlockSize = SML_BLOCK_SIZE( aFetchInfo->mRowBlock );
    stlInt32              sAggrRowCount = 0;

    smlRid    sPrevRowId = SML_INVALID_RID;
    smpHandle sTablePageHandle;
    stlBool   sTablePageLatchRelease = STL_TRUE;
    smnIndexHeader      * sIndexHeader = (smnIndexHeader *)(aIterator->mCommon.mRelationHandle);
    
    sSlotIterator = &aIterator->mCommon.mSlotIterator;
    
    if( aFetchInfo->mAggrFetchInfo != NULL )
    {
        sBlockIdx = 0;
    }
    else
    {
        sBlockIdx = SML_USED_BLOCK_SIZE( aFetchInfo->mRowBlock );
    }

    *aEndOfSlot = STL_TRUE;
    *aEndOfScan = STL_FALSE;

    SMP_FOREACH_ROWITEM_ITERATOR( sSlotIterator )
    {
        aIterator->mCurSlotSeq++;
        sSlotSeq = SMP_GET_CURRENT_ROWITEM_SEQ( sSlotIterator );
        
        if( sSlotSeq >= aIterator->mSlotSeqFence )
        {
            *aEndOfScan = STL_TRUE;
            break;
        }

        sKey = SMP_GET_CURRENT_ROWITEM( sSlotIterator );
    
        SMNMPB_GET_LEAF_KEY_HDR_SIZE( sKey, &sSlotHdrSize );
        sCurPtr = sKey + sSlotHdrSize;

        if( SMNMPB_IS_DELETED( sKey ) == STL_TRUE )
        {
            continue;
        }

        aIterator->mRowRid.mTbsId = aIterator->mBaseRelTbsId;
        SMNMPB_GET_ROW_PID( sKey, &aIterator->mRowRid.mPageId );
        SMNMPB_GET_ROW_SEQ( sKey, &aIterator->mRowRid.mOffset );

        sColOrder = 0;
        
        /* set valueblock */
        sValueBlock = aFetchInfo->mColList;
        
        while( sValueBlock != NULL )
        {
            SMP_GET_COLUMN_LEN_ZERO_INSENS( sCurPtr, & sColLenSize, & sColLen );
            sCurPtr += sColLenSize;

            if( sColOrder == KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock ) )
            {
                sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( sValueBlock );
                sCurInReadRow->mValue  = sCurPtr;
                sCurInReadRow->mLength = sColLen;

                sValueBlock = sValueBlock->mNext;
            }

            sCurPtr += sColLen;
            sColOrder++;
        }
        
        /* key-filter 평가 */
        sKeyFilter  = aFetchInfo->mPhysicalFilter;
        sMatchPhysicalFilter = STL_TRUE;
            
        while( sKeyFilter != NULL )
        {
            if( sKeyFilter->mFunc( sKeyFilter->mColVal1->mValue,
                                   sKeyFilter->mColVal1->mLength,
                                   sKeyFilter->mColVal2->mValue,
                                   sKeyFilter->mColVal2->mLength )
                == STL_FALSE )
            {
                sMatchPhysicalFilter = STL_FALSE;
                break;
            }

            sKeyFilter = sKeyFilter->mNext;
        }

        if( sMatchPhysicalFilter == STL_FALSE )
        {
            continue;
        }
        
        /* fetch key values */
        sValueBlock = aFetchInfo->mColList;
        
        while( sValueBlock != NULL )
        {
            STL_DASSERT( KNL_GET_BLOCK_USED_CNT( sValueBlock ) <= sBlockIdx );
                
            sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( sValueBlock );

            sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sValueBlock, sBlockIdx );
            sDataValue->mLength = sCurInReadRow->mLength;
            
            DTL_COPY_VALUE( sDataValue,
                            sCurInReadRow->mValue,
                            sCurInReadRow->mLength );

            sValueBlock = sValueBlock->mNext;
        }
            
        /**
         * RowId Column을 참조하는 경우, RowId Column Block에 값을 만든다. 
         */
        if( aFetchInfo->mRowIdColList != NULL )
        {
            STL_TRY( dtlRowIdSetValueFromRowIdInfo(
                         aFetchInfo->mTableLogicalId,
                         aIterator->mRowRid.mTbsId,
                         aIterator->mRowRid.mPageId,
                         aIterator->mRowRid.mOffset,
                         KNL_GET_BLOCK_SEP_DATA_VALUE( aFetchInfo->mRowIdColList,
                                                       sBlockIdx ),
                         KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );            
        }
        
        /* logical key filter 평가 */
        if( aFetchInfo->mFilterEvalInfo != NULL )
        {
            aFetchInfo->mFilterEvalInfo->mBlockIdx = sBlockIdx;

            STL_TRY( knlEvaluateOneExpression( aFetchInfo->mFilterEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );
                        
            STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF(
                             aFetchInfo->mFilterEvalInfo->mResultBlock )
                         == STL_TRUE );
            
            if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_SEP_DATA_VALUE(
                                          aFetchInfo->mFilterEvalInfo->mResultBlock,
                                          sBlockIdx ) ) )
            {
                continue;
            }
        }

        if( aFetchInfo->mFetchRecordInfo != NULL )
        {
            if ( (sPrevRowId.mTbsId  == aIterator->mRowRid.mTbsId) &&
                 (sPrevRowId.mPageId == aIterator->mRowRid.mPageId) &&
                 (sTablePageLatchRelease == STL_FALSE) )
            {
                /**
                 * 기존 Page 를 그대로 사용한다
                 */
            }
            else
            {
                if( sTablePageLatchRelease == STL_FALSE )
                {
                    sTablePageLatchRelease = STL_TRUE;
                    STL_TRY( smpRelease( &sTablePageHandle, aEnv ) == STL_SUCCESS );
                }
                
                STL_TRY( smpAcquire( NULL,
                                     aIterator->mRowRid.mTbsId,
                                     aIterator->mRowRid.mPageId,
                                     KNL_LATCH_MODE_SHARED,
                                     &sTablePageHandle,
                                     aEnv ) == STL_SUCCESS );

                sPrevRowId.mTbsId  = aIterator->mRowRid.mTbsId;
                sPrevRowId.mPageId = aIterator->mRowRid.mPageId;
                sTablePageLatchRelease = STL_FALSE;
            }

            STL_TRY( smdFetchRecord4Index( (smiIterator*)aIterator,
                                           sIndexHeader->mBaseRelHandle,
                                           aFetchInfo->mFetchRecordInfo,
                                           & sTablePageHandle,
                                           & sTablePageLatchRelease,
                                           & aIterator->mRowRid,
                                           sBlockIdx,
                                           aIterator->mAgableViewScn,
                                           & sMatchPhysicalFilter,
                                           aEnv ) == STL_SUCCESS );

            if( sMatchPhysicalFilter == STL_FALSE )
            {
                continue;
            }
        }

        if( aFetchInfo->mAggrFetchInfo != NULL )
        {
            sBlockIdx++;
            
            if( sBlockIdx == sBlockSize )
            {
                STL_TRY( smoBlockBuildAggrFunc( aFetchInfo->mAggrFetchInfo,
                                                0, /* aStartBlockIdx */
                                                sBlockSize,
                                                aEnv )
                         == STL_SUCCESS );
                sBlockIdx = 0;
                sAggrRowCount = 1;
            }
        }
        else
        {
            if( aFetchInfo->mSkipCnt > 0 )
            {
                aFetchInfo->mSkipCnt--;
                continue;
            }
            else
            {
                SML_SET_RID_VALUE( aFetchInfo->mRowBlock, sBlockIdx, aIterator->mRowRid );
                SML_SET_SCN_VALUE( aFetchInfo->mRowBlock, sBlockIdx, SMI_GET_VIEW_SCN( aIterator ) );

                aFetchInfo->mFetchCnt--;
                sBlockIdx++;
            }
 
        
            if( (aFetchInfo->mFetchCnt == 0) ||
                (sBlockIdx == sBlockSize) )
            {
                *aEndOfSlot = STL_FALSE;
                *aEndOfScan = STL_FALSE;
                break;
            }
        }
    }

    if( sTablePageLatchRelease == STL_FALSE )
    {
        sTablePageLatchRelease = STL_TRUE;
        STL_TRY( smpRelease( &sTablePageHandle, aEnv ) == STL_SUCCESS );
    }
    
    if( aFetchInfo->mAggrFetchInfo == NULL )
    {
        if( aFetchInfo->mColList != NULL )
        {
            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aFetchInfo->mColList, 0, sBlockIdx );
        }

        if( aFetchInfo->mRowIdColList != NULL )
        {
            KNL_SET_ONE_BLOCK_SKIP_CNT( aFetchInfo->mRowIdColList, 0 );
            KNL_SET_ONE_BLOCK_USED_CNT( aFetchInfo->mRowIdColList, sBlockIdx );
        }
    
        if( aFetchInfo->mFetchRecordInfo != NULL )
        {
            if( aFetchInfo->mFetchRecordInfo->mColList != NULL )
            {
                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
                    aFetchInfo->mFetchRecordInfo->mColList,
                    0,
                    sBlockIdx );
            }

            /**
             * Logical KeyFilter와 Logical Table Filter에 모두 RowId Column이 있는 경우,
             * RowId Column에 값이 중복 처리되는 것을 방지하기 위해,
             * sIterator->mCommon.mFetchRecordInfo->mRowIdColList 는 NULL임.
             * smdmphFetchRecord4Index()함수에서 호출되는
             * smdmphGetValidVersion에서 RowId 값을 설정하지 않도록 ...
             */
            STL_DASSERT( aFetchInfo->mFetchRecordInfo->mRowIdColList == NULL );
        }
    
        SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sBlockIdx );
    }
    else
    {
        if( sBlockIdx > 0 )
        {
            STL_TRY( smoBlockBuildAggrFunc( aFetchInfo->mAggrFetchInfo,
                                            0, /* aStartBlockIdx */
                                            sBlockIdx,
                                            aEnv )
                     == STL_SUCCESS );
            sAggrRowCount = 1;
        }

        SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sAggrRowCount );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sTablePageLatchRelease == STL_FALSE )
    {
        sTablePageLatchRelease = STL_TRUE;
        (void) smpRelease( &sTablePageHandle, aEnv );
    }
    
    return STL_FAILURE;
}


/**
 * @brief 주어진 Unstable 페이지에서 읽을 수 있는 버전의 Key들을 조사하고 주어진 컬럼들을 copy한다
 * @param[in] aIterator 검색 정보를 담고 있는 iterator
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageHandle 페이지 Handle
 * @param[in,out] aFetchInfo   Fetch 정보
 * @param[out] aEndOfSlot  Slot Scan 의 종료 여부
 * @param[out] aEndOfScan  Iterator Scan 의 종료 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbGetValidVersions( smnmpbIterator  * aIterator,
                                  smlTbsId          aTbsId,
                                  smpHandle       * aPageHandle,
                                  smlFetchInfo    * aFetchInfo,
                                  stlBool         * aEndOfSlot,
                                  stlBool         * aEndOfScan,
                                  smlEnv          * aEnv )
{
    stlChar             * sCurPtr;
    stlUInt16             sSlotHdrSize;
    knlPhysicalFilter   * sKeyFilter;
    stlUInt8              sColLenSize;
    stlInt64              sColLen = 0;
    knlValueBlockList   * sValueBlock;
    knlColumnInReadRow  * sCurInReadRow;
    stlInt32              sColOrder = 0;
    dtlDataValue        * sDataValue;
    stlChar             * sKey;
    stlBool               sMatchPhysicalFilter;
    smpRowItemIterator  * sSlotIterator;
    stlInt32              sBlockIdx = 0;
    stlInt16              sSlotSeq;
    stlInt64              sBlockSize = SML_BLOCK_SIZE( aFetchInfo->mRowBlock );
    stlBool               sIsMatch = STL_FALSE;
    smxlTransId           sMyTransId = SMA_GET_TRANS_ID( aIterator->mCommon.mStatement );
    smlScn                sMyViewScn = aIterator->mCommon.mViewScn;
    smlScn                sMyCommitScn = SMA_GET_COMMIT_SCN(aIterator->mCommon.mStatement);
    smlTcn                sMyViewTcn = aIterator->mCommon.mViewTcn;
    smlScn                sInsertCommitScn = SML_INFINITE_SCN;
    smxlTransId           sInsertTransId = SML_INVALID_TRANSID;
    smlScn                sDeleteCommitScn = SML_INFINITE_SCN;
    smxlTransId           sDeleteTransId = SML_INVALID_TRANSID;
    stlBool               sIsDeleted;
    stlInt16              sTxRelCase = -1;
    stlBool               sRowLevelVisibility = STL_FALSE;
    smlTcn                sInsertTcn;
    smlTcn                sDeleteTcn;
    stlBool               sHasValidVersion = STL_TRUE;
    stlInt32              sAggrRowCount = 0;
    smlScn                sCommitScn = SML_INFINITE_SCN;

    smlRid    sPrevRowId = SML_INVALID_RID;
    smlRid    sRowRid;
    smpHandle sTablePageHandle;
    stlBool   sTablePageLatchRelease = STL_TRUE;
    smnIndexHeader      * sIndexHeader = (smnIndexHeader *)(aIterator->mCommon.mRelationHandle);

    sSlotIterator = &aIterator->mCommon.mSlotIterator;
    
    if( aFetchInfo->mAggrFetchInfo != NULL )
    {
        sBlockIdx = 0;
    }
    else
    {
        sBlockIdx = SML_USED_BLOCK_SIZE( aFetchInfo->mRowBlock );
    }

    *aEndOfSlot = STL_TRUE;
    *aEndOfScan = STL_FALSE;
    
    SMP_FOREACH_ROWITEM_ITERATOR( sSlotIterator )
    {
        aIterator->mCurSlotSeq++;
        sSlotSeq = SMP_GET_CURRENT_ROWITEM_SEQ( sSlotIterator );
        
        if( sSlotSeq >= aIterator->mSlotSeqFence )
        {
            *aEndOfScan = STL_TRUE;
            break;
        }

        sTxRelCase = -1;
        sRowLevelVisibility = STL_FALSE;
        sHasValidVersion = STL_TRUE;
        sDeleteTransId = SML_INVALID_TRANSID;
        sDeleteCommitScn = SML_INFINITE_SCN;
        
        sKey = SMP_GET_CURRENT_ROWITEM( sSlotIterator );
        SMNMPB_GET_LEAF_KEY_HDR_SIZE( sKey, &sSlotHdrSize );
        sCurPtr = sKey + sSlotHdrSize;

        STL_TRY( smnmpbGetInsertCommitScn( aTbsId,
                                           aPageHandle,
                                           sKey,
                                           sMyViewScn,
                                           &sInsertCommitScn,
                                           &sInsertTcn,
                                           &sInsertTransId,
                                           aEnv ) == STL_SUCCESS );

        sIsDeleted = SMNMPB_IS_DELETED( sKey );
        if( sIsDeleted == STL_TRUE )
        {
            STL_TRY( smnmpbGetDeleteCommitScn( aTbsId,
                                               aPageHandle,
                                               sKey,
                                               sMyViewScn,
                                               &sDeleteCommitScn,
                                               &sDeleteTcn,
                                               &sDeleteTransId,
                                               aEnv ) == STL_SUCCESS );
        }

        if( (smnmpbIsSameTrans(sMyTransId, sMyCommitScn, sInsertTransId, sInsertCommitScn) == STL_TRUE) ||
            ( (sIsDeleted == STL_TRUE) &&
              (smnmpbIsSameTrans(sMyTransId, sMyCommitScn, sDeleteTransId, sDeleteCommitScn) == STL_TRUE) ) )
        {
            if( sMyViewTcn == SML_INFINITE_TCN )
            {
                if( sIsDeleted == STL_TRUE )
                {
                    sTxRelCase = 2;
                }
                else
                {
                    sTxRelCase = 1;
                }
            }
            else
            {
                if( sInsertCommitScn != SML_INFINITE_SCN )
                {
                    if( sInsertCommitScn > sMyViewScn )
                    {
                        if( SMNMPB_IS_DUPKEY( sKey ) == STL_TRUE )
                        {
                            sRowLevelVisibility = STL_TRUE;
                        }
                        else
                        {
                            /* invalid version */
                            sTxRelCase = 0;
                        }
                        
                        STL_THROW( RAMP_ROW_LEVEL_VISIBILITY );
                    }

                    if( sIsDeleted == STL_TRUE )
                    {
                        if( sDeleteTcn == SMNMPB_INVALID_TCN )
                        {
                            sRowLevelVisibility = STL_TRUE;
                            STL_THROW( RAMP_ROW_LEVEL_VISIBILITY );
                        }
                
                        if( sDeleteTcn < sMyViewTcn )
                        {
                            /* invalid version */
                            sTxRelCase = 2;
                        }
                        else
                        {
                            /* valid version */
                            sTxRelCase = 1;
                        }
                    }
                    else
                    {
                        /* valid version */
                        sTxRelCase = 1;
                    }
                    
                    STL_THROW( RAMP_ROW_LEVEL_VISIBILITY );
                }
            
                if( sInsertTcn == SMNMPB_INVALID_TCN )
                {
                    sRowLevelVisibility = STL_TRUE;
                    STL_THROW( RAMP_ROW_LEVEL_VISIBILITY );
                }
                else
                {
                    if( sIsDeleted == STL_TRUE )
                    {
                        if( sDeleteTcn == SMNMPB_INVALID_TCN )
                        {
                            sRowLevelVisibility = STL_TRUE;
                            STL_THROW( RAMP_ROW_LEVEL_VISIBILITY );
                        }
                    }
                }

                if( sInsertTcn >= sMyViewTcn )
                {
                    if( SMNMPB_IS_DUPKEY( sKey ) == STL_TRUE )
                    {
                        sRowLevelVisibility = STL_TRUE;
                        STL_THROW( RAMP_ROW_LEVEL_VISIBILITY );
                    }
                    else
                    {
                        /* invalid version */
                        sTxRelCase = 0;
                    }
                }
                else
                {
                    if( sIsDeleted == STL_TRUE)
                    {
                        if( sDeleteTcn < sMyViewTcn )
                        {
                            /* invalid version */
                            sTxRelCase = 2;
                        }
                        else
                        {
                            /* valid version */
                            sTxRelCase = 1;
                        }
                    }
                    else
                    {
                        /* valid version */
                        sTxRelCase = 1;
                    }
                }
            }
        }
        else
        {
            while( STL_TRUE )
            {
                if( sInsertCommitScn != SML_INFINITE_SCN )
                {
                    if( sInsertCommitScn > sMyViewScn )
                    {
                        if( SMNMPB_IS_DUPKEY( sKey ) == STL_TRUE )
                        {
                            sRowLevelVisibility = STL_TRUE;
                        }
                        else
                        {
                            /* invalid version */
                            sTxRelCase = 0;
                        }
                        break;
                    }
                    else
                    {
                        /* 내 tx가 아닌 tx 가 insert 했음 */
                        /* delete commit scn 체크해 봐야 함 */
                    }
                }
                else
                {
                    if( SMNMPB_IS_DUPKEY( sKey ) == STL_TRUE )
                    {
                        sRowLevelVisibility = STL_TRUE;
                    }
                    else
                    {
                        /* invalid version */
                        sTxRelCase = 0;
                    }
                    break;
                }

                STL_ASSERT( sTxRelCase == -1 );
                if( sIsDeleted != STL_TRUE )
                {
                    sTxRelCase = 1;
                    break;
                }

                if( sDeleteCommitScn != SML_INFINITE_SCN )
                {
                    if( sDeleteCommitScn <= sMyViewScn )
                    {
                        sTxRelCase = 2;
                    }
                    else
                    {
                        sTxRelCase = 1;
                    }
                }
                else
                {
                    sTxRelCase = 1;
                }

                STL_ASSERT( sTxRelCase >= 0 );
                break;
            } /* while */
        }

        STL_RAMP( RAMP_ROW_LEVEL_VISIBILITY );
    
        if( sRowLevelVisibility == STL_TRUE )
        {
            sRowRid.mTbsId = aIterator->mBaseRelTbsId;
            SMNMPB_GET_ROW_PID( sKey, &sRowRid.mPageId );
            SMNMPB_GET_ROW_SEQ( sKey, &sRowRid.mOffset );

            if ( (sPrevRowId.mTbsId  == sRowRid.mTbsId) &&
                 (sPrevRowId.mPageId == sRowRid.mPageId) &&
                 (sTablePageLatchRelease == STL_FALSE) )
            {
                /**
                 * 기존 Page 를 그대로 사용한다
                 */
            }
            else
            {
                if( sTablePageLatchRelease == STL_FALSE )
                {
                    sTablePageLatchRelease = STL_TRUE;
                    STL_TRY( smpRelease( &sTablePageHandle, aEnv ) == STL_SUCCESS );
                }
                
                STL_TRY( smpAcquire( NULL,
                                     sRowRid.mTbsId,
                                     sRowRid.mPageId,
                                     KNL_LATCH_MODE_SHARED,
                                     &sTablePageHandle,
                                     aEnv ) == STL_SUCCESS );

                sPrevRowId.mTbsId  = sRowRid.mTbsId;
                sPrevRowId.mPageId = sRowRid.mPageId;
                sTablePageLatchRelease = STL_FALSE;
            }

            STL_TRY( smnmpbCheckRowColWithKey( aIterator,
                                               aFetchInfo,
                                               &sTablePageHandle,
                                               &sTablePageLatchRelease,
                                               sMyTransId,
                                               sMyViewScn,
                                               sMyCommitScn,
                                               sMyViewTcn,
                                               sKey,
                                               &sIsMatch,
                                               &sInsertCommitScn,
                                               aEnv ) == STL_SUCCESS );
            
            if( sIsMatch == STL_TRUE )
            {
                sTxRelCase = 1;
            }
            else
            {
                sTxRelCase = 0;
            }
        }

        STL_ASSERT( sTxRelCase >= 0 );

        switch( sTxRelCase )
        {
            case 0:
            case 2:
                sHasValidVersion = STL_FALSE;
                break;
            case 1:
                sHasValidVersion = STL_TRUE;
                
                /**
                 * DDL statement는 현재 commit scn보다 작은 view scn을 가질수도 있다.
                 * - SCN_VALUE는 version conflict를 확인하기 위해서 사용되므로 MAX값을 반환해야 한다. 
                 */
                sCommitScn = SMI_GET_VIEW_SCN( aIterator );
                
                if( sInsertCommitScn != SML_INFINITE_SCN )
                {
                    sCommitScn = STL_MAX( sCommitScn, sInsertCommitScn );
                }
                break;
            default:
                STL_DASSERT( 0 );
                break;
        }

        if( sHasValidVersion == STL_FALSE )
        {
            continue;
        }
        
        aIterator->mRowRid.mTbsId = aIterator->mBaseRelTbsId;
        SMNMPB_GET_ROW_PID( sKey, &aIterator->mRowRid.mPageId );
        SMNMPB_GET_ROW_SEQ( sKey, &aIterator->mRowRid.mOffset );

        sColOrder = 0;

        /* set valueblock */
        sValueBlock = aFetchInfo->mColList;
        
        while( sValueBlock != NULL )
        {
            SMP_GET_COLUMN_LEN_ZERO_INSENS( sCurPtr, & sColLenSize, & sColLen );
            sCurPtr += sColLenSize;

            if( sColOrder == KNL_GET_BLOCK_COLUMN_ORDER( sValueBlock ) )
            {
                sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( sValueBlock );
                sCurInReadRow->mValue  = sCurPtr;
                sCurInReadRow->mLength = sColLen;

                sValueBlock = sValueBlock->mNext;
            }

            sCurPtr += sColLen;
            sColOrder++;
        }
        
        /* key-filter 평가 */
        sKeyFilter  = aFetchInfo->mPhysicalFilter;
        sMatchPhysicalFilter = STL_TRUE;
            
        while( sKeyFilter != NULL )
        {
            if( sKeyFilter->mFunc( sKeyFilter->mColVal1->mValue,
                                   sKeyFilter->mColVal1->mLength,
                                   sKeyFilter->mColVal2->mValue,
                                   sKeyFilter->mColVal2->mLength )
                == STL_FALSE )
            {
                sMatchPhysicalFilter = STL_FALSE;
                break;
            }

            sKeyFilter = sKeyFilter->mNext;
        }

        if( sMatchPhysicalFilter == STL_FALSE )
        {
            continue;
        }
            
        /* fetch key values */
        sValueBlock = aFetchInfo->mColList;
        
        while( sValueBlock != NULL )
        {
            STL_DASSERT( KNL_GET_BLOCK_USED_CNT( sValueBlock ) <= sBlockIdx );
                
            sCurInReadRow = KNL_GET_BLOCK_COLUMN_IN_ROW( sValueBlock );

            sDataValue = KNL_GET_BLOCK_SEP_DATA_VALUE( sValueBlock, sBlockIdx );
            sDataValue->mLength = sCurInReadRow->mLength;
            
            DTL_COPY_VALUE( sDataValue,
                            sCurInReadRow->mValue,
                            sCurInReadRow->mLength );

            sDataValue->mLength = sCurInReadRow->mLength;
            sValueBlock = sValueBlock->mNext;
        }

        /**
         * RowId Column을 참조하는 경우, RowId Column Block에 값을 만든다. 
         */
        if( aFetchInfo->mRowIdColList != NULL )
        {
            STL_TRY( dtlRowIdSetValueFromRowIdInfo(
                         aFetchInfo->mTableLogicalId,
                         aIterator->mRowRid.mTbsId,
                         aIterator->mRowRid.mPageId,
                         aIterator->mRowRid.mOffset,
                         KNL_GET_BLOCK_SEP_DATA_VALUE( aFetchInfo->mRowIdColList,
                                                       sBlockIdx ),
                         KNL_ERROR_STACK(aEnv) )
                     == STL_SUCCESS );            
        }
        
        /* logical key filter 평가 */
        if( aFetchInfo->mFilterEvalInfo != NULL )
        {
            aFetchInfo->mFilterEvalInfo->mBlockIdx = sBlockIdx;

            STL_TRY( knlEvaluateOneExpression( aFetchInfo->mFilterEvalInfo,
                                               KNL_ENV(aEnv) )
                     == STL_SUCCESS );
                        
            STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF(
                             aFetchInfo->mFilterEvalInfo->mResultBlock )
                         == STL_TRUE );
            
            if( !DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_SEP_DATA_VALUE(
                                          aFetchInfo->mFilterEvalInfo->mResultBlock,
                                          sBlockIdx ) ) )
            {
                continue;
            }
        }

        if( aFetchInfo->mFetchRecordInfo != NULL )
        {
            if ( (sPrevRowId.mTbsId  == aIterator->mRowRid.mTbsId) &&
                 (sPrevRowId.mPageId == aIterator->mRowRid.mPageId) &&
                 (sTablePageLatchRelease == STL_FALSE) )
            {
                /**
                 * 기존 Page 를 그대로 사용한다
                 */
            }
            else
            {
                if( sTablePageLatchRelease == STL_FALSE )
                {
                    sTablePageLatchRelease = STL_TRUE;
                    STL_TRY( smpRelease( &sTablePageHandle, aEnv ) == STL_SUCCESS );
                }
                
                STL_TRY( smpAcquire( NULL,
                                     aIterator->mRowRid.mTbsId,
                                     aIterator->mRowRid.mPageId,
                                     KNL_LATCH_MODE_SHARED,
                                     &sTablePageHandle,
                                     aEnv ) == STL_SUCCESS );

                sPrevRowId.mTbsId  = aIterator->mRowRid.mTbsId;
                sPrevRowId.mPageId = aIterator->mRowRid.mPageId;
                sTablePageLatchRelease = STL_FALSE;
            }

            STL_TRY( smdFetchRecord4Index( (smiIterator*)aIterator,
                                           sIndexHeader->mBaseRelHandle,
                                           aFetchInfo->mFetchRecordInfo,
                                           & sTablePageHandle,
                                           & sTablePageLatchRelease,
                                           & aIterator->mRowRid,
                                           sBlockIdx,
                                           aIterator->mAgableViewScn,
                                           & sMatchPhysicalFilter,
                                           aEnv ) == STL_SUCCESS );

            if( sMatchPhysicalFilter == STL_FALSE )
            {
                continue;
            }
        }
        
        if( aFetchInfo->mAggrFetchInfo != NULL )
        {
            sBlockIdx++;
            
            if( sBlockIdx == sBlockSize )
            {
                STL_TRY( smoBlockBuildAggrFunc( aFetchInfo->mAggrFetchInfo,
                                                0, /* aStartBlockIdx */
                                                sBlockSize,
                                                aEnv )
                         == STL_SUCCESS );
                sBlockIdx = 0;
                sAggrRowCount = 1;
            }
        }
        else
        {
            if( aFetchInfo->mSkipCnt > 0 )
            {
                aFetchInfo->mSkipCnt--;
                continue;
            }
            else
            {
#ifdef STL_DEBUG
                /**
                 * DROP USER u2 와 DROP TABLE u1.t1 가 동시에 수행될 때,
                 * (u1->u2 SELECT ON u1.t1) TABLE_PRIVILEGES row 의 referential constraint 위배하는 현상을
                 * 분석하기 위해 추가한 System Fatal 임
                 */
                if ( aFetchInfo->mRowBlock->mRidBlock == NULL )
                {
                    knlSystemFatal( "smnmpbGetValidVersions()", KNL_ENV(aEnv) );
                }
#endif
                
                SML_SET_RID_VALUE( aFetchInfo->mRowBlock, sBlockIdx, aIterator->mRowRid );
                SML_SET_SCN_VALUE( aFetchInfo->mRowBlock, sBlockIdx, sCommitScn );

                aFetchInfo->mFetchCnt--;
                sBlockIdx++;
            }
 
        
            if( (aFetchInfo->mFetchCnt == 0) ||
                (sBlockIdx == sBlockSize) )
            {
                *aEndOfSlot = STL_FALSE;
                *aEndOfScan = STL_FALSE;
                break;
            }
        }
    }

    if( sTablePageLatchRelease == STL_FALSE )
    {
        sTablePageLatchRelease = STL_TRUE;
        STL_TRY( smpRelease( &sTablePageHandle, aEnv ) == STL_SUCCESS );
    }
    
    if( aFetchInfo->mAggrFetchInfo == NULL )
    {
        if( aFetchInfo->mColList != NULL )
        {
            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aFetchInfo->mColList, 0, sBlockIdx );
        }

        if( aFetchInfo->mRowIdColList != NULL )
        {
            KNL_SET_ONE_BLOCK_SKIP_CNT( aFetchInfo->mRowIdColList, 0 );
            KNL_SET_ONE_BLOCK_USED_CNT( aFetchInfo->mRowIdColList, sBlockIdx );
        }
    
        if( aFetchInfo->mFetchRecordInfo != NULL )
        {
            if( aFetchInfo->mFetchRecordInfo->mColList != NULL )
            {
                KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
                    aFetchInfo->mFetchRecordInfo->mColList,
                    0,
                    sBlockIdx );
            }

            STL_DASSERT( aFetchInfo->mFetchRecordInfo->mRowIdColList == NULL );
        }
    
        SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sBlockIdx );
    }
    else
    {
        if( sBlockIdx > 0 )
        {
            STL_TRY( smoBlockBuildAggrFunc( aFetchInfo->mAggrFetchInfo,
                                            0, /* aStartBlockIdx */
                                            sBlockIdx,
                                            aEnv )
                     == STL_SUCCESS );
            sAggrRowCount = 1;
        }
        
        SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sAggrRowCount );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sTablePageLatchRelease == STL_FALSE )
    {
        sTablePageLatchRelease = STL_TRUE;
        STL_TRY( smpRelease( &sTablePageHandle, aEnv ) == STL_SUCCESS );
    }
    
    return STL_FAILURE;
}


/**
 * @brief 현재 page에서 Max key range에 부합하지 않는 key의 첫 위치를 찾는다
 * @param[in,out] aIterator 현재 페이지에 대한 정보를 담은 iterator
 * @param[in,out] aFetchInfo Fetch 정보
 * @param[in] aPageFrame 현재 leaf page의 frame 포인터
 * @param[in] aStartSeq 첫 key의 sequence
 * @param[in] aEnv Storage Manager Environment
 */
inline stlStatus smnmpbFindMaxFence( smnmpbIterator    * aIterator,
                                     smlFetchInfo      * aFetchInfo,
                                     void              * aPageFrame,         
                                     stlInt16            aStartSeq,
                                     smlEnv            * aEnv )
{
    stlChar   * sCurKey;
    smpHandle   sPageHandle;
    stlInt16    sKeyCount;
    stlBool     sResult;

    SMP_MAKE_DUMMY_HANDLE( &sPageHandle, aPageFrame );

    sKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle );

    if ( sKeyCount == 0 )
    {
        aIterator->mSlotSeqFence = 0;
    }
    else if( sKeyCount == aStartSeq )
    {
        aIterator->mSlotSeqFence = sKeyCount;
    }
    else
    {
        STL_ASSERT( aStartSeq < sKeyCount );

        while( STL_TRUE )
        {
            sCurKey = smpGetNthRowItem( &sPageHandle, sKeyCount - 1 );

            STL_TRY( smnmpbCheckMaxRange( sCurKey,
                                          aFetchInfo->mRange->mMaxRange,
                                          STL_TRUE,  /* aIsLeaf */
                                          &sResult,
                                          aEnv ) == STL_SUCCESS );

            if( sResult == STL_TRUE )
            {
                /**
                 * 마지막 key가 max range를 통과한다
                 * -> 현재 page의 모든 key가 max range를 통과한다
                 */
                aIterator->mSlotSeqFence = sKeyCount;
                break;
            }

            if( aStartSeq == sKeyCount - 1 )
            {
                /**
                 * 시작 key가 마지막 키이며 max range를 통과하지 못했다
                 * -> min과 max range를 모두 만족시키는 key는 없다
                 */
                aIterator->mSlotSeqFence = aStartSeq;
                break;
            }

            if( aStartSeq + 1 == sKeyCount - 1 )
            {
                /* 시작 key의 바로 다음 key가 마지막 키인데
                 * max range를 만족시키지 못한다
                 */
                sCurKey = smpGetNthRowItem( &sPageHandle, aStartSeq );

                STL_TRY( smnmpbCheckMaxRange( sCurKey,
                                              aFetchInfo->mRange->mMaxRange,
                                              STL_TRUE,  /* aIsLeaf */
                                              &sResult,
                                              aEnv ) == STL_SUCCESS );

                if( sResult == STL_TRUE )
                {
                    /* start key는 max range를 만족시킨다 */
                    aIterator->mSlotSeqFence = aStartSeq + 1;
                }
                else
                {
                    /* start key도 max range를 만족시키지 못한다 */
                    aIterator->mSlotSeqFence = aStartSeq;
                }
                break;
            }

            /**
             * aStartSeq 뒤로 key가 2개 이상 있다
             * 단일 fetch 에 대한 튜닝을 한다
             */

            sCurKey = smpGetNthRowItem( &sPageHandle, aStartSeq + 1 );

            STL_TRY( smnmpbCheckMaxRange( sCurKey,
                                          aFetchInfo->mRange->mMaxRange,
                                          STL_TRUE,  /* aIsLeaf */
                                          &sResult,
                                          aEnv ) == STL_SUCCESS );

            if( sResult != STL_TRUE )
            {
                /**
                 * 시작 key 바로 다음 key가 max range를 만족시키지 못한다
                 * -> 시작 key를 검사한다
                 */
                sCurKey = smpGetNthRowItem( &sPageHandle, aStartSeq );

                STL_TRY( smnmpbCheckMaxRange( sCurKey,
                                              aFetchInfo->mRange->mMaxRange,
                                              STL_TRUE,  /* aIsLeaf */
                                              &sResult,
                                              aEnv ) == STL_SUCCESS );

                if( sResult == STL_TRUE )
                {
                    /* start key는 max range를 만족시킨다 */
                    aIterator->mSlotSeqFence = aStartSeq + 1;
                }
                else
                {
                    /* start key도 max range를 만족시키지 못한다 */
                    aIterator->mSlotSeqFence = aStartSeq;
                }
                break;
            }


            /**
             * aStartSeq 뒤로 key가 2개 이상 있으며
             * 단일 fetch 도 아니다
             */
            STL_TRY( smnmpbAfterLastLeafKey( &sPageHandle,
                                             aIterator,
                                             aFetchInfo,
                                             aStartSeq + 2,
                                             &aIterator->mSlotSeqFence,
                                             aEnv ) == STL_SUCCESS );
            break;
        } /* while */
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    

/**
 * @brief 일반 Scan을 위해 열린 Iterator에서 다음 row를 반환한다
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 */
stlStatus smnmpbFetchNext( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv )
{
    smnmpbIterator  * sIterator = (smnmpbIterator*)aIterator;
    smlPid            sPid = sIterator->mPageBufPid;
    smpHandle         sPageHandle = {NULL, (void*)sIterator->mPageBuf};
    stlInt32          sState = 0;
    stlBool           sEndOfScan;
    stlBool           sEndOfSlot;
    void            * sRelHandle = sIterator->mCommon.mRelationHandle;
    smlTbsId          sTbsId = smsGetTbsId(SME_GET_SEGMENT_HANDLE(sRelHandle));

    STL_PARAM_VALIDATE( aFetchInfo->mRowBlock != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFetchInfo->mAggrFetchInfo == NULL, KNL_ERROR_STACK(aEnv) );
    
    aFetchInfo->mIsEndOfScan = STL_TRUE;
    SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );

    if( sPid == SMP_NULL_PID )
    {
        STL_TRY( smnmpbFindFirst( sIterator,
                                  aFetchInfo,
                                  &sPageHandle,
                                  aEnv ) == STL_SUCCESS );
        sState = 1;
            
        sPid = sIterator->mPageBufPid;
        STL_ASSERT( sPid != SMP_NULL_PID );

        smpBeforeNthRowItem( SMP_FRAME(&sPageHandle),
                             &sIterator->mCommon.mSlotIterator,
                             sIterator->mCurSlotSeq + 1 );

        STL_TRY( smnmpbFindMaxFence( sIterator,
                                     aFetchInfo,
                                     SMP_FRAME(&sPageHandle),
                                     sIterator->mCurSlotSeq + 1,
                                     aEnv ) == STL_SUCCESS );
    }

    while( sPid != SMP_NULL_PID )
    {
        if( SMXL_IS_AGABLE_VIEW_SCN(sIterator->mAgableViewScn, smpGetMaxViewScn(&sPageHandle)) )
        {
            STL_TRY( smnmpbGetValidVersionsStablePage( sIterator,
                                                       SMP_FRAME(&sPageHandle),
                                                       aFetchInfo,
                                                       &sEndOfSlot,
                                                       &sEndOfScan,
                                                       aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smnmpbGetValidVersions( sIterator,
                                             sTbsId,
                                             &sPageHandle,
                                             aFetchInfo,
                                             &sEndOfSlot,
                                             &sEndOfScan,
                                             aEnv ) == STL_SUCCESS );
        }

        if( ( aFetchInfo->mFetchCnt == 0 ) ||
            ( SML_USED_BLOCK_SIZE( aFetchInfo->mRowBlock ) == SML_BLOCK_SIZE( aFetchInfo->mRowBlock ) ) )
        {
            aFetchInfo->mIsEndOfScan = STL_FALSE;
            break;
        }
            
        if( sEndOfScan == STL_TRUE )
        {
            aFetchInfo->mIsEndOfScan = STL_TRUE;
            break;
        }

        /* 현재 페이지에서 적당한 row를 찾지 못했기 때문에 다음 페이지로 이동함 */
        STL_RAMP( RAMP_FETCH_NEXT_PAGE );
            
        if( sEndOfSlot == STL_TRUE )
        {
            sPid = SMNMPB_GET_NEXT_PAGE( &sPageHandle );

            if( sState == 1 )
            {
                sState = 0;
                
                if( SMP_IS_DUMMY_HANDLE( &sPageHandle ) == STL_FALSE )
                {
                    STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
                }
            }
            
            if( sPid != SMP_NULL_PID )
            {
                /* 다음 페이지가 존재하므로 caching함 */

                /**
                 * Event 검사
                 */
                STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );

                STL_TRY( smnmpbSnapshot( sTbsId,
                                         sPid,
                                         &sPageHandle,
                                         (stlChar*)sIterator->mPageBuf,
                                         aEnv )
                         == STL_SUCCESS );

                if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
                {
                    /* Leaf Scan 도중 만난 free leaf page는 skip한다 */
                    STL_THROW( RAMP_FETCH_NEXT_PAGE );
                }

                sIterator->mPageBufPid = sPid;
                sIterator->mCurSlotSeq = -1;
                
                STL_TRY( smnmpbFindMaxFence( sIterator,
                                             aFetchInfo,
                                             SMP_FRAME(&sPageHandle),
                                             sIterator->mCurSlotSeq + 1,
                                             aEnv ) == STL_SUCCESS );

                SMP_INIT_ROWITEM_ITERATOR( SMP_FRAME(&sPageHandle),
                                           &sIterator->mCommon.mSlotIterator );
            }
            else
            {
                /* 다음 페이지가 없으므로 EOF를 리턴 */
                sIterator->mPageBufPid = sPid;
                sIterator->mRowRid.mTbsId = 0;
                sIterator->mRowRid.mPageId = SMP_NULL_PID;
                sIterator->mRowRid.mOffset = -1;
                aFetchInfo->mIsEndOfScan = STL_TRUE;
                break;
            }
        }

    } /* while( sPid != SMP_NULL_PID ) */

    if( sState == 1 )
    {
        if( SMP_IS_DUMMY_HANDLE( &sPageHandle ) == STL_FALSE )
        {
            if( aFetchInfo->mIsEndOfScan != STL_TRUE )
            {
                stlMemcpy( sIterator->mPageBuf, SMP_FRAME(&sPageHandle), SMP_PAGE_SIZE );
                smpBeforeNthRowItem( (void*)sIterator->mPageBuf,
                                     &sIterator->mCommon.mSlotIterator,
                                     sIterator->mCurSlotSeq + 1 );
            }

            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        if( SMP_IS_DUMMY_HANDLE( &sPageHandle ) == STL_FALSE )
        {
            (void) smpRelease( &sPageHandle, aEnv );
        }
    }

    return STL_FAILURE;
}


/**
 * @brief Aggregation을 위한 Scan을 한다.
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 */
stlStatus smnmpbFetchAggr( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv )
{
    smnmpbIterator  * sIterator = (smnmpbIterator*)aIterator;
    smlPid            sPid = SMP_NULL_PID;
    smpHandle         sPageHandle;
    stlInt32          sState = 0;
    stlBool           sEndOfScan;
    stlBool           sEndOfSlot;
    void            * sRelHandle = sIterator->mCommon.mRelationHandle;
    smlTbsId          sTbsId = smsGetTbsId(SME_GET_SEGMENT_HANDLE(sRelHandle));

    STL_PARAM_VALIDATE( aFetchInfo->mRowBlock != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aFetchInfo->mAggrFetchInfo != NULL, KNL_ERROR_STACK(aEnv) );
    
    SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );

    if( sPid == SMP_NULL_PID )
    {
        STL_TRY( smnmpbFindFirst( sIterator,
                                  aFetchInfo,
                                  &sPageHandle,
                                  aEnv ) == STL_SUCCESS );
        sState = 1;
            
        sPid = sIterator->mPageBufPid;
        STL_ASSERT( sPid != SMP_NULL_PID );

        smpBeforeNthRowItem( SMP_FRAME(&sPageHandle),
                             &sIterator->mCommon.mSlotIterator,
                             sIterator->mCurSlotSeq + 1 );

        STL_TRY( smnmpbFindMaxFence( sIterator,
                                     aFetchInfo,
                                     SMP_FRAME(&sPageHandle),
                                     sIterator->mCurSlotSeq + 1,
                                     aEnv ) == STL_SUCCESS );
    }

    /**
     * traverse all of index pages
     */
    
    while( sPid != SMP_NULL_PID )
    {
        if( SMXL_IS_AGABLE_VIEW_SCN(sIterator->mAgableViewScn, smpGetMaxViewScn(&sPageHandle)) )
        {
            STL_TRY( smnmpbGetValidVersionsStablePage( sIterator,
                                                       SMP_FRAME(&sPageHandle),
                                                       aFetchInfo,
                                                       &sEndOfSlot,
                                                       &sEndOfScan,
                                                       aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smnmpbGetValidVersions( sIterator,
                                             sTbsId,
                                             &sPageHandle,
                                             aFetchInfo,
                                             &sEndOfSlot,
                                             &sEndOfScan,
                                             aEnv ) == STL_SUCCESS );
        }

        if( sEndOfScan == STL_TRUE )
        {
            break;
        }

        STL_RAMP( RAMP_FETCH_NEXT_PAGE );

        sPid = SMNMPB_GET_NEXT_PAGE( &sPageHandle );

        if( SMP_IS_DUMMY_HANDLE( &sPageHandle ) == STL_FALSE )
        {
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
        }

        /**
         * fetch next page
         */
        
        if( sPid != SMP_NULL_PID )
        {
            /**
             * Event 검사
             */
            STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );

            STL_TRY( smpAcquire( NULL,
                                 sTbsId,
                                 sPid,
                                 KNL_LATCH_MODE_SHARED,
                                 &sPageHandle,
                                 aEnv )
                     == STL_SUCCESS );
            sState = 1;
            
            if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
            {
                /* Leaf Scan 도중 만난 free leaf page는 skip한다 */
                STL_THROW( RAMP_FETCH_NEXT_PAGE );
            }

            SMP_INIT_ROWITEM_ITERATOR( SMP_FRAME(&sPageHandle),
                                       &(sIterator->mCommon.mSlotIterator) );

            sIterator->mPageBufPid = sPid;
            sIterator->mCurSlotSeq = -1;
                
            STL_TRY( smnmpbFindMaxFence( sIterator,
                                         aFetchInfo,
                                         SMP_FRAME(&sPageHandle),
                                         sIterator->mCurSlotSeq + 1,
                                         aEnv ) == STL_SUCCESS );
        }
        else
        {
            /* 다음 페이지가 없으므로 EOF를 리턴 */
            sIterator->mPageBufPid = sPid;
            sIterator->mRowRid.mTbsId = 0;
            sIterator->mRowRid.mPageId = SMP_NULL_PID;
            sIterator->mRowRid.mOffset = -1;
            break;
        }
    } /* while( sPid != SMP_NULL_PID ) */

    aFetchInfo->mIsEndOfScan = STL_TRUE;
    
    if( sState == 1 )
    {
        if( SMP_IS_DUMMY_HANDLE( &sPageHandle ) == STL_FALSE )
        {
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        if( SMP_IS_DUMMY_HANDLE( &sPageHandle ) == STL_FALSE )
        {
            (void) smpRelease( &sPageHandle, aEnv );
        }
    }

    return STL_FAILURE;
}


/**
 * @brief 주어진 페이지내의 특정 키를 min Key Range에 적용한다
 * @param[in] aKeySlot 페이지내의 특정 key의 포인터
 * @param[in] aCompList 적용할 condition list
 * @param[in] aIsLeaf leaf node의 key인지 여부
 * @param[out] aResult 비교 결과
 * @param[in,out] aEnv Storage Manager Environment
 */
inline stlStatus smnmpbCheckMinRange( char              * aKeySlot,
                                      knlCompareList    * aCompList,
                                      stlBool             aIsLeaf,
                                      stlBool           * aResult,
                                      smlEnv            * aEnv )
{
    knlCompareList   * sCompList = aCompList;
    dtlCompareResult   sResult;
    stlChar          * sColPtr   = aKeySlot;
    stlUInt16          sSlotHdrSize;
    stlInt32           sColOrder = -1;
    stlUInt8           sColLenSize = 0;
    stlInt64           sColLen = 0;
    stlBool            sIsInclude = STL_TRUE;

    SMNMPB_GET_LEAF_KEY_HDR_SIZE(aKeySlot, &sSlotHdrSize);
    sColPtr += ( (aIsLeaf == STL_TRUE) ?  sSlotHdrSize : SMNMPB_INTL_SLOT_HDR_SIZE );

    while( sCompList != NULL )
    {
        while( sColOrder < sCompList->mColOrder )
        {
            sColPtr += (sColLenSize + sColLen);
            SMP_GET_COLUMN_LEN_ZERO_INSENS( sColPtr, &sColLenSize, &sColLen );
            sColOrder++;
        }
            
        if( sColLen == 0 )
        {
            if( ( sCompList->mRangeLen == 0 ) &&
                ( sCompList->mIsIncludeEqual ) )
            {
                sResult = DTL_COMPARISON_EQUAL;
            }
            else
            {
                sResult = ( !sCompList->mIsNullFirst ) - ( sCompList->mIsNullFirst );
            }
        }
        else
        {
            if( sCompList->mRangeLen == 0 )  /* range 가 NULL */
            {
                /* IS NULL or IS NOT NULL */
                sResult = ( sCompList->mIsIncludeEqual )
                    ? ( sCompList->mIsNullFirst ) - ( !sCompList->mIsNullFirst )
                    : DTL_COMPARISON_EQUAL;
            }
            else 
            {
                sResult = sCompList->mCompFunc( sColPtr + sColLenSize,
                                                sColLen,
                                                sCompList->mRangeVal,
                                                sCompList->mRangeLen );

                if( sCompList->mIsAsc )
                {
                    /* Do Nothing */
                }
                else
                {
                    sResult = -sResult;
                }
                
                if( (!sCompList->mIsIncludeEqual) && (sResult == DTL_COMPARISON_EQUAL) )
                {
                    sIsInclude = STL_FALSE;
                    break;
                }
                else
                {
                    /*
                     * compare next range
                     */
                }
            }
        }
                
        if( sResult == DTL_COMPARISON_LESS )
        {
            sIsInclude = STL_FALSE;
            break;
        }
        else
        {
            if( sResult == DTL_COMPARISON_GREATER )
            {
                sIsInclude = STL_TRUE;
                
                if( sCompList->mNext != NULL )
                {
                    if( sCompList->mColOrder != sCompList->mNext->mColOrder )
                    {
                        break;
                    }
                }
            }
            else
            {
                sIsInclude = STL_TRUE;
            }
        }
            
        sCompList = sCompList->mNext;
    } /* while( sCompList != NULL ) */

    *aResult = sIsInclude;
        
    return STL_SUCCESS;
}

/**
 * @brief 주어진 페이지내의 특정 키를 Max Key Range에 적용한다
 * @param[in] aKeySlot 페이지내의 특정 key의 포인터
 * @param[in] aCompList 적용할 condition list
 * @param[in] aIsLeaf leaf node의 key인지 여부
 * @param[out] aResult 비교 결과
 * @param[in,out] aEnv Storage Manager Environment
 */
inline stlStatus smnmpbCheckMaxRange( char              * aKeySlot,
                                      knlCompareList    * aCompList,
                                      stlBool             aIsLeaf,
                                      stlBool           * aResult,
                                      smlEnv            * aEnv )
{
    knlCompareList   * sCompList = aCompList;
    dtlCompareResult   sResult;
    stlChar          * sColPtr   = aKeySlot;
    stlUInt16          sSlotHdrSize;
    stlInt32           sColOrder = -1;
    stlUInt8           sColLenSize = 0;
    stlInt64           sColLen = 0;
    stlBool            sIsInclude = STL_TRUE;

    SMNMPB_GET_LEAF_KEY_HDR_SIZE(aKeySlot, &sSlotHdrSize);
    sColPtr += ( (aIsLeaf == STL_TRUE) ?  sSlotHdrSize : SMNMPB_INTL_SLOT_HDR_SIZE );

    while( sCompList != NULL )
    {
        while( sColOrder < sCompList->mColOrder )
        {
            sColPtr += (sColLenSize + sColLen);
            SMP_GET_COLUMN_LEN_ZERO_INSENS( sColPtr, &sColLenSize, &sColLen );
            sColOrder++;
        }

        if( sColLen == 0 )
        {
            if( ( sCompList->mRangeLen == 0 ) &&
                ( sCompList->mIsIncludeEqual ) )
            {
                sResult = DTL_COMPARISON_EQUAL;
            }
            else
            {
                sResult = ( !sCompList->mIsNullFirst ) - ( sCompList->mIsNullFirst );
            }
        }
        else
        {
            if( sCompList->mRangeLen == 0 )  /* range 가 NULL */
            {
                /* IS NULL or IS NOT NULL */
                sResult = ( sCompList->mIsIncludeEqual )
                    ? ( sCompList->mIsNullFirst ) - ( !sCompList->mIsNullFirst )
                    : DTL_COMPARISON_EQUAL;
            }
            else 
            {
                sResult = sCompList->mCompFunc( sColPtr + sColLenSize,
                                                sColLen,
                                                sCompList->mRangeVal,
                                                sCompList->mRangeLen );

                if( sCompList->mIsAsc )
                {
                    /* Do Nothing */
                }
                else
                {
                    sResult = -sResult;
                }

                if( (!sCompList->mIsIncludeEqual) && (sResult == DTL_COMPARISON_EQUAL) )
                {
                    sIsInclude = STL_FALSE;
                    break;
                }
                else
                {
                    /*
                     * compare next range
                     */
                }
            }
        }

        if( sResult == DTL_COMPARISON_GREATER )
        {
            sIsInclude = STL_FALSE;
            break;
        }
        else
        {
            if( sResult == DTL_COMPARISON_LESS )
            {
                sIsInclude = STL_TRUE;

                if( sCompList->mNext != NULL )
                {
                    if( sCompList->mColOrder != sCompList->mNext->mColOrder )
                    {
                        break;
                    }
                }
            }
            else
            {
                sIsInclude = STL_TRUE;
            }
        }

        sCompList = sCompList->mNext;
    } /* while( sCompList != NULL ) */

    *aResult = sIsInclude;

    return STL_SUCCESS;
}


/**
 * @brief 현재 page에서 Min key range에 부합하지 않는 key의 마지막 위치를 찾는다
 * @param[in,out] aIterator    현재 페이지에 대한 정보를 담은 iterator
 * @param[in,out] aFetchInfo   Fetch 정보
 * @param[in]     aPageFrame   현재 leaf page의 frame 포인터
 * @param[in]     aEndSeq      마지막 key의 sequence
 * @param[in]     aEnv         Storage Manager Environment
 */
inline stlStatus smnmpbFindMinFence( smnmpbIterator    * aIterator,
                                     smlFetchInfo      * aFetchInfo,
                                     void              * aPageFrame,
                                     stlInt16            aEndSeq,
                                     smlEnv            * aEnv )
{
    stlChar   * sCurKey;
    smpHandle   sPageHandle;
    stlInt16    sKeyCount;
    stlBool     sResult;

    SMP_MAKE_DUMMY_HANDLE( &sPageHandle, aPageFrame );

    sKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle );

    if ( sKeyCount == 0 )
    {
        aIterator->mSlotSeqFence = -1;
    }
    else if( aEndSeq == -1 )
    {
        aIterator->mSlotSeqFence = -1;
    }
    else
    {
        STL_ASSERT( (aEndSeq < sKeyCount) &&
                    (aEndSeq >= 0) );
        sCurKey = smpGetNthRowItem( &sPageHandle, 0 );

        STL_TRY( smnmpbCheckMinRange( sCurKey,
                                      aFetchInfo->mRange->mMinRange,
                                      STL_TRUE,  /* aIsLeaf */
                                      &sResult,
                                      aEnv ) == STL_SUCCESS );

        if( sResult == STL_TRUE )
        {
            aIterator->mSlotSeqFence = -1;
        }
        else
        {
            STL_TRY( smnmpbBeforeFirstLeafKey( &sPageHandle,
                                               aIterator,
                                               aFetchInfo,
                                               aEndSeq,
                                               &aIterator->mSlotSeqFence,
                                               aEnv ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
    

/**
 * @brief 특정 Leaf node에서 max key range에 맞는 마지막 Key의 바로 다음 위치를 찾는다
 * @param[in] aPageHandle 조사할 internal page의 handle
 * @param[in,out] aIterator scan 조건과 현재 위치를 저장한 iterator
 * @param[in,out] aFetchInfo Fetch 정보
 * @param[in] aStartSeq Binary Search의 시작 Slot Sequence
 * @param[out] aSlotSeq Iterator가 위치하여야 할 sequence
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbAfterLastLeafKey( smpHandle         * aPageHandle,
                                  smnmpbIterator    * aIterator,
                                  smlFetchInfo      * aFetchInfo,
                                  stlInt16            aStartSeq,
                                  stlInt16          * aSlotSeq,
                                  smlEnv            * aEnv )
{
    stlUInt16              sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(aPageHandle);
    stlInt16               sLeft = aStartSeq;
    stlInt16               sRight = sTotalKeyCount - 1;
    stlInt16               sMed = sRight;
    stlChar              * sMedKey;
    dtlCompareResult       sResult;
    smpOffsetTableHeader * sOtHeader = SMP_GET_OFFSET_TABLE_HEADER(SMP_FRAME(aPageHandle));
    stlInt32               sColOrder;
    stlUInt8               sColLenSize;
    stlInt64               sColLen;
    stlChar              * sKeyBody = NULL;
    stlBool                sIsInclude = STL_TRUE;
    knlCompareList       * sCompList;
    stlUInt16              sSlotHdrSize;

    STL_TRY_THROW( aFetchInfo->mRange->mMaxRange != NULL, RAMP_FINISH );
    
    while( sLeft <= sRight )
    {
        sMed = (sLeft + sRight) / 2;
        sMedKey = SMP_GET_NTH_ROW_ITEM( aPageHandle, sOtHeader, sMed );
        SMNMPB_GET_LEAF_KEY_HDR_SIZE( sMedKey, &sSlotHdrSize );
        sKeyBody = sMedKey + sSlotHdrSize;
        sColOrder = -1;
        sCompList = aFetchInfo->mRange->mMaxRange;
        sColLenSize = 0;
        sColLen = 0;
        sIsInclude = STL_TRUE;

        while( sCompList != NULL )
        {
            while( sColOrder < sCompList->mColOrder )
            {
                sKeyBody += (sColLenSize + sColLen);
                SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyBody, &sColLenSize, &sColLen );
                sColOrder++;
            }
            
            if( sColLen == 0 )
            {
                if( ( sCompList->mRangeLen == 0 ) &&
                    ( sCompList->mIsIncludeEqual ) )
                {
                    sResult = DTL_COMPARISON_EQUAL;
                }
                else
                {
                    sResult = ( !sCompList->mIsNullFirst ) - ( sCompList->mIsNullFirst );
                }
            }
            else
            {
                if( sCompList->mRangeLen == 0 )  /* range 가 NULL */
                {
                    /* IS NULL or IS NOT NULL */
                    sResult = ( sCompList->mIsIncludeEqual )
                        ? ( sCompList->mIsNullFirst ) - ( !sCompList->mIsNullFirst )
                        : DTL_COMPARISON_EQUAL;
                }
                else 
                {
                    sResult = sCompList->mCompFunc ( sKeyBody + sColLenSize,
                                                     sColLen,
                                                     sCompList->mRangeVal,
                                                     sCompList->mRangeLen );

                    if( sCompList->mIsAsc )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        sResult = -sResult;
                    }

                    if( (!sCompList->mIsIncludeEqual) && (sResult == DTL_COMPARISON_EQUAL) )
                    {
                        sIsInclude = STL_FALSE;
                        break;
                    }
                    else
                    {
                        /*
                         * compare next range
                         */
                    }
                }
            }
                
            if( sResult == DTL_COMPARISON_GREATER )
            {
                sIsInclude = STL_FALSE;
                break;
            }
            else
            {
                if( sResult == DTL_COMPARISON_LESS )
                {
                    sIsInclude = STL_TRUE;
                    
                    if( sCompList->mNext != NULL )
                    {
                        if( sCompList->mColOrder != sCompList->mNext->mColOrder )
                        {
                            break;
                        }
                    }
                }
                else
                {
                    sIsInclude = STL_TRUE;
                }
            }
            
            sCompList = sCompList->mNext;
        } /* while( sCompList != NULL ) */

        if( sIsInclude == STL_TRUE )
        {
            sLeft = sMed + 1;
        }
        else
        {
            sRight = sMed - 1;
        }
    }
    
    if( sIsInclude == STL_FALSE )
    {
        sMed--;
    }

    STL_RAMP( RAMP_FINISH );
    
    /* after last 이므로 현재 발견한 위치 바로 뒤로 세팅 */
    *aSlotSeq = sMed + 1;

    return STL_SUCCESS;
}


/**
 * @brief Leaf node에서 max key range에 부합하는 마지막 key를 읽을 준비를 한다
 * @param[in] aPageHandle 위치를 찾을 leaf page
 * @param[in] aStartPid 최초 출발할 페이지 아이디
 * @param[in] aIterator 위치를 이동시킬 iterator
 * @param[in,out] aFetchInfo Fetch 정보
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbFindLastLeafKey( smpHandle         * aPageHandle,
                                 smlPid              aStartPid,
                                 smnmpbIterator    * aIterator,
                                 smlFetchInfo      * aFetchInfo,
                                 smlEnv            * aEnv )
{
    void      * sRelHandle = aIterator->mCommon.mRelationHandle;
    stlBool     sResult;
    smlPid      sNextPid;
    smlPid      sCurPid = aStartPid;

    STL_DASSERT( SMP_IS_DUMMY_HANDLE( aPageHandle ) == STL_FALSE );

    while( STL_TRUE )
    {
        if( smpGetFreeness( aPageHandle ) == SMP_FREENESS_FREE )
        {
            sResult = STL_FALSE;
        }
        else
        {
            STL_TRY( smnmpbCheckMaxRangeWithMaxKey( aPageHandle,
                                                    aIterator,
                                                    aFetchInfo,
                                                    &sResult,
                                                    aEnv ) == STL_SUCCESS );
        }

        if( sResult == STL_TRUE )
        {
            /* 현재 노드의 최대 key가 max range를 만족함(SMO). 다음 노드로 */
            sNextPid = SMNMPB_GET_NEXT_PAGE(aPageHandle);
            
            STL_TRY_THROW( sNextPid != SMP_NULL_PID, RAMP_TERMINATE );
            
            sCurPid = sNextPid;

            if( SMP_IS_DUMMY_HANDLE( aPageHandle ) == STL_FALSE )
            {
                STL_TRY( smpRelease( aPageHandle, aEnv ) == STL_SUCCESS );
            }

            STL_TRY( smnmpbSnapshot( smsGetTbsId(SME_GET_SEGMENT_HANDLE(sRelHandle)),
                                     sCurPid,
                                     aPageHandle,
                                     (stlChar*)aIterator->mPageBuf,
                                     aEnv )
                     == STL_SUCCESS );
            continue;
        }
        break;
    }

    STL_RAMP( RAMP_TERMINATE );

    STL_TRY( smnmpbAfterLastLeafKey( aPageHandle,
                                     aIterator,
                                     aFetchInfo,
                                     0,
                                     &aIterator->mCurSlotSeq,
                                     aEnv ) == STL_SUCCESS );

    aIterator->mPageBufPid = sCurPid;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief 특정 Internal node에서 key range에 맞는 마지막 child 노드 pid를 찾는다
 * @param[in] aPageHandle 조사할 internal page의 handle
 * @param[in] aIterator scan 조건과 현재 위치를 저장한 iterator
 * @param[in,out] aFetchInfo Fetch 정보
 * @param[out] aChildPid 찾아낸 하위 노드의 Page Id
 * @param[in] aEnv Storage Manager Environment
 * @todo Key가 1개인 경우에 검증이 필요합니다.
 */
stlStatus smnmpbFindLastChild( smpHandle         * aPageHandle,
                               smnmpbIterator    * aIterator,
                               smlFetchInfo      * aFetchInfo,
                               smlPid            * aChildPid,
                               smlEnv            * aEnv )
{
    stlUInt16              sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(aPageHandle);
    stlUInt16              sEffectiveKeyCount = sTotalKeyCount;
    stlInt16               sLeft = 0;
    stlInt16               sRight = sTotalKeyCount - 1;
    stlInt16               sMed = 0;
    stlChar              * sMedKey = NULL;
    stlChar              * sLastKey;
    dtlCompareResult       sResult;
    stlBool                sIsLastSibling = SMNMPB_IS_LAST_SIBLING(aPageHandle);
    smpOffsetTableHeader * sOtHeader = SMP_GET_OFFSET_TABLE_HEADER(SMP_FRAME(aPageHandle));
    stlInt32               sColOrder;
    stlUInt8               sColLenSize;
    stlInt64               sColLen;
    stlChar              * sKeyBody = NULL;
    stlBool                sIsInclude = STL_TRUE;
    knlCompareList       * sCompList;

    STL_DASSERT( sTotalKeyCount > 0 );

    /* 마지막 sibling일 경우 마지막 key는 max Key 이므로 compare에서 제외한다 */
    if( sIsLastSibling == STL_TRUE )
    {
        sRight--;
        sEffectiveKeyCount--;
    }

    while( sLeft <= sRight )
    {
        sMed = (sLeft + sRight) >> 1;
        sMedKey = SMP_GET_NTH_ROW_ITEM( aPageHandle, sOtHeader, sMed );
        sKeyBody = sMedKey + SMNMPB_INTL_SLOT_HDR_SIZE;
        sColOrder = -1;
        sCompList = aFetchInfo->mRange->mMaxRange;
        sColLenSize = 0;
        sColLen = 0;
        sIsInclude = STL_TRUE;
            
        while( sCompList != NULL )
        {
            while( sColOrder < sCompList->mColOrder )
            {
                sKeyBody += (sColLenSize + sColLen);
                SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyBody, &sColLenSize, &sColLen );
                sColOrder++;
            }
            
            if( sColLen == 0 )
            {
                if( ( sCompList->mRangeLen == 0 ) &&
                    ( sCompList->mIsIncludeEqual ) )
                {
                    sResult = DTL_COMPARISON_EQUAL;
                }
                else
                {
                    sResult = ( !sCompList->mIsNullFirst ) - ( sCompList->mIsNullFirst );
                }
            }
            else
            {
                if( sCompList->mRangeLen == 0 )  /* range 가 NULL */
                {
                    /* IS NULL or IS NOT NULL */
                    sResult = ( sCompList->mIsIncludeEqual )
                        ? ( sCompList->mIsNullFirst ) - ( !sCompList->mIsNullFirst )
                        : DTL_COMPARISON_EQUAL;
                }
                else 
                {
                    sResult = sCompList->mCompFunc( sKeyBody + sColLenSize,
                                                    sColLen,
                                                    sCompList->mRangeVal,
                                                    sCompList->mRangeLen );

                    if( sCompList->mIsAsc )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        sResult = -sResult;
                    }
                    
                    if( (!sCompList->mIsIncludeEqual) && (sResult == DTL_COMPARISON_EQUAL) )
                    {
                        sIsInclude = STL_FALSE;
                        break;
                    }
                    else
                    {
                        /*
                         * compare next range
                         */
                    }
                }
            }
                
            if( sResult == DTL_COMPARISON_GREATER )
            {
                sIsInclude = STL_FALSE;
                break;
            }
            else
            {
                if( sResult == DTL_COMPARISON_LESS )
                {
                    sIsInclude = STL_TRUE;
                    
                    if( sCompList->mNext != NULL )
                    {
                        if( sCompList->mColOrder != sCompList->mNext->mColOrder )
                        {
                            break;
                        }
                    }
                }
                else
                {
                    sIsInclude = STL_TRUE;
                }
            }
            
            sCompList = sCompList->mNext;
        } /* while( sCompList != NULL ) */
        
        if( sIsInclude != STL_TRUE )
        {
            sRight = sMed - 1;
        }
        else
        {
            sLeft = sMed + 1;
        }
    }

    if( sIsInclude == STL_TRUE )
    {
        sMed++;
        sMedKey = SMP_GET_NTH_ROW_ITEM( aPageHandle, sOtHeader, sMed );
    }

    if( sMed >= sEffectiveKeyCount )
    {
        if( sIsLastSibling == STL_TRUE )
        {
            sLastKey = SMP_GET_NTH_ROW_ITEM( aPageHandle, sOtHeader, sTotalKeyCount - 1 );
            SMNMPB_INTL_SLOT_GET_CHILD_PID(sLastKey, aChildPid);
            STL_ASSERT( *aChildPid != SMP_NULL_PID );
        }
        else
        {
            /* 현재 노드에는 해당 key를 삽입할 만한 하위 노드가 없음(SMO) */
            *aChildPid = SMP_NULL_PID;
        }
    }
    else
    {
        SMNMPB_INTL_SLOT_GET_CHILD_PID(sMedKey, aChildPid);
    }

    return STL_SUCCESS;
}



/**
 * @brief 특정 Internal node에서 마지막 key가 현재 max keyrange에 부합하는지 체크한다
 * @param[in] aPageHandle 조사할 internal page의 handle
 * @param[in] aIterator scan 조건과 현재 위치를 저장한 iterator
 * @param[in,out] aFetchInfo Fetch 정보
 * @param[out] aResult 체크결과 condition에 부합하는지 여부
 * @param[in] aEnv Storage Manager Environment
 */
inline stlStatus smnmpbCheckMaxRangeWithMaxKey( smpHandle         * aPageHandle,
                                                smnmpbIterator    * aIterator,
                                                smlFetchInfo      * aFetchInfo,
                                                stlBool           * aResult,
                                                smlEnv            * aEnv )
{
    stlInt16           sLastKeySeq = SMP_GET_PAGE_ROW_ITEM_COUNT( aPageHandle ) - 1;
    stlChar          * sLastKey;
    stlBool            sIsLeaf = SMNMPB_GET_LEVEL(aPageHandle ) == 0 ? STL_TRUE : STL_FALSE;

    if( (sIsLeaf != STL_TRUE) &&
        (SMNMPB_IS_LAST_SIBLING(aPageHandle) == STL_TRUE) )
    {
        sLastKeySeq--;
    }

    if( (sIsLeaf == STL_TRUE) && (sLastKeySeq == -1) )
    {
        *aResult = STL_TRUE;
    }
    else
    {
        if( sLastKeySeq < 0 )
        {
            *aResult = STL_TRUE;
        }
        else
        {
            sLastKey = smpGetNthRowItem( aPageHandle, sLastKeySeq );

            STL_TRY( smnmpbCheckMaxRange( sLastKey,
                                          aFetchInfo->mRange->mMaxRange,
                                          sIsLeaf,
                                          aResult,
                                          aEnv ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Iterator에서 마지막 key를 읽을 준비를 한다
 * @param[in] aIterator 위치를 이동시킬 iterator
 * @param[in,out] aFetchInfo Fetch 정보
 * @param[out] aPageHandle 찾은 첫 leaf page
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbFindLast( void          * aIterator,
                          smlFetchInfo  * aFetchInfo,
                          smpHandle     * aPageHandle,
                          smlEnv        * aEnv )
{
    smpHandle        sPageHandle;
    smnmpbIterator * sIterator = (smnmpbIterator*)aIterator;
    void           * sRelHandle = sIterator->mCommon.mRelationHandle;
    smlPid           sCurPid;
    smlPid           sChildPid;
    stlBool          sResult;
    smlPid           sRootPid;
    stlInt32         sState = 0;
    stlInt32         sFixState = 0;
    smlTbsId         sTbsId = smsGetTbsId(SME_GET_SEGMENT_HANDLE(sRelHandle));

    SMP_MAKE_DUMMY_HANDLE( &sPageHandle, NULL );
    
    STL_RAMP( RAMP_RETRY_FROM_ROOT );
    
    sCurPid = SMN_GET_ROOT_PAGE_ID(sRelHandle);
    sRootPid = sCurPid;

    while( STL_TRUE )
    {
        /**
         * Scan Scalability를 높이기 위해서 Latch의 사용보다는
         * Mirror 페이지를 이용한다.
         */

        if( (sRootPid == sCurPid) &&
            (SMN_GET_MIRROR_ROOT_PID( sRelHandle ) != SMP_NULL_PID) )
        {
            STL_TRY( smpFix( sTbsId,
                             SMN_GET_MIRROR_ROOT_PID( sRelHandle ),
                             &sPageHandle,
                             aEnv )
                     == STL_SUCCESS );
            sFixState = 1;
        }
        else
        {
            STL_TRY( smpAcquire( NULL,
                                 sTbsId,
                                 sCurPid,
                                 KNL_LATCH_MODE_SHARED,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
            sState = 1;
        
            if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
            {
                /* 이미 Free된 Page에 도달. 처음서 부터 다시 시도 */
                if( sState == 1 )
                {
                    sState = 0;
                    STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
                }
            
                if( sFixState == 1 )
                {
                    sFixState = 0;
                    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
                }
            
                STL_THROW( RAMP_RETRY_FROM_ROOT );
            }
        }

        if( SMNMPB_IS_LEAF(&sPageHandle) == STL_TRUE )
        {
            /**
             * Leaf는 Mirror페이지가 될수 없다.
             */
            STL_DASSERT( sFixState == 0 );
            
            sChildPid = sCurPid;
            break;
        }

        while( STL_TRUE )
        {
            /* Sibling으로 이동중에는 free node를 만날 수 없다 */
            STL_DASSERT( (sRootPid == sCurPid) ||
                         (smpGetFreeness( &sPageHandle ) != SMP_FREENESS_FREE) );

            STL_TRY( smnmpbCheckMaxRangeWithMaxKey( &sPageHandle,
                                                    aIterator,
                                                    aFetchInfo,
                                                    &sResult,
                                                    aEnv ) == STL_SUCCESS );
            if( sResult == STL_TRUE )
            {
                /* 현재 노드의 최대 key가 max range를 만족함(SMO). 다음 노드로 */
                sCurPid = SMNMPB_GET_NEXT_PAGE(&sPageHandle);
                if( sCurPid == SMP_NULL_PID )
                {
                    /**
                     * Max Node로 이동한다.
                     */
                    sChildPid = smnmpbGetLastChild( &sPageHandle );
                    
                    if( sState == 1 )
                    {
                        sState = 0;
                        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
                    }
                    
                    if( sFixState == 1 )
                    {
                        sFixState = 0;
                        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
                    }
                    
                    sCurPid = sChildPid;
                    break;
                }

                if( sState == 1 )
                {
                    sState = 0;
                    STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
                }
                    
                if( sFixState == 1 )
                {
                    sFixState = 0;
                    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
                }
                    
                STL_TRY( smpAcquire( NULL,
                                     sTbsId,
                                     sCurPid,
                                     KNL_LATCH_MODE_SHARED,
                                     &sPageHandle,
                                     aEnv ) == STL_SUCCESS );
                sState = 1;
                
                continue;
            }
            
            STL_TRY( smnmpbFindLastChild( &sPageHandle,
                                          sIterator,
                                          aFetchInfo,
                                          &sChildPid,   /* out */
                                          aEnv ) == STL_SUCCESS );
            STL_ASSERT( sChildPid != SMP_NULL_PID );
            
            if( sState == 1 )
            {
                sState = 0;
                STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
            }
            
            if( sFixState == 1 )
            {
                sFixState = 0;
                STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
            }
                    
            sCurPid = sChildPid;
            break;
        }
    }

    STL_TRY( smnmpbFindLastLeafKey( &sPageHandle,
                                    sCurPid,
                                    sIterator,
                                    aFetchInfo,
                                    aEnv ) == STL_SUCCESS );

    *aPageHandle = sPageHandle;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, aEnv );
    }
    
    if( sFixState == 1 )
    {
        (void)smpUnfix( &sPageHandle, aEnv );
    }
                    
    return STL_FAILURE;
}



/**
 * @brief 일반 Scan을 위해 열린 Iterator에서 이전 row를 반환한다
 * @param[in]       aIterator    이전에 읽은 위치를 가진 iterator
 * @param[in,out]   aFetchInfo   Fetch 정보
 * @param[in]       aEnv         Storage Manager Environment
 */
stlStatus smnmpbFetchPrev( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv )
{
    smnmpbIterator  * sIterator = (smnmpbIterator*)aIterator;
    smlPid            sPid = sIterator->mPageBufPid;
    smlPid            sCurPid;
    stlChar         * sCurRow;
    smlScn            sCommitScn;
    smxlTransId       sWaitTransId;
    stlBool           sHasValidVersion;
    smpHandle         sPageHandle = {NULL, (void*)sIterator->mPageBuf};
    stlInt32          sBlockIdx = 0;
    stlBool           sMatchPhysicalFilter;
    stlInt32          sState = 0;
    void            * sRelHandle = sIterator->mCommon.mRelationHandle;
    smlTbsId          sTbsId = smsGetTbsId(SME_GET_SEGMENT_HANDLE(sRelHandle));

    smlRid    sPrevRowId = SML_INVALID_RID;
    smpHandle sTablePageHandle;
    stlBool   sTablePageLatchRelease = STL_TRUE;
    smnIndexHeader      * sIndexHeader = (smnIndexHeader *)(sIterator->mCommon.mRelationHandle);

    STL_PARAM_VALIDATE( aFetchInfo->mRowBlock != NULL, KNL_ERROR_STACK(aEnv) );
    
    aFetchInfo->mIsEndOfScan = STL_TRUE;
    SML_RESET_USED_BLOCK( aFetchInfo->mRowBlock );

    while( (aFetchInfo->mFetchCnt > 0) &&
           (sBlockIdx < SML_BLOCK_SIZE( aFetchInfo->mRowBlock )) )
    {
        if( sPid == SMP_NULL_PID )
        {
            STL_TRY( smnmpbFindLast( sIterator,
                                     aFetchInfo,
                                     &sPageHandle,
                                     aEnv ) == STL_SUCCESS );
            sState = 1;
            
            sPid = sIterator->mPageBufPid;
            STL_ASSERT( sPid != SMP_NULL_PID );

            smpAfterNthRowItem( SMP_FRAME(&sPageHandle),
                                &sIterator->mCommon.mSlotIterator,
                                sIterator->mCurSlotSeq - 1 );

            STL_TRY( smnmpbFindMinFence( sIterator,
                                         aFetchInfo,
                                         SMP_FRAME(&sPageHandle),
                                         sIterator->mCurSlotSeq - 1,
                                         aEnv ) == STL_SUCCESS );
        }

        while( sPid != SMP_NULL_PID )
        {
            if( sIterator->mCurSlotSeq == SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle) )
            {
                SMP_GET_LAST_ROWITEM( SMP_FRAME(&sPageHandle),
                                      &sIterator->mCommon.mSlotIterator,
                                      sCurRow );
            }
            else
            {
                SMP_GET_PREV_ROWITEM( &sIterator->mCommon.mSlotIterator, sCurRow );
            }

            if( sCurRow != NULL )
            {
                /* Index는 free slot이 없으므로 무조건 감소시켜도 맞는다 */
                sIterator->mCurSlotSeq--;
                if( sIterator->mCurSlotSeq <= sIterator->mSlotSeqFence )
                {
                    aFetchInfo->mIsEndOfScan = STL_TRUE;
                    break;
                }

                STL_TRY( smnmpbGetValidVersion( sTbsId,
                                                &sPageHandle,
                                                sIterator,
                                                aFetchInfo,
                                                sCurRow,
                                                aFetchInfo->mColList,
                                                sBlockIdx,
                                                &sCommitScn,
                                                &sWaitTransId,
                                                &sHasValidVersion,
                                                &sMatchPhysicalFilter,   /* for Physical Key-Filter */
                                                aEnv ) == STL_SUCCESS );

                if( sHasValidVersion != STL_TRUE )
                {
                    continue;
                }

                sIterator->mLastValidKeyPid = sIterator->mPageBufPid;

                if( sMatchPhysicalFilter == STL_TRUE )
                {
                    /**
                     * RowId Column을 참조하는 경우, RowId Column Block에 값을 만든다. 
                     */
                    if( aFetchInfo->mRowIdColList != NULL )
                    {
                        STL_TRY( dtlRowIdSetValueFromRowIdInfo(
                                     aFetchInfo->mTableLogicalId,
                                     sIterator->mRowRid.mTbsId,
                                     sIterator->mRowRid.mPageId,
                                     sIterator->mRowRid.mOffset,
                                     KNL_GET_BLOCK_SEP_DATA_VALUE( aFetchInfo->mRowIdColList,
                                                                   sBlockIdx ),
                                     KNL_ERROR_STACK(aEnv) )
                                 == STL_SUCCESS );            
                    }
                    
                    /* logical key filter 평가 */
                    if( aFetchInfo->mFilterEvalInfo == NULL )
                    {
                        /* found */
                        /* Do Nothing */
                    }
                    else
                    {
                        aFetchInfo->mFilterEvalInfo->mBlockIdx = sBlockIdx;

                        STL_TRY( knlEvaluateOneExpression( aFetchInfo->mFilterEvalInfo,
                                                           KNL_ENV(aEnv) )
                                 == STL_SUCCESS );
                        
                        STL_DASSERT( KNL_GET_BLOCK_IS_SEP_BUFF(
                                         aFetchInfo->mFilterEvalInfo->mResultBlock )
                                     == STL_TRUE );
            
                        if( DTL_BOOLEAN_IS_TRUE( KNL_GET_BLOCK_SEP_DATA_VALUE(
                                                     aFetchInfo->mFilterEvalInfo->mResultBlock,
                                                     sBlockIdx ) ) )
                        {
                            /* Do Nothing */
                        }
                        else
                        {
                            aFetchInfo->mIsEndOfScan = STL_TRUE;
                            continue;
                        }
                    }

                    if( aFetchInfo->mFetchRecordInfo == NULL )
                    {
                        aFetchInfo->mIsEndOfScan = STL_FALSE;
                        break;
                    }
                    else
                    {
                        if ( (sPrevRowId.mTbsId  == sIterator->mRowRid.mTbsId) &&
                             (sPrevRowId.mPageId == sIterator->mRowRid.mPageId) &&
                             (sTablePageLatchRelease == STL_FALSE) )
                        {
                            /**
                             * 기존 Page 를 그대로 사용한다
                             */
                        }
                        else
                        {
                            if( sTablePageLatchRelease == STL_FALSE )
                            {
                                sTablePageLatchRelease = STL_TRUE;
                                STL_TRY( smpRelease( &sTablePageHandle, aEnv ) == STL_SUCCESS );
                            }
                
                            STL_TRY( smpAcquire( NULL,
                                                 sIterator->mRowRid.mTbsId,
                                                 sIterator->mRowRid.mPageId,
                                                 KNL_LATCH_MODE_SHARED,
                                                 &sTablePageHandle,
                                                 aEnv ) == STL_SUCCESS );

                            sPrevRowId.mTbsId  = sIterator->mRowRid.mTbsId;
                            sPrevRowId.mPageId = sIterator->mRowRid.mPageId;
                            sTablePageLatchRelease = STL_FALSE;
                        }

                        /* Fetch Record */
                        STL_TRY( smdFetchRecord4Index( (smiIterator*)sIterator,
                                                       sIndexHeader->mBaseRelHandle,
                                                       aFetchInfo->mFetchRecordInfo,
                                                       & sTablePageHandle,
                                                       & sTablePageLatchRelease,
                                                       & sIterator->mRowRid,
                                                       sBlockIdx,
                                                       sIterator->mAgableViewScn,
                                                       & sMatchPhysicalFilter,
                                                       aEnv ) == STL_SUCCESS );

                        if( sMatchPhysicalFilter == STL_TRUE )
                        {
                            aFetchInfo->mIsEndOfScan = STL_FALSE;
                            break;
                        }
                        else
                        {
                            aFetchInfo->mIsEndOfScan = STL_TRUE;
                            continue;
                        }
                    }
                }
                else
                {
                    aFetchInfo->mIsEndOfScan = STL_TRUE;
                    continue;
                }
            } /* if */

            /* 현재 페이지에서 적당한 row를 찾지 못했기 때문에 다음 페이지로 이동함 */
            sCurPid = sIterator->mPageBufPid;

            STL_RAMP( RAMP_FETCH_PREV_PAGE );

            /* table page latch 를 오래 잡고 있지 않도록(최대 하나의 index page 구간동안만) latch 를 release 한다 */
            if( sTablePageLatchRelease == STL_FALSE )
            {
                sTablePageLatchRelease = STL_TRUE;
                STL_TRY( smpRelease( &sTablePageHandle, aEnv ) == STL_SUCCESS );
            }
            
            sPid = SMNMPB_GET_PREV_PAGE( &sPageHandle );

            if( sState == 1 )
            {
                sState = 0;
                
                if( SMP_IS_DUMMY_HANDLE( &sPageHandle ) == STL_FALSE )
                {
                    STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
                }
            }
            
            KNL_BREAKPOINT( KNL_BREAKPOINT_SMNMPBFETCH_PREV_BEFORE_MOVE_PREV_PAGE, KNL_ENV(aEnv) );

            if( sPid != SMP_NULL_PID )
            {
                /**
                 * Event 검사
                 */
                STL_TRY( knlCheckQueryTimeout( KNL_ENV(aEnv) ) == STL_SUCCESS );

                /* 다음 페이지가 존재하므로 caching함 */
                
                STL_TRY( smnmpbSnapshot( smsGetTbsId(SME_GET_SEGMENT_HANDLE(sIterator->mCommon.mRelationHandle)),
                                         sPid,
                                         &sPageHandle,
                                         (stlChar*)sIterator->mPageBuf,
                                         aEnv )
                         == STL_SUCCESS );

                if( (smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE) ||
                    (SMNMPB_GET_NEXT_PAGE( &sPageHandle ) != sCurPid) )
                {
                    /* Leaf Scan 도중 SMO를 만남 -> 최근의 valid key를 가진 노드의 이전부터 다시 */
                    if( sIterator->mLastValidKeyPid == SMP_NULL_PID )
                    {
                        /* valid한 key를 만난 적이 없다. 맨 뒤부터 다시 스캔 */
                        STL_TRY( smnmpbFindLast( sIterator,
                                                 aFetchInfo,
                                                 &sPageHandle,
                                                 aEnv ) == STL_SUCCESS );
                        sState = 1;
                        
                        sCurPid = SMP_NULL_PID;
                        sPid = sIterator->mPageBufPid;
                    }
                    else
                    {
                        STL_TRY( smnmpbSnapshot( smsGetTbsId(SME_GET_SEGMENT_HANDLE(sIterator->mCommon.mRelationHandle)),
                                                 sIterator->mLastValidKeyPid,
                                                 &sPageHandle,
                                                 (stlChar*)sIterator->mPageBuf,
                                                 aEnv )
                                 == STL_SUCCESS );

                        sCurPid = sIterator->mLastValidKeyPid;
                        sPid = SMP_NULL_PID;
                        STL_THROW( RAMP_FETCH_PREV_PAGE );
                    }
 
                }
                else
                {
                    sIterator->mCurSlotSeq = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle);
                }

                sIterator->mPageBufPid = sPid;

                STL_TRY( smnmpbFindMinFence( sIterator,
                                             aFetchInfo,
                                             SMP_FRAME(&sPageHandle),
                                             sIterator->mCurSlotSeq - 1,
                                             aEnv ) == STL_SUCCESS );
                smpAfterNthRowItem( SMP_FRAME(&sPageHandle),
                                    &sIterator->mCommon.mSlotIterator,
                                    sIterator->mCurSlotSeq - 1 );
            }
            else
            {
                /* 다음 페이지가 없으므로 EOF를 리턴 */
                sIterator->mPageBufPid = sPid;
                sIterator->mRowRid.mTbsId = 0;
                sIterator->mRowRid.mPageId = SMP_NULL_PID;
                sIterator->mRowRid.mOffset = -1;
                aFetchInfo->mIsEndOfScan = STL_TRUE;
                break;
            }
        } /* while( STL_TRUE ) */

        if( aFetchInfo->mIsEndOfScan == STL_TRUE )
        {
            /**
             * @remark Multiple Range를 지원하는 경우 (key filter와 filter 구성은 동일한 range scan)
             * 다음 Range 범위의 Record들을 평가하도록 한다.
             * @todo predicate이 아닌 range만 변경하도록 코드 수정
             */

            /* if( sIterator->mCommon.mPredicate != NULL ) */
            /* {    */
            /*     if( sIterator->mCommon.mPredicate->mNext != NULL ) */
            /*     { */
            /*         if( sState == 1 ) */
            /*         { */
            /*             sState = 0; */
                        
            /*             if( SMP_IS_DUMMY_HANDLE( &sPageHandle ) == STL_FALSE ) */
            /*             { */
            /*                 STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS ); */
            /*             } */
            /*         } */
                    
            /*         sIterator->mCommon.mPredicate = sIterator->mCommon.mPredicate->mNext; */
            /*         sPid = SMP_NULL_PID; */
            /*         STL_THROW( RAMP_RETRY ); */
            /*     } */
            /*     break; */
            /* } */

            break;
        }
        else
        {
            if( aFetchInfo->mSkipCnt > 0 )
            {
                aFetchInfo->mSkipCnt--;
                continue;
            }
            else
            {
                aFetchInfo->mFetchCnt--;
            }

            /**
             * DDL statement는 현재 commit scn보다 작은 view scn을 가질수도 있다.
             * - SCN_VALUE는 version conflict를 확인하기 위해서 사용되므로 MAX값을 반환해야 한다. 
             */
            if( sCommitScn == SML_INFINITE_SCN )
            {
                sCommitScn = SMI_GET_VIEW_SCN( sIterator );
            }
            else
            {
                sCommitScn = STL_MAX( sCommitScn, SMI_GET_VIEW_SCN( sIterator ) );
            }
            
            SML_SET_RID_VALUE( aFetchInfo->mRowBlock, sBlockIdx, sIterator->mRowRid );
            SML_SET_SCN_VALUE( aFetchInfo->mRowBlock, sBlockIdx, sCommitScn );

            sBlockIdx++;
        }
    }/* while( sBlockIdx < knlGetBlockAllocCnt(aColList) ) */

    if( sTablePageLatchRelease == STL_FALSE )
    {
        sTablePageLatchRelease = STL_TRUE;
        STL_TRY( smpRelease( &sTablePageHandle, aEnv ) == STL_SUCCESS );
    }
    
    if( aFetchInfo->mColList != NULL )
    {
        KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT( aFetchInfo->mColList, 0, sBlockIdx );
    }

    if( aFetchInfo->mRowIdColList != NULL )
    {
        KNL_SET_ONE_BLOCK_SKIP_CNT( aFetchInfo->mRowIdColList, 0 );
        KNL_SET_ONE_BLOCK_USED_CNT( aFetchInfo->mRowIdColList, sBlockIdx );
    }
    
    if( aFetchInfo->mFetchRecordInfo != NULL )
    {
        if( aFetchInfo->mFetchRecordInfo->mColList != NULL )
        {
            KNL_SET_ALL_BLOCK_SKIP_AND_USED_CNT(
                aFetchInfo->mFetchRecordInfo->mColList,
                0,
                sBlockIdx );
        }

        /**
         * Logical KeyFilter와 Logical Table Filter에 모두 RowId Column이 있는 경우,
         * RowId Column에 값이 중복 처리되는 것을 방지하기 위해,
         * sIterator->mCommon.mFetchRecordInfo->mRowIdColList 는 NULL임.
         * smdmphFetchRecord4Index()함수에서 호출되는
         * smdmphGetValidVersion에서 RowId 값을 설정하지 않도록 ...
         */
        STL_DASSERT( aFetchInfo->mFetchRecordInfo->mRowIdColList == NULL );
    }

    
    SML_SET_USED_BLOCK_SIZE( aFetchInfo->mRowBlock, sBlockIdx );

    if( sState == 1 )
    {
        if( SMP_IS_DUMMY_HANDLE( &sPageHandle ) == STL_FALSE )
        {
            if( aFetchInfo->mIsEndOfScan != STL_TRUE )
            {
                stlMemcpy( sIterator->mPageBuf, SMP_FRAME(&sPageHandle), SMP_PAGE_SIZE );
                smpAfterNthRowItem( (void*)sIterator->mPageBuf,
                                    &sIterator->mCommon.mSlotIterator,
                                    sIterator->mCurSlotSeq - 1 );
            }

            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sTablePageLatchRelease == STL_FALSE )
    {
        sTablePageLatchRelease = STL_TRUE;
        STL_TRY( smpRelease( &sTablePageHandle, aEnv ) == STL_SUCCESS );
    }
    
    if( sState == 1 )
    {
        if( SMP_IS_DUMMY_HANDLE( &sPageHandle ) == STL_FALSE )
        {
            (void) smpRelease( &sPageHandle, aEnv );
        }
    }

    return STL_FAILURE;
}


/** @} */

