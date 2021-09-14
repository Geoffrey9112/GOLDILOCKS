/*******************************************************************************
 * smxlFxTable.c
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
#include <smxl.h>
#include <sms.h>
#include <smxlDef.h>
#include <smxlFxTable.h>

/**
 * @file smxlFxTable.c
 * @brief Storage Manager Layer Local Transaction Manager Fixed Table Internal Routines
 */

/**
 * @addtogroup smxlFxTable
 * @{
 */

knlFxColumnDesc gSmxlTransColumnDesc[] =
{
    {
        "PHYSICAL_TRANS_ID",
        "physical transaction identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smxlFxTrans, mSmxlTransId ),
        STL_SIZEOF(smxlTransId),
        STL_FALSE  /* nullable */
    },
    {
        "LOGICAL_TRANS_ID",
        "logical transaction identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smxlFxTrans, mSmlTransId ),
        STL_SIZEOF(smlTransId),
        STL_FALSE  /* nullable */
    },
    {
        "SLOT_ID",
        "transaction slot identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smxlFxTrans, mSlotId ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "STATE",
        "transaction state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smxlFxTrans, mState ),
        10,
        STL_FALSE  /* nullable */
    },
    {
        "IS_GLOBAL",
        "global transaction",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smxlFxTrans, mGlobal ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "ATTRIBUTE",
        "transaction attribute",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smxlFxTrans, mAttr ),
        50,
        STL_FALSE  /* nullable */
    },
    {
        "ISOLATION_LEVEL",
        "transaction isolation level",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smxlFxTrans, mIsolationLevel ),
        20,
        STL_FALSE  /* nullable */
    },
    {
        "VIEW_SCN",
        "transaction view scn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smxlFxTrans, mTransViewScn ),
        STL_SIZEOF( smlScn ),
        STL_FALSE  /* nullable */
    },
    {
        "COMMIT_SCN",
        "transaction commit scn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smxlFxTrans, mCommitScn ),
        STL_SIZEOF( smlScn ),
        STL_FALSE  /* nullable */
    },
    {
        "TCN",
        "transaction change number",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smxlFxTrans, mTcn ),
        STL_SIZEOF( smlTcn ),
        STL_FALSE  /* nullable */
    },
    {
        "BEGIN_LSN",
        "transaction begin lsn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smxlFxTrans, mBeginLsn ),
        STL_SIZEOF( smrLsn ),
        STL_FALSE  /* nullable */
    },
    {
        "USED_UNDO_PAGE_COUNT",
        "used undo page count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smxlFxTrans, mUsedUndoPageCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "UNDO_SEGMENT_ID",
        "undo segment identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smxlFxTrans, mUndoSegmentId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "SEQ",
        "transaction sequence number",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smxlFxTrans, mTransSeq ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "BEGIN_TIME",
        "transaction begin time",
        KNL_FXTABLE_DATATYPE_TIMESTAMP,
        STL_OFFSETOF( smxlFxTrans, mBeginTime ),
        STL_SIZEOF( stlTime ),
        STL_FALSE  /* nullable */
    },
    {
        "PROPAGATE_LOG",
        "propagate log for CDC",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smxlFxTrans, mPropagateLog ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "REPREPARABLE",
        "re-preparable transaction",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smxlFxTrans, mRepreparable ),
        STL_SIZEOF( stlBool ),
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

stlStatus smxlOpenTransCallback( void   * aStmt,
                                 void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv )
{
    smxlTransId sTransId = 0;
    
    stlMemcpy( aPathCtrl, &sTransId, STL_SIZEOF(smxlTransId) );
    
    return STL_SUCCESS;
}

stlStatus smxlCloseTransCallback( void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smxlBuildRecordTransCallback( void              * aDumpObjHandle,
                                        void              * aPathCtrl,
                                        knlValueBlockList * aValueList,
                                        stlInt32            aBlockIdx,
                                        stlBool           * aTupleExist,
                                        knlEnv            * aEnv )
{
    smxlTransId sTransId;
    stlUInt32   i;
    smxlFxTrans sFxTrans;
    
    stlMemcpy( &sTransId, aPathCtrl, STL_SIZEOF(smxlTransId) );
        
    for( i = SMXL_TRANS_SLOT_ID( sTransId ); i < SMXL_TRANS_TABLE_SIZE; i++ )
    {
        if( SMXL_TRANS_TABLE(i).mState == SMXL_STATE_IDLE )
        {
            continue;
        }

        stlMemset( &sFxTrans, 0x00, STL_SIZEOF( smxlFxTrans ) );
        sFxTrans.mSmxlTransId = SMXL_TRANS_TABLE(i).mTransId;
        sFxTrans.mSmlTransId = SMXL_TO_SML_TRANS_ID( SMXL_TRANS_TABLE(i).mTransId,
                                                     SMXL_TRANS_TABLE(i).mTransSeq );
        sFxTrans.mSlotId = SMXL_TRANS_SLOT_ID( SMXL_TRANS_TABLE(i).mTransId );
        sFxTrans.mTransViewScn = SMXL_TRANS_TABLE(i).mTransViewScn;
        sFxTrans.mCommitScn = SMXL_TRANS_TABLE(i).mCommitScn;
        sFxTrans.mTcn = SMXL_TRANS_TABLE(i).mTcn;
        sFxTrans.mBeginTime = SMXL_TRANS_TABLE(i).mBeginTime;
        sFxTrans.mPropagateLog = SMXL_TRANS_TABLE(i).mPropagateLog;
        sFxTrans.mRepreparable = SMXL_TRANS_TABLE(i).mRepreparable;
        sFxTrans.mGlobal = SMXL_TRANS_TABLE(i).mIsGlobal;
        sFxTrans.mTransSeq = SMXL_TRANS_TABLE(i).mTransSeq;
        sFxTrans.mBeginLsn = SMXL_TRANS_TABLE(i).mBeginLsn;
        sFxTrans.mUsedUndoPageCount = SMXL_TRANS_TABLE(i).mUsedUndoPageCount;

        stlMemcpy( &sFxTrans.mUndoSegmentId,
                   &SMXL_TRANS_TABLE(i).mUndoSegRid,
                   STL_SIZEOF(smlRid) );
        
        switch( SMXL_TRANS_TABLE(i).mIsolationLevel )
        {
            case SML_TIL_READ_UNCOMMITTED:
                stlStrncpy( sFxTrans.mIsolationLevel, "READ UNCOMMITTED", 21 );
                break;
            case SML_TIL_READ_COMMITTED:
                stlStrncpy( sFxTrans.mIsolationLevel, "READ COMMITTED", 21 );
                break;
            case SML_TIL_SERIALIZABLE:
                stlStrncpy( sFxTrans.mIsolationLevel, "SERIALIZABLE", 21 );
                break;
            default:
                STL_ASSERT( 0 );
                break;
        }
        
        switch( SMXL_TRANS_TABLE(i).mState )
        {
            case SMXL_STATE_ACTIVE:
                stlStrncpy( sFxTrans.mState, "ACTIVE", 11 );
                break;
            case SMXL_STATE_BLOCK:
                stlStrncpy( sFxTrans.mState, "BLOCK", 11 );
                break;
            case SMXL_STATE_PREPARE:
                stlStrncpy( sFxTrans.mState, "PREPARE", 11 );
                break;
            case SMXL_STATE_COMMIT:
                stlStrncpy( sFxTrans.mState, "COMMIT", 11 );
                break;
            case SMXL_STATE_ROLLBACK:
                stlStrncpy( sFxTrans.mState, "ROLLBACK", 11 );
                break;
            case SMXL_STATE_IDLE:
                stlStrncpy( sFxTrans.mState, "IDLE", 11 );
                break;
            case SMXL_STATE_PRECOMMIT:
                stlStrncpy( sFxTrans.mState, "PRECOMMIT", 11 );
                break;
            default:
                STL_ASSERT( 0 );
                break;
        }

        if( SMXL_TRANS_TABLE(i).mAttr & SMXL_ATTR_UPDATABLE )
        {
            stlStrncat( sFxTrans.mAttr, "UPDATABLE", 51);

            if( SMXL_TRANS_TABLE(i).mAttr & SMXL_ATTR_LOCKABLE )
            {
                stlStrncat( sFxTrans.mAttr, " | LOCKABLE", 51);
            }
        }
        else
        {
            if( SMXL_TRANS_TABLE(i).mAttr & SMXL_ATTR_LOCKABLE )
            {
                stlStrncat( sFxTrans.mAttr, "LOCKABLE", 51);
            }
            else
            {
                stlStrncat( sFxTrans.mAttr, "READ_ONLY", 51);
            }
        }

        STL_TRY( knlBuildFxRecord( gSmxlTransColumnDesc,
                                   &sFxTrans,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );

        break;
    }
    
    if ( i < SMXL_TRANS_TABLE_SIZE )
    {
        sTransId = i + 1;
        
        stlMemcpy( aPathCtrl, &sTransId, STL_SIZEOF(smxlTransId) );
        *aTupleExist = STL_TRUE;
    }
    else
    {
        *aTupleExist = STL_FALSE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmxlTransTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    NULL,
    smxlOpenTransCallback,
    smxlCloseTransCallback,
    smxlBuildRecordTransCallback,
    STL_SIZEOF( smxlTransId ),
    "X$TRANSACTION",
    "transaction information",
    gSmxlTransColumnDesc
};

knlFxColumnDesc gSmxlUndoSegColumnDesc[] =
{
    {
        "SEGMENT_ID",
        "logical segment id",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smxlFxUndoSeg, mSegmentId ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "PHYSICAL_ID",
        "physical segment id",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smxlFxUndoSeg, mPhysicalId ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "ALLOC_PAGE_COUNT",
        "allocated page count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smxlFxUndoSeg, mAllocPageCount ),
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

stlStatus smxlOpenUndoSegCallback( void   * aStmt,
                                   void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv )
{
    *((stlInt64*)aPathCtrl) = 0;
    
    return STL_SUCCESS;
}

stlStatus smxlCloseUndoSegCallback( void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smxlBuildRecordUndoSegCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aTupleExist,
                                          knlEnv            * aEnv )
{
    smxlFxUndoSeg      sFxUndoSeg;
    stlInt64           sSegmentId;
    smxlUndoRelCache * sUndoRelCache;
    smxlUndoRelEntry * sUndoEntry;

    *aTupleExist = STL_FALSE;
    sSegmentId = *((stlInt64*)aPathCtrl);

    STL_TRY_THROW( sSegmentId < SMXL_UNDO_REL_COUNT, RAMP_FINISH );
    
    sUndoEntry = &(SMXL_UNDO_REL_ENTRY( sSegmentId ));
    sUndoRelCache = (smxlUndoRelCache*)(sUndoEntry->mRelHandle);

    sFxUndoSeg.mSegmentId = sSegmentId;
    sFxUndoSeg.mPhysicalId = smsGetSegmentId( sUndoRelCache->mSegHandle );
    sFxUndoSeg.mAllocPageCount = smsGetAllocPageCount( sUndoRelCache->mSegHandle );

    STL_TRY( knlBuildFxRecord( gSmxlUndoSegColumnDesc,
                               &sFxUndoSeg,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    *((stlInt64*)aPathCtrl) = sSegmentId + 1;
    *aTupleExist = STL_TRUE;

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmxlUndoSegTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    NULL,
    smxlOpenUndoSegCallback,
    smxlCloseUndoSegCallback,
    smxlBuildRecordUndoSegCallback,
    STL_SIZEOF( stlInt64 ),
    "X$UNDO_SEGMENT",
    "undo segment information",
    gSmxlUndoSegColumnDesc
};

/** @} */
