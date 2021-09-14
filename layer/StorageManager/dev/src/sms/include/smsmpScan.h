/*******************************************************************************
 * smsmpScan.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smsmpScan.h 1551 2011-07-28 03:36:30Z mycomman $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMSMPSCAN_H_
#define _SMSMPSCAN_H_ 1

/**
 * @file smsmpScan.h
 * @brief Storage Manager Layer Memory Pending Full Scan Component Internal Routines
 */

/**
 * @defgroup smsmpScan Memory Pending Segment Full Scan
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmpAllocSegIterator( smlStatement  * aStatement,
                                 void         ** aSegIterator,
                                 smlEnv        * aEnv );
stlStatus smsmpFreeSegIterator( void   * aSegIterator,
                                smlEnv * aEnv );
stlStatus smsmpGetFirstPage( smlTbsId      aTbsId,
                             void        * aSegHandle,
                             void        * aSegIterator,
                             smlPid      * aPageId,
                             smlEnv      * aEnv );
stlStatus smsmpGetNextPage( smlTbsId      aTbsId,
                            void        * aSegIterator,
                            smlPid      * aPageId,
                            smlEnv      * aEnv );

/** @} */
    
#endif /* _SMSMPSCAN_H_ */
