/*******************************************************************************
 * sms.h
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


#ifndef _SMS_H_
#define _SMS_H_ 1

/**
 * @file sms.h
 * @brief Storage Manager Layer Segment Internal Routines
 */

/**
 * @defgroup sms Segment
 * @ingroup smInternal
 * @{
 */

stlStatus smsAllocSegHdr( smxmTrans       * aMiniTrans,
                          smlTbsId          aTbsId,
                          smlSegType        aSegType,
                          smlSegmentAttr  * aAttr,
                          stlBool           aHasRelHdr,
                          smlRid          * aSegRid,
                          void           ** aSegHandle,
                          smlEnv          * aEnv );
stlStatus smsAllocSegHdrForTruncate( smxmTrans   * aMiniTrans,
                                     void        * aOrgSegHandle,
                                     smlRid      * aNewSegRid,
                                     void       ** aNewSegHandle,
                                     smlEnv      * aEnv );
stlStatus smsFreeSegHdr( smxlTransId   aTransId,
                         void        * aSegHandle,
                         smlEnv      * aEnv );
stlStatus smsCreate( smxlTransId   aTransId,
                     smlTbsId      aTbsId,
                     smlSegType    aSegType,
                     void        * aSegHandle,
                     stlInt64      aInitialExtent,
                     smlEnv      * aEnv );
stlStatus smsDrop( smxlTransId  aTransId,
                   void       * aSegmentHandle,
                   stlBool      aOnStartup,
                   smlEnv     * aEnv );
stlStatus smsDropBody( smxlTransId  aTransId,
                       void       * aSegmentHandle,
                       smlEnv     * aEnv );
stlStatus smsPendingDrop( smxlTransId  aTransId,
                          void       * aSegmentHandle,
                          smlEnv     * aEnv );
stlStatus smsReset( smxlTransId  aTransId,
                    void       * aSegHandle,
                    stlBool      aFreePages,
                    smlEnv     * aEnv );
stlStatus smsAllocPage( smxmTrans       * aMiniTrans,
                        void            * aSegmentHandle,
                        smpPageType       aPageType,
                        smpIsAgableFunc   sIsAgableFunc,
                        smpHandle       * aPageHandle,
                        smlEnv          * aEnv );
stlStatus smsPreparePage( smxlTransId   aTransId,
                          void        * aSegmentHandle,
                          stlUInt32     aPageCount,
                          smlEnv      * aEnv );
stlStatus smsAllocMultiPage( smxmTrans       * aMiniTrans,
                             void            * aSegmentHandle,
                             smpPageType       aPageType,
                             stlInt32          aPageCount,
                             smpIsAgableFunc   aIsAgableFunc,
                             smpHandle       * aPageHandle,
                             smlEnv          * aEnv );
stlStatus smsAllocExtent( smxlTransId   aTransId,
                          void        * aSegmentHandle,
                          smpPageType   aPageType,
                          smlPid      * aFirstDataPid,
                          smlEnv      * aEnv );
stlStatus smsStealPages( smxmTrans   * aMiniTrans,
                         void        * aDstSegHandle,
                         void        * aSrcSegHandle,
                         stlInt64    * aStolenPageCount,
                         smlEnv      * aEnv );
stlStatus smsGetInsertablePage( smxmTrans            * aMiniTrans,
                                void                 * aSegmentHandle,
                                smpPageType            aPageType,
                                smpIsAgableFunc        sIsAgableFunc,
                                smsSearchHint        * aSearchHint,
                                smpHandle            * aPageHandle,
                                stlBool              * aIsNewPage,
                                smlEnv               * aEnv );
stlStatus smsUpdatePageStatus( smxmTrans   * aMiniTrans,
                               void        * aSegmentHandle,
                               smpHandle   * aPageHandle,
                               smpFreeness   aFreeness,
                               smlScn        aScn,
                               smlEnv      * aEnv );
stlStatus smsAlterSegmentAttr( smxlTransId      aTransId,
                               void           * aSegmentHandle,
                               smlSegmentAttr * aSegmentAttr,
                               smlEnv         * aEnv );
stlStatus smsBuildCache( smlTbsId     aTbsId,
                         smlPid       aSegHdrPid,
                         void      ** aSegmentHandle,
                         smlEnv     * aEnv );
stlStatus smsAllocSegIterator( smlStatement   * aStmt,
                               void           * aSegmentHandle,
                               void          ** aSegIterator,
                               smlEnv         * aEnv );
stlStatus smsFreeSegIterator( void    * aSegmentHandle,
                              void    * aSegIterator,
                              smlEnv  * aEnv );
stlStatus smsGetFirstPage( void   * aSegmentHandle,
                           void   * aSegIterator,
                           smlPid * aPageId,
                           smlEnv * aEnv );
stlStatus smsGetNextPage( void    * aSegmentHandle,
                          void    * aSegIterator,
                          smlPid  * aPageId,
                          smlEnv  * aEnv );
stlInt32 smsGetHeaderSize( void * aSegmentHandle );
stlInt32 smsGetCacheSize( void * aSegmentHandle );

stlStatus smsMapCreate( smxlTransId  aTransId,
                        smlEnv     * aEnv );
stlStatus smsGetSegmentHandle( smlRid    aSegRid,
                               void   ** aSegHandle,
                               smlEnv  * aEnv );
stlStatus smsSetTransToHandle( void       * aSegmentHandle,
                               smxlTransId  aTransId,
                               smlScn       aTransViewScn,
                               smlEnv     * aEnv );
stlStatus smsShrinkTo( smxlTransId      aTransId,
                       void           * aSegmentHandle,
                       smlSegmentAttr * aAttr,
                       smlEnv         * aEnv );
stlInt64 smsGetAllocPageCount( void * aSegmentHandle );
stlStatus smsGetUsedPageCount4Statistics( void     * aSegmentHandle,
                                          stlInt64 * aUsedPageCount,
                                          smlEnv   * aEnv );
stlInt32 smsGetMinExtents( void * aSegmentHandle );

void smsMergeSegmentAttr( void           * aSegmentHandle,
                          smlSegmentAttr * aMergeSegAttr,
                          smlSegmentAttr * aMergedSegAttr );

void * smsGetSegmentMapHandle();
inline smlSegType smsGetSegType( void * aSegmentHandle );
inline smlTbsId smsGetTbsId( void * aSegmentHandle );
inline stlBool smsHasRelHdr( void * aSegmentHandle );
smlRid smsGetSegRid( void   * aSegmentHandle );
stlInt64 smsGetSegmentId( void * aSegmentHandle );
smlScn smsGetValidScn( void * aSegmentHandle );
stlInt64 smsGetValidSeqFromHandle( void * aSegmentHandle );
stlStatus smsGetValidSeq( smlRid     aSegRid,
                          stlInt64 * aValidSeq,
                          smlEnv   * aEnv );
void smsSetUsablePageCountInExt( void    * aSegmentHandle,
                                 stlInt64  aUsablePageCountInExt );
stlInt64 smsGetUsablePageCountInExt( void * aSegmentHandle );
stlStatus smsGetSegmentState( smlRid     aSegRid,
                              stlInt8  * aSegState,
                              smlEnv   * aEnv );
stlStatus smsFreeSegMapPage( smxlTransId  aTransId,
                             smlRid       aSegRid,
                             smlEnv     * aEnv );
stlStatus smsFreeSegMapColumnMeta( smxlTransId   aTransId,
                                   smlPid        aColPid,
                                   smlEnv      * aEnv );
stlStatus smsCreateSegHint( smlSessionEnv * aSessionEnv,
                            smlEnv        * aEnv );

stlStatus smsSetUsableSegment( smlStatement * aStatement,
                               void         * aSegmentHandle,
                               smlEnv       * aEnv );
stlBool smsIsUsableSegment( void * aSegmentHandle );
stlStatus smsUnusablePending( stlUInt32   aActionFlag,
                              smgPendOp * aPendOp,
                              smlEnv    * aEnv );
stlStatus smsUsablePending( stlUInt32   aActionFlag,
                            smgPendOp * aPendOp,
                            smlEnv    * aEnv );
stlStatus smsSetUnusableSegment( smlStatement * aStatement,
                                 void         * aSegmentHandle,
                                 smlEnv       * aEnv );
stlBool smsIsIrrecoverableSegment( stlInt64 sSegmentId );
stlStatus smsValidateSegment( void   * aSegmentHandle,
                              smlEnv * aEnv );
stlStatus smsIrrecoverableSegmentToUnusable( smlEnv * aEnv );
stlStatus smsAddIrrecoverableSegment( stlInt64    aSegmentId,
                                      smlEnv    * aEnv );
stlBool smsIsValidSegmentId( stlInt64  aSegmentId );

stlStatus smsStartupNoMount( void  ** aWarmupEntry,
                             smlEnv * aEnv );
stlStatus smsStartupMount( smlEnv * aEnv );
stlStatus smsStartupOpen( smlEnv * aEnv );
stlStatus smsWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv );
stlStatus smsCooldown( smlEnv * aEnv );
stlStatus smsBootdown( smlEnv * aEnv );
stlStatus smsShutdownClose( void * aEnv );
stlStatus smsRegisterFxTables( smlEnv * aEnv );

stlStatus smsInternalError( void           * aSegmentHandle,
                            smlEnv         * aEnv,
                            const stlChar  * aFormat,
                            ... );

stlStatus smsAllocSegMapIterator( smlStatement  * aStatement,
                                  void         ** aIterator,
                                  smlEnv        * aEnv );
stlStatus smsFreeSegMapIterator( void   * aIterator,
                                 smlEnv * aEnv );
stlStatus smsGetFirstSegment( void    * aIterator,
                              void   ** aSegmentHandle,
                              smlRid  * aSegmentRid,
                              smlEnv  * aEnv );
stlStatus smsGetNextSegment( void    * aIterator,
                             void   ** aSegmentHandle,
                             smlRid  * aSegmentRid,
                             smlEnv  * aEnv );

stlStatus smsAllocColMetaHdr( smxmTrans  * aMiniTrans,
                              smlSegType   aSegType,
                              smlPid     * aPageId,
                              smlEnv     * aEnv );

inline stlBool smsIsSegHdrCollenPage( stlInt32  aPageType );

/** @} */
    
#endif /* _SMS_H_ */
