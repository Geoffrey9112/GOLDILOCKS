/*******************************************************************************
 * smnRedo.c
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
#include <smr.h>
#include <smp.h>
#include <smn.h>
#include <smnmpb.h>
#include <smnDef.h>
#include <smnRedo.h>
#include <smnMtxUndo.h>

/**
 * @file smnRedo.c
 * @brief Storage Manager Index Redo Internal Routines
 */

/**
 * @addtogroup smnRedo
 * @{
 */


smrRedoVector gSmnRedoVectors[SMR_INDEX_LOG_MAX_COUNT] =
{
    { /* SMR_LOG_MEMORY_BTREE_INSERT_LEAF */
        smnRedoMemoryBtreeInsertLeaf,
        NULL,
        "MEMORY_BTREE_INSERT_LEAF"
    },
    { /* SMR_LOG_MEMORY_BTREE_DUPKEY */
        smnRedoMemoryBtreeDupKey,
        NULL,
        "MEMORY_BTREE_DUPKEY"
    },
    { /* SMR_LOG_MEMORY_BTREE_INSERT_INTL */
        smnRedoMemoryBtreeInsertIntl,
        NULL,
        "MEMORY_BTREE_INSERT_INTL"
    },
    { /* SMR_LOG_MEMORY_BTREE_DELETE_LEAF */
        smnRedoMemoryBtreeDeleteLeaf,
        NULL,
        "MEMORY_BTREE_DELETE_LEAF"
    },
    { /* SMR_LOG_MEMORY_BTREE_UNDELETE_LEAF */
        smnRedoMemoryBtreeUndeleteLeaf,
        NULL,
        "MEMORY_BTREE_UNDELETE_LEAF"
    },
    { /* SMR_LOG_MEMORY_BTREE_DELETE_INTL */
        smnRedoMemoryBtreeDeleteIntl,
        NULL,
        "MEMORY_BTREE_DELETE_INTL"
    },
    { /* SMR_LOG_SET_ROOT_PAGE_ID */
        smnRedoSetRootPid,
        NULL,
        "SET_ROOT_PAGE_ID"
    },
    { /* SMR_LOG_MEMORY_BTREE_NEW_ROOT */
        smnRedoMemoryBtreeNewRoot,
        NULL,
        "MEMORY_BTREE_NEW_ROOT"
    },
    { /* SMR_LOG_MEMORY_BTREE_SPLIT_NEW_PAGE */
        smnRedoMemoryBtreeSplitNewPage,
        NULL,
        "MEMORY_BTREE_SPLIT_NEW_PAGE"
    },
    { /* SMR_LOG_MEMORY_BTREE_SPLIT_OLD_PAGE */
        smnRedoMemoryBtreeSplitOldPage,
        NULL,
        "MEMORY_BTREE_SPLIT_OLD_PAGE"
    },
    { /* SMR_LOG_MEMORY_BTREE_SET_PREV_PAGE_ID */
        smnRedoMemoryBtreeSetPrevPageId,
        NULL,
        "MEMORY_BTREE_SET_PREV_PAGE_ID"
    },
    { /* SMR_LOG_MEMORY_BTREE_SET_NEXT_PAGE_ID */
        smnRedoMemoryBtreeSetNextPageId,
        NULL,
        "MEMORY_BTREE_SET_NEXT_PAGE_ID"
    },
    { /* SMR_LOG_MEMORY_BTREE_SET_CHILD_PID */
        smnRedoMemoryBtreeSetChildPid,
        NULL,
        "MEMORY_BTREE_SET_CHILD_PID"
    },
    { /* SMR_LOG_MEMORY_BTREE_SET_NEXT_FREE_PAGE */
        smnRedoMemoryBtreeSetNextFreePage,
        NULL,
        "MEMORY_BTREE_SET_NEXT_FREE_PAGE"
    },
    { /* SMR_LOG_MEMORY_BTREE_AGING_COMPACT */
        smnRedoMemoryBtreeAgingCompact,
        NULL,
        "MEMORY_BTREE_AGING_COMPACT"
    },
    { /* SMR_LOG_ADD_EMPTY_NODE */
        smnRedoAddEmptyNode,
        NULL,
        "ADD_EMPTY_NODE"
    },
    { /* SMR_LOG_REMOVE_EMPTY_NODE */
        smnRedoRemoveEmptyNode,
        NULL,
        "REMOVE_EMPTY_NODE"
    },
    { /* SMR_LOG_MEMORY_BTREE_PAGE_IMAGE */
        smnRedoPageImage,
        NULL,
        "MEMORY_BTREE_PAGE_IMAGE"
    },
    { /* SMR_LOG_MEMORY_BTREE_SET_NODE_FLAG */
        smnRedoMemoryBtreeSetNodeFlag,
        NULL,
        "MEMORY_BTREE_SET_NODE_FLAG"
    },
    { /* SMR_LOG_ALTER_DATATYPE */
        smnRedoAlterDatatype,
        NULL,
        "ALTER_DATATYPE"
    },
    { /* SMR_LOG_SET_MIRROR_ROOT_PAGE_ID */
        smnRedoSetMirrorRootPid,
        NULL,
        "SET_MIRROR_ROOT_PAGE_ID"
    }
};

stlStatus smnRedoMemoryBtreeInsertLeaf( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv )
{
    stlChar          * sPtr = (stlChar*)aData;
    smxlTransId        sTransId;
    smlScn             sViewScn;
    stlChar          * sColPtr;
    stlChar          * sKeySlot;
    stlInt16           sSlotSeq;
    stlBool            sFoundKey;
    stlInt16           sTotalKeySize = 0;
    stlUInt16          sTotalKeyCount;
    stlInt16           sRetryPhase = 0;
    stlUInt8           sNewRtsSeq = SMP_RTS_SEQ_NULL;
    smlRid             sRowRid;
    stlBool            sIsFreed;
    smxlTransId        sWaitTransId;
    stlBool            sIsCommitted;
    stlInt16           sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(aPageHandle);
    stlUInt8           sInsertRtsSeq;
    stlUInt8           sInsertRtsVerNo;
    stlUInt8           sNewInsertRtsVerNo;
    stlUInt8           sDeleteRtsSeq;
    stlUInt8           sDeleteRtsVerNo;
    stlUInt16          sSlotHdrSize;

    STL_WRITE_INT64( &sTransId, sPtr );
    sPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sViewScn, sPtr );
    sPtr += STL_SIZEOF(smlScn);

    /* Leaf Node여야 함 */
    SMN_TRY_INTERNAL_ERROR( SMNMPB_GET_LEVEL(aPageHandle) == 0,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );
                            
    sTotalKeyCount = SMP_GET_PAGE_ROW_ITEM_COUNT(aPageHandle);

    SMNMPB_GET_LEAF_KEY_HDR_SIZE(sPtr, &sSlotHdrSize);
    sColPtr = sPtr + sSlotHdrSize;
    sRowRid.mTbsId = 0; /* 사용 안함 */
    SMNMPB_GET_ROW_PID( sPtr, &sRowRid.mPageId );
    SMNMPB_GET_ROW_SEQ( sPtr, &sRowRid.mOffset );

    STL_TRY( smnmpbFindTargetKeyForRedo( aPageHandle,
                                         sColPtr,
                                         &sRowRid,
                                         &sSlotSeq,
                                         &sFoundKey,
                                         aEnv ) == STL_SUCCESS );

    SMN_TRY_INTERNAL_ERROR( sSlotSeq <= sTotalKeyCount,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    if( sFoundKey == STL_TRUE )
    {
        /* runtime과 redo time의 compact 시점의 차이와 agable scn의 차이로 key가 이미 존재할 수도 있다 */
        sKeySlot = smpGetNthRowItem( aPageHandle, sSlotSeq );
        STL_TRY( smnmpbIsDeleteCommitted( SMP_FRAME(aPageHandle),
                                          sKeySlot,
                                          &sIsCommitted,
                                          &sWaitTransId,
                                          aEnv ) == STL_SUCCESS );

        SMN_TRY_INTERNAL_ERROR( (SMNMPB_IS_DELETED( sKeySlot ) == STL_TRUE) &&
                                (sIsCommitted == STL_TRUE),
                                aEnv,
                                ( aPageHandle, aEnv, NULL ) );
        
        SMNMPB_GET_INS_RTS_INFO( sKeySlot, &sInsertRtsSeq, &sInsertRtsVerNo );
        SMNMPB_GET_DEL_RTS_INFO( sKeySlot, &sDeleteRtsSeq, &sDeleteRtsVerNo );

        if( SMP_IS_VALID_RTS_SEQ(sInsertRtsSeq) == STL_TRUE )
        {
            STL_TRY( smpEndUseRts( aPageHandle,
                                   sKeySlot,
                                   sInsertRtsSeq,
                                   sInsertRtsVerNo,
                                   aEnv ) == STL_SUCCESS );
        }

        if( SMP_IS_VALID_RTS_SEQ(sDeleteRtsSeq) == STL_TRUE )
        {
            STL_TRY( smpEndUseRts( aPageHandle,
                                   sKeySlot,
                                   sDeleteRtsSeq,
                                   sDeleteRtsVerNo,
                                   aEnv ) == STL_SUCCESS );
        }

        STL_TRY( smpFreeNthSlot( aPageHandle,
                                 sSlotSeq,
                                 smnmpbGetLeafKeySize(sKeySlot, sKeyColCount) ) == STL_SUCCESS );
        SMP_REMOVE_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), STL_SIZEOF(smpOffsetSlot) );
        
        sTotalKeyCount--;
    }

    SMNMPB_GET_INS_RTS_INFO( sPtr, &sInsertRtsSeq, &sInsertRtsVerNo );
    sTotalKeySize = aDataSize - STL_SIZEOF(smxlTransId) - STL_SIZEOF(smlScn);

    SMN_TRY_INTERNAL_ERROR( sTotalKeySize > sColPtr - sPtr,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );
        
    /* 1. RTS 할당 */
    while( STL_TRUE )
    {
        if( sRetryPhase == 1 )
        {
            /*
             * reclaimed space가 조금이라도 있다면 key를 물리적으로 삭제해야만
             * 공간을 확보할수 있다.
             * 페이지를 aging하지 않는다면 Compact해도 공간을 확보할수 없다.
             */
            if( smpGetReclaimedSpace(SMP_FRAME(aPageHandle)) > 0 )
            {
                STL_TRY( smnmpbDoAgingLeafNode( NULL,
                                                aPageHandle,
                                                &sSlotSeq,
                                                STL_TRUE,
                                                &sIsFreed,
                                                aEnv ) == STL_SUCCESS );
            }
            if( smpGetTotalFreeSpace(SMP_FRAME(aPageHandle)) >= sTotalKeySize + STL_SIZEOF(smpOffsetSlot) )
            {
                STL_TRY( smnmpbCompactPage( NULL,
                                            aPageHandle,
                                            STL_INT16_MAX,
                                            aEnv ) == STL_SUCCESS );
            }
            else
            {
                sRetryPhase++;
                continue;
            }
        }
        else if( sRetryPhase == 2 )
        {
            /* Compact 했는데도 insert하지 못함 */
            SMN_TRY_INTERNAL_ERROR( STL_FALSE,
                                    aEnv,
                                    ( aPageHandle, aEnv, NULL ) );
        }
        else
        {
            SMN_TRY_INTERNAL_ERROR( sRetryPhase == 0,
                                    aEnv,
                                    ( aPageHandle, aEnv, NULL ) );
            /* do nothing */
        }

        if( SMP_IS_VALID_RTS_SEQ( sInsertRtsSeq ) == STL_TRUE )
        {
            STL_TRY( smpAllocRts( NULL,
                                  aPageHandle,
                                  sTransId,
                                  SML_MAKE_VIEW_SCN(sViewScn),
                                  smnmpbStabilizeKey,
                                  &sNewRtsSeq,
                                  aEnv ) == STL_SUCCESS );
            if( SMP_IS_VALID_RTS_SEQ(sNewRtsSeq) != STL_TRUE )
            {
                /* RTS 할당 실패 -> compact 후 재시도 */
                sRetryPhase++;
                continue;
            }
        }
        else
        {
            /* TBK 타입의 Insert 이거나 index building중의 로그임 */
            SMN_TRY_INTERNAL_ERROR( ((SMNMPB_IS_TBK( sPtr ) == STL_TRUE) &&
                                     (sInsertRtsSeq == SMP_RTS_SEQ_NULL)) ||
                                    (sInsertRtsSeq == SMP_RTS_SEQ_STABLE),
                                    aEnv,
                                    ( aPageHandle, aEnv, NULL ) );
        }

        /* 2. Key Slot 할당 Test */
        STL_TRY( smpAllocNthSlot( aPageHandle,
                                  sSlotSeq,
                                  sTotalKeySize,
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
    } /* while */

    /* 실제로 Key slot을 alloc 받음 */
    STL_TRY( smpAllocNthSlot( aPageHandle,
                              sSlotSeq,
                              sTotalKeySize,
                              STL_FALSE, /* aIsForTest */
                              &sKeySlot ) == STL_SUCCESS );

    stlMemcpy( sKeySlot, sPtr, sTotalKeySize );

    if( SMP_IS_VALID_RTS_SEQ( sInsertRtsSeq ) == STL_TRUE )
    {
        /* RTS 사용 시작 */
        STL_TRY( smpBeginUseRts( aPageHandle,
                                 sKeySlot,
                                 sTransId,
                                 SML_MAKE_VIEW_SCN(sViewScn),
                                 sNewRtsSeq,
                                 &sNewInsertRtsVerNo,
                                 aEnv ) == STL_SUCCESS );

        SMNMPB_SET_INS_RTS_INFO( sKeySlot, &sNewRtsSeq, &sNewInsertRtsVerNo );
    }

    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    smpSetMaxViewScn( aPageHandle, sViewScn );

    STL_TRY( smnmpbValidateIndexPage( aPageHandle,
                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smnRedoMemoryBtreeDupKey( smlRid    * aDataRid,
                                    void      * aData,
                                    stlUInt16   aDataSize,
                                    smpHandle * aPageHandle,
                                    smlEnv    * aEnv )
{
    stlChar          * sPtr = (stlChar*)aData;
    smxlTransId        sTransId;
    smlScn             sViewScn;
    stlChar          * sColPtr;
    stlChar          * sKeySlot;
    stlChar          * sOrgKeySlot;
    stlInt16           sSlotSeq;
    stlInt16           sAftSlotSeq;
    stlBool            sFoundKey;
    stlUInt8           sNewRtsSeq = SMP_RTS_SEQ_NULL;
    stlUInt8           sInsertRtsSeq;
    stlUInt8           sDeleteRtsSeq;
    stlUInt8           sInsertRtsVerNo;
    stlUInt8           sDeleteRtsVerNo = 0;
    stlUInt8           sNewInsertRtsVerNo;
    smlRid             sRowRid;
    stlUInt16          sTotalKeySize;
    stlInt16           sRetryPhase = 0;
    stlInt16           sKeyHdrSize;
    stlBool            sIsFreed;
    stlInt16           sBodySize;
    stlUInt16          sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(aPageHandle);
    stlUInt8           sInsTcn = 0;

    /* Leaf Node여야 함 */
    SMN_TRY_INTERNAL_ERROR( SMNMPB_GET_LEVEL(aPageHandle) == 0,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    STL_WRITE_INT64( &sTransId, sPtr );
    sPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sViewScn, sPtr );
    sPtr += STL_SIZEOF(smlScn);

    sTotalKeySize = aDataSize - STL_SIZEOF(smxlTransId) - STL_SIZEOF(smlScn);

    SMNMPB_GET_LEAF_KEY_HDR_SIZE(sPtr, &sKeyHdrSize);
    sColPtr = sPtr + sKeyHdrSize;
    sBodySize = aDataSize - (sColPtr - (char*)aData);
    sTotalKeySize = sKeyHdrSize + sBodySize;
    
    SMN_TRY_INTERNAL_ERROR( sTotalKeySize > SMNMPB_TBK_HDR_SIZE,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );
    
    sRowRid.mTbsId = 0; /* 사용 안함 */ 
    SMNMPB_GET_ROW_PID( sPtr, &sRowRid.mPageId );
    SMNMPB_GET_ROW_SEQ( sPtr, &sRowRid.mOffset );

    STL_TRY( smnmpbFindTargetKeyForRedo( aPageHandle,
                                         sColPtr,
                                         &sRowRid,
                                         &sSlotSeq,
                                         &sFoundKey,
                                         aEnv ) == STL_SUCCESS );

    /* 1. 해당 key 값과 RowRid를 갖는 key가 존재하여야 한다 */
    SMN_TRY_INTERNAL_ERROR( sSlotSeq <= SMP_GET_PAGE_ROW_ITEM_COUNT(aPageHandle),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );
    
    if( sFoundKey != STL_TRUE )
    {
        /* runtime과 redo time의 compact 시점의 차이와 agable scn의 차이로 dup할 key가 없을 수도 있다 */
        sOrgKeySlot = NULL;
    }
    else
    {
        /* 1. 해당 key에 delete bit가 세팅외어 있어야 한다 */
        sOrgKeySlot = smpGetNthRowItem( aPageHandle, sSlotSeq );
        
        SMN_TRY_INTERNAL_ERROR( SMNMPB_IS_DELETED( sOrgKeySlot ) == STL_TRUE,
                                aEnv,
                                ( aPageHandle, aEnv, NULL ) );
    }

    SMNMPB_GET_INS_RTS_INFO( sPtr, &sInsertRtsSeq, &sInsertRtsVerNo );

    while( STL_TRUE )
    {
        if( sRetryPhase == 1 )
        {
            /*
             * reclaimed space가 조금이라도 있다면 key를 물리적으로 삭제해야만
             * 공간을 확보할수 있다.
             * 페이지를 aging하지 않는다면 Compact해도 공간을 확보할수 없다.
             */
            if( smpGetReclaimedSpace(SMP_FRAME(aPageHandle)) > 0 )
            {
                sAftSlotSeq = sSlotSeq;
                STL_TRY( smnmpbDoAgingLeafNode( NULL,
                                                aPageHandle,
                                                &sAftSlotSeq,
                                                STL_TRUE,
                                                &sIsFreed,
                                                aEnv ) == STL_SUCCESS );
                if( sSlotSeq != sAftSlotSeq )
                {
                    /* aging 연산으로 위치가 변경되었음 */
                    sSlotSeq = sAftSlotSeq;
                }
                if( sIsFreed == STL_TRUE )
                {
                    /* Aging 도중에 target key가 free되었음. 일반적으로는 SK 타입으로 */
                    /* insert해야되지만, REDO는 똑같이 재연해야 하므로 DK 타입으로    */
                    /* 생성하도록 함                                                  */
                    sOrgKeySlot = NULL;
                }
                else
                {
                    if( sOrgKeySlot != NULL )
                    {
                        /* 기존 키가 존재 했으면 */
                        sOrgKeySlot = smpGetNthRowItem(aPageHandle, sSlotSeq);
                    }
                }
            }
            if( (smpGetTotalFreeSpace(SMP_FRAME(aPageHandle)) >= sTotalKeySize + STL_SIZEOF(smpOffsetSlot)) )
            {
                STL_TRY( smnmpbCompactPage( NULL, /* aRelationHandle */
                                            aPageHandle,
                                            STL_INT16_MAX,
                                            aEnv ) == STL_SUCCESS );
                if( sOrgKeySlot != NULL )
                {
                    /* 기존 키가 존재 했으면 */
                    sOrgKeySlot = smpGetNthRowItem(aPageHandle, sSlotSeq);
                }
            }
            else
            {
                sRetryPhase++;
                continue;
            }
        }
        else if( sRetryPhase == 2 )
        {
            /* Compact 했는데도 insert하지 못함 */
            SMN_TRY_INTERNAL_ERROR( STL_FALSE,
                                    aEnv,
                                    ( aPageHandle, aEnv, NULL ) );
        }
        else
        {
            /* do nothing */
        }

        if( SMP_IS_VALID_RTS_SEQ(sInsertRtsSeq) == STL_TRUE )
        {
            STL_TRY( smpAllocRts( NULL,
                                  aPageHandle,
                                  sTransId,
                                  SML_MAKE_VIEW_SCN(sViewScn),
                                  smnmpbStabilizeKey,
                                  &sNewRtsSeq,
                                  aEnv ) == STL_SUCCESS );

            if( SMP_IS_VALID_RTS_SEQ(sNewRtsSeq) != STL_TRUE )
            {
                /* RTS 할당 실패 -> compact 후 재시도 */
                sRetryPhase++;
                continue;
            }
        }

        if( sOrgKeySlot == NULL )
        {
            /* dup key 대상이 free되었음 */
            /* Key Slot 할당 */
            STL_TRY( smpAllocNthSlot( aPageHandle,
                                      sSlotSeq,
                                      sTotalKeySize,
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
            if( SMNMPB_IS_TBK( sOrgKeySlot ) != STL_TRUE )
            {
                /* Key Slot 할당 */
                STL_TRY( smpReallocNthSlot( aPageHandle,
                                            sSlotSeq,
                                            sTotalKeySize,
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
                /* Dupkey이면 */
                sKeySlot = sOrgKeySlot;
                break;
            }
        }
    } /* while */

    if( sOrgKeySlot != NULL )
    {
        SMN_TRY_INTERNAL_ERROR( sOrgKeySlot == smpGetNthRowItem(aPageHandle, sSlotSeq),
                                aEnv,
                                ( aPageHandle, aEnv, NULL ) );
        
        SMNMPB_GET_INS_RTS_INFO( sOrgKeySlot, &sInsertRtsSeq, &sInsertRtsVerNo );
        SMNMPB_GET_DEL_RTS_INFO( sOrgKeySlot, &sDeleteRtsSeq, &sDeleteRtsVerNo );
    }

    if( SMP_IS_VALID_RTS_SEQ(sNewRtsSeq) == STL_TRUE )
    {
        STL_TRY( smpBeginUseRts( aPageHandle,
                                 sKeySlot,
                                 sTransId,
                                 SML_MAKE_VIEW_SCN(sViewScn),
                                 sNewRtsSeq,
                                 &sNewInsertRtsVerNo,
                                 aEnv ) == STL_SUCCESS );
    }

    if( sOrgKeySlot == NULL )
    {
        /* Key Slot 할당 */
        STL_TRY( smpAllocNthSlot( aPageHandle,
                                  sSlotSeq,
                                  sTotalKeySize,
                                  STL_FALSE, /* aIsForTest */
                                  &sKeySlot ) == STL_SUCCESS );

        SMN_TRY_INTERNAL_ERROR( sKeySlot != NULL,
                                aEnv,
                                ( aPageHandle, aEnv, NULL ) );
    }
    else
    {
        /* 기존 RTS들과 관계를 끊는다 */
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

        if( SMNMPB_IS_TBK( sOrgKeySlot ) != STL_TRUE )
        {
            /* Key Slot 할당 */
            STL_TRY( smpReallocNthSlot( aPageHandle,
                                        sSlotSeq,
                                        sTotalKeySize,
                                        STL_FALSE, /* aIsForTest */
                                        &sKeySlot ) == STL_SUCCESS );
            
            SMN_TRY_INTERNAL_ERROR( sKeySlot != NULL,
                                    aEnv,
                                    ( aPageHandle, aEnv, NULL ) );
            
            /**
             * Offset Slot 공간을 재사용했다.
             */
            SMP_REMOVE_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), STL_SIZEOF(smpOffsetSlot) );
        }
        else
        {
            SMP_REMOVE_RECLAIMED_SPACE( SMP_FRAME(aPageHandle),
                                        smnmpbGetLeafKeySize( sKeySlot, sKeyColCount ) 
                                        + STL_SIZEOF(smpOffsetSlot) );
        }
    }

    /* make key */
    smnmpbWriteKeyHeader( sKeySlot,
                          STL_TRUE,
                          STL_TRUE,
                          sNewRtsSeq,
                          sNewInsertRtsVerNo,
                          sTransId,
                          sViewScn,
                          &sRowRid );
    
    SMNMPB_SET_INS_TCN( sKeySlot, &sInsTcn );
    
    stlMemcpy( sKeySlot + SMNMPB_TBK_HDR_SIZE, sColPtr, sBodySize );

    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    smpSetMaxViewScn( aPageHandle, sViewScn );
    
    STL_TRY( smnmpbValidateIndexPage( aPageHandle,
                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smnRedoMemoryBtreeInsertIntl( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv )
{
    smlPid     sOrgChildPid;
    smlRid     sPropRowRid;
    stlChar  * sPtr = (stlChar*)aData;
    stlChar  * sPropKey;
    stlInt16   sPropKeySize;

    /* Leaf Node가 아니어야 함 */
    SMN_TRY_INTERNAL_ERROR( SMNMPB_GET_LEVEL(aPageHandle) > 0,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    /* LOG CONTENTS : org child pid + prop key rowrid.pid + pro key rowrid.offset + prop key body */
    STL_WRITE_INT32( &sOrgChildPid, sPtr);
    sPtr += STL_SIZEOF(smlPid);
    STL_WRITE_INT32( &sPropRowRid.mPageId, sPtr);
    sPtr += STL_SIZEOF(smlPid);
    STL_WRITE_INT16( &sPropRowRid.mOffset, sPtr);
    sPtr += STL_SIZEOF(stlInt16);

    sPropKeySize = aDataSize;
    STL_TRY( smpAllocNthSlot( aPageHandle,
                              aDataRid->mOffset,
                              sPropKeySize,
                              STL_FALSE, /* aIsForTest */
                              &sPropKey ) == STL_SUCCESS );

    /* alloc slot이 실패하면 compact해서 공간 확보가 가능한지 체크 */
    if( sPropKey == NULL )
    {
        /* compact 후 재시도 */
        STL_TRY( smnmpbCompactPage( NULL,
                                    aPageHandle,
                                    STL_INT16_MAX,
                                    aEnv ) == STL_SUCCESS );

        STL_TRY( smpAllocNthSlot( aPageHandle,
                                  aDataRid->mOffset,
                                  sPropKeySize,
                                  STL_FALSE, /* aIsForTest */
                                  &sPropKey ) == STL_SUCCESS );

        /* internal node에서는 signpost의 내용이 정확하므로 compact 후 재 할당할 시 반드시 성공해야함 */
        SMN_TRY_INTERNAL_ERROR( sPropKey != NULL,
                                aEnv,
                                ( aPageHandle, aEnv, NULL ) );
    }

    SMNMPB_INTL_SLOT_SET_CHILD_PID( sPropKey, &sOrgChildPid );
    /* Max Key가 아니여야 함 */
    SMN_TRY_INTERNAL_ERROR( sPropKeySize > STL_SIZEOF(smlPid),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    SMNMPB_INTL_SLOT_SET_ROW_PID( sPropKey, &sPropRowRid.mPageId );
    SMNMPB_INTL_SLOT_SET_ROW_OFFSET( sPropKey, &sPropRowRid.mOffset );
    stlMemcpy( sPropKey + SMNMPB_INTL_SLOT_HDR_SIZE, sPtr, aDataSize - (sPtr - (stlChar*)aData) );
    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smnRedoMemoryBtreeDeleteLeaf( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv )
{
    stlChar          * sPtr = (stlChar*)aData;
    smxlTransId        sTransId;
    smlScn             sViewScn;
    stlChar          * sColPtr;
    stlChar          * sKeySlot;
    stlChar          * sNewKeySlot = NULL;
    stlInt16           sSlotSeq;
    stlBool            sFoundKey;
    stlUInt8           sRtsSeq;
    stlUInt8           sRtsVerNo;
    stlUInt8           sInsertRtsSeq;
    stlUInt8           sInsertRtsVerNo;
    stlUInt8           sNewInsertRtsVerNo;
    stlUInt8           sNewDeleteRtsVerNo;
    stlUInt8           sNewRtsSeq = SMP_RTS_SEQ_NULL;
    smlRid             sRowRid;
    stlInt16           sRetryPhase = 0;
    stlBool            sIsFreed;
    stlUInt16          sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(aPageHandle);
    stlInt16           sNewKeySize = 0;
    stlInt16           sOrgKeySize;
    smpRts           * sSrcInsertRts;
    stlUInt16          sSlotHdrSize;
    smlScn             sTmpScn;
    stlUInt8           sDelTcn = 0;

    /* Leaf Node여야 함 */
    SMN_TRY_INTERNAL_ERROR( SMNMPB_GET_LEVEL(aPageHandle) == 0,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    STL_WRITE_INT64( &sTransId, sPtr );
    sPtr += STL_SIZEOF(smxlTransId);
    STL_WRITE_INT64( &sViewScn, sPtr );
    sPtr += STL_SIZEOF(smlScn);

    SMNMPB_GET_LEAF_KEY_HDR_SIZE(sPtr, &sSlotHdrSize);
    sColPtr = sPtr + sSlotHdrSize;
    sRowRid.mTbsId = 0; /* 사용 안함 */ 
    SMNMPB_GET_ROW_PID( sPtr, &sRowRid.mPageId );
    SMNMPB_GET_ROW_SEQ( sPtr, &sRowRid.mOffset );

    STL_TRY( smnmpbFindTargetKeyForRedo( aPageHandle,
                                         sColPtr,
                                         &sRowRid,
                                         &sSlotSeq,
                                         &sFoundKey,
                                         aEnv ) == STL_SUCCESS );

    /* 1. 해당 key 값과 RowRid를 갖는 key가 존재하여야 한다 */
    SMN_TRY_INTERNAL_ERROR( (sFoundKey == STL_TRUE) &&
                            (sSlotSeq < SMP_GET_PAGE_ROW_ITEM_COUNT(aPageHandle)),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    sKeySlot = smpGetNthRowItem( aPageHandle, sSlotSeq );
    sOrgKeySize = smnmpbGetLeafKeySize( sKeySlot, sKeyColCount );

    sNewKeySize = smnmpbGetLeafKeySize( sPtr, sKeyColCount );
    
    SMN_TRY_INTERNAL_ERROR( (SMNMPB_IS_TBK(sPtr) != STL_TRUE) ||
                            (sNewKeySize > SMNMPB_TBK_HDR_SIZE),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );
    
    SMNMPB_GET_DEL_RTS_INFO( sPtr, &sRtsSeq, &sRtsVerNo );

    while( STL_TRUE )
    {
        if( sRetryPhase == 1 )
        {
            /*
             * reclaimed space가 조금이라도 있다면 key를 물리적으로 삭제해야만
             * 공간을 확보할수 있다.
             * 페이지를 aging하지 않는다면 Compact해도 공간을 확보할수 없다.
             */
            if( smpGetReclaimedSpace(SMP_FRAME(aPageHandle)) > 0 )
            {
                STL_TRY( smnmpbDoAgingLeafNode( NULL,
                                                aPageHandle,
                                                &sSlotSeq,
                                                STL_TRUE,
                                                &sIsFreed,
                                                aEnv ) == STL_SUCCESS );
                
                SMN_TRY_INTERNAL_ERROR( sIsFreed != STL_TRUE,
                                        aEnv,
                                        ( aPageHandle, aEnv, NULL ) );
            }
            if( smpGetTotalFreeSpace(SMP_FRAME(aPageHandle)) >= sNewKeySize - sOrgKeySize )
            {
                STL_TRY( smnmpbCompactPage( NULL, /* aRelationHandle */
                                            aPageHandle,
                                            STL_INT16_MAX,
                                            aEnv ) == STL_SUCCESS );
                sKeySlot = smpGetNthRowItem(aPageHandle, sSlotSeq);
            }
            else
            {
                sRetryPhase++;
                continue;
            }
        }
        else if( sRetryPhase == 2 )
        {
            /* Compact 했는데도 insert하지 못함 */
            SMN_TRY_INTERNAL_ERROR( STL_FALSE,
                                    aEnv,
                                    ( aPageHandle, aEnv, NULL ) );
        }
        else
        {
            /* do nothing */
        }

        if( SMP_IS_VALID_RTS_SEQ( sRtsSeq ) == STL_TRUE )
        {
            /* RTS 할당 받음 */
            STL_TRY( smpAllocRts( NULL,
                                  aPageHandle,
                                  sTransId,
                                  SML_MAKE_VIEW_SCN(sViewScn),
                                  smnmpbStabilizeKey,
                                  &sNewRtsSeq,
                                  aEnv ) == STL_SUCCESS );

            if( SMP_IS_VALID_RTS_SEQ(sNewRtsSeq) != STL_TRUE )
            {
                /* After image의 key가 Rts bound면 꼭 rts를 할당받도록 한다 */
                sRetryPhase++;
                continue;
            }
        }

        if( sNewKeySize > sOrgKeySize )
        {
            STL_TRY( smpReallocNthSlot( aPageHandle,
                                        sSlotSeq,
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
        /* 모든 과정 성공 */
        break;
    } /* while */

    if( sNewKeySlot != NULL )
    {
        STL_TRY( smpReallocNthSlot( aPageHandle,
                                    sSlotSeq,
                                    sNewKeySize,
                                    STL_FALSE,
                                    &sNewKeySlot ) == STL_SUCCESS );

        if( SMP_IS_VALID_RTS_SEQ(sNewRtsSeq) == STL_TRUE )
        {
            STL_TRY( smpBeginUseRts( aPageHandle,
                                     sNewKeySlot,
                                     sTransId,
                                     SML_MAKE_VIEW_SCN(sViewScn),
                                     sNewRtsSeq,
                                     &sNewDeleteRtsVerNo,
                                     aEnv ) == STL_SUCCESS );
        }

        SMNMPB_GET_INS_RTS_INFO( sKeySlot, &sInsertRtsSeq, &sInsertRtsVerNo );
        if( SMP_IS_VALID_RTS_SEQ(sInsertRtsSeq) == STL_TRUE )
        {
            /* RTS의 Offset 변경 */
            sSrcInsertRts = SMP_GET_NTH_RTS(SMP_FRAME(aPageHandle), sInsertRtsSeq);
            if( sInsertRtsVerNo == sSrcInsertRts->mPrevVerCnt )
            {
                STL_TRY( smpBeginUseRts( aPageHandle,
                                         sNewKeySlot,
                                         sSrcInsertRts->mTransId,
                                         sSrcInsertRts->mScn,
                                         sInsertRtsSeq,
                                         &sNewInsertRtsVerNo,
                                         aEnv ) == STL_SUCCESS );
                
                SMN_TRY_INTERNAL_ERROR( sNewInsertRtsVerNo == sInsertRtsVerNo,
                                        aEnv,
                                        ( aPageHandle, aEnv, NULL ) );

                STL_TRY( smpEndUseRts( aPageHandle,
                                       sKeySlot,
                                       sInsertRtsSeq,
                                       sInsertRtsVerNo,
                                       aEnv ) == STL_SUCCESS );
            }
            else
            {
                /* do nothing */
            }
        }

        stlMemcpy( sNewKeySlot, sKeySlot, SMNMPB_RBK_HDR_SIZE );
        stlMemcpy( sNewKeySlot + SMNMPB_TBK_HDR_SIZE,
                   sKeySlot + SMNMPB_RBK_HDR_SIZE,
                   sOrgKeySize - SMNMPB_RBK_HDR_SIZE );
        SMNMPB_SET_DELETED( sNewKeySlot );
        SMNMPB_SET_DEL_TCN( sNewKeySlot, &sDelTcn );
        SMNMPB_SET_DEL_RTS_INFO( sNewKeySlot, &sNewRtsSeq, &sNewDeleteRtsVerNo );
        SMNMPB_SET_TBK( sNewKeySlot );
        SMNMPB_SET_TBK_DEL_TRANS_ID( sNewKeySlot, &sTransId );
        sTmpScn = SML_MAKE_VIEW_SCN(sViewScn);
        SMNMPB_SET_TBK_DEL_SCN( sNewKeySlot, &sTmpScn );
        SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sOrgKeySize );
        SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sNewKeySize + STL_SIZEOF(smpOffsetSlot));
        sOrgKeySize = sNewKeySize;
        sKeySlot = sNewKeySlot;
    }
    else
    {
        if( SMP_IS_VALID_RTS_SEQ(sNewRtsSeq) == STL_TRUE )
        {
            STL_TRY( smpBeginUseRts( aPageHandle,
                                     sKeySlot,
                                     sTransId,
                                     SML_MAKE_VIEW_SCN(sViewScn),
                                     sNewRtsSeq,
                                     &sNewDeleteRtsVerNo,
                                     aEnv ) == STL_SUCCESS );
        }

        SMNMPB_SET_DELETED( sKeySlot );
        SMNMPB_SET_DEL_TCN( sKeySlot, &sDelTcn );
        SMNMPB_SET_DEL_RTS_INFO( sKeySlot, &sNewRtsSeq, &sNewDeleteRtsVerNo );
        if( SMNMPB_IS_TBK( sKeySlot ) == STL_TRUE )
        {
            SMNMPB_SET_TBK_DEL_TRANS_ID( sKeySlot, &sTransId );
            sTmpScn = SML_MAKE_VIEW_SCN(sViewScn);
            SMNMPB_SET_TBK_DEL_SCN( sKeySlot, &sTmpScn );
        }
        SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sOrgKeySize + STL_SIZEOF(smpOffsetSlot) );
    }

    SMN_TRY_INTERNAL_ERROR( (SMNMPB_IS_TBK( sKeySlot ) == STL_TRUE) ||
                            (sNewRtsSeq != SMP_RTS_SEQ_NULL),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    SMP_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    smpSetMaxViewScn( aPageHandle, sViewScn );
    
    STL_TRY( smnmpbValidateIndexPage( aPageHandle,
                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smnRedoMemoryBtreeUndeleteLeaf( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv )
{
    stlChar          * sPtr = (stlChar*)aData;
    stlChar          * sColPtr;
    stlChar          * sKeySlot;
    stlInt16           sSlotSeq;
    stlBool            sFoundKey;
    stlUInt8           sInsertRtsSeq;
    stlUInt8           sDeleteRtsSeq;
    stlUInt8           sInsertRtsVerNo;
    stlUInt8           sDeleteRtsVerNo;
    smlRid             sRowRid;
    stlUInt16          sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT(aPageHandle);
    stlUInt16          sSlotHdrSize;
    stlUInt8           sTmpRtsSeq;
    stlUInt8           sTmpRtsVerNo;
    smxlTransId        sTmpTransId;
    smlScn             sTmpScn;
    stlUInt8           sDelTcn = SMNMPB_INVALID_TCN;

    /* Leaf Node여야 함 */
    SMN_TRY_INTERNAL_ERROR( SMNMPB_GET_LEVEL(aPageHandle) == 0,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    SMNMPB_GET_LEAF_KEY_HDR_SIZE(sPtr, &sSlotHdrSize);
    sColPtr = sPtr + sSlotHdrSize;
    sRowRid.mTbsId = 0; /* 사용 안함 */ 
    SMNMPB_GET_ROW_PID( sPtr, &sRowRid.mPageId );
    SMNMPB_GET_ROW_SEQ( sPtr, &sRowRid.mOffset );

    STL_TRY( smnmpbFindTargetKeyForRedo( aPageHandle,
                                         sColPtr,
                                         &sRowRid,
                                         &sSlotSeq,
                                         &sFoundKey,
                                         aEnv ) == STL_SUCCESS );

    /* 1. 해당 key 값과 RowRid를 갖는 key가 존재하여야 한다 */
    SMN_TRY_INTERNAL_ERROR( (sFoundKey == STL_TRUE) &&
                            (sSlotSeq < SMP_GET_PAGE_ROW_ITEM_COUNT(aPageHandle)),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    sKeySlot = smpGetNthRowItem( aPageHandle, sSlotSeq );
    SMN_TRY_INTERNAL_ERROR( SMNMPB_IS_DELETED( sKeySlot ) == STL_TRUE,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );
    SMNMPB_GET_INS_RTS_INFO( sKeySlot, &sInsertRtsSeq, &sInsertRtsVerNo );
    SMNMPB_GET_DEL_RTS_INFO( sKeySlot, &sDeleteRtsSeq, &sDeleteRtsVerNo );
    if( SMP_IS_VALID_RTS_SEQ(sDeleteRtsSeq) == STL_TRUE )
    {
        STL_TRY( smpEndUseRts( aPageHandle,
                               sKeySlot,
                               sDeleteRtsSeq,
                               sDeleteRtsVerNo,
                               aEnv ) == STL_SUCCESS );
        sTmpRtsSeq = SMP_RTS_SEQ_NULL;
        sTmpRtsVerNo = 0;
        SMNMPB_SET_DEL_RTS_INFO( sKeySlot, &sTmpRtsSeq, &sTmpRtsVerNo );
    }
    if( SMNMPB_IS_TBK( sKeySlot ) == STL_TRUE )
    {
        /* TBK */
        sTmpTransId = SML_INVALID_TRANSID;
        sTmpScn     = SML_INFINITE_SCN;
        SMNMPB_SET_TBK_DEL_TRANS_ID( sKeySlot, &sTmpTransId );
        SMNMPB_SET_TBK_DEL_SCN( sKeySlot, &sTmpScn );
    }
    SMNMPB_UNSET_DELETED( sKeySlot );
    SMNMPB_SET_DEL_TCN( sKeySlot, &sDelTcn );

    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );
    SMP_REMOVE_RECLAIMED_SPACE( SMP_FRAME(aPageHandle),
                                smnmpbGetLeafKeySize( sKeySlot, sKeyColCount )
                                + STL_SIZEOF(smpOffsetSlot));

    STL_TRY( smnmpbValidateIndexPage( aPageHandle,
                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnRedoMemoryBtreeDeleteIntl( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv )
{
    smlPid     sChildPid;
    smlPid     sOrgChildPid;
    stlChar  * sKeySlot;
    stlInt16   sKeySize;

    /* Leaf Node가 아니어야 함 */
    SMN_TRY_INTERNAL_ERROR( SMNMPB_GET_LEVEL(aPageHandle) > 0,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    /* LOG CONTENTS : child pid of deleted internal key */
    STL_WRITE_INT32( &sChildPid, aData);

    sKeySlot = smpGetNthRowItem(aPageHandle, aDataRid->mOffset);
    STL_WRITE_INT32( &sOrgChildPid, sKeySlot);
    sKeySize = smnmpbGetIntlKeySize(sKeySlot, SMNMPB_GET_KEY_COLUMN_COUNT(aPageHandle));

    SMN_TRY_INTERNAL_ERROR( sChildPid == sOrgChildPid,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    STL_TRY( smpFreeNthSlot( aPageHandle,
                             aDataRid->mOffset,
                             sKeySize ) == STL_SUCCESS );

    SMP_DECREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );
    SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sKeySize );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smnRedoSetRootPid( smlRid    * aDataRid,
                             void      * aData,
                             stlUInt16   aDataSize,
                             smpHandle * aPageHandle,
                             smlEnv    * aEnv )
{
    smnIndexHeader * sHeader;

    sHeader = (smnIndexHeader*)(SMP_FRAME(aPageHandle) + aDataRid->mOffset);

    STL_WRITE_INT32( &sHeader->mRootPageId, aData );

    return STL_SUCCESS;
}

stlStatus smnRedoSetMirrorRootPid( smlRid    * aDataRid,
                                   void      * aData,
                                   stlUInt16   aDataSize,
                                   smpHandle * aPageHandle,
                                   smlEnv    * aEnv )
{
    smnIndexHeader * sHeader;

    sHeader = (smnIndexHeader*)(SMP_FRAME(aPageHandle) + aDataRid->mOffset);

    STL_WRITE_INT32( &sHeader->mMirrorRootPid, aData );

    return STL_SUCCESS;
}

stlStatus smnRedoMemoryBtreeNewRoot( smlRid    * aDataRid,
                                     void      * aData,
                                     stlUInt16   aDataSize,
                                     smpHandle * aPageHandle,
                                     smlEnv    * aEnv )
{
    stlChar          * sPtr = (stlChar*)aData;
    smlPid             sChildPid;
    stlChar          * sKeySlot;

    /* Leaf Node가 아니어야 함 */
    SMN_TRY_INTERNAL_ERROR( SMNMPB_GET_LEVEL(aPageHandle) > 0,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    SMN_TRY_INTERNAL_ERROR( aDataSize == STL_SIZEOF(smlPid),
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    STL_WRITE_INT32( &sChildPid, sPtr );

    /* LOG CONTENTS : right child pid */
    STL_TRY( smpAllocNthSlot( aPageHandle,
                              0,
                              STL_SIZEOF(smlPid),
                              STL_FALSE, /* aIsForTest */
                              &sKeySlot ) == STL_SUCCESS );

    SMN_TRY_INTERNAL_ERROR( sKeySlot != NULL,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    STL_WRITE_INT32( sKeySlot, &sChildPid );
    SMP_INCREASE_ACTIVE_SLOT_COUNT( SMP_FRAME(aPageHandle) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smnRedoMemoryBtreeSplitNewPage( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv )
{
    stlChar          * sCurPtr = (stlChar*)aData;
    stlUInt16          sRtsCount;
    void *             sRtsArray;
    stlUInt16          sRightKeyCount;
    stlChar          * sCurKey;
    stlUInt16          sSlotSize;
    stlUInt16          sOffsetArray[SMP_PAGE_SIZE/STL_SIZEOF(stlUInt16)];
    stlInt32           i;
    stlInt16           sLevel;
    stlUInt16          sKeyColCount;
    stlInt16           sActiveKeyCount = 0;
    stlUInt16          sReclaimedSpace = 0;

    /* LOG CONTENTS : RtsCount + RtsArray + KeyCount + KeyData */
    STL_WRITE_INT16( &sRtsCount, sCurPtr );
    sCurPtr += STL_SIZEOF(stlUInt16);
    sRtsArray = sCurPtr;
    sCurPtr += smpGetRtsSize() * sRtsCount;
    STL_WRITE_INT16( &sRightKeyCount, sCurPtr );
    sCurPtr += STL_SIZEOF(stlUInt16);
    sCurKey = sCurPtr;

    if( sRightKeyCount > 0 )
    {
        sLevel = SMNMPB_GET_LEVEL( aPageHandle );
        sKeyColCount = SMNMPB_GET_KEY_COLUMN_COUNT( aPageHandle );

        /* physical REDO */
        for( i = 0; i < sRightKeyCount; i++ )
        {
            if( sLevel == 0 )
            {
                sSlotSize = smnmpbGetLeafKeySize( sCurKey, sKeyColCount );
                if( SMNMPB_IS_DELETED(sCurKey) != STL_TRUE )
                {
                    sActiveKeyCount++;
                }
                else
                {
                    sReclaimedSpace += ( sSlotSize + STL_SIZEOF(smpOffsetSlot) );
                }
            }
            else
            {
                if( (i == 0) && (SMNMPB_GET_NEXT_PAGE( aPageHandle ) == SMP_NULL_PID) )
                {
                    /* MAX KEY */
                    sSlotSize = STL_SIZEOF(smlPid);
                }
                else
                {
                    sSlotSize = smnmpbGetIntlKeySize( sCurKey, sKeyColCount );
                }
                sActiveKeyCount++;
            }
            /* 뒤로 부터 채워간다 */
            sOffsetArray[sRightKeyCount - i - 1] = sCurKey - sCurPtr;
            sCurKey += sSlotSize;
        }

        smpReformPage( aPageHandle,
                       sReclaimedSpace,
                       sActiveKeyCount,
                       sRtsCount,
                       sRtsArray,
                       sRightKeyCount,
                       sOffsetArray,
                       sCurKey - sCurPtr,
                       sCurPtr );
    }

    STL_TRY( smnmpbValidateIndexPage( aPageHandle,
                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smnRedoMemoryBtreeSplitOldPage( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv )
{
    stlUInt16          sLeftKeyCount;
    stlChar          * sCurPtr = (stlChar*)aData;
    stlChar          * sColPtr;
    smlRid             sRowRid;
    stlInt16           sSlotSeq;
    stlBool            sFoundKey;
    stlInt16           sLastKeySize;
    stlChar          * sLastKey;
    stlChar          * sKeySlot;
    stlUInt16          sSlotHdrSize;
    stlUInt8           sRtsSeq;
    stlUInt8           sRtsVerNo;
    smxlTransId        sTmpTransId;
    smlScn             sTmpScn;
    stlUInt8           sDelTcn = SMNMPB_INVALID_TCN;

    /* LOG CONTENTS : LeftKeyCount + LastKeyData  */
    STL_WRITE_INT16( &sLeftKeyCount, sCurPtr );
    sCurPtr += STL_SIZEOF(stlUInt16);
    sLastKeySize = aDataSize - STL_SIZEOF(stlUInt16);
    sLastKey = sCurPtr;

    if( SMNMPB_GET_LEVEL( aPageHandle ) == 0 )
    {
        if( sLastKeySize > 0 )
        {
            /* Leaf Node */
            SMNMPB_GET_LEAF_KEY_HDR_SIZE(sCurPtr, &sSlotHdrSize);
            sColPtr = sCurPtr + sSlotHdrSize;
            sRowRid.mTbsId = 0; /* 사용 안함 */ 
            SMNMPB_GET_ROW_PID( sCurPtr, &sRowRid.mPageId );
            SMNMPB_GET_ROW_SEQ( sCurPtr, &sRowRid.mOffset );

            STL_TRY( smnmpbFindTargetKeyForRedo( aPageHandle,
                                                 sColPtr,
                                                 &sRowRid,
                                                 &sSlotSeq,
                                                 &sFoundKey,
                                                 aEnv ) == STL_SUCCESS );
        }
        else
        {
            sSlotSeq  = 0;
            sFoundKey = STL_FALSE;
        }

        if( sFoundKey == STL_TRUE )
        {
            /* 해당 key 존재. 해당 slot 이후로 모두 truncate */
            STL_TRY( smnmpbTruncateNode( NULL,
                                         aPageHandle,
                                         sSlotSeq + 1,
                                         aEnv ) == STL_SUCCESS );
        }
        else
        {
            /* 해당 key 존재하지 않음. 해당 slot 포함 이후로 모두 truncate */
            STL_TRY( smnmpbTruncateNode( NULL,
                                         aPageHandle,
                                         sSlotSeq,
                                         aEnv ) == STL_SUCCESS );
        }

        if( SMP_GET_PAGE_ROW_ITEM_COUNT( aPageHandle ) == 0 )
        {
            if( sLastKeySize > 0 )
            {
                /**
                 * Empty Node에 달릴 경우에 key count가 0이되면 parent internal node를 못찾는다
                 * Old Page의 rowitem 개수가 redo 중 aging에 의해 0이 되면 Empty Node 처리시 힘들어 지므로
                 * 로깅된 마지막 키를 DEAD 상태로 집어넣는다
                 */
                STL_TRY( smpAllocNthSlot( aPageHandle,
                                          0,
                                          sLastKeySize,
                                          STL_FALSE, /* aIsForTest */
                                          &sKeySlot ) == STL_SUCCESS );

                SMN_TRY_INTERNAL_ERROR( sKeySlot != NULL,
                                        aEnv,
                                        ( aPageHandle, aEnv, NULL ) );
            
                stlMemcpy( sKeySlot, sLastKey, sLastKeySize );
                sRtsSeq = SMP_RTS_SEQ_STABLE;
                sRtsVerNo = 0;
                SMNMPB_SET_INS_RTS_INFO( sKeySlot, &sRtsSeq, &sRtsVerNo );
                SMNMPB_SET_DELETED( sKeySlot );
                SMNMPB_SET_DEL_TCN( sKeySlot, &sDelTcn );
                SMNMPB_SET_DEL_RTS_INFO( sKeySlot, &sRtsSeq, &sRtsVerNo );
                if( SMNMPB_IS_TBK( sKeySlot ) == STL_TRUE )
                {
                    sTmpTransId = SML_INVALID_TRANSID;
                    sTmpScn     = 0;
                    SMNMPB_SET_TBK_INS_TRANS_ID( sKeySlot, &sTmpTransId );
                    SMNMPB_SET_TBK_INS_SCN( sKeySlot, &sTmpScn );
                    SMNMPB_SET_TBK_DEL_TRANS_ID( sKeySlot, &sTmpTransId );
                    SMNMPB_SET_TBK_DEL_SCN( sKeySlot, &sTmpScn );
                }

                SMP_ADD_RECLAIMED_SPACE( SMP_FRAME(aPageHandle), sLastKeySize + STL_SIZEOF(smpOffsetSlot) );
            }
        }
    }
    else
    {
        /* Internal Node */
        if( sLastKeySize > 0 )
        {
            /* max key가 아니어야 한다 */
            SMN_TRY_INTERNAL_ERROR( (stlChar*)aData + aDataSize - sCurPtr > STL_SIZEOF(smlPid),
                                    aEnv,
                                    ( aPageHandle, aEnv, NULL ) );

            sColPtr = sCurPtr + SMNMPB_INTL_SLOT_HDR_SIZE;
            sRowRid.mTbsId = 0; /* 사용 안함 */ 
            STL_WRITE_INT32( &sRowRid.mPageId, sCurPtr + STL_SIZEOF(smlPid) );
            STL_WRITE_INT16( &sRowRid.mOffset, sCurPtr + STL_SIZEOF(smlPid) + STL_SIZEOF(smlPid) );

            STL_TRY( smnmpbFindTargetKeyForRedo( aPageHandle,
                                                 sColPtr,
                                                 &sRowRid,
                                                 &sSlotSeq,
                                                 &sFoundKey,
                                                 aEnv ) == STL_SUCCESS );

            /* internal node에는 hole이 없으므로 무조건 해당 key가 존재함 */
            STL_TRY( smnmpbTruncateNode( NULL,
                                         aPageHandle,
                                         sSlotSeq + 1,
                                         aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smnmpbTruncateNode( NULL,
                                         aPageHandle,
                                         0,
                                         aEnv ) == STL_SUCCESS );
        }
    }

    STL_TRY( smnmpbValidateIndexPage( aPageHandle,
                                      aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smnRedoMemoryBtreeSetPrevPageId( smlRid    * aDataRid,
                                           void      * aData,
                                           stlUInt16   aDataSize,
                                           smpHandle * aPageHandle,
                                           smlEnv    * aEnv )
{
    smnmpbLogicalHdr * sHdr = SMNMPB_GET_LOGICAL_HEADER( aPageHandle );

    STL_WRITE_INT32( &sHdr->mPrevPageId, aData );

    return STL_SUCCESS;
}

stlStatus smnRedoMemoryBtreeSetNextPageId( smlRid    * aDataRid,
                                           void      * aData,
                                           stlUInt16   aDataSize,
                                           smpHandle * aPageHandle,
                                           smlEnv    * aEnv )
{
    smnmpbLogicalHdr * sHdr = SMNMPB_GET_LOGICAL_HEADER( aPageHandle );

    STL_WRITE_INT32( &sHdr->mNextPageId, aData );

    return STL_SUCCESS;
}


stlStatus smnRedoMemoryBtreeSetChildPid( smlRid    * aDataRid,
                                         void      * aData,
                                         stlUInt16   aDataSize,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv )
{
    stlChar * sKey = smpGetNthRowItem(aPageHandle, aDataRid->mOffset);
    smlPid    sChildPid;

    SMN_TRY_INTERNAL_ERROR( SMNMPB_GET_LEVEL( aPageHandle ) > 0,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    STL_WRITE_INT32( &sChildPid, aData);

    SMNMPB_INTL_SLOT_SET_CHILD_PID( sKey, &sChildPid );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smnRedoMemoryBtreeSetNextFreePage( smlRid    * aDataRid,
                                             void      * aData,
                                             stlUInt16   aDataSize,
                                             smpHandle * aPageHandle,
                                             smlEnv    * aEnv )
{
    smnmpbLogicalHdr * sHdr = SMNMPB_GET_LOGICAL_HEADER( aPageHandle );

    STL_WRITE_INT32( &sHdr->mNextFreePageId, aData );

    return STL_SUCCESS;
}


stlStatus smnRedoMemoryBtreeAgingCompact( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv )
{
    stlInt16   sDummySeq = 0;
    stlBool    sIsFreed;

    STL_TRY( smnmpbDoAgingLeafNode( NULL,
                                    aPageHandle,
                                    &sDummySeq,
                                    STL_TRUE,
                                    &sIsFreed,
                                    aEnv ) == STL_SUCCESS );

    STL_TRY( smnmpbCompactPage( NULL,
                                aPageHandle,
                                STL_INT16_MAX,
                                aEnv ) == STL_SUCCESS );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smnRedoAddEmptyNode( smlRid    * aDataRid,
                               void      * aData,
                               stlUInt16   aDataSize,
                               smpHandle * aPageHandle,
                               smlEnv    * aEnv )
{
    smnIndexHeader * sHeader;
    smlPid           sPid;

    sHeader = (smnIndexHeader*)(SMP_FRAME(aPageHandle) + aDataRid->mOffset);
    STL_WRITE_INT32( &sPid, aData );

    if( sHeader->mEmptyNodeCount == 0 )
    {
        sHeader->mFirstEmptyNode = sPid;
        sHeader->mEmptyNodeCount = 1;
    }
    else
    {
        sHeader->mEmptyNodeCount++;
    }
    sHeader->mLastEmptyNode = sPid;

    return STL_SUCCESS;
}



stlStatus smnRedoRemoveEmptyNode( smlRid    * aDataRid,
                                  void      * aData,
                                  stlUInt16   aDataSize,
                                  smpHandle * aPageHandle,
                                  smlEnv    * aEnv )
{
    smnIndexHeader * sHeader;
    smlPid           sPid;
    smlPid           sNextFreePid;
    stlChar        * sPtr = aData;

    sHeader = (smnIndexHeader*)(SMP_FRAME(aPageHandle) + aDataRid->mOffset);
    STL_WRITE_INT32( &sPid, sPtr );
    sPtr += STL_SIZEOF(smlPid);
    STL_WRITE_INT32( &sNextFreePid, sPtr );
    sPtr += STL_SIZEOF(smlPid);

    /* empty node는 동시에 한넘만 조정하기 때문에 전,후가 정확해야 한다 */
    SMN_TRY_INTERNAL_ERROR( sHeader->mFirstEmptyNode == sPid,
                            aEnv,
                            ( aPageHandle, aEnv, NULL ) );

    if( sHeader->mEmptyNodeCount == 1 )
    {
        sHeader->mLastEmptyNode = SMP_NULL_PID;
        sHeader->mEmptyNodeCount = 0;
    }
    else
    {
        sHeader->mEmptyNodeCount--;
    }
    sHeader->mFirstEmptyNode = sNextFreePid;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smnRedoPageImage( smlRid    * aDataRid,
                            void      * aData,
                            stlUInt16   aDataSize,
                            smpHandle * aPageHandle,
                            smlEnv    * aEnv )
{
    stlMemcpy( SMP_FRAME(aPageHandle), aData, SMP_PAGE_SIZE );

    smpInitVolatileArea( aPageHandle );
    
    return STL_SUCCESS;
}


stlStatus smnRedoMemoryBtreeSetNodeFlag( smlRid    * aDataRid,
                                         void      * aData,
                                         stlUInt16   aDataSize,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv )
{
    smnmpbLogicalHdr * sHdr = SMNMPB_GET_LOGICAL_HEADER( aPageHandle );

    STL_DASSERT( aDataSize == STL_SIZEOF(stlUInt8) );

    sHdr->mFlag = *(stlUInt8*)aData;

    return STL_SUCCESS;
}


stlStatus smnRedoAlterDatatype( smlRid    * aDataRid,
                                void      * aData,
                                stlUInt16   aDataSize,
                                smpHandle * aPageHandle,
                                smlEnv    * aEnv )
{
    smnIndexHeader * sIndexHeader;
    stlUInt8         sDataType;
    stlInt32         sColumnOrder;
    stlInt64         sColumnSize;
    stlInt32         sOffset = 0;
    
    STL_DASSERT( aDataSize == ( STL_SIZEOF(stlInt32) +
                                STL_SIZEOF(stlInt8)  +
                                STL_SIZEOF(stlInt64)) );

    STL_READ_MOVE_INT32( &sColumnOrder, aData, sOffset );
    STL_READ_MOVE_INT8( &sDataType, aData, sOffset );
    STL_READ_MOVE_INT64( &sColumnSize, aData, sOffset );
    
    sIndexHeader = (smnIndexHeader*)( SMP_FRAME(aPageHandle) + aDataRid->mOffset );

    sIndexHeader->mKeyColTypes[sColumnOrder] = sDataType;
    sIndexHeader->mKeyColSize[sColumnOrder] = sColumnSize;

    return STL_SUCCESS;
}


/** @} */
