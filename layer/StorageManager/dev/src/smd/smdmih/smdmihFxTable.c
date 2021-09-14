/*******************************************************************************
 * smdmihFxTable.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:
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
#include <smp.h>
#include <smdDef.h>
#include <smsmfDef.h>
#include <smdmihDef.h>

/**
 * @file smdmihFxTable.c
 * @brief Storage Manager Layer Memory Instant Hash Table Fixed Table Routines
 */

/**
 * @addtogroup smdmihFxTable
 * @{
 */


/*******************************************************************************
 * X$INST_HASH_TABLES 을 위한 코드
 *******************************************************************************/

knlFxColumnDesc gSmdmihFxTablesColumnDesc[] =
{
    {
        "HANDLE_ID",
        "instant table handle identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smdmihFxTables, mInstTableId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "TBS_ID",
        "tablespace identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmihFxTables, mTbsId ),
        STL_SIZEOF( smlTbsId ),
        STL_FALSE  /* nullable */
    },
    {
        "KEY_COL_COUNT",
        "key column count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmihFxTables, mKeyColCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "ROW_COL_COUNT",
        "row column count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmihFxTables, mRowColCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "UNIQUENESS",
        "uniqueness flag",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smdmihFxTables, mUniqueness ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "SKIP_NULL",
        "skip null flag",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smdmihFxTables, mSkipNull ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "ALLOC_EXT_COUNT",
        "allocated extent count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmihFxTables, mAllocExtCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "ROW_LEN",
        "row length",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmihFxTables, mRowLen ),
        STL_SIZEOF( stlInt32 ),
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

stlStatus smdmihOpenFxTablesCallback( void   * aStmt,
                                      void   * aDumpObjHandle,
                                      void   * aPathCtrl,
                                      knlEnv * aEnv )
{
    *((smdInstHandleListNode**)aPathCtrl) = SMD_GET_SESS_INST_HANDLE_LIST( aEnv );

    return STL_SUCCESS;
}

stlStatus smdmihCloseFxTablesCallback( void   * aDumpObjHandle,
                                       void   * aPathCtrl,
                                       knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smdmihBuildRecordFxTablesCallback( void              * aDumpObjHandle,
                                             void              * aPathCtrl,
                                             knlValueBlockList * aValueList,
                                             stlInt32            aBlockIdx,
                                             stlBool           * aTupleExist,
                                             knlEnv            * aEnv )
{
    smdInstHandleListNode * sHandleNode;
    smdmihFxTables          sFxTables;
    smdmihTableHeader     * sHashHeader;
    smdmifTableHeader     * sFlatHeader;

    *aTupleExist = STL_FALSE;
    sHandleNode = *(smdInstHandleListNode**)aPathCtrl;

    while( sHandleNode != NULL )
    {
        if( sHandleNode->mHandle->mRelationType != SML_MEMORY_INSTANT_HASH_TABLE )
        {
            sHandleNode = sHandleNode->mNext;
            continue;
        }
        
        sHashHeader = (smdmihTableHeader*)sHandleNode->mHandle;
        sFlatHeader = &(sHashHeader->mBaseHeader);

        sFxTables.mInstTableId = sHandleNode->mInstTableId;
        sFxTables.mTbsId = sFlatHeader->mTbsId;
        sFxTables.mKeyColCount = sHashHeader->mKeyColCount;
        sFxTables.mRowColCount = sHashHeader->mRowColCount;
        sFxTables.mRowLen = sHashHeader->mRowLen;
        sFxTables.mUniqueness = sHashHeader->mUniqueness;
        sFxTables.mSkipNull = sHashHeader->mSkipNull;

        if( sFlatHeader->mSegHandle == NULL )
        {
            sFxTables.mAllocExtCount = 0;
        }
        else
        {
            sFxTables.mAllocExtCount = SMS_SEGMENT_HANDLE_GET_ALLOC_EXT_COUNT( sFlatHeader->mSegHandle );
        }
        
        STL_TRY( knlBuildFxRecord( gSmdmihFxTablesColumnDesc,
                                   (void*)&sFxTables,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );
    
        *((smdInstHandleListNode**)aPathCtrl) = sHandleNode->mNext;
        *aTupleExist = STL_TRUE;

        break;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmdmihFxTablesTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    smdmihOpenFxTablesCallback,
    smdmihCloseFxTablesCallback,
    smdmihBuildRecordFxTablesCallback,
    STL_SIZEOF( smdInstHandleListNode* ),
    "X$INST_HASH_TABLES",
    "session instant hash table list",
    gSmdmihFxTablesColumnDesc
};


/*******************************************************************************
 * D$MEMORY_INST_HASH_ROW 을 위한 코드
 *******************************************************************************/

knlFxColumnDesc gSmdmihFxRowColumnDesc[] =
{
    {
        "DIR_SEQ",
        "directory slot sequence",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smdmihFxRow, mDirSeq ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "HASH_VALUE",
        "hash value",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmihFxRow, mHashValue ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "ROW_SEQ",
        "record sequence",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smdmihFxRow, mRowSeq ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "COLUMN_ORDER",
        "column order",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmihFxRow, mColumnOrder ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "COLUMN_DATA",
        "column data",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smdmihFxRow, mStringValue ),
        64,
        STL_TRUE  /* nullable */
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

    
stlStatus smdmihGetDumpObjectCallback( stlInt64   aTransId,
                                       void     * aStmt,
                                       stlChar  * aDumpObjName,
                                       void    ** aDumpObjHandle,
                                       knlEnv   * aEnv )
{
    stlChar               *sToken1;
    stlChar               *sToken2;
    stlChar               *sToken3;
    stlInt32               sTokenCount;
    stlInt64               sHandleId;
    smdInstHandleListNode *sNode;

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
        STL_TRY( stlStrToInt64( sToken1,
                                STL_NTS,
                                &sToken1,
                                10,
                                &sHandleId,
                                KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

        sNode = SMD_GET_SESS_INST_HANDLE_LIST( aEnv );

        while( sNode != NULL )
        {
            if( sNode->mInstTableId == sHandleId )
            {
                STL_TRY_THROW( sNode->mHandle->mRelationType ==
                               SML_MEMORY_INSTANT_HASH_TABLE,
                               RAMP_FINISH );
                
                *aDumpObjHandle = sNode->mHandle;
                break; 
            }

            sNode = sNode->mNext;
        }
    }

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmihOpenFxCallback( void   * aStmt,
                                void   * aDumpObjHandle,
                                void   * aPathCtrl,
                                knlEnv * aEnv )
{
    smdmihTableHeader   * sTableHeader;
    smdmihFxPathCtrl    * sPathCtrl;

    sTableHeader = (smdmihTableHeader*)aDumpObjHandle;
    sPathCtrl = (smdmihFxPathCtrl*)aPathCtrl;

    sPathCtrl->mDirectory = sTableHeader->mDirectory;
    sPathCtrl->mDirIdx = -1;
    sPathCtrl->mRowSeq = 0;
    sPathCtrl->mColumnOrder = sTableHeader->mRowColCount;
    sPathCtrl->mKeyLa = KNL_LOGICAL_ADDR_NULL;
    sPathCtrl->mNextKeyLa = KNL_LOGICAL_ADDR_NULL;

    return STL_SUCCESS;
}

stlStatus smdmihCloseFxCallback( void   * aDumpObjHandle,
                                 void   * aPathCtrl,
                                 knlEnv * aEnv )
{
    return STL_SUCCESS;
}

stlStatus smdmihBuildRecordFxRowCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aTupleExist,
                                          knlEnv            * aEnv )
{
    smdmihTableHeader   * sTableHeader;
    smdmihFxPathCtrl    * sPathCtrl;
    smdmihFxRow           sFxRow;
    stlUInt32             sHashValue;
    stlInt64              sStrLen;
    dtlDataValue          sDataValue;
    stlChar             * sValue = NULL;
    stlInt32              sColOffset;
    stlInt32              sColLenSize;
    stlUInt16             sColLen16;
    stlChar             * sRow;
    smdmifDefColumn     * sColumn;
    stlInt64              sColLen64;
    knlLogicalAddr        sVarColLa;
    knlLogicalAddr        sKeyLa;
    stlChar             * sVarCol = NULL;
    knlRegionMark         sRegionMark;
    stlInt32              sState = 0;
    smdmihDirSlot       * sDirSlot;

    *aTupleExist = STL_FALSE;
    sTableHeader = (smdmihTableHeader*)aDumpObjHandle;
    sPathCtrl = (smdmihFxPathCtrl*)aPathCtrl;

    if( sPathCtrl->mColumnOrder >= sTableHeader->mRowColCount )
    {
        sKeyLa = sPathCtrl->mNextKeyLa;

        while( sKeyLa == KNL_LOGICAL_ADDR_NULL )
        {
            sPathCtrl->mDirIdx += 1;

            if( sPathCtrl->mDirIdx >= sTableHeader->mBucketCount )
            {
                STL_THROW( RAMP_FINISH );
            }

            sDirSlot = &(sPathCtrl->mDirectory[sPathCtrl->mDirIdx]);
            sKeyLa = sDirSlot->mKeyLa;
        }
            
        sRow = KNL_TO_PHYSICAL_ADDR( sKeyLa );
        SMDMIH_GET_OVERFLOW_KEY( sRow, sPathCtrl->mNextKeyLa );
            
        sPathCtrl->mColumnOrder = 0;
        sPathCtrl->mKeyLa = sKeyLa;
        sPathCtrl->mRowSeq += 1;
    }

    KNL_INIT_REGION_MARK( &sRegionMark );

    sColumn = &(sTableHeader->mColumns[sPathCtrl->mColumnOrder]);
    sColOffset = sColumn->mOffset;
    sColLenSize = sTableHeader->mColumnInfo[sPathCtrl->mColumnOrder].mColLenSize;
    sRow = KNL_TO_PHYSICAL_ADDR( sPathCtrl->mKeyLa );
    SMDMIH_GET_HASH_KEY( sRow, sHashValue );

    sDataValue.mBufferSize = sColumn->mAllocSize;

    if( sColumn->mIsFixedPart == STL_TRUE )
    {
        sValue = sRow + sColOffset + sColLenSize;
    
        if( sColLenSize == 1 )
        {
            sColLen16 = *(stlUInt8*)(sRow + sColOffset);
        }
        else
        {
            STL_WRITE_INT16( &sColLen16, sRow + sColOffset );
        }
    
        sDataValue.mLength = sColLen16;
    }
    else
    {
        STL_WRITE_INT64( &sVarColLa, (sRow + sColumn->mOffset) );
                    
        if( sVarColLa == KNL_LOGICAL_ADDR_NULL )
        {
            sColLen64 = 0;
        }
        else
        {
            sVarCol = (stlChar*)KNL_TO_PHYSICAL_ADDR( sVarColLa );

            /**
             * 첫번째 Block만 출력한다.
             */
            sValue = SMDMIF_VAR_COL_VALUE_POS( sVarCol );
            sColLen64 = SMDMIF_VAR_COL_GET_LEN( sVarCol );
            
            sColLenSize = 0;
        }
        
        sDataValue.mLength = sColLen64;
    }

    STL_TRY( knlMarkRegionMem( &(SML_ENV(aEnv))->mOperationHeapMem,
                               &sRegionMark,
                               aEnv )
             == STL_SUCCESS );
    sState = 1;
    
    STL_TRY( knlAllocRegionMem( &(SML_ENV(aEnv))->mOperationHeapMem,
                                sDataValue.mBufferSize,
                                &sDataValue.mValue,
                                aEnv )
             == STL_SUCCESS );
    
    if( sValue != NULL )
    {
        stlMemcpy( sDataValue.mValue, sValue, sDataValue.mLength );
    }
    
    sDataValue.mType = sColumn->mDataType;

    if( sDataValue.mLength > 0 )
    {
        /* 사용가능한 버퍼사이즈에 맞게 잘라서 문자열로 변환한다. */
        STL_TRY( dtlToStringForAvailableSizeDataValue( &sDataValue,
                                                       DTL_PRECISION_NA,
                                                       DTL_SCALE_NA,
                                                       64,
                                                       sFxRow.mStringValue,
                                                       &sStrLen,
                                                       KNL_DT_VECTOR(aEnv),
                                                       aEnv,
                                                       KNL_ERROR_STACK(aEnv) )
                 == STL_SUCCESS );
    }
    else
    {
        sStrLen = 0;
    }
    
    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &(SML_ENV(aEnv))->mOperationHeapMem,
                                       &sRegionMark,
                                       STL_FALSE, /* aFreeChunk */
                                       aEnv )
             == STL_SUCCESS );
    
    sFxRow.mStringValue[sStrLen] = '\0';
    
    sFxRow.mHashValue = sHashValue;
    sFxRow.mRowSeq = sPathCtrl->mRowSeq;
    sFxRow.mColumnOrder = sPathCtrl->mColumnOrder;
    sFxRow.mDirSeq = sPathCtrl->mDirIdx;

    STL_TRY( knlBuildFxRecord( gSmdmihFxRowColumnDesc,
                               (void*)&sFxRow,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );
    
    sPathCtrl->mColumnOrder++;

    *aTupleExist = STL_TRUE;
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    if( sState == 1 )
    {
        (void) knlFreeUnmarkedRegionMem( &(SML_ENV(aEnv))->mOperationHeapMem,
                                         &sRegionMark,
                                         STL_FALSE, /* aFreeChunk */
                                         aEnv );
    }

    return STL_FAILURE;
}

knlFxTableDesc gSmdmihFxRowTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    smdmihGetDumpObjectCallback,
    smdmihOpenFxCallback,
    smdmihCloseFxCallback,
    smdmihBuildRecordFxRowCallback,
    STL_SIZEOF( smdmihFxPathCtrl ),
    "D$MEMORY_INST_HASH_ROW",
    "session instant hash row",
    gSmdmihFxRowColumnDesc
};

/*******************************************************************************
 * D$MEMORY_INST_HASH_DIR 을 위한 코드
 *******************************************************************************/

knlFxColumnDesc gSmdmihFxDirColumnDesc[] =
{
    {
        "DIR_SEQ",
        "directory slot sequence",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smdmihFxDir, mDirSeq ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "COLLISION_COUNT",
        "collision count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmihFxDir, mCollisionCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "HASH_KEY",
        "hash key",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmihFxDir, mHashKey ),
        STL_SIZEOF( stlInt32 ),
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

    
stlStatus smdmihBuildRecordFxDirCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aTupleExist,
                                          knlEnv            * aEnv )
{
    smdmihTableHeader   * sTableHeader;
    smdmihFxPathCtrl    * sPathCtrl;
    smdmihFxDir           sFxDir;
    smdmihDirSlot       * sDirSlot;
    
    *aTupleExist = STL_FALSE;
    sTableHeader = (smdmihTableHeader*)aDumpObjHandle;
    sPathCtrl = (smdmihFxPathCtrl*)aPathCtrl;

    sPathCtrl->mDirIdx += 1;
    
    STL_TRY_THROW( sPathCtrl->mDirIdx < sTableHeader->mBucketCount, RAMP_FINISH );

    sDirSlot = &(sPathCtrl->mDirectory[sPathCtrl->mDirIdx]);

    sFxDir.mCollisionCount = sDirSlot->mCollisionCount;
    sFxDir.mDirSeq = sPathCtrl->mDirIdx;
    sFxDir.mHashKey = sDirSlot->mHashKey;

    STL_TRY( knlBuildFxRecord( gSmdmihFxDirColumnDesc,
                               (void*)&sFxDir,
                               aValueList,
                               aBlockIdx,
                               aEnv )
             == STL_SUCCESS );
    
    *aTupleExist = STL_TRUE;
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmdmihFxDirTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    smdmihGetDumpObjectCallback,
    smdmihOpenFxCallback,
    smdmihCloseFxCallback,
    smdmihBuildRecordFxDirCallback,
    STL_SIZEOF( smdmihFxPathCtrl ),
    "D$MEMORY_INST_HASH_DIR",
    "session instant hash directory",
    gSmdmihFxDirColumnDesc
};

/** @} */
