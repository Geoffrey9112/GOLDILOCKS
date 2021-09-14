/*******************************************************************************
 * smsmfScan.h
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


#ifndef _SMSMFSCAN_H_
#define _SMSMFSCAN_H_ 1

/**
 * @file smsmfScan.h
 * @brief Storage Manager Layer Memory Flat Full Scan Component Internal Routines
 */

/**
 * @defgroup smsmfScan Memory Flat Segment Full Scan
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmfAllocSegIterator( smlStatement   * aStmt,
                                 void          ** aSegIterator,
                                 smlEnv         * aEnv );
stlStatus smsmfFreeSegIterator( void   * aSegIterator,
                                smlEnv * aEnv );
stlStatus smsmfGetFirstPage( smlTbsId      aTbsId,
                             void        * aSegHandle,
                             void        * aSegIterator,
                             smlPid      * aPageId,
                             smlEnv      * aEnv );
stlStatus smsmfGetNextPage( smlTbsId      aTbsId,
                            void        * aSegIterator,
                            smlPid      * aPageId,
                            smlEnv      * aEnv );

/** @} */
    
#endif /* _SMSMFSCAN_H_ */
