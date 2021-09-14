/*******************************************************************************
 * smsmbScan.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMSMBSCAN_H_
#define _SMSMBSCAN_H_ 1

/**
 * @file smsmbScan.h
 * @brief Storage Manager Layer Memory Bitmap Full Scan Component Internal Routines
 */

/**
 * @defgroup smsmbScan Memory Bitmap Segment Full Scan
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmbAllocSegIterator( smlStatement   * aStmt,
                                 void          ** aSegIterator,
                                 smlEnv         * aEnv );
stlStatus smsmbFreeSegIterator( void   * aSegIterator,
                                smlEnv * aEnv );
stlStatus smsmbGetFirstPage( smlTbsId      aTbsId,
                             void        * aSegHandle,
                             void        * aSegIterator,
                             smlPid      * aPageId,
                             smlEnv      * aEnv );
stlStatus smsmbGetNextPage( smlTbsId      aTbsId,
                            void        * aSegIterator,
                            smlPid      * aPageId,
                            smlEnv      * aEnv );
/** @} */
    
#endif /* _SMSMBSCAN_H_ */
