/*******************************************************************************
 * smsmcScan.h
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


#ifndef _SMSMCSCAN_H_
#define _SMSMCSCAN_H_ 1

/**
 * @file smsmcScan.h
 * @brief Storage Manager Layer Memory Circular Full Scan Component Internal Routines
 */

/**
 * @defgroup smsmcScan Memory Curcular Segment Full Scan
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmcAllocSegIterator( smlStatement   * aStmt,
                                 void          ** aSegIterator,
                                 smlEnv         * aEnv );
stlStatus smsmcFreeSegIterator( void   * aSegIterator,
                                smlEnv * aEnv );
stlStatus smsmcGetFirstPage( smlTbsId      aTbsId,
                             void        * aSegHandle,
                             void        * aSegIterator,
                             smlPid      * aPageId,
                             smlEnv      * aEnv );
stlStatus smsmcGetNextPage( smlTbsId      aTbsId,
                            void        * aSegIterator,
                            smlPid      * aPageId,
                            smlEnv      * aEnv );

/** @} */
    
#endif /* _SMSMCSCAN_H_ */
