/*******************************************************************************
 * smqUndo.h
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


#ifndef _SMQUNDO_H_
#define _SMQUNDO_H_ 1

/**
 * @file smqUndo.h
 * @brief Sequence Undo Component Internal Routines
 */

/**
 * @defgroup smqUndo Table Redo
 * @ingroup smqInternal
 * @{
 */

stlStatus smqUndoCreateSequence( smxmTrans * aMiniTrans,
                                 smlRid      aTargetRid,
                                 void      * aLogBody,
                                 stlInt16    aLogSize,
                                 smlEnv    * aEnv );

/** @} */
    
#endif /* _SMQUNDO_H_ */
