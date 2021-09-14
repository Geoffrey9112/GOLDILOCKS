/*******************************************************************************
 * smxlUndoRelMgr.h
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


#ifndef _SMXLUNDORELMGR_H_
#define _SMXLUNDORELMGR_H_ 1

/**
 * @file smxlUndoRelMgr.h
 * @brief Storage Manager Layer Recovery Undo Relaion Manager Component Internal Routines
 */

/**
 * @defgroup smxlUndoRelMgr Undo Relation Manager
 * @ingroup smxlInternal
 * @{
 */
stlStatus smxlInitUndoRelEntry( stlInt16   aRelId,
                                smlRid     aSegRid,
                                void     * aSegHandle,
                                smlEnv   * aEnv );
void * smxlGetUndoRelHandle( smxlUndoRelEntry * aUndoRelEntry );
stlStatus smxlBuildRelCache( void      * aSegHandle,
                             stlInt32    aRelSeqId,
                             void     ** aRelHandle,
                             smlEnv    * aEnv );
void smxlInitUndoRelCache( smxlTransId        aTransId,
                           smxlUndoRelCache * aUndoRelCache );
void * smxlGetUndoRelEntry( smxlTransId aTransId );
stlStatus smxlFreeUndoRelation( smxlTransId   aTransId,
                                smlEnv      * aEnv );
stlStatus smxlRestructureUndoRelation( smlEnv * aEnv );
stlStatus smxlGetRelationSeqId( smlRid   * aSegmentRid,
                                void     * aSegmentHandle,
                                stlInt32 * aRelSeqId,
                                smlEnv   * aEnv );
stlStatus smxlInitRelationHeader( smxlTransId   aTransId,
                                  stlInt32      aRelSeqId,
                                  smlRid      * aSegmentRid,
                                  void        * aSegmentHandle,
                                  smlEnv      * aEnv );
/** @} */

#endif /* _SMXLUNDORELMGR_H_ */
