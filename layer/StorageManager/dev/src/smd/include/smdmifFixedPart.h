/*******************************************************************************
 * smdmifFixedPart.h
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 *
 ******************************************************************************/


/**
 * @file smdmifFixedPart.h
 * @brief Storage Manager Layer Memory Instant Flat Table Routines
 */

/**
 * @defgroup smdmifFixedPart Memory Instant Flat Table
 * @ingroup smdInternal
 * @{
 */

#ifndef _SMDMIF_FIXED_PART_H_
#define _SMDMIF_FIXED_PART_H_ 1

#include <smdmifDef.h>

typedef struct smdmifFixedPartHeader
{
    smdmifDefBlockHeader mCommon;
    knlLogicalAddr       mNextFixedPart;
    stlInt64             mRowPosNumBase;    /**< 이 fixed part의 첫번재 row의 position number이다.*/
    stlUInt32            mMaxRowCount;
    stlUInt32            mRowLen;
    stlUInt32            mFirstRowOffset;
    stlUInt32            mInsertRowOffset;
} smdmifFixedPartHeader;

typedef stlChar*   smdmifFixedRow;

typedef stlChar*   smdmifFixedCol;

typedef stlUInt16  smdmifFixedColLength;

typedef stlChar    smdmifFixedRowHeader;


typedef stlStatus (*smdmifVerifyVarCol)( void *aVarCol );

#define SMDMIF_FIXED_ROW_HEADER_SIZE       STL_SIZEOF(smdmifFixedRowHeader)

#define SMDMIF_FIRST_FIXED_ROW_OFFSET(aFixedRowAlign)                                            \
    ( ( STL_SIZEOF(smdmifFixedPartHeader) + (aFixedRowAlign) + SMDMIF_FIXED_ROW_HEADER_SIZE ) /  \
      (aFixedRowAlign) * (aFixedRowAlign) - SMDMIF_FIXED_ROW_HEADER_SIZE )

/*
 * smdmifFixedPart 관련 내부 인터페이스
 */
#define SMDMIF_FIXED_PART_SET_NEXT(aPrevFixedPart, aNextFixedPartAddr) \
    ( (aPrevFixedPart)->mNextFixedPart = (aNextFixedPartAddr) )

/*
 * smdmifFixedPart 관련 외부 인터페이스
 */
#define SMDMIF_FIXED_PART_GET_MY_ADDR(aFixedPart) \
    ( SMDMIF_DEF_BLOCK_ADDR(aFixedPart) )

#define SMDMIF_FIXED_PART_GET_INSERT_ROW(aFixedPart) \
    ( (stlChar*)(aFixedPart) + ((smdmifFixedPartHeader *)(aFixedPart))->mInsertRowOffset )

#define SMDMIF_FIXED_PART_GET_INSERT_ROW_ADDR(aFixedPart)         \
    ( SMDMIF_FIXED_PART_GET_MY_ADDR(aFixedPart) +                 \
      ((smdmifFixedPartHeader *)(aFixedPart))->mInsertRowOffset )

#define SMDMIF_FIXED_PART_GET_FIRST_ROW(aFixedPart) \
    ( (stlChar*)(aFixedPart) + ((smdmifFixedPartHeader *)(aFixedPart))->mFirstRowOffset )

#define SMDMIF_FIXED_PART_GET_FIRST_ROW_ADDR(aFixedPart) \
    ( SMDMIF_FIXED_PART_GET_MY_ADDR(aFixedPart) + ((smdmifFixedPartHeader *)(aFixedPart))->mFirstRowOffset )

#define SMDMIF_FIXED_PART_IS_INSERTABLE(aFixedPart)                    \
    ( SMDMIF_DEF_BLOCK_ITEM_COUNT(aFixedPart) <                        \
      ((smdmifFixedPartHeader *)(aFixedPart))->mMaxRowCount ?          \
      STL_TRUE : STL_FALSE )

#define SMDMIF_FIXED_PART_ALLOC_ROW(aFixedPart, aRow, aRowAddr)       \
    (aRow) = SMDMIF_FIXED_PART_GET_INSERT_ROW( aFixedPart );          \
    (aRowAddr) = SMDMIF_FIXED_PART_GET_INSERT_ROW_ADDR( aFixedPart ); \
    (SMDMIF_DEF_BLOCK_ITEM_COUNT(aFixedPart))++;                      \
    ((smdmifFixedPartHeader *)(aFixedPart))->mInsertRowOffset += ((smdmifFixedPartHeader *)(aFixedPart))->mRowLen

#define SMDMIF_FIXED_PART_GET_ROW(aFixedPart, aIndex)                           \
    ( (aFixedPart) == NULL ? NULL :                                             \
      ( (aIndex) >= SMDMIF_DEF_BLOCK_ITEM_COUNT(aFixedPart) ? NULL :            \
        ( SMDMIF_FIXED_PART_GET_FIRST_ROW(aFixedPart) +                         \
          ((smdmifFixedPartHeader *)(aFixedPart))->mRowLen * (aIndex) ) ) )

#define SMDMIF_FIXED_PART_GET_ROW_ADDR(aFixedPart, aIndex)                            \
    ( (aFixedPart) == NULL ? KNL_LOGICAL_ADDR_NULL :                                  \
      ( (aIndex) >= SMDMIF_DEF_BLOCK_ITEM_COUNT(aFixedPart) ? KNL_LOGICAL_ADDR_NULL : \
        ( SMDMIF_FIXED_PART_GET_FIRST_ROW_ADDR(aFixedPart) +                          \
          ((smdmifFixedPartHeader *)(aFixedPart))->mRowLen * (aIndex) ) ) )

#define SMDMIF_FIXED_PART_GET_ROW_COUNT(aFixedPart)         \
    ( SMDMIF_DEF_BLOCK_ITEM_COUNT(aFixedPart) )

#define SMDMIF_FIXED_PART_GET_MAX_ROW_COUNT(aFixedPart)      \
    ( ((smdmifFixedPartHeader *)(aFixedPart))->mMaxRowCount )

#define SMDMIF_FIXED_PART_GET_NEXT_FIXED_PART(aFixedPart)    \
    ( (smdmifFixedPartHeader *)knlToPhysicalAddr(((smdmifFixedPartHeader *)(aFixedPart))->mNextFixedPart) )

#define SMDMIF_FIXED_PART_GET_NEXT_FIXED_PART_ADDR(aFixedPart) \
    ( ((smdmifFixedPartHeader *)(aFixedPart))->mNextFixedPart )

#define SMDMIF_FIXED_PART_GET_TOTAL_ROW_COUNT_SO_FAR(aFixedPart) \
    ( ((smdmifFixedPartHeader *)(aFixedPart))->mRowPosNumBase +  \
      SMDMIF_DEF_BLOCK_ITEM_COUNT(aFixedPart) )

void smdmifFixedPartInit( void          *aFixedPart,
                          void          *aPrevFixedPart,
                          stlInt32       aBlockSize,
                          stlInt32       aFixedRowLen,
                          stlInt16       aFixedRowAlign );

void smdmifFixedPartUnsetDeletion( void *aFixedPart );

stlStatus smdmifFixedPartVerify( void               *aFixedPart,
                                 smdmifDefColumn    *aColList,
                                 stlInt32            aColCount,
                                 smdmifVerifyVarCol  aVerifyVarCol );

stlStatus smdmifFixedRowVerify( smdmifFixedRow      aRow,
                                smdmifDefColumn    *aColList,
                                stlInt32            aColCount,
                                smdmifVerifyVarCol  aVerifyVarCol );

/*
 * smdmifFixedRow
 */
#define SMDMIF_FIXED_ROW_DELETE_MASK              (0x01)
#define SMDMIF_FIXED_ROW_DELETE_YES               (0x01)
#define SMDMIF_FIXED_ROW_DELETE_NO                (0x00)

#define SMDMIF_FIXED_ROW_DUP_MASK                 (0x02)
#define SMDMIF_FIXED_ROW_DUP_YES                  (0x02)
#define SMDMIF_FIXED_ROW_DUP_NO                   (0x00)

#define SMDMIF_FIXED_ROW_FLAG_MASK                (0x04)
#define SMDMIF_FIXED_ROW_FLAG_YES                 (0x04)
#define SMDMIF_FIXED_ROW_FLAG_NO                  (0x00)

#define SMDMIF_FIXED_ROW_HEADER_INIT_VALUE        (0x00)

/*
 * smdmifFixedRow 관련 외부 인터페이스
 */
#define SMDMIF_FIXED_ROW_WRITE_HEADER(aFixedRow) \
    ( *(smdmifFixedRowHeader*)(aFixedRow) = SMDMIF_FIXED_ROW_HEADER_INIT_VALUE )

#define SMDMIF_FIXED_ROW_GET_COL(aFixedRow, aColInfo) \
    ( (aFixedRow) + (aColInfo)->mOffset )

#define SMDMIF_FIXED_ROW_GET_VAR_COL_POS(aFixedRow, aColInfo) \
    ( (knlLogicalAddr *)SMDMIF_FIXED_ROW_GET_COL(aFixedRow, aColInfo) )

#define SMDMIF_FIXED_ROW_GET_VAR_COL_ADDR(aFixedRow, aColInfo) \
    ( *SMDMIF_FIXED_ROW_GET_VAR_COL_POS(aFixedRow, aColInfo) )

#define SMDMIF_FIXED_ROW_GET_VAR_COL(aFixedRow, aColInfo) \
    ( knlToPhysicalAddr( SMDMIF_FIXED_ROW_GET_VAR_COL_ADDR(aFixedRow, aColInfo) ) )

#define SMDMIF_FIXED_ROW_DELETE(aFixedRow)    \
    ( *(smdmifFixedRowHeader *)(aFixedRow) |= SMDMIF_FIXED_ROW_DELETE_YES )

#define SMDMIF_FIXED_ROW_UNDELETE(aFixedRow)  \
    ( *(smdmifFixedRowHeader *)(aFixedRow) &= ~SMDMIF_FIXED_ROW_DELETE_MASK )

#define SMDMIF_FIXED_ROW_IS_DELETED(aFixedRow)                                 \
    ( (*(smdmifFixedRowHeader *)(aFixedRow) & SMDMIF_FIXED_ROW_DELETE_MASK) == \
      SMDMIF_FIXED_ROW_DELETE_YES ? STL_TRUE : STL_FALSE )

#define SMDMIF_FIXED_ROW_SET_DUP(aFixedRow)    \
    ( *(smdmifFixedRowHeader *)(aFixedRow) |= SMDMIF_FIXED_ROW_DUP_YES )

#define SMDMIF_FIXED_ROW_IS_DUP(aFixedRow)                                  \
    ( (*(smdmifFixedRowHeader *)(aFixedRow) & SMDMIF_FIXED_ROW_DUP_MASK) == \
      SMDMIF_FIXED_ROW_DUP_YES ? STL_TRUE : STL_FALSE )

#define SMDMIF_FIXED_ROW_SET_FLAG(aFixedRow)    \
    ( *(smdmifFixedRowHeader *)(aFixedRow) |= SMDMIF_FIXED_ROW_FLAG_YES )

#define SMDMIF_FIXED_ROW_UNSET_FLAG(aFixedRow)  \
    ( *(smdmifFixedRowHeader *)(aFixedRow) &= ~SMDMIF_FIXED_ROW_FLAG_MASK )

#define SMDMIF_FIXED_ROW_IS_FLAG_SET(aFixedRow)                               \
    ( (*(smdmifFixedRowHeader *)(aFixedRow) & SMDMIF_FIXED_ROW_FLAG_MASK ) == \
      SMDMIF_FIXED_ROW_FLAG_YES ? STL_TRUE : STL_FALSE )


void smdmifFixedRowWriteCol( smdmifFixedRow   aRow,
                             smdmifDefColumn *aColInfo,
                             stlChar         *aSourceCol,
                             stlInt32         aSourceColLen );

void smdmifFixedRowUpdateCol( smdmifFixedRow   aRow,
                              smdmifDefColumn *aColInfo,
                              stlChar         *aSourceCol,
                              stlInt32         aSourceColLen );

/*
 * smdmifFixedCol 관련 외부 인터페이스
 */
#define SMDMIF_FIXED_COL_ALIGN   \
      ( STL_SIZEOF(smdmifFixedColLength) )

#define SMDMIF_FIXED_COL_SIZE(aColSize)  \
    ( (aColSize) + STL_SIZEOF(smdmifFixedColLength) )

#define SMDMIF_FIXED_COL_GET_LEN(aCol)   \
    ( *(smdmifFixedColLength*)(aCol) )

#define SMDMIF_FIXED_COL_GET_VALUE(aCol)  \
    ( (aCol) + STL_SIZEOF(smdmifFixedColLength) )

#endif /* _SMDMIF_FIXED_PART_H_ */

/** @} */

