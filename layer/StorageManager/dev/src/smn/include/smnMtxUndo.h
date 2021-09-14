/*******************************************************************************
 * smnMtxUndo.h
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


#ifndef _SMNMTXUNDO_H_
#define _SMNMTXUNDO_H_ 1

/**
 * @file smnMtxUndo.h
 * @brief Storage Manager Layer Index Mini-Transaction Undo Component Internal Routines
 */

/**
 * @defgroup smnMtxUndo Index Mini-Transaction Undo
 * @ingroup smnInternal
 * @{
 */

stlStatus smnMtxUndoMemoryBtreeInsert( smlRid    * aDataRid,
                                       void      * aData,
                                       stlUInt16   aDataSize,
                                       smpHandle * aPageHandle,
                                       smlEnv    * aEnv );

/** @} */
    
#endif /* _SMNMTXUNDO_H_ */
