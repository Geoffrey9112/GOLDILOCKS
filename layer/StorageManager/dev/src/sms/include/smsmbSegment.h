/*******************************************************************************
 * smsmbSegment.h
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


#ifndef _SMSMBSEGMENT_H_
#define _SMSMBSEGMENT_H_ 1

/**
 * @file smsmbSegment.h
 * @brief Storage Manager Layer Memory Bitmap Segment Component Internal Routines
 */

/**
 * @defgroup smsmbSegment Memory Bitmap Segment
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmbAllocHandle( smxlTransId       aTransId,
                            smlRid            aSegRid,
                            smlTbsId          aTbsId,
                            smlSegmentAttr  * aAttr,
                            stlBool           aHasRelHdr,
                            smsSegHdr       * aSegHdr,
                            void           ** aSegmentHandle,
                            smlEnv          * aEnv );
stlStatus smsmbCloneHandle( smxlTransId     aTransId,
                            smlRid          aNewSegRid,
                            void          * aOrgSegHandle,
                            smsSegHdr     * aNewSegHdr,
                            void         ** aNewSegHandle,
                            smlEnv        * aEnv );
stlStatus smsmbCreate( smxlTransId    aTransId,
                       smlTbsId       aTbsId,
                       void         * aSegHandle,
                       stlInt32       aInitialExtents,
                       smlEnv       * aEnv );
stlStatus smsmbDrop( smxlTransId   aTransId,
                     smlTbsId      aTbsId,
                     void        * aSegmentHandle,
                     smlEnv      * aEnv );
stlStatus smsmbReset( smxlTransId   aTransId,
                      smlTbsId      aTbsId,
                      void        * aSegHandle,
                      stlBool       aFreePages,
                      smlEnv      * aEnv );
stlStatus smsmbAllocPage( smxmTrans       * aMiniTrans,
                          smlTbsId          aTbsId,
                          void            * aSegmentHandle,
                          smpPageType       aPageType,
                          smpIsAgableFunc   aIsAgableFunc,
                          smpHandle       * aPageHandle,
                          smlEnv          * aEnv );
stlStatus smsmbGetInsertablePage( smxmTrans       * aMiniTrans,
                                  smlTbsId          aTbsId,
                                  void            * aSegmentHandle,
                                  smpPageType       aPageType,
                                  smpIsAgableFunc   aIsAgableFunc,
                                  smsSearchHint   * aSearchHint,
                                  smpHandle       * aPageHandle,
                                  stlBool         * aIsNewPage,
                                  smlEnv          * aEnv );
stlStatus smsmbUpdatePageStatus( smxmTrans   * aMiniTrans,
                                 smlTbsId      aTbsId,
                                 smpHandle   * aPageHandle,
                                 smpFreeness   aFreeness,
                                 smlScn        aScn,
                                 smlEnv      * aEnv );
stlStatus smsmbUpdatePageStatusInternal( smxmTrans   * aMiniTrans,
                                         smlTbsId      aTbsId,
                                         smpHandle   * aPageHandle,
                                         smpFreeness   aFreeness,
                                         smlEnv      * aEnv );
stlStatus smsmbGetValidHandle( void         * aFstSegHandle,
                               smxlTransId    aTransId,
                               smlScn         aTransViewScn,
                               void        ** aValidSegHandle,
                               smlEnv       * aEnv );
stlStatus smsmbBuildCache( smlTbsId     aTbsId,
                           smlPid       aSegHdrPid,
                           void      ** aSegmentHandle,
                           smlEnv     * aEnv );
stlStatus smsmbAllocExtent( smxlTransId    aTransId,
                            smlTbsId       aTbsId,
                            void         * aCache,
                            smpPageType    aPageType,
                            smlPid       * aFirstDataPid,
                            smlEnv       * aEnv );
stlStatus smsmbAddNewExtent( smxlTransId   aTransId,
                             smlTbsId      aTbsId,
                             smsmbCache  * aCache,
                             stlInt32      aExtCount,
                             stlBool       aIsForCreate,
                             smlEnv      * aEnv );
stlBool smsmbIsAgableForScan( smlTbsId    aTbsId,
                              smlScn      aPageScn,
                              smlEnv    * aEnv );
stlBool smsmbIsAgableForAlloc( smlTbsId          aTbsId,
                               smlScn            aPageScn,
                               smlPid            aPageId,
                               smpIsAgableFunc   aIsAgableFunc,
                               smlEnv          * aEnv );
stlInt32 smsmbGetHeaderSize();
stlInt32 smsmbGetCacheSize();
stlInt64 smsmbGetAllocPageCount( void * aSegmentHandle );
void smsmbSetFreeness( stlChar       * aBitArray,
                       stlInt32        aOffset,
                       smpFreeness     aFreeness );
smpFreeness smsmbGetFreeness( stlChar  * aBitArray,
                              stlInt32   aOffset );
smpFreeness smsmbRepresentFreeness( void    * aHeader,
                                    stlBool   aIsLeaf );

stlStatus smsmbGetUsedPageCount( void       * aSegmentHandle,
                                 stlInt64   * aUsedPageCount,
                                 smlEnv     * aEnv );

stlStatus smsmbInitCache( smsmbCache * aCache,
                          smlEnv     * aEnv );
stlStatus smsmbGetInsertablePageInternal( smxmTrans       * aMiniTrans,
                                          smlTbsId          aTbsId,
                                          void            * aSegmentHandle,
                                          smpPageType       aPageType,
                                          smpFreeness       aFreeness,
                                          smpIsAgableFunc   aIsAgableFunc,
                                          smsSearchHint   * aSearchHint,
                                          smpHandle       * aPageHandle,
                                          stlBool         * aIsNewPage,
                                          smlEnv          * aEnv );

/** @} */
    
#endif /* _SMSMBSEGMENT_H_ */
