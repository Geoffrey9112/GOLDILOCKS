/*******************************************************************************
 * smnmpbDml.c
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
#include <sms.h>
#include <smxm.h>
#include <smxl.h>
#include <smp.h>
#include <sme.h>
#include <sma.h>
#include <smn.h>
#include <smnDef.h>
#include <smnmpbDef.h>
#include <smnmpb.h>

/**
 * @file smnmpbDml.c
 * @brief Storage Manager Layer Memory B-Tree Index DML Routines
 */

/**
 * @todo fetch prev 구현
 * @todo lock row 구현
 * @todo 90:10 split 구현(?)
 * @todo smpReallocNthSlot 제거 고려(free & alloc으로)
 * @todo 성능 향상을 위해 index page 정보에 접근하는 방법을 offset으로 단순화
 */

/**
 * @addtogroup smn
 * @{
 */

/**
 * @brief 주어진 Internal Key의 총 길이를 구한다
 * @param[in] aKey 조사할 key의 시작 위치
 * @param[in] aColCount 조사할 key가 가진 column count
 * @return internal key size
 */
inline stlInt16 smnmpbGetIntlKeySize( stlChar * aKey, stlUInt16 aColCount )
{
    stlUInt16   i;
    stlInt16   sKeySize = SMNMPB_INTL_SLOT_HDR_SIZE;
    stlChar  * sPtr = aKey + sKeySize;
    stlUInt8   sColLenSize;
    stlInt64   sColLen;

    for( i = 0; i < aColCount; i++ )
    {
        SMP_GET_COLUMN_LEN_ZERO_INSENS(sPtr, &sColLenSize, &sColLen);
        sKeySize += sColLenSize + sColLen;
        sPtr += sColLenSize + sColLen;
    }

    return sKeySize;
}


/**
 * @brief 주어진 Leaf Key의 총 길이를 구한다
 * @param[in] aKey 조사할 key의 시작 위치
 * @param[in] aColCount 조사할 key가 가진 column count
 * @return leaf key size
 */
inline stlInt16 smnmpbGetLeafKeySize( stlChar * aKey, stlUInt16 aColCount )
{
    stlUInt16   i;
    stlInt16    sKeySize;
    stlChar   * sPtr;
    stlUInt8    sColLenSize;
    stlInt64    sColLen;

    SMNMPB_GET_LEAF_KEY_HDR_SIZE( aKey, &sKeySize );
    sPtr = aKey + sKeySize;
    for( i = 0; i < aColCount; i++ )
    {
        SMP_GET_COLUMN_LEN_ZERO_INSENS(sPtr, &sColLenSize, &sColLen);
        sKeySize += sColLenSize + sColLen;
        sPtr += sColLenSize + sColLen;
    }

    return sKeySize;
}

/**
 * @brief Leaf node index key의 길이와 header를 뺀 body 정보를 구한다.
 * @param[in] aKey 조사할 key의 시작 위치
 * @param[in] aColCount 조사할 key가 가진 column count
 * @param[out] aKeySize key의 크기
 * @param[out] aKeyBodySize key header를 뺀 body의 크기
 * @param[out] aKeyBody key header를 뺀 body의 시작 위치
 * @param[out] aKeyRowRid key row Rid
 */
void smnmpbGetLeafKeySizeAndBody( stlChar     * aKey,
                                  stlUInt16     aColCount,
                                  stlUInt16   * aKeySize,
                                  stlUInt16   * aKeyBodySize,
                                  stlChar    ** aKeyBody,
                                  smlRid      * aKeyRowRid )
{
    stlUInt16  sKeyHdrSize;

    SMNMPB_GET_LEAF_KEY_HDR_SIZE( aKey, &sKeyHdrSize );
    *aKeySize = smnmpbGetLeafKeySize( aKey, aColCount );
    *aKeyBodySize = *aKeySize - sKeyHdrSize;
    *aKeyBody = aKey + sKeyHdrSize;
    SMNMPB_GET_ROW_PID( aKey, &(aKeyRowRid->mPageId) );
    SMNMPB_GET_ROW_SEQ( aKey, &(aKeyRowRid->mOffset) );
}

/**
 * @brief Intl node index key의 길이와 header를 뺀 body 정보를 구한다.
 * @param[in] aKey 조사할 key의 시작 위치
 * @param[in] aColCount 조사할 key가 가진 column count
 * @param[out] aKeySize key의 크기
 * @param[out] aKeyBodySize key header를 뺀 body의 크기
 * @param[out] aKeyBody key header를 뺀 body의 시작 위치
 * @param[out] aKeyRowRid key row Rid
 */
void smnmpbGetIntlKeySizeAndBody( stlChar     * aKey,
                                  stlUInt16     aColCount,
                                  stlUInt16   * aKeySize,
                                  stlUInt16   * aKeyBodySize,
                                  stlChar    ** aKeyBody,
                                  smlRid      * aKeyRowRid )
{
    *aKeySize = smnmpbGetIntlKeySize( aKey, aColCount );
    *aKeyBodySize = *aKeySize - SMNMPB_INTL_SLOT_HDR_SIZE;
    *aKeyBody = aKey + SMNMPB_INTL_SLOT_HDR_SIZE;
    SMNMPB_INTL_SLOT_GET_ROW_PID( aKey, &(aKeyRowRid->mPageId) );
    SMNMPB_INTL_SLOT_GET_ROW_OFFSET( aKey, &(aKeyRowRid->mOffset) );
}

/**
 * @brief 주어진 Key를 insert한 Transaction이 commit했는지를 체크한다 
 *        commit 되지 않은경우 SML_INFINITE_SCN를 반환한다
 * @param[in] aPageFrame 조사할 key가 담긴 페이지의 시작 위치
 * @param[in] aKeySlot 조사할 Key
 * @param[out] aIsCommitted commit 되어있는지 여부
 * @param[out] aWaitTransId 해당 Rts의 Transaction Id
 * @param[in] aEnv Storage Manager Environment
 */
inline stlStatus smnmpbIsInsertCommitted( void        * aPageFrame,
                                          stlChar     * aKeySlot,
                                          stlBool     * aIsCommitted,
                                          smxlTransId * aWaitTransId,
                                          smlEnv      * aEnv )
{
    stlUInt8      sInsertRtsSeq;
    stlUInt8      sInsertRtsVerNo;
    smxlTransId   sTransId;
    smlScn        sScn;
    smlScn        sCommitScn;
    smpRts      * sRts;

    *aIsCommitted = STL_FALSE;
    *aWaitTransId = SML_INVALID_TRANSID;

    SMNMPB_GET_INS_RTS_INFO( aKeySlot, &sInsertRtsSeq, &sInsertRtsVerNo );

    while( STL_TRUE )
    {
        if( sInsertRtsSeq == SMP_RTS_SEQ_STABLE )
        {
            *aIsCommitted = STL_TRUE;
            break;
        }
        else if( SMP_IS_VALID_RTS_SEQ( sInsertRtsSeq ) == STL_TRUE )
        {
            sRts = SMP_GET_NTH_RTS( aPageFrame, sInsertRtsSeq );
            if( sInsertRtsVerNo < sRts->mPrevVerCnt )
            {
                *aIsCommitted = STL_TRUE;
                break;
            }
            sScn = sRts->mScn;
            sTransId = sRts->mTransId;
            if( SML_IS_VIEW_SCN( sScn ) != STL_TRUE )
            {
                *aIsCommitted = STL_TRUE;
            }
            else
            {
                STL_TRY( smxlGetCommitScn( sTransId,
                                           SML_MAKE_REAL_SCN(sScn),
                                           &sCommitScn,
                                           aEnv ) == STL_SUCCESS );
                if( sCommitScn == SML_INFINITE_SCN )
                {
                    *aWaitTransId = sTransId;
                }
                else
                {
                    *aIsCommitted = STL_TRUE;
                }
            }
            break;
        }
        else
        {
            /* TBK임 */
            STL_ASSERT( SMNMPB_IS_TBK( aKeySlot ) == STL_TRUE );
            SMNMPB_GET_TBK_INS_TRANS_ID( aKeySlot, &sTransId );
            SMNMPB_GET_TBK_INS_SCN( aKeySlot, &sScn );
            if( SML_IS_VIEW_SCN( sScn ) == STL_TRUE )
            {
                STL_TRY( smxlGetCommitScn( sTransId,
                                           SML_MAKE_REAL_SCN(sScn),
                                           &sCommitScn,
                                           aEnv ) == STL_SUCCESS );
                if( sCommitScn == SML_INFINITE_SCN )
                {
                    *aWaitTransId = sTransId;
                }
                else
                {
                    *aIsCommitted = STL_TRUE;
                }
            }
            else
            {
                *aIsCommitted = STL_TRUE;
            }
            break;
        }
    } /* while */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief 주어진 Key를 insert한 Transaction의 commit scn을 구한다. 
 *        commit 되지 않은경우 SML_INFINITE_SCN를 반환한다
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageHandle 조사할 key가 담긴 페이지의 Handle
 * @param[in] aKeySlot 조사할 Key
 * @param[in] aMyStmtViewScn 내 statement view scn
 * @param[out] aCommitScn 구해진 Commit Scn
 * @param[out] aTcn 구해진 Statement Tcn
 * @param[out] aWaitTransId 해당 Rts의 Transaction Id
 * @param[in] aEnv Storage Manager Environment
 */
inline stlStatus smnmpbGetInsertCommitScn( smlTbsId      aTbsId,
                                           smpHandle   * aPageHandle,
                                           stlChar     * aKeySlot,
                                           smlScn        aMyStmtViewScn,
                                           smlScn      * aCommitScn,
                                           smlTcn      * aTcn,
                                           smxlTransId * aWaitTransId,
                                           smlEnv      * aEnv )
{
    stlUInt8      sInsertRtsSeq;
    stlUInt8      sInsertRtsVerNo;
    smxlTransId   sTransId;
    smlScn        sScn;
    stlUInt8      sTcn;

    *aWaitTransId = SML_INVALID_TRANSID;

    SMNMPB_GET_INS_RTS_INFO( aKeySlot, &sInsertRtsSeq, &sInsertRtsVerNo );
    SMNMPB_GET_INS_TCN( aKeySlot, &sTcn );
    *aTcn = sTcn;

    if( sInsertRtsSeq == SMP_RTS_SEQ_STABLE )
    {
        *aCommitScn = 0;
    }
    else if( SMP_IS_VALID_RTS_SEQ( sInsertRtsSeq ) == STL_TRUE )
    {
        STL_TRY( smpGetCommitScn( aTbsId,
                                  aPageHandle,
                                  sInsertRtsSeq,
                                  sInsertRtsVerNo,
                                  aMyStmtViewScn,
                                  aWaitTransId,
                                  aCommitScn,
                                  aEnv ) == STL_SUCCESS );
    }
    else
    {
        /* TBK임 */
        STL_ASSERT( SMNMPB_IS_TBK( aKeySlot ) == STL_TRUE );
        SMNMPB_GET_TBK_INS_TRANS_ID( aKeySlot, &sTransId );
        SMNMPB_GET_TBK_INS_SCN( aKeySlot, &sScn );
        if( SML_IS_VIEW_SCN( sScn ) == STL_TRUE )
        {
            STL_TRY( smxlGetCommitScn( sTransId,
                                       SML_MAKE_REAL_SCN(sScn),
                                       aCommitScn,
                                       aEnv ) == STL_SUCCESS );
            if( *aCommitScn == SML_INFINITE_SCN )
            {
                *aWaitTransId = sTransId;
            }
        }
        else
        {
            *aCommitScn = sScn;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Key를 Delete한 Transaction이 commit했는지를 체크한다 
 *        commit 되지 않은경우 SML_INFINITE_SCN를 반환한다
 * @param[in] aPageFrame 조사할 key가 담긴 페이지의 시작 위치
 * @param[in] aKeySlot 조사할 Key
 * @param[out] aIsCommitted commit 되어있는지 여부
 * @param[out] aWaitTransId 해당 Rts의 Transaction Id
 * @param[in] aEnv Storage Manager Environment
 */
inline stlStatus smnmpbIsDeleteCommitted( void        * aPageFrame,
                                          stlChar     * aKeySlot,
                                          stlBool     * aIsCommitted,
                                          smxlTransId * aWaitTransId,
                                          smlEnv      * aEnv )
{
    stlUInt8      sDeleteRtsSeq;
    stlUInt8      sDeleteRtsVerNo;
    smxlTransId   sTransId;
    smlScn        sScn;
    smlScn        sCommitScn;
    smpRts      * sRts;

    *aIsCommitted = STL_FALSE;
    *aWaitTransId = SML_INVALID_TRANSID;

    SMNMPB_GET_DEL_RTS_INFO( aKeySlot, &sDeleteRtsSeq, &sDeleteRtsVerNo );

    while( STL_TRUE )
    {
        if( sDeleteRtsSeq == SMP_RTS_SEQ_STABLE )
        {
            *aIsCommitted = STL_TRUE;
            break;
        }
        else if( SMP_IS_VALID_RTS_SEQ( sDeleteRtsSeq ) == STL_TRUE )
        {
            sRts = SMP_GET_NTH_RTS( aPageFrame, sDeleteRtsSeq );
            if( sDeleteRtsVerNo < sRts->mPrevVerCnt )
            {
                *aIsCommitted = STL_TRUE;
                break;
            }
            sScn = sRts->mScn;
            sTransId = sRts->mTransId;
            if( SML_IS_VIEW_SCN( sScn ) != STL_TRUE )
            {
                *aIsCommitted = STL_TRUE;
            }
            else
            {
                STL_TRY( smxlGetCommitScn( sTransId,
                                           SML_MAKE_REAL_SCN(sScn),
                                           &sCommitScn,
                                           aEnv ) == STL_SUCCESS );
                if( sCommitScn == SML_INFINITE_SCN )
                {
                    *aWaitTransId = sTransId;
                }
                else
                {
                    *aIsCommitted = STL_TRUE;
                }
            }
            break;
        }
        else
        {
            /* TBK임 */
            STL_ASSERT( SMNMPB_IS_TBK( aKeySlot ) == STL_TRUE );
            SMNMPB_GET_TBK_DEL_TRANS_ID( aKeySlot, &sTransId );
            SMNMPB_GET_TBK_DEL_SCN( aKeySlot, &sScn );
            if( SML_IS_VIEW_SCN( sScn ) == STL_TRUE )
            {
                STL_TRY( smxlGetCommitScn( sTransId,
                                           SML_MAKE_REAL_SCN(sScn),
                                           &sCommitScn,
                                           aEnv ) == STL_SUCCESS );
                if( sCommitScn == SML_INFINITE_SCN )
                {
                    *aWaitTransId = sTransId;
                }
                else
                {
                    *aIsCommitted = STL_TRUE;
                }
            }
            else
            {
                *aIsCommitted = STL_TRUE;
            }
            break;
        }
    } /* while */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Key를 Delete한 Transaction이 Agable한지 체크한다 
 * @param[in] aPageFrame 조사할 key가 담긴 페이지의 시작 위치
 * @param[in] aKeySlot 조사할 Key
 * @param[in] aAgableScn aging 가능한 Scn
 * @param[out] aIsAgable aging 가능한지 여부
 * @param[in] aEnv Storage Manager Environment
 */
inline stlStatus smnmpbIsKeyAgable( void       * aPageFrame,
                                    stlChar    * aKeySlot,
                                    smlScn       aAgableScn,
                                    stlBool    * aIsAgable,
                                    smlEnv     * aEnv )
{
    stlUInt8      sDeleteRtsSeq;
    stlUInt8      sDeleteRtsVerNo;
    stlUInt8      sPrevVerCnt;
    smxlTransId   sTransId;
    smlScn        sScn;
    smlScn        sCommitScn;
    smpRts      * sRts;

    *aIsAgable = STL_FALSE;

    SMNMPB_GET_DEL_RTS_INFO( aKeySlot, &sDeleteRtsSeq, &sDeleteRtsVerNo );

    if( sDeleteRtsSeq == SMP_RTS_SEQ_STABLE )
    {
        *aIsAgable = STL_TRUE;
    }
    else if( SMP_IS_VALID_RTS_SEQ( sDeleteRtsSeq ) == STL_TRUE )
    {
        sRts = SMP_GET_NTH_RTS( aPageFrame, sDeleteRtsSeq );
        sScn = sRts->mScn;
        
        if( SML_IS_VIEW_SCN( sScn ) == STL_TRUE )
        {
            *aIsAgable = STL_FALSE;
        }
        else
        {
            sPrevVerCnt = sRts->mPrevVerCnt;
            
            if( sDeleteRtsVerNo == sPrevVerCnt )
            {
                sCommitScn = sScn;
            }
            else if( sDeleteRtsVerNo == sPrevVerCnt - 1 )
            {
                sCommitScn = sRts->mPrevCommitScn;
            }
            else
            {
                if( (sScn <= aAgableScn) || (sRts->mPrevCommitScn <= aAgableScn) )
                {
                    sCommitScn = 0;
                }
                else
                {
                    sCommitScn = SML_INFINITE_SCN;
                }
            }

            if( sCommitScn <= aAgableScn )
            {
                *aIsAgable = STL_TRUE;
            }
        }
    }
    else
    {
        /* TBK임 */
        STL_ASSERT( SMNMPB_IS_TBK( aKeySlot ) == STL_TRUE );
        SMNMPB_GET_TBK_DEL_TRANS_ID( aKeySlot, &sTransId );
        SMNMPB_GET_TBK_DEL_SCN( aKeySlot, &sScn );
        if( SML_IS_VIEW_SCN( sScn ) == STL_TRUE )
        {
            STL_TRY( smxlGetCommitScn( sTransId,
                                       SML_MAKE_REAL_SCN(sScn),
                                       &sCommitScn,
                                       aEnv ) == STL_SUCCESS );
        }
        if( sCommitScn <= aAgableScn )
        {
            *aIsAgable = STL_TRUE;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Key를 Delete한 Transaction의 commit scn을 구한다. 
 *        commit 되지 않은경우 SML_INFINITE_SCN를 반환한다
 * @param[in] aTbsId Tablespace Identifier
 * @param[in] aPageHandle 조사할 key가 담긴 페이지의 Handle
 * @param[in] aKeySlot 조사할 Key
 * @param[in] aMyStmtViewScn 내 statement view scn
 * @param[out] aCommitScn 구해진 Commit Scn
 * @param[out] aTcn 구해진 Statement Tcn
 * @param[out] aWaitTransId 해당 Rts의 Transaction Id
 * @param[in] aEnv Storage Manager Environment
 */
inline stlStatus smnmpbGetDeleteCommitScn( smlTbsId      aTbsId,
                                           smpHandle   * aPageHandle,
                                           stlChar     * aKeySlot,
                                           smlScn        aMyStmtViewScn,
                                           smlScn      * aCommitScn,
                                           smlTcn      * aTcn,
                                           smxlTransId * aWaitTransId,
                                           smlEnv      * aEnv )
{
    stlUInt8      sDeleteRtsSeq;
    stlUInt8      sDeleteRtsVerNo;
    smxlTransId   sTransId;
    smlScn        sScn;
    stlUInt8      sTcn;

    STL_ASSERT( SMNMPB_IS_DELETED(aKeySlot) == STL_TRUE );

    *aWaitTransId = SML_INVALID_TRANSID;
    SMNMPB_GET_DEL_RTS_INFO( aKeySlot, &sDeleteRtsSeq, &sDeleteRtsVerNo );
    SMNMPB_GET_DEL_TCN( aKeySlot, &sTcn );
    *aTcn = sTcn;

    if( sDeleteRtsSeq == SMP_RTS_SEQ_STABLE )
    {
        *aCommitScn = 0;
    }
    else if( SMP_IS_VALID_RTS_SEQ( sDeleteRtsSeq ) == STL_TRUE )
    {

        STL_TRY( smpGetCommitScn( aTbsId,
                                  aPageHandle,
                                  sDeleteRtsSeq,
                                  sDeleteRtsVerNo,
                                  aMyStmtViewScn,
                                  aWaitTransId,
                                  aCommitScn,
                                  aEnv ) == STL_SUCCESS );
    }
    else
    {
        /* TBK임 */
        STL_ASSERT( SMNMPB_IS_TBK( aKeySlot ) == STL_TRUE );
        SMNMPB_GET_TBK_DEL_TRANS_ID( aKeySlot, &sTransId );
        SMNMPB_GET_TBK_DEL_SCN( aKeySlot, &sScn );
        if( SML_IS_VIEW_SCN( sScn ) == STL_TRUE )
        {
            STL_TRY( smxlGetCommitScn( sTransId,
                                       SML_MAKE_REAL_SCN(sScn),
                                       aCommitScn,
                                       aEnv ) == STL_SUCCESS );
            if( *aCommitScn == SML_INFINITE_SCN )
            {
                *aWaitTransId = sTransId;
            }
        }
        else
        {
            *aCommitScn = sScn;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief 주어진 Key가 주어진 rtsseq를 사용하고 있으면 stabilize 한다
 * @param[in] aKey stamping을 시도할 key
 * @param[in] aRtsSeq 체크할 Rts의 Sequence
 * @param[in] aCommitScn stamping할 commit scn
 * @param[in] aLastRtsVerNo stabilize 하기 전의 Rts Ver No 의 최대 값
 * @param[out] aReferenceCount aLastRtsVerNo와 같은 ver no를 가지는 stabilized key의 개수
 */
void smnmpbStabilizeKey( stlChar   * aKey,
                         stlUInt8    aRtsSeq,
                         stlUInt8    aLastRtsVerNo,
                         smlScn      aCommitScn,
                         stlInt16  * aReferenceCount )
{
    stlUInt8  sRtsSeq;
    stlUInt8  sRtsVerNo;

    *aReferenceCount = 0;
    SMNMPB_GET_INS_RTS_INFO(aKey, &sRtsSeq, &sRtsVerNo);
    if( aRtsSeq == sRtsSeq )
    {
        if( sRtsVerNo == aLastRtsVerNo )
        {
            (*aReferenceCount)++;
        }

        sRtsSeq = SMP_RTS_SEQ_STABLE;
        sRtsVerNo = 0;
        SMNMPB_SET_INS_RTS_INFO(aKey, &sRtsSeq, &sRtsVerNo);
    }
    if( SMNMPB_IS_DELETED(aKey) == STL_TRUE )
    {
        SMNMPB_GET_DEL_RTS_INFO(aKey, &sRtsSeq, &sRtsVerNo);
        if( aRtsSeq == sRtsSeq )
        {
            if( sRtsVerNo == aLastRtsVerNo )
            {
                (*aReferenceCount)++;
            }

            sRtsSeq = SMP_RTS_SEQ_STABLE;
            sRtsVerNo = 0;
            SMNMPB_SET_DEL_RTS_INFO(aKey, &sRtsSeq, &sRtsVerNo);
        }
    }
}


/**
 * @brief 주어진 리프노드 내의 RTS를 모두 stamping하고 dead key들을 aging 한다
 * @param[in] aRelationHandle Index의 relation handle
 * @param[in] aPageHandle aging을 할 페이지의 handle
 * @param[in,out] aTracingSlotSeq aging으로 변화된 slot seqquence 
 * @param[in] aNeedRowStamping row stamping도 필요한지 여부
 * @param[out] aIsFreed @a aTracingSlotSeq가 Free되었는지 여부
 * @param[in,out] aEnv Storage manager environment
 */
stlStatus smnmpbDoAgingLeafNode( void      * aRelationHandle,
                                 smpHandle * aPageHandle,
                                 stlInt16  * aTracingSlotSeq,
                                 stlBool     aNeedRowStamping,
                                 stlBool   * aIsFreed,
                                 smlEnv    * aEnv )
{
    stlUInt8               sRtsCount = SMP_GET_CURRENT_RTS_COUNT( SMP_FRAME(aPageHandle) );
    stlInt16               i;
    stlInt16               sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( aPageHandle );
    stlChar              * sCurKey;
    stlUInt8               sInsertRtsSeq;
    stlUInt8               sInsertRtsVerNo;
    stlUInt8               sDeleteRtsSeq;
    stlUInt8               sDeleteRtsVerNo;
    stlInt16               sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(aPageHandle);
    smlTbsId               sTbsId = smpGetTbsId(aPageHandle);
    smlScn                 sAgableScn = smxlGetAgableTbsScn( sTbsId, aEnv );
    smnmpbStatistics     * sStatistics;
    stlBool                sIsAgable;
    smpOffsetTableHeader * sOtHeader = SMP_GET_OFFSET_TABLE_HEADER(SMP_FRAME(aPageHandle));

    STL_ASSERT( *aTracingSlotSeq <= sTotalKeyCount );

    *aIsFreed = STL_FALSE;
    if( aRelationHandle != NULL )
    {
        sStatistics = SMNMPB_GET_MODULE_SPECIFIC_DATA( aRelationHandle );
        SMNMPB_INC_STATISTICS( sStatistics, AgingCount );
    }

    if( aNeedRowStamping == STL_TRUE )
    {
        for( i = 0; i < sRtsCount; i++ )
        {
            smpTryStampRts( NULL, sTbsId, aPageHandle, i, smnmpbStabilizeKey, aEnv );
        }
        
    }
    else
    {
        for( i = 0; i < sRtsCount; i++ )
        {
            smpTryStampRts( NULL, sTbsId, aPageHandle, i, NULL, aEnv );
        }
    }

    i = 0;
    while( i < sTotalKeyCount )
    {
        sCurKey = SMP_GET_NTH_ROW_ITEM( aPageHandle, sOtHeader, i );

        if( SMNMPB_IS_DELETED(sCurKey) == STL_TRUE )
        {
            STL_TRY( smnmpbIsKeyAgable( SMP_FRAME(aPageHandle),
                                        sCurKey,
                                        sAgableScn,
                                        &sIsAgable,
                                        aEnv ) == STL_SUCCESS );
            if( sIsAgable == STL_TRUE )
            {
                /* DEAD KEY 임 */
                /* Aging으로 노드가 완전히 비게되면 node free시 상위 노드를 찾을 수 없게되므로 */
                /* 이런 현상을 막기위해 노드의 유일한 key는 DEAD 상태라도 free하지 않는다 */
                if( sTotalKeyCount > 1 )
                {
                    SMNMPB_GET_INS_RTS_INFO( sCurKey, &sInsertRtsSeq, &sInsertRtsVerNo );
                    SMNMPB_GET_DEL_RTS_INFO( sCurKey, &sDeleteRtsSeq, &sDeleteRtsVerNo );
                    if( SMP_IS_VALID_RTS_SEQ(sDeleteRtsSeq) == STL_TRUE )
                    {
                        STL_TRY( smpEndUseRts( aPageHandle,
                                               sCurKey,
                                               sDeleteRtsSeq,
                                               sDeleteRtsVerNo,
                                               aEnv ) == STL_SUCCESS );
                    }
                    if( SMP_IS_VALID_RTS_SEQ(sInsertRtsSeq) == STL_TRUE )
                    {
                        STL_TRY( smpEndUseRts( aPageHandle,
                                               sCurKey,
                                               sInsertRtsSeq,
                                               sInsertRtsVerNo,
                                               aEnv ) == STL_SUCCESS );
                    }

                    STL_TRY( smpFreeNthSlot( aPageHandle,
                                             i,
                                             smnmpbGetLeafKeySize(sCurKey, sKeyColCount) ) == STL_SUCCESS );
                    SMP_REMOVE_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), STL_SIZEOF(smpOffsetSlot) );
                    
                    sTotalKeyCount--;
                    if( i < *aTracingSlotSeq )
                    {
                        (*aTracingSlotSeq)--;
                    }
                    else if( i == *aTracingSlotSeq )
                    {
                        *aIsFreed = STL_TRUE;
                    }
                    continue;
                }
            }
        }
        i++;
    }

    STL_TRY( smnmpbValidateIndexPage( aPageHandle,
                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief source page에서 주어진 범위의 Leaf key들을 destination page로 copy한다
 *        aging 이후에 호출됨을 가정하기 때문에 본 함수에서는 deadkey의 skip 코드가 없다
 *        따라서 compact로 인해 기존 key slot seq나 key count에 변화가 없다(!!!)
 * @param[in] aSrcPage source page의 Handle
 * @param[in] aDstPage destination page의 Handle(Key count가 0이어야 함)
 * @param[in] aFromSeq copy를 시작한 key의sequence
 * @param[in] aToSeq copy를 종료할 key의sequence
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbCopyLeafKeys( smpHandle * aSrcPage,
                              smpHandle * aDstPage,
                              stlInt16    aFromSeq,
                              stlInt16    aToSeq,
                              smlEnv    * aEnv )
{
    smxlTransId          sInsertTransId = SML_INVALID_TRANSID;
    smxlTransId          sDeleteTransId = SML_INVALID_TRANSID;
    stlInt16             sTotalKeySize;
    stlChar            * sSrcKey;
    stlChar            * sDstKey;
    stlChar            * sDstKey2;
    smpRts             * sSrcInsertRts = NULL;
    smpRts             * sSrcDeleteRts = NULL;
    stlUInt8             sInsertRtsSeq;
    stlUInt8             sDeleteRtsSeq;
    stlUInt8             sNewInsertRtsSeq;
    stlUInt8             sNewDeleteRtsSeq;
    stlUInt8             sInsertRtsVerNo;
    stlUInt8             sDeleteRtsVerNo;
    stlInt16             i;
    smlScn               sInsertRtsScn;
    smlScn               sDeleteRtsScn;
    stlUInt16            sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(aSrcPage);
    smpRowItemIterator   sIterator;
    stlInt16             sNextSeq = aFromSeq;
    stlInt16             sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(aSrcPage);
    stlUInt8             sDstRtsCnt = 0;
    stlUInt8             sDstRtsSeqs[SMP_MAX_RTS_COUNT];

    stlMemset( sDstRtsSeqs, SMP_RTS_SEQ_NULL, STL_SIZEOF(stlUInt8) * SMP_MAX_RTS_COUNT );

    i = 0;
    smpBeforeNthRowItem( SMP_FRAME(aSrcPage), &sIterator, aFromSeq );

    aToSeq = (aToSeq > sTotalKeyCount - 1) ? sTotalKeyCount - 1 : aToSeq;

    while( sNextSeq <= aToSeq )
    {
        SMP_GET_NEXT_ROWITEM( &sIterator, sSrcKey );
        STL_ASSERT( sSrcKey != NULL );
        sNextSeq++;

        /* key slot 할당 테스트 */
        sTotalKeySize = smnmpbGetLeafKeySize(sSrcKey, sKeyColCount);
        STL_TRY( smpAllocNthSlot( aDstPage,
                                  i,
                                  sTotalKeySize,
                                  STL_TRUE,
                                  &sDstKey ) == STL_SUCCESS );

        /* RTS 처리 */
        /* insert commit scn 을 체크 */
        SMNMPB_GET_INS_RTS_INFO( sSrcKey, &sInsertRtsSeq, &sInsertRtsVerNo );
        sNewInsertRtsSeq = sInsertRtsSeq;
        /* 필요한 경우 insert rts를 할당 */
        if( SMP_IS_VALID_RTS_SEQ(sInsertRtsSeq) == STL_TRUE )
        {
            sSrcInsertRts = SMP_GET_NTH_RTS(SMP_FRAME(aSrcPage), sInsertRtsSeq);
            sInsertTransId = sSrcInsertRts->mTransId;
            sInsertRtsScn = sSrcInsertRts->mScn;
            if( sDstRtsSeqs[sInsertRtsSeq] == SMP_RTS_SEQ_NULL )
            {
                sNewInsertRtsSeq = sDstRtsCnt;

                STL_TRY( smpAllocRtsForCopy( aDstPage,
                                             sInsertTransId,
                                             sInsertRtsScn, /* SML_MAKE_VIEW_SCN을 사용하면 안됨 */
                                             sNewInsertRtsSeq,
                                             aEnv ) == STL_SUCCESS );
                STL_ASSERT( SMP_IS_VALID_RTS_SEQ(sNewInsertRtsSeq) == STL_TRUE );

                /* prev version 위치를 나누어 세팅함 */
                STL_TRY( smpSetRtsPrevLink( aDstPage,
                                            sNewInsertRtsSeq,
                                            sSrcInsertRts->mPrevPid,
                                            sSrcInsertRts->mPrevOffset,
                                            sSrcInsertRts->mPrevVerCnt,
                                            sSrcInsertRts->mPrevCommitScn) == STL_SUCCESS );

                sDstRtsSeqs[sInsertRtsSeq] = sNewInsertRtsSeq;
                sDstRtsCnt++;
            }
            else
            {
                sNewInsertRtsSeq = sDstRtsSeqs[sInsertRtsSeq];
            }

            STL_TRY( smpBeginUseRtsForCopy( aDstPage,
                                            sDstKey,
                                            sInsertTransId,
                                            sInsertRtsScn, /* SML_MAKE_VIEW_SCN을 사용하면 안됨 */
                                            sNewInsertRtsSeq,
                                            sInsertRtsVerNo,
                                            aEnv ) == STL_SUCCESS );

            KNL_ASSERT( sInsertRtsVerNo <= sSrcInsertRts->mPrevVerCnt,
                        aEnv,
                        ("RID(%d,%d,%d), RTS_SEQ(%d), RTS_VER_NO(%d), RTS_PREV_VER_CNT(%d)\nPAGE BODY\n%s",
                         smpGetTbsId(aDstPage),
                         smpGetPageId(aDstPage),
                         SMP_GET_CURRENT_ROWITEM_SEQ(&sIterator),
                         sInsertRtsSeq,
                         sInsertRtsVerNo,
                         sSrcInsertRts->mPrevVerCnt,
                         knlDumpBinaryForAssert( SMP_FRAME(aSrcPage),
                                                 SMP_PAGE_SIZE,
                                                 &aEnv->mLogHeapMem,
                                                 KNL_ENV(aEnv))) );
        }
        else
        {
            /* Insert Rts가 NULL 이거나 STABLE 상태임 */
            sInsertRtsScn = SML_INFINITE_SCN;
        }

        /* delete commit scn 을 체크 */
        if( (SMNMPB_IS_DELETED(sSrcKey) == STL_TRUE) )
        {
            SMNMPB_GET_DEL_RTS_INFO( sSrcKey, &sDeleteRtsSeq, &sDeleteRtsVerNo );
        }
        else
        {
            sDeleteRtsSeq   = SMP_RTS_SEQ_NULL;
            sDeleteRtsVerNo = 0;
        }

        sNewDeleteRtsSeq = sDeleteRtsSeq;
        /* 필요한 경우 delete rts를 할당 */
        if( SMP_IS_VALID_RTS_SEQ(sDeleteRtsSeq) == STL_TRUE )
        {
            sSrcDeleteRts = SMP_GET_NTH_RTS(SMP_FRAME(aSrcPage), sDeleteRtsSeq);
            sDeleteTransId = sSrcDeleteRts->mTransId;
            sDeleteRtsScn = sSrcDeleteRts->mScn;

            if( sInsertRtsSeq == sDeleteRtsSeq )
            {
                sNewDeleteRtsSeq = sNewInsertRtsSeq;
            }
            else
            {
                if( sDstRtsSeqs[sDeleteRtsSeq] == SMP_RTS_SEQ_NULL )
                {
                    sNewDeleteRtsSeq = sDstRtsCnt;
                    STL_TRY( smpAllocRtsForCopy( aDstPage,
                                                 sDeleteTransId,
                                                 sDeleteRtsScn, /* SML_MAKE_VIEW_SCN을 사용하면 안됨 */
                                                 sNewDeleteRtsSeq,
                                                 aEnv ) == STL_SUCCESS );
                    STL_ASSERT( SMP_IS_VALID_RTS_SEQ(sNewDeleteRtsSeq) == STL_TRUE );

                    STL_ASSERT( sDeleteRtsVerNo <= sSrcDeleteRts->mPrevVerCnt );
                    STL_TRY( smpSetRtsPrevLink( aDstPage,
                                                sNewDeleteRtsSeq,
                                                sSrcDeleteRts->mPrevPid,
                                                sSrcDeleteRts->mPrevOffset,
                                                sSrcDeleteRts->mPrevVerCnt,
                                                sSrcDeleteRts->mPrevCommitScn)
                             == STL_SUCCESS );

                    sDstRtsSeqs[sDeleteRtsSeq] = sNewDeleteRtsSeq;
                    sDstRtsCnt++;
                }
                else
                {
                    sNewDeleteRtsSeq = sDstRtsSeqs[sDeleteRtsSeq];
                }
            }

            STL_TRY( smpBeginUseRtsForCopy( aDstPage,
                                            sDstKey,
                                            sDeleteTransId,
                                            sDeleteRtsScn, /* SML_MAKE_VIEW_SCN을 사용하면 안됨 */
                                            sNewDeleteRtsSeq,
                                            sDeleteRtsVerNo,
                                            aEnv ) == STL_SUCCESS );

        }
        else
        {
            /* delete rts 가 NULL 이거나 STABLE 상태임 */ 
            sDeleteRtsScn = SML_INFINITE_SCN;
        }

        /* 실제 key slot 할당 */
        STL_TRY( smpAllocNthSlot( aDstPage,
                                  i,
                                  sTotalKeySize,
                                  STL_FALSE,
                                  &sDstKey2 ) == STL_SUCCESS );
        STL_ASSERT( sDstKey2 == sDstKey );
        stlMemcpy( sDstKey, sSrcKey, sTotalKeySize );
        if( SMP_IS_VALID_RTS_SEQ(sNewInsertRtsSeq ) == STL_TRUE )
        {
            SMNMPB_SET_INS_RTS_INFO( sDstKey, &sNewInsertRtsSeq, &sInsertRtsVerNo );
        }
        if( SMP_IS_VALID_RTS_SEQ(sNewDeleteRtsSeq ) == STL_TRUE )
        {
            SMNMPB_SET_DEL_RTS_INFO( sDstKey, &sNewDeleteRtsSeq, &sDeleteRtsVerNo );
        }

        if( SMNMPB_IS_DELETED(sSrcKey) != STL_TRUE )
        {
            SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aDstPage) );
        }
        else
        {
            SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aDstPage), sTotalKeySize + STL_SIZEOF(smpOffsetSlot) );
        }

        i++;
    } /* while */

    smpSetMaxViewScn( aDstPage, smpGetMaxViewScn( aSrcPage ) );
    
    STL_TRY( smnmpbValidateIndexPage( aDstPage,
                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief source page에서 주어진 범위의 Internal key들을 destination page로 copy한다
 * @param[in] aSrcPage source page의 Handle
 * @param[in] aDstPage destination page의 Handle(Key count가 0이어야 함)
 * @param[in] aFromSeq copy를 시작한 key의sequence
 * @param[in] aToSeq copy를 종료할 key의sequence
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbCopyIntlKeys( smpHandle * aSrcPage,
                              smpHandle * aDstPage,
                              stlInt16    aFromSeq,
                              stlInt16    aToSeq,
                              smlEnv    * aEnv )
{
    stlInt16             sTotalKeySize;
    stlInt16             sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(aSrcPage);
    stlChar            * sSrcKey;
    stlChar            * sDstKey;
    stlUInt16            sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(aSrcPage);
    smpRowItemIterator   sIterator;
    stlInt16             sNextSeq = aFromSeq;
    stlInt16             i = 0;

    smpBeforeNthRowItem( SMP_FRAME(aSrcPage), &sIterator, aFromSeq );
    aToSeq = (aToSeq > sTotalKeyCount - 1) ? sTotalKeyCount - 1 : aToSeq;
    
    while( sNextSeq <= aToSeq )
    {
        SMP_GET_NEXT_ROWITEM( &sIterator, sSrcKey );
        sNextSeq++;
        STL_ASSERT( sSrcKey != NULL );

        if( (sNextSeq == sTotalKeyCount) &&
            (SMNMPB_IS_LAST_SIBLING(aSrcPage) == STL_TRUE) ) /* Max key */
        {
            sTotalKeySize = STL_SIZEOF(smlPid);
        }
        else
        {
            sTotalKeySize = smnmpbGetIntlKeySize(sSrcKey, sKeyColCount);
        }
        STL_TRY( smpAllocNthSlot( aDstPage,
                                  i,
                                  sTotalKeySize,
                                  STL_FALSE,
                                  &sDstKey ) == STL_SUCCESS );
        stlMemcpy( sDstKey, sSrcKey, sTotalKeySize );
        i++; 
        SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aDstPage) );
    } /* while */
   
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Leaf 페이지를 재조정하여 최소의 공간만을 사용하도록 한다
 * @param[in] aRelationHandle Relation Handle
 * @param[in] aPageHandle Compact 연산을 할 페이지의 Handle
 * @param[in] aRowCountLimit Compact 연산에 적용할 row의 개수. 나머지는 free된다.
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbCompactPage( void      * aRelationHandle,
                             smpHandle * aPageHandle,
                             stlInt16    aRowCountLimit,
                             smlEnv    * aEnv )
{
    stlInt64           sTmpPage[SMP_PAGE_SIZE/STL_SIZEOF(stlInt64)];
    smpHandle          sTmpHandle;
    smnmpbStatistics * sStatistics;

    if( aRelationHandle != NULL )
    {
        sStatistics = SMNMPB_GET_MODULE_SPECIFIC_DATA( aRelationHandle );
        SMNMPB_INC_STATISTICS( sStatistics, CompactCount );
    }
    
    SMP_MAKE_DUMMY_HANDLE( &sTmpHandle, sTmpPage );
    stlMemcpy( sTmpPage, SMP_FRAME(aPageHandle), SMP_PAGE_SIZE );

    STL_TRY( smpResetBody(aPageHandle) == STL_SUCCESS );

    if( SMNMPB_GET_LEVEL(aPageHandle) == 0 )
    {
        STL_TRY( smnmpbCopyLeafKeys( &sTmpHandle,
                                     aPageHandle,
                                     0,
                                     aRowCountLimit - 1,
                                     aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smnmpbCopyIntlKeys( &sTmpHandle,
                                     aPageHandle,
                                     0,
                                     aRowCountLimit - 1,
                                     aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 페이지의 key들 중 특정 개수 이상은 버리고 compact 한다
 * @param[in] aRelationHandle Relation Handle
 * @param[in] aPageHandle truncate 연산을 할 페이지의 Handle
 * @param[in] aLeftKeyCount 남을 key들의 개수
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbTruncateNode( void      * aRelationHandle,
                              smpHandle * aPageHandle,
                              stlUInt16   aLeftKeyCount,
                              smlEnv    * aEnv )

{
    STL_TRY( smnmpbCompactPage( aRelationHandle,
                                aPageHandle,
                                aLeftKeyCount,
                                aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief 새로 insert되는 SK(Simple Key) 타입의 Key slot의 header에 정보를 세팅한다
 * @param[in,out] aKey 정보를 세팅할 Key의 시작위치
 * @param[in] aIsDupKey duplicate key인지 여부
 * @param[in] aIsTbk TBK type key인지 여부
 * @param[in] aRtsSeq 사용하는 RTS의 순번
 * @param[in] aRtsVerNo 사용하는 RTS의 Version No
 * @param[in] aTransId Trasaction ID
 * @param[in] aViewScn Trasaction 의 view scn
 * @param[in] aRowRid Key가 가리키는 row의 위치
 */
inline void smnmpbWriteKeyHeader( stlChar     * aKey,
                                  stlBool       aIsDupKey,
                                  stlBool       aIsTbk,
                                  stlUInt8      aRtsSeq,
                                  stlUInt8      aRtsVerNo,
                                  smxlTransId   aTransId,
                                  smlScn        aViewScn,
                                  smlRid      * aRowRid )
{
    smxlTransId sTmpTransId = SML_INVALID_TRANSID;
    smlScn      sTmpScn;
    stlUInt8    sRtsSeq = SMP_RTS_SEQ_NULL;
    stlUInt8    sRtsVerNo = 0;

    SMNMPB_INIT_KEY_HDR_BITMAP( aKey );
    SMNMPB_UNSET_DELETED( aKey );
    SMNMPB_SET_ROW_PID( aKey, &aRowRid->mPageId );
    SMNMPB_SET_ROW_SEQ( aKey, &aRowRid->mOffset );
    SMNMPB_SET_INS_RTS_INFO( aKey, &aRtsSeq, &aRtsVerNo );
    SMNMPB_SET_DEL_RTS_INFO( aKey, &sRtsSeq, &sRtsVerNo);

    if( aIsDupKey == STL_TRUE )
    {
        SMNMPB_SET_DUPKEY( aKey );
    }
    if( aIsTbk == STL_TRUE )
    {
        SMNMPB_SET_TBK( aKey );
        SMNMPB_SET_TBK_INS_TRANS_ID( aKey, &aTransId );
        sTmpScn = SML_MAKE_VIEW_SCN(aViewScn);
        SMNMPB_SET_TBK_INS_SCN( aKey, &sTmpScn );
        SMNMPB_SET_TBK_DEL_TRANS_ID( aKey, &sTmpTransId );
        sTmpScn = SML_INFINITE_SCN;
        SMNMPB_SET_TBK_DEL_SCN( aKey, &sTmpScn );
    }
    else
    {
        SMNMPB_UNSET_TBK( aKey );
    }
}



/**
 * @brief 주어진 key slot의 body에 컬럼들을 기록한다
 * @param[in,out] aStartPtr 컬럼들을 기록할 시작 위치
 * @param[in] aColList 기록을 시작할 첫 컬럼
 * @param[in] aBlockIdx aColList에서 사용할 block idx
 */
inline void smnmpbWriteKeyBody( stlChar           * aStartPtr,
                                knlValueBlockList * aColList,
                                stlInt32            aBlockIdx )
{
    stlChar           * sPtr = aStartPtr;
    knlValueBlockList * sCurCol = aColList;
    dtlDataValue      * sDataValue;

    while( sCurCol != NULL )
    {
        sDataValue = KNL_GET_BLOCK_DATA_VALUE(sCurCol, aBlockIdx);
        
        SMP_WRITE_COLUMN_AND_MOVE_PTR( sPtr,
                                       sDataValue->mValue,
                                       sDataValue->mLength,
                                       STL_FALSE );
        sCurCol = sCurCol->mNext;
    }
}


/**
 * @brief 주어진 Page(Internal 노드)에서 마지막 Child page id를 반환한다
 * @param[in] aPageHandle 체크할 internal node
 */
inline smlPid smnmpbGetLastChild( smpHandle  * aPageHandle )
{
    stlUInt16   sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(aPageHandle);
    stlChar   * sLastKey;
    smlPid      sPid;

    sLastKey = smpGetNthRowItem( aPageHandle, sTotalKeyCount - 1 );
    SMNMPB_INTL_SLOT_GET_CHILD_PID(sLastKey, &sPid);

    return sPid;
}

inline void smnmpbMakeValueListFromSlot( stlChar           * aKeyBody,
                                         knlValueBlockList * aColumns,
                                         stlChar           * aColBuf )
{
    knlValueBlockList * sCurCol;
    stlChar           * sColPtr;
    stlChar           * sBufPtr;
    stlUInt8            sColLenSize;
    stlInt64            sColLen;
    dtlDataValue      * sDataValue;
    
    for( sCurCol = aColumns, sColPtr = aKeyBody, sBufPtr = aColBuf;
         sCurCol != NULL;
         sCurCol = sCurCol->mNext )
    {
        SMP_GET_COLUMN_LEN_ZERO_INSENS( sColPtr, &sColLenSize, &sColLen );
        sDataValue = KNL_GET_BLOCK_DATA_VALUE( sCurCol, 0 );
        sDataValue->mBufferSize = sColLen;
        sDataValue->mLength = sColLen;
        sDataValue->mValue  = sBufPtr;
        
        stlMemcpy( sBufPtr, sColPtr + sColLenSize, sColLen );
        sColPtr += sColLenSize + sColLen;
        sBufPtr = (stlChar*)STL_ALIGN((stlUInt64)(sBufPtr + sColLenSize + sColLen),8);
    }
}


/**
 * @brief 주어진 페이지내의 특정 키와 value 형식의 키를 비교한다
 * @param[in] aKeySlot 페이지내의 특정 key의 포인터
 * @param[in] aKeyColCount key의 컬럼개수
 * @param[in] aValColumns 비교할 key 값을 담은 구조체
 * @param[in] aBlockIdx aValColumns에서 사용할 block idx
 * @param[in] aRowRid 비교할 key가 가리키는 row의 위치. 비교를 원하지 않을 경우 NULL.
 * @param[in] aKeyColFlags index가 가진 key column들의 flag 정보
 * @param[in] aIsLeaf leaf node의 key인지 여부
 * @param[out] aResult 비교 결과
 * @param[in,out] aEnv Storage Manager Environment
 */
inline stlStatus smnmpbCompareKeys( char              * aKeySlot,
                                    stlUInt16           aKeyColCount, 
                                    knlValueBlockList * aValColumns,
                                    stlInt32            aBlockIdx, /* of ValColumns */
                                    smlRid            * aRowRid,
                                    stlUInt8          * aKeyColFlags,
                                    stlBool             aIsLeaf,
                                    dtlCompareResult  * aResult,
                                    smlEnv            * aEnv )
{
    stlChar           * sKeyBody;
    stlUInt16           sSlotHdrSize;
    dtlDataValue      * sValue;
    stlUInt8            sColLenSize;
    stlInt64            sColLen;
    smlRid              sRowRid;
    knlValueBlockList * sValColumn;
    dtlCompareResult    sResult = DTL_COMPARISON_EQUAL;
    stlUInt8          * sKeyFlag;

    if( aIsLeaf == STL_TRUE )
    {
        SMNMPB_GET_LEAF_KEY_HDR_SIZE(aKeySlot, &sSlotHdrSize);
        sKeyBody = aKeySlot + sSlotHdrSize;
    }
    else
    {
        sKeyBody = aKeySlot + SMNMPB_INTL_SLOT_HDR_SIZE;
    }

    sValColumn = aValColumns;
    sKeyFlag = aKeyColFlags;
        
    while( sValColumn != NULL )
    {
        sValue = &(sValColumn->mValueBlock->mDataValue[aBlockIdx]);
        SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyBody, &sColLenSize, &sColLen );
            
        if( sColLen == 0 )
        {
            /**
             * value1이 NULL
             */
            if( sValue->mLength == 0 )
            {
                /* value1(NULL) = value2(NULL) */
                sResult = DTL_COMPARISON_EQUAL;
            }
            else if( (*sKeyFlag & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                     == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
            {
                /* value1(NULL) < value2(NOTNULL) */
                sResult = DTL_COMPARISON_LESS;
                break;
            }
            else 
            {
                /* value1(NULL) > value2(NOTNULL) */
                sResult = DTL_COMPARISON_GREATER;
                break;
            }
        }
        else
        {
            /**
             * value1이 NOT NULL
             */
            if( sValue->mLength == 0 )
            {
                if( (*sKeyFlag & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                    == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
                {                    
                    /* value1(NOTNULL) > value2(NULL) */
                    sResult = DTL_COMPARISON_GREATER;
                    break;
                }
                else 
                {
                    /* value1(NOTNULL) < value2(NULL) */
                    sResult = DTL_COMPARISON_LESS;
                    break;
                }
            }
            else 
            {
                /**
                 * value1(NOTNULL) & value2(NOTNULL)
                 */

                sResult = SMNMPB_COMPARE_FUNC(sValue->mType)( sKeyBody + sColLenSize,
                                                              sColLen,
                                                              sValue->mValue,
                                                              sValue->mLength );

                if( sResult != DTL_COMPARISON_EQUAL )
                {
                    /**
                     * 인덱스가 DESC이면 결과 보정. 
                     */
                    if( (*sKeyFlag & DTL_KEYCOLUMN_INDEX_ORDER_MASK )
                        == DTL_KEYCOLUMN_INDEX_ORDER_DESC )
                    {
                        sResult = -sResult;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                    
                    break;
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }
            
        sKeyFlag++;
        sKeyBody += (sColLen + sColLenSize);
        sValColumn = sValColumn->mNext;
    }

    if( (sResult == DTL_COMPARISON_EQUAL) && (aRowRid != NULL) )
    {
        if( aIsLeaf == STL_TRUE )
        {
            SMNMPB_GET_ROW_PID( aKeySlot, &sRowRid.mPageId );
            SMNMPB_GET_ROW_SEQ( aKeySlot, &sRowRid.mOffset );
        }
        else
        {
            SMNMPB_INTL_SLOT_GET_ROW_PID( aKeySlot, &sRowRid.mPageId );
            SMNMPB_INTL_SLOT_GET_ROW_OFFSET( aKeySlot, &sRowRid.mOffset );
        }
                
        if( sRowRid.mPageId > aRowRid->mPageId )
        {
            sResult = DTL_COMPARISON_GREATER;
        }
        else if( sRowRid.mPageId < aRowRid->mPageId )
        {
            sResult = DTL_COMPARISON_LESS;
        }
        else
        {
            if( sRowRid.mOffset > aRowRid->mOffset )
            {
                sResult = DTL_COMPARISON_GREATER;
            }
            else if( sRowRid.mOffset < aRowRid->mOffset )
            {
                sResult = DTL_COMPARISON_LESS;
            }
            else
            {
                sResult = DTL_COMPARISON_EQUAL;
            }
        }
    }
            
    *aResult = sResult;
    
    return STL_SUCCESS;
}


/**
 * @brief 주어진 Page(Internal 노드)에서 가장 큰 key와 주어진 키를 비교한다
 * @param[in] aPageHandle 체크할 internal node
 * @param[in] aValColumns 비교할 key 값
 * @param[in] aBlockIdx aValColumns에서 사용할 block idx
 * @param[in] aRowRid 비교할 key가 가리키는 row의 위치
 * @param[in] aKeyColFlags index가 가진 key column들의 flag 정보
 * @param[out] aResult 비교 결과
 * @param[in,out] aEnv Storage Manager Environment
 */
inline stlStatus smnmpbCompareWithMaxKey( smpHandle         * aPageHandle,
                                          knlValueBlockList * aValColumns,
                                          stlInt32            aBlockIdx, /* of aValColumns */
                                          smlRid            * aRowRid,
                                          stlUInt8          * aKeyColFlags,
                                          dtlCompareResult  * aResult,
                                          smlEnv            * aEnv )
{
    stlInt16           sLastKeySeq = SMP_GET_PAGE_ROW_ITEM_COUNT( aPageHandle ) - 1;
    stlChar          * sLastKey;
    stlBool            sIsLeaf;

    if( SMNMPB_GET_LEVEL(aPageHandle ) > 0 )
    {
        sIsLeaf = STL_FALSE;
        if( SMNMPB_IS_LAST_SIBLING(aPageHandle) == STL_TRUE )
        {
            *aResult = DTL_COMPARISON_GREATER;
            STL_THROW( RAMP_FINISH );
        }
    }
    else
    {
        sIsLeaf = STL_TRUE;
    }

    if( sLastKeySeq < 0 )
    {
        /* key가 없음. 무조건 이 노드를 사용하게 함 */
        *aResult = DTL_COMPARISON_GREATER;
    }
    else
    {
        sLastKey = smpGetNthRowItem( aPageHandle, sLastKeySeq );

        STL_TRY( smnmpbCompareKeys( sLastKey,
                                    SMNMPB_GET_KEY_COLUMN_COUNT(aPageHandle),
                                    aValColumns,
                                    aBlockIdx,
                                    aRowRid,
                                    aKeyColFlags,
                                    sIsLeaf,
                                    aResult,
                                    aEnv ) == STL_SUCCESS );
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Page(Internal 노드)에서 가장 큰 key와 주어진 키를 비교한다
 * @param[in] aTbsId index의 tablespace ID
 * @param[in] aParentPid key를 찾을 parent 노드의 pid
 * @param[in] aParentSlotSeq parent key의 slot 순번
 * @param[in] aParentSmoNo parent 노드의 SMO No.
 * @param[in] aChildPid parent 노드에서 찾을 child 노드의 pid
 * @param[in] aValColumns 비교할 key 값
 * @param[in] aBlockIdx aValColumns에서 사용할 block idx
 * @param[in] aRowRid 비교할 key가 가리키는 row의 위치
 * @param[in] aKeyColFlags index가 가진 key column들의 flag 정보
 * @param[out] aResult 비교 결과
 * @param[in,out] aEnv Storage Manager Environment
 */
stlStatus smnmpbCompareWithParentKey( smlTbsId            aTbsId,
                                      smlPid              aParentPid,
                                      stlInt16            aParentSlotSeq,
                                      stlUInt64           aParentSmoNo,
                                      smlPid              aChildPid,
                                      knlValueBlockList * aValColumns,
                                      stlInt32            aBlockIdx, /* of aValColumns */
                                      smlRid            * aRowRid,
                                      stlUInt8          * aKeyColFlags,
                                      dtlCompareResult  * aResult,
                                      smlEnv            * aEnv )
{
    smpHandle            sPageHandle;
    stlChar            * sKeySlot;
    stlBool              sHasLatch = STL_FALSE;
    smlPid               sCurPid = aParentPid;
    smlPid               sChildPid;
    smlPid               sNextPid;
    stlInt16             sCurSeq;
    smpRowItemIterator   sIterator;
    stlBool              sIsFirstTry = STL_TRUE;
    stlInt16             sTotalKeyCount;
    stlInt16             sKeyColCount;

    while( STL_TRUE )
    {
        STL_TRY( smpAcquire( NULL,
                             aTbsId,
                             sCurPid,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        sHasLatch = STL_TRUE;

        STL_ASSERT( SMNMPB_GET_LEVEL(&sPageHandle) > 0 );
        STL_DASSERT( smpGetFreeness( &sPageHandle ) != SMP_FREENESS_FREE );
        sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle );
        sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(&sPageHandle);
        sNextPid = SMNMPB_GET_NEXT_PAGE(&sPageHandle);

        if( sIsFirstTry == STL_TRUE )
        {
            if( SMN_GET_PAGE_SMO_NO(&sPageHandle) == aParentSmoNo )
            {
                /* 내려올때 지났던 바로 그 slot이 아직 valid한지 체크한다 */
                sKeySlot = smpGetNthRowItem( &sPageHandle, aParentSlotSeq );
                STL_WRITE_INT32( &sChildPid, sKeySlot );
                if( sChildPid == aChildPid )
                {
                    sCurSeq = aParentSlotSeq;
                    STL_THROW( RAMP_DO_COMPARE );
                }
            }
            sIsFirstTry = STL_FALSE;
        }

        /* 이 노드안에서 해당 Org PID를 가지는 key가 있는지 체크한다 */
        SMP_GET_FIRST_ROWITEM( SMP_FRAME(&sPageHandle), &sIterator, sKeySlot );
        sCurSeq = 0;
        while( sKeySlot != NULL )
        {
            STL_WRITE_INT32( &sChildPid, sKeySlot );
            if( (sChildPid == aChildPid) ||
                ( (sNextPid == SMP_NULL_PID) && (sCurSeq == sTotalKeyCount - 1) ) )
            {
                /* 1. target key를 찾았거나
                 * 2. 현재 노드의 Next Pid == SMP_NULL_PID 이고 마지막 키면 이를 이용한다 */
                STL_THROW( RAMP_DO_COMPARE );
            }
            /* index에는 free offset slot이 중간에 없으므로 그냥 ++ 해도 된다 */
            SMP_GET_NEXT_ROWITEM( &sIterator, sKeySlot );
            sCurSeq++;
        }

        /* 현재 노드에는 aChildPid를 갖는 key가 없으므로 다음 노드로 이동한다 */
        STL_ASSERT( (SMN_GET_PAGE_SMO_NO(&sPageHandle) > aParentSmoNo) &&
                    (sNextPid != SMP_NULL_PID) );
        sCurPid = sNextPid;
        sHasLatch = STL_FALSE;
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
    }

    STL_RAMP( RAMP_DO_COMPARE );

    if( (sNextPid == SMP_NULL_PID) && (sCurSeq == sTotalKeyCount - 1) )
    {
        /* max key(key value를 가지고 있지 않음)는 무조건 greater를 리턴 */
        *aResult = DTL_COMPARISON_GREATER;
    }
    else
    {
        sKeySlot = smpGetNthRowItem( &sPageHandle, sCurSeq );

        STL_TRY( smnmpbCompareKeys( sKeySlot,
                                    sKeyColCount,
                                    aValColumns,
                                    aBlockIdx,
                                    aRowRid,
                                    aKeyColFlags,
                                    STL_FALSE, /* sIsLeaf */
                                    aResult,
                                    aEnv ) == STL_SUCCESS );
    }

    sHasLatch = STL_FALSE;
    STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sHasLatch == STL_TRUE )
    {
        (void)smpRelease( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
}



/**
 * @brief 주어진 Internal 노드에서 해당 key가 들어갈 leaf 노드로의 Path를 가지는 자식 노드를 구한다
 * @param[in] aPageHandle 체크할 internal node
 * @param[in] aValColumns insert할 key 값
 * @param[in] aBlockIdx aValColumns에서 사용할 block idx
 * @param[in] aRowRid insert할 key가 가리키는 row의 위치
 * @param[in] aKeyColFlags index의 key column의 Flags
 * @param[out] aChildPid 주어진 key가 들어갈 leaf로의 path를 가진 자식 page id
 * @param[out] aKeySlotSeq 자식 page id를 가진 key의 sequence
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbFindChildNode( smpHandle         * aPageHandle,
                               knlValueBlockList * aValColumns,
                               stlInt32            aBlockIdx, /* of aValColumns */
                               smlRid            * aRowRid,
                               stlUInt8          * aKeyColFlags,
                               smlPid            * aChildPid,
                               stlInt16          * aKeySlotSeq,
                               smlEnv            * aEnv )
{
    stlUInt16              sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(aPageHandle);
    stlUInt16              sEffectiveKeyCount = sTotalKeyCount;
    stlInt16               sLeft = 0;
    stlInt16               sRight = sTotalKeyCount - 1;
    stlInt16               sMed = 0;
    stlChar              * sMedKey = NULL;
    stlChar              * sKeyBody = NULL;
    stlChar              * sLastKey;
    dtlCompareResult       sResult = DTL_COMPARISON_EQUAL;
    stlBool                sIsLastSibling = SMNMPB_IS_LAST_SIBLING(aPageHandle);
    smpOffsetTableHeader * sOtHeader = SMP_GET_OFFSET_TABLE_HEADER(SMP_FRAME(aPageHandle));
    dtlDataValue         * sValue;
    stlUInt8               sColLenSize;
    stlInt64               sColLen;
    smlRid                 sRowRid;
    knlValueBlockList    * sValColumn = NULL;
    stlUInt8             * sKeyFlag;

    STL_ASSERT( sTotalKeyCount != 0 );

    /* 마지막 sibling일 경우 마지막 key는 max Key 이므로 compare에서 제외한다 */
    if( sIsLastSibling == STL_TRUE )
    {
        sRight--;
        sEffectiveKeyCount--;
        if( sEffectiveKeyCount == 0 )
        {
            /* Max Key 하나만 있는 internal node 임 */
            *aKeySlotSeq = 0;
            sLastKey = SMP_GET_NTH_ROW_ITEM( aPageHandle, sOtHeader, 0 );
            SMNMPB_INTL_SLOT_GET_CHILD_PID(sLastKey, aChildPid);

            STL_THROW( RAMP_FINISH );
        }
    }

    while( sLeft <= sRight )
    {
        sMed = (sLeft + sRight) >> 1;
        sMedKey = SMP_GET_NTH_ROW_ITEM( aPageHandle, sOtHeader, sMed );
        sKeyBody = sMedKey + SMNMPB_INTL_SLOT_HDR_SIZE;
        sKeyFlag = aKeyColFlags;

        sValColumn = aValColumns;
        
        while( sValColumn != NULL )
        {
            sValue = &(sValColumn->mValueBlock->mDataValue[aBlockIdx]);
            SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyBody, &sColLenSize, &sColLen );
            
            if( sColLen == 0 )
            {
                /**
                 * value1이 NULL
                 */
                if( sValue->mLength == 0 )
                {
                    /* value1(NULL) = value2(NULL) */
                    sResult = DTL_COMPARISON_EQUAL;
                }
                else if( (*sKeyFlag & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                         == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
                {
                    /* value1(NULL) < value2(NOTNULL) */
                    sResult = DTL_COMPARISON_LESS;
                    break;
                }
                else 
                {
                    /* value1(NULL) > value2(NOTNULL) */
                    sResult = DTL_COMPARISON_GREATER;
                    break;
                }
            }
            else
            {
                /**
                 * value1이 NOT NULL
                 */
                if( sValue->mLength == 0 )
                {
                    if( (*sKeyFlag & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                        == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
                    {                    
                        /* value1(NOTNULL) > value2(NULL) */
                        sResult = DTL_COMPARISON_GREATER;
                        break;
                    }
                    else 
                    {
                        /* value1(NOTNULL) < value2(NULL) */
                        sResult = DTL_COMPARISON_LESS;
                        break;
                    }
                }
                else 
                {
                    /**
                     * value1(NOTNULL) & value2(NOTNULL)
                     */

                    sResult = SMNMPB_COMPARE_FUNC(sValue->mType)( sKeyBody + sColLenSize,
                                                                  sColLen,
                                                                  sValue->mValue,
                                                                  sValue->mLength );

                    if( sResult != DTL_COMPARISON_EQUAL )
                    {
                        /**
                         * 인덱스가 DESC이면 결과 보정. 
                         */
                        if( (*sKeyFlag & DTL_KEYCOLUMN_INDEX_ORDER_MASK )
                            == DTL_KEYCOLUMN_INDEX_ORDER_DESC )
                        {
                            sResult = -sResult;
                        }
                        else
                        {
                            /* Do Nothing */
                        }
                    
                        break;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
            }
            
            sKeyFlag++;
            sKeyBody += (sColLen + sColLenSize);
            sValColumn = sValColumn->mNext;
        }

        if( sResult == DTL_COMPARISON_EQUAL )
        {
            SMNMPB_INTL_SLOT_GET_ROW_PID( sMedKey, &sRowRid.mPageId );
            SMNMPB_INTL_SLOT_GET_ROW_OFFSET( sMedKey, &sRowRid.mOffset );
                
            if( sRowRid.mPageId > aRowRid->mPageId )
            {
                sResult = DTL_COMPARISON_GREATER;
            }
            else if( sRowRid.mPageId < aRowRid->mPageId )
            {
                sResult = DTL_COMPARISON_LESS;
            }
            else
            {
                if( sRowRid.mOffset > aRowRid->mOffset )
                {
                    sResult = DTL_COMPARISON_GREATER;
                }
                else if( sRowRid.mOffset < aRowRid->mOffset )
                {
                    sResult = DTL_COMPARISON_LESS;
                }
                else
                {
                    sResult = DTL_COMPARISON_EQUAL;
                }
            }
        }
            
        if( sResult == DTL_COMPARISON_GREATER )
        {
            sRight = sMed - 1;
        }
        else if( sResult == DTL_COMPARISON_LESS )
        {
            sLeft = sMed + 1;
        }
        else
        {
            break;
        }
    }
    
    if( sResult == DTL_COMPARISON_LESS )
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
            *aKeySlotSeq = sTotalKeyCount - 1;
        }
        else
        {
            /* 현재 노드에는 해당 key를 삽입할 만한 하위 노드가 없음(SMO) */
            *aChildPid = SMP_NULL_PID;
            *aKeySlotSeq = -1;
            STL_ASSERT( STL_FALSE );
        }
    }
    else
    {
        SMNMPB_INTL_SLOT_GET_CHILD_PID(sMedKey, aChildPid);
        STL_ASSERT( *aChildPid != SMP_NULL_PID );
        *aKeySlotSeq = sMed;
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;
}


/**
 * @brief 주어진 Leaf 노드에서 해당 key가 위치할 Slot Sequence를 구한다
 * @param[in] aValColumns 위치를 찾을 key 값
 * @param[in] aBlockIdx aValColumns에서 사용할 block idx
 * @param[in] aRowRid 위를 찾을 key가 가리키는 row의 위치
 * @param[out] aPageHandle 최종적으로 target으로 선정된 leaf page id
 * @param[out] aSlotSeq leaf page에서의 주어진 key의 sequence
 * @param[out] aFoundKey 찾은 위치에 이미 RowRid까지 같은 key가 존재하는지 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbFindPosition( knlValueBlockList * aValColumns,
                              stlInt32            aBlockIdx, /* of aValColumns */
                              smlRid            * aRowRid,
                              smpHandle         * aPageHandle,
                              stlInt16          * aSlotSeq,
                              stlBool           * aFoundKey,
                              smlEnv            * aEnv )
{
    stlUInt16              sTotalKeyCount;
    stlUInt8             * sKeyColFlags;
    stlInt16               sLeft;
    stlInt16               sRight;
    stlInt16               sMed;
    stlChar              * sMedKey;
    stlChar              * sKeyBody = NULL;
    dtlCompareResult       sResult;
    smpOffsetTableHeader * sOtHeader = SMP_GET_OFFSET_TABLE_HEADER(SMP_FRAME(aPageHandle));
    dtlDataValue         * sValue;
    stlUInt8               sColLenSize;
    stlInt64               sColLen;
    smlRid                 sRowRid;
    stlUInt16              sSlotHdrSize;
    knlValueBlockList    * sValColumn = NULL;
    stlUInt8             * sKeyFlag;
    
    *aFoundKey = STL_FALSE;

    sKeyColFlags = SMNMPB_GET_KEY_COLUMN_FLAGS(aPageHandle);
    sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(aPageHandle);
    sLeft = 0;
    sRight = sTotalKeyCount - 1;
    sMed = 0;
    sResult = DTL_COMPARISON_EQUAL;

    while( sLeft <= sRight )
    {
        sMed = (sLeft + sRight) >> 1;
        sMedKey = SMP_GET_NTH_ROW_ITEM( aPageHandle, sOtHeader, sMed );
        SMNMPB_GET_LEAF_KEY_HDR_SIZE( sMedKey, &sSlotHdrSize );
        sKeyBody = sMedKey + sSlotHdrSize;
        sKeyFlag = sKeyColFlags;

        sValColumn = aValColumns;
        
        while( sValColumn != NULL )
        {
            sValue = &(sValColumn->mValueBlock->mDataValue[aBlockIdx]);
            SMP_GET_COLUMN_LEN_ZERO_INSENS( sKeyBody, &sColLenSize, &sColLen );
            
            if( sColLen == 0 )
            {
                /**
                 * value1이 NULL
                 */
                if( sValue->mLength == 0 )
                {
                    /* value1(NULL) = value2(NULL) */
                    sResult = DTL_COMPARISON_EQUAL;
                }
                else if( (*sKeyFlag & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                         == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
                {
                    /* value1(NULL) < value2(NOTNULL) */
                    sResult = DTL_COMPARISON_LESS;
                    break;
                }
                else 
                {
                    /* value1(NULL) > value2(NOTNULL) */
                    sResult = DTL_COMPARISON_GREATER;
                    break;
                }
            }
            else
            {
                /**
                 * value1이 NOT NULL
                 */
                if( sValue->mLength == 0 )
                {
                    if( (*sKeyFlag & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK)
                        == DTL_KEYCOLUMN_INDEX_NULL_ORDER_FIRST )
                    {                    
                        /* value1(NOTNULL) > value2(NULL) */
                        sResult = DTL_COMPARISON_GREATER;
                        break;
                    }
                    else 
                    {
                        /* value1(NOTNULL) < value2(NULL) */
                        sResult = DTL_COMPARISON_LESS;
                        break;
                    }
                }
                else 
                {
                    /**
                     * value1(NOTNULL) & value2(NOTNULL)
                     */

                    sResult = SMNMPB_COMPARE_FUNC(sValue->mType)( sKeyBody + sColLenSize,
                                                                  sColLen,
                                                                  sValue->mValue,
                                                                  sValue->mLength );

                    if( sResult != DTL_COMPARISON_EQUAL )
                    {
                        /**
                         * 인덱스가 DESC이면 결과 보정. 
                         */
                        if( (*sKeyFlag & DTL_KEYCOLUMN_INDEX_ORDER_MASK )
                            == DTL_KEYCOLUMN_INDEX_ORDER_DESC )
                        {
                            sResult = -sResult;
                        }
                        else
                        {
                            /* Do Nothing */
                        }
                    
                        break;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
            }
            
            sKeyFlag++;
            sKeyBody += (sColLen + sColLenSize);
            sValColumn = sValColumn->mNext;
        }
        
        if( (sResult == DTL_COMPARISON_EQUAL) && (aRowRid != NULL) )
        {
            SMNMPB_GET_ROW_PID( sMedKey, &sRowRid.mPageId );
            SMNMPB_GET_ROW_SEQ( sMedKey, &sRowRid.mOffset );
                
            if( sRowRid.mPageId > aRowRid->mPageId )
            {
                sResult = DTL_COMPARISON_GREATER;
            }
            else if( sRowRid.mPageId < aRowRid->mPageId )
            {
                sResult = DTL_COMPARISON_LESS;
            }
            else
            {
                if( sRowRid.mOffset > aRowRid->mOffset )
                {
                    sResult = DTL_COMPARISON_GREATER;
                }
                else if( sRowRid.mOffset < aRowRid->mOffset )
                {
                    sResult = DTL_COMPARISON_LESS;
                }
                else
                {
                    sResult = DTL_COMPARISON_EQUAL;
                }
            }
        }
            
        if( sResult == DTL_COMPARISON_GREATER )
        {
            sRight = sMed - 1;
        }
        else if( sResult == DTL_COMPARISON_LESS )
        {
            sLeft = sMed + 1;
        }
        else
        {
            if( aRowRid != NULL )
            {
                /* DUP 발견 */
                *aFoundKey = STL_TRUE;
                break;
            }
            else
            {
                sRight = sMed - 1;
            }
        }
    }
    if( sResult == DTL_COMPARISON_LESS )
    {
        sMed++;
    }

    *aSlotSeq = sMed;

    return STL_SUCCESS;
}


/**
 * @brief 주어진 Key와 같은 key가 존재하는지 체크한다
 * @param[in] aStatement Statement
 * @param[in] aMiniTrans Latch를 잡고 logging을 하는데 사용될 mini transaction
 * @param[in] aSavepoint aMinnniTrans에서 현재 페이지를 잡기 직전의 시점을 가리키는 savepoint
 * @param[in] aTbsId Index가 존재하는 Tablespace의 ID
 * @param[in] aValColumns 찾을 key 값
 * @param[in] aBlockIdx aValColumns에서 사용할 block idx
 * @param[in] aRowRid 찾을 key가 가리키는 row의 위치
 * @param[in,out] aPageHandle 최종적으로 target으로 선정된 leaf page id
 * @param[out] aIsUniqueViolated unique check도중에 violation을 발견했는지 여부
 * @param[out] aIsMyStmtViolation unique violation의 원인이 자신의 Statement인지에 대한 여부
 * @param[out] aIsNeedRetry unique check도중에 다른 tx를 기다리는 상황이 발생하여 retry해야 하는지 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbCheckUniqueness( smlStatement      * aStatement,
                                 smxmTrans         * aMiniTrans,
                                 smxmSavepoint     * aSavepoint,
                                 smlTbsId            aTbsId,
                                 knlValueBlockList * aValColumns,
                                 stlInt32            aBlockIdx,
                                 smlRid            * aRowRid,
                                 smpHandle         * aPageHandle,
                                 stlBool           * aIsUniqueViolated,
                                 stlBool           * aIsMyStmtViolation,
                                 stlInt32          * aIsNeedRetry,
                                 smlEnv            * aEnv )
{
    stlChar           * sKeySlot;
    dtlCompareResult    sResult;
    stlUInt8          * sKeyColFlags;
    smlPid              sPrevPageId;
    smlPid              sNextPageId;
    stlInt16            sTotalKeyCount;
    stlInt16            sFirstSlotSeq;
    stlBool             sFoundKey = STL_FALSE;
    smxlTransId         sMyTransId = smxlGetTransId(smxmGetTransId(aMiniTrans));
    smxlTransId         sWaitTransId;
    smpHandle           sPageHandle = *aPageHandle;
    smlPid              sTargetPageId = smpGetPageId(aPageHandle);
    stlUInt64           sSmoNo = SMN_GET_PAGE_SMO_NO(aPageHandle);
    stlInt16            i;
    stlBool             sIsCommitted;
    smlPid              sRowPid;
    stlInt16            sRowOffset;
    knlValueBlockList * sCurCol;
    smxmSavepoint       sMinSavepoint;
    smxmSavepoint       sInsertSavepoint;
    smxmSavepoint     * sSavepointPtr;
    stlBool             sFoundInsertNode = STL_FALSE;
    stlUInt8            sInsertTcn;

    *aIsUniqueViolated = STL_FALSE;
    *aIsMyStmtViolation = STL_FALSE;
    *aIsNeedRetry = SMNMPB_RETRY_NONE;

    sSavepointPtr = aSavepoint;

    /* Insert/Delete 할 Key 컬럼이 모두 NULL value인지 체크 */
    sCurCol = aValColumns;
    while( sCurCol != NULL )
    {
        if( KNL_GET_BLOCK_DATA_LENGTH(sCurCol, aBlockIdx) != 0 )
        {
            break;
        }
        sCurCol = sCurCol->mNext;
    }
    
    STL_TRY_THROW( sCurCol != NULL, RAMP_RETURN );

    sKeyColFlags = SMNMPB_GET_KEY_COLUMN_FLAGS(&sPageHandle);

    /* 1. 처음 같은 value가 나타나는 page를 찾는다*/
    while( STL_TRUE )
    {
        if( smpGetFreeness(&sPageHandle) == SMP_FREENESS_FREE )
        {
            *aIsNeedRetry = SMNMPB_RETRY_CONCURRENCY;
            STL_THROW( RAMP_RETURN );
        }

        /* 이전 페이지를 미리 구해 둠 */
        sPrevPageId = SMNMPB_GET_PREV_PAGE(&sPageHandle);

        sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle );
        if( sTotalKeyCount > 0 )
        {
            sKeySlot = smpGetNthRowItem(&sPageHandle, 0);
            STL_TRY( smnmpbCompareKeys( sKeySlot,
                                        SMNMPB_GET_KEY_COLUMN_COUNT(&sPageHandle),
                                        aValColumns,
                                        aBlockIdx,
                                        NULL,
                                        sKeyColFlags,
                                        STL_TRUE,  /* aIsLeaf */
                                        &sResult,
                                        aEnv ) == STL_SUCCESS );

            if( sResult == DTL_COMPARISON_LESS )
            {
                /* 현재 노드의 첫 키가 비교하려는 값보다 작으므로 현재 노드부터 시작한다 */
                break;
            }
        }

        /* 이전 페이지가 없으면 현재 페이지를 풀지 않고 루프에서 나온다 */
        if( sPrevPageId == SMP_NULL_PID )
        {
            break;
        }

        /* 첫 key가 같은 값이므로 이전 페이지를 검사해야 한다 */
        STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                          sSavepointPtr, 
                                          aEnv ) == STL_SUCCESS );

        KNL_BREAKPOINT( KNL_BREAKPOINT_SMNMPBCHECKUNIQUENESS_BEFORE_FETCH_LEFT, KNL_ENV(aEnv) );

        STL_TRY( smpAcquire( aMiniTrans,
                             aTbsId,
                             sPrevPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        sKeyColFlags = SMNMPB_GET_KEY_COLUMN_FLAGS(&sPageHandle);
    }

    /* 2. 중간에 SMO가 발생했거나 같은 key가 없을 수도 있으므로
       다시 순방향으로 가며 처음 나타나는 page를 찾는다. */
    while( STL_TRUE )
    {
        /* 이후 페이지를 미리 구해 둠 */
        sNextPageId = SMNMPB_GET_NEXT_PAGE( &sPageHandle );

        sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle );
        if( (sTotalKeyCount > 0) &&
            (smpGetFreeness(&sPageHandle) != SMP_FREENESS_FREE) )
        {
            sKeySlot = smpGetNthRowItem(&sPageHandle, sTotalKeyCount - 1);
            STL_TRY( smnmpbCompareKeys( sKeySlot,
                                        SMNMPB_GET_KEY_COLUMN_COUNT(&sPageHandle),
                                        aValColumns,
                                        aBlockIdx,
                                        NULL,
                                        sKeyColFlags,
                                        STL_TRUE,  /* aIsLeaf */
                                        &sResult,
                                        aEnv ) == STL_SUCCESS );

            if( (sResult == DTL_COMPARISON_EQUAL) ||
                (sResult == DTL_COMPARISON_GREATER) )
            {
                /* 마지막 키가 비교하려는 키보다 크거나 같으므로 현재 노드부터 시작한다 */
                STL_TRY( smxmMarkSavepoint( aMiniTrans,
                                            &sMinSavepoint,
                                            aEnv ) == STL_SUCCESS );

                sSavepointPtr = &sMinSavepoint;
                break;
            }
        }

        if( sNextPageId == SMP_NULL_PID )
        {
            STL_TRY( smxmMarkSavepoint( aMiniTrans,
                                        &sMinSavepoint,
                                        aEnv ) == STL_SUCCESS );

            sSavepointPtr = &sMinSavepoint;
            break;
        }

        if( smpGetPageId( &sPageHandle ) == sTargetPageId )
        {
            /* latch가 풀린 사이에 SMO가 일어났으면 root부터 다시 retry 한다 */
            if( SMN_GET_PAGE_SMO_NO(&sPageHandle) != sSmoNo )
            {
                STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                                  aSavepoint,
                                                  aEnv )
                         == STL_SUCCESS );
                
                *aIsNeedRetry = SMNMPB_RETRY_CONCURRENCY;
                STL_THROW( RAMP_RETURN );
            }
            
            STL_TRY( smxmMarkSavepoint( aMiniTrans,
                                        &sInsertSavepoint,
                                        aEnv ) == STL_SUCCESS );
            sSavepointPtr = &sInsertSavepoint;
            *aPageHandle = sPageHandle;
            sFoundInsertNode = STL_TRUE;
        }
        
        /* 현재 페이지는 해당 Key가 없다. 다음 페이지로 이동 */

        KNL_BREAKPOINT( KNL_BREAKPOINT_SMNMPBCHECKUNIQUENESS_BEFORE_FETCH_RIGHT1, KNL_ENV(aEnv) );

        STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                          sSavepointPtr,
                                          aEnv )
                 == STL_SUCCESS );
                
        STL_TRY( smpAcquire( aMiniTrans,
                             aTbsId,
                             sNextPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        sKeyColFlags = SMNMPB_GET_KEY_COLUMN_FLAGS(&sPageHandle);
    }

    /* 3. Value만으로 첫번째 위치를 찾는다 */
    STL_TRY( smnmpbFindPosition( aValColumns,
                                 aBlockIdx,
                                 NULL,
                                 &sPageHandle,
                                 &sFirstSlotSeq,
                                 &sFoundKey,
                                 aEnv ) == STL_SUCCESS );

    /* 4. 순방향으로 진행하면서 같은 값들에 대해 validation 검사를 한다 */
    while( STL_TRUE )
    {
        if( smpGetPageId( &sPageHandle ) == sTargetPageId )
        {
            /* latch가 풀린 사이에 SMO가 일어났으면 root부터 다시 retry 한다 */
            if( SMN_GET_PAGE_SMO_NO(&sPageHandle) != sSmoNo )
            {
                STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                                  aSavepoint,
                                                  aEnv )
                         == STL_SUCCESS );
                
                *aIsNeedRetry = SMNMPB_RETRY_CONCURRENCY;
                STL_THROW( RAMP_RETURN );
            }
            
            STL_TRY( smxmMarkSavepoint( aMiniTrans,
                                        &sInsertSavepoint,
                                        aEnv ) == STL_SUCCESS );
            sSavepointPtr = &sInsertSavepoint;
            *aPageHandle = sPageHandle;
            sFoundInsertNode = STL_TRUE;
        }
        
        if(smpGetFreeness(&sPageHandle) != SMP_FREENESS_FREE )
        {
            for( i = sFirstSlotSeq; i < sTotalKeyCount; i++ )
            {
                sKeySlot = smpGetNthRowItem( &sPageHandle, i );
                STL_TRY( smnmpbCompareKeys( sKeySlot,
                                            SMNMPB_GET_KEY_COLUMN_COUNT(&sPageHandle),  
                                            aValColumns,
                                            aBlockIdx,
                                            NULL,
                                            sKeyColFlags,
                                            STL_TRUE,  /* aIsLeaf */
                                            &sResult,
                                            aEnv ) == STL_SUCCESS );

                if( sResult != DTL_COMPARISON_EQUAL )
                {
                    STL_THROW( RAMP_SKIP_CHECK_UNIQUENESS );
                }

                if( SMNMPB_IS_DELETED( sKeySlot ) != STL_TRUE )
                {
                    STL_TRY( smnmpbIsInsertCommitted( SMP_FRAME(&sPageHandle),
                                                      sKeySlot,
                                                      &sIsCommitted,
                                                      &sWaitTransId,
                                                      aEnv ) == STL_SUCCESS );

                    if( (sIsCommitted != STL_TRUE) &&  (sWaitTransId != sMyTransId) )
                    {
                        /* 내 Tx가 아닌 것에 의해 이미 같은 값으로 변경 중에 있음.
                           모든 latch를 다 풀고 wait 후 retry */
                        STL_TRY( smxmRollbackToSavepoint( aMiniTrans, aSavepoint, aEnv ) == STL_SUCCESS );
                        STL_TRY( smxlWaitTrans( sMyTransId,
                                                sWaitTransId,
                                                SMA_GET_LOCK_TIMEOUT(aStatement),
                                                aEnv ) == STL_SUCCESS );
                        *aIsNeedRetry = SMNMPB_RETRY_CONCURRENCY;
                        STL_THROW( RAMP_RETURN );
                    }

                    SMNMPB_GET_ROW_PID( sKeySlot, &sRowPid );
                    SMNMPB_GET_ROW_SEQ( sKeySlot, &sRowOffset );

                    if( (aRowRid->mPageId != sRowPid) || (aRowRid->mOffset != sRowOffset) )
                    {
                        *aIsUniqueViolated = STL_TRUE;

                        if( (sIsCommitted != STL_TRUE) &&  (sWaitTransId == sMyTransId) )
                        {
                            SMNMPB_GET_INS_TCN( sKeySlot, &sInsertTcn );

                            if( sInsertTcn != SMNMPB_INVALID_TCN )
                            {
                                if( sInsertTcn == SMA_GET_TCN( aStatement ) )
                                {
                                    *aIsMyStmtViolation = STL_TRUE;
                                }
                            }
                        }

                        STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                                          sSavepointPtr,
                                                          aEnv )
                                 == STL_SUCCESS );

                        if( sFoundInsertNode == STL_FALSE )
                        {
                            STL_TRY( smpAcquire( aMiniTrans,
                                                 aTbsId,
                                                 sTargetPageId,
                                                 KNL_LATCH_MODE_EXCLUSIVE,
                                                 &sPageHandle,
                                                 aEnv ) == STL_SUCCESS );

                            /* latch가 풀린 사이에 SMO가 일어났으면 root부터 다시 retry 한다 */
                            if( SMN_GET_PAGE_SMO_NO(&sPageHandle) != sSmoNo )
                            {
                                STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                                                  aSavepoint,
                                                                  aEnv )
                                         == STL_SUCCESS );

                                *aIsNeedRetry = SMNMPB_RETRY_CONCURRENCY;
                                STL_THROW( RAMP_RETURN );
                            }

                            *aPageHandle = sPageHandle;
                            sFoundInsertNode = STL_TRUE;
                        }

                        STL_THROW( RAMP_RETURN );
                    }
                }
                else
                {
                    STL_TRY( smnmpbIsDeleteCommitted( SMP_FRAME(&sPageHandle),
                                                      sKeySlot,
                                                      &sIsCommitted,
                                                      &sWaitTransId,
                                                      aEnv ) == STL_SUCCESS );

                    if( (sIsCommitted != STL_TRUE) &&  (sWaitTransId != sMyTransId) )
                    {
                        /* 내 Tx가 아닌 것에 의해 이미 같은 값으로 변경 중에 있음.
                           모든 latch를 다 풀고 wait 후 retry */
                        STL_TRY( smxmRollbackToSavepoint( aMiniTrans, aSavepoint, aEnv ) == STL_SUCCESS );
                        STL_TRY( smxlWaitTrans( sMyTransId,
                                                sWaitTransId,
                                                SMA_GET_LOCK_TIMEOUT(aStatement),
                                                aEnv ) == STL_SUCCESS );
                        *aIsNeedRetry = SMNMPB_RETRY_CONCURRENCY;
                        STL_THROW( RAMP_RETURN );
                    }
                    else
                    {
                        /* 내 Tx에 의해 이미 delete 되었거나, 다른 Tx에 의해 delete후 커밋 되어있음 */
                    }
                }
            } /* for */
        } /* if free node가 아니면 */
        
        sNextPageId = SMNMPB_GET_NEXT_PAGE( &sPageHandle );
        if( sNextPageId == SMP_NULL_PID )
        {
            STL_THROW( RAMP_SKIP_CHECK_UNIQUENESS );
        }

        /* 현재 페이지를 풀고 다음 페이지를 잡는다 */
        STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                          sSavepointPtr, 
                                          aEnv ) == STL_SUCCESS );
        
        KNL_BREAKPOINT( KNL_BREAKPOINT_SMNMPBCHECKUNIQUENESS_BEFORE_FETCH_RIGHT2, KNL_ENV(aEnv) );

        if( sNextPageId == sTargetPageId )
        {
            STL_TRY( smpAcquire( aMiniTrans,
                                 aTbsId,
                                 sNextPageId,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smpAcquire( aMiniTrans,
                                 aTbsId,
                                 sNextPageId,
                                 KNL_LATCH_MODE_SHARED,
                                 &sPageHandle,
                                 aEnv ) == STL_SUCCESS );
        }

        sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle );
        sFirstSlotSeq = 0;
    } /* while */


    STL_RAMP( RAMP_SKIP_CHECK_UNIQUENESS );

    STL_ASSERT( sSavepointPtr != aSavepoint );
    
    /* 현재 페이지를 풀고 다음 페이지를 잡는다 */
    STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                      sSavepointPtr, 
                                      aEnv ) == STL_SUCCESS );

    /**
     * Insertable Page를 찾지 못한 경우는 Node Free로 인식할수 있다.
     * 처음부터 다시 시작한다.
     */
    if( sFoundInsertNode == STL_FALSE )
    {
        STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                          aSavepoint,
                                          aEnv )
                 == STL_SUCCESS );
        
        *aIsNeedRetry = SMNMPB_RETRY_CONCURRENCY;
    }
    
    STL_RAMP( RAMP_RETURN );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smnmpbAllocMultiPage( smxmTrans   * aMiniTrans,
                                void        * aSegmentHandle,
                                smpPageType   aPageType ,
                                stlInt16      aPageCount,
                                smpHandle   * aPageArr,
                                stlInt32    * aNeedRetry,
                                smlEnv      * aEnv )
{
    smlPid      sPidArray[KNL_ENV_MAX_LATCH_DEPTH];
    smxmTrans   sMiniTrans;
    stlInt16    sState = 0;
    smpHandle   sPageHandle;
    stlInt16    i = 0;
    stlInt16    j = 0;

    STL_ASSERT( aPageCount < KNL_ENV_MAX_LATCH_DEPTH );

    /* 이 안에서 비정상 종료하면 page leak이 발생함 */
    for( i = 0; i < aPageCount; i++ )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            smxmGetTransId(aMiniTrans),
                            smsGetSegRid(aSegmentHandle),
                            smxmGetAttr( aMiniTrans ) |
                            SMXM_ATTR_NO_VALIDATE_PAGE,
                            aEnv ) == STL_SUCCESS );
        sState = 1;

        if( smsAllocPage( &sMiniTrans,
                          aSegmentHandle,
                          aPageType,
                          smnmpbIsAgable,
                          &sPageHandle,
                          aEnv )
            == STL_FAILURE )
        {
            STL_TRY( stlGetLastErrorCode( KNL_ERROR_STACK(aEnv) ) ==
                     SML_ERRCODE_NO_MORE_EXTENDIBLE_DATAFILE );

            *aNeedRetry = SMNMPB_RETRY_NO_SPACE;

            (void)smxmRollback( &sMiniTrans, aEnv );
            
            for( j = 0; j < i; j++ )
            {
                (void)smxmBegin( &sMiniTrans,
                                 smxmGetTransId(aMiniTrans),
                                 smsGetSegRid(aSegmentHandle),
                                 smxmGetAttr( aMiniTrans ) |
                                 SMXM_ATTR_NO_VALIDATE_PAGE,
                                 aEnv );

                (void)smpAcquire( &sMiniTrans,
                                  smsGetTbsId(aSegmentHandle),
                                  sPidArray[j],
                                  KNL_LATCH_MODE_EXCLUSIVE,
                                  &sPageHandle,
                                  aEnv );

                (void)smsUpdatePageStatus( &sMiniTrans,
                                           aSegmentHandle,
                                           &sPageHandle,
                                           SMP_FREENESS_FREE,
                                           0,  /* aScn */
                                           aEnv );

                (void)smxmCommit( &sMiniTrans, aEnv );
            }
            
            STL_THROW( RAMP_FINISH );
        }
        
        sPidArray[i] = smpGetPageId(&sPageHandle);

        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    }

    for( j = 0; j < aPageCount; j++ )
    {
        STL_TRY( smpAcquire( aMiniTrans,
                             smsGetTbsId(aSegmentHandle),
                             sPidArray[j],
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &aPageArr[j],
                             aEnv ) == STL_SUCCESS );
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmRollback( &sMiniTrans, aEnv );
    }

    for( j = 0; j < i; j++ )
    {
        (void)smxmBegin( &sMiniTrans,
                         smxmGetTransId(aMiniTrans),
                         smsGetSegRid(aSegmentHandle),
                         smxmGetAttr( aMiniTrans ) |
                         SMXM_ATTR_NO_VALIDATE_PAGE,
                         aEnv );

        (void)smpAcquire( &sMiniTrans,
                          smsGetTbsId(aSegmentHandle),
                          sPidArray[j],
                          KNL_LATCH_MODE_EXCLUSIVE,
                          &sPageHandle,
                          aEnv );

        (void)smsUpdatePageStatus( &sMiniTrans,
                                   aSegmentHandle,
                                   &sPageHandle,
                                   SMP_FREENESS_FREE,
                                   0,  /* aScn */
                                   aEnv );

        (void)smxmCommit( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}


/**
 * @brief SMO에 의해 하위로 부터 올라온 Key를 고려하여 현재 Root를 분할하고 새로운 Root를 만든다.
 * @param[in] aMiniTrans SMO시 Latch를 잡고 logging을 하는데 사용될 mini transaction
 * @param[in] aRelationHandle Index의 relation handle
 * @param[in] aOldRootPage Split될 현재 root node
 * @param[in] aLeftChildPid 왼쪽 child node page id
 * @param[in] aPropKeyBody 하위 노드에서 올라온 key의 body
 * @param[in] aPropKeyBodySize 하위 노드에서 올라온 key의 body size
 * @param[in] aPropKeyRowRid 하위 노드에서 올라온 key가 가리키는 row의 위치
 * @param[out] aTargetPage Propagate되어 올라온 key가 삽입될 target page handle
 * @param[in,out] aTargetKeySeq Old root node에 삽입될 key의 seq 위치
 * @param[out] aOrgKeyPage Split key를 저장하고 있는 page
 * @param[out] aOrgKeySeq Split key가 저장된 slot seq
 * @param[out] aPageArr 새로 alloc될 페이지들의 배열
 * @param[out] aMirrorPageHandle Mirrored Page Handle
 * @param[out] aSmoNo 이전 SMO 연산에 관련된 page들에 세팅될 SmoNo
 * @param[out] aNeedRetry Retry 시도 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbMakeNewRootNode( smxmTrans * aMiniTrans,
                                 void      * aRelationHandle,
                                 smpHandle * aOldRootPage,
                                 smlPid      aLeftChildPid,
                                 stlChar   * aPropKeyBody,
                                 stlUInt16   aPropKeyBodySize,
                                 smlRid    * aPropKeyRowRid,
                                 smpHandle * aTargetPage,
                                 stlInt16  * aTargetKeySeq,
                                 smpHandle * aOrgKeyPage,
                                 stlInt16  * aOrgKeySeq,
                                 smpHandle * aPageArr,
                                 smpHandle * aMirrorPageHandle,
                                 stlUInt64 * aSmoNo,
                                 stlInt32  * aNeedRetry,
                                 smlEnv    * aEnv )
{
    smnmpbLogicalHdr     sLogicalHdr;
    smpHandle            sNewRootPage;
    smlPid               sNewRootPid;
    stlUInt16            sCurLevel;
    stlInt16             sSplitPageCount;
    stlInt16             sAllocPageCount;
    stlUInt8           * sKeyColTypes;
    stlUInt8           * sKeyColFlags;
    stlUInt16            sKeyColCount;
    smnmpbPropKeyStr     sSplitKeyStr[KNL_ENV_MAX_LATCH_DEPTH];
    stlInt16             sPropKeyNodeSeq;
    stlBool              sUseMirrorRoot;
    stlInt16             i;

    sCurLevel    = SMNMPB_GET_LEVEL(aOldRootPage) + 1;
    sKeyColTypes = SMNMPB_GET_KEY_COLUMN_TYPES(aOldRootPage);
    sKeyColFlags = SMNMPB_GET_KEY_COLUMN_FLAGS(aOldRootPage);
    sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(aOldRootPage);

    /**
     * 시스템이 정한 최대 인덱스 높이를 넘을수 없다.
     */
    STL_TRY_THROW( sCurLevel < SMNMPB_MAX_INDEX_DEPTH,
                   RAMP_ERR_MAX_INDEX_DEPTH );
    STL_TRY_THROW( aPropKeyBodySize <= SMN_MAX_KEY_BODY_SIZE,
                   RAMP_ERR_TOO_LARGE_KEY);

    /* split position, split에 필요한 page 수를 구한다. */
    STL_TRY( smnmpbCalculateSplitPositions( aMiniTrans,
                                            aRelationHandle,
                                            aOldRootPage,
                                            sCurLevel,
                                            aPropKeyBody,
                                            aPropKeyBodySize,
                                            aPropKeyRowRid,
                                            *aTargetKeySeq,
                                            &sSplitPageCount,
                                            sSplitKeyStr,
                                            &sPropKeyNodeSeq,
                                            &sUseMirrorRoot,
                                            aEnv ) == STL_SUCCESS );

    /* 새 root node 생성 시 필요한 page 수
     * : current level에서 split할 페이지 수 + new root node
     *   + 하위 level을 위한 split page 수 + mirror root node(optional) */
    sAllocPageCount = (sSplitPageCount - 1) + sCurLevel +
        ((sUseMirrorRoot == STL_TRUE)? 1: 0);

    /* SMO의 최상위. split 되지 않음. 하위 노드들을 위해 Page들을 alloc해 줌 */
    STL_TRY( smnmpbAllocMultiPage( aMiniTrans,
                                   SME_GET_SEGMENT_HANDLE(aRelationHandle),
                                   SMP_PAGE_TYPE_INDEX_DATA,
                                   sAllocPageCount,
                                   aPageArr,
                                   aNeedRetry,
                                   aEnv ) == STL_SUCCESS );

    STL_TRY_THROW( *aNeedRetry == SMNMPB_RETRY_NONE, RAMP_FINISH );

    if( sUseMirrorRoot == STL_TRUE )
    {
        *aMirrorPageHandle = aPageArr[sAllocPageCount - 1];
        sAllocPageCount--;
    }

    /* SMO Number를 증가시키고 따온다 */
    *aSmoNo = SMN_GET_NEXT_INDEX_SMO_NO(aRelationHandle);

    /* Initialize the new root node */
    sNewRootPage = aPageArr[sAllocPageCount - 1];
    SMN_SET_PAGE_SMO_NO( &sNewRootPage, *aSmoNo );

    stlMemset( &sLogicalHdr, 0x00, STL_SIZEOF(smnmpbLogicalHdr) );
    sLogicalHdr.mPrevPageId = SMP_NULL_PID;
    sLogicalHdr.mNextPageId = SMP_NULL_PID;
    sLogicalHdr.mNextFreePageId = SMP_NULL_PID;
    sLogicalHdr.mFlag = SMNMPB_NODE_FLAG_LAST_SIBLING; /* root는 무조건 last sibling */
    sLogicalHdr.mLevel      = sCurLevel;
    sLogicalHdr.mKeyColCount = sKeyColCount; 
    stlMemcpy( &sLogicalHdr.mKeyColTypes, sKeyColTypes, sKeyColCount);
    stlMemcpy( &sLogicalHdr.mKeyColFlags, sKeyColFlags, sKeyColCount );

    STL_TRY( smpInitBody( &sNewRootPage,
                          SMP_ORDERED_SLOT_TYPE,
                          STL_SIZEOF(smnmpbLogicalHdr),
                          (void*)&sLogicalHdr,
                          0,
                          0,
                          STL_FALSE,
                          aMiniTrans,
                          aEnv) == STL_SUCCESS );

    /* Old root node를 sSiblingPageCount만큼 split한다. */
    STL_TRY( smnmpbSplitRootNode( aMiniTrans,
                                  aRelationHandle,
                                  &sNewRootPage,
                                  aOldRootPage,
                                  aPropKeyRowRid,
                                  aPropKeyBody,
                                  aPropKeyBodySize,
                                  aLeftChildPid,
                                  sSplitPageCount,
                                  sSplitKeyStr,
                                  sPropKeyNodeSeq,
                                  (smpHandle *)&(aPageArr[sCurLevel - 1]),
                                  aTargetPage,
                                  aTargetKeySeq,
                                  aOrgKeyPage,
                                  aOrgKeySeq,
                                  aEnv ) == STL_SUCCESS );

    for( i = 0; i < (sSplitPageCount - 1); i++ )
    {
        SMN_SET_PAGE_SMO_NO( &aPageArr[i + sCurLevel - 1], *aSmoNo );
    }

    SMN_SET_PAGE_SMO_NO( aOldRootPage, *aSmoNo );

    sNewRootPid = smpGetPageId( &sNewRootPage );

    /* Set Root Pid at Segment.  마지막에 해야함  */
    STL_TRY( smnSetRootPageId( aMiniTrans,
                               aRelationHandle,
                               sNewRootPid,
                               aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MAX_INDEX_DEPTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXCEEDED_MAX_INDEX_DEPTH,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      sCurLevel );
    }
 
    STL_CATCH( RAMP_ERR_TOO_LARGE_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXCEEDED_MAX_KEY_SIZE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      SMN_MAX_KEY_BODY_SIZE );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief SMO에 의해 하위로 부터 올라온 Key를 상위 internal node에 insert한다.
 * @param[in] aMiniTrans SMO시 Latch를 잡고 logging을 하는데 사용될 mini transaction
 * @param[in] aRelationHandle Index의 relation handle
 * @param[in] aPathStack 탐색시 내려온 path를 저장한 stack
 * @param[in] aCurDepth path stack 중 현재 의 Depth 값
 * @param[in] aOrgChildPid 기존의 child node page id
 * @param[in] aPropKeyBody 하위 노드에서 올라온 key의 body
 * @param[in] aPropKeyBodySize 하위 노드에서 올라온 key의 body size
 * @param[in] aPropKeyRowRid 하위 노드에서 올라온 key가 가리키는 row의 위치
 * @param[out] aPageArr 새로 alloc될 페이지들의 배열
 * @param[out] aMirrorPageHandle Mirrored Page Handle
 * @param[out] aNeedRetry Smo 도중에 Root 생성시 다른 Smo operation과 충돌나서 다시해야 함
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbPropagateInternalKey( smxmTrans       * aMiniTrans,
                                      void            * aRelationHandle,
                                      smnmpbPathStack * aPathStack,
                                      stlUInt16         aCurDepth,
                                      smlPid            aOrgChildPid,
                                      stlChar         * aPropKeyBody,
                                      stlUInt16         aPropKeyBodySize,
                                      smlRid          * aPropKeyRowRid,
                                      smpHandle       * aPageArr,
                                      smpHandle       * aMirrorPageHandle,
                                      stlInt32        * aNeedRetry,
                                      smlEnv          * aEnv )
{
    smpHandle            sPageHandle;
    smpHandle            sNewPageHandle;
    smlPid               sCurPid;
    smlPid               sNewPageId;
    smlPid               sNextPid;
    stlInt16             sCurSeq;
    stlUInt64            sSmoNo;
    smlPid               sChildPid;
    stlUInt16            sTotalKeyCount;
    stlUInt16            sKeyColCount;
    smpRowItemIterator   sIterator;
    stlBool              sIsFirstTry = STL_TRUE;
    stlChar            * sPropKeySlot;
    stlChar            * sPropKeyBody = NULL;
    stlUInt16            sPropKeyBodySize = 0;
    smlRid               sPropKeyRowRid;
    stlUInt16            sTotalSize;
    stlUInt16            sLeftKeyCount;
    stlChar              sLogBuf[SMP_PAGE_SIZE];
    stlChar            * sLogPtr;
    smpHandle            sOrgKeyPage;
    smpHandle            sNewKeyPage;
    stlInt16             sOrgKeySeq;
    stlInt16             sNewKeySeq;
    stlChar            * sOrgKeySlot;
    stlChar            * sNewKeySlot;
    smnmpbLogicalHdr     sLogicalHdr;
    stlUInt16            sKeySize;
    stlUInt16            sLastKeySize;
    stlUInt16            sRtsCount;   /* logging용 dummy 변수 */
    stlChar            * sDataPtr;
    stlUInt16            sDataSize;
    stlChar            * sLastKeySlot;
    smlPid               sRightChildPid;
    stlInt16             i;
    stlUInt16            sOrgDataSize = 0;
    stlUInt64            sIndexSmoNo;
    stlBool              sInsertNewKeyLeft = STL_TRUE;
    stlBool              sIsLastSibling;
    smxmSavepoint        sSavepoint;

    STL_TRY_THROW( aPropKeyBodySize <= SMN_MAX_KEY_BODY_SIZE,
                   RAMP_ERR_TOO_LARGE_KEY);

    /* 1. target page를 잡는다 */
    sCurPid = aPathStack->mStack[aCurDepth - 1].mPid;
    sCurSeq = aPathStack->mStack[aCurDepth - 1].mSeq;
    sSmoNo  = aPathStack->mStack[aCurDepth - 1].mSmoNo;
    
    while( STL_TRUE )
    {
        STL_TRY( smxmMarkSavepoint( aMiniTrans,
                                    &sSavepoint,
                                    aEnv ) == STL_SUCCESS );
        
        STL_TRY( smpAcquire( aMiniTrans,
                             smsGetTbsId(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                             sCurPid,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );

        STL_DASSERT( smpGetFreeness(&sPageHandle) != SMP_FREENESS_FREE );
        STL_TRY( smnmpbValidateIndexPage( &sPageHandle,
                                          aEnv )
                 == STL_SUCCESS );
        
        sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle );
        sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(&sPageHandle);

        if( sIsFirstTry == STL_TRUE )
        {
            if( SMN_GET_PAGE_SMO_NO(&sPageHandle) == sSmoNo )
            {
                /* 내려올때 지났던 바로 그 slot이 아직 valid한지 체크한다 */
                sOrgKeySlot = smpGetNthRowItem( &sPageHandle, sCurSeq );
                STL_WRITE_INT32( &sChildPid, sOrgKeySlot );
                STL_ASSERT( sChildPid == aOrgChildPid );
                
                STL_THROW( RAMP_DO_INSERT );
            }
        }

        /**
         * 최상위 ROOT의 Split은 재시도 한다.
         */
        if( aCurDepth == 1 )
        {
            *aNeedRetry = SMNMPB_RETRY_CONCURRENCY;
            STL_THROW( RAMP_RETRY_FROM_ROOT );
        }
        
        /* 이 노드안에서 해당 Org PID를 가지는 key가 있는지 체크한다 */
        sNextPid = SMNMPB_GET_NEXT_PAGE(&sPageHandle);
        SMP_GET_FIRST_ROWITEM( SMP_FRAME(&sPageHandle), &sIterator, sOrgKeySlot );
        sCurSeq = 0;
        while( sOrgKeySlot != NULL )
        {
            STL_WRITE_INT32( &sChildPid, sOrgKeySlot );
            if( sChildPid == aOrgChildPid )
            {
                /* target key를 찾았으면 이를 이용한다 */
                aPathStack->mStack[aCurDepth - 1].mSeq = sCurSeq;
                aPathStack->mStack[aCurDepth - 1].mSmoNo = SMN_GET_PAGE_SMO_NO( &sPageHandle );
                STL_THROW( RAMP_DO_INSERT );
            }
            /* index에는 free offset slot이 중간에 없으므로 그냥 ++ 해도 된다 */
            SMP_GET_NEXT_ROWITEM( &sIterator, sOrgKeySlot );
            sCurSeq++;
        }

        /* 현재 노드에는 Org PID를 갖는 key가 없으므로 다음 노드로 이동한다 */
        sIsFirstTry = STL_FALSE;
        aPathStack->mStack[aCurDepth - 1].mPid = sNextPid;
        aPathStack->mStack[aCurDepth - 1].mSeq = 0;
        sCurPid = aPathStack->mStack[aCurDepth - 1].mPid;
        /* 다음 노드로 이동해도 SMO NO는 그대로 유지함 */
        STL_ASSERT( sNextPid != SMP_NULL_PID );
        
        STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                          &sSavepoint, 
                                          aEnv )
                 == STL_SUCCESS );
    }

    STL_RAMP( RAMP_DO_INSERT );

    sNextPid = SMNMPB_GET_NEXT_PAGE(&sPageHandle);

    sKeySize = aPropKeyBodySize + SMNMPB_INTL_SLOT_HDR_SIZE;
    
    /* 기존 key 앞에 새 key를 넣는다 */
    STL_TRY( smpAllocNthSlot( &sPageHandle,
                              sCurSeq,
                              sKeySize,
                              STL_TRUE,
                              &sNewKeySlot ) == STL_SUCCESS );

    /* alloc slot이 실패하면 compact해서 공간 확보가 가능한지 체크 */
    if( (sNewKeySlot == NULL) &&
        (smpGetTotalFreeSpace(SMP_FRAME(&sPageHandle)) >= sKeySize + STL_SIZEOF(smpOffsetSlot)) )
    {
        /* compact 후 재시도 */
        STL_TRY( smnmpbCompactPage( aRelationHandle,
                                    &sPageHandle,
                                    STL_INT16_MAX,
                                    aEnv ) == STL_SUCCESS );
            
        STL_TRY( smpAllocNthSlot( &sPageHandle,
                                  sCurSeq,
                                  sKeySize,
                                  STL_TRUE,
                                  &sNewKeySlot ) == STL_SUCCESS );

        /* internal node에서는 signpost의 내용이 정확하므로 compact 후 재 할당할 시 반드시 성공해야함 */
        KNL_ASSERT( (sNewKeySlot != NULL),
                    aEnv,
                    ("TBS_ID(%d), PAGE_ID(%d), RECL(%d), SLOT_SIZE(%d), PAGE_BODY : %s",
                    smpGetTbsId(&sPageHandle),
                    smpGetPageId(&sPageHandle),
                    smpGetReclaimedSpace(SMP_FRAME(&sPageHandle)),
                    sKeySize,
                    knlDumpBinaryForAssert( SMP_FRAME(&sPageHandle),
                                            SMP_PAGE_SIZE,
                                            &aEnv->mLogHeapMem,
                                            KNL_ENV(aEnv))) );
    }

    /* 2. 현재 internal node가 full이므로 split한다 */

    if( sNewKeySlot == NULL )
    {
        if( aCurDepth == 1 )
        {
            STL_DASSERT( SMN_GET_ROOT_PAGE_ID(aRelationHandle) == smpGetPageId(&sPageHandle) );

            /* 2.1 현재 internal node가 root이기 때문에 split 후 새로운 root를 생성한다 */
            STL_TRY( smnmpbMakeNewRootNode( aMiniTrans,
                                            aRelationHandle,
                                            &sPageHandle,
                                            aOrgChildPid,
                                            aPropKeyBody,
                                            aPropKeyBodySize,
                                            aPropKeyRowRid,
                                            &sNewKeyPage,
                                            &sCurSeq,
                                            &sOrgKeyPage,
                                            &sOrgKeySeq,
                                            aPageArr,
                                            aMirrorPageHandle,
                                            &sSmoNo,
                                            aNeedRetry,
                                            aEnv ) == STL_SUCCESS );

            STL_TRY_THROW( *aNeedRetry == SMNMPB_RETRY_NONE, RAMP_RETRY_FROM_ROOT );
            
            for( i = 0; i < aPathStack->mDepth; i++ )
            {
                aPathStack->mStack[i].mSmoNo = sSmoNo;
            }

            sNewKeySeq = sCurSeq;
        }
        else
        {
            /* 2.2 internal node split을 위해 parent node로 split key를 propagate한다 */
            /* 2.2.1 split할 노드에 들어있는 data들의 총 크기를 구한다 */
            sOrgDataSize = ( SMP_PAGE_SIZE
                             - STL_SIZEOF(smpPhyHdr)
                             - STL_SIZEOF(smpOrderedSlotSignpost)
                             - STL_SIZEOF(smnmpbLogicalHdr)
                             - STL_SIZEOF(smpOffsetTableHeader)
                             - STL_SIZEOF(smpPhyTail) );

            SMP_GET_FIRST_ROWITEM( SMP_FRAME(&sPageHandle), &sIterator, sPropKeySlot );
            sLeftKeyCount = 0;
            sTotalSize = 0;
            /* 2.2.2 key size를 기준으로 split 지점을 계산하고 상위로 올려질 key를 찾는다 */
            while( sPropKeySlot != NULL )
            {
                if( sLeftKeyCount == sCurSeq )
                {
                    sTotalSize += sKeySize;
                    if( sTotalSize > (sOrgDataSize * 0.5) )
                    {
                        sPropKeyBody = aPropKeyBody;
                        sPropKeyBodySize = aPropKeyBodySize;
                        sPropKeyRowRid = *aPropKeyRowRid;
                        break;
                    }
                }
                if( (sLeftKeyCount == sTotalKeyCount - 1) &&
                    (SMNMPB_IS_LAST_SIBLING(&sPageHandle) == STL_TRUE) ) /* Max Key */
                {
                    /* max key 까지 합쳐도 페이지 반 약간 넘은 -> split 하러 들어온 것이 잘못되었음 */
                    STL_ASSERT( STL_FALSE );
                }
                else
                {
                    sLeftKeyCount++;
                    sTotalSize += smnmpbGetIntlKeySize( sPropKeySlot, sKeyColCount );
                    if( sTotalSize > (sOrgDataSize * 0.5) )
                    {
                        sPropKeyBody = sPropKeySlot + SMNMPB_INTL_SLOT_HDR_SIZE;
                        sPropKeyBodySize = smnmpbGetIntlKeySize(sPropKeySlot, sKeyColCount) 
                            - SMNMPB_INTL_SLOT_HDR_SIZE;
                        SMNMPB_INTL_SLOT_GET_ROW_PID( sPropKeySlot, &sPropKeyRowRid.mPageId );
                        SMNMPB_INTL_SLOT_GET_ROW_OFFSET( sPropKeySlot, &sPropKeyRowRid.mOffset );
                        if( sLeftKeyCount <= sCurSeq )
                        {
                            sInsertNewKeyLeft = STL_FALSE;
                        }
                        break;
                    }
                }
                SMP_GET_NEXT_ROWITEM( &sIterator, sPropKeySlot );
            }

            /* 2.2.3 parent node로 split key를 propagate한다 */
            STL_TRY( smnmpbPropagateInternalKey( aMiniTrans,
                                                 aRelationHandle,
                                                 aPathStack,
                                                 aCurDepth - 1,
                                                 smpGetPageId(&sPageHandle),
                                                 sPropKeyBody,
                                                 sPropKeyBodySize,
                                                 &sPropKeyRowRid,
                                                 aPageArr,
                                                 aMirrorPageHandle,
                                                 aNeedRetry,
                                                 aEnv ) == STL_SUCCESS );
            STL_TRY_THROW( *aNeedRetry == SMNMPB_RETRY_NONE, RAMP_RETRY_FROM_ROOT );

            sIsLastSibling = SMNMPB_IS_LAST_SIBLING(&sPageHandle);

            sNewPageHandle = aPageArr[SMNMPB_GET_LEVEL(&sPageHandle)];
            sNewPageId = smpGetPageId(&sNewPageHandle);
            SMN_SET_PAGE_SMO_NO( &sPageHandle, aPathStack->mStack[aCurDepth-1].mSmoNo );
            SMN_SET_PAGE_SMO_NO( &aPageArr[SMNMPB_GET_LEVEL(&sPageHandle)],
                                 aPathStack->mStack[aCurDepth-1].mSmoNo );
            
            stlMemset( &sLogicalHdr, 0x00, STL_SIZEOF(smnmpbLogicalHdr) );
            sLogicalHdr.mPrevPageId = SMP_NULL_PID; /* internal node의 prev pid는 사용안함 */
            sLogicalHdr.mNextPageId = SMNMPB_GET_NEXT_PAGE(&sPageHandle);
            sLogicalHdr.mNextFreePageId = SMP_NULL_PID;
            sLogicalHdr.mFlag = (sIsLastSibling == STL_TRUE) ?
                SMNMPB_NODE_FLAG_LAST_SIBLING : SMNMPB_NODE_FLAG_INIT ;
            sLogicalHdr.mLevel    = SMNMPB_GET_LEVEL(&sPageHandle);
            sLogicalHdr.mKeyColCount = sKeyColCount;
            stlMemcpy( &sLogicalHdr.mKeyColTypes,
                       SMNMPB_GET_KEY_COLUMN_TYPES(&sPageHandle),
                       sKeyColCount);
            stlMemcpy( &sLogicalHdr.mKeyColFlags,
                       SMNMPB_GET_KEY_COLUMN_FLAGS(&sPageHandle),
                       sKeyColCount );

            STL_TRY( smpInitBody( &sNewPageHandle,
                                  SMP_ORDERED_SLOT_TYPE,
                                  STL_SIZEOF(smnmpbLogicalHdr),
                                  (void*)&sLogicalHdr,
                                  0,
                                  0,
                                  STL_FALSE,
                                  aMiniTrans,
                                  aEnv) == STL_SUCCESS );

            /* 2.2.3 internal node를 split 한다 */
            /* 2.2.3.1 new page */
            STL_TRY( smnmpbCopyIntlKeys( &sPageHandle,
                                         &sNewPageHandle,
                                         sLeftKeyCount,
                                         SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle ) - 1,
                                         aEnv ) == STL_SUCCESS );
            if( sInsertNewKeyLeft !=  STL_TRUE )
            {
                sCurSeq -= sLeftKeyCount;
            }
            /* LOG CONTENTS : RtsCount(0) + KeyCount + KeyData */
            sLogPtr = sLogBuf;
            sRtsCount = 0;
            stlMemcpy( sLogPtr, &sRtsCount, STL_SIZEOF(stlUInt16) );
            sLogPtr += STL_SIZEOF(stlUInt16);
            sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sNewPageHandle );
            stlMemcpy( sLogPtr, &sTotalKeyCount, STL_SIZEOF(stlUInt16) );
            sLogPtr += STL_SIZEOF(stlUInt16);
            smpGetPageDataSection( &sNewPageHandle, &sDataPtr, &sDataSize );
            stlMemcpy( sLogPtr, sDataPtr, sDataSize );
            sLogPtr += sDataSize;
            STL_TRY( smxmWriteLog( aMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMR_LOG_MEMORY_BTREE_SPLIT_NEW_PAGE,
                                   SMR_REDO_TARGET_PAGE,
                                   sLogBuf,
                                   sLogPtr - sLogBuf,
                                   sNewPageHandle.mPch->mTbsId,
                                   sNewPageId,
                                   0,
                                   SMXM_LOGGING_REDO,
                                   aEnv ) == STL_SUCCESS );


            /* 2.2.3.2 old page */
            SMNMPB_GET_LOGICAL_HEADER(&sPageHandle)->mNextPageId = sNewPageId;
            STL_TRY( smxmWriteLog( aMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMR_LOG_MEMORY_BTREE_SET_NEXT_PAGE_ID,
                                   SMR_REDO_TARGET_PAGE,
                                   (void*)&sNewPageId,
                                   STL_SIZEOF(smlPid),
                                   sPageHandle.mPch->mTbsId,
                                   smpGetPageId(&sPageHandle),
                                   0,
                                   SMXM_LOGGING_REDO,
                                   aEnv ) == STL_SUCCESS );

            STL_TRY( smnmpbTruncateNode( aRelationHandle,
                                         &sPageHandle,
                                         sLeftKeyCount,
                                         aEnv ) == STL_SUCCESS );

            /* LOG CONTENTS : LeftKeyCount + LastKeyData  */
            sLogPtr = sLogBuf;
            stlMemcpy( sLogPtr, &sLeftKeyCount, STL_SIZEOF(stlUInt16) );
            sLogPtr += STL_SIZEOF(stlUInt16);
            sLastKeySlot = smpGetNthRowItem( &sPageHandle,
                                             SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle) - 1 );
            sLastKeySize = smnmpbGetIntlKeySize(sLastKeySlot, sKeyColCount);
            stlMemcpy( sLogPtr, sLastKeySlot, sLastKeySize );
            sLogPtr += sLastKeySize;
            STL_TRY( smxmWriteLog( aMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMR_LOG_MEMORY_BTREE_SPLIT_OLD_PAGE,
                                   SMR_REDO_TARGET_PAGE,
                                   sLogBuf,
                                   sLogPtr - sLogBuf,
                                   sPageHandle.mPch->mTbsId,
                                   smpGetPageId(&sPageHandle),
                                   0,
                                   SMXM_LOGGING_REDO,
                                   aEnv ) == STL_SUCCESS );

            if( sIsLastSibling == STL_TRUE )
            {
                SMNMPB_GET_LOGICAL_HEADER(&sPageHandle)->mFlag &= ~SMNMPB_NODE_FLAG_LAST_SIBLING_MASK;
                STL_TRY( smxmWriteLog( aMiniTrans,
                                       SMG_COMPONENT_INDEX,
                                       SMR_LOG_MEMORY_BTREE_SET_NODE_FLAG,
                                       SMR_REDO_TARGET_PAGE,
                                       (void*)&(SMNMPB_GET_LOGICAL_HEADER(&sPageHandle)->mFlag),
                                       STL_SIZEOF(stlUInt8),
                                       sPageHandle.mPch->mTbsId,
                                       smpGetPageId(&sPageHandle),
                                       0,
                                       SMXM_LOGGING_REDO,
                                       aEnv ) == STL_SUCCESS );
            }

            if( sInsertNewKeyLeft == STL_TRUE )
            {
                if( sCurSeq < sLeftKeyCount )
                {
                    /* new key와 org key 모두 left */
                    sOrgKeyPage = sPageHandle;
                    sNewKeyPage = sPageHandle;
                    sOrgKeySeq = sCurSeq + 1;
                    sNewKeySeq = sCurSeq;
                }
                else
                {
                    /* new key는 left, org key는 right */
                    STL_ASSERT( sPropKeyBody == aPropKeyBody );
                    sOrgKeyPage = sNewPageHandle;
                    sNewKeyPage = sPageHandle;
                    sOrgKeySeq = 0;
                    sNewKeySeq = sCurSeq;
                }
            }
            else
            {
                /* new key와 org key 모두 right */
                sOrgKeyPage = sNewPageHandle;
                sNewKeyPage = sNewPageHandle;
                sOrgKeySeq = sCurSeq + 1;
                sNewKeySeq = sCurSeq;
            }
        }
    }
    else
    {
        sOrgKeyPage = sPageHandle;
        sOrgKeySeq = sCurSeq + 1;
        sNewKeyPage = sPageHandle;
        sNewKeySeq = sCurSeq;

        sIndexSmoNo = SMN_GET_NEXT_INDEX_SMO_NO(aRelationHandle);
        for( i = 0; i < aPathStack->mDepth; i++ )
        {
            aPathStack->mStack[i].mSmoNo = sIndexSmoNo;
        }

        /* SMO의 최상위. split 되지 않음. 하위 노드들을 위해 Page들을 alloc해 줌 */

        if( (aCurDepth == 1) && (SMN_GET_MIRROR_ROOT_PID(aRelationHandle) != SMP_NULL_PID) )
        {
            STL_TRY( smnmpbAllocMultiPage( aMiniTrans,
                                           SME_GET_SEGMENT_HANDLE(aRelationHandle),
                                           SMP_PAGE_TYPE_INDEX_DATA,
                                           SMNMPB_GET_LEVEL(&sPageHandle) + 1,
                                           aPageArr,
                                           aNeedRetry,
                                           aEnv ) == STL_SUCCESS );

            STL_TRY_THROW( *aNeedRetry == SMNMPB_RETRY_NONE, RAMP_RETRY_FROM_ROOT );
            
            *aMirrorPageHandle = aPageArr[SMNMPB_GET_LEVEL(&sPageHandle)];
        }
        else
        {
            STL_TRY( smnmpbAllocMultiPage( aMiniTrans,
                                           SME_GET_SEGMENT_HANDLE(aRelationHandle),
                                           SMP_PAGE_TYPE_INDEX_DATA,
                                           SMNMPB_GET_LEVEL(&sPageHandle),
                                           aPageArr,
                                           aNeedRetry,
                                           aEnv ) == STL_SUCCESS );
            
            STL_TRY_THROW( *aNeedRetry == SMNMPB_RETRY_NONE, RAMP_RETRY_FROM_ROOT );
        }

        SMN_SET_PAGE_SMO_NO( &sPageHandle, sIndexSmoNo );
    }

    /* Old root node를 split한 후 propagate된 key를 삽입할 key slot을
     * 할당받고 child pid 갱신을 위해 기존 key slot을 가져온다 */
    STL_TRY( smpAllocNthSlot( &sNewKeyPage,
                              sNewKeySeq,
                              sKeySize,
                              STL_FALSE,
                              &sNewKeySlot ) == STL_SUCCESS );

    STL_ASSERT( sNewKeySlot != NULL );
    sOrgKeySlot = smpGetNthRowItem( &sOrgKeyPage, sOrgKeySeq );

    sRightChildPid = smpGetPageId(&aPageArr[SMNMPB_GET_LEVEL(&sPageHandle) - 1]);

    /* 3. 기존 key가 가리키는 child pid를 올라온 prop pid로 변경하고
     *    기존 child pid와 prop key body를 가지는 새 key를 기존 key 바로 앞에 insert한다 */
    SMNMPB_INTL_SLOT_GET_CHILD_PID( sOrgKeySlot, &sChildPid);
    STL_ASSERT( aOrgChildPid == sChildPid );

    SMNMPB_INTL_SLOT_SET_CHILD_PID( sOrgKeySlot, &sRightChildPid );
    SMNMPB_INTL_SLOT_SET_CHILD_PID( sNewKeySlot, &aOrgChildPid );
    SMNMPB_INTL_SLOT_SET_ROW_PID( sNewKeySlot, &aPropKeyRowRid->mPageId );
    SMNMPB_INTL_SLOT_SET_ROW_OFFSET( sNewKeySlot, &aPropKeyRowRid->mOffset );
    stlMemcpy( sNewKeySlot + SMNMPB_INTL_SLOT_HDR_SIZE, aPropKeyBody, aPropKeyBodySize );
    STL_ASSERT( aOrgChildPid != SMP_NULL_PID );

    /* 4. mini-transaction을 이용하여, 로그를 쓰되 latch들은 그대로 두고 restart한다(TODO) */
    /* write REDO log */
    /* LOG CONTENTS : org child pid + prop key rowrid.pid + pro key rowrid.offset + prop key body */
    sLogPtr = sLogBuf;
    STL_WRITE_INT32( sLogPtr, &aOrgChildPid );
    sLogPtr += STL_SIZEOF(smlPid);
    STL_WRITE_INT32( sLogPtr, &aPropKeyRowRid->mPageId );
    sLogPtr += STL_SIZEOF(smlPid);
    STL_WRITE_INT16( sLogPtr, &aPropKeyRowRid->mOffset );
    sLogPtr += STL_SIZEOF(stlInt16);
    stlMemcpy( sLogPtr, aPropKeyBody, aPropKeyBodySize );
    sLogPtr += aPropKeyBodySize;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_MEMORY_BTREE_INSERT_INTL,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogPtr - sLogBuf,
                           sNewKeyPage.mPch->mTbsId,
                           sNewKeyPage.mPch->mPageId,
                           sNewKeySeq,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    /* LOG CONTENTS : new right child pid */
    sLogPtr = sLogBuf;
    STL_WRITE_INT32( sLogPtr, &sRightChildPid );
    sLogPtr += STL_SIZEOF(smlPid);

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_MEMORY_BTREE_SET_CHILD_PID,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogPtr - sLogBuf,
                           sOrgKeyPage.mPch->mTbsId,
                           sOrgKeyPage.mPch->mPageId,
                           sOrgKeySeq,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(&sNewKeyPage) );

    STL_RAMP( RAMP_RETRY_FROM_ROOT );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LARGE_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXCEEDED_MAX_KEY_SIZE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      SMN_MAX_KEY_BODY_SIZE );
    }
 
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Leaf를 Split한다
 * @param[in] aMiniTrans SMO시 Latch를 잡고 logging을 하는데 사용될 mini transaction
 * @param[in] aRelationHandle index의 relation handle
 * @param[in,out] aPageHandle target leaf page id
 * @param[in] aPropKeyBody insert할 key의 body
 * @param[in] aKeyBodySize insert할 key의 body size
 * @param[in] aRowRid insert할 key가 가리키는 row의 위치
 * @param[in,out] aSlotSeq insert할 leaf page에서의 key의 sequence
 * @param[in] aPathStack index를 root 노드부터 탐색해 온 길에 관한 정보
 * @param[out] aNeedRetry Smo 도중에 Root 생성시 다른 Smo operation과 충돌나서 다시해야 함
 * @param[out] aEmptyPageCount SMO로 인해 빈키만 있는 page의 수
 * @param[out] aEmptyPid SMO로 인해 insert/delete되는 반대쪽 노드에 빈 키만 있는 경우의 해당 PID array
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbSplitLeafNode( smxmTrans         * aMiniTrans,
                               void              * aRelationHandle,
                               smpHandle         * aPageHandle,
                               stlChar           * aPropKeyBody,
                               stlUInt16           aKeyBodySize,
                               smlRid            * aRowRid,
                               stlInt16          * aSlotSeq,
                               smnmpbPathStack   * aPathStack,
                               stlInt32          * aNeedRetry,
                               stlInt16          * aEmptyPageCount,
                               smlPid            * aEmptyPid,
                               smlEnv            * aEnv )
{
    smlPid                sNextPid = SMNMPB_GET_NEXT_PAGE(aPageHandle);
    stlChar             * sKeySlot;
    stlUInt16             sKeySize;
    stlUInt16             sKeyHdrSize;
    stlUInt16             sAccDataSize = 0;
    smpRowItemIterator    sIterator;
    stlUInt16             sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(aPageHandle);
    stlUInt16             sLeftKeyCount;
    stlUInt16             sMaxKeyBodySize;
    smlRid                sMaxKeyRowRid;
    smpHandle             sNextPageHandle;
    smpHandle             sNewPage;
    smlPid                sNewPageId;
    stlChar             * sPropKeyBody = NULL;
    smnmpbLogicalHdr      sLogicalHdr;
    stlChar               sLogBuf[SMP_PAGE_SIZE];
    stlChar             * sLogPtr;
    stlUInt16             sRtsCount;
    stlUInt16             sKeyCount;
    stlChar             * sDataPtr;
    stlUInt16             sDataSize;
    stlUInt16             sOrgDataSize = 0;
    smpHandle             sPageArr[KNL_ENV_MAX_LATCH_DEPTH];
    stlUInt64             sSmoNo;
    stlInt16              i;
    smnmpbStatistics    * sStatistics;
    stlBool               sInsertNewKeyLeft = STL_TRUE;
    smpHandle             sMirrorPageHandle;
    stlInt16              sOrgKeySeq;
    smpHandle             sOrgKeyPage;
    smpHandle           * sCurPage;

    *aEmptyPageCount = 0;
    SMP_MAKE_DUMMY_HANDLE( &sMirrorPageHandle, NULL );

    STL_TRY_THROW( aKeyBodySize <= SMN_MAX_KEY_BODY_SIZE,
                   RAMP_ERR_TOO_LARGE_KEY);

    sStatistics = SMNMPB_GET_MODULE_SPECIFIC_DATA( aRelationHandle );
    SMNMPB_INC_STATISTICS( sStatistics, SplitCount );
    
    if( sNextPid != SMP_NULL_PID )
    {
        /* 1. target의 next page를 잡는다 */
        STL_TRY( smpAcquire( aMiniTrans,
                             smsGetTbsId(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                             sNextPid,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sNextPageHandle,
                             aEnv ) == STL_SUCCESS );
    
        STL_DASSERT( smpGetFreeness( &sNextPageHandle ) != SMP_FREENESS_FREE );
        STL_TRY( smnmpbValidateIndexPage( &sNextPageHandle,
                                          aEnv )
                 == STL_SUCCESS );
    }

    /* 2. 현재 leaf node의 split을 수행한다 */
    if( aPathStack->mDepth == 1 )
    {
        if( aPathStack->mStack[0].mSmoNo > SMN_GET_PAGE_SMO_NO( aPageHandle ) )
        {
            /* SMO 최상단 노드의 split 직전에 다른 thread에 의해 이미 split되어 */
            /* 현재 내가 가진 pathstack이 valid하지 않음 -> 처음부터 재시도 */
            *aNeedRetry = SMNMPB_RETRY_CONCURRENCY;
            STL_THROW( RAMP_RETRY_FROM_ROOT );
        }

        /* 2.1 현재 leaf node가 root이기 때문에 split 후 새로운 root를 생성한다. */
        STL_TRY( smnmpbMakeNewRootNode( aMiniTrans,
                                        aRelationHandle,
                                        aPageHandle,
                                        SMP_NULL_PID,
                                        aPropKeyBody,  /* aPropKeyBody    */
                                        aKeyBodySize,
                                        aRowRid,       /* aPropKeyRowRid  */
                                        &sNewPage,
                                        aSlotSeq,
                                        &sOrgKeyPage,
                                        &sOrgKeySeq,
                                        sPageArr,
                                        &sMirrorPageHandle,
                                        &sSmoNo,
                                        aNeedRetry,
                                        aEnv ) == STL_SUCCESS );
        
        STL_TRY_THROW( *aNeedRetry == SMNMPB_RETRY_NONE, RAMP_RETRY_FROM_ROOT );
            
        for( i = 0; i < aPathStack->mDepth; i++ )
        {
            aPathStack->mStack[i].mSmoNo = sSmoNo;
        }

        /* 2.1.1 Original page부터 split된 sibling들을 뒤지며 empty pid를 찾는다 */
        sCurPage = aPageHandle;
        i = 0;

        while( STL_TRUE )
        {
            if( smpGetPageId(sCurPage) != smpGetPageId(&sNewPage) )
            {
                if( SMP_GET_ACTIVE_SLOT_COUNT(SMP_FRAME(sCurPage)) == 0 )
                {
                    aEmptyPid[i++] = smpGetPageId(sCurPage);
                }
            }

            if( (SMNMPB_GET_NEXT_PAGE(sCurPage) == SMP_NULL_PID) ||
                (SMNMPB_GET_NEXT_PAGE(sCurPage) == sNextPid) )
            {
                break;
            }

            sCurPage = smxmGetPageHandle( aMiniTrans,
                                          smsGetTbsId(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                                          SMNMPB_GET_NEXT_PAGE(sCurPage),
                                          KNL_LATCH_MODE_EXCLUSIVE );
        }
        *aEmptyPageCount = i;

        /* 2.1.2 새로 key가 insert될 target node를 반환한다 */
        *aPageHandle = sNewPage;
    }
    else
    {
        /* 2.2 leaf node split을 위해 parent node로 split key를 propagate한다 */
        /* 2.2.1 split할 노드에 들어있는 data들의 총 크기를 구한다 */
        sOrgDataSize = ( SMP_PAGE_SIZE
                         - STL_SIZEOF(smpPhyHdr)
                         - STL_SIZEOF(smpOrderedSlotSignpost)
                         - STL_SIZEOF(smnmpbLogicalHdr)
                         - (STL_SIZEOF(smpRts) * SMP_GET_CURRENT_RTS_COUNT( SMP_FRAME(aPageHandle) ))
                         - STL_SIZEOF(smpOffsetTableHeader)
                         - STL_SIZEOF(smpPhyTail) );

        /* 2.2.2 key size를 기준으로 split 지점을 계산하고 상위로 올려질 key를 찾는다 */
        SMP_GET_FIRST_ROWITEM(SMP_FRAME(aPageHandle), &sIterator, sKeySlot);
        sLeftKeyCount = 0;
        sMaxKeyBodySize = 0;

        while( sKeySlot != NULL )
        {
            /* 새로운 key가 들어가는 insert에대해서 수행(delete와 dupkey는 새 key를 넣지 않는다) */
            if( (aKeyBodySize > 0 ) && (sLeftKeyCount == *aSlotSeq) )
            {
                sAccDataSize += aKeyBodySize + SMNMPB_TBK_HDR_SIZE + STL_SIZEOF(smpOffsetSlot); /* 최악의 경우를 생각 */
                if( sAccDataSize > (sOrgDataSize * 0.5) )
                {
                    /* 새로운 key가 propagate 되어야 하는 경우 */
                    sPropKeyBody      = aPropKeyBody;
                    sMaxKeyBodySize   = aKeyBodySize;
                    sMaxKeyRowRid     = *aRowRid;
                    sInsertNewKeyLeft = STL_TRUE;
                    break;
                }
            }

            sKeySize = smnmpbGetLeafKeySize(sKeySlot, sKeyColCount);
            sAccDataSize += sKeySize + STL_SIZEOF(smpOffsetSlot);
            if( sAccDataSize > (sOrgDataSize * 0.5) )
            {
                /* 기존 key가 propagate 되어야 하는 경우 */
                SMNMPB_GET_LEAF_KEY_HDR_SIZE( sKeySlot, &sKeyHdrSize );
                sKeySize = smnmpbGetLeafKeySize(sKeySlot, sKeyColCount);
                sPropKeyBody = sKeySlot + sKeyHdrSize;
                sMaxKeyBodySize = sKeySize - sKeyHdrSize;
                SMNMPB_GET_ROW_PID( sKeySlot, &sMaxKeyRowRid.mPageId );
                SMNMPB_GET_ROW_SEQ( sKeySlot, &sMaxKeyRowRid.mOffset );
                sLeftKeyCount++;

                if( sLeftKeyCount <= *aSlotSeq )
                {
                    sInsertNewKeyLeft = STL_FALSE;
                }
                break;
            }
            sLeftKeyCount++;
            SMP_GET_NEXT_ROWITEM(&sIterator, sKeySlot);
        }

        STL_ASSERT( sMaxKeyBodySize != 0 );

        /* 2.2.3 parent node로 split key를 propagate한다 */
        STL_TRY( smnmpbPropagateInternalKey( aMiniTrans,
                                             aRelationHandle,
                                             aPathStack,
                                             aPathStack->mDepth - 1,
                                             smpGetPageId(aPageHandle),
                                             sPropKeyBody,
                                             sMaxKeyBodySize,
                                             &sMaxKeyRowRid,
                                             sPageArr,
                                             &sMirrorPageHandle,
                                             aNeedRetry,
                                             aEnv ) == STL_SUCCESS );
        STL_TRY_THROW( *aNeedRetry == SMNMPB_RETRY_NONE, RAMP_RETRY_FROM_ROOT );

        /* Redo시에 Org page에대한 compact를 강제하기위해 로그를 남긴다 */
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_AGING_COMPACT,
                               SMR_REDO_TARGET_PAGE,
                               NULL,
                               0,
                               aPageHandle->mPch->mTbsId,
                               smpGetPageId(aPageHandle),
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );


        sNewPage = sPageArr[0];
        sNewPageId = smpGetPageId(&sNewPage);

        SMN_SET_PAGE_SMO_NO( aPageHandle, aPathStack->mStack[aPathStack->mDepth - 1].mSmoNo );
        SMN_SET_PAGE_SMO_NO( &sNewPage, aPathStack->mStack[aPathStack->mDepth - 1].mSmoNo );

        stlMemset( &sLogicalHdr, 0x00, STL_SIZEOF(smnmpbLogicalHdr) );
        sLogicalHdr.mPrevPageId = smpGetPageId(aPageHandle);
        sLogicalHdr.mNextPageId = SMNMPB_GET_NEXT_PAGE(aPageHandle);
        sLogicalHdr.mNextFreePageId = SMP_NULL_PID;
        sLogicalHdr.mFlag     = SMNMPB_NODE_FLAG_INIT; /* leaf node는 last sibling을 사용하지 않음 */
        sLogicalHdr.mLevel    = 0;
        sLogicalHdr.mKeyColCount = sKeyColCount;
        stlMemcpy( &sLogicalHdr.mKeyColTypes, SMNMPB_GET_KEY_COLUMN_TYPES(aPageHandle), sKeyColCount );
        stlMemcpy( &sLogicalHdr.mKeyColFlags, SMNMPB_GET_KEY_COLUMN_FLAGS(aPageHandle), sKeyColCount );
    
        STL_TRY( smpInitBody( &sNewPage,
                              SMP_ORDERED_SLOT_TYPE,
                              STL_SIZEOF(smnmpbLogicalHdr),
                              (void*)&sLogicalHdr,
                              SMN_GET_INITRANS(aRelationHandle),
                              SMN_GET_MAXTRANS(aRelationHandle),
                              STL_FALSE,
                              aMiniTrans,
                              aEnv) == STL_SUCCESS );

        /* 2.2.4 leaf node를 split 한다 */
        /* 2.2.4.1 new page */
        STL_TRY( smnmpbCopyLeafKeys( aPageHandle,
                                     &sNewPage,
                                     sLeftKeyCount,
                                     SMP_GET_PAGE_ROW_ITEM_COUNT( aPageHandle ) - 1,
                                     aEnv ) == STL_SUCCESS );

        STL_DASSERT( SMN_GET_INITRANS(aRelationHandle) <=
                     SMP_GET_CURRENT_RTS_COUNT( SMP_FRAME(&sNewPage) ) );
        
        /* LOG CONTENTS : RtsCount + RTSs + KeyCount + KeyData */
        sLogPtr = sLogBuf;
        sRtsCount = SMP_GET_CURRENT_RTS_COUNT( SMP_FRAME(&sNewPage) );
        stlMemcpy( sLogPtr, &sRtsCount, STL_SIZEOF(stlUInt16) );
        sLogPtr += STL_SIZEOF(stlUInt16);
        stlMemcpy( sLogPtr,
                   SMP_GET_NTH_RTS( SMP_FRAME(&sNewPage), 0 ), 
                   sRtsCount * smpGetRtsSize() );
        sLogPtr += sRtsCount * smpGetRtsSize();
        sKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sNewPage );
        stlMemcpy( sLogPtr, &sKeyCount, STL_SIZEOF(stlUInt16) );
        sLogPtr += STL_SIZEOF(stlUInt16);
        smpGetPageDataSection( &sNewPage, &sDataPtr, &sDataSize );
        stlMemcpy( sLogPtr, sDataPtr, sDataSize );
        sLogPtr += sDataSize;
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_SPLIT_NEW_PAGE,
                               SMR_REDO_TARGET_PAGE,
                               sLogBuf,
                               sLogPtr - sLogBuf,
                               sNewPage.mPch->mTbsId,
                               smpGetPageId(&sNewPage),
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    
        /* 2.2.4.1 old page */
        SMNMPB_GET_LOGICAL_HEADER(aPageHandle)->mNextPageId = sNewPageId;
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_SET_NEXT_PAGE_ID,
                               SMR_REDO_TARGET_PAGE,
                               (void*)&sNewPageId,
                               STL_SIZEOF(smlPid),
                               aPageHandle->mPch->mTbsId,
                               smpGetPageId(aPageHandle),
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );

        STL_TRY( smnmpbTruncateNode( aRelationHandle,
                                     aPageHandle,
                                     sLeftKeyCount,
                                     aEnv ) == STL_SUCCESS );

        /* LOG CONTENTS : LeftKeyCount + LastKeyData  */
        sLogPtr = sLogBuf;
        stlMemcpy( sLogPtr, &sLeftKeyCount, STL_SIZEOF(stlUInt16) );
        sLogPtr += STL_SIZEOF(stlUInt16);
        sKeySlot = smpGetNthRowItem( aPageHandle,
                                     SMP_GET_PAGE_ROW_ITEM_COUNT(aPageHandle) - 1 );
        sKeySize = smnmpbGetLeafKeySize(sKeySlot, sKeyColCount);
        stlMemcpy( sLogPtr, sKeySlot, sKeySize );
        sLogPtr += sKeySize;
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_SPLIT_OLD_PAGE,
                               SMR_REDO_TARGET_PAGE,
                               sLogBuf,
                               sLogPtr - sLogBuf,
                               aPageHandle->mPch->mTbsId,
                               smpGetPageId(aPageHandle),
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );

        /* 현재 leaf node의 next page가 존재한다면 next page의 prev page id를 갱신한다. */
        if( sNextPid != SMP_NULL_PID )
        {
            (SMNMPB_GET_LOGICAL_HEADER(&sNextPageHandle))->mPrevPageId = smpGetPageId(&sNewPage);
            STL_TRY( smxmWriteLog( aMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMR_LOG_MEMORY_BTREE_SET_PREV_PAGE_ID,
                                   SMR_REDO_TARGET_PAGE,
                                   (void*)&sNewPageId,
                                   STL_SIZEOF(smlPid),
                                   smpGetTbsId(&sNextPageHandle),
                                   smpGetPageId(&sNextPageHandle),
                                   0,
                                   SMXM_LOGGING_REDO,
                                   aEnv ) == STL_SUCCESS );
        }

        /* 2.2.5 새로 key가 insert될 target node와 empty pid를 반환한다 */
        if( sInsertNewKeyLeft != STL_TRUE )
        {
            *aSlotSeq -= sLeftKeyCount;
            /* 새 키를 right node에 insert */
            if( SMP_GET_ACTIVE_SLOT_COUNT(SMP_FRAME(aPageHandle)) == 0 )
            {
                *aEmptyPageCount = 1;
                aEmptyPid[0] = smpGetPageId(aPageHandle);
            }
            *aPageHandle = sNewPage;
        }
        else
        {
            if( SMP_GET_ACTIVE_SLOT_COUNT(SMP_FRAME(&sNewPage)) == 0 )
            {
                *aEmptyPageCount = 1;
                aEmptyPid[0] = smpGetPageId(&sNewPage);
            }
        }
    }

    /**
     * root의 변경이 발생했다면 mirror page에 반영시켜야 한다.
     */
    if( SMP_IS_DUMMY_HANDLE( &sMirrorPageHandle ) == STL_FALSE )
    {
        STL_TRY( smnmpbMirrorRoot( aRelationHandle,
                                   aMiniTrans,
                                   &sMirrorPageHandle,
                                   aEnv )
                 == STL_SUCCESS );
    }

    STL_RAMP( RAMP_RETRY_FROM_ROOT );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LARGE_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXCEEDED_MAX_KEY_SIZE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      SMN_MAX_KEY_BODY_SIZE );
    }
 
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 새 root node를 만들 때 기존의 root node에서 split할 node 수와 split position을 구한다.
 * @param[in] aMiniTrans SMO시 Latch를 잡고 logging을 하는데 사용될 mini transaction
 * @param[in] aRelationHandle Index의 relation handle
 * @param[in] aOrgPageHandle Split될 현재 root node
 * @param[in] aCurLevel Split될 현재 root node level
 * @param[in] aPropKeyBody 하위 노드에서 올라온 key의 body
 * @param[in] aPropKeyBodySize 하위 노드에서 올라온 key의 body size
 * @param[in] aPropKeyRowRid 하위 노드에서 올라온 key가 가리키는 row의 위치
 * @param[in] aPropKeySeq Old root node에 삽입될 key의 seq
 * @param[out] aSplitNodeCount Split될 node 수
 * @param[out] aSplitKeyArr Split될 key slot array
 * @param[out] aPropKeyNodeSeq Propagate된 key가 들어갈 node의 seq
 * @param[out] aUseMirrorRoot Root mirroring page를 사용하는지 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbCalculateSplitPositions( smxmTrans         * aMiniTrans,
                                         void              * aRelationHandle,
                                         smpHandle         * aOrgPageHandle,
                                         stlUInt16           aCurLevel,
                                         stlChar           * aPropKeyBody,
                                         stlUInt16           aPropKeyBodySize,
                                         smlRid            * aPropKeyRowRid,
                                         stlInt16            aPropKeySeq,
                                         stlInt16          * aSplitNodeCount,
                                         smnmpbPropKeyStr  * aSplitKeyArr,
                                         stlInt16          * aPropKeyNodeSeq,
                                         stlBool           * aUseMirrorRoot,
                                         smlEnv            * aEnv )
{
    stlBool       sMaximizeRoot;
    stlInt16      sReservedNodeCount;
    stlInt16      sSplitNodeCount;
    stlInt64      sMirrorDepth;

    sMirrorDepth = knlGetPropertyBigIntValueByID( KNL_PROPERTY_INDEX_ROOT_MIRRORING_DEPTH,
                                                  KNL_ENV( aEnv ) );

    *aPropKeyNodeSeq = -1;
    *aUseMirrorRoot = STL_FALSE;
    sSplitNodeCount = 2;

    if( (SMN_GET_MIRROR_ROOT_PID(aRelationHandle) != SMP_NULL_PID) ||
        (sMirrorDepth <= (aCurLevel + 1)) )
    {
        *aUseMirrorRoot = STL_TRUE;
        sMaximizeRoot = knlGetPropertyBoolValueByID( KNL_PROPERTY_INDEX_ROOT_MAXIMIZE_FANOUT,
                                                     KNL_ENV( aEnv ) );

        /**
         * INDEX_ROOT_MAXIMIZE_FANOUT property가 TRUE 일 경우
         * mini-transaction이 허용하는 page latch 수만큼 root node의 fanout을 확장한다.
         */
        if( sMaximizeRoot == STL_TRUE )
        {
            /**
             * B-Tree의 key 크기가 크다면 fanout 확장이 성능향상에 미치는 영향이 미미하다.
             *  : Root node의 fanout을 최대화할 수 있는 maximum은 mini-transaction이
             *    현재 획득하고 있는 latch 수와 split이 완료된 후 필요한 최대 latch 수의 합을
             *    maximum latch수에서 뺀 값이다.
             *   1. mini-transaction이 획득한 latch 수 : smxmGetLatchedPageCount()
             *   2. split을 위한 index path page 수 : aCurLevel + 1
             *   3. split 완료 후 회득해야 할 page latch 수 : 7
             *    ( new root를 index handle에 설정하기 위해 segment handle의 latch 획득,
             *      mirror root를 index handle에 설정할 때, 만약 old mirror root가 있다면 latch 획득,
             *      insert key가 완료된 후 rts stamping을 위한 latch 획득,
             *      undo log를 기록하기 위해 최대 3개 page latch 획득 )
             */
            if( SMP_GET_PAGE_ROW_ITEM_COUNT(aOrgPageHandle) > KNL_ENV_MAX_LATCH_DEPTH )
            {
                sReservedNodeCount = aCurLevel + 1 + smxmGetLatchedPageCount( aMiniTrans ) + 7;
                sSplitNodeCount    = KNL_ENV_MAX_LATCH_DEPTH - sReservedNodeCount;
            }
        }
    }

    *aSplitNodeCount = sSplitNodeCount;

    if( SMNMPB_IS_LEAF(aOrgPageHandle) == STL_TRUE )
    {
        STL_TRY( smnmpbCalculateLeafSplitPositions( aOrgPageHandle,
                                                    aPropKeyBody,
                                                    aPropKeyBodySize,
                                                    aPropKeyRowRid,
                                                    aPropKeySeq,
                                                    &sSplitNodeCount,
                                                    aSplitKeyArr,
                                                    aPropKeyNodeSeq,
                                                    aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smnmpbCalculateIntlSplitPositions( aOrgPageHandle,
                                                    aPropKeyBody,
                                                    aPropKeyBodySize,
                                                    aPropKeyRowRid,
                                                    aPropKeySeq,
                                                    &sSplitNodeCount,
                                                    aSplitKeyArr,
                                                    aPropKeyNodeSeq,
                                                    aEnv ) == STL_SUCCESS );
    }

    STL_TRY_THROW( (1 < sSplitNodeCount) && (sSplitNodeCount <= *aSplitNodeCount),
                   RAMP_ERR_INTERNAL );

    *aSplitNodeCount = sSplitNodeCount;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INTERNAL,
                      "- smnmpbCalculateSplitPositions()",
                      KNL_ERROR_STACK(aEnv) );
        
        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbCalculateIntlSplitPositions( smpHandle         * aOrgPageHandle,
                                             stlChar           * aPropKeyBody,
                                             stlUInt16           aPropKeyBodySize,
                                             smlRid            * aPropKeyRowRid,
                                             stlInt16            aPropKeySeq,
                                             stlInt16          * aSplitNodeCount,
                                             smnmpbPropKeyStr  * aSplitKeyArr,
                                             stlInt16          * aPropKeyNodeSeq,
                                             smlEnv            * aEnv )
{
    stlChar             * sCurKey;
    stlUInt16             sSplitNodeSize;
    smpRowItemIterator    sIterator;
    stlUInt16             sRemainSize = 0;
    stlUInt16             sAccDataSize = 0;
    stlUInt16             sSlotHdrSize;
    stlUInt16             sSplitSeq = 0;
    stlInt16              sCurKeySeq;
    stlUInt16             sCurKeySize;
    stlInt16              sSplitBeginPos;
    stlInt16              sSplitNodeCount = *aSplitNodeCount;
    stlInt16              sKeyColCount;
    stlInt32              sState = 0;

    STL_TRY_THROW( aPropKeyBodySize > 0, RAMP_ERR_INTERNAL );
    sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT( aOrgPageHandle );

    *aPropKeyNodeSeq = -1;

    sSlotHdrSize = SMNMPB_INTL_SLOT_HDR_SIZE + STL_SIZEOF(smpOffsetSlot);

    /* split할 노드에 들어있는 data들의 총 크기를 구한다 */
    sRemainSize = ( SMP_PAGE_SIZE
                    - STL_SIZEOF(smpPhyHdr)
                    - STL_SIZEOF(smpOrderedSlotSignpost)
                    - STL_SIZEOF(smnmpbLogicalHdr)
                    - STL_SIZEOF(smpOffsetTableHeader)
                    - STL_SIZEOF(smpPhyTail) );

    sSplitNodeSize = sRemainSize / sSplitNodeCount;

    /* 첫번째 key부터 key size를 계산하여 상위로 올려질 key를 구한다. */
    SMP_GET_FIRST_ROWITEM( SMP_FRAME(aOrgPageHandle), &sIterator, sCurKey );
    sCurKeySeq = 0;
    sSplitBeginPos = 0;
    sSplitNodeCount--;

    while( sCurKey != NULL )
    {
        if( sCurKeySeq == aPropKeySeq )
        {
            *aPropKeyNodeSeq = sSplitSeq;
            sAccDataSize += (aPropKeyBodySize + sSlotHdrSize);

            /* Propagate된 key가 split node의 max key가 되는 경우 */
            if( sSplitNodeSize < sAccDataSize )
            {
                /* Split을 위한 propagation key의 크기가 root node의 남은 공간보다
                 * 더 크면 propagate할 수 없다.
                 */ 
                if( (aPropKeyBodySize + sSlotHdrSize) <= (sRemainSize - sSlotHdrSize) )
                {
                    aSplitKeyArr[sSplitSeq].mPropKeyBody   = aPropKeyBody;
                    aSplitKeyArr[sSplitSeq].mKeyBodySize   = aPropKeyBodySize;
                    aSplitKeyArr[sSplitSeq].mRowRid        = *aPropKeyRowRid;
                    aSplitKeyArr[sSplitSeq].mBeginPos      = sSplitBeginPos;
                    aSplitKeyArr[sSplitSeq].mIsPropKeyLast = STL_TRUE;

                    /* Propagation이 발생하고 남은 공간을 이용하여 나머지 split할 크기를 계산한다.
                     */
                    sRemainSize -= (aSplitKeyArr[sSplitSeq].mKeyBodySize + sSlotHdrSize);
                    sSplitNodeSize = sRemainSize / sSplitNodeCount;
                    sSplitNodeCount--;

                    sAccDataSize = 0;
                    sSplitBeginPos = sCurKeySeq;

                    sSplitSeq++;

                    if( sSplitSeq >= sSplitNodeCount )
                    {
                        break;
                    }
                }
            }
        }

        /* Split position을 aSplitKeySeq에 등록 */
        sCurKeySize = smnmpbGetIntlKeySize( sCurKey, sKeyColCount );
        sAccDataSize += (sCurKeySize + STL_SIZEOF(smpOffsetSlot));
        if( sSplitNodeSize < sAccDataSize )
        {
            /* 마지막 key가 propagate 되는 경우 별도 처리 */
            if( (sCurKeySeq + 1) == SMP_GET_PAGE_ROW_ITEM_COUNT(aOrgPageHandle) )
            {
                break;
            }

            /*
             * Split을 위한 propagation key가 root node의 남은 공간보다
             * key size가 더 클수 없다.
             */ 
            if( (sCurKeySize + STL_SIZEOF(smpOffsetSlot)) <= (sRemainSize - sSlotHdrSize) )
            {
                /* Split node의 max key를 설정 */
                smnmpbGetIntlKeySizeAndBody( sCurKey,
                                             sKeyColCount,
                                             &sCurKeySize,
                                             &aSplitKeyArr[sSplitSeq].mKeyBodySize,
                                             &aSplitKeyArr[sSplitSeq].mPropKeyBody,
                                             &aSplitKeyArr[sSplitSeq].mRowRid );
                aSplitKeyArr[sSplitSeq].mBeginPos = sSplitBeginPos;
                aSplitKeyArr[sSplitSeq].mIsPropKeyLast = STL_FALSE;

                sRemainSize -= (aSplitKeyArr[sSplitSeq].mKeyBodySize + STL_SIZEOF(smpOffsetSlot));
                sSplitNodeSize = sRemainSize / sSplitNodeCount;
                sSplitNodeCount--;

                sSplitBeginPos = sCurKeySeq + 1;
                sAccDataSize = 0;

                sSplitSeq++;

                if( sSplitSeq >= sSplitNodeCount )
                {
                    break;
                }
            }
        }

        sCurKeySeq++;
        SMP_GET_NEXT_ROWITEM( &sIterator, sCurKey );
    }

    sState = 2;
    STL_TRY_THROW( sRemainSize >= sSlotHdrSize, RAMP_ERR_INTERNAL );

    if( *aPropKeyNodeSeq < 0 )
    {
        sState = 3;
        STL_TRY_THROW( aPropKeySeq >= sSplitBeginPos, RAMP_ERR_INTERNAL );
        *aPropKeyNodeSeq = sSplitSeq;
    }

    sState = 4;
    STL_TRY_THROW( sSplitBeginPos > 0, RAMP_ERR_INTERNAL );

    /* 마지막 sibling node를 위한 propagate key(infinite)를 설정 */
    aSplitKeyArr[sSplitSeq].mPropKeyBody = NULL;
    aSplitKeyArr[sSplitSeq].mKeyBodySize = 0;
    aSplitKeyArr[sSplitSeq].mRowRid      = SML_INVALID_RID;
    aSplitKeyArr[sSplitSeq].mBeginPos    = sSplitBeginPos;
    aSplitKeyArr[sSplitSeq].mIsPropKeyLast = STL_FALSE;

    *aSplitNodeCount = sSplitSeq + 1;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INTERNAL,
                      "- smnmpbCalculateIntlSplitPositions()",
                      KNL_ERROR_STACK(aEnv),
                      sState );
        
        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbCalculateLeafSplitPositions( smpHandle         * aOrgPageHandle,
                                             stlChar           * aPropKeyBody,
                                             stlUInt16           aPropKeyBodySize,
                                             smlRid            * aPropKeyRowRid,
                                             stlInt16            aPropKeySeq,
                                             stlInt16          * aSplitNodeCount,
                                             smnmpbPropKeyStr  * aSplitKeyArr,
                                             stlInt16          * aPropKeyNodeSeq,
                                             smlEnv            * aEnv )
{
    stlChar             * sCurKey;
    stlUInt16             sRemainSize = 0;
    stlUInt16             sSplitNodeSize;
    smpRowItemIterator    sIterator;
    stlUInt16             sAccDataSize = 0;
    stlUInt16             sSlotHdrSize;
    stlUInt16             sSplitSeq = 0;
    stlInt16              sCurKeySeq;
    stlUInt16             sKeySize;
    stlInt16              sSplitBeginPos;
    stlInt16              sSplitNodeCount = *aSplitNodeCount;
    stlInt16              sKeyColCount;
    stlInt32              sState = 0;

    *aPropKeyNodeSeq = -1;
    sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT( aOrgPageHandle );

    sSlotHdrSize = SMNMPB_TBK_HDR_SIZE + STL_SIZEOF(smpOffsetSlot);

    /* split할 노드에 들어있는 data들의 총 크기를 구한다 */
    sRemainSize = ( SMP_PAGE_SIZE
                    - STL_SIZEOF(smpPhyHdr)
                    - STL_SIZEOF(smpOrderedSlotSignpost)
                    - STL_SIZEOF(smnmpbLogicalHdr)
                    - (STL_SIZEOF(smpRts) * SMP_GET_CURRENT_RTS_COUNT( SMP_FRAME(aOrgPageHandle) ))
                    - STL_SIZEOF(smpOffsetTableHeader)
                    - STL_SIZEOF(smpPhyTail) );

    sSplitNodeSize = sRemainSize / sSplitNodeCount;

    /* 첫번째 key부터 key size를 계산하여 상위로 올려질 key를 구한다. */
    SMP_GET_FIRST_ROWITEM( SMP_FRAME(aOrgPageHandle), &sIterator, sCurKey );
    sCurKeySeq = 0;
    sSplitBeginPos = 0;
    sSplitNodeCount--;

    while( sCurKey != NULL )
    {
        if( sCurKeySeq == aPropKeySeq )
        {
            *aPropKeyNodeSeq = sSplitSeq;
            if( aPropKeyBodySize > 0 )
            {
                sState = 1;
                STL_TRY_THROW( aPropKeyBody != NULL, RAMP_ERR_INTERNAL );
                sAccDataSize += (aPropKeyBodySize + sSlotHdrSize);

                /* Propagate 된 key가 split node의 max key가 되는 경우
                 * 이전 key를 max key로 설정하고 propagate 된 key는 next의
                 * 첫번째 key 로 설정한다 */
                if( sSplitNodeSize < sAccDataSize )
                {
                    /*
                     * Split을 위한 propagation key가 root node의 남은 공간보다
                     * key size가 더 클수 없다.
                     */ 
                    if( (aPropKeyBodySize + sSlotHdrSize) <= (sRemainSize - sSlotHdrSize) )
                    {
                        aSplitKeyArr[sSplitSeq].mKeyBodySize   = aPropKeyBodySize;
                        aSplitKeyArr[sSplitSeq].mPropKeyBody   = aPropKeyBody;
                        aSplitKeyArr[sSplitSeq].mRowRid        = *aPropKeyRowRid;
                        aSplitKeyArr[sSplitSeq].mBeginPos      = sSplitBeginPos;
                        aSplitKeyArr[sSplitSeq].mIsPropKeyLast = STL_TRUE;

                        sRemainSize -= (aSplitKeyArr[sSplitSeq].mKeyBodySize + sSlotHdrSize);
                        sSplitNodeSize = sRemainSize / sSplitNodeCount;
                        sSplitNodeCount--;

                        sAccDataSize = 0;
                        sSplitBeginPos = sCurKeySeq;
                        sSplitSeq++;

                        if( sSplitSeq >= (*aSplitNodeCount - 1) )
                        {
                            break;
                        }
                    }
                }
            }
        }

        /* Split position을 aSplitKeySeq에 등록 */
        sKeySize = smnmpbGetLeafKeySize( sCurKey, sKeyColCount );
        sAccDataSize += (sKeySize + STL_SIZEOF(smpOffsetSlot));
        if( sSplitNodeSize < sAccDataSize )
        {
            /* Leaf node의 split key를 하나이상 만들었을 때
             * 마지막 key까지 체크되었으면 종료 */
            if( (sSplitSeq > 0) &&
                ((sCurKeySeq + 1) >= SMP_GET_PAGE_ROW_ITEM_COUNT(aOrgPageHandle)) )
            {
                break;
            }

            /*
             * Split을 위한 propagation key가 root node의 남은 공간보다
             * key size가 더 클수 없다.
             */ 
            if( (sKeySize + STL_SIZEOF(smpOffsetSlot)) <= (sRemainSize - sSlotHdrSize) )
            {
                /* Split node의 max key로 현재 key를 설정 */
                smnmpbGetLeafKeySizeAndBody( sCurKey,
                                             sKeyColCount,
                                             &sKeySize,
                                             &aSplitKeyArr[sSplitSeq].mKeyBodySize,
                                             &aSplitKeyArr[sSplitSeq].mPropKeyBody,
                                             &aSplitKeyArr[sSplitSeq].mRowRid );

                aSplitKeyArr[sSplitSeq].mBeginPos = sSplitBeginPos;
                aSplitKeyArr[sSplitSeq].mIsPropKeyLast = STL_FALSE;
                sSplitBeginPos = sCurKeySeq + 1;
                sAccDataSize = 0;

                sRemainSize -= (aSplitKeyArr[sSplitSeq].mKeyBodySize + STL_SIZEOF(smpOffsetSlot));
                sSplitNodeSize = sRemainSize / sSplitNodeCount;
                sSplitNodeCount--;

                sSplitSeq++;

                if( sSplitSeq >= (*aSplitNodeCount - 1) )
                {
                    break;
                }
            }
        }

        sCurKeySeq++;
        SMP_GET_NEXT_ROWITEM( &sIterator, sCurKey );
    }

    sState = 2;
    STL_TRY_THROW( sRemainSize >= sSlotHdrSize, RAMP_ERR_INTERNAL );

    if( *aPropKeyNodeSeq < 0 )
    {
        sState = 3;
        STL_TRY_THROW( aPropKeySeq >= sSplitBeginPos, RAMP_ERR_INTERNAL );
        *aPropKeyNodeSeq = sSplitSeq;
    }

    sState = 4;
    STL_TRY_THROW( sSplitBeginPos > 0, RAMP_ERR_INTERNAL );

    /* 마지막 sibling node를 위한 propagate key(infinite)를 설정 */
    aSplitKeyArr[sSplitSeq].mPropKeyBody = NULL;
    aSplitKeyArr[sSplitSeq].mKeyBodySize = 0;
    aSplitKeyArr[sSplitSeq].mRowRid      = SML_INVALID_RID;
    aSplitKeyArr[sSplitSeq].mBeginPos    = sSplitBeginPos;
    aSplitKeyArr[sSplitSeq].mIsPropKeyLast = STL_FALSE;

    *aSplitNodeCount = sSplitSeq + 1;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INTERNAL,
                      "- smnmpbCalculateLeafSplitPositions()",
                      KNL_ERROR_STACK(aEnv),
                      sState );
        
        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }

    STL_FINISH;

    return STL_SUCCESS;
}

/**
 * @brief Old root node를 3개 이상 node로 split하고 new root node로 propagate한다.
 * @param[in] aMiniTrans SMO시 Latch를 잡고 logging을 하는데 사용될 mini transaction
 * @param[in] aRelationHandle index의 relation handle
 * @param[in] aNewRootPage 새로 만들어진 root page handle
 * @param[in] aOldRootPage Split될 old root page handle
 * @param[in] aPropKeyRowRid 하위 노드에서 올라온 key가 가리키는 row의 위치
 * @param[in] aPropKeyBody 하위 노드에서 올라온 key의 body
 * @param[in] aPropKeyBodySize 하위 노드에서 올라온 key의 body size
 * @param[in] aOrgChildPid 기존의 child node page id
 * @param[in] aSplitPageCount Split할 page 수
 * @param[in] aSplitKeyArr 현재 root node를 split하기 위해 계산된 split key structure
 * @param[in] aPropKeyNodeSeq Propagate된 key가 들어갈 page idx
 * @param[in] aPageArr Split을 위해 미리 만들어진 page handle
 * @param[in,out] aTargetPage target leaf page handle
 * @param[in,out] aTargetKeySeq insert할 leaf page에서의 key의 sequence
 * @param[out] aOrgKeyPage Split된 page를 가리키는 key를 저장하고 있는 page 
 * @param[out] aOrgKeySeq Split된 page를 가리키는 key가 저장된 slot seq
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbSplitRootNode( smxmTrans         * aMiniTrans,
                               void              * aRelationHandle,
                               smpHandle         * aNewRootPage,
                               smpHandle         * aOldRootPage,
                               smlRid            * aPropKeyRowRid,
                               stlChar           * aPropKeyBody,
                               stlUInt16           aPropKeyBodySize,
                               smlPid              aOrgChildPid,
                               stlInt16            aSplitPageCount,
                               smnmpbPropKeyStr  * aSplitKeyArr,
                               stlInt16            aPropKeyNodeSeq,
                               smpHandle         * aPageArr,
                               smpHandle         * aTargetPage,
                               stlInt16          * aTargetKeySeq,
                               smpHandle         * aOrgKeyPage,
                               stlInt16          * aOrgKeySeq,
                               smlEnv            * aEnv )
{
    smpHandle      sNewChildPage;
    smpHandle    * sNextPage;
    stlInt16       i;
    stlChar      * sLastKeySlot;
    smlPid         sNewChildPid;
    smlPid         sNextPageId;
    stlInt16       sSplitBeginPos;
    stlInt16       sSplitEndPos;
    stlInt16       sLeftKeyCount;
    stlUInt16      sKeySize;
    stlUInt16      sKeyColCount;
    stlChar        sLogBuf[SMP_PAGE_SIZE];
    stlChar      * sLogPtr;
    stlInt16       sSplitSeq = aSplitPageCount - 1;
    /* stlInt64       sKeyValue[SMNMPB_MAX_KEY_BODY_SIZE/STL_SIZEOF(stlInt64)]; */

    STL_ASSERT( aOldRootPage != NULL );

    sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(aOldRootPage);
    sSplitEndPos = SMP_GET_PAGE_ROW_ITEM_COUNT( aOldRootPage );

    sNextPageId = SMNMPB_GET_NEXT_PAGE( aOldRootPage );

    /* knlLogMsg( NULL, */
    /*            KNL_ENV( aEnv ), */
    /*            KNL_LOG_LEVEL_INFO, */
    /*            "EXPAND_ROOT_FANOUT_START : SPLIT_COUNT(%d), ORG_PID(%d), " */
    /*            "TARGET_KEY_SEQ(%d), TOTAL_KEY_COUNT(%d), PROP_KEY_NODE_SEQ(%d)\n", */
    /*            aSplitPageCount, */
    /*            smpGetPageId( aOldRootPage ), */
    /*            *aTargetKeySeq, */
    /*            SMP_GET_PAGE_ROW_ITEM_COUNT( aOldRootPage ), */
    /*            aPropKeyNodeSeq ); */

    for( i = 0; i < aSplitPageCount; i++ )
    {
        sSplitBeginPos = aSplitKeyArr[sSplitSeq].mBeginPos;
        sNewChildPage  = (sSplitSeq == 0)? *aOldRootPage : aPageArr[i];
        sNewChildPid   = smpGetPageId( &sNewChildPage );
 
        if( aPropKeyNodeSeq == sSplitSeq )
        {
            STL_ASSERT( (sSplitBeginPos <= *aTargetKeySeq) &&
                        (*aTargetKeySeq <= sSplitEndPos) );

            /* propagate된 key는 지금 split되는 node에 저장된다 */
            *aTargetKeySeq = *aTargetKeySeq - sSplitBeginPos;
            *aTargetPage   = sNewChildPage;

            if( aSplitKeyArr[sSplitSeq].mIsPropKeyLast == STL_TRUE )
            {
                STL_ASSERT( i > 0 );

                /* propagate된 key가 split position인 경우, 이전 key는
                 * 오른쪽 sibling이 있으면 오른쪽 sibling의 첫번째 key slot으로 이동 */
                *aOrgKeyPage = aPageArr[i - 1];
                *aOrgKeySeq  = 0;
            }
            else
            {
                /* propagate된 key가 저장될 위치에 있었던 이전 key는
                 * 다음 key slot으로 이동 */
                *aOrgKeyPage = sNewChildPage;
                *aOrgKeySeq  = *aTargetKeySeq + 1;
            }
        }

        /* Split하고, split된 key를 root page로 propagate한다 */
        if( sSplitSeq > 0 )
        {
            STL_TRY( smnmpbCopyRootKeys( aRelationHandle,
                                         aMiniTrans,
                                         aOldRootPage,
                                         &sNewChildPage,
                                         sSplitBeginPos,
                                         sSplitEndPos - 1,
                                         aEnv ) == STL_SUCCESS );
        }

        /* 처음으로 split되는 경우 new root에 처음으로 삽입되는 key가 되므로
         * key body는 기록하지 않고 child pid만 기록한다. */
        if( i == 0 )
        {
            STL_ASSERT( (aSplitKeyArr[sSplitSeq].mPropKeyBody == NULL) &&
                        (aSplitKeyArr[sSplitSeq].mKeyBodySize == 0)    &&
                        (SML_IS_INVALID_RID(aSplitKeyArr[sSplitSeq].mRowRid) == STL_TRUE) );
        }

        if( SMNMPB_IS_LEAF(aOldRootPage) == STL_TRUE )
        {
            /* Leaf page의 경우 next page의 prev page id를 설정하고 로깅한다. */
            if( sNextPageId != SMP_NULL_PID )
            {
                sNextPage = smxmGetPageHandle( aMiniTrans,
                                               smsGetTbsId(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                                               sNextPageId,
                                               KNL_LATCH_MODE_EXCLUSIVE );

                STL_ASSERT( sNextPage != NULL );

                (SMNMPB_GET_LOGICAL_HEADER(sNextPage))->mPrevPageId = sNewChildPid;
                STL_TRY( smxmWriteLog( aMiniTrans,
                                       SMG_COMPONENT_INDEX,
                                       SMR_LOG_MEMORY_BTREE_SET_PREV_PAGE_ID,
                                       SMR_REDO_TARGET_PAGE,
                                       (void*)&sNewChildPid,
                                       STL_SIZEOF(smlPid),
                                       smpGetTbsId( sNextPage ),
                                       smpGetPageId( sNextPage ),
                                       0,
                                       SMXM_LOGGING_REDO,
                                       aEnv ) == STL_SUCCESS );
            }
        }

        if( SMNMPB_GET_NEXT_PAGE(&sNewChildPage) != sNextPageId )
        {
            /* New page의 next page Id를 설정하고 로깅한다. */
            (SMNMPB_GET_LOGICAL_HEADER(&sNewChildPage))->mNextPageId = sNextPageId;
            STL_TRY( smxmWriteLog( aMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMR_LOG_MEMORY_BTREE_SET_NEXT_PAGE_ID,
                                   SMR_REDO_TARGET_PAGE,
                                   (void*)&sNextPageId,
                                   STL_SIZEOF(smlPid),
                                   smpGetTbsId(&sNewChildPage),
                                   smpGetPageId(&sNewChildPage),
                                   0,
                                   SMXM_LOGGING_REDO,
                                   aEnv ) == STL_SUCCESS );
        }

        /* stlMemset( sKeyValue, 0x00, SMNMPB_MAX_KEY_BODY_SIZE ); */
        /* if( aSplitKeyArr[sSplitSeq].mPropKeyBody != NULL ) */
        /* { */
        /*     STL_TRY( smnmpbMakeKeyString( aSplitKeyArr[sSplitSeq].mPropKeyBody, */
        /*                                   0, */
        /*                                   SMNMPB_GET_KEY_COLUMN_COUNT(aOldRootPage), */
        /*                                   SMNMPB_GET_KEY_COLUMN_TYPES(aOldRootPage), */
        /*                                   (stlChar*)sKeyValue, */
        /*                                   KNL_ENV( aEnv ) ) */
        /*              == STL_SUCCESS ); */
        /* } */
        /* knlLogMsg( NULL, */
        /*            KNL_ENV( aEnv ), */
        /*            KNL_LOG_LEVEL_INFO, */
        /*            "EXPAND_ROOT_FANOUT : [%d-th] SPLITTED_PID(%d), NEXT_PID(%d), " */
        /*            "SPLIT_RANGE(%d, %d), TARGET_KEY_SEQ(%d), KEY_VALUE(%s)\n", */
        /*            i, */
        /*            sNewChildPid, */
        /*            sNextPageId, */
        /*            sSplitBeginPos, */
        /*            sSplitEndPos, */
        /*            *aTargetKeySeq, */
        /*            sKeyValue ); */

        /* Split key를 root 로 propagate한다 */
        STL_TRY( smnmpbPropagateKey2RootPage( aRelationHandle,
                                              aMiniTrans,
                                              aNewRootPage,
                                              &aSplitKeyArr[sSplitSeq],
                                              sNewChildPid,
                                              aEnv ) == STL_SUCCESS );

        /* 지금 split된 node는 다음 split node의 next node이다 */
        sNextPageId  = smpGetPageId( &sNewChildPage );
        sSplitEndPos = sSplitBeginPos;
        sSplitSeq--;
    }

    sLeftKeyCount = aSplitKeyArr[1].mBeginPos;

    /* Split된 key들을 org root page에서 삭제하고 logging한다. */
    STL_TRY( smnmpbTruncateNode( aRelationHandle,
                                 aOldRootPage,
                                 sLeftKeyCount,
                                 aEnv ) == STL_SUCCESS );


    /* LOG CONTENTS : LeftKeyCount + LastKeyData  */
    STL_ASSERT( SMP_GET_PAGE_ROW_ITEM_COUNT(aOldRootPage) == sLeftKeyCount );
    sLogPtr = sLogBuf;

    if( sLeftKeyCount > 0 )
    {
        sLeftKeyCount--;
        sLastKeySlot = smpGetNthRowItem( aOldRootPage, sLeftKeyCount );

        sKeySize = (SMNMPB_IS_LEAF(aOldRootPage) == STL_TRUE)?
            smnmpbGetLeafKeySize( sLastKeySlot, sKeyColCount ) :
            smnmpbGetIntlKeySize( sLastKeySlot, sKeyColCount );
    }
    else
    {
        sLastKeySlot = NULL;
        sKeySize = 0;
    }

    stlMemcpy( sLogPtr, &sLeftKeyCount, STL_SIZEOF(stlUInt16) );
    sLogPtr += STL_SIZEOF(stlUInt16);
    stlMemcpy( sLogPtr, sLastKeySlot, sKeySize );
    sLogPtr += sKeySize;
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_MEMORY_BTREE_SPLIT_OLD_PAGE,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogPtr - sLogBuf,
                           smpGetTbsId(aOldRootPage),
                           smpGetPageId(aOldRootPage),
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    /* knlLogMsg( NULL, */
    /*            KNL_ENV( aEnv ), */
    /*            KNL_LOG_LEVEL_INFO, */
    /*            "EXPAND_ROOT_FANOUT_END : TARGET_KEY(%d, %d), ORG_KEY(%d, %d)\n", */
    /*            smpGetPageId(aTargetPage), */
    /*            *aTargetKeySeq, */
    /*            smpGetPageId(aOrgKeyPage), */
    /*            *aOrgKeySeq ); */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Root page에 key를 저장하고 org key의 child page정보를 수정한다.
 * @param[in] aRelationHandle index의 relation handle
 * @param[in] aMiniTrans SMO시 Latch를 잡고 logging을 하는데 사용될 mini transaction
 * @param[in] aRootPageHandle Fanout을 확장할 root page handle
 * @param[in] aPropKey Root node에 삽입될 propagate key
 * @param[in] aChildPid child page의 page id
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbPropagateKey2RootPage( void             * aRelationHandle,
                                       smxmTrans        * aMiniTrans,
                                       smpHandle        * aRootPageHandle,
                                       smnmpbPropKeyStr * aPropKey,
                                       smlPid             aChildPid,
                                       smlEnv           * aEnv )
{
    stlChar      sLogBuf[SMP_PAGE_SIZE];
    stlChar    * sLogPtr;
    stlChar    * sNewKeySlot;
    stlUInt16    sKeySize;
    stlUInt16    sLogType;

    /**
     * Right most child pid를 저장하기 위해 key body가 null로 propagate될 수 있음.
     */
    if( SMP_GET_PAGE_ROW_ITEM_COUNT( aRootPageHandle ) == 0 )
    {
        sKeySize = STL_SIZEOF( smlPid );
        sLogType = SMR_LOG_MEMORY_BTREE_NEW_ROOT;
    }
    else
    {
        sKeySize = aPropKey->mKeyBodySize + SMNMPB_INTL_SLOT_HDR_SIZE;
        sLogType = SMR_LOG_MEMORY_BTREE_INSERT_INTL;
    }

    /**
     * Root page의 aKeySeq 위치에 새로운 key를 저장하기 위해 slot을 할당받음.
     */
    STL_TRY( smpAllocNthSlot( aRootPageHandle,
                              0,
                              sKeySize,
                              STL_FALSE,
                              &sNewKeySlot ) == STL_SUCCESS );

    STL_ASSERT( sNewKeySlot != NULL );

    /**
     * New key slot에 key와 정보를 기록한다.
     */
    SMNMPB_INTL_SLOT_SET_CHILD_PID( sNewKeySlot, &aChildPid );

    /**
     * Root page에 새로운 key insert를 logging한다.
     */
    sLogPtr = sLogBuf;
    STL_WRITE_INT32( sLogPtr, &aChildPid );
    sLogPtr += STL_SIZEOF(smlPid);

    if( sLogType == SMR_LOG_MEMORY_BTREE_INSERT_INTL )
    {
        SMNMPB_INTL_SLOT_SET_ROW_PID( sNewKeySlot, &(aPropKey->mRowRid.mPageId) );
        SMNMPB_INTL_SLOT_SET_ROW_OFFSET( sNewKeySlot, &(aPropKey->mRowRid.mOffset) );

        stlMemcpy( sNewKeySlot + SMNMPB_INTL_SLOT_HDR_SIZE,
                   aPropKey->mPropKeyBody,
                   aPropKey->mKeyBodySize );

        STL_WRITE_INT32( sLogPtr, &(aPropKey->mRowRid.mPageId) );
        sLogPtr += STL_SIZEOF(smlPid);
        STL_WRITE_INT16( sLogPtr, &(aPropKey->mRowRid.mOffset) );
        sLogPtr += STL_SIZEOF(stlInt16);
        stlMemcpy( sLogPtr, aPropKey->mPropKeyBody, aPropKey->mKeyBodySize );
        sLogPtr += aPropKey->mKeyBodySize;
    }

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_INDEX,
                           sLogType,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogPtr - sLogBuf,
                           smpGetTbsId( aRootPageHandle ),
                           smpGetPageId( aRootPageHandle ),
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aRootPageHandle) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Page split을 위해 새로운 page를 할당하여 key를 copy한다.
 * @param[in] aRelationHandle index의 relation handle
 * @param[in] aMiniTrans SMO시 Latch를 잡고 logging을 하는데 사용될 mini transaction
 * @param[in] aOrgChildPage Split을 수행할 original page
 * @param[in] aNewChildPage Split을 수행할 new page
 * @param[in] aSplitBeginPos Original page에서 옮길 첫번째 key 위치
 * @param[in] aSplitEndPos Original page에서 옮길 마지막 key 위치
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbCopyRootKeys( void        * aRelationHandle,
                              smxmTrans   * aMiniTrans,
                              smpHandle   * aOrgChildPage,
                              smpHandle   * aNewChildPage,
                              stlInt16      aSplitBeginPos,
                              stlInt16      aSplitEndPos,
                              smlEnv      * aEnv )
{
    stlInt16              sTotalKeyCount;
    smnmpbLogicalHdr    * sOrgPageLogicalHdr;
    smnmpbLogicalHdr      sNewPageLogicalHdr;
    stlUInt16             sRtsCount;   /* logging용 dummy 변수 */
    stlChar             * sDataPtr;
    stlUInt16             sDataSize;
    smlPid                sNewChildPid;
    stlChar               sLogBuf[SMP_PAGE_SIZE];
    stlChar             * sLogPtr;

    sOrgPageLogicalHdr = SMNMPB_GET_LOGICAL_HEADER( aOrgChildPage );
    stlMemset( &sNewPageLogicalHdr, 0x00, STL_SIZEOF(smnmpbLogicalHdr) );
    sNewPageLogicalHdr.mNextPageId = sOrgPageLogicalHdr->mNextPageId;
    sNewPageLogicalHdr.mNextFreePageId = SMP_NULL_PID;
    sNewPageLogicalHdr.mLevel    = sOrgPageLogicalHdr->mLevel;
    sNewPageLogicalHdr.mFlag     = sOrgPageLogicalHdr->mFlag;
    sNewPageLogicalHdr.mKeyColCount = sOrgPageLogicalHdr->mKeyColCount;
    stlMemcpy( sNewPageLogicalHdr.mKeyColTypes,
               sOrgPageLogicalHdr->mKeyColTypes,
               sOrgPageLogicalHdr->mKeyColCount );
    stlMemcpy( sNewPageLogicalHdr.mKeyColFlags,
               sOrgPageLogicalHdr->mKeyColFlags,
               sOrgPageLogicalHdr->mKeyColCount );

    if( sOrgPageLogicalHdr->mLevel == 0 )
    {
        sNewPageLogicalHdr.mPrevPageId = smpGetPageId( aOrgChildPage );
        STL_TRY( smpInitBody( aNewChildPage,
                              SMP_ORDERED_SLOT_TYPE,
                              STL_SIZEOF(smnmpbLogicalHdr),
                              (void*)&sNewPageLogicalHdr,
                              SMN_GET_INITRANS(aRelationHandle),
                              SMN_GET_MAXTRANS(aRelationHandle),
                              STL_FALSE,
                              aMiniTrans,
                              aEnv) == STL_SUCCESS );

        /**
         * Original Page에서 New Page로 key를 옮긴다.
         */
        STL_TRY( smnmpbCopyLeafKeys( aOrgChildPage,
                                     aNewChildPage,
                                     aSplitBeginPos,
                                     aSplitEndPos,
                                     aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smpInitBody( aNewChildPage,
                              SMP_ORDERED_SLOT_TYPE,
                              STL_SIZEOF(smnmpbLogicalHdr),
                              (void*)&sNewPageLogicalHdr,
                              0,
                              0,
                              STL_FALSE,
                              aMiniTrans,
                              aEnv) == STL_SUCCESS );

        /**
         * Original Page에서 New Page로 key를 옮긴다.
         */
        STL_TRY( smnmpbCopyIntlKeys( aOrgChildPage,
                                     aNewChildPage,
                                     aSplitBeginPos,
                                     aSplitEndPos,
                                     aEnv ) == STL_SUCCESS );
    }

    /* LOG CONTENTS : RtsCount + RTSs + KeyCount + KeyData */
    sLogPtr = sLogBuf;
    sRtsCount = SMP_GET_CURRENT_RTS_COUNT( SMP_FRAME(aNewChildPage) );
    stlMemcpy( sLogPtr, &sRtsCount, STL_SIZEOF(stlUInt16) );
    sLogPtr += STL_SIZEOF(stlUInt16);
    stlMemcpy( sLogPtr,
               SMP_GET_NTH_RTS( SMP_FRAME(aNewChildPage), 0 ), 
               sRtsCount * smpGetRtsSize() );
    sLogPtr += sRtsCount * smpGetRtsSize();
    sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( aNewChildPage );
    stlMemcpy( sLogPtr, &sTotalKeyCount, STL_SIZEOF(stlUInt16) );
    sLogPtr += STL_SIZEOF(stlUInt16);
    smpGetPageDataSection( aNewChildPage, &sDataPtr, &sDataSize );
    stlMemcpy( sLogPtr, sDataPtr, sDataSize );
    sLogPtr += sDataSize;
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_MEMORY_BTREE_SPLIT_NEW_PAGE,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogPtr - sLogBuf,
                           smpGetTbsId( aNewChildPage ),
                           smpGetPageId( aNewChildPage ),
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );


    /* org page's next page id */
    sNewChildPid = smpGetPageId( aNewChildPage );
    SMNMPB_GET_LOGICAL_HEADER(aOrgChildPage)->mNextPageId = sNewChildPid;
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_MEMORY_BTREE_SET_NEXT_PAGE_ID,
                           SMR_REDO_TARGET_PAGE,
                           (void*)&sNewChildPid,
                           STL_SIZEOF(smlPid),
                           smpGetTbsId( aOrgChildPage ),
                           smpGetPageId( aOrgChildPage ),
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    if( SMNMPB_IS_LAST_SIBLING(aOrgChildPage) == STL_TRUE )
    {
        SMNMPB_GET_LOGICAL_HEADER(aOrgChildPage)->mFlag &= ~SMNMPB_NODE_FLAG_LAST_SIBLING_MASK;
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_SET_NODE_FLAG,
                               SMR_REDO_TARGET_PAGE,
                               (void*)&(SMNMPB_GET_LOGICAL_HEADER(aOrgChildPage)->mFlag),
                               STL_SIZEOF(stlUInt8),
                               smpGetTbsId( aOrgChildPage ),
                               smpGetPageId( aOrgChildPage ),
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief 주어진 Leaf 노드에 해당 key를 Insert한다.
 *        필요한 경우 SMO가 발생할 수 있다
 * @param[in] aMiniTrans Latch를 잡고 logging을 하는데 사용될 mini transaction
 * @param[in] aRelationHandle Insert 하려는 Index의 Relation Handle
 * @param[in] aPageHandle target leaf page id
 * @param[in] aSlotSeq insert할 key가 들어갈 순서
 * @param[in] aValColumns insert할 key 값
 * @param[in] aBlockIdx aValColumns에서 사용할 block idx
 * @param[in] aRowRid insert할 key가 가리키는 row의 위치
 * @param[in] aUndoRid Key와 대응되는 Undo record의 위치
 * @param[in] aTcn insert할 key의 Statement Tcn
 * @param[in] aKeyBodySize insert할 key의 Key value를 기록한 부분의 길이
 * @param[in] aPathStack Index를 root부터 탐색해 내려온 길에 대한 정보
 * @param[in] aIsIndexBuilding 현재 build index 중인지에 대한 flag
 * @param[in] aNeedRetry Smo 도중에 Root 생성시 다른 Smo operation과 충돌나서 다시해야 함
 * @param[in] aIsSmoOccurred Insert Key 도중에 SMO가 발생했는지 여부
 * @param[out] aEmptyPageCount SMO로 인해 빈키만 있는 page의 수
 * @param[out] aEmptyPid SMO로 인해 insert되는 반대쪽 노드에 빈 키만 있는 경우의 해당 PID
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbInsertIntoLeaf( smxmTrans         * aMiniTrans,
                                void              * aRelationHandle,
                                smpHandle         * aPageHandle,
                                stlInt16            aSlotSeq,
                                knlValueBlockList * aValColumns,
                                stlInt32            aBlockIdx,
                                smlRid            * aRowRid,
                                smlRid            * aUndoRid,
                                smlTcn              aTcn,
                                stlUInt16           aKeyBodySize,
                                smnmpbPathStack   * aPathStack,
                                stlBool             aIsIndexBuilding,
                                stlInt32          * aNeedRetry,
                                stlBool           * aIsSmoOccurred,
                                stlInt16          * aEmptyPageCount,
                                smlPid            * aEmptyPid,
                                smlEnv            * aEnv )
{
    smxlTransId  sTransId = smxlGetTransId(smxmGetTransId( aMiniTrans ));
    smlScn       sViewScn = smxlGetTransViewScn( sTransId );
    stlUInt8     sNewRtsSeq;
    stlUInt8     sNewInsertRtsVerNo;
    stlInt16     sRetryPhase = 0;
    stlUInt16    sSlotHdrSize = SMNMPB_RBK_HDR_SIZE;
    stlChar    * sKeySlot = NULL;
    smlRid       sUndoRid;
    stlChar      sLogBuf[SMP_PAGE_SIZE];
    stlChar    * sPtr;
    stlChar    * sKeyBody;
    stlBool      sIsFreed;
    stlUInt16    sTmpSlotHdrSize;
    stlUInt8     sInsTcn = aTcn;
    stlUInt8     sDelTcn = SMNMPB_INVALID_TCN;

    STL_ASSERT( aKeyBodySize > 0 );

    *aNeedRetry = SMNMPB_RETRY_NONE;
    *aIsSmoOccurred = STL_FALSE;
    *aEmptyPageCount = 0;
    
    STL_TRY_THROW( aKeyBodySize <= SMN_MAX_KEY_BODY_SIZE,
                   RAMP_ERR_TOO_LARGE_KEY);

    /* Log buffer를 이용하여 key body를 미리 만들어 놓는다. */
    sPtr = sLogBuf;
    STL_WRITE_INT32( sPtr, &aRowRid->mPageId );
    sPtr += STL_SIZEOF(smlPid);
    STL_WRITE_INT16( sPtr, &aRowRid->mOffset );
    sPtr += STL_SIZEOF(stlInt16);
    smnmpbWriteKeyBody( sPtr, aValColumns, aBlockIdx );
    sKeyBody = sPtr;
    sPtr += aKeyBodySize;

    /* 1. RTS 할당 */
    while( STL_TRUE )
    {
        if( sRetryPhase == 1 )
        {
            /* aging 할 필요가 있으면 aging 한다 */
            if( smpGetReclaimedSpace(SMP_FRAME(aPageHandle)) >=
                 sSlotHdrSize + aKeyBodySize + STL_SIZEOF(smpOffsetSlot) )
            {
                STL_TRY( smnmpbDoAgingLeafNode( aRelationHandle,
                                                aPageHandle,
                                                &aSlotSeq,
                                                STL_FALSE, /* aNeedRowStamping */
                                                &sIsFreed,
                                                aEnv ) == STL_SUCCESS );
            }

            /* aging 등으로 인해 빈 공간이 생겼으면 compact 한다 */
            if( (smpGetTotalFreeSpace(SMP_FRAME(aPageHandle)) >=
                 sSlotHdrSize + aKeyBodySize + STL_SIZEOF(smpOffsetSlot)) )
            {
                STL_TRY( smnmpbCompactPage( aRelationHandle,
                                            aPageHandle,
                                            STL_INT16_MAX,
                                            aEnv ) == STL_SUCCESS );
            }
            else
            {
                /* Aging 후 compact 해도 insert할 공간이 없음. split으로 */
                sRetryPhase++;
                continue;
            }
        }
        else if( sRetryPhase == 2 )
        {
            *aIsSmoOccurred = STL_TRUE;

            /* 모든 SMO을 수행하기 전에 undo 공간을 확보함으로써                      */
            /* undo 공간이 모자라 operation 중에 rollback이 되는 경우를 사전에 방지함 */
            if( aIsIndexBuilding != STL_TRUE )
            {
                STL_TRY( smxlPrepareUndoSpace( sTransId, 1, aEnv ) == STL_SUCCESS );
            }

            /* split으로 인해 aPageHandle, aSlotSeq가 변경될 수 있다 */
            STL_TRY( smnmpbSplitLeafNode( aMiniTrans,
                                          aRelationHandle,
                                          aPageHandle, /* in,out */
                                          sKeyBody,
                                          aKeyBodySize,
                                          aRowRid,
                                          &aSlotSeq, /* in,out */
                                          aPathStack,
                                          aNeedRetry,
                                          aEmptyPageCount,
                                          aEmptyPid,
                                          aEnv ) == STL_SUCCESS );
            STL_TRY_THROW( *aNeedRetry == SMNMPB_RETRY_NONE, RAMP_RETRY_FROM_ROOT );
        }
        else if( sRetryPhase == 3 )
        {
            /* Split 했는데도 insert하지 못함 */
            STL_ASSERT( STL_FALSE );
        }
        else
        {
            /* do nothing */
        }

        /* compact, split 이후에는 rts-bound key 형태로 추진한다 */
        sSlotHdrSize = SMNMPB_RBK_HDR_SIZE;

        if( (SMXL_IS_UNDO_BOUND_TRANS( sTransId ) == STL_TRUE) &&
            (aIsIndexBuilding != STL_TRUE) )
        {
            /* DS mode가 아니면 RTS 할당 받음 */
            STL_TRY( smpAllocRts( aMiniTrans,
                                  aPageHandle,
                                  sTransId,
                                  SML_MAKE_VIEW_SCN(sViewScn),
                                  smnmpbStabilizeKey,
                                  &sNewRtsSeq,
                                  aEnv ) == STL_SUCCESS );

            if( SMP_IS_VALID_RTS_SEQ(sNewRtsSeq) != STL_TRUE )
            {
                /* RTS 할당 실패 */
                sSlotHdrSize = SMNMPB_TBK_HDR_SIZE;
            }
        }
        else
        {
            /* DS mode면 */
            sNewRtsSeq = SMP_RTS_SEQ_STABLE;
        }

        /* 2. Key Slot 할당 */
        STL_TRY( smpAllocNthSlot( aPageHandle,
                                  aSlotSeq,
                                  sSlotHdrSize + aKeyBodySize,
                                  STL_TRUE, /* IsForTest */
                                  &sKeySlot ) == STL_SUCCESS );

        if( sKeySlot == NULL )
        {
            /* Key Slot 할당 실패 */
            sRetryPhase++;
            continue;
        }
        else
        {
            /* 모든 과정 성공 */
            break;
        }
    } /* while */

    /* write undo record. logical undo를 위해 key value와 row rid를 저장 */
    if( aIsIndexBuilding != STL_TRUE )
    {
        if( SML_IS_INVALID_RID( *aUndoRid ) == STL_TRUE )
        {
            STL_TRY( smxlInsertUndoRecord( aMiniTrans,
                                           SMG_COMPONENT_INDEX,
                                           SMN_UNDO_LOG_MEMORY_BTREE_INSERT_LEAF,
                                           sLogBuf,
                                           sPtr - sLogBuf,
                                           ((smlRid){ aPageHandle->mPch->mTbsId,
                                                   aSlotSeq,
                                                   aPageHandle->mPch->mPageId }),
                                           &sUndoRid,
                                           aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smnmpbMarkCompletionInUndoRecord( aMiniTrans,
                                                       aUndoRid,
                                                       aEnv )
                     == STL_SUCCESS );
        }
    }

    /* 이후로는 실패하면 안됨 */

    if( SMP_IS_VALID_RTS_SEQ(sNewRtsSeq ) == STL_TRUE )
    {
        /* DS mode가 아니면 */
        STL_TRY( smpBeginUseRts( aPageHandle,
                                 sKeySlot,
                                 sTransId,
                                 SML_MAKE_VIEW_SCN(sViewScn),
                                 sNewRtsSeq,
                                 &sNewInsertRtsVerNo,
                                 aEnv ) == STL_SUCCESS );
    }
    else
    {
        sNewInsertRtsVerNo = 0;
    }

    /* 실제로 key slot을 할당받음 */
    STL_TRY( smpAllocNthSlot( aPageHandle,
                              aSlotSeq,
                              sSlotHdrSize + aKeyBodySize,
                              STL_FALSE, /* IsForTest */
                              &sKeySlot ) == STL_SUCCESS );

    /* 할당된 key slot에 key image write */
    smnmpbWriteKeyHeader( sKeySlot,
                          STL_FALSE,
                          sSlotHdrSize == SMNMPB_TBK_HDR_SIZE ? STL_TRUE : STL_FALSE,
                          sNewRtsSeq,
                          sNewInsertRtsVerNo,
                          sTransId,
                          sViewScn,
                          aRowRid );
    
    if( aTcn >= SMNMPB_MAX_TCN )
    {
        sInsTcn = SMNMPB_INVALID_TCN;
    }

    SMNMPB_SET_INS_TCN( sKeySlot, &sInsTcn );
    SMNMPB_SET_DEL_TCN( sKeySlot, &sDelTcn );
    
    SMNMPB_GET_LEAF_KEY_HDR_SIZE( sKeySlot, &sTmpSlotHdrSize );
    stlMemcpy( sKeySlot + sTmpSlotHdrSize,
               sLogBuf + STL_SIZEOF(smlPid) + STL_SIZEOF(stlInt16),
               aKeyBodySize );

    /* write REDO log */
    sPtr = sLogBuf;
    STL_WRITE_INT64( sPtr, &sTransId );
    sPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( sPtr, &sViewScn );
    sPtr += STL_SIZEOF(smlScn);
    stlMemcpy( sPtr, sKeySlot, sSlotHdrSize + aKeyBodySize);
    sPtr +=  sSlotHdrSize + aKeyBodySize;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_MEMORY_BTREE_INSERT_LEAF,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sPtr - sLogBuf,
                           aPageHandle->mPch->mTbsId,
                           aPageHandle->mPch->mPageId,
                           aSlotSeq,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    smpSetMaxViewScn( aPageHandle, sViewScn );

    STL_RAMP( RAMP_RETRY_FROM_ROOT );

    STL_TRY( smnmpbValidateIndexPage( aPageHandle,
                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LARGE_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXCEEDED_MAX_KEY_SIZE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      SMN_MAX_KEY_BODY_SIZE );
    }
 
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 Leaf 노드 해당 Key를 dupkey로 만든다.
 *        필요한 경우 SMO가 발생할 수 있다
 * @param[in] aMiniTrans Latch를 잡고 logging을 하는데 사용될 mini transaction
 * @param[in] aRelationHandle Insert 하려는 Index의 Relation Handle
 * @param[in] aPageHandle target leaf page id
 * @param[in] aSlotSeq insert할 key가 들어갈 순서
 * @param[in] aValColumns insert할 key 값
 * @param[in] aBlockIdx aValColumns에서 사용할 block idx
 * @param[in] aRowRid insert할 key가 가리키는 row의 위치
 * @param[in] aUndoRid Key와 대응되는 Undo record의 위치
 * @param[in] aTcn insert할 key의 Statement Tcn
 * @param[in] aKeyBodySize insert할 key의 Key value를 기록한 부분의 길이
 * @param[in] aPathStack Index를 root부터 탐색해 내려온 길에 대한 정보
 * @param[out] aNeedRetry Smo 도중에 Root 생성시 다른 Smo operation과 충돌나서 다시해야 함
 * @param[out] aIsSmoOccurred Smo가 발생하였는지 여부
 * @param[out] aEmptyPageCount SMO로 인해 빈키만 있는 page의 수
 * @param[out] aEmptyPid SMO로 인해 insert되는 반대쪽 노드에 빈 키만 있는 경우의 해당 PID
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbMakeDupKey( smxmTrans         * aMiniTrans,
                            void              * aRelationHandle,
                            smpHandle         * aPageHandle,
                            stlInt16            aSlotSeq,
                            knlValueBlockList * aValColumns,
                            stlInt32            aBlockIdx,
                            smlRid            * aRowRid,
                            smlRid            * aUndoRid,
                            smlTcn              aTcn,
                            stlUInt16           aKeyBodySize,
                            smnmpbPathStack   * aPathStack,
                            stlInt32          * aNeedRetry,
                            stlBool           * aIsSmoOccurred,
                            stlInt16          * aEmptyPageCount,
                            smlPid            * aEmptyPid,
                            smlEnv            * aEnv )
{
    smxlTransId  sTransId = smxlGetTransId(smxmGetTransId( aMiniTrans ));
    smlScn       sViewScn = smxlGetTransViewScn( sTransId );
    stlInt16     sRetryPhase = 0;
    stlUInt16    sOrgKeySize;
    stlUInt16    sOrgKeyHdrSize;
    stlUInt16    sNewKeySize = SMNMPB_TBK_HDR_SIZE + aKeyBodySize;
    stlChar    * sKeySlot = NULL;
    stlChar    * sOrgKeySlot = NULL;
    stlUInt8     sInsertRtsSeq = SMP_RTS_SEQ_NULL;
    stlUInt8     sDeleteRtsSeq = SMP_RTS_SEQ_NULL;
    stlUInt8     sNewRtsSeq;
    stlUInt8     sInsertRtsVerNo = 0;
    stlUInt8     sNewInsertRtsVerNo;
    stlUInt8     sDeleteRtsVerNo = 0;
    smlRid       sUndoRid;
    stlChar      sLogBuf[SMP_PAGE_SIZE];
    stlChar    * sPtr;
    stlInt16     sAftSlotSeq;
    stlBool      sIsFreed;
    stlUInt8     sTmpRtsSeq;
    stlUInt8     sTmpRtsVerNo;
    smxlTransId  sTmpTransId;
    smlScn       sTmpScn;
    stlUInt8     sInsTcn = aTcn;
    stlUInt8     sDelTcn = SMNMPB_INVALID_TCN;
    
    STL_ASSERT( aKeyBodySize > 0 );

    *aIsSmoOccurred = STL_FALSE;
    *aNeedRetry = SMNMPB_RETRY_NONE;
    *aEmptyPageCount = 0;

    STL_TRY_THROW( aKeyBodySize <= SMN_MAX_KEY_BODY_SIZE,
                   RAMP_ERR_TOO_LARGE_KEY);

    /* 1. 기존 정보 채취 */
    sOrgKeySlot = smpGetNthRowItem(aPageHandle, aSlotSeq);
    STL_ASSERT( SMNMPB_IS_DELETED(sOrgKeySlot) == STL_TRUE );

    while( STL_TRUE )
    {
        if( sRetryPhase == 1 )
        {
            /* aging 할 필요가 있으면 aging 한다 */
            if( smpGetReclaimedSpace(SMP_FRAME(aPageHandle)) >= sNewKeySize + STL_SIZEOF(smpOffsetSlot) )
            {
                sAftSlotSeq = aSlotSeq;
                STL_TRY( smnmpbDoAgingLeafNode( aRelationHandle,
                                                aPageHandle,
                                                &sAftSlotSeq,
                                                STL_FALSE, /* aNeedRowStamping */
                                                &sIsFreed,
                                                aEnv ) == STL_SUCCESS );
                if( sIsFreed == STL_TRUE )
                {
                    /* dupkey로 만들려 했던 key가 aging되어서, 일반 insert로 전환한다 */
                    STL_TRY( smnmpbInsertIntoLeaf( aMiniTrans,
                                                   aRelationHandle,
                                                   aPageHandle,
                                                   sAftSlotSeq,
                                                   aValColumns,
                                                   aBlockIdx,
                                                   aRowRid,
                                                   aUndoRid,
                                                   aTcn,
                                                   aKeyBodySize,
                                                   aPathStack,
                                                   STL_FALSE, /* aIsIndexBuilding */
                                                   aNeedRetry,
                                                   aIsSmoOccurred,
                                                   aEmptyPageCount,
                                                   aEmptyPid,
                                                   aEnv ) == STL_SUCCESS );
                    STL_THROW( RAMP_FINISH_INSERT );

                }
                if( aSlotSeq != sAftSlotSeq )
                {
                    /* aging 연산으로 insert 해야 할 위치가 변경되었음 */
                    aSlotSeq = sAftSlotSeq;
                }
            }
            if( (smpGetTotalFreeSpace(SMP_FRAME(aPageHandle)) >= sNewKeySize + STL_SIZEOF(smpOffsetSlot)) )
            {
                STL_TRY( smnmpbCompactPage( aRelationHandle,
                                            aPageHandle,
                                            STL_INT16_MAX,
                                            aEnv ) == STL_SUCCESS );
                sOrgKeySlot = smpGetNthRowItem(aPageHandle, aSlotSeq);
            }
            else
            {
                /* aging 후 compact를 해도 공간이 부족하여 split으로 전환한다 */
                sRetryPhase++;
                continue;
            }
        }
        else if( sRetryPhase == 2 )
        {
            *aIsSmoOccurred = STL_TRUE;
            sAftSlotSeq = aSlotSeq;

            /* 모든 SMO을 수행하기 전에 undo 공간을 확보함으로써                      */
            /* undo 공간이 모자라 operation 중에 rollback이 되는 경우를 사전에 방지함 */
            STL_TRY( smxlPrepareUndoSpace( sTransId, SMP_PAGE_SIZE, aEnv ) == STL_SUCCESS );

            STL_TRY( smnmpbSplitLeafNode( aMiniTrans,
                                          aRelationHandle,
                                          aPageHandle, /* in,out */
                                          NULL,
                                          0,
                                          aRowRid,
                                          &sAftSlotSeq, /* in,out */
                                          aPathStack,
                                          aNeedRetry,
                                          aEmptyPageCount,
                                          aEmptyPid,
                                          aEnv ) == STL_SUCCESS );
            STL_TRY_THROW( *aNeedRetry == SMNMPB_RETRY_NONE, RAMP_RETRY_FROM_ROOT );
            /* split 연산으로 insert 해야 할 위치가 변경되었음 */
            aSlotSeq = sAftSlotSeq;
            sOrgKeySlot = smpGetNthRowItem(aPageHandle, aSlotSeq);
        }
        else if( sRetryPhase == 3 )
        {
            /* Split 했는데도 insert하지 못함 */
            STL_ASSERT( STL_FALSE );
        }
        else
        {
            /* do nothing */
        }

        if( SMXL_IS_UNDO_BOUND_TRANS( sTransId ) == STL_TRUE )
        {
            /* DS mode가 아니면 RTS 할당 받음 */
            STL_TRY( smpAllocRts( aMiniTrans,
                                  aPageHandle,
                                  sTransId,
                                  SML_MAKE_VIEW_SCN(sViewScn),
                                  smnmpbStabilizeKey,
                                  &sNewRtsSeq,
                                  aEnv ) == STL_SUCCESS );
            if( SMP_IS_VALID_RTS_SEQ(sNewRtsSeq) != STL_TRUE )
            {
                /* RTS 할당 실패. do nothing */
            }
        }
        else
        {
            /* DS mode면 */
            sNewRtsSeq = SMP_RTS_SEQ_STABLE;
        }

        if( SMNMPB_IS_TBK( sOrgKeySlot ) != STL_TRUE )
        {
            /* Key Slot 할당 */
            STL_TRY( smpReallocNthSlot( aPageHandle,
                                        aSlotSeq,
                                        sNewKeySize,
                                        STL_TRUE, /* aIsForTest */
                                        &sKeySlot ) == STL_SUCCESS );

            if( sKeySlot == NULL )
            {
                /* Key Slot 할당 실패 */
                sRetryPhase++;
                continue;
            }
            else
            {
                /* 모든 과정 성공 */
                break;
            }
        }
        else
        {
            /* 이미 dup key 형태라 새로 key slot을 할당할 필요가 없음 */
            sKeySlot = sOrgKeySlot;
            break;
        }
    } /* while */

    /* insert key에대한 undo log  작성 */
    sPtr = sLogBuf;
    STL_WRITE_INT32( sPtr, &aRowRid->mPageId );
    sPtr += STL_SIZEOF(smlPid);
    STL_WRITE_INT16( sPtr, &aRowRid->mOffset );
    sPtr += STL_SIZEOF(stlInt16);
    sOrgKeySize = smnmpbGetLeafKeySize( sOrgKeySlot, SMNMPB_GET_KEY_COLUMN_COUNT(aPageHandle) );
    SMNMPB_GET_LEAF_KEY_HDR_SIZE( sOrgKeySlot, &sOrgKeyHdrSize );
    stlMemcpy( sPtr, sOrgKeySlot + sOrgKeyHdrSize, sOrgKeySize - sOrgKeyHdrSize );
    sPtr += sOrgKeySize - sOrgKeyHdrSize;

    if( SML_IS_INVALID_RID( *aUndoRid ) == STL_TRUE )
    {
        STL_TRY( smxlInsertUndoRecord( aMiniTrans,
                                       SMG_COMPONENT_INDEX,
                                       SMN_UNDO_LOG_MEMORY_BTREE_DUPKEY,
                                       sLogBuf,
                                       sPtr - sLogBuf,
                                       ((smlRid){ aPageHandle->mPch->mTbsId,
                                               aSlotSeq,
                                               aPageHandle->mPch->mPageId }),
                                       &sUndoRid,
                                       aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smnmpbMarkCompletionInUndoRecord( aMiniTrans,
                                                   aUndoRid,
                                                   aEnv )
                 == STL_SUCCESS );
    }
      
    /* !! 이후로는 실패하면 안됨 !! */
    if( SMP_IS_VALID_RTS_SEQ( sNewRtsSeq ) == STL_TRUE )
    {
        /* DS mode가 아니면 */
        STL_TRY( smpBeginUseRts( aPageHandle,
                                 sKeySlot,
                                 sTransId,
                                 SML_MAKE_VIEW_SCN(sViewScn),
                                 sNewRtsSeq,
                                 &sNewInsertRtsVerNo,
                                 aEnv ) == STL_SUCCESS );
    }

    if( SMNMPB_IS_TBK( sOrgKeySlot ) != STL_TRUE )
    {
        /* Key Slot 할당 */
        STL_TRY( smpReallocNthSlot( aPageHandle,
                                    aSlotSeq,
                                    sNewKeySize,
                                    STL_FALSE, /* aIsForTest */
                                    &sKeySlot ) == STL_SUCCESS );
        SMP_REMOVE_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), STL_SIZEOF(smpOffsetSlot) );
    }
    else
    {
        SMP_REMOVE_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sOrgKeySize + STL_SIZEOF(smpOffsetSlot) );
    }

    /* 기존 RTS들과 관계를 끊는다 */
    SMNMPB_GET_INS_RTS_INFO( sOrgKeySlot, &sInsertRtsSeq, &sInsertRtsVerNo );
    SMNMPB_GET_DEL_RTS_INFO( sOrgKeySlot, &sDeleteRtsSeq, &sDeleteRtsVerNo );
    if( SMP_IS_VALID_RTS_SEQ(sInsertRtsSeq) == STL_TRUE )
    {
        STL_TRY( smpEndUseRts( aPageHandle,
                               sOrgKeySlot,
                               sInsertRtsSeq,
                               sInsertRtsVerNo,
                               aEnv ) == STL_SUCCESS );
    }
    if( SMP_IS_VALID_RTS_SEQ(sDeleteRtsSeq) == STL_TRUE )
    {
        STL_TRY( smpEndUseRts( aPageHandle,
                               sOrgKeySlot,
                               sDeleteRtsSeq,
                               sDeleteRtsVerNo,
                               aEnv ) == STL_SUCCESS );
    }

    /* make Key */
    if( SMNMPB_IS_TBK( sOrgKeySlot ) == STL_TRUE )
    {
        STL_ASSERT( sKeySlot == sOrgKeySlot );
    }
    else
    {
        /* RBK -> TBK */
        smnmpbWriteKeyHeader( sKeySlot,
                              STL_TRUE,
                              STL_TRUE,
                              sNewRtsSeq,
                              sNewInsertRtsVerNo,
                              sTransId,
                              sViewScn,
                              aRowRid );

        SMNMPB_SET_TBK( sKeySlot );
        stlMemcpy( sKeySlot + SMNMPB_TBK_HDR_SIZE,
                   sOrgKeySlot + SMNMPB_RBK_HDR_SIZE,
                   sOrgKeySize - SMNMPB_RBK_HDR_SIZE );
    }

    if( aTcn >= SMNMPB_MAX_TCN )
    {
        sInsTcn = SMNMPB_INVALID_TCN;
    }
    
    SMNMPB_SET_INS_TCN( sKeySlot, &sInsTcn );
    SMNMPB_SET_DEL_TCN( sKeySlot, &sDelTcn );
    SMNMPB_UNSET_DELETED( sKeySlot );
    SMNMPB_SET_DUPKEY( sKeySlot );
    SMNMPB_SET_INS_RTS_INFO( sKeySlot, &sNewRtsSeq, &sNewInsertRtsVerNo );
    sTmpRtsSeq = SMP_RTS_SEQ_NULL;
    sTmpRtsVerNo = 0;
    SMNMPB_SET_DEL_RTS_INFO( sKeySlot, &sTmpRtsSeq, &sTmpRtsVerNo );
    SMNMPB_SET_TBK_INS_TRANS_ID( sKeySlot, &sTransId );
    sTmpScn = SML_MAKE_VIEW_SCN(sViewScn);
    SMNMPB_SET_TBK_INS_SCN( sKeySlot, &sTmpScn );
    sTmpTransId = SML_INVALID_TRANSID;
    sTmpScn = SML_INFINITE_SCN;
    SMNMPB_SET_TBK_DEL_TRANS_ID( sKeySlot, &sTmpTransId );
    SMNMPB_SET_TBK_DEL_SCN( sKeySlot, &sTmpScn );

    /* write REDO log */
    sPtr = sLogBuf;
    STL_WRITE_INT64( sPtr, &sTransId );
    sPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( sPtr, &sViewScn );
    sPtr += STL_SIZEOF(smlScn);
    stlMemcpy( sPtr, sKeySlot, sNewKeySize);
    sPtr +=  sNewKeySize;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_MEMORY_BTREE_DUPKEY,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sPtr - sLogBuf,
                           aPageHandle->mPch->mTbsId,
                           aPageHandle->mPch->mPageId,
                           aSlotSeq,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    smpSetMaxViewScn( aPageHandle, sViewScn );

    STL_RAMP( RAMP_RETRY_FROM_ROOT );
    STL_RAMP( RAMP_FINISH_INSERT );

    STL_TRY( smnmpbValidateIndexPage( aPageHandle,
                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LARGE_KEY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_EXCEEDED_MAX_KEY_SIZE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      SMN_MAX_KEY_BODY_SIZE );
    }
 
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 key에 insert/delete될 leaf node를 root로 부터 찾아 내려간다
 * @param[in] aRelationHandle Key를 삽입/삭제할 Index의 Relation의 handle
 * @param[in] aMiniTrans Latch를 잡고 logging을 하는데 사용될 mini transaction
 * @param[in] aValColumns 삽입/삭제할 Key의 컬럼 정보들
 * @param[in] aBlockIdx aValColumns에서 사용할 block idx
 * @param[in] aRowRid 삽입/삭제할 key가 가리키는 row의 위치
 * @param[out] aPathStack root node로 부터 target leaf를 찾아내려간 node path의 기록
 * @param[out] aPageHandle 주어진 key가 삽입/삭제될 leaf node
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbFindTargetLeaf( void              * aRelationHandle,
                                smxmTrans         * aMiniTrans,
                                knlValueBlockList * aValColumns,
                                stlInt32            aBlockIdx,
                                smlRid            * aRowRid,
                                smnmpbPathStack   * aPathStack,
                                smpHandle         * aPageHandle,
                                smlEnv            * aEnv )
{
    smpHandle          sPageHandle;
    void             * sSegHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    stlBool            sHasLatch = STL_FALSE;
    stlUInt64          sCurSmoNo;
    stlUInt64          sPrevSmoNo;
    stlUInt8         * sKeyColFlags;
    dtlCompareResult   sResult;
    smlPid             sCurPid;
    smlPid             sChildPid;
    smlPid             sParentPid;
    stlInt16           sParentSlotSeq;
    stlUInt64          sParentSmoNo;
    smlPid             sNextPid;
    stlInt16           sKeySlotSeq;
    smnmpbStatistics * sStatistics;
    smxmSavepoint      sSavepoint;
    stlBool            sIsSemiLeafOrLeaf = STL_FALSE;
    smlTbsId           sTbsId = smsGetTbsId(sSegHandle);

    STL_RAMP( RAMP_RETRY_FROM_ROOT );

    aPathStack->mDepth = 0;

    /* Root에 대한 SMO시 relation header가 SMO-> RootPid 순으로 */
    /* 변경되기때문에 읽을때는 RootPid->SMO 순으로 읽어야 한다  */
    /* 단, Root 페이지의 SMO가 relation header와 같을때는       */
    /* 현재의 root pid와 같은지 한번 더 확인이 필요하다         */
    sCurPid = SMN_GET_ROOT_PAGE_ID(aRelationHandle);

    /* 순서를 보장하기 위해 memory barrier를 사용한다 */
    stlMemBarrier();

    sPrevSmoNo = SMN_GET_INDEX_SMO_NO(aRelationHandle);
    sStatistics = SMNMPB_GET_MODULE_SPECIFIC_DATA( aRelationHandle );

    STL_TRY( smnmpbFindChildNodeInRoot( aRelationHandle,
                                        aMiniTrans,
                                        aValColumns,
                                        aBlockIdx,
                                        aRowRid,
                                        aPathStack,
                                        &sChildPid,
                                        &sIsSemiLeafOrLeaf,
                                        aEnv )
             == STL_SUCCESS );

    STL_TRY_THROW( sIsSemiLeafOrLeaf == STL_FALSE, RAMP_SKIP_TRAVERSE );

    sCurPid = sChildPid;
    
    while( STL_TRUE )
    {
        /* 하위로 진행 */
        STL_TRY( smpAcquire( NULL,
                             sTbsId,
                             sCurPid,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        sHasLatch = STL_TRUE;

        if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
        {
            /* 이미 Free된 Page에 도달. 처음서 부터 다시 시도 */
            sHasLatch = STL_FALSE;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
            STL_THROW( RAMP_RETRY_FROM_ROOT );
        }
        
        aPathStack->mStack[aPathStack->mDepth].mPid = sCurPid;

        if( SMNMPB_IS_LEAF(&sPageHandle) == STL_TRUE )
        {
            sHasLatch = STL_FALSE;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );

            sChildPid = sCurPid;
            break;
        }
       
        while( STL_TRUE )
        {
            /* Sibling으로 진행 */
            sCurSmoNo = SMN_GET_PAGE_SMO_NO( &sPageHandle );
            sKeyColFlags = SMNMPB_GET_KEY_COLUMN_FLAGS(&sPageHandle);

            /* SMO 발견. 현재 노드에 들어가야 되는지 비교 */
            STL_TRY( smnmpbCompareWithMaxKey( &sPageHandle,
                                              aValColumns,
                                              aBlockIdx,
                                              aRowRid,
                                              sKeyColFlags,
                                              &sResult,
                                              aEnv ) == STL_SUCCESS );
            
            if( sResult == DTL_COMPARISON_LESS )
            {
                sParentPid = aPathStack->mStack[aPathStack->mDepth - 1].mPid;
                sParentSlotSeq = aPathStack->mStack[aPathStack->mDepth - 1].mSeq;
                sParentSmoNo = aPathStack->mStack[aPathStack->mDepth - 1].mSmoNo;
                
                STL_TRY( smnmpbCompareWithParentKey( sTbsId,
                                                     sParentPid,
                                                     sParentSlotSeq,
                                                     sParentSmoNo,
                                                     sCurPid,
                                                     aValColumns,
                                                     aBlockIdx,
                                                     aRowRid,
                                                     sKeyColFlags,
                                                     &sResult,
                                                     aEnv ) == STL_SUCCESS );

                if( sResult == DTL_COMPARISON_LESS )
                {
                    /* parent key가 더 작음 -> 다음 sibling에 들어가야 함 */
                    sNextPid = SMNMPB_GET_NEXT_PAGE(&sPageHandle);
                    if( sNextPid == SMP_NULL_PID )
                    {
                        /* 마지막 sibling 노드임 */
                        STL_ASSERT( SMNMPB_IS_LAST_SIBLING( &sPageHandle ) == STL_TRUE );
                        sChildPid = smnmpbGetLastChild( &sPageHandle );
                        sKeySlotSeq = SMP_GET_PAGE_ROW_ITEM_COUNT(&sPageHandle) - 1;
                        break;
                    }

                    SMNMPB_INC_STATISTICS( sStatistics, LinkTraverseCount );

                    /* 현재 노드에 들어갈 곳이 없음. 다음 sibling 노드로 */
                    
                    if( sHasLatch == STL_TRUE )
                    {
                        sHasLatch = STL_FALSE;
                        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
                    }
            
                    sCurPid = sNextPid;
                    STL_TRY( smpAcquire( NULL,
                                         sTbsId,
                                         sCurPid,
                                         KNL_LATCH_MODE_SHARED,
                                         &sPageHandle,
                                         aEnv ) == STL_SUCCESS );
                    sHasLatch = STL_TRUE;

                    /* Sibling으로 이동 중에는 Free Page를 만날 리가 없다. */
                    STL_DASSERT( smpGetFreeness( &sPageHandle ) != SMP_FREENESS_FREE );

                    continue;
                }
                else
                {
                    /* parent key가 더 큼 -> 현재 노드의 마지막에 들어가야 함 */
                }
            }
            else
            {
                /* SMO가 발생하였지만 현재 노드의 중간에 들어감 */
            }

            STL_TRY( smnmpbFindChildNode( &sPageHandle,
                                          aValColumns,
                                          aBlockIdx,
                                          aRowRid,
                                          sKeyColFlags,
                                          &sChildPid,
                                          &sKeySlotSeq,
                                          aEnv ) == STL_SUCCESS );
            STL_ASSERT( sChildPid != SMP_NULL_PID );
            break;
        } /* while */

        aPathStack->mStack[aPathStack->mDepth].mPid = sCurPid;
        aPathStack->mStack[aPathStack->mDepth].mSeq = sKeySlotSeq;
        aPathStack->mStack[aPathStack->mDepth].mSmoNo = sCurSmoNo;
        aPathStack->mDepth++;

        if( SMNMPB_IS_SEMI_LEAF( &sPageHandle ) == STL_TRUE )
        {
            if( sHasLatch == STL_TRUE )
            {
                sHasLatch = STL_FALSE;
                STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
            }
            
            break;
        }
        
        if( sHasLatch == STL_TRUE )
        {
            sHasLatch = STL_FALSE;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
        }
            
        sCurPid = sChildPid;
    }

    STL_RAMP( RAMP_SKIP_TRAVERSE );

    sCurPid = sChildPid;

    /* Phase 2 : leaf node sibling 이동 */
    STL_TRY( smxmMarkSavepoint( aMiniTrans, &sSavepoint, aEnv ) == STL_SUCCESS );
        
    KNL_BREAKPOINT( KNL_BREAKPOINT_SMNMPBDML_BEFORE_MOVE_TO_LEAF,
                    (knlEnv*)aEnv );

    STL_TRY( smpAcquire( aMiniTrans,
                         sTbsId,
                         sCurPid,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    aPathStack->mStack[aPathStack->mDepth].mPid = sCurPid;

    STL_ASSERT( SMNMPB_IS_LEAF(&sPageHandle) == STL_TRUE );

    if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
    {
        /* 이미 Free된 Page에 도달. 처음서 부터 다시 시도 */
        STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                          &sSavepoint, 
                                          aEnv ) == STL_SUCCESS );
        STL_THROW( RAMP_RETRY_FROM_ROOT );
    }
 
    while( STL_TRUE )
    {
        /* leaf Sibling으로 진행 */
        sCurSmoNo = SMN_GET_PAGE_SMO_NO( &sPageHandle );
        sKeyColFlags = SMNMPB_GET_KEY_COLUMN_FLAGS(&sPageHandle);

        if( sCurSmoNo > sPrevSmoNo )
        {
            if( aPathStack->mDepth == 0 )
            {
                /* root노드에서 SMO 발견 다시 시도 */
                STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                                  &sSavepoint, 
                                                  aEnv ) == STL_SUCCESS );
                STL_THROW( RAMP_RETRY_FROM_ROOT );
            }

            /**
             * Sibling으로 이동 중에는 Free Page를 만날 리가 없다.
             */
            STL_DASSERT( smpGetFreeness( &sPageHandle ) != SMP_FREENESS_FREE );

            STL_TRY( smnmpbCompareWithMaxKey( &sPageHandle,
                                              aValColumns,
                                              aBlockIdx,
                                              aRowRid,
                                              sKeyColFlags,
                                              &sResult,
                                              aEnv ) == STL_SUCCESS );

            if( sResult == DTL_COMPARISON_LESS )
            {
                sParentPid = aPathStack->mStack[aPathStack->mDepth - 1].mPid;
                sParentSlotSeq = aPathStack->mStack[aPathStack->mDepth - 1].mSeq;
                sParentSmoNo = aPathStack->mStack[aPathStack->mDepth - 1].mSmoNo;
                STL_TRY( smnmpbCompareWithParentKey( sTbsId,
                                                     sParentPid,
                                                     sParentSlotSeq,
                                                     sParentSmoNo,
                                                     sCurPid,
                                                     aValColumns,
                                                     aBlockIdx,
                                                     aRowRid,
                                                     sKeyColFlags,
                                                     &sResult,
                                                     aEnv ) == STL_SUCCESS );

                if( sResult == DTL_COMPARISON_LESS )
                {
                    /* Parent key가 더 작으므로 다음 노드를 확인 */
                    sNextPid = SMNMPB_GET_NEXT_PAGE(&sPageHandle);
                    /* 마지막 sibling 노드가 아니어야 함 */
                    STL_ASSERT( sNextPid != SMP_NULL_PID );

                    STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                                      &sSavepoint, 
                                                      aEnv ) == STL_SUCCESS );
                    sCurPid = sNextPid;

                    SMNMPB_INC_STATISTICS( sStatistics, LinkTraverseCount );

                    STL_TRY( smpAcquire( aMiniTrans,
                                         sTbsId,
                                         sCurPid,
                                         KNL_LATCH_MODE_EXCLUSIVE,
                                         &sPageHandle,
                                         aEnv ) == STL_SUCCESS );

                    aPathStack->mStack[aPathStack->mDepth].mPid = sCurPid;
                    continue;
                }
                else
                {
                    /* Parent key가 더 크므로 현재 페이지에 넣음 */
                }
            }
            else
            {
                /* SMO가 일어났지만 현재 노드의 중간에 들어간다 */
            }
        }
        else
        {
            /* SMO가 일어나지 않았으므로 현재 노드에 들어가야 함 */
        }

        /* target leaf를 찾음 */
        break;
    }

    *aPageHandle = sPageHandle;

    return STL_SUCCESS;

    STL_FINISH;

    if( sHasLatch == STL_TRUE )
    {
        (void)smpRelease( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
} 


/**
 * @brief Root Node에서 키에 맞는 Child Node을 얻는다.
 * @param[in]  aRelationHandle   Key를 삽입/삭제할 Index의 Relation의 handle
 * @param[in]  aMiniTrans        Latch를 잡고 logging을 하는데 사용될 mini transaction
 * @param[in]  aValColumns      삽입/삭제할 Key의 컬럼 정보들
 * @param[in]  aBlockIdx        aValColumns에서 사용할 block idx
 * @param[in]  aRowRid          삽입/삭제할 key가 가리키는 row의 위치
 * @param[out] aPathStack        root node로 부터 target leaf를 찾아내려간 node path의 기록
 * @param[out] aChildPid         Child Page Identifier
 * @param[out] aIsSemiLeafOrLeaf Child Page가 Leaf인지 여부
 * @param[in]  aEnv              Environment
 */
stlStatus smnmpbFindChildNodeInRoot( void              * aRelationHandle,
                                     smxmTrans         * aMiniTrans,
                                     knlValueBlockList * aValColumns,
                                     stlInt32            aBlockIdx,
                                     smlRid            * aRowRid,
                                     smnmpbPathStack   * aPathStack,
                                     smlPid            * aChildPid,
                                     stlBool           * aIsSemiLeafOrLeaf,
                                     smlEnv            * aEnv )
{
    smpHandle          sPageHandle;
    smpHandle          sDataPageHandle;
    void             * sSegHandle = SME_GET_SEGMENT_HANDLE(aRelationHandle);
    stlUInt64          sCurSmoNo;
    stlUInt64          sPrevSmoNo;
    stlUInt8         * sKeyColFlags;
    dtlCompareResult   sResult;
    smlPid             sCurPid;
    smlPid             sChildPid;
    stlInt16           sKeySlotSeq;
    smlTbsId           sTbsId = smsGetTbsId(sSegHandle);
    stlInt32           sState = 0;
    smlPid             sMirrorRootPid;
    stlUInt64          sExclLockSeqA = 0;
    stlUInt64          sExclLockSeqB = 0;
    stlBool            sIsTimedOut;

    STL_RAMP( RAMP_RETRY_FROM_ROOT );

    *aIsSemiLeafOrLeaf = STL_FALSE;
    aPathStack->mDepth = 0;

    sCurPid = SMN_GET_ROOT_PAGE_ID(aRelationHandle);
    stlMemBarrier();
    sPrevSmoNo = SMN_GET_INDEX_SMO_NO(aRelationHandle);
    sMirrorRootPid = SMN_GET_MIRROR_ROOT_PID( aRelationHandle );
    
    /**
     * Mirror Page가 있다면 
     */
        
    if( sMirrorRootPid != SMP_NULL_PID )
    {
        STL_TRY( smpFix( sTbsId,
                         sCurPid,
                         &sPageHandle,
                         aEnv )
                 == STL_SUCCESS );
        
        sExclLockSeqA = knlGetExclLockSeq( &sPageHandle.mPch->mPageLatch );
        
        STL_TRY( smpFix( sTbsId,
                         sMirrorRootPid,
                         &sDataPageHandle,
                         aEnv )
                 == STL_SUCCESS );
        sState = 1;

        /**
         * 현재 누군가가 락을 획득하고 있다면 Mirror Root를 이용하지 않는다.
         */
        
        if( sExclLockSeqA & STL_INT64_C(0x0000000000000001) )
        {
            STL_TRY( knlAcquireLatch( &sPageHandle.mPch->mPageLatch,
                                      KNL_LATCH_MODE_SHARED,
                                      KNL_LATCH_PRIORITY_NORMAL,
                                      STL_INFINITE_TIME,
                                      &sIsTimedOut,
                                      (knlEnv*)aEnv ) == STL_SUCCESS );
            sState = 2;

            STL_TRY( smpUnfix( &sDataPageHandle, aEnv ) == STL_SUCCESS );

            sDataPageHandle = sPageHandle;
        }
        else
        {
            /**
             * Root Node에 SMO가 발생한 후 아직 Mirror Root에 반영되지 않았을 경우 Retry
             */
            if( SMN_GET_PAGE_SMO_NO(&sPageHandle) != SMN_GET_PAGE_SMO_NO(&sDataPageHandle) )
            {
                STL_TRY( smpUnfix( &sDataPageHandle, aEnv ) == STL_SUCCESS );
                STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );

                sState = 0;
                STL_THROW( RAMP_RETRY_FROM_ROOT );
            }
        }
    }
    else
    {
        STL_TRY( smpAcquire( NULL,
                             sTbsId,
                             sCurPid,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );
        sState = 2;

        sDataPageHandle = sPageHandle;

        if( sCurPid != SMN_GET_ROOT_PAGE_ID(aRelationHandle) )
        {
            /**
             * Root Node SMO 발견 다시 시도
             */

            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
            STL_THROW( RAMP_RETRY_FROM_ROOT );
        }
    }
 
    if( smpGetFreeness( &sPageHandle ) == SMP_FREENESS_FREE )
    {
        /**
         * 이미 Free된 Page에 도달. 처음서 부터 다시 시도
         */

        if( sState == 2 )
        {
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smpUnfix( &sDataPageHandle, aEnv ) == STL_SUCCESS );
            STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
        }

        sState = 0;
        STL_THROW( RAMP_RETRY_FROM_ROOT );
    }
        
    aPathStack->mStack[aPathStack->mDepth].mPid = sCurPid;

    if( SMNMPB_IS_LEAF(&sPageHandle) == STL_TRUE )
    {
        if( sState == 2 )
        {
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smpUnfix( &sDataPageHandle, aEnv ) == STL_SUCCESS );
            STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
        }
        
        sState = 0;
        sChildPid = sCurPid;
        *aIsSemiLeafOrLeaf = STL_TRUE;
        
        STL_THROW( RAMP_FINISH );
    }
       
    sCurSmoNo = SMN_GET_PAGE_SMO_NO( &sPageHandle );
    sKeyColFlags = SMNMPB_GET_KEY_COLUMN_FLAGS(&sPageHandle);

    if( sCurSmoNo >= sPrevSmoNo )
    {
        if( SMN_GET_ROOT_PAGE_ID(aRelationHandle) != sCurPid )
        {
            /**
             * Root 노드에서 SMO 발견 다시 시도
             */
                    
            if( sState == 2 )
            {
                STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
            }
            else
            {
                STL_TRY( smpUnfix( &sDataPageHandle, aEnv ) == STL_SUCCESS );
                STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
            }
        
            sState = 0;
            
            STL_THROW( RAMP_RETRY_FROM_ROOT );
        }
    }

    /**
     * 현재 노드에 들어가야 되는지 비교
     */
    
    STL_TRY( smnmpbCompareWithMaxKey( &sDataPageHandle,
                                      aValColumns,
                                      aBlockIdx,
                                      aRowRid,
                                      sKeyColFlags,
                                      &sResult,
                                      aEnv ) == STL_SUCCESS );

    if( sResult == DTL_COMPARISON_LESS )
    {
        /**
         * Max Key 보다 크다면 SMO가 발생한 경우이다.
         */
        
        if( sState == 2 )
        {
            STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smpUnfix( &sDataPageHandle, aEnv ) == STL_SUCCESS );
            STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
        }
        
        sState = 0;
            
        STL_THROW( RAMP_RETRY_FROM_ROOT );
    }
    
    STL_TRY( smnmpbFindChildNode( &sDataPageHandle,
                                  aValColumns,
                                  aBlockIdx,
                                  aRowRid,
                                  sKeyColFlags,
                                  &sChildPid,
                                  &sKeySlotSeq,
                                  aEnv ) == STL_SUCCESS );
    STL_ASSERT( sChildPid != SMP_NULL_PID );

    /**
     * Mirror를 이용해서 Child Node를 찾는 경우라면
     */
    
    if( sState == 1 )
    {
        /**
         * Mirror Page를 사용하는 동안 Root Page의 변경이 있는지 검사한다.
         */
    
        sExclLockSeqB = knlGetExclLockSeq( &sPageHandle.mPch->mPageLatch );

        if( sExclLockSeqA != sExclLockSeqB )
        {
            STL_TRY( smpUnfix( &sDataPageHandle, aEnv ) == STL_SUCCESS );
            STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
        
            sState = 0;
            
            STL_THROW( RAMP_RETRY_FROM_ROOT );
        }
    }

    aPathStack->mStack[aPathStack->mDepth].mPid = sCurPid;
    aPathStack->mStack[aPathStack->mDepth].mSeq = sKeySlotSeq;
    aPathStack->mStack[aPathStack->mDepth].mSmoNo = sCurSmoNo;
    aPathStack->mDepth++;

    if( SMNMPB_IS_SEMI_LEAF( &sPageHandle ) == STL_TRUE )
    {
        *aIsSemiLeafOrLeaf = STL_TRUE;
    }

    if( sState == 2 )
    {
        STL_TRY( smpRelease( &sPageHandle, aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smpUnfix( &sDataPageHandle, aEnv ) == STL_SUCCESS );
        STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
    }
    sState = 0;
    
    STL_RAMP( RAMP_FINISH );
        
    *aChildPid = sChildPid;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 2 )
    {
        (void)smpRelease( &sPageHandle, aEnv );
    }
    else
    {
        (void)smpUnfix( &sDataPageHandle, aEnv );
        (void)smpUnfix( &sPageHandle, aEnv );
    }

    return STL_FAILURE;
} 


/**
 * @brief 주어진 Index에 새 Key를 삽입한다
 * @param[in] aStatement 사용중인 statement
 * @param[in] aRelationHandle Key를 삽입할 Index의 Relation의 handle
 * @param[in] aColumns 삽입할 Key의 컬럼 정보들
 * @param[in] aBlockIdx aColumns에서 사용할 block idx
 * @param[in] aRowRid 삽입할 key가 가리키는 row의 위치
 * @param[in] aUndoRid Key와 대응되는 Undo record의 위치
 * @param[in] aIsIndexBuilding 현재 build index 중인지에 대한 flag
 * @param[in] aIsDeferred uniqueness violation 을 deferred로 처리할지 여부
 * @param[out] aUniqueViolationScope 삽입할 key가 uniqueness violation을 발생시킨 범위
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbInsert( smlStatement            * aStatement,
                        void                    * aRelationHandle,
                        knlValueBlockList       * aColumns,
                        stlInt32                  aBlockIdx,
                        smlRid                  * aRowRid,
                        smlRid                  * aUndoRid,
                        stlBool                   aIsIndexBuilding,
                        stlBool                   aIsDeferred,
                        smlUniqueViolationScope * aUniqueViolationScope,
                        smlEnv                  * aEnv )
{
    knlValueBlockList * sCurCol;
    stlUInt16           sKeyBodySize = 0;
    stlUInt32           sAttr;
    smpHandle           sPageHandle;
    smnmpbPathStack     sPathStack;
    stlUInt16           sState = 0;
    stlBool             sCheckUniqueness = SMN_GET_UNIQUENESS(aRelationHandle);
    smxmTrans           sMiniTrans;
    smxmSavepoint       sSavepoint;
    stlInt16            sKeySlotSeq;
    stlBool             sFoundKey;
    stlInt32            sNeedRetry = SMNMPB_RETRY_NONE;
    stlBool             sIsSmoOccurred = STL_FALSE;
    stlUInt8            sColLenSize;
    dtlDataValue      * sValue;
    stlBool             sIsMyStmtViolation = STL_FALSE;
    stlBool             sIsUniqueViolated;
    stlInt16            sEmptyPageCount;
    stlInt32            sNoSpaceRetryCount = 0;
    smlPid              sEmptyPid[KNL_ENV_MAX_LATCH_DEPTH];

#ifdef STL_DEBUG
    stlInt32            i = 0;
#endif

    for( sCurCol = aColumns; sCurCol != NULL; sCurCol = sCurCol->mNext )
    {
        sValue = &(sCurCol->mValueBlock->mDataValue[aBlockIdx]);
        SMP_GET_COLUMN_LEN_SIZE( sValue->mLength, &sColLenSize );
        sKeyBodySize += (sColLenSize + sValue->mLength);

#ifdef STL_DEBUG
        STL_DASSERT( SMN_GET_KEY_COL_SIZE( aRelationHandle, i ) >= sValue->mLength );
        i++;
#endif
    }

    if( SMN_GET_LOGGING_FLAG( aRelationHandle ) == STL_TRUE )
    {
        sAttr = SMXM_ATTR_REDO;
    }
    else
    {
        smxlSetRepreparable( SMA_GET_TRANS_ID( aStatement ), STL_FALSE );
        sAttr = SMXM_ATTR_NOLOG_DIRTY;
    }
    
    STL_RAMP( RAMP_RETRY_FROM_ROOT );

    sNeedRetry = SMNMPB_RETRY_NONE;
    sIsSmoOccurred = STL_FALSE;
    sEmptyPageCount = 0;

    STL_TRY( smxmBegin( &sMiniTrans,
                        aStatement->mTransId,
                        smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smxmMarkSavepoint( &sMiniTrans, &sSavepoint, aEnv ) == STL_SUCCESS );

    /* find leaf */
    STL_TRY( smnmpbFindTargetLeaf( aRelationHandle,
                                   &sMiniTrans,
                                   aColumns,
                                   aBlockIdx,
                                   aRowRid,
                                   &sPathStack,
                                   &sPageHandle,
                                   aEnv ) == STL_SUCCESS );

    /* find insertable position & check uniqueness */
    *aUniqueViolationScope = SML_UNIQUE_VIOLATION_SCOPE_NONE;
    
    sPathStack.mStack[sPathStack.mDepth].mPid = smpGetPageId(&sPageHandle);
    sPathStack.mStack[sPathStack.mDepth].mSmoNo = SMN_GET_PAGE_SMO_NO( &sPageHandle );

    if( sCheckUniqueness == STL_TRUE )
    {
        /* Uniqueness 체크 해야 함 */
        STL_TRY( smnmpbCheckUniqueness( aStatement,
                                        &sMiniTrans,
                                        &sSavepoint,
                                        smsGetTbsId(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                                        aColumns,
                                        aBlockIdx,
                                        aRowRid,
                                        &sPageHandle,
                                        &sIsUniqueViolated,
                                        &sIsMyStmtViolation,
                                        &sNeedRetry,
                                        aEnv ) == STL_SUCCESS );

        if( sNeedRetry != SMNMPB_RETRY_NONE )
        {
            (void)smxmCommit( &sMiniTrans, aEnv );
            STL_THROW( RAMP_RETRY_FROM_ROOT );
        }

        if( sIsUniqueViolated == STL_TRUE )
        {
            if( sIsMyStmtViolation == STL_TRUE )
            {
                *aUniqueViolationScope = SML_UNIQUE_VIOLATION_SCOPE_STMT;
            }
            else
            {
                *aUniqueViolationScope = SML_UNIQUE_VIOLATION_SCOPE_ROW;
            }

            STL_TRY_THROW( aIsDeferred == STL_TRUE, RAMP_FINISH );
        }
    }

    STL_TRY( smnmpbFindPosition( aColumns,
                                 aBlockIdx,
                                 aRowRid,
                                 &sPageHandle,
                                 &sKeySlotSeq,
                                 &sFoundKey,
                                 aEnv ) == STL_SUCCESS );

    sPathStack.mStack[sPathStack.mDepth].mSeq = sKeySlotSeq;
    sPathStack.mDepth++;

    if( sFoundKey == STL_TRUE )
    {
        STL_ASSERT( aIsIndexBuilding != STL_TRUE );

        STL_TRY( smnmpbMakeDupKey( &sMiniTrans,
                                   aRelationHandle,
                                   &sPageHandle,
                                   sKeySlotSeq,
                                   aColumns,
                                   aBlockIdx,
                                   aRowRid,
                                   aUndoRid,
                                   SMA_GET_TCN( aStatement ),
                                   sKeyBodySize,
                                   &sPathStack,
                                   &sNeedRetry,
                                   &sIsSmoOccurred,
                                   &sEmptyPageCount,
                                   sEmptyPid,
                                   aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smnmpbInsertIntoLeaf( &sMiniTrans,
                                       aRelationHandle,
                                       &sPageHandle,
                                       sKeySlotSeq,
                                       aColumns,
                                       aBlockIdx,
                                       aRowRid,
                                       aUndoRid,
                                       SMA_GET_TCN( aStatement ),
                                       sKeyBodySize,
                                       &sPathStack,
                                       aIsIndexBuilding,
                                       &sNeedRetry,
                                       &sIsSmoOccurred,
                                       &sEmptyPageCount,
                                       sEmptyPid,
                                       aEnv ) == STL_SUCCESS );
    }

    if( sNeedRetry != SMNMPB_RETRY_NONE )
    {
        sState = 0;
        (void)smxmCommit( &sMiniTrans, aEnv );

        if( sNeedRetry == SMNMPB_RETRY_NO_SPACE )
        {
            /**
             * 무한 Retry를 방지한다.
             * - Error는 이미 Push된 상태이다.
             * - Empty Node가 하나 이상 존재해야 retry한다.
             */
            STL_TRY( sNoSpaceRetryCount == 0 );
            STL_TRY( SMN_GET_EMPTY_NODE_COUNT(aRelationHandle) > 0 );
            
            stlPopError( KNL_ERROR_STACK(aEnv) );
            STL_TRY( smnmpbProcessEmptyNodes( aRelationHandle,
                                              aStatement->mTransId,
                                              aEnv ) == STL_SUCCESS );
            sNoSpaceRetryCount++;
        }
        
        STL_THROW( RAMP_RETRY_FROM_ROOT );
    }

    STL_RAMP( RAMP_FINISH );
    
    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    /* phase 2 : SMO로 인해 empty node가 생긴 경우 empty node list에 달기 */
    if( sEmptyPageCount > 0 )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            aStatement->mTransId,
                            smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                            sAttr,
                            aEnv ) == STL_SUCCESS );
        sState = 1;

        STL_TRY( smnmpbRegisterEmptyNode( aRelationHandle,
                                          &sMiniTrans,
                                          sEmptyPageCount,
                                          sEmptyPid,
                                          STL_TRUE, /* aCheckAgingFlag */
                                          aEnv ) == STL_SUCCESS );
        
        sState = 0;
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
    }

    if( sIsSmoOccurred == STL_TRUE )
    {
        STL_TRY( smnmpbProcessEmptyNodes( aRelationHandle,
                                          aStatement->mTransId,
                                          aEnv ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            {
                (void)smxmCommit( &sMiniTrans, aEnv );
            }
    }

    return STL_FAILURE;

}


/**
 * @brief 주어진 Index에 새 Key를 삽입한다
 * @param[in] aStatement 사용중인 statement
 * @param[in] aRelationHandle Key를 삽입할 Index의 Relation의 handle
 * @param[in] aColumns 삽입할 Key의 컬럼 정보들
 * @param[in] aRowBlock SM Row Block
 * @param[in] aBlockFilter Block내에 삽입해야할 row에 대한 boolean형 filter (nullable)
 * @param[in] aIsDeferred uniqueness violation 을 deferred로 처리할지 여부
 * @param[out] aViolationCnt uniqueness violation된 개수
 * @param[out] aUniqueViolationScope 삽입할 key가 uniqueness violation을 발생시킨 범위
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbBlockInsert( smlStatement            * aStatement,
                             void                    * aRelationHandle,
                             knlValueBlockList       * aColumns,
                             smlRowBlock             * aRowBlock,
                             stlBool                 * aBlockFilter,
                             stlBool                   aIsDeferred,
                             stlInt32                * aViolationCnt,
                             smlUniqueViolationScope * aUniqueViolationScope,
                             smlEnv                  * aEnv )
{
    stlInt32                  sBlockIdx;
    stlInt32                  sUsedBlockCnt;
    stlInt32                  sSkipBlockCnt;
    smlRid                    sRowRid;
    stlInt32                  sViolationCnt = 0;
    smlUniqueViolationScope   sUniqueViolationScope;
    stlInt32                  sLogIdx = 0;
    smlRid                    sUndoRid = SML_INVALID_RID;
    stlInt32                  sStartBlockIdx;
    stlInt32                  sEndBlockIdx;
    stlChar                   sUndoBuffer[SMN_MAX_BLOCK_KEY_BUFFER_SIZE];
    stlInt8                   sUsedKeyCount = 0;
    smlScn                    sStmtViewScn;
    smlTcn                    sStmtTcn;
    stlInt32                  sOffset = 0;
    smxmTrans                 sMiniTrans;
    smxlTransId               sTransId;
    smlRid                    sSegmentRid;
    stlInt32                  sState = 0;
    smlScn                    sRowScn;
    
    sStmtViewScn = SMA_GET_STMT_VIEW_SCN( aStatement );
    sStmtTcn = SMA_GET_TCN( aStatement );
    sTransId = SMA_GET_TRANS_ID( aStatement );
    sSegmentRid = smsGetSegRid( SME_GET_SEGMENT_HANDLE(aRelationHandle) );
    
    *aUniqueViolationScope = SML_UNIQUE_VIOLATION_SCOPE_NONE;

    sUsedBlockCnt = KNL_GET_BLOCK_USED_CNT( aColumns );
    sSkipBlockCnt = KNL_GET_BLOCK_SKIP_CNT( aColumns );
    sEndBlockIdx = sSkipBlockCnt;

    if( SMN_GET_LOGGING_FLAG( aRelationHandle ) == STL_FALSE )
    {
        smxlSetRepreparable( sTransId, STL_FALSE );
    }
    
    do
    {
        if( SMN_GET_LOGGING_FLAG( aRelationHandle ) != STL_TRUE )
        {
            /**
             * no logging index의 rollback은 row 를 기반으로 한다.
             */

            sStartBlockIdx = sEndBlockIdx;
            sEndBlockIdx = sUsedBlockCnt;
            sBlockIdx = sStartBlockIdx;
            
            STL_TRY( smxmBegin( &sMiniTrans,
                                sTransId,
                                sSegmentRid,
                                SMXM_ATTR_NOLOG_DIRTY,
                                aEnv )
                     == STL_SUCCESS);
            sState = 1;

            /**
             * marshalling row rid
             */

            sOffset = 0;
            
            STL_WRITE_MOVE_INT8( sUndoBuffer,
                                 &sUsedKeyCount,
                                 sOffset );
            STL_WRITE_MOVE_INT64( sUndoBuffer,
                                  &sStmtViewScn,
                                  sOffset );
            STL_WRITE_MOVE_INT32( sUndoBuffer,
                                  &sStmtTcn,
                                  sOffset );
                
            for( sLogIdx = 0; sBlockIdx < sUsedBlockCnt; sBlockIdx++ )
            {
                if( (aBlockFilter != NULL) && (aBlockFilter[sBlockIdx] == STL_FALSE) )
                {
                    continue;
                }

                if( sLogIdx >= SMN_MAX_BLOCK_KEY_COUNT )
                {
                    sEndBlockIdx = sBlockIdx;
                    break;
                }
                    
                sRowRid = SML_GET_RID_VALUE( aRowBlock, sBlockIdx );
                sRowScn = SML_GET_SCN_VALUE( aRowBlock, sBlockIdx );

                sStmtViewScn = STL_MAX( sStmtViewScn, sRowScn );

                STL_WRITE_MOVE_INT32( sUndoBuffer,
                                      &sRowRid.mPageId,
                                      sOffset );
                STL_WRITE_MOVE_INT16( sUndoBuffer,
                                      &sRowRid.mOffset,
                                      sOffset );

                sLogIdx++;
            }

            /**
             * 한건이라도 있으면 undo record를 기록한다.
             */
            
            if( sLogIdx > 0 )
            {
                /**
                 * Version conflict로 인하여 Statement의 View Scn보다
                 * Row의 View Scn을 신뢰해야 한다.
                 */
                STL_WRITE_INT64( sUndoBuffer + 1, &sStmtViewScn );
                
                STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                               SMG_COMPONENT_INDEX,
                                               SMN_UNDO_LOG_MEMORY_BTREE_BLOCK_INSERT,
                                               sUndoBuffer,
                                               sOffset,
                                               SML_INVALID_RID,
                                               &sUndoRid,
                                               aEnv )
                         == STL_SUCCESS );
            }

            sState = 0;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
        }
        else
        {
            /**
             * logging index의 rollback은 undo key record를 기반으로 한다.
             */

            sStartBlockIdx = sSkipBlockCnt;
            sEndBlockIdx = sUsedBlockCnt;
        }

        /**
         * key insertion
         */
        
        for( sBlockIdx = sStartBlockIdx; sBlockIdx < sEndBlockIdx; sBlockIdx++ )
        {
            sRowRid = SML_GET_RID_VALUE( aRowBlock, sBlockIdx );

            if( (aBlockFilter != NULL) && (aBlockFilter[sBlockIdx] == STL_FALSE) )
            {
                continue;
            }
        
            STL_TRY( smnmpbInsert( aStatement,
                                   aRelationHandle,
                                   aColumns,
                                   sBlockIdx,
                                   &sRowRid,
                                   &sUndoRid,
                                   STL_FALSE, /* aIsIndexBuilding */
                                   aIsDeferred,
                                   &sUniqueViolationScope,
                                   aEnv )
                     == STL_SUCCESS );

            if( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_NONE )
            {
                sViolationCnt++;
                *aUniqueViolationScope = STL_MAX( *aUniqueViolationScope,
                                                  sUniqueViolationScope );

                if( aIsDeferred == STL_FALSE )
                {
                    if( sState == 1 )
                    {
                        sState = 0;
                        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
                    }

                    STL_THROW( RAMP_FINISH );
                }
            }
        }

        if( sBlockIdx == sUsedBlockCnt )
        {
            break;
        }
            
    } while( STL_TRUE );

    STL_RAMP( RAMP_FINISH );
        
    *aViolationCnt += sViolationCnt;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smxmCommit( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}


/**
 * @brief 주어진 row rid의 row를 가리키는 key를 삭제한다
 * @param[in] aStatement 사용하는 statement
 * @param[in] aRelationHandle 삭제할 Key가 존재하는 relation의 핸들
 * @param[in] aColumns 삭제할 Key의 컬럼 정보들
 * @param[in] aRowBlock SM Row Block
 * @param[in] aBlockFilter Block내에 삭제해야할 row에 대한 boolean형 filter (nullable)
 * @param[out] aViolationCnt uniqueness violation된 개수
 * @param[out] aUniqueViolationScope 삭제할 key가 uniqueness violation을 해소했는지 여부
 * @param[in,out] aEnv Storage Manager Environment
 */
stlStatus smnmpbBlockDelete( smlStatement            * aStatement,
                             void                    * aRelationHandle,
                             knlValueBlockList       * aColumns,
                             smlRowBlock             * aRowBlock,
                             stlBool                 * aBlockFilter,
                             stlInt32                * aViolationCnt,
                             smlUniqueViolationScope * aUniqueViolationScope,
                             smlEnv                  * aEnv )
{
    stlInt32                  sBlockIdx;
    stlInt32                  sUsedBlockCnt;
    stlInt32                  sSkipBlockCnt;
    smlRid                    sRowRid;
    stlInt32                  sViolationCnt = 0;
    smlUniqueViolationScope   sUniqueViolationScope;
    stlInt32                  sLogIdx = 0;
    smlRid                    sUndoRid = SML_INVALID_RID;
    stlInt32                  sStartBlockIdx;
    stlInt32                  sEndBlockIdx;
    stlChar                   sUndoBuffer[SMN_MAX_BLOCK_KEY_BUFFER_SIZE];
    stlInt8                   sUsedKeyCount = 0;
    smlScn                    sStmtViewScn;
    smlTcn                    sStmtTcn;
    stlInt32                  sOffset = 0;
    smxmTrans                 sMiniTrans;
    smxlTransId               sTransId;
    smlRid                    sSegmentRid;
    stlInt32                  sState = 0;
    smlScn                    sRowScn;
    
    sStmtViewScn = SMA_GET_STMT_VIEW_SCN( aStatement );
    sStmtTcn = SMA_GET_TCN( aStatement );
    sTransId = SMA_GET_TRANS_ID( aStatement );
    sSegmentRid = smsGetSegRid( SME_GET_SEGMENT_HANDLE(aRelationHandle) );
    
    *aUniqueViolationScope = SML_UNIQUE_VIOLATION_SCOPE_NONE;

    sUsedBlockCnt = KNL_GET_BLOCK_USED_CNT( aColumns );
    sSkipBlockCnt = KNL_GET_BLOCK_SKIP_CNT( aColumns );
    sEndBlockIdx = sSkipBlockCnt;
    
    if( SMN_GET_LOGGING_FLAG( aRelationHandle ) == STL_TRUE )
    {
        smxlSetRepreparable( sTransId, STL_FALSE );
    }
    
    do
    {
        if( SMN_GET_LOGGING_FLAG( aRelationHandle ) != STL_TRUE )
        {
            /**
             * no logging index의 rollback은 row 를 기반으로 한다.
             */

            sStartBlockIdx = sEndBlockIdx;
            sEndBlockIdx = sUsedBlockCnt;
            sBlockIdx = sStartBlockIdx;
            
            STL_TRY( smxmBegin( &sMiniTrans,
                                sTransId,
                                sSegmentRid,
                                SMXM_ATTR_NOLOG_DIRTY,
                                aEnv )
                     == STL_SUCCESS);
            sState = 1;
    
            /**
             * marshalling row rid
             */

            sOffset = 0;
            
            STL_WRITE_MOVE_INT8( sUndoBuffer,
                                 &sUsedKeyCount,
                                 sOffset );
            STL_WRITE_MOVE_INT64( sUndoBuffer,
                                  &sStmtViewScn,
                                  sOffset );
            STL_WRITE_MOVE_INT32( sUndoBuffer,
                                  &sStmtTcn,
                                  sOffset );
                
            for( sLogIdx = 0; sBlockIdx < sUsedBlockCnt; sBlockIdx++ )
            {
                if( (aBlockFilter != NULL) && (aBlockFilter[sBlockIdx] == STL_FALSE) )
                {
                    continue;
                }

                if( sLogIdx >= SMN_MAX_BLOCK_KEY_COUNT )
                {
                    sEndBlockIdx = sBlockIdx;
                    break;
                }
                    
                sRowRid = SML_GET_RID_VALUE( aRowBlock, sBlockIdx );
                sRowScn = SML_GET_SCN_VALUE( aRowBlock, sBlockIdx );

                sStmtViewScn = STL_MAX( sStmtViewScn, sRowScn );

                STL_WRITE_MOVE_INT32( sUndoBuffer,
                                      &sRowRid.mPageId,
                                      sOffset );
                STL_WRITE_MOVE_INT16( sUndoBuffer,
                                      &sRowRid.mOffset,
                                      sOffset );

                sLogIdx++;
            }

            /**
             * 한건이라도 있으면 undo record를 기록한다.
             */
            
            if( sLogIdx > 0 )
            {
                /**
                 * Version conflict로 인하여 Statement의 View Scn보다
                 * Row의 View Scn을 신뢰해야 한다.
                 */
                STL_WRITE_INT64( sUndoBuffer + 1, &sStmtViewScn );
                
                STL_TRY( smxlInsertUndoRecord( &sMiniTrans,
                                               SMG_COMPONENT_INDEX,
                                               SMN_UNDO_LOG_MEMORY_BTREE_BLOCK_DELETE,
                                               sUndoBuffer,
                                               sOffset,
                                               SML_INVALID_RID,
                                               &sUndoRid,
                                               aEnv )
                         == STL_SUCCESS );
            }

            sState = 0;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
        }
        else
        {
            /**
             * logging index의 rollback은 undo key record를 기반으로 한다.
             */

            sStartBlockIdx = sSkipBlockCnt;
            sEndBlockIdx = sUsedBlockCnt;
        }
        
        for( sBlockIdx = sStartBlockIdx; sBlockIdx < sEndBlockIdx; sBlockIdx++ )
        {
            sRowRid = SML_GET_RID_VALUE( aRowBlock, sBlockIdx );

            if( (aBlockFilter != NULL) && (aBlockFilter[sBlockIdx] == STL_FALSE) )
            {
                continue;
            }
        
            STL_TRY( smnmpbDelete( aStatement,
                                   aRelationHandle,
                                   aColumns,
                                   sBlockIdx,
                                   &sRowRid,
                                   &sUndoRid,
                                   &sUniqueViolationScope,
                                   aEnv )
                     == STL_SUCCESS );

            if( sUniqueViolationScope != SML_UNIQUE_VIOLATION_SCOPE_NONE )
            {
                sViolationCnt--;
                *aUniqueViolationScope = STL_MAX( *aUniqueViolationScope,
                                                  sUniqueViolationScope );
            }
            
            STL_DASSERT( sUniqueViolationScope <= SML_UNIQUE_VIOLATION_SCOPE_RESOLVED );
        }

        if( sBlockIdx == sUsedBlockCnt )
        {
            break;
        }
            
    } while( STL_TRUE );
        
    *aViolationCnt += sViolationCnt;

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) smxmCommit( &sMiniTrans, aEnv );
    }

    return STL_FAILURE;
}


/**
 * @brief 주어진 Leaf 노드 해당 Key를 Delete mark 한다.
 *        필요한 경우 SMO가 발생할 수 있다
 * @param[in] aStatement Statement
 * @param[in] aMiniTrans Latch를 잡고 logging을 하는데 사용될 mini transaction
 * @param[in] aRelationHandle Delete 하려는 Index의 Relation Handle
 * @param[in] aPageHandle target leaf page id
 * @param[in] aSlotSeq Delete할 key가 들어갈 순서
 * @param[in] aValColumns Delete할 key 값
 * @param[in] aBlockIdx aValColumns에서 사용할 block idx
 * @param[in] aRowRid delete할 key가 가리키는 row의 위치
 * @param[in] aUndoRid Key와 대응되는 Undo record의 위치
 * @param[in] aKeyBodySize delete할 key의 Key value를 기록한 부분의 길이
 * @param[in] aPathStack Index를 root부터 탐색해 내려온 길에 대한 정보
 * @param[in] aIsWriteUndoLog Undo log를 써야하는지 여부
 * @param[out] aNeedRetry Smo 도중에 Root 삭제시 다른 Smo operation과 충돌나서 다시해야 함
 * @param[out] aEmptyPageCount SMO로 인해 빈키만 있는 page의 수
 * @param[out] aEmptyPid SMO로 인해 delete되는 반대쪽 노드에 빈 키만 있는 경우의 해당 PID
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbDeleteFromLeaf( smlStatement      * aStatement,
                                smxmTrans         * aMiniTrans,
                                void              * aRelationHandle,
                                smpHandle         * aPageHandle,
                                stlInt16            aSlotSeq,
                                knlValueBlockList * aValColumns,
                                stlInt32            aBlockIdx,
                                smlRid            * aRowRid,
                                smlRid            * aUndoRid,
                                stlUInt16           aKeyBodySize,
                                smnmpbPathStack   * aPathStack,
                                stlBool             aIsWriteUndoLog,
                                stlInt32          * aNeedRetry,
                                stlInt16          * aEmptyPageCount,
                                smlPid            * aEmptyPid,
                                smlEnv            * aEnv )
{
    smxlTransId  sTransId = smxlGetTransId(smxmGetTransId( aMiniTrans ));
    smlScn       sViewScn = smxlGetTransViewScn( sTransId );
    stlInt16     sRetryPhase = 0;
    stlUInt16    sOrgKeySize = 0;
    stlUInt16    sNewKeySize = 0;
    stlChar    * sKeySlot = NULL;
    stlChar    * sNewKeySlot = NULL;
    smlRid       sUndoRid;
    stlChar      sLogBuf[SMP_PAGE_SIZE];
    stlChar    * sPtr;
    stlUInt8     sNewRtsSeq;
    stlInt16     sAftSlotSeq;
    stlBool      sIsFreed;
    stlUInt8     sInsertRtsSeq;
    stlUInt8     sInsertRtsVerNo;
    stlUInt8     sNewInsertRtsVerNo;
    stlUInt8     sNewDeleteRtsVerNo;
    smpRts     * sSrcInsertRts;
    smlScn       sTmpScn;
    stlUInt8     sDelTcn;

    *aNeedRetry = SMNMPB_RETRY_NONE;
    *aEmptyPageCount = 0;
    
    sKeySlot = smpGetNthRowItem(aPageHandle, aSlotSeq);
    sOrgKeySize = smnmpbGetLeafKeySize( sKeySlot, SMNMPB_GET_KEY_COLUMN_COUNT(aPageHandle) );

    STL_DASSERT( SMNMPB_IS_DELETED( sKeySlot ) == STL_FALSE );
    
    while( STL_TRUE )
    {
        if( sRetryPhase == 1 )
        {
            /* aging 할 필요가 있으면 aging 한다 */
            if( smpGetReclaimedSpace(SMP_FRAME(aPageHandle)) >= sNewKeySize + STL_SIZEOF(smpOffsetSlot) )
            {
                sAftSlotSeq = aSlotSeq;
                STL_TRY( smnmpbDoAgingLeafNode( aRelationHandle,
                                                aPageHandle,
                                                &sAftSlotSeq,
                                                STL_FALSE /* aNeedRowStamping */,
                                                &sIsFreed,
                                                aEnv ) == STL_SUCCESS );
                STL_ASSERT( sIsFreed != STL_TRUE );
                if( aSlotSeq != sAftSlotSeq )
                {
                    /* aging 연산으로 delete 해야 할 순번이 변경되었음 */
                    aSlotSeq = sAftSlotSeq;
                }
            }
            if( smpGetTotalFreeSpace(SMP_FRAME(aPageHandle)) >= sNewKeySize + STL_SIZEOF(smpOffsetSlot) )
            {
                STL_TRY( smnmpbCompactPage( aRelationHandle,
                                            aPageHandle,
                                            STL_INT16_MAX,
                                            aEnv ) == STL_SUCCESS );
                /* compact 연산으로 delete 해야 할 위치가 변경되었음 */
                sKeySlot = smpGetNthRowItem(aPageHandle, aSlotSeq);
            }
            else
            {
                sRetryPhase++;
                continue;
            }
        }
        else if( sRetryPhase == 2 )
        {
            sAftSlotSeq = aSlotSeq;

            /* 모든 SMO을 수행하기 전에 undo 공간을 확보함으로써                      */
            /* undo 공간이 모자라 operation 중에 rollback이 되는 경우를 사전에 방지함 */
            STL_TRY( smxlPrepareUndoSpace( sTransId, SMP_PAGE_SIZE, aEnv ) == STL_SUCCESS );

            STL_TRY( smnmpbSplitLeafNode( aMiniTrans,
                                          aRelationHandle,
                                          aPageHandle, /* in,out */
                                          NULL,
                                          0,
                                          aRowRid,
                                          &sAftSlotSeq, /* in,out */
                                          aPathStack,
                                          aNeedRetry,
                                          aEmptyPageCount,
                                          aEmptyPid,
                                          aEnv ) == STL_SUCCESS );
            STL_TRY_THROW( *aNeedRetry == SMNMPB_RETRY_NONE, RAMP_RETRY_FROM_ROOT );
            /* split 연산으로 delete 해야 할 위치가 변경되었음 */
            aSlotSeq = sAftSlotSeq;
            sKeySlot = smpGetNthRowItem(aPageHandle, aSlotSeq);
        }
        else if( sRetryPhase == 3 )
        {
            /* Split 했는데도 delete하지 못함 */
            STL_ASSERT( STL_FALSE );
        }
        else
        {
            /* do nothing */
        }

        if( SMXL_IS_UNDO_BOUND_TRANS( sTransId ) == STL_TRUE )
        {
            /* DS mode가 아니면 RTS 할당 받음 */
            STL_TRY( smpAllocRts( (aIsWriteUndoLog == STL_TRUE) ? aMiniTrans : NULL,
                                  aPageHandle,
                                  sTransId,
                                  SML_MAKE_VIEW_SCN(sViewScn),
                                  smnmpbStabilizeKey,
                                  &sNewRtsSeq,
                                  aEnv ) == STL_SUCCESS );
            if( SMP_IS_VALID_RTS_SEQ(sNewRtsSeq) != STL_TRUE )
            {
                /* RTS 할당 실패 */
                if( SMNMPB_IS_TBK(sKeySlot) != STL_TRUE )
                {
                    sNewKeySize = sOrgKeySize + SMNMPB_TBK_HDR_SIZE - SMNMPB_RBK_HDR_SIZE;
                    STL_TRY( smpReallocNthSlot( aPageHandle,
                                                aSlotSeq,
                                                sNewKeySize,
                                                STL_TRUE,
                                                &sNewKeySlot ) == STL_SUCCESS );
                   if( sNewKeySlot == NULL )
                   {
                       /* tbk로 변환할 공간 할당 실패 */
                       sRetryPhase++;
                       continue;
                   }
                }
            }
        }
        else
        {
            /* DS mode면 */
            sNewRtsSeq = SMP_RTS_SEQ_STABLE;
        }

        /* 모든 과정 성공 */
        break;
    } /* while */

    /* undo log record를 작성 */
    if( aIsWriteUndoLog == STL_TRUE )
    {
        if( SML_IS_INVALID_RID( *aUndoRid ) == STL_TRUE )
        {
            STL_TRY( smxlInsertUndoRecord( aMiniTrans,
                                           SMG_COMPONENT_INDEX,
                                           SMN_UNDO_LOG_MEMORY_BTREE_DELETE_LEAF,
                                           sKeySlot,
                                           sOrgKeySize,
                                           ((smlRid){ aPageHandle->mPch->mTbsId,
                                                   aSlotSeq,
                                                   aPageHandle->mPch->mPageId } ),
                                           &sUndoRid,
                                           aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smnmpbMarkCompletionInUndoRecord( aMiniTrans,
                                                       aUndoRid,
                                                       aEnv )
                     == STL_SUCCESS );
        }
    }

    if( sNewKeySlot != NULL )
    {
        /* realloc 받아야 함 */
        STL_TRY( smpReallocNthSlot( aPageHandle,
                                    aSlotSeq,
                                    sNewKeySize,
                                    STL_FALSE,
                                    &sNewKeySlot ) == STL_SUCCESS );
        /* 페이지 변경 */
        if( SMP_IS_VALID_RTS_SEQ( sNewRtsSeq ) == STL_TRUE )
        {
            /* DS mode가 아니면 */
            STL_TRY( smpBeginUseRts( aPageHandle,
                                     sNewKeySlot,
                                     sTransId,
                                     SML_MAKE_VIEW_SCN(sViewScn),
                                     sNewRtsSeq,
                                     &sNewDeleteRtsVerNo,
                                     aEnv ) == STL_SUCCESS );
        }
        else
        {
            sNewDeleteRtsVerNo = 0;
        }


        SMNMPB_GET_INS_RTS_INFO( sKeySlot, &sInsertRtsSeq, &sInsertRtsVerNo );
        if( SMP_IS_VALID_RTS_SEQ(sInsertRtsSeq) == STL_TRUE )
        {
            /* RTS의 Offset 변경 */
            sSrcInsertRts = SMP_GET_NTH_RTS(SMP_FRAME(aPageHandle), sInsertRtsSeq);
            if( sSrcInsertRts->mPrevVerCnt == sInsertRtsVerNo )
            {
                STL_TRY( smpBeginUseRts( aPageHandle,
                                         sNewKeySlot,
                                         sSrcInsertRts->mTransId,
                                         sSrcInsertRts->mScn,
                                         sInsertRtsSeq,
                                         &sNewInsertRtsVerNo,
                                         aEnv ) == STL_SUCCESS );

                STL_ASSERT( sNewInsertRtsVerNo == sInsertRtsVerNo );

                STL_TRY( smpEndUseRts( aPageHandle,
                                       sKeySlot,
                                       sInsertRtsSeq,
                                       sInsertRtsVerNo,
                                       aEnv ) == STL_SUCCESS );
            }
            else
            {
                sNewInsertRtsVerNo = sInsertRtsVerNo;
            }
        }
        else
        {
            sNewInsertRtsVerNo = sInsertRtsVerNo;
        }

        stlMemcpy( sNewKeySlot, sKeySlot, SMNMPB_RBK_HDR_SIZE );
        stlMemcpy( sNewKeySlot + SMNMPB_TBK_HDR_SIZE,
                   sKeySlot + SMNMPB_RBK_HDR_SIZE,
                   sOrgKeySize - SMNMPB_RBK_HDR_SIZE );

        if( aStatement == NULL )
        {
            /**
             * Rollback시에는 Insert Tcn을 Delete Tcn으로 설정한다.
             */
            SMNMPB_GET_INS_TCN( sNewKeySlot, &sDelTcn );
        }
        else
        {
            sDelTcn = SMA_GET_TCN(aStatement);
            
            if( SMA_GET_TCN(aStatement) >= SMNMPB_MAX_TCN )
            {
                sDelTcn = SMNMPB_INVALID_TCN;
            }
        }
        
        SMNMPB_SET_DEL_TCN( sNewKeySlot, &sDelTcn );
        
        SMNMPB_SET_INS_RTS_INFO( sNewKeySlot, &sInsertRtsSeq, &sNewInsertRtsVerNo );
        SMNMPB_SET_DELETED( sNewKeySlot );
        SMNMPB_SET_DEL_RTS_INFO( sNewKeySlot, &sNewRtsSeq, &sNewDeleteRtsVerNo );
        SMNMPB_SET_TBK( sNewKeySlot );
        SMNMPB_SET_TBK_DEL_TRANS_ID( sNewKeySlot, &sTransId );
        sTmpScn = SML_MAKE_VIEW_SCN(sViewScn);
        SMNMPB_SET_TBK_DEL_SCN( sNewKeySlot, &sTmpScn );
        SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sOrgKeySize );
        SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sNewKeySize + STL_SIZEOF(smpOffsetSlot) );
        sOrgKeySize = sNewKeySize;
        sKeySlot = sNewKeySlot;
    }
    else
    {
        /* 페이지 변경 */
        if( SMP_IS_VALID_RTS_SEQ( sNewRtsSeq ) == STL_TRUE )
        {
            /* DS mode가 아니면 */
            STL_TRY( smpBeginUseRts( aPageHandle,
                                     sKeySlot,
                                     sTransId,
                                     SML_MAKE_VIEW_SCN(sViewScn),
                                     sNewRtsSeq,
                                     &sNewDeleteRtsVerNo,
                                     aEnv ) == STL_SUCCESS );
        }
        else
        {
            sNewDeleteRtsVerNo = 0;
        }

        if( aStatement == NULL )
        {
            /**
             * Rollback시에는 Insert Tcn을 Delete Tcn으로 설정한다.
             */
            SMNMPB_GET_INS_TCN( sKeySlot, &sDelTcn );
        }
        else
        {
            sDelTcn = SMA_GET_TCN(aStatement);
            
            if( SMA_GET_TCN(aStatement) >= SMNMPB_MAX_TCN )
            {
                sDelTcn = SMNMPB_INVALID_TCN;
            }
        }
        
        SMNMPB_SET_DEL_TCN( sKeySlot, &sDelTcn );
        
        SMNMPB_SET_DELETED( sKeySlot );
        SMNMPB_SET_DEL_RTS_INFO( sKeySlot, &sNewRtsSeq, &sNewDeleteRtsVerNo );
        if( SMNMPB_IS_TBK( sKeySlot ) == STL_TRUE )
        {
            SMNMPB_SET_TBK_DEL_TRANS_ID( sKeySlot, &sTransId );
            sTmpScn = SML_MAKE_VIEW_SCN(sViewScn);
            SMNMPB_SET_TBK_DEL_SCN( sKeySlot, &sTmpScn );
        }
        SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sOrgKeySize + STL_SIZEOF(smpOffsetSlot) );
    }

    /* write REDO log */
    sPtr = sLogBuf;
    STL_WRITE_INT64( sPtr, &sTransId );
    sPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( sPtr, &sViewScn );
    sPtr += STL_SIZEOF(smlScn);
    stlMemcpy( sPtr, sKeySlot, sOrgKeySize);
    sPtr +=  sOrgKeySize;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_MEMORY_BTREE_DELETE_LEAF,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sPtr - sLogBuf,
                           aPageHandle->mPch->mTbsId,
                           aPageHandle->mPch->mPageId,
                           aSlotSeq,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    SMP_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    smpSetMaxViewScn( aPageHandle, sViewScn );

    STL_RAMP( RAMP_RETRY_FROM_ROOT );

    STL_TRY( smnmpbValidateIndexPage( aPageHandle,
                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Key가 rollback되어야 함을 표시한다.
 * @param[in] aMiniTrans Mini-transaction pointer
 * @param[in] aUndoRid Undo Record의 위치
 * @param[in] aEnv Storage Manager Environment
 * @note 해당 함수는 no logging index에서만 유효하다.
 */
stlStatus smnmpbMarkCompletionInUndoRecord( smxmTrans * aMiniTrans,
                                            smlRid    * aUndoRid,
                                            smlEnv    * aEnv )
{
    smpHandle   sPageHandle;
    stlChar   * sUndoRecord;
    stlChar   * sKeyBody;
    stlInt32    sState = 0;

    /**
     * no logging index에서만 유효하다
     */
    
    STL_DASSERT( (smxmGetAttr( aMiniTrans ) & SMXM_ATTR_NOLOG_DIRTY) ==
                 SMXM_ATTR_NOLOG_DIRTY );
    
    /**
     * 유효한 key count 증가
     */
    
    STL_TRY( smpFix( aUndoRid->mTbsId,
                     aUndoRid->mPageId,
                     &sPageHandle,
                     aEnv )
             == STL_SUCCESS );
    sState = 1;

    sUndoRecord = smpGetNthRowItem( &sPageHandle, aUndoRid->mOffset );
    
    sKeyBody = sUndoRecord + SMXL_UNDO_REC_HDR_SIZE;
    (*(stlUInt8*)sKeyBody) += 1;

    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    if( sState == 1 )
    {
        smpUnfix( &sPageHandle, aEnv );
    }
            
    return STL_FAILURE;
}

/**
 * @brief Index header에 empty page list를 register한다.
 * @param[in] aRelationHandle index 핸들
 * @param[in] aMiniTrans logging시 사용할 mini transaction
 * @param[in] aEmptyPageCount 추가할 새 empty page 수
 * @param[in] aEmptyPid 추가할 새 empty page ID
 * @param[in] aCheckAgingFlag Aging flag 체크 여부
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smnmpbRegisterEmptyNode( void      * aRelationHandle,
                                   smxmTrans * aMiniTrans,
                                   stlInt16    aEmptyPageCount,
                                   smlPid    * aEmptyPid,
                                   stlBool     aCheckAgingFlag,
                                   smlEnv    * aEnv )
{
    smnIndexHeader * sHeader;
    smlRid           sSegRid;
    smpHandle        sPageHandle;
    smpHandle        sSegHandle;
    smlPid           sLastPid;
    smpHandle        sLastPage;
    smxmSavepoint    sSavepoint;
    stlInt32         sEmptyNodeCount;
    stlBool          sIsSuccess = STL_TRUE;
    stlInt32         sState = 0;
    stlInt16         i;
    smxlTransId      sTransId;
    stlUInt32        sAttr;

    /* minitransaction 상태 정보 저장 */
    sAttr = smxmGetAttr( aMiniTrans );
    sTransId = smxmGetTransId( aMiniTrans );

    STL_TRY( smxmMarkSavepoint( aMiniTrans, &sSavepoint, aEnv ) == STL_SUCCESS );
    sSegRid = smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle));

    while( 1 )
    {
        STL_TRY( smpAcquire( aMiniTrans,
                             sSegRid.mTbsId,
                             sSegRid.mPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sSegHandle,
                             aEnv ) == STL_SUCCESS );

        if( (aCheckAgingFlag == STL_TRUE) &&
            (SMN_GET_IS_AGING_EMPTY_NODE(aRelationHandle) == STL_TRUE) )
        {
            /* 누군가 Aging 진행중. 다시 시도한다 */
            STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                              &sSavepoint, 
                                              aEnv ) == STL_SUCCESS );

            while( SMN_GET_IS_AGING_EMPTY_NODE(aRelationHandle) == STL_TRUE )
            {
                stlBusyWait();
            }
        }
        else
        {
            break;
        }
    }

    if( aCheckAgingFlag == STL_TRUE )
    {
        /* Aging 하겠다고 찜함 */
        ((smnIndexHeader*)aRelationHandle)->mIsAgingEmptyNode = STL_TRUE;
        sState = 1;
    }

    sHeader = (smnIndexHeader*)((stlChar*)smpGetBody(SMP_FRAME(&sSegHandle)) +
                                smsGetHeaderSize(SME_GET_SEGMENT_HANDLE(aRelationHandle)));

    /* segment page에 대한 x-latch로 empty node list의 동시성 제어를 하기때문에 */
    /* index header의 latch를 따로 잡을 필요가 없음 */
    sEmptyNodeCount = sHeader->mEmptyNodeCount;
    sLastPid = sHeader->mLastEmptyNode;

    /* index 페이지들을 잡기위해 segment 페이지를 푼다 */
    STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                      &sSavepoint, 
                                      aEnv ) == STL_SUCCESS );

    i = 0;
    
    while( i < aEmptyPageCount )
    {
        STL_TRY( smxmMarkSavepoint( aMiniTrans, &sSavepoint, aEnv ) == STL_SUCCESS );

        STL_TRY( smpAcquire( aMiniTrans,
                             smsGetTbsId(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                             aEmptyPid[i],
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );

        if( (SMP_GET_ACTIVE_SLOT_COUNT(SMP_FRAME(&sPageHandle)) > 0) ||
            (SMNMPB_IS_ON_EMPTY_LIST(&sPageHandle) == STL_TRUE) )
        {
            STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                              &sSavepoint, 
                                              aEnv ) == STL_SUCCESS );

            /*
             * SMO 시에는 empty node였지만, 이후에 valid한 key가
             * insert되었을 경우 empty list에 매달지 않는다.
             */ 
            i++;
            continue;
        }

        STL_DASSERT( smpGetFreeness( &sPageHandle ) != SMP_FREENESS_FREE );
        STL_DASSERT( SMNMPB_IS_ON_EMPTY_LIST( &sPageHandle ) != STL_TRUE );
        STL_DASSERT( SMNMPB_GET_NEXT_FREE_PAGE( &sPageHandle ) == SMP_NULL_PID );

        if( sEmptyNodeCount == 0 )
        {
            STL_TRY( smpAcquire( aMiniTrans,
                                 sSegRid.mTbsId,
                                 sSegRid.mPageId,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sSegHandle,
                                 aEnv ) == STL_SUCCESS );

            sHeader = (smnIndexHeader*)((stlChar*)smpGetBody(SMP_FRAME(&sSegHandle)) +
                                smsGetHeaderSize(SME_GET_SEGMENT_HANDLE(aRelationHandle)));

            /* latch를 놓은 사이에 변하지 않았어야 한다 */
            STL_ASSERT( sHeader->mEmptyNodeCount == 0 );

            sHeader->mFirstEmptyNode = aEmptyPid[i];
            sHeader->mEmptyNodeCount = 1;
        }
        else
        {
            STL_TRY( smpTryAcquire( aMiniTrans,
                                    smsGetTbsId(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                                    sLastPid,
                                    KNL_LATCH_MODE_EXCLUSIVE,
                                    &sLastPage,
                                    &sIsSuccess,
                                    aEnv ) == STL_SUCCESS );

            /**
             * Split과의 Deadlock이 발생할수 있기 때문에 누군가가 잡고 있다면
             * 다시 시작한다.
             */
            if( sIsSuccess == STL_FALSE )
            {
                STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                                  &sSavepoint, 
                                                  aEnv ) == STL_SUCCESS );

                continue;
            }

            /* 모든 index page를 얻은 후에 segment page를 얻어야 함 */
            STL_TRY( smpAcquire( aMiniTrans,
                                 sSegRid.mTbsId,
                                 sSegRid.mPageId,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sSegHandle,
                                 aEnv ) == STL_SUCCESS );

            sHeader = (smnIndexHeader*)((stlChar*)smpGetBody(SMP_FRAME(&sSegHandle)) +
                                        smsGetHeaderSize(SME_GET_SEGMENT_HANDLE(aRelationHandle)));

            /* latch를 놓은 사이에 변하지 않았어야 한다 */
            STL_ASSERT( sHeader->mLastEmptyNode == sLastPid );

            SMNMPB_SET_NEXT_FREE_PAGE(&sLastPage, aEmptyPid[i]);
            STL_TRY( smxmWriteLog( aMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMR_LOG_MEMORY_BTREE_SET_NEXT_FREE_PAGE,
                                   SMR_REDO_TARGET_PAGE,
                                   (stlChar*)&aEmptyPid[i],
                                   STL_SIZEOF(smlPid),
                                   sLastPage.mPch->mTbsId,
                                   sLastPage.mPch->mPageId,
                                   0,
                                   SMXM_LOGGING_REDO,
                                   aEnv ) == STL_SUCCESS );

            sHeader->mEmptyNodeCount++;
        }

        sHeader->mLastEmptyNode = aEmptyPid[i];
        sLastPid = sHeader->mLastEmptyNode;
        sEmptyNodeCount = sHeader->mEmptyNodeCount;

        ((smnIndexHeader*)aRelationHandle)->mFirstEmptyNode = sHeader->mFirstEmptyNode;
        ((smnIndexHeader*)aRelationHandle)->mLastEmptyNode = sHeader->mLastEmptyNode;
        ((smnIndexHeader*)aRelationHandle)->mEmptyNodeCount = sHeader->mEmptyNodeCount;

        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_ADD_EMPTY_NODE,
                               SMR_REDO_TARGET_PAGE,
                               (stlChar*)&aEmptyPid[i],
                               STL_SIZEOF(smlPid),
                               sSegRid.mTbsId,
                               sSegRid.mPageId,
                               (stlChar*)sHeader - (stlChar*)SMP_FRAME(&sSegHandle),
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );

        /* EmptyNode에 Empty Node List에 달렸음을 세팅한다 */
        SMNMPB_GET_LOGICAL_HEADER(&sPageHandle)->mFlag |= SMNMPB_NODE_FLAG_EMPTY_LIST;
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_SET_NODE_FLAG,
                               SMR_REDO_TARGET_PAGE,
                               (void*)&(SMNMPB_GET_LOGICAL_HEADER(&sPageHandle)->mFlag),
                               STL_SIZEOF(stlUInt8),
                               sPageHandle.mPch->mTbsId,
                               smpGetPageId(&sPageHandle),
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );

        STL_TRY( smxmCommit( aMiniTrans, aEnv ) == STL_SUCCESS );

        STL_TRY( smxmBegin( aMiniTrans,
                            sTransId,
                            smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                            sAttr,
                            aEnv ) == STL_SUCCESS );

        sLastPage = sPageHandle;
        i++;
    }

    /* 다 했음을 체크 */
    stlMemBarrier();

    if( aCheckAgingFlag == STL_TRUE )
    {
        sState = 0;
        ((smnIndexHeader*)aRelationHandle)->mIsAgingEmptyNode = STL_FALSE; 
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        ((smnIndexHeader*)aRelationHandle)->mIsAgingEmptyNode = STL_FALSE;
    }

    return STL_FAILURE;
}


/**
 * @brief 주어진 row rid의 row를 가리키는 key를 삭제한다
 * @param[in] aStatement 사용하는 statement
 * @param[in] aRelationHandle 삭제할 Key가 존재하는 relation의 핸들
 * @param[in] aColumns 삭제할 Key의 컬럼 정보들
 * @param[in] aBlockIdx aColumns에서 사용할 block idx
 * @param[in] aRowRid 삭제할 Key가 가리키는 Row의 RID
 * @param[in] aUndoRid Key와 대응되는 Undo record의 위치
 * @param[out] aUniqueViolationScope 삭제할 key가 uniqueness violation을 해소했는지 여부
 * @param[in,out] aEnv Storage Manager Environment
 */
stlStatus smnmpbDelete( smlStatement            * aStatement,
                        void                    * aRelationHandle,
                        knlValueBlockList       * aColumns,
                        stlInt32                  aBlockIdx,
                        smlRid                  * aRowRid,
                        smlRid                  * aUndoRid,
                        smlUniqueViolationScope * aUniqueViolationScope,
                        smlEnv                  * aEnv )
{
    stlUInt32           sAttr;
    smxmTrans           sMiniTrans;
    stlUInt16           sState = 0;
    stlBool             sIsUniqueResolved;
    smxlTransId         sTransId = SMA_GET_TRANS_ID(aStatement);
    stlInt16            sEmptyPageCount;
    smlPid              sEmptyPidList[KNL_ENV_MAX_LATCH_DEPTH];

    *aUniqueViolationScope = SML_UNIQUE_VIOLATION_SCOPE_NONE;

    if( SMN_GET_LOGGING_FLAG( aRelationHandle ) == STL_TRUE )
    {
        sAttr = SMXM_ATTR_REDO;
    }
    else
    {
        smxlSetRepreparable( SMA_GET_TRANS_ID( aStatement ), STL_FALSE );
        sAttr = SMXM_ATTR_NOLOG_DIRTY;
    }
    
    STL_TRY( smxmBegin( &sMiniTrans,
                        sTransId,
                        smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                        sAttr,
                        aEnv ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smnmpbDeleteInternal( aStatement,
                                   &sMiniTrans,
                                   aRelationHandle,
                                   aColumns,
                                   aBlockIdx,
                                   aRowRid,
                                   aUndoRid,
                                   STL_TRUE,
                                   &sIsUniqueResolved,
                                   &sEmptyPageCount,
                                   sEmptyPidList,
                                   aEnv ) == STL_SUCCESS );

    /* Delete 연산 후 empty가 된 node들을 empty node list에 연결한다
     * Empty node을 list에 연결할 때 Aging이 진행중이면 busy wait을 하게 되고
     * 이때 lock을 잡고 있으면 deadlock이 발생할 수 있기 때문에
     * delete연산 후 mini-transaction을 새로 begin해야한다.
     */
    if( sEmptyPageCount > 0 )
    {
        STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

        STL_TRY( smxmBegin( &sMiniTrans,
                            sTransId,
                            smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                            sAttr,
                            aEnv ) == STL_SUCCESS );

        STL_TRY( smnmpbRegisterEmptyNode( aRelationHandle,
                                          &sMiniTrans,
                                          sEmptyPageCount,
                                          sEmptyPidList,
                                          STL_TRUE, /* aCheckAgingFlag */
                                          aEnv ) == STL_SUCCESS );
    }

    if( sIsUniqueResolved == STL_TRUE )
    {
        *aUniqueViolationScope = SML_UNIQUE_VIOLATION_SCOPE_RESOLVED;
    }

    sState = 0;
    STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmCommit(&sMiniTrans, aEnv);
    }

    return STL_FAILURE;
}


/**
 * @brief 주어진 row rid의 row를 가리키는 key를 삭제한다
 *        Delete key에 의해 leaf node가 split이 될 수 있고, 이때 유효한 key가
 *        없는 empty node가 발생할 수 있으며, 재사용하기 위해 empty pid list를 반환한다.
 * @param[in] aStatement Statement
 * @param[in] aMiniTrans 사용하는 mini transaction
 * @param[in] aRelationHandle 삭제할 Key가 존재하는 relation의 핸들
 * @param[in] aColumns 삭제할 Key의 컬럼 정보들
 * @param[in] aBlockIdx aColumns에서 사용할 block idx
 * @param[in] aRowRid 삭제할 Key가 가리키는 Row의 RID
 * @param[in] aUndoRid Key와 대응되는 Undo record의 위치
 * @param[in] aIsWriteUndoLog undo log를 기록해야 하는지 여부
 * @param[out] aIsUniqueResolved 삭제할 key가 uniqueness violation을 해제시키는지 여부
 * @param[out] aEmptyPageCount valid key가 없는 node의 수
 * @param[out] aEmptyPidList valid key가 없는 node의 Pid list
 * @param[in,out] aEnv Storage Manager Environment
 * @note Rollback에서 사용되는 경우에 aStatement는 NULL일수 있다.
 */
stlStatus smnmpbDeleteInternal( smlStatement      * aStatement,
                                smxmTrans         * aMiniTrans,
                                void              * aRelationHandle,
                                knlValueBlockList * aColumns,
                                stlInt32            aBlockIdx,
                                smlRid            * aRowRid,
                                smlRid            * aUndoRid,
                                stlBool             aIsWriteUndoLog,
                                stlBool           * aIsUniqueResolved,
                                stlInt16          * aEmptyPageCount,
                                smlPid            * aEmptyPidList,
                                smlEnv            * aEnv )
{
    knlValueBlockList * sCurCol;
    stlUInt16           sKeyBodySize = 0;
    smpHandle           sPageHandle;
    smnmpbPathStack     sPathStack;
    stlBool             sCheckUniqueness = SMN_GET_UNIQUENESS(aRelationHandle);
    smlPid              sChildPid;
    smlPid              sNextFreePageId;
    smxmSavepoint       sSavepoint;
    stlInt16            sKeySlotSeq;
    stlBool             sFoundKey;
    stlInt32            sNeedRetry = SMNMPB_RETRY_NONE;
    stlInt16            sActiveKeyCount;
    smxlTransId         sTransId;
    stlUInt32           sAttr;
    stlChar             sKeyVal[8192];
    stlChar           * sPtr = sKeyVal;
    stlInt64            sStrLen;
    dtlDataValue      * sValue;
    stlUInt8            sColLenSize;
    stlBool             sIsMyStmtViolation;
    stlInt32            sNoSpaceRetryCount = 0;
    stlInt32            sState = 1;

    for( sCurCol = aColumns; sCurCol != NULL; sCurCol = sCurCol->mNext )
    {
        sValue = &(sCurCol->mValueBlock->mDataValue[aBlockIdx]);
        SMP_GET_COLUMN_LEN_SIZE( sValue->mLength, &sColLenSize );
        sKeyBodySize += (sColLenSize + sValue->mLength);
    }
    
    /* minitransaction 상태 정보 저장 */
    sAttr = smxmGetAttr( aMiniTrans );
    sTransId = smxmGetTransId( aMiniTrans );

    STL_RAMP( RAMP_RETRY_DELETE_FROM_ROOT );

    STL_TRY( smxmMarkSavepoint( aMiniTrans, &sSavepoint, aEnv ) == STL_SUCCESS );
    sNeedRetry = SMNMPB_RETRY_NONE;

    /* find leaf */
    STL_TRY( smnmpbFindTargetLeaf( aRelationHandle,
                                   aMiniTrans,
                                   aColumns,
                                   aBlockIdx,
                                   aRowRid,
                                   &sPathStack,
                                   &sPageHandle,
                                   aEnv ) == STL_SUCCESS );
  
    /* Leaf level 도달 */
    /* find insertable position & check uniqueness */
    *aIsUniqueResolved = STL_FALSE;

    /**  
     * aStatement가 NULL인 경우(Rollback)는 Uniqueness를 검사할 필요가 없다.
     */
    if( (sCheckUniqueness == STL_TRUE) &&
        (aStatement != NULL) )
    {    
        /* Uniqueness 체크 해야 함 */
        STL_TRY( smnmpbCheckUniqueness( aStatement,
                                        aMiniTrans,
                                        &sSavepoint,
                                        smsGetTbsId(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                                        aColumns,
                                        aBlockIdx,
                                        aRowRid,
                                        &sPageHandle,
                                        aIsUniqueResolved,
                                        &sIsMyStmtViolation,
                                        &sNeedRetry,
                                        aEnv ) == STL_SUCCESS );

        if( sNeedRetry != SMNMPB_RETRY_NONE )
        {
            STL_DASSERT( sNeedRetry == SMNMPB_RETRY_CONCURRENCY );
            STL_TRY( smxmCommit( aMiniTrans, aEnv ) == STL_SUCCESS );
            STL_TRY( smxmBegin( aMiniTrans,
                                sTransId,
                                smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                                sAttr,
                                aEnv ) == STL_SUCCESS );
            STL_THROW( RAMP_RETRY_DELETE_FROM_ROOT );
        }
    }

    STL_TRY( smnmpbFindPosition( aColumns,
                                 aBlockIdx,
                                 aRowRid,
                                 &sPageHandle,
                                 &sKeySlotSeq,
                                 &sFoundKey,
                                 aEnv ) == STL_SUCCESS );

    /* 반드시 해당 key가 존재하여야 함 */
    if( sFoundKey != STL_TRUE )
    {
        if( aStatement == NULL )
        {
            sCurCol = aColumns;

            while( sCurCol != NULL )
            {
                *sPtr = '[';
                sPtr++;
                (void)dtlToStringDataValue( KNL_GET_BLOCK_DATA_VALUE( sCurCol, aBlockIdx ),
                                            DTL_PRECISION_NA,
                                            DTL_SCALE_NA,
                                            SMP_PAGE_SIZE,
                                            sPtr,
                                            &sStrLen,
                                            KNL_DT_VECTOR(aEnv),
                                            aEnv,
                                            KNL_ERROR_STACK(aEnv) );
                sPtr += sStrLen;
                *sPtr = ']';
                sPtr++;
                sCurCol = sCurCol->mNext;
            }
            *sPtr = '\0';
        }

        /* undo 중엔 retry가 발생하면 안됨 */
        KNL_ASSERT( aStatement != NULL,
                    aEnv,
                    ("TxID(%ld), Page RID(%d,%d,%d), Row RID(%d,%d,%d)\nKey Value(%s)\nPage(%s)",
                     smxmGetTransId(aMiniTrans),
                     smpGetTbsId(&sPageHandle),
                     smpGetPageId(&sPageHandle),
                     sKeySlotSeq,
                     aRowRid->mTbsId,
                     aRowRid->mPageId,
                     aRowRid->mOffset,
                     sKeyVal,
                     knlDumpBinaryForAssert( SMP_FRAME(&sPageHandle),
                                             SMP_PAGE_SIZE,
                                             &aEnv->mLogHeapMem,
                                             KNL_ENV(aEnv))) );

        STL_TRY( smxmCommit( aMiniTrans, aEnv ) == STL_SUCCESS );
        STL_TRY( smxmBegin( aMiniTrans,
                            sTransId,
                            smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                            sAttr,
                            aEnv ) == STL_SUCCESS );
        
        STL_THROW( RAMP_ERR_INTERNAL );
    }

    sPathStack.mStack[sPathStack.mDepth].mPid = smpGetPageId(&sPageHandle);
    sPathStack.mStack[sPathStack.mDepth].mSeq = sKeySlotSeq;
    sPathStack.mStack[sPathStack.mDepth].mSmoNo = SMN_GET_PAGE_SMO_NO( &sPageHandle );
    sPathStack.mDepth++;
    
    STL_TRY( smnmpbDeleteFromLeaf( aStatement,
                                   aMiniTrans,
                                   aRelationHandle,
                                   &sPageHandle,
                                   sKeySlotSeq,
                                   aColumns,
                                   aBlockIdx,
                                   aRowRid,
                                   aUndoRid,
                                   sKeyBodySize,
                                   &sPathStack,
                                   aIsWriteUndoLog,
                                   &sNeedRetry,
                                   aEmptyPageCount,
                                   aEmptyPidList,
                                   aEnv ) == STL_SUCCESS );

    if( sNeedRetry != SMNMPB_RETRY_NONE )
    {
        /* undo 중엔 retry가 발생하면 안됨 */
        STL_ASSERT( aStatement != NULL );
        sState = 0;
        STL_TRY( smxmCommit( aMiniTrans, aEnv ) == STL_SUCCESS );
        
        if( sNeedRetry == SMNMPB_RETRY_NO_SPACE )
        {
            /**
             * 무한 Retry를 방지한다.
             * - Error는 이미 Push된 상태이다.
             * - Empty Node가 하나 이상 존재해야 retry한다.
             */
            STL_TRY( sNoSpaceRetryCount == 0 );
            STL_TRY( SMN_GET_EMPTY_NODE_COUNT(aRelationHandle) > 0 );
            
            stlPopError( KNL_ERROR_STACK(aEnv) );
            STL_TRY( smnmpbProcessEmptyNodes( aRelationHandle,
                                              aStatement->mTransId,
                                              aEnv ) == STL_SUCCESS );
            sNoSpaceRetryCount++;
        }
        
        STL_TRY( smxmBegin( aMiniTrans,
                            sTransId,
                            smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                            sAttr,
                            aEnv ) == STL_SUCCESS );
        sState = 1;
        STL_THROW( RAMP_RETRY_DELETE_FROM_ROOT );
    }

    /* 이후 free node를 위해 저장 */
    sActiveKeyCount = SMP_GET_ACTIVE_SLOT_COUNT(SMP_FRAME(&sPageHandle));
    sChildPid = smpGetPageId(&sPageHandle);
    sNextFreePageId = SMNMPB_GET_NEXT_FREE_PAGE(&sPageHandle);

    /* Delete에 의해 target page가 empty node가 되었다면 delete 과정에서
     * 발생한 SMO에 의해 empty node가 된 list에 추가한다 */
    if( (sActiveKeyCount == 0) &&
        (SMN_GET_ROOT_PAGE_ID(aRelationHandle) != sChildPid) &&
        (sNextFreePageId == SMP_NULL_PID) &&
        (SMN_GET_LAST_EMPTY_NODE(aRelationHandle) != sChildPid) )
    {
        aEmptyPidList[*aEmptyPageCount] = sChildPid;
        (*aEmptyPageCount)++;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INTERNAL,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      "smnmpbDelete()",
                      0 );
        
        KNL_SYSTEM_FATAL_DASSERT( STL_FALSE, KNL_ENV(aEnv) );
    }
    
    STL_FINISH;

    if( sState == 0 )
    {
        /**
         * 상위에서 생성한 Mini-Transaction을 만들어야 한다.
         */
        (void) smxmBegin( aMiniTrans,
                          sTransId,
                          smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                          sAttr,
                          aEnv );
    }

    return STL_FAILURE;
}


/**
 * @brief 주어진 row rid의 row를 가리키는 key를 삭제한다
 * @param[in] aStatement Statement
 * @param[in] aMiniTrans 사용하는 mini transaction
 * @param[in] aRelationHandle 삭제할 Key가 존재하는 relation의 핸들
 * @param[in] aColumns 삭제할 Key의 컬럼 정보들
 * @param[in] aRowRid 삭제할 Key가 가리키는 Row의 RID
 * @param[in,out] aEnv Storage Manager Environment
 * @note Rollback에서 사용되는 경우에 aStatement는 NULL일수 있다.
 */
stlStatus smnmpbUndeleteInternal( smlStatement      * aStatement,
                                  smxmTrans         * aMiniTrans,
                                  void              * aRelationHandle,
                                  knlValueBlockList * aColumns,
                                  smlRid            * aRowRid,
                                  smlEnv            * aEnv )
{
    smnmpbPathStack     sPathStack;
    smpHandle           sPageHandle;
    stlInt16            sSlotSeq;
    stlBool             sFoundKey;
    stlChar           * sKeySlot;
    stlInt16            sKeySize;
    stlUInt8            sDeleteRtsSeq;
    stlUInt8            sDeleteRtsVerNo;
    stlUInt8            sTmpRtsSeq;
    stlUInt8            sTmpRtsVerNo;
    stlUInt8            sDelTcn = SMNMPB_INVALID_TCN;
    smxlTransId         sTmpTransId = SML_INVALID_TRANSID;
    smlScn              sTmpScn = SML_INFINITE_SCN;
    stlChar             sLogBuf[SMP_PAGE_SIZE];
    stlChar           * sLogPtr;
    smnIndexHeader    * sIndexHeader;

    sIndexHeader = (smnIndexHeader*)aRelationHandle;
    
    /* Logical 하게 기존 key를 찾기 위해 위치를 찾는다 */
    STL_TRY( smnmpbFindTargetLeaf( aRelationHandle,
                                   aMiniTrans,
                                   aColumns,
                                   0,
                                   aRowRid,
                                   &sPathStack,
                                   &sPageHandle,
                                   aEnv ) == STL_SUCCESS );

    STL_TRY( smnmpbFindPosition( aColumns,
                                 0,
                                 aRowRid,
                                 &sPageHandle,
                                 &sSlotSeq,
                                 &sFoundKey,
                                 aEnv ) == STL_SUCCESS );

    /* 반드시 찾아야 함 */
    STL_ASSERT( sFoundKey == STL_TRUE );

    sKeySlot = smpGetNthRowItem( &sPageHandle, sSlotSeq );
    SMNMPB_UNSET_DELETED( sKeySlot );
    SMNMPB_GET_DEL_RTS_INFO( sKeySlot, &sDeleteRtsSeq, &sDeleteRtsVerNo );

    if( SMP_IS_VALID_RTS_SEQ(sDeleteRtsSeq) == STL_TRUE )
    {
        KNL_ASSERT( sDeleteRtsVerNo == 
                    (SMP_GET_NTH_RTS(SMP_FRAME(&sPageHandle), sDeleteRtsSeq) )->mPrevVerCnt,
                    aEnv,
                    ("RID(%d,%d,%d), RTS_SEQ(%d), RTS_VER_NO(%d), RTS_PREV_VER_CNT(%d)\nPAGE BODY\n%s",
                     aRowRid->mTbsId,
                     aRowRid->mPageId,
                     aRowRid->mOffset,
                     sDeleteRtsSeq,
                     sDeleteRtsVerNo,
                     (SMP_GET_NTH_RTS(SMP_FRAME(&sPageHandle), sDeleteRtsSeq) )->mPrevVerCnt,
                     knlDumpBinaryForAssert( SMP_FRAME(&sPageHandle),
                                             SMP_PAGE_SIZE,
                                             &aEnv->mLogHeapMem,
                                             KNL_ENV(aEnv))) );

        STL_TRY( smpEndUseRts( &sPageHandle,
                               sKeySlot,
                               sDeleteRtsSeq,
                               sDeleteRtsVerNo,
                               aEnv ) == STL_SUCCESS );
        sTmpRtsSeq = SMP_RTS_SEQ_NULL;
        sTmpRtsVerNo = 0;
        SMNMPB_SET_DEL_RTS_INFO( sKeySlot, &sTmpRtsSeq, &sTmpRtsVerNo );
    }
    else
    {
        /* tbk */
        KNL_ASSERT( sDeleteRtsSeq == SMP_RTS_SEQ_NULL,
                    aEnv,
                    ("RID(%d,%d,%d), sDeleteRtsSeq(%d)\nPAGE BODY\n%s",
                     aRowRid->mTbsId,
                     aRowRid->mPageId,
                     aRowRid->mOffset,
                     sDeleteRtsSeq,
                     knlDumpBinaryForAssert( SMP_FRAME(&sPageHandle),
                                             SMP_PAGE_SIZE,
                                             &aEnv->mLogHeapMem,
                                             KNL_ENV(aEnv))) );

        SMNMPB_SET_TBK_DEL_TRANS_ID( sKeySlot, &sTmpTransId);
        SMNMPB_SET_TBK_DEL_SCN( sKeySlot, &sTmpScn);
    }
    
    SMNMPB_SET_DEL_TCN( sKeySlot, &sDelTcn );

    sLogPtr = sLogBuf;
    sKeySize = smnmpbGetLeafKeySize(sKeySlot, sIndexHeader->mKeyColCount);
    stlMemcpy( sLogPtr, sKeySlot, sKeySize );
    sLogPtr += sKeySize;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_MEMORY_BTREE_UNDELETE_LEAF,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogPtr - sLogBuf,
                           sPageHandle.mPch->mTbsId,
                           smpGetPageId(&sPageHandle),
                           sSlotSeq,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );


    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(&sPageHandle) );
    SMP_REMOVE_RECLAIMED_SPACE( SMP_FRAME(&sPageHandle), sKeySize + STL_SIZEOF(smpOffsetSlot) );

    STL_TRY( smnmpbValidateIndexPage( &sPageHandle,
                                      aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnmpbProcessEmptyNodes( void        * aRelationHandle,
                                   smxlTransId   aTransId,
                                   smlEnv      * aEnv )
{
    stlInt32        i;
    smlRid          sSegRid;
    smxmTrans       sMiniTrans;
    smxmSavepoint   sSavepoint;
    stlUInt32       sAttr;
    stlInt16        sState = 0;
    smpHandle       sSegPage;
    smlPid          sNextPid;
    stlBool         sSkipRemove;

    if( SMN_GET_LOGGING_FLAG( aRelationHandle ) == STL_TRUE )
    {
        sAttr = SMXM_ATTR_REDO;
    }
    else
    {
        sAttr = SMXM_ATTR_NOLOG_DIRTY;
    }

    for( i = 0; i < SMNMPB_MAX_INDEX_DEPTH; i++ )
    {
        if( (SMN_GET_EMPTY_NODE_COUNT(aRelationHandle) > 0) &&
            (SMN_GET_IS_AGING_EMPTY_NODE(aRelationHandle) != STL_TRUE) )
        {
            sSegRid = smsGetSegRid( SME_GET_SEGMENT_HANDLE(aRelationHandle) );

            STL_TRY( smxmBegin( &sMiniTrans,
                                aTransId,
                                smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                                sAttr,
                                aEnv ) == STL_SUCCESS );
            sState = 1;

            STL_TRY( smxmMarkSavepoint( &sMiniTrans, &sSavepoint, aEnv ) == STL_SUCCESS );

            STL_TRY( smpAcquire( &sMiniTrans,
                                 sSegRid.mTbsId,
                                 sSegRid.mPageId,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sSegPage,
                                 aEnv ) == STL_SUCCESS );

            if( SMN_GET_IS_AGING_EMPTY_NODE(aRelationHandle) == STL_TRUE )
            {
                /* 누군가 Aging 진행중. 믿고 중단한다 */
                sState = 0;
                STL_TRY( smxmRollback( &sMiniTrans, aEnv ) == STL_SUCCESS );
                STL_THROW( RAMP_SKIP_PROCESSING );
            }
            
            /* Aging 하겠다고 찜함 */
            ((smnIndexHeader*)aRelationHandle)->mIsAgingEmptyNode = STL_TRUE; 
            sState = 2;

            STL_TRY( smxmRollbackToSavepoint( &sMiniTrans,
                                              &sSavepoint, 
                                              aEnv ) == STL_SUCCESS );

            if( SMN_GET_FIRST_EMPTY_NODE(aRelationHandle) == SMP_NULL_PID )
            {
                ((smnIndexHeader*)aRelationHandle)->mIsAgingEmptyNode = STL_FALSE; 
                STL_TRY( smxmRollback( &sMiniTrans, aEnv ) == STL_SUCCESS );
                break;
            }

            sSkipRemove = STL_FALSE;
            sNextPid = SMP_NULL_PID;
            STL_TRY( smnmpbRemoveEmptyNodeFromTree( aRelationHandle,
                                                    &sMiniTrans,
                                                    &sSkipRemove,
                                                    &sNextPid,
                                                    aEnv ) == STL_SUCCESS );

            /* aging flag를 다시 되돌림 */
            STL_TRY( smpAcquire( &sMiniTrans,
                                 sSegRid.mTbsId,
                                 sSegRid.mPageId,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sSegPage,
                                 aEnv ) == STL_SUCCESS );

            ((smnIndexHeader*)aRelationHandle)->mIsAgingEmptyNode = STL_FALSE; 

            sState = 0;
            STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );

            if( sSkipRemove == STL_TRUE )
            {
                STL_THROW( RAMP_SKIP_PROCESSING );
            }
 
            /* Empty node들의 next 노드들 중 빈 마지막 노드가 있으면 등록한다 */
            if( sNextPid != SMP_NULL_PID )
            {
                STL_TRY( smxmBegin( &sMiniTrans,
                                    aTransId,
                                    smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                                    sAttr,
                                    aEnv ) == STL_SUCCESS );
                sState = 1;

                STL_TRY( smnmpbRegisterEmptyNode( aRelationHandle,
                                                  &sMiniTrans,
                                                  1,
                                                  &sNextPid,
                                                  STL_TRUE, /* aCheckAgingFlag */
                                                  aEnv ) == STL_SUCCESS );

                sState = 0;
                STL_TRY( smxmCommit( &sMiniTrans, aEnv ) == STL_SUCCESS );
            } /* if */
        }
        else
        {
            break;
        } /* if */
    } /* for */

    STL_RAMP( RAMP_SKIP_PROCESSING );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            {
                ((smnIndexHeader*)aRelationHandle)->mIsAgingEmptyNode = STL_FALSE; 
            }
        case 1:
            {
                (void)smxmRollback( &sMiniTrans, aEnv );
            }
        default:
            {
                break;
            }
    }

    return STL_FAILURE;
}


/**
 * @brief index의 특정 level에서 특정 node를 제거한다
 * @param[in] aMiniTrans logging 및 page latch에 사용할 mini transaction
 * @param[in] aPrevPage 제거할 노드의 이전 페이지
 * @param[in] aTargetPage 제거할 페이지
 * @param[in] aNextPage 제거할 노드의 다음 페이지
 * @param[in] aSmoNo Target 페이지에 설정될 SMO Number
 * @param[in,out] aEnv Storage Manager Environment
 */
stlStatus smnmpbRemoveSiblingLink( smxmTrans * aMiniTrans,
                                   smpHandle * aPrevPage,
                                   smpHandle * aTargetPage,
                                   smpHandle * aNextPage,
                                   stlUInt64   aSmoNo,
                                   smlEnv    * aEnv )
{
    smlPid   sPrevPid = SMNMPB_GET_PREV_PAGE(aTargetPage);
    smlPid   sNextPid = SMNMPB_GET_NEXT_PAGE(aTargetPage);

    if( aPrevPage != NULL )
    {
        (SMNMPB_GET_LOGICAL_HEADER(aPrevPage))->mNextPageId = sNextPid;
            
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_SET_NEXT_PAGE_ID,
                               SMR_REDO_TARGET_PAGE,
                               (stlChar*)&sNextPid,
                               STL_SIZEOF(smlPid),
                               aPrevPage->mPch->mTbsId,
                               aPrevPage->mPch->mPageId,
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }

    if( aNextPage != NULL )
    {
        (SMNMPB_GET_LOGICAL_HEADER(aNextPage))->mPrevPageId = sPrevPid;
            
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_SET_PREV_PAGE_ID,
                               SMR_REDO_TARGET_PAGE,
                               (stlChar*)&sPrevPid,
                               STL_SIZEOF(smlPid),
                               aNextPage->mPch->mTbsId,
                               aNextPage->mPch->mPageId,
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }

    /**
     * mirrored root로 인하여 root page는 제외시킨다.
     */
    if( (aPrevPage != NULL) || (aNextPage != NULL) )
    {
        SMN_SET_PAGE_SMO_NO( aTargetPage, aSmoNo );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief index header의 empty node list에서 특정 node를 제거한다
 * @param[in] aRelationHandle index의 핸들
 * @param[in] aMiniTrans logging 및 page latch에 사용할 mini transaction
 * @param[in] aSegPage index sement page의 핸들
 * @param[in] aEmptyPage empty node list에서 제거될 page의 핸들
 * @param[in,out] aEnv Storage Manager Environment
 */
stlStatus smnmpbRemoveEmptyNode( void      * aRelationHandle,
                                 smxmTrans * aMiniTrans,
                                 smpHandle * aSegPage,
                                 smpHandle * aEmptyPage,
                                 smlEnv    * aEnv )
{
    smnIndexHeader * sHdr;
    smlPid           sEmptyPid = smpGetPageId(aEmptyPage);
    smlPid           sNextEmptyPid = SMNMPB_GET_NEXT_FREE_PAGE(aEmptyPage);
    stlChar          sLogBuf[STL_SIZEOF(smlPid) * 2];
    stlChar        * sLogPtr = sLogBuf;
    
    STL_DASSERT( SMNMPB_IS_ON_EMPTY_LIST( aEmptyPage ) == STL_TRUE );

    sHdr = (smnIndexHeader*)((stlChar*)smpGetBody(SMP_FRAME(aSegPage)) +
                             smsGetHeaderSize(SME_GET_SEGMENT_HANDLE(aRelationHandle)));

    if( SMN_GET_EMPTY_NODE_COUNT(aRelationHandle) == 1 )
    {
        sHdr->mFirstEmptyNode = SMP_NULL_PID;
        sHdr->mLastEmptyNode = SMP_NULL_PID;
        sHdr->mEmptyNodeCount = 0;
    }
    else
    {
        sHdr->mFirstEmptyNode = SMNMPB_GET_NEXT_FREE_PAGE(aEmptyPage);
        sHdr->mEmptyNodeCount--;
    }

    ((smnIndexHeader*)aRelationHandle)->mEmptyNodeCount = sHdr->mEmptyNodeCount;
    ((smnIndexHeader*)aRelationHandle)->mFirstEmptyNode = sHdr->mFirstEmptyNode;
    ((smnIndexHeader*)aRelationHandle)->mLastEmptyNode = sHdr->mLastEmptyNode;

    /* write REDO log */
    STL_WRITE_INT32(sLogPtr, &sEmptyPid);
    sLogPtr += STL_SIZEOF(smlPid);
    STL_WRITE_INT32(sLogPtr, &sNextEmptyPid);
    sLogPtr += STL_SIZEOF(smlPid);

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_REMOVE_EMPTY_NODE,
                           SMR_REDO_TARGET_PAGE,
                           sLogBuf,
                           sLogPtr - sLogBuf,
                           aSegPage->mPch->mTbsId,
                           aSegPage->mPch->mPageId,
                           (stlChar*)sHdr - (stlChar*)SMP_FRAME(aSegPage),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    SMNMPB_GET_LOGICAL_HEADER(aEmptyPage)->mFlag &= ~SMNMPB_NODE_FLAG_EMPTY_LIST_MASK;
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_INDEX,
                           SMR_LOG_MEMORY_BTREE_SET_NODE_FLAG,
                           SMR_REDO_TARGET_PAGE,
                           (void*)&(SMNMPB_GET_LOGICAL_HEADER(aEmptyPage)->mFlag),
                           STL_SIZEOF(stlUInt8),
                           aEmptyPage->mPch->mTbsId,
                           smpGetPageId(aEmptyPage),
                           0,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief tree에서 제거된 child node를 가리켰던 parent node의 key를 찾아 삭제한다
 *        만일 삭제로 인해 node가 비게되면 free list에 달고 recursion 한다
 * @param[in] aRelationHandle index의 핸들
 * @param[in] aMiniTrans logging 및 page latch에 사용할 mini transaction
 * @param[in] aChildPid tree에서 제거된 child node의 page id
 * @param[in] aPathStack parent node를 찾아가기 위한 path 정보
 * @param[in] aCurDepth aPathStack에서 현재 depth 정보
 * @param[out] aSegPage Index segment handle로, SMO의 최상위에서 구해져 내려온다
 * @param[out] aNeedRetry SMO 시도 도중에 root가 바뀜을 발견하여 처음부터 다시해야 한다
 * @param[out] aSkipRemove Key의 삭제를 Skip 했는지 여부
 * @param[out] aRegisterNextLeaf Next Leaf Node의 free 가능성을 조사해야 하는지 여부
 * @param[in,out] aEnv Storage Manager Environment
 */
stlStatus smnmpbRemoveIntlKey( void            * aRelationHandle,
                               smxmTrans       * aMiniTrans,
                               smlPid            aChildPid,
                               smnmpbPathStack * aPathStack,
                               stlInt16          aCurDepth,
                               smpHandle       * aSegPage,
                               stlInt32        * aNeedRetry,
                               stlBool         * aSkipRemove,
                               stlBool         * aRegisterNextLeaf,
                               smlEnv          * aEnv )
{
    smpHandle            sPageHandle;
    smlPid               sCurPid;
    stlInt16             sCurSeq;
    stlChar            * sKeySlot = NULL;
    stlChar            * sPrevKeySlot = NULL;
    stlInt16             sSlotSize;
    stlUInt64            sSmoNo;
    smlPid               sChildPid;
    smlPid               sPrevChildPid;
    stlUInt16            sTotalKeyCount = 0;
    stlUInt16            sKeyColCount;
    smpRowItemIterator   sIterator;
    stlBool              sIsFirstTry = STL_TRUE;
    stlUInt64            sIndexSmoNo;
    smlRid               sSegRid;
    stlInt32             i;
    smpHandle            sMirrorPageHandle;
    smpHandle          * sRootPageHandle;
    stlInt32             sNeedRetry;

    *aNeedRetry = SMNMPB_RETRY_NONE;
    *aSkipRemove = STL_FALSE;
    *aRegisterNextLeaf = STL_FALSE;
    sNeedRetry = SMNMPB_RETRY_NONE;

    /* 1. target page를 잡는다 */
    sCurPid = aPathStack->mStack[aCurDepth - 1].mPid;
    sCurSeq = aPathStack->mStack[aCurDepth - 1].mSeq;
    sSmoNo  = aPathStack->mStack[aCurDepth - 1].mSmoNo;
    while( STL_TRUE )
    {
        STL_TRY( smpAcquire( aMiniTrans,
                             smsGetTbsId(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                             sCurPid,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPageHandle,
                             aEnv ) == STL_SUCCESS );

        STL_DASSERT( smpGetFreeness( &sPageHandle ) != SMP_FREENESS_FREE );
        sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(&sPageHandle);

        if( sIsFirstTry == STL_TRUE )
        {
            sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle );
            if( SMN_GET_PAGE_SMO_NO(&sPageHandle) == sSmoNo )
            {
                sKeySlot = smpGetNthRowItem( &sPageHandle, sCurSeq );

                /* 내려올때 지났던 바로 그 slot이 아직 valid한지 체크한다 */
                STL_WRITE_INT32( &sChildPid, sKeySlot );
                if( sChildPid == aChildPid )
                {
                    STL_THROW( RAMP_DO_DELETE );
                }
            }
        }
        else
        {
            /* Next node로 이동했을 시에 해당 노드의 SMO no는 기존 노드보다 크거나 같아야한다 */
            STL_ASSERT( SMN_GET_PAGE_SMO_NO( &sPageHandle ) >= sSmoNo );
        }

        /* 이 노드안에서 해당 Org PID를 가지는 key가 있는지 체크한다 */
        SMP_GET_FIRST_ROWITEM( SMP_FRAME(&sPageHandle), &sIterator, sKeySlot );
        sCurSeq = 0;
        while( sKeySlot != NULL )
        {
            STL_WRITE_INT32( &sChildPid, sKeySlot );
            if( sChildPid == aChildPid )
            {
                STL_THROW( RAMP_DO_DELETE );
            }
            /* index에는 free offset slot이 중간에 없으므로 그냥 ++ 해도 된다 */
            SMP_GET_NEXT_ROWITEM( &sIterator, sKeySlot );
            sCurSeq++;
        }

        /* 현재 노드에는 target pid를 child로 갖는 key가 없으므로 다음 노드로 이동한다 */
        sIsFirstTry = STL_FALSE;
        aPathStack->mStack[aCurDepth - 1].mPid = SMNMPB_GET_NEXT_PAGE( &sPageHandle );
        aPathStack->mStack[aCurDepth - 1].mSeq = 0;
        sCurPid = aPathStack->mStack[aCurDepth - 1].mPid;
        /* 다음 노드로 이동해도 SMO NO는 그대로 유지함 */
        STL_ASSERT( aPathStack->mStack[aCurDepth - 1].mPid != SMP_NULL_PID );
    }

    /* internal node는 sibling에 대한 link를 조정해 줄 필요가 없으므로 prev, next를 잡지 않음 */

    STL_RAMP( RAMP_DO_DELETE );

    /**
     * Max Key는 High Key에서 같이 처리된다. 하지만 나중을 위해서 코드는 남겨놓는다.
     */
    if( (SMNMPB_IS_LAST_SIBLING( &sPageHandle ) == STL_TRUE) &&
        (sCurSeq + 1 == sTotalKeyCount) )
    {
        /* MAX Key */
        sSlotSize = STL_SIZEOF(smlPid);
        if( sTotalKeyCount == 1 )
        {
            sSegRid = smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle));
            STL_TRY( smpAcquire( aMiniTrans,
                                 sSegRid.mTbsId,
                                 sSegRid.mPageId,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 aSegPage,
                                 aEnv ) == STL_SUCCESS );
            *aSkipRemove = STL_TRUE;
            STL_THROW( RAMP_FINISH_REMOVING );
        }
    }
    else
    {
        sSlotSize = smnmpbGetIntlKeySize(sKeySlot, sKeyColCount);
    }

    if( sTotalKeyCount == 1 )
    {
        if( aCurDepth == 1 )
        {
            if( aPathStack->mStack[0].mSmoNo > SMN_GET_PAGE_SMO_NO( &sPageHandle ) )
            {
                /* SMO 최상단 노드에 이미 다른 SMO가 발생하여 */
                /* 현재 내가 가진 pathstack이 valid하지 않음 -> 처음부터 재시도 */
                *aNeedRetry = SMNMPB_RETRY_CONCURRENCY;
            }
            else
            {
                /* root 페이지의 마지막 key는 삭제하지 않는다 */
                *aSkipRemove = STL_TRUE;
                sSegRid = smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle));
                STL_TRY( smpAcquire( aMiniTrans,
                                     sSegRid.mTbsId,
                                     sSegRid.mPageId,
                                     KNL_LATCH_MODE_EXCLUSIVE,
                                     aSegPage,
                                     aEnv ) == STL_SUCCESS );
            }
        }
        else
        {
            /* 한개 남은 key를 지워야 할 경우에는 상위 노드의 parent key를 삭제한 후에 한다 */
            /* 현재 index level을 낮추는 작업을 하지 않으므로 root가 아니어야 함 */
            STL_DASSERT( aCurDepth > 1 );
            STL_TRY( smnmpbRemoveIntlKey( aRelationHandle,
                                          aMiniTrans,
                                          smpGetPageId(&sPageHandle),
                                          aPathStack,
                                          aCurDepth - 1,
                                          aSegPage,
                                          aNeedRetry,
                                          aSkipRemove,
                                          aRegisterNextLeaf,
                                          aEnv ) == STL_SUCCESS );
        }

        if( (*aNeedRetry != SMNMPB_RETRY_NONE)  ||
            (*aSkipRemove == STL_TRUE) )
        {
            STL_THROW( RAMP_FINISH_REMOVING );
        }

        /* node를 free한다 */
        STL_TRY( smsUpdatePageStatus( aMiniTrans,
                                      SME_GET_SEGMENT_HANDLE(aRelationHandle),
                                      &sPageHandle,
                                      SMP_FREENESS_FREE,
                                      smxlGetSystemScn(),
                                      aEnv ) == STL_SUCCESS );
    }
    else
    {
        /* SMO의 최상위 */
        /* Segment page를 잡는다 */    
        sSegRid = smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle));
        STL_TRY( smpAcquire( aMiniTrans,
                             sSegRid.mTbsId,
                             sSegRid.mPageId,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             aSegPage,
                             aEnv ) == STL_SUCCESS );

        if( sCurSeq + 1 == sTotalKeyCount )
        {
            *aSkipRemove = STL_TRUE;
            STL_THROW( RAMP_FINISH_REMOVING );
        }

        if( sTotalKeyCount == 2 )
        {
            /* 2개의 internal key 중 첫번째 것을 지우면 마지막 key(high key) 만
             * 남게 되므로, 이 key가 free 가능한지 테스트 하기위해
             * 현재 SMO가 종료된 후 next leaf node를 empty node list에 등록한다
             */
            *aRegisterNextLeaf = STL_TRUE;
        }

        /* 현재 index smo no 세팅 */
        sIndexSmoNo = SMN_GET_NEXT_INDEX_SMO_NO(aRelationHandle);
        for( i = 0; i < aPathStack->mDepth; i++ )
        {
            aPathStack->mStack[i].mSmoNo = sIndexSmoNo;
        }
    }

    SMN_SET_PAGE_SMO_NO( &sPageHandle,  aPathStack->mStack[aCurDepth - 1].mSmoNo);

    if( sSlotSize == STL_SIZEOF(smlPid) )
    {
        /* MAX key를 삭제하면 안되므로 대신에 바로 전 key를 지우고 */
        /* 해당 key의 child pid를 가져온다  */
        STL_ASSERT( sTotalKeyCount != 1 );
        sPrevKeySlot = smpGetNthRowItem( &sPageHandle, sCurSeq - 1 );
        STL_WRITE_INT32(&sPrevChildPid, sPrevKeySlot);
        SMNMPB_INTL_SLOT_SET_CHILD_PID( sKeySlot, &sPrevChildPid );
        sSlotSize = smnmpbGetIntlKeySize(sPrevKeySlot, sKeyColCount);
        STL_TRY( smpFreeNthSlot( &sPageHandle, sCurSeq - 1, sSlotSize )== STL_SUCCESS );
        SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(&sPageHandle), sSlotSize );
        /**
        knlLogMsg(  NULL,
                    (knlEnv*)aEnv,
                    KNL_LOG_LEVEL_INFO,
                    "ADD_RECL : TBS_ID(%d), PAGE_ID(%d), RECL(%d), SLOT_SIZE(%d)",
                    smpGetTbsId(&sPageHandle),
                    smpGetPageId(&sPageHandle),
                    smpGetReclaimedSpace(SMP_FRAME(&sPageHandle)),
                    sSlotSize );
                    */

        /* write REDO log */
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_SET_CHILD_PID,
                               SMR_REDO_TARGET_PAGE,
                               (stlChar*)&sPrevChildPid,
                               STL_SIZEOF(smlPid),
                               sPageHandle.mPch->mTbsId,
                               sPageHandle.mPch->mPageId,
                               sCurSeq,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_DELETE_INTL,
                               SMR_REDO_TARGET_PAGE,
                               (stlChar*)&sPrevChildPid,
                               STL_SIZEOF(smlPid),
                               sPageHandle.mPch->mTbsId,
                               sPageHandle.mPch->mPageId,
                               sCurSeq - 1,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smpFreeNthSlot( &sPageHandle, sCurSeq, sSlotSize )== STL_SUCCESS );
        SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(&sPageHandle), sSlotSize );
        /**
        knlLogMsg(  NULL,
                    (knlEnv*)aEnv,
                    KNL_LOG_LEVEL_INFO,
                    "ADD_RECL : TBS_ID(%d), PAGE_ID(%d), RECL(%d), SLOT_SIZE(%d)",
                    smpGetTbsId(&sPageHandle),
                    smpGetPageId(&sPageHandle),
                    smpGetReclaimedSpace(SMP_FRAME(&sPageHandle)),
                    sSlotSize );
        */
        /* write REDO log */
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_DELETE_INTL,
                               SMR_REDO_TARGET_PAGE,
                               (stlChar*)&aChildPid,
                               STL_SIZEOF(smlPid),
                               sPageHandle.mPch->mTbsId,
                               sPageHandle.mPch->mPageId,
                               sCurSeq,  /* Physiological redo를 하기위해 offset이 아닌 seq 사용 */
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }

    SMP_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(&sPageHandle) );

    if( (aCurDepth == 1) &&
        (SMN_GET_MIRROR_ROOT_PID(aRelationHandle) != SMP_NULL_PID) )
    {
        sRootPageHandle = smxmGetPageHandle( aMiniTrans,
                                             smsGetTbsId(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                                             SMN_GET_ROOT_PAGE_ID(aRelationHandle),
                                             KNL_LATCH_MODE_EXCLUSIVE );

        if( sRootPageHandle != NULL )
        {
            STL_TRY( smnmpbAllocMultiPage( aMiniTrans,
                                           SME_GET_SEGMENT_HANDLE(aRelationHandle),
                                           SMP_PAGE_TYPE_INDEX_DATA,
                                           1,
                                           &sMirrorPageHandle,
                                           &sNeedRetry,
                                           aEnv )
                     == STL_SUCCESS );

            if( sNeedRetry == SMNMPB_RETRY_NO_SPACE )
            {
                /**
                 * Empty Node 를 처리하는 도중 공간 할당요구는 무시한다.
                 */
                
                stlPopError( KNL_ERROR_STACK(aEnv) );
                
                STL_TRY( smnmpbMirrorRoot( aRelationHandle,
                                           aMiniTrans,
                                           NULL, /* aMirrorPageHandle */
                                           aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                STL_TRY( smnmpbMirrorRoot( aRelationHandle,
                                           aMiniTrans,
                                           &sMirrorPageHandle,
                                           aEnv )
                         == STL_SUCCESS );
            }
        }
    }

    STL_RAMP( RAMP_FINISH_REMOVING );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief root페이지와 mirror 페이지를 동기화 시킨다.
 *        empty node list에서 free node list로 이동시킨다
 * @param[in] aRelationHandle index 핸들
 * @param[in] aMiniTrans logging시 mini transaction
 * @param[in] aPageHandle mirror page handle
 * @param[in,out] aEnv Storage Manager Environment
 */
stlStatus smnmpbMirrorRoot( void      * aRelationHandle,
                            smxmTrans * aMiniTrans,
                            smpHandle * aPageHandle,
                            smlEnv    * aEnv )
{
    smpHandle * sRootPageHandle;
    smpHandle   sOldMirrorPageHandle;
    smlPid      sMirrorPid = SMP_NULL_PID;
    smlPid      sOldMirrorPid = SMP_NULL_PID;
    smpPhyHdr * sSrcPhyHdr;
    smpPhyHdr * sDstPhyHdr;

    sRootPageHandle = smxmGetPageHandle( aMiniTrans,
                                         smsGetTbsId(SME_GET_SEGMENT_HANDLE(aRelationHandle)),
                                         SMN_GET_ROOT_PAGE_ID(aRelationHandle),
                                         KNL_LATCH_MODE_EXCLUSIVE );

    STL_ASSERT( sRootPageHandle != NULL );

    if( aPageHandle != NULL )
    {
        sSrcPhyHdr = (smpPhyHdr*)SMP_FRAME( sRootPageHandle );
        sDstPhyHdr = (smpPhyHdr*)SMP_FRAME( aPageHandle );
        
        stlMemcpy( sDstPhyHdr->mVolatileArea,
                   sSrcPhyHdr->mVolatileArea,
                   SMP_VOLATILE_AREA_SIZE );
        
        stlMemcpy( smpGetBody(SMP_FRAME(aPageHandle)),
                   smpGetBody(SMP_FRAME(sRootPageHandle)),
                   SMP_PAGE_SIZE - STL_SIZEOF(smpPhyHdr) );
        
        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_PAGE_IMAGE,
                               SMR_REDO_TARGET_PAGE,
                               SMP_FRAME(aPageHandle),
                               SMP_PAGE_SIZE,
                               smpGetTbsId(aPageHandle),
                               smpGetPageId(aPageHandle),
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
        
        sMirrorPid = smpGetPageId( aPageHandle );
    }

    sOldMirrorPid = SMN_GET_MIRROR_ROOT_PID( aRelationHandle );
    
    STL_TRY( smnSetMirrorRootPageId( aMiniTrans,
                                     aRelationHandle,
                                     sMirrorPid,
                                     aEnv )
             == STL_SUCCESS );
    
    if( sOldMirrorPid != SMP_NULL_PID )
    {
        STL_TRY( smpAcquire( aMiniTrans,
                             smpGetTbsId( sRootPageHandle ),
                             sOldMirrorPid,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sOldMirrorPageHandle,
                             aEnv )
                 == STL_SUCCESS );
                             
        STL_TRY( smsUpdatePageStatus( aMiniTrans,
                                      SME_GET_SEGMENT_HANDLE(aRelationHandle),
                                      &sOldMirrorPageHandle,
                                      SMP_FREENESS_FREE,
                                      smxlGetSystemScn(),
                                      aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief empty node list중 free가 가능한 node를 찾아 인덱스 tree에서 제거하고(SMO)
 *        empty node list에서 free node list로 이동시킨다
 * @param[in] aRelationHandle empty node를 달고 있는 index의 핸들
 * @param[in] aMiniTrans logging시 mini transaction
 * @param[out] aSkipRemove free Page를 못했을 경우
 * @param[out] aNextPid empty node의 next node가 빈 페이지인 경우 등록하기 위해 리턴한다
 * @param[in,out] aEnv Storage Manager Environment
 */
stlStatus smnmpbRemoveEmptyNodeFromTree( void       * aRelationHandle,
                                         smxmTrans  * aMiniTrans,
                                         stlBool    * aSkipRemove,
                                         smlPid     * aNextPid,
                                         smlEnv     * aEnv )
{
    smnmpbPathStack      sPathStack;
    stlUInt16            sState = 0;
    knlValueBlockList  * sValCols = NULL;
    knlValueBlockList  * sCurCol;
    knlValueBlockList  * sPrevCol;
    stlChar              sValueBuf[SMP_PAGE_SIZE];
    stlInt16             i;
    stlInt32             sNeedRetry = SMNMPB_RETRY_NONE;
    stlBool              sSkipRemove = STL_FALSE;
    stlBool              sRegisterNextLeaf = STL_FALSE;
    smlTbsId             sTbsId = smsGetTbsId(SME_GET_SEGMENT_HANDLE(aRelationHandle));
    stlUInt16            sKeyColCount;
    stlUInt8           * sKeyColTypes;
    smpHandle            sSegPage;
    smpHandle            sTargetPage;
    smpHandle            sPrevPage;
    smpHandle            sNextPage;
    smlPid               sTargetPid;
    smlPid               sPrevPid;
    smlPid               sNextPid;
    stlChar            * sColPtr;
    smlRid               sRowRid;
    stlChar            * sKeySlot;
    smpRowItemIterator   sIterator;
    smlScn               sAgableScn;
    smlRid               sSegRid;
    smlPid               sTmpPid;
    knlRegionMark        sMemMark;
    stlBool              sFixPage = STL_FALSE;
    smxmSavepoint        sSavepoint;
    stlUInt8             sRtsCount;
    stlBool              sIsAgable;
    stlUInt16            sSlotHdrSize;
    stlInt16             sNextLeafKeyCount = 0;

    *aSkipRemove = STL_FALSE;

    STL_TRY( smxmMarkSavepoint( aMiniTrans, &sSavepoint, aEnv ) == STL_SUCCESS );

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    /* 처리할게 있으면.... */
    STL_TRY_THROW( SMN_GET_EMPTY_NODE_COUNT(aRelationHandle) > 0, RAMP_SKIP_REMOVING );

    /* 첫 empty node를 얻는다 */
    sTargetPid = SMN_GET_FIRST_EMPTY_NODE(aRelationHandle);
    STL_TRY( smpAcquire( NULL,
                         sTbsId,
                         sTargetPid,
                         KNL_LATCH_MODE_SHARED,
                         &sTargetPage,
                         aEnv ) == STL_SUCCESS );
    sFixPage = STL_TRUE;

    STL_DASSERT( smpGetFreeness( &sTargetPage ) != SMP_FREENESS_FREE );

    sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(&sTargetPage);
    sKeyColTypes = SMNMPB_GET_KEY_COLUMN_TYPES(&sTargetPage);

    sPrevCol = NULL;
    for( i = 0; i < sKeyColCount; i++ )
    {
        STL_TRY( knlInitBlockNoBuffer( &sCurCol,
                                       STL_LAYER_STORAGE_MANAGER,
                                       1,
                                       sKeyColTypes[i],
                                       &aEnv->mOperationHeapMem,
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
        if( i == 0 )
        {
            sValCols = sCurCol;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevCol, sCurCol );
        }
        sPrevCol = sCurCol;
    }

    /* 아무 key나 하나 잡아서 value와 rid를 얻고, 이를 이용하여 pathstack을 채운다 */
    sKeySlot = smpGetNthRowItem( &sTargetPage, 0 );
    SMNMPB_GET_LEAF_KEY_HDR_SIZE( sKeySlot, &sSlotHdrSize );
    sColPtr = sKeySlot + sSlotHdrSize;
    smnmpbMakeValueListFromSlot( sColPtr, sValCols, sValueBuf );
    sRowRid.mTbsId = 0;
    SMNMPB_GET_ROW_PID( sKeySlot, &sRowRid.mPageId );
    SMNMPB_GET_ROW_SEQ( sKeySlot, &sRowRid.mOffset );

    sFixPage = STL_FALSE;
    STL_TRY( smpRelease( &sTargetPage, aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_RETRY_REMOVE_FROM_ROOT );

    sNeedRetry = SMNMPB_RETRY_NONE;

    /* find leaf & fill pathstack */
    STL_TRY( smnmpbFindTargetLeaf( aRelationHandle,
                                   aMiniTrans,
                                   sValCols,
                                   0,
                                   &sRowRid,
                                   &sPathStack,
                                   &sTargetPage,
                                   aEnv ) == STL_SUCCESS );
  
    /* Leaf level 도달 */
    /* 목적했던 node가 아니면 split이 발생한 경우이므로 사용중이라 간주, 무시 */
    if( smpGetPageId(&sTargetPage) != sTargetPid )
    {
        STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                          &sSavepoint, 
                                          aEnv ) == STL_SUCCESS );
        
        STL_THROW( RAMP_RETRY_REMOVE_FROM_ROOT );
    }
    
    sPrevPid = SMNMPB_GET_PREV_PAGE(&sTargetPage);

    STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                      &sSavepoint, 
                                      aEnv ) == STL_SUCCESS );

    /* prev -> target -> next 순서로 page를 잡는다 */
    while( sPrevPid != SMP_NULL_PID )
    {
        STL_TRY( smpAcquire( aMiniTrans,
                             sTbsId,
                             sPrevPid,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sPrevPage,
                             aEnv ) == STL_SUCCESS );
        STL_DASSERT( smpGetFreeness( &sPrevPage ) != SMP_FREENESS_FREE );
        if( SMNMPB_GET_NEXT_PAGE( &sPrevPage ) == sTargetPid )
        {
            break;
        }

        sPrevPid = SMNMPB_GET_NEXT_PAGE(&sPrevPage);

        /**
         * Aging Flag를 설정한 이후에 TargetPid를 구했기 때문에
         * Target Page는 인덱스에 반드시 연결되어 있어야 한다.
         */
        STL_DASSERT( sPrevPid != SMP_NULL_PID );
    }

    STL_TRY( smpAcquire( aMiniTrans,
                         sTbsId,
                         sTargetPid,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sTargetPage,
                         aEnv ) == STL_SUCCESS );

    STL_DASSERT( smpGetFreeness( &sTargetPage ) != SMP_FREENESS_FREE );
    sNextPid = SMNMPB_GET_NEXT_PAGE(&sTargetPage);
    if( sNextPid != SMP_NULL_PID )
    {
        STL_TRY( smpAcquire( aMiniTrans,
                             sTbsId,
                             sNextPid,
                             KNL_LATCH_MODE_EXCLUSIVE,
                             &sNextPage,
                             aEnv ) == STL_SUCCESS );
        
        sNextLeafKeyCount = SMP_GET_ACTIVE_SLOT_COUNT( SMP_FRAME(&sNextPage) );
        STL_DASSERT( smpGetFreeness( &sNextPage ) != SMP_FREENESS_FREE );
    }

    sRtsCount = SMP_GET_CURRENT_RTS_COUNT( SMP_FRAME(&sTargetPage) );
    /* Rts들을 모두 Stamping 한다 */
    for( i = 0; i < sRtsCount; i++ )
    {
        smpTryStampRts( NULL, sTbsId, &sTargetPage, i, NULL, aEnv );
    }

    /* 모든 key들이 다 dead key 상태인지 체크한다 */
    SMP_GET_FIRST_ROWITEM(SMP_FRAME(&sTargetPage), &sIterator, sKeySlot);
    sAgableScn = smxlGetAgableTbsScn( sTbsId, aEnv );
    while( sKeySlot != NULL )
    {
        if( SMNMPB_IS_DELETED( sKeySlot ) != STL_TRUE )
        {
            /* delete 됐던 key가 rollback되어 다시 active key가 되었음. Empty Node List에서 제거 */
            /* Segment page를 잡는다 */    
            sSegRid = smsGetSegRid(SME_GET_SEGMENT_HANDLE(aRelationHandle));
            STL_TRY( smpAcquire( aMiniTrans,
                                 sSegRid.mTbsId,
                                 sSegRid.mPageId,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sSegPage,
                                 aEnv ) == STL_SUCCESS );

            /**
             * Aging Flag를 설정한 이후에 TargetPid를 구했기 때문에
             */
            STL_DASSERT( ((smnIndexHeader*)((stlChar*)smpGetBody(SMP_FRAME(&sSegPage)) +
                                            smsGetHeaderSize(SME_GET_SEGMENT_HANDLE(aRelationHandle))))->mFirstEmptyNode ==
                         sTargetPid );

            STL_TRY( smnmpbRemoveEmptyNode( aRelationHandle,
                                            aMiniTrans,
                                            &sSegPage,
                                            &sTargetPage,
                                            aEnv ) == STL_SUCCESS );

            sTmpPid = SMP_NULL_PID;
            SMNMPB_SET_NEXT_FREE_PAGE(&sTargetPage, sTmpPid);

            STL_TRY( smxmWriteLog( aMiniTrans,
                                   SMG_COMPONENT_INDEX,
                                   SMR_LOG_MEMORY_BTREE_SET_NEXT_FREE_PAGE,
                                   SMR_REDO_TARGET_PAGE,
                                   (stlChar*)&sTmpPid,
                                   STL_SIZEOF(smlPid),
                                   sTargetPage.mPch->mTbsId,
                                   sTargetPage.mPch->mPageId,
                                   0,
                                   SMXM_LOGGING_REDO,
                                   aEnv ) == STL_SUCCESS );

            sState = 1;
            STL_THROW( RAMP_SKIP_REMOVING );
        }

        STL_TRY( smnmpbIsKeyAgable( SMP_FRAME(&sTargetPage),
                                    sKeySlot,
                                    sAgableScn,
                                    &sIsAgable,
                                    aEnv) == STL_SUCCESS );
        if( sIsAgable != STL_TRUE )
        {
            /* 아직은 때가 아니다. 나중에 하자 */
            STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                              &sSavepoint, 
                                              aEnv ) == STL_SUCCESS );
            *aSkipRemove = STL_TRUE;
            STL_THROW( RAMP_SKIP_REMOVING );
        }

        SMP_GET_NEXT_ROWITEM( &sIterator, sKeySlot );
    }

    /* 모든 key가 dead key 상태이다 */
    /* 상위 노드에서 target page를 가리키는 internal key를 제거한다 */
    STL_TRY( smnmpbRemoveIntlKey( aRelationHandle,
                                  aMiniTrans,
                                  sTargetPid,
                                  &sPathStack,
                                  sPathStack.mDepth,
                                  &sSegPage,
                                  &sNeedRetry,
                                  &sSkipRemove,
                                  &sRegisterNextLeaf,
                                  aEnv ) == STL_SUCCESS );

    if( sNeedRetry != SMNMPB_RETRY_NONE )
    {
        STL_DASSERT( sNeedRetry == SMNMPB_RETRY_CONCURRENCY );
        
        STL_TRY( smxmRollbackToSavepoint( aMiniTrans,
                                          &sSavepoint, 
                                          aEnv ) == STL_SUCCESS );
        
        STL_THROW( RAMP_RETRY_REMOVE_FROM_ROOT );
    }
    
    if( sSkipRemove == STL_FALSE )
    {
        STL_TRY( smnmpbRemoveSiblingLink( aMiniTrans,
                                          sPrevPid == SMP_NULL_PID ? NULL : &sPrevPage,
                                          &sTargetPage,
                                          sNextPid == SMP_NULL_PID ? NULL : &sNextPage,
                                          sPathStack.mStack[sPathStack.mDepth - 1].mSmoNo,
                                          aEnv ) == STL_SUCCESS );
    }

    STL_TRY( smnmpbRemoveEmptyNode( aRelationHandle,
                                    aMiniTrans,
                                    &sSegPage,
                                    &sTargetPage,
                                    aEnv ) == STL_SUCCESS );
    if( sSkipRemove == STL_FALSE )
    {
        /* node를 free한다 */
        STL_TRY( smsUpdatePageStatus( aMiniTrans,
                                      SME_GET_SEGMENT_HANDLE(aRelationHandle),
                                      &sTargetPage,
                                      SMP_FREENESS_FREE,
                                      smxlGetSystemScn(),
                                      aEnv ) == STL_SUCCESS );
    }
    else
    {
        /**
         * last Key는 empty node list에서 link만 제거한다(원복됨).
         */
        sTmpPid = SMP_NULL_PID;
        SMNMPB_SET_NEXT_FREE_PAGE(&sTargetPage, sTmpPid);

        STL_TRY( smxmWriteLog( aMiniTrans,
                               SMG_COMPONENT_INDEX,
                               SMR_LOG_MEMORY_BTREE_SET_NEXT_FREE_PAGE,
                               SMR_REDO_TARGET_PAGE,
                               (stlChar*)&sTmpPid,
                               STL_SIZEOF(smlPid),
                               sTargetPage.mPch->mTbsId,
                               sTargetPage.mPch->mPageId,
                               0,
                               SMXM_LOGGING_REDO,
                               aEnv ) == STL_SUCCESS );
    }

    if( (sRegisterNextLeaf == STL_TRUE ) &&
        (sNextPid != SMP_NULL_PID) )
    {
        if( (SMNMPB_IS_LAST_SIBLING( &sNextPage ) != STL_TRUE) &&
            (sNextLeafKeyCount == 0) &&
            (SMNMPB_IS_ON_EMPTY_LIST( &sNextPage ) != STL_TRUE))
        {
            *aNextPid = sNextPid;
        }
    }

    STL_RAMP( RAMP_SKIP_REMOVING );

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
    return STL_SUCCESS;
   
    STL_FINISH;

    switch( sState )
    {
        case 1:
            {
                (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                                &sMemMark,
                                                STL_FALSE, /* aFreeChunk */
                                                (knlEnv*)aEnv );
            }
    }


    if( sFixPage == STL_TRUE )
    {
        (void)smpRelease( &sTargetPage, aEnv );
    }

    return STL_FAILURE;
}



/**
 * @brief redo시에 대상이 되는 key를 찾는다
 * @param[in] aPageHandle key를 찾을 페이지 handle
 * @param[in] aColPtr 찾는 Key의 column value
 * @param[in] aRowRid 찾을 Key가 가리키는 Row의 RID
 * @param[out] aSlotSeq 찾은 key의 위치
 * @param[out] aFoundKey 찾은 key와 동일한 key가 존재하는지 여부
 * @param[in,out] aEnv Storage Manager Environment
 */
stlStatus smnmpbFindTargetKeyForRedo( smpHandle * aPageHandle,
                                      stlChar   * aColPtr,
                                      smlRid    * aRowRid,
                                      stlInt16  * aSlotSeq,
                                      stlBool   * aFoundKey,
                                      smlEnv    * aEnv )
{
    stlInt16            sLevel;
    knlValueBlockList * sValCols = NULL;
    knlValueBlockList * sCurCol;
    knlValueBlockList * sPrevCol;
    stlChar             sValColBuf[SMP_PAGE_SIZE];
    stlUInt16           sKeyColCount;
    stlUInt8          * sKeyColTypes;
    stlUInt8          * sKeyColFlags;
    stlInt32            i;
    smlPid              sChildPid;
    knlRegionMark       sMemMark;
    stlInt32            sState = 0;

    sLevel = SMNMPB_GET_LEVEL(aPageHandle);
    sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(aPageHandle);
    sKeyColTypes = SMNMPB_GET_KEY_COLUMN_TYPES(aPageHandle);
    sKeyColFlags = SMNMPB_GET_KEY_COLUMN_FLAGS(aPageHandle);

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    sPrevCol = NULL;
    for( i = 0; i < sKeyColCount; i++ )
    {
        STL_TRY( knlInitBlockNoBuffer( &sCurCol,
                                       STL_LAYER_STORAGE_MANAGER,
                                       1,
                                       sKeyColTypes[i],
                                       &aEnv->mOperationHeapMem,
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
        if( i == 0 )
        {
            sValCols = sCurCol;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevCol, sCurCol );
        }
        sPrevCol = sCurCol;
    }

    /* Logical 하게 target key를 찾는다 */
    smnmpbMakeValueListFromSlot( aColPtr, sValCols, sValColBuf );
    if( sLevel == 0 )
    {
        STL_TRY( smnmpbFindPosition( sValCols,
                                     0,
                                     aRowRid,
                                     aPageHandle,
                                     aSlotSeq,
                                     aFoundKey,
                                     aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smnmpbFindChildNode( aPageHandle,
                                      sValCols,
                                      0,
                                      aRowRid,
                                      sKeyColFlags,
                                      &sChildPid,
                                      aSlotSeq,
                                      aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       (knlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                        &sMemMark,
                                        STL_FALSE, /* aFreeChunk */
                                        (knlEnv*)aEnv );
    }

    return STL_FAILURE;
}

/**
 * @brief 주어진 Slot의 Slot Body 포인터를 얻는다.
 * @param[in] aSlot Slot의 첫번째 포인터
 * @return Slot의 Body 포인터
 */
inline void * smnmpbGetSlotBody( void * aSlot )
{
    stlUInt16 sSlotHdrSize;

    SMNMPB_GET_LEAF_KEY_HDR_SIZE( aSlot, &sSlotHdrSize );
    return (((stlChar*)aSlot) + sSlotHdrSize);
}

void smnmpbTestPrintLogicalHdr( smpHandle * aPageHandle )
{
    smnmpbLogicalHdr * sHeader = SMNMPB_GET_LOGICAL_HEADER(aPageHandle);

    stlPrintf( " mPrevPageId(%d) mNextPageId(%d) mNextFreePageId(%d)"
               " mLevel(%d) mActiveKeyCount mKeyColCount(%d) SmoNo(%d)\n",
               sHeader->mPrevPageId, sHeader->mNextPageId, sHeader->mNextFreePageId,
               sHeader->mLevel, sHeader->mLevel, sHeader->mKeyColCount,
               SMN_GET_PAGE_SMO_NO(aPageHandle) );
}

void smnmpbValidateKeySpace( smpHandle * aPageHandle )
{
    stlChar            * sTargetKey;
    stlChar            * sCurKey;
    smpRowItemIterator   sTargetIterator;
    smpRowItemIterator   sCurIterator;
    stlInt16             sKeySize;
    stlInt16             sUsedSize = 0;
    stlInt16             sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(aPageHandle);
    stlInt16             sKeySeq = 0;
    stlInt16             sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(aPageHandle);

    SMP_GET_FIRST_ROWITEM( SMP_FRAME(aPageHandle), &sTargetIterator, sTargetKey );

    while( sTargetKey != NULL )
    {
        if( SMNMPB_GET_LEVEL(aPageHandle) == 0 )
        {
            sKeySize = smnmpbGetLeafKeySize(sTargetKey, sKeyColCount);

            if( SMNMPB_IS_DELETED( sTargetKey ) != STL_TRUE )
            {
                sUsedSize += ( sKeySize + STL_SIZEOF(smpOffsetSlot) );
            }
        }
        else
        {
            if( (SMNMPB_IS_LAST_SIBLING( aPageHandle ) == STL_TRUE) &&
                (sKeySeq == sTotalKeyCount - 1) )
            {
                sKeySize = STL_SIZEOF(smlPid);
            }
            else
            {
                sKeySize = smnmpbGetIntlKeySize(sTargetKey, sKeyColCount);
            }

            sUsedSize += ( sKeySize + STL_SIZEOF(smpOffsetSlot) );
        }

        STL_ASSERT( (sTargetKey + sKeySize
                     <= (stlChar*)SMP_FRAME(aPageHandle) + SMP_PAGE_SIZE - STL_SIZEOF(smpPhyTail)) );

        SMP_GET_FIRST_ROWITEM( SMP_FRAME(aPageHandle), &sCurIterator, sCurKey );
        while( sCurKey != NULL )
        {
            if( sCurKey != sTargetKey )
            {
                STL_ASSERT( (sCurKey < sTargetKey) ||
                            (sCurKey >= sTargetKey + sKeySize) );
            }
            SMP_GET_NEXT_ROWITEM( &sCurIterator, sCurKey );
        }
        SMP_GET_NEXT_ROWITEM( &sTargetIterator, sTargetKey );
        sKeySeq++;
    }

    STL_DASSERT( ( sUsedSize +
                   smpGetReclaimedSpace(SMP_FRAME(aPageHandle)) +
                   smpGetUnusedSpace(SMP_FRAME(aPageHandle)) ) ==
                 ( (SMP_FRAME(aPageHandle) + SMP_PAGE_SIZE)  -
                   (void*)SMP_GET_OFFSET_TABLE_HEADER(SMP_FRAME(aPageHandle)) -
                   STL_SIZEOF(smpOffsetTableHeader) -
                   STL_SIZEOF(smpPhyTail) ) );
}

stlInt16 smnmpbCountRtsRef( stlChar  * aPageFrame,
                            stlChar  * aKey,
                            stlUInt8   aRtsSeq,
                            smlEnv   * aEnv )
{
    stlUInt8   sRtsSeq;
    stlUInt8   sRtsVerNo;
    stlInt16   sReferenceCount = 0;
    smpRts   * sRts = SMP_GET_NTH_RTS(aPageFrame, aRtsSeq);

    SMNMPB_GET_INS_RTS_INFO(aKey, &sRtsSeq, &sRtsVerNo);
    if( aRtsSeq == sRtsSeq )
    {
        if( sRtsVerNo == sRts->mPrevVerCnt )
        {
            sReferenceCount++;
        }
        KNL_ASSERT( sRts->mPrevVerCnt >= sRtsVerNo,
                    aEnv,
                    ("Offset(%d)aRtsSeq(%d), PrevVerCnt(%d), sRtsVerNo(%d)",
                     aKey - aPageFrame, aRtsSeq, sRts->mPrevVerCnt, sRtsVerNo) );

    }
    if( SMNMPB_IS_DELETED(aKey) == STL_TRUE )
    {
        SMNMPB_GET_DEL_RTS_INFO(aKey, &sRtsSeq, &sRtsVerNo);
        if( aRtsSeq == sRtsSeq )
        {
            if( sRtsVerNo == sRts->mPrevVerCnt )
            {
                sReferenceCount++;
            }
            KNL_ASSERT( sRts->mPrevVerCnt >= sRtsVerNo,
                        aEnv,
                        ("aRtsSeq(%d), PrevVerCnt(%d), sRtsVerNo(%d)",
                         aRtsSeq, sRts->mPrevVerCnt, sRtsVerNo) );
        }
    }

    return sReferenceCount;
}


stlStatus smnmpbIsAgable( smpHandle * aPageHandle,
                          stlBool   * aIsAgable,
                          smlEnv    * aEnv )
{
    smlScn  sPageScn;
    
    *aIsAgable = STL_FALSE;
    
    sPageScn = smpGetAgableScn(aPageHandle);
    
    /**
     * 페이지를 삭제할 당시 살아 있던 트랜잭션들이 모두 종료되었을때 재사용한다.
     */
    
    if( SMXL_IS_AGABLE_VIEW_SCN( smxlGetAgableStmtScn(aEnv), sPageScn ) == STL_TRUE )
    {
        *aIsAgable = STL_TRUE;
    }

    return STL_SUCCESS;
}


/**
 * @brief Index Page가 유효한지 검증힌다.
 * @param[in] aPageHandle Page Handle
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smnmpbValidateIndexPage( smpHandle * aPageHandle,
                                   smlEnv    * aEnv )
{
#ifdef STL_VALIDATE_DATABASE
    smpPhyHdr * sPhyHdr;
    smnmpbLogicalHdr * sLogicalHdr;
    stlInt16           sTotalKeyCount;
    smpOffsetSlot    * sOffsetSlot;
    stlInt16           i;
    stlChar          * sFrame;

    sFrame = SMP_FRAME( aPageHandle );
    
    sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT( aPageHandle );
    sPhyHdr = (smpPhyHdr*)sFrame;
    
    KNL_ASSERT( sPhyHdr->mPageType == SMP_PAGE_TYPE_INDEX_DATA,
                aEnv,
                SMP_FORMAT_PHYSICAL_HDR( sPhyHdr ) );

    STL_TRY( smpValidatePage( aPageHandle, aEnv ) == STL_SUCCESS );

    sLogicalHdr = (smnmpbLogicalHdr*)(sFrame + STL_SIZEOF(smpPhyHdr) + STL_SIZEOF(smpOrderedSlotSignpost));
    if( sLogicalHdr->mLevel == 0 )
    {
        smpCheckRtsRefCount( sFrame, smnmpbCountRtsRef, aEnv );
    }

    sOffsetSlot = smpGetFirstOffsetSlot( aPageHandle );
    for( i = 0; i < sTotalKeyCount; i++ )
    {
        STL_ASSERT( *sOffsetSlot != 0 );
        sOffsetSlot++;
    }

    smnmpbValidateKeySpace( aPageHandle );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
#else
    return STL_SUCCESS;
#endif
}





/** @} */
