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
#include <sms.h>
#include <smp.h>
#include <smdmphRowDef.h>
#include <smdmphDef.h>
#include <smdmphFxTable.h>
#include <smdmph.h>

/**
 * @file smxlFxTable.c
 * @brief Storage Manager Layer Memory Heap Table Fixed Table Routines
 */

/**
 * @addtogroup smdmphFxTable
 * @{
 */

knlFxColumnDesc gSmdmphOffsetTblHdrColumnDesc[] =
{
    {
        "TBS_ID",
        "tablespace identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmphFxOffsetTblHdr, mTbsId ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "PAGE_ID",
        "page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmphFxOffsetTblHdr, mPageId ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "SLOT_COUNT",
        "slot count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmphFxOffsetTblHdr, mSlotCount ),
        STL_SIZEOF( stlInt16 ),
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

stlStatus smdmphGetDumpObjectCallback( stlInt64   aTransId,
                                       void     * aStmt,
                                       stlChar  * aDumpObjName,
                                       void    ** aDumpObjHandle,
                                       knlEnv   * aEnv )
{
    stlBool    sExist = STL_TRUE;
    stlChar  * sToken1;
    stlChar  * sToken2;
    stlChar  * sToken3;
    stlInt32   sTokenCount;
    void     * sRelationHandle;
    smlRelationType   sRelType;

    *aDumpObjHandle = NULL;
    
    STL_TRY( knlTokenizeNextDumpOption( &aDumpObjName,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    if( sTokenCount == 1 )
    {
        STL_TRY( gKnlDumpNameHelper.mGetBaseTableHandle( aTransId,
                                                         ((smlStatement*) aStmt)->mScn,
                                                         NULL,
                                                         sToken1,
                                                         NULL,
                                                         &sRelationHandle,
                                                         &sExist,
                                                         KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }
    else
    {
        if( sTokenCount == 2 )
        {
            STL_TRY( gKnlDumpNameHelper.mGetBaseTableHandle( aTransId,
                                                             ((smlStatement*) aStmt)->mScn,
                                                             sToken1,
                                                             sToken2,
                                                             NULL,
                                                             &sRelationHandle,
                                                             &sExist,
                                                             KNL_ENV( aEnv ) )
                     == STL_SUCCESS );
        }
        else
        {
            sExist = STL_FALSE;
        }
    }
    
    STL_TRY_THROW( sExist == STL_TRUE, RAMP_FINISH );

    sRelType = SME_GET_RELATION_TYPE( sRelationHandle );

    STL_TRY_THROW( sRelType == SML_MEMORY_PERSISTENT_HEAP_ROW_TABLE, RAMP_FINISH );

    STL_TRY( smsValidateSegment( SME_GET_SEGMENT_HANDLE(sRelationHandle),
                                 SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    *aDumpObjHandle = SME_GET_SEGMENT_HANDLE( sRelationHandle );

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmphOpenOffsetTblHdrCallback( void   * aStmt,
                                          void   * aDumpObjHandle,
                                          void   * aPathCtrl,
                                          knlEnv * aEnv )
{
    smdmphOffsetTblHdrPathCtrl * sPathCtrl;
    
    sPathCtrl = (smdmphOffsetTblHdrPathCtrl*)aPathCtrl;
    
    STL_TRY( smsAllocSegIterator( (smlStatement*)aStmt,
                                  aDumpObjHandle,
                                  &sPathCtrl->mSegIterator,
                                  SML_ENV( aEnv ) ) == STL_SUCCESS );

    sPathCtrl->mCurPageId = SMP_NULL_PID;
    sPathCtrl->mSegHandle = aDumpObjHandle;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmphCloseOffsetTblHdrCallback( void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aEnv )
{
    smdmphOffsetTblHdrPathCtrl * sPathCtrl;
    
    sPathCtrl = (smdmphOffsetTblHdrPathCtrl*)aPathCtrl;
    STL_TRY( smsFreeSegIterator( sPathCtrl->mSegHandle,
                                 sPathCtrl->mSegIterator,
                                 SML_ENV( aEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmphBuildRecordOffsetTblHdrCallback( void              * aDumpObjHandle,
                                                 void              * aPathCtrl,
                                                 knlValueBlockList * aValueList,
                                                 stlInt32            aBlockIdx,
                                                 stlBool           * aTupleExist,
                                                 knlEnv            * aEnv )
{
    smdmphOffsetTblHdrPathCtrl * sPathCtrl;
    smlPid                       sPageId;
    smpHandle                    sPageHandle;
    smdmphFxOffsetTblHdr         sOffsetTblHdr;
    smlTbsId                     sTbsId;
    stlInt32                     sState = 0;
    
    *aTupleExist = STL_FALSE;
    sPathCtrl = (smdmphOffsetTblHdrPathCtrl*)aPathCtrl;

    sTbsId = smsGetTbsId( sPathCtrl->mSegHandle );

    if( sPathCtrl->mCurPageId == SMP_NULL_PID )
    {
        STL_TRY( smsGetFirstPage( sPathCtrl->mSegHandle,
                                  sPathCtrl->mSegIterator,
                                  &sPageId,
                                  SML_ENV( aEnv ) ) == STL_SUCCESS);
    }
    else
    {
        STL_TRY( smsGetNextPage( sPathCtrl->mSegHandle,
                                 sPathCtrl->mSegIterator,
                                 &sPageId,
                                 SML_ENV( aEnv ) ) == STL_SUCCESS);
    }
    
    if( sPageId != SMP_NULL_PID )
    {
        STL_TRY( smpAcquire( NULL,
                             sTbsId,
                             sPageId,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             SML_ENV( aEnv ) ) == STL_SUCCESS );
        sState = 1;

        sOffsetTblHdr.mTbsId = sTbsId;
        sOffsetTblHdr.mPageId = sPageId;
        sOffsetTblHdr.mSlotCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sPageHandle );

        sState = 0;
        STL_TRY( smpRelease( &sPageHandle,
                             SML_ENV( aEnv ) ) == STL_SUCCESS );
        
        STL_TRY( knlBuildFxRecord( gSmdmphOffsetTblHdrColumnDesc,
                                   (void*)&sOffsetTblHdr,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );
    
        *aTupleExist = STL_TRUE;
    }
    
    sPathCtrl->mCurPageId = sPageId;
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, SML_ENV( aEnv ) );
    }

    return STL_FAILURE;
}

knlFxTableDesc gSmdmphOffsetTblHdrTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smdmphGetDumpObjectCallback,
    smdmphOpenOffsetTblHdrCallback,
    smdmphCloseOffsetTblHdrCallback,
    smdmphBuildRecordOffsetTblHdrCallback,
    STL_SIZEOF( smdmphOffsetTblHdrPathCtrl ),
    "D$MEMORY_HEAP_OFFSET_TABLE_HEADER",
    "memory heap offset table header information",
    gSmdmphOffsetTblHdrColumnDesc
};


knlFxColumnDesc gSmdmphRowColumnDesc[] =
{
    {
        "TABLESPACE_ID",
        "tablespace id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmphFxRow, mTbsId ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "CURRENT_PAGE_ID",
        "current page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmphFxRow, mCurPageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "CURRENT_SLOT_SEQ",
        "current slot sequence",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmphFxRow, mCurSlotSeq ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_DELETED",
        "is deleted",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmphFxRow, mIsDeleted ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_MASTER",
        "is master row",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmphFxRow, mIsMaster ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_CONT_COL",
        "is continuous column",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmphFxRow, mIsContCol ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_LOCKED",
        "is locked",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmphFxRow, mIsLocked ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "TCN",
        "transaction change number",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmphFxRow, mTcn ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_RTS_BOUND",
        "is rts bound",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmphFxRow, mIsRtsBound ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "RTS_SEQ",
        "rts sequence",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmphFxRow, mRtsSeq ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "SCN",
        "view/commit scn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smdmphFxRow, mScn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "TRANS_ID",
        "Transaction ID",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smdmphFxRow, mTransId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "ROLLBACK_PID",
        "page id of rollback rid",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmphFxRow, mRollbackPid ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "ROLLBACK_OFFSET",
        "offset of rollback rid",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmphFxRow, mRollbackOffset ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "COL_COUNT",
        "column count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmphFxRow, mColCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "PAD_SPACE",
        "paddind space",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmphFxRow, mPadSpace ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "HAS_NEXT_LINK",
        "has next link",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmphFxRow, mHasNextLink ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "NEXT_LINK_PID",
        "page id of next link rid",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmphFxRow, mNextPid ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "NEXT_LINK_OFFSET",
        "offset of rollback rid",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmphFxRow, mNextOffset ),
        STL_SIZEOF( stlInt16 ),
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


stlStatus smdmphOpenRowCallback( void   * aStmt,
                                 void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv )
{
    smdmphFxRowIterator * sIterator;

    sIterator = (smdmphFxRowIterator*)aPathCtrl;

    STL_TRY( smsAllocSegIterator( (smlStatement*)aStmt,
                                  aDumpObjHandle,
                                  &sIterator->mSegIterator,
                                  (smlEnv*)aEnv ) == STL_SUCCESS );

    sIterator->mTbsId = smsGetTbsId( aDumpObjHandle );
    sIterator->mCurPid = SMP_NULL_PID;
    sIterator->mCurSlotSeq = -1;
    sIterator->mTotalSlotCount = -1;
    sIterator->mPageHandle.mFrame = sIterator->mPageBuf;
    sIterator->mPageHandle.mPch = NULL;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmphCloseRowCallback( void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv )
{
     smdmphFxRowIterator * sIterator;
    
    sIterator = (smdmphFxRowIterator*)aPathCtrl;
    STL_TRY( smsFreeSegIterator( aDumpObjHandle,
                                 sIterator->mSegIterator,
                                 SML_ENV( aEnv ) ) == STL_SUCCESS );
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmphBuildRecordRowCallback( void              * aDumpObjHandle,
                                        void              * aPathCtrl,
                                        knlValueBlockList * aValueList,
                                        stlInt32            aBlockIdx,
                                        stlBool           * aTupleExist,
                                        knlEnv            * aEnv )
{
    smdmphFxRowIterator * sIterator;
    smdmphFxRow         * sFxRow;
    smpHandle             sPageHandle;
    stlInt16              sState = 0;
    stlChar             * sCurRow;
    stlUInt8              sRtsSeq;

    sIterator = (smdmphFxRowIterator*)aPathCtrl;
    sFxRow = &sIterator->mRowRecord;

    while( STL_TRUE )
    {
        if( sIterator->mCurSlotSeq + 1 >= sIterator->mTotalSlotCount )
        {
            if( sIterator->mCurPid == SMP_NULL_PID )
            {
                STL_TRY( smsGetFirstPage( aDumpObjHandle,
                                          sIterator->mSegIterator,
                                          &sIterator->mCurPid,
                                          (smlEnv*)aEnv ) == STL_SUCCESS );
            }
            else
            {
                STL_TRY( smsGetNextPage( aDumpObjHandle,
                                         sIterator->mSegIterator,
                                         &sIterator->mCurPid,
                                         (smlEnv*)aEnv ) == STL_SUCCESS );
            }
        
            if( sIterator->mCurPid == SMP_NULL_PID )
            {
                *aTupleExist = STL_FALSE;
                STL_THROW( RAMP_TERMINATE );
            }

            STL_TRY( smpAcquire( NULL,
                                 smsGetTbsId(aDumpObjHandle),
                                 sIterator->mCurPid,
                                 KNL_LATCH_MODE_SHARED,
                                 &sPageHandle,
                                 (smlEnv*)aEnv ) == STL_SUCCESS );
            sState = 1;

            stlMemcpy( sIterator->mPageBuf, SMP_FRAME(&sPageHandle), SMP_PAGE_SIZE );

            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );

            sIterator->mCurSlotSeq = -1;
            sIterator->mTotalSlotCount = SMP_GET_PAGE_ROW_ITEM_COUNT( &sIterator->mPageHandle );

            continue;
        }

        while( 1 )
        {
            sIterator->mCurSlotSeq++;

            if( smpIsNthOffsetSlotFree( &sIterator->mPageHandle,
                                        sIterator->mCurSlotSeq ) == STL_FALSE )
            {
                break;
            }
            
            if( sIterator->mCurSlotSeq >= sIterator->mTotalSlotCount )
            {
                break;
            }
        }

        if( sIterator->mCurSlotSeq >= sIterator->mTotalSlotCount )
        {
            continue;
        }

        sCurRow = smpGetNthRowItem( &sIterator->mPageHandle, sIterator->mCurSlotSeq );

        sFxRow->mTbsId = sIterator->mTbsId;
        sFxRow->mCurPageId = sIterator->mCurPid;
        sFxRow->mCurSlotSeq = sIterator->mCurSlotSeq;
        sFxRow->mIsDeleted = (stlInt16)SMDMPH_IS_DELETED( sCurRow );
        sFxRow->mIsMaster = (stlInt16)SMDMPH_IS_MASTER_ROW( sCurRow );
        sFxRow->mIsContCol = (stlInt16)SMDMPH_IS_CONT_COL( sCurRow );
        sFxRow->mIsLocked = (stlInt16)SMDMPH_IS_LOCKED( sCurRow );
        SMDMPH_GET_TCN( sCurRow, &sFxRow->mTcn );
        sFxRow->mIsRtsBound = (stlInt16)SMDMPH_IS_RTS_BOUND( sCurRow );
        if( (stlBool)sFxRow->mIsRtsBound == STL_TRUE )
        {
            SMDMPH_GET_RTSSEQ( sCurRow, &sRtsSeq );
            sFxRow->mRtsSeq = (stlInt16)sRtsSeq;
            sFxRow->mScn = SML_INFINITE_SCN;
        }
        else
        {
            sFxRow->mRtsSeq = (stlInt16)SMP_RTS_SEQ_NULL;
            SMDMPH_GET_SCN( sCurRow, &sFxRow->mScn );
        }
        SMDMPH_GET_TRANS_ID( sCurRow, &(sFxRow->mTransId) );
        SMDMPH_GET_ROLLBACK_PID( sCurRow, (smlPid*)&sFxRow->mRollbackPid );
        SMDMPH_GET_ROLLBACK_OFFSET( sCurRow, (smlPid*)&sFxRow->mRollbackOffset );
        SMDMPH_GET_COLUMN_CNT( sCurRow, &sFxRow->mColCount );
        SMDMPH_GET_PAD_SPACE( sCurRow, &sFxRow->mPadSpace );
        sFxRow->mHasNextLink = (stlInt16)SMDMPH_HAS_NEXT_LINK( sCurRow );
        if( (stlBool)sFxRow->mHasNextLink == STL_TRUE )
        {
            SMDMPH_GET_LINK_PID( sCurRow, (smlPid*)&sFxRow->mNextPid );
            SMDMPH_GET_LINK_OFFSET( sCurRow, (smlPid*)&sFxRow->mNextOffset );
        }
        else
        {
            sFxRow->mNextPid = SMP_NULL_PID;
            sFxRow->mNextOffset = 0;
        }

        STL_TRY( knlBuildFxRecord( gSmdmphRowColumnDesc,
                                   sFxRow,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );

        *aTupleExist = STL_TRUE;
        break;
    } /* while */

    STL_RAMP( RAMP_TERMINATE );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, (smlEnv*)aEnv );
    }

    return STL_FAILURE;
}


knlFxTableDesc gSmdmphRowTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smdmphGetDumpObjectCallback,
    smdmphOpenRowCallback,
    smdmphCloseRowCallback,
    smdmphBuildRecordRowCallback,
    STL_SIZEOF( smdmphFxRowIterator ),
    "D$MEMORY_HEAP_ROW",
    "memory heap table row information",
    gSmdmphRowColumnDesc
};


knlFxColumnDesc gSmdmphLogicalHdrColumnDesc[] =
{
    {
        "PAGE_ID",
        "page id",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmphFxLogicalHdr, mCurPid ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "SIMPLE_PAGE",
        "simple page",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smdmphFxLogicalHdr, mSimplePage ),
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


stlStatus smdmphOpenLogicalHdrCallback( void   * aStmt,
                                        void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aEnv )
{
    smdmphFxLogicalHdrIterator * sIterator;

    sIterator = (smdmphFxLogicalHdrIterator*)aPathCtrl;
    
    STL_TRY( smsAllocSegIterator( (smlStatement*)aStmt,
                                  aDumpObjHandle,
                                  &sIterator->mSegIterator,
                                  (smlEnv*)aEnv ) == STL_SUCCESS );

    sIterator->mCurPid = SMP_NULL_PID;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmphCloseLogicalHdrCallback( void   * aDumpObjHandle,
                                         void   * aPathCtrl,
                                         knlEnv * aEnv )
{
    return STL_SUCCESS;
}


stlStatus smdmphBuildRecordLogicalHdrCallback( void              * aDumpObjHandle,
                                               void              * aPathCtrl,
                                               knlValueBlockList * aValueList,
                                               stlInt32            aBlockIdx,
                                               stlBool           * aTupleExist,
                                               knlEnv            * aEnv )
{
    smdmphFxLogicalHdrIterator * sIterator;
    smdmphFxLogicalHdr           sFxLogicalHdr;
    smpHandle                 sPageHandle;
    stlInt16                  sState = 0;
    smdmphLogicalHdr        * sLogicalHdr;

    sIterator = (smdmphFxLogicalHdrIterator*)aPathCtrl;
    *aTupleExist = STL_TRUE;

    if( sIterator->mCurPid == SMP_NULL_PID )
    {
        STL_TRY( smsGetFirstPage( aDumpObjHandle,
                                  sIterator->mSegIterator,
                                  &sIterator->mCurPid,
                                  (smlEnv*)aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( smsGetNextPage( aDumpObjHandle,
                                 sIterator->mSegIterator,
                                 &sIterator->mCurPid,
                                 (smlEnv*)aEnv ) == STL_SUCCESS );
    }

    if( sIterator->mCurPid == SMP_NULL_PID )
    {
        *aTupleExist = STL_FALSE;
    }
    else
    {
        STL_TRY( smpAcquire( NULL,
                             smsGetTbsId(aDumpObjHandle),
                             sIterator->mCurPid,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             (smlEnv*)aEnv ) == STL_SUCCESS );
        sState = 1;

        sLogicalHdr = SMDMPH_GET_LOGICAL_HEADER( &sPageHandle );

        sFxLogicalHdr.mCurPid = sIterator->mCurPid;
        sFxLogicalHdr.mSimplePage = sLogicalHdr->mSimplePage;

        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );

        STL_TRY( knlBuildFxRecord( gSmdmphLogicalHdrColumnDesc,
                                   &sFxLogicalHdr,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, (smlEnv*)aEnv );
    }

    return STL_FAILURE;
}

knlFxTableDesc gSmdmphLogicalHdrTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smdmphGetDumpObjectCallback,
    smdmphOpenLogicalHdrCallback,
    smdmphCloseLogicalHdrCallback,
    smdmphBuildRecordLogicalHdrCallback,
    STL_SIZEOF(smdmphFxLogicalHdrIterator),
    "D$MEMORY_HEAP_LOGICAL_HEADER",
    "logical header information of heap table",
    gSmdmphLogicalHdrColumnDesc
};



/** @} */
