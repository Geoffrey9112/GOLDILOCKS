/*******************************************************************************
 * smsUndo.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smsUndo.h 925 2011-06-02 11:32:26Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMSUNDO_H_
#define _SMSUNDO_H_ 1

/**
 * @file smsUndo.h
 * @brief Storage Manager Layer Segment Undo Component Internal Routines
 */

/**
 * @defgroup smsUndo Segment Undo
 * @ingroup smsInternal
 * @{
 */

stlStatus smsUndoAlterSegmentAttr( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv );

stlStatus smsUndoSetUnusableSegment( smxmTrans * aMiniTrans,
                                     smlRid      aTargetRid,
                                     void      * aLogBody,
                                     stlInt16    aLogSize,
                                     smlEnv    * aEnv );

stlStatus smsUndoSetUsableSegment( smxmTrans * aMiniTrans,
                                   smlRid      aTargetRid,
                                   void      * aLogBody,
                                   stlInt16    aLogSize,
                                   smlEnv    * aEnv );

stlStatus smsUndoResetPendingSegment( smxmTrans * aMiniTrans,
                                      smlRid      aTargetRid,
                                      void      * aLogBody,
                                      stlInt16    aLogSize,
                                      smlEnv    * aEnv );
/** @} */
    
#endif /* _SMSUNDO_H_ */
