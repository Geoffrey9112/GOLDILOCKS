/*******************************************************************************
 * smsmpSegment.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smsmpSegment.h 5376 2012-08-22 03:20:49Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMSMPSEGMENT_H_
#define _SMSMPSEGMENT_H_ 1

/**
 * @file smsmpSegment.h
 * @brief Storage Manager Layer Memory Pending Segment Component Internal Routines
 */

/**
 * @defgroup smsmpSegment Memory Pending Segment
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmpAllocHandle( smxlTransId       aTransId,
                            smlRid            aSegRid,
                            smlTbsId          aTbsId,
                            smlSegmentAttr  * aAttr,
                            stlBool           aHasRelHdr,
                            smsSegHdr       * aSegHdr,
                            void           ** aSegmentHandle,
                            smlEnv          * aEnv );
stlStatus smsmpCreate( smxlTransId    aTransId,
                       smlTbsId       aTbsId,
                       void         * aSegmentHandle,
                       stlInt32       aInitialExtents,
                       smlEnv       * aEnv );
stlStatus smsmpDrop( smxlTransId   aTransId,
                     smlTbsId      aTbsId,
                     void        * aSegmentHandle,
                     smlEnv      * aEnv );
stlStatus smsmpReset( smxlTransId   aTransId,
                      smlTbsId      aTbsId,
                      void        * aSegmentHandle,
                      stlBool       aFreePages,
                      smlEnv      * aEnv );
stlStatus smsmpAllocPage( smxmTrans   * aMiniTrans,
                          smlTbsId      aTbsId,
                          void        * aSegmentHandle,
                          smpPageType   aPageType,
                          smpHandle   * aPageHandle,
                          smlEnv      * aEnv );
stlStatus smsmpAllocExtent( smsmpCache  * aCache,
                            smxlTransId   aTransId,
                            smlTbsId      aTbsId,
                            smlPid      * aFirstPid,
                            smlEnv      * aEnv );
stlStatus smsmpGetInsertablePage( smxmTrans       * aMiniTrans,
                                  smlTbsId          aTbsId,
                                  void            * aSegmentHandle,
                                  smpPageType       aPageType,
                                  smpIsAgableFunc   aIsAgableFunc,
                                  smsSearchHint   * aSearchHint,
                                  smpHandle       * aPageHandle,
                                  stlBool         * aIsNewPage,
                                  smlEnv          * aEnv );
stlStatus smsmpUpdatePageStatus( smxmTrans   * aMiniTrans,
                                 smlTbsId      aTbsId,
                                 smpHandle   * aPageHandle,
                                 smpFreeness   aFreeness,
                                 smlScn        aScn,
                                 smlEnv      * aEnv );
stlStatus smsmpBuildCache( smlTbsId    aTbsId,
                           smlPid      aSegHdrPid,
                           void     ** aSegmentHandle,
                           smlEnv    * aEnv );
stlInt32 smsmpGetHeaderSize();
stlInt32 smsmpGetCacheSize();

/** @} */
    
#endif /* _SMSMPSEGMENT_H_ */
