/*******************************************************************************
 * smdUndo.h
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


#ifndef _SMDUNDO_H_
#define _SMDUNDO_H_ 1

/**
 * @file smdUndo.h
 * @brief Table Undo Component Internal Routines
 */

/**
 * @defgroup smdUndo Table Redo
 * @ingroup smdInternal
 * @{
 */

stlStatus smdUndoMemoryHeapInsert( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv );

stlStatus smdUndoMemoryHeapUpdate( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv );

stlStatus smdUndoMemoryHeapUpdateLink( smxmTrans * aMiniTrans,
                                       smlRid      aTargetRid,
                                       void      * aLogBody,
                                       stlInt16    aLogSize,
                                       smlEnv    * aEnv );

stlStatus smdUndoMemoryHeapMigrate( smxmTrans * aMiniTrans,
                                    smlRid      aTargetRid,
                                    void      * aLogBody,
                                    stlInt16    aLogSize,
                                    smlEnv    * aEnv );

stlStatus smdUndoMemoryHeapDelete( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv );

stlStatus smdUndoMemoryHeapCreate( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv );

stlStatus smdUndoMemoryHeapDrop( smxmTrans * aMiniTrans,
                                 smlRid      aTargetRid,
                                 void      * aLogBody,
                                 stlInt16    aLogSize,
                                 smlEnv    * aEnv );

stlStatus smdUndoMemoryHeapAppend( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv );

stlStatus smdUndoMemoryHeapAlterAttr( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smlEnv    * aEnv );
stlStatus smdUndoMemoryPendingCreate( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smlEnv    * aEnv );

stlStatus smdUndoMemoryPendingDrop( smxmTrans * aMiniTrans,
                                    smlRid      aTargetRid,
                                    void      * aLogBody,
                                    stlInt16    aLogSize,
                                    smlEnv    * aEnv );

stlStatus smdUndoMemoryHeapBlockInsert( smxmTrans * aMiniTrans,
                                        smlRid      aTargetRid,
                                        void      * aLogBody,
                                        stlInt16    aLogSize,
                                        smlEnv    * aEnv );

stlStatus smdUndoMemoryPendingInsert( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smlEnv    * aEnv );

stlStatus smdUndoAllocColumnHdrPage( smxmTrans * aMiniTrans,
                                     smlRid      aTargetRid,
                                     void      * aLogBody,
                                     stlInt16    aLogSize,
                                     smlEnv    * aEnv );

/** @} */
    
#endif /* _SMDUNDO_H_ */
