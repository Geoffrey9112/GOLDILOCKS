/*******************************************************************************
 * smfUndo.h
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


#ifndef _SMFUNDO_H_
#define _SMFUNDO_H_ 1

/**
 * @file smfUndo.h
 * @brief Storage Manager Layer Datafile Undo Internal Routines
 */

/**
 * @defgroup smfUndo Datafile Redo
 * @ingroup smInternal
 * @{
 */

stlStatus smfUndoMemoryFileCreate( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv );

stlStatus smfUndoMemoryFileDrop( smxmTrans * aMiniTrans,
                                 smlRid      aTargetRid,
                                 void      * aLogBody,
                                 stlInt16    aLogSize,
                                 smlEnv    * aEnv );

stlStatus smfUndoMemoryTbsCreate( smxmTrans * aMiniTrans,
                                  smlRid      aTargetRid,
                                  void      * aLogBody,
                                  stlInt16    aLogSize,
                                  smlEnv    * aEnv );

stlStatus smfUndoMemoryTbsDrop( smxmTrans * aMiniTrans,
                                smlRid      aTargetRid,
                                void      * aLogBody,
                                stlInt16    aLogSize,
                                smlEnv    * aEnv );

stlStatus smfUndoMemoryDatafileAdd( smxmTrans * aMiniTrans,
                                    smlRid      aTargetRid,
                                    void      * aLogBody,
                                    stlInt16    aLogSize,
                                    smlEnv    * aEnv );

stlStatus smfUndoMemoryDatafileDrop( smxmTrans * aMiniTrans,
                                     smlRid      aTargetRid,
                                     void      * aLogBody,
                                     stlInt16    aLogSize,
                                     smlEnv    * aEnv );

stlStatus smfUndoMemoryTbsRename( smxmTrans * aMiniTrans,
                                  smlRid      aTargetRid,
                                  void      * aLogBody,
                                  stlInt16    aLogSize,
                                  smlEnv    * aEnv );

stlStatus smfUndoMemoryTbsUndo( smxmTrans * aMiniTrans,
                                smlRid      aTargetRid,
                                void      * aLogBody,
                                stlInt16    aLogSize,
                                smlEnv    * aEnv );

/** @} */
    
#endif /* _SMFUNDO_H_ */
