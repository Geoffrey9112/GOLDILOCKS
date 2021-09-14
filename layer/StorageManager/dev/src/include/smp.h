/*******************************************************************************
 * smp.h
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


#ifndef _SMP_H_
#define _SMP_H_ 1

/**
 * @file smp.h
 * @brief Storage Manager Layer Page Layer Internal Routines
 */

/**
 * @defgroup smp Page Access
 * @ingroup smInternal
 * @{
 */

stlStatus smpCreate( smxmTrans  * aMiniTrans,
                     smlTbsId     aTbsId,
                     smlPid       aPid,
                     smpPageType  aPageType,
                     stlInt64     aSegmentId,
                     smlScn       aScn,
                     smpHandle  * aPageHandle,
                     smlEnv     * aEnv );
stlStatus smpAcquire( smxmTrans  * aMiniTrans,
                      smlTbsId     aTbsId,
                      smlPid       aPid,
                      stlUInt32    aLatchMode,
                      smpHandle  * aPageHandle,
                      smlEnv     * aEnv );
stlStatus smpTryAcquire( smxmTrans  * aMiniTrans,
                         smlTbsId     aTbsId,
                         smlPid       aPageId,
                         stlUInt32    aLatchMode,
                         smpHandle  * aPageHandle,
                         stlBool    * aIsSuccess,
                         smlEnv     * aEnv );
stlStatus smpAcquireWithHandle( smxmTrans  * aMiniTrans,
                                stlUInt32    aLatchMode,
                                smpHandle  * aPageHandle,
                                smlEnv     * aEnv );
stlStatus smpRelease( smpHandle * aPageHandle,
                      smlEnv    * aEnv );
stlStatus smpTestExclusive( smlTbsId     aTbsId,
                            smlPid       aPageId,
                            stlBool    * aIsSuccess,
                            smlEnv     * aEnv );
stlStatus smpFix( smlTbsId     aTbsId,
                  smlPid       aPid,
                  smpHandle  * aPageHandle,
                  smlEnv     * aEnv );
stlStatus smpUnfix( smpHandle * aPageHandle,
                    smlEnv    * aEnv );
stlStatus smpValidatePage( smpHandle * aPageHandle,
                           smlEnv    * aEnv );
stlStatus smpInitHdr( smxmTrans   * aMiniTrans,
                      smlTbsId      aTbsId,
                      smpHandle   * aPageHandle,
                      smlRid        aParentRid,
                      smpPageType   aPageType,
                      stlInt64      aSegmentId,
                      smlScn        aScn,
                      smlEnv      * aEnv );
stlStatus smpUpdateFreeness( smxmTrans   * aMiniTrans,
                             smlTbsId      aTbsId,
                             smpHandle   * aPageHandle,
                             smpFreeness   aFreeness,
                             smlEnv      * aEnv );
stlStatus smpUpdateAgableScn( smxmTrans   * aMiniTrans,
                              smpHandle   * aPageHandle,
                              smlScn        aAgableScn,
                              smlEnv      * aEnv );
stlBool smpIsAgable( smpHandle       * aPageHandle,
                     smpIsAgableFunc   aIsAgableFunc,
                     smlEnv          * aEnv );

stlStatus smpPageFlushEventHandler( void      * aData,
                                    stlUInt32   aDataSize,
                                    stlBool   * aDone,
                                    void      * aEnv );
stlStatus smpPageFlushTbsEventHandler( void      * aData,
                                       stlUInt32   aDataSize,
                                       stlBool   * aDone,
                                       void      * aEnv );
smrLsn smpGetMinFlushedLsn();
void smpGetSystemInfo( smpSystemInfo * aSystemInfo );
smpFreeness smpGetFreeness( smpHandle * aPageHandle );
smlScn smpGetAgableScn( smpHandle * aPageHandle );
stlStatus smpSetDirty( smpHandle * aPageHandle,
                       smlEnv    * aEnv );
stlUInt16 smpCurLatchMode( smpHandle * aPageHandle );
void smpSetRecoveryLsn( smpHandle * aPageHandle,
                        smrSbsn     aSbsn,
                        smrLsn      aLsn,
                        stlInt16    aLpsn );
smrLsn smpGetRecoveryLsn( smpHandle * aPageHandle );
stlInt16 smpGetRecoveryLpsn( smpHandle * aPageHandle );
void * smpGetBody( void * aPage );
stlStatus smpInit( smlTbsId        aTbsId,
                   smlDatafileId   aDatafileId,
                   smlEnv        * aEnv );
stlStatus smpExtend( smlStatement  * aStatement,
                     smlTbsId        aTbsId,
                     smlDatafileId   aDatafileId,
                     stlBool         aNeedPending,
                     smlEnv        * aEnv );
stlStatus smpExtendTablespace( smlStatement  * aStatement,
                               smlTbsId        aTbsId,
                               stlBool         aNeedPending,
                               smlEnv        * aEnv );
stlStatus smpShrink( smlStatement  * aStatement,
                     smlTbsId        aTbsId,
                     smlDatafileId   aDatafileId,
                     stlInt16        aPchArrayState,
                     smlEnv        * aEnv );

stlStatus smpShrinkCorruptedTbs( smlTbsId   aTbsId,
                                 smlEnv   * aEnv );
stlStatus smpShrinkTablespace( smlTbsId   aTbsId,
                               smlEnv   * aEnv );
stlStatus smpShrinkDatafile( smlTbsId         aTbsId,
                             smlDatafileId    aDatafileId,
                             smlEnv         * aEnv );

stlStatus smpFlushPage4Shutdown( smlEnv  * aEnv );
stlStatus smpFlushPagesForTablespace( smlStatement * aStatement,
                                      smlTbsId       aTbsId,
                                      stlInt32       aReason,
                                      stlInt32       aDatafileId,
                                      smlEnv       * aEnv );

stlStatus smpStartupNoMount( void   ** aWarmupEntry,
                             smlEnv  * aEnv );
stlStatus smpWarmup( void   * aWarmupEntry,
                     smlEnv * aEnv );
stlStatus smpCooldown( smlEnv * aEnv );
stlStatus smpBootdown( smlEnv * aEnv );
stlStatus smpStartupMount( smlEnv * aEnv );
stlStatus smpStartupPreOpen( smlEnv * aEnv );
stlStatus smpStartupOpen( smlEnv * aEnv );
stlStatus smpShutdownPostClose( smlEnv * aEnv );
stlStatus smpShutdownClose( smlEnv * aEnv );
stlStatus smpRegisterFxTables( smlEnv * aEnv );

stlStatus smpFlushPagesIoSlaveEventHandler( void      * aData,
                                            stlUInt32   aDataSize,
                                            stlBool   * aDone,
                                            void      * aEnv );
smlPid smpGetPageId( smpHandle * aPageHandle );
smlTbsId smpGetTbsId( smpHandle * aPageHandle );
smpPageType smpGetPageType( smpHandle * aPageHandle );
void smpSetVolatileArea( smpHandle * aPageHandle,
                         void      * aData,
                         stlInt32    aDataSize );
void * smpGetVolatileArea( smpHandle * aPageHandle );
void smpInitVolatileArea( smpHandle * aPageHandle );

stlStatus smpInitBody( smpHandle * aPageHandle,
                       stlUInt16   aPageMgmtType,
                       stlUInt16   aLogicalHeaderSize,
                       stlChar   * aLogicalHeader,
                       stlUInt8    aMinRtsCount,
                       stlUInt8    aMaxRtsCount,
                       stlBool     aUseFreeSlotList,
                       smxmTrans * aMtx,
                       smlEnv    * aEnv );

stlStatus smpInitFixedPageBody( smxmTrans  * aMiniTrans,
                                smpHandle  * aPageHandle,
                                stlBool      aUseFreeSlotList,
                                stlUInt16    aPageMgmtType,
                                stlUInt16    aLogicalHeaderSize,
                                stlChar    * aLogicalHeader,
                                stlUInt16    aRowHdrLen,
                                stlInt32     aFromOrd,
                                stlInt32     aToOrd,
                                stlInt16   * aColLengthArray,
                                stlInt16     aContColumnThreshold,
                                smlEnv     * aEnv );

stlStatus smpResetBody( smpHandle * aPageHandle );
stlStatus smpSnapshot( smpHandle * aPageHandle,
                       stlChar   * aPageFrame,
                       smlEnv    * aEnv );

stlStatus smpGetRtsVerCommitScn( smpHandle  * aPageHandle,
                                 smlPid       aPrevPid,
                                 stlInt16     aPrevOffset,
                                 stlUInt8     aPrevVerCnt,
                                 stlUInt8     aTargetVerNo,
                                 smlScn       aMyStmtViewScn,
                                 smlScn     * aCommitScn,
                                 smlEnv     * aEnv );

stlStatus smpGetCommitScn( smlTbsId      aTbsId,
                           smpHandle   * aPageHandle,
                           stlUInt8      aRtsSeq,
                           stlUInt8      aRtsVerNo,
                           smlScn        aMyStmtViewScn,
                           smxlTransId * aWaitTransId,
                           smlScn      * aCommitScn,
                           smlEnv      * aEnv );

inline stlChar * smpGetLogicalHeader( smpHandle * aPageHandle );

stlStatus smpStabilizeRtsRowItems( smpHandle           * aPageHandle,
                                   stlUInt8              aRtsSeq,
                                   smlScn                aCommitScn,
                                   smpStabilizeRowFunc   aStabilizeRow,
                                   smlEnv              * aEnv );

stlStatus smpTryStampRts( smxmTrans           * aMiniTrans,
                          smlTbsId              aTbsId,
                          smpHandle           * aPageHandle,
                          stlUInt8              aRtsSeq,
                          smpStabilizeRowFunc   aStabilizeRow,
                          smlEnv              * aEnv  );

stlStatus smpAllocRts( smxmTrans           * aMiniTrans,
                       smpHandle           * aPageHandle,
                       smxlTransId           aTransId,
                       smlScn                aScn,
                       smpStabilizeRowFunc   aStabilizeRow,
                       stlUInt8            * aRtsSeq,
                       smlEnv              * aEnv  );

stlStatus smpAllocRtsForCopy( smpHandle          * aPageHandle,
                              smxlTransId          aTransId,
                              smlScn               aScn,
                              stlUInt8             aRtsSeq,
                              smlEnv             * aEnv );

stlStatus smpBeginUseRts( smpHandle          * aPageHandle,
                          stlChar            * aSlot,
                          smxlTransId          aTransId,
                          smlScn               aScn,
                          stlUInt8             aRtsSeq,
                          stlUInt8           * aRtsVerNo,
                          smlEnv             * aEnv );

stlStatus smpBeginUseRtsForCopy( smpHandle          * aPageHandle,
                                 stlChar            * aSlot,
                                 smxlTransId          aTransId,
                                 smlScn               aScn,
                                 stlUInt8             aRtsSeq,
                                 stlUInt8             aRtsVerNo,
                                 smlEnv             * aEnv );

stlStatus smpEndUseRts( smpHandle * aPageHandle,
                        stlChar   * aSlot,
                        stlUInt8    aRtsSeq,
                        stlUInt8    aRtsVerNo,
                        smlEnv    * aEnv );

stlStatus smpSetRtsPrevLink( smpHandle * sPageFrame,
                             stlUInt8    sRtsSeq,
                             smlPid      sPrevPid,
                             stlInt16    sPrevOffset,
                             stlInt16    aPrevVerCnt,
                             smlScn      aPrevCommitScn );

inline stlUInt8 smpGetMinRtsCount( smpHandle * aPageHandle );
inline stlUInt8 smpGetMaxRtsCount( smpHandle * aPageHandle );
inline stlUInt8 smpGetCurRtsCount( smpHandle * aPageHandle );

stlStatus smpExtendRts( smpHandle * aPageHandle,
                        stlBool   * aIsSuccess );

stlStatus smpPrepareNewSlotSeq( smpHandle  * aPageHandle,
                                stlUInt16    aSlotSize,
                                stlInt16   * aSlotSeq );

stlStatus smpAllocSlot( smpHandle  * aPageHandle,
                        stlUInt16    aSlotSize,
                        stlBool      aIsForTest,
                        stlChar   ** aSlot,
                        stlInt16   * aSlotSeq );

stlStatus smpAllocFixedSlotArray( smpHandle              * aPageHandle,
                                  stlInt16                 aRowCount,
                                  stlBool                  aIsForTest,
                                  stlInt16                 aRowHdrLen,
                                  stlInt16                 aColCount,
                                  smpIsUsableFixedSlot     aIsUsableFixedSlot,
                                  stlChar               ** aSlotArray,
                                  stlInt16               * aSlotSeq );

stlStatus smpAllocFixedNthSlotArray( smpHandle  * aPageHandle,
                                     stlInt16     aSlotSeq,
                                     stlInt16     aRowCount,
                                     stlBool      aIsForTest,
                                     stlInt16     aRowHdrLen,
                                     stlInt16     aColCount,
                                     stlChar   ** aSlotArray );

stlStatus smpAllocSlot4Undo( smpHandle  * aPageHandle,
                             stlUInt16    aSlotSize,
                             stlBool      aIsForTest,
                             stlChar   ** aSlot,
                             stlInt16   * aSlotSeq );

stlStatus smpAllocNthSlot( smpHandle  * aPageHandle,
                           stlInt16     aSlotSeq,
                           stlUInt16    aSlotSize,
                           stlBool      aIsForTest,
                           stlChar   ** aSlot );

stlStatus smpReallocNthSlot( smpHandle  * aPageHandle,
                             stlInt16     aSlotSeq,
                             stlUInt16    aSlotSize,
                             stlBool      aIsForTest,
                             stlChar   ** aSlot );

stlStatus smpFreeNthSlot( smpHandle * aPageHandle,
                          stlInt16    aSlotSeq,
                          stlUInt16   aSlotSize );

void smpAddReclaimedSpace( smpHandle * aPageHandle,
                           stlInt16    aFreeSize );

void smpRemoveReclaimedSpace( smpHandle * aPageHandle,
                              stlInt16    aFreeSize );

void smpShrinkOffsetTable( smpHandle  * aPageHandle );

inline void smpReformPage( smpHandle * aPageHandle,
                           stlUInt16   aReclaimedSpace,
                           stlUInt16   aActiveSlotCount,
                           stlUInt16   aRtsCount,
                           void      * aRtsArray,
                           stlUInt16   aRowItemCount,
                           stlUInt16 * aOffsetArray,
                           stlUInt16   aDataSactionSize,
                           stlChar   * aDataSaction );

inline smpOffsetSlot * smpGetFirstOffsetSlot( smpHandle * aPageHandle );

inline stlChar * smpGetNthRowItem( smpHandle * aPageHandle, 
                                   stlInt16    aRowItemSeq );

inline stlChar * smpGetUnsafeNthRowItem( smpHandle * aPageHandle, 
                                         stlInt16    aRowItemSeq );

inline void smpSetPageRowItemCount( smpHandle * aPageHandle,
                                    stlUInt16   aRowItemCount);

inline stlStatus smpValidateOffsetTableHeader( stlChar * aFrame,
                                               smlEnv  * aEnv );

inline stlBool smpIsNthOffsetSlotFree( smpHandle * aPageHandle,
                                stlInt16    aSlotSeq );

inline void smpGetPageDataSection( smpHandle  * aPageHandle,
                                   stlChar   ** aDataPtr,
                                   stlUInt16  * aDataSize );

inline stlUInt16 smpGetRtsSize( );

inline void smpBeforeNthRowItem( void * aPageFrame, smpRowItemIterator * aIterator, stlInt16 aRowItemSeq );

inline void smpAfterNthRowItem( void * aPageFrame, smpRowItemIterator * aIterator, stlInt16 aRowItemSeq );

inline stlUInt16 smpGetUnusedSpace( void * aPageFrame );

inline stlUInt16 smpGetReclaimedSpace( void * aPageFrame );

inline stlUInt16 smpGetActiveSlotCount( void * aPageFrame );

inline stlUInt16 smpGetTotalFreeSpace( void * aPageFrame );

stlInt16 smpGetPageMaxFreeSize( stlInt16 aLogicalHdrSize, stlUInt8 aRtsCount );

inline stlBool smpIsPageOverInsertLimit( void     * aPageFrame, 
                                  stlInt16   aPctFree );

inline stlBool smpIsPageOverFreeLimit( void     * aPageFrame, 
                                       stlInt16   aPctUsed );

stlUInt16 smpGetPageMgmtType( smpHandle * aPageHandle );

stlUInt16 smpGetHighWaterMark( smpHandle * aPageHandle );

stlUInt16 smpGetLogicalHeaderSize( smpHandle * aPageHandle );

stlUInt16 smpGetHintRts( smpHandle * aPageHandle );

stlUInt16 smpGetLowWaterMark( smpHandle * aPageHandle );

void smpCheckRtsRefCount( void            * aPageHandle,
                          smpCountRefFunc   aCountRef,
                          smlEnv          * aEnv );

stlInt64 smpGetSegmentId( smpHandle * aPageHandle );

void smpSetMaxViewScn( smpHandle * aPageHandle,
                       smlScn      aViewScn );
smlScn smpGetMaxViewScn( smpHandle * aPageHandle );

void smpTestPrintPageBody( smpHandle * aPageHandle );

inline smrLsn smpGetPageMaxLsn( smpHandle * aPageHandle );

void * STL_THREAD_FUNC smpParallelLoadThread( stlThread * aThread,
                                              void      * aArg );

stlStatus smpBackupPages( stlFile  * aFile,
                          stlChar  * aFileName,
                          smrLsn     aPrevBackupLsn,
                          smlTbsId   aTbsId,
                          smrLsn   * aBackupLsn,
                          smrLid   * aBackupLid,
                          smlEnv   * aEnv );

stlStatus smpApplyBackup( stlChar        * aBackupFileName,
                          stlChar        * aPageFrame,
                          smlDatafileId  * aDatafileId,
                          void          ** aPchArrayList,
                          smlEnv         * aEnv );

stlStatus smpMakePageChecksum( void      * aPageFrame,
                               stlInt64    aChecksumType,
                               smlEnv    * aEnv );

stlStatus smpValidateChecksum( void      * aPageFrame,
                               stlBool   * aIsValid,
                               smlEnv    * aEnv );

stlStatus smpRestoreDatafileByFullBackup( stlFile                 * aFile,
                                          smrRecoveryDatafileInfo * aDatafileInfo,
                                          smrLsn                    aRestoredLsn,
                                          smlEnv                  * aEnv );

stlStatus smpRestoreDatafileByFullBackup4Corruption( stlFile                 * aFile,
                                                     smrRecoveryDatafileInfo * aDatafileInfo,
                                                     smrLsn                    aRestoredLsn,
                                                     smlEnv                  * aEnv );

stlStatus smpRestoreDatafileByIncBackup( stlFile                 * aFile,
                                         smrRecoveryDatafileInfo * aDatafileInfo,
                                         smfBackupIterator       * aBackupIterator,
                                         smfIncBackupFileTail    * aFileTail,
                                         stlChar                 * aBuffer,
                                         stlInt32                  aBufferSize,
                                         smlEnv                  * aEnv );

stlStatus smpRestoreDatafileByIncBackup4Corruption( stlFile                 * aFile,
                                                    smrRecoveryDatafileInfo * aDatafileInfo,
                                                    smfBackupIterator       * aBackupIterator,
                                                    smfIncBackupFileTail    * aFileTail,
                                                    stlChar                 * aBuffer,
                                                    stlInt32                  aBufferSize,
                                                    smlEnv                  * aEnv );

stlStatus smpMakeCorruptedPageList( smrRecoveryDatafileInfo * aDatafileInfo,
                                    smlEnv                  * aEnv );

stlStatus smpDestroyCorruptedPageList( smrRecoveryDatafileInfo * aDatafileInfo,
                                       smlEnv                  * aEnv );

/** @} */
    
#endif /* _SMP_H_ */
