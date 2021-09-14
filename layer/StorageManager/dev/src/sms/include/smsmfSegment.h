/*******************************************************************************
 * smsmfSegment.h
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


#ifndef _SMSMFSEGMENT_H_
#define _SMSMFSEGMENT_H_ 1

/**
 * @file smsmfSegment.h
 * @brief Storage Manager Layer Memory Flat Segment Component Internal Routines
 */

/**
 * @defgroup smsmfSegment Memory Flat Segment
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmfAllocHandle( smxlTransId       aTransId,
                            smlRid            aSegRid,
                            smlTbsId          aTbsId,
                            smlSegmentAttr  * aAttr,
                            stlBool           aHasRelHdr,
                            smsSegHdr       * aSegHdr,
                            void           ** aSegmentHandle,
                            smlEnv          * aEnv );
stlStatus smsmfCreate( smxlTransId    aTransId,
                       smlTbsId       aTbsId,
                       void         * aSegmentHandle,
                       stlInt32       aInitialExtents,
                       smlEnv       * aEnv );
stlStatus smsmfDrop( smxlTransId   aTransId,
                     smlTbsId      aTbsId,
                     void        * aSegmentHandle,
                     smlEnv      * aEnv );
stlStatus smsmfAllocPage( smxmTrans   * aMiniTrans,
                          smlTbsId      aTbsId,
                          void        * aSegmentHandle,
                          smpPageType   aPageType,
                          smpHandle   * aPageHandle,
                          smlEnv      * aEnv );
stlStatus smsmfGetInsertablePage( smxmTrans     * aMiniTrans,
                                  smlTbsId        aTbsId,
                                  void          * aSegmentHandle,
                                  smpPageType     aPageType,
                                  smsSearchHint * aSearchHint,
                                  smpHandle     * aPageHandle,
                                  stlBool       * aIsNewPage,
                                  smlEnv        * aEnv );
stlStatus smsmfUpdatePageStatus( smxmTrans   * aMiniTrans,
                                 smlTbsId      aTbsId,
                                 smpHandle   * aPageHandle,
                                 smpFreeness   aFreeness,
                                 smlScn        aScn,
                                 smlEnv      * aEnv );
stlStatus smsmfAllocNewExtent( smsmfCache  * aCache,
                               smxlTransId   aTransId,
                               smlTbsId      aTbsId,
                               smlPid      * aFristPid,
                               smlExtId    * aExtId,
                               smlEnv      * aEnv );
stlStatus smsmfAllocExtent( smxlTransId   aTransId,
                            smlTbsId      aTbsId,
                            void        * aCache,
                            smpPageType   aPageType,
                            smlPid      * aFirstDataPid,
                            smlEnv      * aEnv );
stlStatus smsmfBuildCache( smlTbsId    aTbsId,
                           smlPid      aSegHdrPid,
                           void     ** aSegmentHandle,
                           smlEnv    * aEnv );
stlInt32 smsmfGetHeaderSize();
stlInt64 smsmfGetAllocPageCount( void * aSegmentHandle );

/** @} */
    
#endif /* _SMSMFSEGMENT_H_ */
