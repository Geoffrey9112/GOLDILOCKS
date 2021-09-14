/*******************************************************************************
 * smtFxTable.c
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
#include <smt.h>
#include <smf.h>
#include <smtDef.h>
#include <smtExtMgr.h>
#include <smlTablespace.h>

/**
 * @file smtFxTable.c
 * @brief Storage Manager Layer Tablespace Fixed Table Internal Routines
 */

/**
 * @addtogroup smtFxTable
 * @{
 */

knlFxColumnDesc gTbsExtColumnDesc[] =
{
    {
        "TBS_ID",
        "tablespace id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smtFxTbsExt, mTbsId ),
        STL_SIZEOF( smlTbsId ),
        STL_FALSE  /* nullable */
    },
    {
        "DATAFILE_ID",
        "datafile id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smtFxTbsExt, mDatafileId ),
        STL_SIZEOF( smlDatafileId ),
        STL_FALSE  /* nullable */
    },
    {
        "DATA_PAGE_SEQ",
        "data page sequence",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smtFxTbsExt, mPageSeqId ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "DATA_PAGE_ID",
        "data page id",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smtFxTbsExt, mDataPid ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "STATE",
        "extent state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smtFxTbsExt, mState ),
        1,
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

stlStatus smtOpenTbsExtCallback( void   * aStmt,
                                 void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv )
{
    smtTbsExtPathCtrl   sPathCtrl;
    smlTbsId            sTbsId;
    smpHandle           sExtBlockMapHandle;
    smtExtBlockMapHdr * sExtBlockMapHdr;
    smtExtBlockMap    * sExtBlockMap;
    smlPid              sExtBlockMapPid;
    smlPid              sFreeExtMap;
    stlInt32            sState = 0;
    smlDatafileId       sDatafileId;
    stlBool             sExist;
    
    sTbsId = smfGetTbsIdByHandle( aDumpObjHandle );

    STL_TRY( smlValidateTablespace( sTbsId,
                                    SME_VALIDATION_BEHAVIOR_READ,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( smfFirstDatafile( sTbsId,
                               &sDatafileId,
                               &sExist,
                               SML_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sExist == STL_TRUE );
    
    sExtBlockMapPid = SMP_MAKE_PID( sDatafileId, SMT_EXT_BLOCK_MAP_PAGE_SEQ );
    
    STL_TRY( smpAcquire( NULL,
                         sTbsId,
                         sExtBlockMapPid,
                         KNL_LATCH_MODE_SHARED,
                         &sExtBlockMapHandle,
                         (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;

    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( &sExtBlockMapHandle ) );
    sExtBlockMap = (smtExtBlockMap*)((stlChar*)sExtBlockMapHdr +
                                     STL_SIZEOF( smtExtBlockMapHdr ));

    sFreeExtMap = sExtBlockMap->mExtBlockDesc[0].mExtBlock;
        
    sState = 0;
    STL_TRY( smpRelease( &sExtBlockMapHandle, (smlEnv*)aEnv ) == STL_SUCCESS );

    sPathCtrl.mExtBlockMapRid.mTbsId = sTbsId;
    sPathCtrl.mExtBlockMapRid.mPageId = sExtBlockMapPid;
    sPathCtrl.mExtBlockMapRid.mOffset = 0;
    sPathCtrl.mExtMapRid.mTbsId = sTbsId;
    sPathCtrl.mExtMapRid.mPageId = sFreeExtMap;
    sPathCtrl.mExtMapRid.mOffset = 0;
    sPathCtrl.mDatafileId = sDatafileId;
    
    stlMemcpy( aPathCtrl, &sPathCtrl, STL_SIZEOF(smtTbsExtPathCtrl) );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    switch( sState )
    {
        case 1:
            (void)smpRelease( &sExtBlockMapHandle, (smlEnv*)aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

stlStatus smtCloseTbsExtCallback( void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smtGetDumpObjectCallback( stlInt64   aTransID,
                                    void     * aStmt,
                                    stlChar  * aDumpObjName,
                                    void    ** aDumpObjHandle,
                                    knlEnv   * aEnv )
{
    stlChar  * sToken1;
    stlChar  * sToken2;
    stlChar  * sToken3;
    stlInt32   sTokenCount;
    stlBool    sExist;
    stlInt64   sTbsId;

    *aDumpObjHandle = NULL;

    STL_TRY( knlTokenizeNextDumpOption( &aDumpObjName,
                                        &sToken1,
                                        &sToken2,
                                        &sToken3,
                                        &sTokenCount,
                                        aEnv ) == STL_SUCCESS );

    STL_TRY_THROW( sTokenCount == 1, RAMP_FINISH );

    STL_TRY( gKnlDumpNameHelper.mGetSpaceHandle( aTransID,
                                                 ((smlStatement*) aStmt)->mScn,
                                                 NULL,
                                                 sToken1,
                                                 NULL,
                                                 (void**)&sTbsId,
                                                 &sExist,
                                                 aEnv ) == STL_SUCCESS );

    STL_TRY_THROW( sExist == STL_TRUE, RAMP_FINISH );

    *aDumpObjHandle = smfGetTbsHandle( (smlTbsId)sTbsId );

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smtBuildRecordTbsExtCallback( void              * aDumpObjHandle,
                                        void              * aPathCtrl,
                                        knlValueBlockList * aValueList,
                                        stlInt32            aBlockIdx,
                                        stlBool           * aTupleExist,
                                        knlEnv            * aEnv )
{
    smpHandle           sPageHandle;
    smtExtBlockMapHdr * sExtBlockMapHdr;
    smtTbsExtPathCtrl   sPathCtrl;
    smtExtBlockMap    * sExtBlockMap;
    smtExtMapHdr      * sExtMapHdr;
    smlTbsId            sTbsId;
    stlChar           * sExtBitArray;
    smtFxTbsExt         sFxTbsExt;
    stlInt32            sState = 0;
    stlInt16            sPageCountInExt;
    smlPid              sFreeExtMap;
    stlBool             sExist;

    *aTupleExist = STL_FALSE;
    
    stlMemcpy( &sPathCtrl, aPathCtrl, STL_SIZEOF(smtTbsExtPathCtrl) );
    sTbsId = sPathCtrl.mExtMapRid.mTbsId;

    sPageCountInExt = smfGetPageCountInExt( sTbsId );

    STL_RAMP( RAMP_RETRY );
    
    STL_TRY( smpAcquire( NULL,
                         sTbsId,
                         sPathCtrl.mExtMapRid.mPageId,
                         KNL_LATCH_MODE_SHARED,
                         &sPageHandle,
                         (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;
    
    sExtMapHdr = (smtExtMapHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
    sExtBitArray = SMT_EXT_BIT_ARRAY( sExtMapHdr );

    if( sPathCtrl.mExtMapRid.mOffset >= sExtMapHdr->mExtCount )
    {
        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );

        STL_TRY( smpAcquire( NULL,
                             sTbsId,
                             sPathCtrl.mExtBlockMapRid.mPageId,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             (smlEnv*)aEnv ) == STL_SUCCESS );
        sState = 1;
        
        sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
        sExtBlockMap = (smtExtBlockMap*)((stlChar*)sExtBlockMapHdr +
                                         STL_SIZEOF( smtExtBlockMapHdr ));

        sPathCtrl.mExtBlockMapRid.mOffset++;

        if( sPathCtrl.mExtBlockMapRid.mOffset >= sExtBlockMapHdr->mExtBlockDescCount )
        {
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
            
            while( 1 )
            {
                STL_TRY( smfNextDatafile( sTbsId,
                                          &sPathCtrl.mDatafileId,
                                          &sExist,
                                          SML_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_TRY_THROW( sExist == STL_TRUE, RAMP_SUCCESS );

                if( smfGetDatafileState( sTbsId, sPathCtrl.mDatafileId ) == SMF_DATAFILE_STATE_CREATED )
                {
                    break;
                }
            }

            sPathCtrl.mExtBlockMapRid.mTbsId = sTbsId;
            sPathCtrl.mExtBlockMapRid.mPageId = SMP_MAKE_PID( sPathCtrl.mDatafileId,
                                                              SMT_EXT_BLOCK_MAP_PAGE_SEQ );
            sPathCtrl.mExtBlockMapRid.mOffset = 0;

            STL_TRY( smpAcquire( NULL,
                                 sTbsId,
                                 sPathCtrl.mExtBlockMapRid.mPageId,
                                 KNL_LATCH_MODE_SHARED,
                                 &sPageHandle,
                                 (smlEnv*)aEnv ) == STL_SUCCESS );
            sState = 1;

            sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
            sExtBlockMap = (smtExtBlockMap*)((stlChar*)sExtBlockMapHdr +
                                             STL_SIZEOF( smtExtBlockMapHdr ));

            sFreeExtMap = sExtBlockMap->mExtBlockDesc[0].mExtBlock;
        
            sState = 0;
            STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
            
            sPathCtrl.mExtMapRid.mTbsId = sTbsId;
            sPathCtrl.mExtMapRid.mPageId = sFreeExtMap;
            sPathCtrl.mExtMapRid.mOffset = 0;

            STL_THROW( RAMP_RETRY );
        }
        
        sPathCtrl.mExtMapRid.mPageId =
            sExtBlockMap->mExtBlockDesc[sPathCtrl.mExtBlockMapRid.mOffset].mExtBlock;
        sPathCtrl.mExtMapRid.mOffset = 0;
            
        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
        
        STL_TRY( smpAcquire( NULL,
                             sTbsId,
                             sPathCtrl.mExtMapRid.mPageId,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             (smlEnv*)aEnv ) == STL_SUCCESS );
        sState = 1;
            
        sExtMapHdr = (smtExtMapHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
        sExtBitArray = SMT_EXT_BIT_ARRAY( sExtMapHdr );
    }

    sFxTbsExt.mTbsId = sTbsId;
    sFxTbsExt.mDatafileId = sPathCtrl.mDatafileId;
    sFxTbsExt.mDataPid = sPathCtrl.mExtMapRid.mPageId + 1 + (sPathCtrl.mExtMapRid.mOffset *
                                                             sPageCountInExt);
    sFxTbsExt.mPageSeqId = SMP_PAGE_SEQ_ID(sFxTbsExt.mDataPid);
    
    if( smtIsFreeExt( sExtBitArray, sPathCtrl.mExtMapRid.mOffset ) == STL_TRUE )
    {
        stlStrncpy( sFxTbsExt.mState, "F", 2 );
    }
    else
    {
        stlStrncpy( sFxTbsExt.mState, "U", 2 );
    }
    
    sState = 0;
    STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
    
    STL_TRY( knlBuildFxRecord( gTbsExtColumnDesc,
                               &sFxTbsExt,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );
        
    *aTupleExist = STL_TRUE;

    sPathCtrl.mExtMapRid.mOffset++;
    
    stlMemcpy( aPathCtrl, &sPathCtrl, STL_SIZEOF(smtTbsExtPathCtrl) );

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smpRelease( &sPageHandle, (smlEnv*)aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

knlFxTableDesc gTbsExtTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smtGetDumpObjectCallback,
    smtOpenTbsExtCallback,
    smtCloseTbsExtCallback,
    smtBuildRecordTbsExtCallback,
    STL_SIZEOF( smtTbsExtPathCtrl ),
    "D$TABLESPACE_EXTENT",
    "tablespace extent information",
    gTbsExtColumnDesc
};

knlFxColumnDesc gTbsExtBlockDescColumnDesc[] =
{
    {
        "TBS_ID",
        "tablespace id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smtFxTbsExtBlockDesc, mTbsId ),
        STL_SIZEOF( smlTbsId ),
        STL_FALSE  /* nullable */
    },
    {
        "DATAFILE_ID",
        "datafile id",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smtFxTbsExtBlockDesc, mDatafileId ),
        STL_SIZEOF( smlDatafileId ),
        STL_FALSE  /* nullable */
    },
    {
        "SEQ",
        "data page sequence",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smtFxTbsExtBlockDesc, mSeqId ),
        STL_SIZEOF( stlUInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "EXTENT_BLOCK_PID",
        "extent block page id",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smtFxTbsExtBlockDesc, mExtBlockPid ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "PREPARED",
        "prepare to allocation",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smtFxTbsExtBlockDesc, mPrepared ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "STATE",
        "extent block state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smtFxTbsExtBlockDesc, mState ),
        16,
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

stlStatus smtOpenTbsExtBlockDescCallback( void   * aStmt,
                                          void   * aDumpObjHandle,
                                          void   * aPathCtrl,
                                          knlEnv * aEnv )
{
    smtTbsExtBlockDescPathCtrl   sPathCtrl;
    smlTbsId                     sTbsId;
    smlPid                       sExtBlockMapPid;
    smlDatafileId                sDatafileId;
    stlBool                      sExist;
    
    sTbsId = smfGetTbsIdByHandle( aDumpObjHandle );

    STL_TRY( smlValidateTablespace( sTbsId,
                                    SME_VALIDATION_BEHAVIOR_READ,
                                    SML_ENV(aEnv) )
             == STL_SUCCESS );
    
    STL_TRY( smfFirstDatafile( sTbsId,
                               &sDatafileId,
                               &sExist,
                               SML_ENV(aEnv) )
             == STL_SUCCESS );
    STL_ASSERT( sExist == STL_TRUE );
    
    sExtBlockMapPid = SMP_MAKE_PID( sDatafileId, SMT_EXT_BLOCK_MAP_PAGE_SEQ );
    
    sPathCtrl.mExtBlockMapRid.mTbsId = sTbsId;
    sPathCtrl.mExtBlockMapRid.mPageId = sExtBlockMapPid;
    sPathCtrl.mExtBlockMapRid.mOffset = -1;
    sPathCtrl.mDatafileId = sDatafileId;
    
    stlMemcpy( aPathCtrl, &sPathCtrl, STL_SIZEOF(smtTbsExtBlockDescPathCtrl) );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

stlStatus smtCloseTbsExtBlockDescCallback( void   * aDumpObjHandle,
                                           void   * aPathCtrl,
                                           knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smtBuildRecordTbsExtBlockDescCallback( void              * aDumpObjHandle,
                                                 void              * aPathCtrl,
                                                 knlValueBlockList * aValueList,
                                                 stlInt32            aBlockIdx,
                                                 stlBool           * aTupleExist,
                                                 knlEnv            * aEnv )
{
    smpHandle                       sPageHandle;
    smtExtBlockMapHdr             * sExtBlockMapHdr;
    smtTbsExtBlockDescPathCtrl      sPathCtrl;
    smtExtBlockMap                * sExtBlockMap;
    smtFxTbsExtBlockDesc            sFxTbsExtBlockDesc;
    stlInt32                        sState = 0;
    stlBool                         sExist;
    smtExtBlockDesc               * sExtBlockDesc;

    *aTupleExist = STL_FALSE;
    
    stlMemcpy( &sPathCtrl, aPathCtrl, STL_SIZEOF(smtTbsExtBlockDescPathCtrl) );

    STL_TRY( smpAcquire( NULL,
                         sPathCtrl.mExtBlockMapRid.mTbsId,
                         sPathCtrl.mExtBlockMapRid.mPageId,
                         KNL_LATCH_MODE_SHARED,
                         &sPageHandle,
                         (smlEnv*)aEnv ) == STL_SUCCESS );
    sState = 1;
    
    sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
    sExtBlockMap = (smtExtBlockMap*)((stlChar*)sExtBlockMapHdr +
                                     STL_SIZEOF( smtExtBlockMapHdr ));

    sPathCtrl.mExtBlockMapRid.mOffset++;

    if( sPathCtrl.mExtBlockMapRid.mOffset >= sExtBlockMapHdr->mExtBlockDescCount )
    {
        sState = 0;
        STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
            
        while( 1 )
        {
            STL_TRY( smfNextDatafile( sPathCtrl.mExtBlockMapRid.mTbsId,
                                      &sPathCtrl.mDatafileId,
                                      &sExist,
                                      SML_ENV(aEnv) )
                     == STL_SUCCESS );

            STL_TRY_THROW( sExist == STL_TRUE, RAMP_SUCCESS );

            if( smfGetDatafileState( sPathCtrl.mExtBlockMapRid.mTbsId,
                                     sPathCtrl.mDatafileId )
                == SMF_DATAFILE_STATE_CREATING )
            {
                /**
                 * 아직 만들어 지지 않은 datafile 이다.
                 */
                continue;
            }
        }

        STL_TRY( smfNextDatafile( sPathCtrl.mExtBlockMapRid.mTbsId,
                                  &sPathCtrl.mDatafileId,
                                  &sExist,
                                  SML_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sExist == STL_TRUE, RAMP_SUCCESS );

        sPathCtrl.mExtBlockMapRid.mPageId = SMP_MAKE_PID( sPathCtrl.mDatafileId,
                                                          SMT_EXT_BLOCK_MAP_PAGE_SEQ );
        sPathCtrl.mExtBlockMapRid.mOffset = 0;
        
        STL_TRY( smpAcquire( NULL,
                             sPathCtrl.mExtBlockMapRid.mTbsId,
                             sPathCtrl.mExtBlockMapRid.mPageId,
                             KNL_LATCH_MODE_SHARED,
                             &sPageHandle,
                             (smlEnv*)aEnv ) == STL_SUCCESS );
        sState = 1;

        sExtBlockMapHdr = (smtExtBlockMapHdr*)smpGetBody( SMP_FRAME( &sPageHandle ) );
        sExtBlockMap = (smtExtBlockMap*)((stlChar*)sExtBlockMapHdr +
                                         STL_SIZEOF( smtExtBlockMapHdr ));
    }

    sExtBlockDesc = &sExtBlockMap->mExtBlockDesc[sPathCtrl.mExtBlockMapRid.mOffset];
        
    sFxTbsExtBlockDesc.mTbsId = sPathCtrl.mExtBlockMapRid.mTbsId;
    sFxTbsExtBlockDesc.mDatafileId = sPathCtrl.mDatafileId;
    sFxTbsExtBlockDesc.mExtBlockPid = sExtBlockDesc->mExtBlock;
    sFxTbsExtBlockDesc.mSeqId = sPathCtrl.mExtBlockMapRid.mOffset;

    if( (sExtBlockDesc->mState & SMT_EBS_PREPARED) == SMT_EBS_PREPARED )
    {
        sFxTbsExtBlockDesc.mPrepared = 1;
    }
    else
    {
        sFxTbsExtBlockDesc.mPrepared = 0;
    }

    switch( sExtBlockDesc->mState & SMT_EBS_MASK )
    {
        case SMT_EBS_UNALLOCATED:
            stlStrncpy( sFxTbsExtBlockDesc.mState, "UNALLOCATED", 16 );
            break;
        case SMT_EBS_FREE:
            stlStrncpy( sFxTbsExtBlockDesc.mState, "FREE", 16 );
            break;
        case SMT_EBS_FULL:
            stlStrncpy( sFxTbsExtBlockDesc.mState, "FULL", 16 );
            break;
        default:
            stlStrncpy( sFxTbsExtBlockDesc.mState, "UNKNOWN", 16 );
            break;
    }
        
    sState = 0;
    STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
            
    STL_TRY( knlBuildFxRecord( gTbsExtBlockDescColumnDesc,
                               &sFxTbsExtBlockDesc,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );
        
    *aTupleExist = STL_TRUE;

    stlMemcpy( aPathCtrl, &sPathCtrl, STL_SIZEOF(smtTbsExtBlockDescPathCtrl) );

    STL_RAMP( RAMP_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)smpRelease( &sPageHandle, (smlEnv*)aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

knlFxTableDesc gTbsExtBlockDescTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    smtGetDumpObjectCallback,
    smtOpenTbsExtBlockDescCallback,
    smtCloseTbsExtBlockDescCallback,
    smtBuildRecordTbsExtBlockDescCallback,
    STL_SIZEOF( smtTbsExtBlockDescPathCtrl ),
    "D$TABLESPACE_EXTENT_BLOCK_DESC",
    "tablespace extent block descriptor information",
    gTbsExtBlockDescColumnDesc
};

/** @} */
