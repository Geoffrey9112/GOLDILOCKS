/*******************************************************************************
 * smdmph.h
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


#ifndef _SMDMPH_H_
#define _SMDMPH_H_ 1

/**
 * @file smdmph.h
 * @brief Storage Manager Layer Memory Heap Table Internal Routines
 */

/**
 * @defgroup smdmph Memory Heap Table
 * @ingroup smInternal
 * @{
 */

stlStatus smdmphInitTableHeader( smlStatement  * aStatement,
                                 smlRid        * aSegmentRid,
                                 void          * aSegmentHandle,
                                 smlTableAttr  * aAttr,
                                 void         ** aRelationHandle,
                                 smlEnv        * aEnv );

stlStatus smdmphCreate( smlStatement  * aStatement,
                        smlTbsId        aTbsId,
                        smlTableAttr  * aAttr,
                        stlBool         aLoggingUndo,
                        stlInt64      * aSegmentId,
                        void         ** aRelationHandle,
                        smlEnv        * aEnv );
    
stlStatus smdmphCreateUndo( smxlTransId   aTransId,
                            void        * aRelationHandle,
                            smlEnv      * aEnv );

stlStatus smdmphDrop( smlStatement * aStatement,
                      void         * aRelationHandle,
                      smlEnv       * aEnv );

stlStatus smdmphCopy( smlStatement      * aStatement,
                      void              * aSrcRelHandle,
                      void              * aDstRelHandle,
                      knlValueBlockList * aColumnList,
                      smlEnv            * aEnv );

stlStatus smdmphCreateForTruncate( smlStatement          * aStatement,
                                   void                  * aOrgRelHandle,
                                   smlDropStorageOption    aDropStorageOption,
                                   void                 ** aNewRelHandle,
                                   stlInt64              * aNewSegmentId,
                                   smlEnv                * aEnv );

stlStatus smdmphInitHeaderForTruncate( smlStatement  * aStatement,
                                       void          * aOrgRelHandle,
                                       void          * aNewSegHandle,
                                       void         ** aNewRelHandle,
                                       smlEnv        * aEnv );

stlStatus smdmphDropAging( smxlTransId   aTransId,
                           void        * aRelationHandle,
                           stlBool       aStartingup,
                           smlEnv      * aEnv );

stlStatus smdmphInitialize( smlStatement * aStatement,
                            void         * aRelationHandle,
                            smlEnv       * aEnv );

stlStatus smdmphFinalize( smlStatement * aStatement,
                          void         * aRelationHandle,
                          smlEnv       * aEnv );

stlStatus smdmphSort( void * RelationHandle, void * aTransactionHandle );

stlStatus smdmphExtractKeyValue( smpHandle          * aPageHandle,
                                 stlBool              aFetchFirst,
                                 smpRowItemIterator * aRowIterator,
                                 smdValueBlockList  * aColumnList,
                                 stlInt32           * aKeyValueSize,
                                 stlInt16           * aRowSeq,
                                 stlBool            * aHasNullInBlock,
                                 stlBool            * aIsSuccess,
                                 smlEnv             * aEnv );

stlStatus smdmphExtractValidKeyValue( void              * aRelationHandle,
                                      smxlTransId         aTransId,
                                      smlScn              aViewScn,
                                      smlTcn              aTcn,
                                      smlRid            * aRowRid,
                                      knlValueBlockList * aColumnList,
                                      smlEnv            * aEnv );

stlStatus smdmphBuild( void * RelationHandle, void * aTransactionHandle );

inline stlInt16 smdmphGetRowSize( stlChar * aRow );


stlStatus smdmphMakePrevHeader( void   * aUndoRecHeader,
                                void   * aOrgRecHeader,
                                smlEnv * aEnv );

stlStatus smdmphGetCommitScn( smlTbsId      aTbsId,
                              smpHandle   * aPageHandle,
                              stlChar     * aRow,
                              smlScn      * aCommitScn,
                              smxlTransId * aTransId,
                              smlEnv      * aEnv );

void smdmphStabilizeRow( stlChar  * aRow,
                         stlUInt8   aRtsSeq,
                         stlUInt8   aLastRtsVerNo,
                         smlScn     aCommitScn,
                         stlInt16 * aReferenceCount );

stlStatus smdmphInsertContCol( smlStatement      * aStatement,
                               void              * aRelationHandle,
                               stlInt32            aValueIdx,
                               knlValueBlockList * aContCol,
                               stlBool             aIsMaster,
                               smlRid            * aPrevRid,
                               stlBool             aIsForUpdate,
                               smlRid            * aFirstRowRid,
                               smlRid            * aLastRowRid,
                               smlEnv            * aEnv );

stlStatus smdmphInsert( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aInsertCols,
                        knlValueBlockList * aUniqueViolation,
                        smlRid            * aRowRid,
                        smlEnv            * aEnv );

stlStatus smdmphBlockInsert( smlStatement      * aStatement,
                             void              * aRelationHandle,
                             knlValueBlockList * aInsertCols,
                             knlValueBlockList * aUniqueViolation,
                             smlRowBlock       * aRowBlock,
                             smlEnv            * aEnv );

stlStatus smdmphBlockInsertInternal( smlStatement       * aStatement,
                                     void               * aRelationHandle,
                                     knlValueBlockList  * aInsertCols,
                                     stlInt32             aStartBlockIdx,
                                     stlInt32             aEndBlockIdx,
                                     smlRowBlock        * aRowBlock,
                                     knlValueBlockList ** aColPtrArray,
                                     stlInt16             aColCount,
                                     smlEnv             * aEnv );

stlStatus smdmphGetInsertablePage( void              * aRelationHandle,
                                   smxmTrans         * aMiniTrans,
                                   smxlTransId         aTransId,
                                   smlScn              aViewScn,
                                   stlInt16            aRowSize,
                                   smpHandle         * aPageHandle,
                                   stlUInt8          * aRtsSeq,
                                   stlInt16          * aSlotSeq,
                                   smlEnv            * aEnv );

stlStatus smdmphCompactPage( smxmTrans * aMiniTrans,
                             smpHandle * aPageHandle,
                             smlEnv    * aEnv );

stlStatus smdmphExtendRowSlot( smxmTrans  * aMiniTrans,
                               smpHandle * aPageHandle,
                               stlInt16    aSlotSeq,
                               stlInt16    aExtendSize,
                               smlEnv    * aEnv );

stlStatus smdmphCompactPageInternal( smxmTrans * aMiniTrans,
                                     smpHandle * aPageHandle,
                                     stlInt16    aSlotSeq,
                                     stlInt16    aExtendSize,
                                     smlEnv    * aEnv );

stlStatus smdmphCopyAndUpdateColumns( stlChar            * aSrcRow,
                                      stlChar            * aDstRow,
                                      knlValueBlockList  * aColumns,
                                      stlInt32             aBlockIdx,
                                      stlInt16             aSrcRowPieceSize,
                                      stlInt16             aTotalColumnCount,
                                      stlInt32             aFirstColOrd,
                                      stlChar            * aRedoLogPtr,
                                      stlChar           ** aRedoLogEnd,
                                      stlBool              aIsColSizeDiff,
                                      smlEnv             * aEnv );


stlStatus smdmphUpdateNextPieceRid( smxmTrans   * aMiniTrans,
                                    smxlTransId   aTransId,
                                    smlScn        aViewScn,
                                    smlTcn        aTcn,
                                    smlRid      * aTargetRid,
                                    smlRid      * aNextRid,
                                    smlEnv      * aEnv );

stlStatus smdmphRemakeUpdatedRowPiece( smlStatement      * aStatement,
                                       void              * aRelationHandle,
                                       stlChar           * aCopiedRowSlot,
                                       knlValueBlockList * aAfterCols,
                                       stlInt32            aValueIdx,
                                       stlInt16            aPieceColCount,
                                       stlInt32            aFirstColOrd,
                                       smlRid            * aPrevPieceRid,
                                       smlRid            * aNextPieceRid,
                                       smlRid            * aFirstRowRid,
                                       smlRid            * aLastRowRid,
                                       smlEnv            * aEnv );

stlStatus smdmphMakeMigratedRow( smxmTrans   * aMiniTrans,
                                 smpHandle   * aPageHandle,
                                 smxlTransId   aTransId,
                                 smlScn        aViewScn,
                                 smlScn        aCommitScn,
                                 smlTcn        aTcn,
                                 stlChar     * aRowSlot,
                                 smlRid      * aRowRid,
                                 stlInt16      aRowSlotSize,
                                 smlEnv      * aEnv );

stlStatus smdmphUpdateInPage( smxmTrans         * aMiniTrans,
                              smpHandle         * aPageHandle,
                              stlChar           * aUndoLog,
                              stlInt16            aUndoLogSize,
                              smlRid            * aSrcRowRid,
                              stlChar           * aSrcRowSlot,
                              stlInt16            aSrcRowSize,
                              stlUInt8            aSrcRtsSeq,
                              stlChar           * aDstRowSlot,
                              stlInt16            aDstRowSize,
                              stlUInt8            aDstRtsSeq,
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

stlStatus smdmphUpdate( smlStatement       * aStatement,
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

stlStatus smdmphAppendColumns( smlStatement      * aStatement,
                               void              * aRelationHandle,
                               stlInt16            aFirstColOrd,
                               knlValueBlockList * aColumns,
                               smlRid              aPrevRid,
                               smlRid              aRowRid,
                               stlInt32            aValueIdx,
                               smlEnv            * aEnv );

stlStatus smdmphAllocRowPiece( smxmTrans   * aMiniTrans,
                               smpHandle   * aPageHandle,
                               smxlTransId   aTransId,
                               smlScn        aViewScn,
                               smlRid        aSrcPieceRid,
                               stlInt16      aSrcPieceSize,
                               stlInt16      aPieceSizeDiff,
                               stlChar    ** aDstPiece,
                               stlUInt8    * aDstRtsSeq,
                               stlBool     * aInSlotUpdate,
                               smlEnv      * aEnv );

stlStatus smdmphAppendRowPiece( smxmTrans   * aMiniTrans,
                                smpHandle   * aPageHandle,
                                smxlTransId   aTransId,
                                smlScn        aViewScn,
                                smlTcn        aTcn,
                                stlBool       aInSlotUpdate,
                                stlInt16      aSrcPieceSize,
                                stlChar     * aAppendPieceBody,
                                stlInt16      aAppendPieceBodySize,
                                stlUInt8      aDstRtsSeq,
                                stlInt16      aReallocPieceSize,
                                stlInt32      aFirstColOrd,
                                stlInt16      aAppendPieceColCount,
                                smlRid        aNxtPieceRid,
                                smlRid        aPieceRid,
                                smlEnv      * aEnv );

stlStatus smdmphDelete( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aPrimaryKey,
                        stlBool           * aVersionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv );

stlStatus smdmphDeleteRowPiece( smxmTrans         * aMiniTrans,
                                void              * aRelationHandle,
                                smpHandle         * aPageHandle,
                                smlRid            * aRowRid,
                                smxlTransId         aMyTransId,
                                smlScn              aMyViewScn,
                                smlTcn              aMyTcn,
                                smlScn              aCommitScn,
                                stlBool             aIsForUpdate,
                                smlEnv            * aEnv );

stlStatus smdmphDeleteContColRows( void            * aRelationHandle,
                                   smlRid          * aStartRid,
                                   smlRid          * aNextColRid,
                                   smxlTransId       aMyTransId,
                                   smlScn            aMyViewScn,
                                   smlTcn            aMyTcn,
                                   stlBool           aIsForUpdate,
                                   dtlDataValue    * aBeforeValue,
                                   smlEnv          * aEnv );

stlStatus smdmphAddColumns( smlStatement      * aStatement,
                            void              * aRelationHandle,
                            knlValueBlockList * aAddColumns,
                            smlRowBlock       * aRowBlock,
                            smlEnv            * aEnv );

stlStatus smdmphDropColumns( smlStatement      * aStatement,
                             void              * aRelationHandle,
                             knlValueBlockList * aDropColumns,
                             smlEnv            * aEnv );

stlStatus smdmphAlterTableAttr( smlStatement  * aStatement,
                                void          * aRelationHandle,
                                smlTableAttr  * aTableAttr,
                                smlIndexAttr  * aIndexAttr,
                                smlEnv        * aEnv );

stlStatus smdmphMergeTableAttr( void          * aRelationHandle,
                                smlTableAttr  * aTableAttr,
                                smlEnv        * aEnv );

stlStatus smdmphCheckHasNull( smlStatement * aStatement,
                              void         * aRelationHandle,
                              stlInt32       aColumnOrder,
                              stlBool      * aHasNull,
                              smlEnv       * aEnv );

stlStatus smdmphCheckExistRow( smlStatement * aStatement,
                               void         * aRelationHandle,
                               stlBool      * aExistRow,
                               smlEnv       * aEnv );

stlStatus smdmphFetchRecord( smlStatement        * aStatement,
                             smlFetchRecordInfo  * aFetchRecordInfo,
                             smlRid              * aRowRid,
                             stlInt32              aBlockIdx,
                             stlBool             * aIsMatched,
                             stlBool             * aDeleted,
                             smlEnv              * aEnv );

stlStatus smdmphFetchRecord4Index( smiIterator         * aIterator,
                                   void                * aBaseRelHandle,
                                   smlFetchRecordInfo  * aFetchRecordInfo,
                                   smpHandle           * aTablePageHandle,
                                   stlBool             * aTablePageLatchReleased,
                                   smlRid              * aRowRid,
                                   stlInt32              aBlockIdx,
                                   smlScn                aAgableViewScn,
                                   stlBool             * aIsMatched,
                                   smlEnv              * aEnv );

stlStatus smdmphFetch( smlStatement        * aStatement,
                       smlFetchRecordInfo  * aFetchRecordInfo,
                       smlRid              * aRowRid,
                       stlInt32              aBlockIdx,
                       stlBool             * aIsMatched,
                       stlBool             * aDeleted,
                       stlBool             * aUpdated,
                       smlEnv              * aEnv );

stlStatus smdmphFetchWithRowid( smlStatement        * aStatement,
                                smlFetchRecordInfo  * aFetchRecordInfo,
                                stlInt64              aSegmentId,
                                stlInt64              aSegmentSeq,
                                smlRid              * aRowRid,
                                stlInt32              aBlockIdx,
                                stlBool             * aIsMatched,
                                stlBool             * aDeleted,
                                smlEnv              * aEnv );

stlStatus smdmphLockRow( smlStatement * aStatement,
                         void         * aRelationHandle,
                         smlRid       * aRowRid,
                         smlScn         aRowScn,
                         stlUInt16      aLockMode,
                         stlInt64       aTimeInterval,
                         stlBool      * aVersionConflict,
                         smlEnv       * aEnv );

stlStatus smdmphAnalyzeUpdateLogRec( stlChar            * aLogRec,
                                     knlRegionMark      * aMemMark,
                                     stlUInt8           * aRtsSeq,
                                     stlInt16           * aColCount,
                                     stlInt32           * aFirstColOrd,
                                     knlValueBlockList ** aColumns,
                                     smlEnv             * aEnv );

stlStatus smdmphGetUpdatePieceSize( stlChar            * aFirstColPtr,
                                    stlInt16             aTotalColCount,
                                    stlInt32             aFirstColOrd,
                                    knlValueBlockList  * aUpdateCols,
                                    knlValueBlockList  * aBeforeCols,
                                    stlInt32             aBlockIdx,
                                    stlChar            * aUndoLogPtr,
                                    stlChar           ** aUndoLogEnd,
                                    stlInt16           * aRowColListSize,
                                    stlBool            * aIsColSizeDiff,
                                    stlInt64           * aRowSizeDiff,
                                    stlInt16           * aUpdateColCount );

stlStatus smdmphCompareKeyValue( smlRid              * aRowRid,
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

stlStatus smdmphUpdatePageStatus( smxmTrans   * aMiniTrans,
                                  void        * aRelationHandle,
                                  void        * aSegmentHandle,
                                  smpHandle   * aPageHandle,
                                  smpFreeness   aFreeness,
                                  smlScn        aScn,
                                  smlEnv      * aEnv );

inline void * smdmphGetSlotBody( void * aSlot );


stlStatus smdmphInitIterator ( void   * aIterator,
                               smlEnv * aEnv );

stlStatus smdmphResetIterator( void   * aIterator,
                               smlEnv * aEnv );

stlStatus smdmphGetMaxRowIdSize( void      * aIterator,
                                 stlUInt16 * aRowIdSize );

stlStatus smdmphGetRowRid( void   * aIterator, 
                           smlRid * aRowRid,
                           smlEnv * aEnv );

stlStatus smdmphMoveToRowRid( void   * aIterator, 
                              smlRid * aRowRid,
                              smlEnv * aEnv );

stlStatus smdmphMoveToPosNum( void   * aIterator,
                              stlInt64 aPosNum,
                              smlEnv * aEnv );

stlStatus smdmphGetValidVersion( smlRid               * aRowRid,
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

stlStatus smdmphGetValidVersionSimplePage( smlRid              * aRowRid,
                                           smdRowInfo          * aRowInfo,
                                           knlPhysicalFilter   * aPhysicalFilter, 
                                           knlExprEvalInfo     * aLogicalFilterEvalInfo,
                                           smlScn              * aCommitScn,
                                           smxlTransId         * aWaitTransId,
                                           stlBool             * aHasValidVersion,
                                           stlBool             * aIsMatched,
                                           stlBool             * aLatchReleased,
                                           smlEnv              * aEnv );

stlStatus smdmphGetValidVersionAndCompare( smdRowInfo           * aRowInfo,
                                           smlScn               * aCommitScn,
                                           smxlTransId          * aWaitTransId,
                                           stlBool              * aIsMatch,
                                           stlBool              * aLatchReleased,
                                           smlEnv               * aEnv );

stlStatus smdmphGetValidVersions( smdmphIterator    * aIterator,
                                  stlChar           * aPageFrame,
                                  smdRowInfo        * aRowInfo,
                                  smlFetchInfo      * aFetchInfo,
                                  stlInt16            aSlotSeq,
                                  stlBool           * aEndOfSlot,
                                  smlEnv            * aEnv );

stlStatus smdmphGetValidVersionsSimplePage( smdmphIterator   * aIterator,
                                            stlChar          * aPageFrame,
                                            smdRowInfo       * aRowInfo,
                                            smlFetchInfo     * aFetchInfo,
                                            stlInt16           aSlotSeq,
                                            stlBool          * aEndOfSlot,
                                            smlEnv           * aEnv );

stlStatus smdmphFetchNext( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

stlStatus smdmphFetchAggr( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

stlStatus smdmphFetchPrev( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

void smdmphValidatePadSpace( smpHandle * aPageHandle );

void smdmphValidateTablePage( smpHandle * aPageHandle, smlEnv * aEnv );

stlBool smdmphIsSimplePage( smpHandle * aPageHandle );
stlInt16 smdmphGetMaxFreeSize( void * aRelationHandle );

stlStatus smdmphFindLastPiece( smlRid     aStartRid,
                               smlRid   * aPrevRid,
                               smlRid   * aCurRid,
                               stlInt32 * aLastColOrd,
                               smlEnv   * aEnv );

stlStatus smdmphIsAgable( smpHandle * aPageHandle,
                          stlBool   * aIsAgable,
                          smlEnv    * aEnv );

/* Redo Module */

stlStatus smdmphRedoMemoryHeapInsert( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmphRedoMemoryHeapUninsert( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv );

stlStatus smdmphRedoMemoryHeapUpdate( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmphRedoMemoryHeapUnupdate( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv );

stlStatus smdmphRedoMemoryHeapMigrate( smlRid    * aDataRid,
                                       void      * aData,
                                       stlUInt16   aDataSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv );

stlStatus smdmphRedoMemoryHeapUnmigrate( smlRid    * aDataRid,
                                         void      * aData,
                                         stlUInt16   aDataSize,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv );

stlStatus smdmphRedoMemoryHeapDelete( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmphRedoMemoryHeapUndelete( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv );

stlStatus smdmphRedoMemoryHeapUpdateLink( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv );

stlStatus smdmphRedoMemoryHeapInsertForUpdate( smlRid    * aDataRid,
                                               void      * aData,
                                               stlUInt16   aDataSize,
                                               smpHandle * aPageHandle,
                                               smlEnv    * aEnv );

stlStatus smdmphRedoMemoryHeapDeleteForUpdate( smlRid    * aDataRid,
                                               void      * aData,
                                               stlUInt16   aDataSize,
                                               smpHandle * aPageHandle,
                                               smlEnv    * aEnv );

stlStatus smdmphRedoMemoryHeapUpdateLogicalHdr( smlRid    * aDataRid,
                                                void      * aData,
                                                stlUInt16   aDataSize,
                                                smpHandle * aPageHandle,
                                                smlEnv    * aEnv );

stlStatus smdmphRedoMemoryHeapAppend( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmphRedoMemoryHeapUnappend( smlRid    * aDataRid,
                                        void      * aData,
                                        stlUInt16   aDataSize,
                                        smpHandle * aPageHandle,
                                        smlEnv    * aEnv );

stlStatus smdmphRedoMemoryPendingInsert( smlRid    * aDataRid,
                                         void      * aData,
                                         stlUInt16   aDataSize,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv );

stlStatus smdmphRedoMemoryHeapCompaction( smlRid    * aDataRid,
                                          void      * aData,
                                          stlUInt16   aDataSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv );

/* Undo Module */
stlStatus smdmphUndoMemoryHeapInsert( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmphUndoMemoryHeapUpdate( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmphUndoMemoryHeapUpdateLink( smxmTrans * aMiniTrans,
                                          smlRid      aTargetRid,
                                          void      * aLogBody,
                                          stlInt16    aLogSize,
                                          smpHandle * aPageHandle,
                                          smlEnv    * aEnv );

stlStatus smdmphUndoMemoryHeapMigrate( smxmTrans * aMiniTrans,
                                       smlRid      aTargetRid,
                                       void      * aLogBody,
                                       stlInt16    aLogSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv );

stlStatus smdmphUndoMemoryHeapDelete( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmphUndoMemoryHeapAppend( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

stlStatus smdmphUndoMemoryHeapBlockInsert( smxmTrans * aMiniTrans,
                                           smlRid      aTargetRid,
                                           void      * aLogBody,
                                           stlInt16    aLogSize,
                                           smpHandle * aPageHandle,
                                           smlEnv    * aEnv );

/* Mtx Undo Module */

stlStatus smdmphMtxUndoMemoryHeapInsert( smlRid    * aDataRid,
                                         void      * aData,
                                         stlUInt16   aDataSize,
                                         smpHandle * aPageHandle,
                                         smlEnv    * aEnv );

/** @} */
    
#endif /* _SMXL_H_ */
