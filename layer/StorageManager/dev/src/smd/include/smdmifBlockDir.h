/*******************************************************************************
 * smdmifBlockDir.h
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
 * @file smdmifBlockDir.h
 * @brief Storage Manager Layer Memory Instant Flat Table Routines
 */

/**
 * @defgroup smdmifBlockDir Memory Instant Flat Table
 * @ingroup smdInternal
 * @{
 */


typedef struct smdmifBlockDirHeader
{
    smdmifDefBlockHeader mCommon;
    knlLogicalAddr       mParent;
    stlUInt32            mMaxDirCount;
    stlInt16             mLevel;
    knlLogicalAddr       mDir[1];
} smdmifBlockDirHeader;

#define SMDMIF_BLOCK_DIR_ADD_DIR_KEY(aBlockDir, aAddr)                   \
    (aBlockDir)->mDir[SMDMIF_DEF_BLOCK_ITEM_COUNT(aBlockDir)] = (aAddr); \
    (SMDMIF_DEF_BLOCK_ITEM_COUNT(aBlockDir))++

#define SMDMIF_BLOCK_DIR_GET_DIR(aBlockDir, aIndex)    \
    ( (aBlockDir)->mDir[(aIndex)] )

typedef stlStatus (*smdmifBlockDirCallbackAlloc)( void                *aTableHandle,
                                                  void               **aBlockDir,
                                                  knlLogicalAddr       aParentDirAddr,
                                                  stlInt16             aLevel,
                                                  smlEnv              *aEnv );

stlStatus smdmifBlockDirInit( void           *aBlockDir,
                              knlLogicalAddr  aParentDirAddr,
                              stlInt16        aLevel,
                              stlInt32        aBlockSize );

stlStatus smdmifBlockDirInsertKey( void                        *aTableHandle,
                                   void                        *aBlockDir,
                                   knlLogicalAddr               aChildBlockAddr,
                                   smdmifBlockDirCallbackAlloc  aAllocBlockDir,
                                   smlEnv                      *aEnv );

void* smdmifBlockDirFindItem( knlLogicalAddr   aBlockDirAddr,
                              stlInt32         aFixedPartMaxRowCount,
                              stlInt64        *aRowNum,
                              smlEnv          *aEnv );


stlStatus smdmifBlockDirVerify( void *aBlockDir );

/** @} */

