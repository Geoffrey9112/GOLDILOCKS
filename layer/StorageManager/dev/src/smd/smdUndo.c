/*******************************************************************************
 * smdUndo.c
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
#include <smp.h>
#include <smxl.h>
#include <smxm.h>
#include <sme.h>
#include <smf.h>
#include <smr.h>
#include <sms.h>
#include <smr.h>
#include <smdDef.h>
#include <smdmphRowDef.h>
#include <smdmpfRowDef.h>
#include <smdmphDef.h>
#include <smdmpfDef.h>
#include <smdmppDef.h>
#include <smdmph.h>
#include <smdmpf.h>
#include <smdUndo.h>

/**
 * @file smdUndo.c
 * @brief Storage Manager Table Undo Internal Routines
 */

/**
 * @addtogroup smdUndo
 * @{
 */

extern smdTableModule * gSmdTableModules[];

extern smdTableUndoModule gSmdmphTableUndoModule;
extern smdTableUndoModule gSmdmpfTableUndoModule;

smdTableUndoModule * gSmdTableUndoModule[SMP_PAGE_TYPE_MAX] =
{
    NULL,        /* INIT */
    NULL,        /* UNFORMAT */
    NULL,        /* EXT_BLOCK_MAP */
    NULL,        /* EXT_MAP */
    NULL,        /* BITMAP_HEADER */
    NULL,        /* CIRCULAR_HEADER */
    NULL,        /* FLAT_HEADER */
    NULL,        /* PENDING_HEADER */
    NULL,        /* UNDO */
    NULL,        /* TRANS */
    NULL,        /* BITMAP_INTERNAL */
    NULL,        /* BITMAP_LEAF */
    NULL,        /* BITMAP_EXTENT_MAP */
    &gSmdmphTableUndoModule,        /* TABLE_DATA */
    &gSmdmpfTableUndoModule,        /* COLUMNAR_TABLE_DATA */
    NULL         /* INDEX_DATA*/
};

smxlUndoFunc gSmdUndoFuncs[SMD_UNDO_LOG_MAX_COUNT] =
{
    smdUndoMemoryHeapInsert,
    smdUndoMemoryHeapUpdate,
    smdUndoMemoryHeapMigrate,
    smdUndoMemoryHeapUpdateLink,
    smdUndoMemoryHeapDelete,
    smdUndoMemoryHeapCreate,
    smdUndoMemoryHeapDrop,
    smdUndoMemoryHeapAppend,
    smdUndoMemoryHeapAlterAttr,
    smdUndoMemoryPendingCreate,
    smdUndoMemoryPendingDrop,
    smdUndoMemoryHeapBlockInsert,
    smdUndoMemoryPendingInsert
};

stlStatus smdUndoMemoryHeapInsert( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv )
{
    smpHandle       sPageHandle;
    smpPageType     sPageType;

    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(&sPageHandle) );

    STL_TRY( gSmdTableUndoModule[sPageType]->mUndoInsert( aMiniTrans,
                                                          aTargetRid,
                                                          aLogBody,
                                                          aLogSize,
                                                          &sPageHandle,
                                                          aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




stlStatus smdUndoMemoryHeapUpdate( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv )
{
    smpHandle       sPageHandle;
    smpPageType     sPageType;

    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(&sPageHandle) );

    STL_TRY( gSmdTableUndoModule[sPageType]->mUndoUpdate( aMiniTrans,
                                                          aTargetRid,
                                                          aLogBody,
                                                          aLogSize,
                                                          &sPageHandle,
                                                          aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdUndoMemoryHeapMigrate( smxmTrans * aMiniTrans,
                                    smlRid      aTargetRid,
                                    void      * aLogBody,
                                    stlInt16    aLogSize,
                                    smlEnv    * aEnv )
{
    smpHandle       sPageHandle;
    smpPageType     sPageType;

    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(&sPageHandle) );

    STL_TRY( gSmdTableUndoModule[sPageType]->mUndoMigrate( aMiniTrans,
                                                           aTargetRid,
                                                           aLogBody,
                                                           aLogSize,
                                                           &sPageHandle,
                                                           aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdUndoMemoryHeapUpdateLink( smxmTrans * aMiniTrans,
                                       smlRid      aTargetRid,
                                       void      * aLogBody,
                                       stlInt16    aLogSize,
                                       smlEnv    * aEnv )
{
    smpHandle       sPageHandle;
    smpPageType     sPageType;

    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(&sPageHandle) );

    STL_TRY( gSmdTableUndoModule[sPageType]->mUndoUpdateLink( aMiniTrans,
                                                              aTargetRid,
                                                              aLogBody,
                                                              aLogSize,
                                                              &sPageHandle,
                                                              aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdUndoMemoryHeapDelete( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv )
{
    smpHandle       sPageHandle;
    smpPageType     sPageType;

    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(&sPageHandle) );

    STL_TRY( gSmdTableUndoModule[sPageType]->mUndoDelete( aMiniTrans,
                                                          aTargetRid,
                                                          aLogBody,
                                                          aLogSize,
                                                          &sPageHandle,
                                                          aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdUndoMemoryHeapCreate( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv )
{
    smxlTransId  sTransId;
    void       * sRelationHandle;
    void       * sSegmentHandle;
    smlTableType sTableType;
    stlBool      sOnStartup = STL_FALSE;

    sTransId = smxmGetTransId( aMiniTrans );

    if( (smfGetServerState() == SML_SERVER_STATE_RECOVERING) ||
        (smrProceedMediaRecovery(aEnv) == STL_TRUE) )
    {
        sOnStartup = STL_TRUE;
    }

    STL_TRY( smeGetRelationHandle( aTargetRid,
                                   &sRelationHandle,
                                   aEnv )
             == STL_SUCCESS);

    if( sRelationHandle != NULL )
    {
        sTableType = SML_GET_TABLE_TYPE( SME_GET_RELATION_TYPE( sRelationHandle ) );
        
        /**
         * new mini-transaction의 사용은 undo page의 delete flag와 atomic함을 보장하지
         * 못한다. 따라서, restart로 인하여 create undo가 여러번 호출될수 있음을
         * 고려해야 한다.
         */
    
        if( SME_GET_RELATION_STATE( sRelationHandle ) != SME_RELATION_STATE_DROPPING )
        {
            STL_TRY( gSmdTableModules[sTableType]->mCreateUndo( sTransId,
                                                                sRelationHandle,
                                                                SML_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
    }
    else
    {
        STL_TRY( smsGetSegmentHandle( aTargetRid,
                                      &sSegmentHandle,
                                      aEnv )
                 == STL_SUCCESS);

        if( sSegmentHandle != NULL )
        {
            STL_TRY( smsDrop( sTransId,
                              sSegmentHandle,
                              sOnStartup,
                              aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * Segment Header만 할당되어 있는 경우
             */
            STL_TRY( smsFreeSegMapPage( sTransId,
                                        aTargetRid,
                                        aEnv ) == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdUndoMemoryHeapDrop( smxmTrans * aMiniTrans,
                                 smlRid      aTargetRid,
                                 void      * aLogBody,
                                 stlInt16    aLogSize,
                                 smlEnv    * aEnv )
{
    smpHandle        sPageHandle;
    stlInt16         sOffset;
    stlUInt16        sData[2];
    void           * sSegmentHandle;
    void           * sRelationHandle;
    smdTableHeader * sTableHeader;

    STL_TRY( smeGetRelationHandle( aTargetRid,
                                   &sRelationHandle,
                                   aEnv ) == STL_SUCCESS);
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( sRelationHandle );
    
    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sTableHeader = (smdTableHeader*)( smpGetBody(SMP_FRAME(&sPageHandle)) +
                                      smsGetHeaderSize( sSegmentHandle) );

    sOffset = ( (stlChar*)&sTableHeader->mRelHeader.mRelState -
                (stlChar*)SMP_FRAME(&sPageHandle) );

    sData[0] = sTableHeader->mRelHeader.mRelState;
    sTableHeader->mRelHeader.mRelState = SME_RELATION_STATE_ACTIVE;
    sData[1] = sTableHeader->mRelHeader.mRelState;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sData,
                           STL_SIZEOF( stlUInt16 ) * 2,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           sOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    SME_SET_RELATION_STATE( sRelationHandle, SME_RELATION_STATE_ACTIVE );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdUndoMemoryHeapAppend( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv )
{
    smpHandle       sPageHandle;
    smpPageType     sPageType;

    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(&sPageHandle) );

    STL_TRY( gSmdTableUndoModule[sPageType]->mUndoAppend( aMiniTrans,
                                                          aTargetRid,
                                                          aLogBody,
                                                          aLogSize,
                                                          &sPageHandle,
                                                          aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdUndoMemoryHeapAlterAttr( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smlEnv    * aEnv )
{
    smpHandle             sPageHandle;
    smdTableHeader      * sTableHeader;
    smlTableAttr          sTableAttr;
    void                * sRelationHandle;
    void                * sSegmentHandle;
    smdTableHeader      * sCache;
    smlRid                sSegmentRid;

    stlMemcpy( &sTableAttr, aLogBody, STL_SIZEOF(smlTableAttr) );
    
    STL_TRY( smeGetRelationHandle( aTargetRid,
                                   &sRelationHandle,
                                   aEnv ) == STL_SUCCESS);
    
    sCache = (smdTableHeader*)sRelationHandle;
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( sRelationHandle );
    sSegmentRid = smsGetSegRid( sSegmentHandle );
    
    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sTableHeader = (smdTableHeader*)( smpGetBody(SMP_FRAME(&sPageHandle)) +
                                      smsGetHeaderSize( sSegmentHandle ) );

    /**
     * applys table attribute to relation header
     */

    STL_DASSERT( sTableAttr.mValidFlags == SML_TABLE_DEFAULT_MEMORY_ATTR );

    sTableHeader->mLoggingFlag = sTableAttr.mLoggingFlag;
    sTableHeader->mLockingFlag = sTableAttr.mLockingFlag;
    sTableHeader->mIniTrans = sTableAttr.mIniTrans;
    sTableHeader->mMaxTrans = sTableAttr.mMaxTrans;
    sTableHeader->mPctFree = sTableAttr.mPctFree;
    sTableHeader->mPctUsed = sTableAttr.mPctUsed;
    
    /**
     * applys table attribute to relation cache
     */

    sCache->mLoggingFlag = sTableHeader->mLoggingFlag;
    sCache->mLockingFlag = sTableHeader->mLockingFlag;
    sCache->mIniTrans = sTableHeader->mIniTrans;
    sCache->mMaxTrans = sTableHeader->mMaxTrans;
    sCache->mPctFree = sTableHeader->mPctFree;
    sCache->mPctUsed = sTableHeader->mPctUsed;
    
    /**
     * logging redo log
     */
    
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_RELATION,
                           SMR_LOG_INIT_REL_HEADER,
                           SMR_REDO_TARGET_PAGE,
                           (stlChar*)sTableHeader,
                           STL_SIZEOF(smdTableHeader),
                           sSegmentRid.mTbsId,
                           sSegmentRid.mPageId,
                           (stlChar*)sTableHeader - (stlChar*)SMP_FRAME(&sPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdUndoMemoryPendingCreate( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smlEnv    * aEnv )
{
    smxlTransId  sTransId;
    void       * sRelationHandle;
    void       * sSegmentHandle;
    smlTableType sTableType;
    stlBool      sOnStartup = STL_FALSE;

    sTransId = smxmGetTransId( aMiniTrans );

    if( (smfGetServerState() == SML_SERVER_STATE_RECOVERING) ||
        (smrProceedMediaRecovery(aEnv) == STL_TRUE) )
    {
        sOnStartup = STL_TRUE;
    }

    STL_TRY( smeGetRelationHandle( aTargetRid,
                                   &sRelationHandle,
                                   aEnv )
             == STL_SUCCESS);

    if( sRelationHandle != NULL )
    {
        sTableType = SML_GET_TABLE_TYPE( SME_GET_RELATION_TYPE( sRelationHandle ) );
        
        STL_TRY( gSmdTableModules[sTableType]->mDropAging( sTransId,
                                                           sRelationHandle,
                                                           sOnStartup,
                                                           SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smsGetSegmentHandle( aTargetRid,
                                      &sSegmentHandle,
                                      aEnv )
                 == STL_SUCCESS);

        if( sSegmentHandle != NULL )
        {
            STL_TRY( smsDrop( sTransId,
                              sSegmentHandle,
                              sOnStartup,
                              aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            /**
             * Segment Header만 할당되어 있는 경우
             */
            STL_TRY( smsFreeSegMapPage( sTransId,
                                        aTargetRid,
                                        aEnv ) == STL_SUCCESS );
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdUndoMemoryPendingDrop( smxmTrans * aMiniTrans,
                                    smlRid      aTargetRid,
                                    void      * aLogBody,
                                    stlInt16    aLogSize,
                                    smlEnv    * aEnv )
{
    smpHandle        sPageHandle;
    stlInt16         sOffset;
    stlUInt16        sData[2];
    void           * sSegmentHandle;
    void           * sRelationHandle;
    smdTableHeader * sTableHeader;

    STL_TRY( smeGetRelationHandle( aTargetRid,
                                   &sRelationHandle,
                                   aEnv ) == STL_SUCCESS);
    sSegmentHandle = SME_GET_SEGMENT_HANDLE( sRelationHandle );
    
    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sTableHeader = (smdTableHeader*)( smpGetBody(SMP_FRAME(&sPageHandle)) +
                                      smsGetHeaderSize( sSegmentHandle) );

    sOffset = ( (stlChar*)&sTableHeader->mRelHeader.mRelState -
                (stlChar*)SMP_FRAME(&sPageHandle) );

    sData[0] = sTableHeader->mRelHeader.mRelState;
    sTableHeader->mRelHeader.mRelState = SME_RELATION_STATE_ACTIVE;
    sData[1] = sTableHeader->mRelHeader.mRelState;

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sData,
                           STL_SIZEOF( stlUInt16 ) * 2,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           sOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    SME_SET_RELATION_STATE( sRelationHandle, SME_RELATION_STATE_ACTIVE );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdUndoMemoryHeapBlockInsert( smxmTrans * aMiniTrans,
                                        smlRid      aTargetRid,
                                        void      * aLogBody,
                                        stlInt16    aLogSize,
                                        smlEnv    * aEnv )
{
    smpHandle       sPageHandle;
    smpPageType     sPageType;

    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sPageType = SMP_GET_PAGE_TYPE( SMP_FRAME(&sPageHandle) );

    STL_TRY( gSmdTableUndoModule[sPageType]->mUndoBlockInsert( aMiniTrans,
                                                               aTargetRid,
                                                               aLogBody,
                                                               aLogSize,
                                                               &sPageHandle,
                                                               aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdUndoMemoryPendingInsert( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smlEnv    * aEnv )
{
    smpHandle    sPageHandle;
    stlChar    * sRowHdr;
    stlChar      sData[2];
    
    STL_TRY( smpAcquire( aMiniTrans,
                         aTargetRid.mTbsId,
                         aTargetRid.mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sRowHdr = smpGetNthRowItem( &sPageHandle, aTargetRid.mOffset );

    SMDMPP_SET_DELETED( sRowHdr );

    sData[0] = sRowHdr[0];
    sData[1] = sRowHdr[0];

    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_RECOVERY,
                           SMR_LOG_BYTES,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sData,
                           STL_SIZEOF(stlChar) * 2,
                           aTargetRid.mTbsId,
                           aTargetRid.mPageId,
                           (stlChar*)sRowHdr - (stlChar*)SMP_FRAME(&sPageHandle),
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdUndoAllocColumnHdrPage( smxmTrans * aMiniTrans,
                                     smlRid      aTargetRid,
                                     void      * aLogBody,
                                     stlInt16    aLogSize,
                                     smlEnv    * aEnv )
{
    smxlTransId  sTransId;

    sTransId = smxmGetTransId( aMiniTrans );

    STL_TRY( smsFreeSegMapColumnMeta( sTransId,
                                      aTargetRid.mPageId,
                                      aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
