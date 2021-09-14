/*******************************************************************************
 * smnUndo.h
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


#ifndef _SMNUNDO_H_
#define _SMNUNDO_H_ 1

/**
 * @file smnUndo.h
 * @brief Index Undo Component Internal Routines
 */

/**
 * @defgroup smnUndo Table Redo
 * @ingroup smnInternal
 * @{
 */
stlStatus smnUndoMemoryBtreeInsert( smxmTrans * aMiniTrans,
                                    smlRid      aTargetRid,
                                    void      * aLogBody,
                                    stlInt16    aLogSize,
                                    smlEnv    * aEnv );

stlStatus smnUndoMemoryBtreeDupKey( smxmTrans * aMiniTrans,
                                    smlRid      aTargetRid,
                                    void      * aLogBody,
                                    stlInt16    aLogSize,
                                    smlEnv    * aEnv );

stlStatus smnUndoMemoryBtreeDelete( smxmTrans * aMiniTrans,
                                    smlRid      aTargetRid,
                                    void      * aLogBody,
                                    stlInt16    aLogSize,
                                    smlEnv    * aEnv );

stlStatus smnUndoMemoryBtreeCreate( smxmTrans * aMiniTrans,
                                    smlRid      aTargetRid,
                                    void      * aLogBody,
                                    stlInt16    aLogSize,
                                    smlEnv    * aEnv );

stlStatus smnUndoMemoryBtreeDrop( smxmTrans * aMiniTrans,
                                  smlRid      aTargetRid,
                                  void      * aLogBody,
                                  stlInt16    aLogSize,
                                  smlEnv    * aEnv );

stlStatus smnUndoMemoryBtreeTruncateCreate( smxmTrans * aMiniTrans,
                                            smlRid      aTargetRid,
                                            void      * aLogBody,
                                            stlInt16    aLogSize,
                                            smlEnv    * aEnv );

stlStatus smnUndoMemoryBtreeTruncateDrop( smxmTrans * aMiniTrans,
                                          smlRid      aTargetRid,
                                          void      * aLogBody,
                                          stlInt16    aLogSize,
                                          smlEnv    * aEnv );

stlStatus smnUndoMemoryBtreeBlockInsert( smxmTrans * aMiniTrans,
                                         smlRid      aTargetRid,
                                         void      * aLogBody,
                                         stlInt16    aLogSize,
                                         smlEnv    * aEnv );

stlStatus smnUndoMemoryBtreeBlockDelete( smxmTrans * aMiniTrans,
                                         smlRid      aTargetRid,
                                         void      * aLogBody,
                                         stlInt16    aLogSize,
                                         smlEnv    * aEnv );

stlStatus smnUndoMemoryBtreeAlterDatatype( smxmTrans * aMiniTrans,
                                           smlRid      aTargetRid,
                                           void      * aLogBody,
                                           stlInt16    aLogSize,
                                           smlEnv    * aEnv );

stlStatus smnUndoMemoryBtreeAlterAttr( smxmTrans * aMiniTrans,
                                       smlRid      aTargetRid,
                                       void      * aLogBody,
                                       stlInt16    aLogSize,
                                       smlEnv    * aEnv );
/** @} */
    
#endif /* _SMNUNDO_H_ */
