/*******************************************************************************
 * smsmcSegment.h
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


#ifndef _SMSMCSEGMENT_H_
#define _SMSMCSEGMENT_H_ 1

/**
 * @file smsmcSegment.h
 * @brief Storage Manager Layer Memory Circular Segment Component Internal Routines
 */

/**
 * @defgroup smsmcSegment Memory Circular Segment
 * @ingroup smsInternal
 * @{
 */

stlStatus smsmcAllocHandle( smxlTransId       aTransId,
                            smlRid            aSegRid,
                            smlTbsId          aTbsId,
                            smlSegmentAttr  * aAttr,
                            stlBool           aHasRelHdr,
                            smsSegHdr       * aSegHdr,
                            void           ** aSegmentHandle,
                            smlEnv          * aEnv );
stlStatus smsmcCreate( smxlTransId    aTransId,
                       smlTbsId       aTbsId,
                       void         * aSegmentHandle,
                       stlInt32       aInitialExtents,
                       smlEnv       * aEnv );
stlStatus smsmcDrop( smxlTransId   aTransId,
                     smlTbsId      aTbsId,
                     void        * aSegmentHandle,
                     smlEnv      * aEnv );
stlStatus smsmcAllocPage( smxmTrans       * aMiniTrans,
                          smlTbsId          aTbsId,
                          void            * aSegmentHandle,
                          smpPageType       aPageType,
                          smpIsAgableFunc   aIsAgableFunc,
                          smpHandle       * aPageHandle,
                          smlEnv          * aEnv );
stlStatus smsmcPreparePage( smxlTransId   aTransId,
                            void        * aSegmentHandle,
                            stlUInt32     aPageCount,
                            smlEnv      * aEnv );
stlStatus smsmcStealPages( smxmTrans     * aMiniTrans,
                           smlTbsId        aTbsId,
                           void          * aDstSegHandle,
                           void          * aSrcSegHandle,
                           stlInt64        aShrinkToPageCount,
                           stlInt64      * aStolenPageCount,
                           smlEnv        * aEnv );
stlStatus smsmcGetInsertablePage( smxmTrans       * aMiniTrans,
                                  smlTbsId          aTbsId,
                                  void            * aSegmentHandle,
                                  smpPageType       aPageType,
                                  smpIsAgableFunc   aIsAgableFunc,
                                  smsSearchHint   * aSearchHint,
                                  smpHandle       * aPageHandle,
                                  stlBool         * aIsNewPage,
                                  smlEnv          * aEnv );
stlStatus smsmcShrinkTo( smxlTransId      aTransId,
                         void           * aSegmentHandle,
                         smlSegmentAttr * aAttr,
                         smlEnv         * aEnv );
stlStatus smsmcAllocExtent( smsmcCache * aCache,
                            smxlTransId   aTransId,
                            smlTbsId      aTbsId,
                            smlPid      * aFristPid,
                            smlExtId    * aExtId,
                            smlEnv      * aEnv );
stlStatus smsmcBuildCache( smlTbsId    aTbsId,
                           smlPid      aSegHdrPid,
                           void     ** aSegmentHandle,
                           smlEnv    * aEnv );
stlInt32 smsmcGetHeaderSize();
stlInt32 smsmcGetCacheSize();
stlInt64 smsmcGetAllocPageCount( void * aSegmentHandle );
stlStatus smsmcSetTransToHandle( void        * aSegmentHandle,
                                 smxlTransId   aTransId,
                                 smlScn        aTransViewScn,
                                 smlEnv      * aEnv );

stlStatus smsmcFreeExtents( smxlTransId   aTransId,
                            smlTbsId      aTbsId,
                            smsmcCache  * aCache,
                            smlPid        aFirstHdrPid,
                            smlPid        aLastHdrPid,
                            smlEnv      * aEnv );
/** @} */
    
#endif /* _SMSMCSEGMENT_H_ */
