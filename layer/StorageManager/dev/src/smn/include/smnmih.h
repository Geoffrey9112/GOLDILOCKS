/*******************************************************************************
 * smnmih.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smnmih.h 7973 2013-04-04 12:25:55Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMNMIH_H_
#define _SMNMIH_H_ 1

#include <smnDef.h>
#include <smnmihDef.h>

/**
 * @file smnmih.h
 * @brief Storage Manager Layer Memory Instant Hash Index Internal Routines
 */

/**
 * @defgroup smnmih Memory Instant Hash Index
 * @ingroup smInternal
 * @{
 */

stlStatus smnmihCreate( smlStatement      * aStatement,
                        smlTbsId            aTbsId,
                        smlIndexAttr      * aAttr,
                        void              * aBaseTableHandle,
                        stlUInt16           aKeyColCount,
                        knlValueBlockList * aKeyColList,
                        stlUInt8          * aKeyColFlags,
                        stlInt64          * aSegmentId,
                        void             ** aRelationHandle,
                        smlEnv            * aEnv );

stlStatus smnmihDrop( smlStatement * aStatement,
                      void         * aRelationHandle,
                      smlEnv       * aEnv );

stlStatus smnmihBuild( smlStatement      * aStatement,
                       void              * aIndexHandle,
                       void              * aBaseTableHandle,
                       stlUInt16           aKeyColCount,
                       knlValueBlockList * aIndexColList,
                       stlUInt8          * aKeyColFlags,
                       stlUInt16           aParallelFactor,
                       smlEnv            * aEnv );

stlStatus smnmihTruncate( smlStatement * aStatement,
                          void         * aRelationHandle,
                          smlEnv       * aEnv );

stlStatus smnmihInsert( smlStatement            * aStatement,
                        void                    * aRelationHandle,
                        knlValueBlockList       * aColumns,
                        stlInt32                  aValueIdx,
                        smlRid                  * aRowRid,
                        smlRid                  * aUndoRid,
                        stlBool                   aIsIndexBuilding,
                        stlBool                   aIsDeferred,
                        smlUniqueViolationScope * aUniqueViolationScope,
                        smlEnv                  * aEnv );

stlStatus smnmihBlockInsert( smlStatement            * aStatement,
                             void                    * aRelationHandle,
                             knlValueBlockList       * aColumns,
                             smlRowBlock             * aRowBlock,
                             stlBool                 * aBlockFilter,
                             stlBool                   aIsDeferred,
                             stlInt32                * aViolationCnt,
                             smlUniqueViolationScope * aUniqueViolationScope,
                             smlEnv                  * aEnv );

stlStatus smnmihInitIterator( void   * aIterator,
                              smlEnv * aEnv );

stlStatus smnmihResetIterator( void   * aIterator,
                               smlEnv * aEnv );

stlStatus smnmihFetchNext( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

stlStatus smnmihInitIndexHeader( smnmihIndexHeader * aHashIndexHeader,
                                 void              * aTableHandle,
                                 smlTbsId            aTbsId,
                                 smlIndexAttr      * aAttr,
                                 stlUInt16           aKeyColCount,
                                 knlValueBlockList * aKeyColList,
                                 stlUInt8          * aKeyColFlags,
                                 smlEnv            * aEnv );

void smnmihCalcBucketCount( smnmihIndexHeader * aIndexHeader,
                            stlInt64            aRowCount );

stlStatus smnmihInitColumns( smnmihIndexHeader * aHashIndexHeader,
                             knlValueBlockList * aKeyColList,
                             smlEnv            * aEnv );

stlStatus smnmihBuildNodes( smnmihIndexHeader * aIndexHeader,
                            smdmifTableHeader * aTableHeader,
                            smlEnv            * aEnv );

stlStatus smnmihAllocSlot( smnmihIndexHeader  * aIndexHeader,
                           stlChar           ** aKey,
                           knlLogicalAddr     * aKeyLa,
                           smlEnv             * aEnv );

stlStatus smnmihAllocDataNode( smnmihIndexHeader  * aIndexHeader,
                               stlChar           ** aDataNode,
                               smlEnv             * aEnv );

void smnmihBeforeFirst( smnmihIterator        * aIterator,
                        smnmihIndexHeader     * aIndexHeader,
                        smlBlockJoinFetchInfo * aJoinFetchInfo,
                        stlBool               * aLeftIncludeNull );

stlBool smnmihCompareKey( smnmihIndexHeader  * aIndexHeader,
                          stlChar            * aRow,
                          stlBool              aAwareNull,
                          knlDataValueList   * aDataValueList );

stlStatus smnmihFetchNextInnerJoin( void          * aIterator,
                                    smlFetchInfo  * aFetchInfo,
                                    smlEnv        * aEnv );

stlStatus smnmihFetchNextLeftOuterJoin( void          * aIterator,
                                        smlFetchInfo  * aFetchInfo,
                                        smlEnv        * aEnv );

stlStatus smnmihFetchNextAntiOuterJoin( void          * aIterator,
                                        smlFetchInfo  * aFetchInfo,
                                        smlEnv        * aEnv );

stlStatus smnmihFetchNextSemiJoin( void          * aIterator,
                                   smlFetchInfo  * aFetchInfo,
                                   smlEnv        * aEnv );

stlStatus smnmihFetchNextAntiSemiJoin( void          * aIterator,
                                       smlFetchInfo  * aFetchInfo,
                                       smlEnv        * aEnv );

stlStatus smnmihSetNextRightRow( smnmihIterator    * aIterator,
                                 smnmihIndexHeader * aIndexHeader,
                                 smlFetchInfo      * aFetchInfo,
                                 stlBool           * aHasMatchRow,
                                 knlLogicalAddr    * aMatchKeyLa,
                                 stlChar          ** aMatchKey,
                                 smlEnv            * aEnv );

stlStatus smnmihFetchNextUnhitRow( smnmihIterator    * aIterator,
                                   smnmihIndexHeader * aIndexHeader,
                                   smlFetchInfo      * aFetchInfo,
                                   stlBool           * aHasMatchRow,
                                   smlEnv            * aEnv );

stlStatus smnmihNullAwareCompare( smnmihIndexHeader  * aIndexHeader,
                                  smlFetchInfo       * aJoinFetchInfo,
                                  knlDataValueList   * aHashConstList,
                                  stlBool              aFirstCompare,
                                  stlBool            * aMatchRow,
                                  stlChar           ** aMatchKey,
                                  smlEnv             * aEnv );

stlStatus smnmihResetHitKeys( smnmihIterator    * aIterator,
                              smnmihIndexHeader * aIndexHeader,
                              smlEnv            * aEnv );

stlStatus smnmihFullScanWithRowFilter( smnmihIterator    * aIterator,
                                       smnmihIndexHeader * aIndexHeader,
                                       smlFetchInfo      * aFetchInfo,
                                       stlBool           * aIsMatched,
                                       smlEnv            * aEnv );

/** @} */
    
#endif /* _SMNMIH_H_ */
