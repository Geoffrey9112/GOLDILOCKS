/*******************************************************************************
 * smdFxTable.c
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
#include <sms.h>
#include <smdDef.h>

extern smsWarmupEntry  * gSmsWarmupEntry;

/**
 * @file smdFxTable.c
 * @brief Storage Manager Layer Table Internal Routines
 */

/**
 * @addtogroup smdFxTable
 * @{
 */

knlFxColumnDesc gSmdTableCacheColumnDesc[] =
{
    {
        "TBS_ID",
        "tablespace identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdFxTableCacheRec, mTbsId ),
        STL_SIZEOF( smlTbsId ),
        STL_FALSE  /* nullable */
    },
    {
        "PHYSICAL_ID",
        "segment physical id",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smdFxTableCacheRec, mPhysicalId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "TABLE_TYPE",
        "table type",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smdFxTableCacheRec, mTableType ),
        SMD_TABLE_TYPE_SIZE + 1,
        STL_FALSE  /* nullable */
    },
    {
        "LOGGING",
        "logging flag",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smdFxTableCacheRec, mLoggingFlag ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "LOCKING",
        "logging flag",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smdFxTableCacheRec, mLockingFlag ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "INITRANS",
        "initial transaction count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdFxTableCacheRec, mIniTrans ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "MAXTRANS",
        "maximum transaction count",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdFxTableCacheRec, mMaxTrans ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "PCTFREE",
        "the percentage of space in page reserved for future updates",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdFxTableCacheRec, mPctFree ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "PCTUSED",
        "the minumim percentage of used space",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdFxTableCacheRec, mPctUsed ),
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


stlStatus smdOpenTableCacheCallback( void   * aStmt,
                                     void   * aDumpObjHandle,
                                     void   * aPathCtrl,
                                     knlEnv * aEnv )
{
    smdFxTableCachePathCtrl * sPathCtrl;
    
    sPathCtrl = (smdFxTableCachePathCtrl*)aPathCtrl;
    
    STL_TRY( smsAllocSegMapIterator( (smlStatement*)aStmt,
                                     &sPathCtrl->mSegIterator,
                                     SML_ENV( aEnv ) )
             == STL_SUCCESS );

    sPathCtrl->mFetchStarted = STL_FALSE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdCloseTableCacheCallback( void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aEnv )
{
    smdFxTableCachePathCtrl * sPathCtrl;
    
    sPathCtrl = (smdFxTableCachePathCtrl*)aPathCtrl;
    
    STL_TRY( smsFreeSegMapIterator( sPathCtrl->mSegIterator,
                                    SML_ENV( aEnv ) )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdBuildRecordTableCacheCallback( void              * aDumpObjHandle,
                                            void              * aPathCtrl,
                                            knlValueBlockList * aValueList,
                                            stlInt32            aBlockIdx,
                                            stlBool           * aTupleExist,
                                            knlEnv            * aEnv )
{
    smdFxTableCachePathCtrl * sPathCtrl;
    void                    * sSegmentHandle;
    void                    * sRelationHandle = NULL;
    smdFxTableCacheRec        sFixedRecord;
    smlRid                    sSegmentRid;
    smdTableHeader          * sCache;
    stlInt32                  sRelType;
    
    *aTupleExist = STL_FALSE;
    sPathCtrl = (smdFxTableCachePathCtrl*)aPathCtrl;

    while( 1 )
    {
        if( sPathCtrl->mFetchStarted == STL_FALSE )
        {
            STL_TRY( smsGetFirstSegment( sPathCtrl->mSegIterator,
                                         &sSegmentHandle,
                                         &sSegmentRid,
                                         SML_ENV( aEnv ) )
                     == STL_SUCCESS );

            sPathCtrl->mFetchStarted = STL_TRUE;
        }
        else
        {
            STL_TRY( smsGetNextSegment( sPathCtrl->mSegIterator,
                                        &sSegmentHandle,
                                        &sSegmentRid,
                                        SML_ENV( aEnv ) )
                     == STL_SUCCESS );
        }

        if( sSegmentHandle == NULL )
        {
            break;
        }
    
        if( smsHasRelHdr( sSegmentHandle ) == STL_TRUE )
        {
            STL_TRY( smeGetRelationHandle( sSegmentRid,
                                           &sRelationHandle,
                                           SML_ENV( aEnv ) )
                     == STL_SUCCESS );

            if( sRelationHandle != NULL )
            {
                sRelType = SME_GET_RELATION_TYPE( sRelationHandle );

                if( SML_RELATION_IS_PERSISTENT_TABLE( sRelType ) == STL_TRUE )
                {
                    break;
                }
            }
        }
    }

    if( sSegmentHandle != NULL )
    {
        STL_DASSERT( sRelationHandle != NULL );
        
        sCache = (smdTableHeader*)sRelationHandle;
        sSegmentHandle = SME_GET_SEGMENT_HANDLE( sRelationHandle );
        
        sFixedRecord.mTbsId = smsGetTbsId( sSegmentHandle );
        sFixedRecord.mPhysicalId = smsGetSegmentId( sSegmentHandle );

        switch( SME_GET_RELATION_TYPE( sRelationHandle ) )
        {
            case SML_MEMORY_PERSISTENT_HEAP_ROW_TABLE:
                stlStrncpy( sFixedRecord.mTableType, "HEAP_ROW", SMD_TABLE_TYPE_SIZE );
                break;
            case SML_MEMORY_PERSISTENT_HEAP_COLUMNAR_TABLE:
                stlStrncpy( sFixedRecord.mTableType, "HEAP_COLUMNAR", SMD_TABLE_TYPE_SIZE );
                break;
            case SML_MEMORY_PERSISTENT_PENDING_TABLE:
                stlStrncpy( sFixedRecord.mTableType, "PENDING", SMD_TABLE_TYPE_SIZE );
                break;
            default:
                stlStrncpy( sFixedRecord.mTableType, "UNKNOWN", SMD_TABLE_TYPE_SIZE );
                break;
        }
        
        sFixedRecord.mLoggingFlag = sCache->mLoggingFlag;
        sFixedRecord.mLockingFlag = sCache->mLockingFlag;
        sFixedRecord.mIniTrans = sCache->mIniTrans;
        sFixedRecord.mMaxTrans = sCache->mMaxTrans;
        sFixedRecord.mPctFree = sCache->mPctFree;
        sFixedRecord.mPctUsed = sCache->mPctUsed;
        
        STL_TRY( knlBuildFxRecord( gSmdTableCacheColumnDesc,
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


knlFxTableDesc gSmdTableCacheTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_OPEN,
    NULL,
    smdOpenTableCacheCallback,
    smdCloseTableCacheCallback,
    smdBuildRecordTableCacheCallback,
    STL_SIZEOF( smdFxTableCachePathCtrl ),
    "X$TABLE_CACHE",
    "Table Relation Cache",
    gSmdTableCacheColumnDesc
};


/** @} */
