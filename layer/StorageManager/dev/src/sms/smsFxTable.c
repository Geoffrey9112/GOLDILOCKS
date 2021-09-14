/*******************************************************************************
 * smsFxTable.c
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
#include <smsDef.h>

extern smsWarmupEntry  * gSmsWarmupEntry;
extern smsSegmentFuncs * gSegmentFuncs[];
/**
 * @file smsFxTable.c
 * @brief Storage Manager Layer Memory Segment Fixed Table Internal Routines
 */

/**
 * @addtogroup smsFxTable
 * @{
 */

knlFxColumnDesc gSmsSegmentColumnDesc[] =
{
    {
        "SEGMENT_TYPE",
        "segment type",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smsFxSegment, mSegType ),
        20,
        STL_FALSE  /* nullable */
    },
    {
        "TBS_ID",
        "tablespace identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smsFxSegment, mTbsId ),
        STL_SIZEOF( smlTbsId ),
        STL_FALSE  /* nullable */
    },
    {
        "PHYSICAL_ID",
        "segment physical id",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smsFxSegment, mSegmentId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "ALLOC_PAGE_COUNT",
        "allocated page count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smsFxSegment, mAllocPageCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "HAS_RELATION_HEADER",
        "has relation header",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smsFxSegment, mHasRelHdr ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "USABLE",
        "usable",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smsFxSegment, mIsUsable ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "STATE",
        "state",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smsFxSegment, mState ),
        20,
        STL_FALSE  /* nullable */
    },
    {
        "VALID_SCN",
        "valid scn",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smsFxSegment, mValidScn ),
        STL_SIZEOF(smlScn),
        STL_FALSE  /* nullable */
    },
    {
        "OBJECT_SEQ",
        "object sequence",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smsFxSegment, mValidSeq ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "USABLE_PAGE_COUNT_IN_EXTENT",
        "usable page count in extent",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smsFxSegment, mUsablePageCountInExt ),
        STL_SIZEOF(stlInt64),
        STL_FALSE  /* nullable */
    },
    {
        "INITIAL_EXTENTS",
        "initial extents",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smsFxSegment, mInitialExtents ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "NEXT_EXTENTS",
        "next extents",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smsFxSegment, mNextExtents ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "MIN_EXTENTS",
        "minmum extents",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smsFxSegment, mMinExtents ),
        STL_SIZEOF(stlInt32),
        STL_FALSE  /* nullable */
    },
    {
        "MAX_EXTENTS",
        "maximum extents",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smsFxSegment, mMaxExtents ),
        STL_SIZEOF(stlInt32),
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

stlStatus smsOpenSegmentCallback( void   * aStmt,
                                  void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv )
{
    smsSegmentPathCtrl * sPathCtrl;
    
    sPathCtrl = (smsSegmentPathCtrl*)aPathCtrl;
    
    STL_TRY( smsAllocSegMapIterator( (smlStatement*)aStmt,
                                     &sPathCtrl->mSegIterator,
                                     SML_ENV( aEnv ) )
             == STL_SUCCESS );

    sPathCtrl->mFetchStarted = STL_FALSE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsCloseSegmentCallback( void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv )
{
    smsSegmentPathCtrl * sPathCtrl;
    
    sPathCtrl = (smsSegmentPathCtrl*)aPathCtrl;
    
    STL_TRY( smsFreeSegMapIterator( sPathCtrl->mSegIterator,
                                    SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smsBuildRecordSegmentCallback( void              * aDumpObjHandle,
                                         void              * aPathCtrl,
                                         knlValueBlockList * aValueList,
                                         stlInt32            aBlockIdx,
                                         stlBool           * aTupleExist,
                                         knlEnv            * aEnv )
{
    smsSegmentPathCtrl * sPathCtrl;
    void               * sSegmentHandle;
    smsFxSegment         sSegment;
    smlRid               sSegmentRid;
    smsCommonCache     * sCache;
    stlInt8              sSegState;
    
    *aTupleExist = STL_FALSE;
    sPathCtrl = (smsSegmentPathCtrl*)aPathCtrl;

    if( sPathCtrl->mFetchStarted == STL_FALSE )
    {
        STL_TRY( smsGetFirstSegment( sPathCtrl->mSegIterator,
                                     &sSegmentHandle,
                                     &sSegmentRid,
                                     SML_ENV( aEnv ) ) == STL_SUCCESS);
        
        sPathCtrl->mFetchStarted = STL_TRUE;
    }
    else
    {
        STL_TRY( smsGetNextSegment( sPathCtrl->mSegIterator,
                                    &sSegmentHandle,
                                    &sSegmentRid,
                                    SML_ENV( aEnv ) ) == STL_SUCCESS);
    }
    
    if( sSegmentHandle != NULL )
    {
        sCache = (smsCommonCache*)sSegmentHandle;

        switch( sCache->mSegType )
        {
            case SML_SEG_TYPE_MEMORY_BITMAP:
                stlStrncpy( sSegment.mSegType, "MEMORY_BITMAP", 21 );
                break;
            case SML_SEG_TYPE_MEMORY_CIRCULAR:
                stlStrncpy( sSegment.mSegType, "MEMORY_CIRCULAR", 21 );
                break;
            case SML_SEG_TYPE_MEMORY_PENDING:
                stlStrncpy( sSegment.mSegType, "MEMORY_PENDING", 21 );
                break;
            case SML_SEG_TYPE_DISK_BITMAP:
                stlStrncpy( sSegment.mSegType, "DISK_BITMAP", 21 );
                break;
            case SML_SEG_TYPE_DISK_CIRCULAR:
                stlStrncpy( sSegment.mSegType, "DISK_CIRCULAR", 21 );
                break;
            default:
                stlStrncpy( sSegment.mSegType, "UNKNOWN", 21 );
                break;
        }

        STL_TRY( smsGetSegmentState( sSegmentRid,
                                     &sSegState,
                                     SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        switch( sSegState )
        {
            case SMS_STATE_ALLOC_HDR:
                stlStrncpy( sSegment.mState, "ALLOC_HDR", 21 );
                break;
            case SMS_STATE_ALLOC_BODY:
                stlStrncpy( sSegment.mState, "ALLOC_BODY", 21 );
                break;
            default:
                stlStrncpy( sSegment.mState, "UNKNOWN", 21 );
                break;
        }

        sSegment.mTbsId = sCache->mTbsId;
        sSegment.mSegmentId = sCache->mSegId;
        sSegment.mHasRelHdr = sCache->mHasRelHdr;
        sSegment.mIsUsable = sCache->mIsUsable;
        sSegment.mValidScn = sCache->mValidScn;
        sSegment.mValidSeq = sCache->mValidSeq;
        sSegment.mUsablePageCountInExt = sCache->mUsablePageCountInExt;
        sSegment.mInitialExtents = sCache->mInitialExtents;
        sSegment.mNextExtents = sCache->mNextExtents;
        sSegment.mMinExtents = sCache->mMinExtents;
        sSegment.mMaxExtents = sCache->mMaxExtents;
        
        sSegment.mAllocPageCount = smsGetAllocPageCount( sSegmentHandle );
        
        STL_TRY( knlBuildFxRecord( gSmsSegmentColumnDesc,
                                   (void*)&sSegment,
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

knlFxTableDesc gSmsSegmentTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    NULL,
    smsOpenSegmentCallback,
    smsCloseSegmentCallback,
    smsBuildRecordSegmentCallback,
    STL_SIZEOF( smsSegmentPathCtrl ),
    "X$SEGMENT",
    "segment information",
    gSmsSegmentColumnDesc
};

/** @} */
