/*******************************************************************************
 * smdmihScan.h
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmihScan.h 6127 2012-10-29 10:46:38Z htkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smdmihScan.h
 * @brief Storage Manager Layer Memory Instant Hash Table Scan Routines
 */

/**
 * @defgroup smdmihScan Memory Instant Hash Table
 * @ingroup smdInternal
 * @{
 */

stlStatus smdmihInitIterator( void   * aIterator,
                              smlEnv * aEnv );

stlStatus smdmihResetIterator( void   * aIterator,
                               smlEnv * aEnv );

stlStatus smdmihFetchNext( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

/** @} */

