/*******************************************************************************
 * smdmifVarPart.h
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
 * @file smdmifVarPart.h
 * @brief Storage Manager Layer Memory Instant Flat Table Routines
 */

/**
 * @defgroup smdmifVarPart Memory Instant Flat Table
 * @ingroup smdInternal
 * @{
 */

#ifndef _SMDMIF_VAR_PART_H_
#define _SMDMIF_VAR_PART_H_ 1

#include <smdmifDef.h>

typedef struct smdmifVarPartHeader
{
    smdmifDefBlockHeader mCommon;
    /* var part는 next가 없다. full scan할 일이 없기 때문이다. */
    /* var part에는 first row offset도 없다. 첫번째 row부터 순회할 필요가 없기 때문이다. */
    stlInt32             mInsertRowOffset;
    stlInt32             mFenceOffset;
} smdmifVarPartHeader;

typedef stlChar*   smdmifVarCol;

typedef stlStatus (*smdmifVarPartCallbackNextVarPart)( void              *aTableHeader,
                                                       void             **aVarPart,
                                                       smlEnv            *aEnv );

/*
 * local definitions
 */
#define SMDMIF_VAR_COL_HEADER_SIZE        STL_SIZEOF(stlInt32)

#define SMDMIF_COL_ALIGNED_POS(aPos)      ( STL_ALIGN( (stlUInt64)(aPos), STL_SIZEOF(stlInt32) ) )

#define SMDMIF_VAR_COL_NEXT_PTR_SIZE      ( STL_SIZEOF(knlLogicalAddr) )

/*
 * smdmifVarPartHeader 관련 로컬 인터페이스
 */
#define SMDMIF_VAR_PART_FREE_SIZE(aVarPart) \
    ( (aVarPart)->mFenceOffset - (aVarPart)->mInsertRowOffset )

#define SMDMIF_VAR_PART_INSERT_POS(aVarPart) \
    ( (stlChar *)(aVarPart) + (aVarPart)->mInsertRowOffset )

#define SMDMIF_VAR_PART_INSERT_POS_ADDR(aVarPart) \
    ( SMDMIF_DEF_BLOCK_ADDR(aVarPart) + (aVarPart)->mInsertRowOffset )

#define SMDMIF_VAR_PART_WRITE_COL(aVarPart, aVarColAddr, aHasNext, aSourcePos, aSourceLen)                 \
    STL_DASSERT( aSourceLen <= 0x00FFFFFFFF );                                                             \
    STL_DASSERT( SMDMIF_VAR_PART_FREE_SIZE(aVarPart) >= aSourceLen );                                      \
    (aVarColAddr) = SMDMIF_VAR_PART_INSERT_POS_ADDR(aVarPart);                                             \
    *(stlInt32 *)(SMDMIF_VAR_PART_INSERT_POS(aVarPart)) =                                                  \
        ( (aHasNext) == STL_TRUE ? 0x01000000 : 0x00000000 ) | aSourceLen;                                 \
    stlMemcpy(SMDMIF_VAR_PART_INSERT_POS(aVarPart) + SMDMIF_VAR_COL_HEADER_SIZE, aSourcePos, aSourceLen);  \
    (aVarPart)->mInsertRowOffset += SMDMIF_COL_ALIGNED_POS(SMDMIF_VAR_COL_HEADER_SIZE + (aSourceLen))

#define SMDMIF_VAR_PART_WRITE_NEXT_COL_PTR(aVarPart, aAddr)                             \
    STL_DASSERT( SMDMIF_VAR_PART_FREE_SIZE(aVarPart) == SMDMIF_VAR_COL_NEXT_PTR_SIZE ); \
    *(knlLogicalAddr *)SMDMIF_VAR_PART_INSERT_POS(aVarPart) = (aAddr)


/*
 * smdmifVarPartHeader 관련 외부 인터페이스
 */
#define SMDMIF_VAR_PART_INIT_HEADER(aVarPart, aBlockSize)                                     \
    ((smdmifVarPartHeader *)(aVarPart))->mInsertRowOffset  = STL_SIZEOF(smdmifVarPartHeader); \
    ((smdmifVarPartHeader *)(aVarPart))->mFenceOffset      = aBlockSize

stlStatus smdmifVarPartWriteCol( void                            *aTableHeader,
                                 void                            *aVarPart,
                                 knlLogicalAddr                  *aVarColAddr,
                                 stlChar                         *aSourceColPos,
                                 stlInt32                         aSourceColLen,
                                 stlInt32                         aVarColPieceMinLen,
                                 smdmifVarPartCallbackNextVarPart aCallbackFunc,
                                 smlEnv                          *aEnv );

stlStatus smdmifVarPartUpdateCol( void                            *aTableHandle,
                                  void                            *aVarPart,
                                  knlLogicalAddr                  *aVarColAddr,
                                  stlChar                         *aSourceColPos,
                                  stlInt32                         aSourceColLen,
                                  stlInt32                         aVarColPieceMinLen,
                                  smdmifVarPartCallbackNextVarPart aAllocVarPart,
                                  smlEnv                          *aEnv );

void smdmifVarPartReadCol( smdmifVarCol  aSourceCol,
                           stlChar      *aTargetPos,
                           stlInt64     *aReadLen );

stlStatus smdmifVarPartReadDataValue( smdmifVarCol   aSourceCol,
                                      dtlDataValue * aDataValue,
                                      smlEnv       * aEnv );

stlStatus smdmifVarPartVerifyCol( void *aVarCol );

/*
 * smdmifVarCol 관련 외부 인터페이스
 */
#define SMDMIF_VAR_COL_HAS_NEXT(aVarCol) \
    ( ( (*(stlUInt32 *)(aVarCol)) & 0xFF000000 ) != 0 ? STL_TRUE : STL_FALSE )

#define SMDMIF_VAR_COL_GET_LEN(aVarCol) \
    ( (*(stlUInt32 *)(aVarCol)) & 0x00FFFFFF )

#define SMDMIF_VAR_COL_VALUE_POS(aVarCol) \
    ( (aVarCol) + STL_SIZEOF(stlInt32) )

#define SMDMIF_VAR_COL_NEXT_PIECE(aVarCol)                \
    ( (smdmifVarCol)knlToPhysicalAddr(*(knlLogicalAddr*)( \
        (aVarCol) + SMDMIF_VAR_COL_HEADER_SIZE + SMDMIF_VAR_COL_GET_LEN(aVarCol))) )

#define SMDMIF_VAR_COL_WRITE_VALUE(aVarCol, aNext, aCol, aLen)                         \
    STL_DASSERT( aLen <= 0x00FFFFFFFF );                                               \
    *(stlInt32 *)(aVarCol) = ( (aNext) == STL_TRUE ? 0x01000000 : 0x00000000 ) | aLen; \
    stlMemcpy((aVarCol) + SMDMIF_VAR_COL_HEADER_SIZE, aCol, aLen)

#endif /* _SMDMIF_VAR_PART_H_ */

stlStatus smdmifTablePrepareNextVarBlock( void      * aHeader,
                                          void     ** aVarPart,
                                          smlEnv    * aEnv );

/** @} */

