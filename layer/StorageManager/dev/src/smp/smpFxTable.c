/*******************************************************************************
 * smpFxTable.c
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
#include <smpDef.h>
#include <smpFxTable.h>
#include <smp.h>
#include <smf.h>

/**
 * @file smpFxTable.c
 * @brief Storage Manager Layer Page Fixed Table Routines
 */

extern smpWarmupEntry * gSmpWarmupEntry;

/**
 * @addtogroup smpFxTable
 * @{
 */

knlLatchScanCallback gPchLatchScanCallback =
{
    smpOpenLatchClass,
    smpGetNextLatchAndDesc,
    smpCloseLatchClass
};

stlStatus smpOpenLatchClass()
{
    return STL_SUCCESS;
}

stlStatus smpGetNextLatchAndDesc( knlLatch ** aLatch, 
                                  stlChar   * aBuf,
                                  void      * aPrevPosition,
                                  knlEnv    * aEnv )
{
    smpPchLatchPathCtrl * sPathCtrl;
    smpCtrlHeader       * sPchArray;

    sPathCtrl = (smpPchLatchPathCtrl*)aPrevPosition;

    if( *aLatch == NULL )
    {
        sPathCtrl->mPchArrayList = gSmpWarmupEntry->mPchArrayList;
        sPathCtrl->mPchPosition = 0;
    }
    else
    {
        sPathCtrl->mPchPosition += 1;

        if( sPathCtrl->mPchPosition >= sPathCtrl->mPchArrayList->mPchCount )
        {
            while( 1 )
            {
                sPathCtrl->mPchArrayList = sPathCtrl->mPchArrayList->mNext;

                if( sPathCtrl->mPchArrayList == NULL )
                {
                    *aLatch = NULL;
                    STL_THROW( RAMP_SUCCESS );
                }
                
                if( sPathCtrl->mPchArrayList->mState == SMP_PCHARRAY_STATE_ACTIVE )
                {
                    break;
                }
            }
            
            sPathCtrl->mPchPosition = 0;
        }
    }

    STL_TRY( knlGetPhysicalAddr( sPathCtrl->mPchArrayList->mPchArray,
                                 (void**)&sPchArray,
                                 (knlEnv*)aEnv ) == STL_SUCCESS );

    *aLatch = &sPchArray[sPathCtrl->mPchPosition].mPageLatch;

    stlSnprintf( aBuf,
                 KNL_LATCH_MAX_DESC_BUF_SIZE,
                 "PCH ( TBS:%d, PAGE:%d, TYPE:%d )",
                 sPchArray[sPathCtrl->mPchPosition].mTbsId,
                 sPchArray[sPathCtrl->mPchPosition].mPageId,
                 sPchArray[sPathCtrl->mPchPosition].mPageType );

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smpCloseLatchClass()
{
    return STL_SUCCESS;
}

knlFxColumnDesc gSmpSignpostColumnDesc[] =
{
    {
        "PAGE_ID",
        "page id",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smpFxSignpost, mPageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "MANAGEMENT_TYPE",
        "page management type",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxSignpost, mPageMgmtType ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "HIGH_WATER_MARK",
        "high water mark",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxSignpost, mHighWaterMark ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "LOGICAL_HDR_SIZE",
        "logical header size",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxSignpost, mLogicalHeaderSize ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "HINT_RTS",
        "hint rts",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxSignpost, mHintRts ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "LOW_WATER_MARK",
        "low water mark",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxSignpost, mLowWaterMark ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "RECLAIMED_SLOT_SPACE",
        "reclaimed slot space",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxSignpost, mReclaimedSlotSpace ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "ACTIVE_SLOT_COUNT",
        "active slot count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxSignpost, mActiveSlotCount ),
        STL_SIZEOF( stlUInt16 ),
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


stlStatus smpGetDumpObjectCallback( stlInt64   aTransID,
                                    void     * aStmt,
                                    stlChar  * aDumpObjName,
                                    void    ** aDumpObjHandle,
                                    knlEnv   * aEnv )
{
    stlChar  * sToken1;
    stlChar  * sToken2;
    stlChar  * sToken3;
    stlChar  * sObjName = NULL;
    stlInt32   sTokenCount;
    stlBool    sExist;
    void     * sRelHandle;
    smlRelationType   sRelType;

    *aDumpObjHandle = NULL;

    STL_TRY( knlTokenizeNextDumpOption( &aDumpObjName,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );

    if( stlStrcmp( sToken1, "TABLE" ) == 0 )
    {
        STL_TRY( knlTokenizeNextDumpOption( &aDumpObjName,
                                            &sToken1,
                                            &sToken2,
                                            &sToken3,
                                            &sTokenCount,
                                            aEnv ) == STL_SUCCESS );
        if( sTokenCount == 1 )
        {
            sObjName = sToken1;
            STL_TRY( gKnlDumpNameHelper.mGetBaseTableHandle( aTransID,
                                                             ((smlStatement*) aStmt)->mScn,
                                                             NULL,
                                                             sToken1,
                                                             NULL,
                                                             &sRelHandle,
                                                             &sExist,
                                                             aEnv ) == STL_SUCCESS );
        }
        else if( sTokenCount == 2 )
        {
            sObjName = sToken2;
            STL_TRY( gKnlDumpNameHelper.mGetBaseTableHandle( aTransID,
                                                             ((smlStatement*) aStmt)->mScn,
                                                             sToken1,
                                                             sToken2,
                                                             NULL,
                                                             &sRelHandle,
                                                             &sExist,
                                                             aEnv ) == STL_SUCCESS );
        }
        else
        {
            sObjName = NULL;
            sExist = STL_FALSE;
        }

        if( sExist == STL_TRUE )
        {
            sRelType = SME_GET_RELATION_TYPE( sRelHandle );
            STL_TRY_THROW( sRelType == SML_MEMORY_PERSISTENT_HEAP_ROW_TABLE,
                           RAMP_ERR_INVALID_ARGUMENT );
        }
    }
    else if( stlStrcmp( sToken1, "INDEX" ) == 0 )
    {
        STL_TRY( knlTokenizeNextDumpOption( &aDumpObjName,
                                            &sToken1,
                                            &sToken2,
                                            &sToken3,
                                            &sTokenCount,
                                            aEnv ) == STL_SUCCESS );
        if( sTokenCount == 1 )
        {
            sObjName = sToken1;
            STL_TRY( gKnlDumpNameHelper.mGetIndexHandle( aTransID,
                                                         ((smlStatement*) aStmt)->mScn,
                                                         NULL,
                                                         sToken1,
                                                         NULL,
                                                         &sRelHandle,
                                                         &sExist,
                                                         aEnv ) == STL_SUCCESS );
        }
        else if( sTokenCount == 2 )
        {
            sObjName = sToken1;
            STL_TRY( gKnlDumpNameHelper.mGetIndexHandle( aTransID,
                                                         ((smlStatement*) aStmt)->mScn,
                                                         sToken1,
                                                         sToken2,
                                                         NULL,
                                                         &sRelHandle,
                                                         &sExist,
                                                         aEnv ) == STL_SUCCESS );
        }
        else
        {
            sObjName = NULL;
            sExist = STL_FALSE;
        }
    }
    else
    {
        sExist = STL_FALSE;
    }

    STL_TRY_THROW( sExist == STL_TRUE, RAMP_ERR_INVALID_ARGUMENT );

    *aDumpObjHandle = SME_GET_SEGMENT_HANDLE(sRelHandle);

    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_ARGUMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_INVALID_DUMP_OPTION_STRING,
                      NULL,
                      KNL_ERROR_STACK(aEnv),
                      sObjName );
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smpOpenSignpostCallback( void   * aStmt,
                                   void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv )
{
    smpFxSignpostIterator * sIterator;

    sIterator = (smpFxSignpostIterator*)aPathCtrl;
    STL_TRY( smsAllocSegIterator( (smlStatement*)aStmt,
                                  aDumpObjHandle,
                                  &sIterator->mSegIterator,
                                  (smlEnv*)aEnv ) == STL_SUCCESS );

    sIterator->mCurPid = SMP_NULL_PID;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smpCloseSignpostCallback( void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv )
{
    return STL_SUCCESS;
}


stlStatus smpBuildRecordSignpostCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aTupleExist,
                                          knlEnv            * aEnv )
{
    smpFxSignpostIterator * sIterator;
    smpFxSignpost         * sFxSignpost;
    smpHandle               sPageHandle;
    stlInt16                sState = 0;

    sIterator = (smpFxSignpostIterator*)aPathCtrl;
    sFxSignpost = &sIterator->mSignpostRecord;

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

        sFxSignpost->mPageId = sIterator->mCurPid;
        sFxSignpost->mPageMgmtType = smpGetPageMgmtType(&sPageHandle);
        if( sFxSignpost->mPageMgmtType == SMP_ORDERED_SLOT_TYPE )
        {
            sFxSignpost->mHighWaterMark = smpGetHighWaterMark(&sPageHandle);
            sFxSignpost->mLogicalHeaderSize = smpGetLogicalHeaderSize(&sPageHandle);
            sFxSignpost->mMinRtsCount = smpGetMinRtsCount(&sPageHandle);
            sFxSignpost->mMaxRtsCount = smpGetMaxRtsCount(&sPageHandle);
            sFxSignpost->mCurRtsCount = SMP_GET_CURRENT_RTS_COUNT( SMP_FRAME(&sPageHandle) );
            sFxSignpost->mHintRts = smpGetHintRts(&sPageHandle);
            sFxSignpost->mLowWaterMark = smpGetLowWaterMark(&sPageHandle);
            sFxSignpost->mReclaimedSlotSpace = smpGetReclaimedSpace(SMP_FRAME(&sPageHandle));
            sFxSignpost->mActiveSlotCount = smpGetActiveSlotCount(SMP_FRAME(&sPageHandle));
        }
        else
        {
            sFxSignpost->mHighWaterMark = STL_UINT16_MAX;
            sFxSignpost->mLogicalHeaderSize = STL_UINT16_MAX;
            sFxSignpost->mMinRtsCount = STL_UINT16_MAX;
            sFxSignpost->mMaxRtsCount = STL_UINT16_MAX;
            sFxSignpost->mCurRtsCount = STL_UINT16_MAX;
            sFxSignpost->mHintRts = STL_UINT16_MAX;
            sFxSignpost->mLowWaterMark = STL_UINT16_MAX;
            sFxSignpost->mReclaimedSlotSpace = STL_UINT16_MAX;
            sFxSignpost->mActiveSlotCount = STL_UINT16_MAX;
        }

        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );

        STL_TRY( knlBuildFxRecord( gSmpSignpostColumnDesc,
                                   sFxSignpost,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );

        *aTupleExist = STL_TRUE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, (smlEnv*)aEnv );
    }

    return STL_FAILURE;
}

knlFxTableDesc gSmpSignpostTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smpGetDumpObjectCallback,
    smpOpenSignpostCallback,
    smpCloseSignpostCallback,
    smpBuildRecordSignpostCallback,
    STL_SIZEOF( smpFxSignpostIterator ),
    "D$PAGE_SIGNPOST",
    "page signpost information of a segment",
    gSmpSignpostColumnDesc
};



knlFxColumnDesc gSmpRtsColumnDesc[] =
{
    {
        "PAGE_ID",
        "page id",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smpFxRts, mCurPid ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "SEQ",
        "sequence",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxRts, mSequence ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "STATUS",
        "RTS Status",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smpFxRts, mStatus ),
        16,
        STL_FALSE  /* nullable */
    },
    {
        "REF_COUNT",
        "reference count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxRts, mRefCount ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "OFFSET_CACHE0",
        "offset cache [0]",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxRts, mOffsetCache0 ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "OFFSET_CACHE1",
        "offset cache [1]",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxRts, mOffsetCache1 ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "OFFSET_CACHE2",
        "offset cache [2]",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxRts, mOffsetCache2 ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "TRANS_ID",
        "transaction id",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smpFxRts, mTransId ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "SCN",
        "scn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smpFxRts, mScn ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "PREV_VER_COUNT",
        "previous version count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxRts, mPrevVerCnt ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "PREV_VER_OFFSET",
        "previous version offset",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxRts, mPrevOffset ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "PREV_VER_PID",
        "previous version pid",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smpFxRts, mPrevPid ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "PREV_COMMIT_SCN",
        "previous commit scn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smpFxRts, mPrevScn ),
        STL_SIZEOF( stlUInt64 ),
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


stlStatus smpOpenRtsCallback( void   * aStmt,
                              void   * aDumpObjHandle,
                              void   * aPathCtrl,
                              knlEnv * aEnv )
{
    smpFxRtsIterator * sIterator;

    sIterator = (smpFxRtsIterator*)aPathCtrl;
    STL_TRY( smsAllocSegIterator( (smlStatement*)aStmt,
                                  aDumpObjHandle,
                                  &sIterator->mSegIterator,
                                  (smlEnv*)aEnv ) == STL_SUCCESS );

    sIterator->mCurPid = SMP_NULL_PID;
    sIterator->mRtsCount = 0;
    sIterator->mCurRtsSeq = SMP_MAX_RTS_COUNT;
    sIterator->mCurRts = NULL;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smpCloseRtsCallback( void   * aDumpObjHandle,
                               void   * aPathCtrl,
                               knlEnv * aEnv )
{
    return STL_SUCCESS;
}


stlStatus smpBuildRecordRtsCallback( void              * aDumpObjHandle,
                                     void              * aPathCtrl,
                                     knlValueBlockList * aValueList,
                                     stlInt32            aBlockIdx,
                                     stlBool           * aTupleExist,
                                     knlEnv            * aEnv )
{
    smpFxRtsIterator * sIterator;
    smpFxRts         * sFxRts;
    smpHandle          sPageHandle;
    stlInt16           sState = 0;

    sIterator = (smpFxRtsIterator*)aPathCtrl;
    sFxRts = &sIterator->mRtsRecord;
    *aTupleExist = STL_TRUE;

    if( sIterator->mCurRtsSeq >= sIterator->mRtsCount )
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

            stlMemcpy( sIterator->mPageBuf, SMP_FRAME(&sPageHandle), SMP_PAGE_SIZE );
            sIterator->mRtsCount = SMP_GET_CURRENT_RTS_COUNT((stlChar*)(sIterator->mPageBuf));
            sIterator->mCurRtsSeq = 0;
            sIterator->mCurRts = SMP_GET_FIRST_RTS((stlChar*)(sIterator->mPageBuf));
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );

        }
    }

    if( *aTupleExist == STL_TRUE )
    {
        STL_ASSERT( sIterator->mCurRts != NULL );
        sFxRts->mCurPid = sIterator->mCurPid;
        sIterator->mRtsRecord.mSequence = sIterator->mCurRtsSeq;
        if( SMP_RTS_IS_FREE(sIterator->mCurRts) == STL_TRUE )
        {
            stlStrcpy( sFxRts->mStatus, "FREE" );
        }
        else if( SMP_RTS_IS_NOT_STAMPED(sIterator->mCurRts) == STL_TRUE )
        {
            stlStrcpy( sFxRts->mStatus, "NOT_STAMPED" );
        }
        else if( SMP_RTS_IS_STAMPED(sIterator->mCurRts) == STL_TRUE )
        {
            stlStrcpy( sFxRts->mStatus, "STAMPED" );
        }
        else
        {
            STL_ASSERT( SMP_RTS_IS_STABLE(sIterator->mCurRts) == STL_TRUE );
            stlStrcpy( sFxRts->mStatus, "STABLE" );
        }
        sFxRts->mRefCount = sIterator->mCurRts->mRefCount;
        sFxRts->mOffsetCache0 =  sIterator->mCurRts->mOffsetCache[0];
        sFxRts->mOffsetCache1 =  sIterator->mCurRts->mOffsetCache[1];
        sFxRts->mOffsetCache2 =  sIterator->mCurRts->mOffsetCache[2];
        sFxRts->mPrevVerCnt = sIterator->mCurRts->mPrevVerCnt;
        sFxRts->mPrevOffset = sIterator->mCurRts->mPrevOffset;
        sFxRts->mPrevPid = sIterator->mCurRts->mPrevPid;
        sFxRts->mTransId = sIterator->mCurRts->mTransId;
        sFxRts->mScn = sIterator->mCurRts->mScn;
        sFxRts->mPrevScn =  sIterator->mCurRts->mPrevCommitScn;

        STL_TRY( knlBuildFxRecord( gSmpRtsColumnDesc,
                                   sFxRts,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );

        sIterator->mCurRtsSeq++;
        sIterator->mCurRts++;
    }

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpRelease( &sPageHandle, (smlEnv*)aEnv );
    }

    return STL_FAILURE;
}

knlFxTableDesc gSmpRtsTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smpGetDumpObjectCallback,
    smpOpenRtsCallback,
    smpCloseRtsCallback,
    smpBuildRecordRtsCallback,
    STL_SIZEOF( smpFxRtsIterator ),
    "D$PAGE_RTS",
    "page rts information of a segment",
    gSmpRtsColumnDesc
};


knlFxColumnDesc gSmpSegHdrColumnDesc[] =
{
    {
        "PAGE_ID",
        "page id",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smpFxSegHdr, mCurPid ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "PHYSICAL_ID",
        "physical id",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smpFxSegHdr, mPhysicalId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "FREENESS",
        "freeness",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smpFxSegHdr, mFreeness ),
        2,
        STL_FALSE  /* nullable */
    },
    {
        "LSN",
        "log sequence number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smpFxSegHdr, mLsn ),
        STL_SIZEOF(smrLsn),
        STL_FALSE  /* nullable */
    },
    {
        "PARENT_ID",
        "parent page identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smpFxSegHdr, mParentId ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "AGABLE_SCN",
        "agable scn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smpFxSegHdr, mAgableScn ),
        STL_SIZEOF(smlScn),
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


stlStatus smpOpenSegHdrCallback( void   * aStmt,
                                 void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv )
{
    smpFxSegHdrIterator * sIterator;

    sIterator = (smpFxSegHdrIterator*)aPathCtrl;
    
    STL_TRY( smsAllocSegIterator( (smlStatement*)aStmt,
                                  aDumpObjHandle,
                                  &sIterator->mSegIterator,
                                  (smlEnv*)aEnv )
             == STL_SUCCESS );

    sIterator->mCurPid = SMP_NULL_PID;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smpCloseSegHdrCallback( void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv )
{
    smpFxSegHdrIterator * sIterator;

    sIterator = (smpFxSegHdrIterator*)aPathCtrl;
    
    STL_TRY( smsFreeSegIterator( aDumpObjHandle,
                                 &sIterator->mSegIterator,
                                 (smlEnv*)aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus smpBuildRecordSegHdrCallback( void              * aDumpObjHandle,
                                        void              * aPathCtrl,
                                        knlValueBlockList * aValueList,
                                        stlInt32            aBlockIdx,
                                        stlBool           * aTupleExist,
                                        knlEnv            * aEnv )
{
    smpFxSegHdrIterator * sIterator;
    smpFxSegHdr           sFxSegHdr;
    smpHandle             sPageHandle;
    stlInt16              sState = 0;
    smpPhyHdr           * sPhyHdr;

    sIterator = (smpFxSegHdrIterator*)aPathCtrl;
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

        sPhyHdr = SMP_GET_PHYSICAL_HDR( SMP_FRAME(&sPageHandle) );

        sFxSegHdr.mCurPid = sIterator->mCurPid;
        sFxSegHdr.mPhysicalId = sPhyHdr->mSegmentId;
        sFxSegHdr.mLsn = sPhyHdr->mLsn;
        stlMemcpy( &sFxSegHdr.mParentId, &sPhyHdr->mParentRid, STL_SIZEOF(stlInt64) );
        sFxSegHdr.mAgableScn = sPhyHdr->mAgableScn;

        switch( sPhyHdr->mFreeness )
        {
            case SMP_FREENESS_UNALLOCATED:
                stlStrcpy( sFxSegHdr.mFreeness, "UA" );
                break;
            case SMP_FREENESS_FULL:
                stlStrcpy( sFxSegHdr.mFreeness, "FU" );
                break;
            case SMP_FREENESS_UPDATEONLY:
                stlStrcpy( sFxSegHdr.mFreeness, "UP" );
                break;
            case SMP_FREENESS_INSERTABLE:
                stlStrcpy( sFxSegHdr.mFreeness, "IN" );
                break;
            case SMP_FREENESS_FREE:
                stlStrcpy( sFxSegHdr.mFreeness, "FR" );
                break;
            default:
                break;
        }
        
        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );

        STL_TRY( knlBuildFxRecord( gSmpSegHdrColumnDesc,
                                   &sFxSegHdr,
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

knlFxTableDesc gSmpSegHdrTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smpGetDumpObjectCallback,
    smpOpenSegHdrCallback,
    smpCloseSegHdrCallback,
    smpBuildRecordSegHdrCallback,
    STL_SIZEOF(smpFxSegHdrIterator),
    "D$SEGMENT_PAGE_HEADER",
    "page header information of a segment",
    gSmpSegHdrColumnDesc
};

knlFxColumnDesc gSmpPageDumpColumnDesc[] =
{
    {
        "ADDRESS",
        "address in a page",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smpFxPageDump, mOffSetStr ),
        SMP_PAGE_DUMP_DATA_OFFSET_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "DATA",
        "dumped page data",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smpFxPageDump, mData ),
        SMP_PAGE_DUMP_DATA_LENGTH * 2,
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

stlStatus smpGetPageDumpObjectCallback( stlInt64   aTransID,
                                        void     * aStmt,
                                        stlChar  * aDumpObjName,
                                        void    ** aDumpObjHandle,
                                        knlEnv   * aEnv )
{   
    stlChar  * sToken1;
    stlChar  * sToken2;
    stlChar  * sToken3;
    stlChar  * sEndPtr;
    stlInt32   sTokenCount;
    stlBool    sExist;
    smlTbsId   sTbsId;
    smlPid     sPageId;
    stlInt64   sPageSeqId;
    stlUInt64  sNum64Bit;
    
    smlDatafileId       sDatafileId;
    smpFxPageDumpHandle sPageDumpHandle;

    *aDumpObjHandle = NULL;
    SML_SET_INVALID_RID( &sPageDumpHandle );
    
    /* Parse TABLESPACE_NAME or TABLESPACE_ID */
    STL_TRY( knlTokenizeNextDumpOption( &aDumpObjName,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );
    STL_TRY_THROW( sTokenCount == 1, RAMP_FINISH );

    if( (sToken1[0] >= '0') && (sToken1[0] <= '9') )
    {
        /* Convert TABLESPACE_ID from string */
        STL_TRY( stlStrToUInt64( sToken1,
                                 STL_NTS,
                                 &sEndPtr,
                                 10,
                                 &sNum64Bit,
                                 KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
        STL_TRY_THROW( (sNum64Bit >= 0) && (sNum64Bit < SML_TBS_MAX_COUNT), RAMP_FINISH );
    }
    else
    {
        /* Get TABLESPACE_ID using TABLESPACE_NAME */
        STL_TRY( gKnlDumpNameHelper.mGetSpaceHandle ( aTransID,
                                                      ((smlStatement*) aStmt)->mScn,
                                                      NULL,
                                                      sToken1,
                                                      NULL,
                                                      (void**)&sNum64Bit,
                                                      &sExist,
                                                      aEnv ) == STL_SUCCESS );
        STL_TRY_THROW( sExist == STL_TRUE, RAMP_FINISH );
    }
    
    sTbsId = (smlTbsId)sNum64Bit;
    
    STL_TRY_THROW( smfIsValidTbs( sTbsId ) == STL_TRUE, RAMP_FINISH );
    
    /* Parse PAGE_ID */
    STL_TRY( knlTokenizeNextDumpOption( &aDumpObjName,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );
    STL_TRY_THROW( sTokenCount == 1, RAMP_FINISH );
    
    STL_TRY( stlStrToUInt64( sToken1,
                             STL_NTS,
                             &sEndPtr,
                             10,
                             &sNum64Bit,
                             KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
    sPageId = (smlPid) sNum64Bit;
    sDatafileId = SMP_DATAFILE_ID( sPageId );
    sPageSeqId  = SMP_PAGE_SEQ_ID( sPageId );
    
    /* Check validation of PageId & Tablespace Id. */
    STL_TRY_THROW( smfIsValidDatafile( sTbsId, sDatafileId ) == STL_TRUE, RAMP_FINISH );
    STL_TRY_THROW( sPageSeqId < smfGetMaxPageCount( sTbsId, sDatafileId ), RAMP_FINISH );
    
    sPageDumpHandle.mTbsId  = sTbsId;
    sPageDumpHandle.mPageId = sPageId;

    stlMemcpy( aDumpObjHandle, &sPageDumpHandle, STL_SIZEOF( smpFxPageDumpHandle ) );
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;
   
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smpOpenPageDumpCallback( void   * aStmt,
                                   void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv )
{
    stlInt32              sState = 0;
    smpFxPageIterator   * sIterator;
    smpFxPageDumpHandle * sHandle;
    
    sIterator = (smpFxPageIterator *)   aPathCtrl;
    sHandle   = (smpFxPageDumpHandle *) &aDumpObjHandle;

    sIterator->mTbsId      = sHandle->mTbsId;
    sIterator->mPageId     = sHandle->mPageId;
    
    /* Page Fix : PageHandle 얻음. */
    STL_TRY( smpFix( sIterator->mTbsId,
                     sIterator->mPageId,
                     &sIterator->mPageHandle,
                     (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;
    /* PageHandle 이용하여 주소를 얻고 PAGE 복사. */
    sIterator->mStartAddr  = SMP_FRAME( &sIterator->mPageHandle );
    sIterator->mSize       = SMP_PAGE_SIZE;
    sIterator->mRemainSize = SMP_PAGE_SIZE;
    sIterator->mCurAddr    = sIterator->mPageSnapshot;
    
    stlMemcpy( sIterator->mPageSnapshot, sIterator->mStartAddr, SMP_PAGE_SIZE );
    
    /* Page Unfix */
    sState = 0;
    STL_TRY( smpUnfix( &sIterator->mPageHandle,
                       (smlEnv*)aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void) smpUnfix( &sIterator->mPageHandle, (smlEnv*)aEnv );
        default:
            break;
    }
    
    return STL_FAILURE;
}

stlStatus smpClosePageDumpCallback( void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smpBuildRecordPageDumpCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aTupleExist,
                                          knlEnv            * aEnv )
{
    stlInt32  sPrintSize;
    stlUInt32 sOffSet;
    stlInt32  sStrSize;
    
    smpFxPageDump       sPageDump;
    smpFxPageIterator * sIterator;

    sIterator = (smpFxPageIterator*)aPathCtrl;    

    *aTupleExist = STL_FALSE;
    
    if( sIterator->mRemainSize > 0 )
    {
        if( sIterator->mRemainSize >= SMP_PAGE_DUMP_DATA_LENGTH )
        {
            sPrintSize = SMP_PAGE_DUMP_DATA_LENGTH;
        }
        else
        {
            sPrintSize = sIterator->mRemainSize;
        }
        /* PrintSize(Default 32) byte를 HexaDecimal로 변환과 OffSet 설정. */
        sOffSet = SMP_PAGE_SIZE - sIterator->mRemainSize;
        sStrSize = stlSnprintf( sPageDump.mOffSetStr,
                                SMP_PAGE_DUMP_DATA_OFFSET_LENGTH,
                                "%04X",
                                sOffSet );
        sPageDump.mOffSetStr[sStrSize] = '\0';
        
        STL_TRY( stlBinary2HexString( (stlUInt8*)(sIterator->mCurAddr),
                                      sPrintSize,
                                      sPageDump.mData,
                                      (SMP_PAGE_DUMP_DATA_LENGTH * 2) + 1,
                                      KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );        


        STL_TRY( knlBuildFxRecord( gSmpPageDumpColumnDesc,
                                   &sPageDump,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv ) == STL_SUCCESS );
        *aTupleExist = STL_TRUE;
        sIterator->mCurAddr += sPrintSize;
        sIterator->mRemainSize -= sPrintSize;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmpPageDumpTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smpGetPageDumpObjectCallback,
    smpOpenPageDumpCallback,
    smpClosePageDumpCallback,
    smpBuildRecordPageDumpCallback,
    STL_SIZEOF(smpFxPageIterator),
    "D$PAGE",
    "page dump table - usage 'tablespace_name|tablespace_id,page_id'",
    gSmpPageDumpColumnDesc
};

knlFxColumnDesc gSmpPchColumnDesc[] =
{
    {
        "TBS_ID",
        "tablespace id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxPchDump, mTbsId ),
        STL_SIZEOF( stlUInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "PAGE_ID",
        "page id",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smpFxPchDump, mPageId ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "LOGICAL_ADDRESS",
        "page body logical address",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smpFxPchDump, mFrame ),
        SMP_PCH_ADDR_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "FIXED_COUNT",
        "page fixed count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smpFxPchDump, mFixedCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "DIRTY",
        "page dirty state",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smpFxPchDump, mDirty ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "PAGE_TYPE",
        "page type",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smpFxPchDump, mPageType ),
        SMP_PCH_PAGE_TYPE_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "DEVICE_TYPE",
        "device type",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smpFxPchDump, mDeviceType ),
        SMP_PCH_DEVICE_TYPE_LENGTH,
        STL_FALSE  /* nullable */
    },
    {
        "FIRST_DIRTY_LSN",
        "first dirty log sequence number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smpFxPchDump, mFirstDirtyLsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "RECOVERY_LSN",
        "recovery point log sequence number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smpFxPchDump, mRecoveryLsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "RECOVERY_SBSN",
        "recovery point string blocked sequence number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smpFxPchDump, mRecoverySbsn ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "RECOVERY_LPSN",
        "recovery point log piece sequence number",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smpFxPchDump, mRecoveryLpsn ),
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

stlStatus smpGetPchDumpObjectCallback( stlInt64   aTransID,
                                       void     * aStmt,
                                       stlChar  * aDumpObjName,
                                       void    ** aDumpObjHandle,
                                       knlEnv   * aEnv )
{
    stlChar  * sToken1;
    stlChar  * sToken2;
    stlChar  * sToken3;
    stlChar  * sEndPtr;
    stlInt32   sTokenCount;
    stlUInt64  sNum64Bit;
    stlBool    sExist;
    
    smpFxPchDumpHandle sPchHandle;

    SML_SET_INVALID_RID( &sPchHandle );
    *aDumpObjHandle = NULL;

    /* Parse TABLESPACE_NAME or TABLESPACE_ID */
    STL_TRY( knlTokenizeNextDumpOption( &aDumpObjName,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );
    STL_TRY_THROW( sTokenCount == 1, RAMP_FINISH );

    if( (sToken1[0] >= '0') && (sToken1[0] <= '9') )
    {
        /* Convert TABLESPACE_ID from string */
        STL_TRY( stlStrToUInt64( sToken1,
                                 STL_NTS,
                                 &sEndPtr,
                                 10,
                                 &sNum64Bit,
                                 KNL_ERROR_STACK( aEnv ) ) == STL_SUCCESS );
        STL_TRY_THROW( (sNum64Bit >= 0) && (sNum64Bit < SML_TBS_MAX_COUNT), RAMP_FINISH );
    }
    else
    {
        /* Get TABLESPACE_ID using TABLESPACE_NAME */
        STL_TRY( gKnlDumpNameHelper.mGetSpaceHandle ( aTransID,
                                                      ((smlStatement*) aStmt)->mScn,
                                                      NULL,
                                                      sToken1,
                                                      NULL,
                                                      (void**)&sNum64Bit,
                                                      &sExist,
                                                      aEnv ) == STL_SUCCESS );
        STL_TRY_THROW( sExist == STL_TRUE, RAMP_FINISH );
    }

    sPchHandle.mTbsId = (smlTbsId) sNum64Bit; 
    
    STL_TRY_THROW( smfIsValidTbs( sPchHandle.mTbsId ) == STL_TRUE, RAMP_FINISH );

    stlMemcpy( aDumpObjHandle, &sPchHandle, STL_SIZEOF( smpFxPchDumpHandle ) );
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;
   
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smpOpenPchCallback( void   * aStmt,
                              void   * aDumpObjHandle,
                              void   * aPathCtrl,
                              knlEnv * aEnv )
{
    smpFxPchIterator   * sIterator;
    smpFxPchDumpHandle * sDumpObjHandle;
    stlBool              sExist;
    
    /* Tablesapce Id와 Iterator 초기화. */
    sIterator      = (smpFxPchIterator*)   aPathCtrl;
    sDumpObjHandle = (smpFxPchDumpHandle*) &aDumpObjHandle;

    sIterator->mTbsId           = sDumpObjHandle->mTbsId;

    /* Tablespace의 처음 datafile_id를 얻음. */
    STL_TRY( smfFirstDatafile ( sIterator->mTbsId,
                                &sIterator->mDatafileId,
                                &sExist,
                                (smlEnv*)aEnv ) == STL_SUCCESS );

    sIterator->mPageSeqId = -1;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smpClosePchCallback( void   * aDumpObjHandle,
                               void   * aPathCtrl,
                               knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smpGetPchPageType( void   * aHandle,
                             void   * aPchDump,
                             knlEnv * aEnv )
{
    smpCtrlHeader * sPch;
    smpFxPchDump  * sPchDump;
    
    sPch     = ((smpHandle *)  aHandle)->mPch;
    sPchDump = (smpFxPchDump *) aPchDump;
        
    switch( sPch->mPageType )
    {
        case SMP_PAGE_TYPE_INIT :
            stlStrncpy( sPchDump->mPageType, "INIT", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_UNFORMAT :
            stlStrncpy( sPchDump->mPageType, "UNFORMAT", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_EXT_BLOCK_MAP :
            stlStrncpy( sPchDump->mPageType, "EXT_BLOCK_MAP", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_EXT_MAP :
            stlStrncpy( sPchDump->mPageType, "EXT_MAP", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_BITMAP_HEADER :
            stlStrncpy( sPchDump->mPageType, "BITMAP_HEADER", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_CIRCULAR_HEADER:
            stlStrncpy( sPchDump->mPageType, "CIRCULAR_HEADER", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_FLAT_HEADER :
            stlStrncpy( sPchDump->mPageType, "FLAT_HEADER", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_PENDING_HEADER :
            stlStrncpy( sPchDump->mPageType, "PENDING_HEADER", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_UNDO :
            stlStrncpy( sPchDump->mPageType, "UNDO", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_TRANS :
            stlStrncpy( sPchDump->mPageType, "TRANS", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_BITMAP_INTERNAL :
            stlStrncpy( sPchDump->mPageType, "BITMAP_INTERNAL", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_BITMAP_LEAF :
            stlStrncpy( sPchDump->mPageType, "BITMAP_LEAF", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_BITMAP_EXTENT_MAP :
            stlStrncpy( sPchDump->mPageType, "BITMAP_EXTENT_MAP", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_TABLE_DATA :
            stlStrncpy( sPchDump->mPageType, "TABLE_DATA", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_TABLE_COLUMNAR_DATA :
            stlStrncpy( sPchDump->mPageType, "TABLE_COLUMNAR_DATA", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_INDEX_DATA :
            stlStrncpy( sPchDump->mPageType, "INDEX_DATA", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        case SMP_PAGE_TYPE_MAX :
            stlStrncpy( sPchDump->mPageType, "MAX", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
        default:
            stlStrncpy( sPchDump->mPageType, "UNKNOWN", SMP_PCH_PAGE_TYPE_LENGTH + 1 );
            break;
    }
    
    return STL_SUCCESS;
}

stlStatus smpGetPchDeviceType( void   * aHandle,
                               void   * aPchDump,
                               knlEnv * aEnv )
{
    smpCtrlHeader * sPch;
    smpFxPchDump  * sPchDump;
    
    sPch     = ((smpHandle *) aHandle)->mPch;
    sPchDump = (smpFxPchDump *) aPchDump;

    switch( SML_TBS_DEVICE_TYPE( sPch->mDeviceType ) )
    {
        case 0 :
            stlStrncpy( sPchDump->mDeviceType, "MEMORY", SMP_PCH_DEVICE_TYPE_LENGTH + 1 );
            break;
        case 1:
            stlStrncpy( sPchDump->mDeviceType, "DISK", SMP_PCH_DEVICE_TYPE_LENGTH + 1 );
            break;
        default:
            stlStrncpy( sPchDump->mDeviceType, "UNKNOWN", SMP_PCH_DEVICE_TYPE_LENGTH + 1 );
            break;
    }

    return STL_SUCCESS;
}

stlStatus smpBuildRecordPchCallback( void              * aDumpObjHandle,
                                     void              * aPathCtrl,
                                     knlValueBlockList * aValueList,
                                     stlInt32            aBlockIdx,
                                     stlBool           * aTupleExist,
                                     knlEnv            * aEnv )
{    
    stlInt32           sAddrSize;
    smpHandle          sPageHandle;   
    smpFxPchDump       sPchDump;
    smpFxPchIterator * sIterator;
    stlBool            sExist;
    
    sIterator = (smpFxPchIterator *) aPathCtrl;
    *aTupleExist = STL_FALSE;

    /* Check Validation of PageSeqId */    
    if( sIterator->mPageSeqId >= smfGetMaxPageCount( sIterator->mTbsId, sIterator->mDatafileId ) )
    {
        sIterator->mPageSeqId = -1;
        
        while( 1 )
        {
            STL_TRY( smfNextDatafile( sIterator->mTbsId,
                                      &sIterator->mDatafileId,
                                      &sExist,
                                      (smlEnv*)aEnv ) == STL_SUCCESS );

            STL_TRY_THROW( sExist == STL_TRUE, RAMP_FINISH );

            if( smfGetDatafileState( sIterator->mTbsId,
                                     sIterator->mDatafileId )
                == SMF_DATAFILE_STATE_CREATING )
            {
                /**
                 * 아직 만들어 지지 않은 datafile 이다.
                 */
                continue;
            }
        }
    }
    sIterator->mPageSeqId++;
        
    /* Get Page Handle */
    STL_TRY( smfGetPchAndFrame( sIterator->mTbsId,
                                sIterator->mDatafileId,
                                sIterator->mPageSeqId,
                                (void**)&sPageHandle.mPch,
                                (void**)&sPageHandle.mFrame,
                                (smlEnv*) aEnv ) == STL_SUCCESS );
    /* Page Id*/
    sPchDump.mPageId = sPageHandle.mPch->mPageId;
        
    /* Tablespace Id */
    sPchDump.mTbsId  = sPageHandle.mPch->mTbsId;
        
    /* Frame(knlLogicalAddr) */
    sAddrSize = stlSnprintf( sPchDump.mFrame,
                             SMP_PCH_ADDR_LENGTH,
                             "0x%p",
                             sPageHandle.mPch->mFrame );
    sPchDump.mFrame[sAddrSize] = '\0';
    
    /* Fixed Count */
    sPchDump.mFixedCount = sPageHandle.mPch->mFixedCount;
    
    /* Dirty */
    sPchDump.mDirty = sPageHandle.mPch->mDirty;
    
    /* Page Type */
    STL_TRY( smpGetPchPageType( &sPageHandle,
                                &sPchDump,
                                aEnv ) == STL_SUCCESS );

    /* Device Type */
    STL_TRY( smpGetPchDeviceType( &sPageHandle,
                                  &sPchDump,
                                  aEnv ) == STL_SUCCESS );

    /* First Dirty Lsn */
    sPchDump.mFirstDirtyLsn = sPageHandle.mPch->mFirstDirtyLsn;
    /* Recovery Lsn */
    sPchDump.mRecoveryLsn = sPageHandle.mPch->mRecoveryLsn;
    /* Recovery Sbsn */
    sPchDump.mRecoverySbsn = sPageHandle.mPch->mRecoverySbsn;
    /* Recovery Lpsn */
    sPchDump.mRecoveryLpsn = sPageHandle.mPch->mRecoveryLpsn;

    STL_TRY( knlBuildFxRecord( gSmpPchColumnDesc,
                               &sPchDump,
                               aValueList,
                               aBlockIdx,
                               aEnv ) == STL_SUCCESS );

    *aTupleExist = STL_TRUE;
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmpPchTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smpGetPchDumpObjectCallback,
    smpOpenPchCallback,
    smpClosePchCallback,
    smpBuildRecordPchCallback,
    STL_SIZEOF(smpFxPchIterator),
    "D$PCH",
    "pch dump table - usage 'tablespace_name|tablespace_id'",
    gSmpPchColumnDesc
};
    
/** @} */
