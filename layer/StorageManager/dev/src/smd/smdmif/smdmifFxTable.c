/*******************************************************************************
 * smdmifFxTable.c
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
#include <smdmifTable.h>
#include <smdmifFxTable.h>
#include <smsmfDef.h>

/**
 * @file smdmifFxTable.c
 * @brief Storage Manager Layer Memory Instant Flat Table Fixed Table Routines
 */

/**
 * @addtogroup smdmifFxTable
 * @{
 */


/*******************************************************************************
 * X$INSTANT_TABLE을 위한 코드
 *******************************************************************************/

knlFxColumnDesc gSmdmifFxTableListColumnDesc[] =
{
    {
        "HANDLE_ID",
        "instant table handle identifier",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smdmifFxTableList, mInstTableId ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "TBS_ID",
        "tablespace identifier",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmifFxTableList, mTbsId ),
        STL_SIZEOF( smlTbsId ),
        STL_FALSE  /* nullable */
    },
    {
        "TYPE",
        "instant table type",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smdmifFxTableList, mTableType ),
        SMDMIF_TABLE_TYPE_NAME_LEN,
        STL_FALSE  /* nullable */
    },
    {
        "ROW_COUNT",
        "row count",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smdmifFxTableList, mRowCount ),
        STL_SIZEOF( stlInt64 ),
        STL_FALSE  /* nullable */
    },
    {
        "COL_COUNT",
        "column count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmifFxTableList, mColCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "VAR_COL_COUNT",
        "variable column count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmifFxTableList, mVarColCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "ALLOC_EXT_COUNT",
        "allocated extent count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmifFxTableList, mAllocExtCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "FIXED_ROW_LEN",
        "fixed row length",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmifFxTableList, mFixedRowLen ),
        STL_SIZEOF( stlInt16 ),
        STL_FALSE  /* nullable */
    },
    {
        "LARGEST_COL_LEN",
        "largest column length",
        KNL_FXTABLE_DATATYPE_SMALLINT,
        STL_OFFSETOF( smdmifFxTableList, mLargestColSize ),
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

static stlStatus smdmifOpenFxTableListCallback( void   * aStmt,
                                                void   * aDumpObjHandle,
                                                void   * aPathCtrl,
                                                knlEnv * aEnv )
{
    *((smdmifFxTablePathCtrl *)aPathCtrl) = SMD_GET_SESS_INST_HANDLE_LIST( aEnv );

    return STL_SUCCESS;
}

static stlStatus smdmifCloseFxTableListCallback( void   * aDumpObjHandle,
                                                 void   * aPathCtrl,
                                                 knlEnv * aEnv )
{
    return STL_SUCCESS;
}

static stlStatus smdmifBuildRecordFxTableListCallback( void              * aDumpObjHandle,
                                                       void              * aPathCtrl,
                                                       knlValueBlockList * aValueList,
                                                       stlInt32            aBlockIdx,
                                                       stlBool           * aTupleExist,
                                                       knlEnv            * aEnv )
{
    smdInstHandleListNode *sHandleNode;
    smdmifFxTableList      sTableList;
    smdmifTableHeader     *sHeader;

    *aTupleExist = STL_FALSE;
    sHandleNode = *(smdmifFxTablePathCtrl *)aPathCtrl;

    if( sHandleNode != NULL )
    {
        sHeader = (smdmifTableHeader*)sHandleNode->mHandle;

        sTableList.mInstTableId = sHandleNode->mInstTableId;
        sTableList.mTbsId = sHeader->mTbsId;
        sTableList.mRowCount = SMDMIF_TABLE_GET_TOTAL_ROW_COUNT( sHeader );
        sTableList.mColCount = sHeader->mColCount;
        sTableList.mVarColCount = sHeader->mVarPartColCount;
        sTableList.mAllocExtCount = sHeader->mSegHandle == NULL ? 0 : SMS_SEGMENT_HANDLE_GET_ALLOC_EXT_COUNT( sHeader->mSegHandle );
        sTableList.mFixedRowLen = sHeader->mFixedRowLen;
        sTableList.mLargestColSize = sHeader->mLargestColSize;

        if( sHeader->mInstType == SMDMIF_TABLE_TYPE_FLAT )
        {
            stlStrncpy( sTableList.mTableType, SMDMIF_TABLE_TYPE_FALT, SMDMIF_TABLE_TYPE_NAME_LEN );
        }
        else if( sHeader->mInstType == SMDMIF_TABLE_TYPE_MERGE_SORT )
        {
            stlStrncpy( sTableList.mTableType, SMDMIF_TABLE_TYPE_MERGE, SMDMIF_TABLE_TYPE_NAME_LEN );
        }
        else if( sHeader->mInstType == SMDMIF_TABLE_TYPE_ORDERED_SORT )
        {
            stlStrncpy( sTableList.mTableType, SMDMIF_TABLE_TYPE_ORDERED, SMDMIF_TABLE_TYPE_NAME_LEN );
        }
        else if( sHeader->mInstType == SMDMIF_TABLE_TYPE_VOLATILE_SORT )
        {
            stlStrncpy( sTableList.mTableType, SMDMIF_TABLE_TYPE_VOLATILE, SMDMIF_TABLE_TYPE_NAME_LEN );
        }
        else if( sHeader->mInstType == SMDMIF_TABLE_TYPE_INDEX_SORT )
        {
            stlStrncpy( sTableList.mTableType, SMDMIF_TABLE_TYPE_INDEX, SMDMIF_TABLE_TYPE_NAME_LEN );
        }
        else
        {
            stlStrncpy( sTableList.mTableType, SMDMIF_TABLE_TYPE_UNKNOWN, SMDMIF_TABLE_TYPE_NAME_LEN );
        }

        STL_TRY( knlBuildFxRecord( gSmdmifFxTableListColumnDesc,
                                   (void*)&sTableList,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );
    
        *((smdmifFxTablePathCtrl *)aPathCtrl) = sHandleNode->mNext;
        *aTupleExist = STL_TRUE;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmdmifFxTableListTableDesc =
{
    KNL_FXTABLE_USAGE_FIXED_TABLE,
    KNL_STARTUP_PHASE_NO_MOUNT,
    NULL,
    smdmifOpenFxTableListCallback,
    smdmifCloseFxTableListCallback,
    smdmifBuildRecordFxTableListCallback,
    STL_SIZEOF( smdmifFxTablePathCtrl ),
    "X$INST_TABLE_LIST",
    "session instant table list",
    gSmdmifFxTableListColumnDesc
};



/*******************************************************************************
 * D$INST_TBL_COLUMN을 위한 코드
 *******************************************************************************/

knlFxColumnDesc gSmdmifFxColumnListColumnDesc[] =
{
    {
        "COL_ORDER",
        "column order",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmifFxColumnList, mOrderNo ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "DATA_TYPE",
        "data type",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smdmifFxColumnList, mType ),
        DTL_DATA_TYPE_NAME_MAX_SIZE ,
        STL_FALSE  /* nullable */
    },
    {
        "IS_FIXED",
        "is fixed part column",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smdmifFxColumnList, mIsFixedCol ),
        STL_SIZEOF( stlBool ),
        STL_FALSE  /* nullable */
    },
    {
        "OFFSET_IN_ROW",
        "column offset in fixed row",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmifFxColumnList, mOffset ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "ALLOC_SIZE",
        "data type",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmifFxColumnList, mAllocSize ),
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

static stlStatus smdmifGetDumpObjectCallback( stlInt64   aTransId,
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
                *aDumpObjHandle = sNode->mHandle;
                break; 
            }

            sNode = sNode->mNext;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmifOpenFxColumnListCallback( void   * aStmt,
                                                 void   * aDumpObjHandle,
                                                 void   * aPathCtrl,
                                                 knlEnv * aEnv )
{
    *((stlInt32 *)aPathCtrl) = 0;

    return STL_SUCCESS;
}

static stlStatus smdmifCloseFxColumnListCallback( void   * aDumpObjHandle,
                                                  void   * aPathCtrl,
                                                  knlEnv * aEnv )
{
    return STL_SUCCESS;
}

static stlStatus smdmifBuildRecordFxColumnListCallback( void              * aDumpObjHandle,
                                                        void              * aPathCtrl,
                                                        knlValueBlockList * aValueList,
                                                        stlInt32            aBlockIdx,
                                                        stlBool           * aTupleExist,
                                                        knlEnv            * aEnv )
{
    smdmifTableHeader    *sHeader;
    stlInt32              sColIndex;
    smdmifFxColumnList    sColList;

    sHeader = (smdmifTableHeader *)aDumpObjHandle;
    sColIndex = *(stlInt32 *)aPathCtrl;

    if( sColIndex < sHeader->mColCount )
    {
        sColList.mOrderNo = sColIndex;
        sColList.mIsFixedCol = sHeader->mCols[sColIndex].mIsFixedPart;
        sColList.mOffset = sHeader->mCols[sColIndex].mOffset;
        sColList.mAllocSize = sHeader->mCols[sColIndex].mAllocSize;

        stlStrncpy( sColList.mType, dtlDataTypeName[sHeader->mCols[sColIndex].mDataType], DTL_DATA_TYPE_NAME_MAX_SIZE );

        STL_TRY( knlBuildFxRecord( gSmdmifFxColumnListColumnDesc,
                                   (void*)&sColList,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );

        *aTupleExist = STL_TRUE;
        *(stlInt32 *)aPathCtrl = sColIndex + 1;
    }
    else
    {
        *aTupleExist = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmdmifFxColumnListTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    smdmifGetDumpObjectCallback,
    smdmifOpenFxColumnListCallback,
    smdmifCloseFxColumnListCallback,
    smdmifBuildRecordFxColumnListCallback,
    STL_SIZEOF( stlInt32 ),
    "D$INST_COLUMN_LIST",
    "instant table column list",
    gSmdmifFxColumnListColumnDesc
};



/*******************************************************************************
 * D$INST_TBL_BLOCK을 위한 코드
 *******************************************************************************/

knlFxColumnDesc gSmdmifFxBlockListColumnDesc[] =
{
    {
        "LOGICAL_ADDR",
        "logical address of block",
        KNL_FXTABLE_DATATYPE_BIGINT,
        STL_OFFSETOF( smdmifFxBlockList, mLogicalAddr ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "BLOCK_TYPE",
        "block type",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smdmifFxBlockList, mBlockType ),
        SMDMIF_BLOCK_TYPE_NAME_LEN ,
        STL_FALSE  /* nullable */
    },
    {
        "ITEM_COUNT",
        "item count",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmifFxBlockList, mItemCount ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "INSTANT_MEM",
        "is not extent memory but instant memory",
        KNL_FXTABLE_DATATYPE_BOOL,
        STL_OFFSETOF( smdmifFxBlockList, mIsInstMem ),
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

static stlStatus smdmifOpenFxBlockListCallback( void   * aStmt,
                                                void   * aDumpObjHandle,
                                                void   * aPathCtrl,
                                                knlEnv * aEnv )
{
    smdmifTableHeader         * sHeader;
    smdmifFxBlockListPathCtrl * sPathCtrl = (smdmifFxBlockListPathCtrl *)aPathCtrl;
    
    sPathCtrl->mInstMemIndex = 0;
    sPathCtrl->mPageId = SMP_NULL_PID;
    sPathCtrl->mSegIterator = NULL;

    sHeader = (smdmifTableHeader *)aDumpObjHandle;
    
    if( sHeader->mSegHandle != NULL )
    {
        STL_TRY( smsAllocSegIterator( (smlStatement*)aStmt,
                                      sHeader->mSegHandle,
                                      (void **)&sPathCtrl->mSegIterator,
                                      SML_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmifCloseFxBlockListCallback( void   * aDumpObjHandle,
                                                 void   * aPathCtrl,
                                                 knlEnv * aEnv )
{
    smdmifTableHeader         * sHeader;
    smdmifFxBlockListPathCtrl * sPathCtrl = (smdmifFxBlockListPathCtrl *)aPathCtrl;
    
    if( sPathCtrl->mSegIterator != NULL )
    {
        sHeader = (smdmifTableHeader *)aDumpObjHandle;
        
        STL_TRY( smsFreeSegIterator( sHeader->mSegHandle,
                                     sPathCtrl->mSegIterator,
                                     SML_ENV(aEnv) )
                 == STL_SUCCESS );
        
        sPathCtrl->mSegIterator = NULL;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmifBuildRecordFxBlockListCallback( void              * aDumpObjHandle,
                                                       void              * aPathCtrl,
                                                       knlValueBlockList * aValueList,
                                                       stlInt32            aBlockIdx,
                                                       stlBool           * aTupleExist,
                                                       knlEnv            * aEnv )
{
    smdmifTableHeader         *sHeader;
    smdmifFxBlockList          sBlockList;
    smdmifFxBlockListPathCtrl *sPathCtrl;
    smdmifDefBlockHeader      *sBlock;
    smpHandle                  sPageHandle;
    smsmfExtHdr               *sExtHdr;

    sHeader = (smdmifTableHeader *)aDumpObjHandle;
    sPathCtrl = (smdmifFxBlockListPathCtrl *)aPathCtrl;

    if( sPathCtrl->mInstMemIndex < sHeader->mInstMemAllocCount )
    {
        sBlock = sHeader->mInstMemList[sPathCtrl->mInstMemIndex];
        sBlockList.mIsInstMem = STL_TRUE;

        sPathCtrl->mInstMemIndex++;
    }
    else
    {
        if( sPathCtrl->mSegIterator == NULL )
        {
            sBlock = NULL;
        }
        else
        {
            if( sPathCtrl->mPageId == SMP_NULL_PID )
            {
                STL_TRY( smsGetFirstPage( sHeader->mSegHandle,
                                          sPathCtrl->mSegIterator,
                                          &sPathCtrl->mPageId,
                                          SML_ENV( aEnv ) ) == STL_SUCCESS );
            }
            else
            {
                STL_TRY( smsGetNextPage( sHeader->mSegHandle,
                                         sPathCtrl->mSegIterator,
                                         &sPathCtrl->mPageId,
                                         SML_ENV( aEnv ) ) == STL_SUCCESS );
            }
        }

        if( sPathCtrl->mPageId == SMP_NULL_PID )
        {
            sBlock = NULL;
        }
        else
        {
            STL_TRY( smpFix( sHeader->mTbsId,
                             sPathCtrl->mPageId,
                             &sPageHandle,
                             SML_ENV( aEnv ) ) == STL_SUCCESS );

            sExtHdr = (smsmfExtHdr *)smpGetBody( SMP_FRAME( &sPageHandle ) );

            STL_TRY( smpUnfix( &sPageHandle,
                               SML_ENV( aEnv ) ) == STL_SUCCESS );

            /* block의 시작 주소는 extent header 바로 다음이다. */
            sBlock = (void *)(sExtHdr + 1);

            sBlockList.mIsInstMem = STL_FALSE;
        }
    }

    if( sBlock == NULL )
    {
        *aTupleExist = STL_FALSE;
    }
    else
    {
        sBlockList.mLogicalAddr = SMDMIF_DEF_BLOCK_ADDR( sBlock );
        sBlockList.mItemCount = SMDMIF_DEF_BLOCK_ITEM_COUNT( sBlock );

        switch( SMDMIF_DEF_BLOCK_TYPE( sBlock ) )
        {
            case SMDMIF_DEF_BLOCK_FIXED_PART:
                stlStrncpy( sBlockList.mBlockType, SMDMIF_BLOCK_TYPE_FIXED_PART, SMDMIF_BLOCK_TYPE_NAME_LEN );
                break;

            case SMDMIF_DEF_BLOCK_VAR_PART:
                stlStrncpy( sBlockList.mBlockType, SMDMIF_BLOCK_TYPE_VAR_PART, SMDMIF_BLOCK_TYPE_NAME_LEN );
                break;

            case SMDMIF_DEF_BLOCK_DIR:
                stlStrncpy( sBlockList.mBlockType, SMDMIF_BLOCK_TYPE_BLOCK_DIR, SMDMIF_BLOCK_TYPE_NAME_LEN );
                break;

            case SMDMIF_DEF_BLOCK_INDEX_NODE:
                stlStrncpy( sBlockList.mBlockType, SMDMIF_BLOCK_TYPE_INDEX_NODE, SMDMIF_BLOCK_TYPE_NAME_LEN );
                break;

            default:
                stlStrncpy( sBlockList.mBlockType, SMDMIF_BLOCK_TYPE_UNKNOWN, SMDMIF_BLOCK_TYPE_NAME_LEN );
        }

        STL_TRY( knlBuildFxRecord( gSmdmifFxBlockListColumnDesc,
                                   (void*)&sBlockList,
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


knlFxTableDesc gSmdmifFxBlockListTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    smdmifGetDumpObjectCallback,
    smdmifOpenFxBlockListCallback,
    smdmifCloseFxBlockListCallback,
    smdmifBuildRecordFxBlockListCallback,
    STL_SIZEOF( smdmifFxBlockListPathCtrl ),
    "D$INST_BLOCK_LIST",
    "instant table block list",
    gSmdmifFxBlockListColumnDesc
};




/*******************************************************************************
 * D$INST_BLOCK을 위한 코드
 *******************************************************************************/

knlFxColumnDesc gSmdmifFxBlockDumpColumnDesc[] =
{
    {
        "ROW_NUM",
        "row sequence number",
        KNL_FXTABLE_DATATYPE_INTEGER,
        STL_OFFSETOF( smdmifFxBlockDump, mRowNum ),
        STL_SIZEOF( stlInt32 ),
        STL_FALSE  /* nullable */
    },
    {
        "DUMP",
        "block dump",
        KNL_FXTABLE_DATATYPE_VARCHAR,
        STL_OFFSETOF( smdmifFxBlockDump, mRowDump ),
        SMDMIF_BLOCK_DATA_ROW_DISPLAY_LEN,
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

static stlStatus smdmifGetDumpObjectBlockDumpCallback( stlInt64   aTransId,
                                                       void     * aStmt,
                                                       stlChar  * aDumpObjName,
                                                       void    ** aDumpObjHandle,
                                                       knlEnv   * aEnv )
{
    stlChar              *sToken1;
    stlChar              *sToken2;
    stlChar              *sToken3;
    stlInt32              sTokenCount;
    knlLogicalAddr        sAddr;

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
                                (stlInt64 *)&sAddr,
                                KNL_ERROR_STACK(aEnv) ) == STL_SUCCESS );

        *aDumpObjHandle = KNL_TO_PHYSICAL_ADDR( sAddr );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmifOpenFxBlockDumpCallback( void   * aStmt,
                                                void   * aDumpObjHandle,
                                                void   * aPathCtrl,
                                                knlEnv * aEnv )
{
    *((stlInt32 *)aPathCtrl) = 0;

    return STL_SUCCESS;
}

static stlStatus smdmifCloseFxBlockDumpCallback( void   * aDumpObjHandle,
                                                 void   * aPathCtrl,
                                                 knlEnv * aEnv )
{
    return STL_SUCCESS;
}

static const stlChar gDigit[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static stlStatus smdmifBuildRecordFxBlockDumpCallback( void              * aDumpObjHandle,
                                                       void              * aPathCtrl,
                                                       knlValueBlockList * aValueList,
                                                       stlInt32            aBlockIdx,
                                                       stlBool           * aTupleExist,
                                                       knlEnv            * aEnv )
{
    stlInt32          sRowNum;
    stlUInt8         *sBlock;
    stlUInt8         *sPos;
    stlUInt8         *sFence;
    smdmifFxBlockDump sDump;
    stlInt32          sCur;
    stlInt32          i, j;

    sBlock = (stlUInt8 *)aDumpObjHandle;
    sRowNum = *(stlInt32 *)aPathCtrl;

    sPos = sBlock + ( sRowNum * SMDMIF_BLOCK_DATA_COLUMN_COUNT * SMDMIF_BLOCK_DATA_COLUMN_SIZE );
    sFence = sBlock + smdmifGetBlockSize( SML_MEMORY_TEMP_SYSTEM_TBS_ID );

    if( sPos < sFence )
    {
        sDump.mRowNum = sRowNum;
        sCur = 0;
        for( i = 0; i < SMDMIF_BLOCK_DATA_COLUMN_COUNT; i++ )
        {
            for( j = 0; j < SMDMIF_BLOCK_DATA_COLUMN_SIZE; j++ )
            {
                if( sPos >= sFence )
                {
                    i = SMDMIF_BLOCK_DATA_COLUMN_COUNT;
                    break;
                }
                sDump.mRowDump[sCur++] = gDigit[(*sPos & 0xF0) >> 4];
                sDump.mRowDump[sCur++] = gDigit[*sPos & 0x0F];

                sPos++;
            }
            if( i < SMDMIF_BLOCK_DATA_COLUMN_COUNT - 1 )
            {
                sDump.mRowDump[sCur++] = ' ';
            }
        }

        STL_DASSERT( sCur < SMDMIF_BLOCK_DATA_ROW_DISPLAY_LEN );

        sDump.mRowDump[sCur] = '\0';

        STL_TRY( knlBuildFxRecord( gSmdmifFxBlockDumpColumnDesc,
                                   (void*)&sDump,
                                   aValueList,
                                   aBlockIdx,
                                   aEnv )
                 == STL_SUCCESS );

        *aTupleExist = STL_TRUE;
        *(stlInt32 *)aPathCtrl = sRowNum + 1;
    }
    else
    {
        *aTupleExist = STL_FALSE;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

knlFxTableDesc gSmdmifFxBlockDumpTableDesc =
{
    KNL_FXTABLE_USAGE_DUMP_TABLE,
    KNL_STARTUP_PHASE_MOUNT,
    smdmifGetDumpObjectBlockDumpCallback,
    smdmifOpenFxBlockDumpCallback,
    smdmifCloseFxBlockDumpCallback,
    smdmifBuildRecordFxBlockDumpCallback,
    STL_SIZEOF( stlInt32 ),
    "D$INST_BLOCK_DATA",
    "instant table block dump",
    gSmdmifFxBlockDumpColumnDesc
};

/** @} */
