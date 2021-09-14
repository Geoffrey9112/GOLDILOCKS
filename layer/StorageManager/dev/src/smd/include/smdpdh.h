/*******************************************************************************
 * smdpdh.h
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


#ifndef _SMDPDH_H_
#define _SMDPDH_H_ 1

/**
 * @file smdpdh.h
 * @brief Storage Manager Layer Dump Table Internal Routines
 */

/**
 * @defgroup smdpdh Dump Table
 * @ingroup smd
 * @{
 */

stlStatus smdpdhInitTableHeader( smlStatement  * aStatement,
                                 smlRid        * aSegmentRid,
                                 void          * aSegmentHandle,
                                 smlTableAttr  * aAttr,
                                 void         ** aRelationHandle,
                                 smlEnv        * aEnv );

stlStatus smdpdhCreate( smlStatement  * aStatement,
                        smlTbsId        aTbsId,
                        smlTableAttr  * aAttr,
                        stlBool         aLoggingUndo,
                        stlInt64      * aSegmentId,
                        void         ** aRelationHandle,
                        smlEnv        * aEnv );
    
stlStatus smdpdhDrop( smlStatement * aStatement,
                      void         * aRelationHandle,
                      smlEnv       * aEnv );

stlStatus smdpdhInitialize( smlStatement * aStatement,
                            void         * aRelationHandle,
                            smlEnv       * aEnv );

stlStatus smdpdhFinalize( smlStatement * aStatement,
                          void         * aRelationHandle,
                          smlEnv       * aEnv );

stlStatus smdpdhInsert( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aInsertColumns,
                        knlValueBlockList * aUniqueViolation,
                        smlRid            * aRowRid,
                        smlEnv            * aEnv );

stlStatus smdpdhUpdate( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aAfterColumns,
                        knlValueBlockList * aBeforeColumns,
                        knlValueBlockList * aPrimaryKey,
                        stlBool           * aVersionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv );

stlStatus smdpdhDelete( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aPrimaryKey,
                        stlBool           * aVersionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv );

stlStatus smdpdhLockRow( smlStatement * aStatement,
                         void         * aRelationHandle,
                         smlRid       * aRowRid,
                         smlScn         aRowScn,
                         stlUInt16      aLockMode,
                         stlInt64       aTimeInterval,
                         stlBool      * aVersionConflict,
                         smlEnv       * aEnv );

stlStatus smdpdhSort( void * RelationHandle, void * aTransactionHandle );

stlStatus smdpdhBuild( void * RelationHandle, void * aTransactionHandle );

inline void * smdpdhGetSlotBody( void * aSlot );







stlStatus smdpdhInitIterator ( void   * aIterator,
                               smlEnv * aEnv );

stlStatus smdpdhFiniIterator ( void   * aIterator,
                               smlEnv * aEnv );

stlStatus smdpdhResetIterator( void   * aIterator,
                               smlEnv * aEnv );

stlStatus smdpdhGetRowRid( void   * aIterator, 
                           smlRid * aRowRid,
                           smlEnv * aEnv );

stlStatus smdpdhMoveToRowRid( void   * aIterator, 
                              smlRid * aRowRid,
                              smlEnv * aEnv );

stlStatus smdpdhMoveToPosNum( void   * aIterator,
                              stlInt64 aPosNum,
                              smlEnv * aEnv );

stlStatus smdpdhFetchNext( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

stlStatus smdpdhFetchPrev( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

/** @} */
    
#endif /* _SMDPDH_H_ */
