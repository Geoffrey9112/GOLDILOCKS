/*******************************************************************************
 * smnFxTable.c
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
#include <sme.h>
#include <smp.h>
#include <smn.h>
#include <sms.h>
#include <smnmpb.h>

extern smsWarmupEntry  * gSmsWarmupEntry;

/**
 * @file smnFxTable.c
 * @brief Storage Manager Layer Index Internal Routines
 */

/**
 * @addtogroup smnFxTable
 * @{
 */

knlFxColumnDesc gSmnIndexHdrColumnDesc[] =
{
    {
        "TBS_ID",
        "tablespace identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnFxIndexHdrRec, mTbsId ),
        STL_SIZEOF( smlTbsId ),
        STL_FALSE  /* nullable */
    },
    {
        "PHYSICAL_ID",
        "segment physical id",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnFxIndexHdrRec, mPhysicalId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_LOGGING",
        "logging",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smnFxIndexHdrRec, mLoggingFlag ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "INITRANS",
        "initial transaction count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnFxIndexHdrRec, mIniTrans ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "MAXTRANS",
        "maximum transaction count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnFxIndexHdrRec, mMaxTrans ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "PCTFREE",
        "percentage of free space",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnFxIndexHdrRec, mPctFree ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_UNIQUE",
        "uniqueness",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smnFxIndexHdrRec, mUniqueness ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_PRIMARY",
        "primary",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smnFxIndexHdrRec, mPrimary ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "COLUMN_COUNT",
        "column count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnFxIndexHdrRec, mKeyColCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "BASE_TABLE_SEG_RID",
        "base table segment rid",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnFxIndexHdrRec, mBaseTableSegRid ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "ROOT_PAGE_ID",
        "root page id",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnFxIndexHdrRec, mRootPageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "MIRROR_ROOT_PAGE_ID",
        "mirrored root page id",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnFxIndexHdrRec, mMirrorRootPid ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_USABLE",
        "is usable",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smnFxIndexHdrRec, mIsUsable ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "EMPTY_NODE_COUNT",
        "empty node count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnFxIndexHdrRec, mEmptyNodeCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "FIRST_EMPTY_NODE",
        "first empty node",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnFxIndexHdrRec, mFirstEmptyNode ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "LAST_EMPTY_NODE",
        "last empty node",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnFxIndexHdrRec, mLastEmptyNode ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "INDEX_SMO_NO",
        "index smo number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnFxIndexHdrRec, mIndexSmoNo ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_AGING_EMPTY_NODE",
        "aging empty node flag",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smnFxIndexHdrRec, mIsAgingEmptyNode ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "BASE_RELATION_HANDLE",
        "base relation handle",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnFxIndexHdrRec, mBaseRelHandle ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "COLUMN_TYPES",
        "column types",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smnFxIndexHdrRec, mKeyColTypes ),
        SMP_PAGE_SIZE / 4,
        STL_FALSE  /* nullable */
    },
    {
        "COLUMN_FLAGS",
        "column flags",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smnFxIndexHdrRec, mKeyColFlags ),
        SMP_PAGE_SIZE / 4,
        STL_FALSE  /* nullable */
    },
    {
        "COLUMN_OREDERS",
        "column orders",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smnFxIndexHdrRec, mKeyColOrder ),
        SMP_PAGE_SIZE / 4,
        STL_FALSE  /* nullable */
    },
    {
        "COLUMN_SIZES",
        "column sizes",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smnFxIndexHdrRec, mKeyColSize ),
        SMP_PAGE_SIZE / 4,
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


stlStatus smnOpenIndexHdrCallback( void   * aStmt,
                                   void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv )
{
    smnFxIndexHdrPathCtrl * sPathCtrl;
    
    sPathCtrl = (smnFxIndexHdrPathCtrl*)aPathCtrl;
    
    STL_TRY( smsAllocSegMapIterator( (smlStatement*)aStmt,
                                     &sPathCtrl->mSegIterator,
                                     SML_ENV( aEnv ) )
             == STL_SUCCESS );

    sPathCtrl->mFetchStarted = STL_FALSE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnCloseIndexHdrCallback( void   * aDumpObjHandle,
                                    void   * aPathCtrl,
                                    knlEnv * aEnv )
{
    smnFxIndexHdrPathCtrl * sPathCtrl;
    
    sPathCtrl = (smnFxIndexHdrPathCtrl*)aPathCtrl;
    
    STL_TRY( smsFreeSegMapIterator( sPathCtrl->mSegIterator,
                                    SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnBuildRecordIndexHdrCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aTupleExist,
                                          knlEnv            * aEnv )
{
    smnFxIndexHdrPathCtrl * sPathCtrl;
    void                  * sSegmentHandle;
    smnFxIndexHdrRec        sFixedRecord;
    smlRid                  sSegmentRid;
    smnIndexHeader        * sHdr;
    stlBool                 sHasLatch = STL_FALSE;
    smpHandle               sPageHandle;
    stlInt32                sRelType;
    stlInt32                i;
    stlChar                 sBuffer[1024];
    
    *aTupleExist = STL_FALSE;
    sPathCtrl = (smnFxIndexHdrPathCtrl*)aPathCtrl;

    while( 1 )
    {
        if( sPathCtrl->mFetchStarted == STL_FALSE )
        {
            STL_TRY( smsGetFirstSegment( sPathCtrl->mSegIterator,
                                         &sSegmentHandle,
                                         &sSegmentRid,
                                         SML_ENV( aEnv ) )
                     == STL_SUCCESS);

            sPathCtrl->mFetchStarted = STL_TRUE;
        }
        else
        {
            STL_TRY( smsGetNextSegment( sPathCtrl->mSegIterator,
                                        &sSegmentHandle,
                                        &sSegmentRid,
                                        SML_ENV( aEnv ) )
                     == STL_SUCCESS);
        }
    
        if( sSegmentHandle == NULL )
        {
            break;
        }
    
        if( smsHasRelHdr( sSegmentHandle ) == STL_TRUE )
        {
            STL_TRY( smpAcquire( NULL,
                                 sSegmentRid.mTbsId,
                                 sSegmentRid.mPageId,
                                 KNL_LATCH_MODE_EXCLUSIVE,
                                 &sPageHandle,
                                 (smlEnv*)aEnv ) == STL_SUCCESS );
            sHasLatch = STL_TRUE;

            sHdr = (smnIndexHeader*)((stlChar*)smpGetBody(SMP_FRAME(&sPageHandle)) +
                                     smsGetHeaderSize(sSegmentHandle));

            sRelType = SME_GET_RELATION_TYPE( sHdr );

            if( SML_RELATION_IS_PERSISTENT_INDEX( sRelType ) == STL_TRUE )
            {
                break;
            }

            sHasLatch = STL_FALSE;
            STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
        }
    }

    if( sSegmentHandle != NULL )
    {
        sFixedRecord.mTbsId = smsGetTbsId( sSegmentHandle );
        sFixedRecord.mPhysicalId = smsGetSegmentId( sSegmentHandle );
        
        sFixedRecord.mLoggingFlag = sHdr->mLoggingFlag;
        sFixedRecord.mIniTrans = sHdr->mIniTrans;
        sFixedRecord.mMaxTrans = sHdr->mMaxTrans;
        sFixedRecord.mPctFree = sHdr->mPctFree;
        sFixedRecord.mUniqueness = sHdr->mUniqueness;
        sFixedRecord.mPrimary = sHdr->mPrimary;
        sFixedRecord.mRootPageId = sHdr->mRootPageId;
        sFixedRecord.mMirrorRootPid = sHdr->mMirrorRootPid;
        sFixedRecord.mIsUsable = smsIsUsableSegment( sSegmentHandle );
        sFixedRecord.mKeyColCount = sHdr->mKeyColCount;
        sFixedRecord.mEmptyNodeCount = sHdr->mEmptyNodeCount;
        sFixedRecord.mFirstEmptyNode = sHdr->mFirstEmptyNode;
        sFixedRecord.mLastEmptyNode = sHdr->mLastEmptyNode;
        sFixedRecord.mIndexSmoNo = sHdr->mIndexSmoNo;
        sFixedRecord.mIsAgingEmptyNode = sHdr->mIsAgingEmptyNode;
        sFixedRecord.mBaseRelHandle = (stlInt64)sHdr->mBaseRelHandle;

        sFixedRecord.mKeyColTypes[0] = 0;
        sFixedRecord.mKeyColFlags[0] = 0;
        sFixedRecord.mKeyColOrder[0] = 0;
        sFixedRecord.mKeyColSize[0] = 0;
        
        for( i = 0; i < sHdr->mKeyColCount - 1; i++ )
        {
            stlSnprintf( sBuffer, 1024, "%d,", sHdr->mKeyColTypes[i] );
            stlStrnncat( sFixedRecord.mKeyColTypes,
                         sBuffer,
                         SMP_PAGE_SIZE / 4,
                         stlStrlen(sBuffer) + 1 );
            
            stlSnprintf( sBuffer, 1024, "%d,", sHdr->mKeyColFlags[i] );
            stlStrnncat( sFixedRecord.mKeyColFlags,
                         sBuffer,
                         SMP_PAGE_SIZE / 4,
                         stlStrlen(sBuffer) + 1 );
            
            stlSnprintf( sBuffer, 1024, "%d,", sHdr->mKeyColOrder[i] );
            stlStrnncat( sFixedRecord.mKeyColOrder,
                         sBuffer,
                         SMP_PAGE_SIZE / 4,
                         stlStrlen(sBuffer) + 1 );
            
            stlSnprintf( sBuffer, 1024, "%ld,", sHdr->mKeyColSize[i] );
            stlStrnncat( sFixedRecord.mKeyColSize,
                         sBuffer,
                         SMP_PAGE_SIZE / 4,
                         stlStrlen(sBuffer) + 1 );
        }
        
        stlSnprintf( sBuffer, 1024, "%d", sHdr->mKeyColTypes[i] );
        stlStrnncat( sFixedRecord.mKeyColTypes,
                     sBuffer,
                     SMP_PAGE_SIZE / 4,
                     stlStrlen(sBuffer) + 1 );
            
        stlSnprintf( sBuffer, 1024, "%d", sHdr->mKeyColFlags[i] );
        stlStrnncat( sFixedRecord.mKeyColFlags,
                     sBuffer,
                     SMP_PAGE_SIZE / 4,
                     stlStrlen(sBuffer) + 1 );
            
        stlSnprintf( sBuffer, 1024, "%d", sHdr->mKeyColOrder[i] );
        stlStrnncat( sFixedRecord.mKeyColOrder,
                     sBuffer,
                     SMP_PAGE_SIZE / 4,
                     stlStrlen(sBuffer) + 1 );
            
        stlSnprintf( sBuffer, 1024, "%ld", sHdr->mKeyColSize[i] );
        stlStrnncat( sFixedRecord.mKeyColSize,
                     sBuffer,
                     SMP_PAGE_SIZE / 4,
                     stlStrlen(sBuffer) + 1 );
        
        STL_TRY( knlBuildFxRecord( gSmnIndexHdrColumnDesc,
                                   (void*)&sFixedRecord,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );
    
        *aTupleExist = STL_TRUE;
    
        sHasLatch = STL_FALSE;
        STL_TRY( smpRelease( &sPageHandle, (smlEnv*)aEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sHasLatch == STL_TRUE )
    {
        (void)smpRelease( &sPageHandle, (smlEnv*)aEnv );
    }

    return STL_FAILURE;
}


knlFxTableDesc gSmnIndexHdrTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    NULL,
    smnOpenIndexHdrCallback,
    smnCloseIndexHdrCallback,
    smnBuildRecordIndexHdrCallback,
    STL_SIZEOF( smnFxIndexHdrPathCtrl ),
    "X$INDEX_HEADER",
    "Index Relation Header",
    gSmnIndexHdrColumnDesc
};





knlFxColumnDesc gSmnIndexCacheColumnDesc[] =
{
    {
        "TBS_ID",
        "tablespace identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnFxIndexCacheRec, mTbsId ),
        STL_SIZEOF( smlTbsId ),
        STL_FALSE  /* nullable */
    },
    {
        "PHYSICAL_ID",
        "segment physical id",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnFxIndexCacheRec, mPhysicalId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_LOGGING",
        "logging",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smnFxIndexCacheRec, mLoggingFlag ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "INITRANS",
        "initial transaction count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnFxIndexCacheRec, mIniTrans ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "MAXTRANS",
        "maximum transaction count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnFxIndexCacheRec, mMaxTrans ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_UNIQUE",
        "uniqueness",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smnFxIndexCacheRec, mUniqueness ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_PRIMARY",
        "primary",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smnFxIndexCacheRec, mPrimary ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "COLUMN_COUNT",
        "column count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smnFxIndexCacheRec, mKeyColCount ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "BASE_TABLE_SEG_RID",
        "base table segment rid",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnFxIndexCacheRec, mBaseTableSegRid ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "ROOT_PAGE_ID",
        "root page id",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnFxIndexCacheRec, mRootPageId ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "EMPTY_NODE_COUNT",
        "empty node count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnFxIndexCacheRec, mEmptyNodeCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "FIRST_EMPTY_NODE",
        "first empty node",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnFxIndexCacheRec, mFirstEmptyNode ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "LAST_EMPTY_NODE",
        "last empty node",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smnFxIndexCacheRec, mLastEmptyNode ),
        STL_SIZEOF( smlPid ),
        STL_FALSE  /* nullable */
    },
    {
        "INDEX_SMO_NO",
        "index smo number",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnFxIndexCacheRec, mIndexSmoNo ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "IS_AGING_EMPTY_NODE",
        "aging empty node flag",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smnFxIndexCacheRec, mIsAgingEmptyNode ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "BASE_RELATION_HANDLE",
        "base relation handle",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smnFxIndexCacheRec, mBaseRelHandle ),
        STL_SIZEOF( stlUInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "COLUMN_TYPES",
        "column types",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smnFxIndexCacheRec, mKeyColTypes ),
        SMP_PAGE_SIZE / 4,
        STL_FALSE  /* nullable */
    },
    {
        "COLUMN_FLAGS",
        "column flags",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smnFxIndexCacheRec, mKeyColFlags ),
        SMP_PAGE_SIZE / 4,
        STL_FALSE  /* nullable */
    },
    {
        "COLUMN_OREDERS",
        "column orders",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smnFxIndexCacheRec, mKeyColOrder ),
        SMP_PAGE_SIZE / 4,
        STL_FALSE  /* nullable */
    },
    {
        "COLUMN_SIZES",
        "column sizes",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smnFxIndexCacheRec, mKeyColSize ),
        SMP_PAGE_SIZE / 4,
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


stlStatus smnOpenIndexCacheCallback( void   * aStmt,
                                     void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    smnFxIndexCachePathCtrl * sPathCtrl;
    
    sPathCtrl = (smnFxIndexCachePathCtrl*)aPathCtrl;
    
    STL_TRY( smsAllocSegMapIterator( (smlStatement*)aStmt,
                                     &sPathCtrl->mSegIterator,
                                     SML_ENV( aEnv ) )
             == STL_SUCCESS );

    sPathCtrl->mFetchStarted = STL_FALSE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnCloseIndexCacheCallback( void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aEnv )
{
    smnFxIndexCachePathCtrl * sPathCtrl;
    
    sPathCtrl = (smnFxIndexCachePathCtrl*)aPathCtrl;
    
    STL_TRY( smsFreeSegMapIterator( sPathCtrl->mSegIterator,
                                    SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smnBuildRecordIndexCacheCallback( void              * aDumpObjHandle,
                                            void              * aPathCtrl,
                                            knlValueBlockList * aValueList,
                                            stlInt32            aBlockIdx,
                                            stlBool           * aTupleExist,
                                            knlEnv            * aEnv )
{
    smnFxIndexCachePathCtrl * sPathCtrl;
    void                    * sSegmentHandle = NULL;
    void                    * sRelationHandle = NULL;
    smnFxIndexCacheRec        sFixedRecord;
    smlRid                    sSegmentRid;
    smnIndexHeader          * sCache;
    stlInt32                  sRelType;
    stlInt32                  i;
    stlChar                   sBuffer[1024];
    
    *aTupleExist = STL_FALSE;
    sPathCtrl = (smnFxIndexCachePathCtrl*)aPathCtrl;

    while( 1 )
    {
        if( sPathCtrl->mFetchStarted == STL_FALSE )
        {
            STL_TRY( smsGetFirstSegment( sPathCtrl->mSegIterator,
                                         &sSegmentHandle,
                                         &sSegmentRid,
                                         SML_ENV( aEnv ) )
                     == STL_SUCCESS);
        
            sPathCtrl->mFetchStarted = STL_TRUE;
        }
        else
        {
            STL_TRY( smsGetNextSegment( sPathCtrl->mSegIterator,
                                        &sSegmentHandle,
                                        &sSegmentRid,
                                        SML_ENV( aEnv ) ) == STL_SUCCESS);
        }
    
        if( sSegmentHandle == NULL )
        {
            break;
        }
    
        if( smsHasRelHdr( sSegmentHandle ) == STL_TRUE )
        {
            STL_TRY( smeGetRelationHandle( sSegmentRid,
                                           &sRelationHandle,
                                           SML_ENV( aEnv ) ) == STL_SUCCESS );

            if( sRelationHandle != NULL )
            {
                sRelType = SME_GET_RELATION_TYPE( sRelationHandle );

                if( SML_RELATION_IS_PERSISTENT_INDEX( sRelType ) == STL_TRUE )
                {
                    break;
                }
            }
        }
    }

    if( sSegmentHandle != NULL )
    {
        STL_DASSERT( sRelationHandle != NULL );
        
        sCache = (smnIndexHeader*)sRelationHandle;
        sSegmentHandle = SME_GET_SEGMENT_HANDLE( sRelationHandle );
        
        sFixedRecord.mTbsId = smsGetTbsId( sSegmentHandle );
        sFixedRecord.mPhysicalId = smsGetSegmentId( sSegmentHandle );
        
        sFixedRecord.mLoggingFlag = sCache->mLoggingFlag;
        sFixedRecord.mIniTrans = sCache->mIniTrans;
        sFixedRecord.mMaxTrans = sCache->mMaxTrans;
        sFixedRecord.mUniqueness = sCache->mUniqueness;
        sFixedRecord.mPrimary = sCache->mPrimary;
        sFixedRecord.mRootPageId = sCache->mRootPageId;
        sFixedRecord.mKeyColCount = sCache->mKeyColCount;
        sFixedRecord.mEmptyNodeCount = sCache->mEmptyNodeCount;
        sFixedRecord.mFirstEmptyNode = sCache->mFirstEmptyNode;
        sFixedRecord.mLastEmptyNode = sCache->mLastEmptyNode;
        sFixedRecord.mIndexSmoNo = sCache->mIndexSmoNo;
        sFixedRecord.mIsAgingEmptyNode = sCache->mIsAgingEmptyNode;
        sFixedRecord.mBaseRelHandle = (stlInt64)sCache->mBaseRelHandle;

        sFixedRecord.mKeyColTypes[0] = 0;
        sFixedRecord.mKeyColFlags[0] = 0;
        sFixedRecord.mKeyColOrder[0] = 0;
        sFixedRecord.mKeyColSize[0] = 0;
        
        for( i = 0; i < sCache->mKeyColCount - 1; i++ )
        {
            stlSnprintf( sBuffer, 1024, "%d,", sCache->mKeyColTypes[i] );
            stlStrnncat( sFixedRecord.mKeyColTypes,
                         sBuffer,
                         SMP_PAGE_SIZE / 4,
                         stlStrlen(sBuffer) + 1 );
            
            stlSnprintf( sBuffer, 1024, "%d,", sCache->mKeyColFlags[i] );
            stlStrnncat( sFixedRecord.mKeyColFlags,
                         sBuffer,
                         SMP_PAGE_SIZE / 4,
                         stlStrlen(sBuffer) + 1 );
            
            stlSnprintf( sBuffer, 1024, "%d,", sCache->mKeyColOrder[i] );
            stlStrnncat( sFixedRecord.mKeyColOrder,
                         sBuffer,
                         SMP_PAGE_SIZE / 4,
                         stlStrlen(sBuffer) + 1 );
            
            stlSnprintf( sBuffer, 1024, "%ld,", sCache->mKeyColSize[i] );
            stlStrnncat( sFixedRecord.mKeyColSize,
                         sBuffer,
                         SMP_PAGE_SIZE / 4,
                         stlStrlen(sBuffer) + 1 );
        }
        
        stlSnprintf( sBuffer, 1024, "%d", sCache->mKeyColTypes[i] );
        stlStrnncat( sFixedRecord.mKeyColTypes,
                     sBuffer,
                     SMP_PAGE_SIZE / 4,
                     stlStrlen(sBuffer) + 1 );
            
        stlSnprintf( sBuffer, 1024, "%d", sCache->mKeyColFlags[i] );
        stlStrnncat( sFixedRecord.mKeyColFlags,
                     sBuffer,
                     SMP_PAGE_SIZE / 4,
                     stlStrlen(sBuffer) + 1 );
            
        stlSnprintf( sBuffer, 1024, "%d", sCache->mKeyColOrder[i] );
        stlStrnncat( sFixedRecord.mKeyColOrder,
                     sBuffer,
                     SMP_PAGE_SIZE / 4,
                     stlStrlen(sBuffer) + 1 );
            
        stlSnprintf( sBuffer, 1024, "%ld", sCache->mKeyColSize[i] );
        stlStrnncat( sFixedRecord.mKeyColSize,
                     sBuffer,
                     SMP_PAGE_SIZE / 4,
                     stlStrlen(sBuffer) + 1 );
        
        STL_TRY( knlBuildFxRecord( gSmnIndexCacheColumnDesc,
                                   (void*)&sFixedRecord,
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


knlFxTableDesc gSmnIndexCacheTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    NULL,
    smnOpenIndexCacheCallback,
    smnCloseIndexCacheCallback,
    smnBuildRecordIndexCacheCallback,
    STL_SIZEOF( smnFxIndexCachePathCtrl ),
    "X$INDEX_CACHE",
    "Index Relation Cache",
    gSmnIndexCacheColumnDesc
};




/** @} */
