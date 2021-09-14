/*******************************************************************************
 * smsmcFxTable.c
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
#include <smt.h>
#include <smxm.h>
#include <smxl.h>
#include <smp.h>
#include <smg.h>
#include <smsDef.h>
#include <smsmcDef.h>
#include <smsmcFxTable.h>
#include <smsmcSegment.h>

extern smsWarmupEntry * gSmsWarmupEntry;

/**
 * @file smsmcFxTable.c
 * @brief Storage Manager Layer Memory Circular Segment Fixed Table Internal Routines
 */

/**
 * @addtogroup smsmcFxTable
 * @{
 */

knlFxColumnDesc gSmsmcExtentColumnDesc[] =
{
    {
        "EXTENT_ID",
        "offset",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smsmcFxExtHdr, mExtId ),
        STL_SIZEOF( smlExtId ),
        STL_FALSE  /* nullable */
    },
    {
        "CURR_EXTENT_PID",
        "next extent header page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smsmcFxExtHdr, mCurrHdrPid ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "NEXT_EXTENT_PID",
        "next extent header page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smsmcFxExtHdr, mNextHdrPid ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "PREV_EXTENT_PID",
        "previous extent header page identifier",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smsmcFxExtHdr, mPrevHdrPid ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "LAST_TRANS_ID",
        "last transaction identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smsmcFxExtHdr, mLastTransId ),
        STL_SIZEOF( smxlTransId ),
        STL_FALSE  /* nullable */
    },
    {
        "LAST_TRANS_VIEW_SCN",
        "last transaction view scn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smsmcFxExtHdr, mLastTransViewScn ),
        STL_SIZEOF( smlScn ),
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

stlStatus smsmcOpenExtentCallback( void   * aStmt,
                                   void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv )
{
    smsmcCache         * sCache;
    smsmcFxExtIterator * sIterator;

    sIterator = (smsmcFxExtIterator*)aPathCtrl;
    sCache = (smsmcCache*)aDumpObjHandle;

    sIterator->mTbsId = sCache->mCommonCache.mTbsId;
    sIterator->mNextExtHdrPid = SMP_NULL_PID;
    sIterator->mStartExtHdrPid = sCache->mHdrPid;

    return STL_SUCCESS;
}

stlStatus smsmcCloseExtentCallback( void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smsmcGetDumpObjectCallback( stlInt64   aTransID,
                                      void     * aStmt,
                                      stlChar  * aDumpObjName,
                                      void    ** aDumpObjHandle,
                                      knlEnv   * aEnv )
{
    stlChar  * sUndoSegIdStr;
    stlChar  * sDummy;
    stlInt32   sTokenCount;
    stlInt64   sUndoSegId;
    stlChar  * sEndPtr;

    *aDumpObjHandle = NULL;
    
    STL_TRY( knlTokenizeNextDumpOption( &aDumpObjName,
                                        &sUndoSegIdStr,
                                        &sDummy,
                                        &sDummy,
                                        &sTokenCount,
                                        KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    STL_TRY_THROW( sTokenCount == 1, RAMP_FINISH );

    STL_TRY( stlStrToInt64( sUndoSegIdStr,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &sUndoSegId,
                            KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    *aDumpObjHandle = smxlGetUndoSegHandleByUndoRelId( sUndoSegId );

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsmcBuildRecordExtentCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aTupleExist,
                                          knlEnv            * aEnv )
{
    smsmcFxExtIterator * sIterator;
    smsmcExtHdr        * sExtHdr;
    smpHandle            sPageHandle;
    stlInt32             sState = 0;
    smsmcFxExtHdr        sFxExtHdr;

    *aTupleExist = STL_FALSE;
    
    sIterator = (smsmcFxExtIterator*)aPathCtrl;

    STL_TRY_THROW( sIterator->mNextExtHdrPid != sIterator->mStartExtHdrPid,
                   RAMP_SUCCESS );

    if( sIterator->mNextExtHdrPid == SMP_NULL_PID )
    {
        sIterator->mNextExtHdrPid = sIterator->mStartExtHdrPid;
    }
    
    STL_TRY( smpFix( sIterator->mTbsId,
                     sIterator->mNextExtHdrPid,
                     &sPageHandle,
                     (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    sExtHdr = (smsmcExtHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );

    sFxExtHdr.mCurrHdrPid = sIterator->mNextExtHdrPid;
    sFxExtHdr.mNextHdrPid = sExtHdr->mNextHdrPid;
    sFxExtHdr.mPrevHdrPid = sExtHdr->mPrevHdrPid;
    sFxExtHdr.mExtId = sExtHdr->mExtId;
    sFxExtHdr.mLastTransId = sExtHdr->mLastTransId;
    sFxExtHdr.mLastTransViewScn = sExtHdr->mLastTransViewScn;
    
    sState = 0;
    STL_TRY( smpUnfix( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlBuildFxRecord( gSmsmcExtentColumnDesc,
                               &sFxExtHdr,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );

    sIterator->mNextExtHdrPid = sFxExtHdr.mNextHdrPid;

    *aTupleExist = STL_TRUE;

    STL_RAMP( RAMP_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void)smpUnfix( &sPageHandle, (smlEnv*)aEnv );
    }

    return STL_FAILURE;
}

knlFxTableDesc gSmsmcExtentTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smsmcGetDumpObjectCallback,
    smsmcOpenExtentCallback,
    smsmcCloseExtentCallback,
    smsmcBuildRecordExtentCallback,
    STL_SIZEOF( smsmcFxExtIterator ),
    "D$MEMORY_UNDO_SEGMENT_EXTENT",
    "memory undo segment extent information",
    gSmsmcExtentColumnDesc
};

/** @} */
