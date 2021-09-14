/*******************************************************************************
 * smxlFxTable.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmpfFxTable.c 13901 2014-11-13 01:31:33Z mkkim $
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
#include <smdmpfRowDef.h>
#include <smdmpfDef.h>
#include <smdmpfFxTable.h>
#include <smdmpf.h>

/**
 * @file smxlFxTable.c
 * @brief Storage Manager Layer Memory Heap Table Fixed Table Routines
 */

/**
 * @addtogroup smdmpfFxTable
 * @{
 */

knlFxColumnDesc gSmdmpfOffsetTblHdrColumnDesc[] =
{
    {
        "TBS_ID",
        "tablespace identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmpfFxOffsetTblHdr, mTbsId ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "PAGE_ID",
        "page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmpfFxOffsetTblHdr, mPageId ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "SLOT_COUNT",
        "slot count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmpfFxOffsetTblHdr, mSlotCount ),
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

stlStatus smdmpfGetDumpObjectCallback( stlInt64   aTransId,
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

    STL_TRY( smsValidateSegment( SME_GET_SEGMENT_HANDLE(sRelationHandle),
                                 SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    *aDumpObjHandle = SME_GET_SEGMENT_HANDLE( sRelationHandle );

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmpfOpenOffsetTblHdrCallback( void   * aStmt,
                                          void   * aDumpObjHandle,
                                          void   * aPathCtrl,
                                          knlEnv * aEnv )
{
    smdmpfOffsetTblHdrPathCtrl * sPathCtrl;
    
    sPathCtrl = (smdmpfOffsetTblHdrPathCtrl*)aPathCtrl;
    
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

stlStatus smdmpfCloseOffsetTblHdrCallback( void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aEnv )
{
    smdmpfOffsetTblHdrPathCtrl * sPathCtrl;
    
    sPathCtrl = (smdmpfOffsetTblHdrPathCtrl*)aPathCtrl;
    STL_TRY( smsFreeSegIterator( sPathCtrl->mSegHandle,
                                 sPathCtrl->mSegIterator,
                                 SML_ENV( aEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmpfBuildRecordOffsetTblHdrCallback( void              * aDumpObjHandle,
                                                 void              * aPathCtrl,
                                                 knlValueBlockList * aValueList,
                                                 stlInt32            aBlockIdx,
                                                 stlBool           * aTupleExist,
                                                 knlEnv            * aEnv )
{
    smdmpfOffsetTblHdrPathCtrl * sPathCtrl;
    smlPid                       sPageId;
    smpHandle                    sPageHandle;
    smdmpfFxOffsetTblHdr         sOffsetTblHdr;
    smlTbsId                     sTbsId;
    stlInt32                     sState = 0;
    
    *aTupleExist = STL_FALSE;
    sPathCtrl = (smdmpfOffsetTblHdrPathCtrl*)aPathCtrl;

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
        
        STL_TRY( knlBuildFxRecord( gSmdmpfOffsetTblHdrColumnDesc,
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

knlFxTableDesc gSmdmpfOffsetTblHdrTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smdmpfGetDumpObjectCallback,
    smdmpfOpenOffsetTblHdrCallback,
    smdmpfCloseOffsetTblHdrCallback,
    smdmpfBuildRecordOffsetTblHdrCallback,
    STL_SIZEOF( smdmpfOffsetTblHdrPathCtrl ),
    "D$MEMORY_HEAP_OFFSET_TABLE_HEADER",
    "memory heap offset table header information",
    gSmdmpfOffsetTblHdrColumnDesc
};


knlFxColumnDesc gSmdmpfRowColumnDesc[] =
{
    {
        "TABLESPACE_ID",
        "tablespace id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmpfFxRow, mTbsId ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "CURRENT_PAGE_ID",
        "current page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmpfFxRow, mCurPageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "CURRENT_SLOT_SEQ",
        "current slot sequence",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmpfFxRow, mCurSlotSeq ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_DELETED",
        "is deleted",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmpfFxRow, mIsDeleted ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_MASTER",
        "is master row",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmpfFxRow, mIsMaster ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_CONT_COL",
        "is continuous column",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmpfFxRow, mIsContCol ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_LOCKED",
        "is locked",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmpfFxRow, mIsLocked ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "TCN",
        "transaction change number",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmpfFxRow, mTcn ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_RTS_BOUND",
        "is rts bound",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmpfFxRow, mIsRtsBound ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "RTS_SEQ",
        "rts sequence",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmpfFxRow, mRtsSeq ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "SCN",
        "view/commit scn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smdmpfFxRow, mScn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "TRANS_ID",
        "Transaction ID",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smdmpfFxRow, mTransId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "ROLLBACK_PID",
        "page id of rollback rid",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmpfFxRow, mRollbackPid ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "ROLLBACK_OFFSET",
        "offset of rollback rid",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmpfFxRow, mRollbackOffset ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "COL_COUNT",
        "column count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmpfFxRow, mColCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "PAD_SPACE",
        "paddind space",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmpfFxRow, mPadSpace ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "HAS_NEXT_LINK",
        "has next link",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmpfFxRow, mHasNextLink ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "NEXT_LINK_PID",
        "page id of next link rid",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmpfFxRow, mNextPid ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "NEXT_LINK_OFFSET",
        "offset of rollback rid",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmpfFxRow, mNextOffset ),
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


stlStatus smdmpfOpenRowCallback( void   * aStmt,
                                 void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv )
{
    smdmpfFxRowIterator * sIterator;

    sIterator = (smdmpfFxRowIterator*)aPathCtrl;

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

stlStatus smdmpfCloseRowCallback( void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv )
{
     smdmpfFxRowIterator * sIterator;
    
    sIterator = (smdmpfFxRowIterator*)aPathCtrl;
    STL_TRY( smsFreeSegIterator( aDumpObjHandle,
                                 sIterator->mSegIterator,
                                 SML_ENV( aEnv ) ) == STL_SUCCESS );
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smdmpfBuildRecordRowCallback( void              * aDumpObjHandle,
                                        void              * aPathCtrl,
                                        knlValueBlockList * aValueList,
                                        stlInt32            aBlockIdx,
                                        stlBool           * aTupleExist,
                                        knlEnv            * aEnv )
{
    smdmpfFxRowIterator * sIterator;
    smdmpfFxRow         * sFxRow;
    smpHandle             sPageHandle;
    stlInt16              sState = 0;
    stlChar             * sCurRow;
    stlUInt8              sRtsSeq;

    sIterator = (smdmpfFxRowIterator*)aPathCtrl;
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
        sFxRow->mIsDeleted = (stlInt16)SMDMPF_IS_DELETED( sCurRow );
        sFxRow->mIsMaster = (stlInt16)SMDMPF_IS_MASTER_ROW( sCurRow );
        sFxRow->mIsContCol = (stlInt16)SMDMPF_IS_CONT_COL( sCurRow );
        sFxRow->mIsLocked = (stlInt16)SMDMPF_IS_LOCKED( sCurRow );
        SMDMPF_GET_TCN( sCurRow, &sFxRow->mTcn );
        sFxRow->mIsRtsBound = (stlInt16)SMDMPF_IS_RTS_BOUND( sCurRow );
        if( (stlBool)sFxRow->mIsRtsBound == STL_TRUE )
        {
            SMDMPF_GET_RTSSEQ( sCurRow, &sRtsSeq );
            sFxRow->mRtsSeq = (stlInt16)sRtsSeq;
            sFxRow->mScn = SML_INFINITE_SCN;
        }
        else
        {
            sFxRow->mRtsSeq = (stlInt16)SMP_RTS_SEQ_NULL;
            SMDMPF_GET_SCN( sCurRow, &sFxRow->mScn );
        }
        SMDMPF_GET_TRANS_ID( sCurRow, &(sFxRow->mTransId) );
        SMDMPF_GET_ROLLBACK_PID( sCurRow, (smlPid*)&sFxRow->mRollbackPid );
        SMDMPF_GET_ROLLBACK_OFFSET( sCurRow, (smlPid*)&sFxRow->mRollbackOffset );
        SMDMPF_GET_COLUMN_CNT( sCurRow, &sFxRow->mColCount );
        SMDMPF_GET_PAD_SPACE( sCurRow, &sFxRow->mPadSpace );
        sFxRow->mHasNextLink = (stlInt16)SMDMPF_HAS_NEXT_LINK( sCurRow );
        if( (stlBool)sFxRow->mHasNextLink == STL_TRUE )
        {
            SMDMPF_GET_LINK_PID( sCurRow, (smlPid*)&sFxRow->mNextPid );
            SMDMPF_GET_LINK_OFFSET( sCurRow, (smlPid*)&sFxRow->mNextOffset );
        }
        else
        {
            sFxRow->mNextPid = SMP_NULL_PID;
            sFxRow->mNextOffset = 0;
        }

        STL_TRY( knlBuildFxRecord( gSmdmpfRowColumnDesc,
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


knlFxTableDesc gSmdmpfRowTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smdmpfGetDumpObjectCallback,
    smdmpfOpenRowCallback,
    smdmpfCloseRowCallback,
    smdmpfBuildRecordRowCallback,
    STL_SIZEOF( smdmpfFxRowIterator ),
    "D$MEMORY_HEAP_ROW",
    "memory heap table row information",
    gSmdmpfRowColumnDesc
};


knlFxColumnDesc gSmdmpfLogicalHdrColumnDesc[] =
{
    {
        "PAGE_ID",
        "page id",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmpfFxLogicalHdr, mCurPid ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "SIMPLE_PAGE",
        "simple page",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smdmpfFxLogicalHdr, mSimplePage ),
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


stlStatus smdmpfOpenLogicalHdrCallback( void   * aStmt,
                                        void   * aDumpObjHandle,
                                        void   * aPathCtrl,
                                        knlEnv * aEnv )
{
    smdmpfFxLogicalHdrIterator * sIterator;

    sIterator = (smdmpfFxLogicalHdrIterator*)aPathCtrl;
    
    STL_TRY( smsAllocSegIterator( (smlStatement*)aStmt,
                                  aDumpObjHandle,
                                  &sIterator->mSegIterator,
                                  (smlEnv*)aEnv ) == STL_SUCCESS );

    sIterator->mCurPid = SMP_NULL_PID;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmpfCloseLogicalHdrCallback( void   * aDumpObjHandle,
                                         void   * aPathCtrl,
                                         knlEnv * aEnv )
{
    return STL_SUCCESS;
}


stlStatus smdmpfBuildRecordLogicalHdrCallback( void              * aDumpObjHandle,
                                               void              * aPathCtrl,
                                               knlValueBlockList * aValueList,
                                               stlInt32            aBlockIdx,
                                               stlBool           * aTupleExist,
                                               knlEnv            * aEnv )
{
    smdmpfFxLogicalHdrIterator * sIterator;
    smdmpfFxLogicalHdr           sFxLogicalHdr;
    smpHandle                 sPageHandle;
    stlInt16                  sState = 0;
    smdmpfLogicalHdr        * sLogicalHdr;

    sIterator = (smdmpfFxLogicalHdrIterator*)aPathCtrl;
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

        sLogicalHdr = SMDMPF_GET_LOGICAL_HEADER( SMP_FRAME(&sPageHandle) );

        sFxLogicalHdr.mCurPid = sIterator->mCurPid;
        sFxLogicalHdr.mSimplePage = sLogicalHdr->mSimplePage;

        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );

        STL_TRY( knlBuildFxRecord( gSmdmpfLogicalHdrColumnDesc,
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

knlFxTableDesc gSmdmpfLogicalHdrTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smdmpfGetDumpObjectCallback,
    smdmpfOpenLogicalHdrCallback,
    smdmpfCloseLogicalHdrCallback,
    smdmpfBuildRecordLogicalHdrCallback,
    STL_SIZEOF(smdmpfFxLogicalHdrIterator),
    "D$MEMORY_HEAP_LOGICAL_HEADER",
    "logical header information of heap table",
    gSmdmpfLogicalHdrColumnDesc
};



/** @} */
