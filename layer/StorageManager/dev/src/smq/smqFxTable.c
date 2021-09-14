/*******************************************************************************
 * smqFxTable.c
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
#include <smp.h>
#include <sma.h>
#include <smxm.h>
#include <smqDef.h>
#include <smqFxTable.h>
#include <smlRecord.h>

/**
 * @file smqFxTable.c
 * @brief Storage Manager Layer Sequence Fixed Table Internal Routines
 */

extern smqWarmupEntry * gSmqWarmupEntry;

/**
 * @addtogroup smqFxTable
 * @{
 */

knlFxColumnDesc gSmqSequenceColumnDesc[] =
{
    {
        "PHYSICAL_ID",
        "sequence physical identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smqFxSeqTable, mSeqId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "START_WITH",
        "start with",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smqFxSeqTable, mStartWith ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "INCREMENT_BY",
        "increment by",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smqFxSeqTable, mIncrementBy ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "MAXVALUE",
        "maximum value",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smqFxSeqTable, mMaxValue ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "MINVALUE",
        "minimum value",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smqFxSeqTable, mMinValue ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "CACHE_SIZE",
        "size of cache",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smqFxSeqTable, mCacheSize ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "NEXT_VALUE",
        "next value",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smqFxSeqTable, mNxtValue ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "CURR_VALUE",
        "current value",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smqFxSeqTable, mCurValue ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "RESTART_VALUE",
        "restart value",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smqFxSeqTable, mRestartValue ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "CYCLE",
        "allow cycle",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smqFxSeqTable, mCycle ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "CACHE_COUNT",
        "current cache count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smqFxSeqTable, mCacheCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        STL_FALSE  /* nullable */
    }
};

stlStatus smqOpenSequenceCallback( void   * aStmt,
                                   void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv )
{
    smqSequencePathCtrl * sPathCtrl;
    smlIteratorProperty   sProperty;
    stlInt32              i;
    stlInt32              sState = 0;
    
    sPathCtrl = (smqSequencePathCtrl*)aPathCtrl;

    SML_INIT_ITERATOR_PROP( sProperty );
    
    STL_TRY( smaAllocStatement(  SML_SESS_ENV(aEnv)->mTransId,
                                 SML_INVALID_TRANSID,
                                 NULL, /* aRelationHandle */
                                 SML_STMT_ATTR_READONLY,
                                 SML_LOCK_TIMEOUT_INFINITE,
                                 STL_FALSE, /* aNeedSnapshotIterator */
                                 &sPathCtrl->mStatement,
                                 SML_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 1;

    STL_TRY( smaAllocIterator( sPathCtrl->mStatement,
                               gSmqWarmupEntry->mSequenceMapHandle,
                               SML_TRM_COMMITTED,
                               SML_SRM_SNAPSHOT, 
                               &sProperty,
                               SML_SCAN_FORWARD,
                               (void**)&sPathCtrl->mIterator,
                               SML_ENV( aEnv ) ) == STL_SUCCESS );
    sState = 2;
    
    KNL_INIT_REGION_MARK(&sPathCtrl->mMemMark);
    STL_TRY( knlMarkRegionMem( &((smlEnv*)aEnv)->mOperationHeapMem,
                               &sPathCtrl->mMemMark,
                               aEnv ) == STL_SUCCESS );
    sState = 3;

    for ( i = 0; i < SMQ_COLUMN_COUNT; i++ )
    {
        STL_TRY( knlInitBlockNoBuffer( &sPathCtrl->mFetchCols[i],
                                       STL_LAYER_STORAGE_MANAGER,
                                       1,
                                       DTL_TYPE_BINARY,
                                       &((smlEnv*)aEnv)->mOperationHeapMem,
                                       aEnv ) == STL_SUCCESS );
    }

    STL_TRY( smlInitRowBlock( & sPathCtrl->mRowBlock,
                              1,
                              &((smlEnv*)aEnv)->mOperationHeapMem,
                              SML_ENV( aEnv ) )
             == STL_SUCCESS );
 
    SMQ_BUILD_COLUMN_LIST( &sPathCtrl->mFetchCols[0],
                           &sPathCtrl->mSeqRecord,
                           sPathCtrl->mDummy );

    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 3:
            (void) knlFreeUnmarkedRegionMem( &(SML_ENV(aEnv)->mOperationHeapMem),
                                             &sPathCtrl->mMemMark,
                                             STL_FALSE, /* aFreeChunk */
                                             aEnv );
        case 2:
            (void) smaFreeIterator( sPathCtrl->mIterator, SML_ENV(aEnv) );
        case 1:
            (void) smaFreeStatement( sPathCtrl->mStatement, SML_ENV(aEnv) );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smqCloseSequenceCallback( void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv )
{
    smqSequencePathCtrl * sPathCtrl;
    
    sPathCtrl = (smqSequencePathCtrl*)aPathCtrl;
    
    STL_TRY( knlFreeUnmarkedRegionMem( &((smlEnv*)aEnv)->mOperationHeapMem,
                                       &sPathCtrl->mMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       aEnv ) == STL_SUCCESS );
    STL_TRY( smaFreeIterator( sPathCtrl->mIterator,
                              SML_ENV( aEnv ) ) == STL_SUCCESS );
    STL_TRY( smaFreeStatement( sPathCtrl->mStatement,
                               SML_ENV( aEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smqBuildRecordSequenceCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aTupleExist,
                                          knlEnv            * aEnv )
{
    smqSequencePathCtrl * sPathCtrl;
    smpHandle             sPageHandle;
    smqFxSeqTable         sFxSeqRecord;
    smqCache            * sCache;
    void               ** sVolatileArea;
    smlRid                sSeqRid;
    smlFetchInfo          sFetchInfo;
    
    sPathCtrl = (smqSequencePathCtrl*)aPathCtrl;

    *aTupleExist = STL_FALSE;
    
    stlMemset( & sFetchInfo, 0x00, STL_SIZEOF( smlFetchInfo ) );

    SML_SET_FETCH_INFO_FOR_TABLE( & sFetchInfo,             /* Fetch Info */
                                  NULL,                     /* physical filter */
                                  NULL,                     /* logical filter */
                                  sPathCtrl->mFetchCols[0], /* read column list */
                                  NULL,                     /* rowid column list */
                                  &sPathCtrl->mRowBlock,    /* row block */
                                  0,                        /* skip count */
                                  1,                        /* fetch count */
                                  STL_FALSE,                /* group key fetch */
                                  NULL                      /* filter evaluate info */
        );

    STL_TRY( sPathCtrl->mIterator->mFetchNext( sPathCtrl->mIterator,
                                               &sFetchInfo,
                                               SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    sSeqRid = SML_GET_RID_VALUE( & sPathCtrl->mRowBlock, 0 );

    if( sFetchInfo.mIsEndOfScan == STL_FALSE )
    {
        STL_TRY( smpAcquire( NULL,
                             sSeqRid.mTbsId,
                             sSeqRid.mPageId,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             SML_ENV( aEnv ) ) == STL_SUCCESS );

        sVolatileArea = smpGetVolatileArea( &sPageHandle );
        sCache = (void*)sVolatileArea[0];

        STL_ASSERT( SML_IS_EQUAL_RID( sSeqRid, sCache->mRid ) == STL_TRUE );

        sFxSeqRecord.mStartWith = sCache->mRecord.mStartWith;
        sFxSeqRecord.mIncrementBy = sCache->mRecord.mIncrementBy;
        sFxSeqRecord.mMaxValue = sCache->mRecord.mMaxValue;
        sFxSeqRecord.mMinValue = sCache->mRecord.mMinValue;
        sFxSeqRecord.mCacheSize = sCache->mRecord.mCacheSize;
        sFxSeqRecord.mRestartValue = sCache->mRecord.mRestartValue;
        sFxSeqRecord.mCycle = sCache->mRecord.mCycle;
        sFxSeqRecord.mCacheCount = sCache->mCacheCount;
        sFxSeqRecord.mNxtValue = sCache->mNxtValue;
        sFxSeqRecord.mCurValue = sCache->mCurValue;

        SMS_MAKE_SEGMENT_ID( &sFxSeqRecord.mSeqId, &sSeqRid );

        STL_TRY( smpRelease( &sPageHandle,
                             SML_ENV( aEnv ) ) == STL_SUCCESS );
        
        STL_TRY( knlBuildFxRecord( gSmqSequenceColumnDesc,
                                   (void*)&sFxSeqRecord,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );

        *aTupleExist = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmqSequenceTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    NULL,
    smqOpenSequenceCallback,
    smqCloseSequenceCallback,
    smqBuildRecordSequenceCallback,
    STL_SIZEOF( smqSequencePathCtrl ),
    "X$SEQUENCE",
    "sequence information",
    gSmqSequenceColumnDesc
};

/** @} */
