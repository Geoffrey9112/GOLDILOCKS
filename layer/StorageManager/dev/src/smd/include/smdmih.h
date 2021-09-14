/*******************************************************************************
 * smdmih.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmih.h 7472 2013-03-04 01:57:08Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMDMIH_H_
#define _SMDMIH_H_ 1

#include <smdmifTable.h>
#include <smdmihDef.h>

/**
 * @file smdmih.h
 * @brief Storage Manager Layer Memory Instant Hash Table Internal Routines
 */

/**
 * @defgroup smdmih Memory Instant Hash Table
 * @ingroup smInternal
 * @{
 */

stlStatus smdmihCreateIot( smlStatement      * aStatement,
                           smlTbsId            aTbsId,
                           smlTableAttr      * aTableAttr,
                           smlIndexAttr      * aIndexAttr,
                           knlValueBlockList * aRowColList,
                           stlUInt16           aKeyColCount,
                           stlUInt8          * aKeyColFlags,
                           stlInt64          * aSegmentId,
                           void             ** aRelationHandle,
                           smlEnv            * aEnv );

stlStatus smdmihDrop( smlStatement * aStatement,
                      void         * aRelationHandle,
                      smlEnv       * aEnv );

stlStatus smdmihCleanup( void    * aRelationHandle,
                         smlEnv  * aEnv );

stlStatus smdmihAlterTableAttr( smlStatement  * aStatement,
                                void          * aRelationHandle,
                                smlTableAttr  * aTableAttr,
                                smlIndexAttr  * aIndexAttr,
                                smlEnv        * aEnv );

stlStatus smdmihTruncate( smlStatement * aStatement,
                          void         * aRelationHandle,
                          smlEnv       * aEnv );

stlStatus smdmihInsert( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aInsertCols,
                        knlValueBlockList * aUniqueViolation,
                        smlRid            * aRowRid,
                        smlEnv            * aEnv );

stlStatus smdmihBlockInsert( smlStatement      * aStatement,
                             void              * aRelationHandle,
                             knlValueBlockList * aInsertCols,
                             knlValueBlockList * aUniqueViolation,
                             smlRowBlock       * aRowBlock,
                             smlEnv            * aEnv );

stlStatus smdmihMerge( smlStatement       * aStatement,
                       void               * aRelationHandle,
                       knlValueBlockList  * aInsertCols,
                       smlMergeRecordInfo * aMergeRecordInfo,
                       smlEnv             * aEnv );

stlStatus smdmihRowCount( void      * aRelationHandle,
                          stlInt64  * aRowCount,
                          smlEnv    * aEnv );

stlStatus smdmihInsertInternal( smdmihTableHeader  * aTableHeader,
                                knlValueBlockList  * aInsertCols,
                                knlValueBlockList  * aUniqueViolation,
                                stlInt32             aBlockIdx,
                                smlMergeRecordInfo * aMergeRecordInfo,
                                smlRid             * aRowId,
                                smlEnv             * aEnv );

stlStatus smdmihUpdate( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aAfterCols,
                        knlValueBlockList * aUpdateCols,
                        knlValueBlockList * aPrimaryKey,
                        stlBool           * aVerionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv );

stlStatus smdmihDelete( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aPrimaryKey,
                        stlBool           * aVerionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv );

stlStatus smdmihInitTableHeader( smdmihTableHeader * aHashTableHeader,
                                 smlTbsId            aTbsId,
                                 smlIndexAttr      * aAttr,
                                 stlUInt16           aKeyColCount,
                                 knlValueBlockList * aRowColList,
                                 stlUInt8          * aKeyColFlags,
                                 smlEnv            * aEnv );

stlStatus smdmihInitColumns( smdmihTableHeader * aHashTableHeader,
                             knlValueBlockList * aRowColList,
                             smlEnv            * aEnv );

stlStatus smdmihExtending( smdmihTableHeader  * aTableHeader,
                           smlEnv             * aEnv );

stlStatus smdmihAllocSlot( smdmihTableHeader  * aTableHeader,
                           stlChar           ** aKey,
                           stlInt32             aKeyLen,
                           smlEnv             * aEnv );

stlStatus smdmihInsertKey( smdmihTableHeader  * aTableHeader,
                           smdmihDirSlot      * aDirSlot,
                           stlInt32             aBucketIdx,
                           stlUInt32            aHashKey,
                           knlValueBlockList  * aInsertCols,
                           knlValueBlockList  * aUniqueViolation,
                           stlInt32             aBlockIdx,
                           smlMergeRecordInfo * aMergeRecordInfo,
                           knlLogicalAddr     * aRowLa,
                           smlEnv             * aEnv );

stlStatus smdmihBlockInsertKey( smdmihTableHeader  * aTableHeader,
                                stlUInt32            aHashKey,
                                knlValueBlockList  * aInsertCols,
                                stlInt32             aStartIdx,
                                stlInt32             aEndIdx,
                                smlMergeRecordInfo * aMergeRecordInfo,
                                smlEnv             * aEnv );

stlStatus smdmihAllocDataNode( smdmihTableHeader  * aTableHeader,
                               stlChar           ** aDataNode,
                               smlEnv             * aEnv );

stlUInt32 smdmihGetHashKeyFromColumn( stlChar  * aColValue,
                                      stlInt64   aColLen );

stlStatus smdmihCreateDir( smdmihTableHeader * aTableHeader,
                           smlEnv            * aEnv );

void smdmihInitDataNode( smdmihNodeHeader * aNodeHeader );

void smdmihInitDirNode( knlLogicalAddr aDirSlotArrayLa,
                        stlInt32       aSlotCount );

stlBool smdmihCompareKeyAndBlock( smdmihTableHeader  * aTableHeader,
                                  knlValueBlockList  * aDataBlockList,
                                  stlInt64             aBlockIdx,
                                  stlChar            * aKey );

stlBool smdmihCompareKeyAndKey( smdmihTableHeader  * aTableHeader,
                                stlChar            * aKey1,
                                stlChar            * aKey2 );

stlStatus smdmihFetchOneRow( smdmihTableHeader * aHeader,
                             void              * aFixedRow,
                             smlFetchInfo      * aFetchInfo,
                             stlInt32            aBlockIdx,
                             stlBool           * aFetched,
                             smlEnv            * aEnv );

stlStatus smdmihFetchOneRow4SemiJoin( smdmihIterator    * aIterator,
                                      smdmihTableHeader * aTableHeader,
                                      smlFetchInfo      * aFetchInfo,
                                      stlBool           * aMatchRow,
                                      smlEnv            * aEnv );

stlStatus smdmihFetchOneRow4AntiSemiJoin( smdmihIterator    * aIterator,
                                          smdmihTableHeader * aTableHeader,
                                          smlFetchInfo      * aFetchInfo,
                                          stlBool           * aMatchRow,
                                          smlEnv            * aEnv );

stlStatus smdmihValidate( smdmihTableHeader  * aTableHeader,
                          smlEnv             * aEnv );

void smdmihFixedRowWriteCol( stlChar   * aDestColumn,
                             stlUInt16   aColLenSize,
                             stlChar   * aDataValue,
                             stlUInt16   aDataSize );

stlStatus smdmihCheckUniqueness( smdmihTableHeader  * aTableHeader,
                                 smdmihDirSlot      * aDirSlot,
                                 stlUInt32            aHashKey,
                                 knlValueBlockList  * aInsertCols,
                                 stlInt32             aBlockIdx,
                                 stlBool            * aUniqueViolation,
                                 stlChar           ** aDupRow,
                                 smlEnv             * aEnv );

stlStatus smdmihMergeRecordInternal( smdmihTableHeader  * aTableHeader,
                                     stlChar            * aRow,
                                     smlMergeRecordInfo * aMergeRecordInfo,
                                     stlInt32             aStartIdx,
                                     stlInt32             aEndIdx,
                                     smlEnv             * aEnv );

stlStatus smdmihFindDataNode( smdmihTableHeader    * aTableHeader,
                              smdmihDirSlot        * aDirSlot,
                              smdmihCommonIterator * aIterator,
                              smlEnv               * aEnv );

stlStatus smdmihMergeRecordCountAst( smdmihTableHeader  * aTableHeader,
                                     stlChar            * aRow,
                                     smlAggrFuncInfo    * aAggrFuncInfo,
                                     stlInt32             aStartIdx,
                                     stlInt32             aEndIdx,
                                     knlValueBlockList  * aReadBlock,
                                     knlValueBlockList  * aUpdateBlock,
                                     smlEnv             * aEnv );

stlStatus smdmihMergeRecordCount( smdmihTableHeader  * aTableHeader,
                                  stlChar            * aRow,
                                  smlAggrFuncInfo    * aAggrFuncInfo,
                                  stlInt32             aStartIdx,
                                  stlInt32             aEndIdx,
                                  knlValueBlockList  * aReadBlock,
                                  knlValueBlockList  * aUpdateBlock,
                                  smlEnv             * aEnv );

stlStatus smdmihMergeRecordCountWithExpr( smdmihTableHeader  * aTableHeader,
                                          stlChar            * aRow,
                                          smlAggrFuncInfo    * aAggrFuncInfo,
                                          stlInt32             aStartIdx,
                                          stlInt32             aEndIdx,
                                          knlValueBlockList  * aReadBlock,
                                          knlValueBlockList  * aUpdateBlock,
                                          smlEnv             * aEnv );

stlStatus smdmihMergeRecordSum( smdmihTableHeader  * aTableHeader,
                                stlChar            * aRow,
                                smlAggrFuncInfo    * aAggrFuncInfo,
                                stlInt32             aStartIdx,
                                stlInt32             aEndIdx,
                                knlValueBlockList  * aReadBlock,
                                knlValueBlockList  * aUpdateBlock,
                                smlEnv             * aEnv );

stlStatus smdmihMergeRecordSumWithExpr( smdmihTableHeader  * aTableHeader,
                                        stlChar            * aRow,
                                        smlAggrFuncInfo    * aAggrFuncInfo,
                                        stlInt32             aStartIdx,
                                        stlInt32             aEndIdx,
                                        knlValueBlockList  * aReadBlock,
                                        knlValueBlockList  * aUpdateBlock,
                                        smlEnv             * aEnv );

stlStatus smdmihMergeRecordMin( smdmihTableHeader  * aTableHeader,
                                stlChar            * aRow,
                                smlAggrFuncInfo    * aAggrFuncInfo,
                                stlInt32             aStartIdx,
                                stlInt32             aEndIdx,
                                knlValueBlockList  * aReadBlock,
                                knlValueBlockList  * aUpdateBlock,
                                smlEnv             * aEnv );

stlStatus smdmihMergeRecordMax( smdmihTableHeader  * aTableHeader,
                                stlChar            * aRow,
                                smlAggrFuncInfo    * aAggrFuncInfo,
                                stlInt32             aStartIdx,
                                stlInt32             aEndIdx,
                                knlValueBlockList  * aReadBlock,
                                knlValueBlockList  * aUpdateBlock,
                                smlEnv             * aEnv );

stlStatus smdmihMergeRecordAggr( smdmihTableHeader  * aTableHeader,
                                 stlChar            * aRow,
                                 smlAggrFuncInfo    * aAggrFuncInfo,
                                 stlInt32             aStartIdx,
                                 stlInt32             aEndIdx,
                                 knlValueBlockList  * aReadBlock,
                                 knlValueBlockList  * aUpdateBlock,
                                 smlEnv             * aEnv );

stlStatus smdmihFetchNext4FullScan( void          * aIterator,
                                    smlFetchInfo  * aFetchInfo,
                                    smlEnv        * aEnv );

stlStatus smdmihFetchNext4SemiJoin( void          * aIterator,
                                    smlFetchInfo  * aFetchInfo,
                                    smlEnv        * aEnv );

stlStatus smdmihFetchNext4AntiSemiJoin( void            * aIterator,
                                        smlFetchInfo    * aFetchInfo,
                                        smlEnv          * aEnv );

stlBool smdmihNeedExtending( smdmihTableHeader * aTableHeader,
                             smdmihDirSlot     * aDirSlot );

stlStatus smdmihNullAwareCompare( smdmihTableHeader     * aTableHeader,
                                  smlBlockJoinFetchInfo * aJoinFetchInfo,
                                  knlDataValueList      * aHashConstList,
                                  stlBool                 aFirstCompare,
                                  stlBool               * aMatchRow,
                                  smlEnv                * aEnv );

/** @} */
    
#endif /* _SMDMIH_H_ */
