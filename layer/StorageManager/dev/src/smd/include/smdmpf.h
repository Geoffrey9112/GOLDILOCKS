/*******************************************************************************
 * smdmpf.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmpf.h 13496 2014-08-29 05:38:43Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMDMPF_H_
#define _SMDMPF_H_ 1

/**
 * @file smdmpf.h
 * @brief Storage Manager Layer Memory Heap Table Internal Routines
 */

/**
 * @defgroup smdmpf Memory Heap Table
 * @ingroup smInternal
 * @{
 */

stlStatus smdmpfInitTableHeader( smlStatement  * aStatement,
                                 smlRid        * aSegmentRid,
                                 void          * aSegmentHandle,
                                 smlTableAttr  * aAttr,
                                 void         ** aRelationHandle,
                                 smlEnv        * aEnv );
stlStatus smdmpfWriteColumnLength( smlStatement    * aStatement,
                                   smlRid          * aSegmentRid,
                                   void            * aSegmentHandle,
                                   smlColumnarAttr * aColumnAttr,
                                   smlEnv          * aEnv );

stlStatus smdmpfBuildColInfo( smlColumnarAttr  * aColumnAttr,
                              stlInt32         * aMemberPageCount,
                              void            ** aColLengthArray,
                              stlInt16         * aRowCount,
                              smlEnv           * aEnv );

stlStatus smdmpfBuildCache( void    * aRelationHandle,
                            void    * aColumnMeta,
                            smlEnv  * aEnv );

stlStatus smdmpfDestroyColInfo( void    * aColLengthArray,
                                smlEnv  * aEnv );

stlStatus smdmpfDestroyCache( void    * aRelationHandle,
                              smlEnv  * aEnv );

stlStatus smdmpfCreate( smlStatement  * aStatement,
                        smlTbsId        aTbsId,
                        smlTableAttr  * aAttr,
                        stlBool         aLoggingUndo,
                        stlInt64      * aSegmentId,
                        void         ** aRelationHandle,
                        smlEnv        * aEnv );
    
stlStatus smdmpfCreateUndo( smxlTransId   aTransId,
                            void        * aRelationHandle,
                            smlEnv      * aEnv );

stlStatus smdmpfDrop( smlStatement * aStatement,
                      void         * aRelationHandle,
                      smlEnv       * aEnv );

stlStatus smdmpfCopy( smlStatement      * aStatement,
                      void              * aSrcRelHandle,
                      void              * aDstRelHandle,
                      knlValueBlockList * aColumnList,
                      smlEnv            * aEnv );

stlStatus smdmpfCreateForTruncate( smlStatement          * aStatement,
                                   void                  * aOrgRelHandle,
                                   smlDropStorageOption    aDropStorageOption,
                                   void                 ** aNewRelHandle,
                                   stlInt64              * aNewSegmentId,
                                   smlEnv                * aEnv );

stlStatus smdmpfInitHeaderForTruncate( smlStatement  * aStatement,
                                       void          * aOrgRelHandle,
                                       void          * aNewSegHandle,
                                       void         ** aNewRelHandle,
                                       smlEnv        * aEnv );

stlStatus smdmpfDropAging( smxlTransId   aTransId,
                           void        * aRelationHandle,
                           stlBool       aStartingup,
                           smlEnv      * aEnv );

stlStatus smdmpfInitialize( smlStatement * aStatement,
                            void         * aRelationHandle,
                            smlEnv       * aEnv );

stlStatus smdmpfFinalize( smlStatement * aStatement,
                          void         * aRelationHandle,
                          smlEnv       * aEnv );

stlStatus smdmpfSort( void * RelationHandle, void * aTransactionHandle );

stlStatus smdmpfExtractKeyValue( smpHandle          * aPageHandle,
                                 stlBool              aFetchFirst,
                                 smpRowItemIterator * aRowIterator,
                                 smdValueBlockList  * aColumnList,
                                 stlInt32           * aKeyValueSize,
                                 stlInt16           * aRowSeq,
                                 stlBool            * aHasNullInBlock,
                                 stlBool            * aIsSuccess,
                                 smlEnv             * aEnv );

stlStatus smdmpfExtractValidKeyValue( void              * aRelationHandle,
                                      smxlTransId         aTransId,
                                      smlScn              aViewScn,
                                      smlTcn              aTcn,
                                      smlRid            * aRowRid,
                                      knlValueBlockList * aColumnList,
                                      smlEnv            * aEnv );

stlStatus smdmpfBuild( void * RelationHandle, void * aTransactionHandle );

stlStatus smdmpfMakePrevHeader( void   * aUndoRecHeader,
                                void   * aOrgRecHeader,
                                smlEnv * aEnv );

stlStatus smdmpfGetCommitScn( smlTbsId      aTbsId,
                              smpHandle   * aPageHandle,
                              stlChar     * aRow,
                              smlScn      * aCommitScn,
                              smxlTransId * aTransId,
                              smlEnv      * aEnv );

stlStatus smdmpfInsertContCols( smlStatement       * aStatement,
                                void               * aRelationHandle,
                                stlInt32             aValueIdx,
                                stlInt32             aFromOrder,
                                stlInt32             aToOrder,
                                knlValueBlockList ** aContCol,
                                stlBool              aIsMaster,
                                stlBool              aIsForUpdate,
                                smlPid             * aContColPid,
                                stlInt16           * aConColOrd,
                                smlEnv             * aEnv );

stlStatus smdmpfInsertContCol( smxlTransId         aTransId,
                               smlTcn              aStmtTcn,
                               void              * aRelationHandle,
                               stlInt32            aValueIdx,
                               knlValueBlockList * aContCol,
                               stlBool             aIsForUpdate,
                               smlPid            * aFirstPageId,
                               smlEnv            * aEnv );

stlStatus smdmpfInsert( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aInsertCols,
                        knlValueBlockList * aUniqueViolation,
                        smlRid            * aRowRid,
                        smlEnv            * aEnv );

stlStatus smdmpfBlockInsert( smlStatement      * aStatement,
                             void              * aRelationHandle,
                             knlValueBlockList * aInsertCols,
                             knlValueBlockList * aUniqueViolation,
                             smlRowBlock       * aRowBlock,
                             smlEnv            * aEnv );

stlStatus smdmpfBlockInsertInternal( smlStatement       * aStatement,
                                     void               * aRelationHandle,
                                     knlValueBlockList  * aInsertCols,
                                     stlInt32             aStartBlockIdx,
                                     stlInt32             aEndBlockIdx,
                                     smlRowBlock        * aRowBlock,
                                     smlEnv             * aEnv );
stlStatus smdmpfInsertRowPiece( void               * aRelationHandle,
                                stlInt32             aValueIdx,
                                smxlTransId          aTransId,
                                smlScn               aViewScn,
                                smlTcn               aTcn,
                                knlValueBlockList ** aInsertCols,
                                stlInt32             aFromOrd,
                                stlBool              aIsMasterRow,
                                stlBool              aIsForUpdate,
                                smlPid             * aContColPid,
                                stlInt16             aContColCount,
                                smlPid             * aNextChainedPageId,
                                smlRid             * aRowRid,
                                smlEnv             * aEnv );

stlStatus smdmpfGetInsertablePage( void              * aRelationHandle,
                                   smxmTrans         * aMiniTrans,
                                   smxlTransId         aTransId,
                                   smlScn              aViewScn,
                                   smlPid              aChainedPageId,
                                   stlUInt8            aLogicalPageType,
                                   smpHandle         * aPageHandle,
                                   stlInt16          * aSlotSeq,
                                   smlEnv            * aEnv );

stlStatus smdmpfAllocMemberPages( void           * aRelationHandle,
                                  smxmTrans      * aMiniTrans,
                                  smxlTransId      aTransId,
                                  smpHandle      * aAnchorPage,
                                  smlEnv         * aEnv );

stlStatus smdmpfGetInsertablePageList( void              * aRelationHandle,
                                       smxmTrans         * aMiniTrans,
                                       smxlTransId         aTransId,
                                       smlScn              aViewScn,
                                       stlUInt8            aLogicalPageType,
                                       smpHandle         * aPageHandle,
                                       stlInt16          * aSlotSeq,
                                       smlEnv            * aEnv );

stlStatus smdmpfCopyAndUpdateColumns( smpHandle          * aPageHandle,
                                      stlInt16             aSlotSeq,
                                      knlValueBlockList  * aColumns,
                                      stlInt32             aBlockIdx,
                                      stlInt16             aSrcRowPieceSize,
                                      stlInt16             aTotalColumnCount,
                                      stlInt32             aFirstColOrd,
                                      smlPid             * aContColPid,
                                      stlChar            * aRedoLogPtr,
                                      stlChar           ** aRedoLogEnd,
                                      smlEnv             * aEnv );


stlStatus smdmpfUpdateInPage( void              * aRelationHandle,
                              smxmTrans         * aMiniTrans,
                              smpHandle         * aPageHandle,
                              stlChar           * aUndoLog,
                              stlInt16            aUndoLogSize,
                              smlRid            * aSrcRowRid,
                              stlChar           * aRowSlot,
                              stlInt32            aValueIdx,
                              knlValueBlockList * aAfterCols,
                              stlInt16            aPieceColCnt,
                              stlInt16            aUpdateColCnt,
                              stlInt32            aFirstColOrd,
                              stlBool             aIsColSizeDiff,
                              smxlTransId         aMyTransId,
                              smlScn              aMyViewScn,
                              smlTcn              aMyTcn,
                              smlEnv            * aEnv );

stlStatus smdmpfUpdate( smlStatement       * aStatement,
                        void               * aRelationHandle,
                        smlRid             * aRowRid,
                        smlScn               aRowScn,
                        stlInt32             aValueIdx,
                        knlValueBlockList  * aAfterCols,
                        knlValueBlockList  * aBeforeCols,
                        knlValueBlockList  * aPrimaryKey,
                        stlBool            * aVersionConflict,
                        stlBool            * aSkipped,
                        smlEnv             * aEnv );

stlStatus smdmpfDelete( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aPrimaryKey,
                        stlBool           * aVersionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv );

stlStatus smdmpfDeleteRowPiece( smxmTrans         * aMiniTrans,
                                void              * aRelationHandle,
                                smpHandle         * aPageHandle,
                                smlRid            * aRowRid,
                                smxlTransId         aMyTransId,
                                smlScn              aMyViewScn,
                                smlTcn              aMyTcn,
                                smlScn              aCommitScn,
                                stlBool             aIsForUpdate,
                                smlEnv            * aEnv );

stlStatus smdmpfDeleteContCol( void         * aRelationHandle,
                               smxlTransId    aMyTransId,
                               smlScn         aMyViewScn,
                               smlTcn         aMyTcn,
                               smlPid         aContColPid,
                               stlBool        aIsForUpdate,
                               smlEnv       * aEnv );

stlStatus smdmpfDropColumns( smlStatement      * aStatement,
                             void              * aRelationHandle,
                             knlValueBlockList * aDropColumns,
                             smlEnv            * aEnv );

stlStatus smdmpfAlterTableAttr( smlStatement  * aStatement,
                                void          * aRelationHandle,
                                smlTableAttr  * aTableAttr,
                                smlIndexAttr  * aIndexAttr,
                                smlEnv        * aEnv );

stlStatus smdmpfMergeTableAttr( void          * aRelationHandle,
                                smlTableAttr  * aTableAttr,
                                smlEnv        * aEnv );

stlStatus smdmpfCheckHasNull( smlStatement * aStatement,
                              void         * aRelationHandle,
                              stlInt32       aColumnOrder,
                              stlBool      * aHasNull,
                              smlEnv       * aEnv );

stlStatus smdmpfCheckExistRow( smlStatement * aStatement,
                               void         * aRelationHandle,
                               stlBool      * aExistRow,
                               smlEnv       * aEnv );

stlStatus smdmpfFetchRecord( smlStatement        * aStatement,
                             smlFetchRecordInfo  * aFetchRecordInfo,
                             smlRid              * aRowRid,
                             stlInt32              aBlockIdx,
                             stlBool             * aIsMatched,
                             stlBool             * aDeleted,
                             smlEnv              * aEnv );

stlStatus smdmpfFetchRecord4Index( smiIterator         * aIterator,
                                   void                * aBaseRelHandle,
                                   smlFetchRecordInfo  * aFetchRecordInfo,
                                   smpHandle           * aTablePageHandle,
                                   stlBool             * aTablePageLatchReleased,
                                   smlRid              * aRowRid,
                                   stlInt32              aBlockIdx,
                                   smlScn                aAgableViewScn,
                                   stlBool             * aIsMatched,
                                   smlEnv              * aEnv );

stlStatus smdmpfFetch( smlStatement        * aStatement,
                       smlFetchRecordInfo  * aFetchRecordInfo,
                       smlRid              * aRowRid,
                       stlInt32              aBlockIdx,
                       stlBool             * aIsMatched,
                       stlBool             * aDeleted,
                       stlBool             * aUpdated,
                       smlEnv              * aEnv );

stlStatus smdmpfFetchWithRowid( smlStatement        * aStatement,
                                smlFetchRecordInfo  * aFetchRecordInfo,
                                stlInt64              aSegmentId,
                                stlInt64              aSegmentSeq,
                                smlRid              * aRowRid,
                                stlInt32              aBlockIdx,
                                stlBool             * aIsMatched,
                                stlBool             * aDeleted,
                                smlEnv              * aEnv );

stlStatus smdmpfLockRow( smlStatement * aStatement,
                         void         * aRelationHandle,
                         smlRid       * aRowRid,
                         smlScn         aRowScn,
                         stlUInt16      aLockMode,
                         stlInt64       aTimeInterval,
                         stlBool      * aVersionConflict,
                         smlEnv       * aEnv );

stlStatus smdmpfAnalyzeUpdateLogRec( stlChar            * aLogRec,
                                     knlRegionMark      * aMemMark,
                                     stlInt16           * aColCount,
                                     stlInt32           * aFirstColOrd,
                                     stlInt16           * aRowPieceSize,
                                     knlValueBlockList ** aColumns,
                                     smlPid             * aContColPid,
                                     smlEnv             * aEnv );

stlStatus smdmpfMakeUndoLog( void               * aRelationHandle,
                             smpHandle          * aPageHandle,
                             stlInt16             aSlotSeq,
                             stlInt16             aTotalColCount,
                             stlInt32             aFirstColOrd,
                             knlValueBlockList  * aUpdateCols,
                             knlValueBlockList  * aBeforeCols,
                             stlInt32             aBlockIdx,
                             stlChar            * aUndoLogPtr,
                             stlChar           ** aUndoLogEnd,
                             stlBool            * aExistContCol,
                             stlInt16           * aUpdateColCount );

stlStatus smdmpfCompareKeyValue( smlRid              * aRowRid,
                                 smpHandle           * aTablePageHandle,
                                 stlBool             * aTablePageLatchRelease,
                                 smxlTransId           aMyTransId,
                                 smlScn                aMyViewScn,
                                 smlScn                aMyCommitScn,
                                 smlTcn                aMyTcn,
                                 knlKeyCompareList   * aKeyCompList,
                                 stlBool             * aIsMatch,
                                 smlScn              * aCommitScn,
                                 smlEnv              * aEnv );

stlStatus smdmpfUpdatePageStatus( smxmTrans   * aMiniTrans,
                                  void        * aRelationHandle,
                                  void        * aSegmentHandle,
                                  smpHandle   * aPageHandle,
                                  smpFreeness   aFreeness,
                                  smlScn        aScn,
                                  smlEnv      * aEnv );

inline void * smdmpfGetSlotBody( void * aSlot );


stlStatus smdmpfInitIterator ( void   * aIterator,
                               smlEnv * aEnv );

stlStatus smdmpfResetIterator( void   * aIterator,
                               smlEnv * aEnv );

stlStatus smdmpfGetMaxRowIdSize( void      * aIterator,
                                 stlUInt16 * aRowIdSize );

stlStatus smdmpfGetRowRid( void   * aIterator, 
                           smlRid * aRowRid,
                           smlEnv * aEnv );

stlStatus smdmpfMoveToRowRid( void   * aIterator, 
                              smlRid * aRowRid,
                              smlEnv * aEnv );

stlStatus smdmpfMoveToPosNum( void   * aIterator,
                              stlInt64 aPosNum,
                              smlEnv * aEnv );

stlStatus smdmpfGetValidVersion( void                 * aRelationHandle,
                                 smlRid               * aRowRid,
                                 smdRowInfo           * aRowInfo,
                                 knlPhysicalFilter    * aPhysicalFilter,
                                 knlExprEvalInfo      * aLogicalFilterEvalInfo,
                                 smlScn               * aCommitScn,
                                 smlTcn               * aRowTcn,
                                 smxlTransId          * aWaitTransId,
                                 stlBool              * aHasValidVersion,
                                 stlBool              * aIsMatched,
                                 stlBool              * aLatchReleased,
                                 smlEnv               * aEnv );

stlStatus smdmpfReadNthColValue( smdRowInfo         * aRowInfo,
                                 smpHandle          * aPageHandle,
                                 stlBool              aNeedPrevVersion,
                                 stlChar            * aUndoLogPtr,
                                 stlInt16             aSlotSeq,
                                 stlInt16             aUndoType,
                                 stlInt32             aFirstColOrd,
                                 knlValueBlockList ** aCurCol,
                                 knlValueBlockList ** aLastCol,
                                 stlInt32             aBlockIdx,
                                 smlEnv             * aEnv );

stlStatus smdmpfGetValidVersionSimplePage( void                * aRelationHandle,
                                           smlRid              * aRowRid,
                                           smdRowInfo          * aRowInfo,
                                           knlPhysicalFilter   * aPhysicalFilter, 
                                           knlExprEvalInfo     * aLogicalFilterEvalInfo,
                                           smlScn              * aCommitScn,
                                           smxlTransId         * aWaitTransId,
                                           stlBool             * aHasValidVersion,
                                           stlBool             * aIsMatched,
                                           stlBool             * aLatchReleased,
                                           smlEnv              * aEnv );

stlStatus smdmpfGetValidVersionAndCompare( smdRowInfo           * aRowInfo,
                                           stlInt16               aSlotSeq,
                                           smlScn               * aCommitScn,
                                           smxlTransId          * aWaitTransId,
                                           stlBool              * aIsMatch,
                                           stlBool              * aLatchReleased,
                                           smlEnv               * aEnv );

stlStatus smdmpfGetValidVersions( smdmpfIterator    * aIterator,
                                  stlChar           * aPageFrame,
                                  smdRowInfo        * aRowInfo,
                                  smlFetchInfo      * aFetchInfo,
                                  stlInt16            aSlotSeq,
                                  stlBool           * aEndOfSlot,
                                  smlEnv            * aEnv );

stlStatus smdmpfGetValidVersionsSimplePage( smdmpfIterator   * aIterator,
                                            stlChar          * aPageFrame,
                                            smdRowInfo       * aRowInfo,
                                            smlFetchInfo     * aFetchInfo,
                                            stlInt16           aSlotSeq,
                                            stlBool          * aEndOfSlot,
                                            smlEnv           * aEnv );

stlStatus smdmpfFetchNext( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

stlStatus smdmpfFetchAggr( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

stlStatus smdmpfFetchPrev( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

void smdmpfValidateTablePage( smpHandle * aPageHandle, smlEnv * aEnv );

stlBool smdmpfIsSimplePage( smpHandle * aPageHandle );
stlInt16 smdmpfGetMaxFreeSize( void * aRelationHandle );

stlStatus smdmpfFindLastPiece( smlRid     aStartRid,
                               smlRid   * aPrevRid,
                               smlRid   * aCurRid,
                               stlInt32 * aLastColOrd,
                               smlEnv   * aEnv );

stlStatus smdmpfIsAgable( smpHandle * aPageHandle,
                          stlBool   * aIsAgable,
                          smlEnv    * aEnv );

inline stlChar * smdmpfGetNthColValue( smpHandle * aPageHandle,
                                       stlInt16    aRowSeq,
                                       stlInt16    aColOrd );

stlStatus smdmpfCompactPage( smxmTrans  * aMiniTrans,
                             smpHandle  * aPageHandle,
                             stlInt16   * aFreeSlotCount,
                             smlEnv     * aEnv );

void smdmpfIsUsableRowHdr( stlChar  * aRowHdr,
                           stlBool  * aIsUsable );

/* Redo Module */

stlStatus smdmpfRedoMemoryHeapInsert( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmpfRedoMemoryHeapUninsert( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv );

stlStatus smdmpfRedoMemoryHeapUpdate( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmpfRedoMemoryHeapUnupdate( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv );

stlStatus smdmpfRedoMemoryHeapMigrate( smlRid    * aDataRid,
                                       void      * aData,
                                       stlUInt16   aDataSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv );

stlStatus smdmpfRedoMemoryHeapUnmigrate( smlRid    * aDataRid,
                                         void      * aData,
                                         stlUInt16   aDataSize,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv );

stlStatus smdmpfRedoMemoryHeapDelete( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmpfRedoMemoryHeapUndelete( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv );

stlStatus smdmpfRedoMemoryHeapUpdateLink( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv );

stlStatus smdmpfRedoMemoryHeapInsertForUpdate( smlRid    * aDataRid,
                                               void      * aData,
                                               stlUInt16   aDataSize,
                                               smpHandle * aPageHandle,
                                               smlEnv    * aEnv );

stlStatus smdmpfRedoMemoryHeapDeleteForUpdate( smlRid    * aDataRid,
                                               void      * aData,
                                               stlUInt16   aDataSize,
                                               smpHandle * aPageHandle,
                                               smlEnv    * aEnv );

stlStatus smdmpfRedoMemoryHeapUpdateLogicalHdr( smlRid    * aDataRid,
                                                void      * aData,
                                                stlUInt16   aDataSize,
                                                smpHandle * aPageHandle,
                                                smlEnv    * aEnv );

stlStatus smdmpfRedoMemoryHeapAppend( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmpfRedoMemoryHeapUnappend( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv );

stlStatus smdmpfRedoMemoryHeapCompaction( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv );

/* Undo Module */
stlStatus smdmpfUndoMemoryHeapInsert( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmpfUndoMemoryHeapUpdate( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmpfUndoMemoryHeapUpdateLink( smxmTrans * aMiniTrans,
                                          smlRid      aTargetRid,
                                          void      * aLogBody,
                                          stlInt16    aLogSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv );

stlStatus smdmpfUndoMemoryHeapMigrate( smxmTrans * aMiniTrans,
                                       smlRid      aTargetRid,
                                       void      * aLogBody,
                                       stlInt16    aLogSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv );

stlStatus smdmpfUndoMemoryHeapDelete( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmpfUndoMemoryHeapAppend( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmpfUndoMemoryHeapBlockInsert( smxmTrans * aMiniTrans,
                                           smlRid      aTargetRid,
                                           void      * aLogBody,
                                           stlInt16    aLogSize,
                                           smpHandle * aPageHandle,
                                           smlEnv    * aEnv );

/* Mtx Undo Module */
stlStatus smdmpfMtxUndoMemoryHeapInsert( smlRid    * aDataRid,
                                         void      * aData,
                                         stlUInt16   aDataSize,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv );

stlStatus smdmpfReadContCols( smlTbsId             aTbsId,
                              knlValueBlockList  * aCurCol,
                              stlInt32             aBlockIdx,
                              stlChar            * aColPtr,
                              smlEnv             * aEnv );

/** @} */
    
#endif /* _SMDMPF_H_ */
