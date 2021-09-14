/*******************************************************************************
 * smqRelation.c
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
#include <sma.h>
#include <sme.h>
#include <smd.h>
#include <smp.h>
#include <smxm.h>
#include <smqDef.h>

extern smqWarmupEntry * gSmqWarmupEntry;

/**
 * @file smqMap.c
 * @brief Storage Manager Layer Sequence Map Internal Routines
 */

/**
 * @addtogroup smq
 * @{
 */

stlStatus smqMapCreate( smlStatement * aStatement,
                        smlEnv       * aEnv )
{
    smlTableAttr        sAttr;
    stlInt64            sSegmentId;
    void              * sRelationHandle;
    smdTableModule    * sModule;
    stlInt64            sExpectedSegmentId;
    smlRid              sSegmentRid;

    sModule = smdGetTableModule( SML_GET_TABLE_TYPE(SML_MEMORY_PERSISTENT_HEAP_ROW_TABLE) );
    STL_ASSERT( sModule != NULL );

    sAttr.mValidFlags = 0;
    sAttr.mValidFlags |= SML_TABLE_LOCKING_YES | SML_TABLE_PCTFREE_YES | SML_TABLE_PCTUSED_YES;
    sAttr.mLockingFlag = STL_FALSE;
    sAttr.mPctFree = 90;
    sAttr.mPctUsed = 10;
    sAttr.mSegAttr.mValidFlags = 0;

    STL_TRY( sModule->mCreate( aStatement,
                               SML_MEMORY_DICT_SYSTEM_TBS_ID,
                               &sAttr,
                               STL_FALSE,
                               &sSegmentId,
                               &sRelationHandle,
                               aEnv) == STL_SUCCESS );

    sSegmentRid = SMQ_SEQUENCE_MAP_RID;
    stlMemcpy( (void*)&sExpectedSegmentId, (void*)&sSegmentRid, STL_SIZEOF( smlRid ) );
    STL_ASSERT( sSegmentId == sExpectedSegmentId );
    
    gSmqWarmupEntry->mSequenceMapHandle = sRelationHandle;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */


/**
 * @addtogroup smqMap
 * @{
 */

stlStatus smqInsertRecord( smlStatement * aStatement,
                           smqRecord    * aRecord,
                           smlRid       * aSequenceRid,
                           smlEnv       * aEnv )
{
    stlInt32            i;
    smlRelationType     sRelType;
    smdTableModule    * sModule;
    knlValueBlockList * sColumnValueList[SMQ_COLUMN_COUNT];
    stlChar             sBuffer[SMQ_SEQUENCE_RECORD_DUMMY_SIZE];
    stlChar           * sDummy = (stlChar*)sBuffer;
    knlRegionMark       sMemMark;
    stlInt32            sState = 0;

    sRelType = SME_GET_RELATION_TYPE( gSmqWarmupEntry->mSequenceMapHandle );
    sModule = smdGetTableModule( SML_GET_TABLE_TYPE(sRelType) );

    KNL_INIT_REGION_MARK(&sMemMark);
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sMemMark,
                               (knlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    for ( i = 0; i < SMQ_COLUMN_COUNT; i++ )
    {
        STL_TRY( knlInitBlockNoBuffer( &sColumnValueList[i],
                                       STL_LAYER_STORAGE_MANAGER,
                                       1,
                                       DTL_TYPE_BINARY,
                                       &aEnv->mOperationHeapMem,
                                       (knlEnv*)aEnv ) == STL_SUCCESS );
    }

    stlMemset( sDummy, 0x00, SMQ_SEQUENCE_RECORD_DUMMY_SIZE );
    
    SMQ_BUILD_COLUMN_LIST( sColumnValueList, aRecord, sDummy );

    STL_TRY( sModule->mInsert( aStatement,
                               gSmqWarmupEntry->mSequenceMapHandle,
                               0,
                               sColumnValueList[0],
                               NULL, /* unique violation */
                               aSequenceRid,
                               aEnv ) == STL_SUCCESS );

    STL_ASSERT( aSequenceRid->mOffset == 0 );
    
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

stlStatus smqUpdateValue( smxmTrans  * aMiniTrans,
                          stlInt64     aSequenceValue,
                          smlRid     * aSequenceRid,
                          smlEnv     * aEnv )
{
    smxmTrans           sMiniTrans;
    smpHandle           sPageHandle;
    stlChar           * sSeqSlot;
    stlChar           * sSeqBody;
    smqRecord           sSeqRecord;
    stlInt64            sRecordLen;
    stlInt32            sState = 0;
    smlRelationType     sRelType;
    smdTableModule    * sModule;
    stlInt16            sSlotBodyOffset;
    stlInt16            sLogOffset = 0;
    stlChar             sLogData[STL_SIZEOF( stlInt64 ) + STL_SIZEOF( stlInt16 )];

    sRelType = SME_GET_RELATION_TYPE( gSmqWarmupEntry->mSequenceMapHandle );
    sModule = smdGetTableModule( SML_GET_TABLE_TYPE(sRelType) );

    if( aMiniTrans == NULL )
    {
        STL_TRY( smxmBegin( &sMiniTrans,
                            SMXL_SYSTEM_TRANSID,
                            SMQ_SEQUENCE_MAP_RID,
                            SMXM_ATTR_REDO,
                            aEnv ) == STL_SUCCESS);
        sState = 1;

        aMiniTrans = &sMiniTrans;
    }

    STL_TRY( smpAcquire( aMiniTrans,
                         aSequenceRid->mTbsId,
                         aSequenceRid->mPageId,
                         KNL_LATCH_MODE_EXCLUSIVE,
                         &sPageHandle,
                         aEnv ) == STL_SUCCESS );

    sSeqSlot = smpGetNthRowItem( &sPageHandle,
                                 aSequenceRid->mOffset );
    sSeqBody = sModule->mGetSlotBody( sSeqSlot );
    sSlotBodyOffset = (stlInt64)sSeqBody - (stlInt64)sSeqSlot;
    
    SMP_READ_COLUMN_ZERO_INSENS( sSeqBody, (void*)&sSeqRecord, &sRecordLen );
    STL_ASSERT( sRecordLen == STL_SIZEOF( smqRecord ) );

    sSeqRecord.mRestartValue = aSequenceValue;

    SMP_WRITE_COLUMN_ZERO_INSENS( sSeqBody, (void*)&sSeqRecord, sRecordLen );

    STL_WRITE_MOVE_INT16( sLogData, &sSlotBodyOffset, sLogOffset );
    STL_WRITE_MOVE_INT64( sLogData, &aSequenceValue, sLogOffset );
        
    STL_TRY( smxmWriteLog( aMiniTrans,
                           SMG_COMPONENT_SEQUENCE,
                           SMR_LOG_UPDATE_SEQUENCE_VALUE,
                           SMR_REDO_TARGET_PAGE,
                           (void*)sLogData,
                           sLogOffset,
                           aSequenceRid->mTbsId,
                           aSequenceRid->mPageId,
                           aSequenceRid->mOffset,
                           SMXM_LOGGING_REDO,
                           aEnv ) == STL_SUCCESS );

    if( sState == 1 )
    {
        STL_TRY( smxmCommit( aMiniTrans, aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smxmRollback( aMiniTrans, aEnv );
    }

    return STL_FAILURE;
}

stlStatus smqDeleteRecord( smlStatement * aStatement,
                           smlRid       * aSequenceRid,
                           smlEnv       * aEnv )
{
    smlRelationType     sRelType;
    smdTableModule    * sModule;
    stlBool             sVersionConflict;
    stlBool             sSkipped;

    sRelType = SME_GET_RELATION_TYPE( gSmqWarmupEntry->mSequenceMapHandle );
    sModule = smdGetTableModule( SML_GET_TABLE_TYPE(sRelType) );
    
    STL_TRY( sModule->mDelete( aStatement,
                               gSmqWarmupEntry->mSequenceMapHandle,
                               aSequenceRid,
                               SML_INFINITE_SCN,
                               0,      /* aValueIdx */
                               NULL,   /* aPrimaryKey */
                               &sVersionConflict,
                               &sSkipped,
                               aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
