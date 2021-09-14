/*******************************************************************************
 * smdmpp.h
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


#ifndef _SMDMPP_H_
#define _SMDMPP_H_ 1

/**
 * @file smdmpp.h
 * @brief Storage Manager Layer Memory Pending Table Internal Routines
 */

/**
 * @defgroup smdmpp Memory Pending Table
 * @ingroup smInternal
 * @{
 */

stlStatus smdmppCreate( smlStatement  * aStatement,
                        smlTbsId        aTbsId,
                        smlTableAttr  * aAttr,
                        stlBool         aLoggingUndo,
                        stlInt64      * aSegmentId,
                        void         ** aRelationHandle,
                        smlEnv        * aEnv );
    
stlStatus smdmppDrop( smlStatement * aStatement,
                      void         * aRelationHandle,
                      smlEnv       * aEnv );

stlStatus smdmppDropPending( stlUInt32   aActionFlag,
                             smgPendOp * aPendOp,
                             smlEnv    * aEnv );

stlStatus smdmppTruncate( smlStatement * aStatement,
                          void         * aRelationHandle,
                          smlEnv       * aEnv );

stlStatus smdmppDropAging( smxlTransId   aTransId,
                           void        * aRelationHandle,
                           stlBool       aOnStartup,
                           smlEnv      * aEnv );

stlStatus smdmppInitTableHeader( smlStatement  * aStatement,
                                 smlRid        * aSegmentRid,
                                 void          * aSegmentHandle,
                                 void         ** aRelationHandle,
                                 smlEnv        * aEnv );

stlStatus smdmppInsert( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aInsertCols,
                        knlValueBlockList * aUniqueViolation,
                        smlRid            * aRowRid,
                        smlEnv            * aEnv );

stlStatus smdmppDelete( smlStatement      * aStatement,
                        void              * aRelationHandle,
                        smlRid            * aRowRid,
                        smlScn              aRowScn,
                        stlInt32            aValueIdx,
                        knlValueBlockList * aPrimaryKey,
                        stlBool           * aVersionConflict,
                        stlBool           * aSkipped,
                        smlEnv            * aEnv );

stlStatus smdmppInitIterator ( void   * aIterator,
                               smlEnv * aEnv );

stlStatus smdmppFetchNext( void          * aIterator,
                           smlFetchInfo  * aFetchInfo,
                           smlEnv        * aEnv );

/** @} */
    
#endif /* _SMDMPP_H_ */
