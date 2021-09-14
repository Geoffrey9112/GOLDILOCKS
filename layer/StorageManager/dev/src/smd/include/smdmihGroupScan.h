/*******************************************************************************
 * smdmihGroupScan.h
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmihGroupScan.h 6127 2012-10-29 10:46:38Z htkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smdmihGroupScan.h
 * @brief Storage Manager Layer Memory Instant Hash Table GroupScan Routines
 */

/**
 * @defgroup smdmihGroupScan Memory Instant Hash Table Group Scan
 * @ingroup smdInternal
 * @{
 */

stlStatus smdmihGroupInitIterator( void   * aIterator,
                                       smlEnv * aEnv );

stlStatus smdmihGroupResetIterator( void   * aIterator,
                                    smlEnv * aEnv );

stlStatus smdmihGroupFetchNext( void          * aIterator,
                                smlFetchInfo  * aFetchInfo,
                                smlEnv        * aEnv );

stlStatus smdmihGroupMoveToNextKey( void          * aIterator,
                                    smlFetchInfo  * aFetchInfo,
                                    smlEnv        * aEnv );

/** @} */

