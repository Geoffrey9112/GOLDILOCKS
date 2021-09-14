/*******************************************************************************
 * smsmbLog.h
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


#ifndef _SMSMBLOG_H_
#define _SMSMBLOG_H_ 1

/**
 * @file smsmbLog.h
 * @brief Storage Manager Layer Memory Bitmap Logging Component Internal Routines
 */

/**
 * @defgroup smsmbLog Memory Bitmap Segment Logging
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmbWriteLogUpdateLeafStatus( smxmTrans   * aMiniTrans,
                                         smlTbsId      aTbsId,
                                         smlPid        aPageId,
                                         stlInt16      aOffset,
                                         smpFreeness   aBeforeFreeness,
                                         smpFreeness   aAfterFreeness,
                                         smlScn        aBeforeScn,
                                         smlScn        aAfterScn,
                                         smlEnv      * aEnv );
stlStatus smsmbWriteLogUpdateInternalStatus( smxmTrans   * aMiniTrans,
                                             smlTbsId      aTbsId,
                                             smlPid        aPageId,
                                             stlInt16      aOffset,
                                             smpFreeness   aBeforeFreeness,
                                             smpFreeness   aAfterFreeness,
                                             smlEnv      * aEnv );
stlStatus smsmbWriteLogFreeExtMap( smxmTrans   * aMiniTrans,
                                   smlTbsId      aTbsId,
                                   smlPid        aPageId,
                                   stlInt16      aOffset,
                                   stlInt32      aStartDesc,
                                   stlInt32      aEndDesc,
                                   smlEnv      * aEnv );
stlStatus smsmbWriteLogUpdateSegHdr( smxmTrans   * aMiniTrans,
                                     smlTbsId      aTbsId,
                                     smlPid        aPageId,
                                     stlInt16      aOffset,
                                     smsmbSegHdr * aSegHdr,
                                     smlEnv      * aEnv );
stlStatus smsmbWriteLogExpandLeaf( smxmTrans   * aMiniTrans,
                                   smlTbsId      aTbsId,
                                   smlPid        aPageId,
                                   stlInt16      aOffset,
                                   smlPid        aStartPid,
                                   stlInt32      aPageCount,
                                   smlEnv      * aEnv );
stlStatus smsmbWriteLogInsertInternalBit( smxmTrans   * aMiniTrans,
                                          smlTbsId      aTbsId,
                                          smlPid        aPageId,
                                          stlInt16      aOffset,
                                          smlPid        aInsertPid,
                                          smpFreeness   aInsertFreeness,
                                          smlEnv      * aEnv );
stlStatus smsmbWriteLogInitLeaf( smxmTrans   * aMiniTrans,
                                 smlTbsId      aTbsId,
                                 smlPid        aPageId,
                                 stlInt16      aOffset,
                                 smlEnv      * aEnv );
stlStatus smsmbWriteLogInitInternal( smxmTrans   * aMiniTrans,
                                     smlTbsId      aTbsId,
                                     smlPid        aPageId,
                                     stlInt16      aOffset,
                                     smlEnv      * aEnv );
stlStatus smsmbWriteLogInitExtMap( smxmTrans   * aMiniTrans,
                                   smlTbsId      aTbsId,
                                   smlPid        aPageId,
                                   stlInt16      aOffset,
                                   smlPid        aPrvMapPid,
                                   smlEnv      * aEnv );
stlStatus smsmbWriteLogAddExt( smxmTrans   * aMiniTrans,
                               smlTbsId      aTbsId,
                               smlPid        aPageId,
                               stlInt16      aOffset,
                               smlExtId      aExtId,
                               smlEnv      * aEnv );

stlStatus smsmbWriteLogLeafLink( smxmTrans   * aMiniTrans,
                                 smlTbsId      aTbsId,
                                 smlPid        aTargetPid,
                                 smlPid        aLinkPid,
                                 smlEnv      * aEnv );

stlStatus smsmbWriteLogUnlinkLstExtMap( smxmTrans   * aMiniTrans,
                                        smsmbCache  * aCache,
                                        smlPid        aPrvPid,
                                        smlEnv      * aEnv );
/** @} */
    
#endif /* _SMSMBLOG_H_ */
