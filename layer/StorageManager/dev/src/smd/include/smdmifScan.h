/*******************************************************************************
 * smdmifScan.h
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
 * @file smdmifScan.h
 * @brief Storage Manager Layer Memory Instant Flat Table Routines
 */

/**
 * @defgroup smdmifScan Memory Instant Flat Table
 * @ingroup smdInternal
 * @{
 */

typedef struct smdmifIterator
{
    smiIterator      mCommon;
    knlLogicalAddr   mCurFixedPartAddr;
    stlInt32         mCurFixedRowIndex;
} smdmifIterator;

#define SMDMIF_SCAN_GET_TABLE_HEADER(aIterator)    \
    ( (smdmifTableHeader *)((aIterator)->mCommon.mRelationHandle) )

#define SMDMIF_BEFORE_FIRST_ROW_INDEX      -1


stlStatus smdmifScanInitIterator( void   *aIterator,
                                  smlEnv *aEnv );

stlStatus smdmifScanResetIterator( void   *aIterator,
                                   smlEnv *aEnv );

stlStatus smdmifScanFetchNext( void          * aIterator,
                               smlFetchInfo  * aFetchInfo,
                               smlEnv        * aEnv );

stlStatus smdmifScanFetchPrev( void          * aIterator,
                               smlFetchInfo  * aFetchInfo,
                               smlEnv        * aEnv );

stlStatus smdmifScanMoveToRowRid( void   *aIterator,
                                  smlRid *aRowRid,
                                  smlEnv *aEnv );

stlStatus smdmifScanMoveToPosNum( void     *aIterator,
                                  stlInt64  aPosNum,
                                  smlEnv   *aEnv );

/** @} */

