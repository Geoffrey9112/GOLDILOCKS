/*******************************************************************************
 * smxlUndo.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smxlUndo.h 7374 2013-02-25 01:37:39Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMXLUNDO_H_
#define _SMXLUNDO_H_ 1

/**
 * @file smxlUndo.h
 * @brief Table Undo Component Internal Routines
 */

/**
 * @defgroup smxlUndo Table Redo
 * @ingroup smxlInternal
 * @{
 */

stlStatus smxlUndoMemoryCircularCreate( smxmTrans * aMiniTrans,
                                        smlRid      aTargetRid,
                                        void      * aLogBody,
                                        stlInt16    aLogSize,
                                        smlEnv    * aEnv );

stlStatus smxlUndoLockForPrepare( smxmTrans * aMiniTrans,
                                  smlRid      aTargetRid,
                                  void      * aLogBody,
                                  stlInt16    aLogSize,
                                  smlEnv    * aEnv );

/** @} */
    
#endif /* _SMXLUNDO_H_ */
