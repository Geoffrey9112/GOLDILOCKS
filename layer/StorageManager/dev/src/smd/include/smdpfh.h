/*******************************************************************************
 * smdpfh.h
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


#ifndef _SMDPFH_H_
#define _SMDPFH_H_ 1

/**
 * @file smdpfh.h
 * @brief Storage Manager Layer Fixed Table Internal Routines
 */

/**
 * @defgroup smdpfh Fixed Table
 * @ingroup smd
 * @{
 */

stlStatus smdpfhInitTableHeader( smlStatement  * aStatement,
                                 smlRid        * aSegmentRid,
                                 void          * aSegmentHandle,
                                 smlTableAttr  * aAttr,
                                 void         ** aRelationHandle,
                                 smlEnv        * aEnv );

stlStatus smdpfhCreate( smlStatement  * aStatement,
                        smlTbsId        aTbsId,
                        smlTableAttr  * aAttr,
                        stlBool         aLoggingUndo,
                        stlInt64      * aSegmentId,
                        void         ** aRelationHandle,
                        smlEnv        * aEnv );
    
stlStatus smdpfhDrop( smlStatement * aStatement,
                      void         * aRelationHandle,
                      smlEnv       * aEnv );

stlStatus smdpfhInitialize( smlStatement * aStatement,
                            void         * aRelationHandle,
                            smlEnv       * aEnv );

stlStatus smdpfhFinalize( smlStatement * aStatement,
                          void         * aRelationHandle,
                          smlEnv       * aEnv );

stlStatus smdpfhInsert( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aInsertCols,
                        knlValueBlockList * aUniqueViolation,
                        smlRid            * aRowRid,
                        smlEnv            * aEnv );

stlStatus smdpfhUpdate( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aAfterCols,
                        knlValueBlockList * aBeforeCols,
                        knlValueBlockList * aPrimaryKey,
                        stlBool           * aVersionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv );

stlStatus smdpfhDelete( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aPrimaryKey,
                        stlBool           * aVersionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv );

stlStatus smdpfhLockRow( smlStatement * aStatement,
                         void         * aRelationHandle,
                         smlRid       * aRowRid,
                         smlScn         aRowScn,
                         stlUInt16      aLockMode,
                         stlInt64       aTimeInterval,
                         stlBool      * aIsVersionConflict,
                         smlEnv       * aEnv );

stlStatus smdpfhSort( void * RelationHandle, void * aTransactionHandle );






inline void * smdpfhGetSlotBody( void * aSlot );






stlStatus smdpfhInitIterator ( void   * aIterator,
                               smlEnv * aEnv );

stlStatus smdpfhFiniIterator ( void   * aIterator,
                               smlEnv * aEnv );

stlStatus smdpfhResetIterator( void   * aIterator,
                               smlEnv * aEnv );

stlStatus smdpfhGetRowRid( void   * aIterator, 
                           smlRid * aRowRid,
                           smlEnv * aEnv );

stlStatus smdpfhMoveToRowRid( void   * aIterator, 
                              smlRid * aRowRid,
                              smlEnv * aEnv );

stlStatus smdpfhMoveToPosNum( void   * aIterator,
                              stlInt64 aPosNum,
                              smlEnv * aEnv );

stlStatus smdpfhFetchNext( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

stlStatus smdpfhFetchPrev( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

/** @} */
    
#endif /* _SMDPFH_H_ */
