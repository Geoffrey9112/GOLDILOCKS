/*******************************************************************************
 * smdmisVolScan.h
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmisVolScan.h 6127 2012-10-29 10:46:38Z htkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smdmisVolScan.h
 * @brief Storage Manager Layer Memory Instant Sort Table Routines
 */

/**
 * @defgroup smdmisVolScan Memory Instant Sort Table
 * @ingroup smdInternal
 * @{
 */

#include <smdmisTable.h>

typedef struct smdmisVolIterator
{
    smiIterator      mCommon;
    smdmisMergeEnv  *mMergeEnv;
} smdmisVolIterator;

#define SMDMIS_VOL_TABLE_ITERATOR_GET_TABLE_HEADER(aIterator)    \
    ( (smdmisTableHeader *)((aIterator)->mCommon.mRelationHandle) )

stlStatus smdmisVolScanInitIterator( void   *aIterator,
                                  smlEnv *aEnv );

stlStatus smdmisVolScanResetIterator( void   *aIterator,
                                   smlEnv *aEnv );

stlStatus smdmisVolScanFetchNext( void              *aIterator,
                                  smlFetchInfo      *aFetchInfo,
                                  smlEnv            *aEnv );

stlStatus smdmisVolScanFetchPrev( void              *aIterator,
                                  smlFetchInfo      *aFetchInfo,
                                  smlEnv            *aEnv );

stlStatus smdmisVolScanMoveToPosNum( void     *aIterator,
                                  stlInt64  aPosNum,
                                  smlEnv   *aEnv );


/** @} */

