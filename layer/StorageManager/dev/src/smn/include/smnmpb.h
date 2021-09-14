/*******************************************************************************
 * smnmpb.h
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


#ifndef _SMNMPB_H_
#define _SMNMPB_H_ 1

#include <smnDef.h>
#include <smnmpbDef.h>

/**
 * @file smnmpb.h
 * @brief Storage Manager Layer Memory Index Internal Routines
 */

/**
 * @defgroup smnmpb Memory Index
 * @ingroup smInternal
 * @{
 */

stlStatus smnmpbCreate( smlStatement       * aStatement,
                        smlTbsId             aTbsId,
                        smlIndexAttr       * aAttr,
                        void               * aBaseTableHandle,
                        stlUInt16            aKeyColCount,
                        knlValueBlockList  * aIndexColList,
                        stlUInt8           * aKeyColFlags,
                        stlInt64           * aSegmentId,
                        void              ** aRelationHandle,
                        smlEnv             * aEnv );

stlStatus smnmpbCreateUndo( smxlTransId   aTransId,
                            void        * aRelationHandle,
                            smlEnv      * aEnv );

stlStatus smnmpbDrop( smlStatement * aStatement,
                      void         * aRelationHandle,
                      smlEnv       * aEnv );

stlStatus smnmpbAlterDataType( smlStatement  * aStatement,
                               void          * aRelationHandle,
                               stlInt32        aColumnOrder,
                               dtlDataType     aDataType,
                               stlInt64        aColumnSize,
                               smlEnv        * aEnv );

stlStatus smnmpbAlterIndexAttr( smlStatement * aStatement,
                                void         * aRelationHandle,
                                smlIndexAttr * aIndexAttr,
                                smlEnv       * aEnv );

stlStatus smnmpbMergeIndexAttr( void          * aRelationHandle,
                                smlIndexAttr  * aIndexAttr,
                                smlEnv        * aEnv );

stlStatus smnmpbCheckHasNull( smlStatement * aStatement,
                              void         * aRelationHandle,
                              stlInt32       aColumnOrder,
                              stlBool      * aHasNull,
                              smlEnv       * aEnv );

stlStatus smnmpbCreateForTruncate( smlStatement          * aStatement,
                                   void                  * aOrgRelHandle,
                                   void                  * aNewBaseRelHandle,
                                   smlDropStorageOption    aDropStorageOption,
                                   void                 ** aNewRelHandle,
                                   stlInt64              * aNewSegmentId,
                                   smlEnv                * aEnv );

stlStatus smnmpbInitHeaderForTruncate( smlStatement  * aStatement,
                                       void          * aOrgRelHandle,
                                       void          * aNewBaseRelHandle,
                                       void          * aNewSegHandle,
                                       void         ** aNewRelHandle,
                                       smlEnv        * aEnv );

stlStatus smnmpbCreatePending( stlUInt32   aActionFlag,
                               smgPendOp * aPendOp,
                               smlEnv    * aEnv );

stlStatus smnmpbDropPending( stlUInt32   aActionFlag,
                             smgPendOp * aPendOp,
                             smlEnv    * aEnv );

stlStatus smnmpbUsablePending( stlUInt32   aActionFlag,
                               smgPendOp * aPendOp,
                               stlInt16    aData,
                               smlEnv    * aEnv );

stlStatus smnmpbUnusablePending( stlUInt32   aActionFlag,
                                 smgPendOp * aPendOp,
                                 stlInt16    aData,
                                 smlEnv    * aEnv );

stlStatus smnmpbDropAging( smxlTransId   aTransId,
                           void        * aRelHandle,
                           stlBool       aOnStartup,
                           smlEnv      * aEnv );

stlStatus smnmpbInitialize( smlStatement * aStatement,
                            void         * aRelationHandle,
                            smlEnv       * aEnv );

stlStatus smnmpbFinalize( smlStatement * aStatement,
                          void         * aRelationHandle,
                          smlEnv       * aEnv );

stlStatus smnmpbBuild( smlStatement      * aStatement,
                       void              * aIndexHandle,
                       void              * aBaseTableHandle,
                       stlUInt16           aKeyColCount,
                       knlValueBlockList * aIndexColList,
                       stlUInt8          * aKeyColFlags,
                       stlUInt16           aParallelFactor,
                       smlEnv            * aEnv );
stlStatus smnmpbBuildUnusable( smlStatement      * aStatement,
                               void              * aIndexHandle,
                               smlEnv            * aEnv );
stlStatus smnmpbBottomUpBuild( smlStatement      * aStatement,
                               void              * aIndexHandle,
                               void              * aBaseTableHandle,
                               stlUInt16           aKeyColCount,
                               knlValueBlockList * aIndexColList,
                               stlUInt8          * aKeyColFlags,
                               stlUInt16           aParallelFactor,
                               smlEnv            * aEnv );
stlStatus smnmpbTopDownBuild( smlStatement      * aStatement,
                              void              * aIndexHandle,
                              void              * aBaseTableHandle,
                              stlUInt16           aKeyColCount,
                              knlValueBlockList * aIndexColList,
                              stlUInt8          * aKeyColFlags,
                              smlEnv            * aEnv );
void * STL_THREAD_FUNC smnmpbSortAndMergeThread( stlThread * aThread,
                                                 void      * aArg );
void * STL_THREAD_FUNC smnmpbBuildTreeThread( stlThread * aThread,
                                              void      * aArg );
stlStatus smnmpbSortAndMerge( smnmpbParallelArgs * aArgs,
                              smlEnv             * aEnv );
stlStatus smnmpbSortKeys( smnmpbParallelArgs  * aArgs,
                          knlQueueInfo        * aRunQueueInfo,
                          knlQueueInfo        * aFreeQueueInfo,
                          smdValueBlockList   * aIndexColList,
                          smdValueBlockList   * aTableColList,
                          stlInt32            * aKeyValueSize,
                          stlInt16            * aRowSeq,
                          smnmpbCompareInfo   * aCompareInfo,
                          smlEnv              * aEnv );
stlStatus smnmpbMergeKeys( smnmpbParallelArgs  * aArgs,
                           knlQueueInfo        * aRunQueueInfo,
                           smlPid              * aMergedRunList,
                           smnmpbCompareInfo   * aCompareInfo,
                           knlQueueInfo        * aFreeQueueInfo,
                           smlEnv              * aEnv );
stlStatus smnmpbBuildTree( smnmpbParallelArgs  * aArgs,
                           smlEnv              * aEnv );
stlStatus smnmpbGroupingRuns( smnmpbParallelArgs * aArgs,
                              smlEnv             * aEnv );
stlStatus smnmpbMergeTree( smnmpbParallelArgs * aArgs,
                           smlEnv             * aEnv );
stlStatus smnmpbMaximizeFanoutRootPage( smxmTrans           * aMiniTrans,
                                        smnmpbParallelArgs  * aArgs,
                                        smpHandle           * aRootPageHandle,
                                        smlEnv              * aEnv );
stlStatus smnmpbSplitChildNode4Build( smnmpbParallelArgs  * aArgs,
                                      stlUInt16             aUnusedSpace,
                                      smlPid                aPageId,
                                      smlPid                aPrevPageId,
                                      stlChar            ** aPropKeyBody,
                                      stlInt16            * aPropKeyBodySize,
                                      smlPid              * aSplittedPid,
                                      smlPid              * aRowPid,
                                      stlInt16            * aRowOffset,
                                      stlBool             * aIsSuccess,
                                      smlEnv              * aEnv );
stlStatus smnmpbFinalizeBuildTree( smnmpbParallelArgs  * aArgs,
                                   smlEnv              * aEnv );
stlStatus smnmpbGetMaxKey( smnmpbParallelArgs * aArgs,
                           smlPid               aExtentPid,
                           stlChar            * aMaxKey,
                           smlEnv             * aEnv );

stlInt32 smnmpbCompare4Build( void * aCompareInfo,
                              void * aValueA,
                              void * aValueB );
stlStatus smnmpbWriteKeyToSortBlock( smnmpbSortBlockHdr * aSortBlockHdr,
                                     stlChar            * aSortBlock,
                                     smlPid               aRowPid,
                                     stlInt16           * aRowSeq,
                                     smdValueBlockList  * aColumnList,
                                     smlTcn               aTcn,
                                     stlInt32           * aKeyValueSize,
                                     stlBool            * aIsSuccess,
                                     smlEnv             * aEnv );
stlStatus smnmpbWriteKeyToSortedRun( smxlTransId           aTransId,
                                     void                * aIndexHandle,
                                     knlHeapQueueContext * aHeapQueueContext,
                                     stlInt32              aSortBlockCount,
                                     knlQueueInfo        * aRunQueueInfo,
                                     knlQueueInfo        * aFreeQueueInfo,
                                     stlInt32              aBuildExtentPageCount,
                                     smlEnv              * aEnv );
stlStatus smnmpbCompactSortBlock( smnmpbSortBlockHdr   * aSortBlockHdr,
                                  stlChar            *** aSortBlock,
                                  stlInt32               aSortBlockCount,
                                  smlEnv               * aEnv );
stlStatus smnmpbSortFetchNext( void      * aIterator,
                               stlInt64    aIteratorIdx,
                               void     ** aItem,
                               knlEnv    * aEnv );
stlStatus smnmpBuildFetchNext( void      * aIterator,
                               stlInt64    aIteratorIdx,
                               void     ** aItem,
                               knlEnv    * aEnv );
stlStatus smnmpGroupingFetchNext( void      * aIterator,
                                  stlInt64    aIteratorIdx,
                                  void     ** aItem,
                                  knlEnv    * aEnv );
stlStatus smnmpbWriteKeyToMergedRun( smnmpbParallelArgs     * aArgs,
                                     knlQueueInfo           * aRunQueueInfo,
                                     smnmpbMergeRunIterator * aMergeIterator,
                                     void                   * aKey,
                                     knlQueueInfo           * aFreeQueueInfo,
                                     smlEnv                 * aEnv );
stlStatus smnmpbFreeExtents( smxlTransId    aTransId,
                             void         * aIndexHandle,
                             stlInt32       aPageCount,
                             knlQueueInfo * aFreeQueueInfo,
                             smlEnv       * aEnv );
stlStatus smnmpbWriteKeyToTree( smnmpbParallelArgs * aArgs,
                                smnmpbTreeIterator * aIterator,
                                void               * aRunKey,
                                smlEnv             * aEnv );
stlStatus smnmpbAllocPage4Build( smnmpbTreeIterator * aIterator,
                                 smlPid             * aPageId,
                                 smlEnv             * aEnv );
stlStatus smnmpbPropageKey4Build( smnmpbTreeIterator * aIterator,
                                  stlInt16             aHeight,
                                  smlPid               aChildPid,
                                  stlChar            * aPropKeyBody,
                                  stlInt16             aPropKeyBodySize,
                                  smlPid               aRowPageId,
                                  stlInt16             aRowOffset,
                                  smlEnv             * aEnv );
stlStatus smnmpbAllocExtent4Build( smxlTransId    aTransId,
                                   stlInt32       aPageCount,
                                   void         * aSegmentHandle,
                                   knlQueueInfo * aFreeQueueInfo,
                                   smlPid       * aExtentPid,
                                   smlEnv       * aEnv );
knlCompareFunc smnmpbGetCompareFunc( stlInt32 sCompareType,
                                     stlBool  s1ByteColLenSize,
                                     stlBool  sPrimaryIndex );
stlInt32 smnmpbCompare4MergeTree( void * aCompareInfo,
                                  void * aValueA,
                                  void * aValueB );
stlStatus smnmpbSplitRun( smnmpbParallelArgs  * aArgs,
                          smnmpbBuildIterator * aIterator,
                          stlChar             * aMaxKey,
                          smnmpbCompareInfo   * aCompareInfo,
                          knlQueueInfo        * aFreeQueueInfo,
                          smlPid              * aExtentPid,
                          smlEnv              * aEnv );
stlStatus smnmpbMergeTreeLevel( smnmpbTreeIterator * aIterator,
                                smnmpbParallelArgs * aArgs,
                                stlInt16             aHeight,
                                smnmpbCompareInfo  * aCompareInfo,
                                smlEnv             * aEnv );

stlStatus smnmpbBuildForTruncate( smlStatement     * aStatement,
                                  void             * aOrgIndexHandle,
                                  void             * aNewIndexHandle,
                                  smlEnv           * aEnv );

inline stlInt16 smnmpbGetIntlKeySize( stlChar * aKey, stlUInt16 aColCount );

inline stlInt16 smnmpbGetLeafKeySize( stlChar * aKey, stlUInt16 aColCount );

void smnmpbGetLeafKeySizeAndBody( stlChar     * aKey,
                                  stlUInt16     aColCount,
                                  stlUInt16   * aKeySize,
                                  stlUInt16   * aKeyBodySize,
                                  stlChar    ** aKeyBody,
                                  smlRid      * aKeyRowRid );

void smnmpbGetIntlKeySizeAndBody( stlChar     * aKey,
                                  stlUInt16     aColCount,
                                  stlUInt16   * aKeySize,
                                  stlUInt16   * aKeyBodySize,
                                  stlChar    ** aKeyBody,
                                  smlRid      * aKeyRowRid );

inline stlStatus smnmpbGetRtsCommitScn( void        * aPageFrame,
                                        stlUInt8      aRtsSeq,
                                        smxlTransId * aTransId,
                                        smlScn      * aCommitScn,
                                        smlEnv      * aEnv );

inline stlStatus smnmpbIsInsertCommitted( void        * aPageFrame,
                                          stlChar     * aKeySlot,
                                          stlBool     * aIsCommitted,
                                          smxlTransId * aWaitTransId,
                                          smlEnv      * aEnv );

inline stlStatus smnmpbGetInsertCommitScn( smlTbsId      aTbsId,
                                           smpHandle   * aPageHandle,
                                           stlChar     * aKeySlot,
                                           smlScn        aMyStmtViewScn,
                                           smlScn      * aCommitScn,
                                           smlTcn      * aTcn,
                                           smxlTransId * aTransId,
                                           smlEnv      * aEnv );

inline stlStatus smnmpbIsDeleteCommitted( void        * aPageFrame,
                                          stlChar     * aKeySlot,
                                          stlBool     * aIsCommitted,
                                          smxlTransId * aWaitTransId,
                                          smlEnv      * aEnv );

inline stlStatus smnmpbIsKeyAgable( void       * aPageFrame,
                                    stlChar    * aKeySlot,
                                    smlScn       aAgableScn,
                                    stlBool    * aIsAgable,
                                    smlEnv     * aEnv );

inline stlStatus smnmpbGetDeleteCommitScn( smlTbsId      aTbsId,
                                           smpHandle   * aPageHandle,
                                           stlChar     * aKeySlot,
                                           smlScn        aMyStmtViewScn,
                                           smlScn      * aCommitScn,
                                           smlTcn      * aTcn,
                                           smxlTransId * aTransId,
                                           smlEnv     * aEnv );

void smnmpbStabilizeKey( stlChar  * aKey,
                         stlUInt8   aRtsSeq,
                         stlUInt8   aLastRtsVerNo,
                         smlScn     aCommitScn,
                         stlInt16 * aRefernceCount );

stlStatus smnmpbDoAgingLeafNode( void      * aRelationHandle,
                                 smpHandle * aPageHandle,
                                 stlInt16  * aTracingSlotSeq,
                                 stlBool     aNeedRowStamping,
                                 stlBool   * aIsFreed,
                                 smlEnv    * aEnv );

stlStatus smnmpbCopyLeafKeys( smpHandle * aSrcPage,
                              smpHandle * aDstPage,
                              stlInt16    aFromSeq,
                              stlInt16    aToSeq,
                              smlEnv    * aEnv );

stlStatus smnmpbSnapshot( smlTbsId    aTbsId,
                          smlPid      aPageId,
                          smpHandle * aDstHandle,
                          stlChar   * aPageFrame,
                          smlEnv    * aEnv );

stlStatus smnmpbCopyIntlKeys( smpHandle * aSrcPage,
                              smpHandle * aDstPage,
                              stlInt16    aFromSeq,
                              stlInt16    aToSeq,
                              smlEnv    * aEnv );

stlStatus smnmpbCompactPage( void      * aRelationHandle,
                             smpHandle * aPageHandle,
                             stlInt16    aRowCountLimit,
                             smlEnv    * aEnv );

stlStatus smnmpbTruncateNode( void      * aRelationHandle,
                              smpHandle * aPageHandle,
                              stlUInt16   aLeftKeyCount,
                              smlEnv    * aEnv );

inline void smnmpbWriteKeyHeader( stlChar      * aKey,
                                  stlBool        aIsDupKey,
                                  stlBool        aIsTbk,
                                  stlUInt8       aRtsSeq,
                                  stlUInt8       aRtsVerNo,
                                  smxlTransId    aTransId,
                                  smlScn         aViewScn,
                                  smlRid       * aRowRid );

inline void smnmpbWriteKeyBody( stlChar           * aKey,
                                knlValueBlockList * aColList,
                                stlInt32            aBlockIdx );

inline smlPid smnmpbGetLastChild( smpHandle  * aPageHandle );

inline void smnmpbMakeValueListFromSlot( stlChar           * aKeyBody,
                                         knlValueBlockList * aColumns,
                                         stlChar           * aColBuf );

inline stlStatus smnmpbCompareKeys( char              * aKeySlot,
                                    stlUInt16           aKeyColCount, 
                                    knlValueBlockList * aValColumns,
                                    stlInt32            aBlockIdx, /* of ValColumns */
                                    smlRid            * aRowRid,
                                    stlUInt8          * aKeyColFlags,
                                    stlBool             aIsLeaf,
                                    dtlCompareResult  * aResult,
                                    smlEnv            * aEnv );

inline stlStatus smnmpbCompareWithMaxKey( smpHandle         * aPageHandle,
                                          knlValueBlockList * aValColumns,
                                          stlInt32            aBlockIdx, /* of ValColumns */
                                          smlRid            * aRowRid,
                                          stlUInt8          * aKeyColFlags,
                                          dtlCompareResult  * aResult,
                                          smlEnv            * aEnv );

stlStatus smnmpbCompareWithParentKey( smlTbsId            aTbsId,
                                      smlPid              aParentPid,
                                      stlInt16            aParentSlotSeq,
                                      stlUInt64           aParentSmoNo,
                                      smlPid              aChildPid,
                                      knlValueBlockList * aValColumns,
                                      stlInt32            aBlockIdx, /* of aValColumns */
                                      smlRid            * aRowRid,
                                      stlUInt8          * aKeyColFlags,
                                      dtlCompareResult  * aResult,
                                      smlEnv            * aEnv );

stlStatus smnmpbFindChildNode( smpHandle         * aPageHandle,
                               knlValueBlockList * aValColumns,
                               stlInt32            aBlockIdx, /* of ValColumns */
                               smlRid            * aRowRid,
                               stlUInt8          * aKeyColFlags,
                               smlPid            * aChildPid,
                               stlInt16          * aKeySlotSeq,
                               smlEnv            * aEnv );

stlStatus smnmpbFindPosition( knlValueBlockList * aValColumns,
                              stlInt32            aBlockIdx, /* of aValColumns */
                              smlRid            * aRowRid,
                              smpHandle         * aPageHandle,
                              stlInt16          * aSlotSeq,
                              stlBool           * aFoundKey,
                              smlEnv            * aEnv );

stlStatus smnmpbCheckUniqueness( smlStatement      * aStatement,
                                 smxmTrans         * aMiniTrans,
                                 smxmSavepoint     * aSavepoint,
                                 smlTbsId            aTbsId,
                                 knlValueBlockList * aValColumns,
                                 stlInt32            aBlockIdx,
                                 smlRid            * aRowRid,
                                 smpHandle         * aPageHandle,
                                 stlBool           * aIsUniqueViolated,
                                 stlBool           * aIsMyStmtViolation,
                                 stlInt32          * aIsNeedRetry,
                                 smlEnv            * aEnv );

stlStatus smnmpbAllocMultiPage( smxmTrans   * aMiniTrans,
                                void        * aRelationHandle,
                                smpPageType   aPageType ,
                                stlInt16      aPageCount,
                                smpHandle   * aPageArr,
                                stlInt32    * aNeedRetry,
                                smlEnv      * aEnv );

stlStatus smnmpbCopyRootKeys( void        * aRelationHandle,
                              smxmTrans   * aMiniTrans,
                              smpHandle   * aOrgChildPage,
                              smpHandle   * aNewChildPage,
                              stlInt16      aSplitBeginPos,
                              stlInt16      aSplitEndPos,
                              smlEnv      * aEnv );

stlStatus smnmpbPropagateKey2RootPage( void             * aRelationHandle,
                                       smxmTrans        * aMiniTrans,
                                       smpHandle        * aRootPageHandle,
                                       smnmpbPropKeyStr * aPropKey,
                                       smlPid             aChildPid,
                                       smlEnv           * aEnv );

stlStatus smnmpbMirrorRoot( void      * aRelationHandle,
                            smxmTrans * aMiniTrans,
                            smpHandle * aPageHandle,
                            smlEnv    * aEnv );

stlStatus smnmpbMakeNewRootNode( smxmTrans * aMiniTrans,
                                 void      * aRelationHandle,
                                 smpHandle * aOldRootPage,
                                 smlPid      aLeftChildPid,
                                 stlChar   * aPropKeyBody,
                                 stlUInt16   aPropKeyBodySize,
                                 smlRid    * aPropKeyRowRid,
                                 smpHandle * aTargetPage,
                                 stlInt16  * aTargetKeySeq,
                                 smpHandle * aOrgKeyPage,
                                 stlInt16  * aOrgKeySeq,
                                 smpHandle * aPageArr,
                                 smpHandle * aMirrorPageHandle,
                                 stlUInt64 * aSmoNo,
                                 stlInt32  * aNeedRetry,
                                 smlEnv    * aEnv );

stlStatus smnmpbCalculateSplitPositions( smxmTrans         * aMiniTrans,
                                         void              * aRelationHandle,
                                         smpHandle         * aOrgPageHandle,
                                         stlUInt16           aCurLevel,
                                         stlChar           * aPropKeyBody,
                                         stlUInt16           aPropKeyBodySize,
                                         smlRid            * aPropKeyRowRid,
                                         stlInt16            aPropKeySeq,
                                         stlInt16          * aSplitNodeCount,
                                         smnmpbPropKeyStr  * sSplitKeyStr,
                                         stlInt16          * aPropKeyNodeSeq,
                                         stlBool           * aUseMirrorRoot,
                                         smlEnv            * aEnv );

stlStatus smnmpbCalculateIntlSplitPositions( smpHandle         * aOrgPageHandle,
                                             stlChar           * aPropKeyBody,
                                             stlUInt16           aPropKeyBodySize,
                                             smlRid            * aPropKeyRowRid,
                                             stlInt16            aPropKeySeq,
                                             stlInt16          * aSplitNodeCount,
                                             smnmpbPropKeyStr  * aSplitKeyArr,
                                             stlInt16          * aPropKeyNodeSeq,
                                             smlEnv            * aEnv );

stlStatus smnmpbCalculateLeafSplitPositions( smpHandle         * aOrgPageHandle,
                                             stlChar           * aPropKeyBody,
                                             stlUInt16           aPropKeyBodySize,
                                             smlRid            * aPropKeyRowRid,
                                             stlInt16            aPropKeySeq,
                                             stlInt16          * aSplitNodeCount,
                                             smnmpbPropKeyStr  * aSplitKeyArr,
                                             stlInt16          * aPropKeyNodeSeq,
                                             smlEnv            * aEnv );

stlStatus smnmpbSplitRootNode( smxmTrans         * aMiniTrans,
                               void              * aRelationHandle,
                               smpHandle         * aNewRootPage,
                               smpHandle         * aOldRootPage,
                               smlRid            * aPropKeyRowRid,
                               stlChar           * aPropKeyBody,
                               stlUInt16           aPropKeyBodySize,
                               smlPid              aOrgChildPid,
                               stlInt16            aSplitPageCount,
                               smnmpbPropKeyStr  * aSplitKeyArr,
                               stlInt16            aPropKeyNodeSeq,
                               smpHandle         * aPageArr,
                               smpHandle         * aTargetPage,
                               stlInt16          * aTargetKeySeq,
                               smpHandle         * aOrgKeyPage,
                               stlInt16          * aOrgKeySeq,
                               smlEnv            * aEnv );

stlStatus smnmpbPropagateInternalKey( smxmTrans       * aMiniTrans,
                                      void            * aRelationHandle,
                                      smnmpbPathStack * aPathStack,
                                      stlUInt16         aCurLevel,
                                      smlPid            aOrgChildPid,
                                      stlChar         * aPropKeyBody,
                                      stlUInt16         aPropKeyBodySize,
                                      smlRid          * aPropKeyRowRid,
                                      smpHandle       * aPageArr,
                                      smpHandle       * aMirrorPageHandle,
                                      stlInt32        * aNeedRetry,
                                      smlEnv          * aEnv );

stlStatus smnmpbSplitLeafNode( smxmTrans         * aMiniTrans,
                               void              * aRelationHandle,
                               smpHandle         * aPageHandle,
                               stlChar           * aPropKeyBody,
                               stlUInt16           aKeyBodySize,
                               smlRid            * aRowRid,
                               stlInt16          * aSlotSeq,
                               smnmpbPathStack   * aPathStack,
                               stlInt32          * aNeedRetry,
                               stlInt16          * aEmptyPageCount,
                               smlPid            * aEmptyPid,
                               smlEnv            * aEnv );

stlStatus smnmpbInsertIntoLeaf( smxmTrans         * aMiniTrans,
                                void              * aRelationHandle,
                                smpHandle         * aPageHandle,
                                stlInt16            aSlotSeq,
                                knlValueBlockList * aColumns,
                                stlInt32            aBlockIdx,
                                smlRid            * aRowRid,
                                smlRid            * aUndoRid,
                                smlTcn              aTcn,
                                stlUInt16           aKeyBodySize,
                                smnmpbPathStack   * aPathStack,
                                stlBool             aIsIndexBuilding,
                                stlInt32          * aNeedRetry,
                                stlBool           * aIsSmoOccurred,
                                stlInt16          * aEmptyPageCount,
                                smlPid            * aEmptyPid,
                                smlEnv            * aEnv );

stlStatus smnmpbFindTargetLeaf( void              * aRelationHandle,
                                smxmTrans         * aMiniTrans,
                                knlValueBlockList * aValColumns,
                                stlInt32            aBlockIdx,
                                smlRid            * aRowRid,
                                smnmpbPathStack   * aPathStack,
                                smpHandle         * aPageHandle,
                                smlEnv            * aEnv );

stlStatus smnmpbFindChildNodeInRoot( void              * aRelationHandle,
                                     smxmTrans         * aMiniTrans,
                                     knlValueBlockList * aValColumns,
                                     stlInt32            aBlockIdx,
                                     smlRid            * aRowRid,
                                     smnmpbPathStack   * aPathStack,
                                     smlPid            * aChildPid,
                                     stlBool           * aIsSemiLeafOrLeaf,
                                     smlEnv            * aEnv );

stlStatus smnmpbInsert( smlStatement            * aStatement,
                        void                    * aRelationHandle,
                        knlValueBlockList       * aColumns,
                        stlInt32                  aBlockIdx,
                        smlRid                  * aRowRid,
                        smlRid                  * aUndoRid,
                        stlBool                   aIsIndexBuilding,
                        stlBool                   aIsDeferred,
                        smlUniqueViolationScope * aUniqueViolationScope,
                        smlEnv                  * aEnv );

stlStatus smnmpbDeleteFromLeaf( smlStatement      * aStatement,
                                smxmTrans         * aMiniTrans,
                                void              * aRelationHandle,
                                smpHandle         * aPageHandle,
                                stlInt16            aSlotSeq,
                                knlValueBlockList * aValColumns,
                                stlInt32            aBlockIdx,
                                smlRid            * aRowRid,
                                smlRid            * aUndoRid,
                                stlUInt16           aKeyBodySize,
                                smnmpbPathStack   * aPathStack,
                                stlBool             aIsWriteUndoLog,
                                stlInt32          * aNeedRetry,
                                stlInt16          * aEmptyPageCount,
                                smlPid            * aEmptyPid,
                                smlEnv            * aEnv );

stlStatus smnmpbMarkCompletionInUndoRecord( smxmTrans * aMiniTrans,
                                            smlRid    * aUndoRid,
                                            smlEnv    * aEnv );

stlStatus smnmpbRegisterEmptyNode( void      * aRelationHandle,
                                   smxmTrans * aMiniTrans,
                                   stlInt16    aEmptyPageCount,
                                   smlPid    * aEmptyPid,
                                   stlBool     aCheckAgingFlag,
                                   smlEnv    * aEnv );

stlStatus smnmpbDelete( smlStatement            * aStatement,
                        void                    * aRelationHandle,
                        knlValueBlockList       * aColumns,
                        stlInt32                  aBlockIdx,
                        smlRid                  * aRowRid,
                        smlRid                  * aUndoRid,
                        smlUniqueViolationScope * aUniqueViolationScope,
                        smlEnv                  * aEnv );

stlStatus smnmpbBlockInsert( smlStatement            * aStatement,
                             void                    * aRelationHandle,
                             knlValueBlockList       * aColumns,
                             smlRowBlock             * aRowBlock,
                             stlBool                 * aBlockFilter,
                             stlBool                   aIsDeferred,
                             stlInt32                * aViolationCnt,
                             smlUniqueViolationScope * aUniqueViolationScope,
                             smlEnv                  * aEnv );
stlStatus smnmpbBlockDelete( smlStatement            * aStatement,
                             void                    * aRelationHandle,
                             knlValueBlockList       * aColumns,
                             smlRowBlock             * aRowBlock,
                             stlBool                 * aBlockFilter,
                             stlInt32                * aViolationCnt,
                             smlUniqueViolationScope * aUniqueViolationScope,
                             smlEnv                  * aEnv );

stlStatus smnmpbDeleteInternal( smlStatement      * aStatement,
                                smxmTrans         * aMiniTrans,
                                void              * aRelationHandle,
                                knlValueBlockList * aColumns,
                                stlInt32            aBlockIdx,
                                smlRid            * aRowRid,
                                smlRid            * aUndoRid,
                                stlBool             aIsWriteUndoLog,
                                stlBool           * aIsUniqueResolved,
                                stlInt16          * aEmptyPageCount,
                                smlPid            * aEmptyPidList,
                                smlEnv            * aEnv );
stlStatus smnmpbUndeleteInternal( smlStatement      * aStatement,
                                  smxmTrans         * aMiniTrans,
                                  void              * aRelationHandle,
                                  knlValueBlockList * aColumns,
                                  smlRid            * aRowRid,
                                  smlEnv            * aEnv );

stlStatus smnmpbProcessEmptyNodes( void        * aRelationHandle,
                                   smxlTransId   aTransId,
                                   smlEnv      * aEnv );

stlStatus smnmpbRemoveIntlKey( void            * aRelationHandle,
                               smxmTrans       * aMiniTrans,
                               smlPid            aChildPid,
                               smnmpbPathStack * aPathStack,
                               stlInt16          aCurDepth,
                               smpHandle       * aSegPage,
                               stlInt32        * aNeedRetry,
                               stlBool         * aIsLastKey,
                               stlBool         * aRegisterNextLeaf,
                               smlEnv          * aEnv );

stlStatus smnmpbRemoveEmptyNodeFromTree( void       * aRelationHandle,
                                         smxmTrans  * aMiniTrans,
                                         stlBool    * aSkipRemove,
                                         smlPid     * aNextPid,
                                         smlEnv     * aEnv );

stlStatus smnmpbFindTargetKeyForRedo( smpHandle * aPageHandle,
                                      stlChar   * aColPtr,
                                      smlRid    * aRowRid,
                                      stlInt16  * aSlotSeq,
                                      stlBool   * aIsDupKey,
                                      smlEnv    * aEnv );

inline void * smnmpbGetSlotBody( void * aSlot );

void smnmpbTestPrintLogicalHdr( smpHandle * aPageHandle );

stlStatus smnmpbInitIterator ( void   * aIterator,
                               smlEnv * aEnv );

stlStatus smnmpbResetIterator( void   * aIterator,
                               smlEnv * aEnv );

stlStatus smnmpbGetRowRid( void   * aIterator,
                           smlRid * aRowRid,
                           smlEnv * aEnv );

stlStatus smnmpbMoveToRowRid( void   * aIterator,
                              smlRid * aRowRid,
                              smlEnv * aEnv );

stlStatus smnmpbMoveToPosNum( void   * aIterator,
                              stlInt64 aPosNum,
                              smlEnv * aEnv );

stlBool smnmpbIsSameTrans( smxlTransId aTransIdA,
                           smlScn      aCommitScnA,
                           smxlTransId aTransIdB,
                           smlScn      aCommitScnB );

inline stlStatus smnmpbCheckMinRange( char              * aKeySlot,
                                      knlCompareList    * aCompList,
                                      stlBool             aIsLeaf,
                                      stlBool           * aResult,
                                      smlEnv            * aEnv );

inline stlStatus smnmpbCheckMaxRange( char              * aKeySlot,
                                      knlCompareList    * aCompList,
                                      stlBool             aIsLeaf,
                                      stlBool           * aResult,
                                      smlEnv            * aEnv );

inline stlStatus smnmpbCheckMinRangeWithMaxKey( smpHandle         * aPageHandle,
                                                smnmpbIterator    * aIterator,
                                                smlFetchInfo      * aFetchInfo,
                                                stlBool           * aResult,
                                                smlEnv            * aEnv );

stlStatus smnmpbFindFirstChild( smpHandle         * aPageHandle,
                                smnmpbIterator    * aIterator,
                                smlFetchInfo      * aFetchInfo,
                                smlPid            * aChildPid,
                                smlEnv            * aEnv );

stlStatus smnmpbBeforeFirstLeafKey( smpHandle         * aPageHandle,
                                    smnmpbIterator    * aIterator,
                                    smlFetchInfo      * aFetchInfo,
                                    stlInt16            aEndSeq,
                                    stlInt16          * aSlotSeq,
                                    smlEnv            * aEnv );

stlStatus smnmpbFindFirstLeafKey( smpHandle         * aPageHandle,
                                  smlPid              aStartPid,
                                  smnmpbIterator    * aIterator,
                                  smlFetchInfo      * aFetchInfo,
                                  smlEnv            * aEnv );

stlStatus smnmpbFindFirst( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smpHandle     * aPageHandle,
                           smlEnv        * aEnv );

inline void smnmpbFetchKeyValues( stlChar           * aColPtr,
                                  knlValueBlockList * aColList,
                                  stlInt32            aBlockIdx );

stlStatus smnmpbCheckRowColWithKey( smnmpbIterator    * aIterator,
                                    smlFetchInfo      * aFetchInfo,
                                    smpHandle         * aTablePageHandle,
                                    stlBool           * aTablePageLatchRelease,
                                    smxlTransId         aMyTransId,
                                    smlScn              aMyViewScn,
                                    smlScn              aMyCommitScn,
                                    smlTcn              aMyViewTcn,
                                    stlChar           * aKey,
                                    stlBool           * aIsMatch,
                                    smlScn            * aCommitScn,
                                    smlEnv            * aEnv );

stlStatus smnmpbGetValidVersion( smlTbsId               aTbsId,
                                 smpHandle            * aPageHandle,
                                 smnmpbIterator       * aIterator,
                                 smlFetchInfo         * aFetchInfo,
                                 stlChar              * aKey,
                                 knlValueBlockList    * aColList,
                                 stlInt32               aBlockIdx,
                                 smlScn               * aCommitScn,
                                 smxlTransId          * aWaitTransId,
                                 stlBool              * aHasValidVersion,
                                 stlBool              * aMatchPhysicalFilter,
                                 smlEnv               * aEnv );

stlStatus smnmpbGetValidVersionsStablePage( smnmpbIterator  * aIterator,
                                            stlChar         * aPageFrame,
                                            smlFetchInfo    * aFetchInfo,
                                            stlBool         * aEndOfSlot,
                                            stlBool         * aEndOfScan,
                                            smlEnv          * aEnv );
stlStatus smnmpbGetValidVersions( smnmpbIterator  * aIterator,
                                  smlTbsId          aTbsId,
                                  smpHandle       * aPageHandle,
                                  smlFetchInfo    * aFetchInfo,
                                  stlBool         * aEndOfSlot,
                                  stlBool         * aEndOfScan,
                                  smlEnv          * aEnv );

inline stlStatus smnmpbFindMaxFence( smnmpbIterator    * aIterator,
                                     smlFetchInfo      * aFetchInfo,
                                     void              * aPageFrame,
                                     stlInt16            aStartSeq,
                                     smlEnv            * aEnv );

stlStatus smnmpbFetchNext( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

stlStatus smnmpbFetchAggr( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

inline stlStatus smnmpbFindMinFence( smnmpbIterator    * aIterator,
                                     smlFetchInfo      * aFetchInfo,
                                     void              * aPageFrame,
                                     stlInt16            aEndSeq,
                                     smlEnv            * aEnv );

stlStatus smnmpbAfterLastLeafKey( smpHandle         * aPageHandle,
                                  smnmpbIterator    * aIterator,
                                  smlFetchInfo      * aFetchInfo,
                                  stlInt16            aStartSeq,
                                  stlInt16          * aSlotSeq,
                                  smlEnv            * aEnv );

stlStatus smnmpbFindLastLeafKey( smpHandle         * aPageHandle,
                                 smlPid              aStartPid,
                                 smnmpbIterator    * aIterator,
                                 smlFetchInfo      * aFetchInfo,
                                 smlEnv            * aEnv );

stlStatus smnmpbFindLastChild( smpHandle         * aPageHandle,
                               smnmpbIterator    * aIterator,
                               smlFetchInfo      * aFetchInfo,
                               smlPid            * aChildPid,
                               smlEnv            * aEnv );

inline stlStatus smnmpbCheckMaxRangeWithMaxKey( smpHandle         * aPageHandle,
                                                smnmpbIterator    * aIterator,
                                                smlFetchInfo      * aFetchInfo,
                                                stlBool           * aResult,
                                                smlEnv            * aEnv );

stlStatus smnmpbFindLast( void          * aIterator,
                          smlFetchInfo  * aFetchInfo,
                          smpHandle     * aPageHandle,
                          smlEnv        * aEnv );

stlStatus smnmpbFetchPrev( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

stlStatus smnmpbInitModuleSpecificData( void   * aRelationHandle,
                                        smlEnv * aEnv );

stlStatus smnmpbDestModuleSpecificData( void   * aRelationHandle,
                                        smlEnv * aEnv );

void smnmpbValidateKeySpace( smpHandle * aPageHandle );

stlInt16 smnmpbCountRtsRef( stlChar  * aPageFrame,
                            stlChar  * aKey,
                            stlUInt8   aRtsSeq,
                            smlEnv   * aEnv );

stlStatus smnmpbValidateIndexPage( smpHandle * aPageHandle,
                                   smlEnv    * aEnv );

stlInt32 smnmpbCompareFxKey( smnmpbFxVerifyOrderIterator * aIterator,
                             stlBool                       aIsLeafA,
                             stlBool                       aIsLeafB,
                             void                        * aValueA,
                             void                        * aValueB );

stlStatus smnmpbVerifyOrderTree( smnmpbFxVerifyOrderIterator * aIterator,
                                 smlPid                        aPageId,
                                 stlUInt16                     aDepth,
                                 stlChar                     * aKeyPa,
                                 stlChar                     * aKeyPb,
                                 stlBool                     * aResult,
                                 smlEnv                      * aEnv );

stlStatus smnmpbVerifyStructureTree( smnmpbFxVerifyStructureIterator * aIterator,
                                     smlPid                            aPageId,
                                     stlUInt16                         aDepth,
                                     smlPid                            aPidPa,
                                     smlPid                          * aPidCn,
                                     stlBool                         * aResult,
                                     smlEnv                          * aEnv );

stlStatus smnmpbAdjustPrevPageId( smnmpbParallelArgs  * aArgs,
                                  smlPid                aPageId,
                                  smlPid                aPrevPageId,
                                  smlEnv              * aEnv );

stlStatus  smnmpbMakeKeyString( stlChar   * aColPtr,
                                stlUInt16   aColIdx,
                                stlUInt16   aColCount,
                                stlUInt8  * aColTypes,
                                stlChar   * aColStrBuf,
                                knlEnv    * aEnv );

stlStatus smnmpbIsAgable( smpHandle * aPageHandle,
                          stlBool   * aIsAgable,
                          smlEnv    * aEnv );

/** @} */
    
#endif /* _SMNMPB_H_ */
