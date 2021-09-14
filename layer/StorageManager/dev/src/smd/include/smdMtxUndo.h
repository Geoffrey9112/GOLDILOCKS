/*******************************************************************************
 * smdMtxUndo.h
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


#ifndef _SMDMTXUNDO_H_
#define _SMDMTXUNDO_H_ 1

/**
 * @file smdMtxUndo.h
 * @brief Storage Manager Layer Table Mini-Transaction Undo Component Internal Routines
 */

/**
 * @defgroup smdMtxUndo Table Mini-Transaction Undo
 * @ingroup smdInternal
 * @{
 */

stlStatus smdMtxUndoMemoryHeapInsert( smlRid    * aDataRid,
                                      void      * aData,
                                      stlUInt16   aDataSize,
                                      smpHandle * aPageHandle,
                                      smlEnv    * aEnv );

/** @} */
    
#endif /* _SMDMTXUNDO_H_ */
