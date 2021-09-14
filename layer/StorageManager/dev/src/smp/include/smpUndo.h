/*******************************************************************************
 * smpUndo.h
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


#ifndef _SMPUNDO_H_
#define _SMPUNDO_H_ 1

/**
 * @file smpUndo.h
 * @brief Page Undo Component Internal Routines
 */

/**
 * @defgroup smpUndo PageUndo
 * @ingroup smpInternal
 * @{
 */

stlStatus smpUndoRtsVersioning( smxmTrans * aMiniTrans,
                                smlRid      aTargetRid,
                                void      * aLogBody,
                                stlInt16    aLogSize,
                                smlEnv    * aEnv );
/** @} */
    
#endif /* _SMPUNDO_H_ */
