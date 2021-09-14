/*******************************************************************************
 * smdmifFxTable.h
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smdmifFxTable.h
 * @brief Storage Manager Layer Memory Instant Flat Fixed Table Routines
 */

/**
 * @defgroup smdmifFxTable Memory Instant Flat Fixed Table
 * @ingroup smdInternal
 * @{
 */

#ifndef _SMDMIF_FIXED_TABLE_H_
#define _SMDMIF_FIXED_TABLE_H_ 1

#include <dtlDef.h>



#define SMDMIF_TABLE_TYPE_NAME_LEN   16
#define SMDMIF_TABLE_TYPE_FALT       "FLAT"
#define SMDMIF_TABLE_TYPE_MERGE      "MERGE SORT"
#define SMDMIF_TABLE_TYPE_ORDERED    "ORDERED SORT"
#define SMDMIF_TABLE_TYPE_VOLATILE   "VOLATILE SORT"
#define SMDMIF_TABLE_TYPE_INDEX      "INDEX SORT"
#define SMDMIF_TABLE_TYPE_UNKNOWN    "UNKNOWN"

#define SMDMIF_BLOCK_TYPE_NAME_LEN   16
#define SMDMIF_BLOCK_TYPE_FIXED_PART "FIXED PART"
#define SMDMIF_BLOCK_TYPE_VAR_PART   "VAR PART"
#define SMDMIF_BLOCK_TYPE_BLOCK_DIR  "BLOCK DIR"
#define SMDMIF_BLOCK_TYPE_INDEX_NODE "INDEX NODE"
#define SMDMIF_BLOCK_TYPE_UNKNOWN    "UNKNOWN"

#define SMDMIF_BLOCK_DATA_COLUMN_SIZE       4
#define SMDMIF_BLOCK_DATA_COLUMN_COUNT      8

/* 0F1245DE 03E2FFDF ... 이런식으로 4바이트당 9글자로 기록되기 때문에 다음 공식이 만들어진다. */
#define SMDMIF_BLOCK_DATA_ROW_DISPLAY_LEN   ( (SMDMIF_BLOCK_DATA_COLUMN_SIZE*2 + 1) * SMDMIF_BLOCK_DATA_COLUMN_COUNT )

typedef struct smdmifFxTableList
{
    stlInt64            mInstTableId;
    smlTbsId            mTbsId;
    stlChar             mTableType[SMDMIF_TABLE_TYPE_NAME_LEN];
    stlInt64            mRowCount;
    stlInt32            mColCount;
    stlInt32            mVarColCount;
    stlInt32            mAllocExtCount;
    stlInt16            mFixedRowLen;
    stlInt16            mLargestColSize;
} smdmifFxTableList;

typedef smdInstHandleListNode * smdmifFxTablePathCtrl;

typedef struct smdmifFxBlockList
{
    stlUInt64       mLogicalAddr;
    stlChar         mBlockType[SMDMIF_BLOCK_TYPE_NAME_LEN];
    stlInt32        mItemCount;
    stlBool         mIsInstMem;
} smdmifFxBlockList;

typedef struct smdmifFxBlockListPathCtrl
{
    smlPid          mPageId;
    stlInt32        mInstMemIndex;
    void           *mSegIterator;
} smdmifFxBlockListPathCtrl;

typedef struct smdmifFxColumnList
{
    stlInt32        mOrderNo;
    stlChar         mType[DTL_DATA_TYPE_NAME_MAX_SIZE];
    stlBool         mIsFixedCol;
    stlInt32        mOffset;
    stlInt32        mAllocSize;
} smdmifFxColumnList;
 
typedef struct smdmifFxBlockDump
{
    stlInt32        mRowNum;
    stlChar         mRowDump[SMDMIF_BLOCK_DATA_ROW_DISPLAY_LEN];
} smdmifFxBlockDump;

#endif /* _SMDMIF_FIXED_TABLE_ */

/** @} */
